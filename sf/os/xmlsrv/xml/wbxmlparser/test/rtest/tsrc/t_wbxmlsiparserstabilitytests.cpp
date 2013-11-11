// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>

#include "stabilitytestclass.h"
#include "testdocuments.h"

static RTest test(_L("t_wbxmlsiparserstabilitytests"));


/**
@SYMTestCaseID 			 	SYSLIB-XML-CT-3724
@SYMTestCaseDesc		    Parsing service indication documents.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Parsing the documents.
@SYMTestExpectedResults 	Parser should cope with parsing the documents.
@SYMREQ 		 		 	REQ0000
*/
static void SiTestsL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3724 SiTestsL tests... "));

	__UHEAP_MARK;

	CStabilityTestClass* siTest = CStabilityTestClass::NewL(test);
	CleanupStack::PushL(siTest);
	siTest->StabilityTestL(KSi1_0Directory(), KSiExt(), &CStabilityTestClass::TestWholeL);
	CleanupStack::PopAndDestroy(siTest);

	CStabilityTestClass* siTestMin = CStabilityTestClass::NewL(test, EFalse, KMinChunkSize);
	CleanupStack::PushL(siTestMin);
	siTestMin->StabilityTestL(KSi1_0Directory(), KSiExt(), &CStabilityTestClass::TestChunkL);
	CleanupStack::PopAndDestroy(siTestMin);

	__UHEAP_MARKEND;
	}


static void MainL()
	{
	SiTestsL();
	}


TInt E32Main()
	{
	__UHEAP_MARK;
	test.Title();
	test.Start(_L("initialising"));

	CTrapCleanup* c=CTrapCleanup::New();

	// start the loader
	RFs fs;
	test (fs.Connect()==KErrNone);
	fs.Close();

	test (c!=0);
	TRAPD(r,MainL());
	test (r==KErrNone);
	delete c;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return KErrNone;
	}
