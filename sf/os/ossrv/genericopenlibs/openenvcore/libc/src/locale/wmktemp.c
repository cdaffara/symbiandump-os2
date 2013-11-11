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
* Description:  Implementation of wmktemp on Symbian
*
*/




#include <wchar.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

#ifndef __SYMBIAN32__
	static int _wmktemp(wchar_t *, int *, int, int);
#else
	static int _wmktemp(wchar_t *, int *, int, int, int);
#endif


static const wchar_t padchar[] =
L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

#ifndef __SYMBIAN32__
wchar_t* wmktemp(wchar_t *path)
#else//__SYMBIAN32__
wchar_t* wmktemp(wchar_t *path, int flag)
#endif//__SYMBIAN32__	
{
    #ifndef __SYMBIAN32__
	return (_wmktemp(path, (int *)NULL, 0, 0) ? path : (wchar_t *)NULL);
	#else//__SYMBIAN32__
	return (_wmktemp(path, (int *)NULL, 0, 0, flag) ? path : (wchar_t *)NULL);
	#endif//__SYMBIAN32__	
}


#ifdef __SYMBIAN32__
static int
_wmktemp(path, doopen, domkdir, slen, sflag)
	wchar_t *path;
	int *doopen;
	int domkdir;
	int slen;
	int sflag;
#else//__SYMBIAN32__
static int
_wmktemp(path, doopen, domkdir, slen)
	wchar_t *path;
	int *doopen;
	int domkdir;
	int slen;
#endif//__SYMBIAN32__
{
	wchar_t *start, *trv, *suffp;
	wchar_t *pad;
	struct stat sbuf;
	int rval;
	uint32_t rand;

	#ifdef __SYMBIAN32__
	int count;
	size_t len = wcslen(path);
	#endif //__SYMBIAN32__
	
	if (doopen != NULL && domkdir) {
		errno = EINVAL;
		return (0);
	}
	
#ifdef __SYMBIAN32__	
	// A single mkdir call introduced below so that tmpdir is created 
	if (sflag)
		{		
		wchar_t wtmpDir[L_tmpnam];
		size_t size;
		size = mbstowcs(wtmpDir,P_tmpdir,L_tmpnam);		
		if (wmkdir(wtmpDir, 0666) == -1)
			{
			if (errno != EEXIST)
				{
				return -1;		//return from here	
				}
			}
		}
#endif//__SYMBIAN32__
	for (trv = path; *trv != L'\0'; ++trv)
		;
	trv -= slen;
	suffp = trv;
	--trv;
	
	#ifdef __SYMBIAN32__
	if (trv < path || len < 6 || wcscmp(&path[len - 6], L"XXXXXX")) {
	#else
	if (trv < path) {
	#endif //__SYMBIAN32__
		errno = EINVAL;
		return (0);
	}
			
	/* Fill space with random characters */
	#ifndef __SYMBIAN32__
	while (trv >= path && *trv == L'X') {
	#else
	count = 0;
	while (trv >= path && count < 6) {
	#endif //__SYMBIAN32__
		rand = arc4random() % (sizeof(padchar) - 1);
		*trv-- = padchar[rand];
		#ifdef __SYMBIAN32__	
		count++;
		#endif //__SYMBIAN32__
	}
	start = trv + 1;

	/*
	 * check the target directory.
	 */
	if (doopen != NULL || domkdir) {
		for (; trv > path; --trv) {
			if (*trv == '/') {
				*trv = L'\0';
				rval = wstat(path, &sbuf);
				*trv = L'/';
				if (rval != 0)
					return (0);
				if (!S_ISDIR(sbuf.st_mode)) {
					errno = ENOTDIR;
					return (0);
				}
				break;
			}
		}
	}

	for (;;) {
		if (doopen) {
		#ifndef __SYMBIAN32__
			if ((*doopen =
			    wopen(path, O_CREAT|O_EXCL|O_RDWR, 0600)) >= 0)
		#else
			if ((*doopen =
			    wopen(path, O_CREAT|O_EXCL|O_RDWR|sflag, 0600)) >= 0)
		#endif//__SYMBIAN32__
				return (1);
			if (errno != EEXIST)
				return (0);
		} else if (domkdir) {
			if (wmkdir(path, 0700) == 0)
				return (1);
			if (errno != EEXIST)
				return (0);
		} else if (wstat(path, &sbuf))
			return (errno == ENOENT);

		/* If we have a collision, cycle through the space of filenames */
		for (trv = start;;) {
			if (*trv == L'\0' || trv == suffp)
				return (0);
			
			#ifdef __SYMBIAN32__
			pad = wcschr((const wchar_t *)padchar, *trv);
			#else
			pad = wcschr(padchar, *trv);
			#endif //__SYMBIAN32__
			
			if (pad == NULL || *++pad == L'\0')
				*trv++ = padchar[0];
			else {
				*trv++ = *pad;
				break;
			}
		}
	}	
}
