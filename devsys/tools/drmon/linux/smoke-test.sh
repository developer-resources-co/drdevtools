#!/usr/bin/env bash
# Smoke test: open each drmon window via its Alt-key accelerator under gdb and
# assert none of them segfault.
#
# Guards the read-only-string-literal crash class (a DOS->Linux port bug):
# string gadgets are edited IN PLACE — PadString() and the key handlers write
# directly into gPtr->gadgText, padding out to xSize — so gadgText must point at
# writable storage. The old DOS idiom `gPtr->gadgText = "literal"` worked because
# those compilers put literals in writable memory; on modern toolchains literals
# live in read-only .rodata, so the first PadString / keystroke segfaults.
# Reported 2026-06-10: Alt+E (Expression window) crashed with SIGSEGV.
# See docs/plans/2026-06-10-port-drmon-linux.md.
#
# Runs INSIDE the toolchain container (needs gdb + tmux). Invoke via `task smoke`.
set -euo pipefail

usage() {
	echo "Usage: $0 [drmon-binary]"
	echo "  Opens each window via its Alt-key under gdb in tmux; exits 1 on SIGSEGV."
	echo "  drmon-binary defaults to ./drmon"
}
[[ "${1:-}" == "-h" || "${1:-}" == "--help" ]] && { usage; exit 0; }

BIN="${1:-./drmon}"
LOG="$(mktemp)"
SESSION="drmon-smoke"
export TERM=xterm-256color

# Alt-keys that open windows. The bug class lives in windows with string gadgets
# (Expression, Command, file requester), but we open the full set so any gadget
# with read-only text is caught.  E=Expr K=Cmd R=Reg M=Mem W=Watch B=Break
# S=Symbol I=ProjInfo O=Console A=About Y=AsciiChart
KEYS=(M-e M-k M-r M-m M-w M-b M-s M-i M-o M-a M-y)

tmux kill-session -t "$SESSION" 2>/dev/null || true
tmux new-session -d -s "$SESSION" -x 80 -y 25
tmux send-keys -t "$SESSION" \
	"gdb -q -batch -ex 'set logging file $LOG' -ex 'set logging overwrite on' -ex 'set logging on' -ex run -ex bt '$BIN' 2>/dev/null" Enter
sleep 3

for combo in "${KEYS[@]}"; do
	tmux send-keys -t "$SESSION" "$combo"; sleep 0.6
	tmux send-keys -t "$SESSION" Escape;  sleep 0.3
done

# Exercise the in-place edit path: open Expression and type into the gadget,
# which writes through gadgText exactly like PadString does.
tmux send-keys -t "$SESSION" M-e;       sleep 0.6
tmux send-keys -t "$SESSION" "12+34";   sleep 0.6

tmux kill-session -t "$SESSION" 2>/dev/null || true

if grep -q SIGSEGV "$LOG" 2>/dev/null; then
	echo "FAIL: drmon crashed (SIGSEGV) while opening a window"
	echo "----- gdb backtrace -----"
	cat "$LOG"
	rm -f "$LOG"
	exit 1
fi
rm -f "$LOG"
echo "PASS: opened windows via Alt-keys (${KEYS[*]}) + typed into Expression — no SIGSEGV"
