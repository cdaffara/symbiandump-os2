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
* RTestEngineServer, RTestEngine and RTestCase.
*
*/

#ifndef TEST_ENGINE_CLIENT_H
#define TEST_ENGINE_CLIENT_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <hal.h>
#include <StifTestInterface.h>
#include <StifTestEventInterface.h>
#include <StifLogger.h>

#include <StifTFwIfProt.h>

// CONSTANTS
// None

// Start and end tags for STIF setting denitions
_LIT( KStifSettingsStartTag, "[StifSettings]" );
_LIT( KStifSettingsEndTag, "[EndStifSettings]" );
// Tag for cabs modifier name
_LIT( KCapsModifier, "CapsModifier=" );
// Test module thread heap and stack names
_LIT( KUserDefStackSize, "TestThreadStackSize=" );
_LIT( KUserDefMinHeap, "TestThreadMinHeap=" );
_LIT( KUserDefMaxHeap, "TestThreadMaxHeap=" );

// Global mutex name
_LIT( KStifTestServerStartupMutex, "KStifTestServerStartupMutex" );

// MACROS
// None

// DATA TYPES

// Opcodes used in message passing between client and server
enum TTestEngineRequests
    {
    // RTestEngineServer requests
    ETestEngineServerCloseSession,
    // RTestEngine requests
    ETestEngineCreateSubSession,
    ETestEngineCloseSubSession,
    ETestEngineSetAttribute,
    ETestEngineAddTestModule,
    ETestEngineRemoveTestModule,
    ETestEngineAddConfigFile,
    ETestEngineRemoveConfigFile,
    ETestEngineEnumerateTestCases,
    ETestEngineGetTestCases,    
    ETestEngineCancelAsyncRequest,
    ETestEngineEvent,
    ETestEngineErrorNotification,
    ETestEngineLoggerSettings,
    ETestEngineCloseLoggerSettings,
    ETestEngineReportTestCase, //Add info about executed test case to test report. Used by reboot mechanism
    // RTestCase requests
    ETestCaseCreateSubSession,
    ETestCaseCloseSubSession,
    ETestCaseRunTestCase,
    ETestCasePause,
    ETestCaseResume,
    ETestCaseNotifyProgress,
    ETestCaseNotifyRemoteType,
    ETestCaseNotifyRemoteMsg,
    ETestCaseCancelAsyncRequest,
    ETestCaseNotifyCommand,
    };

// Package for test case count, used in EnumerateTestCases method
typedef TPckgBuf<TInt> TCaseCount;

// A set of test engine settings which can be set/read by SettingServer
struct TEngineSettings
    {
    TBool iUITestingSupport;
    TBool iSeparateProcesses;
    };

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// Template class delivering test cases from Test Module
template <class T> class CFixedFlatArray;

// CLASS DECLARATION

// DESCRIPTION
// RTestEngineServer is a client class of Test Engine Server
// The connect function starts the server, if is not already running.
// An RSessionBase sends messages to the server with
// the function RSessionBase::SendReceive();
// specifying an opcode (TTestEngineReq) and array of argument pointers.

class RTestEngineServer
        : public RSessionBase 
    {
    public:     // Enumerations

    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * Closes the RTestEngineServer session.
        */
        IMPORT_C void Close();

    public:     // New functions

        /**
        * Connect method creates new RTestEngineServer session that is used
        * to manage the test case execution.
        */
        IMPORT_C TInt Connect();

        /**
        * Version returns client side version number from the 
        * RTestEngineServer.
        */
        IMPORT_C TVersion Version() const;

    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:    // New functions

    public:     // Data

    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

// DESCRIPTION

// RTestEngine class represents a subsession with the RTestEngineServer.
// Each RTestEngine corresponds to a specific CTestEngine (a CObject derived
// class) in the server. Messages are passed via the RTestEngineServer.
// A RTestEngine stores a handle from it's server side CTestEngine, and uses
// this to identify the CTestEngine during communication.

class RTestEngine
        : public RSubSessionBase 
    {

    public:     // Enumerations

    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * Open creates a subsession to Test Engine and opens new handle to
        * RTestEngine. The RTestEngineServer subsession shall be created by
        * calling RTestEngineServer::Connect before calling Open to create new
        * RTestEngine subsession.
        */
        IMPORT_C TInt Open( RTestEngineServer& aServer,
                            const TFileName& aIniFile );

        /**
        * Closes the RTestEngine subsession.
        */
        IMPORT_C void Close();

    public:     // New functions

        /**
        * Sets attributes to Test Framework
        */
        IMPORT_C TInt SetAttribute( TAttribute aAttribute,
                                    const TDesC& aValue );

        /**
        * AddTestModule is used to add dynamically a new Test Module to the
        * Test Framework. Test cases can then be run from the new Test Module
        * without restarting the Test Framework.
        * If the Test Module specified by aTestModule doesn't exists this
        * method returns KErrNotFound. If the Test Module has been already
        * loaded this method returns KErrAlreadyExists.
        */
        IMPORT_C TInt AddTestModule( const TName& aTestModule,
                                     const TFileName& aIniFile );

        /**
        * RemoveTestModule is used to remove the existing Test Module from Test
        * Framework. If there are not test cases executing in the Test Module
        * it will be shut down.
        * If there is test case running from the Test Module this method
        * returns KErrInUse.
        * If the Test Module specified by aTestModule is not loaded this method
        * returns KErrNotFound.
        */
        IMPORT_C TInt RemoveTestModule( const TName& aTestModule );

        /**
        * AddConfigFile is used to add dynamically a new config file to the
        * Test Module specified by aTestModule. New test cases are then 
        * executed by calling first GetTestCases and then RunTestCase. Thus
        * new test cases can be run from the Test Module without restarting
        * the Test Framework.
        */
        IMPORT_C TInt AddConfigFile( const TName& aTestModule,
                                     const TFileName& aConfigFile );

        /**
        * RemoveConfigFile is used to remove a config file from the
        * Test Module specified by aTestModule. 
        */
        IMPORT_C TInt RemoveConfigFile( const TName& aTestModule,
                                        const TFileName& aConfigFile );

        /**
        * Test cases are inquired by two-phased operation calling first the
        * EnumerateTestCases method and then the GetTestCases. Test cases are
        * inquired from Test Engine that will inquire them from Test Modules.
        * EnumerateTestCases requests the needed array size for test cases that
        * will be inquired by GetTestCases method. When the EnumerateTestCases
        * is completed succesfully the GetTestCases method is called to
        * retrieve test cases to CArrayFixFlat <TTestInfo> list object.
        *
        * TTestInfo defines individual test case and, if needed, a test set
        * (TName iParent) where the test case belongs to. TTestInfo is used to
        * create test case by calling RTestCase::Open that constructs the
        * actual test case. Test case is then executed calling
        * RTestCase::RunTestCase.
        */
        IMPORT_C void EnumerateTestCases( TCaseCount& aCount,
                                          TRequestStatus& aStatus );

        /**
        * GetTestCases retrieves test cases from Test Modules to 
        * aTestCaseBuffer that is a list consisting of several TTestInfo 
        * objects. If this method is called without previously succesfully
        * called EnumerateTestCases method, this function returns the
        * KErrNotReady.
        */
        IMPORT_C TInt GetTestCases(
                            CFixedFlatArray<TTestInfo>& aTestCaseBuffer );

        /**
        * Asynchronous request can be canceled with CancelAsyncRequest method.
        * The aReqToCancel parameter defines the request that will be canceled.
        */
        IMPORT_C TInt CancelAsyncRequest( TInt aReqToCancel );
        
        /**
        * Event is used to get and set events in STIF Test Framework.
        * TEventIf defines the event request information.
        */
        IMPORT_C void Event( TEventIfPckg& aEvent,
                             TRequestStatus& aStatus );

        /**
        * Used to get Logger's overwrite parameters.
        */
        IMPORT_C TInt OpenLoggerSession( RTestEngineServer& aServer,
                                            TLoggerSettings& aLoggerSettings );

        /**
        * ErrorNotification obtains error notifications from test engine.
        */
        IMPORT_C void ErrorNotification( TErrorNotificationPckg& aError,
                                         TRequestStatus& aStatus );

        /**
        * Add info about test case to test report.
        */
        IMPORT_C TInt AddTestCaseResultToTestReport(const TTestInfo& aTestInfo,
                                                    const TFullTestResult& aTestResult,
                                                    const TInt aError);
                                         
    public:     // Functions from base classes
        
    protected:  // New functions

    protected:  // Functions from base classes

    private:    // New functions

    public:     // Data

    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };
    
// DESCRIPTION

// RTestCase class represents a subsession with the RTestEngineServer.
// Each RTestCase corresponds to a specific CTestExecution (a CObject
// derived class) in the server. Messages are passed via the RTestEngineServer.
// A RTestCase stores a handle from it's server side CTestExecution,
// and uses this to identify the CTestExecution during communication.

class RTestCase
        : public RSubSessionBase 
    {

    public:     // Structured classes

    public:     // Enumerations

    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * Open creates a subsession for new test case. Test case is initialized
        * using aTestCaseInfo parameter. The aTestCaseInfo is TTestInfoPckg 
        * that is a packaged TTestInfo. TTestInfo is got from the Test Engine
        * by calling RTestEngine::GetTestCases method.
        * If there was previously called RemoveTestModule for the Test Module
        * that is defined in aTestCaseInfo this method returns KErrLocked.
        * This means that new test cases cannot be run from this Test Module
        * and the user should inquire test cases available calling 
        * RTestEngine::GetTestCases.
        */
        IMPORT_C TInt Open( RTestEngineServer& aServer,
                                const TTestInfoPckg& aTestCaseInfo );

        /**
        * Closes the RTestCase subsession. Test case is normally closed
        * immediately after the test case was completed.
        */
        IMPORT_C void Close();

    public:     // New functions

        /**
        * RunTestCase is asynchronous method and it is completed when the test
        * case is completed. Result of the test case is returned to 
        * aTestResult. The aTestResult is TFullTestResultPckg that is a
        * packaged TFullTestResult.
        * The aStatus will have the completion result of this function. If the
        * test case could be run (despite of succesfully or not) the KErrNone 
        * is returned to aStatus.
        */
        IMPORT_C void RunTestCase( TFullTestResultPckg& aTestResult,
                                    TRequestStatus& aStatus );

        /**
        * RunTestCase is asynchronous method and it is completed when the test
        * case is completed. Result of the test case is returned to 
        * aTestResult. The aTestResult is TFullTestResultPckg that is a
        * packaged TFullTestResult.
        * The aStatus will have the completion result of this function. If the
        * test case could be run (despite of succesfully or not) the KErrNone 
        * is returned to aStatus.
        */
        IMPORT_C void RunTestCase( TFullTestResultPckg& aTestResult,
                                   const TDesC& aTestCaseArgs,
                                   TRequestStatus& aStatus );        

        /**
        * Pause suspends the execution of the test case. The subsession where
        * the test case is run is suspended and thus the test case execution is
        * suspended. Test case execution can be later resumed by calling 
        * Resume.
        */
        IMPORT_C TInt Pause();

        /**
        * Resume is used to resume the test case suspended by previously called
        * Pause method. The test case execution should be continued immediately
        * when the Resume is called.
        */
        IMPORT_C TInt Resume();

        /**
        * NotifyProgress requests different progress information from the test
        * case execution. This information can be printed to UI. The progress
        * information is returned to aProgress. The aProgress is
        * TTestProgressPckg that is a packaged TTestProgress 
        */
        IMPORT_C void NotifyProgress( TTestProgressPckg& aProgress,
                                        TRequestStatus& aStatus );
                                        
        /**
        * NotifyRemoteType requests enable message waiting.
        */
        IMPORT_C void NotifyRemoteType( TStifCommandPckg& aType,
                                        TPckg<TInt>& aMsgSize,
                                        TRequestStatus& aStatus );
        /**
        * NotifyRemoteMsg gets messages.
        */
        IMPORT_C TInt NotifyRemoteMsg( TDes8& aMessage,
                                       TStifCommand aType );

        /**
        * Asynchronous request can be canceled with CancelAsyncRequest method.
        * The aReqToCancel parameter defines the request that will be canceled.
        */
        IMPORT_C TInt CancelAsyncRequest( TInt aReqToCancel );

    public:     // Functions from base classes
        
    protected:  // New functions

    protected:  // Functions from base classes

    private:    // New functions

    public:     // Data
    
    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This contains the CFixedFlatArray class definitions.

-------------------------------------------------------------------------------
*/
template <class T>
class CFixedFlatArray 
        :public CBase
    {
    public:     // Enumerations

    private:    // Enumerations

    public:     // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        inline static CFixedFlatArray<T>* NewL(const TInt aSize);

        /**
        * Destructor.
        */
        inline ~CFixedFlatArray();

    public:     // New functions

        /**
        * Return descriptor containing array. Used in server calls
        */
        inline TPtr8& Des();

        /**
        * Returns CFixedFlatArray internal array. Used in server calls
        */
        inline T& operator[] (TInt aIndex) const;

        /**
        * Returns count
        */
        inline TInt Count() const;

        /**
        * Sets aBuf to array slot specified by aIndex
        */
        inline void Set( TInt aIndex, T& aBuf );

    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        inline void ConstructL(const TInt aSize);

        /**
        * C++ default constructor.
        */
        inline CFixedFlatArray();

        /**
        * Check that given index is correct.
        */
        inline void CheckIndex( TInt aIndex ) const;

    protected:  // Data

    private:    // Data
        // Array
        T*      iArray;

        // Count
        TInt    iCount;

        // Buffer
        HBufC8* iBuffer;
        TPtr8   iBufferPtr;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#include <TestEngineClient.inl>


#endif // TEST_ENGINE_CLIENT_H

// End of File
