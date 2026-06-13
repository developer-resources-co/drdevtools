# Calypsi 68000 release notes

Version 5.17, May 2026.

## Highlights

* ISO C 99 compiler for Motorola 68000. This is a freestanding
  implementation with many features you will typically find in a
  hosted compiler.

* Fully re-entrant code model.

* Support for all integer types up to 64 bits `long long`.

* Floating point supported (32 and 64 bits IEEE-754).

* Full support for struct, union, typedef and what you expect to
  find in C.

* Support for (stack allocated) variable sized arrays.

* Optimizing compiler that can output source level debugging
  information.

* Source code debugger included.

* Support for ELF/DWARF, hex output as well as raw and `.pgz`
  (Foenix) style application files.

## Known problems

* The C parser has a known problem in that the keyword form for
  attributes, e.g. `__far` may give unexpected
   errors in some situations. If that happens, use the
   `__attribute__((!far))` form instead.

* Statement expressions are always executed, even inside `if` statement
  expressions with `&&` or `||` operators arranged so that the
  execution should be conditional.

* Complex data type is not supported.

* The debugger documentation is lacking at the moment.

## News

### Version 5.15

* Add compiler command line option `--align-functions`.

* Use `divsl.l` and `divul.l` instructions for 32-bit modulo on 68020
  and up.

* Add support for `cinv` (invalidate cache lines) instruction in
  the assembler.

### Version 5.14

* Add missing libraries for 68040 and 68060, for the A2560K.

### Version 5.13

* The Foenix base addresses for VRAM, Gavin and Beatrix which were
  intended to allow A2560K and A2560U to be configured dynamically
  have been removed. The base addresses are now fixed and depend on
  the `--target=a2560k` or `--target=a2560u` settings. The board
  support has been updated accordingly. The effect of this is slightly
  better code and that there is no way to generate a single executable
  that will run on both target systems.

* The `target` runtime attribute is now explicitly set to
  `none-specified` if not used. This is to prevent mixing modules and
  ensure build integrity.

* Add `__CALYPSI_DEBUG__` macro that is defined and set to 1 if
  compiling with debug information is enabled.

### Version 5.11

* The options `--target=a2560k` and `--target=a2560u` replaces
  `--target=foenix` as the `foenix` is too vague now with additional
  products coming.

* Some changes in board support naming convention, but this should be
  fully transparent if you link with `--target=c64` or
  `--target=mega65`.

### Version 5.10

* Bit-fields layout order is now big endian in the container, placing
  the first bit-field of the container in the leftmost position.

### Version 5.9

* Add support for 68060 by omitting instructions not present on this
  variant. This includes 64-bit divide and multiply, `movep`, `chk2`,
  `cmp2`, `cas2` and `cas`. A separate set of 68060 libraries is now
  included.

### Version 5.8

* Assembly sections using the same name as a compiler data section
  now sets the corresponding type qualifier. This makes the auto
  placement in the linker also work for assembly projects.

### Version 5.7

* Enable tree shaking for C, which means that unreferenced functions
  are no longer included in the output. This can reduce the size of
  the application. You can enable the previous behavior with the
  linker option `--no-tree-shaking`.

* Add local labels based on `---` and `++++` sequences to the
  assembler. References to labels with minus characters goes backwards
  to closest label matching and references to labels with plus
  characters goes forward to the closest matching label.

* Switch tables now use `#pragma clang section rodata` if active.

* C library stub routines are now expected to return a negative errno
  value when there is a problem and should not set the `errno`
  variable. The existing board support routines and semi-hosting have
  been updated accordingly.

### Version 5.6

* Improve code generation for various shift and mask patterns.

* Bitfield assignment operations now performs many operations in place
  without the normal unpacking and normalization, resulting in better
  code.

* Code generate immediate source operand with `AND`, `OR`, `MUL` and
  `DIV` instructions. This did not previously happen due to a minor
  oversight.

### Version 5.4

* Add library build scripts to assist in rebuilding the C library with
  different settings. See the `library-build` folder in the
  installation directory.

### Version 5.3

* Amiga: Add a significant amount of varargs stubs.These are normally
  part of `amiga.lib`, but here they are built into the Amiga board
  support package for Calypsi.

* Add new command option `--inline-on-matching-custom-text-section`
  which prevents inlining of functions placed in a custom section,
  unless the function being inlined into belongs to the same custom
  section. This is useful if the memory system is being manipulated
  and the function much execute in that specific custom
  section.

* `#pragma clang section rodata="..."` now also affects the section
  used for string literals and compiler generated static
  initializers.

### Version 5.2

* Add support for linking overlay systems and linking to one place and
  placing it elsewhere with the `scatter-to` memory property.

* No longer gives overlapping memory errors in the RAW output format.

* Add `#pragma rtattr`.

* Add partial support for 68080, including `ADDIW`, `CMPIW`, `DBcc.L`,
  `EXTUB`, `EXTUW`, `MOV3Q`, `MOVS` and `MOVZ`  instructions.

* Add target and board support for TOS.

* Make the assembler directive `.incbin` load files relative to the
  current source directory. If that file path does not exist, it will
  try as before, based on the given path, relative from the
  compilation directory.

### Version 5.1

* Add support for 68020.

* Inline assembly is now supported. Refer to the Calypsi C compiler
  guide for more details.

* Compiler will now use hex numbers in generated assembly
  code. Smaller numbers and certain larger even numbers may still be
  shown in decimal.

* Formatter variant for `printf()` and `scanf()` are now automatically
  selected based by the linker based on the formatter strings used in
  the application. There is no longer a need to specify the formatter
  on the linker command line, but it can still be done that way if you
  for some reason want to override the selection mechanism.

* Add `simple_call` function attribute to enable a simpler calling
  convention that pushes all parameters on the stack.

* The built-in board support packages now specify their own default
  `cstartup` attribute which eliminates the need to specify it on
  linker command line.

* Add `--cstartup=VALUE` linker option to make it easier to specify a
  custom C startup module. This was previously done using the
  `--rtattr cstartup=VALUE` option (which still works).

### Version 4.4

* Stub routines `_Stub_access()`, `_Stub_raise()` and `_Stub_fcntl()`
  are obsolete and have been removed.

* Enable the `overloadable` attribute which allows for overloading
  functions similar to C++. Overloaded functions are subject to name
  mangling.

* Enable the `unused` attribute. This can be used to silence
  diagnostics about unused entities that cannot be removed.

### Version 4.3

* Board support for Amiga is now provided.

* The package name is now `calypsi-68000` rather than `calypsi-68k`
  and it is recommended that you uninstall any previous version before
  installing this release. This was done to make the naming more
  consistent.

* Stub routines `_Stub_puts()` and `_Stub_putchar()` are obsolete and
  have been removed.

* The `_Exit()` function no longer does any cleanup. In the past it
  flushed and closed I/O stream, this is now done by `exit()`.

* Add command line options `--include-system` and `--include-system-after`
  to add system include directories, either before or after the one
  provided by default.

### Version 4.2

* C library is now automatically deduced based on the provided object
  files and the `--target` option given to the linker.

* The Foenix A2560 board support is now provided. This includes header
  files, pre-built libraries and linker rule files.
  If such linker file is seen on the command line (`a2560*.scm`) is on
  the command line, the appropriate `a2560-*.a` library is
  automatically added to the linker command line.

* Automatically generated linker rules are now included in the list
  file. They can also appear in some linker fatal error messages.

* Linker list files now includes the cross reference part by default.
  Also added the command line option `--no-cross-reference` to make it
  possible to disable it.

### Version 4.1

* Command line option `-v` is removed, use the long version `--version`
  to display the version number. This is because `-v` often means
  verbose in tools.

* Please change all `#include <stubs.h>` to `#include <calypsi/stubs.h>`
  as the stubs file has a new home. There is a temporary redirect
  header file in place to prevent compilation errors at this point,
  but this will be removed in a future release.

* Enable format attribute. The compiler will now warn on mismatched
  variable arguments compared to the supplied formatting string.

* Add named local labels in the assembler. Any identifier (not just a
  number) followed by a `$` is now seen as a local label. This allows
  for more readable names of local labels.

* Add `.argdelim` directive to allow assembler macro arguments to
  contain comma characters.

* Foenix C library names are now fully in lower case.

* Add `.incbin` directive to allow for inserting binary data into
  assembler.

* The compiler can now generate assembly source output using the
  command line option `--assembly-source`.

* Add support for TOS relocatable output from the linker.

* The assembler now allows labels to be indented with spaces, provided
  they are followed by spaces or be surrounded by back quotes.

### Version 3.6.1

* The S-record format can now also be specified using S19, S28 or S37.
  Doing so selects a specific address size format variant. The
  previous `S-record` format selected a variants that was suitable
  based on target.

* The `__CALYPSI_TARGET_M68K__` define is deprecated and replaced by
  `__CALYPSI_TARGET_68000__` for consistency with other targets.
  The old defintion is retained for backwards compatibility.

* The linker accepts much simplified placement rules file (`.scm`)
  which makes it a lot easier to specify the memory system. See the
  user guide, Linker rules file section for more information.

* The assembler now accepts `add` as a synonym for the `addi`
  instruction. This also applies to the instructions with immediate
  form, such as `and`, `cmp`, `eor`, `sub`.

* The Foenix A2560 math unit is now fully used by the compiler when
  specifying `--target=foenix`.

* Support for generating Amiga Hunk output from the linker.

* Semi-hosted debugger stubs are now fully implemented by the debugger.
  This makes it possible to simulate a file system on the target by
  using the file system on the host.

* Remote debugging on a target is now supported and the debugger can
  start in either normal or supervisor mode using the
  `--supervisor-run``

* Add compiler option `-M` for dependency generation control.

* Add assembler directives `.end`, `.global` and `.globl`.

## Corrections

### Version 5.16, April 2026

* Using a struct object as an initializer of an auto struct caused
  an internal error.

* Fix a problem with Apple calls causing an internal error (message
  about function dynamicSize).

* Fix a problem with 68020 bit field instructions that could cause and
  internal error at optimization level 1 and above, if used in inline
  assembly.

* In some very rare situations function calls with many parameters,
  preceded by a parameter expressions utilizing low level runtime
  support routine could cause an internal error.

* Fix a rounding problem with floating point add or subtract where the
  mantissa ended up with all bits set and then was rounded up. This
  caused the result to be half of what is should be.

### Version 5.15, January 2026

* Fix a problem with `long long` shifts using a register variable as
  shift could clobber the register variable.

* Fix a problem with 16-bit right shift in some situations could be
  performed with arithmetic (sign preserving) shift when it should
  have been done using unsigned right shift.

* A `TST` on a register could incorrectly be optimized away at `-O2`
  in some situations if it was preceded by a `CLR` instruction.

### Version 5.14, November 2025

* In some situations the `__get_interrupt_state()` intrinsic could
  cause an internal error when using `-O1` or above.

* Target specific header files for other systems than the active one
  was incorrectly exposed in the include path.

* Right shifts could in some specific situations generate incorrect
  code, when there was a conversion to 8 bits size after the operation
  and the shift operand was 16 bits.

* Fix an internal error 'Non-exhaustive patterns in function defOp1'
  that could occur with `-O2` and cores 68020 and above.

* Linker: Improve handling of scattered placement groups with
  alignment.

### Version 5.13, October 2025

* Linker: Preserve address order of placement groups when they are
  scattered to another memory.

* Multi-dimension array expressions could generate an internal error.

* Debugger: Accessing a field inside an anonymous `struct` by
  expression resulted in an error message that the member could not be
  accessed.

* Debugger: Accessing fields through multiple members failed.

### Version 5.12, September 2025

* Fixed a problem with initializing a static array with a string
  literal causing an internal error.

* Correct installation directories A2560K and A2560U targets so that
  header file can be found.

* Fix an internal error related to initializing a `char` array with a
  string literal surrounded by redundant curly braces.

### Version 5.11, August 2025

* Fixed some issues with combining memories in raw output format into
  a single file.

* Fix problem with Fedora installation in that installed libraries
  very severely stripped down and not working properly.

### Version 5.10, April 2025

* A bit-field of size 0 which fills out the current container,
  incorrectly caused an extra empty container to be allocated if the
  current one was not empty.

* Correct the `SysBase` macro in the Amiga board support.

* `_Static_assert` could cause an internal error.

* A function call returning a `struct` with at least two internal
  layers of `struct` or `union`, immediately accessing a single
  element on return from the function call would result in an
  internal error.

* 68080: Fix incorrect code generation using `extub.l` when it should
  have masked with a small constant instead.

* Amiga: Loading the FD file for the CIA library caused an internal
  error as it has multiple base addresses and pass them along as
  parameters instead of using a global variable.

* Casting a static data address to a function pointer could in some
  situations cause an internal error.

### Version 5.9, March 2025

* Parameter numbering in inline assembly now correctly starts the
  input list with `%0` when there is no output.

* Correct a code generator bug that could occur in some situations
  when a register variable was used twice in the same expression.

### Version 5.8.1, February 2025

* Switch statements with a single statement body (not a composite)
  caused an internal error.

### Version 5.7.2, January 2025

* Corrections made to the tree shaker to improve precision and ensure
  that local references made from the defined roots are properly
  made. Previously it could sometimes generate incorrect undefined
  symbol errors.

* Disable use of Foenix A2560 math unit as it is somewhat unclear if
  it works across the A2560 machines. This allows the
  `--target=foenix` command line option to be used to enable A2560
  board support without imposing the math unit.

* Fix a problem with the question mark operator and void result
  causing an internal error.

* Bump Foenix 68000 board support to use proper prototypes for empty
  parameter lists.

* Root symbols inside archives (libraries) are currently ignored for
  the sake of it being an archive and not a core part of the
  application. In case you perform staged builds with libraries and
  need a root, e.g. an interrupt handler, you may need to specify it
  using the new `--root-symbol` option in the linker.

### Version 5.7.1, December 2024

* Tree shaking could cause incorrect errors about undefined symbols
  when debug information was enabled.

* Expressions that shifted the operand first right then left, or vice
  versa, could in some situations generate incorrect code when `-O2`
  was used.

* Correct a problem with `struct` padding that could get wrong if an
  odd size element was followed by a `struct` as the last element of
  the `struct`.

### Version 5.7, November 2024

* Static functions that are no longer referenced as the result of
  inlining are now properly removed.

* Some corrections in the C library related to I/O operations and
  returning error values from stub routines not being handled
  correctly.

* The assembler now properly parses certain variants of word shift
  instructions, such as `lsl.w d0,d1` and `lsr.w d4`. Previously they
  generated parse errors.

* A function with only a single return statement having an expression
  with a non-auto variable could cause an internal error.

* Expressions `&*(char**)0x1234` caused an internal error.

### Version 5.6, September 2024

* The assembler would not accept 32-bit immediate constant with
  32-bit 68020 multiply and divide instructions. It incorrectly caused
  value a range error.

* Correct a code generator problem with expressions such as
  `*(uint8_t *)0xd020 = (along & 0x0000FF00UL) >> 8;` generating
  incorrect code.

* Inline assembler accessing the return value with `%0` when
  there is no return value defined in the constraint section
  now gives a controlled error rather than a weird internal
  error.

* Unsigned `int` sized bitfields did not operate in signed type
  as it should. This is because `int` bitfield have lower conversion
  rank than a normal `int` type. This affected right shift, divide
  and modulo operations.

* Fixed an internal error in the linker
  `Translator/Linker/List.hs:119:35-72: Irrefutable pattern failed for
  pattern Just mem`.

### Version 5.5, August 2024

* Correct a problem with shrinking precision of right shift which
  caused it to performed signed instead of unsigned.
  Example code that was handled wrong: `uint8_t c = (uint8_t)(a + b) >> 1`.

* Operator `/=` and `%=` are now rewritten to shift and mask
  operations respectively, when the operand is a constant that allows
  it.

* Using `.extern` directive in inline assembly caused an internal
  error when using `-O2`.

### Version 5.4, May 2024

* Inlining a function in a loop with a parameter that was read only
  once as part of being the parameter to the inlined function, would
  get clobbered if it was written to inside the inlined function.

* The linker now gives precedence to a replacement C library given on
  the command line.

### Version 5.3, March 2024

* When adjacent RAW memories were merged the size calculation
  in the list file was off.

* Clobbered registers in inline assembly statements were not always
  obeyed in the register allocation.

### Version 5.2, February 2024

* Selecting simplified exit using `--rtattr exit=simplified` caused an
  error about clashed `exit` runtime attribute instead of selecting
  simplified `exit()`.

* Inline assembly with empty output combined with using named input
  arguments caused an internal error.

* Applications now downloaded 2-3 times faster when using the remote
  debugger. This is because the gdbserver `X` (binary write memory)
  command is used.

* Add missing control registers to `MOVEC` instruction for 68020,
  68030 and 68040.

### Version 5.1, November 2023

* Compound literals could in some situations cause an internal error.

* Forward declared enumerations could cause a segfault.

* Casting a pointer to an integer in a compound literal could cause an
  internal error.

### Version 4.4, June 2023

* The `stderr` stream was not properly initialized.

* Correct `_Generic` to work properly. This is actually part of
  C11, but supported by the front-end in current (non-strict C99)
  default setting.

* Correct `freopen()` which was not properly implemented.

* Correct `tmpnam()` and `tmpfile()`, also use the current directory
  for temporary files as there may not be and directory support in the
  file system used.

* Allocating a stack frame larger than 32K caused an internal error.

### Version 4.3, May 2023

* Correct an internal error in `ControlFlowOptimize` which could
  happen for certain shapes of control flow graphs.

* GCC style statement expressions now works, using then used to cause
  an internal error.

* Correct `__typeof()` extension which could result in an internal error.

* Correct `fopen()` which always would fail.

### Version 4.2, April 2023

* A string literal ending with a dollar sign no longer cause the
  compiler to throw an internal error.

* Assignment operator with an address address expression could in some
  situations generate incorrect code.

### Version 4.1, April 2023

* Fix internal error with `__disable_interrupts()` and
  `__enable_interrupts()` intrinsics and optimization level 2 causing
  an internal error.

* Fixed a problem in the inliner. A variable passed in as parameter
  would not be properly treated if the inlined function took the
  address of it. This could lead to incorrect code or an internal
  error.

* Linker output could sometimes go wrong when there was empty code
  sections.

### Version 3.6.12, February 2023

* Expressions such as `value + '0' - 1` where simplified incorrectly.

* The debugger command line option `-i mi2` was not properly parsed.

* Passing a compound literal value in a function call with a VLA
  caused an internal error.

* Correct an internal error in `ControlFlowOptimize` which could
  happen for certain shapes of control flow graphs.

* Correct debugger interrupting a remote target.

* Multiple initialized static single char variables would not alwasys
  be properly initialized at startup when using ROM based code.

* The register value tracker could in some situation get the value
  wrong. This happened when a known smaller value, say word sized was
  already in a register and the register was reloaded with a wider
  constant with the same lower bit pattern. In such cases it could
  optimize the wider load away at optimization level 1 and above,
  potentially leaving garbage bits in the upper part.

### Version 3.6.10, September 2022

* Passing a structure of size 1 caused an internal error.

* Correct a problem where a known static variable was redeclared in a
  local scope could result in an internal error.

* Initializing a field in a union auto variables using a compound
  literal could result in an internal error.

* The `va_copy` macro could in some situations cause an internal
  error.

* Allow placement of a section to be part of more than one placement
  group. This way it can end up in either of two suitable memories.
  The previous way was too restrictive and would flag an error that
  was not really a problem.

* Moving a small sized `struct` of size not even dividable by four
  could move an extra byte or two.

* Moving small `struct` with only byte sized elements could result in
  misaligned read or write, as it could make use of word or long word
  moves and such `struct` is not word aligned.

* The linker could in some situations with placement groups place
  sections overlapping. This could happen in some situations when
  section had multiple memory alternatives.

### Version 3.6.8, August 2022

* Generating a compound initializer of a type consisting of nested
  arrays and `struct` with implicit initializer values could cause an
  internal error.

* Generated initializers could in some situation generate an internal
  error about duplicate label.

* The linker could produce and internal error on a failed pattern
  mismatch about `pattern Just name`.

* Sign extension could in some situations clobber another kept value.

* In some situations a byte sized variable could end up being
  allocated at the same stack offset as another variable.

* The optimizer could enter an infinite loop and never terminate. This
  happened when the code had an empty infinite loop.

### Version 3.6.6, July 2022

* `va_arg` on `struct` or `union` types could result in an internal
  error.

* Initializing a compound auto variable with a character array using a
  string literal caused an internal error.

* Initializers of an array inside a `struct` only worked when the array
  was last in the `struct`.

* Correct a problem with accessing memory mapped I/O as a struct that
  in some cases could cause an internal error.

### Version 3.6.4, June 2022

* Fixed a problem with assigning the return value of a function call to
  a variable that could happen in certain rare situations.

* Size calculation of a multi-dimensional array inside a `struct` was
  not done right.

* Library function `calloc()` now properly clears the allocated memory.

* Pushing a byte parameter in a function call directly from a memory
  location caused a misaligned access on 68000.

* Certain constants loaded could be done incorrectly, resulting in
  the wrong constant. This happened when a constant was built using
  `moveq` followed by either `rol` or `ror`.

* Incorrect code could result on code like `p->a = p->b + x` in some
  situations.

* Improve sanity checking in the linker, giving better errors on
  misconfigured section placement, e.g. mixing bits and no-bits
  without using placement groups.

* Directives `.space` and `.fillto` would incorrectly give an error
  that the expression needs to be solvable, even if the expression
  was actually solvable.

* Cross jump could in some rare situations terminate the compiler with
  a pattern mismatch.

### Version 3.6.2, April 2022

* Float negation was not correctly done and would result in incorrect
  result.

* Cross-call could lift out compiler generated internal calls in a
  way that violated stack offsets, causing incorrect results. This
  was most likely to happen in repetetive code with 64-bit floating
  point operations.

* Excessive use of placement groups could cause them to end up intermixed
  with memory overlaps, which would cause an error in some output formats
  or very corrupt run-time behavior.

* Improve sort order of memories and sections in the linker list file.

### Version 3.6.1, April 2022

* Add missing `_Stub_exit()` default routine which halts execution.

* The run-time library did not always place code in the `nearcode`
  section as it should.

* Section operators such as `.sectionStart` and `.sectionEnd` did not
  work on sections defined in placement groups and resulted in an
  error message from the linker.

* Section fragments with no-reorder were placed in reverse order if
  the section fragments were bound to a placement group.

* Placement groups could in some situations allocate sections so that
  they overlapped. This typically would happen for linker internal
  sections such as `data_init_table` and stack blocks.

* The Near data area was always copy initialized in a hosted system
  unless explicitly excepted from it. Now it obeys the `--hosted` and
  `--target` command option as it should.

* Minor correction in the cross jump optimizer which in some rare
  sitation could omit a jump back to a loop if a combined block only
  had a `jsr` or `bsr` instruction.

* Fixed a problem with a value switch inside a loop which could in
  some situations generate incorrect code.

* Reading source files with character encoding not matching the
  current locale settings could result a fatal error with 'invalid
  byte sequence' and also cause termination of the debugger. This will
  now generate an error, but not cause termination. To solve issues
  like this you need to set the `LC_TYPE` environment variable to
  match your source text files.

* Foenix math unit now uses internal base pointer variable `_Gavin`
  which is set up by the C startup to match the system in use, either
  a 16/24-bit 68000 or a 32-bit 68020/30/40 based system.

* The `%s` formatting rule did not work properly in reduced `printf()`.

* Fixed a problem with setting and clearing a single bit in memory
  would affect the wrong memory byte when operating on a 16 bits
  object.

* The continue statement in do-while loops incorrectly skipped over
  the loop condition.
