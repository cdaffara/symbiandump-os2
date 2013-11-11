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
* Description: This module contains the implementation of 
* CTestthreadContainerRunnerFactory class member functions.
*
*/

// INCLUDE FILES
#include <stifinternal/TestThreadContainerRunnerFactory.h>
#include "TestThreadContainerRunner.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None


// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunnerFactory

     Method: NewL

     Description: NewL is first phase of two-phased constructor.

	 NewL is first phase of two-phased constructor.

     Parameters: TThreadId aThreadId: in: main thread id.
     			 CActiveScheduler* aScheduler: in: Pointer to main thread active scheduler.
     			 CUiEnvProxy* aUiEnvProxy: in: Pointer to UIEnvProxy.
     
     Return Values: Pointer to new CTestServerThreadStarter object.

     Errors/Exceptions: Leaves if new or ConstructL leaves.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
EXPORT_C CTestThreadContainerRunnerFactory* 
CTestThreadContainerRunnerFactory::NewL( TThreadId aThreadId, CActiveScheduler* aScheduler, CUiEnvProxy* aUiEnvProxy )
	{
	CTestThreadContainerRunnerFactory* self = 
		new(ELeave)CTestThreadContainerRunnerFactory();
	CleanupStack::PushL( self );
	self->ConstructL( aThreadId, aScheduler, aUiEnvProxy );
	CleanupStack::Pop( self );
	return self;
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunnerFactory

     Method: CTestThreadContainerRunnerFactory

     Description: Default constructor.

	 Default constructor.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CTestThreadContainerRunnerFactory::CTestThreadContainerRunnerFactory()
:CActive( EPriorityNormal ),
iTestThreadContainerRunner( NULL ),
iOperation( ENone )
	{
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunnerFactory

     Method: ~CTestThreadContainerRunnerFactory

     Description: Default destructor.

	 Default destructor.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CTestThreadContainerRunnerFactory::~CTestThreadContainerRunnerFactory()
	{
	Cancel();
	iOperationEndSemaphore.Close();
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunnerFactory

     Method: ConstructL

     Description: Second phase of two-phased constructor.

	 Second phase of two-phased constructor.

     Parameters: TThreadId aThreadId: in: main thread id.
     			 CActiveScheduler* aScheduler: in: Pointer to main thread active scheduler.
     			 CUiEnvProxy* aUiEnvProxy: in: Pointer to UIEnvProxy.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainerRunnerFactory::ConstructL( TThreadId aThreadId, CActiveScheduler* aScheduler, CUiEnvProxy* aUiEnvProxy )
	{
	User::LeaveIfNull( aScheduler );
	iThreadId = aThreadId;
	iScheduler = aScheduler;
	iUiEnvProxy = aUiEnvProxy;
	
	TInt ret = iOperationEndSemaphore.CreateLocal( 0 );
	if ( ret != KErrNone )
		{
		User::Leave( ret );
		}
	
	iScheduler->Add( this );
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunnerFactory

     Method: DeleteL

     Description: Performs creation of CTestThreadContainerRunner object.

	 Performs creation of CTestThreadContainerRunner object.

     Parameters: CTestThreadContainerRunner* aTestThreadContainerRunner: in: Pointer to 
     				CTestThreadContainerRunner object which needs to be deleted.
     
     Return Values: None.

     Errors/Exceptions: Leaves if one of the called method leavs.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CTestThreadContainerRunner* CTestThreadContainerRunnerFactory::CreateL()
	{
	if ( IsActive() )
		{
		User::Panic( _L( "E32USER-CBase" ), 42 );
		}

	iOperation = ECreate;
	iStatus = KRequestPending;
	
	RThread thread;
	TInt ret = thread.Open( iThreadId );
	if ( ret != KErrNone )
		{
		User::Leave( ret );
		}
	
	SetActive();
	TRequestStatus* statusPtr = &iStatus;
	thread.RequestComplete( statusPtr, KErrNone );
	thread.Close();
	iOperationEndSemaphore.Wait();

	return iTestThreadContainerRunner;
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunnerFactory

     Method: DeleteL

     Description: Performs deletion of CTestThreadContainerRunner object.

	 Performs deletion of CTestThreadContainerRunner object.

     Parameters: CTestThreadContainerRunner* aTestThreadContainerRunner: in: Pointer to 
     				CTestThreadContainerRunner object which needs to be deleted.
     
     Return Values: None.

     Errors/Exceptions: Leaves if one of the called method leavs.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainerRunnerFactory::DeleteL( CTestThreadContainerRunner* aTestThreadContainerRunner )
	{
	if ( IsActive() )
		{
		User::Panic( _L( "E32USER-CBase" ), 42 );
		}
	
	iOperation = EDelete;
	iStatus = KRequestPending;
	
	RThread thread;
	TInt ret = thread.Open( iThreadId );
	if ( ret != KErrNone )
		{
		User::Leave( ret );
		}
	
	iTestThreadContainerRunner = aTestThreadContainerRunner;
	SetActive();
	TRequestStatus* statusPtr = &iStatus;
	thread.RequestComplete( statusPtr, KErrNone );
	thread.Close();
	iOperationEndSemaphore.Wait();
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunnerFactory

     Method: RunL

     Description: RunL derived from CActive handles the completed requests.

	 RunL derived from CActive handles the completed requests.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: Leaves if one of the called method leavs.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainerRunnerFactory::RunL()
	{
	switch( iOperation )
		{
		case ECreate:
			{
			iTestThreadContainerRunner = CTestThreadContainerRunner::NewL( iThreadId, iScheduler );
			}
			break;
		case EDelete:
			{
			delete iTestThreadContainerRunner;
			iTestThreadContainerRunner = NULL;
			}
			break;
		default:
			{
			User::Panic( _L("STIFInternalError"), KErrNotFound );
			}			
			break;
		}
	iOperation = ENone;
	iOperationEndSemaphore.Signal();
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunnerFactory

     Method: DoCancel

     Description: DoCancel derived from CActive handles the Cancel.

	 DoCancel derived from CActive handles the Cancel.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainerRunnerFactory::DoCancel()
	{	
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunnerFactory

     Method: GetUiEnvProxy

     Description: Gets UIEnvProxy.

     Parameters: None.
     
     Return Values: Pointer to UIEnvProxy.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CUiEnvProxy* CTestThreadContainerRunnerFactory::GetUiEnvProxy()
	{
	return iUiEnvProxy;
	}

// End of File
