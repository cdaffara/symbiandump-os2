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
* Defines kernel-side factory and channel classes which implement
* the extraction LDD.
*
*/




/**
 @file
 @internalComponent
*/


#ifndef KMSLDDK_H
#define KMSLDDK_H

#include <kernel/kernel.h>
#include <kernel/kern_priv.h>

#include "hwkeystore.h"
#include "kmsextrlddcommon.h"


class DKmsExtrLddFactory : public DLogicalDevice
/**
	An instance of this factory object is exported from the
	LDD DLL.  It allocates an instance of DKmsLddChannel when
	the user opens a channel with RBusLogicalChannel::Open.
 */
	{
public:
	DKmsExtrLddFactory();
	
	// implement DLogicalDevice
	virtual TInt Install();
	virtual void GetCaps(TDes8& aCapsDes) const;
	virtual TInt Create(DLogicalChannelBase*& aChannel);
	};


class DKmsExtrLddChannel : public DLogicalChannelBase
/**
	Logical channel referenced from user-side RKmsExtrChannel.
	This derives from DLogicalChannelBase instead of DLogicalChannel
	because it must run in a separate thread to the mean KMS LDD.
 */
	{
public:
	DKmsExtrLddChannel();
	virtual ~DKmsExtrLddChannel();

	// override DLogicalChannelBase
	virtual TInt DoCreate(TInt aUnit, const TDesC8* aInfo, const TVersion& aVer);
	// implement DLogicalChannelBase
	virtual TInt Request(TInt aReqNo, TAny* a1, TAny* a2);

private:
	TInt GetKeyData(TAny* aKdaPckgU, HBuf8*& aData);
	};


#endif	// #ifndef KMSLDDK_H

