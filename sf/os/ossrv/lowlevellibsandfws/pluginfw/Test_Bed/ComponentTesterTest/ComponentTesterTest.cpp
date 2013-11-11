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
// test the CComponentTester class using the test bed.
// It may be used as a basis to develop a full test bed dll.
// For support and comment please contact the authors.
// 
//

#include "ComponentTesterUnitTest.h"
#include "ComponentTester.h"

RTest test(_L("ComponentTesterTest.cpp"));

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Test the CComponentTester class.
 */
class CComponentTester_ComponentTest : public CComponentTester
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
		@return			CComponentTester_ComponentTest* The constructed object.
		@pre 			None.
		@post			CComponentTester_ComponentTest is fully constructed.
	*/
	static CComponentTester_ComponentTest* NewLC(CDataLogger& aDataLogger,
							MComponentTestObserver& aObserver);

private:
	/**
		@fn				CComponentTester_ComponentTest(CDataLogger& aDataLogger,
											MComponentTestObserver& aObserver)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aDataLogger The logging object.
		@param			aObserver The observer of this component test.
		@pre 			None.
		@post			CComponentTester_ComponentTest is fully constructed.
	*/
	inline CComponentTester_ComponentTest(CDataLogger& aDataLogger,
												MComponentTestObserver& aObserver);
	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction,
						to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory.
		@since			7.0
		@return			None
		@pre 			CComponentTester_ComponentTest is fully constructed.
		@post			CComponentTester_ComponentTest is fully initialised.
	*/
	inline void ConstructL();

	};	// CComponentTester_ComponentTest

// ______________________________________________________________________________
//
inline CComponentTester_ComponentTest* CComponentTester_ComponentTest::NewLC(CDataLogger& aDataLogger, MComponentTestObserver& aObserver)
	{
	CComponentTester_ComponentTest* self = new (ELeave) CComponentTester_ComponentTest(aDataLogger, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

inline CComponentTester_ComponentTest::CComponentTester_ComponentTest(CDataLogger& aDataLogger,
									MComponentTestObserver& aObserver)
: CComponentTester(aDataLogger, aObserver)
	{
	// Do nothing here.
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0782
@SYMTestCaseDesc	    Tests for the implementations of CComponentTester class
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for create and destroy operations
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
inline void CComponentTester_ComponentTest::ConstructL()
	{
	// Perform base class initialization
	ComponentTesterConstructL();

	AddUnitTestL(CComponentTester_CreateAndDestroy_UnitTest::NewL(iDataLogger, *this));
//	AddUnitTestL(CComponentTester_ManageUnitTests_UnitTest::NewL(iDataLogger, *this));
//	AddUnitTestL(CComponentTester_RunTests_UnitTest::NewL(iDataLogger, *this));
	}

// ______________________________________________________________________________
//
EXPORT_C CComponentTester* NewComponentTestLC(CDataLogger& aDataLogger,
									MComponentTestObserver&	aComponentTestObserver)
	{
	return CComponentTester_ComponentTest::NewLC(aDataLogger, aComponentTestObserver);
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
	test.Start(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0782 "));

	test.End();
	test.Close();

	return E32Main_TestHarness(NewComponentTestLC);
	}

