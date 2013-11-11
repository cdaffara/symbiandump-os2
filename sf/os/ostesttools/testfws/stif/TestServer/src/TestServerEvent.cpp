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
* Description: <This module contains the implementation of xxx 
* class member functions...>
*
*/

// INCLUDE FILES
#include <e32svr.h>
#include "TestServerEvent.h"
#include "TestThreadContainer.h"
#include "TestServerModuleIf.h"
#include "Logging.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: TEvent

    Method: TEvent

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.
    
    <description of the functionality 
    description continues and...
    continues.>

    Parameters:	<arg1>: <in/out/inout>: <accepted values>: <Description>
    
    Return Values: <value_1: Description
		            value_n: Description line 1
		                     description line 2>

    Errors/Exceptions: <description how errors and exceptions are handled>

    Status: Draft
	
-------------------------------------------------------------------------------
*/
TEvent::TEvent( TThreadId aTestThread ) :
    TEventIf(),
	iReq(NULL),
	iThread( aTestThread ),
	iEventPending(EFalse),
	iExecution( NULL )
	{
    
    }

/*
-------------------------------------------------------------------------------

    Class: TEvent

    Method: ~TEvent

    Description: Destructor
    
    <description of the functionality 
    description continues and...
    continues.>

    Parameters:	<arg1>: <in/out/inout>: <accepted values>: <Description>
    
    Return Values: <value_1: Description
		            value_n: Description line 1
		                     description line 2>

    Errors/Exceptions: <description how errors and exceptions are handled>

    Status: Draft
	
-------------------------------------------------------------------------------
*/    
TEvent::~TEvent()
    {
    // Removed this call, to be studied @js
    //Release(); //safety
    }


/*
-------------------------------------------------------------------------------

    Class: TEvent

    Method: Set

    Description: Set event.

    Parameters:	<arg1>: <in/out/inout>: <accepted values>: <Description>
    
    Return Values: <value_1: Description
		            value_n: Description line 1
			                 description line 2>

    Errors/Exceptions: <description how errors and exceptions are handled>

    Status: Draft
	
-------------------------------------------------------------------------------
*/
void TEvent::Set( TEventType aEventType )
    {
   	iEventPending = ETrue; 
    iEventType = aEventType;
    
   	if( iReq )
   		{ 
   		CompletePendingWait( KErrNone );  
  		if( iEventType == EIndication )
			iEventPending = EFalse;    		
    	} 
    }

/*
-------------------------------------------------------------------------------

    Class: TEvent

    Method: EventPending

    Description: Check event status.

    Parameters:	<arg1>: <in/out/inout>: <accepted values>: <Description>
    
    Return Values: <value_1: Description
		            value_n: Description line 1
			                 description line 2>

    Errors/Exceptions: <description how errors and exceptions are handled>

    Status: Draft
	
-------------------------------------------------------------------------------
*/
TBool TEvent::EventPending()
	{
	if( iEventPending )
		{
		// Indication events are signaled only once
		if( iEventType == EIndication )
			iEventPending = EFalse;
		return ETrue;
		}
	return EFalse;
	}        

/*
-------------------------------------------------------------------------------

    Class: TEvent

    Method: WaitEventPending

    Description: Check waiting status.

    Parameters:	<arg1>: <in/out/inout>: <accepted values>: <Description>
    
    Return Values: <value_1: Description
		            value_n: Description line 1
			                 description line 2>

    Errors/Exceptions: <description how errors and exceptions are handled>

    Status: Draft
	
-------------------------------------------------------------------------------
*/
TBool TEvent::WaitEventPending()
	{
	if( iReq )
	    {
	    return ETrue;
	    }
	else
	    {
	    return EFalse;
	    }
	}        

/*
-------------------------------------------------------------------------------

    Class: TEvent

    Method: SetWaitPending

    Description: First phase of waiting.

    Parameters:	<arg1>: <in/out/inout>: <accepted values>: <Description>
    
    Return Values: <value_1: Description
		            value_n: Description line 1
			                 description line 2>

    Errors/Exceptions: <description how errors and exceptions are handled>

    Status: Draft
	
-------------------------------------------------------------------------------
*/
TInt TEvent::SetWaitPending( TRequestStatus* aStatus )
	{	       
	if( aStatus )
	    {
	    iReq = aStatus;
	    }
	else
	    {
	    iReq = &iRequestStatus;
	    }
    __RDEBUG((_L("Set wait pending(%d) %x"), this, iReq ));

    return KErrNone;
	}

/*
-------------------------------------------------------------------------------

    Class: TEvent

    Method: CompletePendingWait

    Description: Complete pending wait.

    Parameters:	<arg1>: <in/out/inout>: <accepted values>: <Description>
    
    Return Values: <value_1: Description
		            value_n: Description line 1
			                 description line 2>

    Errors/Exceptions: <description how errors and exceptions are handled>

    Status: Draft
	
-------------------------------------------------------------------------------
*/
void TEvent::CompletePendingWait( TInt aError )
	{	       
	_LIT( KTEvent, "TEvent" );
	__ASSERT_ALWAYS( iReq, User::Panic( KTEvent, KErrGeneral ) );
	
	RThread rt;
    if( rt.Open( iThread ) == KErrNone )
        {
        __RDEBUG( (_L("Complete wait(%d) %x"), this, iReq ) );
        rt.RequestComplete( iReq, aError );
	    } 
    rt.Close();
    iReq = NULL;
    
	}
	
/*
-------------------------------------------------------------------------------

    Class: TEvent

    Method: Wait

    Description: Wait event.
    Parameters:	<arg1>: <in/out/inout>: <accepted values>: <Description>
    
    Return Values: <value_1: Description
		            value_n: Description line 1
			                 description line 2>

    Errors/Exceptions: <description how errors and exceptions are handled>

    Status: Draft
	
-------------------------------------------------------------------------------
*/
void TEvent::Wait()
    { 

    User::WaitForRequest( iRequestStatus );
    // else Wait was already completed from other thread
    EventPending();
    } 

/*
-------------------------------------------------------------------------------

    Class: TEvent

    Method: Release

    Description: Release event. Completes conceivably pending unset.

    Parameters:	<arg1>: <in/out/inout>: <accepted values>: <Description>
    
    Return Values: <value_1: Description
		            value_n: Description line 1
			                 description line 2>

    Errors/Exceptions: <description how errors and exceptions are handled>

    Status: Draft
	
-------------------------------------------------------------------------------
*/
void TEvent::Release()
    { 
    if( iReq )
   		{
   		// Wait is pending and we received Release, 
   		// so we complete event with Cancel   
   		CompletePendingWait( KErrCancel );
   		iEventPending = ETrue;
    	} 
    	
	if( iExecution )
		{

		iExecution->UnsetEvent( *this, iUnsetMessage );
		iExecution = NULL;
		
		}
		
    } 

/*
-------------------------------------------------------------------------------

    Class: TEvent

    Method: Unset

    Description: Unset event. Unset blocks until Release is called 
    			(i.e. iUnsetMessage is compeled from release).

    Parameters:	<arg1>: <in/out/inout>: <accepted values>: <Description>
    
    Return Values: <value_1: Description
		            value_n: Description line 1
			                 description line 2>

    Errors/Exceptions: <description how errors and exceptions are handled>

    Status: Draft
	
-------------------------------------------------------------------------------
*/
TInt TEvent::Unset( const RMessage2& aMessage, CTestExecution* aExecution )
    { 

    if( iExecution )
        {
    	return KErrAlreadyExists;
    	}

    if( aExecution == NULL )
        {
    	return KErrArgument;
    	}
    
    iExecution = aExecution;	
    iUnsetMessage = aMessage;
    
    return KErrNone;
    
    } 

// End of File
