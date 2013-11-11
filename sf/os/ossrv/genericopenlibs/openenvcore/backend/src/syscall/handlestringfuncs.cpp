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
#include <e32cmn.h>
#include "sysreent.h"

extern "C" {

EXPORT_C void *_memcpy_r(void *dst0, const void *src0, size_t length)
	{	
	memcpy(dst0, src0, length);
	return (0);
	}

EXPORT_C void *_memmove_r(void *dst0, const void *src0, size_t length)
	{
	memmove(dst0, src0, length);
	return (0);
	}


} //extern C end
