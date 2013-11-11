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


#ifndef __TESTPIPE_H__
#define __TESTPIPE_H__

#include <test/testexecutestepbase.h>
//#include <TestScripterInternal.h>


enum TErrorCode
    {
    IN = 0x01,
    OUT, 
    ERROR
    };

											

_LIT(KTestPipe, "TestPipe");
_LIT(KPipeCheckWriteOnReadfd, "PipeCheckWriteOnReadfd");
_LIT(KPipeCheckReadOnWritefd, "PipeCheckReadOnWritefd");
_LIT(KPipeWriteRead, "PipeWriteRead");
_LIT(KPipeCheckWriteFd, "PipeCheckWriteFd");
_LIT(KPipeCheckReadFd, "PipeCheckReadFd");

/*popen tests names*/
_LIT(KPopenPipeCommandRead, "PopenPipeCommandRead");
_LIT(KMultiplePopenPipeCommandRead, "MultiplePopenPipeCommandRead");
_LIT(KPopenPipeOEExeRead, "PopenPipeOEExeRead");
_LIT(KPopenPipeInvalidPathRead, "PopenPipeInvalidPathRead");
_LIT(KPopenPipeInvalidCommand, "PopenPipeInvalidCommand");
_LIT(KPopenPipeInvalidMode, "PopenPipeInvalidMode");
_LIT(KPopenPipeWrite, "PopenPipeWrite");
_LIT(KPopenBlockingRead, "PopenBlockingRead");

/*popen3 tests names*/
_LIT(KPopen3PipeCommandRead, "Popen3PipeCommandRead");
_LIT(KMultiplePopen3PipeCommandRead, "MultiplePopen3PipeCommandRead");
_LIT(KPopen3PipeOEExeRead, "Popen3PipeOEExeRead");
_LIT(KPopen3PipeInvalidPathRead, "Popen3PipeInvalidPathRead");
_LIT(KPopen3PipeInvalidCommand, "Popen3PipeInvalidCommand");
_LIT(KTestProcessPopen3ChitChat, "TestProcessPopen3ChitChat");
_LIT(KPopen3ReadWriteTest, "Popen3ReadWriteTest");
_LIT(KTestSystem, "TestSystem");
/*Negative tests*/
_LIT(KLseekpipetest, "Lseekpipetest");
_LIT(KOpenMaxfdPipetest, "OpenMaxfdPipetest");
_LIT(KSimultaneousOpenfdPipetest, "SimultaneousOpenfdPipetest");
_LIT(KFopenMaxPopenTest, "FopenMaxPopenTest");
_LIT(KFopenMaxPopen3Test, "FopenMaxPopen3Test");
_LIT(KTestEnvPopen3, "TestEnvPopen3");

//*DEF111452*//
_LIT(KTestsystem_LongName, "Testsystem_LongName");
_LIT(KTestwsystem_LongName, "Testwsystem_LongName");
_LIT(KTestpopen_LongName, "Testpopen_LongName");
_LIT(KTestwpopen_LongName, "Testwpopen_LongName");
_LIT(KTestpopen3_LongName, "Testpopen3_LongName");
_LIT(KTestwpopen3_LongName, "Testwpopen3_LongName");

/*DEF1222259*/

_LIT(KTestPipeWaitForData, "TestPipeWaitForData");

typedef struct data
{	
	int fds[2];
	int write_size;
	int read_size;
}DATA;

class CTestPipe : public CTestStep
	{
public:
	~CTestPipe(); 
	CTestPipe(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL(); 
	TVerdict doTestStepPostambleL();
private:
	
    TInt TestPipe();
    
    /**
    * Pipe Write on Read file desc
    * @since MRT 2.0
    * @param aItem Script line containing parameters.
    * @return Symbian OS error code.
    */
    virtual TInt PipeCheckWriteOnReadfd( );
    
    /**
    * Pipe Read on Write file desc
    * @since MRT 2.0
    * @param aItem Script line containing parameters.
    * @return Symbian OS error code.
    */
    virtual TInt PipeCheckReadOnWritefd( );
    
    /**
    * Pipe Write and Read
    * @since MRT 2.0
    * @param aItem Script line containing parameters.
    * @return Symbian OS error code.
    */
    virtual TInt PipeWriteRead( );
    
    /**
    * Pipe Check Write desc
    * @since MRT 2.0
    * @param aItem Script line containing parameters.
    * @return Symbian OS error code.
    */
    virtual TInt PipeCheckWriteFd( );
    
    /**
    * Pipe Check Read desc
    * @since MRT 2.0
    * @param aItem Script line containing parameters.
    * @return Symbian OS error code.
    */
    virtual TInt PipeCheckReadFd( );
    
    //	void* ReadData(void *data); 
	
    /*popen() tests*/                    
	virtual TInt PopenPipeCommandRead();
	virtual TInt MultiplePopenPipeCommandRead();	
	virtual TInt PopenPipeOEExeRead();
	virtual TInt PopenPipeInvalidPathRead();
	virtual TInt PopenPipeInvalidCommand();
	virtual TInt PopenPipeInvalidMode();	
	virtual TInt PopenPipeWrite();

	virtual TInt PopenBlockingRead();
//	virtual TInt PopenNonBlockingReadWithNoData();
	
	/*popen3() tests*/
	virtual TInt Popen3PipeCommandRead();
    virtual TInt MultiplePopen3PipeCommandRead();	
	virtual TInt Popen3PipeOEExeRead();
	virtual TInt Popen3PipeInvalidPathRead();
	virtual TInt Popen3PipeInvalidCommand();
    virtual TInt TestProcessPopen3ChitChat();
    virtual TInt Popen3ReadWriteTest();
	virtual TInt OpenMaxfdPipetest();
	/*system() test*/
    virtual TInt TestSystem();	
    
    /*Negative tests*/
    virtual TInt Lseekpipetest(); 
    virtual TInt SimultaneousOpenfdPipetest();
    virtual TInt FopenMaxPopenTest();
    virtual TInt FopenMaxPopen3Test();
    virtual TInt TestEnvPopen3();
    
    //*DEF111452*//
    virtual TInt Testwsystem_LongName();
    virtual TInt Testsystem_LongName();
	virtual TInt Testpopen_LongName();
	virtual TInt Testwpopen_LongName();
	virtual TInt Testpopen3_LongName();
	virtual TInt Testwpopen3_LongName();
	
	/*122259*/
	virtual TInt TestPipeWaitForData();
	};

#endif 
