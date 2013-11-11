// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <sqldb.h>				//TSqlResourceTester
#include "SqlResourceTest.h"	//TSqlResourceTestData
#include "SqlAssert.h"			//ESqlPanicInternalError
#include "SqlDbSession.h"		//RSqlDbSession
#include "SqlResourceTester.h"		//TSqlResourceTester

///////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////             TSqlResourceTestData                 /////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma BullseyeCoverage off

#ifdef _DEBUG

/**
Ensures that TSqlResourceTestData singleton is created and returns a pointer to the object.

The function has a meaningfull implementation only in _DEBUG mode.

@return A pointer to TSqlResourceTestData singleton. The return result might be NULL.
*/
TSqlResourceTestData* TSqlResourceTestData::Instance()
	{
	TSqlResourceTestData* instance = static_cast <TSqlResourceTestData*> (Dll::Tls());
	if(!instance)
		{
		instance = new TSqlResourceTestData;
		if(instance)
			{
			if(Dll::SetTls(instance) != KErrNone)
				{
				delete instance;	
				instance = NULL;
				}
			}
		}
	return instance;
	}

/**
Destroys TSqlResourceTestData singleton.

The function has a meaningfull implementation only in _DEBUG mode.
*/
void TSqlResourceTestData::Release()
	{
	TSqlResourceTestData* instance = static_cast <TSqlResourceTestData*> (Dll::Tls());
	delete instance;
	(void)Dll::SetTls(NULL);
	}
		
/**
Initializes TSqlResourceTestData singleton with a reference to RSqlDbSession instance.
If a test data has been previously set by calling TSqlResourceTestData::Set(), then
the test data will be sent now to the SQL server.

The function has a meaningfull implementation only in _DEBUG mode.

@param aDbSession A reference to RDbSession instance.

@return KErrNone The call completed successfully, system-wide error code otherwise.
*/
TInt TSqlResourceTestData::Init(RSqlDbSession& aDbSession)
	{
	iDbSession = &aDbSession;
	TInt rc = KErrNone;
	if(iRqPending)
		{
		rc = iDbSession->SendReceive(iFunction, TIpcArgs(iAllocFailType, iRate));
		}
	iRqPending = EFalse;
	return rc;
	}
	
/**
If the TSqlResourceTestData singleton is already initialized (TSqlResourceTestData::Init() call),
then the test command and data will be sent directly to the SQL server. Othwerwise the test command
and data will be stored and sent later when the TSqlResourceTestData singleton gets initialized.

The function has a meaningfull implementation only in _DEBUG mode.

@param aFunction Test command to be sent to the SQL server
@param aAllocFailType Heap failure allocation type
@param arate Heap failure rate

@return KErrNone The call completed successfully, system-wide error code otherwise.
*/
TInt TSqlResourceTestData::Set(TSqlSrvFunction aFunction, TInt aAllocFailType, TInt aRate)
	{
	__ASSERT_DEBUG(!iRqPending, __SQLPANIC(ESqlPanicMisuse));
	if(iDbSession)	
		{
		return iDbSession->SendReceive(aFunction, TIpcArgs(aAllocFailType, aRate));
		}
	else
		{
		iFunction = aFunction;
		iAllocFailType = aAllocFailType;
		iRate = aRate;
		iRqPending = ETrue;
		return KErrNone;
		}
	}

/**
The function has a meaningfull implementation only in _DEBUG mode.
*/
TSqlResourceTestData::TSqlResourceTestData() :
	iRqPending(EFalse),
	iDbSession(NULL),
	iFunction(ESqlSrvTestBase),
	iAllocFailType(RHeap::ENone),
	iRate(0)
	{
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////             TSqlResourceTester                 ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

//Sends a test command to the SQL server.
//aFunction parameter is the test command code to be sent, 
//aAllocFailType is the heap failure type, aRate is the heap failure rate.
//
//The function will get a pointer to the TSqlResourceTestData instance and call its Set() method to
//send the test command and data.
static TInt SendCommand(TSqlSrvFunction aFunction, TInt aAllocFailType, TInt aRate)
	{
	TInt rc = KErrNoMemory;
	TSqlResourceTestData* instance = TSqlResourceTestData::Instance();
	if(instance)
		{
		rc = instance->Set(aFunction, aAllocFailType, aRate);
		}
	return rc;
	}

//Sends a test command to the SQL server.
//aFunction parameter is the test command code to be sent, 
//
//The function will get a pointer to the TSqlResourceTestData instance and call its Set() method to
//send the test command and data.
static TInt SendCommand(TSqlSrvFunction aFunction)
	{
	return SendCommand(aFunction, RHeap::ENone, 0);
	}

/**
Sends a request to the SQL server to mark the allocated resources.

The function has a meaningfull implementation only in _DEBUG mode.
*/
EXPORT_C void TSqlResourceTester::Mark()
	{
	(void)::SendCommand(ESqlSrvResourceMark);
	}
	
/**
Sends a request to the SQL server to check the allocated resources.
(to compare their count with the marked resource count, made by the 
 TSqlResourceTester::Mark() call)

The function has a meaningfull implementation only in _DEBUG mode.
*/
EXPORT_C void TSqlResourceTester::Check()
	{
	(void)::SendCommand(ESqlSrvResourceCheck);
	}
	
/**
@return Count of the allocated SQL server resources.

The function has a meaningfull implementation only in _DEBUG mode.
*/
EXPORT_C TInt TSqlResourceTester::Count()
	{
	return ::SendCommand(ESqlSrvResourceCount);
	}

/**
Sends a request to the SQL server to simulate out of memory failure.
This call can be used to test the server side of RSqlDatabase class.

The function has a meaningfull implementation only in _DEBUG mode.

@param aAllocFailType Heap failure allocation type
	   	If bit 12 of aAllocFailType is set, then the SQL server will delay
	   	the heap failure simulation until the database is opened.
@param arate Heap failure rate
*/
EXPORT_C void TSqlResourceTester::SetDbHeapFailure(TInt aAllocFailType, TInt aRate)
	{
	if(static_cast <RHeap::TAllocFail> (aAllocFailType) == RHeap::ENone)
		{
		//This is a command to reset the memory allocation failure simulation.
		//Execute it only if there is a valid TSqlResourceTestData instance.
		//Otherwise this function will try to allocate memory for TSqlResourceTestData instance
		//and this happens at the moment when the request is for stopping the simulation (so the OOM test).
		//The test application will find one more memory cell is allocated and will fail.
		TSqlResourceTestData* instance = static_cast <TSqlResourceTestData*> (Dll::Tls());
		if(!instance)
			{
			return;	
			}
		}
	(void)::SendCommand(ESqlSrvSetDbHeapFailure, aAllocFailType, aRate);
	}
	
/**
Sends a request to the SQL server to simulate out of memory failure.
This call can be used to test the server side of RSqlStatement class.

The function has a meaningfull implementation only in _DEBUG mode.

@param aAllocFailType Heap failure allocation type
@param arate Heap failure rate
*/
EXPORT_C void TSqlResourceTester::SetHeapFailure(TInt aAllocFailType, TInt aRate)
	{
	if(static_cast <RHeap::TAllocFail> (aAllocFailType) == RHeap::ENone)
		{
		//This is a command to reset the memory allocation failure simulation.
		//Execute it only if there is a valid TSqlResourceTestData instance.
		//Otherwise this function will try to allocate memory for TSqlResourceTestData instance
		//and this happens at the moment when the request is for stopping the simulation (so the OOM test).
		//The test application will find one more memory cell is allocated and will fail.
		TSqlResourceTestData* instance = static_cast <TSqlResourceTestData*> (Dll::Tls());
		if(!instance)
			{
			return;	
			}
		}
	(void)::SendCommand(ESqlSrvSetHeapFailure, aAllocFailType, aRate);
	}

#else //_DEBUG

void TSqlResourceTestData::Release()
	{
	}

EXPORT_C void TSqlResourceTester::Mark()
	{
	}

EXPORT_C void TSqlResourceTester::Check()
	{
	}

EXPORT_C TInt TSqlResourceTester::Count()
	{
	return 0;
	}

EXPORT_C void TSqlResourceTester::SetDbHeapFailure(TInt, TInt)
	{
	}

EXPORT_C void TSqlResourceTester::SetHeapFailure(TInt, TInt)
	{
	}

#endif//_DEBUG

#pragma BullseyeCoverage on
