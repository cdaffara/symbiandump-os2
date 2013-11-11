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


#include "testpipe.h"

CTestPipe::~CTestPipe() 
	{ 
	}  

CTestPipe::CTestPipe(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestPipe::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EFail);
	return TestStepResult();
	}

TVerdict CTestPipe::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}

TVerdict CTestPipe::doTestStepL()
	{
	int err;
		
	if(TestStepName() == KTestPipe)
		{
   		INFO_PRINTF1(_L("TestPipe():"));
   		err = TestPipe();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KPipeCheckWriteOnReadfd)
   		{
   		INFO_PRINTF1(_L("PipeCheckWriteOnReadfd():"));
   		err = PipeCheckWriteOnReadfd();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KPipeCheckReadOnWritefd)
   		{
   		INFO_PRINTF1(_L("PipeCheckReadOnWritefd():"));
   		err = PipeCheckReadOnWritefd();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KPipeWriteRead)
   		{
   		INFO_PRINTF1(_L("PipeWriteRead():"));
   		err = PipeWriteRead();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KPipeCheckWriteFd)
   		{
   		INFO_PRINTF1(_L("PipeCheckWriteFd():"));
   		err = PipeCheckWriteFd();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KPipeCheckReadFd)
   		{
   		INFO_PRINTF1(_L("PipeCheckReadFd():"));
   		err = PipeCheckReadFd();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}

   	else if(TestStepName() == KPopenPipeCommandRead)
   		{
   		INFO_PRINTF1(_L("Testing PopenPipeCommandRead()..."));
   		err = PopenPipeCommandRead();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  		
   	else if(TestStepName() == KMultiplePopenPipeCommandRead)
   		{
   		INFO_PRINTF1(_L("Testing MultiplePopenPipeCommandRead()..."));
   		err = MultiplePopenPipeCommandRead();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  		   	   				
   	else if(TestStepName() == KPopenPipeOEExeRead)
   		{
   		INFO_PRINTF1(_L("Testing PopenPipeOEExeRead()..."));
   		err = PopenPipeOEExeRead();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  		
   	else if(TestStepName() == KPopenPipeInvalidPathRead)
   		{
   		INFO_PRINTF1(_L("Testing PopenPipeInvalidPathRead()..."));
   		err = PopenPipeInvalidPathRead();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  		
   	else if(TestStepName() == KPopenPipeInvalidCommand)
   		{
   		INFO_PRINTF1(_L("Testing PopenPipeInvalidCommand()..."));
   		err = PopenPipeInvalidCommand();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  		
   	else if(TestStepName() == KPopenPipeInvalidMode)
   		{
   		INFO_PRINTF1(_L("Testing PopenPipeInvalidMode()..."));
   		err = PopenPipeInvalidMode();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  		  	   		
   	else if(TestStepName() == KPopenPipeWrite)
   		{
   		INFO_PRINTF1(_L("Testing PopenPipeWrite()..."));
   		err = PopenPipeWrite();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  		
   	else if(TestStepName() == KPopenBlockingRead)
   		{
   		INFO_PRINTF1(_L("Testing PopenBlockingRead()..."));
   		err = PopenBlockingRead();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	   	   		
   	else if(TestStepName() == KPopen3PipeCommandRead)
   		{
   		INFO_PRINTF1(_L("Testing Popen3PipeCommandRead()..."));
   		err = Popen3PipeCommandRead();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  	
   	else if(TestStepName() == KMultiplePopen3PipeCommandRead)
   		{
   		INFO_PRINTF1(_L("Testing MultiplePopen3PipeCommandRead()..."));
   		err = MultiplePopen3PipeCommandRead();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  		   	   			
   	else if(TestStepName() == KPopen3PipeOEExeRead)
   		{
   		INFO_PRINTF1(_L("Testing Popen3PipeOEExeRead()..."));
   		err = Popen3PipeOEExeRead();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  		
   	else if(TestStepName() == KPopen3PipeInvalidPathRead)
   		{
   		INFO_PRINTF1(_L("Testing Popen3PipeInvalidPathRead()..."));
   		err = Popen3PipeInvalidPathRead();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  		
   	else if(TestStepName() == KPopen3PipeInvalidCommand)
   		{
   		INFO_PRINTF1(_L("Testing PopenPipeInvalidCommand()..."));
   		err = Popen3PipeInvalidCommand();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  		
   	else if(TestStepName() == KTestProcessPopen3ChitChat)
   		{
   		INFO_PRINTF1(_L("Testing TestProcessPopen3ChitChat()..."));
   		err = TestProcessPopen3ChitChat();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}     	   		
	else if(TestStepName() == KPopen3ReadWriteTest)
   		{
   		INFO_PRINTF1(_L("Testing Popen3ReadWriteTest()..."));
   		err = Popen3ReadWriteTest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == KTestSystem)
   		{
   		INFO_PRINTF1(_L("Testing System()..."));
   		err = TestSystem();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  
   	else if(TestStepName() == KLseekpipetest)
   		{
   		INFO_PRINTF1(_L("Lseekpipetest()..."));
   		err = Lseekpipetest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 	   	  
   	else if(TestStepName() == KOpenMaxfdPipetest)
   		{
   		INFO_PRINTF1(_L("LOpenMaxfdPipetest()..."));
   		err = OpenMaxfdPipetest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == KSimultaneousOpenfdPipetest)
   		{
   		INFO_PRINTF1(_L("SimultaneousOpenfdPipetest()..."));
   		err = SimultaneousOpenfdPipetest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 	   	  
	else if(TestStepName() == KFopenMaxPopenTest)
   		{
   		INFO_PRINTF1(_L("FopenMaxPopenTest()..."));
   		err = FopenMaxPopenTest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == KFopenMaxPopen3Test)
   		{
   		INFO_PRINTF1(_L("FopenMaxPopen3Test()..."));
   		err = FopenMaxPopen3Test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == KTestEnvPopen3)
   		{
   		INFO_PRINTF1(_L("TestEnvPopen3()..."));
   		err = TestEnvPopen3();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == KTestsystem_LongName)
   		{
   		INFO_PRINTF1(_L("Testsystem_LongName()..."));
   		err = Testsystem_LongName();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == KTestwsystem_LongName)
   		{
   		INFO_PRINTF1(_L("Testwsystem_LongName()..."));
   		err = Testwsystem_LongName();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == KTestpopen_LongName)
   		{
   		INFO_PRINTF1(_L("Testpopen_LongName()..."));
   		err = Testpopen_LongName();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == KTestwpopen_LongName)
   		{
   		INFO_PRINTF1(_L("Testwpopen_LongName()..."));
   		err = Testwpopen_LongName();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == KTestpopen3_LongName)
   		{
   		INFO_PRINTF1(_L("Testpopen3_LongName()..."));
   		err = Testpopen3_LongName();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == KTestwpopen3_LongName)
   		{
   		INFO_PRINTF1(_L("Testwpopen3_LongName()..."));
   		err = Testwpopen3_LongName();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == KTestPipeWaitForData)
   		{
   		INFO_PRINTF1(_L("TestPipeWaitForData()..."));
   		err = TestPipeWaitForData();
   		SetTestStepResult(err ? static_cast<TVerdict> (err): EPass);
   		}
   		
   	return TestStepResult(); 
	}
