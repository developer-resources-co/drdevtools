//===============================================================================
// version.cpp for untab
// this is a sample file for use with BumpVer
// the file can be of any length, and contain any text
// BumpVer will only attempt to update the file if
// the signature string is found.
// The signature string should be close to the actual version string in the
// file, BumpVer will scan forward from the delimeter character, looking for
// the first occurence of that character. Once found, BumpVer will assume
// the version # immediatly follows. The version # must be in the format
// "#.#.#" where " is the delimeter. Note the delimeter must end the string
// as well as begin it. You may use any character for the delimter.
//===============================================================================

#include "local.h"

// BumpVer signature string, delimiter="
TEXT szVersion[] = "1.0.11";

// other stuff
TEXT szDate[] = __DATE__;
TEXT szTime[] = __TIME__;

//===============================================================================
