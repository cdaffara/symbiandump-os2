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
// The implementation of the transition validation classes upon the CComponentInfo class methods.
// 
//

// ______________________________________________________________________________
//
inline TComponentInfo_Ctor_TransitionValidator::TComponentInfo_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TComponentInfo_Ctor_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TComponentInfo_Ctor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if((reinterpret_cast <CComponentInfo_UnitTestContext&> (iUTContext)).iEntryFunc != (reinterpret_cast <CComponentInfo_UnitTestContext&> (iUTContext)).iEntryFuncPostCheck)
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TComponentInfo_Dtor_TransitionValidator::TComponentInfo_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TComponentInfo_Dtor_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CComponentInfo_UnitTestContext&,iUTContext).iComponentInfo))
		return EFalse;
	return ETrue;
	}

inline TBool TComponentInfo_Dtor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TComponentInfo_Entry_TransitionValidator::TComponentInfo_Entry_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TComponentInfo_Entry_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CComponentInfo_UnitTestContext&,iUTContext).iComponentInfo))
		return EFalse;
	return ETrue;
	}

inline TBool TComponentInfo_Entry_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CComponentInfo_UnitTestContext&,iUTContext).iComponentInfo))
		return EFalse;

	if(REINTERPRET_CAST(CComponentInfo_UnitTestContext&,iUTContext).iEntryFunc != REINTERPRET_CAST(CComponentInfo_UnitTestContext&,iUTContext).iEntryFuncPostCheck)
		{
		_LIT(KComponentInfoValidatePostConditions, "***** TComponentInfo_Entry_TransitionValidator::ValidatePostConditions Failed *****");
		iUTContext.DataLogger().LogInformation(KComponentInfoValidatePostConditions);
		return EFalse;
		}
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TComponentInfo_UnitTestsInfo_TransitionValidator::TComponentInfo_UnitTestsInfo_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TComponentInfo_UnitTestsInfo_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CComponentInfo_UnitTestContext&,iUTContext).iComponentInfo))
		return EFalse;
	return ETrue;
	}

inline TBool TComponentInfo_UnitTestsInfo_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CComponentInfo_UnitTestContext&,iUTContext).iComponentInfo))
		return EFalse;

	if(REINTERPRET_CAST(CComponentInfo_UnitTestContext&,iUTContext).iTestInfoArray != REINTERPRET_CAST(CComponentInfo_UnitTestContext&,iUTContext).iTestInfoArrayPostCheck)
		{
		_LIT(KComponentInfoUTValidatePostConditions, "***** TComponentInfo_UnitTestsInfo_TransitionValidator::ValidatePostConditions Failed *****");
		iUTContext.DataLogger().LogInformation(KComponentInfoUTValidatePostConditions);
		return EFalse;
		}
	return ETrue;
	}
