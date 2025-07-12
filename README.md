# impala - 6502 cross asssembler compatible with PAL65/ASM65 syntax

## Introduction

impala is a cross assembler for the 6502, using the non-standard PAL65
syntax. Impala is written in C++23 (supported by GCC)

impala development is hosted at the
[impala Github repository](https://github.com/brouhaha/impala/).

## License

impala is Copyright 2025 Eric Smith.

impala is Free Software, licensed under the terms of the
[GNU General Public License 3.0](https://www.gnu.org/licenses/gpl-3.0.en.html)
(GPL version 3.0 only, not later versions).

## PAL65/ASM65 History

The 6502 Group was founded in 1975, originally as a special interest group
of the Denver Amateur Computer Society. Due to the shortage of available
6502 development software, members of the 6502 group created editors,
assemblers, language interpreters, and even compilers for the 6502,
initially for homebrew systems, or systems from The Digital Group, but
eventually for the Apple II as well.

Early on, some members of the 6502 Group created and used cross-developemnt
tools that ran on a DECsystem-10, including an assembler called PAL65,
though eventually the majority of the developemnt was done natively on
6502-based systems.

The PAL65 cross-assembler, and the compatible ASM65 native assembler,
used a different syntax for designating the 6502's various addressing modes.

There is a fairly limited amount of source code in existence using
PAL65 syntax (which by convention uses the filename extension ".P65"),
and very little new PAL65 source code is being written. There are few
(if any) assemblers for modern operating systems that recognize this
syntax.

## PAL65/ASM65 Syntax

For details, see a
[scan of the ASM65 user manual](http://www.trailingedge.com/apple2/ASM65.pdf).

Instead of using special syntax around the operand to designate an addressing mode,
PAL65 syntax appends addressing mode suffixes to the instruction mnemonics.

| addressing Mode                  | Standard MOS Technology syntax | PAL65/ASM65 syntax |
| -------------------------------- | ------------------------------ | ------------------ |
| implied                          | PHA                            | PHA                |
| immediate                        | LDA #n                         | LDA#  n            |
| accumulator                      | ROL A                          | ROLA               |
| zero page                        | LDA zp                         | LDA   zp           |
| zero page indexed by X           | LDA zp,X                       | LDAX  zp           |
| zero page indexed by Y           | LDA zp,Y                       | LDAY  zp           |
| zero page indexed by X, indirect | LDA (zp,X)                     | LDAX@ zp           |
| zero page indirect, indexed by Y | LDA (zp),Y                     | LDA@Y zp           |
| absolute                         | LDA abs                        | LDA   abs          |
| absolute, indexed by X           | LDA abs,X                      | LDAX  abs          |
| absolute, indexed by Y           | LDA abs,Y                      | LDAY  abs          |
| indirect                         | JMP (abs)                      | JMP@  abs          |

In the table, n denotes an expression (or constant or symbol) yielding
a byte value (0 to 255 decimal), zp denotes an 8-bit zero page
address, and abs denotes a 16-bit address.

## Building impala

Impala requires the
[Magic Enum](https://github.com/Neargye/magic_enum)
and
[PEGTL](https://github.com/taocpp/PEGTL)
header-only libraries.

The build system uses
[SCons](https://scons.org),
which requires
[Python](https://www.python.org/)
3.6 or later.

To build impala, make sure the Magic Enum and PEGTL headers are in
your system include path, per their documentation. From the top level
directory (above the "src" directory), type "scons". The resulting
executable will be build/impala.

## Running impala

impala is executed from a command line. A single argument provides the
name of the assembly language source file to be assembled. If the
source file name ends in ".p65', then the corresponding output binary
and listing files will have ".bin" and ".lst" extensions replacing the
".p65". Otherwise, ".bin" and ".lst" will be appended to the source
file name to obtain the binary and listing file names.

## Object file format

The original ASM65 object file format used on the Apex operating
system was not documented, as far as I can tell.  The impala object
file output is thought to be compatible with that of ASM65, though
this has not been tested.  It is an ASCII file containing hexadecimal
values, with hexadecimal digits A through F required to be in upper
case. Addresses are four hexadecimal digits, preceded by an asterisk
("*"). Data bytes are two hexadecimal digits each. Apex end-of-file
was a 0x1a character (control-Z, ASCII SUB). Any characters in the
file other than hexadecimal digits and the asterisk are ignored. This
includes whitespace, newlines, etc, none of which are currently
generated by impala.

## Limitations

* impala has only been tested to the extent that it appears to give
  reasonable results when assembling several old ASM65 source files,
  though the resulting object files have not yet been executed or
  verified.

* The .LINK pseudo-op is not yet implemented. Due to limitations of
  the APEX operating system which ASM65 commonly ran on, there was not
  a modern include file mechanism. Instead, a source file could end in
  a link directive, which would switch input to a specified file, but
  any lines beyond the link directive in the original file would not.
  be processed.

* The .ASCII pseudo-op accepted a string of text using any character
  as the delimiter (though by convention usually a single quote), and
  the closing terminator was optional. impala only supports use of the
  single quote, double quote, and question mark as delimiters, and the
  closing delimiter is required.

* impala error handling is extremely poor. Assembly errors generally
  abort the assembly, with a C++ exception message written to the
  console output.

* The listing file generated by impala is not divided into pages, and
  the .PAGE pseudo-op is ignored. The listing format does not exactly
  match that of ASM65. Like ASM65, if a source line generates more
  than three bytes of object code, only the first three are shown.
