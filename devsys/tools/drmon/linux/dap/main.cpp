//=============================================================================
// main.cpp: drmon-dap entry point.
//
// Usage:
//   drmon-dap-snes [--host <host>] [--port <port>] [--symbols <path>]
//   drmon-dap-gen  [--host <host>] [--port <port>] [--symbols <path>]
//
// Communicates with the IDE over stdin/stdout (DAP stdio transport).
// Connects to MAME via TCP on the specified host:port (default 127.0.0.1:41816).
// --symbols loads a symbol file for name resolution and source-level
// breakpoints. The format is auto-detected: binary .sld, Sierra COFF,
// cc65/ca65 .dbg, or WLA-DX .sym.
//=============================================================================

#include "session.hpp"
#include "registers.hpp"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>

// The DRMON_SYSTEM macro is set by CMake: -DDRMON_DAP_SNES or -DDRMON_DAP_GEN
#if defined(DRMON_DAP_SNES)
static const RegTable& kSystemRegs = kSnesRegs;
#elif defined(DRMON_DAP_GEN)
static const RegTable& kSystemRegs = kGenRegs;
#else
#error "Define DRMON_DAP_SNES or DRMON_DAP_GEN"
#endif

int main(int argc, char* argv[]) {
    std::string host = "127.0.0.1";
    int port = 41816;
    const char* symbolPath = nullptr;

    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "--host") == 0)    host = argv[i + 1];
        if (strcmp(argv[i], "--port") == 0)    port = atoi(argv[i + 1]);
        if (strcmp(argv[i], "--symbols") == 0) symbolPath = argv[i + 1];
    }

    DapSession session(host, port, kSystemRegs, symbolPath);
    session.run();
    return 0;
}
