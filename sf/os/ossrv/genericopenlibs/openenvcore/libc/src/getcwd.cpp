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
* Description:
*/
/*
* ==============================================================================
*  Name        : getcwd.cpp
*  Part of     : libc library
*  Description : gets current working directory.
*
* ============================================================================
*/

#include "sysif.h"
#include <sys/errno.h> //error codes (ENOMEM, EINVAL)
#include "sysreent.h"
#include <string.h> //strlen
#include <stdlib.h>

extern "C" {

/*
Gets the path name of the current working directory.
If a buffer is specified, the path name is placed in that buffer,
and the address of the buffer is returned. 
*/
EXPORT_C char* getcwd (char *_buf, size_t _size)
	{
	
        if (_buf != 0 && _size == 0)
		{
		errno = EINVAL;
		return NULL;
		}
	
	char * _ourbuf = _buf;	
	if (_buf==0)
		{
		if (_size >= (KMaxTInt/2))
			{
			errno=ENOMEM;
			return _buf;
			}
		//coverity[alloc_fn]
		//coverity[assign]
		_ourbuf = (char *)	malloc(_size);
		if (_ourbuf == 0)
			{
			errno=ENOMEM;
			return _buf;
			}
		}

   //we are dealing with wide characters from here so we need a temporary buffer
	wchar_t tmpbuf[KMaxFullName];
	//coverity[leave_without_push]
	wchar_t *cret = _getcwd_r(&errno, tmpbuf, _size);

	if (cret)	//we have a path
		{
		//convert it to UTF8
		size_t x = wcstombs(_ourbuf, tmpbuf, _size);	//convert the buffer
		
		//remove the trailing backslash
              int cwd_len = strlen(_ourbuf);
		if(_ourbuf[cwd_len - 1] == '\\')
			_ourbuf[cwd_len - 1] = '\0';
		
		return _ourbuf;
		}

	//deal with the fact we may have allocated our own buffer
	if (_buf != _ourbuf)  //we allocated it.
		{
			free(_ourbuf);
       	}
       	
	return NULL;
	}

} // extern "C"
