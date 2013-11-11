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
* Name        : wcpncpy.c
* Part of     : MRT
* Implementation for wcpncpy API
* Version     : 1.0
*
*/



#include <wchar.h>

//copies a fixed length string of wide characters and
//returns a pointer to its end

EXPORT_C wchar_t *wcpncpy(wchar_t * dst, const wchar_t * src, size_t n)
{
        wchar_t *dptr;
        const wchar_t *sptr;
        size_t count;

        dptr = dst;
        sptr = src;
        count = n;
        while (count > 0)
        {
                --count;
                if ((*dptr++ = *sptr++) == L'\0')
                        break;
        }
        while (count > 0)
        {
                count--;
                *dptr++ = L'\0';
        }

        return (dptr - 1);
}

