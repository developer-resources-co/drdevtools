
#ifndef _DEBUG_H_
#define _DEBUG_H_

/*
 *  C header file for debug.asm library
 */

void Crash( char* );

void PrintRegs(void);


void VdpView(void);

void CrashCallback(void (*)(void));

#endif
