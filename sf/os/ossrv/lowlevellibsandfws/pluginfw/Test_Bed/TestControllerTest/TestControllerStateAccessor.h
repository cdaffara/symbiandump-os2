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
// The definition of the accessor class upon the CTestController
// 
//

#ifndef __TESTCONTROLLERSTATEACCESSOR_H__
#define __TESTCONTROLLERSTATEACCESSOR_H__

#include <e32base.h>
#include <f32file.h>
#include <s32stor.h>
#include <s32file.h>

#include "TestUtilities.h"
#include <ecom/test_bed/testbeddefinitions.h>
#include "StateAccessor.h"
#include "TestController.h"

// ______________________________________________________________________________
//
/**
	@internalComponent
 
	Comments : State accessor for the CTestController object under test.
 */
class TTestController_StateAccessor : public MStateAccessor
	{
public:
	/**
		@fn				InvariantTest(TAny* aTestObject)
		Intended Usage	: Checks for the state invariance of the CTestController has not been violated.	
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			TInt KErrNone if the invariants were valid, error code otherwise.
		@pre 			TTestController_StateAccessor is fully constructed.
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
		@pre 			TTestController_StateAccessor is fully constructed, and
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
		@pre 			TTestController_StateAccessor is fully constructed, and
						valid test object must be passed in.
		@post			Returns an error code dependant on the result of the test
	 */
	inline virtual TInt Externalize(TAny* aTestObject);

	/**
		@fn				InternalizeL(RFileReadStream& aStream, CTestController* aTestController)
		Intended Usage	: Implemented by the developer to set the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			aStream The filestream to read from .
		@param			aTestController A pointer to the CTestController object under test.
		@return			None.
		@pre 			TTestController_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	 */
		inline void InternalizeL(RFileReadStream& aStream, CTestController* aTestController);

	/**
		@fn				ExternalizeL(RFileWriteStream& aStream, CTestController* aTestController)
		Intended Usage	: Implemented by the developer to persist the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			aStream The filestream to persist into.
		@param			aTestController A pointer to the CTestController object under test.
		@return			None.
		@pre 			TTestController_StateAccessor is fully constructed.
		@post			No change to the class under test, its internal state.
						is persisted.
	*/
	inline void ExternalizeL(RFileWriteStream& aStream, CTestController* aTestController);

private:
	/**
		@fn				InternalizeL(CTestController* aTestController)
		Intended Usage	: Implemented by the developer to set the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			None.
		@pre 			TTestController_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	*/
	inline void InternalizeL(CTestController* aTestController);

	/**
		@fn				ExternalizeL(CTestController* aTestController)
		Intended Usage	: Implemented by the developer to persist the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			None.
		@pre 			TTestController_StateAccessor is fully constructed.
		@post			No change to the class under test, its internal state.
						is persisted.
	*/
	inline void ExternalizeL(CTestController* aTestController);
	};	// TTestController_StateAccessor

#include "TestControllerStateAccessor.inl"

#endif // __TESTCONTROLLERSTATEACCESSOR_H__

