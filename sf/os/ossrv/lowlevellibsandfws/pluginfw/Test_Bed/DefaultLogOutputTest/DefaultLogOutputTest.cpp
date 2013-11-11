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
// test the CDefaultLogOutput class using the test bed.
// It may be used as a basis to develop a full test bed dll.
// For support and comment please contact the authors.
// 
//

#include "DefaultLogOutputUnitTest.h"
#include "ComponentTester.h"

RTest test(_L("DefaultLogOutputTest.cpp"));

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Test the CDefaultLogOutput class.
 */
class CDefaultLogOutput_ComponentTest : public CComponentTester
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
		@return			CDefaultLogOutput_ComponentTest* The constructed object.
		@pre 			None.
		@post			CDefaultLogOutput_ComponentTest is fully constructed.
	*/
	static CDefaultLogOutput_ComponentTest* NewLC(CDataLogger& aDataLogger,
							MComponentTestObserver& aObserver);

private:
	/**
		@fn				CDefaultLogOutput_ComponentTest(CDataLogger& aDataLogger,
											MComponentTestObserver& aObserver)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aDataLogger The logging object.
		@param			aObserver The observer of this component test.
		@pre 			None.
		@post			CDefaultLogOutput_ComponentTest is fully constructed.
	*/
	inline CDefaultLogOutput_ComponentTest(CDataLogger& aDataLogger,
												MComponentTestObserver& aObserver);
	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction,
						to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory.
		@since			7.0
		@return			None
		@pre 			CDefaultLogOutput_ComponentTest is fully constructed.
		@post			CDefaultLogOutput_ComponentTest is fully initialised.
	*/
	inline void ConstructL();

	};	// CDefaultLogOutput_ComponentTest

// ______________________________________________________________________________
//
inline CDefaultLogOutput_ComponentTest* CDefaultLogOutput_ComponentTest::NewLC(CDataLogger& aDataLogger, MComponentTestObserver& aObserver)
	{
	CDefaultLogOutput_ComponentTest* self = new (ELeave) CDefaultLogOutput_ComponentTest(aDataLogger, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

inline CDefaultLogOutput_ComponentTest::CDefaultLogOutput_ComponentTest(CDataLogger& aDataLogger,
									MComponentTestObserver& aObserver)
: CComponentTester(aDataLogger, aObserver)
	{
	// Do nothing here.
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0777
@SYMTestCaseDesc	    Tests for the implementations of CDefaultLogOutput class
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for the open and close,create and destroy,write and close operations
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
inline void CDefaultLogOutput_ComponentTest::ConstructL()
	{
	// Perform base class initialization
	ComponentTesterConstructL();

	AddUnitTestL(CDefaultLogOutput_CreateAndDestroy_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CDefaultLogOutput_OpenAndClose_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CDefaultLogOutput_OpenWriteClose_UnitTest::NewL(iDataLogger, *this));
	}

// ______________________________________________________________________________
//
EXPORT_C CComponentTester* NewComponentTestLC(CDataLogger& aDataLogger,
									MComponentTestObserver&	aComponentTestObserver)
	{
	return CDefaultLogOutput_ComponentTest::NewLC(aDataLogger, aComponentTestObserver);
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
	test.Start(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0777 "));

	test.End();
	test.Close();

	return E32Main_TestHarness(NewComponentTestLC);
	}
