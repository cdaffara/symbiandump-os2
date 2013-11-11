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
// Name        : wremove.cpp
// Part of     : LIBC
// Contains the source for wremove
// Version     : 1.0
//



#include <wchar.h>
#include<e32const.h>
#include <errno.h>

extern "C"
{

EXPORT_C int
wremove(const wchar_t *file)	
	{
	struct stat sb;
	if(file)
	{
	if (wstat(file, &sb) < 0)
		return (-1);
	if (S_ISDIR(sb.st_mode))
		return (wrmdir(file));
	return (wunlink(file));
	}
	errno = EFAULT;
	return -1;
	}
	
}//extern "C"



