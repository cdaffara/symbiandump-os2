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
* Name        : wcstoq.c
* Part of     : MRT
* Implementation for wcstoq API
* Version     : 1.0
*
*/



#include <wchar.h>

EXPORT_C long long wcstoq(const wchar_t * __restrict nptr, wchar_t ** __restrict endptr, int base)
{
	return (wcstoll(nptr,endptr,base));
}

