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
* Description: This module contains implementation of CTestExecution 
* class member functions. CTestExecution class handles a test 
* execution subsession specific operations.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include <e32svr.h>
#include "TestEngineClient.h"
#include "StifTFwIfProt.h"
#include <stifinternal/TestServerClient.h>
#include "TestServer.h"
#include "TestServerModuleIf.h"
#include "TestServerCommon.h"
#include "PrintQueue.h"
#include "TestServerEvent.h"
#include "TestThreadContainer.h"
#include "STIFMeasurement.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ==================== LOCAL FUNCTIONS =======================================

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: NewL

    Description: Returns new CTestExecution instance.

    Parameters: CTestModule* aModuleSession   :in:  "Parent" module
                TInt aCaseNumber              :in:  Test case number
                const TFileName& aConfig      :in:  Configuration filename

    Return Values: CTestExecution*                  New instance

    Errors/Exceptions: Function leaves if memory allocation fails or
                       CTestExecution construction leaves.
                       Panics is aModuleSession is NULL.

    Status: Proposal

-------------------------------------------------------------------------------
*/
CTestExecution* CTestExecution::NewL( CTestModule* aModuleSession,
                                      TInt aCaseNumber, 
                                      const TFileName& aConfig )
    {

    __ASSERT_ALWAYS ( aModuleSession, 
                      CTestServer::PanicServer( ENullModuleSession ) );

    CTestExecution* self=new( ELeave ) CTestExecution;
    CleanupClosePushL( *self );
    self->ConstructL( aModuleSession, aCaseNumber, aConfig );
    CleanupStack::Pop();

    return self;

    }


/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: CTestExecution

    Description: Constructor.
    Initialises member variables whose default state is not zero or NULL.

    Parameters:  None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CTestExecution::CTestExecution() : 
    iConfig( 0, 0 ),
    iTestThreadOpen( EFalse ),
    iTestThreadFailure( ETestThreadOk )
    {

    iPrintNotifyAvailable = EFalse;
    iThreadState = ENotStarted;
    iRemoteSendAvailable = EFalse;
    iRemoteReceiveAvailable = EFalse;
    iEventNotifyAvailable = EFalse;
    iCommandNotifyAvailable = EFalse;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: ~CTestExecution

    Description: Destructor

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: Panics is threadmutex is taken.

    Status: Approved
    
-------------------------------------------------------------------------------
*/
CTestExecution::~CTestExecution()
    {
    // Thread must be stopped before CTestExecution can be deleted
    if( iThreadState == EOnGoing ) 
		{
			if( iModuleContainer != NULL )
				{
				iModuleContainer->KillThread( KErrCancel );
				}
			delete iModuleContainer;
			iModuleContainer = NULL;
			CTestServer::PanicServer( EClosingWhileTestRunning );
		}

    delete iPrintHandler;
    iPrintHandler = NULL;
    delete iEventHandler;
    iEventHandler = NULL;
    delete iSndHandler;
    iSndHandler = NULL;
    delete iRcvHandler;
    iRcvHandler = NULL;
    delete iInterferenceHandler;
    iInterferenceHandler = NULL;
    delete iMeasurementHandler;
    iMeasurementHandler = NULL;
    delete iCommandHandler;
    iCommandHandler = NULL;

    // Test interference array(Needed cases when test interference thread is
    // needed to Kill())
    iSTIFTestInterferenceArray.Reset();
    iSTIFTestInterferenceArray.Close();

    // Reset test measurement array
    iMeasurementHandlingArray.Reset();
    iMeasurementHandlingArray.Close();

    // iPrintQueue must be emptied here, because it items are allocated from server heap
    delete iPrintQueue;
    iPrintQueue = NULL;

    // Delete name buffer
    delete iConfigNameBuffer;
    iConfigNameBuffer = NULL;

    delete iTestCaseArgs;
    iTestCaseArgs = NULL;
        
    iStateEvents.ResetAndDestroy();
    iStateEvents.Close();

    iEventArray.ResetAndDestroy();
    iEventArray.Close();

    // Close mutexes
    if ( iPrintMutex.Handle() != 0 ) iPrintMutex.Close();
    if ( iEventMutex.Handle() != 0 ) iEventMutex.Close();
    if ( iSndMutex.Handle() != 0 ) iSndMutex.Close();
    if ( iRcvMutex.Handle() != 0 ) iRcvMutex.Close();
    if ( iInterferenceMutex.Handle() != 0 ) iInterferenceMutex.Close();
    if ( iMeasurementMutex.Handle() != 0 ) iMeasurementMutex.Close();
    if ( iCommandMutex.Handle() != 0 ) iCommandMutex.Close();

    // Mutex for testcomplete and cancel operations. Close mutex
    if ( iTestThreadMutex.Handle() != 0 ) iTestThreadMutex.Close();

    // Close semaphores
    if ( iPrintSem.Handle() != 0 ) iPrintSem.Close();
    if ( iEventSem.Handle() != 0 ) iEventSem.Close();
    if ( iSndSem.Handle() != 0 ) iSndSem.Close();
    if ( iRcvSem.Handle() != 0 ) iRcvSem.Close();
    if ( iInterferenceSem.Handle() != 0 ) iInterferenceSem.Close();
    if ( iMeasurementSem.Handle() != 0 ) iMeasurementSem.Close();
    //if ( iReceiverSem.Handle() != 0 ) iReceiverSem.Close();
    if ( iCommandSem.Handle() != 0 ) iCommandSem.Close();
    
    iMessageQueue.Close();
    
    iTestThread.Close();

    delete iCommandDef;
    iCommandDef = NULL;
    }


/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: ConstructL

    Description: Second level constructor

    Parameters: CTestModule* aModuleSession   :in:  "Parent" module
                TInt aCaseNumber ::           :in:  Test case number
                const TFileName& aConfig      :in:  Configuration filename

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestExecution::ConstructL( CTestModule *aModuleSession,
                                 TInt aCaseNumber,
                                 const TFileName& aConfig )
    {

    // Get the parameters
    iModuleSession = aModuleSession;
    iCaseNumber = aCaseNumber;

    // Construct heap buffer for configuration file
    iConfigNameBuffer = HBufC::NewL( aConfig.Length() );
    iConfig.Set ( iConfigNameBuffer->Des() );
    iConfig.Copy ( aConfig );

    // Create mutexes
    User::LeaveIfError( iPrintMutex.CreateLocal() );
    User::LeaveIfError( iEventMutex.CreateLocal() );
    User::LeaveIfError( iSndMutex.CreateLocal() );
    User::LeaveIfError( iRcvMutex.CreateLocal() );
    User::LeaveIfError( iInterferenceMutex.CreateLocal() );
    User::LeaveIfError( iMeasurementMutex.CreateLocal() );
    User::LeaveIfError( iCommandMutex.CreateLocal() );

    // Mutex for testcomplete and cancel operations. Create mutex
    User::LeaveIfError( iTestThreadMutex.CreateLocal() );

    // Create semaphores
    User::LeaveIfError( iPrintSem.CreateLocal( 0 ) );
    User::LeaveIfError( iEventSem.CreateLocal( 0 ) );
    User::LeaveIfError( iSndSem.CreateLocal( 0 ) );
    User::LeaveIfError( iRcvSem.CreateLocal( 0 ) );
    User::LeaveIfError( iInterferenceSem.CreateLocal( 0 ) );
    User::LeaveIfError( iMeasurementSem.CreateLocal( 0 ) );
    //User::LeaveIfError( iReceiverSem.CreateLocal( 0 ) );
    User::LeaveIfError( iCommandSem.CreateLocal( 0 ) );

    // Create handlers
    iPrintHandler = CPrintHandler::NewL( *this );
    iEventHandler = CEventHandler::NewL( *this );
    iSndHandler  = CSndHandler::NewL( *this );
    iRcvHandler  = CRcvHandler::NewL( *this );
    iInterferenceHandler = CInterferenceHandler::NewL( *this );
    iMeasurementHandler = CMeasurementHandler::NewL( *this );
    iCommandHandler = CCommandHandler::NewL( *this );

    // Create print queue
    iPrintQueue = CPrintQueue::NewL();

    // Start print handler   
    iPrintHandler->StartL();
    
    // Start rcv handler
    iRcvHandler->StartL();

    // Start interference handler
    iInterferenceHandler->StartL();

    // Start measurement handler
    iMeasurementHandler->StartL();

    iCommandDef = CCommandDef::NewL();
    }


/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: RunTestCase

    Description: Starts to execute test case in separate thread.

    Parameters: const RMessage& aMessage  :in:      Message from client.

    Return Values: TInt                             Error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestExecution::RunTestCase( const RMessage2& aMessage )
    {

    __TRACE ( KInit, ( _L( "CTestExecution::RunTestCase in" ) ) );

    TInt ret = KErrNone;
    
    // Store message to be completed when case is finished
    iTestExeMessage = aMessage;

    iFullResult.iStartTime.HomeTime();

    delete iTestCaseArgs;
    iTestCaseArgs = NULL;
    
    TInt testCaseArgsLength = iTestExeMessage.GetDesLength( 1 );
    if ( ( testCaseArgsLength != KErrArgument ) && ( testCaseArgsLength != KErrBadDescriptor ) )
        {
        iTestCaseArgs = HBufC::New( testCaseArgsLength );
        if ( iTestCaseArgs == NULL )
            {
            _LIT(KRunError,"Can't allocate memory for test case arguments");
            __TRACE ( KError, ( CStifLogger::ERed, KRunError() ) );
            CompleteTestExecution( KErrNone,
                                   TFullTestResult::ECaseException,
                                   KErrNoMemory,
                                   KErrGeneral,
                                   KRunError );
            return KErrNone;        
            }
            
        TPtr testCaseArgsPtr( iTestCaseArgs->Des() );
        TRAP( ret, iTestExeMessage.Read( 1, testCaseArgsPtr ) );
        if ( ret != KErrNone )
            {
            _LIT(KRunError,"Can't read test case arguments");
            __TRACE ( KError, ( CStifLogger::ERed, KRunError() ) );
            CompleteTestExecution( KErrNone,
                                   TFullTestResult::ECaseException,
                                   ret,
                                   KErrGeneral,
                                   KRunError );
            return KErrNone;
            }
        }

    // Get a test module, which executes the test case.
    // The test module will be freed either when the test case
    // completes, or the test case is aborted.
    ret = iModuleSession->GetTestModule( iModuleContainer, iConfig );
    if ( ret != KErrNone || iModuleContainer == NULL )
        {
        _LIT(KRunError,"Can't get test module");
        __TRACE ( KError, ( CStifLogger::ERed, KRunError() ) );
        CompleteTestExecution( KErrNone,
                               TFullTestResult::ECaseException,
                               ret,
                               KErrGeneral,
                               KRunError );
        return KErrNone;
        }
    

    // Thread is created, so all thread specific operations are possible, 
    // even if thread is not yet resumed, so mark thread to be "Running"
    SetThreadState ( EOnGoing );

    // Start the test
    iModuleContainer->SetExecutionSubSession( this );
    iModuleContainer->RunTestCase( iConfig, iCaseNumber, aMessage );

    __TRACE ( KRunTestCase, ( _L( "CTestExecution::RunTestCase out. Case ongoing" ) ) );

    return KErrNone;

    }


/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: NotifyPrint

    Description: Handles print request from engine. If print queue is empty,
                 message is stored to be used later when there is something to
                 print, otherwise the first queue item is returned and request
                 completes immediately.

    Parameters: const RMessage& aMessage      :in:  Message from client.

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestExecution::NotifyPrint( const RMessage2& aMessage )
    {

    __ASSERT_ALWAYS( iPrintNotifyAvailable == EFalse, 
                     CTestServer::PanicServer( EPrintQueueBroken ) );

    // Get first item from queue
    TPrintQueueItem *item = iPrintQueue->Pop();

    if ( item == NULL )
        {

        // If thread can't print anymore, and queue is empty, then 
        // complete with Eof, because there will not be any new prints
        if ( iThreadState == EFinished || iThreadState == ECancelled )
            {
            __TRACE ( KPrint, ( _L( "CTestExecution::NotifyPrint to finished thread" ) ) );
            aMessage.Complete ( KErrEof );
            }
        else
            {
            // Print request available, do not complete request
            iNotifyPrintMessage = aMessage;
            iPrintNotifyAvailable = ETrue;
            }

        }
    else
        {
        // Construct message
        TTestProgress progress;
        progress.iPosition = item->iPriority;
        progress.iDescription = item->iData1;
        progress.iText = item->iData2;

        iNotifyPrintMessage = aMessage;
        // Write message to client space and complete request
        WritePrint( progress );

        delete item; 
        }


    return KErrNone;

    }
/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: DoNotifyPrint

    Description: If print notification available, notification is copied to
                    client memory space and request is completed.
                 Else new print queue item is created and appended to print 
                    queue. If queue is full or memory can't be allocated,
                    then message will be discarded.
        
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestExecution::DoNotifyPrint()
    {    

    if ( iPrintNotifyAvailable )
        {
        // Write message to client space
        WritePrint( iProgress );
        }
    else
        {

        TPrintQueueItem *item = 
            new TPrintQueueItem( iProgress.iPosition, 
                                 iProgress.iDescription, 
                                 iProgress.iText );

        if ( item != NULL )
            {
            // Item constructed ok, add it to queue
            if ( iPrintQueue->Push( *item ) != KErrNone )
                { 
                // Queue is full, clean-up
                delete item;
                }
            }
        // else: Could not construct TPrintQueueItem, no free memory.
        // drop printing            
        }
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: DoNotifyInterference

    Description: Handles Test Interference thread append and remove operations.
        
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestExecution::DoNotifyInterference()
    {    
    // Information for this context => information is in this thread's heap and
    // if CUndertaker::RunL is called then test interface thread can called
    // with function call and kill() test interference thread

    // Take information from struct given in 
    // CTestThreadContainer::AddInterferenceThread or
    // CTestThreadContainer::RemoveInterferenceThread
    
    RThread thread;
    TInt ret = thread.Open ( iTestInterference.iThreadId );
    if( ret != KErrNone )
    	{
    	RDebug::Print( _L("CTestExecution::DoNotifyInterference %d"), ret );
        User::Panic( _L("CTestExecution::DoNotifyInterference"), ret );
    	}
    
    if ( iTestInterference.iOperation == TTestInterference::EAppend )
        {
        iSTIFTestInterferenceArray.Append( thread );
        }
    else if (iTestInterference.iOperation == TTestInterference::ERemove )
        {
        // Remove thread from Array.Test interference thread is stopped and killed
        // successfully
        for( TInt i = 0; i < iSTIFTestInterferenceArray.Count(); i++ )
            {
            if( iSTIFTestInterferenceArray[i].Id() == thread.Id() )
                {
                iSTIFTestInterferenceArray.Remove( i );
                break;
                }
            }
        }
    else
        {
        return;
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: DoNotifyMeasurement

    Description: Handles test measurement operations: NewL, Start and Stop.
        
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
TInt CTestExecution::DoNotifyMeasurement()
    {    
    // Information for this context => information is in this thread's heap:
    // From CUndertaker::RunL() or CTestModuleContainer::RunL() method can
    // handle stop and clean operations if these operations are not done 
    // before.
    // Erronous cases(CUndertaker::RunL will be called) or when test case is
    // executed(CTestModuleContainer::RunL() will be called) there can handle
    // test measurement stop and clean operation. 

    if( iTestMeasurement.iMeasurementStruct.iMeasurementOperation == 
                                        CSTIFTestMeasurement::KMeasurementNew)
        {
        // Check that same measurement module don't try to create several times
        // Only one measurement type per test is allowed
        for( TInt i = 0; i < iMeasurementHandlingArray.Count(); i++ )
            {
            if( iMeasurementHandlingArray[i]->iMeasurementModulePtr->MeasurementType() ==
                        iTestMeasurement.iMeasurementStruct.iMeasurementType )
                {
                return KErrAlreadyExists;
                }
            }

        TMeasurementHandling* measurementHandling = NULL;
        measurementHandling = new TMeasurementHandling();

        TBuf<KStifMeasurementTypeLength> modulename;
        if( iTestMeasurement.iMeasurementStruct.iMeasurementType == 1 )
            {
            modulename.Copy( KStifMeasurement01 );
            }
        else if( iTestMeasurement.iMeasurementStruct.iMeasurementType == 2 )
            {
            modulename.Copy( KStifMeasurement02 );
            }
        else if( iTestMeasurement.iMeasurementStruct.iMeasurementType == 3 )
            {
            modulename.Copy( KStifMeasurement03 );
            }
        else if( iTestMeasurement.iMeasurementStruct.iMeasurementType == 4 )
            {
            modulename.Copy( KStifMeasurement04 );
            }
        else if( iTestMeasurement.iMeasurementStruct.iMeasurementType == 5 )
            {
            modulename.Copy( KStifMeasurement05 );
            }
        else if( iTestMeasurement.iMeasurementStruct.iMeasurementType == 6 )
            {
            modulename.Copy( KStifMeasurementBappea );
            }
        else
            {
            __TRACE( KError, (  _L("CTestExecution::DoNotifyMeasurement(): unknown measurement type" ) ) );
            return KErrArgument;
            }

        // Loading should work with and without '.dll' extension.
        TInt r = measurementHandling->iMeasurementModule.Load( modulename );
        if ( r != KErrNone )
            {
            __TRACE( KError, ( CStifLogger::EError, _L("Can't load test measurement module[%S], code = %d"),
                &modulename, r ) );
            delete measurementHandling;
            return KErrNotFound;
            }
        else
            {
            //Print test measururement module name
            __TRACE( KInit, (  _L("Loaded test measurement module[%S]"),
                &modulename ) );
            }

        // Get pointer to first exported function
        // Verify that there is function
        measurementHandling->iMeasurementLibEntry = (CTestMeasurementFactory) measurementHandling->iMeasurementModule.Lookup( 1 );
        if ( measurementHandling->iMeasurementLibEntry == NULL )
            {
            // New instance can't be created
            __TRACE( KInit, (_L( "Test measurement module instance cannot create." ) ) );
            measurementHandling->iMeasurementModule.Close();
            delete measurementHandling;
            return KErrNotFound;
            }
        else
            {
            __TRACE ( KInit, ( _L("Pointer to 1st exported received")));
            }

        CSTIFTestMeasurementImplementation* measurement;
        measurement = NULL;

        // initialize test measurement module
        __TRACE ( KVerbose, (_L("Calling 1st exported at 0x%x"), (TUint32) measurementHandling->iMeasurementLibEntry ));
        TRAPD ( err, measurement = (*measurementHandling->iMeasurementLibEntry)( 
                            iTestMeasurement.iMeasurementStruct.iConfigurationInfo, iTestMeasurement.iMeasurementStruct.iMeasurementType ) );

         // Handle leave from test module
        if ( err != KErrNone )
            {
            __TRACE (KError, ( CStifLogger::EError, _L( "Leave when calling 1st exported function, code %d"), err ) );
            delete measurementHandling;
            return err;
            }
        else if ( measurementHandling->iMeasurementLibEntry == NULL )     // Handle NULL from test measurement
            {
            __TRACE (KError, ( CStifLogger::EError, _L( "NULL pointer received when constructing test measurement module" ) ) );
            delete measurementHandling;
            // Set error codes
            return KErrNoMemory;
            }
        else
            {
            measurementHandling->iMeasurementModulePtr = measurement;
            __TRACE (KInit, (_L("Entrypoint successfully called, Measurement module instance at 0x%x"),
                (TUint32)measurementHandling->iMeasurementLibEntry ) );
            }
        iMeasurementHandlingArray.Append( measurementHandling );
        } // End of NewL operations

    else if( iTestMeasurement.iMeasurementStruct.iMeasurementOperation == 
                                        CSTIFTestMeasurement::KMeasurementStart)
        {
        for( TInt i = 0; i < iMeasurementHandlingArray.Count(); i++ )
            {
            if( iMeasurementHandlingArray[i]->iMeasurementModulePtr->MeasurementType() == iTestMeasurement.iMeasurementStruct.iMeasurementType )
                {
                return iMeasurementHandlingArray[i]->iMeasurementModulePtr->Start();
                }
            }

        }
    else if( iTestMeasurement.iMeasurementStruct.iMeasurementOperation == 
                                        CSTIFTestMeasurement::KMeasurementStop)
        {
        for( TInt i = 0; i < iMeasurementHandlingArray.Count(); i++ )
            {
            if( iMeasurementHandlingArray[i]->iMeasurementModulePtr->MeasurementType() == iTestMeasurement.iMeasurementStruct.iMeasurementType )
                {
                TInt stop_ret = iMeasurementHandlingArray[i]->iMeasurementModulePtr->Stop();
                delete iMeasurementHandlingArray[i];
                iMeasurementHandlingArray.Remove(i);
                return stop_ret;
                }
            }
        }
    else 
        {
        __TRACE (KError, ( CStifLogger::EError, _L("CTestExecution::DoNotifyMeasurement(): Not supported operation") ) );
        return KErrNotSupported;
        }
    
    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: WritePrint

    Description: Writes print notification to client memory space.                 
        
    Parameters: const TInt aPriority          :in:  Priority
                const TStifInfoName& aDes         :in:  Description
                const TName& aBuffer          :in:  Value
    
    Return Values: TInt                             Return value from message write

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestExecution::WritePrint( TTestProgress& aProgress )
    {

    TTestProgressPckg progressPckg( aProgress );
 
    // Write message to client space    
    TRAPD( res, iNotifyPrintMessage.WriteL( 0, progressPckg ) );

    iPrintNotifyAvailable = EFalse;

    // Complete request with possible error code
    iNotifyPrintMessage.Complete( res );

    return res;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: NotifyEvent

    Description: Handles event request from engine. 

    ESetEvent and EUnsetEvent change event status. EEnable enables 
    event notify.

    Parameters: const RMessage& aMessage      :in:  Message from client.
                    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestExecution::NotifyEvent( const RMessage2& aMessage )
    {   

    TEventIf event;
    TEventIfPckg eventPckg( event );
    
    TRAPD( res, aMessage.ReadL( 0, eventPckg ) );

    if( res != KErrNone )
        {
        __TRACE( KError, ( _L( "CTestExecution::NotifyEvent: ReadL failed" ) ));
        return res;
        }
    
    switch( event.Type() )
        {
        case TEventIf::ESetEvent: 
            {
            TInt ind = 0;
            TInt count = iEventArray.Count();
            for( ind=0; ind < count; ind++ )
                {
                if( iEventArray[ind]->Name() == event.Name() )
                    {
                    __TRACE( KMessage, ( _L( "CTestExecution::NotifyEvent(%d): SetEvent %S ( count %i )" ), 
                        this, &event.Name(), count ) );
                    TEventIf::TEventType etype = TEventIf::EIndication;
                    // To get state events working with testcombiner
                    if( ( event.EventType() == TEventIf::EState ) ||                            
                        ( iEventArray[ind]->EventType() == TEventIf::EState ) )
                        {
                        etype = TEventIf::EState;
                        }
                    iEventArray[ind]->Set( etype );
                    }
                }
            }
            break;
        case TEventIf::EUnsetEvent:
            { 
            return UnsetEvent( event, aMessage );
            }
        case TEventIf::EEnable:
            {
            // Store event message
            iNotifyEventMessage = aMessage;
            iEventNotifyAvailable = ETrue;
            TInt errorFromPreviousCmd =  aMessage.Int1();

            __TRACE( KMessage, ( _L( "CTestExecution::NotifyEvent: enable (%d) notify %d" ), 
                errorFromPreviousCmd, this ) );

            // Complete previous Event command
            CompleteEvent( errorFromPreviousCmd );    
            iEventHandler->StartL();
            
            // iNotifyEventMessage is completed from DoNotifyEvent
            return KErrNone; 
            }
        
        default:
            __TRACE( KError, ( _L( "CTestExecution::NotifyEvent: Unknown command %i" ), 
                event.Type() ));
            return KErrArgument;
        }

    __TRACE( KMessage, ( _L( "CTestExecution::NotifyEvent(%d): Done" ), this ) );

    aMessage.Complete( KErrNone );
    //User::After(1000); //Stif-95 This pause is needed to let Info::TestFrameworkEngine thread
                       //get some processor time and run exactly at this moment (by calling
                       //CTestEventController::EventCallBack). When Stif is focused on the screen,
                       //there is no problem if this line is absent, however when another
                       //application (like screen saver) is focused this is required.
    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: DoNotifyEvent

    Description: Forward event request.
        
    Parameters: None
   
    Return Values: None

    Errors/Exceptions: Panics if event array can't be created

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestExecution::DoNotifyEvent()
    {
    
    TInt ret = KErrNone;
        
    switch( iEventDef.iEvent.Type() )
        {
        case TEventIf::EReqEvent:
            {
            __TRACE( KMessage, 
                ( _L( "CTestExecution::DoNotifyEvent(%d): ReqEvent %S" ), 
                    this, &iEventDef.iEvent.Name() ) );
            ret = RequestEvent( iEventDef.iEvent );
            // If error, return error, 
            // otherwise proceed with sending information to engine
            if( ret != KErrNone )
                {
                iEventHandler->StartL();
                return ret;
                }
            }
            break;
        case TEventIf::EWaitEvent:
        case TEventIf::ESetWaitPending:
            {
            __TRACE( KMessage, 
                ( _L( "CTestExecution::DoNotifyEvent(%d): WaitEvent %S" ), 
                    this, &iEventDef.iEvent.Name() ) );
            // We return from here, i.e. nothing is sent to engine
            ret = WaitEvent( iEventDef.iEvent, iEventDef.iStatus );
            iEventHandler->StartL();
            return ret;
            }
        // Add state events to internal array  
        case TEventIf::ESetEvent: 
        case TEventIf::EUnsetEvent:
            {
            __TRACE( KMessage, 
                ( _L( "CTestExecution::DoNotifyEvent(%d): Set/UnsetEvent %S" ), 
                    this, &iEventDef.iEvent.Name() ) );
            EventStateChange( iEventDef.iEvent );
            }
            break;
        case TEventIf::ERelEvent:
            // This is handled later
            __TRACE( KMessage, 
                ( _L( "CTestExecution::DoNotifyEvent(%d): RelEvent %S" ), 
                    this, &iEventDef.iEvent.Name() ) );
            break;
        default:
            iEventHandler->StartL();
            return KErrArgument;
        }
    
    // Construct message
    TEventIfPckg eventPckg( iEventDef.iEvent );

    // Write message to client space
    TRAPD( res, iNotifyEventMessage.WriteL( 0, eventPckg ) );

    if( res != KErrNone )
        {
        __TRACE( KError, ( _L( "CTestExecution::DoNotifyEvent: WriteL failed" ) ) );
        ret = res;
        }
    // Complete request
    // Complete with WriteL result
    // MUST be done before ERelEvent
    iEventNotifyAvailable = EFalse;
    iNotifyEventMessage.Complete( res );
    User::After(1000); //Stif-95 This pause is needed to let Info::TestFrameworkEngine thread
                       //get some processor time and run exactly at this moment (by calling
                       //CTestEventNotifier::RunL). When Stif is focused on the screen,
                       //there is no problem if this line is absent, however when another
                       //application (like screen saver) is focused this is required.

    if( iEventDef.iEvent.Type() == TEventIf::ERelEvent )
        {
        ret = ReleaseEvent( iEventDef.iEvent );
        if( ret != KErrNone )
            {
            // If error, return immediately
            return ret;
            }
        }

    __TRACE( KMessage, ( _L( "CTestExecution::DoNotifyEvent(%d): Done" ), this ) );

    return ret;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: RequestEvent

    Description: Handles TestModule event requests. 
        
    Parameters: const TEventIf& aEvent: in: Event requested  
    
    Return Values: TInt: Success/Error code

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/    
TInt CTestExecution::RequestEvent( TEventIf& aEvent )
    {

    // Requested events are added to iEventArray
    // There can be multiple simultaneous requests for the same event                    
    TEvent* event = new TEvent( iTestThreadId );    
    if( !event )
        {
        return KErrNoMemory;
        }
    event->Copy( aEvent );
    if( iEventArray.Append( event ) != KErrNone )
        {
        delete event;
        return KErrNoMemory;
        }
        
    __TRACE( KMessage,( _L( "CTestExecution::DoNotifyEvent: Req added %S ( count %i )" ), 
         &aEvent.Name(), iEventArray.Count() ));
    
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: WaitEvent

    Description: Handles TestModule event waits. Waiting is implemented here 
                and completed in NotifyEvent    

        
    Parameters: const TEventIf& aEvent: in: Event wait  
    
    Return Values: TInt: Success/Error code

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/    
TInt CTestExecution::WaitEvent( TEventIf& aEvent, TRequestStatus* aStatus )
    {
      
    // Get first "free" event entry from requested events
    TInt ind = 0;
    TInt count = iEventArray.Count();
    TEvent* event = NULL;
    for( ind=0; ind < count; ind++ )
        {
        if( ( iEventArray[ind]->Name() == aEvent.Name() ) &&
            ( iEventArray[ind]->WaitEventPending() == EFalse ) )
            {
            // Found event with correct name and one that is still free
            // i.e. nobody is waiting for it.
            event = iEventArray[ind];
            if( aEvent.Type() == TEventIf::EWaitEvent )
                {
                // For EWaitEvent we complete immediately if event is pending
                if( event->EventPending() )
                    {
                    // Event was already pending, so we may return immediately
                    if( aStatus )
                        {
                        TestThreadRequestComplete( aStatus, KErrNone );
                        }
                    return KErrNone;           
                    }
                }
            }
        }
    if( event == NULL )
        {
        // Event is not in the iEventArray, 
        // so event is not currently requested
        return KErrNotFound;
        }
        
    __TRACE( KMessage,( _L( "CTestExecution::DoNotifyEvent: Waiting %S ( count %i )" ), 
        &aEvent.Name(), count ));
    
    // Event is not pending so we have to wait for it
    // Set wait pending            
    TInt ret = event->SetWaitPending( aStatus );
    // event cannot be freed by the test module, 
    // because it is waiting the event, 

    if( ( aStatus == NULL ) &&
        ( ret == KErrNone ) &&
        ( aEvent.Type() == TEventIf::EWaitEvent ) ) 
        {
        // This is synchronous wait
        // Go waiting, completed from set event            
        event->Wait();
        }

    return ret;
    
    } 
    
/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: ReleaseEvent

    Description: Handles TestModule event releases. 
        
    Parameters: const TEventIf& aEvent: in: Event released  
    
    Return Values: TInt: Success/Error code

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/   
TInt CTestExecution::ReleaseEvent( TEventIf& aEvent )
    {
    
    // Get last event entry from requested events
    TInt ind = 0;
    TInt freeFound = 0;
    TInt eventIndex = 0;
    TInt count = iEventArray.Count();
    for( ind=0; ind < count; ind++ )
        {
        if( iEventArray[ind]->Name() == aEvent.Name() )
            {
            if( iEventArray[ind]->WaitEventPending() == EFalse )
                {
                freeFound++;
                eventIndex = ind;
                }
            }
        }

    if( freeFound == 0 )
        {
        return KErrNotFound;
        }
    __TRACE( KMessage,( 
        _L( "CTestExecution::DoNotifyEvent: Release event %S ( count %i )" ), 
            &aEvent.Name(), iEventArray.Count() ));

    TEvent* event = iEventArray[eventIndex];
    iEventArray.Remove( eventIndex );
    // Release and free event entry
    event->Release(); 

    // Delete used event entry
    delete event;
    
    return KErrNone;
}

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: UnsetEvent

    Description: Handles TestModule event unsets. 
        
    Parameters: const TEventIf& aEvent: in: Event unset  
    
    Return Values: TInt: Success/Error code

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestExecution::UnsetEvent( TEventIf& aEvent, const RMessage2& aMessage )
    {
    
    TInt ret = KErrNone;
    TInt ind = 0;
    TInt count = iEventArray.Count();
    TBool activeEventReq = EFalse;
    for( ind=0; ind < count; ind++ )
        {
        if( iEventArray[ind]->Name() == aEvent.Name() )
            {
             __TRACE( KMessage, ( 
                _L( "CTestExecution::NotifyEvent(%d): UnsetEvent %S ( count %i )" ), 
                this, &aEvent.Name(), count ) );
                
            ret = iEventArray[ind]->Unset( aMessage, this );
            if( ret == KErrNone )
                {
                activeEventReq = ETrue;
                }
            break;
            }
        }
    if( activeEventReq )
        {
        // Unset is completed by release   
        return KErrNone;
        }
        
    // No pending requests or error from unset
    aMessage.Complete( ret );
    
    return KErrNone;
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: EventStateChange

    Description: Handles event setting and unsetting. Saves status of
                state events to enable unsetting of set state events when 
                killing the testcase abnormally.
        
    Parameters: const TEventIf& aEvent: in: Event set/unset  
    
    Return Values: TInt: Success/Error code

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/   
TInt CTestExecution::EventStateChange( TEventIf& aEvent )
    {
    
    TInt ret = KErrNotFound;
    if( aEvent.EventType() == TEventIf::EState )
        {
        
        TInt count = iStateEvents.Count();
        TInt index = 0;
        for(; index < count; index++ )   
            {
            TPtrC name = *(iStateEvents[index]);
            if( name == aEvent.Name() )
                {
                break;
                }
            } 
            
                
        if( aEvent.Type() == TEventIf::ESetEvent )
            {
            if( index < count )
                {
                ret = KErrAlreadyExists;
                }
            else
                {
                // add event to iStateEvents
                HBufC* name = aEvent.Name().Alloc();
                if( name == NULL )
                    {
                    ret = KErrNoMemory;
                    }
                else if( iStateEvents.Append( name ) != KErrNone )
                    {
                    delete name;
                    ret = KErrNoMemory;
                    }
                else
                    {
                    ret = KErrNone;
                    }
                }                
            }
        else if( aEvent.Type() == TEventIf::EUnsetEvent )
            {
            if( index == count )
                {
                ret = KErrNotFound;
                }
            else
                {
                ret = KErrNone;
                delete iStateEvents[index];
                iStateEvents.Remove( index );
                }
            }
        }
    
    return ret;
    
    }
/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: CancelEvent

    Description: Cancels pending event request.
        
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None
    
    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestExecution::CancelEvent()
    {
    
    TInt ret = KErrNotFound;

    __TRACE( KMessage, ( _L( "CTestThreadContainer::CancelEvent(%d): %S [%p]" ), 
            iEventDef.iEvent.Type(), &iEventDef.iEvent.Name(), iEventDef.iStatus ) );

    switch( iEventDef.iEvent.Type() )
        {
        case TEventIf::EReqEvent:
        case TEventIf::ERelEvent:
        case TEventIf::ESetEvent:
        case TEventIf::EUnsetEvent:   
            CompleteEvent( KErrCancel );  
            break;
        case TEventIf::EWaitEvent:
        case TEventIf::ESetWaitPending:
            {
            // Get waited event entry from requested events
            TInt ind = 0;
            TInt count = iEventArray.Count();
            for( ind=0; ind < count; ind++ )
                {
                if( ( iEventArray[ind]->Name() == iEventDef.iEvent.Name() ) &&
                    ( iEventArray[ind]->WaitEventPending() != EFalse ) &&
                    ( iEventArray[ind]->RequestStatus() == iEventDef.iStatus ) )
                    {
                    TEvent* event = iEventArray[ind];
                    // Found event with correct name and one that is waited.
                    event->CompletePendingWait( KErrCancel ); 
                    ret = KErrNone;
                    break;
                    }
                }
            if( ret != KErrNone )
                {
                // Request has been already completed, not handled 
                // by ActiveScheduler yet.
                // DoCancel will do the job, don't worry.
                __TRACE( KError, ( _L( "CTestThreadContainer::CancelEvent(%d): %S, not found 2" ), 
                    iEventDef.iEvent.Type(), &iEventDef.iEvent.Name() ) ); 
                }
            }
            break;
        default:
            CTestServer::PanicServer( EUnknownEventCmd );
            break;
        }
        
    // Synchonize
    iEventHandler->StartL();
        
    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: NotifyRemoteCmd

    Description: Handles the first phase of the RemoteCmd request from engine.

    Parameters: const RMessage& aMessage: in: Message from client.
                    
    Return Values: Symbian OS error code.

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestExecution::NotifyRemoteCmd( const RMessage2& aMessage )
    {   

    // Store RemoteCmd message
    // and the length buffer address
    iNotifyRemoteCmdMessage = aMessage;
    iRemoteSendAvailable = ETrue;
    iSndHandler->StartL();

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: DoRemoteReceive

    Description: Enable remote receive.
    
    Parameters: None 
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestExecution::DoRemoteReceive()
    {    

    switch( iRcvCmdDef.iCommand )
        {
        case EStifCmdReceive:          // "Receive"
            {
            // @js If there's pointer in array, we'll send it down
            if(iMessageQueue.Count() > 0)
                {
                HBufC8 * buf =  iMessageQueue[0];
                iRcvCmdDef.iParam.aRemoteMsgRef->Copy( iMessageQueue[0]->Des() );
                iMessageQueue.Remove(0);
                delete buf; 
                __TRACE( KMessage, 
                        (_L("CTestExecution::ReadRemoteCmdInfo: complete receive 0x%x"), 
                            iRcvCmdDef.iStatus ));
                       
                 // calls the TestCombiner's CRemoteReceiver::RunL()
                 TestThreadRequestComplete( iRcvCmdDef.iStatus, KErrNone );
                }
            else
                {
                iRemoteReceiveAvailable = ETrue;            
                }

            break;
            }
        case EStifCmdReceiveCancel:
            if( iRemoteReceiveAvailable )
                {
                iRemoteReceiveAvailable = EFalse;
                TestThreadRequestComplete( iRcvCmdDef.iStatus, KErrCancel );
                }                
            break;
        default:
            CTestServer::PanicServer( EUnknownRemoteCmd );
            break;
        }
    // Signal test thread
    iRcvHandler->StartL();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: DoRemoteSend

    Description: Send remote messages.
    
    Parameters: None 
    
    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestExecution::DoRemoteSend()
    {  
    switch( iCmdDef.iCommand )
        {  
        case EStifCmdSend:             // "Send"
        case EStifCmdReboot:           // "Send"
        case EStifCmdStoreState:       // "Send"
        case EStifCmdGetStoredState:   // "Receive, this must be done with two phase"
        case EStifCmdMeasurement:      // "Receive, this must be done with two phase"
            {
            if( iRemoteSendAvailable )
                {
                TInt ret = KErrNone;
                TPckg<TStifCommand> remoteCommandPckg( iCmdDef.iCommand );

                // Start first phase of the writing message to client space
                TRAP( ret, 
                    iNotifyRemoteCmdMessage.WriteL( 0, remoteCommandPckg ) );

                if( ret != KErrNone )
                    {
                    __TRACE( KError, ( _L( "CTestExecution::DoRemoteSend: WriteL failed" ) ) );
                    }
                TPckg<TInt> lenPckg( iCmdDef.iLen );

                TRAP( ret, 
                    iNotifyRemoteCmdMessage.WriteL( 1, lenPckg ) );

                if( ret != KErrNone )
                    {
                    __TRACE( KError, ( _L( "CTestExecution::DoRemoteSend: WriteL failed" ) ) );
                    }

                // Complete request
                // Complete with WriteL result
                // CTestRemoteCmdNotifier::RunL() will called
                iRemoteSendAvailable = EFalse;
                iNotifyRemoteCmdMessage.Complete( ret );
                break;
                }
            }
            TestThreadRequestComplete( iCmdDef.iStatus, KErrNotSupported );
            break;
        default:
            CTestServer::PanicServer( EUnknownRemoteCmd );
            break;
        }

    }


/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: ReadRemoteCmdInfo

    Description: Handles the second phase of the RemoteCmd request from engine.

    Parameters: const RMessage& aMessage: in: Message from client.
                    
    Return Values: Symbian OS error code.

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestExecution::ReadRemoteCmdInfo( const RMessage2& aMessage )
    {   
    
    TInt ret( KErrNone );
    TInt completeSend = EFalse;
    TInt returnedErrorCode = KErrNone;
    TInt len = 0;
    TInt value( aMessage.Int2() );
    if( value < 0 )
        {
        returnedErrorCode = value;
        }
    else
        {
        len = value;
        }
  
    switch( aMessage.Int1() )
        {
        case EStifCmdReceive:              // "Receive"
            {

                if( len <= 0 )
                    {
                    __TRACE( KError, 
                        (_L("CTestExecution::ReadRemoteCmdInfo: empty message")));
                    ret = KErrGeneral;
                    break;
                    }
                if( iRcvCmdDef.iParam.aRemoteMsgRef->MaxLength() < len )
                    {
                    __TRACE( KError, 
                        (_L("CTestExecution::ReadRemoteCmdInfo: message overflow")));
                    ret = KErrGeneral;
                    break;
                    }

                HBufC8* buf = HBufC8::New( len );
                if( buf == NULL )
                    { 
                    ret = KErrNoMemory;
                    break;
                    }
                TPtr8 tmp = buf->Des();
                // Read message from client space
                TRAP( ret, aMessage.ReadL( 0, tmp ) );

                
                if( ret != KErrNone )
                    {
                    __TRACE( KError, 
                        (_L("CTestExecution::ReadRemoteCmdInfo: ReadL failed")));
                    break;
                    }
                    
                if( iRemoteReceiveAvailable )
                // @js Checking moved here
                    {                                   
                    iRcvCmdDef.iParam.aRemoteMsgRef->Copy( buf->Des() );
                    delete buf;
                
                    __TRACE( KMessage, 
                        (_L("CTestExecution::ReadRemoteCmdInfo: complete receive 0x%x"), 
                            iRcvCmdDef.iStatus ));
                    // calls the TestCombiner's CRemoteReceiver::RunL()
                    TestThreadRequestComplete( iRcvCmdDef.iStatus, ret );
                    
                    iRemoteReceiveAvailable = EFalse;
                    }
                else
                // @js Adding buf to RPointerArray, handling it in DoRemoteReceive function
                    {
                   iMessageQueue.Append( buf );
                    } 
                
            break;
            }
        case EStifCmdSend:                 // "Send"
            {
            HBufC8* buf = HBufC8::New( iCmdDef.iParam.aRemoteMsgConstRef->Length() );
            if( buf == NULL )
                { 
                ret = KErrNoMemory;
                break;
                }
            TPtr8 tmp = buf->Des();
            tmp.Copy( *iCmdDef.iParam.aRemoteMsgConstRef );
            // Second phase of the writing. Write information to the Engine.
            TRAP( ret, aMessage.WriteL( 0, tmp ) );

            delete buf;
            
            if( ret != KErrNone )
                {
                __TRACE( KError, ( _L( "CTestExecution::ReadRemoteCmdInfo: WriteL failed" ) ) );
                }
            completeSend = ETrue;
            break;
            }
        case EStifCmdReboot:               // "Send"
            {
            TRebootParamsPckg remoteTypePckg( *iCmdDef.iParam.aRebootType );

            // Second phase of the writing. Write information to the Engine.
            TRAP( ret, aMessage.WriteL( 0, remoteTypePckg ) );

            if( ret != KErrNone )
                {
                __TRACE( KError, ( _L( "CTestExecution::ReadRemoteCmdInfo: WriteL failed" ) ) );
                }
            // Because Reboot must block...Don't complete, so now we wait for
            // ever phone booting operation !!!!!
            break;
            }
        case EStifCmdStoreState:          // "Send"
            {
            TRebootStateParamsPckg remoteStatePckg( *iCmdDef.iParam.aRebootState );

            // Second phase of the writing. Write information to the Engine.
            TRAP( ret, aMessage.WriteL( 0, remoteStatePckg ) );

            if( ret != KErrNone )
                {
                __TRACE( KError, ( _L( "CTestExecution::ReadRemoteCmdInfo: WriteL failed" ) ) );
                }
            // Completed from EStifCmdRebootProceed
            break;
            }
        case EStifCmdMeasurement:   // "Receive"
            {
            TGetMeasurementOptions getMeasurementOptions;
            TGetMeasurementOptionsPckg measurementParamsPckg( getMeasurementOptions );

            // Read message from client space
            TRAP( ret, aMessage.ReadL( 0, measurementParamsPckg ) );

            if( ret != KErrNone )
                {
                __TRACE( KError, ( _L( "CTestExecution::ReadRemoteCmdInfo: ReadL failed" ) ) );
                }
            iCmdDef.iParam.aMeasurementOption->iOptions = getMeasurementOptions.iOptions;

            completeSend = ETrue;
            break;
            }
        case EStifCmdGetStoredState:   // "Receive"
            {
            TGetRebootStoredParams getRebootStoredParams;
            TGetRebootStoredParamsPckg rebootStoredParamsPckg( getRebootStoredParams );

            // Read message from client space
            TRAP( ret, aMessage.ReadL( 0, rebootStoredParamsPckg ) );

            if( ret != KErrNone )
                {
                __TRACE( KError, ( _L( "CTestExecution::ReadRemoteCmdInfo: ReadL failed" ) ) );
                }
            iCmdDef.iParam.aRebootStoredRef->iCode = getRebootStoredParams.aCode;
            iCmdDef.iParam.aRebootStoredRef->iName = getRebootStoredParams.aName;

            completeSend = ETrue;
            break;
            }
        case EStifCmdRebootProceed:          
            {
            completeSend = ETrue; // Complete EStifCmdStoreState
            }
            break;
        default:
            {
            iModuleSession->PanicClient( EUnknownRemoteCmd, aMessage );
            break;
            }
        }
    // Complete request with ret
    aMessage.Complete( ret ); 

    if( completeSend )
        {
        // Calls the TestCombiner's CRemoteReceiver::RunL() and
        // returns error code to test module.
        TestThreadRequestComplete( iCmdDef.iStatus, returnedErrorCode );
        }
        
    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: Pause

    Description: Pauses a test case

    Only ongoing tests can be paused. If test is not ongoing, then
    function does nothing.

    Parameters: const RMessage& aMessage      :in:  Message from client.
    
    Return Values: TInt                             Always KErrNone

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestExecution::Pause( const RMessage2& aMessage )
    {

    __TRACE ( KThreadOperation, ( _L( "CTestExecution::Pause" ) ) );

    TInt completionCode = KErrNone;

    // Note that it is allowed to suspend already suspended thread, so
    // there is no need for states to verify that thread is really in
    // resumed state.
    switch ( iThreadState )
        {
        case EOnGoing:
            completionCode = iModuleContainer->PauseThread();
            break;

        case ENotStarted:
            completionCode = KErrNotReady;
            break;

        case EFinished:
        case ECancelled:
            completionCode = KErrCompletion;
            break;

        default:
            CTestServer::PanicServer( EInvalidThreadState );
            break;
        }


    // Complete the request
    aMessage.Complete( completionCode );

    return KErrNone;

    }


/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: Resume

    Description: Resumes a test case. If test is not ongoing,
    then do not resume.

    Parameters: const RMessage& aMessage      :in:  Message from client.
    
    Return Values: TInt                             Always KErrNone

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestExecution::Resume( const RMessage2& aMessage )
    {

    __TRACE ( KThreadOperation, ( _L( "CTestExecution::Resume" ) ) );

    TInt completionCode = KErrNone;

    // Note that it is allowed to resume already resumed thread, so
    // there is no need for states to verify that thread is really in
    // suspended state.
    switch ( iThreadState )
        {
        case EOnGoing:
            completionCode = iModuleContainer->ResumeThread();
            break;

        case ENotStarted:
            completionCode = KErrNotReady;
            break;

        case EFinished:
        case ECancelled:
            completionCode = KErrCompletion;
            break;

        default:
            CTestServer::PanicServer( EInvalidThreadState );
            break;
        }


    // Complete the request
    aMessage.Complete( completionCode );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: CloseTestExecution

    Description: Closes Test Execution subsession

    Parameters: const RMessage& aMessage      :in:  Message from client.
    
    Return Values: TInt                             Always KErrNone

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestExecution::CloseTestExecution( const RMessage2& aMessage )
    {

    // Cancel test request
    CancelTestRequest();

    // Cancel print request
    CancelPrintRequest();
    
    // Cancel event request
    CancelEventRequest();

    // Delete this subsession
    const TUint subsession = aMessage.Int3();
    iModuleSession->DeleteTestExecution( subsession, aMessage );

    // Complete the request
    aMessage.Complete( KErrNone );

    return KErrNone;

    }


/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: CancelPrintRequest

    Description: Cancel print request. Completes ongoing print request
    with KErrCancel.

    Parameters: None
    
    Return Values: TInt                             Always KErrNone

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestExecution::CancelPrintRequest()
    {

    if ( iPrintNotifyAvailable )
        {
        iPrintHandler->Cancel();
        
        iPrintNotifyAvailable = EFalse;
        iNotifyPrintMessage.Complete ( KErrCancel );
        //@spe iPrintNotifyAvailable = EFalse;
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: CancelEventRequest

    Description: Cancel event request. Completes ongoing event request
    with KErrCancel.

    Parameters: None
    
    Return Values: TInt                             Always KErrNone

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestExecution::CancelEventRequest()
    {
    iEventHandler->Cancel();

    if( iEventNotifyAvailable )
        {
        // Cancel request
        iEventNotifyAvailable = EFalse;
        iNotifyEventMessage.Complete ( KErrCancel );
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: CancelInterferenceRequest

    Description: Cancel print request. Completes ongoing print request
    with KErrCancel.

    Parameters: None

    Return Values: TInt: Always KErrNone

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestExecution::CancelInterferenceRequest()
    {

    if ( iInterferenceNotifyAvailable )
        {
        iInterferenceHandler->Cancel();
        
        iInterferenceNotifyAvailable = EFalse;
        iNotifyInterferenceMessage.Complete ( KErrCancel );
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: CancelRemoteCmdRequest

    Description: Cancel RemoteCmd request. Completes ongoing RemoteCmd request
        with KErrCancel.

    Parameters: None
    
    Return Values: TInt: Always KErrNone

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestExecution::CancelRemoteCmdRequest()
    {
    iSndHandler->Cancel();

    // Cancel request
    if( iRemoteSendAvailable )
        {
        iRemoteSendAvailable = EFalse;
        iNotifyRemoteCmdMessage.Complete ( KErrCancel );
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: NotifyCommand

    Description: Handles the first phase of the Command request from engine.

    Parameters: const RMessage& aMessage: in: Message from client.

    Return Values: Symbian OS error code.

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestExecution::NotifyCommand( const RMessage2& aMessage )
    {
    iNotifyCommandMessage = aMessage;
    iCommandNotifyAvailable = ETrue;
    iCommandHandler->StartL();

    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: CancelCommandRequest

    Description: Cancel Command request. Completes ongoing Command request
        with KErrCancel.

    Parameters: None

    Return Values: TInt: Always KErrNone

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestExecution::CancelCommandRequest()
    {
    iCommandHandler->Cancel();

    // Cancel request
    if(iCommandNotifyAvailable)
        {
        iCommandNotifyAvailable = EFalse;
        iNotifyCommandMessage.Complete(KErrCancel);
        }

    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: GetTestCaseArguments

    Description: Get test case arguments

    Parameters: None

    Return Values: Test case arguments

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
const TDesC& CTestExecution::GetTestCaseArguments() const
    {
    if ( iTestCaseArgs != NULL )
        {
        return *iTestCaseArgs;
        }
    return KNullDesC;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: CancelTestRequest

    Description: Cancel test execution request.
    If thread is not in OnGoing state, function does not do anything.

    Otherwise following operations are done:
    1 ) Obtain access to both  PrintMutex and EventMutex to make sure that
       thread can't be accessing print queue or events.
    2 ) Kill the thread
    3 ) Complete original "RunTestCase" request with KErrCancel
    4 ) If print queue is empty, complete print request
    5 ) Give up mutexes
    6 ) Delete module container

    Parameters: None
    
    Return Values: TInt: Always KErrNone

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
TInt CTestExecution::CancelTestRequest()
    {
    // This mutex block and make sure that cancel operation is done succesfully
    // and test case e.g. cannot complete at the same time. Get mutex.
    iTestThreadMutex.Wait();

    // iModuleContainer is an active object, check is active.
    if ( iModuleContainer == NULL || ( iModuleContainer != NULL && !iModuleContainer->IsActive() ) )
        {
        // Test case is already ready, return.
        iTestThreadMutex.Signal();
        return KErrNone;
        }

    if ( iThreadState == EOnGoing )
        {
        // Print mutex access is required to make sure that the dying thread
        // does not have access to print queue. Same is true for event queue.

        // NOTE: iEventArrayMutex is now taken, so the ongoing thread
        // can't set any events so queue will be empty.
        iPrintHandler->Cancel();
        iEventHandler->Cancel();
        iInterferenceHandler->Cancel();
        iCommandHandler->Cancel();

        // Kill the thread
        iModuleContainer->KillThread( KErrCancel );

        // Complete the test execution request
        CompleteTestExecution( KErrCancel, TFullTestResult::ECaseCancelled, 
                               KErrCancel, KErrCancel, _L( "Cancelled" ) );
        iThreadState = ECancelled;

       
        /**        
        * Removed, because may block in certain situations. Implemented now in
        * TestCombiner instead.
        // Unset all pending state events, 
        // because we have killed the testcase abnormally
        // UnsetStateEvents();
        */
        
        // Complete print request if queue is empty
        CompletePrintRequestIfQueueEmpty();
        
        // Delete the module containter
        delete iModuleContainer;
        iModuleContainer = NULL;
        }
        
    // Relinquish mutex.
    iTestThreadMutex.Signal();

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: CancelRequestL

    Description: Cancel asynchronous request

    Parameters: const RMessage& aMessage      :in:  Message from client.
    
    Return Values: TInt                             Error code from CancelXXX-
                                                    function

    Errors/Exceptions: Function panics the client, if invalid message
                       received.

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestExecution::CancelRequestL( const RMessage2& aMessage )
    {
    TInt r = KErrNone;

    switch ( aMessage.Int0() )
        {
        case ETestExecutionRunTestCase:
            {
            r = CancelTestRequest();
            break;
            }
        case ETestExecutionNotifyProgress:
            {
            r = CancelPrintRequest();
            break;
            }
        case ETestExecutionNotifyEvent:
            {
            r = CancelEventRequest();
            break;
            }
        case ETestExecutionNotifyRemoteCmd:
            {
            r = CancelRemoteCmdRequest();
            break;
            }
        case ETestExecutionNotifyCommand:
            {
            r = CancelCommandRequest();
            break;
            }
        default:
            iModuleSession->PanicClient( EInvalidRequestCancel, aMessage );
            break;
        }

    aMessage.Complete( r );

    return r;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: SetThreadState

    Description: A multithread safe thread state modification function

    Parameters: const TTestState aState       :in:  New state
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestExecution::SetThreadState( const TTestState aState )
    {

    iThreadState = aState;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: Complete event

    Description: Completes pending event request. Must be 
        called with iEventSem taken.
        
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None
    
    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestExecution::CompleteEvent( TInt aError )
    {
    if( iEventDef.iStatus )
        {
        __TRACE( KMessage,(_L("Comp Stat %d, %x (error %d)"), 
            this, iEventDef.iStatus, aError));
        TestThreadRequestComplete( iEventDef.iStatus, aError );
        
        iEventDef.iStatus = NULL;
        
        }
        
    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: CompletePrintRequestIfQueueEmpty

    Description: Completes print request if queue is empty.

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestExecution::CompletePrintRequestIfQueueEmpty()
    {

    if ( iPrintNotifyAvailable &&  iPrintQueue->Count() == 0 )
        {
        iNotifyPrintMessage.Complete ( KErrEof );
        iPrintNotifyAvailable = EFalse;
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: CleanupEvents

    Description: Cleanups Events.

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestExecution::CleanupEvents()
    {
    
    iEventArray.ResetAndDestroy();
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: GetRq

    Description: Get request pointers.

    Parameters: None

    Return Values: TRequestStatus*

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
TRequestStatus* CTestExecution::GetRq( TRequestType aType )
    {

    TRequestStatus* status = NULL;

    switch( aType )
        {
        case ERqPrint:
            status = &iPrintHandler->iStatus;
            break;
        case ERqEvent:
            status = &iEventHandler->iStatus;
            break;
        case ERqSnd:
            status = &iSndHandler->iStatus;
            break;
        case ERqRcv:
            status = &iRcvHandler->iStatus;
            break;
        case ERqInterference:
            status = &iInterferenceHandler->iStatus;
            break;
        case ERqMeasurement:
            status = &iMeasurementHandler->iStatus;
            break;
        case ERqCommand:
            status = &iCommandHandler->iStatus;
            break;
        default:
            break;
        }

    return status;

    }


/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: CompleteTestExecution

    Description: Completes test execution

    Parameters: const TInt aCompletionCode    :in:  Request completion code
                const TInt aCaseExecutionCode  in:  Case execution code
                const TFullTestResult::TCaseExecutionResult aCaseExecutionType,
                const TInt aCaseResult        :in:  Case result
                const TDesC& aText            :in:  Completion text

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestExecution::CompleteTestExecution( const TInt aCompletionCode,
    const TFullTestResult::TCaseExecutionResult aCaseExecutionType,
    const TInt aCaseExecutionCode,
    const TInt aCaseResult,
    const TDesC& aText )
    {
    
    TInt completionCode = aCompletionCode;
    
    if( iModuleContainer != NULL )
        {
        // Fill the description
        iFullResult.iEndTime.HomeTime();
        iFullResult.iCaseExecutionResultType = aCaseExecutionType;
        iFullResult.iCaseExecutionResultCode = aCaseExecutionCode;
        iFullResult.iTestResult.iResult = aCaseResult;
        iFullResult.iTestResult.iResultDes = aText;
        CompleteTestExecution( aCompletionCode );
        return;
        }
    else
        {   
        completionCode = KErrGeneral;
        __TRACE ( KError, ( CStifLogger::ERed, 
            _L("CTestExecution::CompleteTestExecution: ModuleContainer NULL") ) );
        }

    iTestExeMessage.Complete ( completionCode );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: CompleteTestExecution

    Description: Completes test execution

    Parameters: const TInt aCompletionCode: in: completion code
                TFullTestResult&: in: test case result

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestExecution::CompleteTestExecution( const TInt aCompletionCode )
    {
    
    TInt completionCode = aCompletionCode;
    
    TFullTestResultPckg fullResultPckg( iFullResult );
    TRAPD( res, iTestExeMessage.WriteL( 0, fullResultPckg ) );
    if ( res != KErrNone )
        {
        completionCode = res;
        }

    iTestExeMessage.Complete ( completionCode );
    
    // @js Now it returns completionCode, used to return res

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: UnsetStateEvents

    Description: Unsets all state events. Must be done if testcase is exited 
                abnormally (i.e. aborted by the user).

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestExecution::UnsetStateEvents()
    {


    // Unset all state events that are set currently
    TInt count = iStateEvents.Count();
    TEventIf event;
    for( TInt i = 0; i < count; i++ )   
        {
        TPtrC name = *(iStateEvents[i]);
        event.Set( TEventIf::EUnsetEvent, name, TEventIf::EState );
        
        // Construct message
        TEventIfPckg eventPckg( event );

        // Write message to client space
        TRAPD( res, iNotifyEventMessage.WriteL( 0, eventPckg ) );

        // Complete with WriteL result
        iEventNotifyAvailable = EFalse;
        iNotifyEventMessage.Complete( res );
        if( res != KErrNone )
            {
            __TRACE( KError,( _L( "CTestExecution::UnsetStateEvents: WriteL failed" ) ));
            break;
            } 
        }
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: TestThreadRequestComplete

    Description: Complete test thread request.

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestExecution::TestThreadRequestComplete( TRequestStatus* aStatus, 
                                                TInt aCode)
    {                                        
    
    if( iThreadState != EOnGoing )
        {
        __TRACE( KError,
            ( _L( "CTestExecution::TestThreadRequestComplete: test thread killed" ) ));
        return;
        }

    if( iTestThreadOpen == EFalse )
        {        
        if( iTestThread.Open( iTestThreadId ) != KErrNone )
            {
            __TRACE( KError,
                ( _L( "CTestExecution::TestThreadRequestComplete: test thread cannot be opened" ) ));
            return;
            }
        iTestThreadOpen = ETrue;
        }
    if( iTestThread.ExitType() != EExitPending ) 
        {
        // test thread has died
        __TRACE( KError,
            ( _L( "CTestExecution::TestThreadRequestComplete: test thread has died" ) ));
        return;
        }    
     
    iTestThread.RequestComplete( aStatus, aCode );
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: CancelTestExecution

    Description: Cancels test(s) execution in special cases e.g. timeout,
                 exit etc.

    Parameters: None
    
    Return Values: TInt: Symbian error code.

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
TInt CTestExecution::CancelTestExecution()
    {
    TInt ret( KErrNone );
    ret = CancelTestRequest();

    return ret;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: KillTestinterferenceThread

    Description: Make sure that any of the test interference thread's won't
                 stay to run if test case is crashed of test interference
                 object is not deleted.

    Parameters: None

    Return Values: TInt: Symbian OS error code.

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestExecution::KillTestinterferenceThread()
    {
    for( TInt i = 0; i < iSTIFTestInterferenceArray.Count(); i++ )
        {
        RDebug::Print( 
            _L( "Test interference's thread[%x] killed by STIF" ), (TInt)iSTIFTestInterferenceArray[i].Id() );
        __TRACE( KInit,
            ( _L( "Test interference's thread[%x] killed by STIF" ), (TInt)iSTIFTestInterferenceArray[i].Id() ) );
        iSTIFTestInterferenceArray[i].Kill( KErrCancel );
        //iSTIFTestInterferenceArray.Remove( i );
        }

    iSTIFTestInterferenceArray.Reset();
    iSTIFTestInterferenceArray.Close();

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: DoNotifyCommand

    Description: Completes command message.

    Parameters: None.

    Return Values: Symbian OS error code.

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestExecution::DoNotifyCommand()
    {
    if(!iCommandNotifyAvailable)
    	{
    	__TRACE(KPrint, (_L("CTestExecution::DoNotifyCommand(): unable to complete notification")))
    	return KErrNone;
    	}

    TInt ret = KErrNone;

    switch(iCommandDef->iCommand)
        {
        case EStopExecution:
            {
            //Unpack received parameters
            TStopExecutionCommandParams par;
            TStopExecutionCommandParamsPckg parPack(par);
            parPack.Copy(iCommandDef->iParamsPckg);

            //Set test case handle
            par.iTestCaseHandle = iNotifyCommandMessage.Int3();

            __TRACE(KPrint, (_L("CTestExecution::DoNotifyCommand(): command [%d] type [%d] code [%d] test handle [%d]"), TInt(iCommandDef->iCommand), TInt(par.iType), TInt(par.iCode), par.iTestCaseHandle));
            iCommandDef->iParamsPckg.Copy(parPack);
            break;
            }
        case ESendTestModuleVersion:
        	{
        	TSendTestModuleVesionCommandParams par;
        	TSendTestModuleVesionCommandParamsPckg parPack(par);
        	parPack.Copy(iCommandDef->iParamsPckg);
        	
        	__TRACE(KPrint, (_L("CTestExecution::DoNotifyCommand(): command ESendTestModuleVersion")));
        	break;
        	}
        default:
            __TRACE(KError, (_L("CTestExecution::DoNotifyCommand(): Unknown command [%d]"), TInt(iCommandDef->iCommand)));
            return KErrNotFound;
        }
    // Construct message
    TPckg<TCommand> commandPckg(iCommandDef->iCommand);

    // Start first phase of the writing message to client space
    TRAP(ret, iNotifyCommandMessage.WriteL(0, commandPckg));
    if(ret != KErrNone)
       {
       __TRACE(KError, (_L( "CTestExecution::DoNotifyCommand(): WriteL failed (0)")));
       }

    TRAP(ret, iNotifyCommandMessage.WriteL(1, iCommandDef->iParamsPckg));
    if(ret != KErrNone)
       {
       __TRACE(KError, (_L( "CTestExecution::DoNotifyCommand(): WriteL failed (1)")));
       }

    // Complete request with WriteL result
    iCommandNotifyAvailable = EFalse;
    iNotifyCommandMessage.Complete(ret);

    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestExecution

    Method: KillTestMeasurement

    Description: Make sure that any of the test measurement operations won't
                 stay to run if test case is crashed or test measurement object
                 is not stopped by user.

    Parameters: None

    Return Values: TInt: Symbian OS error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestExecution::KillTestMeasurement()
    {
    for( TInt i = 0; i < iMeasurementHandlingArray.Count(); i++ )
        {
        iMeasurementHandlingArray[i]->iMeasurementModulePtr->Stop();
        }

    iMeasurementHandlingArray.ResetAndDestroy();
    iMeasurementHandlingArray.Close();

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CPrintHandler class member functions.
    CPrintHandler listens print notifications from test thread.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================


/*
-------------------------------------------------------------------------------

    Class: CPrintHandler

    Method: NewL

    Description: Constructs a new CPrintHandler object.

    Parameters: CTestExecution& aExecution: in: "Parent"

    Return Values: CPrintHandler*: New undertaker

    Errors/Exceptions: Leaves if memory allocation or ConstructL leaves.

    Status: Proposal

-------------------------------------------------------------------------------
*/
CPrintHandler* CPrintHandler::NewL( CTestExecution& aExecution )
    {

    CPrintHandler* self = new( ELeave ) CPrintHandler( aExecution );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CPrintHandler

    Method: ConstructL

    Description: Second level constructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CPrintHandler::ConstructL()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CPrintHandler

    Method: CPrintHandler

    Description: Constructor

    Parameters: CTestModuleContainer& aExecution :in:   "Parent"

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CPrintHandler::CPrintHandler( CTestExecution& aExecution ) :
    CActive( CActive::EPriorityStandard ),
    iExecution( aExecution )
    {
    
    CActiveScheduler::Add ( this );
    
    }

/*
-------------------------------------------------------------------------------

    Class: CPrintHandler

    Method: ~CPrintHandler

    Description: Destructor. 
    Cancels active request.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CPrintHandler::~CPrintHandler()
    {

    Cancel();

    }




/*
-------------------------------------------------------------------------------

    Class: CPrintHandler

    Method: StartL

    Description: Starts to monitor thread.

    Parameters: None

    Return Values: TInt                             Always KErrNone

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CPrintHandler::StartL()
    {
    
    __TRACE( KPrint, ( _L( "CPrintHandler::StartL" ) ) );
    
    if( IsActive() ) 
        {
        CTestServer::PanicServer( EAlreadyActive );
        }

    iStatus = KRequestPending;
    SetActive();
    
    // Signal test thread
    iExecution.iPrintSem.Signal();

    }

/*
-------------------------------------------------------------------------------

    Class: CPrintHandler

    Method: RunL

    Description: Handles thread death.
    Function does:
    1 ) Stops monitoring thread
    1 ) Marks thread death
    2 ) Completes ongoing requests
    3 ) Cleans the memory

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CPrintHandler::RunL()
    {

    __TRACE( KPrint, ( _L( "CPrintHandler::RunL [%d]" ), iStatus.Int() ) );
    
    iExecution.DoNotifyPrint();
    
    // enable print request
    //@speiExecution.iPrintHandler->StartL();
    StartL();
    }


/*
-------------------------------------------------------------------------------

    Class: CPrintHandler

    Method: DoCancel

    Description: Stops print notification listening.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/

void CPrintHandler::DoCancel()
    {

    __TRACE( KPrint, ( _L( "CPrintHandler::DoCancel" ) ) );

    iExecution.iPrintMutex.Wait();  // Take mutex and check test case print
                                    // operation. If pending take print semaphore
                                    // and complete
    if( iStatus == KRequestPending )
        {
        // Signal test thread
        // @remove iExecution.iPrintSem.Wait();
    
        TRequestStatus* status = &iStatus;    
        User::RequestComplete( status, KErrCancel );        
        }

    iExecution.iPrintMutex.Signal();
    }

/*
-------------------------------------------------------------------------------

    Class: CPrintHandler

    Method: RunError

    Description: Handle errors. RunL function does not leave, so one should
    never come here. 

    Print trace and let framework handle error( i.e to do Panic )

    Parameters: TInt aError:                  :in:  Error code

    Return Values:  TInt                            Error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CPrintHandler::RunError( TInt aError )
    {
    
    __TRACE( KError,( _L( "CPrintHandler::RunError" ) ) );

    return aError;
    
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CEventHandler class member functions.
    CEventHandler listens print notifications from test thread.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================


/*
-------------------------------------------------------------------------------

    Class: CEventHandler

    Method: NewL

    Description: Constructs a new CEventHandler object.

    Parameters: CTestExecution& aExecution: in: "Parent"

    Return Values: CEventHandler*: New undertaker

    Errors/Exceptions: Leaves if memory allocation or ConstructL leaves.

    Status: Proposal

-------------------------------------------------------------------------------
*/
CEventHandler* CEventHandler::NewL( CTestExecution& aExecution )
    {

    CEventHandler* self = new( ELeave ) CEventHandler( aExecution );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CEventHandler

    Method: ConstructL

    Description: Second level constructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CEventHandler::ConstructL()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CEventHandler

    Method: CEventHandler

    Description: Constructor

    Parameters: CTestModuleContainer& aExecution :in:   "Parent"

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CEventHandler::CEventHandler( CTestExecution& aExecution ) :
    CActive( CActive::EPriorityStandard ),
    iExecution( aExecution )
    {
    
    CActiveScheduler::Add ( this );
    
    }

/*
-------------------------------------------------------------------------------

    Class: CEventHandler

    Method: ~CEventHandler

    Description: Destructor. 
    Cancels active request.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CEventHandler::~CEventHandler()
    {

    Cancel();

    }




/*
-------------------------------------------------------------------------------

    Class: CEventHandler

    Method: StartL

    Description: Starts to monitor thread.

    Parameters: None

    Return Values: TInt                             Always KErrNone

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CEventHandler::StartL()
    {
    
    __TRACE( KPrint, ( _L( "CEventHandler::StartL" ) ) );

    if( IsActive() ) 
        {
        CTestServer::PanicServer( EAlreadyActive );
        }
    
    iStatus = KRequestPending;
    SetActive();
    
    // Signal test thread
    iExecution.iEventSem.Signal();

    }

/*
-------------------------------------------------------------------------------

    Class: CEventHandler

    Method: RunL

    Description: Handles thread death.
    Function does:
    1 ) Stops monitoring thread
    1 ) Marks thread death
    2 ) Completes ongoing requests
    3 ) Cleans the memory

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CEventHandler::RunL()
    {

    __TRACE( KPrint, ( _L( "CEventHandler::RunL [%d]" ), iStatus.Int() ) );

    switch( iExecution.EventDef().iType )
        {
        case TEventDef::EEventCmd:
            iExecution.DoNotifyEvent();
            break;
        case TEventDef::EEventCmdCancel:
            iExecution.CancelEvent();
            break;
        default:
            CTestServer::PanicServer( EUnknownEventCmd );
            break;  
        }
         
    }


/*
-------------------------------------------------------------------------------

    Class: CEventHandler

    Method: DoCancel

    Description: Stops print notification listening.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/

void CEventHandler::DoCancel()
    {

    __TRACE( KPrint, ( _L( "CEventHandler::DoCancel" ) ) );
    
    iExecution.iEventMutex.Wait();  // Take mutex and check test case event
                                    // operation. If pending take event
                                    // semaphore and complete
    if( iStatus == KRequestPending )
        {
        // Signal test thread
        // @remove iExecution.iEventSem.Wait();

        TRequestStatus* status = &iStatus;    
        User::RequestComplete( status, KErrCancel );

        }

    iExecution.iEventMutex.Signal();

    }

/*
-------------------------------------------------------------------------------

    Class: CEventHandler

    Method: RunError

    Description: Handle errors. RunL function does not leave, so one should
    never come here. 

    Print trace and let framework handle error( i.e to do Panic )

    Parameters: TInt aError:                  :in:  Error code

    Return Values:  TInt                            Error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CEventHandler::RunError( TInt aError )
    {
    
    __TRACE( KError,( _L( "CEventHandler::RunError" ) ) );

    return aError;
    
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CSndHandler class member functions.
    CSndHandler listens print notifications from test thread.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================


/*
-------------------------------------------------------------------------------

    Class: CSndHandler

    Method: NewL

    Description: Constructs a new CSndHandler object.

    Parameters: CTestExecution& aExecution: in: "Parent"

    Return Values: CSndHandler*: New undertaker

    Errors/Exceptions: Leaves if memory allocation or ConstructL leaves.

    Status: Proposal

-------------------------------------------------------------------------------
*/
CSndHandler* CSndHandler::NewL( CTestExecution& aExecution )
    {

    CSndHandler* self = new( ELeave ) CSndHandler( aExecution );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CSndHandler

    Method: ConstructL

    Description: Second level constructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CSndHandler::ConstructL()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CSndHandler

    Method: CSndHandler

    Description: Constructor

    Parameters: CTestModuleContainer& aExecution :in:   "Parent"

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CSndHandler::CSndHandler( CTestExecution& aExecution ) :
    CActive( CActive::EPriorityStandard ),
    iExecution( aExecution )
    {
    
    CActiveScheduler::Add ( this );
    
    }

/*
-------------------------------------------------------------------------------

    Class: CSndHandler

    Method: ~CSndHandler

    Description: Destructor. 
    Cancels active request.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CSndHandler::~CSndHandler()
    {

    Cancel();

    }




/*
-------------------------------------------------------------------------------

    Class: CSndHandler

    Method: StartL

    Description: Starts to monitor thread.

    Parameters: None

    Return Values: TInt                             Always KErrNone

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CSndHandler::StartL()
    {
    
    __TRACE( KPrint, ( _L( "CSndHandler::StartL" ) ) );

    if( IsActive() ) 
        {
        CTestServer::PanicServer( EAlreadyActive );
        }
    
    iStatus = KRequestPending;
    SetActive();
    
    // Signal test thread
    iExecution.iSndSem.Signal();

    }

/*
-------------------------------------------------------------------------------

    Class: CSndHandler

    Method: RunL

    Description: Handles thread death.
    Function does:
    1 ) Stops monitoring thread
    1 ) Marks thread death
    2 ) Completes ongoing requests
    3 ) Cleans the memory

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CSndHandler::RunL()
    {

    __TRACE( KPrint, ( _L( "CSndHandler::RunL [%d]" ), iStatus.Int() ) );
    
    iExecution.DoRemoteSend();
       
    }


/*
-------------------------------------------------------------------------------

    Class: CSndHandler

    Method: DoCancel

    Description: Stops print notification listening.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/

void CSndHandler::DoCancel()
    {

    __TRACE( KPrint, ( _L( "CSndHandler::DoCancel" ) ) );

    iExecution.iSndMutex.Wait();  // Take mutex and check test case send
                                  // operation. If pending take send
                                  // semaphore and complete
    if( iStatus == KRequestPending )
        {
        // Signal test thread
        // @remove iExecution.iSndSem.Wait();

        TRequestStatus* status = &iStatus;    
        User::RequestComplete( status, KErrCancel );

        }

    iExecution.iSndMutex.Signal();
    
    }

/*
-------------------------------------------------------------------------------

    Class: CSndHandler

    Method: RunError

    Description: Handle errors. RunL function does not leave, so one should
    never come here. 

    Print trace and let framework handle error( i.e to do Panic )

    Parameters: TInt aError:                  :in:  Error code

    Return Values:  TInt                            Error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CSndHandler::RunError( TInt aError )
    {
    
    __TRACE( KError,( _L( "CSndHandler::RunError" ) ) );

    return aError;
    
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CSndHandler class member functions.
    CRcvHandler listens print notifications from test thread.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================


/*
-------------------------------------------------------------------------------

    Class: CRcvHandler

    Method: NewL

    Description: Constructs a new CRcvHandler object.

    Parameters: CTestExecution& aExecution: in: "Parent"

    Return Values: CRcvHandler*: New undertaker

    Errors/Exceptions: Leaves if memory allocation or ConstructL leaves.

    Status: Proposal

-------------------------------------------------------------------------------
*/
CRcvHandler* CRcvHandler::NewL( CTestExecution& aExecution )
    {

    CRcvHandler* self = new( ELeave ) CRcvHandler( aExecution );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CRcvHandler

    Method: ConstructL

    Description: Second level constructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CRcvHandler::ConstructL()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CRcvHandler

    Method: CRcvHandler

    Description: Constructor

    Parameters: CTestModuleContainer& aExecution :in:   "Parent"

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CRcvHandler::CRcvHandler( CTestExecution& aExecution ) :
    CActive( CActive::EPriorityStandard ),
    iExecution( aExecution )
    {
    
    CActiveScheduler::Add ( this );
    
    }

/*
-------------------------------------------------------------------------------

    Class: CRcvHandler

    Method: ~CRcvHandler

    Description: Destructor. 
    Cancels active request.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CRcvHandler::~CRcvHandler()
    {

    Cancel();

    }




/*
-------------------------------------------------------------------------------

    Class: CRcvHandler

    Method: StartL

    Description: Starts to monitor thread.

    Parameters: None

    Return Values: TInt                             Always KErrNone

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CRcvHandler::StartL()
    {
    
    __TRACE( KPrint, ( _L( "CRcvHandler::StartL" ) ) );
    
    if( IsActive() ) 
        {
        CTestServer::PanicServer( EAlreadyActive );
        }
    iStatus = KRequestPending;
    SetActive();
    
    // Signal test thread
    iExecution.iRcvSem.Signal();

    }

/*
-------------------------------------------------------------------------------

    Class: CRcvHandler

    Method: RunL

    Description: Handles thread death.
    Function does:
    1 ) Stops monitoring thread
    1 ) Marks thread death
    2 ) Completes ongoing requests
    3 ) Cleans the memory

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CRcvHandler::RunL()
    {

    __TRACE( KPrint, ( _L( "CRcvHandler::RunL [%d]" ), iStatus.Int() ) );
    
    iExecution.DoRemoteReceive();
       
    }


/*
-------------------------------------------------------------------------------

    Class: CRcvHandler

    Method: DoCancel

    Description: Stops print notification listening.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/

void CRcvHandler::DoCancel()
    {

    __TRACE( KPrint, ( _L( "CRcvHandler::DoCancel" ) ) );

    iExecution.iRcvMutex.Wait();  // Take mutex and check test case receive
                                  // operation. If pending take reveive
                                  // semaphore and complete
    if( iStatus == KRequestPending )
        {
        // Signal test thread
        // @remove iExecution.iRcvSem.Wait();

        TRequestStatus* status = &iStatus;    
        User::RequestComplete( status, KErrCancel );
        }
        
    iExecution.iRcvMutex.Signal();

    }

/*
-------------------------------------------------------------------------------

    Class: CRcvHandler

    Method: RunError

    Description: Handle errors. RunL function does not leave, so one should
    never come here. 

    Print trace and let framework handle error( i.e to do Panic )

    Parameters: TInt aError:                  :in:  Error code

    Return Values:  TInt                            Error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CRcvHandler::RunError( TInt aError )
    {
    
    __TRACE( KError,( _L( "CRcvHandler::RunError" ) ) );

    return aError;
    
    }


/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CTestInterferenceHandler class 
    member functions. CTestInterferenceHandler listens print notifications from
    test thread.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================


/*
-------------------------------------------------------------------------------

    Class: CInterferenceHandler

    Method: NewL

    Description: Constructs a new CInterferenceHandler object.

    Parameters: CTestExecution& aExecution: in: "Parent"

    Return Values: CInterferenceHandler*: New undertaker

    Errors/Exceptions: Leaves if memory allocation or ConstructL leaves.

    Status: Proposal

-------------------------------------------------------------------------------
*/
CInterferenceHandler* CInterferenceHandler::NewL( 
                                                CTestExecution& aExecution )
    {
    CInterferenceHandler* self = 
                        new( ELeave ) CInterferenceHandler( aExecution );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CInterferenceHandler

    Method: ConstructL

    Description: Second level constructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CInterferenceHandler::ConstructL()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CInterferenceHandler

    Method: CInterferenceHandler

    Description: Constructor

    Parameters: CTestModuleContainer& aExecution :in: "Parent"

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CInterferenceHandler::CInterferenceHandler( CTestExecution& aExecution ) :
    CActive( CActive::EPriorityStandard ),
    iExecution( aExecution )
    {
    CActiveScheduler::Add ( this );
    
    }

/*
-------------------------------------------------------------------------------

    Class: CInterferenceHandler

    Method: ~CInterferenceHandler

    Description: Destructor. 
    Cancels active request.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CInterferenceHandler::~CInterferenceHandler()
    {
    Cancel();

    }

/*
-------------------------------------------------------------------------------

    Class: CInterferenceHandler

    Method: StartL

    Description: Starts to monitor thread.

    Parameters: None

    Return Values: TInt: Always KErrNone

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CInterferenceHandler::StartL()
    {
    __TRACE( KPrint, ( _L( "CInterferenceHandler::StartL" ) ) );
    
    if( IsActive() ) 
        {
        CTestServer::PanicServer( EAlreadyActive );
        }

    iStatus = KRequestPending;
    SetActive();
    
    // Signal test thread, Notify test thread that operation is done.
    iExecution.iInterferenceSem.Signal();

    }

/*
-------------------------------------------------------------------------------

    Class: CInterferenceHandler

    Method: RunL

    Description: Handles thread death.
    Function does:
    1 ) Stops monitoring thread
    1 ) Marks thread death
    2 ) Completes ongoing requests
    3 ) Cleans the memory

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CInterferenceHandler::RunL()
    {
    __TRACE( KPrint, ( _L( "CInterferenceHandler::RunL [%d]" ), iStatus.Int() ) );
    
    iExecution.DoNotifyInterference();
    
    StartL();

    }

/*
-------------------------------------------------------------------------------

    Class: CInterferenceHandler

    Method: DoCancel

    Description: Stops interference notification listening.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CInterferenceHandler::DoCancel()
    {
    __TRACE( KPrint, ( _L( "CInterferenceHandler::DoCancel" ) ) );

    iExecution.iInterferenceMutex.Wait();   // Take mutex and check test
                                            // interference operation. If
                                            // pending take interference
                                            // semaphore and complete
    if( iStatus == KRequestPending )
        {
    
        TRequestStatus* status = &iStatus;    
        User::RequestComplete( status, KErrCancel );
        }

    iExecution.iInterferenceMutex.Signal();

    }

/*
-------------------------------------------------------------------------------

    Class: CInterferenceHandler

    Method: RunError

    Description: Handle errors. RunL function does not leave, so one should
    never come here. 

    Print trace and let framework handle error( i.e to do Panic )

    Parameters: TInt aError: in: Error code

    Return Values:  TInt: Error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CInterferenceHandler::RunError( TInt aError )
    {
    __TRACE( KError,( _L( "CTestInterferenceHandler::RunError" ) ) );

    return aError;

    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CMeasurementHandler class 
    member functions. CMeasurementHandler handles test measurement operations
    that comes from test execution side to TestServer side.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================


/*
-------------------------------------------------------------------------------

    Class: CMeasurementHandler

    Method: NewL

    Description: Constructs a new CMeasurementHandler object.

    Parameters: CTestExecution& aExecution: in: Pointer to test execution side.

    Return Values: CMeasurementHandler*: New undertaker

    Errors/Exceptions: Leaves if memory allocation or ConstructL leaves.

    Status: Approved

-------------------------------------------------------------------------------
*/
CMeasurementHandler* CMeasurementHandler::NewL( CTestExecution& aExecution )
    {
    CMeasurementHandler* self = 
                        new( ELeave ) CMeasurementHandler( aExecution );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CMeasurementHandler

    Method: ConstructL

    Description: Second level constructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CMeasurementHandler::ConstructL()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CMeasurementHandler

    Method: CMeasurementHandler

    Description: Constructor

    Parameters: CTestModuleContainer& aExecution :in: Pointer to test
                execution side.

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CMeasurementHandler::CMeasurementHandler( CTestExecution& aExecution ) :
    CActive( CActive::EPriorityStandard ),
    iExecution( aExecution )
    {
    CActiveScheduler::Add ( this );

    }

/*
-------------------------------------------------------------------------------

    Class: CMeasurementHandler

    Method: ~CMeasurementHandler

    Description: Destructor. 
    Cancels active request.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CMeasurementHandler::~CMeasurementHandler()
    {
    Cancel();

    }

/*
-------------------------------------------------------------------------------

    Class: CMeasurementHandler

    Method: StartL

    Description: Starts to monitor thread.

    Parameters: None

    Return Values: TInt: Always KErrNone

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CMeasurementHandler::StartL()
    {
    __TRACE( KPrint, ( _L( "CMeasurementHandler::StartL" ) ) );

    if( IsActive() ) 
        {
        CTestServer::PanicServer( EAlreadyActive );
        }

    iStatus = KRequestPending;
    SetActive();

    // Do not Signal here because synchronous operations will be executed and we
    // don't want to signal at first time: iExecution.iMeasurementSem.Signal();
    // Signal() operation will be handled in RunL in this case.

    }

/*
-------------------------------------------------------------------------------

    Class: CMeasurementHandler

    Method: RunL

    Description: Derived from CActive, handles test measurement execution.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CMeasurementHandler::RunL()
    {
    __TRACE( KPrint, ( _L( "CMeasurementHandler::RunL [%d]" ), iStatus.Int() ) );
    
    TInt ret( 0 );
    ret = iExecution.DoNotifyMeasurement();

    // ret value is copied to struct so it can be handled later.
    iExecution.iTestMeasurement.iMeasurementStruct.iOperationResult = ret;
    
    // SetActive again
    StartL();

    // TestServer side operations are done, Signal that operation is done.
    iExecution.iMeasurementSem.Signal();

    }

/*
-------------------------------------------------------------------------------

    Class: CMeasurementHandler

    Method: DoCancel

    Description: Stops measurement notification listening.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
void CMeasurementHandler::DoCancel()
    {
    __TRACE( KPrint, ( _L( "CMeasurementHandler::DoCancel" ) ) );

    iExecution.iMeasurementMutex.Wait();    // Take mutex and check test
                                            // measurement operation. If
                                            // pending take measurement
                                            // semaphore and complete
    if( iStatus == KRequestPending )
        {
    
        TRequestStatus* status = &iStatus;    
        User::RequestComplete( status, KErrCancel );
        }

    iExecution.iMeasurementMutex.Signal();

    }

/*
-------------------------------------------------------------------------------

    Class: CMeasurementHandler

    Method: RunError

    Description: Handle errors. RunL function does not leave, so one should
    never come here. 

    Print trace and let framework handle error( i.e to do Panic )

    Parameters: TInt aError: in: Error code

    Return Values:  TInt: Error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CMeasurementHandler::RunError( TInt aError )
    {
    __TRACE( KError,( 
        _L( "CMeasurementHandler::RunError with error: %d" ), aError ) );

    return aError;

    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CCommandHandler class member functions.
    CCommandHandler listens command notifications from test thread.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================


/*
-------------------------------------------------------------------------------

    Class: CCommandHandler

    Method: NewL

    Description: Constructs a new CCommandHandler object.

    Parameters: CTestExecution& aExecution: in: "Parent"

    Return Values: CCommandHandler*: New object

    Errors/Exceptions: Leaves if memory allocation or ConstructL leaves.

    Status: Proposal

-------------------------------------------------------------------------------
*/
CCommandHandler* CCommandHandler::NewL(CTestExecution& aExecution)
    {
    CCommandHandler* self = new (ELeave) CCommandHandler(aExecution);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/*
-------------------------------------------------------------------------------

    Class: CCommandHandler

    Method: ConstructL

    Description: Second level constructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CCommandHandler::ConstructL()
    {
    }

/*
-------------------------------------------------------------------------------

    Class: CCommandHandler

    Method: CCommandHandler

    Description: Constructor

    Parameters: CTestExecution& aExecution :in:   "Parent"

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
CCommandHandler::CCommandHandler(CTestExecution& aExecution):
    CActive(CActive::EPriorityStandard),
    iExecution(aExecution)
    {
    CActiveScheduler::Add (this);
    }

/*
-------------------------------------------------------------------------------

    Class: CCommandHandler

    Method: ~CCommandHandler

    Description: Destructor.
    Cancels active request.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
CCommandHandler::~CCommandHandler()
    {
    Cancel();
    }

/*
-------------------------------------------------------------------------------

    Class: CCommandHandler

    Method: StartL

    Description: Starts to monitor thread.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CCommandHandler::StartL()
    {
    __TRACE(KPrint, (_L( "CCommandHandler::StartL" )));

    if(IsActive())
        {
        CTestServer::PanicServer(EAlreadyActive);
        }
    iStatus = KRequestPending;
    SetActive();

    // Signal test thread
    iExecution.iCommandSem.Signal();
    }

/*
-------------------------------------------------------------------------------

    Class: CCommandHandler

    Method: RunL

    Description:

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CCommandHandler::RunL()
    {
    __TRACE(KPrint, (_L( "CCommandHandler::RunL [%d]"), iStatus.Int()));

    iExecution.DoNotifyCommand();
//    StartL();
    }


/*
-------------------------------------------------------------------------------

    Class: CCommandHandler

    Method: DoCancel

    Description: Stops command notification listening.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/

void CCommandHandler::DoCancel()
    {
    __TRACE(KPrint, (_L("CCommandHandler::DoCancel")));

    iExecution.iCommandMutex.Wait();  // Take mutex and check test case receive
                                      // operation. If pending take reveive
                                      // semaphore and complete
    if(iStatus == KRequestPending)
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrCancel);
        }

    iExecution.iCommandMutex.Signal();
    }

/*
-------------------------------------------------------------------------------

    Class: CCommandHandler

    Method: RunError

    Description: Handle errors. RunL function does not leave, so one should
    never come here.

    Print trace and let framework handle error( i.e to do Panic )

    Parameters: TInt aError:                  :in:  Error code

    Return Values:  TInt                            Error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CCommandHandler::RunError(TInt aError)
    {
    __TRACE(KError, (_L("CCommandHandler::RunError")));

    return aError;
    }

/*
-------------------------------------------------------------------------------

    Class: CCommandDef

    Method: CCommandDef

    Description: private constructor

    Return Values:  void

-------------------------------------------------------------------------------
*/

CCommandDef::CCommandDef()
	{
	}

/*
-------------------------------------------------------------------------------

    Class: CCommandDef

    Method: ConstructL

    Description: Second phase constructor

    Return Values:  void

-------------------------------------------------------------------------------
*/

void CCommandDef::ConstructL()
	{

	}

/*
-------------------------------------------------------------------------------

    Class: CCommandDef

    Method: NewL

    Description: First phase constructor

    Return Values:  void

-------------------------------------------------------------------------------
*/

CCommandDef* CCommandDef::NewL()
	{
	CCommandDef* self = new(ELeave) CCommandDef;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/*
-------------------------------------------------------------------------------

    Class: CCommandDef

    Method: ~CCommandDef

    Description: destructor

-------------------------------------------------------------------------------
*/

CCommandDef::~CCommandDef()
	{

	}

// End of File
