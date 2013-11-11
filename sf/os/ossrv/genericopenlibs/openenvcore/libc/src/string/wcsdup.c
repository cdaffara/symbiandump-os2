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
* Name        : wcsdup.c
* Part of     : MRT
* Implementation for wcsdup API
* Version     : 1.0
*
*/



#include <stdlib.h>
#include <wchar.h>

//duplicate a wide-character string
EXPORT_C wchar_t *wcsdup (const wchar_t *srcwcs)
{
        size_t len = wcslen(srcwcs) + 1;
        wchar_t *dupwcs;

        len = len * sizeof(wchar_t);
        dupwcs = (wchar_t *)malloc (len);
        if (dupwcs != NULL)
        {
                wmemcpy(dupwcs, srcwcs, (len / sizeof(wchar_t)));
        }
        return dupwcs;
}

