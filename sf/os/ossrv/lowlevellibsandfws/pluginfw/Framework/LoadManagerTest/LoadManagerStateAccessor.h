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
// The state accessor class definitionss for the CLoadManager class
// 
//

#ifndef __LOADMANAGERSTATEACCESSOR_H__
#define __LOADMANAGERSTATEACCESSOR_H__

#include <e32base.h>
#include <f32file.h>
#include <s32stor.h>
#include <s32file.h>

#include "TestUtilities.h"
#include <ecom/test_bed/testbeddefinitions.h>
#include "StateAccessor.h"
#include "LoadManager.h"

const TUid KUniqueDllUid = {0x10009DB1};
const TUid KUniqueImplementationUid_Works = {0x10009DC3};
const TUid KUniqueImplementationUid_Fails = {0x10009DC1};

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : State accessor for the CLoadManager object under test.
 */
class TLoadManager_StateAccessor : public MStateAccessor
	{
public:
	/**
		@fn				InvariantTest(TAny* aTestObject)
		Intended Usage	: Checks that CLoadManager has not violated its invariants.	
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			TInt KErrNone if the invariants were valid, error code otherwise.
		@pre 			TLoadManager_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	 */
	inline TInt InvariantTest(TAny* aTestObject);

	/**
		@fn				Internalize(TAny* aTestObject)
		Intended Usage	: Implemented by the developer to set the state of the object under test.
		@since			7.0
		@param			aTestObject The object to be tested
		@return			KErrNone if the internalize was successful.
		@return			One of the EPOC standard error codes indicating the reason
						for the failure	to load the object state.
		@pre 			TLoadManager_StateAccessor is fully constructed, and
						valid test object must be passed in.
		@post			The class under test is set to the state specified
	 */
	inline virtual TInt Internalize(TAny* aTestObject);

	/**
		@fn				Externalize(TAny* aTestObject)
		Intended Usage	: Implemented by the developer to persist the state of the object under test.
		@since			7.0
		@param			aTestObject The object to be tested
		@return			KErrNone if the internalize was successful.
		@return			One of the EPOC standard error codes indicating the reason
						for the failure	to externalize the object.
		@pre 			TLoadManager_StateAccessor is fully constructed, and
						valid test object must be passed in.
		@post			Returns an error code dependant on the result of the test
	 */
	inline virtual TInt Externalize(TAny* aTestObject);

	/**
		@fn				InternalizeL(RFileReadStream& aStream, CLoadManager* aLoadManager)
		Intended Usage	: Implemented by the developer to set the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			aStream The filestram to read from .
		@param			aLoadManager A pointer to the CLoadManager object under test.
		@return			None.
		@pre 			TLoadManager_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	*/
	inline void InternalizeL(RFileReadStream& aStream, CLoadManager* aLoadManager);

	/**
		@fn				ExternalizeL(RFileWriteStream& aStream, CLoadManager* aLoadManager)
		Intended Usage	: Implemented by the developer to persist the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			aStream The filestream to persist into.
		@param			aLoadManager A pointer to the CLoadManager object under test.
		@return			None.
		@pre 			TLoadManager_StateAccessor is fully constructed.
		@post			No change to the class under test, its internal state.
						is persisted.
	*/
	inline void ExternalizeL(RFileWriteStream& aStream, CLoadManager* aLoadManager);

private:
	/**
		@fn				InternalizeL(CLoadManager* aLoadManager)
		Intended Usage	: Implemented by the developer to set the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			aLoadManager A pointer to the CLoadManager object under test.
		@return			None.
		@pre 			TLoadManager_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	*/
	inline void InternalizeL(CLoadManager* aLoadManager);

	/**
		@fn				ExternalizeL(CLoadManager* aLoadManager)
		Intended Usage	: Implemented by the developer to persist the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			aLoadManager A pointer to the CLoadManager object under test.
		@return			None.
		@pre 			TLoadManager_StateAccessor is fully constructed.
		@post			No change to the class under test, its internal state.
						is persisted.
	*/
	inline void ExternalizeL(CLoadManager* aLoadManager);
	};	// TLoadManager_StateAccessor


#include "LoadManagerStateAccessor.inl"

#endif // __LOADMANAGERSTATEACCESSOR_H__
