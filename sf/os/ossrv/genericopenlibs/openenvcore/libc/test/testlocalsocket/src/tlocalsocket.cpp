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
// Name        : tlocalsocket.cpp
// 
//

#include "tlocalsocket.h"

CTestLocalSocket::~CTestLocalSocket() 
	{ 
	}  

CTestLocalSocket::CTestLocalSocket(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestLocalSocket::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EFail);
	return TestStepResult();
	}

TVerdict CTestLocalSocket::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;
	return TestStepResult();
	}


TVerdict CTestLocalSocket::doTestStepL()
	{

   	if(TestStepName() == KTestBind)
   		{
		INFO_PRINTF1(_L("TestBind():"));
		TestBind();
		}
	if(TestStepName() == KTestLseek)
   		{
		INFO_PRINTF1(_L("TestLseek():"));
		TestLseek();
		}
	if(TestStepName() == KTestFstat)
   		{
		INFO_PRINTF1(_L("TestFstat():"));
		TestFstat();
		}
	if(TestStepName() == KTestThreadSocketRead)
   		{
		INFO_PRINTF1(_L("TestThreadSocketRead():"));
		TestThreadSocketRead();
		}
	if(TestStepName() == KTestThreadSocketWrite)
   		{
		INFO_PRINTF1(_L("TestThreadSocketWrite():"));
		TestThreadSocketWrite();
		}
	if(TestStepName() == KTestThreadSocketRecv)
   		{
		INFO_PRINTF1(_L("TestThreadSocketRecv():"));
		TestThreadSocketRecv();
		}
	if(TestStepName() == KTestThreadSocketSend)
   		{
		INFO_PRINTF1(_L("TestThreadSocketSend():"));
		TestThreadSocketSend();
		}
	if(TestStepName() == KTestMultProc)
   		{
		INFO_PRINTF1(_L("TestMultProc():"));
		TestMultProc();
		}
	if(TestStepName() == KTestMultThread)
   		{
		INFO_PRINTF1(_L("TestMultThread():"));
		TestMultThread();
		}
	if(TestStepName() == KTestSetGetSockOpt)
   		{
		INFO_PRINTF1(_L("TestSetGetSockOpt():"));
		TestSetGetSockOpt();
		}
	if(TestStepName() == KTestSetSockOptNegative1)
   		{
		INFO_PRINTF1(_L("TestSetSockOptNegative1():"));
		TestSetSockOptNegative1();
		}
	if(TestStepName() == KTestSetSockOptNegative2)
   		{
		INFO_PRINTF1(_L("TestSetSockOptNegative2():"));
		TestSetSockOptNegative2();
		}
	if(TestStepName() == KTestSetSockOptNegative3)
   		{
		INFO_PRINTF1(_L("TestSetSockOptNegative3():"));
		TestSetSockOptNegative3();
		}
	if(TestStepName() == KTestSetSockOptNegative4)
   		{
		INFO_PRINTF1(_L("TestSetSockOptNegative4():"));
		TestSetSockOptNegative4();
		}
	if(TestStepName() == KTestGetSockOptNegative1)
   		{
		INFO_PRINTF1(_L("TestGetSockOptNegative1():"));
		TestGetSockOptNegative1();
		}
	if(TestStepName() == KTestGetSockOptNegative2)
   		{
		INFO_PRINTF1(_L("TestGetSockOptNegative2():"));
		TestGetSockOptNegative2();
		}
	if(TestStepName() == KTestGetSockOptNegative3)
   		{
		INFO_PRINTF1(_L("TestGetSockOptNegative3():"));
		TestGetSockOptNegative3();
		}
	if(TestStepName() == KTestGetSockOptNegative4)
   		{
		INFO_PRINTF1(_L("TestGetSockOptNegative4():"));
		TestGetSockOptNegative4();
		}
	if(TestStepName() == KTestLocalSockIoctl)
   		{
		INFO_PRINTF1(_L("TestLocalSockIoctl():"));
		TestLocalSockIoctl();
		}
	if(TestStepName() == KTestLocalSockFcntl)
   		{
		INFO_PRINTF1(_L("TestLocalSockFcntl():"));
		TestLocalSockFcntl();
		}
	if(TestStepName() == KTestNegativeBind1)
   		{
		INFO_PRINTF1(_L("TestNegativeBind1():"));
		TestNegativeBind1();
		}
	if(TestStepName() == KTestNegativeBind2)
   		{
		INFO_PRINTF1(_L("TestNegativeBind2():"));
		TestNegativeBind2();
		}
	if(TestStepName() == KTestNegativeBind3)
   		{
		INFO_PRINTF1(_L("TestNegativeBind3():"));
		TestNegativeBind3();
		}
	if(TestStepName() == KTestNegativeBind4)
   		{
		INFO_PRINTF1(_L("TestNegativeBind4():"));
		TestNegativeBind4();
		}
	if(TestStepName() == KTestNegativeBind5)
   		{
		INFO_PRINTF1(_L("TestNegativeBind5():"));
		TestNegativeBind5();
		}
	if(TestStepName() == KTestListenonUnixsoc)
   		{
		INFO_PRINTF1(_L("TestListenonUnixsoc():"));
		TestListenonUnixsoc();
		}
	if(TestStepName() == KTestNonblockAcceptSetfl)
   		{
		INFO_PRINTF1(_L("TestNonblockAcceptSetfl():"));
		TestNonblockAcceptSetfl();	
		}
	if(TestStepName() == KTestNonblockAcceptSelect)
   		{
		INFO_PRINTF1(_L("TestNonblockAcceptSelect():"));
		TestNonblockAcceptSelect();	
		}
	if(TestStepName() == KTestNonblockingRead)
   		{
		INFO_PRINTF1(_L("TestNonblockingRead():"));
		TestNonblockingRead();	
		}
	if(TestStepName() == KTestNonblockingRecv)
   		{
		INFO_PRINTF1(_L("TestNonblockingRecv():"));
		TestNonblockingRecv();	
		}
	if(TestStepName() == KTestGetSockName)
   		{
		INFO_PRINTF1(_L("TestGetSockName():"));
		TestGetSockName();	
		}
	if(TestStepName() == KTestGetPeerName)
   		{
		INFO_PRINTF1(_L("TestGetPeerName():"));
		TestGetPeerName();	
		}
	if(TestStepName() == KTestThreadRecvfrom)
   		{
		INFO_PRINTF1(_L("TestThreadRecvfrom():"));
		TestThreadRecvfrom();	
		}
	if(TestStepName() == KTestThreadSendto)
   		{
		INFO_PRINTF1(_L("TestThreadSendto():"));
		TestThreadSendto();	
		}
	if(TestStepName() == KTestLocalSockMultipleBind)
   		{
		INFO_PRINTF1(_L("TestLocalSockMultipleBind():"));
		TestLocalSockMultipleBind();	
		}	
	return TestStepResult(); 
	}
