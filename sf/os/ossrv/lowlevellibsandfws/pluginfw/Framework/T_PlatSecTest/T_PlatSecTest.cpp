// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "e32test.h"
#include <ecom/ecom.h>
#include "T_PlatSecInterface.h"

LOCAL_D RTest Test(_L("T_PlatSecTest"));

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0784
@SYMTestCaseDesc	    Tests for list Implementations created with two different UID
@SYMTestPriority 	    High
@SYMTestActions  	    Create new implementations with two different UID.
                        Execute REComSession::FinalClose
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
static void DoTestsL()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0784 CPlatSecInterface1 "));
	//Load the first plugin interface. It should be OK - no PlatSec warnings reported.
	CPlatSecInterface1* p1 = NULL;
	TRAPD(err, p1 = CPlatSecInterface1::NewL());
	if(err == KErrNone)
		{
		TUid implUid = p1->ImplId();
		Test(implUid == KUidPlatSecECom1Impl);
		}
	delete p1;

	Test.Next(_L("CPlatSecInterface2"));
	//Load the second plugin interface. PlatSec warnings will be printed out.
	CPlatSecInterface2* p2 = NULL;
	TRAP(err, p2 = CPlatSecInterface2::NewL());
	if(err == KErrNone)
		{
		TUid implUid = p2->ImplId();
		Test(implUid == KUidPlatSecECom2Impl);
		}
	delete p2;

	REComSession::FinalClose();
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	Test.Title();
	Test.Start(_L("ListImplementationsL tests."));

	CTrapCleanup* cleanup = CTrapCleanup::New();

	//install the scheduler
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	TRAPD(err, ::DoTestsL());

	delete scheduler;
	delete cleanup;

	Test(err==KErrNone);

	Test.End();
	Test.Close();

	__UHEAP_MARKEND;

	return KErrNone;
	}



