/*============================================================================*/
/* slate.h:
/*
/* Author: Kevin T. Seghetti
/* Creation Date: 02-23-93 11:52am
/* Brief Description: This file has the header stuff for the slate screen
/*
/* Details:
/*
/*
/*
/* Dependent Upon:
/*	Text library, vdp library
/*
/* Dependents:
/*
/*
/* Change History:
/* Initials:		Date:
/* Description:
/* 	The slate screen is used to indicate to the publisher the current revision
/*	of the submission.
/*
/* Example Use:
/*
/*	TVersion ver;
/*	ver.release = 0;
/*	ver.milestone = 1;
/*	ver.internal = 1;
/*
/*	ShowSlateScreen("TecMagik","None","None",&ver,__DATE__);
/*
*/
/*============================================================================*/

#ifndef _SLATE_H_
#define _SLATE_H_

#include <alexdef.h>

/*============================================================================*/

typedef struct Version
{
	UWORD release;
	UWORD milestone;
	UWORD internal;
} TVersion;

/*============================================================================*/

UWORD
ShowSlateScreen(char *publisher, char *cartRequirments, char *comments, TVersion *ver, char *date);

/*============================================================================*/

#endif

/*============================================================================*/
