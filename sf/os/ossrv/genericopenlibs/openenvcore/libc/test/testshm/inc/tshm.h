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



#ifndef __TESTSHM_H__
#define __TESTSHM_H__

#include <test/testexecutestepbase.h>

#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>

#include <e32svr.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <pthread.h>


#define MAX_IPC_OBJ 257

_LIT(KIntgTest1, "IntgTest1");
_LIT(KIntgTest2, "IntgTest2");
_LIT(KIntgTest3, "IntgTest3");
_LIT(KIntgTest4, "IntgTest4");
_LIT(KIntgTest5, "IntgTest5");
_LIT(KIntgTest6, "IntgTest6");
_LIT(KIntgTest7, "IntgTest7");
_LIT(KShmCreate, "ShmCreate");
_LIT(KShmAttach, "ShmAttach");
_LIT(KShmDetach, "ShmDetach");
_LIT(KShmControl, "ShmControl");
_LIT(KCreateThreadL, "CreateThreadL");
_LIT(KCreateThreadL1, "CreateThreadL1");

_LIT(Kshmgettest1, "shmgettest1");
_LIT(Kshmgettest2, "shmgettest2");
_LIT(Kshmgettest3, "shmgettest3");
_LIT(Kshmgettest4, "shmgettest4");
_LIT(Kshmgettest5, "shmgettest5");
_LIT(Kshmgettest6, "shmgettest6");
_LIT(Kshmgettest7, "shmgettest7");
_LIT(Kshmgettest8, "shmgettest8");
_LIT(KShmgetCreateKey, "ShmgetCreateKey");
_LIT(Kshmctltest1, "shmctltest1");
_LIT(Kshmctltest2, "shmctltest2");
_LIT(Kshmctltest3, "shmctltest3");
_LIT(Kshmctltest4, "shmctltest4");
_LIT(Kshmctltest5, "shmctltest5");
_LIT(Kshmattest1, "shmattest1");
_LIT(Kshmattest2, "shmattest2");
_LIT(Kshmattest3, "shmattest3");
_LIT(Kshmattest4, "shmattest4");
_LIT(Kshmattest5, "shmattest5");
_LIT(Kshmdttest1, "shmdttest1");
_LIT(Kshmopsvalidatetest, "shmopsvalidatetest");

class CTestShm : public CTestStep
	{
public:

	
	~CTestShm(); 
	CTestShm(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	
	//Helper functions for CreateThreadL1 
	int myerror(const char*, FILE*);
	int body(void*);
	int myfunc1(void*);
	int myfunc2(void*);
	int myfunc3(void*);
	
private:
	
	TInt *iNoOfShmIds;
    char* iShmAddr;
    TInt iThreadId;
    TInt iParamCnt;
        
	TInt ShmKey(TInt& aKey);
	TInt ShmSize(TInt& aSize);
	TInt ShmFlag(TInt& aFlag);
	TInt ShmAddr(char** aAddr);
	TInt ShmId(TInt& aId );
	TInt ShmCmd(TInt& aCmd);
	TInt ShmSegs(TInt& aSegs);
	TInt ErrCode( TInt& aErrCode );
	TInt CheckShmStats(struct shmid_ds& buf);
	void SetShmParms(struct shmid_ds* buf);
	TInt ShmCreate();
	TInt ShmClose();
	TInt ShmAttach();
	TInt ShmDetach();
	TInt ShmControl();
	TInt CreateThreadL();	
	TInt CreateThreadL1();
	
	TInt ShmServerWrite();
	
	TInt IntgTest1();
	TInt IntgTest2();
	TInt IntgTest3();
	TInt IntgTest4();
	TInt IntgTest5();
	TInt IntgTest6(); 
	TInt IntgTest7();
 

	TInt shmgettest1();
	TInt shmgettest2();
	TInt shmgettest3();
	TInt shmgettest4();
	TInt shmgettest5();
	TInt shmgettest6();
	TInt shmgettest7();
	TInt shmgettest8();
	TInt ShmgetCreateKey();
	TInt shmctltest1();
	TInt shmctltest2();
	TInt shmctltest3();
	TInt shmctltest4();
	TInt shmctltest5();
	TInt shmattest1();
	TInt shmattest2();
	TInt shmattest3();
	TInt shmattest4();
	TInt shmattest5();
	TInt shmdttest1();
	TInt shmopsvalidatetest();	
	};
	void* ThreadEntryFunctionRead(void *arg);
#endif 
