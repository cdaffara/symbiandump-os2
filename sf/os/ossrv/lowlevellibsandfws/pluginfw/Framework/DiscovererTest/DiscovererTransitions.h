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
// The transitions over the methods in the CDiscoverer class
// 
//

#ifndef __DISCOVERERTRANSITIONS_H__
#define __DISCOVERERTRANSITIONS_H__

#include "StateAccessor.h"
#include "LeakTestTransition.h"
#include <test_bed/datalogger.h>
#include "Discoverer.h"
#include "DiscovererStateAccessor.h"
#include <test_bed/unittest.h>

class CObserverStub;

#define CTransitionType CLeakTestTransition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Provide all the CreateAndDestroyUnit Test's specific
	parameters and behaviour on the CDiscoverer
	test class for a transition.
 */
class CDiscoverer_UnitTestContext : public CUnitTestContext
	{
public:
	/**
		@fn				CDiscoverer_UnitTestContext(CDataLogger& aDataLogger,
														MStateAccessor& aStateAccessor,
														MTransitionObserver& aObserver,
														RFs& aFs)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest's Transitions.
		@param			aStateAccessor WhiteBox state access to the CDiscoverer class under test.
		@param			aFs A reference to an connected file server instance.
		@pre 			None.
		@post			CDiscoverer_UnitTestContext is fully constructed, and initialised.
	*/
	CDiscoverer_UnitTestContext(CDataLogger& aDataLogger,
								MStateAccessor& aStateAccessor,
								MTransitionObserver& aObserver,
								RFs& aFs);

	/**
		@fn				CDiscoverer_UnitTestContext::~CDiscoverer_UnitTestContext()
		Intended Usage	: Default Destructor	
		@since			7.0
		@pre 			CDiscoverer_UnitTestContext is fully constructed
		@post			CDiscoverer_UnitTestContext is fully destroyed
	 */
	virtual ~CDiscoverer_UnitTestContext();

	/**
		@fn				DiscovererObserver()
		Intended Usage	: Discoverer Observer acessor	
		@since			7.0
		@return			MDiscovererObserver& A reference to the observer interface 
						to be used for this unit test
		@pre 			CDiscoverer_UnitTestContext is fully constructed
		@post			CDiscoverer_UnitTestContext is fully destroyed
	 */
	MDiscovererObserver& DiscovererObserver();

	/** The instance of the class under test */
	CDiscoverer* iDiscoverer;
	/** A reference to the observer stub of the CDiscoverer */
	CObserverStub* iDiscovererObserver;
	/** A reference to an connected file server instance. */
	RFs& iFs;
	/** A Dir object referring to the discovery directory file list */
	CDir* iDir;
	/** A parse referring to a DLL's file path */
	TParse iPath;
	/** A modified TEntry referring to a DLL */
	TEntry* iReturnedFileEntry;
	/** A drive letter referring to the mounted / unmounted drive */
	TDriveUnit iDriveUnit;
	/** A status pointer for signalling async completion from the stub routines */
	TRequestStatus* iNotificationStatus;
	}; // CDiscoverer_UnitTestContext

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CDiscoverer::NewL method.
 */
class CDiscoverer_NewL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CDiscoverer_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			Discoverer_CreateAndDestroy_UnitTestContext 
						The parameters to use for test purposes. 
		@pre 			None.
		@post			CDiscoverer_NewL_Transition is fully constructed.
	*/
	inline CDiscoverer_NewL_Transition(CUnitTestContext& aUTContext,
								TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDiscoverer::NewL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, (@see CDiscoverer::NewL)
		@since			7.0
		@return			None
		@pre 			CDiscoverer_NewL_Transition is fully constructed.
		@post			No change in the CDiscoverer_NewL_Transition apart
						from iDiscoverer, which may have changed state.
						(@see CDiscoverer::NewL post-condition) for iDiscoverer's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				inline CDiscoverer_UnitTestContext& Context() const
		Intended Usage	: Obtain the context for this unit test
		@since			7.0
		@return			CDiscoverer_UnitTestContext& The unit test context object.
		@pre 			CDiscoverer_NewL_Transition is fully constructed.
	 */
	inline CDiscoverer_UnitTestContext& Context() const;
	};	// CDiscoverer_NewL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CDiscoverer::Dtor method.

 */
class CDiscoverer_Dtor_Transition : public CTransition
	{
public:
	/**
		@fn				CDiscoverer_Dtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			Discoverer_CreateAndDestroy_UnitTestContext 
						The parameters to use for test purposes. 
		@pre 			None.
		@post			CDiscoverer_Dtor_Transition is fully constructed.
	*/
	inline CDiscoverer_Dtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDiscoverer::Dtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, (@see CDiscoverer::Dtor)
		@since			7.0
		@return			None
		@pre 			CDiscoverer_Dtor_Transition is fully constructed.
		@post			No change in the CDiscoverer_Dtor_Transition apart
						from iDiscoverer, which may have changed state.
						(@see CDiscoverer::Dtor post-condition) for iDiscoverer's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				inline CDiscoverer_UnitTestContext& Context() const
		Intended Usage	: Obtain the context for this unit test
		@since			7.0
		@return			CDiscoverer_UnitTestContext& The unit test context object.
		@pre 			CDiscoverer_Dtor_Transition is fully constructed.
	 */
	inline CDiscoverer_UnitTestContext& Context() const;
	};	// CDiscoverer_Dtor_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CDiscoverer::Resume method.
 */
class CDiscoverer_Resume_Transition : public CTransition
	{
public:
	/**
		@fn				CDiscoverer_Resume_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDiscoverer_Resume_Transition is fully constructed.
	*/
	inline CDiscoverer_Resume_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDiscoverer::Resume method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, (@see CDiscoverer::Resume)
		@since			7.0
		@return			None
		@pre 			CDiscoverer_Resume_Transition is fully constructed.
		@post			No change in the CDiscoverer_Resume_Transition apart
						from iDiscoverer, which may have changed state.
						(@see CDiscoverer::Resume post-condition) for iDiscoverer's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDiscoverer_UnitTestContext
		@pre 			iUTContext is a valid CDiscoverer_UnitTestContext.
		@post			No change in the CDiscoverer_Resume_Transition
	*/
	inline CDiscoverer_UnitTestContext& Context() const;
	};	// CDiscoverer_Resume_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CDiscoverer::Suspend method.
 */
class CDiscoverer_Suspend_Transition : public CTransition
	{
public:
	/**
		@fn				CDiscoverer_Suspend_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDiscoverer_Suspend_Transition is fully constructed.
	*/
	inline CDiscoverer_Suspend_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDiscoverer::Suspend method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, (@see CDiscoverer::Suspend)
		@since			7.0
		@return			None
		@pre 			CDiscoverer_Suspend_Transition is fully constructed.
		@post			No change in the CDiscoverer_Suspend_Transition apart
						from iDiscoverer, which may have changed state.
						(@see CDiscoverer::Suspend post-condition) for iDiscoverer's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDiscoverer_UnitTestContext
		@pre 			iUTContext is a valid CDiscoverer_UnitTestContext.
		@post			No change in the CDiscoverer_Suspend_Transition
	*/
	inline CDiscoverer_UnitTestContext& Context() const;
	};	// CDiscoverer_Suspend_Transition

// CDiscoverer internal transitions

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CDiscoverer::Notification method.

 */
class CDiscoverer_Notification_Transition : public CTransition
	{
public:
	/**
		@fn				CDiscoverer_Notification_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDiscoverer_Notification_Transition is fully constructed.
	*/
	inline CDiscoverer_Notification_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDiscoverer::Notification method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, (@see CDiscoverer::Notification)
		@since			7.0
		@return			None
		@pre 			CDiscoverer_Notification_Transition is fully constructed.
		@post			No change in the CDiscoverer_Notification_Transition apart
						from iDiscoverer, which may have changed state.
						(@see CDiscoverer::Notification post-condition) for iDiscoverer's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDiscoverer_UnitTestContext
		@pre 			iUTContext is a valid CDiscoverer_UnitTestContext.
		@post			No change in the CDiscoverer_Notification_Transition
	*/
	inline CDiscoverer_UnitTestContext& Context() const;
	};	// CDiscoverer_Notification_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CDiscoverer::ScanDirectoryL method.

 */
class CDiscoverer_ScanDirectoryL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CDiscoverer_ScanDirectoryL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDiscoverer_ScanDirectoryL_Transition is fully constructed.
	*/
	CDiscoverer_ScanDirectoryL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDiscoverer::ScanDirectoryL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, (@see CDiscoverer::ScanDirectoryL)
		@since			7.0
		@return			None
		@pre 			CDiscoverer_ScanDirectoryL_Transition is fully constructed.
		@post			No change in the CDiscoverer_ScanDirectoryL_Transition apart
						from iDiscoverer, which may have changed state.
						(@see CDiscoverer::ScanDirectoryL post-condition) for iDiscoverer's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDiscoverer_UnitTestContext
		@pre 			iUTContext is a valid CDiscoverer_UnitTestContext.
		@post			No change in the CDiscoverer_ScanDirectoryL_Transition
	*/
	inline CDiscoverer_UnitTestContext& Context() const;
	};	// CDiscoverer_ScanDirectoryL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CDiscoverer::ScanDirectoryIncrementL method.

 */
class CDiscoverer_ScanDirectoryIncrementL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CDiscoverer_ScanDirectoryIncrementL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDiscoverer_ScanDirectoryIncrementL_Transition is fully constructed.
	*/
	CDiscoverer_ScanDirectoryIncrementL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDiscoverer::ScanDirectoryIncrementL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, (@see CDiscoverer::ScanDirectoryIncrementL)
		@since			7.0
		@return			None
		@pre 			CDiscoverer_ScanDirectoryIncrementL_Transition is fully constructed.
		@post			No change in the CDiscoverer_ScanDirectoryIncrementL_Transition apart
						from iDiscoverer, which may have changed state.
						(@see CDiscoverer::ScanDirectoryIncrementL post-condition) for iDiscoverer's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDiscoverer_UnitTestContext
		@pre 			iUTContext is a valid CDiscoverer_UnitTestContext.
		@post			No change in the CDiscoverer_ScanDirectoryIncrementL_Transition
	*/
	inline CDiscoverer_UnitTestContext& Context() const;
	};	// CDiscoverer_ScanDirectoryIncrementL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CDiscoverer::ScanDirectoryCancel method.

 */
class CDiscoverer_ScanDirectoryCancel_Transition : public CTransition
	{
public:
	/**
		@fn				CDiscoverer_ScanDirectoryCancel_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDiscoverer_ScanDirectoryCancel_Transition is fully constructed.
	*/
	CDiscoverer_ScanDirectoryCancel_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDiscoverer::ScanDirectoryCancel method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, (@see CDiscoverer::ScanDirectoryCancel)
		@since			7.0
		@return			None
		@pre 			CDiscoverer_ScanDirectoryCancel_Transition is fully constructed.
		@post			No change in the CDiscoverer_ScanDirectoryCancel_Transition apart
						from iDiscoverer, which may have changed state.
						(@see CDiscoverer::ScanDirectoryCancel post-condition) for iDiscoverer's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDiscoverer_UnitTestContext
		@pre 			iUTContext is a valid CDiscoverer_UnitTestContext.
		@post			No change in the CDiscoverer_ScanDirectoryCancel_Transition
	*/
	inline CDiscoverer_UnitTestContext& Context() const;
	};	// CDiscoverer_ScanDirectoryCancel_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CDiscoverer::CompleteNotificationProcessing method.

 */
class CDiscoverer_CompleteNotificationProcessing_Transition : public CTransition
	{
public:
	/**
		@fn				CDiscoverer_CompleteNotificationProcessing_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDiscoverer_CompleteNotificationProcessing_Transition is fully constructed.
	*/
	CDiscoverer_CompleteNotificationProcessing_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDiscoverer::CompleteNotificationProcessing method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, (@see CDiscoverer::CompleteNotificationProcessing)
		@since			7.0
		@return			None
		@pre 			CDiscoverer_CompleteNotificationProcessing_Transition is fully constructed.
		@post			No change in the CDiscoverer_CompleteNotificationProcessing_Transition apart
						from iDiscoverer, which may have changed state.
						(@see CDiscoverer::CompleteNotificationProcessing post-condition) for iDiscoverer's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDiscoverer_UnitTestContext
		@pre 			iUTContext is a valid CDiscoverer_UnitTestContext.
		@post			No change in the CDiscoverer_CompleteNotificationProcessing_Transition
	*/
	inline CDiscoverer_UnitTestContext& Context() const;
	};	// CDiscoverer_CompleteNotificationProcessing_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CDiscoverer::IsValidEntryL method.
 */
class CDiscoverer_IsValidEntryL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CDiscoverer_IsValidEntryL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDiscoverer_IsValidEntryL_Transition is fully constructed.
	*/
	CDiscoverer_IsValidEntryL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDiscoverer::IsValidEntryL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, (@see CDiscoverer::IsValidEntryL)
		@since			7.0
		@return			None
		@pre 			CDiscoverer_IsValidEntryL_Transition is fully constructed.
		@post			No change in the CDiscoverer_IsValidEntryL_Transition apart
						from iDiscoverer, which may have changed state.
						(@see CDiscoverer::IsValidEntryL post-condition) for iDiscoverer's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDiscoverer_UnitTestContext
		@pre 			iUTContext is a valid CDiscoverer_UnitTestContext.
		@post			No change in the CDiscoverer_IsValidEntryL_Transition
	*/
	inline CDiscoverer_UnitTestContext& Context() const;
	};	// CDiscoverer_IsValidEntryL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CDiscoverer::ProcessEntryL method.
 */
class CDiscoverer_ProcessEntryL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CDiscoverer_ProcessEntryL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDiscoverer_ProcessEntryL_Transition is fully constructed.
	*/
	CDiscoverer_ProcessEntryL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDiscoverer::ProcessEntryL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, (@see CDiscoverer::ProcessEntryL)
		@since			7.0
		@return			None
		@pre 			CDiscoverer_ProcessEntryL_Transition is fully constructed.
		@post			No change in the CDiscoverer_ProcessEntryL_Transition apart
						from iDiscoverer, which may have changed state.
						(@see CDiscoverer::ProcessEntryL post-condition) for iDiscoverer's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDiscoverer_UnitTestContext
		@pre 			iUTContext is a valid CDiscoverer_UnitTestContext.
		@post			No change in the CDiscoverer_ProcessEntryL_Transition
	*/
	inline CDiscoverer_UnitTestContext& Context() const;
	};	// CDiscoverer_ProcessEntryL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CDiscoverer::MountDriveL method.
 */
class CDiscoverer_MountDriveL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CDiscoverer_MountDriveL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDiscoverer_MountDriveL_Transition is fully constructed.
	*/
	CDiscoverer_MountDriveL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDiscoverer::MountDriveL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, (@see CDiscoverer::MountDriveL)
		@since			7.0
		@return			None
		@pre 			CDiscoverer_MountDriveL_Transition is fully constructed.
		@post			No change in the CDiscoverer_MountDriveL_Transition apart
						from iDiscoverer, which may have changed state.
						(@see CDiscoverer::MountDriveL post-condition) for iDiscoverer's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDiscoverer_UnitTestContext
		@pre 			iUTContext is a valid CDiscoverer_UnitTestContext.
		@post			No change in the CDiscoverer_MountDriveL_Transition
	*/
	inline CDiscoverer_UnitTestContext& Context() const;
	};	// CDiscoverer_MountDriveL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CDiscoverer::UnmountDrive method.
 */
class CDiscoverer_UnmountDrive_Transition : public CTransition
	{
public:
	/**
		@fn				CDiscoverer_UnmountDrive_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDiscoverer_UnmountDrive_Transition is fully constructed.
	*/
	CDiscoverer_UnmountDrive_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDiscoverer::UnmountDrive method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, (@see CDiscoverer::UnmountDrive)
		@since			7.0
		@return			None
		@pre 			CDiscoverer_UnmountDrive_Transition is fully constructed.
		@post			No change in the CDiscoverer_UnmountDrive_Transition apart
						from iDiscoverer, which may have changed state.
						(@see CDiscoverer::UnmountDrive post-condition) for iDiscoverer's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDiscoverer_UnitTestContext
		@pre 			iUTContext is a valid CDiscoverer_UnitTestContext.
		@post			No change in the CDiscoverer_UnmountDrive_Transition
	*/
	inline CDiscoverer_UnitTestContext& Context() const;
	};	// CDiscoverer_UnmountDrive_Transition


#include "DiscovererTransitions.inl"

#endif		// __DISCOVERERTRANSITIONS_H__
