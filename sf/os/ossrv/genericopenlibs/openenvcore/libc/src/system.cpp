// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : system.cpp
// Part of     : libc library
// executes a given command/executable.
//



#include <stdlib_r.h> //mbstowcs
#include "sysreent.h" //_wsystem_r
#include <errno.h>
#include <string.h>

extern "C" {

#define	MAXPATHLEN	260	/* E32STD.H: KMaxFullName + 4 to avoid data loss */

int _system_r (struct _reent *r, const char* cmd);

/*
Execute command.
*/

EXPORT_C int system (const char* cmd)
	{
	return _system_r (_REENT,cmd);
	}

/*
A reentrant version of system().
*/
int _system_r (struct _reent *r, const char* cmd)
	{
	wchar_t wcmd[MAXPATHLEN+1];
	
	if (cmd==0)
		{
		return 1;	// special case, says that we do support system().	
		}

	if(strlen(cmd) > MAXPATHLEN)
		{
		errno = ENAMETOOLONG;
		return -1;	
		}
		
	if (((size_t)-1 != mbstowcs(wcmd, cmd, MAXPATHLEN)))
		return _wsystem_r(&r->_errno, wcmd);
		r->_errno = EILSEQ;
	return -1;
	}

} // extern "C"
