/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* crypto Mac API implementation
*
*/


/**
 @file
*/

#include <cryptospi/cryptomacapi.h>
#include <cryptospi/macplugin.h>
#include "legacyselector.h"

using namespace CryptoSpi;


EXPORT_C void CMacFactory::CreateMacL(CMac*& aMac,
									  const TUid aAlgorithmUid,
									  const CKey& aKey,
		                              const CCryptoParams* aAlgorithmParams)
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateMacL(aMac, aAlgorithmUid, aKey, aAlgorithmParams);
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateMacL(aMac, aAlgorithmUid, aKey, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //selector	
		}
	}									

CMac* CMac::NewL(MMac* aMac, TInt aHandle)
	{
	CMac* self = new (ELeave) CMac(aMac, aHandle); 		
	return self;
	}
		
CMac::CMac(MMac* aMac, TInt aHandle)
: CCryptoBase(aMac, aHandle)
	{
	}

EXPORT_C CMac::~CMac()
	{
	}

/**
 * Following working methods delegate calls to the plug-in implementation 
 * of the requested MAC algorithm.
 */
EXPORT_C TPtrC8 CMac::MacL(const TDesC8& aMessage)
	{
	return ((MMac*)iPlugin)->MacL(aMessage); 
	}


EXPORT_C void CMac::UpdateL(const TDesC8& aMessage)
	{
	((MMac*)iPlugin)->UpdateL(aMessage);
	}

EXPORT_C TPtrC8 CMac::FinalL(const TDesC8& aMessage)
	{
	return ((MMac*)iPlugin)->FinalL(aMessage);
	}

EXPORT_C void CMac::ReInitialiseAndSetKeyL(const CKey& aKey)
	{
	((MMac*)iPlugin)->ReInitialiseAndSetKeyL(aKey);	
	}

EXPORT_C CMac* CMac::ReplicateL()
	{
	MMac* plugin=((MMac*)iPlugin)->ReplicateL();
	CleanupClosePushL(*plugin);
	RLibrary lib;
	lib.SetHandle(iLibHandle);
	RLibrary duplib=lib;
	User::LeaveIfError(duplib.Duplicate(RThread(), EOwnerProcess));
	CleanupClosePushL(duplib);
	CMac* self=new(ELeave) CMac(plugin, duplib.Handle());
	CleanupStack::Pop(2, plugin); //duplib, plugin
	return self;		
	}

EXPORT_C CMac* CMac::CopyL()
	{
	MMac* plugin=((MMac*)iPlugin)->CopyL();
	CleanupClosePushL(*plugin);
	RLibrary lib;
	lib.SetHandle(iLibHandle);
	RLibrary duplib=lib;
	User::LeaveIfError(duplib.Duplicate(RThread(), EOwnerProcess));
	CleanupClosePushL(duplib);
	CMac* self=new(ELeave) CMac(plugin, duplib.Handle());
	CleanupStack::Pop(2, plugin); //duplib, plugin
	return self;			
	}

//
// Asynchronous MAC interface implementation
// (async not implemented, so no coverage)
// Once we have a dedicated hardware plugin-dll
// the following pre-processor conditions must be removed.
//
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif


EXPORT_C void CMacFactory::CreateAsyncMacL(CAsyncMac*& aMac,
									  const TUid aAlgorithmUid,
									  const CKey& aKey,
		                              const CCryptoParams* aAlgorithmParams)
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateAsyncMacL(aMac, aAlgorithmUid, aKey, aAlgorithmParams);
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateAsyncMacL(aMac, aAlgorithmUid, aKey, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //selector	
		}
	}									

CAsyncMac* CAsyncMac::NewL(MAsyncMac* aMac, TInt aHandle)
	{
	CAsyncMac* self = new (ELeave) CAsyncMac(aMac, aHandle); 		
	return self;
	}
		
CAsyncMac::CAsyncMac(MAsyncMac* aMac, TInt aHandle)
: CCryptoBase(aMac, aHandle)
	{
	}

EXPORT_C CAsyncMac::~CAsyncMac()
	{
	}

/**
 * Following asynchronous working methods delegate calls to the plug-in implementation 
 * of the requested MAC algorithm.
 */
EXPORT_C TPtrC8 CAsyncMac::MacL(const TDesC8& aMessage, TRequestStatus& aStatus)
	{
	return ((MAsyncMac*)iPlugin)->MacL(aMessage, aStatus); 
	}

EXPORT_C void CAsyncMac::UpdateL(const TDesC8& aMessage, TRequestStatus& aStatus)
	{
	((MAsyncMac*)iPlugin)->UpdateL(aMessage, aStatus);
	}

EXPORT_C TPtrC8 CAsyncMac::FinalL(const TDesC8& aMessage, TRequestStatus& aStatus)
	{
	return ((MAsyncMac*)iPlugin)->FinalL(aMessage, aStatus);
	}

EXPORT_C void CAsyncMac::ReInitialiseAndSetKeyL(const CKey& aKey)
	{
	((MAsyncMac*)iPlugin)->ReInitialiseAndSetKeyL(aKey);	
	}

EXPORT_C CAsyncMac* CAsyncMac::ReplicateL()
	{
	MAsyncMac* plugin=((MAsyncMac*)iPlugin)->ReplicateL();
	CleanupClosePushL(*plugin);
	RLibrary lib;
	lib.SetHandle(iLibHandle);
	RLibrary duplib=lib;
	User::LeaveIfError(duplib.Duplicate(RThread(), EOwnerProcess));
	CleanupClosePushL(duplib);
	CAsyncMac* self=new(ELeave) CAsyncMac(plugin, duplib.Handle());
	CleanupStack::Pop(2, plugin); //duplib, plugin
	return self;		
	}

EXPORT_C CAsyncMac* CAsyncMac::CopyL()
	{
	MAsyncMac* plugin=((MAsyncMac*)iPlugin)->CopyL();
	CleanupClosePushL(*plugin);
	RLibrary lib;
	lib.SetHandle(iLibHandle);
	RLibrary duplib=lib;
	User::LeaveIfError(duplib.Duplicate(RThread(), EOwnerProcess));
	CleanupClosePushL(duplib);
	CAsyncMac* self=new(ELeave) CAsyncMac(plugin, duplib.Handle());
	CleanupStack::Pop(2, plugin); //duplib, plugin
	return self;			
	}


EXPORT_C void CAsyncMac::Cancel()
	{
	((MAsyncMac*)iPlugin)->Cancel();	
	}
