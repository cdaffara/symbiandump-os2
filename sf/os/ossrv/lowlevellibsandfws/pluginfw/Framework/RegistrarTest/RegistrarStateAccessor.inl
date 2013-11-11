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
// Implementation of the state accessor class for the CRegistrar class
// 
//

// ______________________________________________________________________________
//
inline TInt TRegistrar_StateAccessor::InvariantTest(TAny* aTestObject)
	{
	CRegistrar* registrar = REINTERPRET_CAST(CRegistrar*, aTestObject);

	// The only invariants over all cases
	if(	registrar						== NULL ||
		registrar->iDiscoverer			== NULL)
		return KTestBedFailedInvariant;
	return KErrNone;
	}

inline TInt TRegistrar_StateAccessor::Internalize(TAny* /* aTestObject */)
	{
	// No reload state
	return KErrNone;
	}

inline TInt TRegistrar_StateAccessor::Externalize(TAny* /* aTestObject */)
	{
	// No persistent state yet
	return KErrNone;
	}

inline void TRegistrar_StateAccessor::ParseL(CRegistrar& aRegistrar, 
											 CRegistryData::CDllData& aDll, 
											 const TDesC& aRscFileName)
	{
	aRegistrar.ParseL(aDll, aRscFileName);
	}

inline void TRegistrar_StateAccessor::ParseRegistrationDataL(CRegistrar& aRegistrar, 
															 const TEntry& aEntry, 
															 const TDesC& aRscFileName,
															 const TDriveUnit& aDrive,
															 TBool aUpdate,
															 TInt aRegistryDriveIndex)
	{
	aRegistrar.ParseRegistrationDataL(aEntry, aRscFileName, aDrive, aUpdate, aRegistryDriveIndex);
	}

inline void TRegistrar_StateAccessor::DiscoveriesBegin(CRegistrar& aRegistrar)
	{
	aRegistrar.DiscoveriesBegin();
	}

inline void TRegistrar_StateAccessor::DiscoveriesComplete(CRegistrar& aRegistrar, TBool aSuccessful)
	{
	aRegistrar.DiscoveriesComplete(aSuccessful);
	}

inline void TRegistrar_StateAccessor::DriveRemoved(CRegistrar& aRegistrar, TDriveUnit aDrive)
	{
	aRegistrar.DriveRemovedL(aDrive);
	}

inline void TRegistrar_StateAccessor::DriveReinstatedL(CRegistrar& aRegistrar,
													   TDriveUnit aDrive)
	{
	aRegistrar.DriveReinstatedL(aDrive);
	}

void TRegistrar_StateAccessor::RegisterDiscoveryL(CRegistrar& aRegistrar, const TEntry& aEntry)
	{
	aRegistrar.RegisterDiscoveryL(aEntry);
	}

