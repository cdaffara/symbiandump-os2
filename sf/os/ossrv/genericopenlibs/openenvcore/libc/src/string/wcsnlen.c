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
* Name        : wcsnlen.c
* Part of     : MRT
* Implementation for wcsnlen API
* Version     : 1.0
*
*/



#include <wchar.h>

//determine the length of a fixed-size wide-character string
EXPORT_C size_t wcsnlen(const wchar_t *s, size_t maxlen)
{
        size_t len = 0;
        while(s[len] != L'\0' && maxlen > 0)
        {
                len++;
                maxlen--;
        }
        return (len);
}

