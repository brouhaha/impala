// symbol_table.hh
//
// Copyright 2025 Eric Smith
// SPDX-License-Identifier: GPL-3.0-only

#ifndef SYMBOL_TABLE_HH
#define SYMBOL_TABLE_HH

#include <cstdint>
#include <format>
#include <iostream> // XXX debug only
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>

#include "value.hh"

struct SymbolTableError: public std::runtime_error
{
  SymbolTableError(const std::string& what);
};

struct SymbolMultiplyDefined: public SymbolTableError
{
  SymbolMultiplyDefined(const std::string& symbol,
			std::size_t line1,
			std::size_t line2);
};

struct SymbolValueRedefined: public SymbolTableError
{
  SymbolValueRedefined(const std::string& symbol,
		       std::uint16_t value1,
		       std::uint16_t value2);
};

class SymbolTable
{
public:
  static std::shared_ptr<SymbolTable> create();

  void set_lookup_undefined_ok(bool value);

  void define_symbol(unsigned source_line_number,
		     const std::string& symbol,
		     ValueSP value);

  bool contains(const std::string& symbol) const;

  ValueSP lookup_symbol(unsigned source_line_number,
			const std::string& symbol);

  std::size_t get_symbol_definition_line(const std::string& symbol) const;

  const std::set<std::size_t>& get_symbol_reference_line_numbers(const std::string& symbol) const;

protected:
  SymbolTable();

  struct Entry
  {
    ValueSP value;
    std::size_t definition_line_number = 0;
    std::set<std::size_t> reference_line_numbers;
  };

  bool m_lookup_undefined_ok;
  std::map<std::string, Entry> m_symbol_table;
};

#endif // SYMBOL_TABLE_HH
