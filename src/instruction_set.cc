// instruction_set.cc
//
// Copyright 2025 Eric Smith
// SPDX-License-Identifier: GPL-3.0-only

#include <bitset>
#include <format>
#include <stdexcept>

#include "instruction_set.hh"
#include "utility.hh"

using enum InstructionSet::Set;
using enum InstructionSet::Mode;

const magic_enum::containers::array<InstructionSet::Mode, std::uint8_t> s_operand_size_bytes
{
  /* IMPLIED      */ 0,
  /* ACCUMULATOR  */ 0,
  /* IMMEDIATE    */ 1,
  /* ZERO_PAGE    */ 1,
  /* ZERO_PAGE_X  */ 1,
  /* ZERO_PAGE_Y  */ 1,
  /* ZP_X_IND     */ 1,
  /* ZP_IND_Y     */ 1,
  /* ABSOLUTE     */ 2,
  /* ABSOLUTE_X   */ 2,
  /* ABSOLUTE_Y   */ 2,
  /* ABSOLUTE_IND */ 2,
  /* RELATIVE     */ 1,
};

const magic_enum::containers::array<InstructionSet::Mode, std::string> s_mos_address_mode_prefixes
{
  /* IMPLIED      */ "",
  /* ACCUMULATOR  */ "",
  /* IMMEDIATE    */ "#",
  /* ZERO_PAGE    */ "",
  /* ZERO_PAGE_X  */ "",
  /* ZERO_PAGE_Y  */ "",
  /* ZP_X_IND     */ "(",
  /* ZP_IND_Y     */ "(",
  /* ABSOLUTE     */ "",
  /* ABSOLUTE_X   */ "",
  /* ABSOLUTE_Y   */ "",
  /* ABSOLUTE_IND */ "(",
  /* RELATIVE     */ "",
};

const magic_enum::containers::array<InstructionSet::Mode, std::string> s_mos_address_mode_suffixes
{
  /* IMPLIED      */ "",
  /* ACCUMULATOR  */ "",
  /* IMMEDIATE    */ "",
  /* ZERO_PAGE    */ "",
  /* ZERO_PAGE_X  */ ",x",
  /* ZERO_PAGE_Y  */ ",y",
  /* ZP_X_IND     */ ",x)",
  /* ZP_IND_Y     */ "),y",
  /* ABSOLUTE     */ "",
  /* ABSOLUTE_X   */ ",x",
  /* ABSOLUTE_Y   */ ",y",
  /* ABSOLUTE_IND */ ")",
  /* RELATIVE     */ "",
};

const magic_enum::containers::array<InstructionSet::Mode, std::string> s_pal65_address_mode_suffixes
{
  /* IMPLIED      */ "",
  /* ACCUMULATOR  */ "a",
  /* IMMEDIATE    */ "#",
  /* ZERO_PAGE    */ "",
  /* ZERO_PAGE_X  */ "x",
  /* ZERO_PAGE_Y  */ "y",
  /* ZP_X_IND     */ "x@",
  /* ZP_IND_Y     */ "@y",
  /* ABSOLUTE     */ "",
  /* ABSOLUTE_X   */ "x",
  /* ABSOLUTE_Y   */ "y",
  /* ABSOLUTE_IND */ "@",
  /* RELATIVE     */ "",
};

const std::vector<InstructionSet::Info> s_main_table
{
  { "adc", BASE, IMMEDIATE,    0x69 },
  { "adc", BASE, ZERO_PAGE,    0x65 },
  { "adc", BASE, ZERO_PAGE_X,  0x75 },
  { "adc", BASE, ZP_X_IND,     0x61 },
  { "adc", BASE, ZP_IND_Y,     0x71 },
  { "adc", BASE, ABSOLUTE,     0x6d },
  { "adc", BASE, ABSOLUTE_X,   0x7d },
  { "adc", BASE, ABSOLUTE_Y,   0x79 },

  { "and", BASE, IMMEDIATE,    0x29 },
  { "and", BASE, ZERO_PAGE,    0x25 },
  { "and", BASE, ZP_X_IND,     0x21 },
  { "and", BASE, ZP_IND_Y,     0x31 },
  { "and", BASE, ABSOLUTE,     0x2d },
  { "and", BASE, ABSOLUTE_X,   0x3d },
  { "and", BASE, ABSOLUTE_Y,   0x39 },

  { "asl", BASE, ACCUMULATOR,  0x0a },
  { "asl", BASE, ZERO_PAGE,    0x06 },
  { "asl", BASE, ZP_X_IND,     0x16 },
  { "asl", BASE, ABSOLUTE,     0x0e },
  { "asl", BASE, ABSOLUTE_X,   0x1e },

  { "bcc", BASE, RELATIVE,     0x90 },

  { "bcs", BASE, RELATIVE,     0xb0 },

  { "beq", BASE, RELATIVE,     0xf0 },

  { "bit", BASE, ZERO_PAGE,    0x24 },
  { "bit", BASE, ABSOLUTE,     0x2c },

  { "bmi", BASE, RELATIVE,     0x30 },

  { "bne", BASE, RELATIVE,     0xd0 },

  { "bpl", BASE, RELATIVE,     0x10 },

  { "brk", BASE, IMPLIED,      0x00 },

  { "bvc", BASE, RELATIVE,     0x50 },

  { "bvs", BASE, RELATIVE,     0x70 },

  { "clc", BASE, IMPLIED,      0x18 },

  { "cld", BASE, IMPLIED,      0xd8 },

  { "cli", BASE, IMPLIED,      0x58 },

  { "clv", BASE, IMPLIED,      0xb8 },

  { "cmp", BASE, IMMEDIATE,    0xc9 },
  { "cmp", BASE, ZERO_PAGE,    0xc5 },
  { "cmp", BASE, ZERO_PAGE_X,  0xd5 },
  { "cmp", BASE, ZP_X_IND,     0xc1 },
  { "cmp", BASE, ZP_IND_Y,     0xd1 },
  { "cmp", BASE, ABSOLUTE,     0xcd },
  { "cmp", BASE, ABSOLUTE_X,   0xdd },
  { "cmp", BASE, ABSOLUTE_Y,   0xd9 },

  { "cpx", BASE, IMMEDIATE,    0xe0 },
  { "cpx", BASE, ZERO_PAGE,    0xe4 },
  { "cpx", BASE, ABSOLUTE,     0xec },

  { "cpy", BASE, IMMEDIATE,    0xc0 },
  { "cpy", BASE, ZERO_PAGE,    0xc4 },
  { "cpy", BASE, ABSOLUTE,     0xcc },

  { "dec", BASE, ZERO_PAGE,    0xc6 },
  { "dec", BASE, ZERO_PAGE_X,  0xd6 },
  { "dec", BASE, ABSOLUTE,     0xce },
  { "dec", BASE, ABSOLUTE_X,   0xde },

  { "dex", BASE, IMPLIED,      0xca },

  { "dey", BASE, IMPLIED,      0x88 },

  { "eor", BASE, IMMEDIATE,    0x49 },
  { "eor", BASE, ZERO_PAGE,    0x45 },
  { "eor", BASE, ZERO_PAGE_X,  0x55 },
  { "eor", BASE, ZP_X_IND,     0x41 },
  { "eor", BASE, ZP_IND_Y,     0x51 },
  { "eor", BASE, ABSOLUTE,     0x4d },
  { "eor", BASE, ABSOLUTE_X,   0x5d },
  { "eor", BASE, ABSOLUTE_Y,   0x59 },

  { "inc", BASE, ZERO_PAGE,    0xe6 },
  { "inc", BASE, ZERO_PAGE_X,  0xf6 },
  { "inc", BASE, ABSOLUTE,     0xee },
  { "inc", BASE, ABSOLUTE_X,   0xfe },

  { "inx", BASE, IMPLIED,      0xe8 },

  { "iny", BASE, IMPLIED,      0xc8 },

  { "jmp", BASE, ABSOLUTE,     0x4c },
  { "jmp", BASE, ABSOLUTE_IND, 0x6c },

  { "jsr", BASE, ABSOLUTE,     0x20 },
  
  { "lda", BASE, IMMEDIATE,    0xa9 },
  { "lda", BASE, ZERO_PAGE,    0xa5 },
  { "lda", BASE, ZERO_PAGE_X,  0xb5 },
  { "lda", BASE, ZP_X_IND,     0xa1 },
  { "lda", BASE, ZP_IND_Y,     0xb1 },
  { "lda", BASE, ABSOLUTE,     0xad },
  { "lda", BASE, ABSOLUTE_X,   0xbd },
  { "lda", BASE, ABSOLUTE_Y,   0xb9 },

  { "ldx", BASE, IMMEDIATE,    0xa2 },
  { "ldx", BASE, ZERO_PAGE,    0xa6 },
  { "ldx", BASE, ABSOLUTE,     0xae },
  { "ldx", BASE, ABSOLUTE_Y,   0xbe },

  { "ldy", BASE, IMMEDIATE,    0xa0 },
  { "ldy", BASE, ZERO_PAGE,    0xa4 },
  { "ldy", BASE, ZERO_PAGE_X,  0xb4 },
  { "ldy", BASE, ABSOLUTE,     0xac },
  { "ldy", BASE, ABSOLUTE_X,   0xbc },

  { "lsr", BASE, ACCUMULATOR,  0x4a },
  { "lsr", BASE, ZERO_PAGE,    0x46 },
  { "lsr", BASE, ZP_X_IND,     0x56 },
  { "lsr", BASE, ABSOLUTE,     0x4e },
  { "lsr", BASE, ABSOLUTE_X,   0x5e },

  { "nop", BASE, IMPLIED,      0xea },

  { "ora", BASE, IMMEDIATE,    0x09 },
  { "ora", BASE, ZERO_PAGE,    0x05 },
  { "ora", BASE, ZP_X_IND,     0x01 },
  { "ora", BASE, ZP_IND_Y,     0x11 },
  { "ora", BASE, ABSOLUTE,     0x0d },
  { "ora", BASE, ABSOLUTE_X,   0x1d },
  { "ora", BASE, ABSOLUTE_Y,   0x19 },

  { "pha", BASE, IMPLIED,      0x48 },
  
  { "php", BASE, IMPLIED,      0x08 },

  { "pla", BASE, IMPLIED,      0x68 },

  { "plp", BASE, IMPLIED,      0x28 },

  { "rol", BASE, ACCUMULATOR,  0x2a },
  { "rol", BASE, ZERO_PAGE,    0x26 },
  { "rol", BASE, ZP_X_IND,     0x36 },
  { "rol", BASE, ABSOLUTE,     0x2e },
  { "rol", BASE, ABSOLUTE_X,   0x3e },

  { "ror", BASE, ACCUMULATOR,  0x6a },
  { "ror", BASE, ZERO_PAGE,    0x66 },
  { "ror", BASE, ZP_X_IND,     0x76 },
  { "ror", BASE, ABSOLUTE,     0x6e },
  { "ror", BASE, ABSOLUTE_X,   0x7e },

  { "rti", BASE, IMPLIED,      0x40 },

  { "rts", BASE, IMPLIED,      0x60 },

  { "sbc", BASE, IMMEDIATE,    0xe9 },
  { "sbc", BASE, ZERO_PAGE,    0xe5 },
  { "sbc", BASE, ZERO_PAGE_X,  0xf5 },
  { "sbc", BASE, ZP_X_IND,     0xe1 },
  { "sbc", BASE, ZP_IND_Y,     0xf1 },
  { "sbc", BASE, ABSOLUTE,     0xed },
  { "sbc", BASE, ABSOLUTE_X,   0xfd },
  { "sbc", BASE, ABSOLUTE_Y,   0xf9 },

  { "sec", BASE, IMPLIED,      0x38 },

  { "sed", BASE, IMPLIED,      0xf8 },

  { "sei", BASE, IMPLIED,      0x78 },

  { "sta", BASE, ZERO_PAGE,    0x85 },
  { "sta", BASE, ZERO_PAGE_X,  0x95 },
  { "sta", BASE, ZP_X_IND,     0x81 },
  { "sta", BASE, ZP_IND_Y,     0x91 },
  { "sta", BASE, ABSOLUTE,     0x8d },
  { "sta", BASE, ABSOLUTE_X,   0x9d },
  { "sta", BASE, ABSOLUTE_Y,   0x99 },

  { "stx", BASE, ZERO_PAGE,    0x86 },
  { "stx", BASE, ZERO_PAGE_Y,  0x96 },
  { "stx", BASE, ABSOLUTE,     0x8e },

  { "sty", BASE, ZERO_PAGE,    0x84 },
  { "sty", BASE, ZERO_PAGE_X,  0x94 },
  { "sty", BASE, ABSOLUTE,     0x8c },

  { "tax", BASE, IMPLIED,      0xaa },

  { "tay", BASE, IMPLIED,      0xa8 },

  { "tsx", BASE, IMPLIED,      0xba },

  { "txa", BASE, IMPLIED,      0x8a },

  { "txs", BASE, IMPLIED,      0x9a },

  { "tya", BASE, IMPLIED,      0x98 },
};

InstructionSet::UnrecognizedMnemonic::UnrecognizedMnemonic(const std::string& mnemonic):
  std::runtime_error(std::format("unrecognized mnemonic {}", mnemonic))
{
}

std::shared_ptr<InstructionSet> InstructionSet::create()
{
  auto p = new InstructionSet();
  return std::shared_ptr<InstructionSet>(p);
}

InstructionSet::InstructionSet()
{
  std::bitset<0x100> opcode_used;
  for (const auto& info: s_main_table)
  {
    std::string pal65_mnemonic = info.mnemonic + s_pal65_address_mode_suffixes[info.mode];
    if (opcode_used[info.opcode])
    {
      throw std::logic_error(std::format("duplicate opcode {:02x}", info.opcode));
    }
    opcode_used[info.opcode] = true;
    if (m_by_mnemonic.contains(pal65_mnemonic))
    {
      if (! pal65_compatible_modes(m_by_mnemonic.at(pal65_mnemonic).at(0).mode, info.mode))
      {
	throw std::logic_error(std::format("duplicate PAL65 mnemonic {}", pal65_mnemonic));
      }
    }
    m_by_mnemonic[pal65_mnemonic].push_back(info);
  }
}

bool InstructionSet::valid_mnemonic(const std::string& mnemonic) const
{
  std::string s = utility::downcase_string(mnemonic);
  return m_by_mnemonic.contains(s);
}

const std::vector<InstructionSet::Info>& InstructionSet::get(const std::string& mnemonic) const
{
  std::string s = utility::downcase_string(mnemonic);
  if (! m_by_mnemonic.contains(s))
  {
    throw UnrecognizedMnemonic(mnemonic);
  }
  return m_by_mnemonic.at(s);
}

std::uint32_t InstructionSet::get_length(Mode mode)
{
  switch (mode)
  {
  case IMPLIED:
  case ACCUMULATOR:
    return 1;

  case IMMEDIATE:
    return 2;

  case ZERO_PAGE:
  case ZERO_PAGE_X:
  case ZERO_PAGE_Y:
  case ZP_X_IND:
  case ZP_IND_Y:
    return 2;

  case ABSOLUTE:
  case ABSOLUTE_X:
  case ABSOLUTE_Y:
  case ABSOLUTE_IND:
    return 3;

  case RELATIVE:
    return 2;

  default:
    throw std::logic_error("unrecognized address mode");
  }
}

std::uint8_t InstructionSet::operand_size_bytes(Mode mode)
{
  return s_operand_size_bytes[mode];
}

bool InstructionSet::pal65_compatible_modes(Mode m1, Mode m2)
{
  if (((m1 == ZERO_PAGE) && (m2 == ABSOLUTE)) ||
      ((m1 == ABSOLUTE)  && (m2 == ZERO_PAGE)))
  {
    return true;
  }
  if (((m1 == ZERO_PAGE_X) && (m2 == ABSOLUTE_X)) ||
      ((m1 == ABSOLUTE_X)  && (m2 == ZERO_PAGE_X)))
  {
    return true;
  }
  if (((m1 == ZERO_PAGE_Y) && (m2 == ABSOLUTE_Y)) ||
      ((m1 == ABSOLUTE_Y)  && (m2 == ZERO_PAGE_Y)))
  {
    return true;
  }
  return false;
}
