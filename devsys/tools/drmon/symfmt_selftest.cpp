//=============================================================================
// symfmt_selftest.cpp — standalone unit test for the shared ca65/WLA parsers.
//
// Proves parseCa65Dbg()/parseWlaSym() (the same code the TUI _symbolList /
// tableSld and the DAP SymbolTable consume) produce the expected labels and
// source-line mappings, and that each parser content-sniffs — rejecting the
// other format and garbage so the loader dispatch chain falls through cleanly.
//
// Built and run by `task test-symbols` (g++ + symfmt.cpp, no CMake target).
//=============================================================================

#include "symfmt.hpp"

#include <stdio.h>
#include <string.h>

static int failures = 0;

#define CHECK(cond, msg) do { \
    if (cond) { printf("PASS  %s\n", msg); } \
    else      { printf("FAIL  %s\n", msg); ++failures; } \
} while (0)

static void writeFile(const char* path, const char* text)
{
    FILE* f = fopen(path, "wb");
    fwrite(text, 1, strlen(text), f);
    fclose(f);
}

// Find a label's address by name (0 if absent).
static uint32_t labelAddr(const SymData& d, const char* name)
{
    for (size_t i = 0; i < d.labels.size(); ++i)
        if (d.labels[i].name == name) return d.labels[i].addr;
    return 0;
}

// Address mapped to file:line (0 if absent).
static uint32_t lineAddr(const SymData& d, const char* file, int line)
{
    for (size_t i = 0; i < d.lines.size(); ++i)
        if (d.lines[i].file == file && d.lines[i].line == line)
            return d.lines[i].addr;
    return 0;
}

// Minimal real-format fixtures (mirror test_symbols.py): label reset@0x808000;
// main.s line 10 -> 0x808000, line 20 -> 0x808010.
static const char* CA65 =
    "version\tmajor=2,minor=0\n"
    "file\tid=0,name=\"main.s\",size=100,mtime=0x0,mod=0\n"
    "seg\tid=0,name=\"CODE\",start=0x808000,size=0x0020,addrsize=far,type=ro\n"
    "span\tid=0,seg=0,start=0,size=16\n"
    "span\tid=1,seg=0,start=16,size=16\n"
    "line\tid=0,file=0,line=10,span=0\n"
    "line\tid=1,file=0,line=20,span=1\n"
    "sym\tid=0,name=\"reset\",addrsize=absolute,val=0x808000,seg=0,type=lab\n";

static const char* WLA =
    "; wla symbolic information for test\n"
    "\n"
    "[labels]\n"
    "80:8000 reset\n"
    "\n"
    "[source files v2]\n"
    "0001:0001 00000000 main.s\n"
    "\n"
    "[addr-to-line mapping v2]\n"
    "00000000 80:0000 8000 0001:0001:0000000a\n"
    "00000010 80:0010 8010 0001:0001:00000014\n";

int main()
{
    const char* ca65Path = "/tmp/symfmt_selftest.dbg";
    const char* wlaPath  = "/tmp/symfmt_selftest.sym";
    const char* junkPath = "/tmp/symfmt_selftest.junk";
    writeFile(ca65Path, CA65);
    writeFile(wlaPath, WLA);
    writeFile(junkPath, "this is not a symbol file\n");

    // --- ca65 ---
    SymData ca;
    CHECK(parseCa65Dbg(ca65Path, ca), "ca65: parses");
    CHECK(labelAddr(ca, "reset") == 0x808000, "ca65: label reset == 0x808000");
    CHECK(lineAddr(ca, "main.s", 10) == 0x808000, "ca65: main.s:10 == 0x808000");
    CHECK(lineAddr(ca, "main.s", 20) == 0x808010, "ca65: main.s:20 == 0x808010");

    // --- WLA ---
    SymData wl;
    CHECK(parseWlaSym(wlaPath, wl), "wla: parses");
    CHECK(labelAddr(wl, "reset") == 0x808000, "wla: label reset == 0x808000");
    CHECK(lineAddr(wl, "main.s", 10) == 0x808000, "wla: main.s:10 == 0x808000");
    CHECK(lineAddr(wl, "main.s", 20) == 0x808010, "wla: main.s:20 == 0x808010");

    // --- content-sniff / fall-through ---
    SymData x;
    CHECK(!parseCa65Dbg(wlaPath, x),  "sniff: ca65 parser rejects a WLA file");
    CHECK(!parseWlaSym(ca65Path, x),  "sniff: WLA parser rejects a ca65 file");
    CHECK(!parseCa65Dbg(junkPath, x), "sniff: ca65 parser rejects garbage");
    CHECK(!parseWlaSym(junkPath, x),  "sniff: WLA parser rejects garbage");

    if (failures) { printf("\n%d symfmt self-test failure(s)\n", failures); return 1; }
    printf("\nAll symfmt self-tests passed.\n");
    return 0;
}
