//=============================================================================
// spawn_window.cpp — launch an independent drmon session in a new xterm.
//
// SpawnNewWindow() reads the current binary path from /proc/self/exe, forks,
// and exec's xterm with the binary as the child command.  Each call cascades
// the window position by 80x25 pixels so windows don't stack exactly on top.
// A one-time SIGCHLD handler prevents zombie accumulation.
//=============================================================================
#include "spawn_window.hpp"

#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

static int g_windowCount = 0;

static void reap_children(int) {
    while (waitpid(-1, NULL, WNOHANG) > 0) {}
}

pid_t SpawnNewWindow() {
    char binary[1024];
    ssize_t len = readlink("/proc/self/exe", binary, sizeof(binary) - 1);
    if (len <= 0) return -1;
    binary[len] = '\0';

    g_windowCount++;

    char title[32];
    snprintf(title, sizeof(title), "drmon #%d", g_windowCount + 1);

    // +X+Y position offset — cascade each new window so they don't overlap
    char geom[32];
    snprintf(geom, sizeof(geom), "+%d+%d",
             g_windowCount * 80, g_windowCount * 25);

    // Install SIGCHLD handler once to reap exited child windows
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

    pid_t pid = fork();
    if (pid < 0) return -1;

    if (pid == 0) {
        setsid();  // detach from parent's process group
        execlp("xterm", "xterm",
               "-T",        title,
               "-geometry", geom,
               "-e",        binary,
               (char*)NULL);
        _exit(1);  // exec failed
    }

    return pid;
}
