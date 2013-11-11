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
* Description:  Contains the source for getws and putws
 *
*/


#include<e32const.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

extern "C"
{
	
EXPORT_C wchar_t* getws(wchar_t* str)
	{
	size_t size;
	wchar_t* res = NULL;
	
	if(!str)
		{
		errno = EFAULT;
		return NULL;
		}
	res = fgetws(str, BUFSIZ, stdin);
	if(res)
	{
	size = wcslen (str);
	if( (str[size-1] == L'\n') && (res[size-1] == L'\n') )
		{
		str[size-1] = L'\0';
		res[size-1] = L'\0';
		}
	}
	return res;
	}

EXPORT_C int putws(wchar_t* str)
	{
	if(!str)
		{
		errno = EFAULT;
		return -1;
		}
	return (fputws(str, stdout));
	}
 
}//extern "C"


