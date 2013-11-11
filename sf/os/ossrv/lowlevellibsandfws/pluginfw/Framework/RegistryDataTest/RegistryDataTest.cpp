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
// test the CRegistryData class using the test bed.
// It may be used as a basis to develop a full test bed dll.
// For support and comment please contact the authors.
// 
//

#include "StateAccessor.h"
#include <test_bed/testbeddefinitions.h>
#include <test_bed/unittest.h>
#include "LeakTestTransition.h"
#include <test_bed/datalogger.h>
#include "ComponentTester.h"

#include "RegistryData.h"

#include "RegistryDataUnitTests.h"

RTest test(_L("RegistryDataTest.cpp"));

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Test the CRegistryData class.
 */
class CRegistryData_ComponentTest : public CComponentTester
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
		@return			CRegistryData_ComponentTest* The constructed object.
		@pre 			None.
		@post			CRegistryData_ComponentTest is fully constructed.
	*/
	static CRegistryData_ComponentTest* NewLC(CDataLogger& aDataLogger,
							MComponentTestObserver& aObserver);

private:
	/**
		@fn				CRegistryData_ComponentTest(CDataLogger& aDataLogger,
											MComponentTestObserver& aObserver)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aDataLogger The logging object.
		@param			aObserver The observer of this component test.
		@pre 			None.
		@post			CRegistryData_ComponentTest is fully constructed.
	*/
	inline CRegistryData_ComponentTest(CDataLogger& aDataLogger,
												MComponentTestObserver& aObserver);
	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction,
						to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory.
		@since			7.0
		@return			None
		@pre 			CRegistryData_ComponentTest is fully constructed.
		@post			CRegistryData_ComponentTest is fully initialised.
	*/
	inline void ConstructL();

	};	// CRegistryData_ComponentTest

// ______________________________________________________________________________
//
inline CRegistryData_UnitTestContext::CRegistryData_UnitTestContext(CDataLogger& aDataLogger,
												MStateAccessor& aStateAccessor,
												MTransitionObserver& aObserver,
												RFs& aFs)
: CUnitTestContext(aDataLogger, aStateAccessor, aObserver), iFs(aFs)
	{
	//Do nothing
	}

inline CRegistryData_UnitTestContext::~CRegistryData_UnitTestContext()
	{
	delete iRegistryData;
	delete iDllData;
	delete iNewDllData;
	}

// ______________________________________________________________________________
//
inline CRegistryData_ComponentTest* CRegistryData_ComponentTest::NewLC(CDataLogger& aDataLogger, MComponentTestObserver& aObserver)
	{
	CRegistryData_ComponentTest* self = new (ELeave) CRegistryData_ComponentTest(aDataLogger, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

inline CRegistryData_ComponentTest::CRegistryData_ComponentTest(CDataLogger& aDataLogger,
									MComponentTestObserver& aObserver)
: CComponentTester(aDataLogger, aObserver)
	{
	// Do nothing here.
	}

inline void CRegistryData_ComponentTest::ConstructL()
	{
	// Perform base class initialization
	ComponentTesterConstructL();

	AddUnitTestL(CRegistryData_CreateAndDestroy_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CRegistryData_RetrieveInfo_UnitTest::NewL(iDataLogger, *this));

	AddUnitTestL(CRegistryData_GetImplInfoWhileDiscovering_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CRegistryData_AddData_UnitTest::NewL(iDataLogger, *this));
	}

// ______________________________________________________________________________
//
EXPORT_C CComponentTester* NewComponentTestLC(CDataLogger& aDataLogger,
									MComponentTestObserver&	aComponentTestObserver)
	{
	return CRegistryData_ComponentTest::NewLC(aDataLogger, aComponentTestObserver);
	}

// ___________________________________________________________________________
//
// This section of the module simply includes the exported test harness template which
// makes this a "whole" CPP file with a E32Main entry point below. The test MMP
// project file can then produce a EXE for the test project instead of a DLL.

#include "ECom/Test_Bed/TestHarnessTemplate.h"

GLDEF_C TInt E32Main()
    {
	test.Title();
	test.Start(_L(" @SYMTestCaseID:SYSLIB-ECOM-LEGACY-REGISTRYDATATEST-0001 "));

	test.End();
	test.Close();

	return E32Main_TestHarness(NewComponentTestLC);
	}

