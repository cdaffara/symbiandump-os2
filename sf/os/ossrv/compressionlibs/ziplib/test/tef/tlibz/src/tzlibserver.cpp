// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tzlibserver.cpp
// 
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
#include "tzlibserver.h"
#include "tzlib.h"


_LIT(KServerName, "tzlib");

CZlibTestServer* CZlibTestServer::NewL()
	{
	CZlibTestServer *server = new(ELeave) CZlibTestServer();
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
	CZlibTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CZlibTestServer::NewL());
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

CTestStep* CZlibTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	
	if(aStepName == Ktest_zlibVersion)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_compress01)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_deflate01)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_deflate02)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_deflateEnd)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_inflate01)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_inflate02)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_inflate03)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_inflate04)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_inflate05)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_inflate06)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_inflateEnd)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_deflateSetDictionary01)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_deflateSetDictionary02)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_deflateSetDictionary03)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_deflateSetDictionary04)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_deflateSetDictionary05)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_gzio)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_gzdirect)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == Ktest_gzdirectnull)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == Ktest_gzclearerr)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == Ktest_gzclearerr_null)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == Ktest_gzerror_streamend)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == Ktest_gzungetcnegative)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_gzseeknegative)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == Ktest_gzdirecttxt)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzungetcChain)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzseekBack)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzseekAppend)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzseekHugeOffset)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzseekNoSize)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzopenLongPath01)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzseekLongPath01)
		{
		testStep = new CTestZlib(aStepName);
		}		
	else if(aStepName == KtestGzopenLongPath02)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzseekMixedFile01)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzopenNoMode)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzopenNoPath)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzopenNoPathMode)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzseekConcatedFile01)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzopenDiffMode)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzseekConcatedFile02)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzprintf01)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzprintf02)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzflushNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzflushRepeat)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzflushHugeBuf)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzrewindNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzrewindTransparent)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzgetsBufNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzgetsSmallBuf)
		{
		testStep = new CTestZlib(aStepName);
		}
	
	else if(aStepName == KtestDeflateReset)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestDeflateReset_fail)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestDeflateInit2_bits)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestDeflateInit2_level)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestInflateInit2_bits)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzread)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzread_fail)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestUncompress)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestUncompressfail)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestInflateSetDictionary)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestInflateSetDictionary_size)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestInflateSetDictionary_null)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzgets)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestgzgetsFail)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestgzgetsopenFail)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestInflate)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestInflate_fail1)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestInflate_fail2)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestInflate_fail3)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestInflateend)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestInflateend_fail)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestInflateReset)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestInflateResetfail1)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestInflateInit2_)
		{
		testStep = new CTestZlib(aStepName);
		}			
	else if(aStepName == KtestInflateInit_)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestInflateInit2_negative)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestInflateInit_negative)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestInflateInit2_versioncheck)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestInflateInit_versioncheck)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestAdlerinit)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestAdler)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestCompress)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestCompress_negative)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestCompress2_positive)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestCompress2_negative)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestCompressbound)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestDeflatebound)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestDeflateparams)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestDeflateparamsfail1)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestDeflateparamsfail2)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestCrcinit)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestCrc)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGet_crc_table)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestDeflateInit_)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestDeflateInit_level)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestDeflateInit2_)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestDeflatefail)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestDeflatefail2)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestZlibversion)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzputc)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzopen)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzopenmode)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzopenfail)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzputcfail)
		{
		testStep = new CTestZlib(aStepName);
		}							
	else if(aStepName == KtestGzputcreturn)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzputs)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzputsfail)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzprintf)
		{
		testStep = new CTestZlib(aStepName);
		}		
	else if(aStepName == KtestGzprintf_trying)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzwrite)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzwritefail)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGztell)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGztell1)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGztellfail1)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestDeflatecopy)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestDeflatecopyfail)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzclose)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzclose_fail)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzeof)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzeoffail1)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzeoffail2)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzgetc)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzflush)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzflushsync)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzflushfail)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzerror)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzerrorfail1)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzgetcfail)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestDeflateSetDictionary)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestDeflateSetDictionary_nodict)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestDeflateSetDictionary_fail)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestDeflateend)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestDeflateendfail1)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestDeflate)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzseek)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzseekfail1)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzseekfail2)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzsetparams)
		{
		testStep = new CTestZlib(aStepName);
		}		
	else if(aStepName == KtestGzsetparams_fail1)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzsetparams_fail2)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzsetparams_fail3)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzrewind)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzrewindfail)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzdopen)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestGzdopen_fail)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestGzdopen_fail2)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestInflateSync)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestinflateSyncfail)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestInflateSyncPoint)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestInflateSyncPoint_null)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KtestZerror)
		{
		testStep = new CTestZlib(aStepName);
		}	
	else if(aStepName == KtestZerror1)
		{
		testStep = new CTestZlib(aStepName);
		}		
	else if(aStepName == KtestZerror2)
		{
		testStep = new CTestZlib(aStepName);
		}
	
	
	else if(aStepName == KTestDeflateTest01)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflatecopyDestNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateCopyStreamStateNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateInit2_WindowBits)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateInit2_StreamSize)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateInit2_MemLevel)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateInit2_Level)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateInit2_Strategy)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateInit2_Version)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateInit2_VersionNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateInit2_StreamNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateInit2_Version)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateInit2_VersionNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateInit2_WindowBits)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateInit2_StreamNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflate_HuffmanStrategy)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflate_AvailInZero)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestGzsetparamDefaultCompression)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateResetNullStream)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateResetStreamStateNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflate_Scenarios)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflate_NullStream)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflate_StreamStateNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateEndNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateEndStreamStateNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflate_WindowBits)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateBoundStreamNotNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateBoundStreamNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateBoundStreamStateNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateSetDictionaryWrap)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateSetDictionaryLen)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateParamsStreamStateNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateSyncAvailInNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateSyncStreamNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateSyncStreamStateNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestAdlerScenarios)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestGzsetparamsFileNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateTest01)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflatecopyDestNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateCopyStreamStateNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateInit2_WindowBits)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateInit2_StreamSize)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateInit2_MemLevel)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateInit2_Level)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateInit2_Strategy)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateInit2_Version)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateInit2_VersionNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateInit2_StreamNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateInit2_Version)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateInit2_VersionNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateInit2_WindowBits)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateInit2_StreamNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflate_HuffmanStrategy)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflate_AvailInZero)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestGzsetparamDefaultCompression)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateResetNullStream)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateResetStreamStateNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflate_Scenarios)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflate_NullStream)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflate_StreamStateNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateEndNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateEndStreamStateNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflate_WindowBits)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateBoundStreamNotNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateBoundStreamNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateBoundStreamStateNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateSetDictionaryWrap)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateSetDictionaryLen)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateSetDictionaryBadMode)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateSetDictionaryStreamStateNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateParamsStreamStateNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateSyncAvailInNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateSyncStreamNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateSyncStreamStateNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateSyncPointStreamStateNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestAdlerScenarios)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestGzsetparamsFileNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestGzopenWrite)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestGzreadLargeFile)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestGzopenWriteNoPath)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestGzreadLenZero)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestGzreadBufZero)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestGzreadNonGzFile)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestGzrewindNonGzFile)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestGzrewindFileNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestGzflushWithZFinish)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestUncompressLenSmall)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestUncompressBufNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestUncompressLenNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateScenarios)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateStreamStateNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateResetStreamStateNull)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateTuneL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateTuneFailL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDefInfGZipDefaultHeaderIgnoreL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDefInfGZipDefaultHeaderAutoL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDefInfGZipSpecifiedHeaderManualL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDefInfGZipSpecifiedHeaderAutoL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDefInfZlibHeaderAutoL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflateSetHeaderFailsL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateGetHeaderFailsL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDefInfRawL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDefRawFailsL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDefInfRawFailsL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflatePrimeL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestDeflatePrimeFailsL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflatePrimeFailsL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateCopyL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateCopyFailsParamsL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateCopyFailsMemL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateBackL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateBackEndFailsL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateBackFailsL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateBackInitFailsParamsL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestInflateBackInitFailsMem)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestAdler32CombineL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestCrc32CombineL)
		{
		testStep = new CTestZlib(aStepName);
		}
	else if(aStepName == KTestZlibCompileFlagsL)
		{
		testStep = new CTestZlib(aStepName);
		}
	
	return testStep;
	}

