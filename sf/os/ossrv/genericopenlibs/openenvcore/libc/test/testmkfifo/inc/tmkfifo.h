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



#ifndef __TESTMKFIFO_H__
#define __TESTMKFIFO_H__

#include <test/testexecutestepbase.h>
#include <e32svr.h>
#include <f32file.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#include <e32std.h>
#include <e32base.h>
#include <stdio.h>
#include <strings.h>
#include <sys/param.h>
#include <sys/types.h>
#include <err.h>

_LIT(KIntgTest1, "IntgTest1");
_LIT(KIntgTest2, "IntgTest2");
_LIT(KIntgTest3, "IntgTest3");
_LIT(KIntgTest4, "IntgTest4");
_LIT(KIntgTest5, "IntgTest5");
_LIT(KIntgTest6, "IntgTest6");
_LIT(KIntgTest7, "IntgTest7");
_LIT(KFifoCreateNULL, "FifoCreateNULL");
_LIT(KFifoCreateInSystemDir, "FifoCreateInSystemDir");
_LIT(KFifoCreateNameTooLong, "FifoCreateNameTooLong");
_LIT(KFifoCreateNonExistantDir, "FifoCreateNonExistantDir");
_LIT(KTMkfifoReaderWriter, "TMkfifoReaderWriter");
_LIT(KTMkfifoReaderWriter1, "TMkfifoReaderWriter1");
_LIT(KFifoCreateNULL1, "FifoCreateNULL1");
_LIT(KFifoCreate_invalid, "FifoCreate_invalid");
enum TMode
    {
    TREAD = 0x00,
    TWRITE
    };    
    
class CTestMkfifo : public CTestStep
	{
public:
	~CTestMkfifo(); 
	CTestMkfifo(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
protected:  // Functions from base classes

	static void* ThreadEntryFunctionW(void* arg);
	static void* ThreadEntryFunctionR(void* arg);
	static void* ThreadEntryFunctionWrite(void* arg);
private:
	  /**
        * Fifo create
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt FifoCreate(  );
        
        /**
        * Fifo create with NULL pathname
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt FifoCreateNULL(  );
        
        /**
        * Fifo create in system directory
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt FifoCreateInSystemDir (  );
        
        /**
        * Fifo create with a pathname > FILENAMEMAX
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt FifoCreateNameTooLong (  );
        
        /**
        * Fifo create: pathname corresponds to a file and not a dir
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt FifoCreateInNonDir (  );
        
        /**
        * Fifo create: pathname contains a non existant dir
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt FifoCreateNonExistantDir (  );
        
        /**
        * Open Fifo
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt FifoOpen(  );        
        
        /**
        * Open Fifo in write non-blocking mode
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt FifoOpenWNonBlock(  );
        
        /**
        * Open Fifo in read non-blocking mode
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt FifoOpenRNonBlock(  );
        
        /**
        * Close Fifo
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt FifoClose(  );
        
        /**
        * Write to Fifo
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt FifoWrite(  );
        
        /**
        * Write and read from Fifo
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt FifoWriteRead(  );
        
        /**
        * Delete fifo
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt DeleteFifo(  );
        
        /**
        * Create thread
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CreateThreadL(  );
        
       
        /**
        * Open Fifo in the specified mode
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */                      
        virtual TInt OpenFifo(TMode mode);
        
        /**
        * Read from Fifo
        * @since MRT 2.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt FifoRead(  );
        
        TInt IntgTest1();
        TInt IntgTest2();
        TInt IntgTest3();
        TInt IntgTest4();
        TInt IntgTest5();
        TInt IntgTest6();
        TInt IntgTest7();
        TInt TMkfifoReaderWriter();
        TInt TMkfifoReaderWriter1();
        virtual TInt FifoCreateNULL1(  );
        virtual TInt FifoCreate_invalid(  );

  private:    // Data
		TInt iParamCnt;
        TInt iBlocked;
        TInt iThreadId;

	};
#endif 
