#pragma once
#include <sys/types.h>

// Spawn an independent drmon session in a new terminal window.
// Terminal chosen via $DRMON_TERMINAL, else x-terminal-emulator, else xterm.
// Returns the child terminal PID, or -1 on failure (no terminal / exec failed).
pid_t SpawnNewWindow();
