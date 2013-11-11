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
* The only thing we could fail on is write access to a read-only file
* 
*
*/



#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

EXPORT_C int access(const char *fn, int flags)
	{
	struct stat s;
	if (stat(fn, &s))
		return -1;
	if (s.st_mode & S_IFDIR)
		return 0;
	if ((flags&W_OK)&&(s.st_mode&S_IWRITE)==0)
		return -1;
	return 0;
	}

EXPORT_C int waccess(const wchar_t *fn, int flags)
	{
	struct stat s;
	if (wstat(fn, &s))
		return -1;
	if (s.st_mode & S_IFDIR)
		return 0;
	if ((flags&W_OK)&&(s.st_mode&S_IWRITE)==0)
		return -1;
	return 0;
	}
