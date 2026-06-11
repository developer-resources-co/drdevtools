#pragma once
#include <functional>
#include <string>
#include <mutex>
#include <thread>
#include <atomic>
#include <cstdint>
#include "registers.hpp"

struct MameRegs {
    unsigned long v[19]; // sized for max(SNES=11, GEN=19)
    int count;
};

// Clean MAME bridge TCP client for the DAP build.
// No dependency on any TUI header — speaks the bridge ASCII protocol directly.
class MameBackend {
public:
    MameBackend(std::string host, int port, const RegTable& regs);
    ~MameBackend();

    // Connect to bridge (handshake + REGS announcement).  Returns true on success.
    bool connect();
    void disconnect();
    bool isConnected() const;

    // Execution control
    void run();                      // C command
    void stop();                     // H command
    bool step();                     // S: step one instruction, correct for branches

    // Data
    MameRegs    getRegisters();
    bool        readMemory(uint32_t addr, uint8_t* buf, uint32_t len);
    bool        writeMemory(uint32_t addr, const uint8_t* buf, uint32_t len);

    // Breakpoints (B+/B-)
    bool setBreakpoint(uint32_t addr);
    bool clearBreakpoint(uint32_t addr);
    bool clearAllBreakpoints();

    // Poll: send ? → "running"/"stopped <pc> <reason>"/""
    // Returns (isRunning, pc, reason).
    struct PollResult { bool running; uint32_t pc; std::string reason; bool ok; };
    PollResult poll();

    // Poll thread: fires cb when execution transitions running→stopped.
    using StopCb = std::function<void(std::string reason, uint32_t pc)>;
    void startPollThread(StopCb cb);
    void stopPollThread();

    const RegTable& regTable() const { return reg_; }

private:
    // Low-level: send cmd (no newline needed), fill reply (null-terminated), return 0 ok/-1 err
    int cmd(const char* c, char* reply, int replySz);
    // Read exactly <len> bytes from bridge R command into buf. Returns true on success.
    bool bridgeRead(uint32_t addr, uint8_t* buf, uint32_t len);

    // SNES single-step helper: compute next instruction address(es).
    // Returns next fall-through PC and optionally a branch-target alt PC.
    struct StepTargets { uint32_t next; uint32_t alt; bool hasAlt; };
    StepTargets snesNextPC(uint32_t pcl, const MameRegs& r);

    std::string  host_;
    int          port_;
    const RegTable& reg_;
    int          fd_ = -1;
    mutable std::mutex sockMu_;

    std::atomic<bool> pollRun_{false};
    bool lastRunning_ = false;
    std::thread pollThread_;
    StopCb stopCb_;
};
