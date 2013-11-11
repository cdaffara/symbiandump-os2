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
* Description: CATSInterface: This object executes test cases from 
* STIF Test Framework.
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32cons.h>
#include <e32svr.h>
#include "ATSInterface.h"
#include "ATSInterfaceRunner.h"

#include "StifTestInterface.h"


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
// None

// ================= MEMBER FUNCTIONS ========================================= 


// ================= MEMBER FUNCTIONS =========================================


/*
-------------------------------------------------------------------------------

    Class: CATSInterface

    Method: CATSInterface

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.
    
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
CATSInterface::CATSInterface()
    {
    // Initialize buffers to zero
    iEngineIniFile.Zero();
    iModuleIniFile.Zero();
    iConfigFile.Zero();
    iTestModule.Zero();

    }


/*
-------------------------------------------------------------------------------

    Class: CATSInterface

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if some of called leaving functions leaves
                       Leaves each time the LogErrorAndLeaveL is called

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CATSInterface::ConstructL()
    {
    RDebug::Print(_L("Creating module list object"));
    TRAPD(err, iModuleList = CTestModuleList::NewL(NULL));
    if(err != KErrNone)
        {
        LogErrorAndLeaveL(_L("CATSInterface::ConstructL"), _L("CTestModuleList::NewL"), err);
        return;
        }
    if(!iModuleList)
        {
        LogErrorAndLeaveL(_L("CATSInterface::ConstructL"), _L("CTestModuleList::NewL - iModuleList is NULL"), KErrGeneral);
        return;
        }

    // Read command line
    ParseCommandLineL();

    // Add to module list info about module taken from command line
    RDebug::Print(_L("Adding command line module to list"));
    TName moduleName;
    moduleName.Copy(iTestModule);
    moduleName.LowerCase();
    err = iModuleList->AddTestModule(moduleName);
    if(err != KErrNone && err != KErrAlreadyExists)
        {
        LogErrorAndLeaveL(_L("CATSInterface::ConstructL"), _L("CTestModuleList::AddTestModule - Could not add module to list of modules"), err);
        return;
        }

    //Get added module
    CTestModuleInfo* moduleInfo = iModuleList->GetModule(moduleName);
    if(!moduleInfo)
        {
        LogErrorAndLeaveL(_L("CATSInterface::ConstructL"), _L("CTestModuleList::GetModule - Could not add get module info from list"), KErrGeneral);
        return;
        }

    //Add ini file if given
    if(iModuleIniFile.Length() > 0)
        {
        TFileName filename;
        filename.Copy(iModuleIniFile);
        filename.LowerCase();
        moduleInfo->SetIniFile(filename);
        }

    //Add config file if given
    if(iConfigFile.Length() > 0)
        {
        TFileName filename;
        filename.Copy(iConfigFile);
        filename.LowerCase();
        moduleInfo->AddCfgFile(filename);
        }

    //Now check all config files if there are included modules
    _LIT(KIncludeModuleStart, "[New_Include_Module]");
    _LIT(KIncludeModuleEnd, "[End_Include_Module]");

    RDebug::Print(_L("Start parsing included modules"));
    CTestCaseFileInfo* finfo = iModuleList->GetUncheckedCfgFile();
    while(finfo)
        {
        TFileName fname;
        finfo->GetCfgFileName(fname);

        RDebug::Print(_L("Checking file: '%S'"), &fname);
        finfo->SetChecked();

        CStifParser* parser = NULL;

        TRAP(err, parser = CStifParser::NewL(_L(""), fname));
        if(err != KErrNone)
            {
            LogErrorAndLeaveL(_L("CATSInterface::ConstructL"), _L("CStifParser::NewL - Could not create parser"), err);
            return;
            }
        CleanupStack::PushL(parser);

        ParseTestModulesL(parser, iModuleList, KIncludeModuleStart, KIncludeModuleEnd);

        CleanupStack::PopAndDestroy(parser);
        finfo = iModuleList->GetUncheckedCfgFile();
        }
    RDebug::Print(_L("End parsing included modules"));

    // Create Test Engine
    RDebug::Print(_L("Creating test engine"));
    TInt ret = iTestEngineServ.Connect();
    if ( ret != KErrNone )
        {
        // Log error
        LogErrorAndLeaveL( _L("CATSInterface::ConstructL"), _L("iTestEngineServ.Connect"), ret );
        return;
        }

    ret = iTestEngine.Open( iTestEngineServ, iEngineIniFile );
    if ( ret != KErrNone )
        {
        // Log error
        LogErrorAndLeaveL( _L("CATSInterface::ConstructL"), _L("iTestEngine.Open"), ret );
        return;
        }

/*
    // Add test module
    ret = iTestEngine.AddTestModule( iTestModule, iModuleIniFile );
    if ( ret != KErrNone && ret != KErrAlreadyExists )
        {
        // Log error
        LogErrorAndLeaveL( _L("CATSInterface::ConstructL"), _L("iTestEngine.AddTestModule"), ret );
        return;
        }
*/
    // Add all test modules and config files
    RDebug::Print(_L("Start creating test modules"));
    moduleInfo = NULL;
    TInt i;
    TInt modCnt = iModuleList->Count();

    for(i = 0; i < modCnt; i++)
        {
        RDebug::Print(_L("Processing module"));
        // Get module
        moduleInfo = iModuleList->GetModule(i);
        if(!moduleInfo)
            {
            RDebug::Print(_L("Could not get module info at index %d"), i);
            continue;
            }

        // Get module name
        TName moduleName;
        moduleInfo->GetModuleName(moduleName);
        RDebug::Print(_L("module name: '%S'"), &moduleName);

        // Get ini file, if exists
        TFileName ini;
        moduleInfo->GetIniFileName(ini);
        if(ini.Length() == 0)
            {
            RDebug::Print(_L("ini file not found"));
            }
        else
            {
            RDebug::Print(_L("ini file: '%S'"), &ini);
            }

        // Create test module
        RDebug::Print(_L("Adding module to test engine"));
        ret = iTestEngine.AddTestModule(moduleName, ini);
        if(ret != KErrNone && ret != KErrAlreadyExists)
            {
            LogErrorAndLeaveL(_L("CATSInterface::ConstructL"), _L("iTestEngine.AddTestModule"), ret);
            return;
            }

        //Add test case files
        TInt cfgCnt = moduleInfo->CountCfgFiles();
        TInt j;
        TFileName cfgFile;
        for(j = 0; j < cfgCnt; j++)
            {
            moduleInfo->GetCfgFileName(j, cfgFile);
            if(cfgFile.Length() > 0)
                {
                RDebug::Print(_L("config file: '%S'"), &cfgFile);

                ret = iTestEngine.AddConfigFile(moduleName, cfgFile);
                if(ret != KErrNone && ret != KErrAlreadyExists)
                    {
                    // Log error
                    LogErrorAndLeaveL(_L("CATSInterface::ConstructL"), _L("RTestEngine::AddConfigFile"), ret);
                    return;
                    }
                }
            else
                {
                RDebug::Print(_L("Got empty cfg file"));
                }
            }
        if(cfgCnt == 0)
            {
            RDebug::Print(_L("cfg file not found"));
            }

        RDebug::Print(_L("Module '%S' processed correctly"), &moduleName);
        }

    RDebug::Print(_L("End creating test modules"));

    // Create console screen
    iConsole = Console::NewL(
        iTestModule, TSize( KConsFullScreen, KConsFullScreen ) );
    }


/*
-------------------------------------------------------------------------------

    Class: CATSInterface

    Method: NewL

    Description: Two-phased constructor.

    Parameters: None

    Return Values: CATSInterface* : pointer to created CATSInterface object

    Errors/Exceptions: Leaves if memory allocation for CATSInterface fails
                       Leaves if ConstructL leaves

    Status: Approved
    
-------------------------------------------------------------------------------
*/
CATSInterface* CATSInterface::NewL()
    {
    // Create CATSInterface and return it
    CATSInterface* self =  new ( ELeave ) CATSInterface();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


/*
-------------------------------------------------------------------------------

    Class: CATSInterface

    Method: ~CATSInterface

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CATSInterface::~CATSInterface()
    {
    // Close Test Engine
    iTestEngine.Close();
    iTestEngineServ.Close();

    delete iModuleList;
    delete iConsole;

    }

/*
-------------------------------------------------------------------------------

    Class: CATSInterface

    Method: ParseTestModulesL

    Description: Parse and search for module info and fill list of modules.

    Parameters: CStifParser*     aParser:       in: CStifParser object
                CTestModuleList* aModuleList:   in: list of modules
                TPtrC&           aSectionStart: in: descriptor with start of section string
                TPTrC&           aSectionEnd:   in: descriptor with end of section string

    Return Values: None

    Errors/Exceptions: Leaves if some of called leaving methods leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CATSInterface::ParseTestModulesL(CStifParser* aParser, CTestModuleList* aModuleList, const TDesC& aSectionStart, const TDesC& aSectionEnd)
    {
    //First let's find all modules given in Stif's ini file and store that info in CTestModuleList object
    CStifSectionParser* sectionParser = NULL;
    CStifItemParser* item = NULL;

    sectionParser = aParser->SectionL(aSectionStart, aSectionEnd);

    while(sectionParser)
        {
        RDebug::Print(_L("Found '%S' and '%S' sections"), &aSectionStart, &aSectionEnd);
        CleanupStack::PushL(sectionParser);
        RDebug::Print(_L("Starting to read module information"));

        // Get name of module
        _LIT(KModuleName, "ModuleName=");
        item = sectionParser->GetItemLineL(KModuleName);
        CleanupStack::PushL(item);
        if(!item)
            {
            CleanupStack::PopAndDestroy(item);
            LogErrorAndLeaveL(_L("CATSInterface::ParseTestModulesL"), _L("CStifItemParser::GetItemLineL - line not found from module section"), KErrNotFound);
            return;
            }
        else
            {
            RDebug::Print(_L("'%S' found"), &KModuleName);
            }

        TPtrC name;
        TName moduleName;
        TInt ret(KErrNone);
        ret = item->GetString(KModuleName, name);
        if(ret != KErrNone)
            {
            CleanupStack::PopAndDestroy(item);
            LogErrorAndLeaveL(_L("CATSInterface::ParseTestModulesL"), _L("CStifItemParser::GetString - Module name parsing left with error"), ret);
            return;
            }
        else
            {
            RDebug::Print(_L("Module '%S' found from ini-file"), &name);
            moduleName.Copy(name);
            moduleName.LowerCase();
            ret = aModuleList->AddTestModule(moduleName);
            if(ret != KErrNone && ret != KErrAlreadyExists)
                {
                LogErrorAndLeaveL(_L("CATSInterface::ParseTestModulesL"), _L("CTestModuleList::AddTestModule - Could not add module to list of modules"), ret);
                return;
                }
            }
        CleanupStack::PopAndDestroy(item);

        //Get pointer to added module
        CTestModuleInfo* moduleInfo = aModuleList->GetModule(moduleName);
        if(!moduleInfo)
            {
                LogErrorAndLeaveL(_L("CATSInterface::ParseTestModulesL"), _L("CTestModuleList::GetModule - Could not add get module info from list"), KErrNotFound);
                return;
            }

        // Get ini file, if it exists
        RDebug::Print(_L("Start parsing ini file"));
        _LIT(KIniFile, "IniFile=");
        item = sectionParser->GetItemLineL(KIniFile);
        if(item)
            {
            RDebug::Print(_L("'%S' found"), &KIniFile);
            CleanupStack::PushL(item);
            TPtrC iniFile;
            ret = item->GetString(KIniFile, iniFile);
            if(ret == KErrNone)
                {
                RDebug::Print(_L("Initialization file '%S' found, file can be empty"), &iniFile);
                TFileName filename;
                filename.Copy(iniFile);
                filename.LowerCase();
                TStifUtil::CorrectFilePathL( filename );
                moduleInfo->SetIniFile(filename);
                }
            else
                {
                RDebug::Print(_L("Initialization file not found"));
                }
            CleanupStack::PopAndDestroy(item);
            }
        else
            {
            RDebug::Print(_L("'%S' not found"), &KIniFile);
            }

        // Get config (testcase) file
        RDebug::Print(_L("Start parsing cfg files"));
        TPtrC cfgTag;
        for(TInt i = 0; i < 2; i++)
            {
            //Set tag for config files
            if(i == 0)
                {
                cfgTag.Set(_L("ConfigFile="));
                }
                else
                {
                cfgTag.Set(_L("TestCaseFile="));
                }
            //Read data
            item = sectionParser->GetItemLineL(cfgTag);
            while(item)
                {
                CleanupStack::PushL(item);
                RDebug::Print(_L("Item '%S' found"), &cfgTag);
                TPtrC cfgFile;
                ret = item->GetString(cfgTag, cfgFile);
                if(ret == KErrNone)
                    {
                    TFileName ifile;
                    ifile.Copy(cfgFile);
                    ifile.LowerCase();
                    TStifUtil::CorrectFilePathL( ifile );
                    RDebug::Print(_L("Configuration file '%S' found"), &ifile);
                    moduleInfo->AddCfgFile(ifile);
                    }
                else
                    {
                    RDebug::Print(_L("Configuration file not found"));
                    }
                CleanupStack::PopAndDestroy(item);
                item = sectionParser->GetNextItemLineL(cfgTag);
                }
            }

        RDebug::Print(_L("Module '%S' information read correctly"), &moduleName);

        // Get next section
        CleanupStack::PopAndDestroy(sectionParser);
        sectionParser = aParser->NextSectionL(aSectionStart, aSectionEnd);
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CATSInterface

    Method: ParseCommandLineL

    Description: Parse command line parameters

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if module name not found from command line

    Status: Approved
    
-------------------------------------------------------------------------------
*/
void CATSInterface::ParseCommandLineL()
    {
    // Command line params
    _LIT( KTestModule, "-testmodule" );
    _LIT( KConfigFile, "-config" );
    _LIT( KEngineIniFile, "-engineini" );
    _LIT( KModuleIniFile, "-moduleini" );

	const TInt length = User::CommandLineLength();

    HBufC* cmdLine = HBufC::NewLC( length );
    TPtr ptr = cmdLine->Des();

	User::CommandLine( ptr );

    TBool moduleFound( EFalse );
    TLex lex( ptr );
    // Parse the command line
    while ( !lex.Eos() )
        {
        TPtrC tmpPtr = lex.NextToken();
        // Check the test module name
        if ( tmpPtr == KTestModule )
            {
            TPtrC module = lex.NextToken();
            if ( module.Ptr() )
                {
                iTestModule.Copy( module );
                moduleFound = ETrue;
                }
            }
        // Check the module's config file
        else if ( tmpPtr == KConfigFile )
            {
            TPtrC config = lex.NextToken();
            if ( config.Ptr() )
                {
                iConfigFile.Copy( config );
                TStifUtil::CorrectFilePathL( iConfigFile );
                }
            }
        // Check the engine's ini file
        else if ( tmpPtr == KEngineIniFile )
            {
            TPtrC iniFile = lex.NextToken();
            if ( iniFile.Ptr() )
                {
                iEngineIniFile.Copy( iniFile );
                TStifUtil::CorrectFilePathL( iEngineIniFile );
                }
            }
        // Check the module's ini file
        else if ( tmpPtr == KModuleIniFile )
            {
            TPtrC iniFile = lex.NextToken();
            if ( iniFile.Ptr() )
                {
                iModuleIniFile.Copy( iniFile );
                TStifUtil::CorrectFilePathL( iModuleIniFile );
                }
            }
        else
            {
            // Skip unknown commands
            }
        } // while

    // Destroy command line buffer
    CleanupStack::PopAndDestroy( cmdLine );

    // Module name has to exists
    if ( !moduleFound )
        {
        User::Leave( KErrArgument );
        }
    }


/*
-------------------------------------------------------------------------------

    Class: CATSInterface

    Method: RunTestsL

    Description: Starts testing

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if RunAllTestCasesL leaves
                       Leaves if RemoveTestModule returns error

    Status: Approved
    
-------------------------------------------------------------------------------
*/
void CATSInterface::RunTestsL()
    {
    // Run all test cases
    RunAllTestCasesL();

    /*
    // Remove test module
    User::LeaveIfError( iTestEngine.RemoveTestModule( iTestModule ) );
    */
    RDebug::Print(_L("Start removing test modules"));
    CTestModuleInfo* moduleInfo = NULL;
    TInt i;
    TInt modCnt = iModuleList->Count();

    for(i = 0; i < modCnt; i++)
        {
        RDebug::Print(_L("Processing module"));
        // Get module
        moduleInfo = iModuleList->GetModule(i);
        if(!moduleInfo)
            {
            RDebug::Print(_L("Could not get module info at index %d"), i);
            continue;
            }

        // Get module name
        TName moduleName;
        moduleInfo->GetModuleName(moduleName);
        RDebug::Print(_L("module name: '%S'"), &moduleName);

        // Remove test module
        User::LeaveIfError(iTestEngine.RemoveTestModule(moduleName));
        RDebug::Print(_L("Module '%S' removed"), &moduleName);
        }

    RDebug::Print(_L("End removing test modules"));
    }

/*
-------------------------------------------------------------------------------

    Class: CATSInterface

    Method: RunAllTestCasesL

    Description: Run all test cases from test module.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if some of called leaving methods leaves

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CATSInterface::RunAllTestCasesL()
    {
    TInt ret( KErrNone );

    /*
    // Add given config file to test module
    if ( iConfigFile.Length() > 0 )
        {
        ret = iTestEngine.AddConfigFile( iTestModule, iConfigFile );
        if ( ret != KErrNone && ret != KErrAlreadyExists )
            {
            // Log error
            LogErrorAndLeaveL( _L("CATSInterface::RunAllTestCasesL"), _L("iTestEngine.AddConfigFile"), ret );
            return;
            }
        }
    */

    // Enumerate test cases
    TCaseCount caseCount;
    TRequestStatus status;
    iTestEngine.EnumerateTestCases( caseCount, status );
    User::WaitForRequest( status );

    // Check that enumerate succeeded
    if ( status != KErrNone )
        {
        // Log error 
        LogErrorAndLeaveL( _L("CATSInterface::RunAllTestCasesL"), _L("iTestEngine.EnumerateTestCases"), status.Int() );
        return;
        }

    // Get test cases to buffer
    CFixedFlatArray<TTestInfo>* testCases = 
        CFixedFlatArray<TTestInfo>::NewL( caseCount() );
    CleanupStack::PushL( testCases );

    ret = iTestEngine.GetTestCases( *testCases );
    if ( ret != KErrNone )
        {
        // Log error 
        LogErrorAndLeaveL( _L("CATSInterface::RunAllTestCasesL"), _L("iTestEngine.GetTestCases"), status.Int() );
        return;
        }

    //variables used to get version of STIF
    TInt majorV;
    TInt minorV;
    TInt buildV;
    TBuf<30> relDate;
    TStifUtil::STIFVersion(majorV, minorV, buildV, relDate);
    
    TBuf<50> version;
    version.Format(_L("STIF v%d.%d.%d - "), majorV, minorV, buildV);
    version.Append(relDate);
    version.Append(_L("\n"));
    
    iConsole->Printf(version);	//printing STIF version information (version and release date)
    iConsole->Printf( _L("Test case count: [%d]\n\n"), testCases->Count() );

    // Loop through all test cases in buffer and run them
    const TInt count = testCases->Count();
    for ( TInt i = 0; i < count; i++ )
        {
#ifdef _DEBUG
        RDebug::Print( ( *testCases)[i].iTestCaseInfo.iTitle );
#endif
        iConsole->Printf( _L("Now running test case: [%d] [%S] "), i+1,
            &( *testCases )[i].iTestCaseInfo.iTitle );

        // Run test case
        RunTestCaseL( ( *testCases )[i] );
        }

    // End test set
    CleanupStack::PopAndDestroy( testCases );

    }


/*
-------------------------------------------------------------------------------

    Class: CATSInterface

    Method: RunTestCaseL

    Description: Run test case

    Parameters: TTestInfo& aTestInfo: in: TTestInfo: Test info

    Return Values: None

    Errors/Exceptions: Leaves if some of called leaving methods leaves

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CATSInterface::RunTestCaseL( TTestInfo& aTestInfo )
    {
    TInt testResult( KErrNone );
    CATSInterfaceRunner* runner;

    // Trap to catch errors from test case executing
    TRAPD( trapError,
        runner = CATSInterfaceRunner::NewL( this, aTestInfo );
        CleanupStack::PushL( runner );

        testResult = RunATestCaseL( runner );

        CleanupStack::PopAndDestroy( runner );
        );

    if ( trapError != KErrNone )
        {
        testResult = trapError;
        }

    if ( testResult != KErrNone ) // Test case is FAILED
        {
        // Test case failed, print out the error
        iConsole->Printf( _L("\nTest case FAILED! err=[%d]\n"), testResult );
        }

    else // Test case is PASSED
        {
        iConsole->Printf( _L("\nTest case PASSED!\n") );
        testResult = KErrNone;
        }
    }


/*
-------------------------------------------------------------------------------

    Class: CATSInterface

    Method: RunATestCaseL

    Description: Run a test case

    Parameters: CATSInterfaceRunner* aTestCase: in: Pointer to test case runner

    Return Values: TInt KErrNone: Test case passed
                   other error code: Test case failed or cannot be executed

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CATSInterface::RunATestCaseL( CATSInterfaceRunner* aTestCase )
    {
    iTestCompletedError = KErrNone;

    // Create timer
    CActiveTimer* timer = CActiveTimer::NewL( iConsole );
    CleanupStack::PushL( timer );

    // Start test case and timer
    aTestCase->StartTestL();
    timer->StartL();

    // Wait for test case completed
    CActiveScheduler::Start();

    timer->Cancel();
    CleanupStack::PopAndDestroy( timer );

    // test completion error is set in TestCompleted method
    return iTestCompletedError;
    }


/*
-------------------------------------------------------------------------------

    Class: CATSInterface

    Method: TestEngineServer

    Description: Return handle to Test Engine Server.

    Parameters: None
    
    Return Values: RTestEngineServer&: Reference to RTestEngineServer handle

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
RTestEngineServer& CATSInterface::TestEngineServer()
    {
    return iTestEngineServ;

    }


/*
-------------------------------------------------------------------------------

    Class: CATSInterface

    Method: TestEngine

    Description: Return handle to Test Engine.

    Parameters: None
    
    Return Values: RTestEngine&: reference to RTestEngine handle

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
RTestEngine& CATSInterface::TestEngine()
    {
    return iTestEngine;

    }


/*
-------------------------------------------------------------------------------

    Class: CATSInterface

    Method: TestCompleted

    Description: Test case completed

    This method is called when test case is completed or error occurred
    during the test.

    Parameters: TInt aError: in: Symbian OS error: Test result
    
    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
void CATSInterface::TestCompleted( TInt aError )
    {
    // Store completion error
    iTestCompletedError = aError;

    // Stop the scheduler
    CActiveScheduler::Stop();

    }
    
/*
-------------------------------------------------------------------------------

    Class: CATSInterface

    Method: LogErrorAndLeaveL

    Description: Write error Logger and leave.

    This function is called if some function returns error and the error cannot
    be logged another way Logger, e.g. RTestEngineServer and
    RTestEngine methods.

    Parameters: const TDesC& aFunction: in: any string: Function where the error
                 occurred
                const TDesC& aDescription: in: any string: Description for error
                const TInt aError: in: Symbian OS error: Test result
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CATSInterface::LogErrorAndLeaveL( const TDesC& aFunction, 
                                      const TDesC& aDescription, 
                                      const TInt aError )
    {

    RDebug::Print( _L("%S: %S [%d]"), &aFunction, &aDescription, aError );
    User::Leave( aError );

    }

// ================= OTHER EXPORTED FUNCTIONS ================================= 

/*
-------------------------------------------------------------------------------
   
    Function: E32Main

    Description: Main function called by E32.

    Parameters: None

    Return Values: TInt: KErrNone :No errors occurred
                   TInt: Other Symbian OS Error :Error catch by TRAP

    Errors/Exceptions: TRAP is used to catch errors from leaving methods.

    Status: Approved

-------------------------------------------------------------------------------
*/
GLDEF_C TInt E32Main()
    {
 
    TInt processHandleCountBefore;
    TInt threadHandleCountBefore;
    RThread().HandleCount( processHandleCountBefore, threadHandleCountBefore );
    TInt reqsBefore = RThread().RequestCount();

    TInt processHandleCountAfter;
    TInt threadHandleCountAfter;
    TInt reqsAfter;

    __UHEAP_MARK;

    CTrapCleanup* cleanup = CTrapCleanup::New();
    if ( cleanup == NULL )
        {
        __UHEAP_MARKEND;
        return KErrNoMemory;
        }

    CActiveScheduler* activeScheduler = new CActiveScheduler;
    if ( activeScheduler == NULL )
        {
        delete cleanup;
        __UHEAP_MARKEND;
        return KErrNoMemory;
        }
    CActiveScheduler::Install( activeScheduler );

    // Construct the test client
    CATSInterface* test = NULL;
    TRAPD( err, test = CATSInterface::NewL() );
    if ( err != KErrNone )
        {
#ifdef _DEBUG
        RDebug::Print(_L("ATSInterface construction failed %d: "), err );
#endif
        delete cleanup;
        delete activeScheduler;
        __UHEAP_MARKEND;
        return err;
        }

    // Run tests
    TRAP( err, test->RunTestsL() );
    if ( err != KErrNone )
        {
#ifdef _DEBUG
        RDebug::Print(_L("RunTestsL left with %d: "), err );
#endif
        }


    // Deallocate resources
    delete test;
    delete activeScheduler;
    delete cleanup;

    reqsAfter = RThread().RequestCount();
    RThread().HandleCount( processHandleCountAfter, threadHandleCountAfter );

    if ( reqsAfter != reqsBefore )
        {
#ifdef _DEBUG
        RDebug::Print(_L("Request count not matching! %d vs. %d: "),
            reqsBefore, reqsAfter );
#endif
        }
    if ( threadHandleCountAfter != threadHandleCountBefore )
        {
#ifdef _DEBUG
        RDebug::Print(_L("Handle count not matching! %d vs. %d: "),
            threadHandleCountBefore, threadHandleCountAfter );
#endif
        }

    __UHEAP_MARKEND;

    return err;
    }

// End of File
