#!/usr/bin/env bash
# Reproducible Tier-1 verification: launch headless MAME + the 65816 gdbstub Lua, run the RSP
# client (test_65816.py), tear down. Retries on the rare MAME-0.277 SNES-init race (an
# intermittent SIGSEGV at boot, *before* the RSP socket is up — a MAME issue, not the plugin;
# under gdb the same launch runs indefinitely). A real assertion failure persists across
# retries and still exits non-zero.
#
# Usage: verify.sh            (ATTEMPTS=N to change retry budget; default 4)
set -euo pipefail

usage() { sed -n '2,9p' "$0"; exit 0; }
[[ "${1:-}" == "-h" || "${1:-}" == "--help" ]] && usage

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ATTEMPTS="${ATTEMPTS:-4}"
LOG=/tmp/mame-rsp.log

for i in $(seq 1 "$ATTEMPTS"); do
	fuser -k 2159/tcp 2>/dev/null || true
	"$HERE/run-mame.sh" > "$LOG" 2>&1 &
	mpid=$!
	rc=0
	python3 "$HERE/test_65816.py" || rc=$?
	kill "$mpid" 2>/dev/null || true
	wait "$mpid" 2>/dev/null || true
	if [ "$rc" -eq 0 ]; then
		echo "verify: PASS (attempt $i/$ATTEMPTS)"
		exit 0
	fi
	echo "verify: attempt $i/$ATTEMPTS failed (rc=$rc) — retrying (likely the MAME boot race)"
done
echo "verify: FAILED after $ATTEMPTS attempts"
exit 1
