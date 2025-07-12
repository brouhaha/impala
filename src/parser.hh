// parser.hh
//
// Copyright 2025 Eric Smith
// SPDX-License-Identifier: GPL-3.0-only

#ifndef PARSER_HH
#define PARSER_HH

#include <memory>
#include <string>
#include <vector>

#include "ast_node.hh"
#include "instruction_set.hh"
#include "symbol_table.hh"
#include "value.hh"

class ASTStack;
using ASTStackSP = std::shared_ptr<ASTStack>;

class ParseError: public std::runtime_error
{
public:
  ParseError();
  ParseError(const std::string& what);
};

class Parser
{
public:
  static std::shared_ptr<Parser> create(std::shared_ptr<InstructionSet> instruction_set_sp,
					std::shared_ptr<SymbolTable> symbol_table_sp);

  Parser           (const Parser& ) = delete;  // no copy constructor
  Parser           (      Parser& ) = delete;  // no move constructor
  Parser& operator=(const Parser& ) = delete;  // no copy assignment
  Parser& operator=(      Parser&&) = delete;  // no move assignment

  StatementSP parse(unsigned pass_number,
		    unsigned source_line_number,
		    std::uint16_t location_counter,
		    const std::string& s);

  std::uint16_t get_location_counter() const;

  const std::vector<InstructionSet::Info>& get_instruction_info(const std::string& mnemonic);

protected:
  Parser(std::shared_ptr<InstructionSet> instruction_set_sp,
	 std::shared_ptr<SymbolTable> symbol_table_sp);

  std::shared_ptr<InstructionSet> m_instruction_set_sp;
  std::shared_ptr<SymbolTable> m_symbol_table_sp;
  unsigned m_pass_number;
  unsigned m_source_line_number;
  std::uint16_t m_location_counter;

public:
  ASTStackSP m_ast_stack;  // for grammar actions
};

#endif // PARSER_HH
