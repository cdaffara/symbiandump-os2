// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// 
//

#include <unistd.h>
#include <errno.h>
#include "sysreent.h"
#include <dirent.h>
#include <utf.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <sys/errno.h>
#include "sysif.h"

#define	MAXPATHLEN	260	/* E32STD.H: KMaxFullName + 4 to avoid data loss */

extern "C" {
/*
Opens a directory.
*/
EXPORT_C DIR* opendir(const char* _path)
	{
	wchar_t _wpath[MAXPATHLEN+1];
	
	if (mbstowcs(_wpath, _path, MAXPATHLEN) == (size_t)-1)
		{
		errno = EILSEQ;
		return 0;
		}
	return _opendir_r(&errno, _wpath);
	}

/* 
A wide-character version of opendir()
*/
EXPORT_C WDIR* wopendir(const wchar_t* _wpath)
	{
	if (!_wpath)
		{
		errno = ENOENT;
		return NULL;
		}
	return (WDIR*)_opendir_r(&errno, _wpath);
	}

EXPORT_C struct dirent* readdir(DIR* dp)
	{
	return _readdir_r(&errno, dp);
	}

EXPORT_C struct wdirent* wreaddir(WDIR* wdp)
	{
	if (!wdp)
		{
		errno = EBADF;
		return NULL;
		}
	return _wreaddir_r(wdp);
	}

/*
Sets the position (associated with the directory stream that dirp points to) 
to the beginning of the directory. 
Additionally, it causes the directory stream to refer to the current state of 
the corresponding directory.
*/
EXPORT_C void wrewinddir(WDIR* wdp)
	{
	_wrewinddir_r(wdp);
	}

EXPORT_C void rewinddir(DIR* dp)
	{
	WDIR* wdp = (__EPOC32_DIR*)dp;
	wrewinddir(wdp);
	}

/*
closes the directory stream that dirp refers to. 
The memory associated with the directory stream is released. 
When this function returns, the value of dirp no longer point to 
an accessible object of type DIR
*/

EXPORT_C int wclosedir(WDIR* wdp)
	{
	return _wclosedir_r(&errno,wdp);
	}

EXPORT_C int closedir(DIR* dp)
	{
	if (!dp)
		{
		errno = EBADF;
		return -1;
		}
		
	WDIR* wdp =  (__EPOC32_DIR*)dp;
	return wclosedir(wdp);
	}

/*
sets the position of the next readdir() operation on the directory 
stream specified by dp to the position specified by index.
*/

EXPORT_C void wseekdir(WDIR* wdp, off_t index)
	{
	if(!wdp)
		{
	 	errno = EFAULT ;
		return ;	
		}
	wdp->iIndex = index;
	}

EXPORT_C void seekdir(DIR* dp, long index)
	{
    WDIR* wdp = (__EPOC32_DIR*)dp;
	wseekdir(wdp, index);
	}


EXPORT_C off_t wtelldir(const WDIR* wdp)
	{
	if(!wdp)
		{
	 	errno = EFAULT ;
		return -1 ;	
		}
	if(wdp->iCount < 0 )
		{
		return -1;
		}
	return wdp->iIndex;
	}

/*
Returns the current location associated with the directory stream dir.
*/
EXPORT_C long telldir(DIR* dp)
	{
	WDIR* wdp =  (__EPOC32_DIR*)dp;
	return (long)wtelldir(wdp);
	}

}// extern "C"
