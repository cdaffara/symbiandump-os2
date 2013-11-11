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
* Name        : tclock.h
* 
*
*/



#ifndef __TESTCLOCK_H__
#define __TESTCLOCK_H__

#include <test/testexecutestepbase.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <spawn.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>

_LIT(KTestgetclockid1, "Testgetclockid1");
_LIT(KTestgetclockid2, "Testgetclockid2");
_LIT(KTestgetclockid3, "Testgetclockid3");
_LIT(KTestgetclockid4, "Testgetclockid4");
_LIT(KTestgetclockid5, "Testgetclockid5");
_LIT(KTestclockresolution1, "Testclockresolution1");
_LIT(KTestclockresolution2, "Testclockresolution2");
_LIT(KTestclockresolution3, "Testclockresolution3");
_LIT(KTestclocknanosleep1, "Testclocknanosleep1");
_LIT(KTestclocknanosleep2, "Testclocknanosleep2");
_LIT(KTestclocknanosleep3, "Testclocknanosleep3");
_LIT(KTestclocknanosleep4, "Testclocknanosleep4");
_LIT(KTestclocknanosleep5, "Testclocknanosleep5");
_LIT(KTestclocknanosleep6, "Testclocknanosleep6");
_LIT(KTestclockgettime1, "Testclockgettime1");
_LIT(KTestclockgettime2, "Testclockgettime2");
_LIT(KTestclockgettime3, "Testclockgettime3");
_LIT(KTestclocksettime1, "Testclocksettime1");
_LIT(KTestclocksettime2, "Testclocksettime2");
_LIT(KTestclocksettime3, "Testclocksettime3");

enum TMode
    {
    TREAD = 0x00,
    TWRITE
    };    
    
class CTestclock : public CTestStep
	{
public:
	~CTestclock(); 
	CTestclock(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
protected:  // Functions from base classes

	static void* ThreadEntryFunctionW(void* arg);
	static void* ThreadEntryFunctionR(void* arg);
	static void* ThreadEntryFunctionWrite(void* arg);
private:

        virtual TInt Testgetclockid1 (  );
        virtual TInt Testgetclockid2 (  );
        virtual TInt Testgetclockid3 (  );
        virtual TInt Testgetclockid4 (  ); 
        virtual TInt Testgetclockid5 (  );
        virtual TInt Testclockresolution1 (  );  
        virtual TInt Testclockresolution2 (  );
        virtual TInt Testclockresolution3 (  );
        virtual TInt Testclocknanosleep1 (  );
        virtual TInt Testclocknanosleep2 (  );
        virtual TInt Testclocknanosleep3 (  );
        virtual TInt Testclocknanosleep4 (  );
        virtual TInt Testclocknanosleep5 (  );
        virtual TInt Testclocknanosleep6 (  );
        virtual TInt Testclockgettime1 (  );
        virtual TInt Testclockgettime2 (  );
        virtual TInt Testclockgettime3 (  );  
        virtual TInt Testclocksettime1 (  );  
        virtual TInt Testclocksettime2 (  );  
        virtual TInt Testclocksettime3 (  );  
        
  private:    // Data
		TInt iParamCnt;
        TInt iBlocked;
        TInt iThreadId;

	};
#endif 
