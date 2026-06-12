#pragma once
#include <sys/types.h>

// Spawn an independent drmon session in a new xterm window.
// Returns the child xterm PID, or -1 on failure.
pid_t SpawnNewWindow();
