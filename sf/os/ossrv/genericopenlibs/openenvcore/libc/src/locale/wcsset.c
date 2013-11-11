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
* Name        : wcsset.c
* Part of     : libc
* Interface   : 
* Implementation of wcsset and wcsnset on Symbian
* Version     :
*
*/




#include <wchar.h>

EXPORT_C
wchar_t* wcsnset (wchar_t* wcs, wchar_t wc, size_t maxSize)
	{
	wchar_t *temp = wcs;
	int i = 0;
	if ( wcs == NULL )
		{
		return NULL;
		}
	while( *wcs != 0 && i < maxSize)
		{
		*wcs = wc;
		wcs++;
		i++;		
		}
	return temp;
	}

EXPORT_C
wchar_t* wcsset (wchar_t* wcs, wchar_t wc)
	{
	wchar_t *temp = wcs;

	while( *wcs != 0 )
		{
		*wcs = wc;
		wcs++;		
		}
	return temp;
	}
