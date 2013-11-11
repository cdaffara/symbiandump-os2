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
// The unit test class implementations for the CDataLogger class.
// 
//

#include "DataLoggerUnitTest.h"

// ______________________________________________________________________________
//
_LIT(KDataLoggerCreateAndDestroyUnitTest,"CDataLogger_CreateAndDestroy_UnitTest");

CDataLogger_CreateAndDestroy_UnitTest* CDataLogger_CreateAndDestroy_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDataLogger_CreateAndDestroy_UnitTest* self = 
					new(ELeave) CDataLogger_CreateAndDestroy_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CDataLogger_CreateAndDestroy_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	iUTContext = NULL;
	delete iStateAccessor;
	iStateAccessor = NULL;
	/* delete any validators used */
	delete iCtorValidator;
	iCtorValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;
	return error;
	}

inline CDataLogger_CreateAndDestroy_UnitTest::~CDataLogger_CreateAndDestroy_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iDtorValidator;
	}

inline CDataLogger_CreateAndDestroy_UnitTest::CDataLogger_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDataLoggerCreateAndDestroyUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CDataLogger_CreateAndDestroy_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDataLogger_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CDataLogger_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TDataLogger_Ctor_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TDataLogger_Dtor_TransitionValidator(*iUTContext);
	
	AddTransitionL(new(ELeave)CDataLogger_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CDataLogger_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
_LIT(KDataLoggerDumpMemoryBlockUnitTest,"CDataLogger_DumpMemoryBlock_UnitTest");

CDataLogger_DumpMemoryBlock_UnitTest* CDataLogger_DumpMemoryBlock_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDataLogger_DumpMemoryBlock_UnitTest* self = 
					new(ELeave) CDataLogger_DumpMemoryBlock_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CDataLogger_DumpMemoryBlock_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	iUTContext = NULL;
	delete iStateAccessor;
	iStateAccessor = NULL;
	/* delete any validators used */
	delete iCtorValidator;
	iCtorValidator = NULL;
	delete iDumpMemoryBlockValidator;
	iDumpMemoryBlockValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;
	return error;
	}

inline CDataLogger_DumpMemoryBlock_UnitTest::~CDataLogger_DumpMemoryBlock_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iDumpMemoryBlockValidator;
	delete iDtorValidator;
	}

inline CDataLogger_DumpMemoryBlock_UnitTest::CDataLogger_DumpMemoryBlock_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDataLoggerDumpMemoryBlockUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CDataLogger_DumpMemoryBlock_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDataLogger_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CDataLogger_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	// Set the address of the block to be logged as the address of the literal
	_LIT8(KDumpedText, "Text dumped by the DumpMemoryBlock method");
	iUTContext->iAddress = KDumpedText().Ptr();

	// Set the number of bytes to be logged to as the length of the text to be dumped
	TInt byteLength = KDumpedText().Length();
	iUTContext->iLength = byteLength;

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TDataLogger_Ctor_TransitionValidator(*iUTContext);
	iDumpMemoryBlockValidator = new(ELeave) TDataLogger_DumpMemoryBlock_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TDataLogger_Dtor_TransitionValidator(*iUTContext);	
	
	AddTransitionL(new(ELeave)CDataLogger_NewL_Transition(*iUTContext,*iCtorValidator));	
	AddTransitionL(new(ELeave)CDataLogger_DumpMemoryBlock_Transition(*iUTContext,*iDumpMemoryBlockValidator));
	AddTransitionL(new(ELeave)CDataLogger_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
_LIT(KDataLoggerLogInformationUnitTest,"CDataLogger_LogInformation_UnitTest");

CDataLogger_LogInformation_UnitTest* CDataLogger_LogInformation_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDataLogger_LogInformation_UnitTest* self = 
					new(ELeave) CDataLogger_LogInformation_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CDataLogger_LogInformation_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	iUTContext = NULL;
	delete iStateAccessor;
	iStateAccessor = NULL;
	/* delete any validators used */
	delete iCtorValidator;
	iCtorValidator = NULL;
	delete iLogInformationValidator;
	iLogInformationValidator = NULL;
	delete iLogInformationNarrowValidator;
	iLogInformationNarrowValidator =NULL;
	delete iLogInformationWithParametersValidator;
	iLogInformationWithParametersValidator = NULL;
	delete iLogInformationWithParametersNarrowValidator;
	iLogInformationWithParametersNarrowValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;

	return error;
	}

inline CDataLogger_LogInformation_UnitTest::~CDataLogger_LogInformation_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iLogInformationValidator;
	delete iLogInformationNarrowValidator;
	delete iLogInformationWithParametersValidator;
	delete iLogInformationWithParametersNarrowValidator;
	delete iDtorValidator;
	}

inline CDataLogger_LogInformation_UnitTest::CDataLogger_LogInformation_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDataLoggerLogInformationUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CDataLogger_LogInformation_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDataLogger_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CDataLogger_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	// Wide descriptor to be used as passed in parameter in LogInformationWithParameters transition
	_LIT(KTextWide,"Parameter - wide descriptor version");
	HBufC* hptr1 = KTextWide().AllocL(); 
	iUTContext->iArg = hptr1;

	// Narrow descriptor to be used as passed in parameter in LogInformationWithParametersNarrow transition
	_LIT8(KTextNarrow,"Parameter - narrow descriptor version"); 
	HBufC8* hptr2 = KTextNarrow().AllocL();; 
	iUTContext->iArgNarrow = hptr2;

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TDataLogger_Ctor_TransitionValidator(*iUTContext);
	iLogInformationValidator = new(ELeave) TDataLogger_LogInformation_TransitionValidator(*iUTContext);
	iLogInformationNarrowValidator = new(ELeave) TDataLogger_LogInformationNarrow_TransitionValidator(*iUTContext);
	iLogInformationWithParametersValidator = new(ELeave) TDataLogger_LogInformationWithParameters_TransitionValidator(*iUTContext);
	iLogInformationWithParametersNarrowValidator = new(ELeave) TDataLogger_LogInformationWithParametersNarrow_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TDataLogger_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CDataLogger_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CDataLogger_LogInformation_Transition(*iUTContext,*iLogInformationValidator));
	AddTransitionL(new(ELeave)CDataLogger_LogInformationNarrow_Transition(*iUTContext,*iLogInformationNarrowValidator));
	AddTransitionL(new(ELeave)CDataLogger_LogInformationWithParameters_Transition(*iUTContext,*iLogInformationWithParametersValidator));
	AddTransitionL(new(ELeave)CDataLogger_LogInformationWithParametersNarrow_Transition(*iUTContext,*iLogInformationWithParametersNarrowValidator));
	AddTransitionL(new(ELeave)CDataLogger_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
_LIT(KDataLoggerReportInformationUnitTest,"CDataLogger_ReportInformation_UnitTest");

CDataLogger_ReportInformation_UnitTest* CDataLogger_ReportInformation_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDataLogger_ReportInformation_UnitTest* self = 
					new(ELeave) CDataLogger_ReportInformation_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CDataLogger_ReportInformation_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	iUTContext = NULL;
	delete iStateAccessor;
	iStateAccessor = NULL;
	/* delete any validators used */
	delete iCtorValidator;
	iCtorValidator = NULL;
	delete iReportInformationValidator;
	iReportInformationValidator = NULL;
	delete iReportInformationWithParametersValidator;
	iReportInformationWithParametersValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;

	return error;
	}

inline CDataLogger_ReportInformation_UnitTest::~CDataLogger_ReportInformation_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iReportInformationValidator;
	delete iReportInformationWithParametersValidator;
	delete iDtorValidator;
	}

inline CDataLogger_ReportInformation_UnitTest::CDataLogger_ReportInformation_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDataLoggerReportInformationUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CDataLogger_ReportInformation_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDataLogger_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CDataLogger_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	// Text to be inserted into the report by the ReportInformation method
	_LIT(KReportComment, "This is a report comment");
	iUTContext->iReportComment = &KReportComment();

	// The text to be inserted into the report by the ReportInformationWithParameters method
	 _LIT(KFormatReport, "This report text will be followed by a parameter: %S");
	iUTContext->iFormatReport = &KFormatReport();

	// The parameter to be appended to the end of the text above by the ReportInformationWithParameters method
	_LIT(KReportArg, "This is the report parameter");
	iUTContext->iReportArg = &KReportArg();

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TDataLogger_Ctor_TransitionValidator(*iUTContext);
	iReportInformationValidator = new(ELeave) TDataLogger_ReportInformation_TransitionValidator(*iUTContext);
	iReportInformationWithParametersValidator = new(ELeave) TDataLogger_ReportInformationWithParameters_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TDataLogger_Dtor_TransitionValidator(*iUTContext);
		
	AddTransitionL(new(ELeave)CDataLogger_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CDataLogger_ReportInformation_Transition(*iUTContext,*iReportInformationValidator));
	AddTransitionL(new(ELeave)CDataLogger_ReportInformationWithParameters_Transition(*iUTContext,*iReportInformationWithParametersValidator));
	AddTransitionL(new(ELeave)CDataLogger_Dtor_Transition(*iUTContext,*iDtorValidator));
	}


