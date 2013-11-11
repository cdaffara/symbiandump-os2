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
* WARNING: All identifiers here must begin with an underscore.  This file is
* included by stdio.h and others and we therefore must only use identifiers
* in the namespace allotted to us.
* 
*
*/



#ifndef _REENT_H_
#define _REENT_H_


#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include <time.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <wchar.h>
#include <pthread.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


#include <_ansi.h>
#include <sys/types.h>

/*
struct _reent

This structure contains *all* globals needed by the library.
It's raison d'etre is to facilitate threads by making all library routines
reentrant.  
Currently only error number, which is per thread basis and
pointer to CLocalSystemInterface is stored in _reent structure.
*/


#define _ASCTIME_SIZE	(26+8)	/* 26 min, plus caution factor! */
struct _reent
	{
	int _errno;			/* local copy of errno */
	void *_netdb;		/* used by gethostbyaddr and similar netdb functions */
	void *_memaddr;		/* To hold mapped memory address during mmap syscall */
	};

/*
This exported API is used for fetching reent structure for TLS
*/
struct _reent*	ImpurePtr	(void);

/*
Support for explicit release of all STDLIB resources belonging to this thread
*/
void CloseSTDLIB();

#define _REENT (ImpurePtr())
#define __errno_r(ptr) ((ptr)->_errno)

#ifdef __cplusplus
}
#endif

#endif /* _REENT_H_ */
