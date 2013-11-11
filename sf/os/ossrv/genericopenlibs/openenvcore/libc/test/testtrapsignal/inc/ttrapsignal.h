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
* Name        : ttrapsignal.h
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

_LIT(KTestsigactionapi, "Testsigactionapi");

_LIT(KTestcustomsignalapi, "Testcustomsignalapi");
_LIT(KTestcustomsigsetapi1, "Testcustomsigsetapi1");
_LIT(KTestcustomsigsetapi2, "Testcustomsigsetapi2");
_LIT(KTestcustombsd_signalapi, "Testcustombsd_signalapi");
_LIT(KTestcustomsigactionapi, "Testcustomsigactionapi");

_LIT(KTestnegcustomsignalapi, "Testnegcustomsignalapi");
_LIT(KTestnegcustomsigsetapi, "Testnegcustomsigsetapi");
_LIT(KTestnegcustombsd_signalapi, "Testnegcustombsd_signalapi");
_LIT(KTestnegcustomsigactionapi, "Testnegcustomsigactionapi");

_LIT(KTestignoresignalapi, "Testignoresignalapi");
_LIT(KTestignoresigsetapi1, "Testignoresigsetapi1");
_LIT(KTestignoresigsetapi2, "Testignoresigsetapi2");
_LIT(KTestignorebsd_signalapi, "Testignorebsd_signalapi");
_LIT(KTestignoresigactionapi, "Testignoresigactionapi");
_LIT(KTestsigignoreapi, "Testsigignoreapi");

_LIT(KTestnegignoresignalapi, "Testnegignoresignalapi");
_LIT(KTestnegignoresigsetapi, "Testnegignoresigsetapi");
_LIT(KTestnegignorebsd_signalapi, "Testnegignorebsd_signalapi");
_LIT(KTestnegignoresigactionapi, "Testnegignoresigactionapi");
_LIT(KTestnegsigignoreapi, "Testnegsigignoreapi");

_LIT(KTestnegdefaultsignalapi, "Testnegdefaultsignalapi");
_LIT(KTestnegdefaultsigsetapi, "Testnegdefaultsigsetapi");
_LIT(KTestnegdefaultbsd_signalapi, "Testnegdefaultbsd_signalapi");
_LIT(KTestnegdefaultsigactionapi, "Testnegdefaultsigactionapi");

_LIT(KTestsigsethold1, "Testsigsethold1");

_LIT(KTestsigactionapi1, "Testsigactionapi1");
_LIT(KTestsigactionapi2, "Testsigactionapi2");
_LIT(KTestsigactionapi3, "Testsigactionapi3");
_LIT(KTestsigactionapi4, "Testsigactionapi4");
_LIT(KTestsigactionapi5, "Testsigactionapi5");
_LIT(KTestsigactionapi6, "Testsigactionapi6");

enum TMode
    {
    TREAD = 0x00,
    TWRITE
    };    
    
class CTestTrapsignal : public CTestStep
	{
public:
	~CTestTrapsignal(); 
	CTestTrapsignal(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
protected:  // Functions from base classes

	static void* ThreadEntryFunctionW(void* arg);
	static void* ThreadEntryFunctionR(void* arg);
	static void* ThreadEntryFunctionWrite(void* arg);
private:

    virtual TInt Testsigactionapi (  );
	
    virtual TInt Testcustomsignalapi (  );
    virtual TInt Testcustomsigsetapi1 (  );
    virtual TInt Testcustomsigsetapi2 (  );
    virtual TInt Testcustombsd_signalapi (  );
    virtual TInt Testcustomsigactionapi (  );
        
    virtual TInt Testnegcustomsignalapi (  );
    virtual TInt Testnegcustomsigsetapi (  );
    virtual TInt Testnegcustombsd_signalapi (  );
    virtual TInt Testnegcustomsigactionapi (  );
        
    virtual TInt Testignoresignalapi (  );
    virtual TInt Testignoresigsetapi1 (  );
    virtual TInt Testignoresigsetapi2 (  );
    virtual TInt Testignorebsd_signalapi (  );
    virtual TInt Testignoresigactionapi (  );
    virtual TInt Testsigignoreapi (  );
    
    virtual TInt Testnegignoresignalapi (  );
    virtual TInt Testnegignoresigsetapi (  );
    virtual TInt Testnegignorebsd_signalapi (  );
    virtual TInt Testnegignoresigactionapi (  );
    virtual TInt Testnegsigignoreapi (  );
            
    virtual TInt Testnegdefaultsignalapi (  );
    virtual TInt Testnegdefaultsigsetapi (  );
    virtual TInt Testnegdefaultbsd_signalapi (  );
    virtual TInt Testnegdefaultsigactionapi (  );

    virtual TInt Testsigsethold1 (  );
    
    virtual TInt Testsigactionapi1 (  );
    virtual TInt Testsigactionapi2 (  );
    virtual TInt Testsigactionapi3 (  );
    virtual TInt Testsigactionapi4 (  );
    virtual TInt Testsigactionapi5 (  );
    virtual TInt Testsigactionapi6 (  );
    
  private:    // Data
		TInt iParamCnt;
        TInt iBlocked;
        TInt iThreadId;

	};
#endif 
