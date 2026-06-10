// Shim <bios.h> — Borland BIOS services. Only _bios_keybrd is used; stubbed.
#ifndef DRMON_SHIM_BIOS_H
#define DRMON_SHIM_BIOS_H

#define _KEYBRD_READY        1
#define _KEYBRD_KEYINFO      2
#define _KEYBRD_SHIFTSTATUS  3
#define _NKEYBRD_READY       0x11
#define _NKEYBRD_KEYINFO     0x10
#define _NKEYBRD_SHIFTSTATUS 0x12

#ifdef __cplusplus
extern "C" {
#endif

int _bios_keybrd(int cmd);

#ifdef __cplusplus
}
#endif
#endif
