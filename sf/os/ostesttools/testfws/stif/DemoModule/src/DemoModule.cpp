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
* Description: This file (DemoModule.cpp) contains all test 
* framework related parts of this test module. Actual test cases 
* are implemented in file DemoModuleCases.cpp.
*
* CTestModuleDemo is an example of test module implementation. This example
* uses hard coded test cases (i.e it does not have any test case
* configuration file).
*
* Example uses function pointers to call test cases. This provides an easy
* method to add new test cases.
*
* See function Cases in file DemoModuleCases.cpp for instructions how to
* add new test cases. It is not necessary to modify this file when adding
* new test cases.
*
* To take this module into use, add following lines to test framework
* initialisation file:
*
* # Demo module
* [New_Module]
* ModuleName= DemoModule
* [End_Module]
*
*/

// INCLUDE FILES
#include <StifTestInterface.h>
#include "DemoModule.h"
#include <e32math.h>
#include "SettingServerClient.h"

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

// ==================== LOCAL FUNCTIONS =======================================


// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CDemoModule

    Method: CDemoModule

    Description: C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CDemoModule::CDemoModule()
    {
    }

/*
-------------------------------------------------------------------------------

    Class: CDemoModule

    Method: ConstructL

    Description: Symbian 2nd phase constructor that can leave.

    Note: If OOM test case uses STIF Logger, then STIF Logger must be created
    with static buffer size parameter (aStaticBufferSize). Otherwise Logger 
    allocates memory from heap and therefore causes error situations with OOM 
    testing. For more information about STIF Logger construction, see STIF 
    Users Guide.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CDemoModule::ConstructL()
    {
    //Read logger settings to check whether test case name is to be
    //appended to log file name.
    RSettingServer settingServer;
    TInt ret = settingServer.Connect();
    if(ret != KErrNone)
        {
        User::Leave(ret);
        }
    // Struct to StifLogger settigs.
    TLoggerSettings loggerSettings; 
    // Parse StifLogger defaults from STIF initialization file.
    ret = settingServer.GetLoggerSettings(loggerSettings);
    if(ret != KErrNone)
        {
        User::Leave(ret);
        } 
    // Close Setting server session
    settingServer.Close();
    iAddTestCaseTitleToLogName = loggerSettings.iAddTestCaseTitle;
    
    // Constructing static buffer size logger, needed with OOM testing because
    // normally logger allocates memory from heap!
    iStdLog = CStifLogger::NewL( KDemoModuleLogPath, 
                              KDemoModuleLogFile,
                              CStifLogger::ETxt,
                              CStifLogger::EFile,
                              ETrue,
                              ETrue,
                              ETrue,
                              EFalse,
                              ETrue,
                              EFalse,
                              100 );
    iLog = iStdLog;
    
    // Sample how to use logging
    _LIT( KLogInfo, "DemoModule logging starts!" );
    iLog->Log( KLogInfo );
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleDemo

    Method: NewL

    Description: Two-phased constructor. Constructs new CTestModuleDemo
    instance and returns pointer to it.

    Parameters:    None

    Return Values: CTestModuleDemo*: new object.

    Errors/Exceptions: Leaves if memory allocation fails or ConstructL leaves.

    Status: Approved

-------------------------------------------------------------------------------
*/
CDemoModule* CDemoModule::NewL()
    {
    CDemoModule* self = new (ELeave) CDemoModule;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleDemo

    Method: ~CTestModuleDemo

    Description: Destructor.

    Parameters:    None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CDemoModule::~CDemoModule()
    { 
    iLog = NULL;
    delete iStdLog;
    iStdLog = NULL;
    delete iTCLog;
    iTCLog = NULL;
    }

/*
-------------------------------------------------------------------------------
    Class: CTestModuleDemo

    Method: InitL

    Description: Method for test case initialization
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
-------------------------------------------------------------------------------
*/
TInt CDemoModule::InitL( TFileName& /*aIniFile*/, 
                         TBool /*aFirstTime*/ )
    {
    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleDemo

    Method: GetTestCases

    Description: GetTestCases is used to inquire test cases 
    from the test module. Because this test module has hard coded test cases
    (i.e cases are not read from file), paramter aConfigFile is not used.

    This function loops through all cases defined in Cases() function and 
    adds corresponding items to aTestCases array.

    Parameters: const TFileName&  : in: Configuration file name. Not used                                                       
                RPointerArray<TTestCaseInfo>& aTestCases: out: 
                      Array of TestCases.
    
    Return Values: KErrNone: No error

    Errors/Exceptions: Function leaves if any memory allocation operation fails

    Status: Proposal
    
-------------------------------------------------------------------------------
*/      
TInt CDemoModule::GetTestCasesL( const TFileName& /*aConfig*/, 
                                 RPointerArray<TTestCaseInfo>& aTestCases )
    {
    // Loop through all test cases and create new
    // TTestCaseInfo items and append items to aTestCase array    
    for( TInt i = 0; Case(i).iMethod != NULL; i++ )
        {
        // Allocate new TTestCaseInfo from heap for a testcase definition.
        TTestCaseInfo* newCase = new( ELeave ) TTestCaseInfo();
    
        // PushL TTestCaseInfo to CleanupStack.    
        CleanupStack::PushL( newCase );

        // Set number for the testcase.
        // When the testcase is run, this comes as a parameter to RunTestCaseL.
        newCase->iCaseNumber = i;

        // Set title for the test case. This is shown in UI to user.
        newCase->iTitle.Copy( Case(i).iCaseName );

        // Append TTestCaseInfo to the testcase array. After appended 
        // successfully the TTestCaseInfo object is owned (and freed) 
        // by the TestServer. 
        User::LeaveIfError(aTestCases.Append ( newCase ) );
        
        // Pop TTestCaseInfo from the CleanupStack.
        CleanupStack::Pop( newCase );
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleDemo

    Method: RunTestCase

    Description: Run a specified testcase.

    Function runs a test case specified by test case number. Test case file
    parameter is not used.

    If case number is valid, this function runs a test case returned by
    function Cases(). 

    Parameters: const TInt aCaseNumber: in: Testcase number 
                const TFileName& : in: Configuration file name. Not used
                TTestResult& aResult: out: Testcase result

    Return Values: KErrNone: Testcase ran.
                   KErrNotFound: Unknown testcase

    Errors/Exceptions: None
    
    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CDemoModule::RunTestCaseL( const TInt aCaseNumber,
                                const TFileName& /* aConfig */,
                                TTestResult& aResult )
    {
    // Return value
    TInt execStatus = KErrNone;

    // Get the pointer to test case function
    TCaseInfo tmp = Case ( aCaseNumber );

    _LIT( KLogInfo, "Starting testcase [%S]" );
    iLog->Log( KLogInfo, &tmp.iCaseName);

    // Check that case number was valid
    if ( tmp.iMethod != NULL )
        {
        //Open new log file with test case title in file name
        if(iAddTestCaseTitleToLogName)
            {
            //delete iLog; //Close currently opened log
            //iLog = NULL;
            //Delete test case logger if exists
            if(iTCLog)
                {
                delete iTCLog;
                iTCLog = NULL;
                }
                
            TFileName logFileName;
            TName title;
            TestModuleIf().GetTestCaseTitleL(title);
        
            logFileName.Format(KDemoModuleLogFileWithTitle, &title);

            iTCLog = CStifLogger::NewL(KDemoModuleLogPath, 
                                     logFileName,
                                     CStifLogger::ETxt,
                                     CStifLogger::EFile,
                                     ETrue,
                                     ETrue,
                                     ETrue,
                                     EFalse,
                                     ETrue,
                                     EFalse,
                                     100);
            iLog = iTCLog;
            }

        // Valid case was found, call it via function pointer
        iMethod = tmp.iMethod;        
        //execStatus  = ( this->*iMethod )( aResult );
        TRAPD(err, execStatus  = ( this->*iMethod )( aResult ));
        if(iAddTestCaseTitleToLogName)
            {
            //Restore standard log and destroy test case logger
            iLog = iStdLog;
            delete iTCLog; //Close currently opened log
            iTCLog = NULL;
            }
        User::LeaveIfError(err);
        
        }
    else
        {
        // Valid case was not found, return error.
        execStatus = KErrNotFound;
        }

    // Return case execution status (not the result of the case execution)
    return execStatus;

    }

/*
-------------------------------------------------------------------------------

    Class: CDemoModule

    Method: OOMTestQueryL

    Description: Checks test case information for OOM execution. 

    Return Values: TBool

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TBool CDemoModule::OOMTestQueryL( const TFileName& /* aTestCaseFile */, 
                                  const TInt aCaseNumber, 
                                  TOOMFailureType& /* aFailureType */, 
                                  TInt& aFirstMemFailure, 
                                  TInt& aLastMemFailure ) 
    {
    _LIT( KLogInfo, "CDemoModule::OOMTestQueryL" );
    iLog->Log( KLogInfo ); 

    aFirstMemFailure = Case( aCaseNumber ).iFirstMemoryAllocation;
    aLastMemFailure = Case( aCaseNumber ).iLastMemoryAllocation;

    return Case( aCaseNumber ).iIsOOMTest;
    }

/*
-------------------------------------------------------------------------------

    Class: CDemoModule

    Method: OOMTestInitializeL

    Description: Used to perform the test environment setup for a particular
    OOM test case. Test Modules may use the initialization file to read 
    parameters for Test Module initialization but they can also have their own
    configure file or some other routine to initialize themselves. 

    NOTE: User may add implementation for OOM test environment initialization.
    Usually no implementation is required.

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CDemoModule::OOMTestInitializeL( const TFileName& /* aTestCaseFile */, 
                                      const TInt /* aCaseNumber */ )
    {
    _LIT( KLogInfo, "CDemoModule::OOMTestInitializeL" );
    iLog->Log( KLogInfo ); 

    }

/*
-------------------------------------------------------------------------------

    Class: CDemoModule

    Method: OOMHandleWarningL

    Description: Used in OOM testing to provide a way to the derived TestModule 
    to handle warnings related to non-leaving or TRAPped allocations.

    In some cases the allocation should be skipped, either due to problems in 
    the OS code or components used by the code being tested, or even inside the
    tested components which are implemented this way on purpose (by design), so
    it is important to give the tester a way to bypass allocation failures.

    NOTE: User may add implementation for OOM test warning handling. Usually no
    implementation is required.    

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CDemoModule::OOMHandleWarningL( const TFileName& /* aTestCaseFile */,
                                     const TInt /* aCaseNumber */, 
                                     TInt& /* aFailNextValue */ )
    {
    _LIT( KLogInfo, "CDemoModule::OOMHandleWarningL" );
    iLog->Log( KLogInfo );

    }

/*
-------------------------------------------------------------------------------

    Class: CDemoModule

    Method: OOMTestFinalizeL

    Description: Used to perform the test environment cleanup for a particular OOM 
    test case.

    NOTE: User may add implementation for OOM test environment finalization.
    Usually no implementation is required.   

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CDemoModule::OOMTestFinalizeL( const TFileName& /* aTestCaseFile */, 
                                    const TInt /* aCaseNumber */ )
    {
    _LIT( KLogInfo, "CDemoModule::OOMTestFinalizeL" );
    iLog->Log( KLogInfo );

    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point
// Returns: CTestModuleBase*: Pointer to Test Module object
// -----------------------------------------------------------------------------
//
EXPORT_C CTestModuleBase* LibEntryL()
    {
    return CDemoModule::NewL();

    }

// -----------------------------------------------------------------------------
// SetRequirements handles test module parameters(implements evolution
// version 1 for test module's heap and stack sizes configuring).
// Returns: TInt: Symbian error code.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SetRequirements( CTestModuleParam*& /*aTestModuleParam*/, 
                                TUint32& /*aParameterValid*/ )
    {

    /* --------------------------------- NOTE ---------------------------------
    USER PANICS occurs in test thread creation when:
    1) "The panic occurs when the value of the stack size is negative."
    2) "The panic occurs if the minimum heap size specified is less
       than KMinHeapSize".
       KMinHeapSize: "Functions that require a new heap to be allocated will
       either panic, or will reset the required heap size to this value if
       a smaller heap size is specified".
    3) "The panic occurs if the minimum heap size specified is greater than
       the maximum size to which the heap can grow".
    Other:
    1) Make sure that your hardware or Symbian OS is supporting given sizes.
       e.g. Hardware might support only sizes that are divisible by four.
    ------------------------------- NOTE end ------------------------------- */

    // Normally STIF uses default heap and stack sizes for test thread, see:
    // KTestThreadMinHeap, KTestThreadMinHeap and KStackSize.
    // If needed heap and stack sizes can be configured here by user. Remove
    // comments and define sizes.

/*
    aParameterValid = KStifTestModuleParameterChanged;

    CTestModuleParamVer01* param = CTestModuleParamVer01::NewL();
    // Stack size
    param->iTestThreadStackSize= 16384; // 16K stack
    // Heap sizes
    param->iTestThreadMinHeap = 4096;   // 4K heap min
    param->iTestThreadMaxHeap = 1048576;// 1M heap max

    aTestModuleParam = param;
*/
    return KErrNone;

    }

// End of File
