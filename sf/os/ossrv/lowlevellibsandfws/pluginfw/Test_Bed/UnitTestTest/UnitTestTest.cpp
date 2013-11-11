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
// Demonstrates a simple set of derived class implementations to
// test the CUnitTest class using the test bed.
// It may be used as a basis to develop a full test bed dll.
// For support and comment please contact the authors.
// 
//

#include "UnitTestUnitTest.h"
#include "ComponentTester.h"

RTest test(_L("UnitTestTest.cpp"));

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Test the CUnitTest class.
 */
class CUnitTest_ComponentTest : public CComponentTester
	{
public:
	/**
		@fn				NewLC(CDataLogger& aDataLogger,
							 MComponentTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave  		KErrNoMemory
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this component test.
		@return			CUnitTest_ComponentTest* The constructed object.
		@pre 			None.
		@post			CUnitTest_ComponentTest is fully constructed.
	*/
	static CUnitTest_ComponentTest* NewLC(CDataLogger& aDataLogger,
							MComponentTestObserver& aObserver);

private:
	/**
		@fn				CUnitTest_ComponentTest(CDataLogger& aDataLogger,
											MComponentTestObserver& aObserver)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aDataLogger The logging object.
		@param			aObserver The observer of this component test.
		@pre 			None.
		@post			CUnitTest_ComponentTest is fully constructed.
	*/
	inline CUnitTest_ComponentTest(CDataLogger& aDataLogger,
												MComponentTestObserver& aObserver);
	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction,
						to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory.
		@since			7.0
		@return			None
		@pre 			CUnitTest_ComponentTest is fully constructed.
		@post			CUnitTest_ComponentTest is fully initialised.
	*/
	inline void ConstructL();

	};	// CUnitTest_ComponentTest

// ______________________________________________________________________________
//
inline CUnitTest_ComponentTest* CUnitTest_ComponentTest::NewLC(CDataLogger& aDataLogger, MComponentTestObserver& aObserver)
	{
	CUnitTest_ComponentTest* self = new (ELeave) CUnitTest_ComponentTest(aDataLogger, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

inline CUnitTest_ComponentTest::CUnitTest_ComponentTest(CDataLogger& aDataLogger,
									MComponentTestObserver& aObserver)
: CComponentTester(aDataLogger, aObserver)
	{
	// Do nothing here.
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0783
@SYMTestCaseDesc	    Tests for the implementations of CUnitTest class
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for create and destroy operations
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
inline void CUnitTest_ComponentTest::ConstructL()
	{
	// Perform base class initialization
	ComponentTesterConstructL();

	AddUnitTestL(CUnitTest_CreateAndDestroy_UnitTest::NewL(iDataLogger, *this));
//	AddUnitTestL(CUnitTest_ManageTransitions_UnitTest::NewL(iDataLogger, *this));
//	AddUnitTestL(CUnitTest_RunTests_UnitTest::NewL(iDataLogger, *this));
//	AddUnitTestL(CUnitTest_AllOther_UnitTest::NewL(iDataLogger, *this));
	}

// ______________________________________________________________________________
//
EXPORT_C CComponentTester* NewComponentTestLC(CDataLogger& aDataLogger,
									MComponentTestObserver&	aComponentTestObserver)
	{
	return CUnitTest_ComponentTest::NewLC(aDataLogger, aComponentTestObserver);
	}

// ___________________________________________________________________________
//
// This section of the module simply includes the exported test harness template which
// makes this a "whole" CPP file with a E32Main entry point below. The test MMP
// project file can then produce a EXE for the test project instead of a DLL.

#include <ecom/test_bed/testharnesstemplate.h>

GLDEF_C TInt E32Main()
    {
	test.Title();
	test.Start(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0783 "));

	test.End();
	test.Close();

	return E32Main_TestHarness(NewComponentTestLC);
	}

