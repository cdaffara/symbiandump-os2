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
// The implementation of the validation classes for the CRegistar tests.
// 
//

// ______________________________________________________________________________
//
inline TRegistrar_Ctor_TransitionValidator::TRegistrar_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistrar_Ctor_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TRegistrar_Ctor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext).iRegistrar))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistrar_Dtor_TransitionValidator::TRegistrar_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistrar_Dtor_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext).iRegistrar))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistrar_Dtor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistrar_Default_TransitionValidator::TRegistrar_Default_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistrar_Default_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext).iRegistrar))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistrar_Default_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext).iRegistrar))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistrar_RegisterDiscoveryL_TransitionValidator::TRegistrar_RegisterDiscoveryL_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistrar_RegisterDiscoveryL_TransitionValidator::ValidatePreConditions()
	{
	// Only pre condition is that object is constructed
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext).iRegistrar))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistrar_RegisterDiscoveryL_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistrar_UnitTestContext&, iUTContext).iRegistrar))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistrar_DriveRemoved_TransitionValidator::TRegistrar_DriveRemoved_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistrar_DriveRemoved_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext).iRegistrar))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistrar_DriveRemoved_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	//Do a list implementations and check that we get a list unavailable error
	CRegistrar_UnitTestContext& context = REINTERPRET_CAST(CRegistrar_UnitTestContext&, iUTContext);

	TRAPD(error, context.iRegistryData.ListImplementationsL(context.iInterfaceUid));
	if(error != KEComErrNoInterfaceIdentified)
		{
		return EFalse;
		}

	if(iUTContext.StateAccessor().InvariantTest(context.iRegistrar))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistrar_CheckRegWhileDiscovering_TransitionValidator::TRegistrar_CheckRegWhileDiscovering_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistrar_CheckRegWhileDiscovering_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CRegistrar_UnitTestContext&,iUTContext).iRegistrar))
		return EFalse;
	return ETrue;
	}

inline TBool TRegistrar_CheckRegWhileDiscovering_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	//Do a list implementations and check that we get a list unavailable error
	CRegistrar_UnitTestContext& context = REINTERPRET_CAST(CRegistrar_UnitTestContext&, iUTContext);

	TRAPD(error, context.iRegistryData.ListImplementationsL(context.iInterfaceUid));
	if(error != KEComErrListCurrentlyUnavailable)
		{
		_LIT(KCheckRegLeaveMessage, "In PostCondition - ListImplementationsL left with %d (expected KEComErrListCurrentlyUnavailable)");
		context.DataLogger().LogInformationWithParameters(KCheckRegLeaveMessage, error);
		return EFalse;
		}

	if(iUTContext.StateAccessor().InvariantTest(context.iRegistrar))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TRegistrar_NewLHasContent_TransitionValidator::TRegistrar_NewLHasContent_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TRegistrar_NewLHasContent_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TRegistrar_NewLHasContent_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// If this validation fails it is probably because there are no registered 
	// interfaces.  This is most likely because the idx file is not valid.

	CRegistrar_UnitTestContext& context = REINTERPRET_CAST(CRegistrar_UnitTestContext&, iUTContext);

	TRAPD(error, context.iRegistryData.ListImplementationsL(context.iInterfaceUid));
	if(error == KEComErrNoInterfaceIdentified)
		{
		// Then the index hasn't been rebuilt so
		return EFalse;
		}

	if(iUTContext.StateAccessor().InvariantTest(context.iRegistrar))
		return EFalse;
	return ETrue;
	}

