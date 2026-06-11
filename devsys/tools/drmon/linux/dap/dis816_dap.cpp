// Wrapper: inject the DAP stub preamble before dis816.cpp's own includes.
// This satisfies memory.hpp's _object* declaration and general.hpp's
// _symbolList references without pulling in moninc.hpp / ncurses.
#include "disasm_preamble.hpp"
#include "../../dis816.cpp"
