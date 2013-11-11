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



#ifndef __TESTINET_H__
#define __TESTINET_H__

// INCLUDE FILES
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

#define BUFSIZE 20

_LIT(KInet_addr_with_valid_input, "Inet_addr_with_valid_input");
_LIT(KInet_addr_with_invalid_input, "Inet_addr_with_invalid_input");
_LIT(KInet_ntoaTest, "Inet_ntoaTest");
_LIT(KInet_ptonTest, "Inet_ptonTest");
_LIT(KInet_ntopTest, "Inet_ntopTest");

class CTestInet : public CTestStep
	{
public:
	~CTestInet(); 
	CTestInet(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	TInt Inet_addr_with_valid_input();
	TInt Inet_addr_with_invalid_input();
	TInt Inet_ntoaTest();
	TInt Inet_ptonTest();
	TInt Inet_ntopTest();
public: //data
	TInt iParamCnt;
	};
#endif 
