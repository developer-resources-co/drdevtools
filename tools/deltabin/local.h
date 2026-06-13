
#if !defined(LOCAL_H)
#define LOCAL_H

// Target system stuff
#ifdef SYSTEMGEN
#define GENESIS
#endif

#ifdef SYSTEMSNES
#define SNES
#endif

#ifdef SNES
#define PLATFORMNAME "SNES"
#endif

#ifdef GENESIS
#define PLATFORMNAME "Genesis"
#endif


//
typedef unsigned char TEXT;
typedef TEXT *TEXTSTR;

#endif

