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



#ifndef __TESTNETDB_H__
#define __TESTNETDB_H__

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
#include <unistd.h>
#include <stdapis/net/if.h>

#define MAXBUF 256

_LIT(KGethostbynameNegativeTest, "GethostbynameNegativeTest");
_LIT(KGetHostByNameLocalHost, "GetHostByNameLocalHost");
_LIT(KGethostbyaddrTest, "GethostbyaddrTest");
_LIT(KGetprotobynameTest, "GetprotobynameTest");
_LIT(KGetprotobynumberTest, "GetprotobynumberTest");
_LIT(KGetservbynameTest, "GetservbynameTest");
_LIT(KGetservbyportTest, "GetservbyportTest");
_LIT(KGetaddrinfoTest, "GetaddrinfoTest");
_LIT(KGetnameinfoTest, "GetnameinfoTest");
_LIT(KGetnameinfoTest1, "GetnameinfoTest1");
_LIT(KSetserventTest, "SetserventTest");
_LIT(KGai_strerrorTest, "Gai_strerrorTest");
_LIT(KFreeaddrinfoTest, "FreeaddrinfoTest");
_LIT(KGetserventTest, "GetserventTest");
_LIT(KEndserventTest, "EndserventTest");
_LIT(KSetDefaultIfTest1, "SetDefaultIfTest1");
_LIT(KSetDefaultIfTest2, "SetDefaultIfTest2");
_LIT(KGetAddrInfoMultipleIPTest, "GetAddrInfoMultipleIPTest");
_LIT(KGetnameinfoTest_localhost, "GetnameinfoTest_localhost");
_LIT(KGetAddrInfoNegativeTests, "GetAddrInfoNegativeTests");

class CTestNetdb : public CTestStep
	{
public:
	~CTestNetdb(); 
	CTestNetdb(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
    virtual TInt GethostbyaddrTest();
    virtual TInt GetprotobynameTest();
    virtual TInt GetprotobynumberTest();
    virtual TInt GetservbynameTest();
    virtual TInt GetservbyportTest();
    virtual TInt GetaddrinfoTest();
    virtual TInt GetnameinfoTest();
    virtual TInt GetnameinfoTest1();
    virtual TInt SetserventTest();
    virtual TInt Gai_strerrorTest();
    virtual TInt FreeaddrinfoTest();
    virtual TInt GetserventTest();
    virtual TInt EndserventTest();
    virtual TInt GethostbynameNegativeTest();
    virtual TInt GetHostByNameLocalHost();
    virtual TInt SetDefaultIfTest1();
    virtual TInt SetDefaultIfTest2();
    virtual TInt GetAddrInfoMultipleIPTest();
    virtual TInt GetnameinfoTest_localhost();
    virtual TInt GetAddrInfoNegativeTests();
public:
	TInt iParamCnt;  
	};
#endif 
