# Calypsi 6502 release notes

Version 5.17, May 2026.

## Highlights

* ISO C 99 compiler for the MOS 6502 and WDC 65C02. This is a
  freestanding implementation with many features you will typically find in a
  hosted compiler.

* Fully re-entrant code model.

* Support for all integer types up to 64 bits `long long`.

* Floating point supported (32 and 64 bits IEEE-754).

* Full support for struct, union, typedef and what you expect to
  find in C.

* The C stack is located outside the zero page, which means it can be
  as large as available memory (in the full 64K address range).

* Support for (stack allocated) variable sized arrays.

* Run-time model based on using pseudo registers in the zero page.
  The register file is 48 bytes.

* Parameter passing uses A and pseudo zero page registers (up to 8
  bytes). Any remaining argument is passed on the stack.

* The compiler utilizes zero page as much as possible for local and
  temporary variables.

* Values of type `long long` are passed by reference rather than by value.
  The run-time keeps track of values and will create
  temporaries and copy data as needed for correct handling. The result
  is quite efficient code for passing around `long long` values,
  but operations will of course suffer a bit on a small 8-bit target
  like the 6502.

* Optimizing compiler that can output source level debugging
  information.

* Source code debugger included.

* Support for ELF/DWARF, hex output as well as raw and `.prg`
  (Commodore) style application files.

## Known problems

* The C parser has a known problem in that the keyword form for
  attributes, e.g. `__zpage` may give unexpected
   errors in some situations. If that happens, use the
   `__attribute__((zpage))` form instead.

* Statement expressions are always executed, even inside `if` statement
  expressions with `&&` or `||` operators arranged so that the
  execution should be conditional.

* Complex data type is not supported.

* The debugger documentation is lacking at the moment.

## News

### Version 5.17

* Add command line option `string-literals=shifted-petscii` to
  translate letters suitable for shifted PETSCII output.

* Commodore 64 and MEGA65 now uses CR as end of line for text files.

### Version 5.15

* Improvements to the low level use resource analysis, making it more
  precise and able to remove some more dead instructions.

* Add compiler command line option `--align-functions`.

### Version 5.13

* The `target` runtime attribute is now explicitly set to
  `none-specified` if not used. This is to prevent mixing modules and
  ensure build integrity.

* Add `__CALYPSI_DEBUG__` macro that is defined and set to 1 if
  compiling with debug information is enabled.

### Version 5.11

* Target Commander X16 is no longer supported.

* Target Foenix F256 is no longer supported.

* The banked code model is no longer supported.

* Some changes in board support naming convention, but this should be
  fully transparent if you link with `--target=c64` or
  `--target=mega65`.

### Version 5.8

* Add a mechanism to restore zero page pseudo registers to allow safe
  return to Commodore 64 and MEGA65 BASIC. To enable it, place the
  following line somewhere in your application
  `#pragma require __preserve_zp`. Refer to the *Target specifics*
  chapter in the user guide for more information.

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

* Improve use of `(dd),y` for addressing inside small arrays. This
  requires `-O2` and that the compiler knows the size of the array,
  which typically meaning that the array needs to be inside a
  `struct`.

* MEGA65: The symbols `HTRAP00` to `HTRAP3F` are now recognized as
  having arbitrary register effects and setting the carry flag.
  This prevents instructions from being optimized away when doing a
  Hyppo call from inline assembly. You will need to import the
  symbol used, e.g. `" .extern HTRAP00"\\n`.
  The `HTRAP` symbols are defined in the supplied MEGA65 library which
  is automatically linked with when using `--target=mega65`.

* Update Commodore board support for some fixes related to file I/O.

* C library stub routines are now expected to return a negative errno
  value when there is a problem and should not set the `errno`
  variable. The existing board support routines and semi-hosting have
  been updated accordingly.

### Version 5.6

* Improve code generation for various shift and mask patterns.

* Bitfield assignment operations now performs many operations in place
  without the normal unpacking and normalization, resulting in better
  code.

### Version 5.5

* Scatter to sections are no longer automatically aligned, as it
  could cause placement issues when the block was not placed on
  an alignment based on its size.

* Add command line option `--no-interprocedural-cross-jump` to
  disable jumps between functions. One situation when you may need
  this is when you are doing your own manual bank system and functions
  from the same compilation unit may not always be mapped in at the
  same time.

* Add relocation operator `.swap` which swaps the bytes of a 16 bit
  operand. This can be used on the MEGA65 to swap the bytes for the
  `PHW` immediate instruction to match the byte order used by `JSR`
  for the return address.

### Version 5.4

* Add library build scripts to assist in rebuilding the C library with
  different settings. See the `library-build` folder in the
  installation directory.

### Version 5.3

* The linker is now less aggressive on merging memories in the
  PRG and RAW formats. Memories are only merged in the address
  range `0x0000`-`0xffff` and can be prevented completely
  by specifying the `--no-merge-raw-memories` command line option.

* Add new command option `--inline-on-matching-custom-text-section`
  which prevents inlining of functions placed in a custom section,
  unless the function being inlined into belongs to the same custom
  section. This is useful if the memory system is being manipulated
  and the function much execute in that specific custom
  section.

* `#pragma clang section rodata="..."` now also affects the section
  used for string literals and compiler generated static
  initializers.

* Add missing 65cnr02 libraries for Commander X16 targets.

* Provide default bank slots to the linker to prevent error
  message about `codeSlot` runtime attribute mismatch.

### Version 5.2

* Add support for linking banked systems with the `scatter-to` memory
  property.

* No longer gives overlapping memory errors in the PRG and RAW output
  formats. This is useful for banked and custom overlay systems.

* Add 65CNR02 core. This is the original 65C02 without Rockwell
  extensions. It can also be used to build code intended to work on
  both the 65C02 and the 65816 in emulation mode.

* Add `#pragma rtattr`.

* Add `--initialize-large-data` command line option to the
  linker. This is used to enable initializing object larger than 64K
  and objects that cross 64K banks, even when the Huge attribute is
  not active. This is mainly intended for the MEGA65 target.

* Applications now downloaded 2-3 times faster when using the remote
  debugger. This is because the gdbserver `X` (binary write memory)
  command is used.

* Rename code and data slot options, from `--code-bank-block` to
  `--code-slot`, and `--data-bank-block` to `--data-slot`.
  This is done to follow established naming.

* Rename MMU relocation operators to use "slot" rather than "block",
  e.g. `.mmuslot5` replaces `.mmublock5`.

* Cross call and interprocedure cross jumps are disabled in the banked
  code model as a temporary measure, as it causes problems linking due
  to functions being placed in different banks.

* Make the assembler directive `.incbin` load files relative to the
  current source directory. If that file path does not exist, it will
  try as before, based on the given path, relative from the
  compilation directory.

* The default C stack size is now 4096 (was 256).

### Version 5.1

* Inline assembly is now supported. Refer to the Calypsi C compiler
  guide for more details.

* Compiler will now use hex numbers in generated assembly
  code. Smaller numbers and certain larger even numbers may still be
  shown in decimal.

* Include board support packages for Commodore 64, MEGA65 and
  Commander X16.

* Formatter variant for `printf()` and `scanf()` are now automatically
  selected based by the linker based on the formatter strings used in
  the application. There is no longer a need to specify the formatter
  on the linker command line, but it can still be done that way if you
  for some reason want to override the selection mechanism.

* 45GS02: Add missing conditional long branch instructions. These use
  mnemonics with an `L`, e.g. `LBCC` for a long branch on carry
  clear. This is to distinguish them from the normal short form. This
  is not according the instruction manual (which shows the same name),
  but makes life simpler here. (The idea of prefixing with `L` come
  from 6809.)

* Add intrinsic function `__no_operation()` to emit a `NOP`
  instruction.

* Add intrinsic function `__break_with(n)` to emit a `BRK n`
  instruction.

* The built-in board support packages now specify their own default
  `cstartup` attribute which eliminates the need to specify it on
  linker command line.

* Add `--cstartup=VALUE` linker option to make it easier to specify a
  custom C startup module. This was previously done using the
  `--rtattr cstartup=VALUE` option (which still works).

* The code generator now makes use of the MEGA65 math unit for integer
  multiply and divide.

### Version 4.4

* Board support for Foenix F256K and F256 Jr. are now provided.

* Stub routines `_Stub_access()`, `_Stub_raise()` and `_Stub_fcntl()`
  are obsolete and have been removed.

* Enable the `overloadable` attribute which allows for overloading
  functions similar to C++. Overloaded functions are subject to name
  mangling.

* Enable the `unused` attribute. This can be used to silence
  diagnostics about unused entities that cannot be removed.

### Version 4.3

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

* Add `.incbin` directive to allow for inserting binary data into
  assembler.

* The compiler can now generate assembly source output using the
  command line option `--assembly-source`.

* Support for the F256 Jr. The compiler gets new address space
  attributes `far`, `huge` and `vram` to allow access to data
  in the full 2Mb address space provided by the F256 jr.

* Support for 45GS02 in the MEGA65. This adds instruction support in
  the assembler, C compiler code generator and debugger simulator.

* Banked code model available for the F256 Jr. and the MEGA65.

* Add `__tinycore_call_failed()` intrinsic function meant to
  test the return of a TinyCore kernel call. The error status is
  returned in the carry flag and this intrinsic allows for testing it
  by wrapping the actual call.

* The assembler now allows labels to be indented with spaces, provided
  they are followed by spaces or be surrounded by back quotes.

### Version 3.6.1

* The S-record format can now also be specified using S19, S28 or S37.
  Doing so selects a specific address size format variant. The
  previous `S-record` format selected a variants that was suitable
  based on target.

* Semi-hosted debugger stubs are now fully implemented by the debugger.
  This makes it possible to simulate a file system on the target by
  using the file system on the host.

* Add compiler option `-M` for dependency generation control.

* Add assembler directives `.end`, `.global` and `.globl`.

### Version 3.5.1

* 64 bit floating point support based on Berkeley SoftFloat.

* Implement count leading zeroes builtin functions, __builtin_clz,
  __builtin_clzl and __builtin_clz.

* The vector address with the `interrupt` attribute is now optional.
  Omitting it means that no contribution to the vector section is
  generated.

* This release also contains a number of corrections to the compiler
  that could generated incorrect code or cause internal errors.

### Version 3.4.1

* New tutorial chapter in the user guide that shows how to
  download, build and run a simple project. This can be done either
  using the simulator or by using a Commodore 64.

* The optimizer can now inline functions. This can be further
  controlled using various command line options.

* Add pragma directive and attribute to control placement of functions,
  global and static data objects in custom sections.

* Add attribute to allow specifying the alignment of a given function,
  global or static data object.

* Add new intrinsic function `__break_instruction()` which emits a `BRK`
  instruction.

* Improvements to optimizer to make more cases of multiply with constant
  turn into shift operations and a subtract. In general, constants that
  can be expressed as `(1 << n) - (1 << m)` are now optimizer, e.g.
  7, 14, 15, 30, 60, 62, 63.

* Provide a more solid mechanism to replace the C-startup module which
  is responsible for the low level initialization. The default variant
  show work in most embedded systems scenarios, some use under a host
  operating system may need custom changes. Refer to the user guide
  for more information.

### Version 3.3.1

* Accessing elements in a small array can now  make better code use of
  addressing modes. This is done by looking at the actual size of the
  array being accessed rather than relying on pointer and index types
  in the access.

* Multiply by a constant with two bits set (three if `--speed` is
  enabled) is now done inline using a series of shifts and additions.
  This improves performance when multiplying with many common constants,
  e.g. multiply by `10 (8 + 2)` and `320 (256 + 64)`.

* New command line options `--space` and `--speed` to tune the optimizer
  are provided. Optimize for space is the default.

* The compiler can now install Commodore 64 friendly kernal interrupts
  using the `kernal_interrupt` attribute. Refer to the user guide for
  more information.

## Corrections

### Version 5.17, May 2026

* Fixed a problem in the low peephole optimizer that in some rare
  cases could remove a load instruction when it should not have.

### Version 5.16, April 2026

* Using a struct object as an initializer of an auto struct caused
  an internal error.

* In some very rare situations function calls with many parameters,
  preceded by a parameter expressions utilizing low level runtime
  support routine could cause an internal error.

### Version 5.15, January 2026

* Fixed a problem with extracting the 8-bit upper part from a 16-bit
  value in some rare situations could cause incorrect code.

### Version 5.14, November 2025

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

* Fix an internal error related to initializing a `char` array with a
  string literal surrounded by redundant curly braces.

* Fixed a register spill issue that in some rare situations could omit
  doing the spill operation.

### Version 5.11, August 2025

* Fixed some issues with combining memories in raw output format into
  a single file.

* Fix problem with Fedora installation in that installed libraries
  very severely stripped down and not working properly.

### Version 5.10, April 2025

* A bit-field of size 0 which fills out the current container,
  incorrectly caused an extra empty container to be allocated if the
  current one was not empty.

* The `overloadable` attribute caused a segmentation fault if the function
  type had address space qualifiers, e.g. `__zpage`.

* `_Static_assert` could cause an internal error.

* A function call returning a `struct` with at least two internal
  layers of `struct` or `union`, immediately accessing a single
  element on return from the function call would result in an
  internal error.

* Volatile address operands could in some situations cause an internal
  error.

* Casting a static data address to a function pointer could in some
  situations cause an internal error.

### Version 5.9, March 2025

* Parameter numbering in inline assembly now correctly starts the
  input list with `%0` when there is no output.

* Correct a code generator bug that could occur in some situations
  when a register variable was used twice in the same expression.

* Incorrect offset could be used for calculations being made of an
  argument to a function call. There were a series of conditions for
  this. 1) There were other byte sized arguments stored in the zero
  page area later in the argument list; 2) The expression calculation
  accessed a local variable on the simulated C stack; and 3) Typically
  the expression called a library routine such as divide and multiply.
  In such situation the access to the local variable on the C stack
  could be accessed at the wrong offset.

* MEGA65: Fixed an internal error with banked function pointers used as
  initializers.

### Version 5.8.1, February 2025

* Correct value in a compound literals casting the address of an
  external variable to an integer of fewer bits caused a linker range
  error.

* Switch statements with a single statement body (not a composite)
  caused an internal error.

* Correct spill handling which in some cases could result in an
  internal error, typically illegal instruction related to using the X
  register.

* Correct a problem in the peephole optimizer that for multiple
  successive spill/fill operations with push/pull in an extended block
  in some situations could rearrange the order of operations with the
  rest of the code.

### Version 5.7.2, January 2025

* Corrections made to the tree shaker to improve precision and ensure
  that local references made from the defined roots are properly
  made. Previously it could sometimes generate incorrect undefined
  symbol errors.

* Fix a problem with the question mark operator and void result
  causing an internal error.

* Root symbols inside archives (libraries) are currently ignored for
  the sake of it being an archive and not a core part of the
  application. In case you perform staged builds with libraries and
  need a root, e.g. an interrupt handler, you may need to specify it
  using the new `--root-symbol` option in the linker.

* Fixed a problem with incrementing a 32 bit register variable value
  for use in some operation, which in some situations could leave the
  register variable incorrectly increment afterwards.

### Version 5.7.1, December 2024

* Tree shaking could cause incorrect errors about undefined symbols
  when debug information was enabled.

* Reading a byte sized volatile and then writing it back to the same
  location was optimized away at optimization level 1 and above.

* Spilling a pseudo zero page register and then taking it back to
  a different location could in some situation result in incorrect
  register being used.

* Expressions that shifted the operand first right then left, or vice
  versa, could in some situations generate incorrect code when `-O2`
  was used.

### Version 5.7, November 2024

* Static functions that are no longer referenced as the result of
  inlining are now properly removed.

* Fix placement in linker to allow better for PRG format with its
  execution header at a fixed address and use of placement
  groups. Previously this could cause and error about using multiple
  memory areas in some situations, e.g. with switch tables.

* Some corrections in the C library related to I/O operations and
  returning error values from stub routines not being handled
  correctly.

* Fix a problem with 65c02 and 45gs02 in that the `SMBx`
  instruction in some situations could clobber a register
  variable.

* A function with only a single return statement having an expression
  with a non-auto variable could cause an internal error.

* Expressions `&*(char**)0x1234` caused an internal error.

### Version 5.6, September 2024

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

* Correct a problem combining bytes using
  `short x = char_expr_1 | (char_expr_2 << 8)` which could
  lead to that the two parts where temporarily stored in the
  same pseudo register location.

* 45gs02: The compiler tracks values in pseudo registers, but it
  failed to do it properly when the pseudo register was rewritten to
  use the absolute addressing mode rather than zero page, due to
  instruction not having zero page addressing. Examples are `ASW`,
  `INW` and `ORW`. This could lead to  incorrect code at the highest
  optimization level in some very rare situations.

* In some situations the compiler could generate illegal `LDZ`
  instructions causing an internal error.

* Handle MEGA65 `PHW` with immediate operand in inline assembly
  without throwing an internal error.

* Operator `/=` and `%=` are now rewritten to shift and mask
  operations respectively, when the operand is a constant that allows
  it.

* Using `.extern` directive in inline assembly caused an internal
  error when using `-O2`.

### Version 5.4, May 2024

* Inlining a function in a loop with a parameter that was read only
  once as part of being the parameter to the inlined function, would
  get clobbered if it was written to inside the inlined function.

* MEGA65: Correct use/def semantics of `ASW` instruction which was not
  done right and could cause writes to zero page pseudo registers
  prior to be incorrectly optimized away.

* Incorrect code could be generated in some rare situations with
  address expressions.

* MEGA65: Correct date table initializers for scattered sections,
  now also handling section operators.

* MEGA65: Correct routines that initialize variables, they expected a
  32-bit pointer, but the table address passed from C startup was
  16-bit. These are now both 16-bit.

* MEGA65: Data initializer table entries now use the scatter-to
  address if it exists, otherwise it falls back to the normal
  address. In practice this handles the situation when data is banked
  together with code, but needs to be initialized. The initialization
  of such data is done using its scatter-to address as the copy
  routine does not know how to set up the custom bank system. This
  is specific to the MEGA65 target which allows a custom bank handling
  as well as addressing the same data directly using 4 byte pointers.

* The linker now gives precedence to a replacement C library given on
  the command line.

### Version 5.3, March 2024

* Add missing 45GS02 instruction `RTS #n`.

* When adjacent PRG and RAW memories were merged the size calculation
  in the list file was off.

* Fix and internal error that could occur when doing `+=` on a
  huge pointer.

* Clobbered registers in inline assembly statements were not always
  obeyed in the register allocation.

### Version 5.2, February 2024

* Selecting simplified exit using `--rtattr exit=simplified` caused an
  error about clashed `exit` runtime attribute instead of selecting
  simplified `exit()`.

* Inline assembly with empty output combined with using named input
  arguments caused an internal error.

* 45GS02: Do not optimize away `EOM` and `CLV` instructions.

* 45GS02: Add missing definition of `_MapShadow` pseudo register which
  is used to keep track of the last value put into the `MAP`.

* Correct output file names for the PRG format when using
  `--raw-multiple-memories`. The main output file incorrectly changed
  name to be of the memory, rather than what is indicated on the
  command line. Extra output files using now gets the correct
  extension `.raw` (not `.prg`).

### Version 5.1, November 2023

* Compound literals could in some situations cause an internal error.

* Forward declared enumerations could cause a segfault.

* Casting a pointer to an integer in a compound literal could cause an
  internal error.

### Version 4.5, August 2023

* When using `--target=foenix` the Foenix SDK header files were not
  automatically added to the system include path as they should.

### Version 4.4, June 2023

* The `stderr` stream was not properly initialized.

* Correct `_Generic` to work properly. This is actually part of
  C11, but supported by the front-end in current (non-strict C99)
  default setting.

* Improvements to derived linker rule files.

* Correct `freopen()` which was not properly implemented.

* Correct `tmpnam()` and `tmpfile()`, also use the current directory
  for temporary files as there may not be and directory support in the
  file system used.

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

* A couple of library support routines `_Float64EQ`, `_LoadVspEA` and
  `_LoadVfpEA` could result in that the optimizer would keep a value
  over such call that got clobbered, this could happen in rare cases
  at optimization level `-O2`.

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

### Version 3.6.10, September 2022

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

* Single stepping an `RTI` instruction has been corrected.

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

* Loading a byte sized volatile variable could in some situations
  cause an internal error.

* The optimizer could enter an infinite loop and never terminate. This
  happened when the code had an empty infinite loop.

### Version 3.6.6, July 2022

* `va_arg` on `struct` or `union` types could result in an internal
  error.

* The code generator could in some situations generate a `BIT`
  immediate instruction which is not legal for 6502.

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

* Section operators such as `.sectionStart` and `.sectionEnd` did not
  work on sections defined in placement groups and resulted in an
  error message from the linker.

* Section fragments with no-reorder were placed in reverse order if
  the section fragments were bound to a placement group.

* Placement groups could in some situations allocate sections so that
  they overlapped. This typically would happen for linker internal
  sections such as `data_init_table` and stack blocks.

* Minor correction in the cross jump optimizer which in some rare
  situation could omit a jump back to a loop if a combined block only
  had a `jsr` instruction.

* Fixed a problem with a value switch inside a loop which could in
  some situations generate incorrect code.

* Reading source files with character encoding not matching the
  current locale settings could result a fatal error with 'invalid
  byte sequence' and also cause termination of the debugger. This will
  now generate an error, but not cause termination. To solve issues
  like this you need to set the `LC_TYPE` environment variable to
  match your source text files.

* The `%s` formatting rule did not work properly in reduced `printf()`.

* The continue statement in do-while loops incorrectly skipped over
  the loop condition.

### Version 3.5.1, January 2022

* Suppressing vector sections in the linker did not work properly and
  would cause an error about unplaced vector sections.

* Linker output files with multiple address ranges are now emitted in
  sorted order. This is not needed for all formats, but recommended
  for some, e.g. Motorola S-record.

### Version 3.4.3, December 2021

* Allow the linker scheme interpreter to `load` additional files that
  are relative to the loaded top level `.scm` file.

* Fix problem where omitted bit fields in a compound literal initializer
  could cause an internal error.

* The --stack-size and --cstack-size linker command line options  had
  the effect of adjusting the opposite stack, now corrected.

* Properly set the start address in hex output files, both for the
  Intel HEX and Motorola S-Record formats.

* Corrected a problem which prevented the __low_level_init symbol from
  being replaced by and external function. A local weak symbol was
  always kept if it was referred to from withing the same module.

* The loop counter was not properly updated in a for loops when
  executing a continue statement.

* The C library is now built with functions made weak to make it
  possible to replace standard functions with your own alternatives.

### Version 3.4.2, October 2021

* Inline of a function with a parameter that was also assigned too
  would create incorrect code (due to that it was not properly renamed
  for parameter use).

* Conditional expressions (?-mark operator) could in some situations
  perform a side-effect in either the true or false expression,
  regardless of the outcome of the test expression. Result values were
  correctly handled, but some potential side-effects that was part of
  them could happen when it was not supposed to.

* A conditional expression where the two alternatives returned a
  struct or union literal could cause an internal error.

* When initializing a compound literal of bit-fields the compiler could
  terminate with an internal error about typeSize not supported for an
  struct/union type.

* Initializing a field using a union value could cause an internal
  error in some situations. Also improved code generation when using a
  compound literal as the initializer value.

* Post-increment or post-decrement on a union value would cause an
  internal error.

* Returning a struct/union value through a conditional expression
  could cause an internal error.

* Shift operations with an shift could that was an expression could in
  some situations cause an internal error.

* Compile time folded floating point cast to integer type could in
  some cases cause an incorrect compile time constant to be
  generated.

* Fix the linker scheme interpreter so that it can access its standard
  library.

* Correct problem related to string literal data initializers that in
  some situations could cause an internal error.

* Place string literals in the cfar section rather than cnear in the
  Medium data model (which is how it was intended).

* Assignments to compound objects in loops or similar merging flows
  could cause bogus warnings about some ?temp variable being read
  before assigned and also result in incorrect code.

* The optimizer could loop forever.

* Semantics on inline has been corrected. `static inline` functions
  now generate a local copy of the function if not inlined.

* The function inliner was overly aggressive on doing inlining and has
  been tamed slightly in this update.

### Version 3.3.2, August 2021

* Correct conversions between `long long` and floating point numbers.

* Correct inline functions. A function that is marked as inline is only
  code generated as a separate function if an `extern` declaration of
  it seen in the compilation unit:

        static inline int dup (int x) {
          x * 2;
        }

        ...

        // This creates a globally visible version of the 'dup' function.
        extern inline int dup (int x);

* Initializing arrays using implicit zeros could generate inefficient
  code, example:

        void foo (int x) {
           int temp[20] = { x, x + 1, };
           ...
        }

* Reading a byte sized volatile operand and using the value could result
  in an internal error.

* Correct the C header files to provide better separation between
  the standard C library and POSIX. Previously some POSIX symbols
  and definitions leaked in when including `stdio.h`, `stdlib.h`,
  `time.h` or `wctype.h`.

* The POSIX library header file `sys/types.h` incorrectly defined
  symbols such as `TRUE`, `FALSE` and had its own definition of
  `NULL` which is also (and properly) provided by `stddef.h`.

* The linker would complain about debug `_Stub_xxx` function existed
  but was enabled, even when there were non-debug variant available.

* Allow specifying command line option `--rtattr` multiple times,

* Compound literals were not implemented and caused an internal
  error if used.

* Using enumeration constants that could not be represented by the 'int'
  type could lead to an internal error.

### Version 3.3.1, July 2021

* `--char-is-unsigned` is the default as before, though the online
  help   incorrectly stated that `char-is-signed` was the default.

* Fixed a couple of problems that could lead to internal error.

* Fixed a problem with spill position that in some rare situations
  could cause the instruction performing the spill could be placed at
  the wrong location. As a result the value was overwritten rather than
  preserved.

* Corrected a couple of problems that could lead to incorrect code being generated.

### Version 3.2.1

* Initial Beta release for 6502, April 2021.
