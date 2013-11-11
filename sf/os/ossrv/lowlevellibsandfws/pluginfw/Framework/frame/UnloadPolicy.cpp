// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implementation of the CUnloadPolicy class
// 
//

/**
 @internalComponent
 @file
*/
#include <ecom/ecom.h>
#include <ecom/ecomerrorcodes.h>
#include <ecom/ecomresolverparams.h>
#include <ecom/implementationinformation.h>
#include "UnloadPolicy.h"
#include "TestUtilities.h"	// For __FILE__LINE__
#include "EComUidCodes.h"

// Design Note:
// 	Dll text is per process.
// 	RLibrary handles are per thread.
// 	Therefore, data related to each loaded dll is per thread so no need for any mutex on say
// 	the reference counter.

CUnloadPolicy* CUnloadPolicy::NewLC(const TEntry& aDllEntry)
	{
	CUnloadPolicy* self = new(ELeave) CUnloadPolicy();
	CleanupStack::PushL(self);
	self->ConstructL(aDllEntry);
	return self;
	}

// Default d'tor
CUnloadPolicy::~CUnloadPolicy()
	{
	// Make sure the library is unloaded
	iLibrary.Close();
	// Remove the instance variable
	delete iDllEntry;
	}

// Default c'tor
CUnloadPolicy::CUnloadPolicy()
	{
	// Do nothing here
	}

void CUnloadPolicy::ConstructL(const TEntry& aDllEntry)
	{
	iDllEntry = CEComEntry::NewL(aDllEntry.iName,aDllEntry.iType[1],aDllEntry.iType[2]);
	}

TUnloadPolicyStatus CUnloadPolicy::DecreaseReference()
	{
	// Already finished with this DLL OR
	// Note: There could be many clients per process	
	if (iReferencesInUseCount == 0 || --iReferencesInUseCount == 0)
		{
		return EDeleteMe;		
		}
	return EDontDeleteMe;
	}

void CUnloadPolicy::IncreaseReference()
	{
	++iReferencesInUseCount;
	}

TInt CUnloadPolicy::ReferenceCount() const
	{
	return iReferencesInUseCount;
	}

TLibraryFunction CUnloadPolicy::LoadDllAndReturnProxyL()
	{
	// Function at ordinal 1 is InstantiationMethodL()
	const TInt KImplementationGroupProxy = 1;

	// If we have already loaded the library
	if(iReferencesInUseCount > 0)
		{
		IncreaseReference();
		return iLibrary.Lookup(KImplementationGroupProxy);
		}
	const TDesC& libraryPath = iDllEntry->GetName();
	if(libraryPath.Length())
		{
		// Load up the specified library and its default method
		// Dynamically load DLL	
		User::LeaveIfError(iLibrary.Load(libraryPath, TUidType(KDynamicLibraryUid,iDllEntry->GetSecondUid(),iDllEntry->GetThirdUid())));
		// Add to the reference count now to keep the
		// library loaded for the instantiation call.
		// IF the client fails before instantiating an implementation
		// we can rely upon CEcomServerSession to correctly call
		// for an implementations reference to be removed.
		IncreaseReference();
		return iLibrary.Lookup(KImplementationGroupProxy);
		}
	return NULL;
	}	

const CEComEntry& CUnloadPolicy::DllEntryInformation() const
	{
	return *iDllEntry;
	}

