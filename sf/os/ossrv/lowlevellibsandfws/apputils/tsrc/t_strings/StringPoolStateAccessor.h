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
// The definition of the accessor class upon the RStringPool
// 
//

#ifndef __STRINGPOOLSTATEACCESSOR_H__
#define __STRINGPOOLSTATEACCESSOR_H__

#include <e32base.h>
#include <f32file.h>
#include <s32stor.h>
#include <s32file.h>

#include <ecom/test_bed/testutilities.h>
#include <ecom/test_bed/testbeddefinitions.h>
#include <ecom/test_bed/stateaccessor.h>
#include <stringpool.h>
// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : State accessor for the RStringPool object under test.
 */
class TStringPool_StateAccessor : public MStateAccessor
	{
public:
	/**
		@fn				InvariantTest(TAny* aTestObject)
		Intended Usage	:	
		@since			6.0
		@param			TAny* A pointer to the object under test.
		@return			TInt KErrNone if the invariants were valid, error code otherwise.
		@pre 			TStringPool_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	 */
	inline TInt InvariantTest(TAny* aTestObject);

	/**
		@fn				Internalize(TAny* aTestObject)
		Intended Usage	: Implemented by the developer to set the state of the object under test.
		@since			6.0
		@param			aTestObject The object to be tested
		@return			KErrNone if the internalize was successful.
		@return			One of the EPOC standard error codes indicating the reason
						for the failure	to load the object state.
		@pre 			TStringPool_StateAccessor is fully constructed, and
						valid test object must be passed in.
		@post			The class under test is set to the state specified
	 */
	inline virtual TInt Internalize(TAny* aTestObject);

	/**
		@fn				Externalize(TAny* aTestObject)
		Intended Usage	: Implemented by the developer to persist the state of the object under test.
		@since			6.0
		@param			aTestObject The object to be tested
		@return			KErrNone if the internalize was successful.
		@return			One of the EPOC standard error codes indicating the reason
						for the failure	to externalize the object.
		@pre 			TStringPool_StateAccessor is fully constructed, and
						valid test object must be passed in.
		@post			Returns an error code dependant on the result of the test
	 */
	inline virtual TInt Externalize(TAny* aTestObject);

	/**
		@fn				InternalizeL(RFileReadStream& aStream, RStringPool* aStringPool)
		Intended Usage	: Implemented by the developer to set the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			6.0
		@param			aStream The filestream to read from .
		@param			aStringPool A pointer to the RStringPool object under test.
		@return			None.
		@pre 			TStringPool_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	 */
		inline void InternalizeL(RFileReadStream& aStream, RStringPool* aStringPool);

	/**
		@fn				ExternalizeL(RFileWriteStream& aStream, RStringPool* aStringPool)
		Intended Usage	: Implemented by the developer to persist the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			6.0
		@param			aStream The filestream to persist into.
		@param			aStringPool A pointer to the RStringPool object under test.
		@return			None.
		@pre 			TStringPool_StateAccessor is fully constructed.
		@post			No change to the class under test, its internal state.
						is persisted.
	*/
	inline void ExternalizeL(RFileWriteStream& aStream, RStringPool* aStringPool);

private:
	/**
		@fn				InternalizeL(RStringPool* aStringPool)
		Intended Usage	: Implemented by the developer to set the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			6.0
		@param			TAny* A pointer to the object under test.
		@return			None.
		@pre 			TStringPool_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	*/
	inline void InternalizeL(RStringPool* aStringPool);

	/**
		@fn				ExternalizeL(RStringPool* aStringPool)
		Intended Usage	: Implemented by the developer to persist the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			6.0
		@param			TAny* A pointer to the object under test.
		@return			None.
		@pre 			TStringPool_StateAccessor is fully constructed.
		@post			No change to the class under test, its internal state.
						is persisted.
	*/
	inline void ExternalizeL(RStringPool* aStringPool);
	};	// TStringPool_StateAccessor

#include "StringPoolStateAccessor.inl"

#endif // __STRINGPOOLSTATEACCESSOR_H__

