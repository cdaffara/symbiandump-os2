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
// The implementation of the validation classes for the CResolver tests.
// 
//

// ______________________________________________________________________________
//
inline TDefaultResolver_Ctor_TransitionValidator::TDefaultResolver_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDefaultResolver_Ctor_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TDefaultResolver_Ctor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDefaultResolver_UnitTestContext&,iUTContext).iDefaultResolver))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDefaultResolver_Dtor_TransitionValidator::TDefaultResolver_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDefaultResolver_Dtor_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDefaultResolver_UnitTestContext&,iUTContext).iDefaultResolver))
		return EFalse;
	return ETrue;
	}

inline TBool TDefaultResolver_Dtor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDefaultResolver_Default_TransitionValidator::TDefaultResolver_Default_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDefaultResolver_Default_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDefaultResolver_UnitTestContext&,iUTContext).iDefaultResolver))
		return EFalse;
	return ETrue;
	}

inline TBool TDefaultResolver_Default_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDefaultResolver_UnitTestContext&,iUTContext).iDefaultResolver))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDefaultResolver_IdentifyImplementationL_TransitionValidator::TDefaultResolver_IdentifyImplementationL_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDefaultResolver_IdentifyImplementationL_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDefaultResolver_UnitTestContext&,iUTContext).iDefaultResolver))
		return EFalse;
	return ETrue;
	}

inline TBool TDefaultResolver_IdentifyImplementationL_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	CDefaultResolver_UnitTestContext& context = 
		REINTERPRET_CAST(CDefaultResolver_UnitTestContext&,iUTContext);

	_LIT(KIdentifiedUidMessage, "IdentifyImplmentationL() returned uid 0x%x");
	context.DataLogger().LogInformationWithParameters(KIdentifiedUidMessage, 
													  context.iResolvedImpUid.iUid);

	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDefaultResolver_UnitTestContext&,iUTContext).iDefaultResolver))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDefaultResolver_Match_TransitionValidator::TDefaultResolver_Match_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDefaultResolver_Match_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDefaultResolver_UnitTestContext&,iUTContext).iDefaultResolver))
		return EFalse;
	return ETrue;
	}

inline TBool TDefaultResolver_Match_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	CDefaultResolver_UnitTestContext& context = 
		REINTERPRET_CAST(CDefaultResolver_UnitTestContext&,iUTContext);
	if(context.iMatchResult != context.iExpectedMatchResult)
		{
		// Log that we got the wrong result but carry on because we don't want the test to end
		_LIT(KIncorrectMatchResult, "CDefaultResolver::Match returned incorrect result");
		context.DataLogger().LogInformation(KIncorrectMatchResult);
		}

	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDefaultResolver_UnitTestContext&,iUTContext).iDefaultResolver))
		return EFalse;
	return ETrue;
	}

