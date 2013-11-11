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
* Name        : tsharedmemory.h
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
#include <time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>

_LIT(KTestsharedmemory1, "Testsharedmemory1");
_LIT(KTestsharedmemory2, "Testsharedmemory2");
_LIT(KTestsharedmemory3, "Testsharedmemory3");
_LIT(KTestsharedmemory4, "Testsharedmemory4");
_LIT(KTestsharedmemory5, "Testsharedmemory5");
_LIT(KTestsharedmemory6, "Testsharedmemory6");
_LIT(KTestsharedmemory7, "Testsharedmemory7");
_LIT(KTestsharedmemory8, "Testsharedmemory8");
_LIT(KTestsharedmemory9, "Testsharedmemory9");
_LIT(KTestsharedmemory10, "Testsharedmemory10");
_LIT(KTestsharedmemory11, "Testsharedmemory11");
_LIT(KTestsharedmemory12, "Testsharedmemory12");
_LIT(KTestsharedmemory13, "Testsharedmemory13");
_LIT(KTestsharedmemory14, "Testsharedmemory14");
_LIT(KTestsharedmemory15, "Testsharedmemory15");
_LIT(KTestsharedmemory16, "Testsharedmemory16");
_LIT(KTestsharedmemory17, "Testsharedmemory17");
_LIT(KTestsharedmemory18, "Testsharedmemory18");
_LIT(KTestsharedmemory19, "Testsharedmemory19");
_LIT(KTestsharedmemory20, "Testsharedmemory20");
_LIT(KTestsharedmemory21, "Testsharedmemory21");
_LIT(KTestsharedmemory22, "Testsharedmemory22");
_LIT(KTestsharedmemory23, "Testsharedmemory23");
_LIT(KTestsharedmemory24, "Testsharedmemory24");
_LIT(KTestsharedmemory25, "Testsharedmemory25");
_LIT(KTestsharedmemory26, "Testsharedmemory26");
_LIT(KTestsharedmemory27, "Testsharedmemory27");
_LIT(KTestsharedmemory28, "Testsharedmemory28");
_LIT(KTestsharedmemory29, "Testsharedmemory29");
_LIT(KTestsharedmemory30, "Testsharedmemory30");

enum TMode
    {
    TREAD = 0x00,
    TWRITE
    };    
    
class CTestsharedmemory : public CTestStep
	{
public:
	~CTestsharedmemory(); 
	CTestsharedmemory(const TDesC& aStepName);
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
        virtual TInt Testsharedmemory1 (  );
        virtual TInt Testsharedmemory2 (  );
        virtual TInt Testsharedmemory3 (  );
        virtual TInt Testsharedmemory4 (  ); 
        virtual TInt Testsharedmemory5 (  );
        virtual TInt Testsharedmemory6 (  );
        virtual TInt Testsharedmemory7 (  ); 
        virtual TInt Testsharedmemory8 (  );
        virtual TInt Testsharedmemory9 (  );
        virtual TInt Testsharedmemory10 (  );  
        virtual TInt Testsharedmemory11 (  );  
        virtual TInt Testsharedmemory12 (  );  
        virtual TInt Testsharedmemory13 (  );   
        virtual TInt Testsharedmemory14 (  );  
        virtual TInt Testsharedmemory15 (  );   
        virtual TInt Testsharedmemory16 (  );   
        virtual TInt Testsharedmemory17 (  );  
        virtual TInt Testsharedmemory18 (  );   
        virtual TInt Testsharedmemory19 (  );   
        virtual TInt Testsharedmemory20 (  );   
        virtual TInt Testsharedmemory21 (  );   
        virtual TInt Testsharedmemory22 (  );   
        virtual TInt Testsharedmemory23 (  );   
        virtual TInt Testsharedmemory24 (  );   
        virtual TInt Testsharedmemory25 (  );    
        virtual TInt Testsharedmemory26 (  );     
        virtual TInt Testsharedmemory27 (  ); 
        virtual TInt Testsharedmemory28 (  ); 
        virtual TInt Testsharedmemory29 (  ); 
        virtual TInt Testsharedmemory30 (  ); 
        
  private:    // Data
		TInt iParamCnt;
        TInt iBlocked;
        TInt iThreadId;

	};
#endif 
