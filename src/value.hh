// value.hh
//
// Copyright 2025 Eric Smith
// SPDX-License-Identifier: GPL-3.0-only

#ifndef VALUE_HH
#define VALUE_HH

#include <cstdint>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

class ValueError: public std::runtime_error
{
public:
  ValueError(const std::string& what);
};

class ValueUnknownError: public ValueError
{
public:
  ValueUnknownError(const std::set<std::string>& unknown_symbols);

protected:
  const std::set<std::string>& m_unknown_symbols;
};

class ValueDivideByZeroError: public ValueError
{
public:
  ValueDivideByZeroError();
};

// This might be more convenient as a template
//     template<typename T>
// rather than hardwiring it to std::uint16_t
class Value
{
public:
  static std::shared_ptr<Value> create(std::uint16_t value);
  static std::shared_ptr<Value> create(const std::string& unknown_symbol);
  static std::shared_ptr<Value> create(const std::set<std::string>& unknown_symbols);

  bool known() const;
  std::uint16_t get() const;
  const std::set<std::string>& get_unknown_symbols() const;

protected:
  Value(std::uint16_t value);
  Value(const std::string& unknown_symbol);
  Value(const std::set<std::string>& unknown_symbols);

  bool m_known;
  std::uint16_t m_value;
  std::set<std::string> m_unknown_symbols;
};

using ValueSP = std::shared_ptr<Value>;

ValueSP operator+(const ValueSP& left, const ValueSP& right);
ValueSP operator-(const ValueSP& left, const ValueSP& right);
ValueSP operator*(const ValueSP& left, const ValueSP& right);
ValueSP operator/(const ValueSP& left, const ValueSP& right);

ValueSP low_byte(const ValueSP& operand);
ValueSP high_byte(const ValueSP& operand);

#endif // VALUE_HH
