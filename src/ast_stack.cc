// ast_stack.cc
//
// Copyright 2025 Eric Smith
// SPDX-License-Identifier: GPL-3.0-only

#include "ast_node.hh"
#include "ast_stack.hh"

ASTStackUnderflow::ASTStackUnderflow():
  std::runtime_error("AST stack underflow")
{
}

std::shared_ptr<ASTStack> ASTStack::create()
{
  auto p = new ASTStack();
  return std::shared_ptr<ASTStack>(p);
}

ASTStack::ASTStack()
{
}

bool ASTStack::empty() const
{
  return m_stack.empty();
}

std::size_t ASTStack::size() const
{
  return m_stack.size();
}

void ASTStack::clear()
{
  m_stack.clear();
}

void ASTStack::push(ASTNodeSP node)
{
  m_stack.push_back(node);
}

ASTNodeSP ASTStack::pop()
{
  ASTNodeSP node = m_stack.back();
  m_stack.pop_back();
  return node;
}

ASTNodeSP ASTStack::peek_top() const
{
  return m_stack.back();
}

void ASTStack::debug_dump(std::ostream& os)
{
  for (unsigned i = m_stack.size() - 1;
       i != static_cast<unsigned>(-1);
       --i)
  {
    os << std::format("stack[{}] = {}\n", i, m_stack[i]->debug_dump());
  }
}
