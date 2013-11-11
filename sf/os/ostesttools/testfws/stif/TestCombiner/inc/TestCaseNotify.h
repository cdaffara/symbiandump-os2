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
* Description: This file contains the header file of
* the CTestProgressNotifier and the CTestEventNotifier.
*
*/

#ifndef TEST_CASE_NOTIFY_H
#define TEST_CASE_NOTIFY_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <stifinternal/TestServerClient.h>
#include "TestCombiner.h"
#include "StifCommand.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION

// DESCRIPTION
// CTestProgressNotifier is an active object used for notifications called from
// Test Module using the RTestExecution API.
NONSHARABLE_CLASS(CTestProgressNotifier)
    :public CActive
    {
    public: // Enumerations
        
    private: // Enumerations
        // TTestProgressStatus defines the status of notification
        enum TTestProgressStatus
            {
            ETestProgressIdle,
            ETestProgressPending,
            ETestProgressCompleted
            };

    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CTestProgressNotifier* NewL( CTestCombiner* aTestCombiner, 
                                            CTCTestCase* aTestCase );
          
        /**
        * Destructor.
        */
        ~CTestProgressNotifier();
        
        /**
        * Checks is UI print progress finished.
        */
        TBool IsPrintProgressFinished();
          
    public: // New functions
          
    public: // Functions from base classes
        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel.
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );
          
    protected: // New functions
        
    protected: // Functions from base classes
        
    private:
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Parametric C++ constructor.
        */
        CTestProgressNotifier( CTestCombiner* aTestCombiner, 
                               CTCTestCase* aTestCase );

        /**
        * Start progress notifier.
        */
         void StartL();

    public: // Data
     
    protected: // Data
    
    private: // Data
        // Pointer to CTestCombiner
        CTestCombiner*      iTestCombiner;
          
        // Pointer to CTCTestCase 
        CTCTestCase*         iTestCase;

        // Internal state
        TTestProgressStatus iState;

        // Test Progress
        TTestProgress       iProgress;
        TTestProgressPckg   iProgressPckg;

        // Indication for UI print process
        TBool               iIsPrintProcessFinished;

    public: // Friend classes
    
    protected: // Friend classes
    
    private: // Friend classes
    
    };     
    
// DESCRIPTION
// CTestErrorNotifier is an active object used for notifications called from
// Test Module using the RTestExecution API.
NONSHARABLE_CLASS(CTestErrorNotifier)
    :public CActive
    {
    public: // Enumerations
        
    private: // Enumerations
        // TTestProgressStatus defines the status of notification
        enum TTestErrorStatus
            {
            ETestErrorIdle,
            ETestErrorPending,
            ETestErrorCompleted
            };

    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CTestErrorNotifier* NewL( CTestCombiner* aTestCombiner, 
                                         CTCTestModule* aTestModule );
          
        /**
        * Destructor.
        */
        ~CTestErrorNotifier();
          
    public: // New functions
          
    public: // Functions from base classes
        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel.
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );
          
    protected: // New functions
        
    protected: // Functions from base classes
        
    private:
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Parametric C++ constructor.
        */
        CTestErrorNotifier( CTestCombiner* aTestCombiner, 
                            CTCTestModule* aTestModule );

        /**
        * Start progress notifier.
        */
         void StartL();

    public: // Data
     
    protected: // Data
    
    private: // Data
        // Pointer to CTestCombiner
        CTestCombiner*      iTestCombiner;
          
        //Pointer to CTCTestCase 
        CTCTestModule*         iTestModule;

        // Internal state
        TTestErrorStatus iState;

        // Test Progress
        TErrorNotification       iError;
        TErrorNotificationPckg   iErrorPckg;

    public: // Friend classes
    
    protected: // Friend classes
    
    private: // Friend classes
    
    };     

// CLASS DECLARATION

// DESCRIPTION
// CTestEventNotify is an active object used to get event notify from
// Test Server.
NONSHARABLE_CLASS(CTestEventNotify)
    :public CActive
    {
    public: // Enumerations
    
    private: // Enumerations
        // TTestEventStatus defines the status of notification
        enum TTestEventNotifyStatus
            {
            ETestEventNotifyIdle,
            ETestEventNotifyPending,
            ETestEventNotifyCompleted
            };

    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CTestEventNotify* NewL( CTestCombiner* aTestCombiner,
                                       CTCTestCase* aTestCase,
                                       TEventIf& aEvent );
          
        /**
        * Destructor.
        */
        ~CTestEventNotify();
          
    public: // New functions     
          
        TEventIf& Event(){ return iEvent; };
        
    public: // Functions from base classes
        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel.
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );
          
    protected:  // New functions
    
    protected:  // Functions from base classes
    
    private:
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( TEventIf& aEvent );

        /**
        * Parametric C++ constructor.
        */
        CTestEventNotify( CTestCombiner* aTestCombiner, 
                          CTCTestCase* aTestCase );
        

    public:    //Data
     
    protected:  // Data
 
    private:     // Data
        // Pointer to CTestCombiner
        CTestCombiner* iTestCombiner;
        
        // Pointer CTestCase
        CTCTestCase* iTestCase;

        // Internal state
        TTestEventNotifyStatus iState;

        // Event interface
        TEventIf iEvent;
        TEventIfPckg iEventPckg;

    public:      // Friend classes

    protected:  // Friend classes

    private:     // Friend classes

    };
    
// CLASS DECLARATION

// DESCRIPTION
// CTestEventNotifier is an active object used for event notify from
// Test Module using the RTestExecution API.
NONSHARABLE_CLASS(CTestEventNotifier)
    :public CActive
    {
    public: // Enumerations
    
    private: // Enumerations
        // TTestEventStatus defines the status of notification
        enum TTestEventStatus
            {
            ETestEventIdle,
            ETestEventPending,
            ETestEventCompleted,
            ETestEventWaitUnset
            };

    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CTestEventNotifier* NewL( CTestCombiner* aTestCombiner, 
                                         CTCTestCase* aTestCase  );
          
        /**
        * Destructor.
        */
        ~CTestEventNotifier();
          
    public: // New functions     
          
    public: // Functions from base classes
        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel.
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );
          
    protected:  // New functions
    
    protected:  // Functions from base classes
    
    private:
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Parametric C++ constructor.
        */
        CTestEventNotifier( CTestCombiner* aTestCombiner, 
                            CTCTestCase* aTestCase );

        /**
        * StartL starts the CTestEventNotifier active object.
        */
        void StartL();

    public:    //Data
     
    protected:  // Data
 
    private:     // Data
        // Pointer to CTestCombiner
        CTestCombiner* iTestCombiner;

        // Pointer CTestCase
        CTCTestCase* iTestCase;

        // Internal state
        TTestEventStatus iState;

        // Event interface
        TEventIf iEvent;
        TEventIfPckg iEventPckg;

    public:      // Friend classes

    protected:  // Friend classes

    private:     // Friend classes

    };

// DESCRIPTION
// CTestCommandNotifier is an active object used for command notify from
// Test Module using the RTestExecution API.
NONSHARABLE_CLASS(CTestCommandNotifier): public CActive
    {
    public:     // Enumerations
        // None

    private:    // Enumerations
        // None

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestCommandNotifier* NewL(CTestCombiner* aTestCombiner, CTCTestCase* aTestCase);

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
        CTestCommandNotifier(CTestCombiner* aTestCombiner, CTCTestCase* aTestCase);

    public:     // Data
        // None

    protected:  // Data
        // None

    private:    // Data

        // Pointer to CTestCombiner
        CTestCombiner*     iTestCombiner;

        // Pointer to CTCTestCase
        CTCTestCase*       iTestCase;

        // Command to be sent
        TCommand                        iCommand;
        TCommandPckg                    iCommandPckg;
        TBuf8<KMaxCommandParamsLength2> iParamsPckg;

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None
    };

#endif // TEST_CASE_NOTIFY_H

// End of File
