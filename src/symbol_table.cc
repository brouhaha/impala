// symbol_table.cc
//
// Copyright 2025 Eric Smith
// SPDX-License-Identifier: GPL-3.0-only

#include "symbol_table.hh"

SymbolTableError::SymbolTableError(const std::string& what):
  std::runtime_error("Symbol table error: " + what)
{
}

SymbolMultiplyDefined::SymbolMultiplyDefined(const std::string& symbol,
					     std::size_t line1,
					     std::size_t line2):
  SymbolTableError(std::format("symbol \"{}\" multiply defined, lines {} and {}",
			       symbol,
			       line1,
			       line2))
{
}

SymbolValueRedefined::SymbolValueRedefined(const std::string& symbol,
					   std::uint16_t value1,
					   std::uint16_t value2):
  SymbolTableError(std::format("symbol \"{}\" value redefined from 0x{:04x} to 0x{:04x}",
			       symbol,
			       value1,
			       value2))
{
}

std::shared_ptr<SymbolTable> SymbolTable::create()
{
  SymbolTable* p = new SymbolTable();
  return std::shared_ptr<SymbolTable>(p);
}

SymbolTable::SymbolTable():
  m_lookup_undefined_ok(false)
{
}

void SymbolTable::set_lookup_undefined_ok(bool value)
{
  m_lookup_undefined_ok = value;
}

void SymbolTable::define_symbol(unsigned source_line_number,
				const std::string& symbol,
				ValueSP value)
{
  if (! m_symbol_table.contains(symbol))
  {
    m_symbol_table.emplace(symbol, Entry { value, source_line_number, {} });
  }
  else
  {
    Entry& entry = m_symbol_table.at(symbol);
    if (entry.definition_line_number != source_line_number)
    {
      throw SymbolMultiplyDefined(symbol, entry.definition_line_number, source_line_number);
    }
    std::uint16_t old_value = entry.value->get();
    std::uint16_t new_value = entry.value->get();
    if (new_value != old_value)
    {
      throw SymbolValueRedefined(symbol, old_value, new_value);
    }
  }
}

bool SymbolTable::contains(const std::string& symbol) const
{
  return m_symbol_table.contains(symbol);
}

ValueSP SymbolTable::lookup_symbol(unsigned source_line_number,
				   const std::string& symbol)
{
  if (! m_symbol_table.contains(symbol))
  {
    if (m_lookup_undefined_ok)
    {
      return Value::create(symbol);
      // Note - this didn't create a placeholder symbol table entry,
      // so the referencing line number isn't recorded. This should
      // only happen during pass 1, so in pass 2 it should get defined
      // properly.
    }
    else
    {
      throw SymbolTableError(std::format("symbol {} undefined", symbol));
    }
  }
  auto entry = m_symbol_table.at(symbol);
  entry.reference_line_numbers.insert(source_line_number);
  return m_symbol_table.at(symbol).value;
}

std::size_t SymbolTable::get_symbol_definition_line(const std::string& symbol) const
{
  if (! m_symbol_table.contains(symbol))
  {
    throw SymbolTableError(std::format("symbol {} undefined", symbol));
  }
  const Entry& entry = m_symbol_table.at(symbol);
  return entry.definition_line_number;
}

const std::set<std::size_t>& SymbolTable::get_symbol_reference_line_numbers(const std::string& symbol) const
{
  if (! m_symbol_table.contains(symbol))
  {
    throw SymbolTableError(std::format("symbol {} undefined", symbol));
  }
  
  const Entry& entry = m_symbol_table.at(symbol);
  return entry.reference_line_numbers;
}
