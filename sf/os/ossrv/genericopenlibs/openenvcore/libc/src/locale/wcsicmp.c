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
* Name        : wcsicmp.c
* Part of     : libc
* Interface   : 
* Implementation of wcsicmp on Symbian
* Version     :
*
*/




#include <wchar.h>
#include <wctype.h>

EXPORT_C
int wcsicmp(const wchar_t* wcs1,const wchar_t* wcs2)  
{
	while (towlower(*wcs1) == towlower(*wcs2))
  	{
    		if (*wcs1 == 0)
      			return 0;
    		wcs1++;
    		wcs2++;
  	}
  	return towlower(*wcs1) - towlower(*wcs2);	
}


/***
*  Compare the first n wide-chars from the wide-character strings wcs1 and wcs2
*  return 0, if two strings are equal, >0, if first string is greater than second
*  string or <0, if first string is less than second string.
***/

EXPORT_C
int wcsnicmp (const wchar_t *wcs1, const wchar_t *wcs2, size_t n)
{
	int i;	
	if (n == 0)
		return 0;
		
	for(i = 0; i < n; i++)
	{
		if (towlower(*wcs1) != towlower(*wcs2))
			return towlower(*wcs1) - towlower(*wcs2);
		if (*wcs1++ == 0)
			break;
		wcs2++;
	}
	return 0;
}
