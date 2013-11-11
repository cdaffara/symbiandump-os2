/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : tmiscsignal.h
* 
*
*/



#ifndef __TESTMKFIFO_H__
#define __TESTMKFIFO_H__

#include <test/testexecutestepbase.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <spawn.h>
#include <sys/wait.h>
#include <sys/timespec.h>

_LIT(KTestpsignal1, "Testpsignal1");
_LIT(KTestpsignal2, "Testpsignal2");
_LIT(KTestcombinesignal1, "Testcombinesignal1");
_LIT(KTestcombinesignal2, "Testcombinesignal2");
_LIT(KTestcombinesignal3, "Testcombinesignal3");
_LIT(KTestsigchildsignal, "Testsigchildsignal");
_LIT(KTestsigpipesignal, "Testsigpipesignal");
_LIT(KTestinheritsignal, "Testinheritsignal");
_LIT(KTestsigalrmsignal1, "Testsigalrmsignal1");
_LIT(KTestsigalrmsignal2, "Testsigalrmsignal2");
_LIT(KTestsigalrmsignal3, "Testsigalrmsignal3");
_LIT(KTestsigalrmsignal4, "Testsigalrmsignal4");
_LIT(KTestsigalrmsignal5, "Testsigalrmsignal5");

enum TMode
    {
    TREAD = 0x00,
    TWRITE
    };    
    
class CTestMiscsignal : public CTestStep
	{
public:
	~CTestMiscsignal(); 
	CTestMiscsignal(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
protected:  // Functions from base classes

	static void* ThreadEntryFunctionW(void* arg);
	static void* ThreadEntryFunctionR(void* arg);
	static void* ThreadEntryFunctionWrite(void* arg);
private:

        virtual TInt Testpsignal1 (  );
        virtual TInt Testpsignal2 (  );
        virtual TInt Testcombinesignal1 (  );
        virtual TInt Testcombinesignal2 (  );
        virtual TInt Testcombinesignal3 (  );
        virtual TInt Testsigchildsignal (  );
        virtual TInt Testsigpipesignal (  );
        virtual TInt Testinheritsignal (  );
        virtual TInt Testsigalrmsignal1 (  );
        virtual TInt Testsigalrmsignal2(  );
        virtual TInt Testsigalrmsignal3 (  );
        virtual TInt Testsigalrmsignal4(  );
        virtual TInt Testsigalrmsignal5(  );
        
  private:    // Data
		TInt iParamCnt;
        TInt iBlocked;
        TInt iThreadId;

	};
#endif 
