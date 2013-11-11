/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of wtmpnam on Symbian
*
*/




#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <wchar.h>
#include <stdlib.h>
#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif


#ifdef EMULATOR

GET_STATIC_ARRAY_FROM_TLS(wbuf, wchar_t)
GET_STATIC_VAR_FROM_TLS(wtmpcount, u_long)

#define wbuf (GET_WSD_VAR_NAME(wbuf, s)())
#define wtmpcount (*GET_WSD_VAR_NAME(wtmpcount, s)())
#endif //EMULATOR

#ifdef __SYMBIAN32__
extern wchar_t* wmktemp(wchar_t *, int);
#else//__SYMBIAN32__
extern wchar_t* wmktemp(wchar_t *);
#endif//__SYMBIAN32__

EXPORT_C
wchar_t* wtmpnam(wchar_t *s)	
{
#ifndef EMULATOR	
	static u_long wtmpcount;
	static wchar_t wbuf[L_tmpnam];
#endif //EMULATOR

wchar_t tmpDir[L_tmpnam];
size_t size;

	if (s == NULL)
			s = wbuf;
	
	size = mbstowcs(tmpDir,P_tmpdir,L_tmpnam);
#ifndef __SYMBIAN32__		
	if(!tmpDir)
#else//__SYMBIAN32__
	if(wcslen(tmpDir) == 0)
#endif//__SYMBIAN32__	
		return NULL;
#ifndef __SYMBIAN32__
	(void)swprintf(s,	L_tmpnam,	L"%lstmp.%lu.XXXXXX", tmpDir, wtmpcount);
#else
	(void)swprintf(s,	L_tmpnam,	L"%ls\\tmp.%lu.XXXXXX", tmpDir, wtmpcount);
#endif//__SYMBIAN32__	

	++wtmpcount;
#ifdef __SYMBIAN32__
	return (wmktemp(s, O_TMPFILE));
#else//__SYMBIAN32__
	return (wmktemp(s));
#endif//__SYMBIAN32__
}
