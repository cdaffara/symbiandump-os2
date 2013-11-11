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
* Description: This file contains the header file of the 
* CTestCaseController, the CTestProgressNotifier and the 
* CTestEventNotifier.
*
*/

#ifndef TEST_CASE_CONTROLLER_H
#define TEST_CASE_CONTROLLER_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <stifinternal/TestServerClient.h>
#include <atslogger.h>
#include "TestEngine.h"
#include "TestCallBack.h"

#include "StifHWReset.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None
typedef CStifHWReset*( *CTestInterfaceFactory )();

// FORWARD DECLARATIONS
class CTestModuleController;
class CTestReport;
class CTestCaseTimeout;

// CLASS DECLARATION

// DESCRIPTION
// CTestCaseController is an active object used for running the test case
// using the RTestExecution API.
class CTestCaseController
        : public CActive
    {
    public:     // Enumerations
        // None

    private:    // Enumerations

        // TTestCaseStatus defines the status of test case
        enum TTestCaseStatus
            {
            ETestCaseIdle,
            ETestCaseRunning,
            ETestCaseCompleted,
            ETestCaseTimeout,
            ETestCaseSuicided, //test case has killed itself
            };

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestCaseController* NewL( CTestEngine* aEngine,
            CTestReport* aTestReport,
            CAtsLogger& aAtsLogger,
            RTestExecution aTestExecution, 
            const TTestInfo& aTestInfo );

        /**
        * Destructor.
        */
        ~CTestCaseController();

    public:     // New functions

        /**
        * StartL starts the CTestCaseController active object.
        */
        void StartL( const RMessage2& aMessage );

        /**
        * Timeouts a testcase
        */
        void Timeout();

        /**
        * Kills a testcase
        */
        void Suicide(TStopExecutionType aType, TInt aCode);

    public:     // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );
        
        /**
        * Get testcase info(test module, config file, test case, etc).
        */
        void GetTestCaseInfo( TTestInfo& aTestInfo );

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Parametric C++ constructor.
        */
        CTestCaseController( CTestEngine* aEngine, 
                             CTestReport* aTestReport,
                             CAtsLogger& aAtsLogger,
                             RTestExecution aTestExecution,
                             const TTestInfo& aTestInfo );
        
        /**
        * Generates XML log about the test case.
        */
        void GenerateXmlLogL();

    public:     // Data
        // None
    
    protected:  // Data
        // None

    private:    // Data

        // Pointer to CTestEngine
        CTestEngine*        iEngine;

        // Pointer to Test Report
        CTestReport*        iTestReport;

        // Handle to RTestExecution
        RTestExecution      iTestExecution;

        // Pointer to timeout handler
        CTestCaseTimeout*    iTimeout;

        // Test Info
        TTestInfo           iTestInfo;

        // Test case arguments
		HBufC*              iTestCaseArguments;
        
        // Internal state
        TTestCaseStatus     iState;

        // Test Result
        TFullTestResult     iResult;
        TFullTestResultPckg iResultPckg;

        // Message to be completed when case is finished
        RMessage2            iMessage;

        // AtsLogger
        CAtsLogger&         iAtsLogger;

        // RDebug logging for testcase status
        CStifLogger*        iRDebugLogger;

        // Stop execution type
        TStopExecutionType  iStopExecutionType;

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes

        friend class CTestRemoteCmdNotifier;

    };

// DESCRIPTION
// CTestProgressNotifier is an active object used for notifications called from
// Test Module using the RTestExecution API.
class CTestProgressNotifier
        : public CActive
    {
    public:     // Enumerations
        // None

    private:    // Enumerations

        // TTestProgressStatus defines the status of notification
        enum TTestProgressStatus
            {
            ETestProgressIdle,
            ETestProgressPending,
            ETestProgressCompleted
            };

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestProgressNotifier* NewL( CTestEngine* aEngine, 
                             RTestExecution aTestExecution );

        /**
        * Destructor.
        */
        ~CTestProgressNotifier();

    public:     // New functions

        /**
        * StartL starts the CTestProgressNotifier active object.
        */
        void StartL( const RMessage2& aMessage );

    public:     // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Parametric C++ constructor.
        */
        CTestProgressNotifier( CTestEngine* aEngine, 
                               RTestExecution aTestExecution );

    public:     // Data
        // None

    protected:  // Data
        // None

    private:    // Data

        // Pointer to CTestEngine
        CTestEngine*        iEngine;

        // Handle to RTestExecution
        RTestExecution      iTestExecution;

        // Internal state
        TTestProgressStatus iState;

        // Test Progress
        TTestProgress       iProgress;
        TTestProgressPckg   iProgressPckg;

        // Message to be completed when notification is completed
        RMessage2           iMessage;
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// DESCRIPTION
// CTestEventNotifier is an active object used for event notify from
// Test Module using the RTestExecution API.
class CTestEventNotifier
        : public CActive
    {
    public:     // Enumerations
        // None

    private:    // Enumerations

        // TTestEventStatus defines the status of notification
        enum TTestEventStatus
            {
            ETestEventIdle,
            ETestEventPending,
            ETestEventCompleted
            };

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestEventNotifier* NewL( CTestEngine* aEngine,  
            RTestExecution aTestExecution );

        /**
        * Destructor.
        */
        ~CTestEventNotifier();
        
    public:     // New functions

        /**
        * StartL starts the CTestEventNotifier active object.
        */
        void Start();

        /**
        * For event control.
        */
        void CtlEvent( const TEventIf& aEvent, TRequestStatus& aStatus );

		/**
		* Check if CtlEvent should be called
		*/
        TBool CheckCtlEvent( const TEventIf& aEvent );

    public:     // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the cancel
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );

    protected:      // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Parametric C++ constructor.
        */
        CTestEventNotifier( CTestEngine* aEngine,
             RTestExecution aTestExecution);

    public:     // Data
        // None

    protected:  // Data
        // None

    private:    // Data

        // Pointer to CTestEngine
        CTestEngine* iEngine;

        // Handle to RTestExecution
        RTestExecution iTestExecution;

        // Internal state
        TTestEventStatus iState;

        // Event interface
        TEventIf iEvent;
        TEventIfPckg iEventPckg;
        TEventIf iEventNotify;
        TEventIfPckg iEventNotifyPckg;

        // Event array
        RPointerArray<HBufC> iEventArray;

        // Associated event controller
        CTestEventController* iController;

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// DESCRIPTION
// CTestEventController is used for event controlling from
// TestEngine.
class CTestEventController
        : public CBase
    {
    public:     // Enumerations
        // None

    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestEventController* NewL( CTestEngine* aEngine,
                                           const TEventIf& aEvent,
                                           TRequestStatus* aStatus );
  
        static CTestEventController* NewL( CTestEngine* aEngine,
                                           const TEventIf& aEvent,
                                           RMessage2& aMessage );

        /**
        * Destructor.
        */
        ~CTestEventController();

    public:     // New functions
        /**
        * Event complete.
        */ 
        void EventComplete();

    public:     // Functions from base classes

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( TRequestStatus* aStatus );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( RMessage2& aMessage );

        /**
        * Parametric C++ constructor.
        */
        CTestEventController( CTestEngine* aEngine, const TEventIf& aEvent );

        /**
        * Check all events.
        */ 
        TBool CheckEventsL();

        /**
        * Check client events.
        */ 
        TBool CheckClientEventsL();

        /**
        * Check testcase events.
        */ 
        TBool CheckTestcaseEventsL(); 

        /**
        * Callback for event controller.
        */ 
        static TInt EventCallBack( TAny* aTestEventController );

    public:     // Data
        // None
    
    protected:  // Data
        // None

    private:    // Data

        // Pointer to CTestEngine
        CTestEngine*    iEngine;

        // Either of these is completed when ready
        // If aRequestStatus exists, it is completed,
        // otherwise aMessage is completed.
        TRequestStatus* iRequestStatus;
        RMessage2        iMessage;

        // Event interface
        TEventIf iEvent;
        TEventIfPckg iEventPckg;

        // Number of active event commands
        TInt iActiveEventCmds;
        
        RPointerArray<CCallBack> iEventCallBacks;

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// DESCRIPTION
// CTestCaseTimeout is used for time controlling
class CTestCaseTimeout
        : public CActive
    {
    public:     // Enumerations
        // None

    private:    // Enumerations
        // None
         
    public:     // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CTestCaseTimeout* NewL( CTestCaseController* aCase,
                     TTimeIntervalMicroSeconds aTimeout );

        /**
        * Destructor.
        */
        ~CTestCaseTimeout();
        
    public:     // New functions
        
        /**
        * Starts timeout counting
        */
        void Start();

    public:     // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the cancel
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL(CTestCaseController* aCase,
               TTimeIntervalMicroSeconds aTimeout );

        /**
        * C++ constructor.
        */
        CTestCaseTimeout();

    public:     // Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
        CTestCaseController* iCase;
        TTimeIntervalMicroSeconds iTimeout;
        RTimer iTimer;
        
        // Storage for testcase timeout.
        TTime iTestCaseTimeout;

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };
    
// DESCRIPTION
// CTestProgressNotifier is an active object used for notifications called from
// Test Module using the RTestExecution API.
class CTestRemoteCmdNotifier
        : public CActive
    {
    public:     // Enumerations
        // None

    private:    // Enumerations

        // TTestProgressStatus defines the status of notification
        enum TTestProgressStatus
            {
            ETestProgressIdle,
            ETestProgressPending,
            ETestProgressCompleted
            };

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestRemoteCmdNotifier* NewL( CTestEngine* aEngine, 
                             RTestExecution aTestExecution ,
                             CTestCaseController* aTestCaseController,
                             CAtsLogger& aAtsLogger );

        /**
        * Destructor.
        */
        ~CTestRemoteCmdNotifier();

    public:     // New functions

        /**
        * StartL starts the CTestRemoteCmdNotifier active object.
        */
        //void StartL( const RMessage& aMessage );

        /**
        * EnableReceive prepares to start active object
        */
        void EnableReceive( const RMessage2& aMessage );

        /**
        * GetReceivedMsg get received message.
        */
        void GetReceivedMsg( const RMessage2& aMessage );

        /**
        * Start starts the CTestCaseController active object.
        */
        void Start( const RMessage2& aMessage );

        /**
        * CancelReq cancels the request.
        */
        void CancelReq();

    public:     // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Parametric C++ constructor.
        */
        CTestRemoteCmdNotifier( CTestEngine* aEngine, 
                               RTestExecution aTestExecution,
                               CTestCaseController* aTestCaseController,
                               CAtsLogger& aAtsLogger );

        /**
        * Loads dynamically reset module.
        */
        TInt DynamicResetModule( CTestModuleIf::TRebootType aResetType );

    public:     // Data
        // None

    protected:  // Data
        // None

    private:    // Data

        // Pointer to CTestEngine
        CTestEngine*        iEngine;

        // Handle to RTestExecution
        RTestExecution      iTestExecution;

        // Internal state
        TTestProgressStatus iState;

        // Remote command's definition
        TStifCommand          iRemoteType;
        TStifCommandPckg      iRemoteTypePckg;
                
        TInt                    iMsgSize;              
        TPckg<TInt>             iMsgSizePckg;              

        // Message to be completed when notification is completed
        RMessage2           iMessage;
        
        HBufC8*             iReceivedMsg;

        TInt                iRebootCode;
        TName               iRebootName;

        // Pointer to CTestCaseController
        CTestCaseController* iTestCaseController;

        TBool               iMessageAvail;
        
        CAtsLogger&         iAtsLogger;

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// DESCRIPTION
// CTestCommandNotifier is an active object used for command notify from
// Test Module using the RTestExecution API.
// It was created to allow test case to kill itself.
class CTestCommandNotifier: public CActive
    {
    public:     // Enumerations
        // None

    private:    // Enumerations
        // None

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestCommandNotifier* NewL(CTestEngine* aEngine, RTestExecution aTestExecution);

        /**
        * Destructor.
        */
        ~CTestCommandNotifier();

    public:     // New functions

        /**
        * StartL starts the CTestCommandNotifier active object.
        */
        void Start();

    public:     // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the cancel
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError(TInt aError);

    protected:      // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Parametric C++ constructor.
        */
        CTestCommandNotifier(CTestEngine* aEngine, RTestExecution aTestExecution);

    public:     // Data
        // None

    protected:  // Data
        // None

    private:    // Data

        // Pointer to CTestEngine
        CTestEngine*     iEngine;

        // Handle to RTestExecution
        RTestExecution   iTestExecution;

        // Command to be sent
        TCommand                        iCommand;
        TCommandPckg                    iCommandPckg;
        TBuf8<KMaxCommandParamsLength2>  iParamsPckg;

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };


#endif // TEST_CASE_CONTROLLER_H

// End of File
