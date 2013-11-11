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
* Name        : fpathconf.c
* Part of     : libc library
* gets configurable system variables.
*
*/



#include <errno.h>
#include <limits.h> //_POSIX_XXXX_XXXX
#include <sys/unistd.h> //_PC_XXXX_XXXX

/*
 * fpathconf --
 * get configurable system variables.
 *

 */
EXPORT_C
long
fpathconf(int fd, int name)
{
	long ret = -1;
	#ifdef __SYMBIAN32__
	fd = fd; /*to fix the warning 'variable/argument not used in function' */
	#endif //__SYMBIAN32__
	switch(name) {
	case _PC_LINK_MAX:
		ret = _POSIX_LINK_MAX;
		break;
	case _PC_NAME_MAX:
		ret = _POSIX_NAME_MAX;
		break;
	case _PC_PATH_MAX:
		ret = _POSIX_PATH_MAX;
		break;
	case _PC_PIPE_BUF:
		ret = _POSIX_PIPE_BUF;
		break;
	case _PC_NO_TRUNC:
		ret = _POSIX_NO_TRUNC;
		break;
	case _PC_VDISABLE: 			//no concept of terminal
	case _PC_MAX_CANON:			//no concept of terminal
	case _PC_MAX_INPUT:			//no concept of terminal
	case _PC_CHOWN_RESTRICTED:	//no concept of ownership
		errno = ENOTSUP;
		break;
	default:
		errno = EINVAL;
		break;
	}
	return ret;
}
