/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* crypto hash API implementation
* crypto hash API implementation
*
*/


/**
 @file
*/

#include <cryptospi/cryptohashapi.h>
#include "legacyselector.h"
#include <cryptospi/hashplugin.h>


using namespace CryptoSpi;

//
// Hash Factory Implementation
//
EXPORT_C void CHashFactory::CreateHashL(CHash*& aHash,
									TUid aAlgorithmUid,
									TUid aOperationMode,
									const CKey* aKey,
									const CCryptoParams* aAlgorithmParams)
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateHashL(aHash, aAlgorithmUid, aOperationMode, aKey, aAlgorithmParams);
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateHashL(aHash, aAlgorithmUid, aOperationMode, aKey, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //selector	
		}
	}							

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT		

EXPORT_C void CHashFactory::CreateHashL(CHash*& aHash,
								 TUid aAlgorithmUid,
								 const CCryptoParams* aAlgorithmParams)
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateHashL(aHash, aAlgorithmUid, aAlgorithmParams);
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateHashL(aHash, aAlgorithmUid, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //selector	
		}
	}
#endif

//
// Hash implementation
//
CHash* CHash::NewL(MHash* aHash, TInt aHandle)
	{
	CHash* self=new(ELeave) CHash(aHash, aHandle);
	return self;		
	}
					
EXPORT_C CHash::~CHash()
	{
	}

EXPORT_C TPtrC8 CHash::Hash(const TDesC8& aMessage)
	{
	return ((MHash*)iPlugin)->Hash(aMessage);
	}
	
EXPORT_C void CHash::Update(const TDesC8& aMessage)
	{
	((MHash*)iPlugin)->Update(aMessage);	
	}

EXPORT_C TPtrC8 CHash::Final(const TDesC8& aMessage)
	{
	MHash* hash=(MHash*)iPlugin;
	TPtrC8 hashValue=hash->Final(aMessage);
	hash->Reset();
	return hashValue;
	}
	
EXPORT_C CHash* CHash::ReplicateL()
	{
	MHash* plugin=((MHash*)iPlugin)->ReplicateL();
	CleanupClosePushL(*plugin);
	RLibrary lib;
	lib.SetHandle(iLibHandle);
	RLibrary duplib=lib;
	User::LeaveIfError(duplib.Duplicate(RThread(), EOwnerProcess));
	CleanupClosePushL(duplib);
	CHash* self=new(ELeave) CHash(plugin, duplib.Handle());
	CleanupStack::Pop(2, plugin); //duplib, plugin
	return self;		
	}
	
EXPORT_C void CHash::SetKeyL(const CKey& aKey)
	{
	((MHash*)iPlugin)->SetKeyL(aKey);
	}
			
EXPORT_C void CHash::SetOperationModeL(TUid aOperationMode)
	{
	((MHash*)iPlugin)->SetOperationModeL(aOperationMode);
	}

EXPORT_C CHash* CHash::CopyL()
	{
	MHash* plugin=((MHash*)iPlugin)->CopyL();
	CleanupClosePushL(*plugin);
	RLibrary lib;
	lib.SetHandle(iLibHandle);
	RLibrary duplib=lib;
	User::LeaveIfError(duplib.Duplicate(RThread(), EOwnerProcess));
	CleanupClosePushL(duplib);
	CHash* self=new(ELeave) CHash(plugin, duplib.Handle());
	CleanupStack::Pop(2, plugin); //duplib, plugin
	return self;			
	}

CHash::CHash(MHash* aHash, TInt aHandle)
: CCryptoBase(aHash, aHandle)
	{
	}

//
// Asynchronous hash implementation
// (async not implemented, so no coverage)
//
//
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

EXPORT_C void CHashFactory::CreateAsyncHashL(CAsyncHash*& aHash, 
											TUid aAlgorithmUid,
											TUid aOperationMode,
											const CKey* aKey,
											const CCryptoParams* aAlgorithmParams)
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateAsyncHashL(aHash, aAlgorithmUid, aOperationMode, aKey, aAlgorithmParams);
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateAsyncHashL(aHash, aAlgorithmUid, aOperationMode, aKey, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //selector
		}
	}

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT		
EXPORT_C void CHashFactory::CreateAsyncHashL(CAsyncHash*& aHash, 
								      TUid aAlgorithmUid,
								      const CCryptoParams* aAlgorithmParams)
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateAsyncHashL(aHash, aAlgorithmUid, aAlgorithmParams);
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateAsyncHashL(aHash, aAlgorithmUid, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //selector
		}
	
	}
#endif

CAsyncHash* CAsyncHash::NewL(MAsyncHash* aHash, TInt aHandle)
	{
	CAsyncHash* self=new(ELeave) CAsyncHash(aHash, aHandle);
	return self;
	}

EXPORT_C CAsyncHash::~CAsyncHash()
	{
	}

EXPORT_C void CAsyncHash::Hash(const TDesC8& aMessage, TPtrC8& aHash, TRequestStatus& aStatus)
	{
	((MAsyncHash*)iPlugin)->Hash(aMessage, aHash, aStatus);
	}
	
EXPORT_C void CAsyncHash::Update(const TDesC8& aMessage, TRequestStatus& aStatus)
	{
	((MAsyncHash*)iPlugin)->Update(aMessage, aStatus);	
	}

EXPORT_C void CAsyncHash::Final(const TDesC8& aMessage, TPtrC8& aFinal, TRequestStatus& aStatus)
	{
	((MAsyncHash*)iPlugin)->Final(aMessage, aFinal, aStatus);
	}

EXPORT_C void CAsyncHash::Cancel()
	{
	((MAsyncHash*)iPlugin)->Cancel();	
	}

EXPORT_C CAsyncHash* CAsyncHash::ReplicateL()
	{
	MAsyncHash* plugin=((MAsyncHash*)iPlugin)->ReplicateL();
	CleanupClosePushL(*plugin);
	RLibrary lib;
	lib.SetHandle(iLibHandle);
	RLibrary duplib=lib;
	User::LeaveIfError(duplib.Duplicate(RThread(), EOwnerProcess));
	CleanupClosePushL(duplib);
	CAsyncHash* self=new(ELeave) CAsyncHash(plugin, duplib.Handle());
	CleanupStack::Pop(2, plugin); //duplib, plugin
	return self;		
	}

EXPORT_C CAsyncHash* CAsyncHash::CopyL()
	{
	MAsyncHash* plugin=((MAsyncHash*)iPlugin)->CopyL();
	CleanupClosePushL(*plugin);
	RLibrary lib;
	lib.SetHandle(iLibHandle);
	RLibrary duplib=lib;
	User::LeaveIfError(duplib.Duplicate(RThread(), EOwnerProcess));
	CleanupClosePushL(duplib);
	CAsyncHash* self=new(ELeave) CAsyncHash(plugin, duplib.Handle());
	CleanupStack::Pop(2, plugin); //duplib, plugin
	return self;	
	}

EXPORT_C void CAsyncHash::SetKeyL(const CKey& aKey)
	{
	((MAsyncHash*)iPlugin)->SetKeyL(aKey);
	}
			
EXPORT_C void CAsyncHash::SetOperationModeL(TUid aOperationMode)
	{
	((MAsyncHash*)iPlugin)->SetOperationModeL(aOperationMode);
	}
	
CAsyncHash::CAsyncHash(MAsyncHash* aAsyncHash, TInt aHandle)
: CCryptoBase(aAsyncHash, aHandle)
	{		
	}


