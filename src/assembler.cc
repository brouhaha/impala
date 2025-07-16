// assembler.cc
//
// Copyright 2025 Eric Smith
// SPDX-License-Identifier: GPL-3.0-only

#include <format>
#include <iostream>
#include <stdexcept>

#include "assembler.hh"

AssemblerError::AssemblerError(const std::string& what):
  std::runtime_error(std::format("Error: {}", what))
{
}

AssemblerError::AssemblerError(std::size_t source_line_number,
			       const std::string& what):
  std::runtime_error(std::format("Error at line {}: {}", source_line_number, what))
{
}

std::string untabify(const std::string& s)
{
  unsigned col = 0;
  std::string result;
  for (char c: s)
  {
    if (c == '\t')
    {
      do
      {
	result += ' ';
	++col;
      }
      while (col & 7);
    }
    else
    {
      result += c;
      ++col;
    }
  }
  return result;
}

Assembler::Assembler(std::filesystem::path source_filename,
		     std::filesystem::path object_filename,
		     std::filesystem::path listing_filename)
{
  m_source_file.open(source_filename, std::ios::in);
  if (! m_source_file.is_open())
  {
    throw AssemblerError("can't open source file");
  }

  m_object_file.open(object_filename, std::ios::out | std::ios::binary);
  if (! m_object_file.is_open())
  {
    throw AssemblerError("can't open object file");
  }

  m_listing_file.open(listing_filename, std::ios::out);
  if (! m_listing_file.is_open())
  {
    throw AssemblerError("can't open listing file");
  }

  m_instruction_set_sp = InstructionSet::create();
  m_pseudo_op_sp = PseudoOp::create();
  m_parser_sp = Parser::create(m_instruction_set_sp, m_symbol_table_sp);
  m_symbol_table_sp = SymbolTable::create();
}

Assembler::~Assembler()
{
}

ValueSP Assembler::evaluate(ExpressionSP expression_sp) const
{
  if (! expression_sp)
  {
#if 0
    if (nullptr_returns_default_value)
    {
      return Value();
    }
#endif
    throw AssemblerError(m_source_line_number, "evaluate nullptr expression");
  }
  ExpressionEvaluationContext context { m_symbol_table_sp,
					m_source_line_number };
  return expression_sp->evaluate(context);
}

std::uint16_t Assembler::convert_operand_uint16(ExpressionSP expression_sp)
{
  ValueSP value_sp = evaluate(expression_sp);
  try
  {
    return value_sp->get();
  }
  catch (const ValueUnknownError& e)
  {
    if (m_pass_number == 1)
    {
      return 0x0100;  // value that won't be interpreted as page zero
    }
    throw AssemblerError(m_source_line_number, "expression evaluation error");
  }
}

void Assembler::assemble()
{
  for (int p = 1; p <= 2; ++p)
  {
    assemble_pass(p);
  }
}

void Assembler::assemble_pass(int pass_number)
{
  if ((pass_number < 1) || (pass_number > 2))
  {
    throw AssemblerError(m_source_line_number, std::format("invalid pass number {}", pass_number));
  }

  std::cout << std::format("starting pass {}\n", pass_number);

  m_pass_number = pass_number;
  m_end_reached = false;
  m_error_count = 0;
  m_warning_count = 0;

  m_prev_object_code_address = -1;  // guaranteed not to match any real address

  m_symbol_table_sp->set_lookup_undefined_ok(m_pass_number == 1);

  if (m_pass_number != 1)
  {
    m_source_file.clear(); // seekg will clear the eof bit, but not the fail bit
    m_source_file.seekg(0, std::ios::beg);  // rewind source file
  }
  m_source_line_number = 0;

  while ((! m_end_reached) && std::getline(m_source_file, m_source_line))
  {
    m_source_line = untabify(m_source_line);
    ++m_source_line_number;

    m_listing_show_address = false;
    m_object_code_address = m_location_counter;
    m_object_code_bytes.clear();
    m_object_code_bytes_start_of_word.clear();
    
    try
    {
      m_statement_sp = m_parser_sp->parse(m_pass_number,
					  m_source_line_number,
					  m_location_counter,
					  m_source_line);
    }
    catch (const ParseError& parse_error)
    {
      std::cerr << std::format("line {} parse failed\n", m_source_line_number);
    }

    assemble_line();

    if (m_pass_number == 2)
    {
      write_listing_line(m_listing_file);
      write_object_bytes();
    }
    m_location_counter += m_object_code_bytes.size();
  }

  if (m_pass_number == 2)
  {
    // XXX list_symbol_table(m_listing_file);
  }

  std::cerr << std::format("Pass {}: detected {} errors, {} warnings\n",
			   m_pass_number, m_error_count, m_warning_count);
}

void Assembler::define_symbol(const std::string& symbol,
			      ValueSP value)
{
  m_symbol_table_sp->define_symbol(m_source_line_number, symbol, value);
}

void Assembler::assemble_line()
{
  std::string mnemonic = m_statement_sp->get_mnemonic();
  if ((! mnemonic.size()) ||
      (m_instruction_set_sp->valid_mnemonic(mnemonic)))
  {
    assemble_instruction();
  }
  else if (PseudoOp::valid_mnemonic(mnemonic))
  {
    assemble_pseudo_op();
  }
  else
  {
    throw AssemblerError(m_source_line_number, std::format("Unrecognized mnemonic \"{}\"", mnemonic));
  }
}

void Assembler::assemble_instruction()
{
  std::string label = m_statement_sp->get_label();
  if (label.size())
  {
    define_symbol(label,
		  Value::create(m_location_counter));
  }

  std::string mnemonic = m_statement_sp->get_mnemonic();
  if (! mnemonic.size())
  {
    return;  // no instruction, just a label
  }

  // At most, infos will have two entries, for corresponding zero page and absolute (possibly indexed) statements.
  const std::vector<InstructionSet::Info>& infos = m_instruction_set_sp->get(mnemonic);
  bool expect_operand = false;
  switch (infos.size())
  {
  case 1:
    expect_operand = InstructionSet::operand_size_bytes(infos[0].mode) > 0;
    break;
  case 2:
    if ((InstructionSet::operand_size_bytes(infos[0].mode) != 1) ||
	(InstructionSet::operand_size_bytes(infos[1].mode) != 2))
    {
      throw std::logic_error("internal error: instruction with wrong modes");
    }
    expect_operand = true;
    break;
  default:
    throw std::logic_error(std::format("internal error: instruction with {} modes", infos.size()));
  }
  std::size_t operand_count = m_statement_sp->get_operand_count();
  if (operand_count != expect_operand)
  {
    throw AssemblerError(m_source_line_number,
			 std::format("\"{}\" instruction requires {} operands, but {} provided",
				     mnemonic,
				     expect_operand,
				     operand_count));
  }
  bool found = false;
  std::uint8_t opcode;
  std::uint16_t operand_value;
  std::size_t operand_size = 0;
  if (! expect_operand)
  {
    found = true;
    opcode = infos[0].opcode;
  }
  else
  {
    auto expression_sp = m_statement_sp->get_operand(0);
    operand_value = convert_operand_uint16(expression_sp);
    operand_size = (operand_value > 0x00ff) ? 2 : 1;
    for (const auto& info: infos)
    {
      if (info.mode == InstructionSet::Mode::RELATIVE)
      {
	found = true;
	operand_size = 1;
	std::int32_t displacement = operand_value - (m_location_counter + 2);
	if ((m_pass_number == 2) &&
	    ((displacement < std::numeric_limits<std::int8_t>::min()) ||
	     (displacement > std::numeric_limits<std::int8_t>::max())))
	{
	  throw AssemblerError(m_source_line_number,
			       std::format("relative branch displacement {} out of range",
					   displacement));
	}
	opcode = info.opcode;
	operand_value = displacement & 0xff;
      }
      else if ((infos.size() == 1) ||
	       InstructionSet::operand_size_bytes(info.mode) >= operand_size)
      {
	found = true;
	opcode = info.opcode;
	operand_size = InstructionSet::operand_size_bytes(info.mode);
	break;
      }
    }
    if (! found)
    {
      throw AssemblerError(m_source_line_number,
			   std::format("no \"{}\" instruction with {} byte operand for value 0x{:04x}",
				       mnemonic,
				       operand_size,
				       operand_value));
    }
  }
  emit_byte(opcode);
  switch (operand_size)
  {
  case 0:
    break;
  case 1:
    emit_byte(operand_value);
    break;
  case 2:
    emit_word(operand_value);
    break;
  }
}

void Assembler::assemble_pseudo_op()
{
  std::string mnemonic = m_statement_sp->get_mnemonic();
  const PseudoOp::Info& pseudo_op_info = PseudoOp::lookup_mnemonic(mnemonic);

  std::string label = m_statement_sp->get_label();
  if (label.size())
  {
    if (pseudo_op_info.flags[PseudoOp::Flag::LABEL_DISALLOWED])
    {
      throw AssemblerError(m_source_line_number,
			   std::format("Pseudo-op {} not allowed to have label", pseudo_op_info.mnemonic));
    }
    if (! pseudo_op_info.flags[PseudoOp::Flag::LABEL_ISNT_LOC])
    {
      define_symbol(label, Value::create(m_location_counter));
    }
  }
  (this->*s_assemble_pseudo_op_fn_ptrs[pseudo_op_info.pseudo_op])(pseudo_op_info);
}

void Assembler::write_object_bytes()
{
  if (! m_object_code_bytes.size())
  {
    return;
  }
  if (m_object_code_address != m_prev_object_code_address)
  {
    m_object_file << std::format("*{:04X}", m_object_code_address);
  }
  for (std::size_t i = 0; i < m_object_code_bytes.size(); i++)
  {
    m_object_file << std::format("{:02X}", m_object_code_bytes[i]);
  }
  m_object_code_address += m_object_code_bytes.size();
  m_prev_object_code_address = m_object_code_address;
}


void Assembler::write_listing_line(std::ostream& os)
{
  std::string line = std::format("{:-5}  ", m_source_line_number);

  if (m_listing_show_address || m_object_code_bytes.size())
  {
    line += std::format("{:04x} ", m_object_code_address);
  }
  else
  {
    line += "     ";
  }

  std::string s_obj;
  std::size_t i = 0;
  while ((i < MAX_OBJECT_BYTES_PER_LISTING_LINE) && (i < m_object_code_bytes.size()))
  {
    if (m_object_code_bytes_start_of_word[i])
    {
      s_obj += std::format(" {:04x}", (m_object_code_bytes[i+1] << 8) | m_object_code_bytes[i]);
      i += 2;
    }
    else
    {
      s_obj += std::format(" {:02x}", m_object_code_bytes[i]);
      ++i;
    }
  }
  s_obj += std::string(9 - s_obj.size(), ' ');
  line += s_obj;

  line += "  " + m_source_line + '\n';
  os << line;
}


void Assembler::list_symbol_table(std::ostream& os)
{
  (void) os;
#if 0
  os << "\n";
  os << "symbol    value   def    referenced\n";
  os << "--------  ------  -----  ----------\n";
  for (const auto& [symbol, entry]: m_symbol_table_sp)
  {
    os << std::format("{:8}  {:06x}  {:5}", symbol, entry.value, entry.definition_line);
    for (const auto& line_number: entry.reference_lines)
    {
      os << std::format("  {:5}", line_number);
    }
    os << "\n";
  }
#endif
}


void Assembler::emit_byte(std::uint8_t byte)
{
  m_object_code_bytes.push_back(byte);
  m_object_code_bytes_start_of_word.push_back(false);
}

void Assembler::emit_word(std::uint16_t word)
{
  m_object_code_bytes.push_back(word & 0xff);
  m_object_code_bytes_start_of_word.push_back(true);
  m_object_code_bytes.push_back(word >> 8);
  m_object_code_bytes_start_of_word.push_back(false);
}

void Assembler::assemble_pseudo_op_unimplemented([[maybe_unused]] const PseudoOp::Info& pseudo_op_info)
{
  throw AssemblerError(m_source_line_number,
		       std::format("unimplmented pseudo-op {}", pseudo_op_info.mnemonic));
}

void Assembler::assemble_pseudo_op_ascii([[maybe_unused]] const PseudoOp::Info& pseudo_op_info)
{
  auto operand_sp = m_statement_sp->get_operand(0);
  StringConstantSP string_sp = dynamic_pointer_cast<StringConstant>(operand_sp);
  const std::string& string = string_sp->get();
  for (auto c: string)
  {
    std::uint8_t byte = static_cast<std::uint8_t>(c);
    emit_byte(byte);
  }
}

void Assembler::assemble_pseudo_op_byte([[maybe_unused]] const PseudoOp::Info& pseudo_op_info)
{
  if (! m_statement_sp->get_operand_count())
  {
    emit_byte(0);
  }
  else
  {
    for (const auto& expression_sp: m_statement_sp->get_operands())
    {
      // ASM65 silently truncates .BYTE operands to low byte
      std::uint16_t value = convert_operand_uint16(expression_sp);
      emit_byte(static_cast<std::uint8_t>(value & 0xff));
    }
  }
}

void Assembler::assemble_pseudo_op_def([[maybe_unused]] const PseudoOp::Info& pseudo_op_info)
{
  auto operand_sp = m_statement_sp->get_operand(0);
  auto symbol_sp = dynamic_pointer_cast<Symbol>(operand_sp);
  std::uint16_t value = convert_operand_uint16(m_statement_sp->get_operand(1));
  define_symbol(symbol_sp->get(), Value::create(value));
  m_listing_show_address = true;
  m_object_code_address = value;
}

void Assembler::assemble_pseudo_op_end([[maybe_unused]] const PseudoOp::Info& pseudo_op_info)
{
  m_end_reached = true;
}

void Assembler::assemble_pseudo_op_hbyte([[maybe_unused]] const PseudoOp::Info& pseudo_op_info)
{
  if (! m_statement_sp->get_operand_count())
  {
    emit_byte(0);
  }
  else
  {
    for (const auto& expression_sp: m_statement_sp->get_operands())
    {
      std::uint16_t value = convert_operand_uint16(expression_sp);
      emit_byte(static_cast<std::uint8_t>(value >> 8));
    }
  }
}

void Assembler::assemble_pseudo_op_list([[maybe_unused]] const PseudoOp::Info& pseudo_op_info)
{
  // XXX ignore for now, but should affect listing output
}

void Assembler::assemble_pseudo_op_loc([[maybe_unused]] const PseudoOp::Info& pseudo_op_info)
{
  std::uint16_t addr = convert_operand_uint16(m_statement_sp->get_operand(0));
  m_location_counter = addr;
  m_object_code_address = addr;
  m_listing_show_address = true;
}

void Assembler::assemble_pseudo_op_nolist([[maybe_unused]] const PseudoOp::Info& pseudo_op_info)
{
  // XXX ignore for now, but should affect listing output
}

void Assembler::assemble_pseudo_op_page([[maybe_unused]] const PseudoOp::Info& pseudo_op_info)
{
  // XXX ignore for now, but should affect listing output
}

void Assembler::assemble_pseudo_op_word([[maybe_unused]] const PseudoOp::Info& pseudo_op_info)
{
  if (! m_statement_sp->get_operand_count())
  {
    emit_word(0);
  }
  else
  {
    for (const auto& expression_sp: m_statement_sp->get_operands())
    {
      // ASM65 silently truncates .BYTE operands to low byte
      std::uint16_t value = convert_operand_uint16(expression_sp);
      emit_word(value);
    }
  }
}

const magic_enum::containers::array<PseudoOp::PseudoOpEnum, Assembler::AssemblePseudoOpFnPtr> Assembler::s_assemble_pseudo_op_fn_ptrs
{
  & Assembler::assemble_pseudo_op_ascii,
  & Assembler::assemble_pseudo_op_byte,
  & Assembler::assemble_pseudo_op_def,
  & Assembler::assemble_pseudo_op_end,
  & Assembler::assemble_pseudo_op_hbyte,
  & Assembler::assemble_pseudo_op_unimplemented,  // link
  & Assembler::assemble_pseudo_op_list,
  & Assembler::assemble_pseudo_op_loc,
  & Assembler::assemble_pseudo_op_nolist,
  & Assembler::assemble_pseudo_op_page,
  & Assembler::assemble_pseudo_op_word,
};
