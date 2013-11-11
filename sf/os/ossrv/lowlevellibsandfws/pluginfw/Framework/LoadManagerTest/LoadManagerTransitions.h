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
// The definition of the transitions on the CLoadManager classes.
// 
//

#ifndef __LOADMANAGERTRANSITIONS_H__
#define __LOADMANAGERTRANSITIONS_H__

#include "StateAccessor.h"
#include "LeakTestTransition.h"
#include <test_bed/datalogger.h>
#include "LoadManager.h"
#include "LoadManagerStateAccessor.h"
#include <unittest.h>
#include "RegistryData.h"

#define CTransitionType CLeakTestTransition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Provide all the CLoadManager specific
	parameters and behaviour on the CLoadManager
	test class for a transition.
 */
class CLoadManager_UnitTestContext : public CUnitTestContext
	{
public:
	/**
		@fn				CLoadManager_UnitTestContext(CDataLogger& aDataLogger,
														MStateAccessor& aStateAccessor,
														MTransitionObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest's Transitions.
		@param			aStateAccessor WhiteBox state access to the CLoadManager class under test.
		@pre 			None.
		@post			CLoadManager_UnitTestContext is fully constructed, and initialised.
	*/
	inline CLoadManager_UnitTestContext(CDataLogger& aDataLogger,
										MStateAccessor& aStateAccessor,
										MTransitionObserver& aObserver);

	/**
		@fn				~CLoadManager_UnitTestContext()
		Intended Usage	: Default Destructor
		@since			7.0
		@pre 			CLoadManager_UnitTestContext is fully constructed.
		@post			CLoadManager_UnitTestContext is fully destroyed
		*/
		virtual inline ~CLoadManager_UnitTestContext();

	/** The instance of the class under test */
	CLoadManager* iLoadManager;
	/** The registry data for the LoadManager to use */
	CRegistryData* iRegistryData;
	/** UID for instantiation test */
	TUid iUniqueImplementationUid;
	/** The load method pointer */
	TProxyNewLPtr iLoadMethod;
	/** Information on the dll to be loaded */
	TEntry	iDllEntry;
	}; // CLoadManager_UnitTestContext

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CLoadManager::NewL method.
	Note: This transition cannot be leak tested because it may legitimately leave 
 	memory allocated after a leave which appears to be a leak but is in fact a class member.

 */
class CLoadManager_NewL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CLoadManager_NewL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CLoadManager_NewL_Transition is fully constructed.
	*/
	CLoadManager_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CLoadManager::NewL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CLoadManager::NewL)
		@since			7.0
		@return			None
		@pre 			CLoadManager_NewL_Transition is fully constructed.
		@post			No change in the CLoadManager_NewL_Transition apart
						from iLoadManager, which may have changed state.
						(@see CLoadManager::NewL post-condition) for iLoadManager's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CLoadManager_UnitTestContext
		@pre 			iUTContext is a valid CLoadManager_UnitTestContext.
		@post			No change in the CLoadManager_NewL_Transition
	*/
	inline CLoadManager_UnitTestContext& Context() const;
	};	// CLoadManager_NewL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CLoadManager::Dtor method.
 */
class CLoadManager_Dtor_Transition : public CTransition
	{
public:
	/**
		@fn				CLoadManager_Dtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CLoadManager_Dtor_Transition is fully constructed.
	*/
	CLoadManager_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CLoadManager::Dtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CLoadManager::Dtor)
		@since			7.0
		@return			None
		@pre 			CLoadManager_Dtor_Transition is fully constructed.
		@post			No change in the CLoadManager_Dtor_Transition apart
						from iLoadManager, which may have changed state.
						(@see CLoadManager::Dtor post-condition) for iLoadManager's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CLoadManager_UnitTestContext
		@pre 			iUTContext is a valid CLoadManager_UnitTestContext.
		@post			No change in the CLoadManager_Dtor_Transition
	*/
	inline CLoadManager_UnitTestContext& Context() const;
	};	// CLoadManager_Dtor_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CLoadManager::InstantiationMethodL method.
 */
class CLoadManager_InstantiationMethodL_Transition : public CTransition
	{
public:
	/**
		@fn				CLoadManager_InstantiationMethodL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CLoadManager_InstantiationMethodL_Transition is fully constructed.
	*/
	CLoadManager_InstantiationMethodL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CLoadManager::InstantiationMethodL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CLoadManager::InstantiationMethodL)
		@since			7.0
		@return			None
		@pre 			CLoadManager_InstantiationMethodL_Transition is fully constructed.
		@post			No change in the CLoadManager_InstantiationMethodL_Transition apart
						from iLoadManager, which may have changed state.
						(@see CLoadManager::InstantiationMethodL post-condition) for iLoadManager's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CLoadManager_UnitTestContext
		@pre 			iUTContext is a valid CLoadManager_UnitTestContext.
		@post			No change in the CLoadManager_InstantiationMethodL_Transition
	*/
	inline CLoadManager_UnitTestContext& Context() const;
	};	// CLoadManager_InstantiationMethodL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CLoadManager::DestroyedThis method.
 */
class CLoadManager_DestroyedThis_Transition : public CTransition
	{
public:
	/**
		@fn				CLoadManager_DestroyedThis_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CLoadManager_DestroyedThis_Transition is fully constructed.
	*/
	CLoadManager_DestroyedThis_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CLoadManager::DestroyedThis method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CLoadManager::DestroyedThis)
		@since			7.0
		@return			None
		@pre 			CLoadManager_DestroyedThis_Transition is fully constructed.
		@post			No change in the CLoadManager_DestroyedThis_Transition apart
						from iLoadManager, which may have changed state.
						(@see CLoadManager::DestroyedThis post-condition) for iLoadManager's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CLoadManager_UnitTestContext
		@pre 			iUTContext is a valid CLoadManager_UnitTestContext.
		@post			No change in the CLoadManager_DestroyedThis_Transition
	*/
	inline CLoadManager_UnitTestContext& Context() const;
	};	// CLoadManager_DestroyedThis_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Special transition which deletes an object and then immediately
	creates a new one to test a condition for defect FOT56ULPM
 */
class CLoadManager_DestroyCreate_Transition : public CTransition
	{
public:
	/**
		@fn				CLoadManager_DestroyCreate_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CLoadManager_DestroyCreate_Transition is fully constructed.
	*/
	CLoadManager_DestroyCreate_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CLoadManager::DestroyedThis method followed by the
						creation of a new object immediately afterward.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CLoadManager::DestroyedThis)
		@since			7.0
		@return			None
		@pre 			CLoadManager_DestroyCreate_Transition is fully constructed.
		@post			No change in the CLoadManager_DestroyCreate_Transition apart
						from iLoadManager, which may have changed state.
						(@see CLoadManager::DestroyedThis post-condition) for iLoadManager's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CLoadManager_UnitTestContext
		@pre 			iUTContext is a valid CLoadManager_UnitTestContext.
		@post			No change in the CLoadManager_DestroyedThis_Transition
	*/
	inline CLoadManager_UnitTestContext& Context() const;
	};	// CLoadManager_DestroyCreate_Transition


#include "LoadManagerTransitions.inl"

#endif // __LOADMANAGERTRANSITIONS_H__
