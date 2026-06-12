// Regression guard for the DOS 8.3 filename/path buffer-overflow fix.
//   docs/plans/2026-06-13-dos-83-buffer-overflow-fix.md
//
// WritePrivateProfileString() (profile.cpp) writes a tmpnam()-generated temp path into
// a local szTempName buffer.  That buffer was sized [13] for DOS 8.3 names, but glibc
// tmpnam() writes ~L_tmpnam (~20) bytes — a stack-buffer-overflow.  It is reachable
// in-app via the Settings menu -> Save (SaveConfig -> WritePrivateProfileInt -> ...String,
// config.cpp:395).  Built with AddressSanitizer this driver aborts (overflow inside
// tmpnam at profile.cpp) before the fix, and exits cleanly after szTempName is sized
// _MAX_PATH.  See linux/overflow-test.sh / `task overflow`.
#include <cstdio>
#include <pclib/general.hpp>
#include <pclib/profile.hpp>

int main(void)
{
	const char *ini = "/tmp/drmon_overflow_regression.ini";
	remove(ini);
	// The exact copy path SaveConfig() exercises.
	WritePrivateProfileString((char*)"drmon", (char*)"TestKey", (char*)"TestValue", (char*)ini);
	remove(ini);
	printf("overflow-test: WritePrivateProfileString completed, no buffer overflow\n");
	return 0;
}
