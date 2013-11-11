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
* Name        : wcscasecmp.c
* Part of     : MRT
* Implementation for wcscasecmp API
* Version     : 1.0
*
*/



#include <wchar.h>
#include <wctype.h>

//compare two wide-character strings, ignoring case
EXPORT_C int wcscasecmp(const wchar_t *s1, const wchar_t *s2)
{
        const wchar_t *ps1 = (const wchar_t *)s1;
        const wchar_t *ps2 = (const wchar_t *)s2;

        while (towlower(*ps1) == towlower(*ps2++))
        {
                if (*ps1++ == '\0')
                {
                        return (0);
                }
        }
        return (towlower(*ps1) - towlower(*--ps2));
}

