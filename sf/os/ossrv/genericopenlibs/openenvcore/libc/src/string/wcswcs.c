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
* Name        : wcswcs.h
* Part of     : MRT
* Implementation for wcswcs API
* Version     : 1.0
*
*/



#include <wchar.h>

#undef wcswcs

EXPORT_C wchar_t *wcswcs(const wchar_t * __restrict s, const wchar_t * __restrict find)
{
	return(wcsstr(s,find));
}
