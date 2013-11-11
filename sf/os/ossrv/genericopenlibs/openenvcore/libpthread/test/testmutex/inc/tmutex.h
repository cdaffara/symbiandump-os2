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



#ifndef __TESTMUTEX_H__
#define __TESTMUTEX_H__

#include <test/testexecutestepbase.h>
#include <errno.h>
#include <e32svr.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <e32std.h>
#include <stdio.h>

_LIT(KPThreadMutexDestroy, "PThreadMutexDestroy");
_LIT(KPThreadMutexLock, "PThreadMutexLock");
_LIT(KPThreadMutexLockRecursive, "PThreadMutexLockRecursive");
_LIT(KPThreadMutexLockErrorCheck, "PThreadMutexLockErrorCheck");
_LIT(KPThreadMutexTimedlock, "PThreadMutexTimedlock");
_LIT(KPThreadMutexTrylock, "PThreadMutexTrylock");
_LIT(KPThreadMutexUnlock, "PThreadMutexUnlock");
_LIT(KPThreadOnce, "PThreadOnce");
_LIT(KPThreadMutexAttrInit, "PThreadMutexAttrInit");
_LIT(KPThreadMutexAttrInitInvalidPointerL, "PThreadMutexAttrInitInvalidPointerL");

_LIT(KPThreadMutexAttrDestroy, "PThreadMutexAttrDestroy");
_LIT(KPThreadMutexAttrGetpshared, "PThreadMutexAttrGetpshared");
_LIT(KPThreadMutexAttrSetpshared, "PThreadMutexAttrSetpshared");
_LIT(KPThreadMutexattrGettype, "PThreadMutexattrGettype");
_LIT(KPThreadMutexattrSettype, "PThreadMutexattrSettype");
_LIT(KPThreadMutexInit, "PThreadMutexInit");
_LIT(KSemInit, "SemInit");
_LIT(KSemDestroy, "SemDestroy");
_LIT(KSemTryWait, "SemTryWait");
_LIT(KSemWait, "SemWait");
_LIT(KSemTimedWait, "SemTimedWait");
_LIT(KSemPost, "SemPost");
_LIT(KSemClose, "SemClose");
_LIT(KSemUnlink, "SemUnlink");
_LIT(KSemGetValue, "SemGetValue");
_LIT(KSemMicroSleepwaitNP, "SemMicroSleepwaitNP");
_LIT(KSemPostMultiple, "SemPostMultiple");
_LIT(KPThreadCondAttrInit, "PThreadCondAttrInit");
_LIT(KPThreadCondAttrDestroy, "PThreadCondAttrDestroy");
_LIT(KPThreadCondInit, "PThreadCondInit");
_LIT(KPThreadCondDestroy, "PThreadCondDestroy");
_LIT(KPThreadCondTimedwait, "PThreadCondTimedwait");
_LIT(KPThreadCondWait, "PThreadCondWait");
_LIT(KPThreadCondSignal, "PThreadCondSignal");
_LIT(KPThreadCondBroadcast, "PThreadCondBroadcast");
_LIT(KDelay, "Delay");
_LIT(KCreateThreadL, "CreateThreadL");
_LIT(KSetExitValueL, "SetExitValueL");
_LIT(KPThreadAttrStaticInitL, "PThreadAttrStaticInitL");
_LIT(KPThreadAttrStaticInitTimedLockL, "PThreadAttrStaticInitTimedLockL");
_LIT(KPThreadAttrStaticInitTryLockL, "PThreadAttrStaticInitTryLockL");
_LIT(KPThreadAttrStaticInitUnlockL, "PThreadAttrStaticInitUnlockL");
_LIT(KWaitForThreadCompleteL, "WaitForThreadCompleteL");


_LIT(KIntgTest1, "IntgTest1");
_LIT(KIntgTest2, "IntgTest2");
_LIT(KIntgTest3, "IntgTest3");
_LIT(KIntgTest4, "IntgTest4");
_LIT(KIntgTest5, "IntgTest5");
_LIT(KIntgTest6, "IntgTest6");
_LIT(KIntgTest7, "IntgTest7");
_LIT(KIntgTest8, "IntgTest8");
_LIT(KIntgTest9, "IntgTest9");
_LIT(KIntgTest10, "IntgTest10");
_LIT(KIntgTest11, "IntgTest11");
_LIT(KIntgTest12, "IntgTest12");
_LIT(KIntgTest13, "IntgTest13");
_LIT(KIntgTest14, "IntgTest14");
_LIT(KIntgTest15, "IntgTest15");
_LIT(KIntgTest16, "IntgTest16");
_LIT(KIntgTest17, "IntgTest17");
_LIT(KIntgTest18, "IntgTest18");
_LIT(KIntgTest19, "IntgTest19");
_LIT(KIntgTest20, "IntgTest20");
_LIT(KIntgTest21, "IntgTest21");
_LIT(KIntgTest22, "IntgTest22");
_LIT(KIntgTest23, "IntgTest23");
_LIT(KIntgTest24, "IntgTest24");
_LIT(KIntgTest25, "IntgTest25");
_LIT(KIntgTest26, "IntgTest26");
_LIT(KIntgTest27, "IntgTest27");
_LIT(KIntgTest28, "IntgTest28");
_LIT(KIntgTest29, "IntgTest29");
_LIT(KIntgTest30, "IntgTest30");
_LIT(KIntgTest31, "IntgTest31");
_LIT(KIntgTest32, "IntgTest32");
_LIT(KIntgTest33, "IntgTest33");
_LIT(KIntgTest34, "IntgTest34");
_LIT(KIntgTest35, "IntgTest35");
_LIT(KIntgTest36, "IntgTest36");
_LIT(KIntgTest37, "IntgTest37");
_LIT(KIntgTest38, "IntgTest38");
_LIT(KIntgTest39, "IntgTest39");
_LIT(KPThreadMutexTryLockWait1, "PThreadMutexTryLockWait1");
_LIT(KPThreadMutexTryLockWait2, "PThreadMutexTryLockWait2");

class CTestMutex : public CTestStep
	{
public:
	TInt iErroCode;
    // we are not considering the syn.
    TInt iNoofThreadToComplete;
        
	~CTestMutex(); 
	CTestMutex(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
protected:
	// Functions from base classes

    static void* ThreadEntryFunctionL(void* arg);
    static void* ThreadEntryFunction650L(void* arg);
    static void* ThreadEntryFunction338L(void* arg);
    static void* ThreadEntryFunction339L(void* arg);
    static void* ThreadEntryFunction332L(void* arg);
    static void* ThreadEntryFunction333L(void* arg);
    static void* ThreadEntryFunction334L(void* arg);
    static void* ThreadEntryFunction335L(void* arg);
    static void* ThreadEntryFunction362L(void* arg);
    static void* ThreadEntryFunction1027L(void* arg);
    static void* ThreadEntryFunction1037L(void* arg);
    static void* ThreadEntryFunction1040L(void* arg);
    static void* ThreadEntryFunction1062L(void* arg);
    static void* ThreadEntryFunction1063L(void* arg);
    static void* ThreadEntryFunction1070L(void* arg);
    static void* ThreadEntryFunctionRecursiveL(void* arg);
    static void* ThreadEntryFunctionErrorCheckL(void* arg);
    static void* ThreadEntryFunctionSemaphoreL(void* arg);
    static void* ThreadEntryFunctionSemaphoreMultiplePostEntry1L(void* arg);
    static void* ThreadEntryFunctionSemaphoreMultiplePostEntry2L(void* arg);
    static void* ThreadEntryFunctionSemaphoreMultiplePostEntry3L(void* arg);
    static void* ThreadEntryFunctionSemPostL(void* arg);

private:
	pthread_t iThreadId;
    pthread_mutex_t iMutex;
    sem_t iSemId;
    pthread_mutexattr_t iMutexAttr;
    CIdle* iIdleTimer;
    TInt iExitValueL;


		/**
        * Test methods are listed below. 
        */
        
         TInt WaitForThreadCompleteL(TInt val);
         TInt Delay(TInt val);
         TInt SetExitValueL(TInt val);
         TInt PThreadAttrStaticInitL(  );
         TInt PThreadAttrStaticInitTimedLockL(TInt val, TInt typeTimeLockCheck);
         TInt PThreadAttrStaticInitTryLockL(  );
         TInt PThreadAttrStaticInitUnlockL(TInt val,TInt errVal);
         TInt CreateThreadL(TInt threadType);
        void SendSignal();
        
        /**
        * Thread mutex initialize
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadMutexInit(TInt initVal, TInt errVal);
        
        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadOnce(  );
        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadMutexDestroy(TInt errVal  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadMutexLock(TInt errVal );
        
        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadMutexLockRecursive(  );
        
        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadMutexLockErrorCheck(  );
        

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadMutexTimedlock(TInt val,TInt typeTimeLockCheck);

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadMutexTrylock(TInt val);

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadMutexUnlock(TInt errVal);

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadMutexAttrInit(TInt errVal);

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadMutexAttrInitInvalidPointerL(TInt errVal  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadMutexAttrDestroy(TInt val);

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadMutexAttrGetpshared(TInt val);

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadMutexAttrSetpshared(TInt val, TInt errVal  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadMutexattrGettype(TInt errVal);

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadMutexattrSettype(TInt val, TInt errVal );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt SemInit(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt SemDestroy(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt SemTryWait(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt SemWait(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt SemTimedWait(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt SemPost(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt SemOpen(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt SemClose(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt SemUnlink(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt SemGetValue(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt SemMicroSleepwaitNP(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt SemPostMultiple(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadCondAttrInit(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadCondAttrDestroy(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadCondInit(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadCondDestroy(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadCondTimedwait(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadCondWait(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadCondSignal(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt PThreadCondBroadcast(  );

        /**
        * 
        * @since MRT 1.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
       
  
                            
        static void PThreadOnceCallBack (void);
        /**
        * Handles the callback functionality.
        * @since 3.0
        * @param aThis Pointer to this object
        * @return 0
        */
        static TInt HandleIdleCallbackL(TAny *aThis);
        
        /**
        * Handles timer callback functionality.
        * @since 3.0
        */
        void HandleIdleCommandL();
    
        /**
        * Start the idle timer for any delayed fucntionaliy.
        * @since 3.0
        */
        void StartDelayedServiceL();


		TInt IntgTest1();
		TInt IntgTest2();
		TInt IntgTest3();
		TInt IntgTest4();
		TInt IntgTest5();
		TInt IntgTest6();
		TInt IntgTest7();
		TInt IntgTest8();
		TInt IntgTest9();
		TInt IntgTest10();
		TInt IntgTest11();
		TInt IntgTest12();
		TInt IntgTest13();
		TInt IntgTest14();
		TInt IntgTest15();
		TInt IntgTest16();
		TInt IntgTest17();
		TInt IntgTest18();
		TInt IntgTest19();
		TInt IntgTest20();
		TInt IntgTest21();
		TInt IntgTest22();
		TInt IntgTest23();
		TInt IntgTest24();
		TInt IntgTest25();
		TInt IntgTest26();
		TInt IntgTest27();
		TInt IntgTest28();
		TInt IntgTest29();
		TInt IntgTest30();
		TInt IntgTest31();
		TInt IntgTest32();
		TInt IntgTest33();
		TInt IntgTest34();
		TInt IntgTest35();
		TInt IntgTest36();	
		TInt IntgTest37();
		TInt IntgTest38();	
		TInt IntgTest39();	
		TInt PThreadMutexTryLockWait1();
		TInt PThreadMutexTryLockWait2();
		
	};
#endif 

