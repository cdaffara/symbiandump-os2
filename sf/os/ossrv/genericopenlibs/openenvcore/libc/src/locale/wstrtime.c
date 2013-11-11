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
* Name        : wstrtime.c
* Part of     : libc
* Interface   : 
* Implementation of wstrtime on Symbian
* Version     :
*
*/



#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <wchar.h>

EXPORT_C 
wchar_t* wstrtime(const wchar_t* timestr)
{
	time_t t;
	struct tm *dt;
	wchar_t *wcsdt;

	if ( timestr == NULL ) 
	{
		errno = EFAULT;
		return NULL;
	}
	wcsdt = (wchar_t *)timestr;
	
	t =  time(NULL);
	dt = localtime(&t);
	swprintf(wcsdt,20,L"%d:%d:%d",dt->tm_hour, dt->tm_min, dt->tm_sec);
	return wcsdt;
}
