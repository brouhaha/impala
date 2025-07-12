// ast_stack.hh
//
// Copyright 2025 Eric Smith
// SPDX-License-Identifier: GPL-3.0-only

#ifndef AST_STACK_HH
#define AST_STACK_HH

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

class ASTNode;
using ASTNodeSP = std::shared_ptr<ASTNode>;

class ASTStackUnderflow: public std::runtime_error
{
public:
  ASTStackUnderflow();
};

class ASTStack
{
public:
  static std::shared_ptr<ASTStack> create();

  bool empty() const;
  std::size_t size() const;

  void clear();
  void push(ASTNodeSP node);
  ASTNodeSP pop();
  ASTNodeSP peek_top() const;

  template <typename T>
  std::shared_ptr<T> pop()
  {
    return std::dynamic_pointer_cast<T>(pop());
  }

  template <typename T>
  std::shared_ptr<T> peek_top() const
  {
    return std::dynamic_pointer_cast<T>(peek_top());
  }

  void debug_dump(std::ostream& os);

protected:
  ASTStack();
  std::vector<ASTNodeSP> m_stack;
};
using ASTStackSP = std::shared_ptr<ASTStack>;

#endif // AST_STACK
