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
* Kernel-side classes which implement the KMS LDD.  Specifically, this file
* contains the factory, channel, and entrypoint implemenataions.
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
#include "kmslddcommon.h"


class DKmsLddFactory : public DLogicalDevice
/**
	An instance of this factory object is exported from the
	LDD DLL.  It allocates an instance of DKmsLddChannel when
	the user opens a channel with RBusLogicalChannel::Open.
 */
	{
public:
	DKmsLddFactory();
	
	// implement DLogicalDevice
	virtual TInt Install();
	virtual void GetCaps(TDes8& aCapsDes) const;
	virtual TInt Create(DLogicalChannelBase*& aChannel);
	};


class DKmsLddChannel : public DLogicalChannel
/**
	Logical channel referenced from user-side RKmsChannel.
	The KMS uses this to create and delete keys, and to associate
	<process,operation> permissions with them.  Only the KMS
	can open this channel.
 */
	{
public:
	DKmsLddChannel();
	virtual ~DKmsLddChannel();

	// override DObject
	virtual TInt RequestUserHandle(DThread* aThread, TOwnerType aType);
	// override DLogicalChannelBase
	virtual TInt DoCreate(TInt aUnit, const TDesC8* aInfo, const TVersion& aVer);
	// implement DLogicalChannel
	virtual void HandleMsg(TMessageBase* aMsg);

private:
	TInt DoRequest(TInt aReqNo, TThreadMessage& aMsg);
	TInt GenerateKey(TInt aLength, TSecureId aOwner, TAny* aHandlePtr, TThreadMessage& aMsg);
	TInt StoreKey(const TAny* aDataPtr, TSecureId aOwner, TAny* aHandlePtr, TThreadMessage& aMsg);
	TInt UsageAllowed(DProcess* aUserProcess, TKeyHandle aHandle, TInt aOperation, TAny* aUserAllowedPtr, TThreadMessage& aMsg);
	
	TInt ExtractUsageIdentifer(TAny* aUiArgsU, KmsLddImpl::TUsageIdentifierArgsPckg& aUiArgsK, DProcess*& aClientProc, TThreadMessage& aMsg);

	void DoCancel(TUint aMask);

	static TInt PanicClient(TThreadMessage& aMsg, KmsLddImpl::TClientPanic aReason);

private:
	/** Client who opened the channel.  This must be the key management server. */
	DThread* iClient;
	};


#endif	// #ifndef KMSLDDK_H

