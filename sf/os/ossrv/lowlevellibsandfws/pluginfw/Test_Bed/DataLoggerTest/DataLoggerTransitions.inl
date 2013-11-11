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
// The implementation of the transition classes upon the CDataLogger class methods.
// 
//

// ______________________________________________________________________________
//
_LIT(KDataLoggerNewLTransition,"CDataLogger_NewL_Transition");

inline CDataLogger_NewL_Transition::CDataLogger_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KDataLoggerNewLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDataLogger_NewL_Transition::TransitMethodL()
	{
	_LIT(KDataLoggerNewLTransitMethod, "CDataLogger::NewL transition");
	Context().DataLogger().LogInformation(KDataLoggerNewLTransitMethod);
	Context().iDataLogger = CDataLogger::NewL(Context().iLoggingInfo);
	}

inline CDataLogger_UnitTestContext& CDataLogger_NewL_Transition::Context() const
	{
	return REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDataLoggerDtorTransition,"CDataLogger_Dtor_Transition");

inline CDataLogger_Dtor_Transition::CDataLogger_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDataLoggerDtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDataLogger_Dtor_Transition::TransitMethodL()
	{
	_LIT(KDataLoggerDtorTransitMethod, "CDataLogger::Dtor transition");
	Context().DataLogger().LogInformation(KDataLoggerDtorTransitMethod);
	delete Context().iDataLogger;
	Context().iDataLogger = NULL;
	}

inline CDataLogger_UnitTestContext& CDataLogger_Dtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDataLoggerDumpMemoryBlockTransition,"CDataLogger_DumpMemoryBlock_Transition");

inline CDataLogger_DumpMemoryBlock_Transition::CDataLogger_DumpMemoryBlock_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDataLoggerDumpMemoryBlockTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDataLogger_DumpMemoryBlock_Transition::TransitMethodL()
	{
	_LIT(KDataLoggerDumpMemoryBlockTransitMethod, "CDataLogger::DumpMemoryBlock transition");
	Context().DataLogger().LogInformation(KDataLoggerDumpMemoryBlockTransitMethod);
	Context().iDataLogger->DumpMemoryBlock(Context().iAddress, Context().iLength);
	}

inline CDataLogger_UnitTestContext& CDataLogger_DumpMemoryBlock_Transition::Context() const
	{
	return REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDataLoggerLogInformationTransition,"CDataLogger_LogInformation_Transition");

inline CDataLogger_LogInformation_Transition::CDataLogger_LogInformation_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDataLoggerLogInformationTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDataLogger_LogInformation_Transition::TransitMethodL()
	{
	_LIT(KDataLoggerLogInformationTransitMethod, "CDataLogger::LogInformation transition");
	_LIT(KComment, "This is a wide descriptor comment");
	Context().DataLogger().LogInformation(KDataLoggerLogInformationTransitMethod);
	Context().iDataLogger->LogInformation(KComment);
	}

inline CDataLogger_UnitTestContext& CDataLogger_LogInformation_Transition::Context() const
	{
	return REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext);
	}


// ______________________________________________________________________________
//
_LIT(KDataLoggerLogInformationNarrowTransition,"CDataLogger_LogInformationNarrow_Transition");

inline CDataLogger_LogInformationNarrow_Transition::CDataLogger_LogInformationNarrow_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDataLoggerLogInformationNarrowTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDataLogger_LogInformationNarrow_Transition::TransitMethodL()
	{
	_LIT(KDataLoggerLogInformationNarrowTransitMethod, "CDataLogger::LogInformationNarrow transition");
	_LIT8(KCommentNarrow, "This is a narrow descriptor comment");
	Context().DataLogger().LogInformation(KDataLoggerLogInformationNarrowTransitMethod);
	Context().iDataLogger->LogInformation(KCommentNarrow);
	}

inline CDataLogger_UnitTestContext& CDataLogger_LogInformationNarrow_Transition::Context() const
	{
	return REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDataLoggerLogInformationWithParametersTransition,"CDataLogger_LogInformationWithParameters_Transition");

inline CDataLogger_LogInformationWithParameters_Transition::CDataLogger_LogInformationWithParameters_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDataLoggerLogInformationWithParametersTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDataLogger_LogInformationWithParameters_Transition::TransitMethodL()
	{
	_LIT(KDataLoggerLogInformationWithParametersTransitMethod, "CDataLogger::LogInformationWithParameters transition");
	_LIT(KFormat, "This text will be followed by a parameter: %S");
	Context().DataLogger().LogInformation(KDataLoggerLogInformationWithParametersTransitMethod);
	Context().iDataLogger->LogInformationWithParameters(KFormat, Context().iArg);
	}

inline CDataLogger_UnitTestContext& CDataLogger_LogInformationWithParameters_Transition::Context() const
	{
	return REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDataLoggerLogInformationWithParametersNarrowTransition,"CDataLogger_LogInformationWithParametersNarrow_Transition");

inline CDataLogger_LogInformationWithParametersNarrow_Transition::CDataLogger_LogInformationWithParametersNarrow_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDataLoggerLogInformationWithParametersNarrowTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDataLogger_LogInformationWithParametersNarrow_Transition::TransitMethodL()
	{
	_LIT(KDataLoggerLogInformationWithParametersNarrowTransitMethod, "CDataLogger::LogInformationWithParametersNarrow transition");
	_LIT8(KFormatNarrow, "This text will be followed by a parameter: %S");
	Context().DataLogger().LogInformation(KDataLoggerLogInformationWithParametersNarrowTransitMethod);
	Context().iDataLogger->LogInformationWithParameters(KFormatNarrow, Context().iArgNarrow);
	}

inline CDataLogger_UnitTestContext& CDataLogger_LogInformationWithParametersNarrow_Transition::Context() const
	{
	return REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext);
	}


// ______________________________________________________________________________
//
_LIT(KDataLoggerReportInformationTransition,"CDataLogger_ReportInformation_Transition");

inline CDataLogger_ReportInformation_Transition::CDataLogger_ReportInformation_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDataLoggerReportInformationTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDataLogger_ReportInformation_Transition::TransitMethodL()
	{
	_LIT(KDataLoggerReportInformationTransitMethod, "CDataLogger::ReportInformation transition");
	Context().DataLogger().LogInformation(KDataLoggerReportInformationTransitMethod);
	Context().iDataLogger->ReportInformation(*(Context().iReportComment));
	}

inline CDataLogger_UnitTestContext& CDataLogger_ReportInformation_Transition::Context() const
	{
	return REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext);
	}


// ______________________________________________________________________________
//
_LIT(KDataLoggerReportInformationWithParametersTransition,"CDataLogger_ReportInformationWithParameters_Transition");

inline CDataLogger_ReportInformationWithParameters_Transition::CDataLogger_ReportInformationWithParameters_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDataLoggerReportInformationWithParametersTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDataLogger_ReportInformationWithParameters_Transition::TransitMethodL()
	{
	_LIT(KDataLoggerReportInformationWithParametersTransitMethod, "CDataLogger::ReportInformationWithParameters transition");
	Context().DataLogger().LogInformation(KDataLoggerReportInformationWithParametersTransitMethod);
	Context().iDataLogger->ReportInformationWithParameters(*(Context().iFormatReport), Context().iReportArg);
	}

inline CDataLogger_UnitTestContext& CDataLogger_ReportInformationWithParameters_Transition::Context() const
	{
	return REINTERPRET_CAST(CDataLogger_UnitTestContext&,iUTContext);
	}


// Add additional Transition class Implementations here...

