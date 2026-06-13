//=============================================================================
// symfmt.hpp — modern homebrew symbol-file parsers (ca65 .dbg, WLA-DX .sym).
//
// Dependency-free: depends only on the C++ standard library, no TUI/DAP/global
// headers. Shared by the legacy TUI (gnu++98) and the DAP front end (C++17), so
// it is deliberately written in the gnu++98 common subset: <stdint.h> (not
// <cstdint>), no C++11 features, file-scope helper types only.
//
// Each parser content-sniffs and returns false (leaving any partial output) if
// the file is not its format, so callers can chain them like the existing
// loadSld() || loadCoff() dispatch.
//=============================================================================

#ifndef DRMON_SYMFMT_HPP
#define DRMON_SYMFMT_HPP

#include <stdint.h>
#include <string>
#include <vector>

// A name->address label (e.g. ca65 `sym ... type=lab`, WLA `[labels]`).
struct SymLabel { uint32_t addr; std::string name; };

// A source file:line -> address mapping (for source-level breakpoints).
struct SymLine  { uint32_t addr; std::string file; int line; };

struct SymData
{
    std::vector<SymLabel> labels;
    std::vector<SymLine>  lines;
};

// cc65/ca65 debug file (`ld65 --dbgfile foo.dbg`). Returns false if not ca65.
bool parseCa65Dbg(const char* path, SymData& out);

// WLA-DX symbol file (`wlalink -S [-A]`). Returns false if not a WLA .sym.
bool parseWlaSym(const char* path, SymData& out);

#endif // DRMON_SYMFMT_HPP
