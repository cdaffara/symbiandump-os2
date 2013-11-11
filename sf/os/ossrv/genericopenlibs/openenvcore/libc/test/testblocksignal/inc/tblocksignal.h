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
* Name        : tblocksignal.h
* 
*
*/



#ifndef __TESTBLOCKSIGNAL_H__
#define __TESTBLOCKSIGNAL_H__

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

_LIT(KTestempty1, "Testempty1");
_LIT(KTestempty2, "Testempty2");
_LIT(KTestaddset1, "Testaddset1");
_LIT(KTestaddset2, "Testaddset2");
_LIT(KTestaddset3, "Testaddset3");
_LIT(KTestaddset4, "Testaddset4");
_LIT(KTestaddset5, "Testaddset5");
_LIT(KTestaddndelset1, "Testaddndelset1");
_LIT(KTestaddndelset2, "Testaddndelset2");
_LIT(KTestaddndelset3, "Testaddndelset3");
_LIT(KTestaddndelset4, "Testaddndelset4");
_LIT(KTestsigprocmask1, "Testsigprocmask1");
_LIT(KTestsigprocmask2, "Testsigprocmask2");
_LIT(KTestsigprocmask3, "Testsigprocmask3");
_LIT(KTestsigprocmask4, "Testsigprocmask4");
_LIT(KTestsigprocmask5, "Testsigprocmask5");
_LIT(KTestsigprocmask6, "Testsigprocmask6");
_LIT(KTestsigprocmask7, "Testsigprocmask7");
_LIT(KTestsigprocmask8, "Testsigprocmask8");
_LIT(KTestsigfillset1, "Testsigfillset1");
_LIT(KTestsigfillset2, "Testsigfillset2");
_LIT(KTestsigandset1, "Testsigandset1");
_LIT(KTestsigandset2, "Testsigandset2");
_LIT(KTestsigandset3, "Testsigandset3");
_LIT(KTestsigorset1, "Testsigorset1");
_LIT(KTestsigorset2, "Testsigorset2");
_LIT(KTestsigorset3, "Testsigorset3");
_LIT(KTestandorset1, "Testandorset1");
_LIT(KTestandorset2, "Testandorset2");
_LIT(KTestandorset3, "Testandorset3");
_LIT(KTestsighold1, "Testsighold1");
_LIT(KTestsighold2, "Testsighold2");
_LIT(KTestsigrelse1, "Testsigrelse1");
_LIT(KTestsigrelse2, "Testsigrelse2");
_LIT(KTestholdnrelse1, "Testholdnrelse1");
_LIT(KTestsigpause1, "Testsigpause1");
_LIT(KTestnegsigpause1, "Testnegsigpause1");

enum TMode
    {
    TREAD = 0x00,
    TWRITE
    };    
    
class CTestBlocksignal : public CTestStep
	{
public:
	~CTestBlocksignal(); 
	CTestBlocksignal(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
protected:  // Functions from base classes

	static void* ThreadEntryFunctionW(void* arg);
	static void* ThreadEntryFunctionR(void* arg);
	static void* ThreadEntryFunctionWrite(void* arg);
private:

        /**
        * Fifo create with a pathname > FILENAMEMAX
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt Testempty1 (  );
        virtual TInt Testempty2 (  );
        virtual TInt Testaddset1 (  );
        virtual TInt Testaddset2 (  );
        virtual TInt Testaddset3 (  );
        virtual TInt Testaddset4 (  );
        virtual TInt Testaddset5 (  );
        virtual TInt Testaddndelset1 (  );
        virtual TInt Testaddndelset2 (  );
        virtual TInt Testaddndelset3 (  );
        virtual TInt Testaddndelset4 (  );
        virtual TInt Testsigprocmask1 (  );
        virtual TInt Testsigprocmask2 (  );
        virtual TInt Testsigprocmask3 (  );
        virtual TInt Testsigprocmask4 (  );
        virtual TInt Testsigprocmask5 (  );
        virtual TInt Testsigprocmask6 (  );
        virtual TInt Testsigprocmask7 (  ); 
        virtual TInt Testsigprocmask8 (  );   
        virtual TInt Testsigfillset1 (  );
        virtual TInt Testsigfillset2 (  );
        virtual TInt Testsigandset1 (  );
        virtual TInt Testsigandset2 (  );
        virtual TInt Testsigandset3 (  );
        virtual TInt Testsigorset1 (  );
        virtual TInt Testsigorset2 (  );  
        virtual TInt Testsigorset3 (  );      
        virtual TInt Testandorset1 (  );      
        virtual TInt Testandorset2 (  );      
        virtual TInt Testandorset3 (  ); 
        virtual TInt Testsighold1 (  );   
        virtual TInt Testsighold2 (  );     
        virtual TInt Testsigrelse1 (  ); 
        virtual TInt Testsigrelse2 (  );      
        virtual TInt Testholdnrelse1 (  );    
        virtual TInt Testsigpause1 (  );    
        virtual TInt Testnegsigpause1 (  ); 
        
  private:    // Data
		TInt iParamCnt;
        TInt iBlocked;
        TInt iThreadId;

	};
#endif 
