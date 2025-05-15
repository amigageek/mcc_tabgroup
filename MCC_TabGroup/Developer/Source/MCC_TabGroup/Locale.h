#ifndef LOCALE_H
#define LOCALE_H 1

/*
** Locale.h
**
** (c) 2006 by Guido Mersmann
**
** Object source created by SimpleCat
*/

/*************************************************************************/

const char CatCompBlock[];

#include "LocaleStrings.h"

/*
** Prototypes
*/

BOOL   Locale_Open( STRPTR catname, ULONG version, ULONG revision);
void   Locale_Close(void);
STRPTR GetString(long ID);

/*************************************************************************/

#endif /* LOCALE_H */
