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
//

#include <e32uid.h>

#include "TestUtilities.h"	// For __FILE__LINE__
#include "MagicTest.h"
#include "MagicUnitTests.h"

LOCAL_D RTest test(_L("MagicTest.cpp"));

CExampleInterface_ComponentTester::CExampleInterface_ComponentTester(CDataLogger& aDataLogger,
					   MComponentTestObserver& aObserver)
: CComponentTester(aDataLogger, aObserver)
	{
	}

CExampleInterface_ComponentTester* CExampleInterface_ComponentTester::NewLC(CDataLogger& aDataLogger, MComponentTestObserver& aObserver)
	{
	CExampleInterface_ComponentTester* self = new (ELeave) CExampleInterface_ComponentTester(aDataLogger, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0775
@SYMTestCaseDesc	    Tests for implementations of CExampleInterface class
@SYMTestPriority 	    High
@SYMTestActions  	    Unit tests for CExampleInterface class
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CExampleInterface_ComponentTester::ConstructL()
	{
	ComponentTesterConstructL();
	AddUnitTestL(CExampleInterfaceCreateAndDestroyUnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CExampleInterface_ListImplementations_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CExampleInterfaceAltCreateAndDestroyUnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CExampleInterface_DefectHAN4WZHSY_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CExampleInterface_DefectCUO4YCEUE_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CExampleInterface_DefectEVS4Z9BPG_UnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CExampleInterface_DefectKRN53SL4Q_UnitTest::NewL(iDataLogger, *this));
	}

EXPORT_C CComponentTester* NewComponentTestLC(CDataLogger& aDataLogger,
											MComponentTestObserver&	aComponentTestObserver)
	{
	return CExampleInterface_ComponentTester::NewLC(aDataLogger, aComponentTestObserver);
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
	test.Start(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0775 "));
	test.End();
	test.Close();

	return E32Main_TestHarness(NewComponentTestLC);
	}

