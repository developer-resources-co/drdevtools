//=============================================================================
// session.cpp: DAP request handlers for drmon-dap (cppdap).
//=============================================================================

#include "session.hpp"

#include "dap/session.h"
#include "dap/protocol.h"
#include "dap/io.h"

#include <cstdio>
#include <cstring>
#include <string>

DapSession::DapSession(std::string host, int port, const RegTable& regs)
    : backend_(std::move(host), port, regs)
    , session_(dap::Session::create())
{
    registerHandlers();
}

//=============================================================================
// Helpers
//=============================================================================

static std::string hexAddr(uint32_t addr) {
    char buf[12];
    snprintf(buf, sizeof(buf), "0x%x", addr);
    return buf;
}

static std::string base64Encode(const uint8_t* data, size_t len) {
    static const char kA[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    out.reserve((len + 2) / 3 * 4);
    for (size_t i = 0; i < len; i += 3) {
        uint32_t v = (uint32_t)data[i] << 16;
        if (i + 1 < len) v |= (uint32_t)data[i+1] << 8;
        if (i + 2 < len) v |= data[i+2];
        out += kA[(v >> 18) & 63];
        out += kA[(v >> 12) & 63];
        out += (i + 1 < len) ? kA[(v >>  6) & 63] : '=';
        out += (i + 2 < len) ? kA[(v      ) & 63] : '=';
    }
    return out;
}

//=============================================================================
// Handler registration
//=============================================================================

void DapSession::registerHandlers() {
    auto* s = session_.get();

    // --- initialize -----------------------------------------------------------
    s->registerHandler([&](const dap::InitializeRequest&) {
        dap::InitializeResponse resp;
        resp.supportsConfigurationDoneRequest    = true;
        resp.supportsReadMemoryRequest           = true;
        resp.supportsInstructionBreakpoints      = true;
        session_->send(dap::InitializedEvent{});
        return resp;
    });

    // --- attach ---------------------------------------------------------------
    // host/port come from command-line args passed to the DapSession constructor.
    s->registerHandler([&](const dap::AttachRequest&) -> dap::AttachResponse {
        backend_.disconnect();
        backend_.connect(); // best-effort; errors surface via dead-slave poll
        return dap::AttachResponse{};
    });

    // --- configurationDone ---------------------------------------------------
    s->registerHandler([&](const dap::ConfigurationDoneRequest&) {
        backend_.startPollThread([&](std::string reason, uint32_t /*pc*/) {
            dap::StoppedEvent ev;
            ev.reason   = (reason == "bp") ? "breakpoint" : reason;
            ev.threadId = 1;
            ev.allThreadsStopped = true;
            session_->send(ev);
        });
        return dap::ConfigurationDoneResponse{};
    });

    // --- setBreakpoints (source-level — not supported; return unverified) ----
    s->registerHandler([&](const dap::SetBreakpointsRequest& req)
                        -> dap::SetBreakpointsResponse {
        dap::SetBreakpointsResponse resp;
        if (!req.breakpoints.has_value()) return resp;
        for (const auto& sb : req.breakpoints.value()) {
            dap::Breakpoint b;
            b.verified = false;
            b.message  = "Source breakpoints not supported; use instruction breakpoints";
            b.line     = sb.line;
            resp.breakpoints.push_back(b);
        }
        return resp;
    });

    // --- setInstructionBreakpoints -------------------------------------------
    s->registerHandler([&](const dap::SetInstructionBreakpointsRequest& req)
                        -> dap::SetInstructionBreakpointsResponse {
        std::vector<uint32_t> desired;
        for (const auto& ib : req.breakpoints) {
            uint32_t addr = (uint32_t)strtoul(ib.instructionReference.c_str(), nullptr, 0);
            desired.push_back(addr);
        }
        {
            std::lock_guard<std::mutex> lk(bptMu_);
            auto delta = bptable_.sync(desired);
            for (uint32_t addr : delta.removed) backend_.clearBreakpoint(addr);
            for (uint32_t addr : delta.added)   backend_.setBreakpoint(addr);
        }

        dap::SetInstructionBreakpointsResponse resp;
        {
            std::lock_guard<std::mutex> lk(bptMu_);
            for (const auto& bp : bptable_.all()) {
                dap::Breakpoint b;
                b.verified             = true;
                b.instructionReference = hexAddr(bp.addr);
                resp.breakpoints.push_back(b);
            }
        }
        return resp;
    });

    // --- continue -------------------------------------------------------------
    s->registerHandler([&](const dap::ContinueRequest&) {
        backend_.run();
        dap::ContinueResponse resp;
        resp.allThreadsContinued = true;
        return resp;
    });

    // --- pause ----------------------------------------------------------------
    s->registerHandler([&](const dap::PauseRequest&) {
        backend_.stop();
        return dap::PauseResponse{};
    });

    // --- next (step over — for bare metal, same as stepIn) -------------------
    s->registerHandler([&](const dap::NextRequest&) {
        backend_.step();
        dap::StoppedEvent ev;
        ev.reason = "step"; ev.threadId = 1; ev.allThreadsStopped = true;
        session_->send(ev);
        return dap::NextResponse{};
    });

    // --- stepIn ---------------------------------------------------------------
    s->registerHandler([&](const dap::StepInRequest&) {
        backend_.step();
        dap::StoppedEvent ev;
        ev.reason = "step"; ev.threadId = 1; ev.allThreadsStopped = true;
        session_->send(ev);
        return dap::StepInResponse{};
    });

    // --- threads --------------------------------------------------------------
    s->registerHandler([&](const dap::ThreadsRequest&) {
        dap::ThreadsResponse resp;
        dap::Thread t; t.id = 1; t.name = "maincpu";
        resp.threads.push_back(t);
        return resp;
    });

    // --- stackTrace -----------------------------------------------------------
    s->registerHandler([&](const dap::StackTraceRequest&) {
        dap::StackTraceResponse resp;
        MameRegs r   = backend_.getRegisters();
        uint32_t pcl = (uint32_t)r.v[backend_.regTable().pcIndex];

        dap::StackFrame f;
        f.id                          = 1;
        f.name                        = "maincpu";
        f.instructionPointerReference = hexAddr(pcl);
        f.line = 0; f.column = 0;
        resp.stackFrames.push_back(f);
        resp.totalFrames = 1;
        return resp;
    });

    // --- scopes ---------------------------------------------------------------
    s->registerHandler([&](const dap::ScopesRequest&) {
        dap::ScopesResponse resp;
        dap::Scope sc;
        sc.name               = "Registers";
        sc.variablesReference = 1;
        sc.expensive          = false;
        resp.scopes.push_back(sc);
        return resp;
    });

    // --- variables ------------------------------------------------------------
    s->registerHandler([&](const dap::VariablesRequest& req) {
        dap::VariablesResponse resp;
        if (req.variablesReference != 1) return resp;

        MameRegs r          = backend_.getRegisters();
        const RegTable& rt  = backend_.regTable();
        for (int i = 0; i < rt.count; i++) {
            dap::Variable v;
            v.name  = rt.names[i];
            char buf[20];
            snprintf(buf, sizeof(buf), "0x%lx", r.v[i]);
            v.value              = buf;
            v.variablesReference = 0;
            v.memoryReference    = hexAddr((uint32_t)r.v[i]);
            resp.variables.push_back(v);
        }
        return resp;
    });

    // --- readMemory -----------------------------------------------------------
    s->registerHandler([&](const dap::ReadMemoryRequest& req) {
        dap::ReadMemoryResponse resp;
        uint32_t addr  = (uint32_t)strtoul(req.memoryReference.c_str(), nullptr, 0);
        uint32_t count = (uint32_t)req.count;
        std::vector<uint8_t> buf(count, 0);
        backend_.readMemory(addr, buf.data(), count);
        resp.address = hexAddr(addr);
        resp.data    = base64Encode(buf.data(), count);
        return resp;
    });

    // --- evaluate -------------------------------------------------------------
    s->registerHandler([&](const dap::EvaluateRequest& req) {
        dap::EvaluateResponse resp;
        const std::string& expr = req.expression;
        uint32_t addr  = 0;
        bool     isAddr = false;
        if (expr.size() > 1 && expr[0] == '$') {
            addr = (uint32_t)strtoul(expr.c_str() + 1, nullptr, 16);
            isAddr = true;
        } else if (expr.size() > 2 && expr[0] == '0' && (expr[1] == 'x' || expr[1] == 'X')) {
            addr = (uint32_t)strtoul(expr.c_str(), nullptr, 0);
            isAddr = true;
        }
        if (isAddr) {
            uint8_t b = 0;
            backend_.readMemory(addr, &b, 1);
            char buf[16]; snprintf(buf, sizeof(buf), "0x%02x", b);
            resp.result = buf;
            resp.type   = "byte";
        } else {
            const RegTable& rt = backend_.regTable();
            MameRegs r         = backend_.getRegisters();
            for (int i = 0; i < rt.count; i++) {
                if (strcasecmp(expr.c_str(), rt.names[i]) == 0) {
                    char buf[20]; snprintf(buf, sizeof(buf), "0x%lx", r.v[i]);
                    resp.result = buf;
                    resp.type   = "register";
                    break;
                }
            }
            if (resp.result.empty())
                resp.result = "<unknown expression>";
        }
        return resp;
    });

    // --- disconnect / terminate -----------------------------------------------
    auto finish = [&]() {
        backend_.stopPollThread();
        backend_.disconnect();
        std::lock_guard<std::mutex> lk(doneMu_);
        done_ = true;
        doneCv_.notify_one();
    };
    s->registerHandler([finish](const dap::DisconnectRequest&) {
        finish();
        return dap::DisconnectResponse{};
    });
    s->registerHandler([finish](const dap::TerminateRequest&) {
        finish();
        return dap::TerminateResponse{};
    });
}

//=============================================================================
// run() — binds to stdio and blocks until IDE disconnects
//=============================================================================

void DapSession::run() {
    auto reader = dap::file(stdin,  false);
    auto writer = dap::file(stdout, false);
    session_->bind(reader, writer, [&]() {
        // stdin closed before DisconnectRequest arrived — signal done
        std::lock_guard<std::mutex> lk(doneMu_);
        done_ = true;
        doneCv_.notify_one();
    });

    std::unique_lock<std::mutex> lk(doneMu_);
    doneCv_.wait(lk, [&]{ return done_; });
}
