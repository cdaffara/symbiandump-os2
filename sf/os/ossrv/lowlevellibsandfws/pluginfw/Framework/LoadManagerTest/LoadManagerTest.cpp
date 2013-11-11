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
// test the CLoadManager class using the test bed.
// It may be used as a basis to develop a full test bed dll.
// For support and comment please contact the authors.
// 
//

#include "LoadManagerUnitTest.h"
#include "ComponentTester.h"

RTest test(_L("LoadManagerTest.cpp"));

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Test the CLoadManager class.
 */
class CLoadManager_ComponentTest : public CComponentTester
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
		@return			CLoadManager_ComponentTest* The constructed object.
		@pre 			None.
		@post			CLoadManager_ComponentTest is fully constructed.
	*/
	static CLoadManager_ComponentTest* NewLC(CDataLogger& aDataLogger,
							MComponentTestObserver& aObserver);

private:
	/**
		@fn				CLoadManager_ComponentTest(CDataLogger& aDataLogger,
											MComponentTestObserver& aObserver)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aDataLogger The logging object.
		@param			aObserver The observer of this component test.
		@pre 			None.
		@post			CLoadManager_ComponentTest is fully constructed.
	*/
	inline CLoadManager_ComponentTest(CDataLogger& aDataLogger,
												MComponentTestObserver& aObserver);
	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction,
						to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory.
		@since			7.0
		@return			None
		@pre 			CLoadManager_ComponentTest is fully constructed.
		@post			CLoadManager_ComponentTest is fully initialised.
	*/
	inline void ConstructL();

	};	// CLoadManager_ComponentTest


// ______________________________________________________________________________
//
inline CLoadManager_ComponentTest* CLoadManager_ComponentTest::NewLC(CDataLogger& aDataLogger, MComponentTestObserver& aObserver)
	{
	CLoadManager_ComponentTest* self = new (ELeave) CLoadManager_ComponentTest(aDataLogger, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

inline CLoadManager_ComponentTest::CLoadManager_ComponentTest(CDataLogger& aDataLogger,
									MComponentTestObserver& aObserver)
: CComponentTester(aDataLogger, aObserver)
	{
	// Do nothing here.
	}

inline void CLoadManager_ComponentTest::ConstructL()
	{
	// Perform base class initialization
	ComponentTesterConstructL();

	AddUnitTestL(CLoadManager_CreateAndDestroy_UnitTest::NewL(iDataLogger, *this));
	//AddUnitTestL(CLoadManager_FindInstantiationAndDestroy_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CLoadManager_FindInstantiationFailure_UnitTest::NewL(iDataLogger, *this));
	//AddUnitTestL(CLoadManager_DefectFOT56ULPM_UnitTest::NewL(iDataLogger, *this));
	}

// ______________________________________________________________________________
//
EXPORT_C CComponentTester* NewComponentTestLC(CDataLogger& aDataLogger,
									MComponentTestObserver&	aComponentTestObserver)
	{
	return CLoadManager_ComponentTest::NewLC(aDataLogger, aComponentTestObserver);
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
	test.Start(_L(" @SYMTestCaseID:SYSLIB-ECOM-LEGACY-LOADMANAGERTEST-0001 "));

	test.End();
	test.Close();

	return E32Main_TestHarness(NewComponentTestLC);
	}

