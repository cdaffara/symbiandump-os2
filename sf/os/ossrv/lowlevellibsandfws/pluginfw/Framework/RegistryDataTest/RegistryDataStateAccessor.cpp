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
//


#include "RegistryDataStateAccessor.h"
#include "TestUtilities.h"	// For __FILE__LINE__

// ______________________________________________________________________________
//
TInt TRegistryData_StateAccessor::InvariantTest(TAny* aTestObject)
	{
	CRegistryData* regData = REINTERPRET_CAST(CRegistryData*, aTestObject);
	if(regData->iRegistrations == NULL)
		return KTestBedFailedInvariant;
	return KErrNone;
	}

TInt TRegistryData_StateAccessor::Internalize(TAny* /* aTestObject */)
	{
	// Not required
	return KErrNone;
	}

TInt TRegistryData_StateAccessor::Externalize(TAny* /* aTestObject */)
	{
	// Not required
	return KErrNone;
	}

inline void TRegistryData_StateAccessor::BuildIndexesL(CRegistryData& aRegData)
	{
	aRegData.BuildIndexesL();
	}

inline TInt TRegistryData_StateAccessor::IndexedFind(CRegistryData& aRegData, const TUid& aImplementationUid)
	{
	return aRegData.IndexedFind(aImplementationUid);
	}

inline TBool TRegistryData_StateAccessor::MatchOnUid()
	{
	return CRegistryData::MatchOnUid(iIndexItem, iKey);
	}

inline void TRegistryData_StateAccessor::RestoreL(CRegistryData& aRegData, RFileReadStream& aStore)
	{
	aRegData.RestoreL(aStore);
	}

inline void TRegistryData_StateAccessor::StoreL(CRegistryData& aRegData, RFileWriteStream& aStore)
	{
	aRegData.StoreL(aStore);
	}

TBool TRegistryData_StateAccessor::IsRegisteredDrive(CRegistryData& aRegData, 
													 TDriveUnit aDriveUnit)
	{
	TInt driveIndex = KErrNotFound;
	const TInt driveCount = aRegData.iRegistrations->Count();
	for(TInt index = 0; (index<driveCount) && (driveIndex==KErrNotFound); ++index)
		{
		if((*(aRegData.iRegistrations))[index]->iDrive == aDriveUnit)
			driveIndex = index;
		}

	// Return ETrue if the drive has been found
	return driveIndex != KErrNotFound;
	}

