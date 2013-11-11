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
// The unit test class implementations for the RStringPool class.
// 
//

#ifdef __WINS__
#pragma warning(disable : 4097)			// disable typedef warning
#endif

#include "StringPoolUnitTest.h"



// ______________________________________________________________________________
//
_LIT(KStringPoolCaseInsensitiveUnitTest,"CStringPool_CaseInsensitive_UnitTest");

CStringPool_CaseInsensitive_UnitTest* CStringPool_CaseInsensitive_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CStringPool_CaseInsensitive_UnitTest* self = 
					new(ELeave) CStringPool_CaseInsensitive_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CStringPool_CaseInsensitive_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	return error;
	}

inline CStringPool_CaseInsensitive_UnitTest::~CStringPool_CaseInsensitive_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	}

inline CStringPool_CaseInsensitive_UnitTest::CStringPool_CaseInsensitive_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KStringPoolCaseInsensitiveUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CStringPool_CaseInsensitive_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TStringPool_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CStringPool_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	iTrueValidator = new(ELeave) TStringPool_True_TransitionValidator(*iUTContext);
	iFalseValidator = new(ELeave) TStringPool_False_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CStringPool_NewL_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FAeqA_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FAneA_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FAneB_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FAeqB_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FAeqD_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FAneD_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FDesc_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FCopy_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FCDesC_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FClose_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FStringToken_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FCDesC_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Dtor_Transition(*iUTContext,*iFalseValidator));
	}


// ______________________________________________________________________________
//
_LIT(KStringPoolSmallTableUnitTest,"CStringPool_ShortTable_UnitTest");

CStringPool_ShortTable_UnitTest* CStringPool_ShortTable_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CStringPool_ShortTable_UnitTest* self = 
					new(ELeave) CStringPool_ShortTable_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CStringPool_ShortTable_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	return error;
	}

inline CStringPool_ShortTable_UnitTest::~CStringPool_ShortTable_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	}

inline CStringPool_ShortTable_UnitTest::CStringPool_ShortTable_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KStringPoolSmallTableUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CStringPool_ShortTable_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TStringPool_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CStringPool_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	iTrueValidator = new(ELeave) TStringPool_True_TransitionValidator(*iUTContext);
	iFalseValidator = new(ELeave) TStringPool_False_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CStringPool_ShortCtor_Transition(*iUTContext,*iFalseValidator));
	// testing  defect fixes :  BEU-55DJG3, KRN-56NDEZ,APY-57TEH3,  HAD-57SK27
	AddTransitionL(new(ELeave)CStringPool_BEU55DJG3_Transition(*iUTContext,*iTrueValidator));	
	AddTransitionL(new(ELeave)CStringPool_KRN56NDEZ_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_APY57TEH3_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_HAD57SK27_Transition(*iUTContext,*iTrueValidator));

	// end defect fix tests
	AddTransitionL(new(ELeave)CStringPool_FAeqA_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FAneA_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FAneB_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FAeqB_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FAeqD_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FAneD_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FDesc_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FCopy_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FCDesC_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FClose_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FInt_Transition(*iUTContext,*iTrueValidator));
	
	AddTransitionL(new(ELeave)CStringPool_StringF_Index_Table_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FStringToken_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FCDesC_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Dtor_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Short_CB_Transition(*iUTContext,*iTrueValidator));
	
	}


// ______________________________________________________________________________
//
_LIT(KStringPoolIrrelevantTableUnitTest,"CStringPool_IrrelevantTable_UnitTest");

CStringPool_IrrelevantTable_UnitTest* CStringPool_IrrelevantTable_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CStringPool_IrrelevantTable_UnitTest* self = 
					new(ELeave) CStringPool_IrrelevantTable_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CStringPool_IrrelevantTable_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	return error;
	}

inline CStringPool_IrrelevantTable_UnitTest::~CStringPool_IrrelevantTable_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	}

inline CStringPool_IrrelevantTable_UnitTest::CStringPool_IrrelevantTable_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KStringPoolIrrelevantTableUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CStringPool_IrrelevantTable_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TStringPool_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CStringPool_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	iTrueValidator = new(ELeave) TStringPool_True_TransitionValidator(*iUTContext);
	iFalseValidator = new(ELeave) TStringPool_False_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CStringPool_IrrelevantCtor_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FAeqA_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FAneA_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FAneB_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FAeqB_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FAeqD_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FAneD_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FDesc_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FCopy_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FCDesC_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FClose_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FInt_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FStringToken_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FCDesC_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Dtor_Transition(*iUTContext,*iFalseValidator));
	}


// ______________________________________________________________________________
//
_LIT(KStringPoolLargeTableUnitTest,"CStringPool_LongTable_UnitTest");

CStringPool_LongTable_UnitTest* CStringPool_LongTable_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CStringPool_LongTable_UnitTest* self = 
					new(ELeave) CStringPool_LongTable_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CStringPool_LongTable_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	return error;
	}

inline CStringPool_LongTable_UnitTest::~CStringPool_LongTable_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	}

inline CStringPool_LongTable_UnitTest::CStringPool_LongTable_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KStringPoolLargeTableUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CStringPool_LongTable_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TStringPool_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CStringPool_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	iTrueValidator = new(ELeave) TStringPool_True_TransitionValidator(*iUTContext);
	iFalseValidator = new(ELeave) TStringPool_False_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CStringPool_LongCtor_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FAeqA_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FAneA_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FAneB_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FAeqB_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FAeqD_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FAneD_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FDesc_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FCopy_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FCDesC_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FClose_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FInt_Transition(*iUTContext,*iTrueValidator));

	AddTransitionL(new(ELeave)CStringPool_StringF_Index_Table_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FStringToken_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_FCDesC_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Dtor_Transition(*iUTContext,*iFalseValidator));
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolCaseSensitiveUnitTest,"CStringPool_CaseSensitive_UnitTest");

CStringPool_CaseSensitive_UnitTest* CStringPool_CaseSensitive_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CStringPool_CaseSensitive_UnitTest* self = 
					new(ELeave) CStringPool_CaseSensitive_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

inline TInt CStringPool_CaseSensitive_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	return error;
	}

inline CStringPool_CaseSensitive_UnitTest::~CStringPool_CaseSensitive_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	}

inline CStringPool_CaseSensitive_UnitTest::CStringPool_CaseSensitive_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KStringPoolCaseSensitiveUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CStringPool_CaseSensitive_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TStringPool_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CStringPool_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	iTrueValidator = new(ELeave) TStringPool_True_TransitionValidator(*iUTContext);
	iFalseValidator = new(ELeave) TStringPool_False_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CStringPool_ShortCSCtor_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_AeqA_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_AneA_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_AneB_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_AeqB_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_AeqD_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_AneD_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Desc_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Copy_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_CDesC_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Close_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Int_Transition(*iUTContext,*iTrueValidator));

	AddTransitionL(new(ELeave)CStringPool_String_Index_Table_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_StringToken_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_CDesC_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Dtor_Transition(*iUTContext,*iFalseValidator));
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolSmallTableCSUnitTest,"CStringPool_ShortTableCS_UnitTest");

CStringPool_ShortTableCS_UnitTest* CStringPool_ShortTableCS_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CStringPool_ShortTableCS_UnitTest* self = 
					new(ELeave) CStringPool_ShortTableCS_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

inline TInt CStringPool_ShortTableCS_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	return error;
	}

inline CStringPool_ShortTableCS_UnitTest::~CStringPool_ShortTableCS_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	}

inline CStringPool_ShortTableCS_UnitTest::CStringPool_ShortTableCS_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KStringPoolSmallTableCSUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CStringPool_ShortTableCS_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TStringPool_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CStringPool_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	iTrueValidator = new(ELeave) TStringPool_True_TransitionValidator(*iUTContext);
	iFalseValidator = new(ELeave) TStringPool_False_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CStringPool_ShortCSCtor_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_AeqA_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_AneA_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_AneB_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_AeqB_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_AeqD_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_AneD_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Desc_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Copy_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_CDesC_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Close_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Int_Transition(*iUTContext,*iTrueValidator));
	
	AddTransitionL(new(ELeave)CStringPool_String_Index_Table_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_StringToken_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_CDesC_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Dtor_Transition(*iUTContext,*iFalseValidator));
	
	
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolIrrelevantTableCSUnitTest,"CStringPool_IrrelevantTableCS_UnitTest");

CStringPool_IrrelevantTableCS_UnitTest* CStringPool_IrrelevantTableCS_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CStringPool_IrrelevantTableCS_UnitTest* self = 
					new(ELeave) CStringPool_IrrelevantTableCS_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

inline TInt CStringPool_IrrelevantTableCS_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	return error;
	}

inline CStringPool_IrrelevantTableCS_UnitTest::~CStringPool_IrrelevantTableCS_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	}

inline CStringPool_IrrelevantTableCS_UnitTest::CStringPool_IrrelevantTableCS_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KStringPoolIrrelevantTableCSUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CStringPool_IrrelevantTableCS_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TStringPool_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CStringPool_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	iTrueValidator = new(ELeave) TStringPool_True_TransitionValidator(*iUTContext);
	iFalseValidator = new(ELeave) TStringPool_False_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CStringPool_IrrelevantCSCtor_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_AeqA_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_AneA_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_AneB_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_AeqB_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_AeqD_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_AneD_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Desc_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Copy_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_CDesC_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Close_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Int_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_StringToken_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_CDesC_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Dtor_Transition(*iUTContext,*iFalseValidator));
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolLargeCSUnitTest,"CStringPool_LongTableCS_UnitTest");

CStringPool_LongTableCS_UnitTest* CStringPool_LongTableCS_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CStringPool_LongTableCS_UnitTest* self = 
					new(ELeave) CStringPool_LongTableCS_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

inline TInt CStringPool_LongTableCS_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	return error;
	}

inline CStringPool_LongTableCS_UnitTest::~CStringPool_LongTableCS_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	}

inline CStringPool_LongTableCS_UnitTest::CStringPool_LongTableCS_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KStringPoolLargeCSUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CStringPool_LongTableCS_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TStringPool_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CStringPool_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	iTrueValidator = new(ELeave) TStringPool_True_TransitionValidator(*iUTContext);
	iFalseValidator = new(ELeave) TStringPool_False_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CStringPool_LargeCSCtor_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_AeqA_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_AneA_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_AneB_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_AeqB_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_AeqD_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_AneD_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Desc_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Copy_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_CDesC_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Close_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Int_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_String_Index_Table_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_StringToken_Transition(*iUTContext,*iTrueValidator));
	AddTransitionL(new(ELeave)CStringPool_CDesC_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_Dtor_Transition(*iUTContext,*iFalseValidator));
	}



// ______________________________________________________________________________
//
_LIT(KStringPoolMultipleTableShortCIUnitTest,"CStringPool_MultipleTableShortCI_UnitTest");

CStringPool_MultipleTableShortCI_UnitTest* CStringPool_MultipleTableShortCI_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CStringPool_MultipleTableShortCI_UnitTest* self = 
					new(ELeave) CStringPool_MultipleTableShortCI_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

inline TInt CStringPool_MultipleTableShortCI_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	return error;
	}

inline CStringPool_MultipleTableShortCI_UnitTest::~CStringPool_MultipleTableShortCI_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	}

inline CStringPool_MultipleTableShortCI_UnitTest::CStringPool_MultipleTableShortCI_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KStringPoolMultipleTableShortCIUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CStringPool_MultipleTableShortCI_UnitTest::ConstructL()
	{
// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TStringPool_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CStringPool_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	iTrueValidator = new(ELeave) TStringPool_True_TransitionValidator(*iUTContext);
	iFalseValidator = new(ELeave) TStringPool_False_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CStringPool_MultipleTableCICtor_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_FAeqB_Transition(*iUTContext,*iTrueValidator));	
	AddTransitionL(new(ELeave)CStringPool_MutlipleCICeqC_Transition(*iUTContext,*iTrueValidator));	
	AddTransitionL(new(ELeave)CStringPool_MultipleCIStringToIndex_Transition(*iUTContext,*iTrueValidator));	
	AddTransitionL(new(ELeave)CStringPool_MultipleCIDynamicString_Transition(*iUTContext,*iTrueValidator));	
	AddTransitionL(new(ELeave)CStringPool_MultipleCIIndexToStringDynamic_Transition(*iUTContext,*iTrueValidator));	

	AddTransitionL(new(ELeave)CStringPool_MultipleTableDtor_Transition(*iUTContext,*iTrueValidator));
	}

// ______________________________________________________________________________
//
_LIT(KStringPoolMultipleTableShortCSUnitTest,"CStringPool_MultipleTableShortCS_UnitTest");

CStringPool_MultipleTableShortCS_UnitTest* CStringPool_MultipleTableShortCS_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CStringPool_MultipleTableShortCS_UnitTest* self = 
					new(ELeave) CStringPool_MultipleTableShortCS_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

inline TInt CStringPool_MultipleTableShortCS_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	return error;
	}

inline CStringPool_MultipleTableShortCS_UnitTest::~CStringPool_MultipleTableShortCS_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iTrueValidator;
	delete iFalseValidator;
	}

inline CStringPool_MultipleTableShortCS_UnitTest::CStringPool_MultipleTableShortCS_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KStringPoolMultipleTableShortCSUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CStringPool_MultipleTableShortCS_UnitTest::ConstructL()
	{
// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TStringPool_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CStringPool_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	iTrueValidator = new(ELeave) TStringPool_True_TransitionValidator(*iUTContext);
	iFalseValidator = new(ELeave) TStringPool_False_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CStringPool_MultipleTableCSCtor_Transition(*iUTContext,*iFalseValidator));
	AddTransitionL(new(ELeave)CStringPool_AeqB_Transition(*iUTContext,*iTrueValidator));	
	AddTransitionL(new(ELeave)CStringPool_MutlipleCSCeqC_Transition(*iUTContext,*iTrueValidator));	
	AddTransitionL(new(ELeave)CStringPool_MultipleCSStringToIndex_Transition(*iUTContext,*iTrueValidator));	
	AddTransitionL(new(ELeave)CStringPool_MultipleCSDynamicString_Transition(*iUTContext,*iTrueValidator));	
	AddTransitionL(new(ELeave)CStringPool_MultipleCSIndexToStringDynamic_Transition(*iUTContext,*iTrueValidator));	

	AddTransitionL(new(ELeave)CStringPool_MultipleTableDtor_Transition(*iUTContext,*iTrueValidator));
	}


