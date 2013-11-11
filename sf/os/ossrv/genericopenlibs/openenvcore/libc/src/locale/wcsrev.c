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
* Name        : wcsrev.c
* Part of     : libc
* Interface   : 
* Implementation of wcsrev on Symbian
* Version     :
*
*/




#include <wchar.h>

EXPORT_C
wchar_t* wcsrev(wchar_t *wcs) 
{
	wchar_t  *eptr;
	wchar_t  *ptr;
	wchar_t   wc;
	
	eptr = ptr = wcs;
	while (*(eptr+1))
		eptr++;
	while (wcs < eptr)
	{
		wc = *wcs;
		*wcs = *eptr;
		*eptr = wc;
		wcs++;
		eptr--;
	}
	return ptr;
}
