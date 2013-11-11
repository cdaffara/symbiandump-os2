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
* Description: CUIEngine: This object executes test cases from 
* STIF Test Framework.
* 
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32svr.h>
#include <collate.h>
#include "UIEngineEvent.h"
#include <stifinternal/UIEngineContainer.h>
#include "UIEngineError.h"
#include <stifinternal/UIEngine.h>
#include "Logging.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
_LIT( KUIEngine, "CUIEngine" );

// MACROS
#ifdef LOGGER
#undef LOGGER
#endif
#define LOGGER iLogger

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

    Class: CUIEngine

    Method: CUIEngine

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.
    
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIEngine::CUIEngine( CUIIf* aUi  ):
    iUi( aUi )
    {

    __RDEBUG( _L( "CUIEngine::CUIEngine()" ) );
    
    }

/*
-------------------------------------------------------------------------------

    Class: CUIEngine

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if called Open method returns error

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEngine::ConstructL()
    {
    // Logger's setting definitions
    TLoggerSettings loggerSettings;
    loggerSettings.iCreateLogDirectories = EFalse;
    loggerSettings.iOverwrite = ETrue;
    loggerSettings.iTimeStamp = ETrue;
    loggerSettings.iLineBreak = ETrue;
    loggerSettings.iEventRanking = EFalse;
    loggerSettings.iThreadId = EFalse;
    loggerSettings.iHardwareFormat = CStifLogger::ETxt;
    loggerSettings.iHardwareOutput = CStifLogger::EFile;
    loggerSettings.iEmulatorFormat = CStifLogger::ETxt;
    loggerSettings.iEmulatorOutput = CStifLogger::EFile;
    loggerSettings.iUnicode = EFalse;
    loggerSettings.iAddTestCaseTitle = EFalse;

    // Create Logger, Note: Use protected NewL because initialization file not
    // readed yet.
    iLogger = CStifLogger::NewL( KUiLogDir, KUiLogFile, loggerSettings );
    
    // Initialize logger    
    iUi->InitializeLogger( iLogger );

    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngine

    Method: NewL

    Description: Two-phased constructor.
    
    Parameters: None
    
    Return Values: CUIEngine* : Pointer to created UI engine object

    Errors/Exceptions: Leaves if memory allocation for CUIEngine fails
                       Leaves if ConstructL leaves

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIEngine* CUIEngine::NewL( CUIIf* aUi )
    {
    
    CUIEngine* self =  
        new ( ELeave ) CUIEngine( aUi );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    
    }



/*
-------------------------------------------------------------------------------

    Class: CUIEngine

    Method: ~CUIEngine

    Description: Destructor
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIEngine::~CUIEngine()
    {
    
    __RDEBUG( _L( "CUIEngine::~CUIEngine()" ) );
    
    // Delete all containers, cannot use ResetAndDestroy,
    // because destructor is private
    TInt count = iContainerArray.Count();
    for( TInt i=0; i< count; i++ )
        {
        delete iContainerArray[i];
        }
    iContainerArray.Reset();    
    iContainerArray.Close();

    delete iLogger;
    iLogger = NULL;
    
    delete iError;
    iError = 0;
        
    }


/*
-------------------------------------------------------------------------------

    Class: VUIEngine

    Method: Open

    Description: Open test engine.

    Parameters: TDesC& aTestFrameworkIni: in: Initialization file to Test Framework

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIEngine::Open( const TDesC& aTestFrameworkIni )
    {
    
    if( ( iTestEngine.SubSessionHandle() != 0 ) ||
        ( iTestEngineServ.Handle() != 0 ) )
        {
        __TRACE( KError, ( CStifLogger::EError, _L( "CUIEngine::Open() Already open" ) ) );
        return KErrInUse;     
        }

    // Check aTestFrameworkIni max length
    if( aTestFrameworkIni.Length() > KMaxFileName )
        {
        __TRACE( KPrint, ( CStifLogger::EError, _L( "CUIEngine::Open() method's parameter length is incorrect" ) ) );
        return KErrArgument;        
        }

    // Create Test Engine
    TInt ret = iTestEngineServ.Connect();

    if ( ret != KErrNone )
        {
        __TRACE( KPrint, ( _L( "CUIEngine::Open. Engine server open failed: ret = %d"), ret ) );

        return ret;
        }

    ret = iTestEngine.Open( iTestEngineServ, aTestFrameworkIni );

    if ( ret != KErrNone )
        {
        __TRACE( KPrint, ( _L( "CUIEngine::Open. Engine. Open failed: ret = %d"), ret ) );

        return ret;
        }

    TErrorNotification error;
    if( ErrorPrint( error ) != KErrNotSupported )
        {
        // Start printer
        iError = CUIEngineErrorPrinter::NewL( this );
        iError->StartL( iTestEngine );
        }

    return ret;
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngine

    Method: Close

    Description: Close test engine.

    Parameters: None

    Return Values: TInt KErrNone: Always returned KErrNone

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIEngine::Close()
    {
    
    delete iError;
    iError = 0;
    
    // Delete all containers, cannot use ResetAndDestroy,
    // because destructor is private
    TInt count = iContainerArray.Count();
    for( TInt i=0; i< count; i++ )
        {
        delete iContainerArray[i];
		iContainerArray[i] = NULL;
        }
    iContainerArray.Reset();
    
    iTestEngine.Close();
    iTestEngineServ.Close();

    __TRACE( KPrint, ( _L( "CUIEngine::Close." ) ) );

    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CUIEngine

    Method: AddTestModule

    Description: Add test module to module list of test engine

    Parameters: TDesC& aModuleName: in: Testmodule, which is added to module list
                TDesC& aIniFile: in: Initialization file to the test module

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIEngine::AddTestModule( const TDesC& aModuleName, 
                               const TDesC& aIniFile )
    {
    // Check aModuleName max length
    if( aModuleName.Length() > KMaxName )
        {
        __TRACE( KPrint, ( CStifLogger::EError, _L( "CUIEngine::AddTestModule() method's first parameter length is incorrect" ) ) );
        return KErrArgument;        
        }
    // Check aIniFile max length
    if( aIniFile.Length() > KMaxFileName )
        {
        __TRACE( KPrint, ( CStifLogger::EError, _L( "CUIEngine::AddTestModule() method's second parameter length is incorrect" ) ) );
        return KErrArgument;        
        }    


    TInt ret = iTestEngine.AddTestModule( aModuleName, aIniFile );

    if ( ret != KErrNone && ret != KErrAlreadyExists )
        {
		__TRACE( KPrint, ( _L( "CUIEngine::AddTestModule. Add module failed: ret = %d" ), ret  ) );

        return ret;
        }

    return KErrNone;
    
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngine

    Method: RemoveTestModule

    Description: Remove test module from test engine

    Parameters: TDesC& aModuleName: in: Testmodule, which is removed of module list

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIEngine::RemoveTestModule( const TDesC& aModuleName )
    {
    // Remove test module of module list of test engine
    TInt ret = iTestEngine.RemoveTestModule( aModuleName );

    if ( ret != KErrNone )
        {
        __TRACE( KPrint, ( _L( "CUIEngine::RemoveTestModule. Remove module failed: ret = %d"), ret  ) );

        return ret;
        }

    return ret;
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngine

    Method: AddTestCaseFile

    Description: Add test case file to test case file list of test engine

    Parameters: TDesC& aModuleName: in: Testmodule, which own test cases of test case list.
                TDesC& aCaseFile: in: Test case list, which is added to test case list

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIEngine::AddTestCaseFile( const TDesC& aModuleName, 
                                 const TDesC& aCaseFile )
    {
    // Check aModuleName max length
    if( aModuleName.Length() > KMaxName )
        {
        __TRACE( KPrint, ( CStifLogger::EError, _L( "CUIEngine::AddTestCaseFile() method's first parameter length is incorrect" ) ) );
        return KErrArgument;        
        }
    // Check aCaseFile max length
    if( aCaseFile.Length() > KMaxFileName )
        {
        __TRACE( KPrint, ( CStifLogger::EError, _L( "CUIEngine::AddTestCaseFile() method's second parameter length is incorrect" ) ) );
        return KErrArgument;        
        }
    
    TInt ret( KErrNone );

    // Add given test case file to test module
    if ( aCaseFile.Length() == 0 )
        {
        ret = KErrArgument;
        __TRACE( KPrint, (  _L( "CUIEngine::AddTestCaseFile. Test case file not defined: ret = %d"), ret  ) );
        }
    else
        {
        // Add test case file to test case file list of test engine 
        ret = iTestEngine.AddConfigFile( aModuleName, aCaseFile );

        if ( ret != KErrNone && ret != KErrAlreadyExists )
            {
            __TRACE( KPrint, ( _L( "CUIEngine::AddTestCaseFile. Add test case file failed: ret = %d"), ret  ) );
            }
        }

    return ret;
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngine

    Method: RemoveTestCaseFile

    Description: Remove test case file of test case file list of test engine

    Parameters: TDesC& aModuleName: in: Testmodule, which own test cases of test case list
                TDesC& aCaseFile: in: Test case list, which is removed of test case list

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIEngine::RemoveTestCaseFile( const TDesC& aModuleName, 
                                    const TDesC& aCaseFile )
    {   
    
    TInt ret( KErrNone );

    // Remove given test case file from test module
    if ( aCaseFile.Length() == 0 )
        {
        ret = KErrArgument;
        __TRACE( KPrint, (  _L( "CUIEngine::RemoveTestCaseFile. Test case file not defined: ret = %d"), ret  ) );
        }
    else
        {
        // Remove test case file  
        ret = iTestEngine.RemoveConfigFile( aModuleName, aCaseFile );

        if ( ret != KErrNone )
            {
            __TRACE( KPrint, ( _L( "CUIEngine::RemoveTestCaseFile. Remove test case file failed: ret = %d"), ret  ) );
            }
        }

    return ret;

    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngine

    Method: GetTestCases

    Description: Get test cases of enumerated list of test engine. Method is copied 
                 list of test cases to aTestCaseInfo. Copied tests are test cases 
                 of test module (aTestModule) and defined
                 in test case file (aTestCaseFile), which are given as argument

                 Method copied all enumerated test cases if aTestModule
                 and aTestCaseFile are not defined
    
    Parameters: RPointerArray<CTestInfo>& aTestCaseInfo: inout: List of test cases
                TDesC& aModuleName: in: Testmodule, which own test cases of test case list
                TDesC& aTestCaseFile: in: Test case list, which is got of test case list

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIEngine::GetTestCasesL( RPointerArray<CTestInfo>& aTestCaseInfo,
                               const TDesC& aTestModule, 
                               const TDesC& aTestCaseFile )
    { 
	__TRACE( KPrint, (  _L( "GetTestCasesL()") ) );

    TInt ret(KErrNone);
    TInt count(0);

    // Enumerate test cases
    TCaseCount caseCount;

    TRequestStatus status;
    iTestEngine.EnumerateTestCases( caseCount, status );
    User::WaitForRequest( status );
    ret = status.Int();

    // Check that enumerate succeeded
    if ( ret != KErrNone )
        {
        __TRACE( KPrint, (  _L( "CUIEngine::GetTestCases. Test case enumerate failed: ret = %d"), ret ) );
        TErrorNotification error;
        error.iText = _L("Can't get test cases");
        iUi->ErrorPrint ( error );  
        return ret;
        }

    CFixedFlatArray<TTestInfo>* testCases = NULL;

    // Create test cases to buffer
    TRAPD( err, testCases = CFixedFlatArray<TTestInfo>::NewL( caseCount() ) );
    if( err != KErrNone )
        {        
        if( err == KErrNoMemory )
            {
            _LIT( KNotEnoughHeapMemory, 
                "Not enough heap memory available. Either reduce the test case count or increase UI's heap size in mmp file using EPOCHEAPSIZE.");
            RDebug::Print( KNotEnoughHeapMemory );
        	__TRACE( KError, ( KNotEnoughHeapMemory ) );        
        	}
        else
           {
            _LIT( KGeneralError,
                "CUIEngine::GetTestCasesL: CFixedFlatArray<TTestInfo>::NewL fails with value: %d");
            RDebug::Print( KGeneralError, err );
        	__TRACE( KError, ( KGeneralError, err ) );                   
           }        	
        User::Leave( err );
        }    
            
    CleanupStack::PushL( testCases );

    // Get test cases from Engine.
    ret = iTestEngine.GetTestCases( *testCases );

    if ( ret != KErrNone )
        {
        __TRACE( KPrint, (  _L( "CUIEngine::GetTestCases. Get test cases failed: ret = %d"), ret ) );

        CleanupStack::PopAndDestroy( testCases );
        return ret;
        }

    // count of test cases.
    count = caseCount();

    __TRACE( KPrint, (  _L( "Test case count: %d"), count ) );
    
    HBufC* moduleNameBuf = aTestModule.AllocLC();
    TPtr moduleName( moduleNameBuf->Des() );
    TCollationMethod method = 
        *Mem::CollationMethodByIndex(0); // get the standard method
    method.iFlags |= TCollationMethod::EFoldCase; // ignore case

    moduleName.LowerCase();

    // Remove optional '.DLL' from test module file name
    TParse parse;
    parse.Set( moduleName, NULL, NULL );
    
    if( !parse.Ext().CompareC(  _L(".dll"), 3, &method ) )
        {
        const TInt len = parse.Ext().Length();
        moduleName.Delete ( moduleName.Length()-len, len );
        }
        
    TBool found = EFalse;
    // Update iTestCaseTable.
    for (TInt j=0;j<count;j++, found = EFalse)
        {
        // Check, if module defined.
        if ( aTestModule.Length() > 0 )
            {
            // Check, if test case defined.
            if ( aTestCaseFile.Length() > 0)
                {
                if( moduleName == KTestScripterName )
                    {
                    if( !( *testCases )[j].iConfig.CompareC( aTestCaseFile, 3, &method ) )
                        {
                        found = ETrue;
                        }
                    }
                if( !( *testCases )[j].iModuleName.CompareC( moduleName, 3, &method )
                    && !( *testCases )[j].iConfig.CompareC( aTestCaseFile, 3, &method ) )
                    {
                    found = ETrue;
                    }
                }
            else
                {
                if( moduleName == KTestScripterName )
                    {
                    // Test Module is TestScripter
                    if( ( *testCases )[j].iModuleName.Find( KTestScripterName ) != KErrNotFound )
                        {
                        // iModuleName name is in 'testscripter_testcasefile'
                        // format => Find()
                        found = ETrue;
                        }
                    }
                if( !( *testCases )[j].iModuleName.CompareC( 
                                            moduleName, 3, &method ) )
                    {
                    found = ETrue;
                    }
                }
            }
        else
            {
            found = ETrue;
            }
        
        if( found )
            {
            // Create tmpTestInfo.
            CTestInfo *tmpTestInfo = CTestInfo::NewL();
             
            CleanupStack::PushL( tmpTestInfo );            

            // Copy TTestInfo to CTestInfo.
            tmpTestInfo->SetModuleName( ( *testCases )[j].iModuleName );
            tmpTestInfo->SetTestCaseTitle( ( *testCases )[j].iTestCaseInfo.iTitle );
            tmpTestInfo->SetTestCaseNumber( ( *testCases )[j].iTestCaseInfo.iCaseNumber );
            tmpTestInfo->SetPriority( ( *testCases )[j].iTestCaseInfo.iPriority );
            tmpTestInfo->SetTimeout( ( *testCases )[j].iTestCaseInfo.iTimeout );
            tmpTestInfo->SetTestCaseFile( ( *testCases )[j].iConfig );
            ret = aTestCaseInfo.Append(tmpTestInfo);
            if( ret != KErrNone )
                {
                CleanupStack::PopAndDestroy( tmpTestInfo );
                }
            else
                {
                CleanupStack::Pop( tmpTestInfo );
                }
            }
        }
    
    CleanupStack::PopAndDestroy( moduleNameBuf );

    CleanupStack::PopAndDestroy( testCases );

    return ret;
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngine

    Method: StartTestCase

    Description: Start test case execution.

    Parameters: TInt& aTestId: in: ID for test case. Test ID given of address of
                current UIEngineContainer.

                const CTestInfo& aTestInfo: in: Test case information

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIEngine::StartTestCase( CUIEngineContainer*& aContainer,
                              const CTestInfo& aTestInfo )
    {
    TInt ret( KErrNone );

    // Copy CTestInfo to TTestInfo.
    TTestInfo testCase; 
    testCase.iModuleName = aTestInfo.ModuleName();
    testCase.iTestCaseInfo.iTitle = aTestInfo.TestCaseTitle();
    testCase.iTestCaseInfo.iCaseNumber = aTestInfo.TestCaseNum();
    testCase.iTestCaseInfo.iPriority = aTestInfo.Priority();
    testCase.iTestCaseInfo.iTimeout = aTestInfo.Timeout();
    testCase.iConfig = aTestInfo.TestCaseFile();

    CUIEngineContainer* container = NULL;
    TRAP( ret,
        // Create container.
        container = 
            CUIEngineContainer::NewL( this, testCase, iTestEngineServ, iTestEngine );
    );
    if( ret != KErrNone )
        {
        return ret;
        }
    
    ret = iContainerArray.Append( container );
    if( ret != KErrNone )
        {
        __TRACE( KError, ( CStifLogger::EError, 
            _L( "CUIEngine::StartTestCase() Append failed, cannot start test case" ) ) );
        delete container;
        return ret;
        }
        
    // Call Container to starting test case execution.
    TRAPD( trapError,
    // Call Container to starting test case execution.
    ret = container->StartContainerL();
    );

    if(trapError != KErrNone)
        {
        __TRACE( KPrint, (  _L( "CUIEngine::StartTestCase. Test case starting failed: trapError = %d"), ret ) );
        delete container;
        return trapError;
        }
        
    aContainer = container;

    return ret;
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CUIEngine

    Method: AbortStartedTestCase

    Description: Abort started test case execution.

    Parameters: CUIEngineContainer* aContainer: in: Container running testcase

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: Panics if test case is not found or is already executed

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEngine::AbortStartedTestCase( CUIEngineContainer* aContainer )
    {
    
    // Locate container from array
    TInt index = iContainerArray.Find( aContainer );
    
    if( ( index < 0) ||
        ( aContainer->State() == CUIEngineContainer::EExecuted ) )
        {
        __TRACE( KError, ( CStifLogger::EError, 
            _L( "CUIEngine::AbortStartedTestCase() Testcase executed" ) ) );
        User::Panic( KUIEngine, KErrAccessDenied );
        }
        
    // Remove and delete container
    iContainerArray.Remove( index );
    delete aContainer;
    
    }

    
/*
-------------------------------------------------------------------------------

    Class: CUIEngine

    Method: TestExecuted

    Description: 

    Parameters: CUIEngineContainer* aUIEngineContainer: Address of current
                CUIEngineContainer is test case ID.

                TFullTestResult& aFullTestResult: in: Test result of executed test case

    Return Values: TInt KErrNone: Always returned KErrNone

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEngine::TestExecuted( CUIEngineContainer* aContainer, 
                               TFullTestResult& aFullTestResult )
    {
     // Locate container from array
    TInt index = iContainerArray.Find( aContainer );
    
    if( index < 0 )
        {
        __TRACE( KError, ( CStifLogger::EError, 
            _L( "CUIEngine::TestExecuted() Testcase not found" ) ) );
        User::Panic( KUIEngine, KErrNotFound );
        }
    
    iUi->TestExecuted( aContainer, aFullTestResult );
    
    // Remove and delete container.
    // It is safe to do here,
    // because container and runner does not have any code after
    // this function returns.
    iContainerArray.Remove( index );
    delete aContainer;
   
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngine

    Method: PrintProg

    Description: 

    Parameters: CUIEngineContainer* aContainer: in: Address of current
                CUIEngineContainer is test case ID

                TTestProgress& aProgress: in: Progress information from test case

    Return Values: SymbianOS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIEngine::PrintProg( CUIEngineContainer* aContainer, 
                          TTestProgress& aProgress )
    {

    return iUi->PrintProg( aContainer, aProgress );
        
    }

/*
-------------------------------------------------------------------------------

    Class: CUIEngine

    Method: ErrorPrint

    Description: 

    Parameters: TErrorNotification& aError: in: Error information from framework

    Return Values: SymbianOS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIEngine::ErrorPrint( TErrorNotification& aError )
    {

    return iUi->ErrorPrint( aError );
        
    }
   
/*
-------------------------------------------------------------------------------

    Class: CUIEngine

    Method: RemoteMsg

    Description: Forward Ats send.

    Parameters: const TDesC& aMessage: in: message
    
    Return Values: SymbianOS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIEngine::RemoteMsg( CUIEngineContainer* aContainer, 
                           const TDesC& aMessage )
    {

    return iUi->RemoteMsg( aContainer, aMessage );
        
    }

/*
-------------------------------------------------------------------------------

    Class: CUIEngine

    Method: GoingToReboot

    Description: Forward reboot indication.

    Parameters: None
    
    Return Values: SymbianOS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIEngine::GoingToReboot( CUIEngineContainer* aContainer,
                               TRequestStatus& aStatus )
    {

    return iUi->GoingToReboot( aContainer, aStatus );
        
    }


/*
-------------------------------------------------------------------------------

    DESCRIPTION

    CTestInfo: This object contains test case information.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================


/*
-------------------------------------------------------------------------------

    Class: CTestInfo

    Method: NewL

    Description:

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C CTestInfo* CTestInfo::NewL()
    {
    
    //RDebug::Print(_L("CTestInfo::NewL()"));
    CTestInfo* self =  
        new ( ELeave ) CTestInfo();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestInfo

    Method: CTestInfo

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.
    
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestInfo::CTestInfo()
    {
    }

/*
-------------------------------------------------------------------------------

    Class: CTestInfo

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if called Open method returns error

    Status: Draft

-------------------------------------------------------------------------------
*/
void CTestInfo::ConstructL()
    {   
    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestInfo

    Method: ~CTestInfo

    Description: Destructor
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestInfo::~CTestInfo()
    {
    
    delete iModuleNameBuf;
    delete iTitleBuf;
    delete iTestCaseFileBuf;
    iModuleNameBuf = 0;
    iTitleBuf = 0;
    iTestCaseFileBuf = 0;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestInfo

    Method: SetModuleName

    Description: Set module name.
    
    Parameters: None

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CTestInfo::SetModuleName ( const TDesC& aModuleName ) 
    {
    
    iModuleName.Set(0,0); 
    delete iModuleNameBuf;

    if( aModuleName.Length() < KTestScripterNameLength )
		{
		iModuleNameBuf = aModuleName.Alloc();
		}
    else
        {
        // Check is TestScripter
	    TPtrC check( aModuleName.Mid( 0, KTestScripterNameLength ) );
	    TInt ret = check.Compare( KTestScripterName );
	    if( ret == KErrNone )
		    {
            iModuleNameBuf = ( aModuleName.Mid( 0, KTestScripterNameLength ) ).Alloc();
		    }
	    else
		    {
		    iModuleNameBuf = aModuleName.Alloc();
		    }
        }

    if( iModuleNameBuf == NULL )
        {
        return KErrNoMemory;
        }
    
    iModuleName.Set( iModuleNameBuf->Des() ); 
    
    return KErrNone;
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestInfo

    Method: SetTestCaseTitle

    Description: Set module name.
    
    Parameters: None

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CTestInfo::SetTestCaseTitle ( const TDesC& aTitle )
    { 
    
    iTitle.Set(0,0); 
    delete iTitleBuf;
    iTitleBuf = aTitle.Alloc();
    if( iTitleBuf == NULL )
        {
        return KErrNoMemory;
        }
    
    iTitle.Set( iTitleBuf->Des() ); 
    
    return KErrNone;
    
    };
    
/*
-------------------------------------------------------------------------------

    Class: CTestInfo

    Method: SetTestCaseFile

    Description: Set test case file name.
    
    Parameters: None

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CTestInfo::SetTestCaseFile ( const TDesC& aTestCaseFile )
    { 
    iTestCaseFile.Set(0,0); 
    delete iTestCaseFileBuf;
    iTestCaseFileBuf = aTestCaseFile.Alloc();
    if( iTestCaseFileBuf == NULL )
        {
        return KErrNoMemory;
        }
    
    iTestCaseFile.Set( iTestCaseFileBuf->Des() );
    
    return KErrNone;
     
    };


/*
-------------------------------------------------------------------------------

    Class: CTestInfo

    Method: CopyL

    Description: Copy existing CTestInfo.
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves on error

    Status: Draft

-------------------------------------------------------------------------------
*/
void CTestInfo::CopyL( const CTestInfo& aTestInfo )
    {

    User::LeaveIfError( SetModuleName( aTestInfo.ModuleName() ));
    User::LeaveIfError( SetTestCaseTitle( aTestInfo.TestCaseTitle() ));
    User::LeaveIfError( SetTestCaseFile( aTestInfo.TestCaseFile() ));
    
    iCaseNumber = aTestInfo.TestCaseNum();
    iPriority = aTestInfo.Priority();
    iTimeout = aTestInfo.Timeout();
    iExpectedResult = aTestInfo.ExpectedResult();
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestInfo

    Method: SetTestCaseFile

    Description: Set test case file name.
    
    Parameters: None

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TBool CTestInfo::operator== ( const CTestInfo& aTestInfo ) const
    {
    
    if( ( aTestInfo.ModuleName() == ModuleName() ) &&
        ( aTestInfo.Priority() == Priority() ) &&
        ( aTestInfo.TestCaseFile() == TestCaseFile() ) &&
        ( aTestInfo.TestCaseNum() == TestCaseNum() ) &&
        ( aTestInfo.Timeout() == Timeout() ) )
        {
        return ETrue;
        }
        
             
    return EFalse;
    
    }
    
// ================= OTHER EXPORTED FUNCTIONS ================================= 

// End of File
