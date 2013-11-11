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
// The definition of the state accessor class for the CRegistryData.
// 
//

#ifndef __REGISTRYDATASTATEACCESSOR_H__
#define __REGISTRYDATASTATEACCESSOR_H__

#include "StateAccessor.h"
#include "RegistryData.h"
#include <test_bed/testbeddefinitions.h>

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : State accessor for the CRegistryData object under test.
 */
class TRegistryData_StateAccessor : public MStateAccessor
	{
public:
	/**
		@fn				InvariantTest(TAny* aTestObject)
		Intended Usage	: Checks if CRegistryData has violated its invariants.
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			TInt KErrNone if the invariants were valid, error code otherwise.
		@pre 			TRegistryData_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	 */
	TInt InvariantTest(TAny* aTestObject);

	/**
		@fn				Internalize(TAny* aTestObject)
		Intended Usage	: Implemented by the developer to set the state of the object under test.
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			TInt KErrNone if the state was set, error code otherwise.
		@pre 			TRegistryData_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	*/
	TInt Internalize(TAny* aTestObject);

	/**
		@fn				Externalize(TAny* aTestObject)
		Intended Usage	: Implemented by the developer to persist the state of the object under test.
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			TInt KErrNone if the state was persisted, error code otherwise.
		@pre 			TRegistryData_StateAccessor is fully constructed.
		@post			No change to the class under test.
	*/
	TInt Externalize(TAny* aTestObject);
	void BuildIndexesL(CRegistryData& aRegData);
	TInt IndexedFind(CRegistryData& aRegData, const TUid& aImplementationUid);
	TBool MatchOnUid();
	void RestoreL(CRegistryData& aRegData, RFileReadStream& aStore);
	void StoreL(CRegistryData& aRegData, RFileWriteStream& aStore);

	TBool IsRegisteredDrive(CRegistryData& aRegData, TDriveUnit aDriveUnit);

private:
	// These need to be stored here (rather than in the UnitTestContext) because TIndex
	// is a private class in CRegistryData (but this is a friend).
	CRegistryData::TInterfaceIndex		iIndexItem;
	CRegistryData::TInterfaceIndex		iKey;
	};	// TRegistryData_StateAccessor

#endif		// __REGISTRYDATASTATEACCESSOR_H__
