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
* Name        : wfdopen.c
* Part of     : LIBC
* Contains the source for wfdopen
* Version     : 1.0
*
*/



#include <sys/types.h>
#include <stdlib_r.h>
#include <stdio_r.h>
#include <errno.h>
#include "local.h"

EXPORT_C FILE * wfdopen (int fd, const wchar_t *mode)
	{
	char destmode[4];
	size_t siz;

	destmode[0] = '\0';
	if(mode==NULL)
    {
    	errno = EINVAL;
	    return (NULL);
    }
	siz = wcstombs(destmode, mode, 4);

	return fdopen(fd,destmode);
	}


