/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Defines inline funtions for client API to KMS extract LDD.
*
*/




/**
 @file
 @internalComponent
*/


#ifndef KMSEXTRLDD_INL
#define KMSEXTRLDD_INL

#include "kmsextrldd.h"


inline const TDesC& RKmsExtrChannel::FactoryName()
/**
	This device driver's factory name.  This is used to open a channel
	and to free the logical device.  It is not the driver's filename, and
	is not used to load the logical device.
 */
	{
	_LIT(KFactoryName, "KmsExtrLdd");
	return KFactoryName;
	}

#endif	// KMSEXTRLDD_INL

