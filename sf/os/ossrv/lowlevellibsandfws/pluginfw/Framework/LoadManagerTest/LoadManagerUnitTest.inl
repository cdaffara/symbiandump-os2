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
// The implementation of the inlines for the CLoadManager unit tests.
// 
//

// ______________________________________________________________________________
//
_LIT(KLoadManagerCreateAndDestroyUnitTest,"CLoadManager_CreateAndDestroy_UnitTest");

inline TInt CLoadManager_CreateAndDestroy_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iDtorValidator;
	return error;
	}

inline CLoadManager_CreateAndDestroy_UnitTest::~CLoadManager_CreateAndDestroy_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iDtorValidator;
	}

inline CLoadManager_CreateAndDestroy_UnitTest::CLoadManager_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KLoadManagerCreateAndDestroyUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// ______________________________________________________________________________
//
_LIT(KLoadManagerFindInstantiationAndDestroyUnitTest,"CLoadManager_FindInstantiationAndDestroy_UnitTest");

inline TInt CLoadManager_FindInstantiationAndDestroy_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	// delete any validators used
	delete iCtorValidator;
	delete iInstantiationMethodValidator;
	delete iDestroyThisValidator;
	delete iDtorValidator;
	return error;
	}

inline CLoadManager_FindInstantiationAndDestroy_UnitTest::~CLoadManager_FindInstantiationAndDestroy_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	// delete any validators used
	delete iCtorValidator;
	delete iInstantiationMethodValidator;
	delete iDestroyThisValidator;
	delete iDtorValidator;
	}

inline CLoadManager_FindInstantiationAndDestroy_UnitTest::CLoadManager_FindInstantiationAndDestroy_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KLoadManagerFindInstantiationAndDestroyUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// ______________________________________________________________________________
//
_LIT(KLoadManagerFindInstantiationFailureUnitTest,"CLoadManager_FindInstantiationFailure_UnitTest");

inline TInt CLoadManager_FindInstantiationFailure_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	// delete any validators used
	delete iCtorValidator;
	delete iInstantiationMethodValidator;
	delete iDtorValidator;
	return error;
	}

inline CLoadManager_FindInstantiationFailure_UnitTest::~CLoadManager_FindInstantiationFailure_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	// delete any validators used
	delete iCtorValidator;
	delete iInstantiationMethodValidator;
	delete iDtorValidator;
	}

inline CLoadManager_FindInstantiationFailure_UnitTest::CLoadManager_FindInstantiationFailure_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KLoadManagerFindInstantiationFailureUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// ______________________________________________________________________________
//
_LIT(KLoadManagerDefectFOT56ULPMUnitTest,"CLoadManager_DefectFOT56ULPM_UnitTest");

inline TInt CLoadManager_DefectFOT56ULPM_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	// delete any validators used
	delete iCtorValidator;
	delete iInstantiationMethodValidator;
	delete iDestroyThisValidator;
	delete iDtorValidator;
	return error;
	}

inline CLoadManager_DefectFOT56ULPM_UnitTest::~CLoadManager_DefectFOT56ULPM_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	// delete any validators used
	delete iCtorValidator;
	delete iInstantiationMethodValidator;
	delete iDestroyThisValidator;
	delete iDtorValidator;
	}

inline CLoadManager_DefectFOT56ULPM_UnitTest::CLoadManager_DefectFOT56ULPM_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KLoadManagerDefectFOT56ULPMUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

