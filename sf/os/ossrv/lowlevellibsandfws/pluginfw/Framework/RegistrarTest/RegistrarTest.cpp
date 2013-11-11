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
// test the CRegistrar class using the test bed.
// It may be used as a basis to develop a full test bed dll.
// For support and comment please contact the authors.
// 
//

#include "StateAccessor.h"
#include <test_bed/testbeddefinitions.h>
#include <test_bed/unittest.h>
#include <test_bed/transition.h>
#include <test_bed/datalogger.h>
#include "ComponentTester.h"

#include "Registrar.h"

#include "RegistrarTransitions.h"
#include "RegistrarUnitTests.h"

RTest test(_L("RegistrarTest.cpp"));

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Test the CRegistrar class.
 */
class CRegistrar_ComponentTest : public CComponentTester
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
		@return			CRegistrar_ComponentTest* The constructed object.
		@pre 			None.
		@post			CRegistrar_ComponentTest is fully constructed.
	*/
	static CRegistrar_ComponentTest* NewLC(CDataLogger& aDataLogger,
							MComponentTestObserver& aObserver);

private:
	/**
		@fn				CRegistrar_ComponentTest(CDataLogger& aDataLogger,
											MComponentTestObserver& aObserver)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aDataLogger The logging object.
		@param			aObserver The observer of this component test.
		@pre 			None.
		@post			CRegistrar_ComponentTest is fully constructed.
	*/
	inline CRegistrar_ComponentTest(CDataLogger& aDataLogger,
												MComponentTestObserver& aObserver);
	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction,
						to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory.
		@since			7.0
		@return			None
		@pre 			CRegistrar_ComponentTest is fully constructed.
		@post			CRegistrar_ComponentTest is fully initialised.
	*/
	inline void ConstructL();

	};	// CRegistrar_ComponentTest

// ______________________________________________________________________________
//
inline CRegistrar_UnitTestContext::CRegistrar_UnitTestContext(CDataLogger& aDataLogger,
													MStateAccessor&		 aStateAccessor,
													MTransitionObserver& aObserver,
													CRegistryData&		 aRegistryData,
													MRegistrarObserver&	 aRegistrarObserver,
													RFs&				 aFs)
: CUnitTestContext(aDataLogger, aStateAccessor, aObserver),
iRegistryData(aRegistryData),
iRegistrarObserver(aRegistrarObserver),
iFs(aFs)
	{
	//Do nothing
	}

inline CRegistrar_UnitTestContext::~CRegistrar_UnitTestContext()
	{
	delete iRegistrar;
	delete iDllData;
	}

// ______________________________________________________________________________
//
inline CRegistrar_ComponentTest* CRegistrar_ComponentTest::NewLC(CDataLogger& aDataLogger, MComponentTestObserver& aObserver)
	{
	CRegistrar_ComponentTest* self = new (ELeave) CRegistrar_ComponentTest(aDataLogger, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

inline CRegistrar_ComponentTest::CRegistrar_ComponentTest(CDataLogger& aDataLogger,
									MComponentTestObserver& aObserver)
: CComponentTester(aDataLogger, aObserver)
	{
	// Do nothing here.
	}

inline void CRegistrar_ComponentTest::ConstructL()
	{
	// Perform base class initialization
	ComponentTesterConstructL();

	AddUnitTestL(CRegistrar_CreateAndDestroy_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CRegistrar_BeginComplete_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CRegistrar_RegisterDiscovery_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CRegistrar_RegisterTwice_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CRegistrar_UseWhileDiscovering_UnitTest::NewL(iDataLogger, *this));
	}

// ______________________________________________________________________________
//
EXPORT_C CComponentTester* NewComponentTestLC(CDataLogger& aDataLogger,
									MComponentTestObserver&	aComponentTestObserver)
	{
	return CRegistrar_ComponentTest::NewLC(aDataLogger, aComponentTestObserver);
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
	test.Start(_L(" @SYMTestCaseID:SYSLIB-ECOM-LEGACY-REGISTRARTEST-0001 "));

	test.End();
	test.Close();

	return E32Main_TestHarness(NewComponentTestLC);
	}

