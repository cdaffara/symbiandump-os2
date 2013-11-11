// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tstdlibserver.cpp
//

#include <c32comm.h>

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
#include "tstdlibserver.h"
#include "tstdlib.h"


_LIT(KServerName, "tstdlib");

CStdlibTestServer* CStdlibTestServer::NewL()
	{
	CStdlibTestServer *server = new(ELeave) CStdlibTestServer();
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
	CStdlibTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CStdlibTestServer::NewL());
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

CTestStep* CStdlibTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if(aStepName == Kmalloc_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kmalloc_Test1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kmalloc_Test2)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kcalloc_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kcalloc_Test1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kcalloc_Test2)
		{
		testStep = new CTestStdlib(aStepName);
		}

	if(aStepName == Krealloc_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Krealloc_Test1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Krealloc_Test2)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Krealloc_Test3)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Krealloc_Test4)
		{
		testStep = new CTestStdlib(aStepName);
		}

	if(aStepName == Kfree_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	
	if(aStepName == Kgetenv_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kgetenv_Test1)
		{
		testStep = new CTestStdlib(aStepName);
		}

	if(aStepName == Ksetenv_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Ksetenv_Test1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Ksetenv_Test2)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Ksetenv_Test3)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Ksetenv_Test4)
		{
		testStep = new CTestStdlib(aStepName);
		}

	if(aStepName == Kputenv_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kputenv_Test1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kputenv_Test2)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kputenv_Test3)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kunsetenv_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	
	if(aStepName == Ksystem_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Ksystem_Test1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Ksystem_Test2)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Ksystem_Test3)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Ksystem_Test4)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Ksystem_Test5)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Ksystem_Test6)
		{
		testStep = new CTestStdlib(aStepName);
		}

	if(aStepName == Kabort_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kfopen_Test)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kexit_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kexit_Test1)
		{
		testStep = new CTestStdlib(aStepName);
		}

	if(aStepName == K_exitE_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}

	if(aStepName == K_exit_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}

	if(aStepName == Katexit_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katexit_Test1)
		{
		testStep = new CTestStdlib(aStepName);
		}

	if(aStepName == Kabs_good_param)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kabs_pos_max)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kabs_neg_max)
		{
		testStep = new CTestStdlib(aStepName);
		}

	if(aStepName == Klabs_good_param)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Klabs_pos_max)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Klabs_neg_max)
		{
		testStep = new CTestStdlib(aStepName);
		}

	if(aStepName == Kllabs_good_param)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kllabs_pos_max)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kllabs_neg_max)
		{
		testStep = new CTestStdlib(aStepName);
		}
	
	if(aStepName == Katoi_null_string)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katoi_good_param)
		{
		testStep = new CTestStdlib(aStepName);
		}

	if(aStepName == Katof_null_string)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katof_pos_floatnum)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katof_neg_floatnum)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katof_alpha)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katof_alpha_floatnum)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katof_pos_floatnum_alpha)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katof_neg_floatnum_alpha )
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katof_leading_zero )
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katof_neg_leading_zero )
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katof_pos_floatnum_pos_expo)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katof_neg_floatnum_pos_expo)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katof_pos_floatnum_neg_expo )
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katof_neg_floatnum_neg_expo)
		{
		testStep = new CTestStdlib(aStepName);
		}

	if(aStepName == Katol_null_string)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katol_good_param)
		{
		testStep = new CTestStdlib(aStepName);
		}

	if(aStepName == Katollbasic)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katollspacecheck)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katollsignedsrc)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katolloctalcheck)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katollhexcheck)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katolldigitfirstcheck)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katollalphafirstcheck)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katollalphacheck)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katollleadingzeroscheck)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kdiv_good_param)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kldiv_good_param)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Klldiv_good_param0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Klldiv_good_param1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Klldiv_good_param2)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Klldiv_good_param3)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Klldiv_good_param4)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Krand)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Ksrand)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtol_good_param)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtol_null_string)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtod_null_string)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtod_good_param)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtod_nan)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtod_inf)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtod_neg_cases)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtod_misc_cases)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtoul_null_string)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtoul_good_param)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtoul_neg_leading_zeroes)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtoul_neg_num_alpha)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kqsort_integers)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kqsort_strings)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kbinsearch_integers)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kbinsearch_strings)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kisatty_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kisatty_Test1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kusleep_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kusleep_Test1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kusleep_Test2)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kgetcwd_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kgetcwd_Test1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kgetcwd_Test2)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kgetcwd_Test3)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Ktmpfile_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Ktmpfile_Test1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Ktmpnam_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Ktmpnam_Test1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Krealpath_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Krealpath_Test1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Krealpath_Test2)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Krealpath_Test3)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Krealpath_Test4)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kperror_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kmkstemp_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kmkstemp_Test1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kconfstr_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kconfstr_Test1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kfpathconf_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kfpathconf_Test1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kfilelock_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kfilelock_Test1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kfilelock_Test2)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kfilelock_Test3)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KgetoptTest1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KgetoptTest2)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KgetoptTest3)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KgetoptTest4)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KgetoptTest5)
		{
		testStep = new CTestStdlib(aStepName);
		}   	
	if(aStepName == KgetoptTest6)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KgetoptTest_long1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KgetoptTest_long2)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KgetoptTest_long3)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KgetoptTest_long4)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KTestlseek)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KTestsigemptyset)
		{
		testStep = new CTestStdlib(aStepName);
		}	
	if(aStepName == Kstrfmon1)
		{
		testStep = new CTestStdlib(aStepName);
		}	
	if(aStepName == KTestRealPath5)
		{
		testStep = new CTestStdlib(aStepName);
		}	
	if(aStepName == KTestlseek1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KTestlseek2)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KTestlseek3)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KgetoptTest_long5)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KsetjmpTest)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KabortTest)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KchownTest)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KiconvTest)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KsetgrentTest)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KwcreatTest)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katof_locale)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtod_locale)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kbinsearch_integers1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kcalloc_Test3)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kcalloc_Test4)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kgetenv_Test2)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kreallocf_Test0)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Katexit_Test2)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtol_space_string)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtol_extreme_string)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtol_extreme_string1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtoll_extreme_string)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtoll_extreme_string1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtoll_good_param)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtoul_extreme_string)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kstrtoul_extreme_string1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kqsort_integers1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kqsort_integers2)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kqsort_integers3)
		{
		testStep = new CTestStdlib(aStepName);
		}
	else if (aStepName == Kstrtoullbasic)
		{
		testStep = new CTestStdlib(aStepName);
		}
	else if (aStepName == Kstrtoullneg)
		{
		testStep = new CTestStdlib(aStepName);
		}
	else if (aStepName == Kstrtoullpos)
		{
		testStep = new CTestStdlib(aStepName);
		}
	else if (aStepName == Kstrtoullbase)
		{
		testStep = new CTestStdlib(aStepName);
		}
	else if (aStepName == Kstrtoulpos)
		{
		testStep = new CTestStdlib(aStepName);
		}
	else if (aStepName == Kstrtoimaxbasic)
		{
		testStep = new CTestStdlib(aStepName);
		}
	else if (aStepName == Kstrtoimaxbasic1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	else if (aStepName == Kstrtoimaxbasic2)
		{
		testStep = new CTestStdlib(aStepName);
		}
	else if (aStepName == Kstrtoumaxbasic)
		{
		testStep = new CTestStdlib(aStepName);
		}
	else if (aStepName == Kstrtoumaxbasic1)
		{
		testStep = new CTestStdlib(aStepName);
		}
	else if (aStepName == Kstrtoumaxbasic2)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Kconfstr_Test2)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == Ktmpfile_fseek)
		{
		testStep = new CTestStdlib(aStepName);
		}
	if(aStepName == KtestSymLink)
	        {
	        testStep = new CTestStdlib(aStepName);
	        }
	return testStep;
	}


