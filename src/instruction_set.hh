// instruction_set.hh
//
// Copyright 2025 Eric Smith
// SPDX-License-Identifier: GPL-3.0-only

#ifndef INSTRUCTION_SET_HH
#define INSTRUCTION_SET_HH

#include <cstdint>
#include <map>
#include <memory>
#include <string>

#include <magic_enum.hpp>
#include <magic_enum_containers.hpp>

class InstructionSet
{
public:
  static std::shared_ptr<InstructionSet> create();

  class UnrecognizedMnemonic: public std::runtime_error
  {
  public:
    UnrecognizedMnemonic(const std::string& mnemonic);
  };

  enum class Set
  {
    UNDEFINED,
    BASE,
    ROCKWELL,
    CMOS,
  };

  enum class Mode
  {
    // real modes
    IMPLIED,
    ACCUMULATOR,
    IMMEDIATE,
    ZERO_PAGE,
    ZERO_PAGE_X,
    ZERO_PAGE_Y,
    ZP_X_IND,
    ZP_IND_Y,
    ABSOLUTE,
    ABSOLUTE_X,
    ABSOLUTE_Y,
    ABSOLUTE_IND,
    RELATIVE,
  };

  static std::uint32_t get_length(Mode mode);

  struct Info
  {
    const std::string mnemonic;
    Set set;
    Mode mode;
    std::uint8_t opcode;
  };

  bool valid_mnemonic(const std::string& mnemonic) const;

  const std::vector<Info>& get(const std::string& mnemonic) const;

  static std::uint8_t operand_size_bytes(Mode mode);

  static bool pal65_compatible_modes(Mode m1, Mode m2);

protected:
  InstructionSet();

  std::map<std::string, std::vector<Info>> m_by_mnemonic;
};

#endif // INSTRUCTION_SET_HH
