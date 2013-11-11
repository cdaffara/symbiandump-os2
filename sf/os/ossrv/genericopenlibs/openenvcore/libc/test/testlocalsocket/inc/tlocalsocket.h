/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*  Name        : tlocalsocket.h
*
*
*/




#ifndef __TESTLOCALSOCKET_H__
#define __TESTLOCALSOCKET_H__

#include <test/TestExecuteStepBase.h>

// INCLUDE FILES
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>

#include <e32svr.h>
#include <e32def.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <e32std.h>
#include <sys/unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#include <net/if.h>
#include <sys/sockio.h>


_LIT(KTestBind, "TestBind");
_LIT(KTestLseek, "TestLseek");
_LIT(KTestFstat, "TestFstat");
_LIT(KTestThreadSocketRead, "TestThreadSocketRead");
_LIT(KTestThreadSocketWrite, "TestThreadSocketWrite");
_LIT(KTestThreadSocketRecv, "TestThreadSocketRecv");
_LIT(KTestThreadSocketSend, "TestThreadSocketSend");
_LIT(KTestMultProc, "TestMultProc");
_LIT(KTestMultThread, "TestMultThread");
_LIT(KTestSetGetSockOpt, "TestSetGetSockOpt");
_LIT(KTestSetSockOptNegative1, "TestSetSockOptNegative1");
_LIT(KTestSetSockOptNegative2, "TestSetSockOptNegative2");
_LIT(KTestSetSockOptNegative3, "TestSetSockOptNegative3");
_LIT(KTestSetSockOptNegative4, "TestSetSockOptNegative4");
_LIT(KTestGetSockOptNegative1, "TestGetSockOptNegative1");
_LIT(KTestGetSockOptNegative2, "TestGetSockOptNegative2");
_LIT(KTestGetSockOptNegative3, "TestGetSockOptNegative3");
_LIT(KTestGetSockOptNegative4, "TestGetSockOptNegative4");
_LIT(KTestLocalSockIoctl, "TestLocalSockIoctl");
_LIT(KTestLocalSockFcntl, "TestLocalSockFcntl");
_LIT(KTestNegativeBind1, "TestNegativeBind1");
_LIT(KTestNegativeBind2, "TestNegativeBind2");
_LIT(KTestNegativeBind3, "TestNegativeBind3");
_LIT(KTestNegativeBind4, "TestNegativeBind4");
_LIT(KTestNegativeBind5, "TestNegativeBind5");
_LIT(KTestListenonUnixsoc, "TestListenonUnixsoc");
_LIT(KTestNonblockAcceptSetfl, "TestNonblockAcceptSetfl");
_LIT(KTestNonblockAcceptSelect, "TestNonblockAcceptSelect");
_LIT(KTestNonblockingRead, "TestNonblockingRead");
_LIT(KTestNonblockingRecv, "TestNonblockingRecv");
_LIT(KTestGetSockName, "TestGetSockName");
_LIT(KTestGetPeerName, "TestGetPeerName");
_LIT(KTestThreadRecvfrom, "TestThreadRecvfrom");
_LIT(KTestThreadSendto, "TestThreadSendto");
_LIT(KTestLocalSockMultipleBind, "TestLocalSockMultipleBind");

class CTestLocalSocket : public CTestStep
	{
public:
	~CTestLocalSocket(); 
	CTestLocalSocket(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	void GetParameters(char aParamets[10][256]);
	TVerdict TestBind();
	TVerdict TestLseek();
	TVerdict TestFstat();
	TVerdict TestThreadSocketRead();
	TVerdict TestThreadSocketWrite();
	TVerdict TestThreadSocketRecv();
	TVerdict TestThreadSocketSend();
	TVerdict TestMultProc();
	TVerdict TestMultThread();
	TVerdict TestSetGetSockOpt();
	TVerdict TestSetSockOptNegative1();
	TVerdict TestSetSockOptNegative2();
	TVerdict TestSetSockOptNegative3();
	TVerdict TestSetSockOptNegative4();
	TVerdict TestGetSockOptNegative1();
	TVerdict TestGetSockOptNegative2();
	TVerdict TestGetSockOptNegative3();
	TVerdict TestGetSockOptNegative4();
	TVerdict TestLocalSockIoctl();
	TVerdict TestLocalSockFcntl();
	TVerdict TestNegativeBind1();
	TVerdict TestNegativeBind2();
	TVerdict TestNegativeBind3();
	TVerdict TestNegativeBind4();
	TVerdict TestNegativeBind5();
	TVerdict TestListenonUnixsoc();
	TVerdict TestNonblockAcceptSetfl();
	TVerdict TestNonblockAcceptSelect();
	TVerdict TestNonblockingRead();
	TVerdict TestNonblockingRecv();
	TVerdict TestGetSockName();
	TVerdict TestGetPeerName();
	TVerdict TestThreadRecvfrom();
	TVerdict TestThreadSendto();
	TVerdict TestLocalSockMultipleBind();
	};
#endif 
