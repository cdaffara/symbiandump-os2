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

#ifndef MEMSPYDRIVERPANDS_H
#define MEMSPYDRIVERPANDS_H

// System includes
#include <e32property.h>
#include <kernel.h>
#include <kern_priv.h>
#include <sproperty.h>

// User includes
#include "MemSpyDriverLog.h"
#include <memspy/driver/memspydriverpanics.h>


/** 
 * Spoof TProperty, the kernel-side object that encapsulates all P&S keys
 */
class TMemSpyProperty 
	{
public:
	// The property attributes.
	// Meaningful for defined properties only (ie. iType != RProperty::ETypeLimit)
	// Constant while the property is defined
	inline TUint32 Owner() { return iOwner; }

public:
	const TUint		iCategory;
	const TUint		iKey;

public:
	// iType == RProperty::ETypeLimit means not defined
	TUint8	iType;	// updates require the system lock AND the feature lock 
					// reads require only one of them
	// The property attributes.
	// Meaningful for defined properties only (ie. iType != RProperty::ETypeLimit)
	// Constant while the property is defined
	TUint8	iAttr;
	TCompiledSecurityPolicy iReadPolicy;
	TCompiledSecurityPolicy iWritePolicy;
	TUint32	iOwner;

	TUint		iRefCount;	// protected by the feature lock
	TProperty*	iNext;		// hash table collision list link -
							//		protected by the feature lock

	class TBuf
		{ // Viraiable-size buffer for  byte array property values
	public:
		static TBuf* New(TInt aSize);

		TUint16	iBufSize;		// buffer size - constant
		TUint16	iSize;			// actual property size - protected by the system lock
		TUint8	iBytes[1];		// byte array - protected by the system lock
		};

	// The property value
	// Meaningful for defined properties only (ie. iType != RProperty::ETypeLimit)
	union	// the value (ie. iValue or iBuf->iBytes) is protected by the system lock
		{ 
		TBuf*	iBuf;   // pointer updates of a defined property (eg. buffer 
						//   reallocation) require the system AND the feature locks;
						// pointer reads (eg to get/set byte values) require any of them
		TInt	iValue;
		};

	// Called with system or feature locked
	TInt Size()
		{
		return iBuf ? iBuf->iSize : 0;
		}

	// Called with system or feature locked
	TUint8* Buf()
		{
		return iBuf ? iBuf->iBytes : NULL;
		}

	SDblQue	iPendingQue;	// pending subscriptions - protected by the system lock
	};



/**
 * Spoof of DPropertyRef - the kernel-side object that represents user-side P&S property keys
 */
class DMemSpyPropertyRef : public DObject
	{
public:
	TMemSpyProperty*		iProp;
	TRequestStatus*			iStatus;
	DThread*				iClient;
	TPropertySubsRequest	iSubs;
	};


#endif
