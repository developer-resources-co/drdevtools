//=============================================================================
// error.h: error codes
//=============================================================================

// prevent library error handler from being used
#ifndef DRMON_error_H
#define DRMON_error_H
#define PCLIB_error_H


enum {

 NOERR,
 ERROR_NOMEM,
 ERROR_FILENOTFOUND,
 ERROR_NOTMACROFILE,
 ERROR_CANTSAVEFILE,
 ERROR_NOTSYMBOLFILE,
 ERROR_SYNTAX,
 ERROR_INTERNAL,
 ERROR_DIVZERO,
 ERROR_MISBALPAREN,
 ERROR_NOSUCHVAR,
 ERROR_INVALVAR,
 ERROR_NOSUCHWINDOW,
 ERROR_NODEFAULTWINDOW,
 ERROR_MISSINGPARAM,
 ERROR_WINDOWNOGROK,
 ERROR_PARAMNOTFOUND,
 ERROR_NOTSLDFILE,
 ERROR_BROKENSLDFILE,
 ERROR_NOSOURCEINFO,
 ERROR_STRINGNOTFOUND,
 ERROR_CANTOPENLOG,
 ERROR_SLAVEDEAD
 };

extern char *SYSERRORS[];

void
PrintError(int error);

typedef int errorcode;


#endif

//===========================================================================
