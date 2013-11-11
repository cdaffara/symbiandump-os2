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
* Name        : tmulticastclient.h
* 
*
*/




#ifndef __TESTMULTICASTCLIENT_H__
#define __TESTMULTICASTCLIENT_H__

#include <test/testexecutestepbase.h>

#include <e32svr.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <stdapis/netinet/in.h>
#include <stdapis/arpa/inet.h>
#include <e32std.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/stat.h>
#include <errno.h>
#include <utf.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdapis/net/if.h>
#include <pthread.h>

#define MAXBUF 256

_LIT(KTestClient, "TestClient");

class CTestMulticastClient : public CTestStep
	{
public:
	~CTestMulticastClient(); 
	CTestMulticastClient(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	virtual TInt TestClient();
public:
	TInt iParamCnt;  
	};
#endif 
