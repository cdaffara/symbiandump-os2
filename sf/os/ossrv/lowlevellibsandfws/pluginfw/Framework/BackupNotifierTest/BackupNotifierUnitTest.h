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
// The unit test class definitions for the CBackupNotifier class.
// 
//

#ifndef __BACKUPNOTIFIERUNITTEST_H__
#define __BACKUPNOTIFIERUNITTEST_H__

#include <test_bed/testbeddefinitions.h>
#include "BackupNotifierStateAccessor.h"
#include "BackupNotifierTransitionValidation.h"
#include "BackupNotifierTransitions.h"

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for CreateAndDestroy on CBackupNotifier, the class under test.
 */
class CBackupNotifier_CreateAndDestroy_UnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger,
							MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave			KErrNoMemory
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@return			CBackupNotifier_CreateAndDestroy_UnitTest* The constructed object.
		@pre 			None.
		@post			CBackupNotifier_CreateAndDestroy_UnitTest is fully constructed, and initialised.
	 */
	static CBackupNotifier_CreateAndDestroy_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CBackupNotifier_CreateAndDestroy_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CBackupNotifier_CreateAndDestroy_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CBackupNotifier_CreateAndDestroy_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CBackupNotifier_CreateAndDestroy_UnitTest is fully constructed.
		@post			CBackupNotifier_CreateAndDestroy_UnitTest is fully destroyed.
	*/
	~CBackupNotifier_CreateAndDestroy_UnitTest();

private:
	/**
		@fn				CBackupNotifier_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CBackupNotifier class.
		@pre 			None.
		@post			CBackupNotifier_CreateAndDestroy_UnitTest is fully constructed.
	*/
	inline CBackupNotifier_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave 			KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CBackupNotifier_CreateAndDestroy_UnitTest is fully constructed.
		@post			CBackupNotifier_CreateAndDestroy_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CBackupNotifier class tested by this UnitTest's transitions.
	 */
	CBackupNotifier_UnitTestContext* iUTContext;
	TBackupNotifier_StateAccessor*	iStateAccessor;
	/* C'tor, d'tor, and method transition validators */
	TBackupNotifier_Ctor_TransitionValidator*	iCtorValidator;
	// TBackupNotifier_CreateAndDestroy_TransitionValidator*	iCreateAndDestroyValidator;
	TBackupNotifier_Dtor_TransitionValidator*	iDtorValidator;
	};	// CBackupNotifier_CreateAndDestroy_UnitTest

#endif	// __BACKUPNOTIFIERUNITTEST_H__
