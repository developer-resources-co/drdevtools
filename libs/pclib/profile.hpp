
#ifndef LIB_PROFILE_H
#define LIB_PROFILE_H

// User Profile Routines
int GetPrivateProfileInt(char *, char *, int, char *);
int  GetPrivateProfileString(char *, char *, char *, char *, int, char *);
boolean WritePrivateProfileString(char *, char *, char *, char *);
boolean WritePrivateProfileInt(char *, char *, int nValue, char *);

// System .ini Routines
int GetProfileInt(char *, char *, int);
int  GetProfileString(char *, char *, char *, char *, int);
boolean WriteProfileString(char *, char *, char *);
boolean WriteProfileInt(char *, char *, int nValue);

#endif
