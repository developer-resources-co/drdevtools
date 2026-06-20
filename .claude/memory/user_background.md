---
name: user-background
description: "Will Norris — professional background, expertise, and project history relevant to drdevtools"
metadata: 
  node_type: memory
  type: user
  originSessionId: 5d3c78d4-b484-4d71-ad7a-548e022d93b3
---

Will is a professional game developer with commercial SNES experience. He used the
**Zardoz 65816 C compiler** to develop at least two commercial SNES titles:

- *Demolition Man* (SNES)
- *Izzy's Olympic Quest* (SNES, official 1996 Olympics game)

He notes there were likely other commercial titles developed with Zardoz as well.

This makes him a direct primary source on the **existence, real-world use, and developer
experience** of 1990s 65816 tooling — that Zardoz shipped real commercial SNES games, that it
worked in production, what the workflow was like. Treat that as credible firsthand testimony, not
community lore.

**Important boundary (corrected 2026-06-13):** he is *not* a source on the compiler's **internals**
— calling convention / ABI frame layout, codegen details. He used Zardoz to ship games; he didn't
write its code generator, and it was ~30 years ago ("I barely dealt with that issue, maybe once,
then moved on"). Don't ask him to recall the PHD/TCD frame, return-value registers, etc., and don't
frame "Will remembers the ABI" as a contribution shortcut. The durable ABI reference is the
*documented* WDC816CC/ORCA-C prior art (WDC compiler manual + ORCA/C source), captured in the
[[user-background]]-adjacent Zardoz investigation — point at the manual, not the memory.

**How to apply:** When discussing Zardoz, WDC816CC, or SNES C development history, weight Will's
direct experience on use/history accordingly — he doesn't need background on what SNES development
was like, he did it commercially — but cite documentation, not his recall, for compiler-internal
ABI details.
