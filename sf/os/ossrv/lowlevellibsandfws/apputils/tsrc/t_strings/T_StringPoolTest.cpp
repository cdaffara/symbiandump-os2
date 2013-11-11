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
// test the RStringPool class using the test bed.
// It may be used as a basis to develop a full test bed dll.
// For support and comment please contact the authors.
// 
//

#ifdef __WINS__
#pragma warning(disable : 4097)			// disable typedef warning
#endif

#include "StringPoolUnitTest.h"
#include <ecom/test_bed/componenttester.h>

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Test the RStringPool class.
 */
class CStringPool_ComponentTest : public CComponentTester
	{
public:
	/**
		@fn				NewLC(CDataLogger& aDataLogger,
							 MComponentTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave		KErrNoMemory
		@since			6.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this component test.
		@return			CStringPool_ComponentTest* The constructed object.
		@pre 			None.
		@post			CStringPool_ComponentTest is fully constructed.
	*/
	static CStringPool_ComponentTest* NewLC(CDataLogger& aDataLogger, 
							MComponentTestObserver& aObserver);

private:
	/**
		@fn				CStringPool_ComponentTest(CDataLogger& aDataLogger,
											MComponentTestObserver& aObserver)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aDataLogger The logging object.
		@param			aObserver The observer of this component test.
		@pre 			None.
		@post			CStringPool_ComponentTest is fully constructed.
	*/
	inline CStringPool_ComponentTest(CDataLogger& aDataLogger,
												MComponentTestObserver& aObserver);
	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
						to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory.
		@since			6.0
		@return			None 
		@pre 			CStringPool_ComponentTest is fully constructed.
		@post			CStringPool_ComponentTest is fully initialised.
	*/
	inline void ConstructL();

	};	// CStringPool_ComponentTest

// ______________________________________________________________________________
//
inline CStringPool_ComponentTest* CStringPool_ComponentTest::NewLC(CDataLogger& aDataLogger, MComponentTestObserver& aObserver)
	{
	CStringPool_ComponentTest* self = new (ELeave) CStringPool_ComponentTest(aDataLogger, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

inline CStringPool_ComponentTest::CStringPool_ComponentTest(CDataLogger& aDataLogger,
									MComponentTestObserver& aObserver)
: CComponentTester(aDataLogger, aObserver)
	{
	// Do nothing here.
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0493
@SYMTestCaseDesc        Tests the functionality of RStringPool class
@SYMTestPriority        High
@SYMTestActions         Attempt to load short,large,multiple and irrelevant tables
@SYMTestExpectedResults Test must not fail 
@SYMREQ                 REQ0000
*/
inline void CStringPool_ComponentTest::ConstructL()
	{
	// Perform base class initialization
	ComponentTesterConstructL();
	// CI tests with no table loaded 
	AddUnitTestL(CStringPool_CaseInsensitive_UnitTest::NewL(iDataLogger, *this));
	// CS tests with a Short table loaded
	AddUnitTestL(CStringPool_CaseSensitive_UnitTest::NewL(iDataLogger, *this));
	// CI tests with a Short table loaded, and callback test
	//.Also tested defects : BEU-55DJG3 , KRN-56NDEZ, APY-57TEH3, HAD57SK27OK
	AddUnitTestL(CStringPool_ShortTable_UnitTest::NewL(iDataLogger, *this));
	// CS tests with a Short table loaded 
	AddUnitTestL(CStringPool_ShortTableCS_UnitTest::NewL(iDataLogger, *this));
	// CI tests with a Irrelevant table loaded
	AddUnitTestL(CStringPool_IrrelevantTable_UnitTest::NewL(iDataLogger, *this));
	// CS tests with a Irrelevant table loaded
	AddUnitTestL(CStringPool_IrrelevantTableCS_UnitTest::NewL(iDataLogger, *this));
	// CI tests with a Large table loaded
	AddUnitTestL(CStringPool_LongTable_UnitTest::NewL(iDataLogger, *this));
	// CS tests with a Large table loaded
	AddUnitTestL(CStringPool_LongTableCS_UnitTest::NewL(iDataLogger, *this));
	// CI tests with a Multiple Tables
	AddUnitTestL(CStringPool_MultipleTableShortCI_UnitTest::NewL(iDataLogger, *this));
	// CS tests with a Multiple Tables
	AddUnitTestL(CStringPool_MultipleTableShortCS_UnitTest::NewL(iDataLogger, *this));

	}

// ______________________________________________________________________________
//
EXPORT_C CComponentTester* NewComponentTestLC(CDataLogger& aDataLogger,
									MComponentTestObserver&	aComponentTestObserver)
	{
	return CStringPool_ComponentTest::NewLC(aDataLogger, aComponentTestObserver);
	}


// ___________________________________________________________________________
//
// This section of the module simply includes the exported test harness template which 
// makes this a "whole" CPP file with a E32Main entry point below. The test MMP 
// project file can then produce a EXE for the test project instead of a DLL.

#include "ecom/test_bed/testharnesstemplate.h"

GLDEF_C TInt E32Main()
	{
 	return E32Main_TestHarness(NewComponentTestLC);
	}
