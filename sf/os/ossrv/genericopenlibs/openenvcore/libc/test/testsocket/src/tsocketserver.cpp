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
#include "tsocketserver.h"
#include "tsocket.h"


_LIT(KServerName, "tsocket");

CSocketTestServer* CSocketTestServer::NewL()
	{
	CSocketTestServer *server = new(ELeave) CSocketTestServer();
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
	CSocketTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CSocketTestServer::NewL());
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

CTestStep* CSocketTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if(aStepName == KExampleL)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KUDP)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KTCP)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KGetSocketName)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KGetSocketNameUsingFileDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KGetSocketNameUsingInvalidSocketDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}	
	if(aStepName == KGetSocketNameInvalidLength)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KGetSocketNameInvalidSocketBuffer)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KGetPeerSocketName)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KGetPeerSocketNameUsingFileDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KGetPeerSocketNameUsingInvalidSocketDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KGetPeerSocketNameforUnconnectedSocket)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KGetPeerSocketNameInvalidLengthSockAddr)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KBindTest)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KMultipleBindOnSameSocket)
		{
		testStep = new CTestSocket(aStepName);
		}        
	if(aStepName == KBindInvalidAddress)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KBindUsingInvalidSocketDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KBindUsingFileDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KShutdownTest)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KShutDownTestWithInvalidShutdownOption)
		{
		testStep = new CTestSocket(aStepName);
		}       
	if(aStepName == KShutdownUsingFileDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KShutdownUsingInvalidSocketDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KShutdownDisconnectedSocket)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KSocketTest)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KListenTest)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KListenUsingFileDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KListenUsingInvalidSocketDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KAcceptTest)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KUDPAccept)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KAcceptUsingFileDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KAcceptUsingInvalidSocketDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KConnectTestFailCases)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KConnectUsingFileDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KConnectUsingInvalidSocketDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KRecvTestFailCases)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KRecvUsingInvalidSocketDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KRecvUsingFileDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KSendTestFailCases)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KSendUsingFileDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KSendUsingInvalidSocketDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KSocketOptions)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KGetSockOptFailCases)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KSetSockOptFailCases)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KSendToTestFailCases)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KSendToUsingFileDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KSendToUsingInvalidSocketDescriptor)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KTestSendReturnValue)
		{
		testStep = new CTestSocket(aStepName);
		}   
	if(aStepName == KRecvFromTestFailCases)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KSockAtMark)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KBindResvPort)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KBindResvPortFailCases)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KHErrNoLocation)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KSendMsgRecvMsg)
		{
		testStep = new CTestSocket(aStepName);
		}	
	if(aStepName == KTestRecv)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KTestReadStream)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KTestReadDatagram)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KBindFailCases)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KAcceptFailCases)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KAcceptTestZeroAddrLen)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KAcceptTestNullAddr)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KListenUdp)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KGetHostName)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KGetHostNameNull)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KGetHostNameZero)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KTestSktlseek)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KTestSockAfLocal)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KTestSktfstat)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KTestSktfsync)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KTestGreaterThan16kReadWrite)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KTestNotify)
			{
			testStep = new CTestSocket(aStepName);
			}
	if(aStepName == KTestRecvMsgPeekTCP)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KTestRecvMsgPeekUDP)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KTestLargeUDP)
			{
			testStep = new CTestSocket(aStepName);
			}
	if(aStepName == KTestFionread)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KTestBind1)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KTestConnectNonBlocking)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KTCP1)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KTestV4MappedAddress)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KReadTestFailCase)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KTestNotify1)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KTestSockFcntl)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KTestSockIoctl)
		{
		testStep = new CTestSocket(aStepName);
		}
	if(aStepName == KSockErrnoTest)
			{
			testStep = new CTestSocket(aStepName);
			}
	if(aStepName == KSockSendOnClosedConn )
            {
            testStep = new CTestSocket(aStepName);
            }
	if(aStepName == KSockWriteOnClosedConn )
	    {
        testStep = new CTestSocket(aStepName);
	    }
	
	return testStep;
	}
