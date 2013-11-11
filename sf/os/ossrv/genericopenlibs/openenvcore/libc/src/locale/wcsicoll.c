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
* Name        : wcsicoll.c
* Part of     : libc
* Interface   : 
* Implementation of wcsupr on Symbian
* Version     :
*
*/




#include <wchar.h>
#include <limits.h>
#include <wctype.h>
#include <stdlib.h>

/****
*  wcsicoll
*
****/


EXPORT_C
int wcsicoll(const wchar_t *wcs1, const wchar_t *wcs2)
{
	wchar_t *ptr;
	wchar_t temp1[LINE_MAX], temp2[LINE_MAX];

	for (ptr = temp1; *wcs1; wcs1++)
		*ptr++ = towlower(*wcs1);
	*ptr = '\0';
	
	for (ptr = temp2; *wcs2; wcs2++)
		*ptr++ = towlower(*wcs2);
	*ptr = '\0';
	
	return (wcscoll(temp1, temp2));
}

 

/****
*  wcsncoll
*
****/


EXPORT_C
int wcsncoll(const wchar_t* wcs1, const wchar_t* wcs2, size_t n)
{
  wchar_t* tmpa = (wchar_t*)malloc((n + 1)*sizeof(wchar_t));
  wchar_t* tmpb = (wchar_t*)malloc((n + 1)*sizeof(wchar_t));
  int ret;
  
  wmemcpy(tmpa, wcs1, n);
  tmpa[n] = '\0';

  wmemcpy(tmpb, wcs2, n);
  tmpb[n] = '\0';

  ret = wcscoll(tmpa, tmpb);
  
	free(tmpa);
	free(tmpb);
	
	return ret;
}



/****
*  wcsnicoll
*
****/


EXPORT_C
int wcsnicoll(const wchar_t* wcs1, const wchar_t* wcs2, size_t n)
{
  wchar_t* tmpa = (wchar_t*)malloc((n + 1)*sizeof(wchar_t));
  wchar_t* tmpb = (wchar_t*)malloc((n + 1)*sizeof(wchar_t));
  int ret;
  
  wmemcpy(tmpa, wcs1, n);
  tmpa[n] = '\0';

  wmemcpy(tmpb, wcs2, n);
  tmpb[n] = '\0';

  ret = wcsicoll(tmpa, tmpb);
  
  free(tmpa);
  free(tmpb);
  
  return ret;
}

