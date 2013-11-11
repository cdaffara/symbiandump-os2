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
#include "tstringserver.h"
#include "teststring.h"


_LIT(KServerName, "tstring");

CStringTestServer* CStringTestServer::NewL()
	{
	CStringTestServer *server = new(ELeave) CStringTestServer();
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
	CStringTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CStringTestServer::NewL());
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
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL) 
		{
		return KErrNoMemory;  
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
	}

CTestStep* CStringTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps

	if (aStepName == KMemCpyBasicTest)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KMemCpyTestZeroLengthCopy)
		{
		testStep = new CTestString(aStepName);
		}
	
	///memmove
	else if (aStepName == KMemMoveBasicTest)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KMemMoveZeroLength)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KMemMoveOverlap)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strcpy
	else if (aStepName == KStrcpyBasictest)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KStrcpyWriteOver)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KStrcpyBoundry)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strncpy
	else if (aStepName == KStrncpyBasictest)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KStrncpyCutCnt)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KStrncpyCutNul)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KStrncpyNulInc)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KStrncpyZeroLen)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KStrncpyZeroSrc)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strcat
	else if (aStepName == KStrcatBasictest)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KStrcatBoundry)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KStrcatMemset)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strncat
	else if (aStepName == KStrncatBasictest)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KStrncatBoundry)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KStrncatMemset)
		{
		testStep = new CTestString(aStepName);
		}
	
	//memcmp
	else if (aStepName == KMemcmpBasictest)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KMemcmpUnequal)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KMemcmpLtdCount)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KMemcmpZeroCount)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strcmp
	else if (aStepName == KStrcmpBasictest)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KStrcmpMisMatchStrAndLen)
		{
		testStep = new CTestString(aStepName);
		}

	
	//strncmp
	else if (aStepName == KStrncmpBasictest)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KStrncmpMisMatchStrAndLen)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KStrncmpCountCheck)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strcoll
	else if (aStepName == KStrcollBasictest)
		{
		testStep = new CTestString(aStepName);
		}

	//srxfrm
	else if (aStepName == KSrxfrmBasictest)
		{
		testStep = new CTestString(aStepName);
		}
	
	//memchr
	else if (aStepName == KmemchrNotFound)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KmemchrBasictest)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KmemchrFindNul)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KmemchrZeroCount)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strchr
	else if (aStepName == KstrchrNotFound)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrchrBasicTest)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrchrFindNul)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrchrFindFirst)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrchrEmptyString)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrchrNulEmptyStr)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strcspn
	else if (aStepName == KstrcspnWholeStr)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrcspnPartString)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrcspnNone)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrcspnNullString)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrcspnNullSearch)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strpbrk
	else if (aStepName == KstrpbrkNotFound)
		{
		testStep = new CTestString(aStepName);
		}	
	else if (aStepName == KstrpbrkBasictest)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrpbrkEmptySearch)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrpbrkEmptyString)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrpbrkBothEmpty)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrpbrkFindFirst)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strrchr
	else if (aStepName == KstrrchrNotfound)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrrchrBasictest)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrrchrFindNul)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrrchrFindLast)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrrchrEmptyStr)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrrchrEmptyNul)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strspn
	else if (aStepName == KstrspnWholeStr)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrspnPartial)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrspnNone)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrspnNulStr)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrspnNulSearch)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strstr
	else if (aStepName == KstrstrNotFound)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrstrBasictest)
		{
		testStep = new CTestString(aStepName);
		}	
	else if (aStepName == KstrstrLong)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrstrFindEmpty)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrstrEmptyStr)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrstrEmpty)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strtok
	else if (aStepName == KstrtokBasictest)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrtokDelim)
		{
		testStep = new CTestString(aStepName);
		}	
	else if (aStepName == KstrtokChangeDelim)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrtokNewStr)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrtokDiffSap)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrtokNoTok)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrtokEmptyStr)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrtokNoDelim)
		{
		testStep = new CTestString(aStepName);
		}	
	else if (aStepName == KstrtokEmptyDelim)
		{
		testStep = new CTestString(aStepName);
		}

	//memset
	else if (aStepName == KmemsetBasictest)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KmemsetZeroLen)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strerror
	else if (aStepName == KstrerrorBasictest)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrerrorUnknown)
		{
		testStep = new CTestString(aStepName);
		}


	//strlen
	else if (aStepName == KstrlenEmpty)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KstrlenSingleChar)
		{
		testStep = new CTestString(aStepName);
		}	
	else if (aStepName == KstrlenMultiChar)
		{
		testStep = new CTestString(aStepName);
		}
	
	//bcmp
	else if (aStepName == Kbcmpsameargs)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kbcmpdiffargs)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kbcmpzerocnt)
		{
		testStep = new CTestString(aStepName);
		}
	
	//bcopy
	else if (aStepName == Kbcopyfuncheck)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kbcopybasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kbcopyzerocopy)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kbcopysrcconst)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kbcopynullsrc)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kbcopyoverlap)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strsep
	else if (aStepName == Kstrsepbasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrsepsrccheck)
		{
		testStep = new CTestString(aStepName);
		}
	
	//bzero
	else if (aStepName == Kbzerobasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kbzeronocopy)
		{
		testStep = new CTestString(aStepName);
		}
	
	//ffs
	else if (aStepName == Kffsbasic)
		{
		testStep = new CTestString(aStepName);
		}
	
	//index
	else if (aStepName == Kindexbasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kindexnotfound)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kindexend)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kindexbeginning)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kindexfindnull)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kindexnullsrc)
		{
		testStep = new CTestString(aStepName);
		}
	
	//rindex
	else if (aStepName == Krindexbasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Krindexnotfound)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Krindexend)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Krindexbeginning)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Krindexfindnull)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Krindexnullsrc)
		{
		testStep = new CTestString(aStepName);
		}
	
	//memcpy
	else if (aStepName == Kmemcpybasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kmemcpyzerolength)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kmemcpysrcconstantness)
		{
		testStep = new CTestString(aStepName);
		}
	
	//stpcpy
	else if (aStepName == Kstpcpybasic)
		{
		testStep = new CTestString(aStepName);
		}	
	
	//strcasecmp
	else if (aStepName == Kstrcasecmpcasecheck)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrcasecmplargesrc)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrcasecmpsmallsrc)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strncmp
	else if (aStepName == Kstrncmpbasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrncmplargesrc)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrncmpsmallsrc)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strcoll
	else if (aStepName == Kstrcollbasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrcolllargesrc)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrcollsmallsrc)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strndup
	else if (aStepName == Kstrndupbasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrnduplargecnt)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrndupequalcnt)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrndupzerocnt)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrndupnullsrc)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strnlen
	else if (aStepName == Kstrnlenbasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrnlensmallstr)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrnlennullstr)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strtof
	else if (aStepName == Kstrtofbasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtofexpntn)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtofsignedexpntn)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtofinfinity)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtofnan)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strtoimax
	else if (aStepName == Kstrtoimaxbasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtoimaxspacecheck)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtoimaxsignedsrc)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtoimaxoctalcheck)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtoimaxhexcheck)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strtoumax
	else if (aStepName == Kstrtoumaxbasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtoumaxspacecheck)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtoumaxoctalcheck)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtoumaxhexcheck)
		{
		testStep = new CTestString(aStepName);
		}

	//strtold
	else if (aStepName == Kstrtoldbasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtoldexpntn)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtoldsignedexpntn)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtoldinfinity)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtoldnan)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strtoll
	else if (aStepName == Kstrtollbasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtollspacecheck)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtollsignedsrc)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtolloctalcheck)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtollhexcheck)
		{
		testStep = new CTestString(aStepName);
		}
	
	//strtoull
	else if (aStepName == Kstrtoullbasic)
		{
		testStep = new CTestString(aStepName);
		}	
	else if (aStepName == Kstrtoullspacecheck)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtoulloctalcheck)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtoullhexcheck)
		{
		testStep = new CTestString(aStepName);
		}
	
	//swab
	else if (aStepName == Kswabbasic)
		{
		testStep = new CTestString(aStepName);
		}
	
	else if (aStepName == Kstrtoqbasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtoqspacecheck)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtoqsignedsrc)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtoqoctalcheck)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtoqhexcheck)
		{
		testStep = new CTestString(aStepName);
		}
	
	else if (aStepName == Kstrtouqbasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtouqspacecheck)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtouqoctalcheck)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrtouqhexcheck)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrxfrmbasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrptimebasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrftimebasic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrptime_arabic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrptime_heutf)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrftime_arabic)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrftime_heutf)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrftime_timezone)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTestStrtok_r)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTestStrnstr)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTestStrcasestr)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTestMemccpy)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTestStpcpy)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTestWcsColl)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTstWcsCollUnlikestr1)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTstWcsCollUnlikestr2)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTstWcsxfrm)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTstWcsxfrmErr)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTstStrdup)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTstStrndup)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTstWcsdup)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTstStrsep)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrptimebasic1)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTstldexp)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTestmemchr_specialchars)
		{
		testStep = new CTestString(aStepName);
		}	
	else if (aStepName == KStrlcatBasictest)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTestStrcasestr1)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kffsbasic1)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTestStrnstr1)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTestStrnstr2)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTestStrnstr3)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTestStrncasecmp)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTestStrncasecmp1)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTestMemccpy1)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KStrncattest1)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kbcopyfuncheck1)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KStrlcpyBasictest)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KStrlcpyBasictest1)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KStrlcatBasictest1)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTstWcsxfrm1)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTstWcsxfrm2)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTstWcsxfrm3)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTstStrdup2)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTstStrerr_r)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTstStrerr_r1)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTestStrcasestr2)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTestStrnstr4)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == Kstrxfrmbasic1)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTestStrptime_test)
		{
		testStep = new CTestString(aStepName);
		}
	else if (aStepName == KTestStrptime_test1)
		{
		testStep = new CTestString(aStepName);
		}
	return testStep;
	
	}

