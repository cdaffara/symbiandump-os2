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
* Description: This file contains the header file of the CUIEngine.
*
*/

#ifndef STIF_TFW_IF_CONTAINER_H
#define STIF_TFW_IF_CONTAINER_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <StifTFwIf.h>

#include <TestEngineClient.h>
#include <stifinternal/UIEngine.h>

// CONSTANTS
// None
_LIT( KUIStoreDefaultDir, "C:\\TestFramework\\" );
// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CUIEngine;
class CUIEngineRunner;
class CUIEnginePrinter;
class CUIEngineErrorPrinter;
class CUIEngineRemote;

// CLASS DECLARATION

// DESCRIPTION

// CUIEngineContainer is interface between main class CUIEngine
// and CUIEngineRunner, which execute test cases
class CUIEngineContainer
        : public CBase
    {
    public: // Enumerations
        // Case state
        enum TCaseState
            {
            ENotStarted,
            ERunning,
            EPaused,
            EExecuted,
            EFinished,
            };

    private: // Enumerations
        // None

    public: // Constructor

        /**
        * NewL is two-phased constructor.
        */
        static CUIEngineContainer* NewL( CUIEngine* aUIEngine,
                                const TTestInfo& aTestInfo,
                                RTestEngineServer& aTestEngineServ,
                                RTestEngine& aTestEngine );

    public: // New functions


    public: // Functions from base classes

        /**
        * StartContainerL starts test case execution.
        *
        * Returns Symbian OS error code.
        */
        TInt StartContainerL();

        /**
        * TestCaseExecuted informs that test case has executed.
        */
        void TestCaseExecuted( TFullTestResult& aResult, TInt aStatus );

        /**
        * PrintProgress forwards print information from test case.
        */
        void PrintProgress( TTestProgress& aProgress );

        /**
        * RemoteSend forwards remote protocol messages from master.
        */
        void RemoteSend( const TDesC& aRemoteMsg, TInt aStatus );
        
        /**
        * GoingToReboot forwards reboot indication.
        */
        TInt GoingToReboot( TRequestStatus& aStatus );
        
        /**
        * RemoteReceive forwards remote protocol messages to master.
        */
        TInt RemoteReceive( const TDesC& aRemoteMsg );

        /**
        * CancelTest cancels test case execution. TestCompleted() will be called 
        * with aTestExecutionResult set to KErrCancel.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt CancelTest();

        /**
        * PauseTest pauses test case execution.
        *  
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt PauseTest();

        /**
        * ResumeTest resumes test case execution.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt ResumeTest();
        
        /**
        * Return remote identifier.
        */
        TUint32 RemoteId() const { return iRemoteId; };

        /**
        * Set remote identifier.
        */
        void SetRemoteId( TUint32 aRemoteId ){ iRemoteId = aRemoteId; };

        /**
        * Return master identifier.
        */
        TUint32 MasterId() const { return iMasterId; };
        
        /**
        * Return test case execution state. 
        */
        TCaseState State() const { return iState; };
        
        /**
        * Printer signals all prints done event.
        */ 
        void PrintsDone();

    protected: // New functions
        // None

    protected: // Functions from base classes
        // None

    private:  // Constructors and destructor

        /** 
        * C++ default constructor.
        */
        CUIEngineContainer( CUIEngine* aUIEngine,
            const TTestInfo& aTestInfo,
            RTestEngineServer& aTestEngineServ,
            RTestEngine& aTestEngine );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();
        
        /**
        * Destructor of CUIEngineContainer.
        */
        ~CUIEngineContainer();

    public: //Data
        // Pointer to UIEngine
        CUIEngine*                         iUIEngine;

    protected: // Data
        // None

    private: // Data

        // Handle to Test Engine Server
        RTestEngineServer                   iTestEngineServ;

        // Handle to Test Engine
        RTestEngine                         iTestEngine;

        // Handle to Test Case
        RTestCase                           iTestCase;
        
        // Test case runner
        CUIEngineRunner*                    iRunner;
        
        // Test case printer
        CUIEnginePrinter*                   iPrinter;
        
        // Test case remote command handler
        CUIEngineRemote*                    iRemote;

        // Test info and package
        TTestInfo                           iTestInfo;
        TTestInfoPckg                       iTestInfoPckg;

        // Case state
        TCaseState                          iState;
        
        // Remote protocol identifier
        TUint32                             iRemoteId;

        // Master protocol identifier
        TUint32                             iMasterId;

    public: // Friend classes
        friend class CUIEngine;

    protected: // Friend classes
   
    private: // Friend classes

    };


#endif      // STIF_TFW_IF_CONTAINER_H 

// End of File
