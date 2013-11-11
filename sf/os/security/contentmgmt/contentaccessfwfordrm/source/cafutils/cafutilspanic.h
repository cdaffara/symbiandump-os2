/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* The panic method and CAF Utils panic code declarations
*
*/


/**
 @file
 @publishedPartner
 @released
*/


#ifndef __CAF_UTILS_PANIC_H__
#define __CAF_UTILS_PANIC_H__

#include <e32def.h>

enum TCafUtilsPanics
	{
	EInvalidBit
	};


GLDEF_C void Panic(TInt aPanic);

#endif

