// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The definition of the accessor class upon the CTransition
// 
//

#ifndef __TRANSITIONSTATEACCESSOR_H__
#define __TRANSITIONSTATEACCESSOR_H__

#include <e32base.h>
#include <f32file.h>
#include <s32stor.h>
#include <s32file.h>

#include "TestUtilities.h"
#include <ecom/test_bed/testbeddefinitions.h>
#include "StateAccessor.h"
#include <ecom/test_bed/transition.h>

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : State accessor for the CTransition object under test.
 */
class TTransition_StateAccessor : public MStateAccessor
	{
public:
	/**
		@fn				InvariantTest(TAny* aTestObject)
		Intended Usage	:	
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			TInt KErrNone if the invariants were valid, error code otherwise.
		@pre 			TTransition_StateAccessor is fully constructed.
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
		@pre 			TTransition_StateAccessor is fully constructed, and
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
		@pre 			TTransition_StateAccessor is fully constructed, and
						valid test object must be passed in.
		@post			Returns an error code dependant on the result of the test
	 */
	inline virtual TInt Externalize(TAny* aTestObject);

	/**
		@fn				IsRepeated(TAny* aTestObject) const
		Intended Usage	: Implemented by the developer to access the iRepeatThis
						flag in the CTransition object under test.
		@since			7.0
		@param			aTestObject The CTransition object to be tested
		@return			ETrue if the flag is set.
		@pre 			TTransition_StateAccessor is fully constructed, and
						a valid test object must be passed in.
	 */
	inline TBool IsRepeated(TAny* aTestObject) const;

	/**
		@fn				TransitionId(TAny* aTestObject) const
		Intended Usage	: Implemented by the developer to access the iTransitionId
						descriptor in the CTransition object under test.
		@since			7.0
		@param			aTestObject The CTransition object to be tested
		@return			A pointer to the iTransitionId descriptor.
		@pre 			TTransition_StateAccessor is fully constructed, and
						a valid test object must be passed in.
	 */
	inline const TDesC* TransitionId(TAny* aTestObject) const;

	/**
		@fn				TransitionInfo(TAny* aTestObject) const
		Intended Usage	: Implemented by the developer to access the iTransitionId
						descriptor in the CTransition object under test.
		@since			7.0
		@param			aTestObject The CTransition object to be tested
		@return			A pointer to the iTransitionInfo structure.
		@pre 			TTransition_StateAccessor is fully constructed, and
						a valid test object must be passed in.
	 */
	inline const TTransitionInfo* TransitionInfo(TAny* aTestObject) const;

	/**
		@fn				BlockingFlag(TAny* aTestObject) const
		Intended Usage	: Implemented by the developer to access the iBlockingTransition
						flag in the CTransition object under test.
		@since			7.0
		@param			aTestObject The CTransition object to be tested
		@return			ETrue if the flag is set.
		@pre 			TTransition_StateAccessor is fully constructed, and
						a valid test object must be passed in.
	 */
	inline TBool BlockingFlag(TAny* aTestObject) const;

	/**
		@fn				BlockingFlag(TAny* aTestObject) const
		Intended Usage	: Implemented by the developer to access the iLeaveError
						flag in the CTransition object under test.
		@since			7.0
		@param			aTestObject The CTransition object to be tested
		@return			The current value of the error code.
		@pre 			TTransition_StateAccessor is fully constructed, and
						a valid test object must be passed in.
	 */
	inline TInt ErrorCode(TAny* aTestObject) const;

	/**
		@fn				BlockingFlag(TAny* aTestObject) const
		Intended Usage	: Implemented by the developer to access the iStatus
						structure in the CTransition object under test.
		@since			7.0
		@param			aTestObject The CTransition object to be tested
		@return			The contents of iStatus.
		@pre 			TTransition_StateAccessor is fully constructed, and
						a valid test object must be passed in.
	 */
	inline TRequestStatus Status(const TAny* aTestObject) const;

	/**
		@fn				InternalizeL(RFileReadStream& aStream, CTransition* aTransition)
		Intended Usage	: Implemented by the developer to set the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			aStream The filestream to read from .
		@param			aTransition A pointer to the CTransition object under test.
		@return			None.
		@pre 			TTransition_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	 */
		inline void InternalizeL(RFileReadStream& aStream, CTransition* aTransition);

	/**
		@fn				ExternalizeL(RFileWriteStream& aStream, CTransition* aTransition)
		Intended Usage	: Implemented by the developer to persist the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			aStream The filestream to persist into.
		@param			aTransition A pointer to the CTransition object under test.
		@return			None.
		@pre 			TTransition_StateAccessor is fully constructed.
		@post			No change to the class under test, its internal state.
						is persisted.
	*/
	inline void ExternalizeL(RFileWriteStream& aStream, CTransition* aTransition);


private:
	/**
		@fn				InternalizeL(CTransition* aTransition)
		Intended Usage	: Implemented by the developer to set the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			None.
		@pre 			TTransition_StateAccessor is fully constructed.
		@post			The class under test is set to the state specified.
	*/
	inline void InternalizeL(CTransition* aTransition);

	/**
		@fn				ExternalizeL(CTransition* aTransition)
		Intended Usage	: Implemented by the developer to persist the state of the object under test.
		Error Condition	: Leaves with an error code.
		@since			7.0
		@param			TAny* A pointer to the object under test.
		@return			None.
		@pre 			TTransition_StateAccessor is fully constructed.
		@post			No change to the class under test, its internal state.
						is persisted.
	*/
	inline void ExternalizeL(CTransition* aTransition);
	};	// TTransition_StateAccessor

#include "TransitionStateAccessor.inl"

#endif // __TRANSITIONSTATEACCESSOR_H__

