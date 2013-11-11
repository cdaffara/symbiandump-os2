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


#include "tpthread.h"


// Forward declarations of thread entry functions
void* ThreadEntryPoint(void* aParam);
void* SocketReadThreadEntryPoint( void* aParam );
void* SocketWriteThreadEntryPoint( void* aParam );
void* FileReadThreadEntryPoint( void* aParam );
void* FileWriteThreadEntryPoint( void* aParam );
void *ThreadEntryPointDummy(void* aParam ); 

//static functions used for PThreadJoinTest case
static void *first(void *arg); 
static void *second(void *arg); 
static void *third(void *arg);
	

// -----------------------------------------------------------------------------
// CTPThreadBasicApis::PThreadBasicApisTest
// Description: This function does most of the basic pthread api testing
// This test function can be called with below syntax from INI file:
// PThreadBasicAPITest <API> <TestType> [<State> <StackSize>]
// API is an integer which can be any of the below:
// 		PTHREAD_SELF (1)
// 		PTHREAD_CREATE (2)
// 		PTHREAD_DETACH (3)
// 		PTHREAD_JOIN (4)
// 		PTHREAD_EXIT (5)
// TestType is an integer which can be any of the below:
// 		POSITIVE_TEST (1)
// 		NEGATIVE_TEST (2)
// State is an integer which can be any of the below:
// 		JOIN_THREAD (0)
// 		DETACH_THREAD (1)
// StackSize is the size of the stack whic is an integer
// PThreadBasicAPITest PTHREAD_SELF NEGATIVE_TEST
// -----------------------------------------------------------------------------
//
TInt CTestPThread::PThreadBasicApisTest()
    {

    INFO_PRINTF1(_L("In TPThreadBasicApis"));

    TInt retVal = KErrNone;
    TInt apiType = 1;
    
    _LIT( KParameter1, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), KParameter1, apiType);
	
    TInt testType = 1;
    _LIT( KParameter2, "Parameter2" );
	res = GetIntFromConfig(ConfigSection(), KParameter2, testType);
	 
    switch(apiType)
    	{
    	case EPThreadSelf:
    		{
    		retVal = PThreadSelfApiTest();
    		break;
    		}
    	case EPThreadCreate:
    		{
    		retVal = PThreadCreateApiTest( (TTestType)testType );
    		break;
    		}
    	case EPThreadJoin:
    		{
    		retVal = PThreadJoinApiTest(  (TTestType)testType );
    		break;
    		}
    	case EPThreadDetach:
    		{
    		retVal = PThreadDetachApiTest(  (TTestType)testType );
    		break;
    		}
    	case EPThreadExit:
    		{
    		//Read Exit Reasson from INI file
    		TInt exitReason = 0;
    		_LIT( KParameter3, "Parameter3" );
			res = GetIntFromConfig(ConfigSection(), KParameter3, exitReason);
			if(!res)
			{
			 	_LIT(Kerr , "Failed to read parameter exit reason from ini file") ;
			 	ERR_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
			}
    		
    		retVal = PThreadExitApiTest( exitReason );
    		break;
    		}
    	default:
    		{
    		INFO_PRINTF2(_L("Invalid arguments from ini file : %d"), apiType);
    		retVal = KErrArgument;
    		break;
    		}
    	}

    return retVal;
    }
// -----------------------------------------------------------------------------
// CTPThreadBasicApis::PThreadSelfApiTest
// Description: This function tests pthread_self API
// returns KErrNone or error code depending on the test
// -----------------------------------------------------------------------------
//
TInt CTestPThread::PThreadSelfApiTest()
	{

	TInt retVal = KErrNone;

    pthread_t selfID = pthread_self();

	
    if( selfID == 0 )
    	{
	    INFO_PRINTF2(_L("Wrong return Value from pthread_self() : %d"), selfID);
		retVal = KErrPThreadSelf;
    	}
    else
    	{
   		pthread_t threadID = 0;

		pthread_attr_t threadAttr;
		pthread_attr_init( &threadAttr );

		retVal = pthread_create( &threadID, &threadAttr, ThreadEntryPoint,
			(void*)KErrNone );
		if( KErrNone != retVal )
			{
			INFO_PRINTF2(_L("pthread_create FAILED with %d"), retVal );
			retVal = KErrPThreadCreate;
			}
		else
			{
			//Testing pthread self aswell.....
			if(pthread_equal(selfID,selfID) == 0)
				{
				retVal += KErrPThreadSelf;
				}
			if(pthread_equal(selfID,threadID))
				{	
				retVal += KErrPThreadSelf;
				}
			
			//get the thread id again, if it does not match.. ERROR
			pthread_t selfID1 = pthread_self();
			if( selfID1 != selfID )
				{
				INFO_PRINTF1(_L("undefined behaviour from pthread_self!!"));
				retVal += KErrPThreadSelf;
				}
			else
				{
		    	TBuf<20> apiName(KPThreadSelf);
				INFO_PRINTF2(_L("API %S returned 1"),&apiName);
				}
			}
		int threadRetVal = 0;
		TInt ret = pthread_join(threadID, (void**)threadRetVal);
    	}
    return retVal;
	}

// -----------------------------------------------------------------------------
// CTPThreadBasicApis::PThreadCreateApiTest
// Description: This function tests pthread_create and
// pthread attr related APIs
// returns KErrNone or error code depending on the test
// -----------------------------------------------------------------------------
//
TInt CTestPThread::PThreadCreateApiTest(TTestType aTestType )
	{
    TInt retVal = KErrNone;
	pthread_t threadID = 0;
	pthread_attr_t threadAttr;

	pthread_t selfID = pthread_self();
	INFO_PRINTF2(_L("Thread Id is : %d"), selfID);

	if( aTestType == EPositive )
		{
		//Read Attributes like detatch state and stack size from INI file
		TInt threadState = PTHREAD_CREATE_DETACHED;
		TInt stackSize = KDefaultPThreadStackSize;
        TUint stkSize;
        
		ReadThreadAttributes( threadState, stackSize, aTestType);

		//Set all attributes as 0
		memset(&threadAttr, 0, sizeof(threadAttr));
		pthread_attr_init( &threadAttr );
		pthread_attr_setdetachstate( &threadAttr, threadState );
		if (pthread_attr_setstacksize( &threadAttr, stackSize ) == EINVAL)
            {
            // Test case passed
            return KErrNone;
            }
        //This has been added to cover getstack size test case
        if (pthread_attr_getstacksize( &threadAttr, &stkSize ) != 0)
            {
            return KErrPThreadCreate;
            }
        if (stackSize != stkSize)
            {
            return KErrPThreadCreate;
            }
		}

    //Create a thread now
	retVal = pthread_create( &threadID, &threadAttr, ThreadEntryPoint,
			(void*)KErrNone );

	selfID = pthread_self();
	INFO_PRINTF2(_L("Thread Id is : %d"), selfID);

	if( aTestType == ENegative && EINVAL != retVal )
		{
		// As we have sent some wrong parameters, this should return with EINVAL
		INFO_PRINTF2(_L("Wrong return Value from pthread_create() : %d"), retVal);
		retVal = KErrPThreadCreate;
		}
	else if( aTestType == EPositive && KErrNone != retVal )
		{
		// As we have sent some wrong parameters,
		// this should return with EINVAL
		INFO_PRINTF2(_L("pthread_create() Failed with : %d !!"), retVal);
		retVal = KErrPThreadCreate;
		}
	else
		{
    	TBuf<20> apiName(KPThreadCreate);
		INFO_PRINTF3(_L("API Test %S of type %d passed"),&apiName,aTestType);
		retVal = KErrNone;
		}
	pthread_attr_destroy( &threadAttr );
    return retVal;
	}

// -----------------------------------------------------------------------------
// CTPThreadBasicApis::PThreadJoinApiTest
// Description: This function tests pthread_join API
// returns KErrNone or error code depending on the test
// -----------------------------------------------------------------------------
//
TInt CTestPThread::PThreadJoinApiTest(TTestType aTestType )
	{
    TInt retVal = KErrNone;
	pthread_t threadID = 0;
    pthread_attr_t threadAttr;
	TInt threadState = PTHREAD_CREATE_DETACHED;
	TInt stackSize = KDefaultPThreadStackSize;
	TInt moreFlag = 0;

    if( aTestType == EPositive )
    	{
		//Read Attributes like detatch state and stack size from INI file
		ReadThreadAttributes( threadState, stackSize, aTestType);
	
		_LIT( KParameter5, "Parameter5" );
		TBool res = GetIntFromConfig(ConfigSection(), KParameter5, moreFlag);
				
		if( PTHREAD_CREATE_DETACHED == threadState )
			{
			aTestType = ENegative;
			}

		pthread_attr_init( &threadAttr );
		pthread_attr_setdetachstate( &threadAttr, threadState );
		pthread_attr_setstacksize( &threadAttr, stackSize );

	    //Create a thread now
		retVal = pthread_create( &threadID, &threadAttr, ThreadEntryPoint,
			(void*)KErrNone );
    	}

	if( KErrNone != retVal )
		{
		// pthread_Create Failed!!
		INFO_PRINTF2(_L("pthread_create() Failed with : %d"), retVal);
		retVal = KErrPThreadCreate;
		}
	else
		{
		int threadRetVal = 0;
		TInt ret = pthread_join(threadID, (void**)threadRetVal);

		if( ENegative == aTestType )
			{
			//If thread id is 0, then return should be ESRCH
			if( 0 == threadID )
				{
				if(ESRCH != ret )
					{
					INFO_PRINTF2(_L("Unexpected return value from pthread_join: %d"), retVal);
					INFO_PRINTF1(_L("Expected ESRCH as return error code"));
					retVal = KErrPThreadJoin;
					}
				}
			//If thread state is not joinable, then return should be EINVAL
			else if ( PTHREAD_CREATE_JOINABLE == threadState )
				{
				if( EINVAL != ret )
					{
					INFO_PRINTF2(_L("Unexpected return value from pthread_join: %d"), retVal);
					INFO_PRINTF1(_L("Expected EINVAL as return error code"));
					retVal = KErrPThreadJoin;
					}
				}
			}
	    //If its positive test, then return should be 0
	    else if( KErrNone != ret )
	    	{
	    	INFO_PRINTF2(_L("Unexpected return value from pthread_join: %d"), retVal);
	    	INFO_PRINTF1(_L("Expected pthread_join to return successfully"));
			retVal = KErrPThreadJoin;
	    	}
	    else if( moreFlag )
	    	{
	    	//issue pthread_join again on same thread which is terminated already
	    	if( ESRCH != (ret = pthread_join(threadID, (void**)threadRetVal)) )
	    		{
				INFO_PRINTF2(_L("Unexpected return value from pthread_join: %d"), ret);
				INFO_PRINTF1(_L("Expected ESRCH as return error code"));
				retVal = KErrPThreadJoin;
	    		}
	    	}

	    if( KErrNone == retVal )
	    	{
	    	TBuf<20> apiName(KPThreadJoin);
			LogResult(aTestType, apiName);
	    	}
		}

    return retVal;
	}

// -----------------------------------------------------------------------------
// CTPThreadBasicApis::PThreadDetachApiTest
// Description: This function tests pthread_detach API
// returns KErrNone or error code depending on the test
// -----------------------------------------------------------------------------
//
TInt CTestPThread::PThreadDetachApiTest( TTestType aTestType )
	{
    TInt retVal = KErrNone;
	pthread_t threadID = 0;
    pthread_attr_t threadAttr;
	TInt threadState = PTHREAD_CREATE_JOINABLE;
	TInt thrState;
	TInt stackSize = KDefaultPThreadStackSize;
	TInt moreFlag = 0;

    if( aTestType == EPositive )
    	{
		//Read Attributes like detatch state and stack size from INI file
		ReadThreadAttributes( threadState, stackSize, aTestType);
		
		_LIT( KParameter5, "Parameter5" );
		TBool res = GetIntFromConfig(ConfigSection(), KParameter5, moreFlag);
		
		
		if( PTHREAD_CREATE_DETACHED == threadState )
			{
			aTestType = ENegative;
			}

		pthread_attr_init( &threadAttr );
		pthread_attr_setdetachstate( &threadAttr, threadState );
		//testing getdetachstate aswell....
		if(pthread_attr_getdetachstate(&threadAttr, &thrState))
			{
		    retVal += KErrPThreadDetach;
			}
		if(threadState != thrState)
			{
		    retVal += KErrPThreadDetach;
			}
		pthread_attr_setstacksize( &threadAttr, stackSize );

	    //Create a thread now
		retVal += pthread_create( &threadID, &threadAttr, ThreadEntryPoint,
			(void*)KErrNone );
    	}

	if( KErrNone != retVal )
		{
		// pthread_Create Failed!!
		INFO_PRINTF2(_L("pthread_create() Failed with : %d"), retVal);
		retVal = KErrPThreadCreate;
		}
	else
		{
		TInt ret = pthread_detach( threadID );

		if( ENegative == aTestType )
			{
			//If thread id is 0, then return should be ESRCH
			if( 0 == threadID )
				{
				if( ESRCH != ret )
					{
					INFO_PRINTF2(_L("Unexpected return value from pthread_detach: %d"), retVal);
					INFO_PRINTF1(_L("Expected ESRCH as return error code"));
					retVal = KErrPThreadDetach;
					}
				}
			//If thread state is not joinable, then return should be EINVAL
			else if ( PTHREAD_CREATE_JOINABLE == threadState )
				{
				if( EINVAL != ret )
					{
					INFO_PRINTF2(_L("Unexpected return value from pthread_detach: %d"), retVal);
					INFO_PRINTF1(_L("Expected EINVAL as return error code"));
					retVal = KErrPThreadDetach;
					}
				}
			}
	    //If its positive test, and return should be 0
	    else if( ret != KErrNone )
	    	{
	    	INFO_PRINTF2(_L("Unexpected return value from pthread_detach: %d"), retVal);
	    	INFO_PRINTF1(_L("Expected pthread_detach to return successfully"));
			retVal = KErrPThreadDetach;
	    	}
	    else if( moreFlag )
	    	{
	    	//issue pthread_detach again on same thread, which is no more joinable
	    	//return should be EINVAL
	    	ret = pthread_detach( threadID );
	    	if( (EINVAL != ret ) &&  (ESRCH != ret))
	    		{
				INFO_PRINTF2(_L("Unexpected return value from pthread_detach: %d"), ret);
				INFO_PRINTF1(_L("Expected ESRCH as return error code"));
				retVal = KErrPThreadDetach;
	    		}
	    	}

	    if( KErrNone == retVal )
	    	{
	    	TBuf<20> apiName(KPThreadDetach);
			LogResult(aTestType, apiName);
	    	}
		}

    return retVal;
	}

// -----------------------------------------------------------------------------
// CTPThreadBasicApis::PThreadExitApiTest
// Description: This function tests pthread_exit API
// returns KErrNone or error code depending on the test
// -----------------------------------------------------------------------------
//
TInt CTestPThread::PThreadExitApiTest( TInt aExitReason )
	{
    TInt retVal = KErrNone;
	pthread_t threadID = 0;

	pthread_attr_t threadAttr;
	pthread_attr_init( &threadAttr );
	pthread_attr_setdetachstate( &threadAttr, PTHREAD_CREATE_JOINABLE );
    //Create a thread now
	retVal = pthread_create( &threadID, &threadAttr, ThreadEntryPoint,
		(void*)aExitReason );

	if( KErrNone != retVal )
		{
		// pthread_Create Failed!!
		INFO_PRINTF2(_L("pthread_create() Failed with : %d"), retVal);
		retVal = KErrPThreadCreate;
		}
	else
		{
		//Wait for the completion of thread
		TInt exitReason = 0;
		TInt ret = pthread_join( threadID, (void**) &exitReason );
		if( KErrNone != ret )
			{
			INFO_PRINTF2(_L("pthread_join returned with %d!!"), ret);
			retVal = KErrPThreadJoin;
			}
		else if( exitReason != aExitReason )
			{
			INFO_PRINTF2(_L("unexpected return with pthread_exit-> %d!!"), exitReason);
			retVal = KErrPThreadExit;
			}
		}

    if( KErrNone == retVal )
    	{
    	TBuf<20> apiName(KPThreadExit);
		LogResult(EPositive, apiName);
    	}

    return retVal;
	}
	
// -----------------------------------------------------------------------------
// CTestPThread::ReadThreadAttributes
// Description: Reads Thread Attributes from INI file
// -----------------------------------------------------------------------------
//
void CTestPThread::ReadThreadAttributes(TInt& aThreadState,
        		TInt& aStackSize, TTestType& aTestType)
	{
	
	_LIT( KParameter3, "Parameter3" );
	TBool res = GetIntFromConfig(ConfigSection(), KParameter3, aThreadState);
	
	
	_LIT( KParameter4, "Parameter4" );
	res = GetIntFromConfig(ConfigSection(), KParameter4, aStackSize);
	

	if( PTHREAD_STACK_MIN > aStackSize ||
	    ( PTHREAD_CREATE_JOINABLE != aThreadState &&
	      PTHREAD_CREATE_DETACHED != aThreadState ) )
		{
		INFO_PRINTF1(_L("Invalid arguments from INI file!!"));
		aTestType = ENegative;
		}
	}
// -----------------------------------------------------------------------------
// CTPThreadBasicApis::LogResult
// Description: Logs the test api and test result
// -----------------------------------------------------------------------------
//
inline void CTestPThread::LogResult(TTestType aTestType, TPtrC aBuf)
	{
	TBuf<20> testBuf( KPositive );
	if( aTestType == ENegative )
		{
		testBuf.Copy( KNegative );
		}

	INFO_PRINTF3(_L("[%S] %S Test Passed"), &aBuf, &testBuf);
	}


// -----------------------------------------------------------------------------
// CTPThreadBasicApis::TestMultiplePThread
// Description: This function tests pthread_create and pthread_join API
// Two threads will be created and if needed, main thread will wait for both
// threads
// returns KErrNone or error code depending on the test
// -----------------------------------------------------------------------------
//
TInt CTestPThread::TestMultiplePThread()
	{
	int retVal = 0;
	TInt threadState = PTHREAD_CREATE_DETACHED;
	//Read Thread State from INI file
	_LIT( KParameter1, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), KParameter1, threadState);
	
	TInt waitFlag = 0;
	
	_LIT( KParameter2, "Parameter2" );
	res = GetIntFromConfig(ConfigSection(), KParameter2, waitFlag);
	
	pthread_t threadID1 = 0;
	pthread_t threadID2 = 0;
	pthread_attr_t threadAttr;
	pthread_attr_init( &threadAttr );
	pthread_attr_setdetachstate( &threadAttr, threadState );

	//Create a thread now
	retVal = pthread_create( &threadID1, &threadAttr, ThreadEntryPoint, (void*)0);
	INFO_PRINTF2(_L("First pthread_create() returned with %d"), retVal);
	if( retVal == 0)
		{
		//Create one more thread
		retVal = pthread_create( &threadID2, &threadAttr, ThreadEntryPoint, (void*)0);
		INFO_PRINTF2(_L("Second pthread_create() returned with %d"), retVal);
		//If its joinable thread and waitFlag id set
		if( KErrNone == retVal && PTHREAD_CREATE_JOINABLE == threadState &&
			waitFlag )
			{
			int ret = pthread_join(threadID1, (void**)NULL);
			INFO_PRINTF2(_L("First pthread_join() returned with %d"), ret);
			}
		if( KErrNone == retVal && PTHREAD_CREATE_JOINABLE == threadState &&
			waitFlag )
			{
			int ret = pthread_join(threadID2, (void**)NULL);
			INFO_PRINTF2(_L("Second pthread_join() returned with %d"), ret);
			}
		}

	return retVal;
	}

// -----------------------------------------------------------------------------
// CTPThreadBasicApis::SocketReadAndWritePThread
// Description: This function tests pthread_create and pthread_join API
// Two threads will be created and if needed, main thread will wait for both
// threads
// returns KErrNone or error code depending on the test
// -----------------------------------------------------------------------------
//
TInt CTestPThread::SocketReadAndWritePThread( )
	{
	int retVal = 0;
	TInt dataCount = KCount;
	//Read Port Number from INI file
	_LIT( KParameter1, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), KParameter1, dataCount);
	
	
    TThreadParam threadParam;

	if( sem_init( &threadParam.iSemaphore, 0, 0 ) != KErrNone )
    	{
    	INFO_PRINTF2(_L("Semaphore creation Failed with %d"), errno );
    	return KErrGeneral;
    	}

	pthread_t threadID1 = 0;
	pthread_t threadID2 = 0;
	pthread_attr_t threadAttr;
	pthread_attr_init( &threadAttr );
	pthread_attr_setdetachstate( &threadAttr, PTHREAD_CREATE_JOINABLE );

	//Create a Read thread now
	retVal = pthread_create( &threadID1, &threadAttr, SocketReadThreadEntryPoint,
		(void*)&threadParam );
	INFO_PRINTF2(_L("ReadThread Creation returned with %d"), retVal);
	if( retVal == 0)
		{
		//Create Write thread
		retVal = pthread_create( &threadID2, &threadAttr,
			SocketWriteThreadEntryPoint, (void*)&threadParam );
		INFO_PRINTF2(_L("WriteThread Creation returned with %d"), retVal);
		//If its joinable thread and waitFlag id set
		if( KErrNone == retVal )
			{
			TInt exitReason = 0;
			retVal = pthread_join(threadID1, (void**)&exitReason );
			INFO_PRINTF3(_L("ReadThread join returned with %d and the exit reason is %d"), retVal, exitReason );
			retVal = pthread_join(threadID2, (void**)&exitReason );
			INFO_PRINTF3(_L("WriteThread join returned with %d and the exit reason is %d"), retVal, exitReason );
			}
		}

	if( sem_destroy( &threadParam.iSemaphore ) != KErrNone )
		{
		INFO_PRINTF2(_L("sem_destroy Failed with %d"), errno );
		retVal = KErrGeneral;
		}

	return retVal;
	}
	
	
void* SocketReadThreadEntryPoint(void* aParam)
	{
	TInt retVal = KErrNone;
	TInt count = 5;
    TThreadParam* pThreadParam = ( TThreadParam* ) aParam;

	TInt portNum = KPortNum + count;
	int serverFd = 0;
	int newFd = 0;
	size_t addrSize;
	struct sockaddr_in servAddr;
	struct sockaddr_in sockAddr;
	const char* buff = "Server --> Sending Data Item : ";
	char sendBuff[50];
	char recvBuff[100];
	const unsigned int maxRecv = 100;

	serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if( serverFd == -1 )
		{
		retVal = KErrGeneral;
		}

	(&servAddr)->sin_addr.s_addr  = 0x0100007F;
	(&servAddr)->sin_family = AF_INET;

	servAddr.sin_port = htons( (TUint16) portNum );
	retVal |= bind( serverFd, (struct sockaddr*)&servAddr, sizeof(servAddr) );
	if( retVal != 0 )
		{
		close( serverFd );
		retVal = KErrGeneral;
		}

	retVal |= listen( serverFd, 1 );
	if( retVal != 0 )
		{
		close( serverFd );
		retVal = KErrGeneral;
		}

	addrSize = sizeof( sockAddr );

	//Tell the other thread that data is ready for reading
	retVal = sem_post(&pThreadParam->iSemaphore);
	if( retVal != 0 )
		{
		close( serverFd );
		retVal = KErrGeneral;
		}

	newFd = accept( serverFd, (struct sockaddr*)&sockAddr, &addrSize);
	if( newFd == -1 )
		{
		close( serverFd );
		retVal = KErrGeneral;
	    return (void*)retVal;
		}

	if( KErrNone == retVal )
		{
		//Now Receive and Send some Data
		for(TUint i=1; i<=count; i++)
			{
			TInt ret = recv(newFd, recvBuff, maxRecv, 0);
			if( ret == -1 )
				{
				close( newFd );
				close( serverFd );
				retVal = KErrGeneral;
				break;
				}
			sprintf(sendBuff, "%s %d", buff, i);
			ret = send(newFd, sendBuff, sizeof(sendBuff), 0);
			if( ret == -1 )
				{
				close( newFd );
				close( serverFd );
				retVal = KErrGeneral;
				break;
				}
			}
		}

	if( KErrNone == retVal )
		{
		close( newFd );
		close( serverFd );
		}
	return (void*)retVal;
	}

void* SocketWriteThreadEntryPoint(void* aParam)
	{
	TInt retVal = KErrNone;
	TInt count = 5;
    TThreadParam* pThreadParam = ( TThreadParam* ) aParam;

	TInt portNum = KPortNum + count;
	int clientFd;
	size_t addrSize;
	struct sockaddr_in servAddr;
	const char* buff = "Client --> Sending Data Item : ";
	char sendBuff[50];
	char recvBuff[100];
	const unsigned int maxRecv = 100;

	(&servAddr)->sin_addr.s_addr  = 0x0100007F;
	(&servAddr)->sin_family = AF_INET;

	servAddr.sin_port = htons( portNum );

	clientFd = socket(AF_INET, SOCK_STREAM, 0);
	if( clientFd == -1 )
		{
		retVal = KErrGeneral;
		}

    //Wait for the other thread to write some data to file
    retVal = sem_wait(&pThreadParam->iSemaphore);
    if( retVal != 0 )
    	{
    	close(clientFd);
    	retVal = KErrGeneral;
	    return (void*)retVal;
    	}

	addrSize = sizeof(servAddr);
	retVal = connect(clientFd, (struct sockaddr*)&servAddr, addrSize);
	//This should Pass
	if( retVal == -1 )
		{
		close(clientFd);
		retVal = KErrGeneral;
		}

	if( KErrNone == retVal )
		{
		//Now Send and Receive some Data
		for(TUint i=1; i<=count; i++)
			{
			sprintf(sendBuff, "%s %d", buff, i);
			TInt ret = send(clientFd, sendBuff, sizeof(sendBuff), 0);
			if(ret == -1)
				{
				close( clientFd );
				retVal = KErrGeneral;
				break;
				}
			ret = recv(clientFd, recvBuff, maxRecv, 0);
			if(ret == -1)
				{
				close( clientFd );
				retVal = KErrGeneral;
				break;
				}
			}
		}

	if (KErrNone == retVal )
		{
		close( clientFd );
		}

	return (void*)retVal;
	}

void* FileReadThreadEntryPoint( void* aParam )
	{
	TInt retVal = KErrNone;
	char string[80];
	int count = 0;
	int index = 0;
	TThreadParam* pThreadParam = ( TThreadParam* ) aParam;
	//Open the File in read mode
	FILE* fp = fopen( KFileName, "r");


	if( fp == NULL )
		{
		retVal = KErrGeneral;
		}
	else
		{
		for( ;index < pThreadParam->iDataCount; index++ )
			{
			//Wait for the other thread to write some data to file
			retVal = sem_wait(&pThreadParam->iSemaphore);
			if( retVal != 0 )
				{
				retVal = KErrGeneral;
				break;
				}
			fscanf(fp, "%s%d", string, &count);
			}
		}

	fclose( fp );
	return (void*)retVal;
	}

void* FileWriteThreadEntryPoint( void* aParam )
	{
	TInt retVal = KErrNone;
	char string[80] = "Hello World... ";
	int index = 0;
	TThreadParam* pThreadParam = ( TThreadParam* ) aParam;
	//Open the File in write mode
	FILE* fp = fopen( KFileName, "w");

	if( fp == NULL )
		{
		retVal = KErrGeneral;
		}
	else
		{
		for( ;index < pThreadParam->iDataCount; index++ )
			{
			fprintf(fp, "%s%d", string, index);
			//Tell the other thread that data is ready for reading
			retVal = sem_post(&pThreadParam->iSemaphore);
			if( retVal != 0 )
				{
				retVal = KErrGeneral;
				break;
				}
			}
		}

    fclose( fp );
	return (void*)retVal;
	}
//General functions 
void* threadFun(void *)
	{
	TInt* val = 0;

	pthread_t selfID = pthread_self();
	for(int i=0;i < 10;i++)
		{
		printf("The index i -> %d and Thread Id -> %d\n",i,selfID);
		}
	return (void*)val;
	}
	

void* threadwrite(void *)
	{
	TInt* val = 0;

	FILE *fp;
	char buf[] = {"This is a test for 2 thread writing to same file"};
	
	pthread_t selfID = pthread_self();

	fp=fopen("c:\\test1.txt","w");
	if(fp==NULL)
		{
		printf("\nFIle creation failed");
		return (void*)val;
		}
	fprintf(fp,"%s",buf);
	fclose(fp);
	return (void*)val;
	}

void* threadappend(void *)
	{
	TInt* val = 0;

	FILE *fp;
	char buf[] = {"This is a test for 2 thread appending to same file"};
	
	pthread_t selfID = pthread_self();

	fp=fopen("c:\\testapp1.txt","a");
	if(fp==NULL)
		{
		printf("\nFIle creation failed");
		return (void*)val;
		}
	fprintf(fp,"%s",buf);
	fclose(fp);
	return (void*)val;
	}
	
void* threadread(void *)
	{
	TInt* val = 0;

	char a; 
	FILE *fp;
	
	pthread_t selfID = pthread_self();

	fp=fopen("c:\\test1.txt","r");
	if(fp==NULL)
		{ 
		printf("\nFIle opening failed");
		return (void*)val;
		}
	while(fread(&a, sizeof(char), 1, fp), !feof(fp) && !ferror(fp)) 
		{
        printf("I read %c\t", a);
	    }
	return (void*)val;
	}


void* ThreadEntryPoint(void* aParam)
	{
	TInt retVal = KErrNone;
	//Get the thread id
	pthread_t selfID = pthread_self();
	//Make sure that, its valid
	if( selfID == 0 )
		{
		//Exit the thread with the reason
		retVal = KErrPThreadSelf;
		pthread_exit( (void*) retVal );
		}
	//Try to join with the same thread
	else
		{
		int ret = pthread_join( selfID, (void**)NULL );
		if( ret!= EDEADLK )
			{
			//Exit the thread with the reason
			retVal = KErrPThreadJoin;
			pthread_exit( (void*) retVal );
			}
		}

	//If sent Parameter is not 0, then exit the thread
	if( (TInt)aParam != KErrNone )
		{
		//Exit the thread with the reason
		pthread_exit( aParam );
		}

	return (void*)retVal;
	}

// -----------------------------------------------------------------------------
// CTPThreadBasicApis::FileReadAndWritePThread
// Description: This function tests pthread_create and pthread_join API
// Two threads will be created and if needed, main thread will wait for both
// threads
// returns KErrNone or error code depending on the test
// -----------------------------------------------------------------------------
//
TInt CTestPThread::FileReadAndWritePThread( )
	{
	int retVal = 0;
	TInt dataCount = KCount;
	//Read Port Number from INI file
	_LIT( KParameter1, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), KParameter1, dataCount);
	
	
	TThreadParam threadParam;
	threadParam.iDataCount = dataCount;

	if( sem_init( &threadParam.iSemaphore, 0, 0 ) != KErrNone )
		{
		INFO_PRINTF2(_L("Semaphore creation Failed with %d"), errno );
		return KErrGeneral;
		}

	pthread_t threadID1 = 0;
	pthread_t threadID2 = 0;
	pthread_attr_t threadAttr;
	pthread_attr_init( &threadAttr );
	pthread_attr_setdetachstate( &threadAttr, PTHREAD_CREATE_JOINABLE );

	//Create a Read thread now
	retVal = pthread_create( &threadID1, &threadAttr, FileReadThreadEntryPoint,
		(void*)&threadParam );
	INFO_PRINTF2(_L("ReadThread Creation returned with %d"), retVal);
	if( retVal == 0)
		{
		//Create Write thread
		retVal = pthread_create( &threadID2, &threadAttr,
			FileWriteThreadEntryPoint, (void*)&threadParam );
		INFO_PRINTF2(_L("WriteThread Creation returned with %d"), retVal);
		//If its joinable thread and waitFlag id set
		if( KErrNone == retVal )
			{
			TInt exitReason = 0;
			retVal = pthread_join(threadID1, (void**)&exitReason );
			INFO_PRINTF3(_L("ReadThread join returned with %d and the exit reason is %d"), retVal, exitReason );
			retVal = pthread_join(threadID2, (void**)&exitReason );
			INFO_PRINTF3(_L("WriteThread join returned with %d and the exit reason is %d"), retVal, exitReason );
			}
		}

	if( sem_destroy( &threadParam.iSemaphore ) != KErrNone )
		{
		INFO_PRINTF2(_L("sem_destroy Failed with %d"), errno );
		retVal = KErrGeneral;
		}

	return retVal;
	}


// -----------------------------------------------------------------------------
// PThreadSchedTest::PThreadSchedTest
// Description: This function does most of the pthread scheduling attribute set/get  api testing
// This test function can be called with below syntax from INI file:
// PThreadSchedTest <API> <TestType> [<State> <StackSize>]
// API is an integer which can be any of the below:
// 		PTHREAD_ATTR_SETSCHEDPARAM(6)
//		PTHREAD_ATTR_GETSCHEDPARAM(7)
//		PTHREAD_ATTR_SETSCHEDPOLICY(8)
//		PTHREAD_ATTR_GETSCHEDPOLICY(9)
//		PTHREAD_SETSCHEDPARAM(10)
//		PTHREAD_GETSCHEDPARAM(11)

// TestType is an integer which can be any of the below:
// 		POSITIVE_TEST (1)
// 		NEGATIVE_TEST (2)
// State is an integer which can be any of the below:
// 		JOIN_THREAD (0)
// 		DETACH_THREAD (1)
// StackSize is the size of the stack whic is an integer
// -----------------------------------------------------------------------------
TInt CTestPThread::PThreadSchedTest(  )
    {
    
    INFO_PRINTF1(_L("In TPThreadSchedApis"));

    TInt retVal = KErrNone;
    TInt apiType = 1;
    _LIT( KParameter1, "Parameter1");
	TBool res = GetIntFromConfig(ConfigSection(), KParameter1, apiType );
	
    TInt testType = 1;
    _LIT( KParameter2, "Parameter2" );
	res = GetIntFromConfig(ConfigSection(), KParameter2, testType);
	
   
    switch(apiType)
    	{
    	case EPThreadAttrSetSchedParam:
    		{
    		retVal = PThreadAttrSetSchedParmAPITest();
    		break;
    		}
    	case EPThreadAttrGetSchedParam:
    	    {
    	    retVal = PThreadAttrGetSchedParmAPITest((TTestType)testType);
    	    break;
    	    }
        case EPThreadAttrSetSchedPolicy:
            {
        	retVal = PThreadAttrSetSchedPolicyAPITest();
    	    break;
            }
        case EPThreadAttrGetSchedPolicy:
            {
            retVal = PThreadAttrGetSchedPolicyAPITest(/*(TTestType)testType*/);
    	    break;
            }
        case EPThreadSetSchedparam:
            {
            retVal = PThreadSetSchedParamAPITest((TTestType)testType);
            break;
            }
        case EPThreadGetSchedparam:
            {
            retVal = PThreadGetSchedParamAPITest((TTestType)testType);
            break;
            }
    	case EPThreadAttrSetScope:
	    	{
	    	retVal = PThreadAttrSetScopeAPITest();
	    	break;
	    	}
	    case EPThreadAttrGetScope1:
	    	{
	    	retVal = PThreadAttrGetScopeAPITest1();
	    	break;
	    	}
	    case EPThreadAttrGetScope2:
	    	{
	    	retVal = PThreadAttrGetScopeAPITest2();
	    	break;
	    	}
	    case EPThreadKeyCreate1:
	    	{
	    	retVal = PThreadKeyCreateAPITest1();
	    	break;
	    	}
    	case  EPThreadKeyCreate2:
    	    {
	    	retVal = PThreadKeyCreateAPITest2();
	    	break;
	    	}

	    case EPThreadKeyDelete:
	    	{
	    	break;
	    	}
    	case EPThreadSetSpecific:
	    	{
	    	retVal = PThreadSetSpecificAPITest();
	    	break;
	    	}
    	case EPThreadGetSpecific:
	    	{
	    	retVal = PThreadGetSpecificAPITest();
	    	break;
	    	}

    	default:
    		{
    		INFO_PRINTF2(_L("Invalid arguments from INI file : %d"), apiType);
    		retVal = KErrArgument;
    		break;
    		}
    	}

    return retVal;

    }

TInt CTestPThread::PThreadAttrSetSchedParmAPITest()
	{
    TInt retVal = KErrNone;
    pthread_attr_t at;

    if (pthread_attr_init (&at) != 0)
        {
        INFO_PRINTF1(_L("attr_init failed"));
        retVal = KErrPThreadAttrGetSchedParam;
        }
    struct sched_param pa;
 
    pa.sched_priority= 100;
    if (pthread_attr_setschedparam (&at, &pa) != 0)
        {
        INFO_PRINTF1(_L("attr_setschedparam failed"));
        retVal = KErrPThreadAttrGetSchedParam;
        }
    else
        {
        TBuf<100> apiName(KPThreadSetSchedParam);
	    LogResult(EPositive, apiName);
        }
    return retVal;
    }

TInt CTestPThread::PThreadAttrGetSchedParmAPITest(TTestType aTestType)
    {
	TInt retVal = KErrNone;
	pthread_attr_t at,at_test;

    if (pthread_attr_init (&at) != 0)
        {
        INFO_PRINTF1(_L("attr_init failed"));
        retVal = KErrPThreadAttrGetSchedParam;
        }
    struct sched_param pa_first,pa_second;
    pa_first.sched_priority = 100;
    if (pthread_attr_setschedparam (&at, &pa_first) != 0)
        {
        INFO_PRINTF1(_L("attr_setschedparam failed"));
        retVal = KErrPThreadAttrGetSchedParam;
        }
    if( aTestType == EPositive )
		{
		if(pthread_attr_getschedparam(&at,&pa_second) !=0)
   	        {
   	        INFO_PRINTF1(_L("attr_getschedparam failed"));
            retVal = KErrPThreadAttrGetSchedParam;
            }
            if(pa_second.sched_priority != pa_first.sched_priority)
   	        {
   	        INFO_PRINTF1(_L("get_attrib and set_attrib mismatched"));
   	        retVal = KErrPThreadAttrGetSchedParam;
           }
		}
	else if(aTestType == ENegative)
	    {
	    struct sched_param pa_third;
        pa_third.sched_priority=100;
        if (pthread_attr_setschedparam (&at_test, &pa_third) != 0)
            {
            ERR_PRINTF1(_L("attr_setschedparam failed"));
            retVal = KErrPThreadAttrGetSchedParam;
            }
        if(pthread_attr_getschedparam(&at,&pa_second) !=0)
   	        {
   	        ERR_PRINTF1(_L("attr_getschedparam failed"));
            retVal = KErrPThreadAttrGetSchedParam;
            }
            if(pa_second.sched_priority == pa_third.sched_priority)
   	        {
   	        ERR_PRINTF1(_L("get_attrib and set_attrib mismatched"));
   	        retVal = KErrPThreadAttrGetSchedParam;
            }
	    }
	if(retVal !=KErrPThreadAttrGetSchedParam )
       {
       TBuf<100> apiName(KPThreadGetSchedParam);
	   LogResult(EPositive, apiName);
       }
	return retVal;
	}
   //API to test the setting of the scheduling policy.

TInt CTestPThread::PThreadAttrSetSchedPolicyAPITest()
    {
   	TInt retVal =  KErrNone;
   	long int r = rand();
   	pthread_attr_t a;
   	int s;
   	if (r != SCHED_RR)
	    {
	    int e = pthread_attr_setschedpolicy (&a, r);

	    if (e == 0)
	        {
	        ERR_PRINTF1(_L("API for setting the scheduling policy failed\n"));
	        retVal = KErrPThreadAttrSetSchedPolicy;
	        }
	    if (e != EINVAL)
	        {
	        ERR_PRINTF1(_L("API for setting the scheduling policy failed\n"));
	        retVal = KErrPThreadAttrSetSchedPolicy;
	        }
	    }

    int   e = pthread_attr_setschedpolicy (&a, SCHED_RR);
    if (e != 0)
        {
        ERR_PRINTF1(_L("API for setting the scheduling policy failed\n"));
        retVal = KErrPThreadAttrSetSchedPolicy;
        }
    if (pthread_attr_getschedpolicy (&a, &s) != 0)
        {
        ERR_PRINTF1(_L("API for setting the scheduling policy failed\n"));
        retVal = KErrPThreadAttrSetSchedPolicy;
        }
    if (s != SCHED_RR)
        {
        ERR_PRINTF1(_L("schedpolicy could not set to SCHED_RR"));
        retVal = KErrPThreadAttrSetSchedPolicy;
        }
    if(retVal == KErrNone)
        {
        TBuf<100> apiName(KPThreadSetSchedPolicy);
    	LogResult(EPositive, apiName);
        }
   	return retVal;
	}
//Testing for the API pthread_attr_getschedpolicy: Date 01/03/06
TInt CTestPThread::PThreadAttrGetSchedPolicyAPITest(/*TTestType aTestType*/)
    {
	TInt retVal = KErrNone;
	pthread_attr_t a;
	int s,e;
	e = pthread_attr_setschedpolicy (&a, SCHED_RR);
	if (e != 0)
        {
        ERR_PRINTF1(_L("API for setting the scheduling policy failed\n"));
        retVal = KErrPThreadAttrSetSchedPolicy;
        }
    if (pthread_attr_getschedpolicy (&a, &s) != 0)
        {
        ERR_PRINTF1(_L("1st attr_getschedpolicy failed"));
        retVal= KErrPThreadAttrGetSchedPolicy;
        }
    /* XXX What is the correct default value.  */
    if (s != SCHED_RR)
        {
        ERR_PRINTF1(_L("incorrect default value for schedpolicy"));
        retVal= KErrPThreadAttrGetSchedPolicy;
        }
    if(retVal == KErrNone)
        {
    	TBuf<100> apiName(KPThreadGetSchedPolicy);
    	LogResult(EPositive, apiName);
        }
	return retVal;
    }
TInt CTestPThread::PThreadSetSchedParamAPITest(TTestType aTestType)
	{
	TInt retVal = KErrNone;
	int e ;
	struct sched_param param;
	pthread_t thread=100; // Thread which will be assigned the scheduling priority and the policy.
	if(aTestType == EPositive)
		{
		pthread_t thread= pthread_self();
		param.sched_priority = 100; 
		
		e = pthread_setschedparam(thread,SCHED_RR, &param);
		if(e != 0)
			{
			ERR_PRINTF1(_L("API for setting the scheduling policy and priority failed."));
			retVal = KErrPThreadSetSchedParam;
			}
		if(e == EINVAL)
			{
			ERR_PRINTF1(_L("policy or param are invalid settings."));
			retVal = KErrPThreadSetSchedParam;
			}
		else if(e == EFAULT)
			{
			ERR_PRINTF1(_L("param is an invalid pointer."));
			retVal = KErrPThreadSetSchedParam;
			}
		else if(e == ENOTSUP)
			{
			ERR_PRINTF1(_L("policy specifies an unsupported setting."));
			retVal = KErrPThreadSetSchedParam;
			}
		else if(e == ESRCH)
			{
			ERR_PRINTF1(_L("thread does not specify a currently running thread in the process."));
			retVal = KErrPThreadSetSchedParam;
			}
		}
	else if(aTestType == ENegative)
		{
		e = pthread_setschedparam(thread,SCHED_RR, &param);
		if(e == 0)
			{
			ERR_PRINTF1(_L("API for Negatively testing the pthread_setschedparam failed.."));
			retVal = KErrPThreadSetSchedParam;
			}
		if(e != EINVAL && e != EFAULT  && e == ENOTSUP && e != ESRCH )
			{
			ERR_PRINTF1(_L("Negative API test for the pthread_setschedparam failed."));
			retVal = KErrPThreadSetSchedParam;
			}
	    if(retVal == KErrNone)
			{
			TBuf<100> apiName(KPThreadSetSchdPrm);
			LogResult(EPositive, apiName);
			}
		}
	return retVal;
	}

TInt CTestPThread::PThreadGetSchedParamAPITest(TTestType aTestType)
	{
	TInt retVal = KErrNone;
	int e ;
	struct sched_param param;
	int policy;
	pthread_t thread=100;
	if(aTestType == EPositive)
		{
		thread = pthread_self(); // Thread which will be assigned the scheduling priority and the policy.
		e = pthread_getschedparam(thread,&policy,&param);
		if (e == EINVAL)
			{
			ERR_PRINTF1(_L("The locations pointed to by policy or param point are not writeable."));
			retVal = KErrPThreadSetSchedParam;
			}
		else if(e == EFAULT)
			{
			ERR_PRINTF1(_L("policy or param is an invalid pointer."));
			retVal = KErrPThreadSetSchedParam;
			}
		else if (e == ESRCH)
			{
			ERR_PRINTF1(_L("thread does not specify a currently running thread in the process."));
			retVal = KErrPThreadSetSchedParam;
			}
		}
	else if(aTestType == ENegative)
		{
		e = pthread_getschedparam(thread,&policy,&param);
		if (e != EINVAL && e != EFAULT && e != ESRCH  )
			{
			ERR_PRINTF1(_L("Negative test for API pthread_getschedparam failed ."));
			retVal = KErrPThreadSetSchedParam;
			}
		}
    if(retVal == KErrNone)
	    {
	    TBuf<100> apiName(KPThreadGetSchdPrm);
	    LogResult(EPositive, apiName);
	    }

	return retVal;
}

//Stand alone test cases exists , added to STIF framework
TInt CTestPThread::PThreadAttrSetScopeAPITest()
	{
	TInt retVal = KErrNone;
	pthread_attr_t a;
    int e;
    int i;
    int s;
    long int r;
	if (pthread_attr_init (&a) != 0)
	    {
	    ERR_PRINTF1(_L("attr_init failed\n"));
	    retVal = KErrPThreadAttrSetScope;
	    }

    for (i = 0; i < 100; ++i)
		{
        r = rand();

        if (r != PTHREAD_SCOPE_SYSTEM)
			{
            e = pthread_attr_setscope (&a, r);
            if (e != EINVAL)
	            {
	            ERR_PRINTF1(_L("attr_setscope didn't return EINVAL\n"));
	            retVal = KErrPThreadAttrSetScope;
	            }

            if (pthread_attr_getscope(&a, &s) != 0)
	            {
	            ERR_PRINTF1(_L("attr_getscope failed.\n"));
	            retVal = KErrPThreadAttrSetScope;
	            }

            if (s != PTHREAD_SCOPE_SYSTEM)
	            {
	            ERR_PRINTF1(_L("contentionscope changed by invalid setscope call.\n"));
	            retVal = KErrPThreadAttrSetScope;
	            }
			}
		}
    if(retVal == KErrNone)
	    {
	    TBuf<100> apiName(KPThreadSetScope);
	    LogResult(EPositive, apiName);
	    }
	return retVal;
	}
TInt CTestPThread::PThreadAttrGetScopeAPITest1()
	{
	TInt retVal = KErrNone;
	pthread_attr_t a;
    int e;
    int s;
    if (pthread_attr_init (&a) != 0)
	    {
	    ERR_PRINTF1(_L("attr_init failed.\n"));
	    retVal = KErrPThreadAttrGetScope;
	    }

    if (pthread_attr_getscope (&a, &s) != 0)
	    {
	    ERR_PRINTF1(_L("1st attr_getscope failed"));
	    retVal = KErrPThreadAttrGetScope;
	    }
    /* correct default value.  */
    if (s != PTHREAD_SCOPE_SYSTEM)
	    {
	    ERR_PRINTF1(_L("incorrect default value for contentionscope.\n"));
	    retVal = KErrPThreadAttrGetScope;
	    }

    e = pthread_attr_setscope (&a, PTHREAD_SCOPE_SYSTEM);
    if (e != ENOTSUP)
	    {
	        if (e != 0)
		        {
		        ERR_PRINTF1(_L("1st attr_setscope failed."));
		        retVal = KErrPThreadAttrGetScope;
		        }
	        if (pthread_attr_getscope (&a, &s) != 0)
		        {
		        ERR_PRINTF1(_L("2nd attr_getscope failed."));
		        retVal = KErrPThreadAttrGetScope;
		        }
	        if (s != PTHREAD_SCOPE_SYSTEM)
		        {
		        ERR_PRINTF1(_L("\
		        contentionscope could not set to PTHREAD_SCOPE_SYSTEM.\n"));
		        retVal = KErrPThreadAttrGetScope;
		        }
	    }

    e = pthread_attr_setscope (&a, PTHREAD_SCOPE_SYSTEM);
    if (e != 0)
	    {
	    ERR_PRINTF1(_L("2nd attr_setscope failed."));
	    retVal = KErrPThreadAttrGetScope;
	    }
    if (pthread_attr_getscope (&a, &s) != 0)
	    {
	    ERR_PRINTF1(_L("3rd attr_getscope failed."));
	    retVal = KErrPThreadAttrGetScope;
	    }
    if (s != PTHREAD_SCOPE_SYSTEM)
	    {
	    ERR_PRINTF1(_L("contentionscope could not set to PTHREAD_SCOPE_SYSTEM.\n"));
	    retVal = KErrPThreadAttrGetScope;
	    }
    if(retVal == KErrNone)
	    {
	    TBuf<100> apiName(KPThreadGetScope);
	    LogResult(EPositive , apiName);
	    }
	//All test cases passed...

	return retVal;
	}

TInt CTestPThread::PThreadAttrGetScopeAPITest2()
	{
	TInt retVal = KErrNone;
	pthread_attr_t a;
    int s1;
    int s2;

    if (pthread_attr_init (&a) != 0)
	    {
	    ERR_PRINTF1(_L("attr_init failed.\n"));
	    retVal = KErrPThreadAttrGetScope;
	    }

    if (pthread_attr_getscope (&a, &s1) != 0)
	    {
	    ERR_PRINTF1(_L("1st attr_getscope failed."));
	    retVal = KErrPThreadAttrGetScope;
	    }
    if (pthread_attr_getscope (&a, &s2) != 0)
	    {
	    ERR_PRINTF1(_L("2nd attr_getscope failed."));
	    retVal = KErrPThreadAttrGetScope;
	    }
    if (s2 != s1)
	    {
	    ERR_PRINTF1(_L("Both scopes are not equal: failed."));
	    retVal = KErrPThreadAttrGetScope;
	    }

    /* correct default value.  */
     if(s1 != PTHREAD_SCOPE_SYSTEM)
	    {
	    ERR_PRINTF1(_L("incorrect default value for contentionscope\n"));
	    retVal = KErrPThreadAttrGetScope;
	    }
     if(retVal == KErrNone)
	    {
	    TBuf<100> apiName(KPThreadGetScope);
	    LogResult(EPositive , apiName);
	    }
	//All Test cases passed...

    return retVal;
	}

TInt CTestPThread::PThreadKeyCreateAPITest1()
	{
    TInt retVal = KErrNone;
    int max;
    int i;
    pthread_key_t *keys;

	#ifdef PTHREAD_KEYS_MAX
		max = PTHREAD_KEYS_MAX;
	#else
		max = _POSIX_THREAD_KEYS_MAX;
	#endif

    keys = (pthread_key_t *)malloc (max * sizeof (pthread_key_t));

    for (i = 0; i < max; ++i)
		{
        if (pthread_key_create (&keys[i], NULL) != 0)
			{
            ERR_PRINTF1(_L("key_create failed  \n"));
            retVal = KErrPThreadKeyCreate;
			}
        else
			{	
            if (pthread_setspecific (keys[i], (const void *) (i + 100l)) != 0)
				{
                ERR_PRINTF1(_L("setspecific failed\n"));
                retVal = KErrPThreadSetSpecific;

				}
			}
		}

    INFO_PRINTF1(_L("All keys are created and value set successfully\n"));

    for (i = 0; i < max; ++i)
		{
        if (pthread_getspecific (keys[i]) != (void *) (i + 100l))
			{
            ERR_PRINTF1(_L("getspecific failed \n"));
            retVal = KErrPThreadGetSpecific;
			}

        if (pthread_key_delete (keys[i]) != 0)
			{
            ERR_PRINTF1(_L("key_delete failed\n"));
            retVal = KErrPThreadKeyDelete;

			}
		}

    INFO_PRINTF1(_L("All keys get and delete success\n"));

  /* Now it must be once again possible to allocate keys.  */
    if (pthread_key_create (&keys[0], NULL) != 0)
		{
        ERR_PRINTF1(_L("key_create failed \n"));
        retVal = KErrPThreadKeyCreate;
		}

    INFO_PRINTF1(_L("all key 2nd time create success\n"));
    if (pthread_key_delete (keys[0]) != 0)
		{
        ERR_PRINTF1(_L("key_delete failed\n"));
        retVal = KErrPThreadKeyDelete;

		}
    INFO_PRINTF1(_L("all key 2nd time delete success\n"));

    if(retVal == KErrNone)
	    {
	    TBuf<100> apiName(KPThreadGetScope);
	    LogResult(EPositive , apiName);
	    free(keys);
	    }
	//All Test cases passed...

    return retVal;

	}

TInt CTestPThread::PThreadKeyCreateAPITest2()
	{
    TInt retVal = KErrNone;
    int max;
    int i;
    pthread_key_t *keys=NULL;

	#ifdef PTHREAD_KEYS_MAX
		max = PTHREAD_KEYS_MAX;
	#else
		max = _POSIX_THREAD_KEYS_MAX;
	#endif

    keys = (pthread_key_t *)malloc ((max +1) * sizeof (pthread_key_t));
    if(!keys)
        return KErrGeneral;
    for (i = 0;i < max; ++i)
		{
        if (pthread_key_create (&keys[i], NULL) != 0)
			{
            ERR_PRINTF1(_L("key_create failed \n"));
            retVal = KErrPThreadKeyCreate;
			}
		}
    INFO_PRINTF1(_L("All keys are created successfully\n"));

    /* Try more than MAX keys */
    if (pthread_key_create (&keys[i], NULL) != EAGAIN)
		{
        ERR_PRINTF1(_L("key_create failed \n"));
        retVal = KErrPThreadKeyCreate;
		}

    INFO_PRINTF1(_L("Testcase: overflow test passed\n"));
    if(retVal == KErrNone)
	    {
	    TBuf<100> apiName(KPThreadGetScope);
	    LogResult(EPositive , apiName);
	    }
	//All Test cases passed...

    for (i = 0;i < max; ++i)
		{
        if (pthread_key_delete (keys[i]) != 0)
			{
            ERR_PRINTF1(_L("key_create failed \n"));
            retVal = KErrPThreadKeyCreate;
			}
		}

	if (keys )
		{
		free(keys);
		}
    return retVal;
	}


TInt CTestPThread::PThreadSetSpecificAPITest()
	{
    TInt retVal = KErrNone;
    pthread_key_t keys;

    // Max Key testing : Manually changing key for testing purpose only
    keys = 128;

    if (pthread_setspecific (keys, (void *) 20l) != EINVAL)
		{
        ERR_PRINTF1(_L("setspecific failed \n"));
        retVal = KErrPThreadSetSpecific;
		}
    INFO_PRINTF1(_L("setspecfic passed MAX test\n"));

    if (pthread_getspecific (keys) != NULL)
		{
        ERR_PRINTF1(_L("getspecific failed \n"));
        retVal = KErrPThreadGetSpecific;
		}
    INFO_PRINTF1(_L("getspecfic passed MAX test\n"));

    if (pthread_key_delete (keys) != EINVAL)
		{
        ERR_PRINTF1(_L("key_delete failed \n"));
        retVal = KErrPThreadKeyDelete;
		}
    INFO_PRINTF1(_L("key_delete passed MAX test\n"));

    INFO_PRINTF1(_L("Testcase: MAX test passed\n"));
    if(retVal == KErrNone)
	    {
	    TBuf<100> apiName(KPThreadGetScope);
	    LogResult(EPositive , apiName);
	    }
	//All Test cases passed...
    return retVal;
	}

TInt CTestPThread::PThreadGetSpecificAPITest()
	{
    TInt retVal = KErrNone;
    pthread_key_t keys;

    // Max Key testing : Manually changing key for testing purpose only
    keys = 128;

    if (pthread_setspecific (keys, (void *) 20l) != EINVAL)
		{
        ERR_PRINTF1(_L("setspecific failed \n"));
        retVal = KErrPThreadSetSpecific;
		}
    INFO_PRINTF1(_L("setspecfic passed MAX test\n"));

    if (pthread_getspecific (keys) != NULL)
		{
        ERR_PRINTF1(_L("getspecific failed \n"));
        retVal = KErrPThreadGetSpecific;
		}
    INFO_PRINTF1(_L("getspecfic passed MAX test\n"));

    if (pthread_key_delete (keys) != EINVAL)
		{
        ERR_PRINTF1(_L("key_delete failed \n"));
        retVal = KErrPThreadKeyDelete;
		}
    INFO_PRINTF1(_L("key_delete passed MAX test\n"));

    INFO_PRINTF1(_L("Testcase: MAX test passed\n"));

    if(retVal == KErrNone)
	    {
	    TBuf<100> apiName(KPThreadGetScope);
	    LogResult(EPositive , apiName);
	    }
	//All Test cases passed...
    return retVal;
	}
	
//static functions added for PThreadJoinTest case
static void *first(void *arg) 
	{
	if(arg!=NULL)
		{
		printf("the argument passed to this function is not NULL");
		}
	printf("First");
	fflush(stdout);
	return 0;
	}

static void *second(void *arg) 	
	{
	if(arg!=NULL)
		{
		printf("the argument passed to this function is not NULL");
		}
	printf("Second");
	fflush(stdout);
	return 0;
	}

static void *third(void *arg) 
	{
	if(arg!=NULL)
		{
		printf("the argument passed to this function is not NULL");
		}
	printf("Third");
	fflush(stdout);
	return 0;
	}

TInt CTestPThread::PThreadJoinTest()
	{
	TInt retVal = KErrNone, fd, newfd;
	pthread_t new_th[3];
	char buf[64];
	fd = open("c:\\pthread.txt" , O_CREAT | O_RDWR | O_EXCL , 0666);
	if(fd < 0 ) 
  		{
  		INFO_PRINTF2(_L("Failed to create and open a file\n %d"), errno);
   		}
	else
  		{
   		INFO_PRINTF1(_L("File created and opened in current working directory \n"  ));
   		}
   	//writing the contents to a file
	newfd = dup2(fd,1);
	if(newfd < 0 ) 
 		{
  		INFO_PRINTF1(_L("Failed to duplicate file descriptor \n"));
 		}
	else
  		{
  		INFO_PRINTF2(_L("the newfd returned is %d"),newfd);
  		}
  	FILE *fp = fopen("c:\\pthread.txt", "r");
	if(fp)
		{
		fscanf(fp, "%s", buf);
		}
	if(strcmp(buf, "FirstSecondThird") != 0)
		{
		INFO_PRINTF1(_L("All threads are not executed\n"));
		}
	else
		{
		INFO_PRINTF1(_L("Executed all the threads\n"));	
		}	
	
	// Create a first thread. 
	if((retVal = pthread_create(&new_th[0], NULL, first, NULL)) != 0)
		{
	  	INFO_PRINTF1(_L("Error in creating first thread \n"));
	  	INFO_PRINTF2(_L("the value returned is %d\n"),retVal);
	  	}
	 
	// Create a second thread. 
	if((retVal = pthread_create(&new_th[1], NULL, second, NULL) )!= 0)
		{
	  	INFO_PRINTF1(_L("Error in creating second thread \n"));
	  	INFO_PRINTF2(_L("the value returned is %d\n"),retVal);
	  	}
	 
	// Create a third thread. 
	if((retVal = pthread_create(&new_th[2], NULL, third, NULL)) != 0)
	 	{
		INFO_PRINTF1(_L("Error in creating third thread \n"));
		INFO_PRINTF2(_L("the value returned is %d\n"),retVal);
	    }
	
	// Wait for threads to return 
	if((retVal = pthread_join(new_th[0], NULL)) != 0)//first thread returning
		{
	    INFO_PRINTF2(_L("Error in pthread_join() and the value returned is %d\n"),retVal);
	    }
	 
	if((retVal = pthread_join(new_th[1], NULL)) != 0)//second thread returning
		{
	    INFO_PRINTF2(_L("Error in pthread_join() and the value returned is %d\n"),retVal);
	  	}
	 
	if((retVal = pthread_join(new_th[2], NULL)) != 0)//third thread returning
		{
	    INFO_PRINTF2(_L("Error in pthread_join() and the value returned is %d\n"),retVal);
		}	 
	close(fd);
 	close(newfd);
 	fclose(fp);
	//All Test cases passed...
    return retVal;
	}
/*
 * the below test case is added to check the memory leak on navi engine in pthread_exit.
 
 */

static void* ThreadFunc(void*)
	{
	 return 0;
	}
TInt CTestPThread::PThreadExitMemLeak()
	{
		const char* const KThreadName = "THRD1";
		pthread_t threadId = 0;
		int err = 0;
		err = pthread_create(&threadId, 0, &ThreadFunc, (void*)KThreadName);
		if( err  != 0 )
			{
			INFO_PRINTF2(_L("Error in pthread_create() and the value returned is %d\n"),err);
			}
		err = pthread_join(threadId, 0);
		if( err  != 0)
			{
			 INFO_PRINTF2(_L("Error in pthread_join() and the value returned is %d\n"),err);
			}
		return err;
	}
	
