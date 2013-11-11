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
// Implementations for the transition validation classes for ECom
// 
//

// ______________________________________________________________________________
//
inline TExampleInterface_Ctor_TransitionValidator::TExampleInterface_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TExampleInterface_Ctor_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TExampleInterface_Ctor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	CExampleInterface_UnitTestContext& context = 
		REINTERPRET_CAST(CExampleInterface_UnitTestContext&,iUTContext);
	TExampleInterface_StateAccessor& accessor = 
		REINTERPRET_CAST(TExampleInterface_StateAccessor&, iUTContext.StateAccessor());

	_LIT(KTUidMessage, "Created an implementation with uid = 0x%x");
	TUid implUid = accessor.GetDtorKey(context.iExampleInterface);
	context.DataLogger().LogInformationWithParameters(KTUidMessage, implUid.iUid);
	if(accessor.InvariantTest(context.iExampleInterface))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TExampleInterface_Dtor_TransitionValidator::TExampleInterface_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TExampleInterface_Dtor_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CExampleInterface_UnitTestContext&,iUTContext).iExampleInterface))
		return EFalse;
	return ETrue;
	}

inline TBool TExampleInterface_Dtor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TExampleInterface_ListImplementations_TransitionValidator::TExampleInterface_ListImplementations_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TExampleInterface_ListImplementations_TransitionValidator::ValidatePreConditions()
	{
	// static function so there are no preconditions
	return ETrue;
	}

inline TBool TExampleInterface_ListImplementations_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	CExampleInterface_UnitTestContext& context = REINTERPRET_CAST(CExampleInterface_UnitTestContext&,
																  iUTContext);
	const TInt count = context.iImplementations.Count();
	_LIT(KImplementationsReturned,"List Implementations transition returned %d implementations");
	context.DataLogger().LogInformationWithParameters(KImplementationsReturned,count);
	for(TInt i = 0; i < count; ++i)
		{
		CImplementationInformation* info = (context.iImplementations)[i];
		context.DataLogger().LogInformation(info->DisplayName());
		context.DataLogger().LogInformation(info->DataType());
		_LIT(KVersionLogLine,"Version number = %d");
		context.DataLogger().LogInformationWithParameters(KVersionLogLine,info->Version());
		}
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TExampleInterface_Default_TransitionValidator::TExampleInterface_Default_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TExampleInterface_Default_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CExampleInterface_UnitTestContext&,iUTContext).iExampleInterface))
		return EFalse;
	return ETrue;
	}

inline TBool TExampleInterface_Default_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CExampleInterface_UnitTestContext&,iUTContext).iExampleInterface))
		return EFalse;
	return ETrue;
	}

