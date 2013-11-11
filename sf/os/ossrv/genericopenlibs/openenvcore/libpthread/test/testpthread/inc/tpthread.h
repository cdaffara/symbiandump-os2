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



#ifndef __TPTHREAD_H__
#define __TPTHREAD_H__

#include <errno.h>
#include <pthreadtypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdapis/netinet/in.h>


#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdapis/netinet/in.h>
#include <stdapis/arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>  

#include <stdlib.h>
#include <stdio.h>
//Addition over
#include <string.h>
///
#include <test/testexecutestepbase.h>


//Const
const TInt KDefaultPThreadStackSize 	= 0x8000;
const TInt KPortNum 					= 2222;
const TInt KCount 					 	= 10;
const char KFileName[10] 				= "File.txt";


//const TInt KErrPThreadAttrSetSchedParam 		= -1006;
const TInt KErrPThreadAttrGetSchedParam 		= -1007;
const TInt KErrPThreadAttrSetSchedPolicy 		= -1008;
const TInt KErrPThreadAttrGetSchedPolicy 		= -1009;
const TInt KErrPThreadSetSchedParam             = -1010;
//const TInt KErrPThreadGetSchedParam             = -1011;
const TInt KErrPThreadAttrSetScope              = -1012;
const TInt KErrPThreadAttrGetScope              = -1013;
const TInt KErrPThreadKeyCreate                 = -1014;
const TInt KErrPThreadKeyDelete                 = -1015;
const TInt KErrPThreadSetSpecific               = -1016;
const TInt KErrPThreadGetSpecific               = -1017;

//Error Codes
const TInt KErrPThreadSelf 		= -1001;
const TInt KErrPThreadCreate 	= -1002;
const TInt KErrPThreadJoin 		= -1003;
const TInt KErrPThreadDetach 	= -1004;
const TInt KErrPThreadExit 		= -1005;

_LIT(KPThreadSetSchedParam, "pthread_attr_set_schedparam");
_LIT(KPThreadGetSchedParam, "pthread_attr_get_schedparam");
_LIT(KPThreadSetSchedPolicy, "pthread_attr_set_schedpolicy");
_LIT(KPThreadGetSchedPolicy, "pthread_attr_get_schedpolicy");
_LIT(KPThreadSetSchdPrm, "pthread_set_schedparam");
_LIT(KPThreadGetSchdPrm, "pthread_get_schedparam");
_LIT(KPThreadSetScope, "pthread_get_schedparam");
_LIT(KPThreadGetScope, "pthread_get_schedparam");

//Test Type Strings
_LIT(KPositive, "Positive");
_LIT(KNegative, "Negative");

//API Type Strings
_LIT(KPThreadSelf, "pthread_self");
_LIT(KPThreadCreate, "pthread_create");
_LIT(KPThreadJoin, "pthread_join");
_LIT(KPThreadDetach, "pthread_detach");
_LIT(KPThreadExit, "pthread_exit");

_LIT(KPThreadBasicApisTest, "PThreadBasicApisTest");
_LIT(KTestMultiplePThread, "TestMultiplePThread");
_LIT(KSocketReadAndWritePThread, "SocketReadAndWritePThread");
_LIT(KFileReadAndWritePThread, "FileReadAndWritePThread");
_LIT(KPThreadSchedTest, "PThreadSchedTest");
_LIT(KPThreadJoinTest, "PThreadJoinTest");
_LIT(KPThreadExitMemLeak, "PThreadExitMemLeak");


struct TThreadParam
	{
	TInt iDataCount;
	sem_t iSemaphore;
	};


class CTestPThread : public CTestStep
	{
public:
	~CTestPThread(); 
	CTestPThread(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	
private:
		enum TApiType
			{
			EPThreadSelf = 1,
			EPThreadCreate,
			EPThreadJoin,
			EPThreadDetach,
			EPThreadExit,
			EPThreadAttrSetSchedParam,
			EPThreadAttrGetSchedParam,
			EPThreadAttrSetSchedPolicy,
			EPThreadAttrGetSchedPolicy,
			EPThreadSetSchedparam,
			EPThreadGetSchedparam,
			EPThreadAttrSetScope,
			EPThreadAttrGetScope1,
			EPThreadAttrGetScope2,
			EPThreadKeyCreate1,
			EPThreadKeyCreate2,
			EPThreadKeyDelete,
			EPThreadSetSpecific,
			EPThreadGetSpecific,
            //enum addition over.
			};

		enum TTestType
			{
			EPositive = 1,
			ENegative,
			};
private:

	TInt PThreadBasicApisTest();
	TInt TestMultiplePThread();
	TInt SocketReadAndWritePThread();
	TInt FileReadAndWritePThread();
	TInt PThreadSchedTest();
	
	//locals
	TInt PThreadSelfApiTest();
    TInt PThreadCreateApiTest(TTestType aTestType );
    TInt PThreadJoinApiTest(TTestType aTestType );
    TInt PThreadDetachApiTest(TTestType aTestType );
    TInt PThreadExitApiTest(TInt aExitReason );
    
    TInt PThreadAttrSetSchedParmAPITest();
    TInt PThreadAttrGetSchedParmAPITest( TTestType aTestType);
    TInt PThreadAttrSetSchedPolicyAPITest();
    TInt PThreadAttrGetSchedPolicyAPITest(/*TTestType aTestType*/);
    TInt PThreadSetSchedParamAPITest(TTestType aTestType);
	TInt PThreadGetSchedParamAPITest(TTestType aTestType);
	TInt PThreadAttrSetScopeAPITest(); 
	TInt PThreadAttrGetScopeAPITest1();
	TInt PThreadAttrGetScopeAPITest2();
	TInt PThreadKeyCreateAPITest1();
	TInt PThreadKeyDeleteAPITest();
	TInt PThreadSetSpecificAPITest();
	TInt PThreadGetSpecificAPITest();
	TInt PThreadKeyCreateAPITest2();
	TInt PThreadJoinTest();
	TInt PThreadExitMemLeak();
    //Helper functions
	void ReadThreadAttributes(TInt& aThreadState, TInt& aStackSize, TTestType& aTestType);
	inline void LogResult(TTestType aTestType, TPtrC aBuf);
	};

#endif 
