// pseudo_op.hh
//
// Copyright 2025 Eric Smith
// SPDX-License-Identifier: GPL-3.0-only

#ifndef PSEUDO_OP_HH
#define PSEUDO_OP_HH

#include <map>
#include <memory>
#include <string>

#include <magic_enum.hpp>
#include <magic_enum_containers.hpp>

class PseudoOp
{
public:
  enum class PseudoOpEnum
  {
    ASCII,
    BYTE,
    DEF,
    END,
    HBYTE,
    LINK,
    LIST,
    LOC,
    NOLIST,
    PAGE,
    WORD,
  };

  enum class Flag
  {
    LABEL_DISALLOWED, // not allowed to have a label
    LABEL_ISNT_LOC,   // label does not get set to location counter
  };

  using Flags = magic_enum::containers::bitset<Flag>;

  struct Info
  {
    const std::string mnemonic;
    PseudoOpEnum pseudo_op;
    Flags flags = Flags();
  };

  static std::shared_ptr<PseudoOp> create();

  static bool valid_mnemonic(const std::string& mnemonic);
  static const Info& lookup_mnemonic(const std::string& mnemonic);

protected:
  PseudoOp();

  static const magic_enum::containers::array<PseudoOpEnum, Info> s_by_enum;
  static const std::map<std::string, PseudoOpEnum> s_by_mnemonic;
};

#endif // PSEUDO_OP_HH
