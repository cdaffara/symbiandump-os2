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



#ifndef __TESTSEMAPHORE_H__
#define __TESTSEMAPHORE_H__

#include <test/testexecutestepbase.h>
#include <e32svr.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h> 

#include <fcntl.h>
#include <stdio.h>

// MACROS
#define MAX_IPC_OBJ 257


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
_LIT(KSemCreate, "SemCreate");

_LIT(KSemgetCreateKey, "SemgetCreateKey");
_LIT(KSemgetCreatePrivate, "SemgetCreatePrivate");
_LIT(KSemgetGetPrivate, "SemgetGetPrivate");
_LIT(KSemgetCreateKeyMax, "SemgetCreateKeyMax");
_LIT(KSemgetCreatePvtMax, "SemgetCreatePvtMax");
_LIT(KSemgetGetNonExisting, "SemgetGetNonExisting");
_LIT(KSemgetCreateInvalidSizeK, "SemgetCreateInvalidSizeK");
_LIT(KSemgetCreateInvalidSizeP, "SemgetCreateInvalidSizeP");
_LIT(KSemgetGetMore, "SemgetGetMore");
_LIT(KSemgetCreateExisting, "SemgetCreateExisting");
_LIT(KSemgetGetExisting, "SemgetGetExisting");
_LIT(KSemctlInvalidId, "SemctlInvalidId");
_LIT(KSemctlInvalidArgsK, "SemctlInvalidArgsK");
_LIT(KSemctlInvalidArgsP, "SemctlInvalidArgsP");
_LIT(KSemctlStatK, "SemctlStatK");
_LIT(KSemctlStatP, "SemctlStatP");
_LIT(KSemctlSetK, "SemctlSetK");
_LIT(KSemctlSetP, "SemctlSetP");
_LIT(KSemctlGetValK, "SemctlGetValK");
_LIT(KSemctlGetValP, "SemctlGetValP");

_LIT(Ksemctltest8, "semctltest8");
_LIT(Ksemctltest9, "semctltest9");
_LIT(Ksemctltest10, "semctltest10");
_LIT(Ksemctltest11, "semctltest11");
_LIT(Ksemctltest12, "semctltest12");
_LIT(Ksemctltest13, "semctltest13");
_LIT(Ksemctltest14, "semctltest14");
_LIT(Ksemoptest1, "semoptest1");
_LIT(Ksemoptest2, "semoptest2");
_LIT(Ksemoptest3, "semoptest3");
_LIT(Ksemoptest4, "semoptest4");
_LIT(Ksemoptest5, "semoptest5");
_LIT(Ksemoptest6, "semoptest6");
_LIT(Ksemoptest7, "semoptest7");
_LIT(Ksemoptest8, "semoptest8");
_LIT(Ksemoptest9, "semoptest9");
_LIT(Ksemoptest10, "semoptest10");
_LIT(Ksemoptest11, "semoptest11");
_LIT(Ksemoptest12, "semoptest12");
_LIT(Ksemoptest13, "semoptest13");
_LIT(Ksemoptest14, "semoptest14");
_LIT(Ksemoptest15, "semoptest15");

class CTestSemaphore : public CTestStep
	{
public:
	virtual ~CTestSemaphore(); 
	CTestSemaphore(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	void SendSignal();
private:
		/**
        * Test methods are listed below. 
        */
        
        /**
        * Message queue create
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt SemCreate(  );
        
        /**
        * Message queue close
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt SemClose(  );
        
        TInt SemKey(  TInt& aKey );
        
        TInt SemNum(  TInt& aSemNum );
        
        TInt SemFlag(  TInt& aFlag );
        
        TInt ErrCode(  TInt& aErrCode );
        
		TInt ReadIntParam( TInt& aParam );
		
        TInt SemNCreate(  );
        
        TInt SemOp(  );
        
        TInt SemOpVal(  TInt& aSemOp );
        
        TInt SemNClose(  );
        
        TInt SemCtl(  );
        
        TInt ReadSemId(  );
        
        TInt CreateThreadL(  );
        
        TInt WaitForThreadCompleteL( );
    public:     // Data
        TInt iKey;
        TInt iSemNum;
        TInt iExitVal;
        TBool iSemopStartFlag;
    protected:  // Data
        static void* ThreadEntryFunctionSemOpGetNcnt(void* arg);
        static void* ThreadEntryFunctionSemOpGetZcnt(void* arg);
        static void* ThreadEntryFunctionSemOpNegativeVal(void* arg);
        static void* ThreadEntryFunctionSemOpZeroVal(void* arg);
        
    private:    // Data
        TInt *iNumberOfCreateSem;
        TInt iReadSemIdFlag;
        TInt iThreadId;
        TInt iParamCnt;
        
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
		
		TInt SemgetCreateKey();
		TInt SemgetCreatePrivate();
		TInt SemgetGetPrivate();
		TInt SemgetCreateKeyMax();
		TInt SemgetCreatePvtMax();
		TInt SemgetGetNonExisting();
		TInt SemgetCreateInvalidSizeK();
		TInt SemgetCreateInvalidSizeP();
		TInt SemgetGetMore();
		TInt SemgetCreateExisting();
		TInt SemgetGetExisting();
		
		TInt SemctlInvalidId();
		TInt SemctlInvalidArgsK();
		TInt SemctlInvalidArgsP();
		TInt SemctlStatK();
		TInt SemctlStatP();
		TInt SemctlSetK();
		TInt SemctlSetP();
		TInt SemctlGetValK();
		TInt SemctlGetValP();
		
		TInt semctltest8();
		TInt semctltest9();	
		TInt semctltest10();
		TInt semctltest11();
		TInt semctltest12();
		TInt semctltest13();	
		TInt semctltest14();
		
		TInt semoptest1();
		TInt semoptest2();
		TInt semoptest3();
		TInt semoptest4();
		TInt semoptest5();
		TInt semoptest6();
		TInt semoptest7();
		TInt semoptest8();
		TInt semoptest9();
		TInt semoptest10();
		TInt semoptest11();
		TInt semoptest12();
		TInt semoptest13();
		TInt semoptest14();
		TInt semoptest15();
				
	};
#endif 
