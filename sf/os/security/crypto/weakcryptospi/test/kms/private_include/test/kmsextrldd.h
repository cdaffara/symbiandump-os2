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
* Client API for controlling KMS extract LDD.
*
*/




/**
 @file
 @internalComponent
*/

#ifndef KMSEXTRLDD_H
#define KMSEXTRLDD_H

#include <e32cmn.h>

#ifndef __KERNEL_MODE__
	#include <e32std.h>
#endif

#include "keyhandle.h"

/** Extraction LDD filename, as supplied to LoadLogicalDevice. */
_LIT(KKmsExtrLddFileName, "KMSEXTRLDDK");

class RKmsExtrChannel : public RBusLogicalChannel
/**
	This LDD extracts a key from KMSKEXT.  It is defined so test code can
	test the extract functionality which is provided for the ACC LDD.
 */
	{
public:
	static inline const TDesC& FactoryName();
	
#ifndef __KERNEL_MODE__
	IMPORT_C TInt Open();
	IMPORT_C TInt Extract(TKeyHandle aHandle, TInt aOperation, HBufC8*& aData);
#endif
	};

#include "kmsextrldd.inl"

#endif	// #ifndef KMSEXTRLDD_H

