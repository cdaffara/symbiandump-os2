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
* RTestServer, RTestModule and RTestExecution.
*
*/

#ifndef TEST_SERVER_CLIENT_H
#define TEST_SERVER_CLIENT_H


//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "TestEngineClient.h"
#include "StifTestEventInterface.h"
#include "StifTFwIfProt.h"

// CONSTANTS
_LIT( KDefaultExeName, "testserverstarter.exe" );
_LIT( KDefaultUiExeName, "uitestserverstarter.exe" );

// TestScripter related informations
const TInt KTestScripterNameLength = 12;    // In STIF are two same name
_LIT( KTestScripterName, "testscripter" );  // constants, change both when
                                            // needed !!!

// TestCombiner related information
_LIT( KTestCombinerName, "testcombiner" );

// Test module thread heap and stack sizes
const TUint32 KTestThreadMinHeap = 4096;     // 4K heap min
const TUint32 KTestThreadMaxHeap = 1048576;  // 1M heap max
const TUint32 KStackSize         = 16384;    // 16K stack

// MACROS

// DATA TYPES
// Opcodes used in message passing between client and server
enum TTestServerRequests
    {
    // RTestServer requests
    ETestServerCloseSession,
    ETestServerGetServerThreadId,

    // RTestModule requests
    ETestModuleCreateSubSession,
    ETestModuleCloseSubSession,
    ETestModuleEnumerateTestCases,
    ETestModuleGetTestCases,
    ETestModuleCancelAsyncRequest,
    ETestModuleErrorNotification,

    // RTestExecution requests
    ETestExecutionCreateSubSession,
    ETestExecutionCloseSubSession,
    ETestExecutionRunTestCase,      
    ETestExecutionPause,
    ETestExecutionResume,
    ETestExecutionNotifyProgress,
    ETestExecutionNotifyData,
    ETestExecutionCancelAsyncRequest,
    ETestExecutionNotifyEvent,
    ETestExecutionNotifyRemoteCmd,
    ETestExecutionReadRemoteCmdInfo,
    ETestExecutionNotifyCommand,
    };

// 
typedef TPckgBuf<TInt> TCaseSize;

// FORWARD DECLARATIONS
class CTestThreadContainerRunnerFactory;
// FUNCTION PROTOTYPES

// Function for starting the TestServer Server.
IMPORT_C TInt StartNewServer( const TFileName& aModuleFileName, 
                              TFileName& aServerName
                            );

// Function for starting the TestServer Server in either in
// current thread or in new thread
IMPORT_C TInt StartNewServer( const TFileName& aModuleFileName,
                              TFileName& aServerName,
                              const TBool aInNewThread,
                              RSemaphore aSynchronisation
                            );

IMPORT_C TInt StartNewServer( const TFileName& aModuleFileName,
					        TFileName& aServerName,
					        const TBool aInNewThread,
					        RSemaphore aSynchronisation,
					        TBool aUiTestingServer,
					        CTestThreadContainerRunnerFactory* aTestThreadContainerRunnerFactory
					      );

// CLASS DECLARATION

// DESCRIPTION
// RTestServer is a client class of Test Server
// The connect function starts the server, if it not already running.
// An RSessionBase sends messages to the server with the function 
// RSessionBase::SendReceive();
// specifying an opcode( TTestServerReq ) and array of argument pointers.

class RTestServer
        :public RSessionBase 
    {
    public: // Enumerations
		// None

    private: // Enumerations
		// None

    public:  // Constructors and destructor
        
        /**
        * Constructor.
        */
        IMPORT_C RTestServer();

        /**
        * Closes the RTestServer session.
        */
        IMPORT_C void Close();

    public: // New functions
        
        /**
        * Connect method creates new RTestServer session that is used to manage
        * the test case execution.
        */
        IMPORT_C TInt Connect( const TFileName& aModuleName,
                               const TDesC& aConfigFile);

        /**
        * Version returns the client side version number from the RTestServer.
        */
        IMPORT_C TVersion Version() const;

        /**
        * GetServerThreadId returns the server thread id from the RTestServer.
        */
        IMPORT_C TInt GetServerThreadId( TThreadId& aId );
    public: // Functions from base classes
        // None
        
    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:  // Functions from base classes
        // None

        /**
        * Find exe(CapsModifier) from file system
        */
        TInt FindExeL( TFileName aModuleName, TFileName& aPathAndExeModule );

        /**
        * Get caps modifier module name from TestScripter's test case(config)
        * file.
        */
        TInt GetCapsModifier( const TDesC& aConfigFile,
                                    TFileName& aCapsModifierName );

    public:     // Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
        // None

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };
    

// DESCRIPTION

// RTestModule class represents a subsession with the RTestServer.
// Each RTestModule corresponds to a specific CTestModule( a CObject derived
// class ) in the server. Messages are passed via the RTestServer.
// A RTestModule stores a handle from it's server side CTestModule, and uses
// this to identify the CTestModule during communication.

class RTestModule
        :public RSubSessionBase 
    {
    public: // Enumerations
		// None

    private: // Enumerations
		// None

    public:  // Constructors and destructor
		// None

    public: // New functions

        /**
        * Open creates a subsession to Test Server and opens handle.		
        * The RTestServer server session shall be created by calling
        * RTestServer::Connect before calling this function to create new
        * RTestModule subsession.
        */
        IMPORT_C TInt Open( RTestServer& aServer, TFileName& aIniFile );

        /**
        * Closes the RTestModule subsession.
        */
        IMPORT_C void Close();

        /**
        * EnumerateTestCases requests the needed array size for test cases that
        * will be inquired by GetTestCases method. The aConfigFile specifies the
        *( optional ) config file where test cases are defined for this query.
        * When the EnumerateTestCases is completed succesfully the GetTestCases
        * method is called to retrieve test cases to
        * CArrayFixFlat<RTestEngine::TTestInfo> list object.
        */
        IMPORT_C void EnumerateTestCases( TDesC& aConfigFile, 
                                          TCaseSize& aCount,
                                          TRequestStatus& aStatus );

        /**
        * GetTestCases is used to synchronously get test cases from Test Server.
        * The list of test cases is retrieved to aTestCaseBuffer. Test Engine
        * allocates needed memory for the test case array by calling
        * EnumerateTestCases to inquire needed size of aTestCaseBuffer.
        *
        * If this method is called without previously succesfully called
        * EnumerateTestCases the Test Server shall return KErrNotReady.
        */              
        IMPORT_C TInt GetTestCases( CFixedFlatArray<TTestCaseInfo>& aTestCaseBuffer );

        
        /**
        *  
        */
        IMPORT_C void ErrorNotification( TErrorNotificationPckg& aError,                                           
                                         TRequestStatus& aStatus );
		
        /**
        * Asynchronous request can be canceled with CancelAsyncRequest method.
        * The aReqToCancel parameter defines the request that will be canceled.
        */
        IMPORT_C TInt CancelAsyncRequest( TInt aReqToCancel );

    public: // Functions from base classes
        // None
        
    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:  // Functions from base classes
        // None

    public:     // Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
        // None

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };
    

// DESCRIPTION

// RTestExecution class represents a subsession with the RTestServer.
// Each RTestExecution corresponds to a specific CTestExecution( a CObject
// derived class ) in the server. Messages are passed via the RTestServer.
// A RTestExecution stores a handle from it's server side CTestExecution,
// and uses this to identify the CTestExecution during communication.

class RTestExecution
        :public RSubSessionBase 
    {
    public: // Enumerations
		// None

    private: // Enumerations
		// None

    public:  // Constructors and destructor
        // None       

    public: // New functions

        /**
        * Open creates a subsession for new test case. Test case to be executed
        * is specified by aTestCaseNumber. The aConfig specifies the optional
        * config file where the test case is defined. Test cases that can be
        * executed are retrieved from the Test Server by calling two-phased
        * RTestModule::GetTestCases operation.
        */
        IMPORT_C TInt Open( RTestServer& aServer,
                            const TInt aCaseNumber,
                            const TFileName& aConfig );

        /**
        * Closes the RTestExecution subsession. Test case is normally closed
        * immediately after the test case was completed.
        */
        IMPORT_C void Close();
        
         /**
        * RunTestCase is used to execute the test case previously initialized by
        * Open method. RunTestCase is asynchronous method and it is completed
        * when the test case is completed.
        * The result of the test case is returned to aStatus.
        */
        IMPORT_C void RunTestCase( TFullTestResultPckg& aResult,
                                   TRequestStatus& aStatus );

        /**
       * RunTestCase is used to execute the test case previously initialized by
       * Open method. RunTestCase is asynchronous method and it is completed
       * when the test case is completed.
       * The result of the test case is returned to aStatus.
       */
       IMPORT_C void RunTestCase( TFullTestResultPckg& aResult,
                                  const TDesC& aTestCaseArgs,
                                  TRequestStatus& aStatus );

        /**
        * Pause suspends the execution of the test case. The subsession where
        * the test case is run will be suspended and thus the test case
        * execution is suspended. The test case execution can be later resumed
        * by calling Resume.
        */
        IMPORT_C TInt Pause();

        /**
        * Resume is used to resume the test case suspended by previously called
        * Pause method. The test case execution should be continued immediately
        * after the Resume is called.
        */
        IMPORT_C TInt Resume();

        /**
        * NotifyProgress requests different progress information from the Test
        * DLL. This information can be e.g. printed to UI. Progress information
        * is returned to aProgress. The aProgress is TTestProgressPckg that is
        * a packaged TTestProgress.
        */
        IMPORT_C TInt NotifyProgress( TTestProgressPckg& aProgress,
                                      TRequestStatus& aStatus );

        /**
        * NotifyData is used to receive different data from the Test Server,
        * e.g. images or web pages which can be then viewed to tester or
        * forwarded to some kind of comparison tool. 
        */
        IMPORT_C TInt NotifyData( TDes8& aData, TRequestStatus& aStatus );

        /**
        * Asynchronous request can be canceled with CancelAsyncRequest method.
        * The aReqToCancel parameter defines the request that will be canceled.
        */
        IMPORT_C TInt CancelAsyncRequest( TInt aReqToCancel );

        /**
        * NotifyEvent is used to control event system.
        */
        IMPORT_C TInt NotifyEvent( TEventIfPckg& aEvent,
                                   TRequestStatus& aStatus,
                                   TInt aError=KErrNone );
        
        /**
        * NotifyRemoteCmd is used to get notify.
        */
        IMPORT_C TInt NotifyRemoteCmd( TStifCommandPckg& aRemoteCommand,
                                       TPckg<TInt>& aMsgSizePckg,
                                       TRequestStatus& aStatus );
                                   
        /**
        * ReadRemoteCmdInfo for remote message.
        */
        IMPORT_C TInt ReadRemoteCmdInfo( TDes8& aRemoteMsg,
                                         TStifCommand aType,
                                         TInt aError = KErrNone );

        /**
        * NotifyCommand is used to control commands from the test case.
        * It was created to allow test case to kill itself. DEPRECATED !!
        * Use NotifyCommand2 instead.
        */
        IMPORT_C TInt NotifyCommand(TCommandPckg& aCommandPckg,
                                    TBuf8<KMaxCommandParamsLength>& aParamsPckg,
                                    TRequestStatus& aStatus,
                                    TInt aError = KErrNone);
        
        /**
        * NotifyCommand is used to control commands from the test case.
        * It was created to allow test case to kill itself.
        */
        IMPORT_C TInt NotifyCommand2(TCommandPckg& aCommandPckg,
                                    TDes8& aParamsPckg,
                                    TRequestStatus& aStatus,
                                    TInt aError = KErrNone);

    public: // Functions from base classes
        // None
        
    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:  // Functions from base classes
        // None

    public:     // Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
        // None

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None
    };
    

#endif // TEST_SERVER_CLIENT_H

// End of File
