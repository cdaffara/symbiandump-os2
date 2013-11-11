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
* Name        : ttimer.h
* 
*
*/



#ifndef __TESTTIMER_H__
#define __TESTTIMER_H__

#include <test/testexecutestepbase.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <spawn.h>
#include <time.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>


_LIT(KTesttimerapi1, "Testtimerapi1");
_LIT(KTesttimerapi2, "Testtimerapi2");
_LIT(KTesttimerapi3, "Testtimerapi3");
_LIT(KTesttimerapi4, "Testtimerapi4");
_LIT(KTesttimerapi5, "Testtimerapi5");
_LIT(KTesttimerapi6, "Testtimerapi6");
_LIT(KTesttimerapi7, "Testtimerapi7");
_LIT(KTesttimerapi8, "Testtimerapi8");
_LIT(KTesttimerapi9, "Testtimerapi9");
_LIT(KTesttimerapi10, "Testtimerapi10");
_LIT(KTesttimerapi11, "Testtimerapi11");
_LIT(KTesttimerapi12, "Testtimerapi12");
_LIT(KTesttimerapi13, "Testtimerapi13");
_LIT(KTesttimerapi14, "Testtimerapi14");
_LIT(KTesttimerapi15, "Testtimerapi15");
_LIT(KTesttimerapi16, "Testtimerapi16");
_LIT(KTesttimerapi17, "Testtimerapi17");
_LIT(KTesttimerapi18, "Testtimerapi18");
_LIT(KTesttimerapi19, "Testtimerapi19");
_LIT(KTesttimerapi20, "Testtimerapi20");
_LIT(KTesttimerapi21, "Testtimerapi21");
_LIT(KTesttimerapi22, "Testtimerapi22");
_LIT(KTesttimerapi23, "Testtimerapi23");
_LIT(KTesttimerapi24, "Testtimerapi24");
_LIT(KTesttimerapi25, "Testtimerapi25");
_LIT(KTesttimerapi26, "Testtimerapi26");
_LIT(KTesttimerapi27, "Testtimerapi27");
_LIT(KTesttimerapi28, "Testtimerapi28");

enum TMode
    {
    TREAD = 0x00,
    TWRITE
    };    
    
class CTesttimer : public CTestStep
	{
public:
	~CTesttimer(); 
	CTesttimer(const TDesC& aStepName);
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
        virtual TInt Testtimerapi1 (  );
        virtual TInt Testtimerapi2 (  );
        virtual TInt Testtimerapi3 (  );
        virtual TInt Testtimerapi4 (  ); 
        virtual TInt Testtimerapi5 (  );
        virtual TInt Testtimerapi6 (  );
        virtual TInt Testtimerapi7 (  );  
        virtual TInt Testtimerapi8 (  );
        virtual TInt Testtimerapi9 (  );
        virtual TInt Testtimerapi10 (  );
        virtual TInt Testtimerapi11 (  );
        virtual TInt Testtimerapi12 (  );
        virtual TInt Testtimerapi13 (  );
        virtual TInt Testtimerapi14 (  );   
        virtual TInt Testtimerapi15 (  );
        virtual TInt Testtimerapi16 (  );
        virtual TInt Testtimerapi17 (  );
        virtual TInt Testtimerapi18 (  );
        virtual TInt Testtimerapi19 (  ); 
        virtual TInt Testtimerapi20 (  ); 
        virtual TInt Testtimerapi21 (  );   
        virtual TInt Testtimerapi22 (  ); 
        virtual TInt Testtimerapi23 (  );   
        virtual TInt Testtimerapi24 (  );   
        virtual TInt Testtimerapi25 (  );   
        virtual TInt Testtimerapi26 (  );   
        virtual TInt Testtimerapi27 (  );   
        virtual TInt Testtimerapi28 (  ); 
        
  private:    // Data
		TInt iParamCnt;
        TInt iBlocked;
        TInt iThreadId;

	};
#endif 
