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
#include <vector>

// Provided by the per-system disassembler wrapper (dis816_dap.cpp / dis68000_dap.cpp)
extern int procMode;
unsigned int Disassem(unsigned long addr, char* inBuff, char* outBuff, int disMode);

DapSession::DapSession(std::string host, int port, const RegTable& regs,
                       const char* symbolPath)
    : backend_(std::move(host), port, regs)
    , session_(dap::Session::create())
{
    if (symbolPath && *symbolPath)
        symtab_.loadSld(symbolPath)  || symtab_.loadCoff(symbolPath) ||
        symtab_.loadCa65Dbg(symbolPath) || symtab_.loadWlaSym(symbolPath);
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
        resp.supportsDisassembleRequest          = true;
        resp.supportsLoadedSourcesRequest        = true;
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

    // --- setBreakpoints (source-level) ----------------------------------------
    s->registerHandler([&](const dap::SetBreakpointsRequest& req)
                        -> dap::SetBreakpointsResponse {
        dap::SetBreakpointsResponse resp;
        if (!req.breakpoints.has_value()) return resp;

        // Remove previously-set source breakpoints (VS Code sends full replacement list)
        {
            std::lock_guard<std::mutex> lk(bptMu_);
            for (uint32_t addr : srcBptAddrs_) {
                backend_.clearBreakpoint(addr);
                bptable_.remove(addr);
            }
            srcBptAddrs_.clear();
        }

        std::string file;
        if (req.source.path.has_value())       file = req.source.path.value();
        else if (req.source.name.has_value())  file = req.source.name.value();

        for (const auto& sb : req.breakpoints.value()) {
            dap::Breakpoint b;
            b.line = sb.line;
            uint32_t addr = symtab_.addrForSrc(file, sb.line);
            if (addr) {
                backend_.setBreakpoint(addr);
                std::lock_guard<std::mutex> lk(bptMu_);
                bptable_.add(addr);
                srcBptAddrs_.push_back(addr);
                b.verified             = true;
                b.instructionReference = hexAddr(addr);
            } else {
                b.verified = false;
                b.message  = symtab_.empty()
                    ? "No symbol file loaded (use --symbols)"
                    : "Source line not found in symbol file";
            }
            resp.breakpoints.push_back(b);
        }
        return resp;
    });

    // --- loadedSources --------------------------------------------------------
    s->registerHandler([&](const dap::LoadedSourcesRequest&)
                        -> dap::LoadedSourcesResponse {
        dap::LoadedSourcesResponse resp;
        for (const auto& f : symtab_.sourceFiles()) {
            dap::Source src;
            src.name = f;
            src.path = f;
            resp.sources.push_back(src);
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
            if (resp.result.empty() && !symtab_.empty()) {
                const Symbol* sym = symtab_.findByName(expr);
                if (sym) {
                    char buf[20];
                    snprintf(buf, sizeof(buf), "0x%x", sym->addr);
                    resp.result          = buf;
                    resp.type            = "symbol";
                    resp.memoryReference = hexAddr(sym->addr);
                }
            }
            if (resp.result.empty())
                resp.result = "<unknown expression>";
        }
        return resp;
    });

    // --- disassemble ----------------------------------------------------------
    s->registerHandler([&](const dap::DisassembleRequest& req)
                        -> dap::DisassembleResponse {
        dap::DisassembleResponse resp;
        uint32_t base  = (uint32_t)strtoul(req.memoryReference.c_str(), nullptr, 0);
        int64_t  ioff  = req.instructionOffset.has_value()
                             ? (int64_t)req.instructionOffset.value() : 0;
        int      count = (int)req.instructionCount;

        // instructionOffset treated as byte offset (Tier 2 approximation).
        uint32_t addr = (uint32_t)((int64_t)base + ioff);

        uint32_t buflen = (uint32_t)(count * 4 + 16);
        std::vector<uint8_t> mem(buflen, 0);
        backend_.readMemory(addr, mem.data(), buflen);

        // Derive procMode from live FLAGS (SNES: M/X bits → immediate operand size)
        const RegTable& rt = backend_.regTable();
        if (rt.flagsIndex >= 0) {
            MameRegs r     = backend_.getRegisters();
            uint32_t flags = (uint32_t)r.v[rt.flagsIndex];
            int m = (flags >> 5) & 1;   // M=1 → 8-bit accumulator immediate
            int x = (flags >> 4) & 1;   // X=1 → 8-bit index immediate
            procMode = (m == 0 ? 1 : 0) | (x == 0 ? 2 : 0);
        }

        uint32_t cur = addr;
        size_t   idx = 0;
        while ((int)resp.instructions.size() < count && idx + 1 <= mem.size()) {
            char obuf[128] = {};
            unsigned int len = Disassem(cur, (char*)&mem[idx], obuf, 0);
            if (len == 0) len = 1;

            dap::DisassembledInstruction di;
            di.address     = hexAddr(cur);
            di.instruction = obuf;
            if (!symtab_.empty()) {
                const Symbol* sym = symtab_.findByAddr(cur);
                if (sym && sym->addr == cur)
                    di.symbol = sym->name;
            }
            char hbuf[32] = {}; char* h = hbuf;
            for (unsigned i = 0; i < len && idx + i < mem.size(); i++)
                h += sprintf(h, "%02X ", mem[idx + i]);
            if (h > hbuf) *(h - 1) = 0;  // trim trailing space
            di.instructionBytes = hbuf;
            resp.instructions.push_back(di);

            cur += len;
            idx += len;
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
