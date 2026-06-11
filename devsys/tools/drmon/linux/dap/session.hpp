#pragma once
#include <memory>
#include <mutex>
#include <condition_variable>
#include "backend.hpp"
#include "breakpoints.hpp"
#include "dap/session.h"

// Owns the cppdap Session, MameBackend, and BreakpointTable.
// Call run() to enter the blocking DAP message loop (returns on disconnect).
class DapSession {
public:
    DapSession(std::string host, int port, const RegTable& regs);
    void run();  // blocks until IDE disconnects

private:
    void registerHandlers();

    MameBackend     backend_;
    BreakpointTable bptable_;
    std::mutex      bptMu_;

    std::unique_ptr<dap::Session> session_;

    std::mutex              doneMu_;
    std::condition_variable doneCv_;
    bool                    done_ = false;
};
