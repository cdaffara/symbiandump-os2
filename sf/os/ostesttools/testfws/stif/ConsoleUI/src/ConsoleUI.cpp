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
* Description: This module contains implementation of CConsoleMain 
* and CModule class member functions.
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32cons.h>
#include <e32svr.h>
#include <f32file.h>

#include <stifinternal/UIStoreIf.h>

#include "ConsoleUI.h"
#include "ConsoleMenus.h"

#include "StifTestInterface.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
_LIT(KNameTxt,"STIF");
_LIT( KConsoleMain, "CConsoleMain" );

// Commandline params
_LIT( KTestModule,      "-testmodule" );
_LIT( KTestModuleIni,   "-testmoduleini" );
_LIT( KTestCaseFile,    "-testcasefile" );
_LIT( KTestSet,         "-testset" );
_LIT( KTestRun,         "-run" );
_LIT( KTestRunAll,      "all" );
_LIT( KTestRunSeq,      "sequential" );
_LIT( KTestRunPar,      "parallel" );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
LOCAL_C void MainL();



// ==================== LOCAL FUNCTIONS =======================================

// None


// ================= MEMBER FUNCTIONS =========================================



/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: NewL

    Description: Construct the console main class

    Parameters: None

    Return Values: CConsoleMain*                    New object

    Errors/Exceptions: Leaves if memory allocation fails or
                       ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CConsoleMain* CConsoleMain::NewL( )
    {

    CConsoleMain* self = new ( ELeave ) CConsoleMain();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;

    }


/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: ConstructL

    Description: Second level constructor.

    Construct the console
    Construct module and case containers
    Retrieve command line parameters
    Connect to test engine

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if memory allocation fails or fileserver or
                       test engine can't be connected.

    Status: Draft

-------------------------------------------------------------------------------
*/
void CConsoleMain::ConstructL( )
    {
    
    CUIStoreIf::ConstructL();

    // Construct the console
    iConsole = Console::NewL( KNameTxt,
                             TSize( KConsFullScreen, KConsFullScreen ) );

    RDebug::Print(_L("Creating module list object"));
    TRAPD(err, iModuleList = CTestModuleList::NewL(NULL));
    if(err != KErrNone)
        {
        RDebug::Print(_L("Could not create module list (%d)"), err);
        UiError(_L("Could not create module list (err)"));
        User::Leave(err);
        }
    if(!iModuleList)
        {
        RDebug::Print(_L("Could not create module list (NULL)"));
        UiError(_L("Could not create module list (NULL)"));
        User::Leave(KErrGeneral);
        }

    // Get command line parameters
    TFileName iniFile; // TestFramework initialization file
                    
    ProcessCommandlineL( iniFile );

    if(iTestModule)
        {
        // Add to module list info about module taken from command line
        RDebug::Print(_L("Adding command line module to list"));
        TName moduleName;
        moduleName.Copy(iTestModule->Des());
        moduleName.LowerCase();
        err = iModuleList->AddTestModule(moduleName);
        if(err != KErrNone && err != KErrAlreadyExists)
            {
            RDebug::Print(_L("Could not add module to list of modules (%d)"), err);
            UiError(_L("Could not add module to list of modules (err)"));
            User::Leave(err);
            }

        //Get added module
        CTestModuleInfo* moduleInfo = iModuleList->GetModule(moduleName);
        if(!moduleInfo)
            {
            RDebug::Print(_L("Could not add get module info from list"));
            UiError(_L("Could not add get module info from list"));
            User::Leave(KErrGeneral);
            }

        //Add ini file if given
        if(iTestModuleIni && iTestModuleIni->Length() > 0)
            {
            TFileName filename;
            filename.Copy(iTestModuleIni->Des());
            filename.LowerCase();
            moduleInfo->SetIniFile(filename);
            }

        //Add config file if given
        if(iTestCaseFile && iTestCaseFile->Length() > 0)
            {
            TFileName filename;
            filename.Copy(iTestCaseFile->Des());
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
                RDebug::Print(_L("Could not create parser to read content of config file (%d)"), err);
                UiError(_L("Could not create parser to read content of config file (err)"));
                User::Leave(err);
                }
            CleanupStack::PushL(parser);

            ParseTestModulesL(parser, iModuleList, KIncludeModuleStart, KIncludeModuleEnd);

            CleanupStack::PopAndDestroy(parser);
            finfo = iModuleList->GetUncheckedCfgFile();
            }
        RDebug::Print(_L("End parsing included modules"));
        }

    iConsole->Printf(_L("\nTest Framework starting\n"));

    // CUIStore open
    User::LeaveIfError( UIStore().Open( iniFile ) );
    
    }
        
/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: CConsoleMain

    Description: Constructor.
    Initialize non-zero member variables.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CConsoleMain::CConsoleMain( ):iStartCases( EStartCaseNo )
    {
    }


/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: ~CConsoleMain

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CConsoleMain::~CConsoleMain( )
    {	

    iDialogs.ResetAndDestroy();
    iDialogs.Close();
 
    // CUIStore close
    UIStore().Close();
    
    delete iReader;
    iReader = NULL;
    
	delete iScroller;
	iScroller = NULL;
    
    delete iConsole;
    iConsole = NULL;

    delete iMainMenu;
    iMainMenu = NULL;

    delete iTestModule;
    iTestModule = 0;
    
    delete iTestModuleIni;
    iTestModuleIni = 0;
    
    delete iTestCaseFile;
    iTestCaseFile = 0;
    
    delete iTestSetName;
    iTestSetName = 0;

    delete iModuleList;
    iModuleList = 0;
    
    iFilters.ResetAndDestroy();
    iFilters.Close();
    }

/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: ProcessCommandlineL

    Description: Process commandline parameters.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves on error.

    Status: Draft

-------------------------------------------------------------------------------
*/
void CConsoleMain::ProcessCommandlineL( TFileName& aIniFile )
    {

	User me;
    TInt lineLength = me.CommandLineLength();
    HBufC* lineBuf = HBufC::NewLC( lineLength );
    TPtr line( lineBuf->Des() );
    TBool firstTime = ETrue;
    TInt offset = 0;
    TPtrC param;
    TChar c;
    
    me.CommandLine( line );
    TLex lex( line );
    
    while( offset < lineLength )
        {    
        // Get first charecter of the parameter
        lex.SkipSpace();
        c = lex.Peek();
        if( c == 0 )
            {
            // End of line
            break;
            }
        
        // Get the whole parameter 
        param.Set( lex.NextToken() );
        if( firstTime )
            {
            // Filebrowser workaround. It gives the exe name as command line param
            // verify that it is not the same as the executable name
            //@spe TFileName exePath = me.FileName();
			TFileName exePath = RProcess().FileName();
            if ( param == exePath )
                {
                // Discard command line parameters when using filebrowser
                break;
                }

            firstTime = EFalse;
            }
        // Check if -param option
        if( c == '-' )
            {
            // This is option
            if( param == KTestModule )
                {
                // Get test module name
                param.Set( lex.NextToken() );
                if( param.Length() == 0 )
                    {
                    UiError( _L("Test module name not given"));
                    break;
                    }
                if( param.Length() > KMaxName )
                    {
                    UiError( _L("Test module name too long"));
                    break;
                    }
                iTestModule = param.AllocL();
                }
            else if( param == KTestModuleIni )
                {
                // Get test module initialization file
                param.Set( lex.NextToken() );
                if( param.Length() == 0 )
                    {
                    UiError( _L("Test module initialization file name not given"));
                    break;
                    }
                if( param.Length() > KMaxFileName )
                    {
                    UiError( _L("Test module initialization file name too long"));
                    break;
                    }
                TFileName tmpTestModuleIni( param );
                TStifUtil::CorrectFilePathL( tmpTestModuleIni );
                iTestModuleIni = tmpTestModuleIni.AllocL();
                }
            else if( param == KTestCaseFile )
                {
                // Get test case file name
                param.Set( lex.NextToken() );
                if( param.Length() == 0 )
                    {
                    UiError( _L("Test case file name not given"));
                    break;
                    }
                if( param.Length() > KMaxFileName )
                    {
                    UiError( _L("Test case file name too long"));
                    break;
                    }
                TFileName tmpTestCaseFile( param );
                TStifUtil::CorrectFilePathL( tmpTestCaseFile );
                iTestCaseFile = tmpTestCaseFile.AllocL();
                }
            else if( param == KTestSet )
                {
                // Get test set name
                param.Set( lex.NextToken() );
                if( param.Length() == 0 )
                    {
                    UiError( _L("Test set name not given"));
                    break;
                    }
                if( param.Length() > KMaxFileName )
                    {
                    UiError( _L("Test set name too long"));
                    
                    break;
                    }
                iTestSetName = param.AllocL();
                }
            else if( param == KTestRun )
                {
                if( iStartCases )
                    {
                    UiError( _L("Only one -run option allowed") );
                    // Do not start anything
                    iStartCases = EStartCaseNo;
                    break;
                    }
                // Get run type
                lex.Mark();
                param.Set( lex.NextToken() );
                if( param.Length() == 0 )
                    {
                    UiError( _L("Test set name not given"));
                    break;
                    }
                if( ( param == KTestRunAll ) ||
                    ( param == KTestRunSeq ) )
                    {
                    iStartCases = EStartCasesSeq;
                    }
                else if( param == KTestRunPar )
                    {
                    iStartCases = EStartCasesPar;
                    }
                else
                    {
                    lex.UnGetToMark();
                    lex.SkipSpace();
                    if( lex.Val( iTestCaseNum ) != KErrNone )
                        { 
                        UiError( _L("Unknown run type given, valid values are all/sequential/parallel/test case number"));
                        }
                    else
                        {
                        iStartCases = EStartSingleCase;
                        }    
                    break;
                    }
                }            
            }
        else 
            {
            // This is Test Framework ini file
            aIniFile = param;
            TStifUtil::CorrectFilePathL( aIniFile );
            // ini file is last option
            break;
            }
        
        }
      
    CleanupStack::PopAndDestroy( lineBuf );

    }
    
    
/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: Panic

    Description: Console UI panic function

    Parameters: TInt

    Return Values: 

    Errors/Exceptions: Does not return.

    Status: Draft

-------------------------------------------------------------------------------
*/
void CConsoleMain::Panic(TConsoleUIPanic aPanic )
    {
        
    User::Panic (_L("STIF TestFramework Console UI panic"), aPanic );
    
    }


/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: StartL

    Description: Construct menu objects and start the menu handling

    Parameters: None

    Return Values: None

    Errors/Exceptions:

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CConsoleMain::StartL()
    {
    
    // Construct keystroke reader
    iReader           = CConsoleReader::NewL( this, iConsole );

    // Construct the main menu
    TInt majorV;
    TInt minorV;
    TInt buildV;
    TBuf<30> relDate;
    TStifUtil::STIFVersion(majorV, minorV, buildV, relDate);
    
    TBuf<128> version;
    version.Format(_L("STIF v%d.%d.%d - "), majorV, minorV, buildV);
    version.Append(relDate);
    version.Append(_L("\n"));
    iMainMenu = CMainMenu::NewL( this, NULL, _L("Main menu"), version );

    // Add the case menu
    CMenu* caseMenu   = CMenu::NewL( this, iMainMenu, _L("Case menu") );
    iMainMenu->AddItemL ( caseMenu );

    // Add the module menu
    CMenu* moduleMenu = CModuleListView::NewL( this, iMainMenu, _L("Module menu") );
    iMainMenu->AddItemL ( moduleMenu );

    // Add test set menu
    CTestSetMenu* testSetMenu = CTestSetMenu::NewL( this, iMainMenu, _L("Test set menu") );
    iMainMenu->AddItemL ( testSetMenu );
 
    // Construct the case menus
    CMenu* casestart  = NULL;
    CMenu* casestartshow  = NULL;
    
    // Load filters
    UIStore().ReadFiltersL(iFilters);
    
    // If there are no filters defined, then construct menu without filter's menu
    if(iFilters.Count() == 0)
        {
        casestart = CCaseStartMenu::NewL(this, caseMenu, _L("Start new case"));
        caseMenu->AddItemL(casestart);
    CMenu* casestartshow  = NULL;
        casestartshow = CCaseStartMenu::NewL(this, caseMenu, _L("Start & show output"), ETrue);
        caseMenu->AddItemL(casestartshow);
        }
    else // if there are filters defined, create also filter's menu
        {
        CFilterMenu* filtercasestart = NULL;
        CFilterMenu* filtercasestartshow = NULL;
        
        filtercasestart = CFilterMenu::NewL(this, caseMenu, _L("Start new case (filter)"));
        caseMenu->AddItemL(filtercasestart);
        casestart = CCaseStartMenu::NewL(this, caseMenu, _L("Start new case"));
        filtercasestart->SetTestCaseMenu(casestart);
        
        filtercasestartshow = CFilterMenu::NewL(this, caseMenu, _L("Start & show output (filter)"));
        caseMenu->AddItemL(filtercasestartshow);
        casestartshow = CCaseStartMenu::NewL(this, caseMenu, _L("Start & show output"), ETrue);
        filtercasestartshow->SetTestCaseMenu(casestartshow);
        }

	CMultipleCaseMenu* multicasestart =
	    CMultipleCaseMenu::NewL (this, caseMenu, _L("Run multiple tests") );
    caseMenu->AddItemL( multicasestart );
    casestart = CCaseMenu::NewL ( this, caseMenu, _L("Ongoing cases"), 
        CUIStoreIf::EStatusRunning );
    caseMenu->AddItemL( casestart );
    casestart = CCaseMenu::NewL ( this, caseMenu, _L("Executed cases"), 
        CUIStoreIf::EStatusExecuted );
    caseMenu->AddItemL( casestart );
    casestart = CCaseMenu::NewL ( this, caseMenu, _L("Passed cases"), 
        CUIStoreIf::EStatusPassed );
    caseMenu->AddItemL( casestart );
    casestart = CCaseMenu::NewL ( this, caseMenu, _L("Failed cases"), 
        CUIStoreIf::EStatusFailed );
    caseMenu->AddItemL( casestart );
    casestart = CCaseMenu::NewL ( this, caseMenu, _L("Aborted/Crashed cases"), 
        ( CUIStoreIf::EStatusAborted | CUIStoreIf::EStatusCrashed) );
    caseMenu->AddItemL( casestart );
    //Add "test set choice" menu
    CTestSetChoiceMenu* testChoiceMenu = CTestSetChoiceMenu::NewL( this, testSetMenu, _L("Load test set") );
 	testSetMenu->AddItemL( testChoiceMenu );

    // Start stuff according to command line parameters
    if( iTestSetName )
        { 
        // Set test set name
        testSetMenu->SetTestSetFileName( iTestSetName->Des() );
        
        // Load test set
        if( UIStore().LoadTestSet( iTestSetName->Des() ) != KErrNone )
 	       {
           UiError( _L("Test set loading failed"));
           }
        else 
           {
           // Inform test set menu that set is loaded
           testSetMenu->SetCreated();
    
            // Check if test set should be started
    	    if( ( iStartCases == EStartCasesPar ) || 
    	    	( iStartCases == EStartCasesSeq ))         	
            	{
            
	            // Check set starting type
    	    	CStartedTestSet::TSetType setType = 
       	        CStartedTestSet::ESetSequential;
       	        
                if( iStartCases == EStartCasesPar )
                    {
                    setType = 
                        CStartedTestSet::ESetParallel;
                    }
                    
                // Start test set
                RRefArray<CTestSetInfo> setInfos;
                TInt ret = UIStore().TestSets( setInfos );
                if( (  ret != KErrNone ) ||
                    ( setInfos.Count() != 1 ) )
                    {
                    // Should never ever happen
                    User::Panic( KConsoleMain, KErrGeneral );
                    }
                
                const CTestSetInfo& set = setInfos[0];
                setInfos.Reset();
                setInfos.Close();
                
                TInt index;
                ret = UIStore().StartTestSet( set, index, setType );
                if( ret != KErrNone )
                    {
                    UiError( _L("Test set starting failed") );
                    }
                } 
            }
        delete iTestSetName;
        iTestSetName = 0;     
        }
    if( iTestModule )
        {
        TPtrC module( iTestModule->Des() ); 

        // Add all test modules and config files
        RDebug::Print(_L("Start creating test modules"));
        CTestModuleInfo* moduleInfo = NULL;
        TInt i;
        TInt modCnt = iModuleList->Count();
        TInt ret;

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
            RDebug::Print(_L("Adding module to UI store"));
            ret = UIStore().AddTestModule(moduleName, ini);
            if(ret != KErrNone && ret != KErrAlreadyExists)
                {
                RDebug::Print(_L("Test module adding failed (%d)"), ret);
                UiError(_L("Test module adding failed"));
                continue;
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

                    ret = UIStore().AddTestCaseFile(moduleName, cfgFile);
                    if(ret != KErrNone)
                        {
                        // Log error
                        RDebug::Print(_L("Test case file adding failed (%d)"), ret);
                        UiError(_L("Test case file adding failed"));
                        continue;
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

        if( ( iStartCases == EStartCasesPar ) ||
            ( iStartCases == EStartCasesSeq ) )
            {
            // Start running
            CMultipleBaseMenu::TRunType runType = 
                CMultipleBaseMenu::ERunSequential;
            if( iStartCases == EStartCasesPar )
                {
                runType = CMultipleBaseMenu::ERunParallel;
                }
            TRAPD( err,
                //multicasestart->StartRunningL( runType, module, file );
                multicasestart->StartRunningL(runType, KNullDesC, KNullDesC);
                );
            if( err != KErrNone )
                {
                UiError( _L("Stating test cases failed") );
                }
            }
        else if( iStartCases == EStartSingleCase )
            {
            // Get test case
            RRefArray<CTestInfo> testCases;
            //ret = UIStore().TestCases( testCases, module, file );
            ret = UIStore().TestCases(testCases, KNullDesC, KNullDesC);
            if( ret == KErrNone )
                {
                if( ( iTestCaseNum < 0 ) ||
                    ( iTestCaseNum >= testCases.Count() ) )
                    {
                    ret = KErrNotFound;
                    }
                else
                    {         
                    TInt index;           
                    // Start single test case
                    ret = UIStore().StartTestCase( testCases[iTestCaseNum], index );
                    }                                       
                }
            testCases.Reset();       
            testCases.Close();
            if( ret != KErrNone )
                {
                UiError( _L("Starting test case failed"));        
                }
            }

        delete iTestCaseFile;
        iTestCaseFile = 0;
        delete iTestModuleIni;
        iTestModuleIni = 0;
        delete iTestModule;
        iTestModule = 0;
        }
        
    iStartCases = EStartCaseNo;
    
    // Load testcase if some stored 
    UIStore().LoadSavedTestCases();
           
	iScroller = CScrollerTimer::NewL ( this );
	iScroller->StartL();	

    // Print the main menu
    iCurrentMenu = iMainMenu;
    iCurrentMenu->PrintMenuL( CMenu::EMenuPrint );

    // Start to process keyboard events
    iReader->StartL();
    
    return KErrNone;

    }


/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: KeyPressed

    Description: Process keyboard events. Print new menu

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if SelectL leaves

    Status: Draft

-------------------------------------------------------------------------------
*/
void CConsoleMain::KeyPressed()
    {

    TBool cont = ETrue;

    // Read the key
    TKeyCode key = iConsole->KeyCode();
    CMenu* tmp = iCurrentMenu;

    // Let the menu handle the key press
    TRAPD( err, 
        iCurrentMenu = iCurrentMenu->SelectL( key, cont );
        );
    if( err != KErrNone )
        {
        User::InfoPrint( 
            _L("Processing keystroke failed") );  
        }
            
    if ( iCurrentMenu == NULL )
        {
        iCurrentMenu = tmp;
        }

   // If "not-exit" key pressed, continue
    if ( cont )
        {

        // Either only update old menu or new menu.
        if ( tmp == iCurrentMenu )
            {
            TRAP( err, iCurrentMenu->PrintMenuL( CMenu::EMenuRefresh ); );
            }
        else
            {
            TRAP( err, iCurrentMenu->PrintMenuL( CMenu::EMenuPrint ); )
            }
        
        if( err != KErrNone )
            {
            if ( err == KErrNoMemory )
            	{            	
            	ExitWithNoMemoryErrorMessage();                
            	}
            else
            	{
	            User::InfoPrint( 
	                _L("Printing menu failed") );
            	}
            }

        // Enable keystrokes
        iReader->StartL();

        }
    else
        {
        // "Exit", stop scheduler and exit
        CActiveScheduler::Stop();
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: ExitWithNoMemoryErrorMessage

    Description: Displays no memory error message and closes ConsoleUI
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CConsoleMain::ExitWithNoMemoryErrorMessage()
	{
    // "Exit", stop scheduler and exit
    CActiveScheduler::Stop();

    _LIT( KErrorTitle, "Error:\n" );
    _LIT( KErrorMessage, "Test cases execution have been stopped. ConsoleUI does not have enough memory to store information about executed test cases.  Results of executed test cases can be found in TestReport file. ConsoleUI will be closed now." );
    _LIT( KErrorPressAnyKey, "\n\nPress any key" );
    
    TSize size = GetConsole()->ScreenSize();
    size.iWidth = Min( size.iWidth - KMenuOverhead, 
                       KErrorMessage().Length() );
    size.iHeight = ( KErrorTitle().Length() + KErrorMessage().Length() + 
    				 KErrorPressAnyKey().Length() ) / size.iWidth + 3;
    
    CConsoleBase* console = NULL;
    TRAPD( err, console = Console::NewL( _L("Error note"), size ) );
    if ( err != KErrNone )
    	{
    	// We can't create console, exit without displaying error message
    	return;
    	}
    
    console->Printf( KErrorTitle );    
    console->Printf( KErrorMessage ); 
    console->Printf( KErrorPressAnyKey );
        
    console->Getch();
    
    delete console;    
	}

/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: UiError

    Description: Function is called when UI hit an error.
    
    Parameters: const TDesC& aInfo: in: information

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/

void CConsoleMain::UiError( const TDesC& aInfo )
    {
    
    // Error note deletes itself when it completes
    CMenuNotifier::NewL( aInfo, this );
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: Update

    Description: Receives output update notification from CUIStore.
    
    Parameters: CStartedTestCase* aTestCase: in: Test case information
                TInt aFlags: in: Update reason flags

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CConsoleMain::Update( CStartedTestCase* aTestCase, TInt aFlags )
    {
    
   	TInt err( KErrNone );
    
    // First check if some menu should be signaled 
    if( ( aFlags & CUIStoreIf::EStatusExecuted ) &&
        ( aTestCase->iBackPtr ) )
        {
        CMenu* menu = ( CMenu* )aTestCase->iBackPtr;
        TRAP( err, 
            menu->SignalL( aTestCase ); 
            );            
        }

    // Update console if needed
    if( iCurrentMenu && ( iCurrentMenu->Type() & aFlags ) )
        {
        if( aFlags & CUIStoreIf::EPrintUpdate )
            {
            CCaseExecutionView* view = 
                ( CCaseExecutionView* )iCurrentMenu;
            if( view->TestCase() == aTestCase )
                {
                TRAP( err, iCurrentMenu->PrintMenuL( CMenu::EMenuRefresh ) );
                }
            }
        else
            {
            TRAP( err, iCurrentMenu->PrintMenuL( CMenu::EMenuRefresh ); )
            }
        }
    
    	if ( err != KErrNone )
    		{
    		if ( err == KErrNoMemory )
    			{
    			ExitWithNoMemoryErrorMessage();
    			}
    		}
    
    }
                                                              


/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: Error

    Description: Function is called when test framework prints error.
    
    Parameters: TErrorNotification& aError: in: Error description

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/

void CConsoleMain::Error( TErrorNotification& aError )
    {
    
    // Error note deletes itself when it completes
    CMenuNotifier::NewL( aError.iText, this );
    
    }

/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: PopupMsg

    Description: Function is called when testframework 
        wants to print a popup window.
    
    Parameters: TDesC& aMsg: in: message
                

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/

TInt CConsoleMain::PopupMsg( const TDesC& aLine1, 
                             const TDesC& aLine2, 
                             TInt aTimeInSecs )
    {
    
    TInt ret = KErrNone;
    
    CMenuDialog* dialog = NULL;

    TRAP( ret, dialog = CMenuDialog::NewL( this, aLine1, aLine2, aTimeInSecs ); );

    if( ret == KErrNone )
        {
        iDialogs.Append( dialog ); 
        return (TInt)dialog;
        }

    return ret;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: PopupMsg

    Description: Function is called when testframework 
        wants to print a popup window.
    
    Parameters: TDesC& aMsg: in: message
                

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/

TInt CConsoleMain::PopupMsg( const TDesC& aLine1, 
                             const TDesC& aLine2, 
                             TInt aTimeInSecs,
                             TKeyCode& aKey,
                             TRequestStatus& aStatus )
    {
    
    TInt ret = PopupMsg( aLine1, aLine2, aTimeInSecs );
    if( ret < 0 )
        {
        return ret;
        }    
        
    CMenuDialog* dialog = (CMenuDialog*) ret; 
    ret = dialog->WaitForKeypress( aKey, aStatus );

    return (TInt) dialog;
    
    }
/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: Close

    Description: Close instance.
    
    Parameters: TInt aHandle: in: handle to open instance.                

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CConsoleMain::Close( TInt aHandle )
    {
    
    if( aHandle < 0 )
        {
        return;
        }
    CMenuDialog* dialog = (CMenuDialog*) aHandle;
    TInt index = iDialogs.Find( dialog );
    if( index >= 0 )
        {
        delete dialog;
        iDialogs.Remove( index );
        }
        
    }
           
/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: GetConsole

    Description: Returns the console

    Parameters: None

    Return Values: CConsoleBase*                    Console

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CConsoleBase* CConsoleMain::GetConsole()
    {

    return iConsole;

    }

/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: TimerUpdate

    Description: Updates current menu from timer

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CConsoleMain::TimerUpdate()
    {

	iCurrentMenu->TimerUpdate();

    }

/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

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
void CConsoleMain::ParseTestModulesL(CStifParser* aParser, CTestModuleList* aModuleList, const TDesC& aSectionStart, const TDesC& aSectionEnd)
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
            UiError(_L("Line not found from module section"));
            User::Leave(KErrGeneral);
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
            RDebug::Print(_L("Module name parsing left with error (%d)"), ret);
            UiError(_L("Module name parsing left with error (err)"));
            User::Leave(ret);
            }
        else
            {
            RDebug::Print(_L("Module '%S' found from ini-file"), &name);
            moduleName.Copy(name);
            moduleName.LowerCase();
            ret = aModuleList->AddTestModule(moduleName);
            if(ret != KErrNone && ret != KErrAlreadyExists)
                {
                RDebug::Print(_L("Could not add module to list of modules (%d)"), ret);
                UiError(_L("Could not add module to list of modules (err)"));
                User::Leave(ret);
                }
            }
        CleanupStack::PopAndDestroy(item);

        //Get pointer to added module
        CTestModuleInfo* moduleInfo = aModuleList->GetModule(moduleName);
        if(!moduleInfo)
            {
                RDebug::Print(_L("Could not add get module info from list"));
                UiError(_L("Could not add get module info from list"));
                User::Leave(KErrNotFound);
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
				TStifUtil::CorrectFilePathL(filename);
                filename.LowerCase();
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
                    RDebug::Print(_L("Configuration file '%S' found"), &cfgFile);
                    TFileName ifile;
                    ifile.Copy(cfgFile);
					TStifUtil::CorrectFilePathL(ifile);
                    ifile.LowerCase();
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

    Class: CConsoleMain

    Method: GetMainMenu

    Description: Return main menu of console.

    Parameters: None

    Return Values: CMainMenu*  pointer to main menu

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CMainMenu* CConsoleMain::GetMainMenu()
    {
    return dynamic_cast<CMainMenu*>(iMainMenu);
    }

/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: GetFilterArray

    Description: Get filter array for reading purposes

    Parameters: None

    Return Values: const RPointerArray<TDesC>&  array with filters

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
const RPointerArray<TDesC>& CConsoleMain::GetFilterArray(void) const
    {
    return iFilters;
    }

/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: SetFilterIndex

    Description: Set info about which index filter is used

    Parameters: TInt aFilterIndex: filter index selected by user

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CConsoleMain::SetFilterIndex(TInt aFilterIndex)
    {
    iChosenFilterIndex = aFilterIndex;
    }

/*
-------------------------------------------------------------------------------

    Class: CConsoleMain

    Method: GetFilterIndex

    Description: Get info about which index filter is used

    Parameters: None

    Return Values: TInt: filter index selected by user

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CConsoleMain::GetFilterIndex(void)
    {
    return iChosenFilterIndex;
    }

/*
-------------------------------------------------------------------------------

    Class: CScrollerTimer

    Method: NewL

    Description: Construct a new CScrollerTimer object

    Parameters: CConsoleMain*             :in:      Pointer to main console               

    Return Values: CScrollerTimer*                         New CScrollerTimer object

    Errors/Exceptions: Leaves if memory allocation or ConstructL leaves

    Status: Draft

-------------------------------------------------------------------------------
*/
CScrollerTimer* CScrollerTimer::NewL( CConsoleMain* aMain )
    {

    CScrollerTimer* self = new ( ELeave ) CScrollerTimer();
    CleanupStack::PushL( self );
    self->ConstructL( aMain );
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CScrollerTimer

    Method: ConstructL

    Description: Second level constructor

    Parameters: CConsoleMain*             :in:      Pointer to main console

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CScrollerTimer::ConstructL( CConsoleMain* aMain )
    {

    // Store module information
    iMain = aMain;
	iTimer.CreateLocal();

	CActiveScheduler::Add ( this );

    }

/*
-------------------------------------------------------------------------------

    Class: CScrollerTimer

    Method: CScrollerTimer

    Description: Constructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
CScrollerTimer::CScrollerTimer() : CActive (CActive::EPriorityStandard)
    {

    }


/*
-------------------------------------------------------------------------------

    Class: CScrollerTimer

    Method: ~CScrollerTimer

    Description: None

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
CScrollerTimer::~CScrollerTimer( )
    {

	Cancel();
	iTimer.Close();

    }

/*
-------------------------------------------------------------------------------

    Class: CScrollerTimer

    Method: StartL

    Description: None

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CScrollerTimer::StartL( )
    {

	SetActive();
	iTimer.After ( iStatus, KScrollPeriod );	

    }

/*
-------------------------------------------------------------------------------

    Class: CScrollerTimer

    Method: RunL

    Description: None

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CScrollerTimer::RunL( )
    {

	iMain->TimerUpdate();

	// Restart request
	SetActive();
	iTimer.After ( iStatus, KScrollPeriod );

    }

/*
-------------------------------------------------------------------------------

    Class: CScrollerTimer

    Method: DoCancel

    Description: None

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CScrollerTimer::DoCancel( )
    {

	iTimer.Cancel();

    }

/*
-------------------------------------------------------------------------------

    Class: CScrollerTimer

    Method: RunError

    Description: None

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt CScrollerTimer::RunError( TInt aError)
    {
    if ( aError == KErrNoMemory )
    	{
    	iMain->ExitWithNoMemoryErrorMessage();
    	}
  
	return aError;
    }    

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CConsoleReader class.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================



/*
-------------------------------------------------------------------------------

    Class: CConsoleReader

    Method: NewL

    Description: Construct the console main class

    Parameters: CConsoleMain* aMain: in: Pointer to main console
                CConsoleBase* aConsole: in: Console pointer

    Return Values: CConsoleReader*                    New object

    Errors/Exceptions: Leaves if memory allocation fails or
                       ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CConsoleReader* CConsoleReader::NewL( CConsoleMain* aMain, 
                                      CConsoleBase* aConsole )
    {

    CConsoleReader* self = 
        new ( ELeave ) CConsoleReader( aMain, aConsole );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;

    }


/*
-------------------------------------------------------------------------------

    Class: CConsoleReader

    Method: ConstructL

    Description: Second level constructor.
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: 

    Status: Draft

-------------------------------------------------------------------------------
*/
void CConsoleReader::ConstructL( )
    {
    }


/*
-------------------------------------------------------------------------------

    Class: CConsoleReader

    Method: CConsoleReader

    Description: Constructor.

    Parameters: CConsoleMain* aMain: in: Pointer to main console
                CConsoleBase* aConsole: in: Console pointer

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CConsoleReader::CConsoleReader( CConsoleMain* aMain, 
                                CConsoleBase* aConsole ): 
    CActive( EPriorityStandard )
    {
    
    iMain = aMain;
    iConsole = aConsole;
    
    CActiveScheduler::Add( this );

    }


/*
-------------------------------------------------------------------------------

    Class: CConsoleReader

    Method: ~CConsoleReader

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CConsoleReader::~CConsoleReader( )
    {	
    
    Cancel();    
    
    }

/*
-------------------------------------------------------------------------------

    Class: CConsoleReader

    Method: StartL

    Description: Construct menu objects and start the menu handling

    Parameters: None

    Return Values: None

    Errors/Exceptions:

    Status: Draft

-------------------------------------------------------------------------------
*/
void CConsoleReader::StartL( )
    {

    // Start to process keyboard events
    SetActive();
    iConsole->Read(iStatus);

    }
/*
-------------------------------------------------------------------------------

    Class: CConsoleReader

    Method: RunError

    Description: None

    Parameters: TInt aError

    Return Values: Error code

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/    
TInt CConsoleReader::RunError(TInt aError)
    {
    return aError;
    }


/*
-------------------------------------------------------------------------------

    Class: CConsoleReader

    Method: RunL

    Description: Process keyboard events. Print new menu

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if SelectL leaves

    Status: Draft

-------------------------------------------------------------------------------
*/
void CConsoleReader::RunL()
    {

    iMain->KeyPressed();
    
    }


/*
-------------------------------------------------------------------------------

    Class: CConsoleReader

    Method: DoCancel

    Description: Cancel request

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CConsoleReader::DoCancel()
    {
    
    iConsole->ReadCancel();
    
    }


// ================= OTHER EXPORTED FUNCTIONS =================================

/*
-------------------------------------------------------------------------------

    Class: -

    Method: MainL

    Description: The main function that can leave.
    Create the CMainConsole object and create, initialise and 
    start active scheduler.

    When active scheduler is stopped, clean up memory and exit.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if memory allocation or CConsoleMain construction
                       leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
LOCAL_C void MainL()
    {

    // Construct and install active scheduler
    CActiveScheduler* scheduler=new ( ELeave ) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    // Construct the console
    CConsoleMain* mainConsole = CConsoleMain::NewL();
    CleanupStack::PushL( mainConsole );

    // Start the console
    mainConsole->StartL();

    // Start handling requests
    CActiveScheduler::Start();
    // Execution continues from here after CActiveScheduler::Stop()

    // Clean-up
    CleanupStack::PopAndDestroy( mainConsole );
    CleanupStack::PopAndDestroy( scheduler );

    }


/*
-------------------------------------------------------------------------------

    Class: -

    Method: E32Main

    Description: The main function. Execution starts from here.
    Create clean-up stack and trap the MainL function which does the
    real work.

    Parameters: None

    Return Values: TInt                             Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
GLDEF_C TInt E32Main()
    {

    __UHEAP_MARK;

    // Get clean-up stack
    CTrapCleanup* cleanup=CTrapCleanup::New();

    // Call the main function
    TRAPD( error, MainL() );

    // Clean-up
    delete cleanup; 
    cleanup = NULL;

    __UHEAP_MARKEND;

    return error;

    }

#if defined(__WINS__)
/*
-------------------------------------------------------------------------------

    Class: -

    Method: WinsMain

    Description: The WinsMain function.

    Parameters: None

    Return Values: TInt                             Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt WinsMain()
	{
	E32Main();
	return KErrNone;
	}

#endif // __WINS__

// End of File
