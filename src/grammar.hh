// grammar.hh
//
// Copyright 2025 Eric Smith
// SPDX-License-Identifier: GPL-3.0-only

#ifndef GRAMMAR_HH
#define GRAMMAR_HH

#include <span>

#include <tao/pegtl.hpp>
namespace pegtl = tao::pegtl;

#include "parser.hh"
#include "utility.hh"

namespace grammar
{
  struct whitespace: pegtl::plus<pegtl::space> {};

  struct alpha: pegtl::ranges<'a', 'z', 'A', 'Z'> {};

  struct odigit: pegtl::range<'0', '7'> {};

  struct digit: pegtl::range<'0', '9'> {};

  struct xdigit: pegtl::ranges<'0', '9', 'a', 'f', 'A', 'F'> {};

  struct alphanumeric: pegtl::ranges<'a', 'z', 'A', 'Z', '0', '9'> {};

  struct symbol: pegtl::seq<alpha,
			    pegtl::rep_max<9, alphanumeric>> {};

  struct octal_constant: pegtl::seq<pegtl::one<'%'>,
				    pegtl::plus<odigit>> {};

  struct decimal_constant: pegtl::plus<digit> {};

  struct hexadecimal_constant: pegtl::seq<pegtl::one<'$'>,
					  pegtl::plus<xdigit>> {};

  struct character_constant: pegtl::seq<pegtl::one<'\''>,
					pegtl::ascii::print> {};

  struct string_constant_single_quote: pegtl::seq<pegtl::one<'\''>,
						  pegtl::star<pegtl::not_one<'\''>>,
						  pegtl::one<'\''>> {};

  struct string_constant_double_quote: pegtl::seq<pegtl::one<'"'>,
						  pegtl::star<pegtl::not_one<'"'>>,
						  pegtl::one<'"'>> {};

  struct string_constant_question_mark: pegtl::seq<pegtl::one<'?'>,
						   pegtl::star<pegtl::not_one<'?'>>,
						   pegtl::one<'?'>> {};

  struct string_constant: pegtl::sor<string_constant_single_quote,
				     string_constant_double_quote,
				     string_constant_question_mark> {};

  struct location_counter: pegtl::one<'.'> {};

  struct constant: pegtl::sor<octal_constant,
			      decimal_constant,
			      hexadecimal_constant,
			      character_constant,
			      location_counter> {};

  struct unary_operator: pegtl::one<'<', '>'> {};

  struct expression_symbol: pegtl::seq<symbol> {};

  struct unary_expression: pegtl::seq<unary_operator,
				      expression_symbol> {};

  struct binary_adding_operator: pegtl::one<'+', '-'> {};

  struct binary_multiplying_operator: pegtl::one<'*', '/'> {};

  struct expression;

  struct parenthesized_expression: pegtl::seq<pegtl::one<'('>,
					      expression,
					      pegtl::one<')'>> {};

  struct factor: pegtl::sor<constant,
			    expression_symbol,
			    unary_expression,
			    parenthesized_expression> {};

  struct term_additional_factor: pegtl::seq<binary_multiplying_operator,
					    factor> {};

  struct term: pegtl::seq<factor,
			  pegtl::star<term_additional_factor>> {};

  struct expression_additional_term: pegtl::seq<binary_adding_operator,
						term> {};

  struct expression: pegtl::seq<term,
				pegtl::star<expression_additional_term>> {};

  struct expression_list_head: pegtl::seq<expression> {};

  struct expression_list_additional: pegtl::seq<expression> {};

  struct expression_list: pegtl::seq<expression_list_head,
				     pegtl::star<pegtl::seq<pegtl::one<','>,
							    expression_list_additional>>> {};

  struct expression_list_empty: pegtl::seq<> {};

  struct label_empty: pegtl::seq<> {};

  struct label_nonempty: pegtl::seq<symbol,
				    pegtl::one<':'>> {};

  struct label: pegtl::seq<pegtl::opt<whitespace>,
			   pegtl::sor<label_nonempty,
				      label_empty>,
			   pegtl::opt<whitespace>>{};

  struct mnemonic_instruction_zero_operand: pegtl::sor<TAO_PEGTL_ISTRING("asla"),
						       TAO_PEGTL_ISTRING("brk"),
						       TAO_PEGTL_ISTRING("clc"),
						       TAO_PEGTL_ISTRING("cld"),
						       TAO_PEGTL_ISTRING("cli"),
						       TAO_PEGTL_ISTRING("clv"),
						       TAO_PEGTL_ISTRING("dex"),
						       TAO_PEGTL_ISTRING("dey"),
						       TAO_PEGTL_ISTRING("inx"),
						       TAO_PEGTL_ISTRING("iny"),
						       TAO_PEGTL_ISTRING("lsra"),
						       TAO_PEGTL_ISTRING("nop"),
						       TAO_PEGTL_ISTRING("pha"),
						       TAO_PEGTL_ISTRING("php"),
						       TAO_PEGTL_ISTRING("pla"),
						       TAO_PEGTL_ISTRING("plp"),
						       TAO_PEGTL_ISTRING("rola"),
						       TAO_PEGTL_ISTRING("rora"),
						       TAO_PEGTL_ISTRING("rti"),
						       TAO_PEGTL_ISTRING("rts"),
						       TAO_PEGTL_ISTRING("sec"),
						       TAO_PEGTL_ISTRING("sed"),
						       TAO_PEGTL_ISTRING("sei"),
						       TAO_PEGTL_ISTRING("tax"),
						       TAO_PEGTL_ISTRING("tay"),
						       TAO_PEGTL_ISTRING("tsx"),
						       TAO_PEGTL_ISTRING("txa"),
						       TAO_PEGTL_ISTRING("txs"),
						       TAO_PEGTL_ISTRING("tya")> {};

  struct mnemonic_instruction_one_operand: pegtl::sor<TAO_PEGTL_ISTRING("adc"),
                                                      TAO_PEGTL_ISTRING("and"),
                                                      TAO_PEGTL_ISTRING("asl"),
                                                      TAO_PEGTL_ISTRING("bcc"),
                                                      TAO_PEGTL_ISTRING("bcs"),
                                                      TAO_PEGTL_ISTRING("beq"),
                                                      TAO_PEGTL_ISTRING("bit"),
                                                      TAO_PEGTL_ISTRING("bmi"),
                                                      TAO_PEGTL_ISTRING("bne"),
                                                      TAO_PEGTL_ISTRING("bpl"),
                                                      TAO_PEGTL_ISTRING("bvc"),
                                                      TAO_PEGTL_ISTRING("bvs"),
                                                      TAO_PEGTL_ISTRING("cmp"),
                                                      TAO_PEGTL_ISTRING("cpx"),
                                                      TAO_PEGTL_ISTRING("cpy"),
                                                      TAO_PEGTL_ISTRING("dec"),
                                                      TAO_PEGTL_ISTRING("eor"),
                                                      TAO_PEGTL_ISTRING("inc"),
                                                      TAO_PEGTL_ISTRING("jmp"),
                                                      TAO_PEGTL_ISTRING("jsr"),
                                                      TAO_PEGTL_ISTRING("lda"),
                                                      TAO_PEGTL_ISTRING("ldx"),
                                                      TAO_PEGTL_ISTRING("ldy"),
                                                      TAO_PEGTL_ISTRING("lsr"),
                                                      TAO_PEGTL_ISTRING("ora"),
                                                      TAO_PEGTL_ISTRING("rol"),
                                                      TAO_PEGTL_ISTRING("ror"),
                                                      TAO_PEGTL_ISTRING("sbc"),
                                                      TAO_PEGTL_ISTRING("sta"),
                                                      TAO_PEGTL_ISTRING("stx"),
						      TAO_PEGTL_ISTRING("sty")> {};

  struct address_mode_suffix_accumulator: pegtl::one<'a', 'A'> {};
  struct address_mode_suffix_immediate: pegtl::one<'#'> {};
  struct address_mode_suffix_indexed_by_x: pegtl::one<'x', 'X'> {};
  struct address_mode_suffix_indexed_by_y: pegtl::one<'y', 'Y'> {};
  struct address_mode_suffix_indexed_by_x_indirect: pegtl::seq<pegtl::one<'x', 'X'>, pegtl::one<'@'>> {};
  struct address_mode_suffix_indirect_indexed_by_y: pegtl::seq<pegtl::one<'@'>, pegtl::one<'y', 'Y'>> {};
  struct address_mode_suffix_indirect: pegtl::one<'@'> {};

  struct address_mode_one_operand_suffix: pegtl::sor<address_mode_suffix_immediate,
						     address_mode_suffix_indexed_by_x,
						     address_mode_suffix_indexed_by_y,
						     address_mode_suffix_indexed_by_x_indirect,
						     address_mode_suffix_indirect_indexed_by_y,
						     address_mode_suffix_indirect> {};

  struct mnemonic_instruction_one_operand_suffixed: pegtl::seq<mnemonic_instruction_one_operand,
							       pegtl::opt<address_mode_one_operand_suffix>> {};

  struct mnemonic_pseudo_ascii:  TAO_PEGTL_ISTRING(".ascii") {};
  struct mnemonic_pseudo_byte:   TAO_PEGTL_ISTRING(".byte") {};
  struct mnemonic_pseudo_def:    TAO_PEGTL_ISTRING(".def") {};
  struct mnemonic_pseudo_end:    TAO_PEGTL_ISTRING(".end") {};
  struct mnemonic_pseudo_hbyte:  TAO_PEGTL_ISTRING(".hbyte") {};
  struct mnemonic_pseudo_link:   TAO_PEGTL_ISTRING(".link") {};
  struct mnemonic_pseudo_list:   TAO_PEGTL_ISTRING(".list") {};
  struct mnemonic_pseudo_loc:    TAO_PEGTL_ISTRING(".loc") {};
  struct mnemonic_pseudo_nolist: TAO_PEGTL_ISTRING(".nolist") {};
  struct mnemonic_pseudo_page:   TAO_PEGTL_ISTRING(".page") {};
  struct mnemonic_pseudo_word:   TAO_PEGTL_ISTRING(".word") {};

  struct mnemonic_pseudo_zero_operand: pegtl::sor<mnemonic_pseudo_end,
						  mnemonic_pseudo_list,
						  mnemonic_pseudo_nolist,
						  mnemonic_pseudo_page> {};

  struct mnemonic_pseudo_variable_operand: pegtl::sor<mnemonic_pseudo_byte,
						      mnemonic_pseudo_hbyte,
						      mnemonic_pseudo_loc,
						      mnemonic_pseudo_word> {};

  struct instruction_zero_operand: pegtl::seq<mnemonic_instruction_zero_operand> {};

  struct instruction_one_operand: pegtl::seq<mnemonic_instruction_one_operand_suffixed,
					     whitespace,
					     expression> {};

  struct pseudo_op_zero_operand: pegtl::seq<mnemonic_pseudo_zero_operand> {};

  struct pseudo_op_variable_operand: pegtl::seq<mnemonic_pseudo_variable_operand,
						pegtl::sor<pegtl::seq<whitespace,
								      expression_list>,
							   expression_list_empty>> {};

  struct pseudo_op_ascii: pegtl::seq<mnemonic_pseudo_ascii,
				     whitespace,
				     string_constant> {};

  struct pseudo_op_def: pegtl::seq<mnemonic_pseudo_def,
				   whitespace,
				   expression_symbol,
				   pegtl::opt<whitespace>,
				   pegtl::one<'='>,
				   pegtl::opt<whitespace>,
				   expression> {};

  struct pseudo_op_link: pegtl::seq<mnemonic_pseudo_link,
				    whitespace,
				    symbol> {};

  struct comment: pegtl::opt<pegtl::seq<whitespace,
					pegtl::one<';'>,
					pegtl::star<pegtl::any>>> {};

  struct statement_empty: pegtl::seq<> {};

  struct statement: pegtl::seq<label,
			       pegtl::sor<instruction_zero_operand,
					  instruction_one_operand,
					  pseudo_op_zero_operand,
					  pseudo_op_variable_operand,
					  pseudo_op_ascii,
					  pseudo_op_def,
					  pseudo_op_link,
					  statement_empty>,
			       comment> {};

  template<typename Rule>
  struct action: pegtl::nothing<Rule> {};

  template<>
  struct action<expression_symbol>
  {
    template<typename ActionInput>
    static void apply(const ActionInput& in,
		      Parser& parser)
    {
      auto symbol_sp = Symbol::create(utility::downcase_string(in.string()));
      parser.m_ast_stack->push(symbol_sp);
    }
  };

  template<>
  struct action<octal_constant>
  {
    template<typename ActionInput>
    static void apply(const ActionInput& in,
		      Parser& parser)
    {
      unsigned long long value = std::stoull(in.begin() + 1, nullptr, 8);
      auto constant_sp = Constant::create(value);
      parser.m_ast_stack->push(constant_sp);
    }
  };

  template<>
  struct action<decimal_constant>
  {
    template<typename ActionInput>
    static void apply(const ActionInput& in,
		      Parser& parser)
    {
      unsigned long long value = std::stoull(in.begin());
      auto constant_sp = Constant::create(value);
      parser.m_ast_stack->push(constant_sp);
    }
  };

  template<>
  struct action<hexadecimal_constant>
  {
    template<typename ActionInput>
    static void apply(const ActionInput& in,
		      Parser& parser)
    {
      unsigned long long value = std::stoull(in.begin() + 1, nullptr, 16);
      auto constant_sp = Constant::create(value);
      parser.m_ast_stack->push(constant_sp);
    }
  };

  template<>
  struct action<character_constant>
  {
    template<typename ActionInput>
    static void apply(const ActionInput& in,
		      Parser& parser)
    {
      unsigned long long value = in.begin()[1];
      auto constant_sp = Constant::create(value);
      parser.m_ast_stack->push(constant_sp);
    }
  };

  template<>
  struct action<string_constant>
  {
    template<typename ActionInput>
    static void apply(const ActionInput& in,
		      Parser& parser)
    {
      std::string s = in.string();
      s = s.substr(1, s.size() - 2);
      auto string_constant_sp = StringConstant::create(s);
      parser.m_ast_stack->push(string_constant_sp);
    }
  };

  template<>
  struct action<location_counter>
  {
    template<typename ActionInput>
    static void apply([[maybe_unused]] const ActionInput& in,
		      Parser& parser)
    {
      // push constant XXX need a special AST node to handle this properly,
      // e.g. for L'*
      auto constant_sp = Constant::create(parser.get_location_counter());
      parser.m_ast_stack->push(constant_sp);
    }
  };

  template<>
  struct action<binary_adding_operator>
  {
    template<typename ActionInput>
    static void apply([[maybe_unused]] const ActionInput& in,
		      Parser& parser)
    {
      BinaryOperatorEnum binary_operator_enum;
      std::string s = in.string();
      if (s == "+")
      {
	binary_operator_enum = BinaryOperatorEnum::ADDITION;
      }
      else if (s == "-")
      {
	binary_operator_enum = BinaryOperatorEnum::SUBTRACTION;
      }
      else
      {
	throw std::logic_error(std::format("internal error: unrecognized binary adding operator \"{}\"", s));
      }
      // push BinaryOperator
      auto binary_operator = BinaryOperator::create(binary_operator_enum);
      parser.m_ast_stack->push(binary_operator);
    }
  };

  template<>
  struct action<unary_operator>
  {
    template<typename ActionInput>
    static void apply([[maybe_unused]] const ActionInput& in,
		      Parser& parser)
    {
      UnaryOperatorEnum unary_operator_enum;
      std::string s = in.string();
      if (s == "<")
      {
	unary_operator_enum = UnaryOperatorEnum::LOW_BYTE;
      }
      else if (s == ">")
      {
	unary_operator_enum = UnaryOperatorEnum::HIGH_BYTE;
      }
      else
      {
	throw std::logic_error(std::format("internal error: unrecognized unary operator \"{}\"", s));
      }
      // push UnaryOperator
      auto unary_operator = UnaryOperator::create(unary_operator_enum);
      parser.m_ast_stack->push(unary_operator);
    }
  };

  template<>
  struct action<unary_expression>
  {
    template<typename ActionInput>
    static void apply([[maybe_unused]] const ActionInput& in,
		      Parser& parser)
    {
      // pop operand Expression
      auto operand_expression_sp = parser.m_ast_stack->pop<Expression>();

      // pop UnaryOperator
      auto unary_operator = parser.m_ast_stack->pop<UnaryOperator>();

      // push UnaryOperatorExpression
      auto unary_operator_expression_sp = UnaryOperatorExpression::create(unary_operator,
									  operand_expression_sp);
      parser.m_ast_stack->push(unary_operator_expression_sp);
    }
  };

  template<>
  struct action<binary_multiplying_operator>
  {
    template<typename ActionInput>
    static void apply([[maybe_unused]] const ActionInput& in,
		      Parser& parser)
    {
      BinaryOperatorEnum binary_operator_enum;
      std::string s = in.string();
      if (s == "*")
      {
	binary_operator_enum = BinaryOperatorEnum::MULTIPLICATION;
      }
      else if (s == "/")
      {
	binary_operator_enum = BinaryOperatorEnum::DIVISION;
      }
      else
      {
	throw std::logic_error(std::format("internal error: unrecognized binary multiplying operator \"{}\"", s));
      }
      // push BinaryOperator
      auto binary_operator = BinaryOperator::create(binary_operator_enum);
      parser.m_ast_stack->push(binary_operator);
    }
  };

  template<>
  struct action<term_additional_factor>
  {
    template<typename ActionInput>
    static void apply([[maybe_unused]] const ActionInput& in,
		      Parser& parser)
    {
      // pop second operand Expression
      auto second_operand_expression_sp = parser.m_ast_stack->pop<Expression>();

      // pop BinaryOperator
      auto binary_operator = parser.m_ast_stack->pop<BinaryOperator>();

      // pop first operand Exprssion
      auto first_operand_expression_sp = parser.m_ast_stack->pop<Expression>();

      // push BinaryOperatorExpression
      auto binary_operator_expression_sp = BinaryOperatorExpression::create(first_operand_expression_sp,
									    binary_operator,
									    second_operand_expression_sp);
      parser.m_ast_stack->push(binary_operator_expression_sp);
    }
  };

  template<>
  struct action<expression_additional_term>
  {
    template<typename ActionInput>
    static void apply([[maybe_unused]] const ActionInput& in,
		      Parser& parser)
    {
      // pop second operand Expression
      auto second_operand_expression_sp = parser.m_ast_stack->pop<Expression>();

      // pop BinaryOperator
      auto binary_operator = parser.m_ast_stack->pop<BinaryOperator>();

      // pop first operand Exprssion
      auto first_operand_expression_sp = parser.m_ast_stack->pop<Expression>();

      // push BinaryOperatorExpression
      auto binary_operator_expression_sp = BinaryOperatorExpression::create(first_operand_expression_sp,
									    binary_operator,
									    second_operand_expression_sp);
      parser.m_ast_stack->push(binary_operator_expression_sp);
    }
  };

  template<>
  struct action<expression_list_head>
  {
    template<typename ActionInput>
    static void apply([[maybe_unused]] const ActionInput& in,
		      Parser& parser)
    {
      // pop expression
      auto expression_sp = parser.m_ast_stack->pop<Expression>();

      // push expression list
      auto expression_list_sp = ExpressionList::create();
      expression_list_sp->append_expression(expression_sp);
      parser.m_ast_stack->push(expression_list_sp);
    }
  };

  template<>
  struct action<expression_list_additional>
  {
    template<typename ActionInput>
    static void apply([[maybe_unused]] const ActionInput& in,
		      Parser& parser)
    {
      // pop expression
      auto expression_sp = parser.m_ast_stack->pop<Expression>();

      // peek expression list
      auto expression_list_sp = parser.m_ast_stack->peek_top<ExpressionList>();
      expression_list_sp->append_expression(expression_sp);
    }
  };

  template<>
  struct action<expression_list_empty>
  {
    template<typename ActionInput>
    static void apply([[maybe_unused]] const ActionInput& in,
		      Parser& parser)
    {
      // push expression list
      auto expression_list_sp = ExpressionList::create();
      parser.m_ast_stack->push(expression_list_sp);
    }
  };

  template<>
  struct action<label_empty>
  {
    template<typename ActionInput>
    static void apply([[maybe_unused]] const ActionInput& in,
		      Parser& parser)
    {
      // push empty symbol
      auto symbol_sp = Symbol::create("");
      parser.m_ast_stack->push(symbol_sp);
    }
  };

  template<>
  struct action<label_nonempty>
  {
    template<typename ActionInput>
    static void apply(const ActionInput& in,
		      Parser& parser)
    {
      const std::string& s = in.string();
      // push symbol
      auto symbol_sp = Symbol::create(utility::downcase_string(s.substr(0, s.size() - 1)));
      parser.m_ast_stack->push(symbol_sp);
    }
  };

  template<>
  struct action<mnemonic_instruction_zero_operand>
  {
    template<typename ActionInput>
    static void apply(const ActionInput& in,
		      Parser& parser)
    {
      // push Mnemonic
      std::string m = in.string();
      parser.m_ast_stack->push(Mnemonic::create(m));
    }
  };

  template<>
  struct action<mnemonic_instruction_one_operand_suffixed>
  {
    template<typename ActionInput>
    static void apply(const ActionInput& in,
		      Parser& parser)
    {
      // push Mnemonic
      std::string m = in.string();
      parser.m_ast_stack->push(Mnemonic::create(m));
    }
  };

  template<>
  struct action<mnemonic_pseudo_zero_operand>
  {
    template<typename ActionInput>
    static void apply(const ActionInput& in,
		      Parser& parser)
    {
      // push Mnemonic
      std::string m = in.string();
      parser.m_ast_stack->push(Mnemonic::create(m));
    }
  };

  template<>
  struct action<mnemonic_pseudo_variable_operand>
  {
    template<typename ActionInput>
    static void apply(const ActionInput& in,
		      Parser& parser)
    {
      // push Mnemonic
      std::string m = in.string();
      parser.m_ast_stack->push(Mnemonic::create(m));
    }
  };

  template<>
  struct action<mnemonic_pseudo_ascii>
  {
    template<typename ActionInput>
    static void apply(const ActionInput& in,
		      Parser& parser)
    {
      // push Mnemonic
      std::string m = in.string();
      parser.m_ast_stack->push(Mnemonic::create(m));
    }
  };

  template<>
  struct action<mnemonic_pseudo_def>
  {
    template<typename ActionInput>
    static void apply(const ActionInput& in,
		      Parser& parser)
    {
      // push Mnemonic
      std::string m = in.string();
      parser.m_ast_stack->push(Mnemonic::create(m));
    }
  };

  template<>
  struct action<mnemonic_pseudo_link>
  {
    template<typename ActionInput>
    static void apply(const ActionInput& in,
		      Parser& parser)
    {
      // push Mnemonic
      std::string m = in.string();
      parser.m_ast_stack->push(Mnemonic::create(m));
    }
  };

  template<>
  struct action<instruction_zero_operand>
  {
    template<typename ActionInput>
    static void apply([[maybe_unused]] const ActionInput& in,
		      [[maybe_unused]] Parser& parser)
    {
      // pop mnemonic
      auto mnemonic_sp = parser.m_ast_stack->pop<Mnemonic>();

      // push statement
      auto statement_sp = Statement::create();
      statement_sp->set_mnemonic(mnemonic_sp->get());
      parser.m_ast_stack->push(statement_sp);
    }
  };

  template<>
  struct action<instruction_one_operand>
  {
    template<typename ActionInput>
    static void apply([[maybe_unused]] const ActionInput& in,
		      [[maybe_unused]] Parser& parser)
    {
      // pop operand
      auto operand_sp = parser.m_ast_stack->pop<Expression>();

      // pop mnemonic
      auto mnemonic_sp = parser.m_ast_stack->pop<Mnemonic>();

      // push statement
      auto statement_sp = Statement::create();
      statement_sp->set_mnemonic(mnemonic_sp->get());
      statement_sp->add_operand(operand_sp);
      parser.m_ast_stack->push(statement_sp);
    }
  };

  template<>
  struct action<pseudo_op_zero_operand>
  {
    template<typename ActionInput>
    static void apply([[maybe_unused]] const ActionInput& in,
		      [[maybe_unused]] Parser& parser)
    {
      // pop mnemonic
      auto mnemonic_sp = parser.m_ast_stack->pop<Mnemonic>();

      // push statement
      auto statement_sp = Statement::create();
      statement_sp->set_mnemonic(mnemonic_sp->get());
      parser.m_ast_stack->push(statement_sp);
    }
  };

  template <>
  struct action<pseudo_op_variable_operand>
  {
    template<typename ActionInput>
    static void apply([[maybe_unused]] const ActionInput& in,
		      [[maybe_unused]] Parser& parser)
    {
      // pop operands
      auto operands_sp = parser.m_ast_stack->pop<ExpressionList>();

      // pop mnemonic
      auto mnemonic_sp = parser.m_ast_stack->pop<Mnemonic>();

      // push statement
      auto statement_sp = Statement::create();
      statement_sp->set_mnemonic(mnemonic_sp->get());
      statement_sp->set_operands(operands_sp->get());
      parser.m_ast_stack->push(statement_sp);
    }
  };

  template <>
  struct action<pseudo_op_ascii>
  {
    template<typename ActionInput>
    static void apply([[maybe_unused]] const ActionInput& in,
		      [[maybe_unused]] Parser& parser)
    {
      // pop operand
      auto operand_sp = parser.m_ast_stack->pop<Expression>();

      // pop mnemonic
      auto mnemonic_sp = parser.m_ast_stack->pop<Mnemonic>();

      // push statement
      auto statement_sp = Statement::create();
      statement_sp->set_mnemonic(mnemonic_sp->get());
      statement_sp->add_operand(operand_sp);
      parser.m_ast_stack->push(statement_sp);
    }
  };

  template <>
  struct action<pseudo_op_def>
  {
    template<typename ActionInput>
    static void apply([[maybe_unused]] const ActionInput& in,
		      [[maybe_unused]] Parser& parser)
    {
      // pop operands
      auto operand2_sp = parser.m_ast_stack->pop<Expression>();
      auto operand1_sp = parser.m_ast_stack->pop<Expression>();

      // pop mnemonic
      auto mnemonic_sp = parser.m_ast_stack->pop<Mnemonic>();

      // push statement
      auto statement_sp = Statement::create();
      statement_sp->set_mnemonic(mnemonic_sp->get());
      statement_sp->add_operand(operand1_sp);
      statement_sp->add_operand(operand2_sp);
      parser.m_ast_stack->push(statement_sp);
    }
  };

  template <>
  struct action<statement_empty>
  {
    template<typename ActionInput>
    static void apply([[maybe_unused]] const ActionInput& in,
		      Parser& parser)
    {
      // push statement
      auto statement_sp = Statement::create();
      statement_sp->set_mnemonic("");
      parser.m_ast_stack->push(statement_sp);
    }
  };

  template<>
  struct action<statement>
  {
    template<typename ActionInput>
    static void apply([[maybe_unused]] const ActionInput& in,
		      [[maybe_unused]] Parser& parser)
    {
      // pop statement
      auto statement_sp = parser.m_ast_stack->pop<Statement>();

      // pop label
      auto label_sp = parser.m_ast_stack->pop<Symbol>();

      // peek statement
      statement_sp->set_label(label_sp->get());

      // push statement
      parser.m_ast_stack->push(statement_sp);
    }
  };

} // end namespace Grammar

#endif // GRAMMAR_HH
