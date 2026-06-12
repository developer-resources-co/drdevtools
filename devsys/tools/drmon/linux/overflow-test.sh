#!/usr/bin/env bash
# Regression guard for the DOS 8.3 filename/path buffer-overflow fix
# (docs/plans/2026-06-13-dos-83-buffer-overflow-fix.md).
#
# Compiles the REAL profile.cpp + a tiny driver (linux/overflow-test.cpp) under
# AddressSanitizer and runs it.  WritePrivateProfileString() does tmpnam() into a
# szTempName buffer that was sized [13] for DOS 8.3 names; glibc tmpnam() writes
# ~L_tmpnam bytes and overflows it (ASan: stack-buffer-overflow in tmpnam).  Pre-fix
# this aborts; once szTempName is sized _MAX_PATH the run is clean.
#
# Runs INSIDE the toolchain container (needs g++ + ASan). Invoke via `task overflow`.
set -euo pipefail

usage() {
	echo "Usage: $0"
	echo "  Compiles profile.cpp + linux/overflow-test.cpp under ASan; exits 1 on buffer overflow."
}
[[ "${1:-}" == "-h" || "${1:-}" == "--help" ]] && { usage; exit 0; }

cd "$(dirname "$0")/.."          # devsys/tools/drmon
BIN="$(mktemp -u /tmp/drmon-overflow-test.XXXXXX)"

# Same flags as the real build (CMakeLists DRMON_COMPILE_OPTS) + ASan.
c++ -std=gnu++98 -w -fpermissive -fsanitize=address -fno-omit-frame-pointer -g \
	-Ilinux/include -I. -I../../../libs -include linux/linux_compat.hpp \
	profile.cpp linux/overflow-test.cpp -o "$BIN"

# abort_on_error=1 → an ASan-detected overflow makes the process exit non-zero.
rc=0
ASAN_OPTIONS=abort_on_error=1:halt_on_error=1:detect_leaks=0 "$BIN" || rc=$?
rm -f "$BIN"

if [[ "$rc" -eq 0 ]]; then
	echo "PASS: WritePrivateProfileString (szTempName/tmpnam) — no buffer overflow under ASan"
else
	echo "FAIL: AddressSanitizer flagged a buffer overflow (rc=$rc) — a DOS 8.3 buffer is undersized"
	exit 1
fi
