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
// The implementation of the transition classes upon the CDefaultLogOutput class methods.
// 
//

// ______________________________________________________________________________
//
_LIT(KDefaultLogOutputNewLTransition,"CDefaultLogOutput_NewL_Transition");

inline CDefaultLogOutput_NewL_Transition::CDefaultLogOutput_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KDefaultLogOutputNewLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDefaultLogOutput_NewL_Transition::TransitMethodL()
	{
	_LIT(KDefaultLogOutputNewLTransitMethod, "CDefaultLogOutput::NewL transition");
	_LIT(KTitle, "TestLogFile.html");
	Context().DataLogger().LogInformation(KDefaultLogOutputNewLTransitMethod);
	Context().iDefaultLogOutput = new(ELeave) CDefaultLogOutput(KTitle);
	}

inline CDefaultLogOutput_UnitTestContext& CDefaultLogOutput_NewL_Transition::Context() const
	{
	return REINTERPRET_CAST(CDefaultLogOutput_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDefaultLogOutputDtorTransition,"CDefaultLogOutput_Dtor_Transition");

inline CDefaultLogOutput_Dtor_Transition::CDefaultLogOutput_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDefaultLogOutputDtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDefaultLogOutput_Dtor_Transition::TransitMethodL()
	{
	_LIT(KDefaultLogOutputDtorTransitMethod, "CDefaultLogOutput::Dtor transition");
	Context().DataLogger().LogInformation(KDefaultLogOutputDtorTransitMethod);
	delete Context().iDefaultLogOutput;
	Context().iDefaultLogOutput = NULL;
	}

inline CDefaultLogOutput_UnitTestContext& CDefaultLogOutput_Dtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CDefaultLogOutput_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDefaultLogOutputOpenLTransition,"CDefaultLogOutput_OpenL_Transition");

inline CDefaultLogOutput_OpenL_Transition::CDefaultLogOutput_OpenL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KDefaultLogOutputOpenLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDefaultLogOutput_OpenL_Transition::TransitMethodL()
	{
	_LIT(KDefaultLogOutputOpenLTransitMethod, "CDefaultLogOutput::OpenL transition");
	Context().DataLogger().LogInformation(KDefaultLogOutputOpenLTransitMethod);
	Context().iDefaultLogOutput->OpenL();
	}

inline CDefaultLogOutput_UnitTestContext& CDefaultLogOutput_OpenL_Transition::Context() const
	{
	return REINTERPRET_CAST(CDefaultLogOutput_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDefaultLogOutputCloseTransition,"CDefaultLogOutput_Close_Transition");

inline CDefaultLogOutput_Close_Transition::CDefaultLogOutput_Close_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDefaultLogOutputCloseTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDefaultLogOutput_Close_Transition::TransitMethodL()
	{
	_LIT(KDefaultLogOutputCloseTransitMethod, "CDefaultLogOutput::Close transition");
	Context().DataLogger().LogInformation(KDefaultLogOutputCloseTransitMethod);
	Context().iDefaultLogOutput->Close();
	}

inline CDefaultLogOutput_UnitTestContext& CDefaultLogOutput_Close_Transition::Context() const
	{
	return REINTERPRET_CAST(CDefaultLogOutput_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDefaultLogOutputWriteTransition,"CDefaultLogOutput_Write_Transition");

inline CDefaultLogOutput_Write_Transition::CDefaultLogOutput_Write_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDefaultLogOutputWriteTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDefaultLogOutput_Write_Transition::TransitMethodL()
	{
	_LIT(KDefaultLogOutputWriteTransitMethod, "CDefaultLogOutput::Write transition");
	Context().DataLogger().LogInformation(KDefaultLogOutputWriteTransitMethod);
	Context().iDefaultLogOutput->Write(*(Context().iOutput));
	}

inline CDefaultLogOutput_UnitTestContext& CDefaultLogOutput_Write_Transition::Context() const
	{
	return REINTERPRET_CAST(CDefaultLogOutput_UnitTestContext&,iUTContext);
	}

// Add additional Transition class Implementations here...

