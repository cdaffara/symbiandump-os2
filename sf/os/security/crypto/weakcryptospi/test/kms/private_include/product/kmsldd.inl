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
* Defines inline functions for RKmsChannel.
*
*/




/**
 @file
 @internalTechnology
*/

#ifndef KMSLDD_INL
#define KMSLDD_INL

#include "kmsldd.h"

inline const TDesC& RKmsChannel::FactoryName()
/**
	This channel's name, which is used to set the factory object name
	and close the logical device.  Note this is different from the filename.

	@return					This LDD's name, which is used to open a
							channel (handled implicitly by Open,) and
							to unload the LDD with User::FreeLogicalDevice.
 */
	{
	_LIT(KKmsLddName, "KmsLdd");
	return KKmsLddName;
	}

#endif	// #ifndef KMSLDD_INL
