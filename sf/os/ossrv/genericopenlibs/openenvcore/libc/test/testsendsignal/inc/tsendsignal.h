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
* Name        : tsendsignal.h
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

_LIT(KTestRaise, "TestRaise");
_LIT(KTestRaise1, "TestRaise1");
_LIT(KTestNegativeRaise, "TestNegativeRaise");
_LIT(KTestKill1, "TestKill1");
_LIT(KTestKill2, "TestKill2");
_LIT(KTestNegativeKill1, "TestNegativeKill1");
_LIT(KTestNegativeKill2, "TestNegativeKill2");
_LIT(KTestNegativeKill3, "TestNegativeKill3");
_LIT(KTestNegativeKill4, "TestNegativeKill4");
_LIT(KTestSigqueue, "TestSigqueue");
_LIT(KTestNegativeSigqueue1, "TestNegativeSigqueue1");
_LIT(KTestNegativeSigqueue2, "TestNegativeSigqueue2");
_LIT(KTestNegativeSigqueue3, "TestNegativeSigqueue3");
_LIT(KTestNegativeSigqueue4, "TestNegativeSigqueue4");
_LIT(KTestSigKillwithTmpfile, "TestSigKillwithTmpfile");

enum TMode
    {
    TREAD = 0x00,
    TWRITE
    };    
    
class CTestSendsignal : public CTestStep
	{
public:
	~CTestSendsignal(); 
	CTestSendsignal(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
protected:  // Functions from base classes

	static void* ThreadEntryFunctionW(void* arg);
	static void* ThreadEntryFunctionR(void* arg);
	static void* ThreadEntryFunctionWrite(void* arg);
private:
	
        virtual TInt TestRaise (  );
        virtual TInt TestRaise1 (  );
        virtual TInt TestNegativeRaise (  );
        virtual TInt TestKill1 (  );
        virtual TInt TestKill2 (  );
        virtual TInt TestNegativeKill1 (  );
        virtual TInt TestNegativeKill2 (  );
        virtual TInt TestNegativeKill3 (  );
        virtual TInt TestNegativeKill4 (  );
        virtual TInt TestSigqueue (  );
        virtual TInt TestNegativeSigqueue1 (  );
        virtual TInt TestNegativeSigqueue2 (  );
        virtual TInt TestNegativeSigqueue3 (  );
        virtual TInt TestNegativeSigqueue4 (  );
        virtual TInt TestSigKillwithTmpfile (  );
        
  private:    // Data
		TInt iParamCnt;
        TInt iBlocked;
        TInt iThreadId;

	};
#endif 
