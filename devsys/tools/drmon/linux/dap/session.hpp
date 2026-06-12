#pragma once
#include <memory>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <cstdint>
#include "backend.hpp"
#include "breakpoints.hpp"
#include "symbols.hpp"
#include "dap/session.h"

// Owns the cppdap Session, MameBackend, BreakpointTable, and SymbolTable.
// Call run() to enter the blocking DAP message loop (returns on disconnect).
class DapSession {
public:
    DapSession(std::string host, int port, const RegTable& regs,
               const char* symbolPath = nullptr);
    void run();  // blocks until IDE disconnects

private:
    void registerHandlers();

    MameBackend     backend_;
    BreakpointTable bptable_;
    std::mutex      bptMu_;
    SymbolTable     symtab_;
    std::vector<uint32_t> srcBptAddrs_;  // source-breakpoint addrs (for replacement)

    std::unique_ptr<dap::Session> session_;

    std::mutex              doneMu_;
    std::condition_variable doneCv_;
    bool                    done_ = false;
};
