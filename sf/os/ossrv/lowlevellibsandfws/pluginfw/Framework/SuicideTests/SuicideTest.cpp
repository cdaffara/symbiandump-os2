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
#include "SuicideTest.h"
#include "SuicideUnitTests.h"

CSuicideTest::CSuicideTest(CDataLogger& aDataLogger, 
					   MComponentTestObserver& aObserver)
: CComponentTester(aDataLogger, aObserver)
	{
	}

CSuicideTest* CSuicideTest::NewLC(CDataLogger& aDataLogger, MComponentTestObserver& aObserver)
	{
	CSuicideTest* self = new (ELeave) CSuicideTest(aDataLogger, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CSuicideTest::ConstructL()
	{
	ComponentTesterConstructL();
	AddUnitTestL(CSuicideInterfaceFireAndForgetUnitTest::NewL(iDataLogger, *this));
	AddUnitTestL(CSuicideInterfaceCreateAndDestroyUnitTest::NewL(iDataLogger, *this));
	}

EXPORT_C CComponentTester* NewComponentTestLC(CDataLogger& aDataLogger, 
											MComponentTestObserver&	aComponentTestObserver)
	{
	return CSuicideTest::NewLC(aDataLogger, aComponentTestObserver);
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

