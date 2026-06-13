#!/usr/bin/env python3
"""
Tier 3 symbol-loading verifications for drmon-dap.
Runs drmon-dap-snes via stdin/stdout (no MAME required for these checks).
"""

import json
import struct
import subprocess
import sys
import tempfile
import os

# ---------------------------------------------------------------------------
# DAP framing helpers
# ---------------------------------------------------------------------------

def encode(msg: dict) -> bytes:
    body = json.dumps(msg).encode()
    return f"Content-Length: {len(body)}\r\n\r\n".encode() + body

def send(proc, msg):
    proc.stdin.write(encode(msg))
    proc.stdin.flush()

def recv_one(proc) -> dict:
    hdr = b""
    while b"\r\n\r\n" not in hdr:
        ch = proc.stdout.read(1)
        if not ch:
            raise EOFError("process closed stdout")
        hdr += ch
    length = int(hdr.split(b"Content-Length:")[1].split(b"\r\n")[0].strip())
    body = proc.stdout.read(length)
    return json.loads(body)

def recv(proc) -> dict:
    """Read the next response message, discarding any events that arrive first."""
    while True:
        msg = recv_one(proc)
        if msg.get("type") == "response":
            return msg
        # event — discard and keep reading

# ---------------------------------------------------------------------------
# Synthetic test file builders
# ---------------------------------------------------------------------------

def make_sld(path: str):
    """Minimal binary .sld: file 'test.asm', line 10 → 0x808000, line 20 → 0x808010."""
    hdr = b"Source level debugging file" + b"\x00" * (256 - 27)  # 256 bytes total
    # 0x01 filename record: "test.asm" + 0xFF 0xFF terminator
    rec_file = b"\x01" + b"test.asm" + b"\xff\xff"
    # 0x02 line records: addr is little-endian (lo, hi, bank)
    # 0x808000: lo=0x00, hi=0x80, bank=0x80; line 10 (lo=10, hi=0)
    rec_line10 = b"\x02" + bytes([0x00, 0x80, 0x80, 10, 0])
    # 0x808010: lo=0x10, hi=0x80, bank=0x80; line 20
    rec_line20 = b"\x02" + bytes([0x10, 0x80, 0x80, 20, 0])
    with open(path, "wb") as f:
        f.write(hdr + rec_file + rec_line10 + rec_line20)

def make_coff(path: str):
    """Minimal Sierra COFF: one C_LABEL symbol 'RESET' at address 0x808000."""
    # FILHDR (20 bytes, big-endian): magic f_nscns f_timdat f_symptr f_nsyms f_opthdr f_flags
    filhdr = struct.pack(">HHiiiHH", 0x0150, 0, 0, 20, 1, 0, 0)  # H+H+i+i+i+H+H = 20
    # SYMENT (18 bytes): inline name "RESET\0\0\0", value=0x808000, scnum=1, type=0, sclass=6, numaux=0
    name = b"RESET\x00\x00\x00"   # 8 bytes
    n_value = struct.pack(">I", 0x808000)
    n_scnum = struct.pack(">h", 1)
    n_type  = struct.pack(">H", 0)
    n_sclass = bytes([6])   # C_LABEL
    n_numaux = bytes([0])
    syment = name + n_value + n_scnum + n_type + n_sclass + n_numaux   # 18 bytes
    # String table: just the 4-byte length (=4, no strings needed for inline names)
    strtab = struct.pack(">I", 4)
    with open(path, "wb") as f:
        f.write(filhdr + syment + strtab)

def make_ca65_dbg(path: str):
    """Minimal ca65 .dbg (cc65 text format): label 'reset' @ 0x808000;
    main.s line 10 -> 0x808000, line 20 -> 0x808010 (seg start + span offset)."""
    lines = [
        "version\tmajor=2,minor=0",
        'file\tid=0,name="main.s",size=100,mtime=0x00000000,mod=0',
        'seg\tid=0,name="CODE",start=0x808000,size=0x0020,addrsize=far,type=ro,oname="x.bin",ooffs=0',
        "span\tid=0,seg=0,start=0,size=16",
        "span\tid=1,seg=0,start=16,size=16",
        "line\tid=0,file=0,line=10,span=0",
        "line\tid=1,file=0,line=20,span=1",
        'sym\tid=0,name="reset",addrsize=absolute,size=1,scope=0,def=0,val=0x808000,seg=0,type=lab',
    ]
    with open(path, "w") as f:
        f.write("\n".join(lines) + "\n")

def make_wla_sym(path: str):
    """Minimal WLA-DX .sym (v2 sections): label 'reset' @ 0x808000;
    main.s line 10 (0x0a) -> 0x808000, line 20 (0x14) -> 0x808010.
    addr-to-line v2 line = 'OFFS  ROMBANK:OFFS  CPUADDR  FILEHI:FILELO:LINE'."""
    text = (
        "; wla symbolic information for test\n"
        "\n"
        "[labels]\n"
        "80:8000 reset\n"
        "\n"
        "[source files v2]\n"
        "0001:0001 00000000 main.s\n"
        "\n"
        "[addr-to-line mapping v2]\n"
        "00000000 80:0000 8000 0001:0001:0000000a\n"
        "00000010 80:0010 8010 0001:0001:00000014\n"
    )
    with open(path, "w") as f:
        f.write(text)

# ---------------------------------------------------------------------------
# Test runner
# ---------------------------------------------------------------------------

BINARY = os.environ.get("DRMON_DAP_SNES", "/build/drmon-dap-snes")

def run_test(name: str, args: list, steps):
    proc = subprocess.Popen(
        [BINARY] + args,
        stdin=subprocess.PIPE, stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
    )
    try:
        seq = [1]

        def req(method, params=None):
            msg = {"seq": seq[0], "type": "request", "command": method}
            if params:
                msg["arguments"] = params
            seq[0] += 1
            send(proc, msg)
            return recv(proc)

        # initialize
        init_resp = req("initialize", {"clientID": "test", "adapterID": "drmon"})
        assert init_resp["success"], f"initialize failed: {init_resp}"

        steps(req, init_resp)
        print(f"PASS  {name}")
    except Exception as e:
        print(f"FAIL  {name}: {e}")
        raise
    finally:
        proc.terminate()
        proc.wait()

def main():
    failures = []

    # ------------------------------------------------------------------
    # Verification 1: Build (already done; here we just confirm binary exists)
    # ------------------------------------------------------------------
    if not os.path.exists(BINARY):
        print(f"FAIL  build: {BINARY} not found")
        sys.exit(1)
    print(f"PASS  build: {BINARY} exists")

    with tempfile.TemporaryDirectory() as tmpdir:
        sld_path  = os.path.join(tmpdir, "test.sld")
        coff_path = os.path.join(tmpdir, "test.cof")
        ca65_path = os.path.join(tmpdir, "test.dbg")
        wla_path  = os.path.join(tmpdir, "test.sym")
        junk_path = os.path.join(tmpdir, "test.junk")
        make_sld(sld_path)
        make_coff(coff_path)
        make_ca65_dbg(ca65_path)
        make_wla_sym(wla_path)
        with open(junk_path, "wb") as f:
            f.write(b"this is not a symbol file\x00\x01\x02\n")

        # ------------------------------------------------------------------
        # Verification 2: evaluate symbol (COFF)
        # ------------------------------------------------------------------
        def check_eval_symbol(req, init_resp):
            resp = req("evaluate", {"expression": "RESET", "context": "repl"})
            assert resp["success"], f"evaluate failed: {resp}"
            result = resp["body"]["result"]
            assert result == "0x808000", f"expected 0x808000, got {result!r}"

        try:
            run_test("evaluate symbol (COFF)", ["--symbols", coff_path], check_eval_symbol)
        except Exception as e:
            failures.append(f"evaluate symbol: {e}")

        # ------------------------------------------------------------------
        # Verification 3: disassembly labels (COFF)
        # ------------------------------------------------------------------
        def check_disasm_labels(req, init_resp):
            resp = req("disassemble", {
                "memoryReference": "0x808000",
                "instructionOffset": 0,
                "instructionCount": 3,
            })
            assert resp["success"], f"disassemble failed: {resp}"
            insns = resp["body"]["instructions"]
            assert insns, "no instructions returned"
            first = insns[0]
            assert first.get("address") == "0x808000", f"unexpected address: {first.get('address')!r}"
            sym = first.get("symbol")
            assert sym == "RESET", f"expected symbol='RESET', got {sym!r}"

        try:
            run_test("disassembly labels (COFF)", ["--symbols", coff_path], check_disasm_labels)
        except Exception as e:
            failures.append(f"disassembly labels: {e}")

        # ------------------------------------------------------------------
        # Verification 4: source breakpoint smoke (SLD)
        # ------------------------------------------------------------------
        def check_src_bpt(req, init_resp):
            resp = req("setBreakpoints", {
                "source": {"path": "test.asm"},
                "breakpoints": [{"line": 10}, {"line": 20}],
            })
            assert resp["success"], f"setBreakpoints failed: {resp}"
            bpts = resp["body"]["breakpoints"]
            assert len(bpts) == 2, f"expected 2 breakpoints, got {len(bpts)}"
            assert bpts[0]["verified"] is True, f"bpt[0] not verified: {bpts[0]}"
            assert bpts[0].get("instructionReference") == "0x808000", \
                f"wrong ref: {bpts[0].get('instructionReference')!r}"
            assert bpts[1]["verified"] is True, f"bpt[1] not verified: {bpts[1]}"
            assert bpts[1].get("instructionReference") == "0x808010", \
                f"wrong ref: {bpts[1].get('instructionReference')!r}"

        try:
            run_test("source breakpoint smoke (SLD)", ["--symbols", sld_path], check_src_bpt)
        except Exception as e:
            failures.append(f"source breakpoint: {e}")

        # ------------------------------------------------------------------
        # Verification 4b: modern text importers (ca65 .dbg, WLA-DX .sym)
        # Both carry labels AND source-line info; each is exercised for
        # evaluate (label), disassembly annotation, and source breakpoints —
        # the same surface the COFF/SLD cases above cover, proving the shared
        # symfmt parser feeds byAddr_/byName_/srcMap_ for these formats too.
        # ------------------------------------------------------------------
        def make_eval_check(symbol, addr):
            def chk(req, init_resp):
                resp = req("evaluate", {"expression": symbol, "context": "repl"})
                assert resp["success"], f"evaluate failed: {resp}"
                result = resp["body"]["result"]
                assert result == addr, f"expected {addr}, got {result!r}"
            return chk

        def make_disasm_check(symbol, addr):
            def chk(req, init_resp):
                resp = req("disassemble", {
                    "memoryReference": addr, "instructionOffset": 0, "instructionCount": 1,
                })
                assert resp["success"], f"disassemble failed: {resp}"
                insns = resp["body"]["instructions"]
                assert insns, "no instructions returned"
                assert insns[0].get("symbol") == symbol, \
                    f"expected symbol={symbol!r}, got {insns[0].get('symbol')!r}"
            return chk

        def make_srcbp_check(src):
            def chk(req, init_resp):
                resp = req("setBreakpoints", {
                    "source": {"path": src},
                    "breakpoints": [{"line": 10}, {"line": 20}],
                })
                assert resp["success"], f"setBreakpoints failed: {resp}"
                bpts = resp["body"]["breakpoints"]
                assert len(bpts) == 2, f"expected 2 breakpoints, got {len(bpts)}"
                assert bpts[0]["verified"] and bpts[0].get("instructionReference") == "0x808000", \
                    f"bpt[0]: {bpts[0]}"
                assert bpts[1]["verified"] and bpts[1].get("instructionReference") == "0x808010", \
                    f"bpt[1]: {bpts[1]}"
            return chk

        for fmt, path in (("ca65", ca65_path), ("WLA", wla_path)):
            for desc, chk in (
                ("evaluate symbol",   make_eval_check("reset", "0x808000")),
                ("disassembly label", make_disasm_check("reset", "0x808000")),
                ("source breakpoint", make_srcbp_check("main.s")),
            ):
                try:
                    run_test(f"{desc} ({fmt})", ["--symbols", path], chk)
                except Exception as e:
                    failures.append(f"{desc} ({fmt}): {e}")

        # ------------------------------------------------------------------
        # Verification 4c: a non-symbol file is rejected by every loader
        # (the loadSld||loadCoff||loadCa65Dbg||loadWlaSym chain falls through)
        # — no crash, no bogus symbols.
        # ------------------------------------------------------------------
        def check_junk_fallthrough(req, init_resp):
            resp = req("evaluate", {"expression": "reset", "context": "repl"})
            assert resp["success"], f"evaluate failed: {resp}"
            assert "unknown" in resp["body"]["result"].lower(), \
                f"junk file should load no symbols, got {resp['body']['result']!r}"

        try:
            run_test("non-symbol file falls through", ["--symbols", junk_path], check_junk_fallthrough)
        except Exception as e:
            failures.append(f"junk fallthrough: {e}")

        # ------------------------------------------------------------------
        # Verification 5: no-symbols graceful
        # ------------------------------------------------------------------
        def check_no_symbols(req, init_resp):
            # evaluate a register name — should still work
            resp = req("evaluate", {"expression": "PC", "context": "repl"})
            assert resp["success"], f"evaluate PC failed: {resp}"
            assert resp["body"]["result"] != "", "empty result for PC"

            # evaluate an unknown expression — should say 'unknown' not crash
            resp2 = req("evaluate", {"expression": "UNKNOWN_SYM", "context": "repl"})
            assert resp2["success"], f"evaluate UNKNOWN_SYM failed: {resp2}"
            assert "unknown" in resp2["body"]["result"].lower(), \
                f"expected 'unknown', got {resp2['body']['result']!r}"

            # setBreakpoints without symbols — should return verified=false with helpful message
            resp3 = req("setBreakpoints", {
                "source": {"path": "test.asm"},
                "breakpoints": [{"line": 10}],
            })
            assert resp3["success"], f"setBreakpoints failed: {resp3}"
            bpts3 = resp3["body"]["breakpoints"]
            assert bpts3[0]["verified"] is False, f"expected unverified: {bpts3[0]}"
            assert "symbol" in bpts3[0].get("message", "").lower() or \
                   "--symbols" in bpts3[0].get("message", ""), \
                f"expected helpful message, got: {bpts3[0].get('message')!r}"

        try:
            run_test("no-symbols graceful", [], check_no_symbols)
        except Exception as e:
            failures.append(f"no-symbols graceful: {e}")

    if failures:
        print(f"\n{len(failures)} failure(s):")
        for f in failures:
            print(f"  {f}")
        sys.exit(1)
    else:
        print("\nAll Tier 3 symbol verifications passed.")


if __name__ == "__main__":
    main()
