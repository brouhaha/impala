// pseudo_op.cc
//
// Copyright 2025 Eric Smith
// SPDX-License-Identifier: GPL-3.0-only

#include <format>
#include <stdexcept>

#include <magic_enum_utility.hpp>

#include "pseudo_op.hh"
#include "utility.hh"

using enum PseudoOp::PseudoOpEnum;
using enum PseudoOp::Flag;

std::shared_ptr<PseudoOp> PseudoOp::create()
{
  auto p = new PseudoOp();
  return std::shared_ptr<PseudoOp>(p);
}

bool PseudoOp::valid_mnemonic(const std::string& mnemonic)
{
  std::string s = utility::downcase_string(mnemonic);
  return s_by_mnemonic.contains(s);
}

const PseudoOp::Info& PseudoOp::lookup_mnemonic(const std::string& mnemonic)
{
  std::string s = utility::downcase_string(mnemonic);
  PseudoOpEnum pseudo_op_enum = s_by_mnemonic.at(s);
  return s_by_enum[pseudo_op_enum];
}

PseudoOp::PseudoOp()
{
  magic_enum::enum_for_each<PseudoOpEnum>([] (PseudoOpEnum pseudo_op)
      {
	if (s_by_enum[pseudo_op].pseudo_op != pseudo_op)
	{
	  throw std::logic_error(std::format("PseudoOp: s_by_mnemonic table out of order at entry for {}\n",
					     magic_enum::enum_name(pseudo_op)));
	}
      });
}

const magic_enum::containers::array<PseudoOp::PseudoOpEnum, PseudoOp::Info> PseudoOp::s_by_enum
{
  Info { ".ascii",  ASCII },
  Info { ".byte",   BYTE },
  Info { ".def",    DEF },
  Info { ".end",    END },
  Info { ".hbyte",  HBYTE },
  Info { ".link",   LINK },
  Info { ".list",   LIST },
  Info { ".loc",    LOC },
  Info { ".nolist", NOLIST },
  Info { ".page",   PAGE },
  Info { ".word",   WORD },
};

const std::map<std::string, PseudoOp::PseudoOpEnum> PseudoOp::s_by_mnemonic
{
  { ".ascii",  ASCII },
  { ".byte",   BYTE },
  { ".def",    DEF },
  { ".end",    END },
  { ".hbyte",  HBYTE },
  { ".link",   LINK },
  { ".list",   LIST },
  { ".loc",    LOC },
  { ".nolist", NOLIST },
  { ".page",   PAGE },
  { ".word",   WORD },
};
