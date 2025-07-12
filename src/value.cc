// value.cc
//
// Copyright 2025 Eric Smith
// SPDX-License-Identifier: GPL-3.0-only

#include <ranges>

#include "value.hh"

ValueError::ValueError(const std::string& what):
  std::runtime_error("ValueError: " + what)
{
}

static std::string join(const std::set<std::string>& elems, const std::string& delim = ",")
{
  return elems | std::views::join_with(delim) | std::ranges::to<std::string>();
}

ValueUnknownError::ValueUnknownError(const std::set<std::string>& unknown_symbols):
  ValueError("unknown symbols " + join(unknown_symbols)),
  m_unknown_symbols(unknown_symbols)
{
}

ValueDivideByZeroError::ValueDivideByZeroError():
  ValueError("dvision by zero")
{
}

std::shared_ptr<Value> Value::create(std::uint16_t value)
{
  auto p = new Value(value);
  return std::shared_ptr<Value>(p);
}

std::shared_ptr<Value> Value::create(const std::string& unknown_symbol)
{
  auto p = new Value(unknown_symbol);
  return std::shared_ptr<Value>(p);
}

std::shared_ptr<Value> Value::create(const std::set<std::string>& unknown_symbols)
{
  auto p = new Value(unknown_symbols);
  return std::shared_ptr<Value>(p);
}

Value::Value(std::uint16_t value):
  m_known(true),
  m_value(value)
{
}

Value::Value(const std::string& unknown_symbol):
  m_known(false)
{
  m_unknown_symbols.insert(unknown_symbol);
}

Value::Value(const std::set<std::string>& unknown_symbols):
  m_known(false),
  m_unknown_symbols(unknown_symbols)
{
}

bool Value::known() const
{
  return m_known;
}

std::uint16_t Value::get() const
{
  if (m_known)
  {
    return m_value;
  }
  throw ValueUnknownError(m_unknown_symbols);
}

const std::set<std::string>& Value::get_unknown_symbols() const
{
  return m_unknown_symbols;
}

static std::set<std::string> merge_unknowns(const ValueSP& left,
					    const ValueSP& right)
{
  std::set<std::string> unknowns;
  if (! left->known())
  {
    auto ls = left->get_unknown_symbols();
    unknowns.insert(ls.begin(), ls.end());
  }
  if (! right->known())
  {
    auto rs = right->get_unknown_symbols();
    unknowns.insert(rs.begin(), rs.end());
  }
  return unknowns;
}

ValueSP operator+(const ValueSP& left, const ValueSP& right)
{
  if (left->known() && right->known())
  {
    return Value::create(left->get() + right->get());
  }
  return Value::create(merge_unknowns(left, right));
}

ValueSP operator-(const ValueSP& left, const ValueSP& right)
{
  if (left->known() && right->known())
  {
    return Value::create(left->get() - right->get());
  }
  return Value::create(merge_unknowns(left, right));
}

ValueSP operator*(const ValueSP& left, const ValueSP& right)
{
  if (left->known() && right->known())
  {
    return Value::create(left->get() * right->get());
  }
  return Value::create(merge_unknowns(left, right));
}

ValueSP operator/(const ValueSP& left, const ValueSP& right)
{
  if (left->known() && right->known())
  {
    if (right->get() == 0)
    {
      throw ValueDivideByZeroError();
    }
    return Value::create(left->get() / right->get());
  }
  return Value::create(merge_unknowns(left, right));
}

ValueSP low_byte(const ValueSP& operand)
{
  if (operand->known())
  {
    return Value::create(operand->get() & 0xff);
  }
  return operand;
}

ValueSP high_byte(const ValueSP& operand)
{
  if (operand->known())
  {
    return Value::create(operand->get() >> 8);
  }
  return operand;
}

