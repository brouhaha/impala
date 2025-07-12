// ast_node.cc
//
// Copyright 2025 Eric Smith
// SPDX-License-Identifier: GPL-3.0-only

#include <magic_enum_utility.hpp>

#include "ast_node.hh"

std::shared_ptr<Label> Label::create(const std::string& label)
{
  auto p = new Label(label);
  return std::shared_ptr<Label>(p);
}

const std::string& Label::get() const
{
  return m_label;
}

std::string Label::debug_dump()
{
  return std::format("Label(\"{}\")", m_label);
}

Label::Label(const std::string& label):
  m_label(label)
{
}

std::shared_ptr<Mnemonic> Mnemonic::create(const std::string& mnemonic)
{
  auto p = new Mnemonic(mnemonic);
  return std::shared_ptr<Mnemonic>(p);
}

const std::string& Mnemonic::get() const
{
  return m_mnemonic;
}

std::string Mnemonic::debug_dump()
{
  return std::format("Mnemonic(\"{}\")", m_mnemonic);
}

Mnemonic::Mnemonic(const std::string& mnemonic):
  m_mnemonic(mnemonic)
{
}

std::shared_ptr<Constant> Constant::create(uint16_t value)
{
  auto p = new Constant(value);
  return std::shared_ptr<Constant>(p);
}

std::shared_ptr<Constant> Constant::create(ValueSP value_sp)
{
  auto p = new Constant(value_sp);
  return std::shared_ptr<Constant>(p);
}

ValueSP Constant::get() const
{
  return m_value_sp;
}

ValueSP Constant::evaluate([[maybe_unused]] ExpressionEvaluationContext& evaluation_context) const
{
  return m_value_sp;
}

std::string Constant::debug_dump()
{
  return std::format("Constant({})", m_value_sp->get());
}

Constant::Constant(uint16_t value):
  m_value_sp(Value::create(value))
{
}

Constant::Constant(ValueSP value_sp):
  m_value_sp(value_sp)
{
}

std::shared_ptr<StringConstant> StringConstant::create(const std::string& string)
{
  auto p = new StringConstant(string);
  return std::shared_ptr<StringConstant>(p);
}

const std::string& StringConstant::get() const
{
  return m_string;
}

ValueSP StringConstant::evaluate([[maybe_unused]] ExpressionEvaluationContext& evaluation_context) const
{
  throw std::logic_error("can't evaluate string constant");
}

std::string StringConstant::debug_dump()
{
  return std::format("StringConstant('{}')", m_string);
}

StringConstant::StringConstant(const std::string& string):
  m_string(string)
{
}

std::shared_ptr<Symbol> Symbol::create(const std::string& symbol)
{
  auto p = new Symbol(symbol);
  return std::shared_ptr<Symbol>(p);
}

const std::string& Symbol::get() const
{
  return m_symbol;
}

ValueSP Symbol::evaluate(ExpressionEvaluationContext& evaluation_context) const
{
  return evaluation_context.symbol_table_sp->lookup_symbol(evaluation_context.source_line_number, m_symbol);
}

std::string Symbol::debug_dump()
{
  return std::format("Symbol(\"{}\")", m_symbol);
}

Symbol::Symbol(const std::string& symbol):
  m_symbol(symbol)
{
}

std::shared_ptr<UnaryOperator> UnaryOperator::create(UnaryOperatorEnum unary_operator)
{
  auto p = new UnaryOperator(unary_operator);
  return std::shared_ptr<UnaryOperator>(p);
}

UnaryOperatorEnum UnaryOperator::get() const
{
  return m_unary_operator;
}

std::string UnaryOperator::debug_dump()
{
  std::string s;
  switch (m_unary_operator)
  {
  case UnaryOperatorEnum::LOW_BYTE:
    s = "<";
    break;
  case UnaryOperatorEnum::HIGH_BYTE:
    s = ">";
    break;
  default:
    throw std::logic_error(std::format("internal error: UnaryOperatorEnum value invalid"));
  }
  return std::format("op{}", s);
}

UnaryOperator::UnaryOperator(UnaryOperatorEnum unary_operator):
  m_unary_operator(unary_operator)
{
}

std::shared_ptr<UnaryOperatorExpression> UnaryOperatorExpression::create(std::shared_ptr<UnaryOperator> unary_operator,
									 std::shared_ptr<Expression> subexpression)
{
  auto p = new UnaryOperatorExpression(unary_operator, subexpression);
  return std::shared_ptr<UnaryOperatorExpression>(p);
}

ValueSP UnaryOperatorExpression::evaluate(ExpressionEvaluationContext& evaluation_context) const
{
  ValueSP subexpression_value_sp = m_subexpression->evaluate(evaluation_context);
  switch (m_unary_operator->get())
  {
  case UnaryOperatorEnum::LOW_BYTE:
    return low_byte(subexpression_value_sp);
  case UnaryOperatorEnum::HIGH_BYTE:
    return high_byte(subexpression_value_sp);
  default:
    throw std::logic_error(std::format("internal error: UnaryOperatorEnum value invalid"));
  }
}

std::string UnaryOperatorExpression::debug_dump()
{
  return std::format("({}{})",
		     m_unary_operator->debug_dump(),
		     m_subexpression->debug_dump());
}

UnaryOperatorExpression::UnaryOperatorExpression(std::shared_ptr<UnaryOperator> unary_operator,
						 std::shared_ptr<Expression> subexpression):
  m_unary_operator(unary_operator),
  m_subexpression(subexpression)
{
}

std::shared_ptr<BinaryOperator> BinaryOperator::create(BinaryOperatorEnum binary_operator)
{
  auto p = new BinaryOperator(binary_operator);
  return std::shared_ptr<BinaryOperator>(p);
}

BinaryOperatorEnum BinaryOperator::get() const
{
  return m_binary_operator;
}

std::string BinaryOperator::debug_dump()
{
  std::string s;
  switch (m_binary_operator)
  {
  case BinaryOperatorEnum::ADDITION:
    s = "+";
    break;
  case BinaryOperatorEnum::SUBTRACTION:
    s = "-";
    break;
  case BinaryOperatorEnum::MULTIPLICATION:
    s = "*";
    break;
  case BinaryOperatorEnum::DIVISION:
    s = "/";
    break;
  default:
    throw std::logic_error(std::format("internal error: BinaryOperatorEnum value invalid"));
  }
  return std::format("op{}", s);
}

BinaryOperator::BinaryOperator(BinaryOperatorEnum binary_operator):
  m_binary_operator(binary_operator)
{
}

std::shared_ptr<BinaryOperatorExpression> BinaryOperatorExpression::create(std::shared_ptr<Expression> left_subexpression,
									   std::shared_ptr<BinaryOperator> binary_operator,
									   std::shared_ptr<Expression> right_subexpression)
{
  auto p = new BinaryOperatorExpression(left_subexpression, binary_operator, right_subexpression);
  return std::shared_ptr<BinaryOperatorExpression>(p);
}

ValueSP BinaryOperatorExpression::evaluate(ExpressionEvaluationContext& evaluation_context) const
{
  ValueSP left_subexpression_value = m_left_subexpression->evaluate(evaluation_context);
  ValueSP right_subexpression_value = m_right_subexpression->evaluate(evaluation_context);

  switch (m_binary_operator->get())
  {
  case BinaryOperatorEnum::ADDITION:
    return left_subexpression_value + right_subexpression_value;
  case BinaryOperatorEnum::SUBTRACTION:
    return left_subexpression_value - right_subexpression_value;
  case BinaryOperatorEnum::MULTIPLICATION:
    return left_subexpression_value * right_subexpression_value;
  case BinaryOperatorEnum::DIVISION:
    return left_subexpression_value / right_subexpression_value;
  default:
    throw std::logic_error(std::format("internal error: BinaryOperatorEnum value invalid"));
  }
}

std::string BinaryOperatorExpression::debug_dump()
{
  return std::format("({}{}{})",
		     m_left_subexpression->debug_dump(),
		     m_binary_operator->debug_dump(),
		     m_right_subexpression->debug_dump());
}

BinaryOperatorExpression::BinaryOperatorExpression(std::shared_ptr<Expression> left_subexpression,
						   std::shared_ptr<BinaryOperator> binary_operator,
						   std::shared_ptr<Expression> right_subexpression):
  m_left_subexpression(left_subexpression),
  m_binary_operator(binary_operator),
  m_right_subexpression(right_subexpression)
{
}

std::shared_ptr<ExpressionList> ExpressionList::create()
{
  auto p = new ExpressionList();
  return std::shared_ptr<ExpressionList>(p);
}

void ExpressionList::append_expression(ExpressionSP expression_sp)
{
  m_expressions.push_back(expression_sp);
}

const std::vector<std::shared_ptr<Expression>>& ExpressionList::get() const
{
  return m_expressions;
}

std::string ExpressionList::debug_dump()
{
  std::string s = "ExpressionList(";
  bool first = true;
  for (const auto& expression_sp: m_expressions)
  {
    if (first)
    {
      first = false;
    }
    else
    {
      s += ',';
    }
    if (expression_sp)
    {
      s += expression_sp->debug_dump();
    }
    else
    {
      s += "null";
    }
  }
  s += ")";
  return s;
}

ExpressionList::ExpressionList()
{
}

std::shared_ptr<Statement> Statement::create()
{
  auto p = new Statement();
  return std::shared_ptr<Statement>(p);
}

void Statement::set_label(const std::string& label)
{
  m_label = label;
}

void Statement::set_mnemonic(const std::string& mnemonic)
{
  m_mnemonic = mnemonic;
}

void Statement::add_operand(ExpressionSP operand)
{
  m_operands.push_back(operand);
}

void Statement::set_operands(const std::vector<ExpressionSP>& operands)
{
  m_operands = operands;
}

const std::string& Statement::get_label() const
{
  return m_label;
}

const std::string& Statement::get_mnemonic() const
{
  return m_mnemonic;
}

std::size_t Statement::get_operand_count() const
{
  return m_operands.size();
}

const std::shared_ptr<Expression> Statement::get_operand(std::size_t index) const
{
  return m_operands.at(index);  // may throw std::out_of_range
}

const std::vector<std::shared_ptr<Expression>>& Statement::get_operands() const
{
  return m_operands;
}

std::string Statement::debug_dump()
{
  return std::format("Statement");
}

Statement::Statement()
{
}

