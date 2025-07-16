// parser.cc
//
// Copyright 2025 Eric Smith
// SPDX-License-Identifier: GPL-3.0-only

#include <format>
#include <iostream>
#include <stdexcept>

#include "ast_stack.hh"
#include "parser.hh"
#include "grammar.hh"

#include <tao/pegtl/analyze.hpp>

ParseError::ParseError():
  std::runtime_error("Parse error")
{
}

ParseError::ParseError(const std::string& what):
  std::runtime_error("Parse error: " + what)
{
}

std::shared_ptr<Parser> Parser::create(std::shared_ptr<InstructionSet> instruction_set_sp,
				       std::shared_ptr<SymbolTable> symbol_table_sp)
{
  auto p = new Parser(instruction_set_sp, symbol_table_sp);
  return std::shared_ptr<Parser>(p);
}

Parser::Parser(std::shared_ptr<InstructionSet> instruction_set_sp,
	       std::shared_ptr<SymbolTable> symbol_table_sp):
  m_instruction_set_sp(instruction_set_sp),
  m_symbol_table_sp(symbol_table_sp)
{
}

void Parser::check_grammar()
{
  auto grammar_analysis_error_count = pegtl::analyze<grammar::statement>();
  if (grammar_analysis_error_count)
  {
    throw std::logic_error(std::format("internal error: grammar analysis failed, count {}", grammar_analysis_error_count));
  }
}

StatementSP Parser::parse(unsigned pass_number,
			  unsigned source_line_number,
			  std::uint16_t location_counter,
			  const std::string& s)
{
#if 0
  check_grammar();
#endif

  m_pass_number = pass_number;
  m_source_line_number = source_line_number;
  m_location_counter = location_counter;

  m_ast_stack = ASTStack::create();

  pegtl::string_input src_line(s, "from line");

  bool result = pegtl::parse<grammar::statement, grammar::action>(src_line, *this);

  if (! result)
  {
    throw ParseError();
  }

  StatementSP statement_sp = m_ast_stack->pop<Statement>();

  if (! m_ast_stack->empty())
  {
    throw std::logic_error(std::format("internal error: AST stack has {} leftover items", m_ast_stack->size()));
  }

  return statement_sp;
}

std::uint16_t Parser::get_location_counter() const
{
  return m_location_counter;
}

const std::vector<InstructionSet::Info>& Parser::get_instruction_info(const std::string& mnemonic)
{
  return m_instruction_set_sp->get(mnemonic);
}
