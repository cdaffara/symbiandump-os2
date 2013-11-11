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
* Name        : wstrdate.c
* Part of     : libc
* Interface   : 
* Implementation of wstrdate on Symbian
* Version     :
*
*/


 
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <wchar.h>

EXPORT_C
wchar_t* wstrdate( const wchar_t *datestr )
{

	time_t t;
	struct tm *dt;
	wchar_t *wcsdt;

	if ( datestr == NULL )
	{
		errno = EFAULT;
		return NULL;
	}
	wcsdt = (wchar_t *)datestr;
	t =  time(NULL);
	dt = localtime(&t);
	//tm_mday - day of the month [1-31]
	//tm_mon - months since January [0-11]
	//tm_year - years since 1900
	swprintf(wcsdt,20,L"%d/%d/%d",dt->tm_mday,dt->tm_mon+1,dt->tm_year+1900); 
	return wcsdt;
}
