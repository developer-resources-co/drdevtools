//=============================================================================
// spawn_window.cpp — launch an independent drmon session in a new terminal.
//
// "Multiple monitors" = N independent top-level drmon processes, each in its own
// terminal window, each with its own MAME connection and sub-window state.
//
// Terminal-emulator resolution (works natively AND under `task run` in Docker):
//   1. $DRMON_TERMINAL — a launcher prefix INCLUDING its run-command flag, e.g.
//        DRMON_TERMINAL="gnome-terminal --"   "kitty"   "wezterm start --"
//        "konsole -e"   "xterm -e"
//      The drmon binary path (/proc/self/exe) is appended as the final argument.
//   2. else `x-terminal-emulator -e` — the system's *configured default* terminal
//      (Debian/Ubuntu update-alternatives; installing xterm registers it here).
//   3. else `xterm -e`.
//   4. none found  -> return -1 so the caller can tell the user to set $DRMON_TERMINAL.
//
// On a failed exec the parent learns the child's errno via a close-on-exec pipe
// and returns -1 (no more silent failure). A one-time SIGCHLD handler reaps the
// terminal processes so they never become zombies; setsid() detaches each child
// so closing one window can't signal another.
//=============================================================================
#include "spawn_window.hpp"

#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

static void reap_children(int) { while (waitpid(-1, NULL, WNOHANG) > 0) {} }

// Is `cmd` runnable — an explicit path, or found on $PATH?
static int in_path(const char *cmd) {
    if (strchr(cmd, '/')) return access(cmd, X_OK) == 0;
    const char *path = getenv("PATH");
    if (!path || !*path) path = "/usr/bin:/bin";
    char buf[1024];
    while (*path) {
        const char *colon = strchr(path, ':');
        size_t dlen = colon ? (size_t)(colon - path) : strlen(path);
        if (dlen && dlen + 1 + strlen(cmd) + 1 < sizeof buf) {
            memcpy(buf, path, dlen);
            buf[dlen] = '/';
            strcpy(buf + dlen + 1, cmd);
            if (access(buf, X_OK) == 0) return 1;
        }
        path = colon ? colon + 1 : path + dlen;
    }
    return 0;
}

pid_t SpawnNewWindow() {
    char binary[1024];
    ssize_t len = readlink("/proc/self/exe", binary, sizeof(binary) - 1);
    if (len <= 0) return -1;
    binary[len] = '\0';

    // Assemble argv: <terminal prefix...> <binary> NULL
    char *argv[40];
    int n = 0;
    static char termbuf[512];
    const char *envterm = getenv("DRMON_TERMINAL");
    if (envterm && *envterm) {
        snprintf(termbuf, sizeof termbuf, "%s", envterm);
        for (char *t = strtok(termbuf, " \t"); t && n < 36; t = strtok(NULL, " \t"))
            argv[n++] = t;
    } else if (in_path("x-terminal-emulator")) {
        argv[n++] = (char *)"x-terminal-emulator"; argv[n++] = (char *)"-e";
    } else if (in_path("xterm")) {
        argv[n++] = (char *)"xterm"; argv[n++] = (char *)"-e";
    } else {
        return -1;                          // no terminal available
    }
    if (n == 0) return -1;                  // empty DRMON_TERMINAL
    argv[n++] = binary;
    argv[n]   = NULL;

    // Reap exited terminal children (install handler once).
    static bool installed = false;
    if (!installed) {
        struct sigaction sa;
        memset(&sa, 0, sizeof sa);
        sigemptyset(&sa.sa_mask);
        sa.sa_handler = reap_children;
        sa.sa_flags   = SA_RESTART;
        sigaction(SIGCHLD, &sa, NULL);
        installed = true;
    }

    // Close-on-exec pipe: lets the parent distinguish a failed exec (child writes
    // errno, then _exit) from success (write end auto-closes on exec -> EOF).
    int pfd[2] = { -1, -1 };
    if (pipe2(pfd, O_CLOEXEC) != 0) { pfd[0] = pfd[1] = -1; }

    pid_t pid = fork();
    if (pid < 0) {
        if (pfd[0] >= 0) { close(pfd[0]); close(pfd[1]); }
        return -1;
    }
    if (pid == 0) {                         // child
        setsid();                           // detach from parent's session group
        execvp(argv[0], argv);
        int e = errno;                      // exec failed
        if (pfd[1] >= 0) { ssize_t w = write(pfd[1], &e, sizeof e); (void)w; }
        _exit(127);
    }

    // Parent: detect a failed exec.
    int failed = 0;
    if (pfd[0] >= 0) {
        close(pfd[1]);
        int e = 0;
        if (read(pfd[0], &e, sizeof e) > 0) failed = 1;
        close(pfd[0]);
    }
    return failed ? -1 : pid;
}
