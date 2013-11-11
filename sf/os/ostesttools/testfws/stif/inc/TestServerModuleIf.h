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
* Description: This file contains the header file of the CTestExecution.
*
*/

#ifndef TEST_SERVER_MODULE_IF_H
#define TEST_SERVER_MODULE_IF_H


//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <StifTestModule.h>
#include <StifTestInterface.h>
#include "StifTFwIfProt.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES
typedef CSTIFTestMeasurementImplementation*( 
        *CTestMeasurementFactory )( const TDesC&,
                                    CSTIFTestMeasurement::TSTIFMeasurementType );

// FORWARD DECLARATIONS
class TEvent;
class CTestModule;
class CTestModuleContainer;
class CPrintQueue;
class CPrintHandler;
class CEventHandler;
class CSndHandler;
class CRcvHandler;
class CErrorPrintHandler;
class CInterferenceHandler;
class CMeasurementHandler;
class CCommandHandler;

// CLASS DECLARATION

// DESCRIPTION
// TCmdDef represents remote command
class TCmdDef
    {
    public:
        TStifCommand    iCommand;
        TParams         iParam;
        TInt            iLen;
        TRequestStatus* iStatus;
    };

// TCommandDef represents command
class TCommandDef
    {
    public:
        TCommand                       iCommand;
        TBuf8<KMaxCommandParamsLength2> iParamsPckg;
    };
    
// DESCRIPTION
// CCommandDef represents command
class CCommandDef : public CBase
    {
    public: // Enumerations

    private: // Enumerations
        
    public:  // Constructors and destructor
    		static CCommandDef* NewL();
        	~CCommandDef();

    private:  // Constructors and destructor
    		CCommandDef();
        	void ConstructL();

    public: // New functions
          
    public: // Functions from base classes
        
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:
        
    public:		//Data
            TCommand iCommand;
            TBuf8<KMaxCommandParamsLength2> iParamsPckg;
        
    protected:  // Data
        
    private:    // Data
        
    public:     // Friend classes
        
    protected:  // Friend classes
        
    private:    // Friend classes
        
    };

// CLASS DECLARATION

// DESCRIPTION
// TCmdDef represents remote command
class TEventDef
    {
    public:
        enum TEventCmdType
            {
            EEventCmd,
            EEventCmdCancel,
            };
    // Added constructor to TEventDef @js
        TEventDef():iStatus(NULL){};
            
    public:
        TEventCmdType   iType;
        TEventIf        iEvent;
        TRequestStatus* iStatus;
    };

// DESCRIPTION
// TTestInterference represents remote command
class TTestInterference 
    {
    public:
        enum TOperation
            {
            EAppend,
            ERemove,
            };
    public:
        TTestInterference(){};
    public:
        // Thread id.
        TUint iThreadId;
        // Is operations append or remove.
        TOperation iOperation;
    };

// DESCRIPTION
// TTestMeasurement contains measurement related information
// transfering between TestInterface and TestServer.
class TTestMeasurement 
    {
    public:
        TTestMeasurement(){};
    public:
        // Struct for information.
        CSTIFTestMeasurement::TStifMeasurementStruct iMeasurementStruct;

    };

// DESCRIPTION
// TMeasurementHandling keeps information for measurement module
// dynamic loading, types, etc.
class TMeasurementHandling 
    {
    public:
        TMeasurementHandling() : iMeasurement( NULL ),
                                 iMeasurementModulePtr( NULL ){};

        ~TMeasurementHandling()
            { 
            delete iMeasurement;
            delete iMeasurementModulePtr;
            // After close test measurement's pointer is not valid anymore.
            if ( iMeasurementModule.Handle() )
                { 
                iMeasurementModule.Close(); 
                }
            };
    public:
        RLibrary iMeasurementModule;
        CTestMeasurementFactory iMeasurementLibEntry;
        CSTIFTestMeasurementImplementation* iMeasurement;
        CSTIFTestMeasurement::TSTIFMeasurementType iMeasurementType;
        CSTIFTestMeasurementImplementation* iMeasurementModulePtr;
    };

// CLASS DECLARATION

// DESCRIPTION
// CTestExecution represents a subsession object in the CTestServer.
// A session may own any number of CTestExecution objects
class CTestExecution
        :public CObject
    {

    public: // Enumerations
        enum TTestState
            {
            ENotStarted,
            EOnGoing,
            EFinished,
            ECancelled,
            };
        
        enum TRequestType
            {
            ERqPrint,
            ERqEvent,
            ERqSnd,
            ERqRcv,
            ERqInterference,
            ERqMeasurement,
            ERqCommand,
            };

        enum TTestThreadFailure
            {
            ETestThreadOk       = 0,
            ETestMemLeak        = 0x00000001,
            ETestRequestLeak    = 0x00000002,
            ETestHandleLeak     = 0x00000004,
            };

    private: // Enumerations
        // None

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CTestExecution* NewL( CTestModule* aModuleSession, 
                                     TInt aCaseNumber, 
                                     const TFileName& aConfig
                                   );

        /**
        * Destructor
        */
        virtual ~CTestExecution();

    public: // New functions

        /**
        * Close Test Execution
        */
        TInt CloseTestExecution( const RMessage2& aMessage );

        /**
        * Run a test case
        */
        TInt RunTestCase( const RMessage2& aMessage );

        /**
        * Sets a print notification
        */
        TInt NotifyPrint( const RMessage2& aMessage );

        /**
        * Sets a event notification
        */
        TInt NotifyEvent( const RMessage2& aMessage );

        /**
        * Sets a RemoteCmd notification
        */
        TInt NotifyRemoteCmd( const RMessage2& aMessage );

        /**
        * Reads a RemoteCmd notification
        */
        TInt ReadRemoteCmdInfo( const RMessage2& aMessage );

        /**
        * Resumes a paused test case
        */
        TInt Resume( const RMessage2& aMessage );

        /**
        * Pauses ongoing test case
        */
        TInt Pause( const RMessage2& aMessage );


        /**
        * Cancels asynchronous request
        */
        TInt CancelRequestL( const RMessage2& aMessage );


        /**
        * Completes a print request. 
        */
        void DoNotifyPrint();

        /**
        * Completes a interference request. 
        */
        void DoNotifyInterference();

        /**
        * Completes a measurement request. 
        */
        TInt DoNotifyMeasurement();

        /**
        * Completes a print request. 
        */
        void DoErrorPrint();

        /**
        * Enables remote command receiving. 
        */
        void DoRemoteReceive();

        /**
        * Sends remote commands. 
        */
        void DoRemoteSend();
        
        /**
        * Completes a event request. 
        */
        TInt DoNotifyEvent();

        /**
        * Completes a event cancel request. 
        */
        void CancelEvent();
        
        /** 
        * Set thread State. 
        * This function can be called from test execution thread.
        */
        void SetThreadState( const TTestState aState );
        
        /**
        * Completes print request if print queue is empty
        */
        void CompletePrintRequestIfQueueEmpty();

        /**
        * Completes print request if print queue is empty
        */
        void CleanupEvents();

        /**
        * Completes test execution
        */
        void CompleteTestExecution( const TInt aCompletionCode, 
                                    const TFullTestResult::TCaseExecutionResult aCaseExecutionType,
                                    const TInt aCaseExecutionCode,
                                    const TInt aCaseResult,
                                    const TDesC& aText );

        /**
        * Unsets all pending state events.
        */
        void UnsetStateEvents();

        /**
        * Completes test execution
        */
        void CompleteTestExecution( const TInt aCompletionCode );
        
        /**
        * Current thread state.
        */
        TTestState ThreadState(){ return iThreadState; }

        /**
        * Cancels test(s) execution in special cases e.g. timeout, exit etc.
        */
        TInt CancelTestExecution();

        /**
        * Make sure that any of the test interference thread's won't stay
        * to run if test case is crashed of test interference object is not
        * deleted.
        */
        TInt KillTestinterferenceThread();

        /**
        * Make sure that any of the test measurement process's won't stay
        * to run if test case is crashed of test measurement object is not
        * deleted.
        */
        TInt KillTestMeasurement();

        /**
        * Executes command for ongoing test case.
        * Created to allow test case to kill itself.
        */
        TInt NotifyCommand(const RMessage2& aMessage);

        /**
        * Completes a command request.
        * Created to allow test case to kill itself.
        */
        TInt DoNotifyCommand();

        /**
        * Cancels command request.
        * Created to allow test case to kill itself.
        */
        TInt CancelCommandRequest();

        /**
         * Get test case arguments.
         */
        const TDesC& GetTestCaseArguments() const;

    public: // Functions that are called from test thread

        /**
        * Get mutex handles.
        */
        TInt EventMutexHandle(){ return iEventMutex.Handle(); };
        TInt SndMutexHandle(){ return iSndMutex.Handle(); };
        TInt RcvMutexHandle(){ return iRcvMutex.Handle(); };
        TInt PrintMutexHandle(){ return iPrintMutex.Handle(); };
        TInt InterferenceMutexHandle(){ return iInterferenceMutex.Handle(); };
        TInt MeasurementMutexHandle(){ return iMeasurementMutex.Handle(); };
        TInt CommandMutexHandle(){ return iCommandMutex.Handle(); };

        /**
        * Get semaphore handles.
        */
        TInt EventSemHandle(){ return iEventSem.Handle(); };
        TInt SndSemHandle(){ return iSndSem.Handle(); };
        TInt RcvSemHandle(){ return iRcvSem.Handle(); };
        //TInt ReceiverSemHandle(){ return iReceiverSem.Handle(); };
        TInt PrintSemHandle(){ return iPrintSem.Handle(); };
        TInt InterferenceSemHandle(){ return iInterferenceSem.Handle(); };
        TInt MeasurementSemHandle(){ return iMeasurementSem.Handle(); };
        TInt CommandSemHandle(){ return iCommandSem.Handle(); };

        /**
        * Set execution test thread id.
        */ 
        void SetTestThread( TThreadId aThreadId ){ iTestThreadId = aThreadId; };

        /**
        * Access to "shared" memory.
        */
        TFullTestResult& FullResult(){ return iFullResult; };
        TTestProgress& TestProgress(){ return iProgress; };
        TEventDef& EventDef(){ return iEventDef; };
        TCmdDef& SndInfo(){ return iCmdDef; };
        TCmdDef& RcvInfo(){ return iRcvCmdDef; };
        TInt& TestThreadFailure(){ return iTestThreadFailure; };
        TTestInterference& TestInterference(){ return iTestInterference; };
        TTestMeasurement& TestMeasurement(){ return iTestMeasurement; };
        CCommandDef& CommandDef(){ return *iCommandDef; };

        /**
        * Get specific TRequestStatus variable.
        */
        TRequestStatus* GetRq( TRequestType aType );

        /**
        * Get mutex handle.
        * This mutex meaning is to make sure that test complete and test
        * case cancel operations are not executed at the same time.
        */
        TInt TestThreadMutexHandle() { return iTestThreadMutex.Handle(); };

        const TDesC& GetConfigFileName() const { return iConfig; }
    public: // Functions from base classes
        // None

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CTestExecution(); 

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CTestModule* aModuleSession,
                         TInt aCaseNumber,
                         const TFileName& aConfig );

        /**
        * Cancels print request.
        */
        TInt CancelPrintRequest();

        /**
        * Cancels test interference request.
        */
        TInt CancelInterferenceRequest();

        /**
        * Cancels test execution request
        */
        TInt CancelTestRequest();

        /**
        * Cancel event request.
        */
        TInt CancelEventRequest();

        /**
        * Cancel event request.
        */
        TInt CancelRemoteCmdRequest();

        /**
        * Writes a print notification to client memory space.
        */
        TInt WritePrint( TTestProgress& aProgress );

        /**
        * Complete event TRequestStatus if pending. 
        */
        void CompleteEvent( TInt aError );

        /**
        * Handle event unsets. 
        */
        TInt UnsetEvent( TEventIf& aEvent, const RMessage2& aMessage );

        /**
        * Handle event request. 
        */
        TInt RequestEvent( TEventIf& aEvent );

        /**
        * Wait event request. 
        */
        TInt WaitEvent( TEventIf& aEvent, TRequestStatus* aStatus );

        /**
        * Handle event releases. 
        */
        TInt ReleaseEvent( TEventIf& aEvent );

        /**
        * Handle state events. 
        */
        TInt EventStateChange( TEventIf& aEvent );

        /**
        * Complete request in test thread.
        */
        void TestThreadRequestComplete( TRequestStatus* aStatus, TInt aCode );

    public:     // Data
        // None
    protected:  // Data
        // None

    private:    // Data

        // Pointers to things around
        CTestModule* iModuleSession;             // Session owning us
        CTestModuleContainer* iModuleContainer;  // Container used to execute this case

        // Test execution related members
        TInt iCaseNumber;                        // Case to be executed
        TPtr iConfig;                            // Configuration file name
        HBufC* iConfigNameBuffer;                // HBuffer for configuration file;
        HBufC* iTestCaseArgs;                    // HBuffer for test case arguments
		
        RMessage2 iTestExeMessage;               // Test execution message
        TTestState iThreadState;                 // Thread state

        // Print related members
        RMutex          iPrintMutex;             // For iPrintSem semaphore handling
        RSemaphore      iPrintSem;               // Synchonize print queue access
        CPrintQueue*    iPrintQueue;             // Print queue
        RMessage2 iNotifyPrintMessage;           // Print notification message
        TBool iPrintNotifyAvailable;             // Is print notification available?

        // Event related members
        RMessage2 iNotifyEventMessage;           // Event nofitication message
        TBool           iEventNotifyAvailable;   // Is event notification available?
        RMutex          iEventMutex;             // For iEventSem semaphore handling
        RSemaphore      iEventSem;               // Signal event request availability
        TEventDef       iEventDef;

        // Remote command protocol related stuff
        RMessage2       iNotifyRemoteCmdMessage;// RemoteCmd notication message
        TBool           iRemoteSendAvailable;    // Is RemoteSend available
        RMutex          iSndMutex;               // For iSndSem semaphore handling
        RSemaphore      iSndSem;                 // Signal RemoteCmd request availability
        TThreadId       iTestThreadId;           // Thread id for receiver 
        RThread         iTestThread;
        TBool           iTestThreadOpen;
        TBool           iRemoteReceiveAvailable; // Is RemoteReceive available
        RMutex          iRcvMutex;               // For iRcvSem semaphore handling
        RSemaphore      iRcvSem;                 // Signal RemoteCmd request availability
        //RSemaphore      iReceiverSem;            // Signal testmodule request availability
        RPointerArray<HBufC8> iMessageQueue;
        
        TCmdDef         iCmdDef;
        TCmdDef         iRcvCmdDef;  
        TTestProgress   iProgress;
                

        // Result with timestamp information
        TFullTestResult iFullResult;    
        TInt            iTestThreadFailure;         
        
        // Event array        
        RPointerArray<TEvent>           iEventArray; 
        // State event array
        RPointerArray<TDesC>            iStateEvents;  
        
        // Event handlers
        CPrintHandler*          iPrintHandler;
        CEventHandler*          iEventHandler;
        CSndHandler*            iSndHandler;
        CRcvHandler*            iRcvHandler;
        CInterferenceHandler*   iInterferenceHandler;
        CMeasurementHandler*    iMeasurementHandler;
        CCommandHandler*        iCommandHandler;
        
        // Test Interface related members
        RMutex          iInterferenceMutex; // For iInterferenceMutex semaphore handling
        RSemaphore      iInterferenceSem;   // Synchronize interference queue access
        TBool           iInterferenceNotifyAvailable; // Is interference notification
                                                      //available?
        // Test measurement related members
        RMutex          iMeasurementMutex;  // For iMeasurementMutex semaphore handling
        RSemaphore      iMeasurementSem;    // Synchronize measurement queue access
        TBool           iMeasurementNotifyAvailable; // Is measurement notification
                                                      //available?
        RMessage2 iNotifyInterferenceMessage; // Interference notification message

        TTestInterference   iTestInterference; // Struct for information
        // Array for handling test interference thread's kill in
        // panic etc. cases
        RArray<RThread>  iSTIFTestInterferenceArray;

        RMessage2 iNotifyMeasurementMessage; // Measurement notification message

        // Struct for information
        TTestMeasurement   iTestMeasurement;
        // Array for handling measurement
        RPointerArray<TMeasurementHandling>  iMeasurementHandlingArray;

        // This mutex meaning is to make sure that test complete and test
        // case cancel operations are not executed at the same time.
        RMutex          iTestThreadMutex; 

        // Command related members
        CCommandDef*     iCommandDef;             // Command definition (content)
        RMessage2       iNotifyCommandMessage;   // Command nofitication message
        TBool           iCommandNotifyAvailable; // Is command notification available?
        RMutex          iCommandMutex;           // For iCommandSem semaphore handling
        RSemaphore      iCommandSem;             // Signal command request availability
        
    public:     // Friend classes
        // None
        
    protected:  // Friend classes
        // None
    	
    private:    // Friend classes
        friend class CUnderTaker;
        friend class CPrintHandler;
        friend class CEventHandler;
        friend class CSndHandler;
        friend class CRcvHandler;
        friend class TEvent;
        friend class CInterferenceHandler;
        friend class CMeasurementHandler;
        friend class CCommandHandler;
    };

#endif // TEST_SERVER_MODULE_IF_H

// End of File
