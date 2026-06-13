# Calypsi Nut release notes

Version 5.17, May 2026

# News

## Version 5.16, April 2026

* The `.name` directive now accepts prompt numbers to make it
  easy to define prompting MCODE functions, e.g. `.name ASTO,2,1`.

* In the register display the `f` register has been renamed `t`, as
  this is actually the "tone" or "beep" register.

* Add a new `fi` register for peripheral flags.

## Version 5.15, January 2026

## Version 5.14, November 2025

### Version 5.10, April 2025

## Version 5.8, March 2025

## Version 5.8, February 2025

## Version 5.7, November 2024

* Add local labels based on `---` and `++++` sequences to the
  assembler. References to labels with minus characters goes backwards
  to closest label matching and references to labels with plus
  characters goes forward to the closest matching label.

## Version 5.3, March 2024

## Version 5.2, February 2024

## Version 5.1, November 2023

## Version 4.2, April 2023

## Version 4.1, April 2023

## Version 3.7.0, September 2022

* Add named local labels. Any identifier (not just a number) followed
  by a `$` is now seen as a local label. This allows for more readable
  names of local labels.

* Add `.argdelim` directive to allow macro arguments to contain comma
  characters.

## Version 3.6.4, June 2022

* Add assembler directives `.end`, `.global` and `.globl`.

## Version 3.5.1, January 2022

* Windows support added.

## Version 3.3.1, July 2021

Tool chain renamed to Calypsi as there are more targets available
now in addition to the HP-41 Nut.

## Version 3.2.1

Maintenance release, April 2021.

### Highlights

* The assembler and the RPN compiler uses an updated preprocessor
  (Clang 10). This may cause some new diagnostic messages on existing
  code inside assembly style comments (semi-colon), in particular a
  single tick (') results in a warning about an unterminated character
  literal. Either use C style commets (`//`) or alter the assembly
  comment to avoid it.

* Add `--no-secondaries` command line option to `modtool` to make it
  possible to work around problems with some modules that put upper
  bits in the ID area a bit wildly.
  At least the Advantage module has this problem.

* The debugger now supports `commands` to attach a command list to a
  breakpoint. This is inteded for breakpoint automation, e.g. to print
  something and continue, enable another breakpoint or just about
  anything.

* Auto-display expressions are now supported. These are useful for
  having a set of expressions values shown every time the program
  stops.

* Commands to control output during `commands` and `step-commands` are
  provided. The commands are `output`, `echo` and ``silent``.

* The `step-commands` which works similar to `commands`, but is
  executed on every single step. Together with output control commands
  you make up custom output. It can also be used to display all CPU
  registers after every single step using the `info registers`
  command, which is a lot simpler than using auto-display expressions
  for individual registers (when you want to see many registers).

### Corrections

## Version 4.2, April 2023

* Source file paths with multiple successive backslashes caused an
  internal error on Windows.

## Version 3.6.4, June 2022

* Improve sanity checking in the linker, giving better errors on
  misconfigured section placement, e.g. mixing bits and no-bits
  without using placement groups.

* Directives `.space` and `.fillto` would incorrectly give an error
  that the expression needs to be solvable, even if the expression
  was actually solvable.

## Version 3.6.2, April 2022

* Improve sort order of memories and sections in the linker list file.

## Version 3.2.1

* The address of `FLSHAP` was incorrect in `mainframe_cx.h`, now corrected.

* Minor change to module file loader to obey the lower and upper
  property and keep them in a port part (8-9, 10-11, 12-13 or 14-15).
  This problem affected dbnut, modtool and clonix tools.

* Debugger expressions can now properly parse back-tick quoted
  symbols.

## Version 3.1.1

Initial release, June, 2020.
