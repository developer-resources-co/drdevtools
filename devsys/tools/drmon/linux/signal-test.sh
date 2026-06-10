#!/usr/bin/env bash
# Regression guard for drmon's Ctrl+C / signal handling (see the signal block in
# linux/ncurses_io.cpp). Asserts two things about a running drmon:
#
#   1. SIGINT (Ctrl+C) is IGNORED — drmon keeps running. This matches the MS-DOS
#      original, whose ctrlbrk(&CtrlBrkHander) returned 1 to resume the program;
#      the Linux port had dropped it, so Ctrl+C killed drmon and leaked the forced
#      xterm mouse modes (1003h/1006h) into the user's shell.
#   2. A fatal signal (SIGSEGV) RESTORES the terminal — the mouse-disable sequence
#      \033[?1003l is emitted before the process dies, so a crash never leaves the
#      shell leaking mouse reports. atexit() can't do this; a signal handler must.
#
# Runs INSIDE the toolchain container (needs `script` from util-linux to capture
# the raw escape-byte stream through a pty — ncurses requires a tty on stdout, so
# a plain pipe won't do). Invoke via `task signals`.
set -euo pipefail

usage() {
    echo "Usage: $0 [drmon-binary]   (default: ./drmon)"
    echo "  Verifies SIGINT is ignored and SIGSEGV restores the terminal; exits 1 on failure."
}
[[ "${1:-}" == "-h" || "${1:-}" == "--help" ]] && { usage; exit 0; }

BIN="${1:-./drmon}"
NAME="$(basename "$BIN")"
LOG="$(mktemp)"
export TERM=xterm-256color

DISABLE=$'\033[?1003l'      # mouse-tracking-OFF sequence the handler must emit
ENABLE=$'\033[?1003h'       # mouse-tracking-ON  sequence init emits (sanity check)

cleanup() { pkill -9 -x "$NAME" 2>/dev/null || true; rm -f "$LOG"; }
trap cleanup EXIT

# Launch drmon in a logged pty. -f flushes the log after every write so the
# assertions see bytes immediately; script's own stdout is discarded (the
# capture goes to $LOG regardless).
#
# A backgrounded command inherits SIG_IGN for SIGINT/SIGQUIT from the
# non-interactive shell (POSIX job control) — which would mask whether drmon
# itself ignores SIGINT (assertion 1). Reset them to default in the wrapper
# shell before exec'ing drmon, so the test measures drmon's own disposition,
# exactly as an interactive foreground `./drmon` would see it.
script -qfc "trap - INT QUIT; exec $BIN" "$LOG" >/dev/null 2>&1 &
sleep 2

pid="$(pgrep -n -x "$NAME" || true)"
[[ -n "$pid" ]] || { echo "FAIL: drmon did not start"; exit 1; }
grep -qF "$ENABLE" "$LOG" || { echo "FAIL: drmon never initialised the terminal (no 1003h)"; exit 1; }

# 1. SIGINT must be ignored (DOS ctrlbrk parity).
kill -INT "$pid"
sleep 1
if ! kill -0 "$pid" 2>/dev/null; then
    echo "FAIL: SIGINT killed drmon — it must be ignored (DOS ctrlbrk parity)"
    exit 1
fi

# 2. SIGSEGV must restore the terminal (emit the mouse-disable sequence).
kill -SEGV "$pid"
sleep 1
if ! grep -qF "$DISABLE" "$LOG"; then
    echo "FAIL: terminal not restored on SIGSEGV (no mouse-disable sequence in output)"
    exit 1
fi

echo "PASS: SIGINT ignored (drmon survived); SIGSEGV restored the terminal (mouse-disable emitted)"
