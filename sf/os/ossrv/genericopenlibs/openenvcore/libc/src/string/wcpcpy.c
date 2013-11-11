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
* Name        : wcpcpy.c
* Part of     : MRT
* Implementation for wcpcpy API
* Version     : 1.0
*
*/



#include <wchar.h>

//copies a string of wide characters and
//returns a pointer to its end

EXPORT_C wchar_t *wcpcpy(wchar_t *dst, const wchar_t *src)
{
        wchar_t *cp;

        cp = dst;
        while ((*cp = *src) != L'\0')
        {
                cp++;
                src++;
        }

        return (cp);

}

