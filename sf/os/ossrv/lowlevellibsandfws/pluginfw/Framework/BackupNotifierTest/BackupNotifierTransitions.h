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
// The definitions of the transition classes upon the CBackupNotifier class methods.
// 
//

#ifndef __BACKUPNOTIFIERTRANSITIONS_H__
#define __BACKUPNOTIFIERTRANSITIONS_H__

#include "TestUtilities.h"
#include "StateAccessor.h"
#include "LeakTestTransition.h"
#include <test_bed/datalogger.h>
#include "BackupNotifier.h"
#include "BackupNotifierObserver.h"
#include "BackupNotifierStateAccessor.h"
#include <test_bed/unittest.h>

#define CTransitionType CLeakTestTransition

class CNotificationStub : public CBase, public MBackupNotifierObserver
	{
public:
	/**
		@fn				CNotificationStub(CDataLogger& aDataLogger)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@pre 			None.
		@post			CNotificationStub is fully constructed, and initialised.
	*/
	inline CNotificationStub(CDataLogger& aDataLogger);

	/**
		@fn				~CNotificationStub()
		Intended Usage	: Default Destructor
		@since			7.0
		@pre 			CNotificationStub is fully constructed.
		@post			CNotificationStub is fully destroyed
	 */
	virtual inline ~CNotificationStub();

	/**
		@fn				Suspend()
		Intended Usage	: Overload of the MBackupNotifierObserver method.
		Error Condition	: None.
		@since			7.0
		@pre 			CNotificationStub has been fully constructed.
		@post			Suspend message is logged.
	 */
	virtual inline TInt Suspend();

	/**
		@fn				Resume()
		Intended Usage	: Overload of the MBackupNotifierObserver method.
		Error Condition	: None.
		@since			7.0
		@pre 			CNotificationStub has been fully constructed.
		@post			Resume message is logged.
	 */
	virtual inline TInt Resume();

private:
	CDataLogger& iDataLogger;
	};

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the CBackupNotifier specific
	parameters and behaviour on the CBackupNotifier
	test class for a transition.
 */
class CBackupNotifier_UnitTestContext : public CUnitTestContext
	{
public:
	/**
		@fn				CBackupNotifier_UnitTestContext(CDataLogger& aDataLogger,
														MStateAccessor& aStateAccessor,
														MTransitionObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest's Transitions.
		@param			aStateAccessor WhiteBox state access to the CBackupNotifier class under test.
		@pre 			None.
		@post			CBackupNotifier_UnitTestContext is fully constructed, and initialised.
	*/
	inline CBackupNotifier_UnitTestContext(CDataLogger& aDataLogger,
										MStateAccessor& aStateAccessor,
										MTransitionObserver& aObserver);

	/**
		@fn				~CBackupNotifier_UnitTestContext()
		Intended Usage	: Default Destructor
		@since			7.0
		@pre 			CBackupNotifier_UnitTestContext is fully constructed.
		@post			CBackupNotifier_UnitTestContext is fully destroyed
	 */
		virtual inline ~CBackupNotifier_UnitTestContext();

	/** The instance of the class under test */
	CBackupNotifier* iBackupNotifier;
	
	/** The instance of the stubbed observer of the class under test */
	CNotificationStub* iStub;
	}; // CBackupNotifier_UnitTestContext

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CBackupNotifier::NewL method.
 */
class CBackupNotifier_NewL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CBackupNotifier_NewL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CBackupNotifier_NewL_Transition is fully constructed.
	*/
	CBackupNotifier_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CBackupNotifier::NewL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, (@see CBackupNotifier::NewL)
		@since			7.0
		@return			None
		@pre 			CBackupNotifier_NewL_Transition is fully constructed.
		@post			No change in the CBackupNotifier_NewL_Transition apart
						from iBackupNotifier, which may have changed state.
						(@see CBackupNotifier::NewL post-condition) for iBackupNotifier's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CBackupNotifier_UnitTestContext
		@pre 			iUTContext is a valid CBackupNotifier_UnitTestContext.
		@post			No change in the CBackupNotifier_NewL_Transition
	*/
	inline CBackupNotifier_UnitTestContext& Context() const;
	};	// CBackupNotifier_NewL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CBackupNotifier::Dtor method.
 */
class CBackupNotifier_Dtor_Transition : public CTransitionType
	{
public:
	/**
		@fn				CBackupNotifier_Dtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CBackupNotifier_Dtor_Transition is fully constructed.
	*/
	CBackupNotifier_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CBackupNotifier::Dtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, (@see CBackupNotifier::Dtor)
		@since			7.0
		@return			None
		@pre 			CBackupNotifier_Dtor_Transition is fully constructed.
		@post			No change in the CBackupNotifier_Dtor_Transition apart
						from iBackupNotifier, which may have changed state.
						(@see CBackupNotifier::Dtor post-condition) for iBackupNotifier's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CBackupNotifier_UnitTestContext
		@pre 			iUTContext is a valid CBackupNotifier_UnitTestContext.
		@post			No change in the CBackupNotifier_Dtor_Transition
	*/
	inline CBackupNotifier_UnitTestContext& Context() const;
	};	// CBackupNotifier_Dtor_Transition

// Add additional Transition class definitions here...

#include "BackupNotifierTransitions.inl"

#include "BackupNotifierUnitTestContext.inl"

#endif // __BACKUPNOTIFIERTRANSITIONS_H__

