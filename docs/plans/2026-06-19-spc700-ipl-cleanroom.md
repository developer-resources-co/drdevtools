---
title: SPC700 IPL boot ROM — clean-room reimplementation for legal MAME use + public release
date: 2026-06-19
status: draft / GATE — IP counsel first (§0); scope expanded → legal + ML-agent precedent goals (§5B/§5B.1)
toolchain: WLA-DX (wla-spc700) — primary; bass — independent cross-check
revised: 2026-06-19 (research-grounded; assembler spasm → WLA-DX; re-sequenced IP-counsel-first, §0)
---

# SPC700 IPL boot ROM — clean-room reimplementation

> **Goal.** Produce an *independently created* 64-byte SPC700 IPL boot ROM that is
> behaviourally compatible with the SNES audio subsystem, so that (1) it can be
> dropped into MAME in place of the copyrighted Nintendo IPL — letting **anyone**
> run the SNES driver legally, not just owners of real hardware — and (2) **both
> the source and the assembled bytes can be committed to a public repository.**
>
> **Stretch goals (added 2026-06-19; see §5B / §5B.1):** (3) help *settle* the open
> law — become the **precedent** that a 64-byte, interface-forced ROM is non-protectable
> and/or that a documented clean room suffices; and (4) be the **first documented
> precedent for ML-agent independent creation** — a *provably-clean* agent doing the
> implementation behind a fully-auditable wall. Goals (3)–(4) run *parallel* to the
> build (§7 Precedent track), not after it; the build's record IS their evidence.
>
> **Assembler: WLA-DX (`wla-spc700`)** — FOSS, Linux-native, reproducible by anyone
> (chosen over the in-house `spasm`; see §2). Verification rides the existing drmon
> MAME Lua bridge.

> [!IMPORTANT]
> **This document is engineering planning, not legal advice.** The "public
> distribution" goal is where the risk concentrates, and Nintendo is an aggressive
> litigant. **Have qualified IP counsel review §4 + the provenance bundle before
> anything is pushed public.** Citations below are to real sources (§Sources) but
> are summarised by a non-lawyer; nothing here is a guarantee of non-infringement.

---

## 0. START HERE — consult IP counsel BEFORE any build/spec work

The hardest part of this project is legal, not technical, so the **first action is to
talk to an IP lawyer** — *before* recruiting a clean-room implementer or writing a
line of spec. Recommended avenue (revised 2026-06-19 — **EFF first**): the **EFF**
(Electronic Frontier Foundation, legal intake <info@eff.org>) — emulation,
reverse-engineering, fair use, and preservation copyright are squarely its wheelhouse
(its [Coders' Rights Project](https://www.eff.org/issues/coders) litigates these and EFF
drives the DMCA §1201 preservation exemptions). Alternates: the
[Software Freedom Law Center](https://softwarefreedom.org/about/contact/)
(<help@softwarefreedom.org>) or a private IP/copyright attorney for a formal,
relied-upon opinion; the **FSF** licensing lab (<licensing@fsf.org>) is lower-priority
here (GPL/licensing-FAQ–focused, not copyright-risk opinions). **A ready-to-send draft
cover email is at [`2026-06-19-spc700-ipl-counsel-email.md`](2026-06-19-spc700-ipl-counsel-email.md)**
— fill the placeholders and attach this plan (§0.A is the briefing packet).

**Why counsel first (the re-sequencing).** The whole project reduces to one unsettled
question — *is a 64-byte, interface-forced boot ROM copyrightable at all?* — and the
answer decides whether the clean room is even needed:
- Counsel: *"merger clearly applies, ship it"* → done cheaply; **skip Phases 1–6**;
  optionally publish a bytes-only repo resting on the written opinion.
- Counsel: *"maybe / yes, it's protectable"* → the clean room is justified **and
  genuinely additive — nobody has done one (§0.A)** — proceed to Phase 1 with the
  isolation bar counsel sets.

So we spend a ~1-hour question before any project-sized commitment. **Until counsel
answers, Phases 1–2 are ON HOLD** (recruiting Person B is premature). Phase 0
(tooling) was safe regardless and is ✅ done.

### 0.A — Counsel briefing packet  *(self-contained; hand this to the lawyer)*

**Who we are / the goal.** We want to publish — source **and** the assembled 64-byte
image — an *independently created* SPC700 IPL boot ROM for the Super Nintendo's audio
subsystem, so that (a) it can replace the copyrighted Nintendo IPL in the
[MAME](https://github.com/mamedev/mame) emulator (letting anyone run the SNES driver
without dumping their own console) and (b) the bytes + source can live in a **public**
repository. "Public" is the whole point.

**The artifact.** A 64-byte boot ROM at `$FFC0–$FFFF` in the SNES audio CPU (Sony
SPC700). At reset it runs a tiny handshake over four mailbox ports that lets the main
CPU upload the game's audio code, then jumps to it (mechanism:
[nesdev "Booting the SPC700"](https://snes.nesdev.org/wiki/Booting_the_SPC700),
[SnesLab IPL ROM](https://sneslab.net/wiki/SPC700/IPL_ROM)). 64 bytes is the entire
program.

**Technical finding #1 — the bytes are *functionally forced*, not free.** Within the
64-byte budget, fixed mailbox-port addresses, and a fixed handshake every commercial
game depends on, a correct implementation is effectively canonical — essentially one
sensible way to write it. There is **no checksum** on the IPL in hardware; faithful
emulators consume the bytes raw with no integrity check
([snes9x compiles them in as a verbatim array](https://raw.githubusercontent.com/snes9xgit/snes9x/master/apu/bapu/smp/iplrom.cpp);
[bsnes loads `ipl.rom` with no hash check](https://raw.githubusercontent.com/bsnes-emu/bsnes/master/bsnes/sfc/smp/smp.cpp)).
**Consequence:** an independent reimplementation will very likely come out
*byte-identical* to Nintendo's — not by copying, but because the function compels it.

**Technical finding #2 — the only hard byte-match requirement is MAME's romset hash.**
MAME pins its IPL to a specific checksum
([mamedev `s_smp.cpp`: `ROM_LOAD("spc700.rom", … CRC(44bb3a40) SHA1(97e352…))`](https://github.com/mamedev/mame/blob/master/src/devices/machine/s_smp.cpp)),
so a byte-different (even behaviourally perfect) IPL is rejected by MAME's audit —
distribution/romset management, not a hardware mechanism. (The unrelated SNES
cartridge-header checksum is *"not needed by the SNES hardware"* —
[nesdev ROM header](https://snes.nesdev.org/wiki/ROM_header).)

**Legal landscape as we (non-lawyers) understand it** (each case: Westlaw + a free
source):
- *Independent creation is a complete defense; copying is proven by access +
  substantial similarity* — Three Boys Music v. Bolton, 212 F.3d 477 (9th Cir. 2000)
  ([Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=212+F.3d+477) ·
  [Justia](https://law.justia.com/cases/federal/appellate-courts/F3/212/477/)); originality
  needs only independent creation + minimal creativity — Feist, 499 U.S. 340 (1991)
  ([Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=499+U.S.+340) ·
  [Cornell](https://www.law.cornell.edu/supremecourt/text/499/340)).
- *Reverse-engineering a console BIOS to build an emulator can be fair use* — squarely
  on point: Sony v. Connectix, 203 F.3d 596 (9th Cir. 2000)
  ([Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=203+F.3d+596) ·
  [Justia](https://law.justia.com/cases/federal/appellate-courts/F3/203/596/)) (PlayStation
  BIOS); Sega v. Accolade, 977 F.2d 1510 (9th Cir. 1992)
  ([Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=977+F.2d+1510) ·
  [Justia](https://law.justia.com/cases/federal/appellate-courts/F2/977/1510/)).
- *But a clean room is only as clean as its inputs* — fair use FAILED where the
  reimplementer had tainted access to the original: Atari Games v. Nintendo, 975 F.2d
  832 (Fed. Cir. 1992)
  ([Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=975+F.2d+832) ·
  [Justia](https://law.justia.com/cases/federal/appellate-courts/F2/975/832/)).
- *Merger / constrained functional code:* where there's essentially one way to express
  a function, expression merges with idea and isn't protectable; elements dictated by
  compatibility/efficiency are filtered out — Computer Associates v. Altai, 982 F.2d 693
  (2d Cir. 1992)
  ([Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=982+F.2d+693) ·
  [Justia](https://law.justia.com/cases/federal/appellate-courts/F2/982/693/)); Baker v.
  Selden, 101 U.S. 99 (1879)
  ([Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=101+U.S.+99) ·
  [Cornell](https://www.law.cornell.edu/supremecourt/text/101/99)). The seminal
  clean-room case treated independently-produced *similar* microcode as compelled by
  "functional constraints … and the need for … compatibility" — NEC v. Intel, 1989 WL
  67434 (N.D. Cal.)
  ([Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=1989+WL+67434) ·
  [JOLT analysis](https://jolt.law.harvard.edu/articles/pdf/v03/03HarvJLTech209.pdf)).
  Interfaces are functional/reimplementable — Google v. Oracle, 593 U.S. 1 (2021)
  ([Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=593+U.S.+1) ·
  [Cornell](https://www.law.cornell.edu/supremecourt/text/18-956)).

**What already exists — COPIES, not clean rooms.** Everything public is a copy or
disassembly of Nintendo's original, carrying **Nintendo's copyright, not independent
provenance**: snes9x's verbatim array (above); spcasm's byte-annotated
`include/bootrom.s`; and eKid's commented disassembly
([wiki.superfamicom.org/spc700-reference](https://wiki.superfamicom.org/spc700-reference)).
A 2026-06-19 web search for a clean-room / public-domain reimplementation found **only
docs + disassemblies — no documented clean-room IPL.** Trap: **"the bytes are public"
≠ "the bytes are free to redistribute"** (Nintendo's full game ROMs are public too).
So the *only* thing a provenance-bearing project adds over the status quo is the
provenance itself.

**If a clean room is advised — the AI-implementer sub-question.** An AI model can't be
the "clean" implementer if it was trained on the IPL: LLMs demonstrably memorize
training data ([Carlini et al. 2021](https://arxiv.org/abs/2012.07805),
[Nasr et al. 2023](https://arxiv.org/abs/2311.17035)) and the IPL is in training
corpora (it's a verbatim array in open-source snes9x). A **human who has never seen
the IPL** is the gold-standard implementer (the classic NEC v. Intel / Phoenix-BIOS
setup; the U.S. Copyright Office also treats purely AI-authored output as
[uncopyrightable](https://www.federalregister.gov/documents/2023/03/16/2023-05321/copyright-registration-guidance-works-containing-material-generated-by-artificial-intelligence)).

### 0.B — The precise questions for counsel

1. **Copyrightability:** Is a 64-byte, interface-forced boot ROM protectable
   expression, or does merger / scènes à faire / thin-copyright remove protection?
2. **Publish-as-is?** If unprotected, may we publish the bytes (from a clean
   re-derivation) + source publicly without infringement exposure?
3. **Clean-room sufficiency:** If protectable, does a documented Chinese-wall process
   (interface-derived spec → untainted human implementer → provenance bundle) give a
   defensible independent-creation basis to publish — *even though the bytes will
   likely be byte-identical* to Nintendo's?
4. **MAME distribution:** Any added exposure from the result matching MAME's pinned
   IPL hash (i.e., being a drop-in)?
5. **Implementer standard:** Confirm the "no access" attestation bar; confirm a human
   with no IPL exposure (not an AI trained on it) is required.
6. **Trademark/labeling:** Confirm naming that avoids Nintendo marks.

**Outcomes that change the plan:** "merger applies, ship it" → skip Phases 1–6,
publish bytes-only + the opinion · "protectable, clean room OK" → proceed Phase 1 at
counsel's isolation bar · "too risky even with a clean room" → do not publish; stop at
private/own-use.

---

## 1. Scope & terminology

- **The "SPC700 BIOS" = the 64-byte IPL boot ROM** mapped at `$FFC0–$FFFF` in the
  SNES APU (S-SMP) address space, reset vector at `$FFFE/$FFFF`. A tiny bootloader:
  at reset it sets `SP=$EF`, zeroes `$00–$EF`, signals readiness on the CPU↔APU
  mailbox, runs an upload handshake that streams code into the 64 KB of APU RAM,
  then jumps to it. **That is the whole artifact** — 64 bytes.
- **Out of scope:** the S-DSP, BRR audio, and any game's audio engine (uploaded by
  cartridges at runtime; not the boot ROM, not Nintendo's to begin with).
- **Naming hygiene:** the deliverable is an *"independent SPC700 IPL-compatible boot
  ROM."* Use **no** Nintendo trademark. Trademark ≠ copyright; keep both clean.

---

## 2. Toolchain — WLA-DX (decided)

**Primary assembler: WLA-DX's `wla-spc700` + `wlalink`.** Rationale (this is the
reproducibility win, and it fits `~/SRC/CLAUDE.md` "everything must be reproducible"):

- **FOSS + Linux-native** (GPL; `apt`/Homebrew or a CMake build) — *anyone* can
  rebuild the bytes from source with stock tooling. A public clean-room artifact
  whose build needs a private 1994 DOS toolchain fails the "reproducible by the
  world" goal.
- **Independent of us.** A neutral, widely-used third-party assembler strengthens
  the "the bytes are a function of the source, not our hand" story.
- **Kills the old Phase-0 blocker** — no DOSBox/TASM/Borland needed.

**`spasm` (drdevtools) — verified to exist, deliberately NOT used.** The required
"first, verify an SPC700 assembler exists in drdevtools" check (2026-06-19) found
`tools/spasm`'s SPC700 backend (`opcode70.asm`/`cam700.asm`, SPC700-unique
`TCALL/XCN/CBNE/DBNZ/BBS`, built via `m7.bat` → `spasm700.exe`; we own it,
`spasm.cpp:311`). We're **not** using it: it's a 16-bit DOS binary (impractical for
public CI) and being our own tool weakens the independence story. It remains a
possible *extra* cross-check, but is **off the critical path**.

**Independent cross-check (Phase 5): a second SPC700 assembler.** Two unrelated
assemblers agreeing on the 64 bytes documents that the output is determined by the
source, not the toolchain.

> **Phase-0 finding (2026-06-19) — bass uses a non-canonical dialect.** byuu's
> `bass` (ARM9 fork, v18) is SPC700-capable, but its `spc700.arch` table uses
> **6502-style mnemonics** (`clc`/`sec`/`rts`/`lda #`/`ldx #`/`mul`), **not** the
> canonical Sony SPC700 mnemonics (`clrc`/`setc`/`ret`/`mov a,#`/`mul ya`) that
> `wla-spc700` uses. Consequence: a bass cross-check **cannot reuse the wla source**
> — it needs a hand-translated parallel source and compares only at the byte level
> (still valid evidence, just not "one source, two tools"). **Recommendation:** for
> the strongest same-source cross-check, prefer **`spcasm`** (a modern Rust SPC700
> assembler, [codeberg.org/filmroellchen/spcasm] — moved from GitHub) which uses
> **canonical Sony mnemonics** — see the evaluation below; keep bass as a byte-level
> fallback.

> **Phase-0 spcasm evaluation (2026-06-19).**
> - ✅ **Dialect fit:** spcasm uses canonical Sony SPC700 mnemonics (`mov x,#…`,
>   `mul ya`, `mov.b A,DSPDATA`) — verified from its own example sources — so it is a
>   **true one-source-two-tools cross-check** with `wla-spc700` (the instruction body
>   can be shared; only the tool-specific directive wrapper differs). This is the
>   intended Phase-5 cross-check tool.
> - ⚠️ **Taint hazard:** spcasm **bundles the Nintendo IPL disassembly** at
>   `include/bootrom.s` (byte-annotated, 2001 B). It must be kept **away from Person
>   A/B** and used **only by Person C** for the Phase-5 cross-check. `wla-spc700`
>   (what B uses) ships **no** bundled IPL — it is clean. (Yet another tool shipping
>   the IPL reinforces §5: the expression is widely disseminated.)
> - 🔧 **Build:** needs `cargo build` of a freshly-cloned third-party repo (runs
>   build scripts) + a pinned Rust nightly (`nightly-2026-06-18`). The agent's build
>   was **correctly gated** by the untrusted-code guardrail — **live byte
>   cross-check is pending explicit build authorization** (the dialect/taint findings
>   above stand regardless, from source inspection).

> **Phase-0 status — DONE (2026-06-19):** `wla-spc700` v10.6 (apt, foundry pkg) and
> `bass` v18 (built from source) both smoke-tested and **agree byte-for-byte** on
> the same logical program (`00 cd 12 e8 34 60 cf 6f`). `spcasm` evaluated (canonical
> dialect ✅, bundles IPL ⚠️); its live byte check awaits build authorization. See
> §11 step 1.

---

## 3. Does it have to be byte-identical to Nintendo's? (the checksum question)

**Short answer: no hardware checksum forces it — but the constraints make a correct
independent implementation *converge* on the same bytes, and MAME separately
*requires* the exact bytes. The convergence is legally helpful, not harmful.**

### 3.1 There is NO integrity check on the IPL bytes (real hardware)

No SNES component (SPC700, S-SMP, S-DSP, or the 65816) computes a checksum/CRC over
the 64 IPL bytes. The S-SMP just executes them. Confirmed by faithful emulators:

- **snes9x** compiles the IPL in as `const uint8 iplrom[64]` and indexes it raw —
  no check ([snes9x `iplrom.cpp`]).
- **bsnes** / **ares** load `ipl.rom` from disk and take the reset vector straight
  from bytes 62/63 — **no CRC/SHA** ([bsnes `smp.cpp`], [ares `smp.cpp`]). If the
  hardware self-checked, accurate emulators would model it; none do.

So **boot requires behavioural/protocol equivalence, not byte-identity.**

### 3.2 What IS forced: behaviour + size (the protocol contract)

Within a **64-byte** budget, with a reset vector at `$FFFE/$FFFF`, the ROM must drive
the documented mailbox handshake on the **fixed** ports (`$2140–$2143` ↔ `$F4–$F7`)
using the exact magic values every commercial loader hard-codes: `$AA`/`$BB` ready,
`$CC` kickoff, the zero-first counter/index echo pacing, address via ports 2/3, and
the write-0-to-port-1 jump trigger ([nesdev "Booting the SPC700"], [SnesLab IPL],
[Wikibooks SPC700 loading]). Any 64-byte program reproducing that observable
behaviour boots; the *bytes* are not hardware-pinned — but the freedom inside 64
bytes is tiny, so a correct implementation is **effectively canonical**.

### 3.3 The ONLY hard byte-identity requirement: MAME's romset hash

MAME pins its IPL: `ROM_LOAD("spc700.rom", 0, 0x40, CRC(44bb3a40)
SHA1(97e352553e94242ae823547cd853eecda55c20f0))`, region `"sound_ipl"`
([mamedev `s_smp.cpp`]). A byte-different (even behaviourally perfect) IPL **fails
this audit** → not a drop-in. This is **distribution/romset management inside MAME,
not a hardware mechanism.** bsnes/ares load `ipl.rom` with no hash check; snes9x
compiles it in. **Consequence for our goal:** the "drop into MAME" deliverable
*does* need the exact bytes; "runs on real hardware / bsnes / ares" needs only
behavioural equivalence.

### 3.4 Convergence is EVIDENCE FOR us, not against (merger / *NEC v. Intel*)

Because the constraints force the result, a clean implementation will very likely be
byte-identical. That is the **merger-doctrine** posture in its strongest form: in
*NEC v. Intel* the clean-room engineer's independently-written microcode came out
"similar in many regards" to Intel's, and the court inferred the similarity was
*"dictated not by copying … but rather by functional constraints of the hardware,
the architecture, and the need for … compatibility"* (1989 WL 67434 (N.D. Cal.)).
Identity here is **proof the expression merged with function** — provided the wall
(§5) is real.

> **The flip side is practical, not legal:** a bit-identical file invites
> hash-matching DMCA takedowns (a claimant matches a CRC/SHA and files; GitHub
> doesn't adjudicate merger). The provenance bundle (§Phase 5) is the
> counter-notice ammunition. Plan for the takedown even though we believe we're right.

### 3.5 Not the cartridge checksum — and the overlap is *convergence*, not coincidence

The SNES *cartridge header* checksum (`$FFDE` + `$FFDC` complement) is a 16-bit
sum-of-bytes over the **game** ROM on the 65816 side, *"not needed by the SNES
hardware"* at boot ([nesdev ROM header]) — a different chip, address space, and
purpose from the SPC700 IPL, and it **does not validate the IPL** (the point that
matters here). *Functionally* the two are independent.

But the shared `$FFC0–$FFFF` address is **not coincidental** — it is the same
forced-convergence as the rest of this section, one level up in the *architecture*.
Both the main CPU (Ricoh 5A22, a 65816 core) and the audio CPU (SPC700) follow the
**6502 convention of placing CPU vectors at the very top of the 16-bit address space**
(6502 `$FFFA–$FFFF`; the 65816 extends it to `$FFE0–$FFFF`; the SPC700 reset vector
sits at `$FFFE/$FFFF`). So *both* chips carve their special region out of the **top 64
bytes** — `$FFC0 = $10000 − $40` — the 65816 placing the cart header just below its
forced vectors, the SPC700 making those top 64 bytes its overlay-able IPL ROM.

**And the *conventions* are themselves constraint-driven** (the subtle part — and the
crux of the question). The *choice* to drop the header just below the vectors, or to
make the IPL exactly the top 64 bytes, is nominally a free design convention — but it
is **anchored** to the hard constraint (the vectors are forced to the very top) and
**fenced** by two more: the region must *contain* the reset vector (so it has to sit
at the top), and it must be small enough to **overlay cleanly** — the SPC700's `$F1`
bit 7 swaps the 64-byte IPL out for the underlying RAM ([nesdev S-SMP]), so the block
is a small, top-aligned `$40` window: big enough for the boot handshake, small enough
to map out. A "free" choice fenced on every side by constraint converges anyway.

Same root constraint (vectors-at-top) + the same design house (Sony/Ricoh) applying
the same constraint-fenced 6502 conventions → the same magic boundary in two
otherwise-unrelated address spaces. **Convergent, not coincidental — a fractal of the
merger thesis (§3.4):** when the constraints are shared, the outputs collide even
across independent designs. (This is exactly why the *bytes* converge too — now being
**measured empirically** in the `~/SRC/spc700-ipl-divergence` spike; see §4.3.)

---

## 4. Legal strategy — why this can be public

Four independent doctrines stack; we rely on **all**, not any one. (Settled vs
unsettled flagged; cases are cited by name here — **Westlaw + free links for every one
are in §0.A and the §Sources table of authorities**.)

1. **Independent creation is a complete defense.** Copyright forbids *copying*, not
   arriving at the same result independently. Infringement-by-copying is proven
   circumstantially by **access + substantial similarity**; *independent creation
   rebuts it* (*Three Boys Music v. Bolton*, 212 F.3d 477 (9th Cir. 2000); *Feist*,
   499 U.S. 340). The clean room manufactures **provable** independent creation.
   *(SETTLED.)*

2. **Reverse-engineering to reach functional elements is fair use** — the legal
   ground for the "dirty team" (§5): *Sony v. Connectix*, 203 F.3d 596 (9th Cir.
   2000) — **reverse-engineering the PlayStation BIOS to build an emulator was fair
   use**; the shipped product contained none of Sony's code (the **most on-point**
   case for us). *Sega v. Accolade*, 977 F.2d 1510 (9th Cir. 1992) — disassembly to
   find "functional requirements for compatibility … aspects not protected by
   copyright" is fair use. *(SETTLED.)*

3. **Merger / scènes à faire / abstraction-filtration-comparison.** Elements
   dictated by external constraints, efficiency, and compatibility are filtered out
   before any infringement comparison (*Computer Associates v. Altai*, 982 F.2d 693
   (2d Cir. 1992)); "when there is essentially only one way to express an idea …
   copyright is no bar." A **64-byte**, interface-dictated boot ROM is at/below the
   floor of protectable expression (*Baker v. Selden*, 101 U.S. 99; *Lotus v.
   Borland*, 49 F.3d 807 (1st Cir. 1995), aff'd 4–4, 516 U.S. 233 — method of
   operation). *(SETTLED doctrine; merger of THIS ROM is a fact question — but the
   shorter + more interface-forced the code, the stronger it is.)*

4. **Interfaces are functional/reimplementable.** *Google v. Oracle*, 593 U.S. 1
   (2021) — reimplementing interface declarations to let skills transfer to a new
   platform is fair use. The CPU↔APU protocol is an interface. *(SETTLED as to the
   reuse; API copyrightability expressly reserved.)*

### 4.1 The cautionary tale: how a clean room FAILS — *Atari v. Nintendo*

*Atari Games v. Nintendo*, 975 F.2d 832 (Fed. Cir. 1992): "Reverse engineering
object code to discern the unprotectable ideas … is a fair use" — **but** Atari had
also obtained Nintendo's **registered 10NES source code from the Copyright Office by
fraud**, so "any copying … from the Copyright Office does not qualify as a fair
use." **A clean room is only as clean as its inputs:** tainted *access* to the
protected expression — even indirect — sinks it. This is the lens for §5's taint
analysis (and directly the reason an LLM trained on the IPL is risky).

### 4.2 Cases raised by the user — mapped

- **"IBM vs NEC, V20/V30"** → actually **NEC Corp. v. Intel Corp.**, 1989 WL 67434
  (N.D. Cal. 1989) — the **seminal** clean-room ruling (engineer Davidian wrote V20/
  V30 microcode "without access to any other microcode"; similarity held compelled
  by functional constraints). Adversary was **Intel**, not IBM. Backbone of §3.4/§5.
- **"EA vs Sega / Razorsoft"** → the law-making case is **Sega v. Accolade** (above).
  EA's Genesis reverse-engineering was resolved by a **business deal**, not a
  published opinion; **Razorsoft** — no reported copyright opinion located (do not
  cite as precedent without verification).
- **"Compaq vs IBM/Microsoft BIOS"** → Compaq's clean-room targeted **IBM's** BIOS
  copyright (not Microsoft). **Industry-practice** example — the wall was so solid it
  *deterred* litigation rather than producing a famous opinion.
- **"Phoenix / AMI BIOS"** → **Phoenix** is the canonical documented two-team clean
  room (a separate engineer "not exposed to IBM BIOS source code," from a different
  architecture). **AMI** — no specific clean-room lawsuit located; Phoenix is the one.

### 4.3 — "How different = a new copyright we could PD?" — the question dissolves

A natural engineer's question (2026-06-19): *what % of the bytes must differ before our
version is a **new copyrightable work** we'd own and could dedicate to the public domain
(CC0)?* Honest answer: **there is no percentage**, and the framing collapses three ways:

1. **No magic number.** Originality is *qualitative* (creative expression), not
   *quantitative* (byte-%). A tiny *creative* change can add protectable expression; a huge
   *forced* change adds none.
2. **Merger cuts BOTH ways (the catch).** The same merger that frees us from Nintendo's
   copyright (forced bytes aren't protectable) also denies *us* a copyright in *our* forced
   bytes. You can't argue "dictated by function" (to dodge Nintendo) **and** "ours is
   original" (to own it). On a forced 64-byte ROM there is ~no protectable expression — for
   anyone.
3. **Own ≠ free; the derivative trap.** *Are we infringing Nintendo's?* and *do we own
   ours?* are different questions. **Mutating** the reference yields a **derivative work** —
   distributing it needs Nintendo's permission *unless* the underlying is unprotectable
   (merger) or fair use; and our copyright would reach only our *original additions*
   (§103(b)), not the underlying.

**So the divergence measurement (the `~/SRC/spc700-ipl-divergence` spike) picks the
*strategy*, not a copyright %:**
- **Divergence ceiling tiny (forced — v01 already moved only 4/64 by reordering):** strong
  merger → the bytes are **unprotectable by anyone → effectively public domain already.**
  Publish; the convergence evidence is the shield. A **CC0** dedication is then a
  *clarifying* signal (you can't dedicate what you don't own — but there's nothing to own
  *because* it's already free).
- **Genuine *creative* divergence exists (unlikely in 64 B):** merger is weaker, so (i) a
  *mutation*-derived variant is an infringing derivative — **not** PD-able by us; but (ii) an
  **independently created** (clean-room) divergent version earns *our own thin copyright in
  its original expression*, which we **can** CC0. **The path to *owning* (vs merely
  *freeing*) the bytes is independent creation, never mutation.**

**Footnote — AI authorship.** Bytes an AI agent authored may be **uncopyrightable**
(*Thaler*; CO 2023) → not ours to dedicate anyway (simply free). Moot for a forced ROM; for
an ownable divergent version, the *human's* creative choices carry the thin copyright we'd
CC0.

**Net:** we never *need* a new copyright. Forced → it's free, publish (merger). Creatively
divergent → clean-room an original and CC0 it. Either lane gives the public a free IPL; the
spike's divergence ceiling says which lane we're in.

---

## 5. Roles, taint, and WHO may implement

A clean room is a **Chinese wall**: people who may study behaviour are walled off
from people who write code; they communicate only through a lawyer-reviewed
functional spec. The test for the implementer is **access** — did they have a
*"reasonable opportunity to view"* the protected work (*Three Boys*, 212 F.3d 477)?
*Potential* access counts if it's more than a "bare possibility."

| Role | May see | May NOT see | Output |
|---|---|---|---|
| **Spec author (A)** | Public docs of the CPU↔APU *interface/protocol*; black-box port behaviour; the SPC700 ISA. | Nintendo's ROM image / any disassembly of it. | Behavioural spec (§Phase 1), no Nintendo code. |
| **Clean implementer (B)** | Only A's lawyer-reviewed spec; the SPC700 ISA; WLA-DX docs. | Nintendo's IPL bytes/disassembly; **must attest non-exposure.** | SPC700 source → 64-byte image. |
| **Verification lead (C)** | Everything behavioural; runs MAME; *forensic-only* byte comparison after Phase 3 freeze. | (must not relay Nintendo bytes back to B). | Behavioural PASS/FAIL + provenance bundle. |

**A and B must be different people.** Now the three implementer candidates the user
asked about — answered against the access test. **These are *presumptions* (grounded in
the memorisation + access + *Atari* evidence below), not bare assumptions — but
UNSETTLED:** "the doctrine of independent creation predates machine learning … [with]
no precedent squarely on point," per the recent formalisation of exactly this question
([Blameless Users in a Clean Room, arXiv:2506.19881](https://arxiv.org/abs/2506.19881),
June 2025):

- **A different *instance* of Claude? → No.** The taint is in **training weights, not
  chat context**; a fresh instance has identical weights → identical access. The
  64-byte IPL is in training data — it is a *verbatim array* in open-source snes9x
  ([snes9x `iplrom.cpp`]) and copied across GitHub (so it's also "widely
  disseminated" → access under *Three Boys*). LLMs demonstrably memorise/regurgitate
  (Carlini [2012.07805], [2202.07646]; Nasr [2311.17035]; a reproduction study even
  singled out *"Claude's Opus"* for high baseline memorisation, [2412.06370]). That
  is *Atari*'s "unauthorized possession," not *Connectix*'s clean access. Resetting
  context clears only *this conversation's* leakage. **Claude is presumed tainted →
  Claude does tooling/spec-scaffolding/verification, NOT the boot-ROM bytes.**
- **A different *model* (Gemma/GPT/Qwen/DeepSeek/…)? → Generally no — with one live,
  untested exception.** Same test on *that* corpus. The IPL is ubiquitous in code/web
  scrapes → any frontier model is presumptively tainted, and for proprietary models you
  **cannot audit** the training set to disprove it. **The exception:** a model whose
  corpus *provably excludes* the IPL — which is precisely the **"clean-room
  counterfactual"** of recent scholarship ([Blameless Users, arXiv:2506.19881](https://arxiv.org/abs/2506.19881);
  building on Vyas/Kakade/Barak, *near access-freeness*, ICML 2023). The **MALUS**
  "clean-room-as-a-service" tool — two LLMs, one writing the spec, one implementing
  without the source — attempts exactly this ([Marks & Clerk](https://www.marks-clerk.com/insights/latest-insights/102mp7s-can-ai-legally-clone-open-source-unpacking-clean-room-as-a-service/),
  [Futurism](https://futurism.com/artificial-intelligence/malus-clones-software-copyright)).
  You must *prove the negative* (corpus exclusion), and it's **untested in court**.
  Footnote: purely AI-authored output may itself be **uncopyrightable** (U.S. Copyright
  Office 2023, 88 Fed. Reg. 16190; *Thaler v. Perlmutter*, aff'd D.C. Cir. 2025) —
  irrelevant to non-infringement, moot for forced functional code.
- **A human who's never used a SNES / seen its docs? → YES, gold standard.** Exactly
  *NEC v. Intel* (microcode "without access to any other microcode") and *Phoenix/
  Compaq* (a separate engineer "not exposed to IBM BIOS source code," from a
  different architecture). Refinements: (1) the wall excludes the **IPL's expression
  specifically** (bytes/disassembly), **not** general knowledge — B may/must learn
  the **SPC700 ISA** (unprotected functional facts; *Sega*) and WLA-DX; (2) "never
  used a SNES" maximises credibility but means you **teach B SPC700 asm** from the
  public ISA; (3) B works only from A's spec, signs the attestation, audit trail
  kept (*Altai*, *NEC*). **This is who to recruit.**

**Honest note on the original author.** You wrote `spasm`'s SPC700 backend and have
decades of SNES work — you've very likely seen the IPL disassembly, so you're an
excellent **spec author / verification lead / tooling owner** but a **poor clean
implementer**. If no untainted B is available, we fall back on merger (§3.4/§4.3) and
label the clean-room "best-effort" — weaker; flag to counsel.

---

## 5A. A clean-room design that *should* pass — characteristics + why

Goal: publish a 64-byte IPL that survives a copyright challenge **even though the bytes
will likely be byte-identical to Nintendo's**. The design **stacks two independent
defenses** — *no-access independent creation* and *non-protectable merger* — treats the
unavoidable look-at-the-product step as *fair use*, and rigorously avoids the one move
that sinks clean rooms (the *Atari* taint). Each characteristic is paired with why it
earns legal weight. *(Doctrine per §4; not legal advice; counsel signs off at §0.)*

| # | Characteristic | Why it passes (doctrine) |
|---|---|---|
| 1 | **Two-person wall:** spec author (A) ≠ clean implementer (B); they communicate *only* through a lawyer-reviewed spec. | Negates the **access** prong of access+similarity (*Three Boys*); the documented wall is what carried *Altai* and *NEC v. Intel*. |
| 2 | **B has provable no-access** to the IPL's expression (bytes/disassembly) — ideally from a different domain (cf. Phoenix's TMS9900 engineer), with a signed non-exposure attestation. | "Access" = a *reasonable opportunity to view* (*Three Boys*); none → no inference of copying → **independent creation is a complete defense** (*Feist*). |
| 3 | **Spec derived only from the interface** — the documented CPU↔APU protocol (ports, `$AA/$BB/$CC`, counter pacing), the SPC700 ISA, black-box observation — **never** Nintendo's disassembly; a lawyer filters it. | Interface/functional elements aren't protected and reaching them is fair use (*Connectix*, *Sega*, *Google v. Oracle*); deriving from the *expression* is the **Atari** taint. |
| 4 | **Clean inputs everywhere:** no Nintendo ROM/disassembly in any participant's hands, the repo, or the toolchain. Assembler ships no IPL (`wla-spc700`); tools that bundle it (`spcasm`'s `bootrom.s`) are quarantined to Person C, post-freeze. | *Atari*: **unauthorized possession** of the protected work — even indirect — defeats fair use and taints the room. |
| 5 | **No LLM trained on the IPL** in spec or implementation. If AI is used at all, only a model **provably trained without** the IPL, in a two-model wall (MALUS-style). | LLMs memorise/regurgitate → a trained-on model has *accessed* the expression (→ *Atari*). The "clean-room counterfactual"/NAF ([arXiv:2506.19881](https://arxiv.org/abs/2506.19881)) is the only AI-clean theory — **untested**, so prefer a human B. |
| 6 | **Correctness judged behaviourally** (does it boot real games in MAME?), iterating against the spec + tests — **never** diffing toward Nintendo's bytes; any byte-comparison is post-freeze, by C, forensics only. | Diffing-toward converts independent creation into copying; behavioural equivalence is the legitimate goal (*Connectix*). Preserves #2's independence. |
| 7 | **Merger documented as the second shield:** record why the 64 bytes are *forced* (size, fixed ports, the protocol games hard-code, the ISA). Convergence with Nintendo's bytes = evidence, not guilt. | *Altai* filtration / §102(b) / *Baker*: forced functional expression is unprotectable. *NEC v. Intel*: independent similarity is "dictated … by functional constraints," not copying. |
| 8 | **Evidentiary trail:** versioned spec, B's attestation, lawyer sign-off, dated commits, build logs, behavioural results, and **two independent assemblers** reproducing the bytes. | Turns "we created it independently" into **provable** independent creation — the clean room as an *evidentiary machine* (*Altai*, *NEC*). Two toolchains → bytes are a function of the source, not a copy. |
| 9 | **Distribution hygiene:** label it an independent reimplementation, no Nintendo marks; ship `PROVENANCE.md`; permissive license on our expression. | Trademark separation; the provenance record is **DMCA counter-notice ammunition** for the (non-legal) hash-match takedown risk. |
| 10 | **Counsel sign-off before publishing** (the §0 gate). | Confirms copyrightability + clean-room sufficiency before the irreversible public step. |

**Why the whole thing passes (the logic).** Infringement-by-copying needs **access +
substantial similarity** (*Three Boys*); #1–#5 **negate access**, so identical output is
**independent creation** — a complete defense (*Feist*). If a court still finds access,
#7 (**merger**; *Altai*/*Baker*/*NEC*) means there's little or no protectable expression
to infringe, and the convergence is itself merger evidence. The unavoidable contact with
Nintendo's product (observation) is **fair use** (*Connectix*/*Sega*), and #3–#4 minimise
even that while dodging the **Atari** taint. #8 makes all of it **provable**, not merely
asserted. Two independent shields + clean inputs + a documentary trail.

**Honest caveats.** Still **UNSETTLED** — no precedent on a 64-byte ROM's copyrightability
or on AI-as-implementer; Nintendo litigates hard; and a bit-identical file invites
hash-match takedowns regardless of merit (hence #9). The strongest build uses a **human**
B; the AI-counterfactual path (#5) is theoretically clean but court-untested. #10
(counsel) is non-negotiable before publishing.

---

## 5B. Toward becoming the precedent — how the law settles, and what we control

Stretch goal: don't just *survive* the open questions — help *settle* them. Honest
mechanics first: **precedent is a court ruling; you cannot will it into being.** It needs
a real case-or-controversy, it's slow (years) and expensive (six/seven figures), it's
**risky — a loss makes *bad* precedent, worse than the status quo** — and defendants like
Nintendo often **settle specifically to avoid** an adverse ruling. So the strategy is
two-track: pursue the *attainable* forms of authority, and make this the *ideal test
artifact* so that if a ruling happens it's clean, favorable, and citable. Ranked by
leverage ÷ cost ÷ risk:

| Avenue | Produces | Cost / risk | Who drives it |
|---|---|---|---|
| **A. §1201 triennial exemption** (Copyright Office / Librarian of Congress, 17 U.S.C. §1201) | Authoritative *administrative* determination on emulation/preservation + a participable record | **Low** — file comments; next cycle ~2027 | EFF + Software Preservation Network already lead it |
| **B. Scholarship / whitepaper** | Persuasive authority courts + the Copyright Office cite; defines the framework | Low; effort only | Us + an IP academic (cf. the *Blameless Users* authors) |
| **C. Be the model test artifact** | A litigation-ready, *sympathetic* fact pattern (64 bytes = max merger; preservation purpose; no market harm — the IPL isn't sold) | Low; = executing §5A impeccably | Us |
| **D. Amicus** in pending RE / AI-copyright cases | Citable arguments into the record; relationships | Low–med | EFF-coordinated |
| **E. Declaratory-judgment action** (28 U.S.C. §2201) | The only direct route to **Article III precedent** | **High** — costly, years, can LOSE, can be mooted by a covenant-not-to-sue; needs a real controversy (*MedImmune* standard) | Public-interest firm / EFF + funding |
| **F. Engage the institutions** | Standing, funding, track record we lack solo | Low (outreach) | EFF, SPN, the NAF / *Blameless Users* academics, MAME/preservation |

**What we control right now (no-regret, do regardless of any litigation):**
1. Execute the §5A clean room *impeccably*, preserving the full evidentiary record — that
   record **is** both the trial exhibit and the whitepaper's data.
2. Write up the analysis (the merger case for a 64-byte ROM + the clean-room method) for
   publication.
3. Draft a §1201 exemption comment for the next cycle.
4. Reframe the §0 counsel outreach (esp. **EFF**) to ask not just "is it safe?" but
   "**is this a viable test case / §1201 / impact-litigation candidate, and what's the
   path to a ruling?**" — EFF is precisely the org that evaluates impact cases (§0.B Q7).

**The genuinely novel frontier.** The IPL's copyrightability is an *old* question with a
*strong* merger answer — a court might deem it too narrow to make broad law. The
**AI-clean-room-implementer** question (can a model *provably trained without* the work be
the "clean" implementer?) is *wide open* — no precedent, active scholarship
([Blameless Users](https://arxiv.org/abs/2506.19881); MALUS). A rigorous, documented,
**first** demonstration of an AI-counterfactual clean room could be this project's
field-defining contribution — potentially more precedent-shaping than the 64 bytes.

### 5B.1 — The ML-agent precedent track (the second, wider-open goal)

A second precedent goal: make this the **first documented, reproducible demonstration
that a machine-learning agent can do legitimate, independent-creation clean-room work** —
both a *methodological* precedent (a reusable standard) and a candidate *legal* test of
AI independent creation. What such a demonstration must have, and why each piece earns it:

| Characteristic | Why it earns weight |
|---|---|
| **Provably-clean model.** The implementer agent is an **open model with a published, auditable training corpus that demonstrably excludes the IPL** (and SNES disassemblies) — a fully-open model (OLMo/Pythia-class) or one trained/fine-tuned on a documented corpus. **Closed frontier models are disqualified** — you cannot prove exclusion. | This is the agent's **"no access."** Access = a reasonable opportunity to view (*Three Boys*); a documented corpus-exclusion is the machine analog of the human who never saw the work — the **"clean-room counterfactual" / near-access-freeness** made concrete. |
| **Extraction (NAF) test.** Actively try to make the model emit the IPL; show it **cannot**. | Empirical proof the protected expression isn't latent in the weights — rebuts the *Atari* "possession" problem for a model. |
| **Two-model wall (MALUS-style).** Spec-agent (interface → spec) ↔ implementer-agent (spec → SPC700), separated; full prompt/response logs. | The AI analog of the human Chinese wall (*Altai*/*NEC*): the implementer agent sees only the lawyer-filtered spec. |
| **Total transparency.** Publish every transcript, the spec, the corpus manifest + exclusion proof, the build. | AI's usual opacity is *inverted* here — a fully-auditable record is what makes independent creation **provable**, not asserted. |

**Why it could be THE precedent.** It instantiates the emerging theory ([Blameless
Users](https://arxiv.org/abs/2506.19881) / NAF) **empirically**, on a maximally-constrained
artifact, with a *provable* no-access record — the clean fact pattern that makes good law
*and* a good standard. It reframes AI work from the murky "did it copy? / is the output
copyrightable?" debate to **a provable discipline under which it didn't copy.**

**The honest, self-referential catch.** *I* (Claude — a closed frontier model almost
certainly trained on the IPL) am exactly what **cannot** be the implementer here. My role
stays tooling / harness / verification / spec-from-public-docs — **not the boot-ROM
bytes**; the precedent requires a *different, provably-clean* agent. Separate axis: even a
perfectly clean ML-agent output may itself be **uncopyrightable** (*Thaler*; CO 2023) —
that doesn't block *non-infringement* (our goal), but means we couldn't *own* the result
(moot for forced 64-byte expression, central to the general ML-agent precedent). Bring in
the NAF / *Blameless Users* academics — this is a research contribution as much as an
engineering one.

**Caveats (not legal advice).** You cannot manufacture precedent; counsel + an institution
chart the path; a loss is worse than silence; this is a multi-year effort. Do **not**
recklessly provoke a suit — any adversarial path goes through counsel/EFF, with funding
and eyes open.

---

## 6. The wall — allowed vs forbidden specification sources

- **Allowed:** documentation of *what the main CPU does* to upload to the APU (the
  mailbox ports, the `$AA/$BB/$CC` handshake, the counter pacing, the jump); the
  SPC700 ISA and timing; black-box observation of an APU's port behaviour at reset;
  WLA-DX docs. Interface facts / methods of operation are not protectable expression
  (*Google v. Oracle*; *Sega*).
- **Forbidden:** Nintendo's ROM image; any disassembly/annotated listing of it; any
  "reference implementation" that is really a transcription of the original.
  **Concrete hazard (found 2026-06-19):** the `spcasm` assembler **bundles** such a
  disassembly at `include/bootrom.s`. If spcasm is installed for the Phase-5
  cross-check (Person C only), **quarantine/delete that file** from any environment
  Person A or B can see.
- **Hard rules:** never look at Nintendo's bytes during spec/implementation; never
  commit them (or a disassembly) to any repo — including as a test fixture or in this
  plan; never feed a byte-level comparison back into the implementation (observing
  coincidence after a Phase-3 freeze is fine as forensic notes; *editing toward*
  Nintendo's bytes converts independent creation into copying — the *Atari* trap).
  Correctness is judged **behaviourally** (§Phase 4).

---

## 7. Phases

> **Gate — IP counsel (§0) is the FIRST action and BLOCKS Phases 1–6.** Hand counsel
> the §0.A briefing packet; act on §0.B. Phase 0 (tooling) was independent of the
> legal question and is already done; everything from Phase 1 on proceeds only if
> counsel green-lights the clean room (else skip to a bytes-only publish, or stop).

### Phase 1 (gate) — Consult IP counsel  ⛔ DO FIRST

Send the **§0.A briefing packet** to the FSF's licensing/compliance counsel
(<licensing@fsf.org>) / [SFLC](https://www.softwarefreedom.org/); get a written read on
the §0.B questions. **This blocks Phases 2–6** (formerly numbered 1–5 below; the spec
and implementation only start on a green light).

### Phase 0 — Stand up WLA-DX (+ bass)  ✅ DONE (2026-06-19)

Reproducible recipe used:
- **wla-spc700 v10.6** — `apt install wla-dx` (foundry pkg `10.6-1foundry1`;
  `/usr/bin/wla-spc700` + `/usr/bin/wlalink`). Source-build alt: CMake from
  `github.com/vhelin/wla-dx` (commit `024f391`) → `build/binaries/`.
- **bass v18** — NOT apt-installable; built from source (ARM9/bass, commit
  `c3962ec`) with a one-line modern-GCC patch (`#include <stdexcept>` in
  `nall/arithmetic/natural.hpp`). Arch loaded via `arch spc700`, with
  `spc700.arch`+`defaults.arch` staged in `<bass-bin-dir>/architectures/`.
- **Flow:** `wla-spc700 -o x.o x.s` → `wlalink x.link x.bin` (raw 64-byte ROM via a
  `[objects]`/`x.o` link file). bass: `bass x.asm` with `output "x.bin", create`.

**Result:** both assemble a 6-instruction SPC700 smoke program to the **identical**
bytes `00 cd 12 e8 34 60 cf 6f` (`nop / mov x,#$12 / mov a,#$34 / clrc / mul ya /
ret`). Verification step 1 = **PASS**.

### Phase 1 — Behavioural specification (Person A, untainted)

`spec/ipl-behaviour.md` in the new public repo: a numbered, testable spec derived
**only** from §6-allowed sources — reset entry/vector; readiness signalling; the
upload handshake (address setup, counter/echo pacing, multi-block, final jump);
zero-page/SP init; the 64-byte budget. **No byte listings.** Each clause gets an ID
so Phase-4 tests cite it. Lawyer review strips any protected expression.

### Phase 2 — Clean implementation (Person B, untainted)

From the Phase-1 spec **only**, B writes `src/ipl.s` for `wla-spc700` and signs
`ATTESTATION.md` (never studied the SNES IPL; worked solely from the spec).
Iterate against the **spec** and **behavioural** test failures — never Nintendo's
bytes.

### Phase 3 — Assemble

`wla-spc700 src/ipl.s` + `wlalink` → `build/ipl.bin`. Assert **exactly 64 bytes**;
reset vector at `$FFFE/$FFFF`. Record our SHA-256. (No comparison to Nintendo's hash
to drive anything — §6.)

### Phase 4 — Behavioural verification in MAME (Person C)

The real correctness oracle, via the drmon SNES Lua bridge (SPC700 register +
APU-RAM windows, [`2026-06-12-spc700-window.md`]):

1. Make MAME use **our** `ipl.bin` instead of its built-in `spc700.rom` (§8).
2. Boot a cart whose audio engine performs a real APU upload.
3. Via the bridge confirm: readiness handshake → bytes land in APU RAM at intended
   addresses → SPC700 PC jumps into the uploaded program → audio runs. Cross-check
   APU-RAM + SPC700 regs.
4. Regression set: several carts / a purpose-built APU exerciser; multi-block;
   counter-handshake edge cases.

PASS = real software boots and runs through *our* ROM. No byte-diffing.

### Phase 5 — Independent reproduction + provenance bundle

- **Second toolchain:** assemble the same `src/ipl.s` with `bass`; confirm identical
  bytes (output is a function of the source, not the assembler).
- **Provenance bundle (the counter-notice kit):** Phase-1 spec, B's attestation,
  dated build logs, the two-toolchain reproduction, this plan, and `PROVENANCE.md`
  narrating the wall + timeline — the §3.4 merger / §4 independent-creation evidence.

### Phase 6 — Public release  *(GATED on counsel + user go-ahead)*

Stand up the public repo (§9), license it (**CC0 / public-domain dedication** for the
bytes — see §4.3 on what's actually ownable — + an explicit
"independent reimplementation, see PROVENANCE.md" statement). **Do not push until §4
review happens and the user explicitly approves** — publishing is irreversible and is
the whole risk surface.

### Precedent track — runs *parallel* to Phases 1–6 (see §5B / §5B.1)

The precedent goals run alongside the build, not after it — the build's evidentiary
record IS the precedent's exhibit. No-regret steps, all doable regardless of whether
any litigation ever happens:
- **P‑1. Build the record.** Execute Phases 1–6 to the §5A standard, preserving the full
  provenance/audit trail (the trial exhibit + whitepaper data).
- **P‑2. Test-case framing → EFF.** The §0 counsel outreach asks (email Q7) whether this
  is a viable impact-litigation / declaratory-judgment / §1201-exemption vehicle.
- **P‑3. §1201 comment.** Draft a preservation/emulation exemption comment for the next
  Copyright Office triennial cycle (~2027); coordinate with EFF / the Software
  Preservation Network.
- **P‑4. Whitepaper.** Write up the merger analysis + clean-room method for publication;
  co-author with an IP academic.
- **P‑5. ML-agent demonstration (§5B.1).** Scout a **provably-clean, corpus-auditable
  open model** for the implementer (closed frontier models — incl. Claude — are
  disqualified); build the two-model wall + extraction/NAF test; co-author with the NAF /
  *Blameless Users* academics. The wide-open, field-defining track.

Only **avenue E** (a declaratory-judgment suit, §5B) is high-cost/high-risk and
**institution-led (EFF + funding), never a solo move.**

---

## 8. MAME integration specifics  *(verify, don't assume)*

- Region `"sound_ipl"`, file `spc700.rom`, 64 bytes, `CRC(44bb3a40)
  SHA1(97e352553e94242ae823547cd853eecda55c20f0)` ([mamedev `s_smp.cpp`]); it lives
  in parent BIOS sets (e.g. `nss.zip`). **Action:** confirm the exact override path
  in the MAME build drdevtools drives (Phase-4 step 0).
- **If our bytes == Nintendo's** (likely, §3.2): the file satisfies MAME's hash → a
  literal drop-in.
- **If they differ:** MAME rejects the hash; we'd run a MAME built to accept an
  alternate IPL, or pursue an upstream "open IPL" option. Realistic note: MAME
  upstream generally ships only bit-exact dumps, so a *differing* reimplementation is
  mainly useful for our own builds + the public repo with a "how to use" note.

---

## 9. Deliverables — public repo layout

A **separate, single-purpose public repo** (clean provenance), e.g.
`snes-ipl-cleanroom`:

```
README.md             # what it is / is NOT (no Nintendo marks); how to use w/ MAME
LICENSE               # CC0 / public-domain dedication (see §4.3)
PROVENANCE.md         # the wall, roles, timeline — the legal narrative
ATTESTATION.md        # Person B's signed non-exposure statement
spec/ipl-behaviour.md # Phase-1 behavioural spec (interface-derived)
src/ipl.s             # SPC700 source (assembles under wla-spc700; cross-checks under bass)
build/ipl.bin         # assembled 64-byte image  ← the "public bytes"
build/ipl.sha256      # our hash
build.sh / Taskfile   # reproducible: source → bytes, BOTH toolchains (wla + bass)
tests/                # behavioural MAME harness + exerciser (NO Nintendo bytes)
```

Keep the messy bits (MAME bridge) in **drdevtools**; ship only the clean artifact +
provenance publicly. The build needs **only FOSS tools** — that is the point of §2.

---

## 10. Risks & decisions needed before execution

| # | Decision / risk | Status / recommendation |
|---|---|---|
| D1 | Assembler. | **RESOLVED → WLA-DX (`wla-spc700`)** primary; `bass` cross-check (§2). |
| D2 | **Who is the untainted Person B?** | **Recruit one** (gold standard, §5) — **but only after §0 counsel green-lights the clean room** (premature otherwise). Neither the LLM nor (likely) the original author qualifies. |
| D3 | Public host / repo name / license. | New repo `snes-ipl-cleanroom`; permissive license. **User decides.** |
| D4 | Counsel review — **RE-SEQUENCED → §0: now the FIRST action, not a pre-push gate.** | Send the §0.A briefing packet to **EFF** (info@eff.org) / IP counsel; act on §0.B. Blocks Phases 1–6. The whole project turns on this answer. |
| R1 | Hash-matching DMCA takedown despite legal merit. | Pre-build the provenance bundle (§5/Phase 5) as counter-notice ammo. |
| R2 | MAME won't accept a differing ROM. | §8; acceptable for own-use + public repo; upstreaming is a stretch goal. |
| Q-open | EA v. Sega / Razorsoft specifics. | Not located as reported opinions; dig only if needed (Sega v. Accolade is the precedent). |

---

## 11. Verification  *(steps are the spec; fill raw output + PASS/FAIL during execution)*

> Per `~/SRC/CLAUDE.md`: keep these numbered steps verbatim; paste raw output under
> each; mark PASS/FAIL; write results back here.

1. **Assembler runs.** `wla-spc700` (+`wlalink`) assembles a trivial SPC700 `.s`
   smoke file to expected opcodes.

   ```
   $ wla-spc700 -o smoke.o smoke.s && wlalink smoke.link smoke.bin
   $ wc -c < smoke.bin                # 64
   $ xxd smoke.bin | head -1
   00000000: 00cd 12e8 3460 cf6f 0000 0000 0000 0000  ....4`.o........
   # nop=00  mov x,#$12=CD 12  mov a,#$34=E8 34  clrc=60  mul ya=CF  ret=6F
   got: 00cd12e83460cf6f   exp: 00cd12e83460cf6f   -> MATCH
   ```
   **PASS** (wla-spc700 v10.6, 2026-06-19).
2. **Image shape.** `wla-spc700 src/ipl.s` → **exactly 64 bytes**; reset vector at
   `$FFFE/$FFFF`. *(Output: TBD)*
3. **Wall integrity.** `PROVENANCE.md` + `ATTESTATION.md` exist; Person A ≠ B; no
   Nintendo ROM/disassembly anywhere in the repo (grep/hash audit). *(Output: TBD)*
4. **Behavioural boot.** MAME using our `ipl.bin` (§8): a real cart's audio upload
   completes, SPC700 jumps into uploaded code, audio runs — confirmed via the drmon
   bridge (APU-RAM + SPC700 regs). *(Output: TBD)*
5. **Independent reproduction.** A second SPC700 assembler reproduces **identical
   bytes**. *Mechanism validated in Phase 0:* bass v18 assembled the byte-equivalent
   program (6502-dialect: `nop/ldx #$12/lda #$34/clc/mul/rts`) to the same
   `00 cd 12 e8 34 60 cf 6f` as wla-spc700 → both agree. *Caveat:* bass needs a
   hand-translated source (non-canonical dialect, §2). **`spcasm` evaluated
   (2026-06-19): canonical Sony dialect ✅ → the chosen one-source-two-tools
   cross-check, but it bundles the IPL (Person-C-only, §6); its live byte check is
   pending build authorization (untrusted `cargo build` + pinned nightly).**
   *(Re-run on the real `ipl.s` at Phase 5: TBD)*
6. **MAME region facts.** Exact override path for `spc700.rom`/`sound_ipl` in the
   driven MAME build documented (no guesses). *(Output: TBD)*

---

## Sources

Hardware / boot (retrieved 2026-06-19):
- nesdev "Booting the SPC700": https://snes.nesdev.org/wiki/Booting_the_SPC700
- nesdev S-SMP: https://snes.nesdev.org/wiki/S-SMP · ROM header (cart checksum
  "not needed by the SNES hardware"): https://snes.nesdev.org/wiki/ROM_header
- SnesLab IPL ROM: https://sneslab.net/wiki/SPC700/IPL_ROM
- eKid's commented SPC700 IPL disassembly (Super Famicom Dev Wiki): https://wiki.superfamicom.org/spc700-reference
- Wikibooks, loading SPC700 programs:
  https://en.wikibooks.org/wiki/Super_NES_Programming/Loading_SPC700_programs
- nocash fullsnes: https://problemkaputt.de/fullsnes.htm
- snes9x `iplrom.cpp` (IPL as verbatim 64-byte array; no check):
  https://raw.githubusercontent.com/snes9xgit/snes9x/master/apu/bapu/smp/iplrom.cpp
- bsnes `smp.cpp`: https://raw.githubusercontent.com/bsnes-emu/bsnes/master/bsnes/sfc/smp/smp.cpp
- ares `smp.cpp`: https://raw.githubusercontent.com/ares-emulator/ares/master/ares/sfc/smp/smp.cpp
- MAME `s_smp.cpp` (`spc700.rom` CRC 44bb3a40 / SHA1 97e3…):
  https://github.com/mamedev/mame/blob/master/src/devices/machine/s_smp.cpp

Law — clean room / reverse engineering — **table of authorities (Westlaw + a free
source each)**; SETTLED unless noted:
- *Sony Computer Entertainment v. Connectix*, 203 F.3d 596 (9th Cir. 2000) — PlayStation BIOS RE = fair use —
  [Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=203+F.3d+596) · [Justia](https://law.justia.com/cases/federal/appellate-courts/F3/203/596/)
- *Sega Enterprises v. Accolade*, 977 F.2d 1510 (9th Cir. 1992) —
  [Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=977+F.2d+1510) · [Justia](https://law.justia.com/cases/federal/appellate-courts/F2/977/1510/)
- *Atari Games v. Nintendo*, 975 F.2d 832 (Fed. Cir. 1992) — tainted access killed fair use —
  [Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=975+F.2d+832) · [Justia](https://law.justia.com/cases/federal/appellate-courts/F2/975/832/)
- *NEC Corp. v. Intel Corp.*, 1989 WL 67434 (N.D. Cal. 1989) — seminal clean-room microcode —
  [Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=1989+WL+67434) · [Harvard JOLT analysis](https://jolt.law.harvard.edu/articles/pdf/v03/03HarvJLTech209.pdf) (no free full-text)
- *Google LLC v. Oracle America*, 593 U.S. 1 (2021) — interface reuse fair use (API copyrightability reserved) —
  [Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=593+U.S.+1) · [Cornell](https://www.law.cornell.edu/supremecourt/text/18-956)
- *Computer Associates v. Altai*, 982 F.2d 693 (2d Cir. 1992) — abstraction-filtration-comparison —
  [Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=982+F.2d+693) · [Justia](https://law.justia.com/cases/federal/appellate-courts/F2/982/693/)
- *Baker v. Selden*, 101 U.S. 99 (1879) —
  [Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=101+U.S.+99) · [Cornell](https://www.law.cornell.edu/supremecourt/text/101/99);
  *Lotus v. Borland*, 49 F.3d 807 (1st Cir. 1995), aff'd 4–4, 516 U.S. 233 (1996) — *binding 1st Cir. only; no national precedent* —
  [Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=49+F.3d+807) · [Justia](https://law.justia.com/cases/federal/appellate-courts/F3/49/807/)
- *Three Boys Music v. Bolton*, 212 F.3d 477 (9th Cir. 2000) — access standard —
  [Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=212+F.3d+477) · [Justia](https://law.justia.com/cases/federal/appellate-courts/F3/212/477/);
  inverse-ratio rule abrogated, *Skidmore v. Led Zeppelin*, 952 F.3d 1051 (9th Cir. 2020) —
  [Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=952+F.3d+1051) · [Justia](https://law.justia.com/cases/federal/appellate-courts/F3/952/1051/);
  originality, *Feist*, 499 U.S. 340 (1991) —
  [Westlaw](https://www.westlaw.com/Link/Document/FullText?cite=499+U.S.+340) · [Cornell](https://www.law.cornell.edu/supremecourt/text/499/340)

Law / facts — AI (UNSETTLED / empirical):
- **AI clean-room / independent creation (the "should it pass" question):** *Blameless Users in a Clean Room — Defining Copyright Protection for Generative Models* (clean-room counterfactual), [arXiv:2506.19881](https://arxiv.org/abs/2506.19881); near access-freeness — Vyas, Kakade & Barak (ICML 2023); **MALUS** "clean-room-as-a-service" — [Marks & Clerk](https://www.marks-clerk.com/insights/latest-insights/102mp7s-can-ai-legally-clone-open-source-unpacking-clean-room-as-a-service/) · [Futurism](https://futurism.com/artificial-intelligence/malus-clones-software-copyright).
- Memorization: Carlini et al. [arXiv:2012.07805](https://arxiv.org/abs/2012.07805) (USENIX 2021), [arXiv:2202.07646](https://arxiv.org/abs/2202.07646) (ICLR 2023); Nasr et al. [arXiv:2311.17035](https://arxiv.org/abs/2311.17035) (2023); reproduction/counter-nuance [arXiv:2412.06370](https://arxiv.org/abs/2412.06370) (2024, notes "Claude's Opus" baseline memorization).
- Litigation (pleading-stage, not merits): NYT v. OpenAI/Microsoft (MDL 1:25-md-03143); Doe v. GitHub (Copilot); Andersen v. Stability; Tremblay v. OpenAI; Kadrey v. Meta; **Bartz v. Anthropic** (N.D. Cal. 2025 — training fair use, pirated library NOT; output-filtering layer; settled $1.5B).
- Authorship: U.S. Copyright Office, 88 Fed. Reg. 16190 (Mar. 16, 2023); "Zarya of the Dawn" (2023); Thaler v. Perlmutter, 687 F. Supp. 3d 140 (D.D.C. 2023), aff'd No. 23-5233 (D.C. Cir. Mar. 18, 2025).

> **Citation honesty:** summarised by a non-lawyer from sources retrieved 2026-06-19.
> Some pin-cites (notably *Sega*, a few reporter pages) were high-confidence from
> retrieved text but not byte-verified against bound reporters; the *NYT v. OpenAI*
> opinion is image-based and corroborated via secondary sources. Counsel should
> verify before any public reliance.

---

## Appendix — what already exists to build on

- **Assembler (chosen):** WLA-DX `wla-spc700` (FOSS; install in Phase 0). Cross-check:
  `bass`.
- **Assembler (verified, not used):** `tools/spasm` SPC700 backend
  (`opcode70.asm`/`cam700.asm`, `m7.bat`). Ours; off the critical path (§2).
- **SPC700 visibility in MAME:** drmon's SPC700 register + `MTYPE_SPC` APU-RAM windows
  over the Lua bridge — [`docs/plans/2026-06-12-spc700-window.md`](2026-06-12-spc700-window.md).
- **MAME SNES bridge:** `task drmon-snes` / `mame_cpu_bridge.lua`.
