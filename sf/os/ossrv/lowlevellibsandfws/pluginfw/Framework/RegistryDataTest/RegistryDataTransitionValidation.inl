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
// The implementation of the validation classes for the CRegistryData tests
// 
//

// ______________________________________________________________________________
//
inline TRegistryData_Ctor_TransitionValidator::TRegistryData_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistryData_Ctor_TransitionValidator::ValidatePreConditions()
	{
	// Always true
	return ETrue;
	}

inline TBool TRegistryData_Ctor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistryData_Dtor_TransitionValidator::TRegistryData_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistryData_Dtor_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistryData_Dtor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistryData_AddDllDataL_TransitionValidator::TRegistryData_AddDllDataL_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistryData_AddDllDataL_TransitionValidator::ValidatePreConditions()
	{
	CRegistryData_UnitTestContext& context = REINTERPRET_CAST(CRegistryData_UnitTestContext&,
															  iUTContext);
	TRegistryData_StateAccessor& accessor = REINTERPRET_CAST(TRegistryData_StateAccessor&,
															 iUTContext.StateAccessor());

	// Check that the drive we are trying to add to is valid
	if(!(accessor.IsRegisteredDrive(*context.iRegistryData, context.iDriveUnit)))
		{
		_LIT(KAddDllFailedPreConditions, 
			 "CRegistryData_AddDllDataL_Transition failed pre-conditions");
		iUTContext.DataLogger().LogInformation(KAddDllFailedPreConditions);
		return EFalse;
		}

	if(accessor.InvariantTest(context.iRegistryData))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistryData_AddDllDataL_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistryData_ListImplementations_TransitionValidator::TRegistryData_ListImplementations_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistryData_ListImplementations_TransitionValidator::ValidatePreConditions()
	{
	// Just do an invariant test because we should be able to call this function any time
	// after CRegistryData has been constructed.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistryData_ListImplementations_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// This post validation checking assumes the use of the CExample plugin implementation
	// It has to because ListImplementationsL takes an interface uid and we give it the 
	// CExampleInterface uid.

	CRegistryData_UnitTestContext& context = REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);

	const TUid KImplementationClassOne = {0x10009DC3};
	const TUid KImplementationClassTwo = {0x10009DC4};
	
	TBool foundOne = EFalse;
	TBool foundTwo = EFalse;
	TInt impCount = context.iUidList->Count();
	for(TInt index = 0; index < impCount; ++index)
		{
		TUid uid = (*(context.iUidList))[index]->ImplementationUid();
		if(uid == KImplementationClassOne)
			foundOne = ETrue;
		else if(uid == KImplementationClassTwo)
			foundTwo = ETrue;
		}
	if(foundOne == EFalse || foundTwo == EFalse)
		return EFalse;

	if(iUTContext.StateAccessor().InvariantTest(context.iRegistryData))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistryData_GetImplementationInformation_TransitionValidator::TRegistryData_GetImplementationInformation_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistryData_GetImplementationInformation_TransitionValidator::ValidatePreConditions()
	{
	// Just do an invariant test because we should be able to call this any time after
	// CRegistryData has been constructed.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistryData_GetImplementationInformation_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// Check that the return from GetImplementationInformationL is what we expect
	_LIT8(KTestDataType, "text/wml");
	CRegistryData_UnitTestContext& context = REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);
	if(context.iImplementationRef == 0 ||
	   context.iImplementationRef->DataType().Find(KTestDataType) == KErrNotFound)
		return EFalse;

	if(iUTContext.StateAccessor().InvariantTest(context.iRegistryData))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistryData_IsRegisteredWithDate_TransitionValidator::TRegistryData_IsRegisteredWithDate_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistryData_IsRegisteredWithDate_TransitionValidator::ValidatePreConditions()
	{
	// No pre condition checking required other than that the object is constructed.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistryData_IsRegisteredWithDate_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// No post condition checking other than that this object's state is OK
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistryData_PersistNow_TransitionValidator::TRegistryData_PersistNow_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistryData_PersistNow_TransitionValidator::ValidatePreConditions()
	{
	// No pre condition checking required other than that the object is correctly constructed
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistryData_PersistNow_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistryData_TemporaryUninstall_TransitionValidator::TRegistryData_TemporaryUninstall_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistryData_TemporaryUninstall_TransitionValidator::ValidatePreConditions()
	{
	// Only pre condition is that object is fully constructed
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistryData_TemporaryUninstall_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	CRegistryData_UnitTestContext& context = REINTERPRET_CAST(CRegistryData_UnitTestContext&,
															  iUTContext);
	TRegistryData_StateAccessor& accessor = REINTERPRET_CAST(TRegistryData_StateAccessor&,
															 iUTContext.StateAccessor());

	// Check that the drive we have uninstalled is not valid
	if(accessor.IsRegisteredDrive(*context.iRegistryData, context.iDriveUnit))
		{
		_LIT(KUninstallFailedPostConditions, 
			 "CRegistryData_TemporaryUninstallL_Transition failed post-conditions");
		iUTContext.DataLogger().LogInformation(KUninstallFailedPostConditions);
		return EFalse;
		}

	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistryData_UndoTemporaryUninstall_TransitionValidator::TRegistryData_UndoTemporaryUninstall_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistryData_UndoTemporaryUninstall_TransitionValidator::ValidatePreConditions()
	{
	// Only pre condition is that object is fully constructed
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistryData_UndoTemporaryUninstall_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	CRegistryData_UnitTestContext& context = REINTERPRET_CAST(CRegistryData_UnitTestContext&,
															  iUTContext);
	TRegistryData_StateAccessor& accessor = REINTERPRET_CAST(TRegistryData_StateAccessor&,
															 iUTContext.StateAccessor());

	// Check that the drive we have uninstalled is not valid
	if(!(accessor.IsRegisteredDrive(*context.iRegistryData, context.iDriveUnit)))
		{
		_LIT(KUndoUninstallFailedPostConditions, 
			 "CRegistryData_UndoTemporaryUninstallL_Transition failed post-conditions");
		iUTContext.DataLogger().LogInformation(KUndoUninstallFailedPostConditions);
		return EFalse;
		}

	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistryData_UpdateDllData_TransitionValidator::TRegistryData_UpdateDllData_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistryData_UpdateDllData_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistryData_UpdateDllData_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistryData_BuildIndexes_TransitionValidator::TRegistryData_BuildIndexes_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistryData_BuildIndexes_TransitionValidator::ValidatePreConditions()
	{
	// Only pre condition is that object is fully constructed (BuildIndexes is actually
	// called from ConstructL anyway)
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistryData_BuildIndexes_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistryData_IndexedFind_TransitionValidator::TRegistryData_IndexedFind_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistryData_IndexedFind_TransitionValidator::ValidatePreConditions()
	{
	// Only pre condition is that object is fully constructed.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistryData_IndexedFind_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// No post condition (function is const)
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistryData_Restore_TransitionValidator::TRegistryData_Restore_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistryData_Restore_TransitionValidator::ValidatePreConditions()
	{
	// Only pre condition is that object is fully constructed.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistryData_Restore_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// The object has been restored from file so just check that it is a valid object
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistryData_Store_TransitionValidator::TRegistryData_Store_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistryData_Store_TransitionValidator::ValidatePreConditions()
	{
	// Only pre condition is that object is fully constructed
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistryData_Store_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// No post condition as function is const
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistryData_MatchOnUid_TransitionValidator::TRegistryData_MatchOnUid_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistryData_MatchOnUid_TransitionValidator::ValidatePreConditions()
	{
	// No pre condition checking because this function is static
	return ETrue;
	}

inline TBool TRegistryData_MatchOnUid_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// No post condition checking because this function is static
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistryData_ConstructPolicyIndexL_TransitionValidator::TRegistryData_ConstructPolicyIndexL_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistryData_ConstructPolicyIndexL_TransitionValidator::ValidatePreConditions()
	{
	// Only pre condition is object is fully constructed
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistryData_ConstructPolicyIndexL_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// No post condition because function is const
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistryData_SetPolicyObserver_TransitionValidator::TRegistryData_SetPolicyObserver_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistryData_SetPolicyObserver_TransitionValidator::ValidatePreConditions()
	{
	// Only pre condition is object is fully constructed
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistryData_SetPolicyObserver_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	CRegistryData_UnitTestContext& context = REINTERPRET_CAST(CRegistryData_UnitTestContext&,
															  iUTContext);
	if(iUTContext.StateAccessor().InvariantTest(context.iRegistryData))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistryData_GetSecondImplInfo_TransitionValidator::TRegistryData_GetSecondImplInfo_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistryData_GetSecondImplInfo_TransitionValidator::ValidatePreConditions()
	{
	CRegistryData_UnitTestContext& context = REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);

	// Copy the result of the earlier GetImplementationInformation so we 
	// can compare later
	context.iSecondImplementationRef = context.iImplementationRef;

	if(iUTContext.StateAccessor().InvariantTest(context.iRegistryData))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistryData_GetSecondImplInfo_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	CRegistryData_UnitTestContext& context = REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);

	// if the second call to GetImplementationInformation does not return the same thing
	// as the first then this test has failed
	if(context.iImplementationRef != context.iSecondImplementationRef)
		return EFalse;

	if(iUTContext.StateAccessor().InvariantTest(context.iRegistryData))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistryData_DiscoveriesBeginning_TransitionValidator::TRegistryData_DiscoveriesBeginning_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistryData_DiscoveriesBeginning_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext).iRegistryData))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistryData_DiscoveriesBeginning_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	CRegistryData_UnitTestContext& context = REINTERPRET_CAST(CRegistryData_UnitTestContext&,iUTContext);
	
	if(context.iRegistryData->IndexValid())
		return EFalse;

	if(iUTContext.StateAccessor().InvariantTest(context.iRegistryData))
		return EFalse;
	return ETrue;
	}

