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
// The state accessor class definitions for the CRegistrar class.
// 
//

#ifndef __REGISTRARSTATEACCESSOR_H__
#define __REGISTRARSTATEACCESSOR_H__

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : State accessor for the CRegistrar object under test.
 */
class TRegistrar_StateAccessor : public MStateAccessor
	{
public:
	/**
		@fn				InvariantTest(TAny* aTestObject)
		Intended Usage	: Checks if CRegistrar has violated any of its invariants.	
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			TInt KErrNone if the invariants were valid, error code otherwise.
		@pre 			TRegistrar_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	 */
	inline TInt InvariantTest(TAny* aTestObject);

	/**
		@fn				Internalize(TAny* aTestObject)
		Intended Usage	: Implemented by the developer to set the state of the object under test.
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			TInt KErrNone if the state was set, error code otherwise.
		@pre 			TRegistrar_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	*/
	inline TInt Internalize(TAny* aTestObject);

	/**
		@fn				Externalize(TAny* aTestObject)
		Intended Usage	: Implemented by the developer to persist the state of the object under test.
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			TInt KErrNone if the state was persisted, error code otherwise.
		@pre 			TRegistrar_StateAccessor is fully constructed.
		@post			No change to the class under test.
	*/
	inline TInt Externalize(TAny* aTestObject);

/**
	@fn				void ParseL(CRegistrar& aRegistrar, CRegistryData::CDllData& aDll, const TDesC& aRscFileName)
	Intended Usage	:	
	Error Condition	:	
	@since			7.0
	@param			aRegistrar @see CRegistrar::ParseL
	@param			aDll @see CRegistrar::ParseL
	@param			aRscFileName @see CRegistrar::ParseL
	@pre 			This object is fully constructed
	@post			The private method ParseL is executed on aRegistrar
 */
	void ParseL(CRegistrar& aRegistrar, CRegistryData::CDllData& aDll, const TDesC& aRscFileName);

/**
	@fn				void ParseRegistrationDataL(CRegistrar& aRegistrar, const TEntry& anEntry, const TDesC& aRscFileName, const TDriveUnit& aDrive, TBool aUpdate, TInt aRegistryDriveIndex)
	Intended Usage	:	
	Error Condition	:	
	@since			7.0
	@param			aRegistrar " "
	@param			aEntry @see CRegistrar::ParseRegistrationDataL
	@param			aRscFileName @see CRegistrar::ParseRegistrationDataL
	@param			aDrive @see CRegistrar::ParseRegistrationDataL
	@param			aUpdate @see CRegistrar::ParseRegistrationDataL
	@param			aRegistryDriveIndex @see CRegistrar::ParseRegistrationDataL
	@pre 			This object is fully constructed
	@post			The private method ParseRegistrationDataL is executed on aRegistrar
 */
	void ParseRegistrationDataL(CRegistrar& aRegistrar, const TEntry& aEntry, 
								const TDesC& aRscFileName, const TDriveUnit& aDrive, 
								TBool aUpdate, TInt aRegistryDriveIndex);

// Internal methods access
	void DiscoveriesBegin(CRegistrar& aRegistrar);

	void DiscoveriesComplete(CRegistrar& aRegistrar, TBool aSuccessful);

	void DriveRemoved(CRegistrar& aRegistrar, TDriveUnit aDrive);

	void DriveReinstatedL(CRegistrar& aRegistrar, TDriveUnit aDrive);

	void RegisterDiscoveryL(CRegistrar& aRegistrar, const TEntry& aEntry);

	};	// TRegistrar_StateAccessor

#include "RegistrarStateAccessor.inl"

#endif		// __REGISTRARSTATEACCESSOR_H__
