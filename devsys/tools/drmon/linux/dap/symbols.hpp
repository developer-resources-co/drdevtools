#pragma once
#include <cstdint>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>

struct Symbol  { uint32_t addr; std::string name; };
struct SrcLine { uint32_t addr; std::string file; int line; };

struct SymData;  // shared parser output (symfmt.hpp)

class SymbolTable {
public:
    bool loadSld(const char* path);      // Developer Resources binary .sld
    bool loadCoff(const char* path);     // Sierra COFF (magic 0x0150)
    bool loadCa65Dbg(const char* path);  // cc65/ca65 .dbg (text)
    bool loadWlaSym(const char* path);   // WLA-DX .sym (text)
    bool loadElf(const char* path);      // ELF/DWARF (llvm-mos -g output) via libdwarf

    void add(uint32_t addr, const std::string& name);
    void addSrc(const SrcLine& sl);

    // Exact-or-nearest-preceding symbol (within 256 bytes); nullptr if none.
    const Symbol* findByAddr(uint32_t addr) const;
    // Case-insensitive name lookup.
    const Symbol* findByName(const std::string& name) const;
    // Exact source file:line → address; returns 0 if not found.
    uint32_t addrForSrc(const std::string& file, int line) const;
    // Reverse: address → nearest-preceding source file:line (for stackTrace source
    // mapping so an editor can highlight the stopped line). false if none within range.
    bool srcForAddr(uint32_t addr, std::string& file, int& line) const;
    // All source files with at least one line record.
    std::vector<std::string> sourceFiles() const;

    bool empty() const { return byAddr_.empty(); }

private:
    void importSymData(const SymData& d);  // labels + lines from a parsed file
    // Recursive DIE-tree walk for loadElf (records subprogram entry points).
    // void* params keep the libdwarf types out of this header.
    void walkDwarfDies(void* dbg, void* die);

    std::map<uint32_t, Symbol>                byAddr_;  // sorted; lower_bound lookup
    std::unordered_map<std::string, uint32_t> byName_;  // lowercase name → addr
    // file → sorted vector of (line, addr) pairs
    std::map<std::string, std::vector<std::pair<int, uint32_t>>> srcMap_;
};
