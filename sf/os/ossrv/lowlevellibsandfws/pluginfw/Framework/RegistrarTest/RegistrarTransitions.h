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
// The definition of the transition classes on CRegistrar
// 
//

#ifndef __REGISTRARTRANSITIONS_H__
#define __REGISTRARTRANSITIONS_H__

#include "LeakTestTransition.h"

#define CTransitionType CLeakTestTransition

// ______________________________________________________________________________
//
// Literals required by this Context for testing
_LIT(KNotFoundResourceFileName, "Z:\\system\\LIBS\\PLUGINS\\10009DB0.rsc");
_LIT(KResourceFileName, "Z:\\system\\LIBS\\PLUGINS\\10009DB1.rsc");
_LIT(KExampleDllFileName, "Z:\\system\\LIBS\\PLUGINS\\EComExample.dll");
/**
	@internalComponent
	Comments : Provide all the CRegistrar specific
	parameters and behaviour on the CRegistrar
	test class for a transition.
 */
class CRegistrar_UnitTestContext : public CUnitTestContext
	{
public:
	/**
		@fn				CRegistrar_UnitTestContext(CDataLogger&			aDataLogger,
												   MStateAccessor&		aStateAccessor,
												   MTransitionObserver& aObserver,
												   CRegistryData&		aRegistryData,
												   MRegistrarObserver&	aRegistrarObserver,
												   RFs&					aFs)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest's Transitions.
		@param			aStateAccessor WhiteBox state access to the CRegistrar class under test.
		@param			aRegistryData The registry data, used in CRegistrar construction
		@param			aRegistrarObserver The registrar observer, used for notifications.
		@param			aFs File server handle, used in CRegistrar construction
		@pre 			None.
		@post			CRegistrar_UnitTestContext is fully constructed, and initialised.
	*/
	inline CRegistrar_UnitTestContext(CDataLogger&			aDataLogger,
									  MStateAccessor&		aStateAccessor,
									  MTransitionObserver&	aObserver,
									  CRegistryData&		aRegistryData,
									  MRegistrarObserver&	aRegistrarObserver,
									  RFs&					aFs);

	/**
		@fn				~CRegistrar_UnitTestContext()
		Intended Usage	: Default Destructor
		@since			7.0
		@pre 			CRegistrar_UnitTestContext is fully constructed.
		@post			CRegistrar_UnitTestContext is fully destroyed
		*/
		virtual inline ~CRegistrar_UnitTestContext();

/** The instance of the class under test */
	CRegistrar* iRegistrar;
/** The registry data required to construct a CRegistrar object */
	CRegistryData& iRegistryData;
/** The observer of the registrar class */
	MRegistrarObserver& iRegistrarObserver;
/** The file server handle required to construct a CRegistrar object */
	RFs& iFs;
/** The destination for the data discovered during a parse */
	CRegistryData::CDllData* iDllData;
/** Unique Id of an interface implementation */
	TUid	iImplementationUid;
/** Unique Id of an interface */
	TUid	iInterfaceUid;
/** The drive on which interface implementations can be found */
	TDriveUnit	iDriveUnit;
/** Information on a dll which contains interface implementations */
	TEntry		iDllEntry;
/** Stores the return code of the current transition.  Used in post validation. */
	TInt		iReturnErrorCode;
/** Indicates if this is an update operation for ParseRegistrationDataL */
	TBool iUpdate;
/** The entry to update for ParseRegistrationDataL */
	TInt iIndex;
	}; // CRegistrar_UnitTestContext

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistrar::NewL method.
 */
class CRegistrar_NewL_Transition : public CTransition
	{
public:
	/**
		@fn				CRegistrar_NewL_Transition(CUnitTestContext& aUTContext,
												   TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistrar_NewL_Transition is fully constructed.
	*/
	CRegistrar_NewL_Transition(CUnitTestContext& aUTContext,
							   TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistrar::NewL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistrar::NewL)
		@since			7.0
		@return			None
		@pre 			CRegistrar_NewL_Transition is fully constructed.
		@post			No change in the CRegistrar_NewL_Transition apart
						from iRegistrar, which may have changed state.
						(@see CRegistrar::NewL post-condition) for iRegistrar's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistrar_UnitTestContext
		@pre 			iUTContext is a valid CRegistrar_UnitTestContext.
		@post			No change in the CRegistrar_NewL_Transition
	*/
	inline CRegistrar_UnitTestContext& Context() const;
	};	// CRegistrar_NewL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistrar::Dtor method.
 */
class CRegistrar_Dtor_Transition : public CTransition
	{
public:
	/**
		@fn				CRegistrar_Dtor_Transition(CUnitTestContext& aUTContext,
												   TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistrar_Dtor_Transition is fully constructed.
	*/
	CRegistrar_Dtor_Transition(CUnitTestContext& aUTContext,
							   TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistrar::Dtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistrar::Dtor)
		@since			7.0
		@return			None
		@pre 			CRegistrar_Dtor_Transition is fully constructed.
		@post			No change in the CRegistrar_Dtor_Transition apart
						from iRegistrar, which may have changed state.
						(@see CRegistrar::Dtor post-condition) for iRegistrar's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistrar_UnitTestContext
		@pre 			iUTContext is a valid CRegistrar_UnitTestContext.
		@post			No change in the CRegistrar_Dtor_Transition
	*/
	inline CRegistrar_UnitTestContext& Context() const;
	};	// CRegistrar_Dtor_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistrar::DiscoveriesBegin method.
 */
class CRegistrar_DiscoveriesBegin_Transition : public CTransition
	{
public:
	/**
		@fn				CRegistrar_DiscoveriesBegin_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistrar_DiscoveriesBegin_Transition is fully constructed.
	*/
	CRegistrar_DiscoveriesBegin_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistrar::DiscoveriesBegin method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistrar::DiscoveriesBegin)
		@since			7.0
		@return			None
		@pre 			CRegistrar_DiscoveriesBegin_Transition is fully constructed.
		@post			No change in the CRegistrar_DiscoveriesBegin_Transition apart
						from iRegistrar, which may have changed state.
						(@see CRegistrar::DiscoveriesBegin post-condition) for iRegistrar's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistrar_UnitTestContext
		@pre 			iUTContext is a valid CRegistrar_UnitTestContext.
		@post			No change in the CRegistrar_DiscoveriesBegin_Transition
	*/
	inline CRegistrar_UnitTestContext& Context() const;
	};	// CRegistrar_DiscoveriesBegin_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistrar::DiscoveriesComplete method.
 */
class CRegistrar_DiscoveriesComplete_Transition : public CTransition
	{
public:
	/**
		@fn				CRegistrar_DiscoveriesComplete_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistrar_DiscoveriesComplete_Transition is fully constructed.
	*/
	CRegistrar_DiscoveriesComplete_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistrar::DiscoveriesComplete method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistrar::DiscoveriesComplete)
		@since			7.0
		@return			None
		@pre 			CRegistrar_DiscoveriesComplete_Transition is fully constructed.
		@post			No change in the CRegistrar_DiscoveriesComplete_Transition apart
						from iRegistrar, which may have changed state.
						(@see CRegistrar::DiscoveriesComplete post-condition) for iRegistrar's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistrar_UnitTestContext
		@pre 			iUTContext is a valid CRegistrar_UnitTestContext.
		@post			No change in the CRegistrar_DiscoveriesComplete_Transition
	*/
	inline CRegistrar_UnitTestContext& Context() const;
	};	// CRegistrar_DiscoveriesComplete_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistrar::Resume method.
 */
class CRegistrar_Resume_Transition : public CTransition
	{
public:
	/**
		@fn				CRegistrar_Resume_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistrar_Resume_Transition is fully constructed.
	*/
	CRegistrar_Resume_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistrar::Resume method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistrar::Resume)
		@since			7.0
		@return			None
		@pre 			CRegistrar_Resume_Transition is fully constructed.
		@post			No change in the CRegistrar_Resume_Transition apart
						from iRegistrar, which may have changed state.
						(@see CRegistrar::Resume post-condition) for iRegistrar's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistrar_UnitTestContext
		@pre 			iUTContext is a valid CRegistrar_UnitTestContext.
		@post			No change in the CRegistrar_Resume_Transition
	*/
	inline CRegistrar_UnitTestContext& Context() const;
	};	// CRegistrar_Resume_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistrar::Suspend method.
 */
class CRegistrar_Suspend_Transition : public CTransition
	{
public:
	/**
		@fn				CRegistrar_Suspend_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistrar_Suspend_Transition is fully constructed.
	*/
	CRegistrar_Suspend_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistrar::Suspend method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistrar::Suspend)
		@since			7.0
		@return			None
		@pre 			CRegistrar_Suspend_Transition is fully constructed.
		@post			No change in the CRegistrar_Suspend_Transition apart
						from iRegistrar, which may have changed state.
						(@see CRegistrar::Suspend post-condition) for iRegistrar's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistrar_UnitTestContext
		@pre 			iUTContext is a valid CRegistrar_UnitTestContext.
		@post			No change in the CRegistrar_Suspend_Transition
	*/
	inline CRegistrar_UnitTestContext& Context() const;
	};	// CRegistrar_Suspend_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistrar::ParseL method.
 */
class CRegistrar_ParseL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRegistrar_ParseL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistrar_ParseL_Transition is fully constructed.
	*/
	CRegistrar_ParseL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistrar::ParseL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistrar::ParseL)
		@since			7.0
		@return			None
		@pre 			CRegistrar_ParseL_Transition is fully constructed.
		@post			No change in the CRegistrar_ParseL_Transition apart
						from iRegistrar, which may have changed state.
						(@see CRegistrar::ParseL post-condition) for iRegistrar's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistrar_UnitTestContext
		@pre 			iUTContext is a valid CRegistrar_UnitTestContext.
		@post			No change in the CRegistrar_ParseL_Transition
	*/
	inline CRegistrar_UnitTestContext& Context() const;
	};	// CRegistrar_ParseL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistrar::Disable method.
 */
class CRegistrar_Disable_Transition : public CTransition
	{
public:
	/**
		@fn				CRegistrar_Disable_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistrar_Disable_Transition is fully constructed.
	*/
	CRegistrar_Disable_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistrar::Disable method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistrar::Disable)
		@since			7.0
		@return			None
		@pre 			CRegistrar_Disable_Transition is fully constructed.
		@post			No change in the CRegistrar_Disable_Transition apart
						from iRegistrar, which may have changed state.
						(@see CRegistrar::Disable post-condition) for iRegistrar's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistrar_UnitTestContext
		@pre 			iUTContext is a valid CRegistrar_UnitTestContext.
		@post			No change in the CRegistrar_Disable_Transition
	*/
	inline CRegistrar_UnitTestContext& Context() const;
	};	// CRegistrar_Disable_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistrar::DriveReinstatedL method.
 */
class CRegistrar_DriveReinstatedL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRegistrar_DriveReinstatedL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistrar_DriveReinstatedL_Transition is fully constructed.
	*/
	CRegistrar_DriveReinstatedL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistrar::DriveReinstatedL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistrar::DriveReinstatedL)
		@since			7.0
		@return			None
		@pre 			CRegistrar_DriveReinstatedL_Transition is fully constructed.
		@post			No change in the CRegistrar_DriveReinstatedL_Transition apart
						from iRegistrar, which may have changed state.
						(@see CRegistrar::DriveReinstatedL post-condition) for iRegistrar's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistrar_UnitTestContext
		@pre 			iUTContext is a valid CRegistrar_UnitTestContext.
		@post			No change in the CRegistrar_DriveReinstatedL_Transition
	*/
	inline CRegistrar_UnitTestContext& Context() const;
	};	// CRegistrar_DriveReinstatedL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistrar::DriveRemoved method.
 */
class CRegistrar_DriveRemoved_Transition : public CTransition
	{
public:
	/**
		@fn				CRegistrar_DriveRemoved_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistrar_DriveRemoved_Transition is fully constructed.
	*/
	CRegistrar_DriveRemoved_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistrar::DriveRemoved method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistrar::DriveRemoved)
		@since			7.0
		@return			None
		@pre 			CRegistrar_DriveRemoved_Transition is fully constructed.
		@post			No change in the CRegistrar_DriveRemoved_Transition apart
						from iRegistrar, which may have changed state.
						(@see CRegistrar::DriveRemoved post-condition) for iRegistrar's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistrar_UnitTestContext
		@pre 			iUTContext is a valid CRegistrar_UnitTestContext.
		@post			No change in the CRegistrar_DriveRemoved_Transition
	*/
	inline CRegistrar_UnitTestContext& Context() const;
	};	// CRegistrar_DriveRemoved_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistrar::Enable method.
 */
class CRegistrar_Enable_Transition : public CTransition
	{
public:
	/**
		@fn				CRegistrar_Enable_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistrar_Enable_Transition is fully constructed.
	*/
	CRegistrar_Enable_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistrar::Enable method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistrar::Enable)
		@since			7.0
		@return			None
		@pre 			CRegistrar_Enable_Transition is fully constructed.
		@post			No change in the CRegistrar_Enable_Transition apart
						from iRegistrar, which may have changed state.
						(@see CRegistrar::Enable post-condition) for iRegistrar's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistrar_UnitTestContext
		@pre 			iUTContext is a valid CRegistrar_UnitTestContext.
		@post			No change in the CRegistrar_Enable_Transition
	*/
	inline CRegistrar_UnitTestContext& Context() const;
	};	// CRegistrar_Enable_Transition


// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistrar::RegisterDiscoveryL method.
 */
class CRegistrar_RegisterDiscoveryL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRegistrar_RegisterDiscoveryL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistrar_RegisterDiscoveryL_Transition is fully constructed.
	*/
	CRegistrar_RegisterDiscoveryL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistrar::RegisterDiscoveryL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistrar::RegisterDiscoveryL)
		@since			7.0
		@return			None
		@pre 			CRegistrar_RegisterDiscoveryL_Transition is fully constructed.
		@post			No change in the CRegistrar_RegisterDiscoveryL_Transition apart
						from iRegistrar, which may have changed state.
						(@see CRegistrar::RegisterDiscoveryL post-condition) for iRegistrar's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistrar_UnitTestContext
		@pre 			iUTContext is a valid CRegistrar_UnitTestContext.
		@post			No change in the CRegistrar_RegisterDiscoveryL_Transition
	*/
	inline CRegistrar_UnitTestContext& Context() const;
	};	// CRegistrar_RegisterDiscoveryL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistrar::ParseRegistrationDataL method.
 */
class CRegistrar_ParseRegistrationDataL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRegistrar_ParseRegistrationDataL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistrar_ParseRegistrationDataL_Transition is fully constructed.
	*/
	CRegistrar_ParseRegistrationDataL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistrar::ParseRegistrationDataL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, (@see CRegistrar::ParseRegistrationDataL)
		@since			7.0
		@return			None
		@pre 			CRegistrar_ParseRegistrationDataL_Transition is fully constructed.
		@post			No change in the CRegistrar_ParseRegistrationDataL_Transition apart
						from iRegistrar, which may have changed state.
						(@see CRegistrar::ParseRegistrationDataL post-condition) for iRegistrar's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistrar_UnitTestContext
		@pre 			iUTContext is a valid CRegistrar_UnitTestContext.
		@post			No change in the CRegistrar_ParseRegistrationDataL_Transition
	*/
	inline CRegistrar_UnitTestContext& Context() const;
	};	// CRegistrar_ParseRegistrationDataL_Transition


#include "RegistrarTransitions.inl"

#endif		// __REGISTRARTRANSITIONS_H__
