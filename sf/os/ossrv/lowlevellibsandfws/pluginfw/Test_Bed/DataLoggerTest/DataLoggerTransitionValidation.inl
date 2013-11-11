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
// The implementation of the transition validation classes upon the CDataLogger class methods.
// 
//

// ______________________________________________________________________________
//
inline TDataLogger_Ctor_TransitionValidator::TDataLogger_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDataLogger_Ctor_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TDataLogger_Ctor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext).iDataLogger))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDataLogger_Dtor_TransitionValidator::TDataLogger_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDataLogger_Dtor_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext).iDataLogger))
		return EFalse;
	return ETrue;
	}

inline TBool TDataLogger_Dtor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDataLogger_DumpMemoryBlock_TransitionValidator::TDataLogger_DumpMemoryBlock_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDataLogger_DumpMemoryBlock_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext).iDataLogger))
		return EFalse;
	return ETrue;
	}

inline TBool TDataLogger_DumpMemoryBlock_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext).iDataLogger))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDataLogger_LogInformation_TransitionValidator::TDataLogger_LogInformation_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDataLogger_LogInformation_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext).iDataLogger))
		return EFalse;
	return ETrue;
	}

inline TBool TDataLogger_LogInformation_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext).iDataLogger))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDataLogger_LogInformationNarrow_TransitionValidator::TDataLogger_LogInformationNarrow_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDataLogger_LogInformationNarrow_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext).iDataLogger))
		return EFalse;
	return ETrue;
	}

inline TBool TDataLogger_LogInformationNarrow_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext).iDataLogger))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDataLogger_LogInformationWithParameters_TransitionValidator::TDataLogger_LogInformationWithParameters_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDataLogger_LogInformationWithParameters_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext).iDataLogger))
		return EFalse;
	return ETrue;
	}

inline TBool TDataLogger_LogInformationWithParameters_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext).iDataLogger))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDataLogger_LogInformationWithParametersNarrow_TransitionValidator::TDataLogger_LogInformationWithParametersNarrow_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDataLogger_LogInformationWithParametersNarrow_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext).iDataLogger))
		return EFalse;
	return ETrue;
	}

inline TBool TDataLogger_LogInformationWithParametersNarrow_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext).iDataLogger))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDataLogger_ReportInformation_TransitionValidator::TDataLogger_ReportInformation_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDataLogger_ReportInformation_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext).iDataLogger))
		return EFalse;
	return ETrue;
	}

inline TBool TDataLogger_ReportInformation_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext).iDataLogger))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDataLogger_ReportInformationWithParameters_TransitionValidator::TDataLogger_ReportInformationWithParameters_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDataLogger_ReportInformationWithParameters_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext).iDataLogger))
		return EFalse;
	return ETrue;
	}

inline TBool TDataLogger_ReportInformationWithParameters_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext).iDataLogger))
		return EFalse;
	return ETrue;
	}

