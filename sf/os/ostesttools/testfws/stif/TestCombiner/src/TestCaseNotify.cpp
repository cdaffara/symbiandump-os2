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
* Description: This module contains implementation of 
* CTestProgressNotifier class member functions.
*
*/

// INCLUDE FILES
#include "TestCombiner.h"
#include "TestCase.h"
#include "TestCaseNotify.h"
#include "Logging.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
#ifdef LOGGER
#undef LOGGER
#endif
#define LOGGER iTestCombiner->iLog

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================
// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CTestProgressNotifier

     Method: CTestProgressNotifier

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.
     
     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Approved
     
-------------------------------------------------------------------------------
*/
CTestProgressNotifier::CTestProgressNotifier( CTestCombiner* aTestCombiner, 
                                              CTCTestCase* aTestCase ) : 
    CActive( CActive::EPriorityStandard ),
    iTestCombiner( aTestCombiner ),
    iTestCase( aTestCase ),
    iState( ETestProgressIdle ),
    iProgressPckg( iProgress ),
    iIsPrintProcessFinished( EFalse )
    {
    CActiveScheduler::Add( this );
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestProgressNotifier

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.

     Parameters: None

     Return Values: None

     Errors/Exceptions: Leaves if StartL leaves
     
     Status: Approved
     
-------------------------------------------------------------------------------
*/
void CTestProgressNotifier::ConstructL()
    {
    StartL();

    }

/*
-------------------------------------------------------------------------------

     Class: CTestProgressNotifier

     Method: NewL

     Description: Two-phased constructor.
     
     Parameters: CTestCombiner* aTestCombiner: in: Backpointer to TestCombiner
                 CTCTestCase* aTestCase: in: Pointer to used CTCTestCase
     
     Return Values: CTestProgressNotifier* : pointer to created object

     Errors/Exceptions: Leaves if ConstructL leaves or memory allocation fails

     Status: Approved
     
-------------------------------------------------------------------------------
*/
CTestProgressNotifier* CTestProgressNotifier::NewL( CTestCombiner* aTestCombiner, 
                                                    CTCTestCase* aTestCase )
    {
    CTestProgressNotifier* self = 
        new ( ELeave ) CTestProgressNotifier( aTestCombiner, aTestCase );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestProgressNotifier

     Method: ~CTestProgressNotifier

     Description: Destructor
     
     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Approved
     
-------------------------------------------------------------------------------
*/     
CTestProgressNotifier::~CTestProgressNotifier()
    {
    __TRACE( KMessage, ( _L("~CTestProgressNotifier") ));
    Cancel();
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestProgressNotifier

     Method: StartL

     Description: Start ProgressNotifier

     Parameters: None

     Return Values: None

     Errors/Exceptions: Leaves if allready pending

     Status: Approved
     
-------------------------------------------------------------------------------
*/
void CTestProgressNotifier::StartL()
    {
    // Check that this request is not pending!!
    if( iState == ETestProgressPending )
        {
        User::Leave( KErrGeneral );
        }
        
    iState = ETestProgressPending;
    iTestCase->TestExecution().NotifyProgress( iProgressPckg, iStatus );
    SetActive();

    } 
     
/*
-------------------------------------------------------------------------------

     Class: CTestProgressNotifier

     Method: RunL

     Description: RunL handles completed requests.

     Parameters: None

     Return Values: None

     Errors/Exceptions: None

     Status: Approved
     
-------------------------------------------------------------------------------
*/
void CTestProgressNotifier::RunL()
    {

    iState = ETestProgressCompleted;

    // Check that request was successful or completed with acceptable error
    // KErrEof is acceptable error and it means that the test case will not
    // send progresses anymore (because it is closed)
    if ( KErrNone == iStatus.Int() )
        {
        TStifInfoName desc( iProgress.iDescription );

        // Append testid if possible
        if( ( iTestCase->TestId().Length() > 0 ) &&
            ( desc.MaxLength() > 
            ( desc.Length() + iTestCase->TestId().Length() + 1 ) ) )
            {
            desc.Append( _L("_") );
            desc.Append( iTestCase->TestId() );
            }
          // Append module name if possible
        else if( desc.MaxLength() > 
            ( desc.Length() + iTestCase->ModuleName().Length() + 1 ) )
            {
            desc.Append( _L("_") );
            desc.Append( iTestCase->ModuleName() );
            }

        // Forward printing to engine
        iTestCombiner->TestModuleIf().Printf( iProgress.iPosition, 
                                                desc, 
                                                iProgress.iText );
          
        // Set request pending again
        StartL();
        }
    else if ( KErrEof != iStatus.Int() )
        {
        // Leave, error will be handled in RunError
        User::Leave( iStatus.Int() );
        }
	
	if( KErrEof == iStatus.Int()  )
		{
        // KErrEof indicates that all print process to UI is completed.
        iIsPrintProcessFinished = ETrue;
		if(iTestCase->State() == CTestCase::ETestCaseCompleted )
			{
            // Test case is completed so do final complete.
			iTestCase->Complete2();	
			}
		
		}
        
    }


/*
-------------------------------------------------------------------------------

     Class: CTestProgressNotifier

     Method: DoCancel

     Description: Cancel active request

     Parameters: None

     Return Values: None

     Errors/Exceptions: None

     Status: Approved
     
-------------------------------------------------------------------------------
*/
void CTestProgressNotifier::DoCancel()
    {
    switch ( iState )
        {
        case ETestProgressPending:
            iTestCase->TestExecution().CancelAsyncRequest( ETestExecutionNotifyProgress );
            break;
        case ETestProgressIdle:
        case ETestProgressCompleted:
        default:
            // DoCancel called in wrong state => Panic
            _LIT( KTestProgressNotifier, "CTestProgressNotifier" );
            User::Panic( KTestProgressNotifier, KErrGeneral );
            break;
        }
    iState = ETestProgressIdle;

    }


/*
-------------------------------------------------------------------------------

     Class: CTestProgressNotifier

     Method: RunError

     Description: Handle errors. Should newer come here so we return error.

     Parameters: TInt aError: in: Symbian OS error: Error code
     
     Return Values: Symbian OS error value

     Errors/Exceptions: None

     Status: Approved
     
-------------------------------------------------------------------------------
*/
TInt CTestProgressNotifier::RunError( TInt aError )
    {
    return aError;
     
    }

/*
-------------------------------------------------------------------------------

     Class: CTestProgressNotifier

     Method: IsPrintProgressFinished

     Description: Indicates is print process to UI finished

     Parameters: None
     
     Return Values: TBool: ETrue indicates that print process is finished,
     					   EFalse indicates that print process is not finished

     Errors/Exceptions: None

     Status: Proposal
     
-------------------------------------------------------------------------------
*/
TBool CTestProgressNotifier::IsPrintProgressFinished()
    {
    return iIsPrintProcessFinished;

    }

/*
-------------------------------------------------------------------------------

     DESCRIPTION

     This module contains implementation of CTestErrorNotifier class member
     functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CTestErrorNotifier

     Method: CTestErrorNotifier

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.
     
     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Approved
     
-------------------------------------------------------------------------------
*/
CTestErrorNotifier::CTestErrorNotifier( CTestCombiner* aTestCombiner, 
                                        CTCTestModule* aTestModule ) : 
    CActive( CActive::EPriorityStandard ),
    iTestCombiner( aTestCombiner ),
    iTestModule( aTestModule ),
    iState( ETestErrorIdle ),
    iErrorPckg( iError )
    {
    CActiveScheduler::Add( this );
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestErrorNotifier

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.

     Parameters: None

     Return Values: None

     Errors/Exceptions: Leaves if StartL leaves
     
     Status: Approved
     
-------------------------------------------------------------------------------
*/
void CTestErrorNotifier::ConstructL()
    {
    StartL();

    }

/*
-------------------------------------------------------------------------------

     Class: CTestErrorNotifier

     Method: NewL

     Description: Two-phased constructor.
     
     Parameters: CTestCombiner* aTestCombiner: in: Backpointer to TestCombiner
                 CTCTestModule* aTestModule: in: Pointer to used CTCTestModule
     
     Return Values: CTestErrorNotifier* : pointer to created object

     Errors/Exceptions: Leaves if ConstructL leaves or memory allocation fails

     Status: Approved
     
-------------------------------------------------------------------------------
*/
CTestErrorNotifier* CTestErrorNotifier::NewL( CTestCombiner* aTestCombiner, 
                                              CTCTestModule* aTestModule )
    {
    CTestErrorNotifier* self = 
        new ( ELeave ) CTestErrorNotifier( aTestCombiner, aTestModule );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestErrorNotifier

     Method: ~CTestErrorNotifier

     Description: Destructor
     
     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Approved
     
-------------------------------------------------------------------------------
*/     
CTestErrorNotifier::~CTestErrorNotifier()
    {
    __TRACE( KMessage, ( _L("~CTestErrorNotifier") ));
    Cancel();
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestErrorNotifier

     Method: StartL

     Description: Start ProgressNotifier

     Parameters: None

     Return Values: None

     Errors/Exceptions: Leaves if allready pending

     Status: Approved
     
-------------------------------------------------------------------------------
*/
void CTestErrorNotifier::StartL()
    {
    // Check that this request is not pending!!
    if( iState == ETestErrorPending )
        {
        User::Leave( KErrGeneral );
        }
        
    iState = ETestErrorPending;
    iTestModule->TestModule().ErrorNotification( iErrorPckg, iStatus );
    SetActive();

    } 
     
/*
-------------------------------------------------------------------------------

     Class: CTestErrorNotifier

     Method: RunL

     Description: RunL handles completed requests.

     Parameters: None

     Return Values: None

     Errors/Exceptions: None

     Status: Approved
     
-------------------------------------------------------------------------------
*/
void CTestErrorNotifier::RunL()
    {

    iState = ETestErrorCompleted;

    // Check that request was successful or completed with acceptable error
    // KErrEof is acceptable error and it means that the test case will not
    // send progresses anymore (because it is closed)
    if ( KErrNone == iStatus.Int() )
        {
              
        // Forward printing to engine
        iTestCombiner->TestModuleIf().Printf( iError.iPriority, 
                                              iError.iModule, 
                                              iError.iText );
          
        // Set request pending again
        StartL();
        }
    else if ( KErrEof != iStatus.Int() )
        {
        // Leave, error will be handled in RunError
        User::Leave( iStatus.Int() );
        }
          
    }


/*
-------------------------------------------------------------------------------

     Class: CTestErrorNotifier

     Method: DoCancel

     Description: Cancel active request

     Parameters: None

     Return Values: None

     Errors/Exceptions: None

     Status: Approved
     
-------------------------------------------------------------------------------
*/
void CTestErrorNotifier::DoCancel()
    {
    switch ( iState )
        {
        case ETestErrorPending:
            iTestModule->TestModule().CancelAsyncRequest( ETestModuleErrorNotification );
            break;
        case ETestErrorIdle:
        case ETestErrorCompleted:
        default:
            // DoCancel called in wrong state => Panic
            _LIT( KTestErrorNotifier, "CTestErrorNotifier" );
            User::Panic( KTestErrorNotifier, KErrGeneral );
            break;
        }
    iState = ETestErrorIdle;

    }


/*
-------------------------------------------------------------------------------

     Class: CTestErrorNotifier

     Method: RunError

     Description: Handle errors. Should newer come here so we return error.

     Parameters: TInt aError: in: Symbian OS error: Error code
     
     Return Values: Symbian OS error value

     Errors/Exceptions: None

     Status: Approved
     
-------------------------------------------------------------------------------
*/
TInt CTestErrorNotifier::RunError( TInt /*aError*/ )
    {
    
    // Ignore error, stop error note forwarding
    return KErrNone;
     
    }
     
     
/*
-------------------------------------------------------------------------------

     DESCRIPTION

     This module contains implementation of CTestEventNotify class member
     functions. Handles requested events from test modules below.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================


/*
-------------------------------------------------------------------------------

     Class: CTestEventNotify

     Method: CTestEventNotify

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.
     
     Parameters: CTestCombiner* aTestCombiner: in: Backpointer to TestCombiner
                 CTCTestCase* aTestCase: in: Pointer to used CTCTestCase
                 
     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
     
-------------------------------------------------------------------------------
*/
CTestEventNotify::CTestEventNotify( CTestCombiner* aTestCombiner, 
                                    CTCTestCase* aTestCase ) : 
    CActive( CActive::EPriorityStandard ),
    iTestCombiner( aTestCombiner ),
    iTestCase( aTestCase ),
    iState( ETestEventNotifyIdle ),
    iEventPckg( iEvent )
    {
    CActiveScheduler::Add( this );
     
    }

/*
-------------------------------------------------------------------------------

     Class: CTestEventNotify

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.
     
     Parameters: None

     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
     
-------------------------------------------------------------------------------
*/
void CTestEventNotify::ConstructL( TEventIf& aEvent )
    {
    iEvent.Copy( aEvent );
    
    iEvent.SetType( TEventIf::EWaitEvent );
    iState = ETestEventNotifyPending;
    iTestCombiner->TestModuleIf().Event( iEvent, iStatus );
    SetActive();
     
    }

/*
-------------------------------------------------------------------------------

     Class: CTestEventNotify

     Method: NewL

     Description: Two-phased constructor.
     
     Parameters: CTestCombiner* aTestCombiner: in: Backpointer to TestCombiner
                 CTCTestCase* aTestCase: in: Pointer to used CTCTestCase
     
     Return Values: CTestEventNotify* : pointer to created object

     Errors/Exceptions: Leaves if ConstructL leaves or memory allocation fails
     
     Status: Proposal
     
-------------------------------------------------------------------------------
*/
CTestEventNotify* CTestEventNotify::NewL( CTestCombiner* aTestCombiner, 
                                          CTCTestCase* aTestCase,
                                          TEventIf& aEvent )
    {
    CTestEventNotify* self = 
        new ( ELeave ) CTestEventNotify( aTestCombiner, aTestCase );
    CleanupStack::PushL( self );
    self->ConstructL( aEvent );
    CleanupStack::Pop( self );
    return self;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestEventNotify

     Method: ~CTestEventNotify

     Description: Destructor
     
     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
     
-------------------------------------------------------------------------------
*/     
CTestEventNotify::~CTestEventNotify()
    {
    __TRACE( KMessage, ( _L("~CTestEventNotify") ));
     
    Cancel();
        
    }

/*
-------------------------------------------------------------------------------

     Class: CTestEventNotify

     Method: RunL

     Description: Handles completed event wait

     Parameters: None

     Return Values: None

     Errors/Exceptions: Leaves on error.

     Status: Proposal
     
-------------------------------------------------------------------------------
*/

void CTestEventNotify::RunL()
    {
    __TRACE( KMessage, ( _L("CTestEventNotify::RunL(%d): %d, %d"), 
        this, iStatus.Int(), iState ) );
    User::LeaveIfError( iStatus.Int() );

    switch( iState )
        {
        case ETestEventNotifyPending:
            {     
            // Forward event set/unset
            iEvent.SetType( TEventIf::ESetEvent );
            iState = ETestEventNotifyCompleted;
            User::LeaveIfError(
                iTestCase->TestExecution().NotifyEvent( iEventPckg, iStatus ) );
            SetActive();
            }
            break;
        case ETestEventNotifyCompleted:
            // Enable wait again
            iEvent.SetType( TEventIf::ESetWaitPending );
            iState = ETestEventNotifyPending;
            iTestCombiner->TestModuleIf().Event( iEvent, iStatus );
            SetActive();
            break;
        default:          
            User::LeaveIfError( KErrGeneral );
            break;
        }
    __TRACE( KMessage, ( _L("CTestEventNotify::RunL(%d) done: %d"), 
        this, iState ) );

    }

/*
-------------------------------------------------------------------------------

     Class: CTestEventNotify

     Method: DoCancel

     Description: Handle Cancel

     Parameters: None

     Return Values: None

     Errors/Exceptions: None
     
     Status: Proposal
     
-------------------------------------------------------------------------------
*/

void CTestEventNotify::DoCancel()
    {
    __TRACE( KMessage, ( _L("CTestEventNotify::DoCancel")));
 
    _LIT( KTestEventNotify, "CTestEventNotify" );
    switch ( iState )
        {
        case ETestEventNotifyPending:
            {
            TInt ret = 
                iTestCombiner->TestModuleIf().CancelEvent( iEvent, &iStatus );
            if( ret != KErrNone )
                {
                __TRACE( KMessage, ( _L("CTestEventNotify::DoCancel(%d) not pending: %d"), 
                    this, ret ) );
                }
            }
            break;
        case ETestEventNotifyCompleted:            
            iTestCase->TestExecution().
                CancelAsyncRequest( ETestExecutionNotifyEvent );
            break;
        case ETestEventNotifyIdle:
        default:
            // DoCancel called in wrong state => Panic
            User::Panic( KTestEventNotify, KErrGeneral );
            break;
        }
     
    iState = ETestEventNotifyIdle;
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestEventNotify

     Method: RunError

     Description: Handle error. Should newer come here so we return error.

     Parameters: TInt aError: in: error from CActive
     
     Return Values: Symbian OS error value
     
     Errors/Exceptions: None
     
     Status: Proposal
     
-------------------------------------------------------------------------------
*/

TInt CTestEventNotify::RunError( TInt aError )
    {
    __TRACE( KMessage, ( _L("CTestEventNotify::RunError")));
 
    return aError;
    
    }
    
/*
-------------------------------------------------------------------------------

     DESCRIPTION

     This module contains implementation of CTestEventNotifier class member
     functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================


/*
-------------------------------------------------------------------------------

     Class: CTestEventNotifier

     Method: CTestEventNotifier

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.
     
     Parameters: CTestCombiner* aTestCombiner: in: Backpointer to TestCombiner
                 CTCTestCase* aTestCase: in: Pointer to used CTCTestCase
                 
     Return Values: None

     Errors/Exceptions: None

     Status: Approved
     
-------------------------------------------------------------------------------
*/
CTestEventNotifier::CTestEventNotifier( CTestCombiner* aTestCombiner, 
                                        CTCTestCase* aTestCase ) : 
    CActive( CActive::EPriorityStandard ),
    iTestCombiner( aTestCombiner ),
    iTestCase( aTestCase ),
    iState( ETestEventIdle ),
    iEventPckg( iEvent )
    {
    CActiveScheduler::Add( this );
     
    }

/*
-------------------------------------------------------------------------------

     Class: CTestEventNotifier

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.
     
     Parameters: None

     Return Values: None

     Errors/Exceptions: None

     Status: Approved
     
-------------------------------------------------------------------------------
*/
void CTestEventNotifier::ConstructL( )
    {
    StartL();
     
    }

/*
-------------------------------------------------------------------------------

     Class: CTestEventNotifier

     Method: NewL

     Description: Two-phased constructor.
     
     Parameters: CTestCombiner* aTestCombiner: in: Backpointer to TestCombiner
                 CTCTestCase* aTestCase: in: Pointer to used CTCTestCase
     
     Return Values: CTestEventNotifier* : pointer to created object

     Errors/Exceptions: Leaves if ConstructL leaves or memory allocation fails
     
     Status: Approved
     
-------------------------------------------------------------------------------
*/
CTestEventNotifier* CTestEventNotifier::NewL( CTestCombiner* aTestCombiner, 
                                              CTCTestCase* aTestCase  )
    {
    CTestEventNotifier* self = 
        new ( ELeave ) CTestEventNotifier( aTestCombiner, aTestCase );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestEventNotifier

     Method: ~CTestEventNotifier

     Description: Destructor
     
     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
     
-------------------------------------------------------------------------------
*/     
CTestEventNotifier::~CTestEventNotifier()
    {
    __TRACE( KMessage, ( _L("~CTestEventNotifier") ));
     
    
     
    Cancel();
     
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestEventNotifier

     Method: StartL

     Description: Start active object

     Parameters: None

     Return Values: None

     Errors/Exceptions: Leave if already pending

     Status: Proposal
     
-------------------------------------------------------------------------------
*/
void CTestEventNotifier::StartL()
    {
    // Check that this request is not pending!!
    if( iState == ETestEventPending )
        {
        User::Leave( KErrGeneral );
        }
        
    iEvent.SetType( TEventIf::EEnable );
    iState = ETestEventPending;
    __TRACE( KMessage, ( _L("Set event pending %d"), this ) );
    iTestCase->TestExecution().NotifyEvent( iEventPckg, iStatus );
    SetActive();
     
    }

/*
-------------------------------------------------------------------------------

     Class: CTestEventNotifier

     Method: RunL

     Description: Handles completed event request

     Parameters: None

     Return Values: None

     Errors/Exceptions: Leaves on error.

     Status: Proposal
     
-------------------------------------------------------------------------------
*/

void CTestEventNotifier::RunL()
    {
    __TRACE( KMessage, ( _L("CTestEventNotifier::RunL(%d): %d, %d"), 
        this, iStatus.Int(), iState ) );

    User::LeaveIfError( iStatus.Int() );
    
    switch( iState )
        {
        case ETestEventPending:
        case ETestEventWaitUnset:
            {  
            iState = ETestEventCompleted;
            
            switch( iEvent.Type() )
                {
                case TEventIf::ERelEvent:
                    {
                    // Remove from requested events list
                    TInt count = iTestCase->EventArray().Count();
                    TInt ind = 0;
                    for( ; ind < count; ind++)
                        { 
                          if( iTestCase->EventArray()[ind]->Event().Name() == iEvent.Name() )
                              {
                              __RDEBUG( ( _L("TC CTestEventNotifier(RelEvent) %S"), 
                                  &iEvent.Name() ));
                              CTestEventNotify* event = iTestCase->EventArray()[ind];
                              iTestCase->EventArray().Remove( ind );
                              delete event;
                              break;
                              }
                          }
                    if( ind == count )
                        {
                        User::Leave( KErrNotFound );
                        }
                    }
                    break; 
                case TEventIf::EUnsetEvent:
                    {
                    __RDEBUG( ( _L("TC CTestEventNotifier(UnsetEvent) %S"), 
                                  &iEvent.Name() ));
                    TInt count = iTestCase->StateEventArray().Count();
                    for( TInt i=0; i<count; i++ )
                        {
                        if( iTestCase->StateEventArray()[i]->Name() == iEvent.Name() )
                            {
                            TEventIf* event = iTestCase->StateEventArray()[i];
                            iTestCase->StateEventArray().Remove(i);
                            delete event;
                            break;
                            }
                        }
                                  
                    if( iTestCombiner->UnsetEvent( iEvent, iStatus ) )
                        {
                         // Event request is pending from modules below test 
                         // combiner, stay waiting
                        iState = ETestEventWaitUnset;
                        // Wait for unset to complete 
                        SetActive();
                        return;
                        }
                    }
                    break;
                case TEventIf::EReqEvent:
                    // Do nothing
                    __RDEBUG( ( _L("TC CTestEventNotifier(ReqEvent) %S"), 
                                  &iEvent.Name() ));
                    break;
                case TEventIf::ESetEvent: 
                    // Do nothing
                    __RDEBUG( ( _L("TC CTestEventNotifier(SetEvent) %S"), 
                                  &iEvent.Name() ));
                    break;
                default:
                    User::Leave( KErrArgument );
                    break;
                }   
                            
            // Forward event request
            iTestCombiner->TestModuleIf().Event( iEvent, iStatus );
            SetActive();
            
            }
            break;
        case ETestEventCompleted:
            __RDEBUG( ( _L("TC CTestEventNotifier(Complete)")));
            switch( iEvent.Type() )
                {
                case TEventIf::EReqEvent:
                    {
                    // Store requested events
                    CTestEventNotify* event = 
                        CTestEventNotify::NewL( iTestCombiner, iTestCase, iEvent );
                    CleanupStack::PushL( event ); 
                    User::LeaveIfError( iTestCase->EventArray().Append( event ) );
                    CleanupStack::Pop( event ); 
                    }
                    break;
                case TEventIf::ESetEvent: 
                    if( iEvent.EventType() == TEventIf::EState )
                        {
                        TEventIf* event = new( ELeave )TEventIf;
                        CleanupStack::PushL( event );
                        event->Copy( iEvent );
                        User::LeaveIfError( iTestCase->StateEventArray().Append( event ) ); 
                        CleanupStack::Pop( event );
                        }
                    break;
                case TEventIf::ERelEvent: 
                case TEventIf::EUnsetEvent:
                    // Do nothing
                    break;
                default:
                    User::Leave( KErrArgument );
                    break;
                }
       
            // Enable request again
            StartL();
            break;
        default:          
            User::LeaveIfError( KErrGeneral );
            break;
        }
    __TRACE( KMessage, ( _L("CTestEventNotifier::RunL(%d) done: %d"), 
        this, iState ) );
    }

/*
-------------------------------------------------------------------------------

     Class: CTestEventNotifier

     Method: DoCancel

     Description: Handle Cancel

     Parameters: None

     Return Values: None

     Errors/Exceptions: None
     
     Status: Proposal
     
-------------------------------------------------------------------------------
*/

void CTestEventNotifier::DoCancel()
    {
    __TRACE( KMessage, ( _L("CTestEventNotifier::DoCancel")));
    _LIT( KTestEventNotifier, "CTestEventNotifier" );
    
    switch ( iState )
        {
        case ETestEventPending:
            iTestCase->TestExecution().
                CancelAsyncRequest( ETestExecutionNotifyEvent );
            break;
        case ETestEventCompleted:            
            {
            TInt ret = iTestCombiner->TestModuleIf().CancelEvent( iEvent, &iStatus );
            if( ret != KErrNone )
                {
                __TRACE( KMessage, ( _L("CTestEventNotifier::DoCancel(%d) not pending: %d"), 
                    this, ret ) );
                }
            }
			User::WaitForRequest( iStatus );
            break;
        case ETestEventIdle:
        default:
            // DoCancel called in wrong state => Panic
            User::Panic( KTestEventNotifier, KErrGeneral );
            break;
        }
     
    iState = ETestEventIdle;
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestEventNotifier

     Method: RunError

     Description: Handle error. Should newer come here so we return error.

     Parameters: TInt aError: in: error from CActive
     
     Return Values: Symbian OS error value
     
     Errors/Exceptions: None
     
     Status: Approved
     
-------------------------------------------------------------------------------
*/

TInt CTestEventNotifier::RunError( TInt aError )
    {
    __TRACE( KMessage, ( _L("CTestEventNotifier::RunError")));
    
     switch( iState )
        {
        case ETestEventPending:
        case ETestEventWaitUnset:
            // Either the event request is cancelled or some 
            // unknown error occurred. We go idle state.
            iState = ETestEventIdle;
            break;
        
        case ETestEventCompleted:
            // Error in event command, forward it to the test module
            iEvent.SetType( TEventIf::EEnable );
            iState = ETestEventPending;
            __TRACE( KMessage, ( _L("Cmd error %d"), this ) );
            iTestCase->TestExecution().NotifyEvent( iEventPckg, iStatus, aError );
            SetActive();
            break;
        default:          
            __TRACE( KMessage, ( _L("Illegal state %d"), this ) );
            return aError;
        }
     
    return KErrNone;
    
    }
    
// ================= MEMBER FUNCTIONS =========================================
/*
-------------------------------------------------------------------------------

    Class: CTestCommandNotifier

    Method: CTestCommandNotifier

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: CTestCombiner* aTestCombiner: in: Pointer to TestCombiner
                CTCTestCase* aTestCase: in: Pointer to test case

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCommandNotifier::CTestCommandNotifier(CTestCombiner* aTestCombiner, 
                                           CTCTestCase* aTestCase):
    CActive(CActive::EPriorityStandard),
    iTestCombiner(aTestCombiner),
    iTestCase(aTestCase),
    iCommandPckg(iCommand)
    {
    CActiveScheduler::Add(this);
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCommandNotifier

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCommandNotifier::ConstructL( )
    {
    __TRACE(KMessage, (_L("CTestCommandNotifier::ConstructL (combiner)")));
    Start();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCommandNotifier

    Method: NewL

    Description: Two-phased constructor.

    Parameters: CTestCombiner* aTestCombiner: in: Pointer to TestCombiner
                CTCTestCase* aTestCase: in: Pointer to test case

    Return Values: CTestCommandNotifier* : pointer to created object

    Errors/Exceptions: Leaves if construction of CTestCommandNotifier fails

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCommandNotifier* CTestCommandNotifier::NewL(CTestCombiner* aTestCombiner, 
                                                 CTCTestCase* aTestCase)
    {
    CTestCommandNotifier* self = new (ELeave) CTestCommandNotifier(aTestCombiner, aTestCase);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCommandNotifier

    Method: ~CTestCommandNotifier

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCommandNotifier::~CTestCommandNotifier()
    {
    __TRACE(KMessage, (_L("CTestEventNotifier::~CTestEventNotifier (combiner)")));
    Cancel();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCommandNotifier

    Method: StartL

    Description: Start active object

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCommandNotifier::Start()
    {
    __TRACE(KMessage, (_L("CTestEventNotifier::StartL (combiner)")));

    iTestCase->TestExecution().NotifyCommand2(iCommandPckg, iParamsPckg, iStatus, KErrNone);
    SetActive();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCommandNotifier

    Method: RunL

    Description: RunL handles completed requests.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if iStatus is not KErrNone
                       Leaves if iState is not ETestEventPending
                       Leaves if some leaving method called here leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCommandNotifier::RunL()
    {
    __TRACE(KMessage, (_L("CTestCommandNotifier::RunL (combiner): iStatus=[%d]"), iStatus.Int()));

    User::LeaveIfError(iStatus.Int());

    switch(iCommand)
        {
        case EStopExecution:
            {
            __TRACE(KMessage, (_L("CTestCommandNotifier::RunL (combiner): StopExecution command received")));
            TStopExecutionCommandParams par;
            TStopExecutionCommandParamsPckg parPack(par);
            parPack.Copy(iParamsPckg);
            __TRACE(KMessage, (_L("CTestCommandNotifier::RunL(combiner): propagating stop execution [%d][%d]"), par.iType, par.iCode));
            iTestCombiner->TestModuleIf().StopExecution(par.iType, par.iCode);
            }
            break;

        case ESendTestModuleVersion:
            {
            __TRACE(KMessage, (_L("CTestCommandNotifier::RunL (combiner): SendTestModuleVersion command received")));
            TSendTestModuleVesionCommandParams params;
            TSendTestModuleVesionCommandParamsPckg paramsPack(params);
            paramsPack.Copy(iParamsPckg);

            __TRACE(KMessage, (_L("CTestCommandNotifier::RunL(combiner): propagating module version [%d][%d][%d][%S]"), params.iMajor, params.iMinor, params.iBuild, &params.iTestModuleName));
            TVersion moduleVersion(params.iMajor, params.iMinor, params.iBuild);
            
            TBool newVersionOfMethod = ETrue;
            iTestCombiner->TestModuleIf().SendTestModuleVersion(moduleVersion, params.iTestModuleName, newVersionOfMethod);
            }
            break;

        default:
            __TRACE(KError, (_L("CTestCommandNotifier::RunL (combiner): Unknown command [%d]."), iCommand));
        }

    // Set request again
    Start();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCommandNotifier

    Method: DoCancel

    Description: Cancel active request

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCommandNotifier::DoCancel()
    {
    __TRACE(KMessage, (_L( "CTestEventNotifier::DoCancel (combiner)")));

    iTestCase->TestExecution().CancelAsyncRequest(ETestExecutionNotifyCommand);
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCommandNotifier

    Method: RunError

    Description: Handle errors.

    Parameters: TInt aError: in: Symbian OS error: Error code

    Return Values: TInt KErrNone: Always returned KErrNone

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestCommandNotifier::RunError(TInt aError)
    {
    __TRACE(KError, (CStifLogger::ERed, _L("CTestCommandNotifier::RunError %d (combiner)"), aError));
    return KErrNone;
    }

// ================= OTHER EXPORTED FUNCTIONS =================================

// None

// End of File
