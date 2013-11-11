/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <sys/types.h>
#include "sysreent.h"
#include <e32cmn.h>

extern "C" {

void *memmove_private(void* dst0, const void*  src0, size_t length)
	{
	return _memmove_r(dst0, src0, length);
	}

void *memcpy_private(void* dst0, const void*  src0, size_t length)
	{
	return _memcpy_r(dst0, src0, length);
	}

int memcmp_private(const unsigned char *s1, const unsigned char *s2, size_t n)
    {
    return memcompare(s1, n, s2, n);
    }

} // extern "C"
