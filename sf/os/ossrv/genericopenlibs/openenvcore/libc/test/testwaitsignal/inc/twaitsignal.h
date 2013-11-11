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
* Name        : twaitsignal.h
* 
*
*/



#ifndef __TESTWAISIGNAL_H__
#define __TESTWAISIGNAL_H__

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


_LIT(KTestpendingsignal1, "Testpendingsignal1");
_LIT(KTestpendingsignal2, "Testpendingsignal2");
_LIT(KTestpendingsignal3, "Testpendingsignal3");
_LIT(KTestpendingsignal4, "Testpendingsignal4");
_LIT(KTestwaitsignal1, "Testwaitsignal1");
_LIT(KTestwaitsignal2, "Testwaitsignal2");
_LIT(KTestwaitsignal3, "Testwaitsignal3");
_LIT(KTestwaitsignal4, "Testwaitsignal4");
_LIT(KTestwaitsignal5, "Testwaitsignal5");
_LIT(KTestwaitsignal6, "Testwaitsignal6");
_LIT(KTestwaitsignal7, "Testwaitsignal7");
_LIT(KTestwaitsignal8, "Testwaitsignal8");
_LIT(KTestwaitsignal9, "Testwaitsignal9");
_LIT(KTestwaitinfosignal1, "Testwaitinfosignal1");
_LIT(KTestwaitinfosignal2, "Testwaitinfosignal2");
_LIT(KTestwaitinfosignal3, "Testwaitinfosignal3");
_LIT(KTestwaitinfosignal4, "Testwaitinfosignal4");
_LIT(KTestwaitinfosignal5, "Testwaitinfosignal5");
_LIT(KTestwaitinfosignal6, "Testwaitinfosignal6");
_LIT(KTestwaitinfosignal7, "Testwaitinfosignal7");
_LIT(KTestwaitinfosignal8, "Testwaitinfosignal8");
_LIT(KTestwaitinfosignal9, "Testwaitinfosignal9");
_LIT(KTesttimedwaitsignal1, "Testtimedwaitsignal1");
_LIT(KTesttimedwaitsignal2, "Testtimedwaitsignal2");
_LIT(KTesttimedwaitsignal3, "Testtimedwaitsignal3");
_LIT(KTesttimedwaitsignal4, "Testtimedwaitsignal4");
_LIT(KTesttimedwaitsignal5, "Testtimedwaitsignal5");
_LIT(KTesttimedwaitsignal6, "Testtimedwaitsignal6");
_LIT(KTesttimedwaitsignal7, "Testtimedwaitsignal7");
_LIT(KTesttimedwaitsignal8, "Testtimedwaitsignal8");
_LIT(KTesttimedwaitsignal9, "Testtimedwaitsignal9");
_LIT(KTesttimedwaitsignal10, "Testtimedwaitsignal10");
_LIT(KTesttimedwaitsignal11, "Testtimedwaitsignal11");
_LIT(KTesttimedwaitsignal12, "Testtimedwaitsignal12");
_LIT(KTesttimedwaitsignal13, "Testtimedwaitsignal13");
_LIT(KTesttimedwaitsignal14, "Testtimedwaitsignal14");

enum TMode
    {
    TREAD = 0x00,
    TWRITE
    };    
    
class CTestWaitsignal : public CTestStep
	{
public:
	~CTestWaitsignal(); 
	CTestWaitsignal(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
protected:  // Functions from base classes

	static void* ThreadEntryFunctionW(void* arg);
	static void* ThreadEntryFunctionR(void* arg);
	static void* ThreadEntryFunctionWrite(void* arg);
private:
		
	    virtual TInt Testpendingsignal1 (  );
	    virtual TInt Testpendingsignal2 (  );
	    virtual TInt Testpendingsignal3 (  );
	    virtual TInt Testpendingsignal4 (  );
        virtual TInt Testwaitsignal1 (  );
        virtual TInt Testwaitsignal2 (  );
        virtual TInt Testwaitsignal3 (  );
        virtual TInt Testwaitsignal4 (  );
        virtual TInt Testwaitsignal5 (  ); 
        virtual TInt Testwaitsignal6 (  );  
        virtual TInt Testwaitsignal7 (  );  
        virtual TInt Testwaitsignal8 (  );   
        virtual TInt Testwaitsignal9 (  );   
        virtual TInt Testwaitinfosignal1 (  );
        virtual TInt Testwaitinfosignal2 (  );
        virtual TInt Testwaitinfosignal3 (  );
        virtual TInt Testwaitinfosignal4 (  );
        virtual TInt Testwaitinfosignal5 (  );
        virtual TInt Testwaitinfosignal6 (  );
        virtual TInt Testwaitinfosignal7 (  );
        virtual TInt Testwaitinfosignal8 (  );
        virtual TInt Testwaitinfosignal9 (  );
        virtual TInt Testtimedwaitsignal1 (  );
        virtual TInt Testtimedwaitsignal2 (  );
        virtual TInt Testtimedwaitsignal3 (  );
        virtual TInt Testtimedwaitsignal4 (  );
        virtual TInt Testtimedwaitsignal5 (  );
        virtual TInt Testtimedwaitsignal6 (  );
        virtual TInt Testtimedwaitsignal7 (  );
        virtual TInt Testtimedwaitsignal8 (  );
        virtual TInt Testtimedwaitsignal9 (  );
        virtual TInt Testtimedwaitsignal10 (  );
        virtual TInt Testtimedwaitsignal11 (  );
        virtual TInt Testtimedwaitsignal12 (  );
        virtual TInt Testtimedwaitsignal13 (  );
        virtual TInt Testtimedwaitsignal14 (  );
        
  private:    // Data
		TInt iParamCnt;
        TInt iBlocked;
        TInt iThreadId;

	};
#endif 
