// assembler.hh
//
// Copyright 2025 Eric Smith
// SPDX-License-Identifier: GPL-3.0-only

#ifndef ASSEMBLER_HH
#define ASSEMBLER_HH

#include <array>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "ast_node.hh"
#include "instruction_set.hh"
#include "parser.hh"
#include "pseudo_op.hh"
#include "symbol_table.hh"
#include "value.hh"

struct AssemblerError: public std::runtime_error
{
  AssemblerError(const std::string& what);
  AssemblerError(std::size_t source_line_number,
		 const std::string& what);
};

class Assembler
{
public:
  using Address = std::uint16_t;

  Assembler(std::filesystem::path source_filename,
	    std::filesystem::path object_filename,
	    std::filesystem::path listing_filename);
  virtual ~Assembler();

  Assembler           (const Assembler& ) = delete;  // no copy constructor
  Assembler           (      Assembler& ) = delete;  // no move constructor
  Assembler& operator=(const Assembler& ) = delete;  // no copy assignment
  Assembler& operator=(      Assembler&&) = delete;  // no move assignment

  void assemble();

private:
  using AssembleInstructionFnPtr = void (Assembler::*) (const InstructionSet::Info& instruction_info);
  using AssemblePseudoOpFnPtr    = void (Assembler::*) (const PseudoOp::Info& pseudo_op_info);

  void assemble_pass(int pass_number);

  void assemble_line();
  void assemble_instruction();
  void assemble_pseudo_op();

  void write_listing_line(std::ostream& os);

  ValueSP evaluate(ExpressionSP expression_sp) const;

  std::uint16_t convert_operand_uint16(ExpressionSP expression_sp);

  void define_symbol(const std::string& symbol,
		     ValueSP value);

  void assemble_pseudo_op_unimplemented(const PseudoOp::Info& pseudo_op_info);

  void assemble_pseudo_op_ascii (const PseudoOp::Info& pseudo_op_info);
  void assemble_pseudo_op_byte  (const PseudoOp::Info& pseudo_op_info);
  void assemble_pseudo_op_def   (const PseudoOp::Info& pseudo_op_info);
  void assemble_pseudo_op_end   (const PseudoOp::Info& pseudo_op_info);
  void assemble_pseudo_op_hbyte (const PseudoOp::Info& pseudo_op_info);
  void assemble_pseudo_op_link  (const PseudoOp::Info& pseudo_op_info);
  void assemble_pseudo_op_list  (const PseudoOp::Info& pseudo_op_info);
  void assemble_pseudo_op_loc   (const PseudoOp::Info& pseudo_op_info);
  void assemble_pseudo_op_nolist(const PseudoOp::Info& pseudo_op_info);
  void assemble_pseudo_op_page  (const PseudoOp::Info& pseudo_op_info);
  void assemble_pseudo_op_word  (const PseudoOp::Info& pseudo_op_info);

  void emit_byte(std::uint8_t byte);
  void emit_word(std::uint16_t word);

  void write_object_bytes();

  void list_symbol_table(std::ostream& os);

  std::ifstream m_source_file;
  std::ofstream m_object_file;
  std::ofstream m_listing_file;

  std::shared_ptr<InstructionSet> m_instruction_set_sp;
  std::shared_ptr<PseudoOp> m_pseudo_op_sp;
  std::shared_ptr<SymbolTable> m_symbol_table_sp;
  std::shared_ptr<Parser> m_parser_sp;

  int m_pass_number;
  bool m_end_reached;
  unsigned m_error_count;
  unsigned m_warning_count;

  unsigned m_source_line_number;
  std::string m_source_line;

  std::uint16_t m_location_counter;
  StatementSP m_statement_sp;

  // object code buffer
  std::uint32_t m_prev_object_code_address;
  std::uint32_t m_object_code_address;
  std::vector<std::uint8_t> m_object_code_bytes;
  std::vector<bool> m_object_code_bytes_start_of_word;

  // listing
  bool m_listing_show_address;  // forces showing address even if no object code bytes
  static constexpr std::size_t MAX_OBJECT_BYTES_PER_LISTING_LINE = 3;

  static const magic_enum::containers::array<PseudoOp::PseudoOpEnum, AssemblePseudoOpFnPtr> s_assemble_pseudo_op_fn_ptrs;
};

#endif // ASSEMBLER_HH
