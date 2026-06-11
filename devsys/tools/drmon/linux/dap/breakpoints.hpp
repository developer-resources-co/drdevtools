#pragma once
#include <vector>
#include <cstdint>

// Tracks breakpoints currently set on the MAME bridge.
// Call sync() to reconcile the live set against a new desired set.
struct Breakpoint {
    uint32_t addr;
};

class BreakpointTable {
public:
    // Returns addresses added and removed vs the previous set.
    struct Delta {
        std::vector<uint32_t> added;
        std::vector<uint32_t> removed;
    };
    Delta sync(const std::vector<uint32_t>& desired);
    void  clear();
    const std::vector<Breakpoint>& all() const { return bps_; }

private:
    std::vector<Breakpoint> bps_;
};
