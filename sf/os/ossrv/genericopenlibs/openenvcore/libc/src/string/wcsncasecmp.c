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
* Name        : wcsncasecmp.c
* Part of     : MRT
* Implementation for wcsncasecmp API
* Version     : 1.0
*
*/



#include <wchar.h>
#include <wctype.h>

//compare two fixed-size wide-character strings, ignoring case
EXPORT_C int wcsncasecmp(const wchar_t *s1, const wchar_t *s2, size_t n)
{

        if (n == 0)
                return (0);
        do
        {
                if (towlower(*s1) != towlower(*s2++))
                {
                        return ((towlower(*s1)) - (towlower( *--s2)));
                }
                if (*s1++ == 0)
                        break;
        } while (--n != 0);
        return (0);
}

