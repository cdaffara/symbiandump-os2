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
// The definition of the transition classes for the CRegistryData tests.
// 
//

#ifndef __REGISTRYDATATRANSITIONS_H__
#define __REGISTRYDATATRANSITIONS_H__

#include "RegistryData.h"

#include <test_bed/datalogger.h>
#include "LeakTestTransition.h"
#include "RegistryDataStateAccessor.h"

#define CTransitionType CLeakTestTransition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Provide all the CRegistryData specific
	parameters and behaviour on the CRegistryData
	test class for a transition.
 */
class CRegistryData_UnitTestContext : public CUnitTestContext
	{
public:
	/**
		@fn				CRegistryData_UnitTestContext(CDataLogger& aDataLogger,
														MStateAccessor& aStateAccessor,
														MTransitionObserver& aObserver,
														RFs& aFs)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest's Transitions.
		@param			aStateAccessor WhiteBox state access to the CRegistryData class under test.
		@param			aFs A reference to a connected file server instance.
		@pre 			None.
		@post			CRegistryData_UnitTestContext is fully constructed, and initialised.
	*/
	inline CRegistryData_UnitTestContext(CDataLogger& aDataLogger,
										MStateAccessor& aStateAccessor,
										MTransitionObserver& aObserver,
										RFs& aFs);

	/**
		@fn				~CRegistryData_UnitTestContext()
		Intended Usage	: Default Destructor
		@since			7.0
		@pre 			CRegistryData_UnitTestContext is fully constructed.
		@post			CRegistryData_UnitTestContext is fully destroyed
		*/
		virtual inline ~CRegistryData_UnitTestContext();

	/** The instance of the class under test */
	CRegistryData* iRegistryData;
	/** A reference to an connected file server instance. */
	RFs&						iFs;
	/** The current drive unit identifier */
	TDriveUnit					iDriveUnit;
	/** The pointer to the DllData entry */
	CRegistryData::CDllData*	iDllData;
	/** The updated entry which we use in call to UpdateDllEntry */
	CRegistryData::CDllData*	iNewDllData;
	/** 
		A reference to the implementation entry in the registry : 
	*/
	const CImplementationInformation* iImplementationRef;
	/** 
		A reference to the implementation entry in the registry : 
	*/
	const CImplementationInformation* iSecondImplementationRef;
	/** 
		A list of returned UIDs from the list methods
	 */
	RImplInfoArray* iUidList;
	/** UID for an implementation of an interface */
	TUid	iImplementationUid;
	/** UID to identify a particular interface */
	TUid	iInterfaceUid;
	/** UID to identify a specific dll */
	TUid	iDllUid;
	/** The directory entry data for a dll */
	TEntry	iDllEntry;
	/** The update flag */
	TBool	iUpdate;
	/** Input file stream for registry load testing */
	RFileReadStream  iReadStream;
	/** Output file streams for registry persistence testing */
	RFileWriteStream iWriteStream;
	/** A methods return status code */
	TInt iMethodCompletionStatus;
	/** The drive index returned by IsRegistered... so we know which drive to update on */
	TInt iFoundDriveIndex;
	/** Used by SetEnabledState() - ETrue to enable an implementation, EFalse to disable it. */
	TBool iEnabledState;
	}; // CRegistryData_UnitTestContext

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistryData::NewL method.
 */
class CRegistryData_NewL_Transition : public CTransition
	{
public:
	/**
		@fn				CRegistryData_NewL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistryData_NewL_Transition is fully constructed.
	*/
	CRegistryData_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistryData::NewL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistryData::NewL)
		@since			7.0
		@return			None
		@pre 			CRegistryData_NewL_Transition is fully constructed.
		@post			No change in the CRegistryData_NewL_Transition apart
						from iRegistryData, which may have changed state.
						(@see CRegistryData::NewL post-condition) for iRegistryData's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistryData_UnitTestContext
		@pre 			iUTContext is a valid CRegistryData_UnitTestContext.
		@post			No change in the CRegistryData_NewL_Transition
	*/
	inline CRegistryData_UnitTestContext& Context() const;
	};	// CRegistryData_NewL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistryData::Dtor method.
 */
class CRegistryData_Dtor_Transition : public CTransition
	{
public:
	/**
		@fn				CRegistryData_Dtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistryData_Dtor_Transition is fully constructed.
	*/
	CRegistryData_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistryData::Dtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistryData::Dtor)
		@since			7.0
		@return			None
		@pre 			CRegistryData_Dtor_Transition is fully constructed.
		@post			No change in the CRegistryData_Dtor_Transition apart
						from iRegistryData, which may have changed state.
						(@see CRegistryData::Dtor post-condition) for iRegistryData's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistryData_UnitTestContext
		@pre 			iUTContext is a valid CRegistryData_UnitTestContext.
		@post			No change in the CRegistryData_Dtor_Transition
	*/
	inline CRegistryData_UnitTestContext& Context() const;
	};	// CRegistryData_Dtor_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistryData::AddDllDataL method.
 */
class CRegistryData_AddDllDataL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRegistryData_AddDllDataL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistryData_AddDllDataL_Transition is fully constructed.
	*/
	CRegistryData_AddDllDataL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistryData::AddDllDataL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistryData::AddDllDataL)
		@since			7.0
		@return			None
		@pre 			CRegistryData_AddDllDataL_Transition is fully constructed.
		@post			No change in the CRegistryData_AddDllDataL_Transition apart
						from iRegistryData, which may have changed state.
						(@see CRegistryData::AddDllDataL post-condition) for iRegistryData's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistryData_UnitTestContext
		@pre 			iUTContext is a valid CRegistryData_UnitTestContext.
		@post			No change in the CRegistryData_AddDllDataL_Transition
	*/
	inline CRegistryData_UnitTestContext& Context() const;
	};	// CRegistryData_AddDllDataL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistryData::GetImplementationInformationL method.
 */
class CRegistryData_GetImplementationInformationL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRegistryData_GetImplementationInformationL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistryData_GetImplementationInformationL_Transition is fully constructed.
	*/
	CRegistryData_GetImplementationInformationL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistryData::GetImplementationInformationL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistryData::GetImplementationInformationL)
		@since			7.0
		@return			None
		@pre 			CRegistryData_GetImplementationInformationL_Transition is fully constructed.
		@post			No change in the CRegistryData_GetImplementationInformationL_Transition apart
						from iRegistryData, which may have changed state.
						(@see CRegistryData::GetImplementationInformationL post-condition) for iRegistryData's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistryData_UnitTestContext
		@pre 			iUTContext is a valid CRegistryData_UnitTestContext.
		@post			No change in the CRegistryData_GetImplementationInformationL_Transition
	*/
	inline CRegistryData_UnitTestContext& Context() const;
	};	// CRegistryData_GetImplementationInformationL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent 
	Comments : Transition test of the CRegistryData::IsRegisteredWithDateL method.
 */
class CRegistryData_IsRegisteredWithDateL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRegistryData_IsRegisteredWithDateL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistryData_IsRegisteredWithDateL_Transition is fully constructed.
	*/
	CRegistryData_IsRegisteredWithDateL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistryData::IsRegisteredWithDateL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistryData::IsRegisteredWithDateL)
		@since			7.0
		@return			None
		@pre 			CRegistryData_IsRegisteredWithDateL_Transition is fully constructed.
		@post			No change in the CRegistryData_IsRegisteredWithDateL_Transition apart
						from iRegistryData, which may have changed state.
						(@see CRegistryData::IsRegisteredWithDateL post-condition) for iRegistryData's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistryData_UnitTestContext
		@pre 			iUTContext is a valid CRegistryData_UnitTestContext.
		@post			No change in the CRegistryData_IsRegisteredWithDateL_Transition
	*/
	inline CRegistryData_UnitTestContext& Context() const;
	};	// CRegistryData_IsRegisteredWithDateL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistryData::ListImplementationsL method.
 */
class CRegistryData_ListImplementationsL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRegistryData_ListImplementationsL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistryData_ListImplementationsL_Transition is fully constructed.
	*/
	CRegistryData_ListImplementationsL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistryData::ListImplementationsL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistryData::ListImplementationsL)
		@since			7.0
		@return			None
		@pre 			CRegistryData_ListImplementationsL_Transition is fully constructed.
		@post			No change in the CRegistryData_ListImplementationsL_Transition apart
						from iRegistryData, which may have changed state.
						(@see CRegistryData::ListImplementationsL post-condition) for iRegistryData's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistryData_UnitTestContext
		@pre 			iUTContext is a valid CRegistryData_UnitTestContext.
		@post			No change in the CRegistryData_ListImplementationsL_Transition
	*/
	inline CRegistryData_UnitTestContext& Context() const;
	};	// CRegistryData_ListImplementationsL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistryData::SetEnabledState method.
 */
class CRegistryData_SetEnabledState_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRegistryData_SetEnabledState_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistryData_SetEnabledState_Transition is fully constructed.
	*/
	CRegistryData_SetEnabledState_Transition(CUnitTestContext& aUTContext,
											TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistryData::SetEnabledState method for the 
						test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory
		@since			7.0
		@return			None
		@pre 			CRegistryData_SetEnabledState_Transition is fully constructed.
		@post			No change in the CRegistryData_SetEnabledState_Transition apart
						from iRegistryData, which may have changed state.
						(@see CRegistryData::SetEnabledState post-condition) for iRegistryData's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistryData_UnitTestContext
		@pre 			iUTContext is a valid CRegistryData_UnitTestContext.
		@post			No change in the CRegistryData_MarkDisabled_Transition
	*/
	inline CRegistryData_UnitTestContext& Context() const;
	};	// CRegistryData_MarkDisabled_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistryData::PersistNowL method.
 */
class CRegistryData_PersistNowL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRegistryData_PersistNowL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistryData_PersistNowL_Transition is fully constructed.
	*/
	CRegistryData_PersistNowL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistryData::PersistNowL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistryData::PersistNowL)
		@since			7.0
		@return			None
		@pre 			CRegistryData_PersistNowL_Transition is fully constructed.
		@post			No change in the CRegistryData_PersistNowL_Transition apart
						from iRegistryData, which may have changed state.
						(@see CRegistryData::PersistNowL post-condition) for iRegistryData's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistryData_UnitTestContext
		@pre 			iUTContext is a valid CRegistryData_UnitTestContext.
		@post			No change in the CRegistryData_PersistNowL_Transition
	*/
	inline CRegistryData_UnitTestContext& Context() const;
	};	// CRegistryData_PersistNowL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistryData::TemporaryUninstallL method.
 */
class CRegistryData_TemporaryUninstallL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRegistryData_TemporaryUninstallL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistryData_TemporaryUninstallL_Transition is fully constructed.
	*/
	CRegistryData_TemporaryUninstallL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistryData::TemporaryUninstall method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistryData::TemporaryUninstall)
		@since			7.0
		@return			None
		@pre 			CRegistryData_TemporaryUninstall_Transition is fully constructed.
		@post			No change in the CRegistryData_TemporaryUninstall_Transition apart
						from iRegistryData, which may have changed state.
						(@see CRegistryData::TemporaryUninstall post-condition) for iRegistryData's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistryData_UnitTestContext
		@pre 			iUTContext is a valid CRegistryData_UnitTestContext.
		@post			No change in the CRegistryData_TemporaryUninstall_Transition
	*/
	inline CRegistryData_UnitTestContext& Context() const;
	};	// CRegistryData_TemporaryUninstall_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistryData::UndoTemporaryUninstallL method.
 */
class CRegistryData_UndoTemporaryUninstallL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRegistryData_UndoTemporaryUninstallL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistryData_UndoTemporaryUninstallL_Transition is fully constructed.
	*/
	CRegistryData_UndoTemporaryUninstallL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistryData::UndoTemporaryUninstallL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistryData::UndoTemporaryUninstallL)
		@since			7.0
		@return			None
		@pre 			CRegistryData_UndoTemporaryUninstallL_Transition is fully constructed.
		@post			No change in the CRegistryData_UndoTemporaryUninstallL_Transition apart
						from iRegistryData, which may have changed state.
						(@see CRegistryData::UndoTemporaryUninstallL post-condition) for iRegistryData's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistryData_UnitTestContext
		@pre 			iUTContext is a valid CRegistryData_UnitTestContext.
		@post			No change in the CRegistryData_UndoTemporaryUninstallL_Transition
	*/
	inline CRegistryData_UnitTestContext& Context() const;
	};	// CRegistryData_UndoTemporaryUninstallL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent 
	Comments : Transition test of the CRegistryData::UpdateDllDataL method.
 */
class CRegistryData_UpdateDllDataL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRegistryData_UpdateDllDataL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistryData_UpdateDllDataL_Transition is fully constructed.
	*/
	CRegistryData_UpdateDllDataL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistryData::UpdateDllDataL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistryData::UpdateDllDataL)
		@since			7.0
		@return			None
		@pre 			CRegistryData_UpdateDllDataL_Transition is fully constructed.
		@post			No change in the CRegistryData_UpdateDllDataL_Transition apart
						from iRegistryData, which may have changed state.
						(@see CRegistryData::UpdateDllDataL post-condition) for iRegistryData's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistryData_UnitTestContext
		@pre 			iUTContext is a valid CRegistryData_UnitTestContext.
		@post			No change in the CRegistryData_UpdateDllDataL_Transition
	*/
	inline CRegistryData_UnitTestContext& Context() const;
	};	// CRegistryData_UpdateDllDataL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistryData::BuildIndexesL method.
 */
class CRegistryData_BuildIndexesL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRegistryData_BuildIndexesL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistryData_BuildIndexesL_Transition is fully constructed.
	*/
	CRegistryData_BuildIndexesL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistryData::BuildIndexesL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistryData::BuildIndexesL)
		@since			7.0
		@return			None
		@pre 			CRegistryData_BuildIndexesL_Transition is fully constructed.
		@post			No change in the CRegistryData_BuildIndexesL_Transition apart
						from iRegistryData, which may have changed state.
						(@see CRegistryData::BuildIndexesL post-condition) for iRegistryData's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistryData_UnitTestContext
		@pre 			iUTContext is a valid CRegistryData_UnitTestContext.
		@post			No change in the CRegistryData_BuildIndexesL_Transition
	*/
	inline CRegistryData_UnitTestContext& Context() const;
	};	// CRegistryData_BuildIndexesL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistryData::IndexedFind method.
 */
class CRegistryData_IndexedFind_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRegistryData_IndexedFind_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistryData_IndexedFind_Transition is fully constructed.
	*/
	CRegistryData_IndexedFind_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistryData::IndexedFind method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistryData::IndexedFind)
		@since			7.0
		@return			None
		@pre 			CRegistryData_IndexedFind_Transition is fully constructed.
		@post			No change in the CRegistryData_IndexedFind_Transition apart
						from iRegistryData, which may have changed state.
						(@see CRegistryData::IndexedFind post-condition) for iRegistryData's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistryData_UnitTestContext
		@pre 			iUTContext is a valid CRegistryData_UnitTestContext.
		@post			No change in the CRegistryData_IndexedFind_Transition
	*/
	inline CRegistryData_UnitTestContext& Context() const;
	};	// CRegistryData_IndexedFind_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistryData::RestoreL method.
 */
class CRegistryData_RestoreL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRegistryData_RestoreL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistryData_RestoreL_Transition is fully constructed.
	*/
	CRegistryData_RestoreL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistryData::RestoreL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistryData::RestoreL)
		@since			7.0
		@return			None
		@pre 			CRegistryData_RestoreL_Transition is fully constructed.
		@post			No change in the CRegistryData_RestoreL_Transition apart
						from iRegistryData, which may have changed state.
						(@see CRegistryData::RestoreL post-condition) for iRegistryData's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistryData_UnitTestContext
		@pre 			iUTContext is a valid CRegistryData_UnitTestContext.
		@post			No change in the CRegistryData_RestoreL_Transition
	*/
	inline CRegistryData_UnitTestContext& Context() const;
	};	// CRegistryData_RestoreL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistryData::StoreL method.
 */
class CRegistryData_StoreL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRegistryData_StoreL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistryData_StoreL_Transition is fully constructed.
	*/
	CRegistryData_StoreL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistryData::StoreL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistryData::StoreL)
		@since			7.0
		@return			None
		@pre 			CRegistryData_StoreL_Transition is fully constructed.
		@post			No change in the CRegistryData_StoreL_Transition apart
						from iRegistryData, which may have changed state.
						(@see CRegistryData::StoreL post-condition) for iRegistryData's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistryData_UnitTestContext
		@pre 			iUTContext is a valid CRegistryData_UnitTestContext.
		@post			No change in the CRegistryData_StoreL_Transition
	*/
	inline CRegistryData_UnitTestContext& Context() const;
	};	// CRegistryData_StoreL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistryData::MatchOnUid method.
 */
class CRegistryData_MatchOnUid_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRegistryData_MatchOnUid_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistryData_MatchOnUid_Transition is fully constructed.
	*/
	CRegistryData_MatchOnUid_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistryData::MatchOnUid method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistryData::MatchOnUid)
		@since			7.0
		@return			None
		@pre 			CRegistryData_MatchOnUid_Transition is fully constructed.
		@post			No change in the CRegistryData_MatchOnUid_Transition apart
						from iRegistryData, which may have changed state.
						(@see CRegistryData::MatchOnUid post-condition) for iRegistryData's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistryData_UnitTestContext
		@pre 			iUTContext is a valid CRegistryData_UnitTestContext.
		@post			No change in the CRegistryData_MatchOnUid_Transition
	*/
	inline CRegistryData_UnitTestContext& Context() const;
	};	// CRegistryData_MatchOnUid_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the CRegistryData::DiscoveriesBeginning method.
 */
class CRegistryData_DiscoveriesBeginning_Transition : public CTransition
	{
public:
	/**
		@fn				CRegistryData_DiscoveriesBeginning_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRegistryData_DiscoveriesBeginning_Transition is fully constructed.
	*/
	CRegistryData_DiscoveriesBeginning_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CRegistryData::DiscoveriesBeginning method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CRegistryData::DiscoveriesBeginning)
		@since			7.0
		@return			None
		@pre 			CRegistryData_DiscoveriesBeginning_Transition is fully constructed.
		@post			No change in the CRegistryData_DiscoveriesBeginning_Transition apart
						from iRegistryData, which may have changed state.
						(@see CRegistryData::DiscoveriesBeginning post-condition) for iRegistryData's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CRegistryData_UnitTestContext
		@pre 			iUTContext is a valid CRegistryData_UnitTestContext.
		@post			No change in the CRegistryData_DiscoveriesBeginning_Transition
	*/
	inline CRegistryData_UnitTestContext& Context() const;
	};	// CRegistryData_DiscoveriesBeginning_Transition

#include "RegistryDataTransitions.inl"

#endif		// __REGISTRYDATATRANSITIONS_H__
