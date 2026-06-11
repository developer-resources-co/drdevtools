#include "breakpoints.hpp"
#include <algorithm>

BreakpointTable::Delta BreakpointTable::sync(const std::vector<uint32_t>& desired) {
    Delta d;
    // Find removed (in live set but not in desired)
    for (const auto& b : bps_) {
        if (std::find(desired.begin(), desired.end(), b.addr) == desired.end())
            d.removed.push_back(b.addr);
    }
    // Find added (in desired but not in live set)
    for (uint32_t addr : desired) {
        bool exists = false;
        for (const auto& b : bps_) if (b.addr == addr) { exists = true; break; }
        if (!exists) d.added.push_back(addr);
    }
    // Apply to live set
    bps_.erase(std::remove_if(bps_.begin(), bps_.end(), [&](const Breakpoint& b) {
        return std::find(d.removed.begin(), d.removed.end(), b.addr) != d.removed.end();
    }), bps_.end());
    for (uint32_t addr : d.added)
        bps_.push_back({addr});
    return d;
}

void BreakpointTable::clear() {
    bps_.clear();
}
