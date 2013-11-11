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
// The definition of the accessor class upon the CUnitTest
// 
//

#ifndef __UNITTESTSTATEACCESSOR_H__
#define __UNITTESTSTATEACCESSOR_H__

#include <e32base.h>
#include <f32file.h>
#include <s32stor.h>
#include <s32file.h>

#include "TestUtilities.h"
#include <ecom/test_bed/testbeddefinitions.h>
#include "StateAccessor.h"
#include <ecom/test_bed/unittest.h>


class CUnitTestTestDerivation : public CUnitTest
	{
public:
	inline CUnitTestTestDerivation(const TDesC& aName, CDataLogger& aDataLogger, MUnitTestObserver& aUnitTestObserver);

	inline void ConstructL();
	};

// ______________________________________________________________________________
//
/**
	@internalComponent
	
	Comments : State accessor for the CUnitTest object under test.
 */
class TUnitTest_StateAccessor : public MStateAccessor
	{
public:
	/**
		@fn				InvariantTest(TAny* aTestObject)
		Intended Usage	: Checks for Incariant violation by the CUnitTest class.
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			TInt KErrNone if the invariants were valid, error code otherwise.
		@pre 			TUnitTest_StateAccessor is fully constructed.
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
		@pre 			TUnitTest_StateAccessor is fully constructed, and
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
		@pre 			TUnitTest_StateAccessor is fully constructed, and
						valid test object must be passed in.
		@post			Returns an error code dependant on the result of the test
	 */
	inline virtual TInt Externalize(TAny* aTestObject);

	/**
		@fn				InternalizeL(RFileReadStream& aStream, CUnitTest* aUnitTest)
		Intended Usage	: Implemented by the developer to set the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			aStream The filestream to read from .
		@param			aUnitTest A pointer to the CUnitTest object under test.
		@return			None.
		@pre 			TUnitTest_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	 */
		inline void InternalizeL(RFileReadStream& aStream, CUnitTest* aUnitTest);

	/**
		@fn				ExternalizeL(RFileWriteStream& aStream, CUnitTest* aUnitTest)
		Intended Usage	: Implemented by the developer to persist the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			aStream The filestream to persist into.
		@param			aUnitTest A pointer to the CUnitTest object under test.
		@return			None.
		@pre 			TUnitTest_StateAccessor is fully constructed.
		@post			No change to the class under test, its internal state.
						is persisted.
	*/
	inline void ExternalizeL(RFileWriteStream& aStream, CUnitTest* aUnitTest);

	inline CUnitTestTestDerivation* CreateUnitTest(CUnitTest& aUnitTest, const TDesC& aName, CDataLogger& aDataLogger, MUnitTestObserver& aUnitTestObserver);

	inline void RunL(CUnitTest& aUnitTest);

	inline void AddTransitionL(CUnitTest& aUnitTest, CTransition* aTransition);

	inline void AddBlockingTransitionL(CUnitTest& aUnitTest, CTransition* aTransition);

	inline void AddLeaveErrorCodeL(CUnitTest& aUnitTest, TInt& aLeaveErrorCode);

private:
	/**
		@fn				InternalizeL(CUnitTest* aUnitTest)
		Intended Usage	: Implemented by the developer to set the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			None.
		@pre 			TUnitTest_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	*/
	inline void InternalizeL(CUnitTest* aUnitTest);

	/**
		@fn				ExternalizeL(CUnitTest* aUnitTest)
		Intended Usage	: Implemented by the developer to persist the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			None.
		@pre 			TUnitTest_StateAccessor is fully constructed.
		@post			No change to the class under test, its internal state.
						is persisted.
	*/
	inline void ExternalizeL(CUnitTest* aUnitTest);
	};	// TUnitTest_StateAccessor

#include "UnitTestStateAccessor.inl"

#endif // __UNITTESTSTATEACCESSOR_H__

