/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/




#if defined (__WINS__)
#define PDD_NAME		_L("ECDRV")
#else
#define PDD_NAME		_L("EUART1")
#define PDD2_NAME		_L("EUART2")
#define PDD3_NAME		_L("EUART3")
#define PDD4_NAME		_L("EUART4")
#endif

#define LDD_NAME		_L("ECOMM")

/**
 * @file
 *
 * Pipe test server implementation
 */
#include "tlibcwcharserver.h"
#include "tlibcwchar.h"


_LIT(KServerName, "tlibcwchar");

CLibcwcharTestServer* CLibcwcharTestServer::NewL()
	{
	CLibcwcharTestServer *server = new(ELeave) CLibcwcharTestServer();
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

static void InitCommsL()
    {
    TInt ret = User::LoadPhysicalDevice(PDD_NAME);
    User::LeaveIfError(ret == KErrAlreadyExists?KErrNone:ret);

#ifndef __WINS__
    ret = User::LoadPhysicalDevice(PDD2_NAME);
    ret = User::LoadPhysicalDevice(PDD3_NAME);
    ret = User::LoadPhysicalDevice(PDD4_NAME);
#endif

    ret = User::LoadLogicalDevice(LDD_NAME);
    User::LeaveIfError(ret == KErrAlreadyExists?KErrNone:ret);
    ret = StartC32();
    User::LeaveIfError(ret == KErrAlreadyExists?KErrNone:ret);
    }

LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	//InitCommsL();
	
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CLibcwcharTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CLibcwcharTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

/**
 * Server entry point
 * @return Standard Epoc error code on exit
 */
TInt main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL) 
		{
		return KErrNoMemory;  
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	
	return KErrNone;
	}

CTestStep* CLibcwcharTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if(aStepName == KTestfgetwcL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestOnefgetwsL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestTwofgetwsL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestThreefgetwsL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestgetwcL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestOnefwideL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestTwofwideL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestThreefwideL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestFourfwideL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestOnefputwcL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestTwofputwcL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestThreefputwcL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestFourfputwcL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestOneputwcL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestTwoputwcL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestThreeputwcL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestFourputwcL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestOnefputwsL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestTwofputwsL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestThreefputwsL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestFourfputwsL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestOneungetwcL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestOneputwcharL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KTestOnegetwcharL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	
	if(aStepName == Kwcslen1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcslen2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwmemcmp1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwmemcmp2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcswidth1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcswidth2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcswidth3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcswidth4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcwidth1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcwidth2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwmemchr1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwmemchr2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwmemcpy1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwmemcpy2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwmemcpy3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwmemset1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwmemset2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwmemset3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwmemmove1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwmemmove2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwmemmove3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwmemmove4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcstol1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcstol2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcstol3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcstol4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcstol5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcstol6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcstol7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcstol8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcstol9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbsinit1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbsinit2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbsinit3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctype1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctype2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctype3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctype4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctype5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctype6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctype7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctype8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctype9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctype10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctype11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctype12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctype13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctype14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctype15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctype16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctype17L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctrans1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctrans2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctrans3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctrans4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctrans5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	
	if(aStepName == KWcsncmp_Test_1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsncmp_Test_2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsncmp_Test_3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsncmp_Test_4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsncmp_Test_5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscmp_Test_1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscmp_Test_2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscmp_Test_3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscmp_Test_4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscat_Test_1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscat_Test_2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscat_Test_3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscat_Test_4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsncat_Test_1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsncat_Test_2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsncat_Test_3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsncat_Test_4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsncat_Test_5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscpy_Test_1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscpy_Test_2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscpy_Test_3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscpy_Test_4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcschr_Test_1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcschr_Test_2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcschr_Test_3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcschr_Test_4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcschr_Test_5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsrchr_Test_1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsrchr_Test_2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsrchr_Test_3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsrchr_Test_4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsrchr_Test_5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsspn_Test_1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsspn_Test_2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsspn_Test_3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsspn_Test_4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcspbrk_Test_1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcspbrk_Test_2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcspbrk_Test_3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcspbrk_Test_4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscoll_Test_1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscoll_Test_2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscoll_Test_3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscoll_Test_4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscspn_Test_1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscspn_Test_2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscspn_Test_3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcscspn_Test_4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsncpy_Test_1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsncpy_Test_2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsncpy_Test_3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsncpy_Test_4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsncpy_Test_5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsstr_Test_1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsstr_Test_2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsstr_Test_3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcsstr_Test_4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcstok1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcstok2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcstok3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcstok4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcswcs_Test_1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcswcs_Test_2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcswcs_Test_3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == KWcswcs_Test_4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	
#ifdef TO_BE_TESTED  
	if(aStepName == Kwprintf_Test0L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test17L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test18L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test19L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test20L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwprintf_Test21L)
  		{
		testStep = new CTestLibcwchar(aStepName);
		}
#endif //TO_BE_TESTED
	if(aStepName == Kfwprintf_Test1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test17L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test18L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test19L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test20L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwprintf_Test21L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test0L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test17L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test18L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test19L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test20L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswprintf_Test21L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
#ifdef TO_BE_TESTED  
	if(aStepName == Kvwprintf_Test0L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test17L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test18L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test19L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test20L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwprintf_Test21L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
#endif //TO_BE_TESTED  
	if(aStepName == Kvfwprintf_Test0L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test17L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test18L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test19L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test20L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwprintf_Test21L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test0L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test17L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test18L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test19L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test20L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswprintf_Test21L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
		
	if(aStepName == Kwscanf_Test1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwscanf_Test2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwscanf_Test3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwscanf_Test4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwscanf_Test5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwscanf_Test6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwscanf_Test7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwscanf_Test8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwscanf_Test9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwscanf_Test10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwscanf_Test11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwscanf_Test12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwscanf_Test13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwscanf_Test14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwscanf_Test15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwscanf_Test16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwscanf_Test17L	)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwscanf_Test1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwscanf_Test2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwscanf_Test3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwscanf_Test4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwscanf_Test5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwscanf_Test6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwscanf_Test7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwscanf_Test8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwscanf_Test9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwscanf_Test10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwscanf_Test11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwscanf_Test12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwscanf_Test13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwscanf_Test14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwscanf_Test15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kfwscanf_Test16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswscanf_Test1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswscanf_Test2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswscanf_Test3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswscanf_Test4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswscanf_Test5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswscanf_Test6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswscanf_Test7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswscanf_Test8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswscanf_Test9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswscanf_Test10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswscanf_Test11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswscanf_Test12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswscanf_Test13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswscanf_Test14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswscanf_Test15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswscanf_Test16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kswscanf_Test17L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
#ifdef TO_BE_TESTED        		
	if(aStepName == Kvwscanf_Test1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwscanf_Test2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwscanf_Test3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwscanf_Test4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwscanf_Test5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwscanf_Test6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwscanf_Test7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwscanf_Test8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwscanf_Test9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwscanf_Test10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwscanf_Test11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwscanf_Test12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwscanf_Test13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwscanf_Test14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwscanf_Test15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwscanf_Test16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvwscanf_Test17L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
#endif //TO_BE_TESTED		
	if(aStepName == Kvfwscanf_Test1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwscanf_Test2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwscanf_Test3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwscanf_Test4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwscanf_Test5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwscanf_Test6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwscanf_Test7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwscanf_Test8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwscanf_Test9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwscanf_Test10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwscanf_Test11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwscanf_Test12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwscanf_Test13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwscanf_Test14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwscanf_Test15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvfwscanf_Test16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswscanf_Test1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswscanf_Test2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswscanf_Test3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswscanf_Test4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswscanf_Test5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswscanf_Test6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswscanf_Test7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswscanf_Test8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswscanf_Test9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswscanf_Test10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswscanf_Test11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswscanf_Test12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswscanf_Test13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswscanf_Test14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswscanf_Test15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswscanf_Test16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kvswscanf_Test17L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	
	
	if(aStepName == Kmblen_Test0L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmblen_Test1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmblen_Test2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmblen_Test3L)
		testStep = new CTestLibcwchar(aStepName);{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmblen_Test4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmblen_Test5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmblen_Test6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmblen_Test7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmblen_Test8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmblen_Test9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwctomb_Test0L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwctomb_Test1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwctomb_Test2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwctomb_Test3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwctomb_Test4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwctomb_Test5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstombs_Test0L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstombs_Test1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstombs_Test2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstombs_Test3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstombs_Test4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstombs_Test5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmbtowc_Test0L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmbtowc_Test1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmbtowc_Test2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmbtowc_Test3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmbtowc_Test4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmbtowc_Test5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmbstowcs_Test0L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmbstowcs_Test1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmbstowcs_Test2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmbstowcs_Test3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmbstowcs_Test4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kmbstowcs_Test5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcrtomb2400)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcrtomb2401)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcrtomb2402)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcrtomb2403)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcrtomb2404)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcrtomb2405)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcrtomb2406)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcrtomb2407)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcrtomb2672)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsrtombs2409)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsrtombs2410)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsrtombs2411)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsrtombs2412)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsrtombs2413)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsrtombs2414)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsrtombs2415)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsrtombs2423)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsrtombs2436)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsrtombs2437)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsrtombs2438)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsrtombs2439)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsrtombs2440)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsrtombs2442)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsrtombs2444)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsrtombs2460)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsrtombs2673 )
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestbtowc1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestbtowc2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestbtowc3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctob1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctob2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwctob3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktesttowlower1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktesttowlower2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktesttowlower3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktesttowupper1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktesttowupper2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktesttowupper3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbsrtowcs1L		)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbsrtowcs2L		)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbsrtowcs3L		)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbsrtowcs4L		)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbsrtowcs5L	)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbrtowc1L		)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbrtowc2L		)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbrtowc3L		)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbrtowc4L		)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbrtowc5L	)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbrtowc6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbrlen0L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbrlen1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbrlen2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbrlen3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbrlen4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbrlen5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbrlen6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbrlen7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbrlen8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbrlen9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstof1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstof2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstof3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstof4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstof5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime1_aL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime2_AL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime3_bL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime4_BL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime5_cL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime6_CL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime7_dL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime8_DL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime9_eL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime10_FL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime11_hL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime12_IL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime13_jL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime14_mL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime15_ML)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime16_nL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime17_pL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime18_rL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime19_RL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime20_SL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime21_tL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime22_TL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime23_uL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime25_wL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime26_xL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime27_XL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime28_yL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime29_YL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime30L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime_mixL)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcsftime2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstod1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstod2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstod3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstod4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstod5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstold1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstold2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstold3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Kwcstold4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}


	if(aStepName == Ktestwcsnlen1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsnlen2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsnlen3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsnlen4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsnlen5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsdup1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsdup2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcscasecmp1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcscasecmp2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcscasecmp3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcscasecmp4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcscasecmp5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcscasecmp6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsncasecmp1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsncasecmp2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsncasecmp3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsncasecmp4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsncasecmp5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsncasecmp6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsncasecmp7L )
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcpcpy1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcpcpy2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcpncpy1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcpncpy2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcpncpy3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcpncpy4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsxfrm1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsxfrm2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoul1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoul2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoul3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoul4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoul5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoul6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoul7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoul8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoul9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoul10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoul11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoul12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoul13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoul14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoul15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoul16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoul17L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoll1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoll2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoll3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoll4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoll5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoll6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoll7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoll8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoll9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoll10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoll11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoll12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoll13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoll14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoll15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoll16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoll17L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoull1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoull2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoull3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoull4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoull5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoull6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoull7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoull8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoull9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoull10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoull11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoull12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoull13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoull14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoull15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoull16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoull17L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoq1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoq2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoq3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoq4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoq5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoq6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoq7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoq8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoq9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoq10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoq11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoq12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoq13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoq14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoq15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoq16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoq17L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstouq1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstouq2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstouq3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstouq4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstouq5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstouq6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstouq7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstouq8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstouq9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstouq10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstouq11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstouq12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstouq13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstouq14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstouq15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstouq16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstouq17L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoumax1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoumax2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoumax3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoumax4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoumax5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoumax6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoumax7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoumax8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoumax9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoumax10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoumax11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoumax12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoumax13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoumax14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoumax15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoumax16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoumax17L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoimax1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoimax2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoimax3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoimax4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoimax5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoimax6L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoimax7L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoimax8L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoimax9L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoimax10L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoimax11L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoimax12L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoimax13L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoimax14L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoimax15L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoimax16L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcstoimax17L	)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbsnrtowcs1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbsnrtowcs2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbsnrtowcs3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbsnrtowcs4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestmbsnrtowcs5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsnrtombs1L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsnrtombs2L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsnrtombs3L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsnrtombs4L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	if(aStepName == Ktestwcsnrtombs5L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
		
	if(aStepName == Kswprintf_Test22L)
		{
		testStep = new CTestLibcwchar(aStepName);
		}
	return testStep;
	}

