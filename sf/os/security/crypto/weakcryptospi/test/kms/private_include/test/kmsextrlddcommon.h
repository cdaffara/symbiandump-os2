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
* Data shared between KMS extract LDD client API and driver, but not
* with the client.
*
*/




/**
 @file
 @internalComponent
*/

#ifndef KMSEXTRLDDCOMMON_H
#define KMSEXTRLDDCOMMON_H

#include "kmsldd.h"


namespace KmsExtrLddImpl

/**
	This namespace contains values and data structures which are shared
	between the extract client API and the driver implementation, but not
	with the client itself.
 */
	{
	inline TVersion Version();
	
	class TKeyDataArgs
	/** Bundles handle and operation arguments when extracting key data. */
		{
	public:
		TKeyHandle iHandle;		///< Key to use.
		TInt iOperation;		///< Operation that thread wants to perform.
		};
	
	enum TControl
	/** Synchronous requests supported by this driver. */
		{
		EGetKeySize,
		EGetKeyData
		};
	}

#include "kmsextrlddcommon.inl"

#endif	// #ifndef KMSEXTRLDDCOMMON_H
