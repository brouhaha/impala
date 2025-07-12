// ast_node.hh
//
// Copyright 2025 Eric Smith
// SPDX-License-Identifier: GPL-3.0-only

#ifndef AST_NODE_HH
#define AST_NODE_HH

#include <memory>
#include <span>
#include <vector>

#include <magic_enum.hpp>
#include <magic_enum_containers.hpp>

#include "symbol_table.hh"
#include "value.hh"

class ASTNode
{
public:
  virtual std::string debug_dump() = 0;
};
using ASTNodeSP = std::shared_ptr<ASTNode>;

class Label: public ASTNode
{
public:
  static std::shared_ptr<Label> create(const std::string& mnemonic);
  const std::string& get() const;
  std::string debug_dump() override;

protected:
  Label(const std::string& mnemonic);
  std::string m_label;
};
using LabelSP = std::shared_ptr<Label>;

class Mnemonic: public ASTNode
{
public:
  static std::shared_ptr<Mnemonic> create(const std::string& mnemonic);
  const std::string& get() const;
  std::string debug_dump() override;

protected:
  Mnemonic(const std::string& mnemonic);
  std::string m_mnemonic;
};
using MnemonicSP = std::shared_ptr<Mnemonic>;

struct ExpressionEvaluationContext
{
  std::shared_ptr<SymbolTable> symbol_table_sp;
  unsigned source_line_number;
};

class Expression: public ASTNode
{
public:
  virtual ValueSP evaluate(ExpressionEvaluationContext& evaluation_context) const = 0;
};
using ExpressionSP = std::shared_ptr<Expression>;

class Constant: public Expression
{
public:
  static std::shared_ptr<Constant> create(std::uint16_t value);
  static std::shared_ptr<Constant> create(ValueSP value_sp);
  ValueSP get() const;
  ValueSP evaluate(ExpressionEvaluationContext& evaluation_context) const override;
  std::string debug_dump() override;

protected:
  Constant(std::uint16_t value);
  Constant(ValueSP value_sp);
  ValueSP m_value_sp;
};
using ConstantSP = std::shared_ptr<Constant>;

class StringConstant: public Expression
{
public:
  static std::shared_ptr<StringConstant> create(const std::string& string);
  const std::string& get() const;
  ValueSP evaluate(ExpressionEvaluationContext& evaluation_context) const override;
  std::string debug_dump() override;

protected:
  StringConstant(const std::string& string);
  std::string m_string;
};
using StringConstantSP = std::shared_ptr<StringConstant>;

class Symbol: public Expression
{
public:
  static std::shared_ptr<Symbol> create(const std::string& symbol);
  const std::string& get() const;
  ValueSP evaluate(ExpressionEvaluationContext& evaluation_context) const override;
  std::string debug_dump() override;

protected:
  Symbol(const std::string& symbol);
  std::string m_symbol;
};
using SymbolSP = std::shared_ptr<Symbol>;

enum class UnaryOperatorEnum
{
  LOW_BYTE,
  HIGH_BYTE,
};

class UnaryOperator: public ASTNode
{
public:
  static std::shared_ptr<UnaryOperator> create(UnaryOperatorEnum unary_operator);
  UnaryOperatorEnum get() const;
  std::string debug_dump() override;

protected:
  UnaryOperator(UnaryOperatorEnum unary_operator);
  UnaryOperatorEnum m_unary_operator;
};
using UnaryOperatorSP = std::shared_ptr<UnaryOperator>;

class UnaryOperatorExpression: public Expression
{
public:
  static std::shared_ptr<UnaryOperatorExpression> create(std::shared_ptr<UnaryOperator> unary_operator,
							 std::shared_ptr<Expression> subexpression);
  ValueSP evaluate(ExpressionEvaluationContext& evaluation_context) const;
  std::string debug_dump() override;

protected:
  UnaryOperatorExpression(std::shared_ptr<UnaryOperator> unary_operator,
			  std::shared_ptr<Expression> subexpression);
  std::shared_ptr<UnaryOperator> m_unary_operator;
  std::shared_ptr<Expression> m_subexpression;
};

enum class BinaryOperatorEnum
{
  ADDITION,
  SUBTRACTION,
  MULTIPLICATION,
  DIVISION
};

class BinaryOperator: public ASTNode
{
public:
  static std::shared_ptr<BinaryOperator> create(BinaryOperatorEnum binary_operator);
  BinaryOperatorEnum get() const;
  std::string debug_dump() override;

protected:
  BinaryOperator(BinaryOperatorEnum binary_operator);
  BinaryOperatorEnum m_binary_operator;
};
using BinaryOperatorSP = std::shared_ptr<BinaryOperator>;

class BinaryOperatorExpression: public Expression
{
public:
  static std::shared_ptr<BinaryOperatorExpression> create(std::shared_ptr<Expression> left_subexpression,
							  std::shared_ptr<BinaryOperator> binary_operator,
							  std::shared_ptr<Expression> right_subexpression);
  ValueSP evaluate(ExpressionEvaluationContext& evaluation_context) const;
  std::string debug_dump() override;

protected:
  BinaryOperatorExpression(std::shared_ptr<Expression> left_subexpression,
			   std::shared_ptr<BinaryOperator> binary_operator,
			   std::shared_ptr<Expression> right_subexpression);
  std::shared_ptr<Expression> m_left_subexpression;
  std::shared_ptr<BinaryOperator> m_binary_operator;
  std::shared_ptr<Expression> m_right_subexpression;
};

class ExpressionList: public ASTNode
{
public:
  static std::shared_ptr<ExpressionList> create();
  void append_expression(std::shared_ptr<Expression> expression_sp);
  const std::vector<std::shared_ptr<Expression>>& get() const;
  std::string debug_dump() override;

protected:
  ExpressionList();
  std::vector<std::shared_ptr<Expression>> m_expressions;
};
using ExpressionListSP = std::shared_ptr<ExpressionList>;

class Statement: public ASTNode
{
public:
  static std::shared_ptr<Statement> create();

  void set_label(const std::string& label);
  void set_mnemonic(const std::string& mnemonic);
  void add_operand(ExpressionSP operand);
  void set_operands(const std::vector<ExpressionSP>& operands);

  const std::string& get_label() const;
  const std::string& get_mnemonic() const;
  std::size_t get_operand_count() const;
  const std::shared_ptr<Expression> get_operand(std::size_t index) const;  // zero-indexed
  const std::vector<std::shared_ptr<Expression>>& get_operands() const;

  std::string debug_dump() override;

protected:
  Statement();
  std::string m_label;
  std::string m_mnemonic;
  std::vector<std::shared_ptr<Expression>> m_operands;
};
using StatementSP = std::shared_ptr<Statement>;

#endif // AST_NODE_HH
