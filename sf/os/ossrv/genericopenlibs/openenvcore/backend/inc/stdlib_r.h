/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* @file
* @internalComponent
* Extension of <stdlib.h> with reentrant functions.
* 
*
*/




#ifndef _STDLIB_R_H_
#define _STDLIB_R_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "reent.h"

/*
EPOC32 malloc uses the thread heap, so it is already thread-safe
and no _malloc_r variants are needed.
*/
#define _malloc_r(x,n)		malloc(n)
#define _calloc_r(x,n,m)	calloc(n,m)
#define _realloc_r(x,p,n)	realloc(p,n)
#define _free_r(x,p)		free(p)

char*	_dtoa_r		(struct _reent *, double, int, int, int *, int*, char**);
void	_mstats_r	(struct _reent *, char *);
int	_system_r	(struct _reent *, const char *);

int	_rand_r		(struct _reent *);
void	_srand_r	(struct _reent *, unsigned);
int	_setenv_r	(struct _reent *, const char *, const char *, int);
void	_unsetenv_r	(struct _reent *, const char *);
char*	_getenv_r	(struct _reent *, const char *);
int	_wsetenv_r	(struct _reent *, const wchar_t *, const wchar_t *, int);
void	_wunsetenv_r	(struct _reent *, const wchar_t *);
wchar_t*	_wgetenv_r	(struct _reent *, const wchar_t *);
unsigned long _strtoul_r(struct _reent *,const char *, char **, int);

/*
It's possible to override exit() by supplying abort(), exit() and _exit()
The generic exit() and abort() routines look like

void exit(int code) _ATTRIBUTE((noreturn))
{
	_atexit_processing_r(_REENT);
 	_exit(code);
}
void abort(void) _ATTRIBUTE((noreturn))
{
	_exit(1);
}

which then allows your _exit() to capture all exits from ESTLIB, 
except for __assert() which calls abort().
*/
 void _atexit_processing_r (struct _reent *);

#ifdef __cplusplus
}
#endif
#endif /* _STDLIB_R_H_ */
