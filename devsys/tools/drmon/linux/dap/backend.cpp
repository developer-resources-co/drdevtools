//=============================================================================
// backend.cpp: clean MAME bridge TCP client for drmon-dap.
// No TUI headers.  Speaks the same ASCII protocol as sliomame.cpp.
//=============================================================================

#include "backend.hpp"

#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <chrono>
#include <thread>
#include <algorithm>

//=============================================================================
// 65816 opcode size table (SNES)
// Encodes base size in bytes for each opcode.  M/X-dependent immediates are
// stored at base size 2; callers adjust using the FLAGS register bits.
//=============================================================================

static const uint8_t kSnesOpSize[256] = {
//       x0 x1 x2 x3  x4 x5 x6 x7  x8 x9 xA xB  xC xD xE xF
/* 0x */ 2, 2, 2, 2,  2, 2, 2, 2,  1, 2, 1, 1,  3, 3, 3, 4,
/* 1x */ 2, 2, 2, 2,  2, 2, 2, 2,  1, 3, 1, 1,  3, 3, 3, 4,
/* 2x */ 3, 2, 4, 2,  2, 2, 2, 2,  1, 2, 1, 1,  3, 3, 3, 4,
/* 3x */ 2, 2, 2, 2,  2, 2, 2, 2,  1, 3, 1, 1,  3, 3, 3, 4,
/* 4x */ 1, 2, 2, 2,  3, 2, 2, 2,  1, 2, 1, 1,  3, 3, 3, 4,
/* 5x */ 2, 2, 2, 2,  3, 2, 2, 2,  1, 3, 1, 1,  4, 3, 3, 4,
/* 6x */ 1, 2, 3, 2,  2, 2, 2, 2,  1, 2, 1, 1,  3, 3, 3, 4,
/* 7x */ 2, 2, 2, 2,  2, 2, 2, 2,  1, 3, 1, 1,  3, 3, 3, 4,
/* 8x */ 2, 2, 3, 2,  2, 2, 2, 2,  1, 2, 1, 1,  3, 3, 3, 4,
/* 9x */ 2, 2, 2, 2,  2, 2, 2, 2,  1, 3, 1, 1,  3, 3, 3, 4,
/* Ax */ 2, 2, 2, 2,  2, 2, 2, 2,  1, 2, 1, 1,  3, 3, 3, 4,
/* Bx */ 2, 2, 2, 2,  2, 2, 2, 2,  1, 3, 1, 1,  3, 3, 3, 4,
/* Cx */ 2, 2, 2, 2,  2, 2, 2, 2,  1, 2, 1, 1,  3, 3, 3, 4,
/* Dx */ 2, 2, 2, 2,  2, 2, 2, 2,  1, 3, 1, 1,  3, 3, 3, 4,
/* Ex */ 2, 2, 2, 2,  2, 2, 2, 2,  1, 2, 1, 1,  3, 3, 3, 4,
/* Fx */ 2, 2, 2, 2,  3, 2, 2, 2,  1, 3, 1, 1,  3, 3, 3, 4,
};

// Opcodes whose immediate operand is M-size-dependent (+1 byte if M=0)
static bool isMImm(uint8_t op) {
    return op==0x09||op==0x29||op==0x49||op==0x69||
           op==0x89||op==0xA9||op==0xC9||op==0xE9;
}
// Opcodes whose immediate operand is X-size-dependent (+1 byte if X=0)
static bool isXImm(uint8_t op) {
    return op==0xA0||op==0xA2||op==0xC0||op==0xE0;
}

static int snesInsnLen(uint8_t op, uint8_t p) {
    int m = (p >> 5) & 1;
    int x = (p >> 4) & 1;
    int n = kSnesOpSize[op];
    if (isMImm(op) && !m) n++;
    if (isXImm(op) && !x) n++;
    return n;
}

//=============================================================================
// Construction
//=============================================================================

MameBackend::MameBackend(std::string host, int port, const RegTable& regs)
    : host_(std::move(host)), port_(port), reg_(regs) {}

MameBackend::~MameBackend() {
    stopPollThread();
    disconnect();
}

//=============================================================================
// Low-level networking
//=============================================================================

// Send <cmd>\n, recv one reply line into reply[replySz].  Returns 0 ok / -1 err.
int MameBackend::cmd(const char* c, char* reply, int replySz) {
    if (fd_ < 0) return -1;

    char sbuf[512];
    int slen = snprintf(sbuf, sizeof(sbuf), "%s\n", c);
    if (slen <= 0 || slen >= (int)sizeof(sbuf)) return -1;

    if (send(fd_, sbuf, slen, MSG_NOSIGNAL) != slen) {
        close(fd_); fd_ = -1;
        return -1;
    }

    int rpos = 0;
    for (;;) {
        struct timeval tv = {2, 0};
        fd_set fds; FD_ZERO(&fds); FD_SET(fd_, &fds);
        int rc = select(fd_ + 1, &fds, nullptr, nullptr, &tv);
        if (rc <= 0) { close(fd_); fd_ = -1; return -1; }
        char ch;
        if (recv(fd_, &ch, 1, 0) <= 0) { close(fd_); fd_ = -1; return -1; }
        if (ch == '\n') break;
        if (reply && rpos < replySz - 1) reply[rpos] = ch;
        rpos++;
    }
    if (reply) reply[(rpos < replySz - 1) ? rpos : (replySz - 1)] = '\0';
    return 0;
}

bool MameBackend::connect() {
    std::lock_guard<std::mutex> lk(sockMu_);
    if (fd_ >= 0) return true;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return false;

    struct sockaddr_in sa{};
    sa.sin_family = AF_INET;
    sa.sin_port   = htons((uint16_t)port_);
    if (inet_pton(AF_INET, host_.c_str(), &sa.sin_addr) <= 0) {
        close(fd); return false;
    }

    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    int rc = ::connect(fd, (struct sockaddr*)&sa, sizeof(sa));
    if (rc < 0 && errno != EINPROGRESS) { close(fd); return false; }

    fd_set wfds; FD_ZERO(&wfds); FD_SET(fd, &wfds);
    struct timeval tv = {2, 0};
    if (select(fd + 1, nullptr, &wfds, nullptr, &tv) <= 0) { close(fd); return false; }
    int err = 0; socklen_t el = sizeof(err);
    getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &el);
    if (err) { close(fd); return false; }

    fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
    struct timeval rtv = {2, 0};
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &rtv, sizeof(rtv));
    fd_ = fd;

    // Handshake
    char reply[256];
    if (cmd("V", reply, sizeof(reply)) < 0) return false;
    if (strncmp(reply, "ok drmon-bridge", 15) != 0) {
        close(fd_); fd_ = -1; return false;
    }

    // Announce register layout (bridge uses this order for G responses)
    char regs_cmd[512];
    int pos = snprintf(regs_cmd, sizeof(regs_cmd), "REGS ");
    for (int i = 0; i < reg_.count; i++) {
        pos += snprintf(regs_cmd + pos, sizeof(regs_cmd) - pos,
                        "%s%s", reg_.names[i], i < reg_.count - 1 ? "," : "");
    }
    if (cmd(regs_cmd, reply, sizeof(reply)) < 0) return false;

    return true;
}

void MameBackend::disconnect() {
    std::lock_guard<std::mutex> lk(sockMu_);
    if (fd_ < 0) return;
    char reply[16];
    cmd("BYE", reply, sizeof(reply));
    close(fd_); fd_ = -1;
}

bool MameBackend::isConnected() const {
    std::lock_guard<std::mutex> lk(sockMu_);
    return fd_ >= 0;
}

//=============================================================================
// Execution control
//=============================================================================

void MameBackend::run() {
    std::lock_guard<std::mutex> lk(sockMu_);
    char reply[32];
    cmd("C", reply, sizeof(reply));
}

void MameBackend::stop() {
    std::lock_guard<std::mutex> lk(sockMu_);
    char reply[64];
    cmd("H", reply, sizeof(reply));
}

//=============================================================================
// Data
//=============================================================================

MameRegs MameBackend::getRegisters() {
    MameRegs r{};
    r.count = reg_.count;
    std::lock_guard<std::mutex> lk(sockMu_);
    char reply[512];
    if (cmd("G", reply, sizeof(reply)) < 0) return r;
    char* p = reply;
    for (int i = 0; i < reg_.count; i++) {
        while (*p == ' ') p++;
        if (!*p) break;
        char* end;
        unsigned long v = strtoul(p, &end, 16);
        r.v[i] = v & reg_.masks[i];
        p = end;
    }
    // For SNES: compute PCL = PB<<16 | PC
    if (reg_.pbIndex >= 0) {
        int pbIdx = reg_.pbIndex;
        int pcIdx = reg_.pcIndex;
        r.v[pcIdx] = (r.v[pbIdx] << 16) | (r.v[pcIdx] & 0xFFFF);
    }
    return r;
}

bool MameBackend::bridgeRead(uint32_t addr, uint8_t* buf, uint32_t len) {
    // R command: R <addr> <len> → hex bytes (no spaces)
    char cmd_buf[64];
    snprintf(cmd_buf, sizeof(cmd_buf), "R %x %x", addr, len);
    // Reply can be up to len*2 chars
    std::vector<char> reply(len * 2 + 4, '\0');
    if (cmd(cmd_buf, reply.data(), (int)reply.size()) < 0) return false;
    // Parse hex pairs
    const char* p = reply.data();
    for (uint32_t i = 0; i < len; i++, p += 2) {
        char b[3] = {p[0], p[1], '\0'};
        buf[i] = (uint8_t)strtoul(b, nullptr, 16);
    }
    return true;
}

bool MameBackend::readMemory(uint32_t addr, uint8_t* buf, uint32_t len) {
    const uint32_t CHUNK = 4096;
    uint32_t done = 0;
    while (done < len) {
        uint32_t n = std::min(CHUNK, len - done);
        std::lock_guard<std::mutex> lk(sockMu_);
        if (!bridgeRead(addr + done, buf + done, n)) {
            memset(buf + done, 0, len - done);
            return false;
        }
        done += n;
    }
    return true;
}

bool MameBackend::writeMemory(uint32_t addr, const uint8_t* buf, uint32_t len) {
    const uint32_t CHUNK = 256;
    uint32_t done = 0;
    while (done < len) {
        uint32_t n = std::min(CHUNK, len - done);
        char cmd_buf[8 + 256*2 + 4];
        int pos = snprintf(cmd_buf, sizeof(cmd_buf), "W %x ", addr + done);
        for (uint32_t i = 0; i < n; i++)
            pos += snprintf(cmd_buf + pos, (int)sizeof(cmd_buf) - pos,
                            "%02x", buf[done + i]);
        std::lock_guard<std::mutex> lk(sockMu_);
        char reply[16];
        if (cmd(cmd_buf, reply, sizeof(reply)) < 0) return false;
        done += n;
    }
    return true;
}

//=============================================================================
// Breakpoints
//=============================================================================

bool MameBackend::setBreakpoint(uint32_t addr) {
    std::lock_guard<std::mutex> lk(sockMu_);
    char c[32]; snprintf(c, sizeof(c), "B+ %x", addr);
    char reply[16];
    return cmd(c, reply, sizeof(reply)) == 0;
}

bool MameBackend::clearBreakpoint(uint32_t addr) {
    std::lock_guard<std::mutex> lk(sockMu_);
    char c[32]; snprintf(c, sizeof(c), "B- %x", addr);
    char reply[16];
    return cmd(c, reply, sizeof(reply)) == 0;
}

bool MameBackend::clearAllBreakpoints() {
    std::lock_guard<std::mutex> lk(sockMu_);
    char reply[16];
    return cmd("BC", reply, sizeof(reply)) == 0;
}

//=============================================================================
// Poll
//=============================================================================

MameBackend::PollResult MameBackend::poll() {
    std::lock_guard<std::mutex> lk(sockMu_);
    PollResult r{};
    if (fd_ < 0) { r.ok = false; return r; }
    char reply[128];
    if (cmd("?", reply, sizeof(reply)) < 0) { r.ok = false; return r; }
    r.ok = true;
    if (strncmp(reply, "running", 7) == 0) {
        r.running = true;
    } else if (strncmp(reply, "stopped", 7) == 0) {
        r.running = false;
        char reason[32] = "unknown";
        sscanf(reply, "stopped %x %31s", &r.pc, reason);
        r.reason = reason;
    } else {
        r.ok = false;
    }
    return r;
}

//=============================================================================
// Single step (SNES: decode instruction at PC to compute branch targets)
//=============================================================================

MameBackend::StepTargets MameBackend::snesNextPC(uint32_t pcl, const MameRegs& r) {
    StepTargets t{pcl + 1, 0, false};

    uint8_t xb[4] = {};
    {
        std::lock_guard<std::mutex> lk(sockMu_);
        bridgeRead(pcl, xb, 4);
    }

    // FLAGS register is at kSnesRegFlagsIdx
    uint8_t p = (uint8_t)(r.v[kSnesRegFlagsIdx] & 0xFF);
    int ilen  = snesInsnLen(xb[0], p);
    uint32_t bank = pcl & 0xFF0000u;
    uint32_t pc16 = pcl & 0xFFFFu;
    uint32_t next = bank | ((pc16 + (uint32_t)ilen) & 0xFFFFu);

    switch (xb[0]) {
    // Conditional branches: both fall-through and taken are candidates
    case 0x10: case 0x30: case 0x50: case 0x70:
    case 0x90: case 0xB0: case 0xD0: case 0xF0: {
        uint32_t taken = bank | ((pc16 + 2 + (uint32_t)(uint16_t)(int8_t)xb[1]) & 0xFFFFu);
        if (taken != next) { t.alt = taken; t.hasAlt = true; }
        break;
    }
    // BRA (unconditional 8-bit)
    case 0x80:
        next = bank | ((pc16 + 2 + (uint32_t)(uint16_t)(int8_t)xb[1]) & 0xFFFFu);
        break;
    // BRL (unconditional 16-bit)
    case 0x82: {
        int16_t off = (int16_t)(xb[1] | ((uint16_t)xb[2] << 8));
        next = bank | ((pc16 + 3 + (uint32_t)(int32_t)off) & 0xFFFFu);
        break;
    }
    // JMP abs (same bank)
    case 0x4C:
        next = bank | (xb[1] | ((uint32_t)xb[2] << 8));
        break;
    // JMP (abs) — must read target from memory; just use ilen fallthrough
    // JMP (abs,X), JSR abs, JSR (abs,X) — use ilen fallthrough (step into)
    // JSL long
    case 0x22:
        next = xb[1] | ((uint32_t)xb[2] << 8) | ((uint32_t)xb[3] << 16);
        break;
    // JML long
    case 0x5C:
        next = xb[1] | ((uint32_t)xb[2] << 8) | ((uint32_t)xb[3] << 16);
        break;
    default:
        next = bank | ((pc16 + (uint32_t)ilen) & 0xFFFFu);
        break;
    }

    t.next = next;
    return t;
}

bool MameBackend::step() {
    // Get registers first (without holding sockMu_ across the read + cmd)
    MameRegs r = getRegisters();
    uint32_t pcl = (uint32_t)r.v[reg_.pcIndex];

    StepTargets t{pcl + 1, 0, false};
    // SNES-specific step decode (for Genesis we just fall through to pcl+2)
    if (reg_.count == 11 && reg_.pbIndex >= 0) {
        t = snesNextPC(pcl, r);
    } else {
        // Genesis / 68000: all instructions ≥ 2 bytes, most 2 or 4
        // Minimal: just use pcl+2 as a placeholder (TODO: proper 68k size table)
        t.next = pcl + 2;
    }

    char cmd_buf[64];
    if (t.hasAlt)
        snprintf(cmd_buf, sizeof(cmd_buf), "S %x %x", t.next, t.alt);
    else
        snprintf(cmd_buf, sizeof(cmd_buf), "S %x", t.next);

    std::lock_guard<std::mutex> lk(sockMu_);
    char reply[64];
    return cmd(cmd_buf, reply, sizeof(reply)) == 0;
}

//=============================================================================
// Poll thread
//=============================================================================

void MameBackend::startPollThread(StopCb cb) {
    stopCb_     = std::move(cb);
    pollRun_    = true;
    lastRunning_ = true; // assume running at start
    pollThread_ = std::thread([this]() {
        while (pollRun_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            if (!pollRun_) break;
            auto r = poll();
            if (!r.ok) continue;
            bool wasRunning = lastRunning_;
            lastRunning_ = r.running;
            if (wasRunning && !r.running && stopCb_) {
                stopCb_(r.reason, r.pc);
            }
        }
    });
}

void MameBackend::stopPollThread() {
    pollRun_ = false;
    if (pollThread_.joinable())
        pollThread_.join();
}
