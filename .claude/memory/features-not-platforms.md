---
name: features-not-platforms
description: "Guard code by feature/capability flags, not platform macros; the platform block declares which features it has."
metadata: 
  node_type: memory
  type: feedback
  originSessionId: d0436cb4-bd6b-40ff-8710-3fe7b8b4395b
---

When code is conditional on hardware/platform capability, gate it with a **feature** macro
(`#ifdef SPC700`), not the **platform** macro (`#ifdef SNES`). The platform block is the one
place that maps a platform to its feature set:

```c
#ifdef SYSTEMSNES
#define SNES
#define SPC700      // SNES has an SPC700 audio co-CPU
#endif
```

Then every feature-specific site is `#ifdef SPC700`. In drmon (`devsys/tools/drmon`) the
platform→feature defines live in `global.hpp` (`SYSTEMSNES`→`SNES`, `SYSTEMGEN`→`GENESIS`).

**Why:** features ≠ platforms. Capabilities recur across platforms; hardcoding `#ifdef SNES`
everywhere means a future platform with the same capability requires editing every call site.
With a feature flag, a new platform just adds `#define <FEATURE>` to its block and the feature
lights up — and platforms that lack it exclude all of it by simply not declaring it (e.g. genmon
drops the entire SPC700 surface for free).

**How to apply:** when adding capability-specific code, (1) `#define <FEATURE>` inside the
owning platform block, (2) guard the feature's code/files/TUs with `#ifdef <FEATURE>`. Never
scatter raw platform macros through feature code. (If a feature flag would activate stale/dead
code — e.g. a missing `#include` — neutralize that first.) Related: [[user-is-will]].
