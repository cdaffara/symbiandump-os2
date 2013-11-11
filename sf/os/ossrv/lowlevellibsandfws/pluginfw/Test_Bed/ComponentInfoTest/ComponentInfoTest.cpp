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
// Set of implementations to test the CComponentInfo class using the test bed.
// 
//

#include "ComponentInfoUnitTest.h"
#include "ComponentTester.h"

// ______________________________________________________________________________
//
/**
	@internalComponent
	
	Comments : Test the CComponentInfo class.
 */
class CComponentInfo_ComponentTest : public CComponentTester
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
		@return			CComponentInfo_ComponentTest* The constructed object.
		@pre 			None.
		@post			CComponentInfo_ComponentTest is fully constructed and initialised.
	*/
	static CComponentInfo_ComponentTest* NewLC(CDataLogger& aDataLogger, 
							MComponentTestObserver& aObserver);

private:
	/**
		@fn				CComponentInfo_ComponentTest(CDataLogger& aDataLogger,
											MComponentTestObserver& aObserver)
		Intended Usage	: Standard constructor method.
		Error Condition	: None.
		@since			7.0
		@param			aDataLogger The logging object.
		@param			aObserver The observer of this component test.
		@pre 			None.
		@post			CComponentInfo_ComponentTest is fully constructed.
	*/
	inline CComponentInfo_ComponentTest(CDataLogger& aDataLogger,
												MComponentTestObserver& aObserver);
	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
						to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory.
		@since			7.0
		@return			None 
		@pre 			CComponentInfo_ComponentTest is fully constructed.
		@post			CComponentInfo_ComponentTest is fully initialised.
	*/
	inline void ConstructL();

	};	// CComponentInfo_ComponentTest

// ______________________________________________________________________________
//
inline CComponentInfo_ComponentTest* CComponentInfo_ComponentTest::NewLC(CDataLogger& aDataLogger, MComponentTestObserver& aObserver)
	{
	CComponentInfo_ComponentTest* self = new (ELeave) CComponentInfo_ComponentTest(aDataLogger, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

inline CComponentInfo_ComponentTest::CComponentInfo_ComponentTest(CDataLogger& aDataLogger,
									MComponentTestObserver& aObserver)
: CComponentTester(aDataLogger, aObserver)
	{
	// Do nothing here.
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0774
@SYMTestCaseDesc	    Tests for implementations of CComponentInfo class 
@SYMTestPriority 	    High
@SYMTestActions  	    Component tests for CComponentInfo class
@SYMTestExpectedResults Test must not fail CComponentInfo class
@SYMREQ                 REQ0000
*/
inline void CComponentInfo_ComponentTest::ConstructL()
	{
	// Perform base class initialization
	ComponentTesterConstructL();

	AddUnitTestL(CComponentInfo_CreateAndDestroy_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CComponentInfo_RetrieveTestNames_UnitTest::NewL(iDataLogger, *this));
	}

// ______________________________________________________________________________
//
EXPORT_C CComponentTester* NewComponentTestLC(CDataLogger& aDataLogger,
									MComponentTestObserver&	aComponentTestObserver)
	{
	return CComponentInfo_ComponentTest::NewLC(aDataLogger, aComponentTestObserver);
	}

// ___________________________________________________________________________
//
// This section of the module simply includes the exported test harness template which 
// makes this a "whole" CPP file with a E32Main entry point below. The test MMP 
// project file can then produce a EXE for the test project instead of a DLL.

#include <ecom/test_bed/testharnesstemplate.h>

GLDEF_C TInt E32Main()
    {
	return E32Main_TestHarness(NewComponentTestLC);
	}

