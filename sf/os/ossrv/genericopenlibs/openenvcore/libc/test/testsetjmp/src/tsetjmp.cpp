/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tsetjmp.h"
#include "tsetjmpserver.h"
#include <unistd.h> 


#define KGLOBVALINIT 1
#define KAUTOVALINIT 2
#define KREGVALINIT 3
#define KVOLAVALINIT 4
#define KSTATVALINIT 5

#define KGLOBVALFIRST 101
#define KAUTOVALFIRST 102
#define KREGVALFIRST 103
#define KVOLAVOLFIRST 104
#define KSTATVALFIRST 105

#define KGLOBVALSECOND 201
#define KAUTOVALSECOND 202
#define KREGVALSECOND 203
#define KVOLAVALSECOND 204
#define KSTATVALSECOND 205

#define KLONGJUMPRETURNVAL 1
#define COMMANDLINELEN 256
#define BYTEVALUE      0xcc
#define MYSEMAPHOREKEY 1616

#define GETSETOPTION 0666

static int globval;

_LIT(KCreateThreadFormat, "CreateThread");
_LIT(KCreateProcessFormat, "CreateProcess");
_LIT(KCallSetjmpTwiceFormat, "CallSetjmpTwice");
_LIT(KLongJmpBufIndexFormat, "LongJmpBufIndex");
_LIT(KWorkerProcessName, "WorkerProcessName");


CSetjmpTest::~CSetjmpTest() 
	{ 
	}  

CSetjmpTest::CSetjmpTest(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CSetjmpTest::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CSetjmpTest::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;
	return TestStepResult();
	}
TVerdict CSetjmpTest::doTestStepL()
	{
	TVerdict err = EFail;
	if(TestStepName() == KsetjmpTest)
		{
		INFO_PRINTF1(_L("setjmpTest():"));
		err = setjmpTest();
		}
	return err; 
	}
TVerdict CSetjmpTest::setjmpTest()
	{
	SetTestStepResult(EFail);
	int             autoval;
	register int    regival;
	volatile int    volaval;
	static int      statval;
	int             result;

	// for each type of storage' variable, set initialize value
	globval = KGLOBVALINIT; 
	autoval = KAUTOVALINIT; 
	regival = KREGVALINIT; 
	volaval = KVOLAVALINIT; 
	statval = KSTATVALINIT; 
		
	GetBoolFromConfig(ConfigSection(), KCreateThreadFormat, iCreateThread);
	GetBoolFromConfig(ConfigSection(), KCreateProcessFormat, iCreateProcess);
	GetBoolFromConfig(ConfigSection(), KCallSetjmpTwiceFormat, iCallSetjmpTwice);
	GetIntFromConfig(ConfigSection(), KLongJmpBufIndexFormat, iLongJmpBufIndex);
	GetStringFromConfig(ConfigSection(), KWorkerProcessName, iWorkerProcessName);
	
	// first long jump return entry
	if( (result = setjmp(iJmpBuffer[0]))!= 0)
	   	{  
    	if(result == KLONGJUMPRETURNVAL)
        	 {
           	 if(CompareToExpectedValue( globval,autoval, regival, volaval, statval))
        	     {
        	     return TestStepResult();
        	     }
        	 else
        		 {
        		 SetTestStepResult(EPass);
        		 return TestStepResult();
        		 }
        	 }
         else
        	 {
        	 return TestStepResult();
        	 }
    	}
 /*
  * Change variables after setjmp, but before longjmp.
  */
	globval = KGLOBVALFIRST; 
	autoval = KAUTOVALFIRST; 
	regival = KREGVALFIRST; 
	volaval = KVOLAVOLFIRST; 
	statval = KSTATVALFIRST; 
	if(iCallSetjmpTwice)
    	{
    	result = setjmp(iJmpBuffer[1]);
    	if(result != 0) 
    		{
    	    if(result == KLONGJUMPRETURNVAL)
    	    	{
    	       	if(CompareToExpectedValue( globval,autoval, regival, volaval, statval))
    	    		{
    	    		return TestStepResult();
    	    		}
     	  	    else
     	  	    	{
     	  	    	SetTestStepResult(EPass);
     	  	    	return TestStepResult();
     	  	    	}
    	    	}
    	    else
    	    	{
    	    	return TestStepResult();
    	    	}
    		}
	    //second long jump return entry
	 	globval = KGLOBVALSECOND; 
	 	autoval = KAUTOVALSECOND; 
	 	regival = KREGVALSECOND; 
	 	volaval = KVOLAVALSECOND; 
	 	statval = KSTATVALSECOND;
	  	}
    
    
    // create a thread and run it in between
	if(iCreateThread)
		{
	    pthread_t testthread;
		pthread_create(&testthread, NULL, (thread_begin_routine) &threadfunction, NULL);	
		pthread_join(testthread, NULL);
		}
	else if(iCreateProcess)
		{
		int semid; 
		key_t key = MYSEMAPHOREKEY; 
		int semflg = IPC_CREAT | GETSETOPTION; 
		int nsems = 1; 

		// set up semaphore 
		semid = semget(key, nsems, semflg);
		if (semid == -1) 
			{
			ERR_PRINTF1(_L("Semaphore initialized failed \n"));
			return TestStepResult();
			} 
		else 
			{
			_LIT(Ksem,"Semaphore initialized success with id: %d \n");
			INFO_PRINTF2(Ksem, semid);
			}

		// create a new file for testing
		char testfilename[L_tmpnam];
		int testfileD = open(tmpnam(testfilename), O_CREAT | O_RDWR);
		if(-1 == testfileD)
			{
			ERR_PRINTF1(_L("Test file can not be opened."));
			return TestStepResult();
			}
		// Create child process using popen(). Child process writes to the Parent therefore "r" 
				
		 char* childprocessbinaryname = (char*) malloc(sizeof(char) * COMMANDLINELEN);
		 char* firstcommandline = (char*) malloc(sizeof(char) * COMMANDLINELEN);
				
		// check buffer, if can not be allocated, then leave
		if(NULL == childprocessbinaryname || NULL == firstcommandline)
			{
			free(childprocessbinaryname);
			free(firstcommandline);			
			User::Leave(KErrGeneral);
			}
		CopyTDescToString(iWorkerProcessName, childprocessbinaryname);
		sprintf(firstcommandline, "%s %d %s %d %d", 
						childprocessbinaryname,
						semid,
						testfilename,
						COMMANDLINELEN,
						BYTEVALUE);	
		FILE* cpFirstAppender = popen(firstcommandline,"r");
		if(cpFirstAppender == NULL)
			{
			ERR_PRINTF1(_L("Sub process created failed."));
			return TestStepResult();
			}
		ssize_t sz = fread(firstcommandline,COMMANDLINELEN,1,cpFirstAppender);
		int ret = pclose(cpFirstAppender);
		free(childprocessbinaryname);
		free(firstcommandline);
		}
	longjmp(iJmpBuffer[iLongJmpBufIndex], KLONGJUMPRETURNVAL);
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CSetjmpTest::CompareToExpectedValue(int gv,int& av, int rv, int vv, int sv)
	{
	TVerdict result = EFail;
	if(!iCallSetjmpTwice)
		{
		if(gv == KGLOBVALFIRST &&
		   av == KAUTOVALFIRST && 
		   vv == KVOLAVOLFIRST &&
		   sv == KSTATVALFIRST)
			{
			result = EPass;
			}
		}
	else
		{
		if(gv == KGLOBVALSECOND &&
			av == KAUTOVALSECOND && 
			vv == KVOLAVALSECOND &&
			sv == KSTATVALSECOND)
			{
			result = EPass;
			}
		}
	if(iLongJmpBufIndex == 0)
		{
		if(rv != KREGVALINIT)
			{
			result = EFail;
			}
		}
	else
		{
		if(rv != KREGVALFIRST)
			{
			result = EFail;
			}
		}
	printf(" g_ : %d,a_: %d, r_: %d, v_: %d, s_: %d \n",gv,av, rv, vv, sv);
	return result;
	}
void* CSetjmpTest::threadfunction(void* /*para*/)
	{
	return NULL;
	}
void CSetjmpTest::CopyTDescToString(const TDesC& aSource, char* aDestination)
	{
	int copyIndex(0);
	for(; copyIndex < aSource.Length(); copyIndex++)
		{
		aDestination[copyIndex] = aSource[copyIndex];
		}
	aDestination[copyIndex] = '\0';
	}

