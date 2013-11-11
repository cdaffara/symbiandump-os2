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
* Description: This file contains TestScripter implementation.
*
*/

// INCLUDE FILES
#include <StifTestEventInterface.h>
#include <StifLogger.h>
#include "TestScripter.h"
#include "TestKeywords.h"
#include "Logging.h"
#include "TestEngineClient.h"
#include "SettingServerClient.h"

#include <stifinternal/UiEnvProxy.h>

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
#ifdef LOGGER
#undef LOGGER
#endif
#define LOGGER iLog

// LOCAL CONSTANTS AND MACROS
// None
 
// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================

/*
-------------------------------------------------------------------------------

     Class: -

     Method: CallBack

     Description: (Function pointer) Called from CScriptBase class. Generic
                  method for call back operations from Test Script Class to
                  TestScripter.
  
     Parameters: CTestScripter* aTestScripter: in: Pointer to TestScripter
                 TStifTSCallBackType aCallType: in: Call back type
                 const TDesC& aLine: in Script line
                    
     Return Values: TInt: Symbian error code

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt CallBack( CTestScripter* aTestScripter,
                TStifTSCallBackType aCallType,
                const TDesC& aLine )
    {
    TInt ret( 0 );
    switch( aCallType )
        {
        case EStifTSCallClass:
            {
            ret = aTestScripter->CallTestClass( aLine );
            break;
            }
        case EStifTSGetObject:
            {
            ret = aTestScripter->GetTestScriptObject( aLine );
            break;
            }
        default:
            {
            ret = KErrArgument;
            break;
            }
        }

    return ret;

    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CTestScripter class 
    member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: CTestScripter

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.
     
     Parameters:    None

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CTestScripter::CTestScripter()
    {

    }

/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.

     Parameters:    None

     Return Values: None

     Errors/Exceptions: None.

     Status: Draft

-------------------------------------------------------------------------------
*/
void CTestScripter::ConstructL()
    {
    __TRACE( KPrint, ( _L("New TestScripter") ) );
    iStdLog = CStifLogger::NewL( KTestScripterLogDir,
                                KTestScripterLogFile );
    iLog = iStdLog;

    iOOMIgnoreFailure = EFalse; // OFF for default

    iCheckHeapBalance = EFalse; // No checking heap balance by default

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
    
    // Initialize parser variables
    iCurrentParser = NULL;
    iCurrentParserReadFirstLine = EFalse;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: NewL

     Description: Two-phased constructor.
          
     Parameters:    None

     Return Values: CTestScripter*: new object

     Errors/Exceptions: Leaves if new or ConstructL leaves.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CTestScripter* CTestScripter::NewL()
    {
     
    CTestScripter* self = new (ELeave) CTestScripter();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
     
    }

/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: ~CTestScripter

     Description: Destructor
     
     Parameters:    None

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CTestScripter::~CTestScripter()
    {
    iTestObjects.ResetAndDestroy();
    iTestModules.ResetAndDestroy();
    iDefinedIni.ResetAndDestroy();
    iDefinedRuntime.ResetAndDestroy();
    iDefinedLocal.ResetAndDestroy();
    iParserStack.ResetAndDestroy();
    iTestObjects.Close();
    iTestModules.Close();
    iDefinedIni.Close();
    iDefinedRuntime.Close();
    iDefinedLocal.Close();
    iParserStack.Close();

    iCurrentParser = NULL;
    delete iSectionParser;
    delete iTestRunner;

    iLog = NULL;
    delete iStdLog;
    iStdLog = NULL;
    delete iTCLog;
    iTCLog = NULL;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: InitL

     Description: InitL is used to initialize the Test Module.

     Parameters: const TFileName& aIniFile: in: Initialization file
                 TBool aFirstTime: in: First time flag 
                         
     Return Values: Symbian OS error code

     Errors/Exceptions: Leaves if ReadInitializationL leaves
     
     Status: Draft

-------------------------------------------------------------------------------
*/        
TInt CTestScripter::InitL( TFileName& aIniFile, 
                           TBool /*aFirstTime*/ )
    {
 
    __TRACEFUNC();
 
    if( aIniFile.Length() > 0 )
        {
        // Read initialization from test case file
        ReadInitializationL( aIniFile, iDefinedIni );
        }
        
    return KErrNone;
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: GetTestCases

     Description: GetTestCases is used to inquired test cases

     Parameters:    const TFileName& aConfigFile: in: Test case file
                    RPointerArray<RTestEngine::TTestCaseInfo>& aTestCases: out: 
                          Array of TestCases 
     
     Return Values: KErrNone: Success
                    Symbian OS error code

     Errors/Exceptions: Leaves if CStifParser::SectionL leaves
                        Leaves if CStifParser::NextSectionL leaves
                        Leaves if memory allocation fails

     Status: Draft
    
-------------------------------------------------------------------------------
*/        
TInt CTestScripter::GetTestCasesL( const TFileName& aConfigFile, 
                                   RPointerArray<TTestCaseInfo>& aTestCases )
    {
    __TRACEFUNC();
    
    if( aConfigFile.Length() == 0 )
        {
        __TRACE( KError, (_L("No test case script file given") ) );
        __RDEBUG( (_L("No test case script file given") ) );
        return KErrNotFound;
        }

    CStifParser* parser = NULL; 
    
    // Open test case file
    TRAPD( err, 
        parser = CStifParser::NewL( _L(""), 
                                    aConfigFile, 
                                    CStifParser::ECStyleComments ) );
    if( err != KErrNone )
        {
        __TRACE( KError, (_L("Given test case script file [%S] not found"),
            &aConfigFile ) );
        __RDEBUG( (_L("Given test case script file [%S] not found"),
            &aConfigFile ) );
        return err;
        }
        
    CleanupStack::PushL( parser );
    CStifSectionParser* section;
    TPtrC tmp;
    TInt index = 0;
    TInt ret = KErrNone;

    // Find first section
    section = parser->SectionL( KTestStartTag, KTestEndTag );
    if( section == NULL )
        {
        ret = KErrNotFound;
        }
    else
        {    
        // Parse all sections
        while( section )
            {
            CleanupStack::PushL( section );
             
            // Get title line
            if( section->GetLine( TTestKeywords::Keyword( TTestKeywords::ETitle ),
                tmp, ENoTag ) != KErrNone )
                {
                __TRACE( KError, (_L("Title not given for test case")));
                User::Leave( KErrNotFound );
                }
            else
                {
                if( tmp.Length() > KMaxName )
                    {
                    tmp.Set( tmp.Left( KMaxName ) );
                    }   
                TTestCaseInfo* tc = new ( ELeave ) TTestCaseInfo();
                CleanupStack::PushL( tc );
                __TRACE( KVerbose, (_L("TestCase: %S"), &tmp));
                tc->iTitle.Copy( tmp );
                tc->iCaseNumber = ++index;
                 
                 // Get timeout if defined
                CStifItemParser* item = section->GetItemLineL(  
                   TTestKeywords::Keyword( TTestKeywords::ETimeout ) );
                if( item )
                    {
                    TInt timeout; // In milliseconds
                    ret = item->GetInt( 
                        TTestKeywords::Keyword( TTestKeywords::ETimeout ), 
                        timeout ); 
                    if( ret != KErrNone )
                        {
                        __TRACE( KError, (_L("Illegal timeout")));
                        User::Leave( ret ); 
                        }
                        
                    // Type cast timeout to TInt64    
                    tc->iTimeout = TInt64( timeout ) * 1000;
                    __TRACE( KMessage, (_L("Timeout: %i"), tc->iTimeout.Int64() ));
                    }
                    
                 // Get priority if defined
                item = section->GetItemLineL(  
                   TTestKeywords::Keyword( TTestKeywords::EPriority ) );
                if( item )
                    {
                    // First try to interpret as integer
                    ret = item->GetInt( 
                        TTestKeywords::Keyword( TTestKeywords::EPriority ), 
                        tc->iPriority ); 
                    if( ret != KErrNone )
                        {
                        TPtrC priority;
                        // If priority was not given as integer, it must be 
                        // one of the predefined values
                        ret = item->GetString( 
                            TTestKeywords::Keyword( TTestKeywords::EPriority ),
                            priority );
                        if( ret != KErrNone )
                            {
                            __TRACE( KError, (_L("Illegal priority")));
                            User::Leave( ret ); 
                            }
                        switch( TTestKeywords::Parse( priority, 
                                                       TTestKeywords::Priority ) )
                            {
                            case TTestKeywords::EPriHigh:
                                tc->iPriority = TTestCaseInfo::EPriorityHigh;
                                break;
                            case TTestKeywords::EPriNormal:
                                tc->iPriority = TTestCaseInfo::EPriorityNormal;
                                break;
                            case TTestKeywords::EPriLow:
                                tc->iPriority = TTestCaseInfo::EPriorityLow;
                                break;
                            default:
                                __TRACE( KError, (_L("Illegal priority")));
                                User::Leave( KErrArgument ); 
                            }
                        }
                    __TRACE( KMessage, (_L("Priority: %i"), tc->iPriority ));
                    }
                    
                aTestCases.Append(tc);
                CleanupStack::Pop( tc );
                }
            CleanupStack::PopAndDestroy( section );
            section = parser->NextSectionL( KTestStartTag, KTestEndTag );
            }
        }
          
    CleanupStack::PopAndDestroy( parser );

    __TRACE( KPrint, (  _L( "Configfile '%S', testcases %d" ),
        &aConfigFile, index ));     
     
    return ret;
     
    }

/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: GetConstantValue

     Description: Internal fuction to get const value defined in 
                  [Define]...[Enddefine] section of script file

     GetConstantValue gets const value defined in [Define]...[Enddefine] 
     section of script file

     Parameters:    const TDesC& aName: in: constant name 
                    TDes& avalue: out: constant value

     Return Values: KErrNone: Value is returned succesfully.
                    KErrNotFound: Constant was not found
                    Any other SymbianOS error
                    
-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestScripter::GetConstantValue( const TDesC& aName, TDes& aValue )
	{
	
	__TRACEFUNC();

    TInt count = iDefinedLocal.Count();
    for(TInt i = 0; i < count; i++)
        {
        if(iDefinedLocal[i]->Name() == aName)
            {
            aValue.Copy(iDefinedLocal[i]->Value());
            return KErrNone;
            }
        }	

	  count = iDefinedIni.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iDefinedIni[i]->Name() == aName )
            {
            aValue.Copy( iDefinedIni[i]->Value() );
            return KErrNone;
            }
        }	
	
    count = iDefinedRuntime.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iDefinedRuntime[i]->Name() == aName )
            {
            aValue.Copy( iDefinedRuntime[i]->Value() );
            return KErrNone;
            }
        }	
    return KErrNotFound;
	}


/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: RunTestCaseL

     Description: Run a specified testcase.

     RunTestCaseL is used to run an individual test case. 
  
     Parameters:    const TInt aCaseNumber: in: Testcase number 
                    const TFileName& aConfig: in: Test case file
                    TTestResult& aResult: out: test case result

     Return Values: KErrNone: Test case started succesfully.
                    KErrNotFound: Testcase not found
                    KErrUnknown: Unknown TestScripter error
                    Any other SymbianOS error

     Errors/Exceptions: Leaves if GetTestCaseL leaves
                        Leaves if RunTestL leaves
                        Leaves if memory allocation fails
     
     Status: Draft

-------------------------------------------------------------------------------
*/
TInt CTestScripter::RunTestCaseL( const TInt aCaseNumber,
                                  const TFileName& aConfig,
                                  TTestResult& aResult )
    {

    // Heap is checked by test server
    //__UHEAP_MARK;

    __TRACEFUNC();

    //Open new log file with test case title in file name
    if(iAddTestCaseTitleToLogName)
        {
        //Delete test case logger if exists
        if(iTCLog)
            {
            delete iTCLog;
            iTCLog = NULL;
            }
            
        TFileName logFileName;
        TName title;
        TestModuleIf().GetTestCaseTitleL(title);
        
        logFileName.Format(KTestScripterLogFileWithTitle, &title);
        iTCLog = CStifLogger::NewL(KTestScripterLogDir, logFileName);
        iLog = iTCLog;
        }

    __TRACE( KMessage, (_L("\n\n***Testcase started***")));

    // Remove locally defined variables 
    iDefinedLocal.ResetAndDestroy();
    // Remove existing function parsers (there shouldn't be any)
    iParserStack.ResetAndDestroy();
    
    // Keep file name of config file
    iConfig = aConfig;

    // Read initialization from test case file
    ReadInitializationL( aConfig, iDefinedRuntime );

    // Get case from test case file
    iSectionParser = GetTestCaseL( aCaseNumber, aConfig );
    iCurrentParser = iSectionParser;
    iCurrentParserReadFirstLine = EFalse;

    // Check parsing result
    if( iSectionParser == NULL )
        {
        __TRACE( KError, (_L("***Parsing testcase failed***\n\n")));
        // Delete runtime defines
        iDefinedRuntime.ResetAndDestroy();
        //__UHEAP_MARKEND;
        return KErrNotFound;
        }

    // When option is set in cfg file, on EKA2 env memory leaking is enabled
	TInt memCellsBef = 0; //memory in current thread allocated before the test case is run
	TInt memCellsAft = 0; //memory in current thread allocated after the test case has run

   	memCellsBef = User::Heap().Count();
	__TRACE(KMessage, (_L("Allocated memory cells before the test case: %d"), memCellsBef));

    CActiveScheduler* activeScheduler =
        new ( ELeave ) CActiveScheduler();
    CleanupStack::PushL( activeScheduler );
     
    if ( CActiveScheduler::Current() == NULL )
    	{
    	CActiveScheduler::Install( activeScheduler );
    	}

    // Run the given testcase described in iSectionParser section
    RunTestL();

    iTestObjects.ResetAndDestroy();
    iTestModules.ResetAndDestroy();

    // TestScripter must stop Active Scheduler after test
    // object is destroyed. Otherwise if unexpected error occurs
    // handling is erronous.
    CleanupStack::PopAndDestroy( activeScheduler );

	// Check for memory leak.
	// It is reported in log always, but result of test case is changed only
	// when option in cfg file is enabled.
	memCellsAft = User::Heap().Count();
	__TRACE(KMessage, (_L("Allocated memory cells after the test case: %d"), memCellsAft));

	// if there is a difference report memory leak
	if(memCellsAft != memCellsBef)
		{
		__TRACE(KError, (_L("Memory leak, %d cell(s) is missing"), memCellsAft - memCellsBef));
		if(iCheckHeapBalance)
			{
			// Memory leaks detection is disabled for UI components testing
			if ( !( TestModuleIf().UITesting() ) )
				{
				UpdateTestCaseResult(KErrGeneral, _L("Memory leak has occured"));
				}
			}
		}

    // Delete parser and set current parser to NULL
    iCurrentParser = NULL;
    delete iSectionParser;
    iSectionParser = NULL;  
    
    // Erase config file name
    iConfig = KNullDesC; 
    
    // Return result 
    aResult = iResult;
    
    if( iResult.iResult == KErrNone )
        {
        __TRACE( KPrint, (_L("***Testcase PASSED***\n\n")));
        TestModuleIf().Printf( KPrintPriNorm, _L("TestScripter"), 
            _L("***Testcase PASSED***\n\n"));
        }
    else
        {        
        __TRACE( KPrint, (_L("***Testcase FAILED***\n\n")));
        TestModuleIf().Printf( KPrintPriNorm, _L("TestScripter"), 
            _L("***Testcase FAILED***\n\n"));
        }
    User::After(300000);
    // Delete runtime defines
    iDefinedRuntime.ResetAndDestroy();
    
    //__UHEAP_MARKEND;
             
    //If log was replaced then restore it
    if(iAddTestCaseTitleToLogName)
        {
        iLog = iStdLog;
        delete iTCLog;
        iTCLog = NULL;
        }
    
    return KErrNone;
     
    }
    
/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: ReadInitializationL

     Description: Read initialization from file.

     Parameters:    const TDesC& aIniFile: in: File that contains initialization
                    
     Return Values: None
     
     Errors/Exceptions: None
     
     Status: Draft

-------------------------------------------------------------------------------
*/
void CTestScripter::ReadInitializationL(
    const TDesC& aIniFile,
    RPointerArray<CDefinedValue>& aDefines )
    {

    CStifParser* parser = NULL;

    // Open file
    TRAPD( err,
        parser = CStifParser::NewL( _L(""),
                                    aIniFile,
                                    CStifParser::ECStyleComments ) );
    if( ( err == KErrNone ) && parser )
        {
        CleanupStack::PushL( parser );
        __TRACE( KMessage, (_L("Read initialization from [%S]"),
            &aIniFile ) );

        CStifSectionParser* section = parser->SectionL( KDefineStartTag,
                                                        KDefineEndTag );
        while(section)
            {
            CleanupStack::PushL( section );
            __TRACE( KMessage, (_L("Read defines")) );

            TPtrC name;
            TPtrC value;
            CStifItemParser* item = section->GetItemLineL( _L("") );
            while( item )
                {
                CleanupStack::PushL( item );

                if( item->GetString( _L(""), name ) != KErrNone )
                    {
                    __TRACE( KError, (_L("No define name given")) );
                    User::Leave( KErrGeneral );
                    }
                if( item->Remainder( value ) != KErrNone )
                    {
                    __TRACE( KError, (_L("No define value given")) );
                    User::Leave( KErrGeneral );
                    }
                TInt count = aDefines.Count();
                TInt i = 0;
                for( ; i < count; i++ )
                    {
                    if( aDefines[i]->Name() == name )
                        {
                        // Update existing
                        aDefines[i]->SetValueL( value );
                        __TRACE(KMessage, (_L("Update define [%S]: [%S]"), &name, &value));
                        break;
                        }
                    }
                if( i == count)
                    {
                    // New define, store it
                    CDefinedValue* define = CDefinedValue::NewL( name, value );
                    CleanupStack::PushL( define );
                    User::LeaveIfError( aDefines.Append( define ) );
                    CleanupStack::Pop( define );
                    }

                CleanupStack::PopAndDestroy( item );
                item = section->GetNextItemLineL();
                }
            CleanupStack::PopAndDestroy( section );
            section = parser->NextSectionL(KDefineStartTag, KDefineEndTag);
            }

		//Read StifSettings section and find value for CheckHeapBalance.
		//(In cfg file in settings section User may also set CapsModifier
		// option. This is handled in TestServerClient.cpp in 
		// RTestServer::GetCapsModifier method).
		section = parser->SectionL(KStifSettingsStartTag, KStifSettingsEndTag);

		if(section)
			{
			CleanupStack::PushL(section);
			__TRACE(KMessage, (_L("Read stif settings")));

			TPtrC value;
			CStifItemParser* item = section->GetItemLineL(_L(""));
			while(item)
				{
				CleanupStack::PushL(item);
				__TRACE( KMessage, (_L("Got settings line")));

				if(item->GetString(_L("CheckHeapBalance="), value) == KErrNone)
					{
					__TRACE(KMessage, (_L("Got CheckHeapBalance item, value=%S"), &value));
					if(value.Compare(_L("off")) == 0)
		       			{
		       			iCheckHeapBalance = EFalse;
		       			}
					else if(value.Compare(_L("on")) == 0)
						{
						iCheckHeapBalance = ETrue;
		       			}
					else
						{
						__TRACE(KError, (_L("Value '%S' for CheckHeapBalance setting is not supported. Aborting"), &value));
						User::Leave(KErrNotSupported);
						}
					}
				CleanupStack::PopAndDestroy(item);
				item = section->GetNextItemLineL();
				}
			CleanupStack::PopAndDestroy(section);
			}

        CleanupStack::PopAndDestroy( parser );
        }
    }


/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: GetTestCaseL

     Description: Get specified test case section from configfile.

     Parameters:    const TInt aCaseNumber: in: Test case number
                    const TFileName& aConfig: in: Configfile name 
     
     Return Values: CStifSectionParser*: pointer to test case section

     Errors/Exceptions: Leaves if CStifParser::NewL leaves
                        Leaves if CStifParser::SectionL leaves
                        Leaves if memory allocation fails
                                

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CStifSectionParser* CTestScripter::GetTestCaseL( const TInt aCaseNumber, 
                                             const TFileName& aConfig )
    {
    __TRACEFUNC();
    CStifParser* parser = NULL;
    
    TRAPD( err, 
        parser = CStifParser::NewL( _L(""), 
                                    aConfig, 
                                    CStifParser::ECStyleComments ); );
    if( err != KErrNone )
        {
        __TRACE( KError, (_L("Test case file [%S] not found"), &aConfig ));
        User::Leave( err );
        }
    CleanupStack::PushL( parser );

    CStifSectionParser* section = NULL;
    TRAP( err, 
        section =parser->SectionL( KTestStartTag, KTestEndTag, aCaseNumber ););
    if( err != KErrNone )
        {
        __TRACE( KError, 
            (_L("Section [%S/%d] not found"), &aConfig, aCaseNumber ));
        User::Leave( err );
        }

    CleanupStack::PopAndDestroy( parser );
    return section;
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: GetSubL

     Description: Get specified function (sub) from stored config file.

     Parameters:    const TDesC& aSubName: in: function name 
     
     Return Values: CStifSectionParser*: pointer to test function section

     Errors/Exceptions: Leaves if CStifParser::NewL leaves
                        Leaves if CStifParser::SectionL leaves
                        Leaves if memory allocation fails
                                
     Status: Draft
    
-------------------------------------------------------------------------------
*/
CStifSectionParser* CTestScripter::GetSubL(const TDesC& aSubName)
    {
    __TRACE(KMessage, (_L("Searching sub [%S]."), &aSubName));
    // Check if config file is set
    if(iConfig.Length() == 0)
        {
        __TRACE(KError, (_L("Searching sub [%S]. Config file is not set."), &aSubName));
        User::Leave(KErrBadName);
        }
        
    // Create parser
    CStifParser* parser = NULL;
    CStifSectionParser* section = NULL;
        
    TRAPD(err, 
          parser = CStifParser::NewL(_L(""), iConfig, CStifParser::ECStyleComments);
         );
    if(err != KErrNone)
        {
        __TRACE(KError, (_L("Searching sub [%S]. Error [%d] when loading config file [%S]."), &aSubName, err, &iConfig));
        User::Leave(err);
        }
    CleanupStack::PushL(parser);

    // Set section tags
    _LIT(KEndSubTag, "[EndSub]");
    TName startSubTag;
    startSubTag.Copy(_L("[Sub "));
    startSubTag.Append(aSubName);
    startSubTag.Append(_L("]"));
    
    // Load section
    TRAP(err, 
         section = parser->SectionL(startSubTag, KEndSubTag, 1);
        );
    if(err != KErrNone)
        {
        __TRACE(KError, (_L("Searching sub [%S]. Searching section %S%S ended with error [%d]."), &aSubName, &startSubTag, &KEndSubTag, err));
        User::Leave(err);
        }
    if(!section)
        {
        __TRACE(KError, (_L("Searching sub [%S]. Section %S%S not found."), &aSubName, &startSubTag, &KEndSubTag));
        User::Leave(err);
        }
    else
        {
        __TRACE(KMessage, (_L("Searching sub [%S]. Section %S%S found."), &aSubName, &startSubTag, &KEndSubTag));
        }

    CleanupStack::PopAndDestroy(parser);
    return section;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: RunTestL

     Description: Run a testcase specified by iSectionParser.

     Parameters:    None
     
     Return Values: None.

     Errors/Exceptions: Leaves if CSectionParser::GetItemLineL leaves
                        Leaves if CTestRunner::NewL leaves
                        Leaves if memory allocation fails                                

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestScripter::RunTestL()
    {
    __TRACEFUNC();
    iResult.iResult = KErrNone;
    iResult.iResultDes.Zero();
     
    // "title" keyword must be in the first line
    TPtrC title;
    if( iCurrentParser->GetLine( 
        TTestKeywords::Keyword( TTestKeywords::ETitle ), title, ENoTag ) 
        != KErrNone )
        {
        __TRACE( KError, (_L("title not found from section")));
        User::Leave( KErrNotFound ); 
        }
    iCurrentParserReadFirstLine = ETrue;

    __TRACE( KMessage, (_L("RunTest: %S"), &title ));
     
    iTestRunner = CTestRunner::NewL( this );

    TestModuleIf().Printf( KPrintPriNorm, _L("RunTest"), _L("%S"), &title );
    
    // Rest of the job is done by test runner
    iTestRunner->SetRunnerActive();

    // Start activeScheduler looping testcase lines
    __TRACE( KMessage, (_L("Start CActiveScheduler")));
    CActiveScheduler::Current()->Start();
  
    delete iTestRunner;
    iTestRunner = NULL;
    __TRACE( KMessage, ( _L("RunTestL: Done")));
 
    // Destroy locally defined variables 
    iDefinedLocal.ResetAndDestroy();
    // Destroy function parsers (there shouldn't be any)
    iParserStack.ResetAndDestroy();
    }     

/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: GetTestModuleL

     Description: Load testmodule if not already loaded, otherwise return
                  description of the loaded testmodule.

     Parameters:    TDesC& aModule: in: module name
                    TDesC& aIniFile: in: ini file name
     
     Return Values: CTCTestModule*: pointer to testmodules description
     
     Errors/Exceptions: Leaves if CTCTestModule::NewL leaves
                        Leaves if RPointerArray::Append fails  

     Status: Draft
    
-------------------------------------------------------------------------------
*/              
TTestModule* CTestScripter::LoadTestModuleL( TDesC& aModule )
    {     
    __TRACEFUNC();
    TInt count = iTestModules.Count();
    for( TInt i=0; i < count; i++ )
        {
        if( iTestModules[i]->ModuleName() == aModule )
            {
            // Found test module, return description
            __TRACE( KMessage, 
                (_L("GetTestModuleL: Use already loaded TestModule (%S)"), 
                &aModule ));
            return iTestModules[i];
            }
        }
         
    __TRACE( KMessage, (_L("GetTestModuleL: Load new TestModule (%S)"), 
        &aModule ));
    TTestModule* module = new (ELeave) TTestModule();
    CleanupStack::PushL( module );
    module->ModuleName() = aModule;
    User::LeaveIfError( iTestModules.Append( module ) );
    CleanupStack::Pop( module );

    TInt ret = module->iLibrary.Load ( aModule );
    
    if( ret != KErrNone )
        {
        __TRACE( KMessage, (_L("GetTestModuleL: %S loading failed"), 
            &aModule ));
        TestModuleIf().Printf( KMessage, _L("Load dll"), _L("%S failed"), 
            &aModule );
        
        User::Leave( ret );
        }
         
    // Get pointer to first exported function    
    module->iLibEntry = (CInterfaceFactory) module->iLibrary.Lookup(1);
    return module;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: CreateObjectL

     Description: Load testmodule if not already loaded, otherwise return
                  description of the loaded testmodule.

     Parameters:    TDesC& aModule: in: module name
                    TDesC& aObjectId: in: object id name
     
     Return Values: None
     
     Errors/Exceptions: Leaves on error

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestScripter::CreateObjectL( TDesC& aModule, TDesC& aObjectId )
    {     
    __TRACEFUNC();

    // Load module and get pointer
    TTestModule* module = LoadTestModuleL( aModule );
    TTestObject* object = new (ELeave) TTestObject();
    CleanupStack::PushL( object );
    object->ObjectId() = aObjectId;
    User::LeaveIfError( iTestObjects.Append( object ) );
    CleanupStack::Pop( object );
        
    // Create object    
    object->iScript = module->iLibEntry( TestModuleIf() );
    //User::LeaveIfNull ( object->iScript );
    if( object->iScript == NULL )
        {
        User::Leave( KErrGeneral );
        }

    // Create continue callback 
    object->iContinue = CTestContinue::NewL( this, object );

    // Create function pointer operation to possible
    object->iScript->SetScripter( &CallBack, this );
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: CreateKernelObjectL

     Description: Load testdriver if not already loaded, otherwise return
                  description of the loaded testdriver.

     Parameters:    TDesC& aDriver: in: driver name
                    TDesC& aObjectId: in: object id name
     
     Return Values: None
     
     Errors/Exceptions: Leaves on error

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestScripter::CreateKernelObjectL( TDesC& aDriver, TDesC& aObjectId )
    {     

    __TRACEFUNC();
    
    if( ( aDriver.Length() > KMaxName ) ||
        ( aObjectId.Length() > KMaxName ) )
        {
        __TRACE( KError, (_L("CreateKernelObjectL: Max lenght exceeded") ) );
        User::Leave( KErrArgument );
        }
    TInt ret = User::LoadLogicalDevice( aDriver );
    if( ( ret != KErrNone ) && ( ret != KErrAlreadyExists ) )
        {
        __TRACE( KError, 
            (_L("CreateKernelObjectL: User::LoadLogicalDevice failed %d"), 
                ret ) );
        User::Leave( ret );
        } 
        
    TTestObjectKernel* object = new (ELeave) TTestObjectKernel();
    CleanupStack::PushL( object );
    object->ObjectId() = aObjectId;
    object->LddName().Copy( aDriver );
    ret = object->KernelTestClass().Open( 
                object->KernelTestClass().VersionRequired(), 
                aDriver );
    if( ret != KErrNone )
        {
        __TRACE( KError, 
            (_L("CreateKernelObjectL: KernelTestClass().Open failed %d"), 
                ret ) );
        User::Leave( ret );
        }
        
    User::LeaveIfError( iTestObjects.Append( object ) );
    CleanupStack::Pop( object );
        
    }
    
/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: GetTestModuleL

     Description: Load testmodule if not already loaded, otherwise return
                  description of the loaded testmodule.

     Parameters:    TDesC& aModule: in: module name
                    TDesC& aIniFile: in: ini file name
     
     Return Values: CTCTestModule*: pointer to testmodules description
     
     Errors/Exceptions: Leaves if CTCTestModule::NewL leaves
                        Leaves if RPointerArray::Append fails  

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt CTestScripter::DeleteObjectL( TDesC& aObjectId )
    {     
    __TRACEFUNC();

    TInt count = iTestObjects.Count();
    for( TInt i=0; i < count; i++ )
        {
        if( iTestObjects[i]->ObjectId() == aObjectId )
            {
            TTestObjectBase* object = iTestObjects[i];
            iTestObjects.Remove( i );
            delete object;
            return KErrNone;
            }
        }
     
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: GetTest

     Description: Get test case from testcase array.

     Parameters:    TDesC& aTestId: in: TestId for testcase
     
     Return Values: CTCTestCase*: running/runned testcase
                    NULL: Testcase with aTestId not running/runned 

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TTestObjectBase* CTestScripter::GetObject( const TDesC& aObjectId )
    {
    __TRACEFUNC();
    
    TInt count = iTestObjects.Count();
    for( TInt i=0; i < count; i++ )
        {
        if( iTestObjects[i]->ObjectId() == aObjectId )
            {
            // Found testcase with specified TestId
            return iTestObjects[i];
            }
        }
    // Object with iTestObjects not found
    return NULL;
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: CallTestClass

     Description: For sub classing operations.
  
     Parameters: const TDesC& aLine: in: script line
                    
     Return Values: TInt: Symbian error code

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt CTestScripter::CallTestClass( const TDesC& aLine )
    {
    CStifItemParser* item = CStifItemParser::NewL( aLine, 0, aLine.Length() );
    CleanupStack::PushL( item );

    TPtrC objectName;
    TInt ret( KErrNone );
    ret = item->GetString( _L( "" ), objectName );
    if( ret != KErrNone )
        {
        CleanupStack::PopAndDestroy( item);
        return ret;
        }

    TTestObjectBase* obj = GetObject( objectName );
    if( obj == NULL )
        {
        CleanupStack::PopAndDestroy(item );
        return KErrNotFound;
        }

    TRAPD( commandResult, commandResult = obj->RunMethodL( *item ) );

    CleanupStack::PopAndDestroy(item );

    return commandResult;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: GetTestScriptObject

     Description: Get object address.
  
     Parameters: const TDesC& aObjectName: in: object name
                    
     Return Values: TInt: Symbian error code

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt CTestScripter::GetTestScriptObject( const TDesC& aObjectName )
    {

    TTestObjectBase* objBase = GetObject( aObjectName );
    if( ( objBase == NULL ) || 
        ( objBase->ObjectType() != TTestObjectBase::EObjectNormal ) )
        {
        return KErrNotFound;
        }
    
    TTestObject* object = ( TTestObject* )objBase;
    
    return (TInt) object->iScript;
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: UpdateTestCaseResult

     Description: Updates result of test case. If there is already some
                  description stored, it is put in the [] brackets.
  
     Parameters: const TInt aResult: in: error code
                 const TDesC& aDescr: in: description
                    
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestScripter::UpdateTestCaseResult(const TInt aResult, const TDesC& aDescr)
    {
    // Create buffer
    RBuf buf;

    TInt ret = buf.Create(iResult.iResultDes.Length() + aDescr.Length() + 5);
    if(ret != KErrNone)
        {
        __TRACE(KError, (_L("UpdateResultDescription: descriptor creation failed [%d]"), ret));
        return;
        }
    CleanupClosePushL(buf);

    // Update result
    iResult.iResult = aResult;
    if(iResult.iResultDes.Length() > 0)
        {
        buf.Format(_L("%S [%S]"), &aDescr, &iResult.iResultDes);
        }
    else
        {
        buf.Copy(aDescr);
        }

    SetResultDescription(buf);
    
    // Close buffer
    CleanupStack::PopAndDestroy(&buf);
    }

/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: SetResultDescription

     Description: Sets result description of test case.
  
     Parameters: const TDesC& aDescr: in: new description
                    
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
EXPORT_C void CTestScripter::SetResultDescription(const TDesC& aDescr)
    {
    if(aDescr.Length() > KStifMaxResultDes)
        {
        iResult.iResultDes.Copy(aDescr.Mid(0, KStifMaxResultDes));
        }
    else
        {
        iResult.iResultDes.Copy(aDescr);
        }
    }

/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: SetLocalValue

     Description: Internal fuction to set value of local variable defined in script 

     Parameters:    const TDesC& aName: in: local variable name 
                    const TDesC& avalue: in: local variable value

     Return Values: KErrNone: Value is returned succesfully.
                    KErrNotFound: Variable was not found
                    Any other SymbianOS error
                    
-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestScripter::SetLocalValue(const TDesC& aName, const TDesC& aValue)
    {	
    __TRACEFUNC();

    TInt count = iDefinedLocal.Count();
    for(TInt i = 0; i < count; i++)
        {
        if(iDefinedLocal[i]->Name() == aName)
            {
            iDefinedLocal[i]->SetValueL(const_cast<TDesC&>(aValue));
            return KErrNone;
            }
        }	

    return KErrNotFound;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestScripter

     Method: GetLocalValue

     Description: Internal fuction to get value of local variable 

     Parameters:    const TDesC& aName: in: local variable name 
                    const TDesC& avalue: in: local variable value

     Return Values: KErrNone: Value is returned succesfully.
                    KErrNotFound: Variable was not found
                    Any other SymbianOS error
                    
-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestScripter::GetLocalValue(const TDesC& aName, TDes& aValue)
    {
    __TRACEFUNC();

    TInt count = iDefinedLocal.Count();
    for(TInt i = 0; i < count; i++)
        {
        if(iDefinedLocal[i]->Name() == aName)
            {
            aValue.Copy(iDefinedLocal[i]->Value());
            return KErrNone;
            }
        }	

    return KErrNotFound;
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CTestRunner class 
    member functions. CTestRunner is used to execute TestScripter testcase by 
    CTestScripter.

-------------------------------------------------------------------------------
*/
// MACROS
#ifdef LOGGER
#undef LOGGER
#endif
#define LOGGER iTestScripter->iLog

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: CTestRunner

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.
     
     Parameters: CTestScripter* aTestScripter: in: Backpointer to CTestScripter

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CTestRunner::CTestRunner( CTestScripter* aTestScripter ): 
    CActive(  CActive::EPriorityHigh ), // Executed with highest priority 
    iState( ERunnerIdle ),
    iTestScripter( aTestScripter ),
    iRemainingTimeValue( 0 )
    {
    CActiveScheduler::Add( this );
    __TRACEFUNC();
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.

     Parameters:    None

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestRunner::ConstructL()
    {
    
    iPauseTimer.CreateLocal();
    
    // Initiaze all OOM related variables to default.
    OOMHeapToNormal();

    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: NewL

     Description: Two-phased constructor.
          
     Parameters: CTestScripter* aTestScripter: in: Backpointer to CTestScripter

     Return Values: CTestRunner*: new object

     Errors/Exceptions: Leaves if new or ConstructL leaves

     Status: Draft
    
-------------------------------------------------------------------------------
*/

CTestRunner* CTestRunner::NewL( CTestScripter* aTestScripter )
    {
    CTestRunner* self = new (ELeave) CTestRunner( aTestScripter );
     
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ~CTestRunner

     Description: Destructor
     
     Parameters:    None

     Return Values: None

     Errors/Exceptions: None

     Status: Approved
    
-------------------------------------------------------------------------------
*/     

CTestRunner::~CTestRunner()
    {
    __TRACEFUNC();
    Cancel();

    iPauseTimer.Close();
    
    delete iLine;
    iLine = NULL;
    
    TInt count = iEventArray.Count();
    TEventIf event( TEventIf::ERelEvent ); 
    for( TInt i = 0; i < count; i++ )
        {
        HBufC* tmp = iEventArray[0];
        event.SetName( iEventArray[0]->Des() );
        iEventArray.Remove(0);
		if( iTestScripter != NULL )
			{
			iTestScripter->TestModuleIf().Event( event );
			}
        delete tmp;
        }
     
    iTestCaseResults.Reset();
    iEventArray.ResetAndDestroy();
    iTestCaseResults.Close();
    iEventArray.Close();

    // Reset test case allow result to CTestModuleIf side too. This is
    // used in TAL-TA5L macro handling.
	if( iTestScripter != NULL )
		{
		User::LeaveIfError( 
			iTestScripter->TestModuleIf().ResetAllowResult() );
		}
		
    // Stop all remaining interference object
    TInt count_inter = iTestInterferenceArray.Count();
    for( TInt a = 0; a < count_inter; a++ )
        {
		if( iTestInterferenceArray[a] != NULL )
			{
			iTestInterferenceArray[a]->iInterference->Stop();
			}
        }
    iTestInterferenceArray.ResetAndDestroy();
    iTestInterferenceArray.Close();
    
    // Stop all remaining measurement modules
    const TInt count_meas = iTestMeasurementArray.Count();
    for( TInt b = 0; b < count_meas; b++ )
        {
		if( iTestMeasurementArray[b] != NULL )
			{
			iTestMeasurementArray[b]->iMeasurement->Stop();
			}
        }
    iTestMeasurementArray.ResetAndDestroy();
    iTestMeasurementArray.Close();

    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: RunL

     Description: Derived from CActive, handles testcase execution.

     Parameters:    None.

     Return Values: None.

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestRunner::RunL()
    {
    __TRACEFUNC();
	__TRACE( KMessage, (_L("CTestRunner::RunL: [%d] "), iStatus.Int() ));
     
    // Check if we need to Pause test case again  
    if( iStatus == KErrNone && iRemainingTimeValue > 0 )
        {           
        // Maximum time for one RTimer::After request                   
        TInt maximumTime = KMaxTInt / 1000;                       
        
        __TRACE( KMessage, (_L("CTestRunner::RunL: Going to reissue After request ") ) );           
        __TRACE( KMessage, (_L("CTestRunner::RunL: iRemainingTimeValue = %d"), iRemainingTimeValue ) );        
        
        if( iRemainingTimeValue < maximumTime )
            {                           
            iPauseTimer.After( iStatus, ( iRemainingTimeValue * 1000  ) );
            iRemainingTimeValue = 0;
            }
        else
            {            
            iRemainingTimeValue -= maximumTime;
            iPauseTimer.After( iStatus, ( maximumTime * 1000 ) );                
            }     
        SetActive();
        }     
    else    
        {  
        TBool continueTask = EFalse;

        User::LeaveIfError( iStatus.Int() );

        if( ( iTestScripter == NULL ) || 
            ( iTestScripter->iCurrentParser == NULL ) )
            {
            __TRACE( KError, (_L("CTestRunner invalid arguments")));
            User::Leave( KErrGeneral );
            }
           
        iState = ERunnerIdle;

        // Get next execution line from configuration section
        TPtrC line;
        // If current parser already has read the first line, then read next line.
        // Otherwise read the first line.
        if(iTestScripter->iCurrentParserReadFirstLine && iTestScripter->iCurrentParser->GetNextLine(line) == KErrNone 
           || !iTestScripter->iCurrentParserReadFirstLine && iTestScripter->iCurrentParser->GetLine(KNullDesC, line) == KErrNone)
            {
            iTestScripter->iCurrentParserReadFirstLine = ETrue;
            
            // Got new execution line 
            __TRACE( KMessage, (_L("Executing line [%S]"), &line));
            
            CStifItemParser* item = PreprocessLineL( line );
                
            CleanupStack::PushL( item ); 
              
            TPtrC keyword;
             // Get first word from line, i.e. keyword
            User::LeaveIfError( item->GetString( _L(""), keyword ) );

            __TRACE( KMessage, (_L("CTestRunner execute %S"), &keyword ));

            // Execute script line 
            continueTask = ExecuteLineL( keyword, item );
            __TRACE( KMessage, (_L("CTestRunner %S executed"), &keyword ));
                         
            if( continueTask )
                {
                  __TRACE( KMessage, (_L("RunL: continueTask")));
                 // Set CTestRunner active again to perform 
                 // next execution line
                 // from testcase section 
                SetRunnerActive();
                }

            CleanupStack::PopAndDestroy( item );
            }
        else // Stop execution if end of test case
            {
            __TRACE( KMessage, (_L("Executing line: no more lines from this section")));
            // There is no more lines in current parser, but it needs to be
            // checked if the parser is not taken for function (sub).
            // If this is true, then we have to get back to the parser which
            // has called the function.
            TInt lastParserIndex = iTestScripter->iParserStack.Count() - 1;
            if(lastParserIndex >= 0)
                {
                // Delete current (the last one) parser
                delete iTestScripter->iParserStack[lastParserIndex];
                iTestScripter->iParserStack.Remove(lastParserIndex);
                if(lastParserIndex >= 1) //There is some other parser on the stack
                    {
                    iTestScripter->iCurrentParser = iTestScripter->iParserStack[lastParserIndex - 1];
                    }
                else //The parser stack is empty
                    {
                    iTestScripter->iCurrentParser = iTestScripter->iSectionParser;
                    }
                __TRACE(KMessage, (_L("Returning to calling parser stored on section stack")));
                
                // Continue with the test case
                __TRACE(KMessage, (_L("RunL: continueTask (end of Sub reached)")));
                SetRunnerActive();
                return;
                }
            // No more execution lines in testcase section
            __TRACE( KMessage, 
                (_L("CTestRunner::RunL: Testcase script done") ));
            iTestScripter->TestModuleIf().Printf( KPrintPriLow, 
                _L("RunL"), _L("Script done"));
            
       
            __TRACE( KMessage, 
                (_L("RunL: All TestCases done, stop CActiveScheduler")));
            CActiveScheduler::Current()->Stop();
            // Now testcase section is executed, 
            // so CTestRunner has done its job and stops
            }
        }
    }
     
/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: DoCancel

     Description: Derived from CActive handles the Cancel

     Parameters:    None.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestRunner::DoCancel()
    {
    __TRACEFUNC();
    __TRACE( KMessage, (_L("CTestRunner::DoCancel")));
    iTestScripter->TestModuleIf().Printf( KPrintPriLow, _L("Runner"), _L("DoCancel"));
    
    if( iState == ERunnerPaused )
        {
        iPauseTimer.Cancel();
        }
   
    CActiveScheduler::Current()->Stop();
         
    iState = ERunnerCancel;
      
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: RunError

     Description: Derived from CActive handles errors from active handler.
  
     Parameters:    TInt aError: in: error from CActive
     
     Return Values: KErrNone: success

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt CTestRunner::RunError( TInt aError )
    {
    __TRACEFUNC();
    __TRACE( KMessage, (_L("CTestRunner::RunError %d"), aError));

    if ( iRunErrorMessage.Length() != 0 )
    	{
	    iTestScripter->TestModuleIf().Printf( KPrintPriLow, _L("Runner"), 
	            _L("RunError : %S"), &iRunErrorMessage );  
	    iRunErrorMessage = KNullDesC;
    	}
    else
    	{
	    iTestScripter->TestModuleIf().Printf( KPrintPriLow, _L("Runner"), 
	            _L("RunError"));        	
    	}

    iState = ERunnerError;
    
    // Return error from here, if none given from execution
    if( iTestScripter->iResult.iResult == KErrNone )
        {
        iTestScripter->UpdateTestCaseResult(aError, _L("CTestRunner::RunError"));
        }
        
    CActiveScheduler::Current()->Stop();
        
    return KErrNone;
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: PreprocessLineL

     Description: Preprocesses script line
  
     Parameters:  TPtrC& line: in: script line
                  CStifItemParser*& aItem: out: New CItemParser for script line.
                    
     Return Values: HBufC* pointer if new memory that has been allocated

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/        
CStifItemParser* CTestRunner::PreprocessLineL( TDesC& line )
    {
    CStifItemParser* item = NULL;
    TPtrC tmp;
    TInt len = 0;
    TInt ret = 0;
        
    // Decide how long buffer should be allocated
    if( line.Length() < KMaxName/2 )
        {
        len = KMaxName;
        }
    else 
        {
        len = line.Length() + KMaxName;
        }
    delete iLine;
    iLine = 0;
    iLine = HBufC::NewL(len);
    TPtr parsedLine(iLine->Des());
    len = 0;
    
    HBufC* sourceLine = line.AllocL();
    CleanupStack::PushL(sourceLine);
    
    //Check for local variable definitions
    item = CStifItemParser::NewL(sourceLine->Des(), 0, sourceLine->Length());
    CleanupStack::PushL(item);
    
    ret = item->GetString(KNullDesC, tmp);
    TBool isVarDefinition = (tmp == TTestKeywords::Keyword(TTestKeywords::EVar));
         
    if(!isVarDefinition)
        {
        while( ret == KErrNone )
            {
            len += CheckDefinedLocals(tmp);
            if((parsedLine.Length() + tmp.Length() + 1) > parsedLine.MaxLength())
                {
                // Allocate bigger buffer
                HBufC* tmpBuf = HBufC::NewL(parsedLine.MaxLength() + KMaxName);
                CleanupStack::PushL(tmpBuf);
                TPtrC ptr(iLine->Des());
                parsedLine.Set(tmpBuf->Des());
                parsedLine.Copy(ptr);
                delete iLine;
                iLine = tmpBuf;
                CleanupStack::Pop(tmpBuf);
                } 
            parsedLine.Append(tmp);        
            ret = item->GetNextString(tmp);
            if(ret == KErrNone)
                {
                // Add space only if we got new string
                parsedLine.Append(_L(" "));        
                }
            }
        CleanupStack::PopAndDestroy(item);
        item = NULL;
        CleanupStack::PopAndDestroy(sourceLine);
        sourceLine = NULL;
        
        //Prepare data for checking for defines
        sourceLine = parsedLine.AllocL();
        CleanupStack::PushL(sourceLine);
        parsedLine.Zero();
        
        item = CStifItemParser::NewL(sourceLine->Des(), 0, sourceLine->Length());
        CleanupStack::PushL(item);
    
        ret = item->GetString(KNullDesC, tmp);
        }

    //Check for defines
    while(ret == KErrNone)
        {
        if(!isVarDefinition)
            {
            len += CheckDefined(tmp);
            }
        
        if((parsedLine.Length() + tmp.Length() + 1) > parsedLine.MaxLength())
            {
            // Allocate bigger buffer
            HBufC* tmpBuf = HBufC::NewL(parsedLine.MaxLength() + KMaxName);
            CleanupStack::PushL(tmpBuf);
            TPtrC ptr(iLine->Des());
            parsedLine.Set(tmpBuf->Des());
            parsedLine.Copy(ptr);
            delete iLine;
            iLine = tmpBuf;
            CleanupStack::Pop(tmpBuf);
            } 
        parsedLine.Append(tmp);        
        ret = item->GetNextString(tmp);
        if( ret == KErrNone )
            {
            // Add space only if we got new string
            parsedLine.Append(_L(" "));        
            }
        }
    
    //Cleaning...
    CleanupStack::PopAndDestroy(item);
    item = NULL;
    CleanupStack::PopAndDestroy(sourceLine);
    sourceLine = NULL;

    __TRACE(KMessage, (_L("Preprocessed line [%S]"), &parsedLine));
    item = CStifItemParser::NewL( parsedLine, 0, parsedLine.Length() );
    
    return item;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: CheckDefined

     Description: Check if aWord is some defined word
  
     Parameters:  TPtrC& aWord: inout: Parsed word, defined or original returned
                    
     Return Values: TInt: length diference between new and old word
     
     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/   
TInt CTestRunner::CheckDefined( TPtrC& aWord )
    {
    
    TInt len = 0;
    TInt i = 0;

    // KLoopCounter word changing to current loop count value.
    if( aWord == KLoopCounter )
        {
        iLoopCounterDes.Zero();
        iLoopCounterDes.AppendNum( iLoopCounter );
        len = iLoopCounterDes.Length() - aWord.Length();
        aWord.Set( iLoopCounterDes );
        return len;
        }

    // First, check values defined in test case file
    TInt count = iTestScripter->iDefinedRuntime.Count();
    for( i = 0; i < count; i++ )
        {
        if( iTestScripter->iDefinedRuntime[i]->Name() == aWord )
            { 
            len = iTestScripter->iDefinedRuntime[i]->Value().Length() - aWord.Length();
            aWord.Set( iTestScripter->iDefinedRuntime[i]->Value() );
            return len;
            }
        }

    // Second, check values defined in test module initialization file
    count = iTestScripter->iDefinedIni.Count();
    for( i = 0; i < count; i++ )
        {
        if( iTestScripter->iDefinedIni[i]->Name() == aWord )
            { 
            len = iTestScripter->iDefinedIni[i]->Value().Length() - aWord.Length();
            aWord.Set( iTestScripter->iDefinedIni[i]->Value() );
            return len;
            }
        }

    return len;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: CheckDefinedLocals

     Description: Check if aWord is a local variable
  
     Parameters:  TPtrC& aWord: inout: Parsed word, defined or original returned
                    
     Return Values: TInt: length diference between new and old word
     
     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/   
TInt CTestRunner::CheckDefinedLocals( TPtrC& aWord )
    {
    
    TInt len = 0;
    TInt i = 0;

    TInt count = iTestScripter->iDefinedLocal.Count();
    for(i = 0; i < count; i++)
        {
        if(iTestScripter->iDefinedLocal[i]->Name() == aWord)
            { 
            len = iTestScripter->iDefinedLocal[i]->Value().Length() - aWord.Length();
            aWord.Set(iTestScripter->iDefinedLocal[i]->Value());
            return len;
            }
        }

    return len;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteLineL

     Description: Executes script line
  
     Parameters:    TDesC& aKeyword: in: keyword string
                    CStifItemParser* aItem: in: script line
                    
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution

     Errors/Exceptions: Leaves on error situations.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteLineL( TDesC& aKeyword,
                                 CStifItemParser* aItem )
    {
    _LIT( KErrMsgDeleteNoParam, "Delete: Name of object is not defined" );
    _LIT( KErrMsgDeleteObjNotFound, "Delete: Can't delete object %S. Object does not exists." );
    _LIT( KErrMsgPauseTimeoutNotDefined, "Pause: No timeout value given or value has invalid format" );
    _LIT( KErrMsgPauseTimeoutNotPositive, "Pause: Timeout value can't be <0" );
    _LIT( KErrMsgCreateTestModeleNotDefined, "Create: Name of test module is not defined" );
    _LIT( KErrMsgCreateObjectIdNotDefined, "Create: Name of test module object is not defined" );
    _LIT( KErrMsgCreateKernelDriverNotDefined, "CreateKernel: Kernel driver name is not defined" );
    _LIT( KErrMsgCreateKernelObjectNameNotDefined, "CreateKernel: Kernel driver object name is not defined" );
    _LIT( KErrMsgCreateKernelFailed, "CreateKernel: Can't creeate kernel driver %S" );
    _LIT( KErrMsgWaitTestClassNameNotDefined, "WaitTestClass: Test class object name is not defined" );
    _LIT( KErrMsgWaitTestClassObjNotFound, "WaitTestClass: Test class object %S does not exists" );
    _LIT( KErrMsgLoopNested, "Loop: Nested loop are not supported" );
    _LIT( KErrMsgLoopNoParam, "Loop: No loop iterations count given or value has invalid format" );
    _LIT( KErrMsgLoopInvalidParam, "Loop: Loop count must be >0" );
    _LIT( KErrMsgLoopUnknownUnexpectedOption, "Loop: Unknown or unexpected loop option");
    _LIT( KErrMsgEndLoopNotOpened, "Endloop: Can't execute endloop. No opened loop exists" );
    _LIT( KErrMsgBringToForegroundNotSupported, "BringToForeground: BringToForeground is not supported in non s60 environment" );
    _LIT( KErrMsgSendToBackgroundNotSupported, "SendToBackground: SendToBackground is not supported in non s60 environment" );    
    _LIT( KErrMsgPressKeyNotSupported, "PressKey: PressKey is not supported in non s60 environment. Check if .cfg file name contains ui_ prefix and UITestingSupport= YES entry is defined in TestFrameworkd.ini" );
    _LIT( KErrMsgTypeTextNotSupported, "TypeText: TypeText is not supported in non s60 environment. Check if .cfg file name contains ui_ prefix and UITestingSupport= YES entry is defined in TestFrameworkd.ini" );
    _LIT( KErrMsgSendPointerEventNotSupported, "SendPointerEvent: SendPointerEvent is not supported in non s60 environment. Check if .cfg file name contains ui_ prefix and UITestingSupport= YES entry is defined in TestFrameworkd.ini" );
    _LIT( KErrVarNameError, "Variable: Could not get variable name");
    _LIT( KErrVarValueError, "Variable: Value too long");
    _LIT( KErrSubNameError, "Sub: Could not get name of Sub to be called");
    _LIT( KErrSubGetError, "Sub: Could not get Sub section");
    
    iRunErrorMessage = KNullDesC;    
    TBool continueTask = ETrue;
    
    TInt key = TTestKeywords::Parse( aKeyword, TTestKeywords::Keyword );

    switch( key )
        {
        // Test case execution control cases
        case TTestKeywords::ECreate:
        case TTestKeywords::ECreateX:
            {
            TPtrC tmp;
            TName module;

            // Testmodule name
            TInt ret = aItem->GetNextString( tmp );
            if ( ret != KErrNone )
            	{
            	iRunErrorMessage = KErrMsgCreateTestModeleNotDefined;
            	User::Leave( KErrArgument );
            	}
            TParse p;
            p.Set( tmp, NULL, NULL );
            // Check if exists in module name
            if( p.ExtPresent() )
                {
                module.Copy( tmp );
                }
            else 
                {
                // No extension in module name, add it here
                _LIT( KDllExtension, ".dll");
                module.Copy( tmp );
                module.Append( KDllExtension );
                }

            // objectid
            ret = aItem->GetNextString( tmp );
            if ( ret != KErrNone )
            	{
            	iRunErrorMessage = KErrMsgCreateObjectIdNotDefined;
            	User::Leave( KErrArgument );
            	}        

            __TRACE( KMessage, (_L("%S %S"), &aKeyword, &tmp));
            iTestScripter->TestModuleIf().Printf( KPrintPriLow, _L("Runner"), 
                _L("%S %S"), &aKeyword, &tmp);

            iTestScripter->CreateObjectL( module, tmp );
            }
            break;
        case TTestKeywords::ECreateKernel:
            {
            TPtrC obj;
            TPtrC driver;

            // Testdriver name
            TInt ret = aItem->GetNextString( driver );
            if ( ret != KErrNone )
            	{
            	iRunErrorMessage = KErrMsgCreateKernelDriverNotDefined;
            	User::Leave( ret );
            	}

            // objectid            
            ret = aItem->GetNextString( obj );
            if ( ret != KErrNone )
            	{
            	iRunErrorMessage = KErrMsgCreateKernelObjectNameNotDefined;
            	User::Leave( ret );
            	}

            __TRACE( KMessage, (_L("%S %S"), &aKeyword, &obj));
            iTestScripter->TestModuleIf().Printf( KPrintPriLow, _L("Runner"), 
                _L("%S %S"), &aKeyword, &obj);
            TInt leaveErr = KErrNone;
            TRAP( leaveErr, iTestScripter->CreateKernelObjectL( driver, obj ));
            if ( leaveErr != KErrNone )
            	{
            	iRunErrorMessage.Format( KErrMsgCreateKernelFailed, &driver );
                User::Leave( leaveErr );
            	}
            }
            break;
        case TTestKeywords::EDelete:
            {
            TPtrC tmp;
            // objectid
            TInt ret = aItem->GetNextString( tmp );
            if ( ret != KErrNone )
            	{
                iRunErrorMessage = KErrMsgDeleteNoParam;
            	User::Leave( ret );
            	}

            __TRACE( KMessage, (_L("%S %S"), &aKeyword, &tmp));
            iTestScripter->TestModuleIf().Printf( KPrintPriLow, _L("Runner"), 
                _L("%S %S"), &aKeyword, &tmp);

            ret = iTestScripter->DeleteObjectL( tmp );
            if ( ret != KErrNone )
            	{
            	iRunErrorMessage.Format( KErrMsgDeleteObjNotFound, &tmp );
            	User::Leave( ret );
            	}
            }
            break;
        case TTestKeywords::ERequest:
        case TTestKeywords::EWait:
        case TTestKeywords::ERelease:
        case TTestKeywords::ESet:
        case TTestKeywords::EUnset:
            continueTask = ExecuteEventL( aKeyword, aItem );
            break;
        case TTestKeywords::EPrint:
            {
            __TRACE( KMessage, (_L("%S"), &aKeyword));
            iTestScripter->TestModuleIf().Printf( KPrintPriLow, _L("Runner"), 
                _L("%S"), &aKeyword );

            RBuf buf;
            buf.CreateL(1024);
            CleanupClosePushL(buf);
            
            TPtrC tmp;

            while( aItem->GetNextString( tmp ) == KErrNone )
                {
                if( buf.Length() + tmp.Length() >= buf.MaxLength() )
                    {
                    buf.ReAllocL(buf.MaxLength() + tmp.Length() * 10);
                    }
                buf.Append( tmp );
                buf.Append( _L(" ") );
                }
            
            iTestScripter->TestModuleIf().Printf( KPrintPriNorm, 
                _L("Test"), 
                _L("%S"), &buf);
            RDebug::Print( _L("Print : Test : %S"), &buf );
            CleanupStack::PopAndDestroy(&buf);
            }
            break;
        case TTestKeywords::EAllowNextResult:
            {
            AddTestCaseResultL( aItem );
            }
            break;
        case TTestKeywords::EWaitTestClass:
            {
            // Just stop script running, continue event is signaled 
            // from test class with specified object name
            TPtrC objectName;
            // Get Object name
            TInt ret = aItem->GetNextString( objectName );
            if ( ret != KErrNone )
            	{
               	iRunErrorMessage = KErrMsgWaitTestClassNameNotDefined;
            	User::Leave( ret );
            	}
            TTestObjectBase* obj = iTestScripter->GetObject ( objectName );
            //User::LeaveIfNull( obj );
            if( obj == NULL )
                {
               	iRunErrorMessage.Format( KErrMsgWaitTestClassObjNotFound, &objectName );
                User::Leave( KErrGeneral );
                }
            continueTask = obj->Wait();
            
            if( continueTask )
                {
                // If OOM testing is ongoing ignore result check(given by user)
                if( !iTestScripter->iOOMIgnoreFailure )
                    {
                    if( iTestCaseResults.Count() == 0 )
                        {
                        // KErrNone is the default result expected 
                        // if nothing else is given 
                        User::LeaveIfError( iTestCaseResults.Append( 
                                                                KErrNone ) );
                        }
                    
                    if( iTestCaseResults.Find( obj->iAsyncResult ) < 0 ) 
                        {
                        __TRACE( KError, ( _L("Command for [%S] failed (%d)"), 
                                &objectName, obj->iAsyncResult ));
                        iTestScripter->UpdateTestCaseResult(obj->iAsyncResult, _L("CTestRunner::ExecuteLineL: asynchronous method returned error"));
                        // Stops execution from CTestRunner::RunError
                        User::Leave( KErrGeneral );
                        }
                    }
                else
                    {
                    __TRACE( KMessage, ( 
                        _L( "OOM test: 'oomignorefailure' is ON, test case result will reset" ) ) );
                    }

                iTestCaseResults.Reset();
                // Reset test case allow result to CTestModuleIf side too. This is
                // used in TAL-TA5L macro handling.
                User::LeaveIfError( 
                    iTestScripter->TestModuleIf().ResetAllowResult() );
                }

            }
            break;
        case TTestKeywords::EPause:
            {
            // Maximum time for one RTimer::After request
            TInt maximumTime = KMaxTInt / 1000;
            // Set iRemainingTimeValue to zero
            iRemainingTimeValue = 0;

            TInt timeout;
            // Read valid results to timeout
            if( aItem->GetNextInt( timeout ) != KErrNone )
                {
                __TRACE( KError, (_L("CTestRunner::ExecuteLineL: No timeout value given for pause")));
                iRunErrorMessage = KErrMsgPauseTimeoutNotDefined;
                User::Leave( KErrArgument );
                }

            // Test case file parsing was success

            __TRACE( KMessage, (_L("CTestRunner::ExecuteLineL: Pause for %d milliseconds"), timeout));

            if( timeout < 0 )
                {
                __TRACE( KError, (_L("CTestRunner::ExecuteLineL: Given pause value < 0")));
                iRunErrorMessage = KErrMsgPauseTimeoutNotPositive;
                User::Leave( KErrArgument );
                }
            else
                {
                // Check is pause value suitable for RTimer::After
                if( timeout < maximumTime )
                    {
                    iPauseTimer.After( iStatus, ( timeout * 1000  ) );
                    }
                else
                    {
                    // Given pause value after multiplication with 1000 is
                    // larger than KMaxTInt, so we need to split it and 
                    // re-request After with remaining value from RunL

                    iRemainingTimeValue =  timeout - maximumTime;
                    iPauseTimer.After( iStatus, maximumTime * 1000 );
                    }

                SetActive();

                // Stop execution after paused for given timeout
                continueTask = EFalse;
                }
            }
            break;
        case TTestKeywords::ELoop:
            {
            if( iLoopTimes > 0 )
                {
                __TRACE( KError, (_L("ExecuteLineL: Nested loop are not supported")));
                iRunErrorMessage = KErrMsgLoopNested;
                User::Leave( KErrNotSupported );
                }
            iLoopTimes = 0;
            iLoopCounter = 0;
            iTimedLoop = EFalse;
            if( aItem->GetNextInt( iLoopTimes ) != KErrNone )
                {
                __TRACE( KError, (_L("ExecuteLineL: No loop count value given for loop")));
                iRunErrorMessage = KErrMsgLoopNoParam;
                User::Leave( KErrArgument );
                }
            if( iLoopTimes < 1 )
            	{
                __TRACE( KError, (_L("ExecuteLineL: Loop count must be >0")));
                iRunErrorMessage = KErrMsgLoopInvalidParam;
                User::Leave( KErrArgument );
            	}
            __TRACE( KMessage, (_L("ExecuteLineL: Loop for %d times" ), iLoopTimes ) );
            
            //Check loop options
            TPtrC option;
            TInt ret = aItem->GetNextString(option);
            if(ret == KErrNone)
                {
                if(option.Compare(_L("msec")) == 0) //time loop option
                    {
                    iTimedLoop = ETrue;
                    iStartTime.HomeTime();
                    iExpectedLoopTime = TInt64(iLoopTimes) * TInt64(1000); //convert to micro seconds
                    __TRACE(KMessage, (_L("ExecuteLineL: Timed loop for %d msec" ), iLoopTimes));
                    
                    ret = aItem->GetNextString(option); //Get next option
                    }
                }
                
            if(ret == KErrNone)
                {
                __TRACE( KError, (_L("ExecuteLineL: Unknown or unexpected loop option [%S]"), &option));
                iRunErrorMessage = KErrMsgLoopUnknownUnexpectedOption;
                User::Leave( KErrNotSupported );
                }
            
            iLoopStartPos = iTestScripter->iCurrentParser->GetPosition();
            }
            break;
        case TTestKeywords::EEndLoop:
            {
            if( iLoopTimes == 0 )
            	{
            	__TRACE( KError, (_L("ExecuteLineL: Can't execute endloop. No opened loop exists")));
            	iRunErrorMessage = KErrMsgEndLoopNotOpened;
            	User::Leave( KErrNotFound );
            	}
            iLoopCounter++;
            __TRACE( KMessage, 
                (_L("ExecuteLineL: Loop executed for %d times" ), 
                    iLoopCounter ) );

            TTime currTime;
            currTime.HomeTime();
            if(((!iTimedLoop) && (iLoopCounter < iLoopTimes)) //Normal loop
               ||
               iTimedLoop && (currTime.MicroSecondsFrom(iStartTime) < iExpectedLoopTime)) //Timed loop
                {
                // Go to beginning of the loop
                User::LeaveIfError(
                    iTestScripter->iCurrentParser->SetPosition( iLoopStartPos ));
                }
            else
                {            
                // End looping
                iLoopCounter = 0;
                iLoopTimes = 0;
                iLoopStartPos = 0;
                }
            }
            break;
        case TTestKeywords::ETimeout:
        case TTestKeywords::EPriority:
            // not used here
            break;
        case TTestKeywords::EOOMIgnoreFailure:
            {
            OOMIgnoreFailureL( aItem );  // Handle parsing
            break;
            }
        case TTestKeywords::EOOMHeapFailNext:
            {
            OOMHeapFailNextL( aItem );   // Handle parsing
            break;
            }
        case TTestKeywords::EOOMHeapSetFail:
            {
            OOMHeapSetFailL( aItem );    // Handle parsing
            break;
            }
       case TTestKeywords::EOOMHeapToNormal:
            {
            // Initialize all OOM related variables back to default.
            OOMHeapToNormal();
            __TRACE( KMessage, (
                _L( "'oomheaptonormal' called, OOM initialization or ending OOM test") ) );
            break;
            }
       case TTestKeywords::ETestInterference:
            {
            TestInterferenceL( aItem );  // Handle parsing
            break;
            }
       case TTestKeywords::EMeasurement:
            {
            MeasurementL( aItem );      // Handle parsing
            break;
            }
       case TTestKeywords::EAllowErrorCodes:
            {
            // Check is KErrNone already appended to iTestCaseResults array.
            if( iTestCaseResults.Find( KErrNone ) < 0 )
                {
                // 'allowerrorcodes' keyword sets KErrNone as a default
                User::LeaveIfError( iTestCaseResults.Append( KErrNone ) );
                // Set test case allow result to CTestModuleIf side too. This
                // is used in TAL-TA5L macro handling.
                User::LeaveIfError( 
                    iTestScripter->TestModuleIf().SetAllowResult( KErrNone ) );
                }

            // Set user given specific error code to be allowed.
            AddTestCaseResultL( aItem );
            }
            break;

       case TTestKeywords::EBringToForeground:
    	   {
    	   if ( iTestScripter->TestModuleIf().UITesting() == true )
    		   {
    		   iTestScripter->TestModuleIf().GetUiEnvProxy()->BringToForeground();
    		   }
    	   else
    		   {
	           	__TRACE( KError, (_L("ExecuteLineL: Can't execute bringtoforeground in non s60 environment. Check if .cfg file name contains ui_ prefix and UITestingSupport= YES entry is defined in TestFrameworkd.ini")));
	        	iRunErrorMessage = KErrMsgBringToForegroundNotSupported;
	        	User::Leave( KErrNotSupported );    		   
    		   }
    	   }
       		break;
       case TTestKeywords::ESendToBackground:
    	   {
    	   if ( iTestScripter->TestModuleIf().UITesting() == true )
    		   {
    		   iTestScripter->TestModuleIf().GetUiEnvProxy()->SendToBackground();
    		   }
    	   else
    		   {
	           	__TRACE( KError, (_L("ExecuteLineL: Can't execute sendtobackground in non s60 environment. Check if .cfg file name contains ui_ prefix and UITestingSupport= YES entry is defined in TestFrameworkd.ini")));
	        	iRunErrorMessage = KErrMsgSendToBackgroundNotSupported;
	        	User::Leave( KErrNotSupported );    		   
    		   }
    	   }
       		break;
       case TTestKeywords::EPressKey:
    	   {
    	   if ( iTestScripter->TestModuleIf().UITesting() == true )
    		   {
    		   continueTask = PressKeyL( aItem );
    		   }
    	   else
    		   {
    		   __TRACE( KError, (_L("ExecuteLineL: Can't execute presskey in non s60 environment. Check if .cfg file name contains ui_ prefix and UITestingSupport= YES entry is defined in TestFrameworkd.ini")));
    		   iRunErrorMessage = KErrMsgPressKeyNotSupported;
    		   User::Leave( KErrNotSupported );    		   
    		   }	

    	   // Check if it was global or local presskey
    	   if ( !continueTask )
    		   {
	           // Stop execution after key is pressed and wait until it is handled
	           SetActive();
    		   }           
    	   }
       		break;
       case TTestKeywords::ETypeText:
    	   {
    	   if ( iTestScripter->TestModuleIf().UITesting() == true )
    		   {
    		   continueTask = TypeTextL( aItem );
    		   }
    	   else
    		   {
    		   __TRACE( KError, (_L("ExecuteLineL: Can't execute typetext in non s60 environment. Check if .cfg file name contains ui_ prefix and UITestingSupport= YES entry is defined in TestFrameworkd.ini")));
    		   iRunErrorMessage = KErrMsgTypeTextNotSupported;
    		   User::Leave( KErrNotSupported );    		   
    		   }

    	   // Check if it was global or local typetext
    	   if ( !continueTask )
    		   {
	           // Stop execution after key is pressed and wait until it is handled
	           SetActive();
    		   }           
    	   }
       		break;
        case TTestKeywords::EVar:
            {
            TName name;
            TName buf;
            TPtrC tmp;
            
            //Get variable name
            if(aItem->GetNextString(tmp) == KErrNone)
                {
                name.Copy(tmp);
                }
            else
                {
                //Error when getting variable name
                __TRACE(KError, (_L("ExecuteLineL: Could not read variable name")));
                iRunErrorMessage = KErrVarNameError;
                User::Leave(KErrArgument);                
                }

            //Get value for variable
            while(aItem->GetNextString(tmp) == KErrNone)
                {
                if(buf.Length() + tmp.Length() >= buf.MaxLength())
                    {
                    //Error when getting variable name
                    __TRACE(KError, (_L("ExecuteLineL: Variable value too long")));
                    iRunErrorMessage = KErrVarValueError;
                    User::Leave(KErrArgument);                
                    }
                buf.Append(tmp);
                buf.Append(_L(" "));
                }
            //Remove last space
            if(buf.Length() > 0)
                {
                buf.SetLength(buf.Length() - 1);
                }
                
            //Store local variable
            TInt count = iTestScripter->iDefinedLocal.Count();
            TInt i;
            for(i = 0; i < count; i++)
                {
                if(iTestScripter->iDefinedLocal[i]->Name() == name)
                    {
                    // Update existing
                    iTestScripter->iDefinedLocal[i]->SetValueL(buf);
                    __TRACE(KMessage, (_L("Update local variable [%S]: [%S]"), &name, &buf));
                    break;
                    }
                }
            if(i == count)
                {
                // New define, store it
                CDefinedValue* define = CDefinedValue::NewL(name, buf);
                CleanupStack::PushL(define);
                User::LeaveIfError(iTestScripter->iDefinedLocal.Append(define));
                CleanupStack::Pop(define);
                __TRACE(KMessage, (_L("Add local variable [%S]: [%S]"), &name, &buf));
                }
            }
            break;
        case TTestKeywords::ECallSub:
            {
            // Get sub name
            TPtrC subName;
            
            //Get sub name
            TInt err = aItem->GetNextString(subName);
            if(err != KErrNone)
                {
                //Error when getting sub name
                __TRACE(KError, (_L("ExecuteLineL: Could not read sub name [%d]"), err));
                iRunErrorMessage = KErrSubNameError;
                User::Leave(KErrArgument);                
                }
            
            // Load section
            CStifSectionParser* subSection = NULL;
            TRAP(err, 
                 subSection = iTestScripter->GetSubL(subName);
                );
            if((err != KErrNone) || (!subSection))
                {
                //Error when getting sub name
                if(err == KErrNone)
                    {
                    err = KErrArgument;
                    }
                __TRACE(KError, (_L("ExecuteLineL: Could not get section for sub [%d]"), err));
                iRunErrorMessage = KErrSubGetError;
                User::Leave(err);                
                }
            
            // Handle new section parser
            CleanupStack::PushL(subSection);
            iTestScripter->iParserStack.AppendL(subSection);
            __TRACE(KMessage, (_L("ExecuteLineL: Section for sub [%S] appended to section stack"), &subName));
            CleanupStack::Pop(subSection);
            iTestScripter->iCurrentParser = subSection;
            iTestScripter->iCurrentParserReadFirstLine = EFalse; //Change it to false, becaue subSection is a new parser and it has nothing read
            }
            break;
        case TTestKeywords::ESetResultDescription:
            {
            __TRACE(KMessage, (_L("%S"), &aKeyword));
            iTestScripter->TestModuleIf().Printf(KPrintPriLow, _L("Runner"), _L("%S"), &aKeyword);

            TName buf;
            TPtrC tmp;

            while(aItem->GetNextString(tmp) == KErrNone)
                {
                if(buf.Length() + tmp.Length() >= buf.MaxLength())
                    {
                    break;
                    }
                if(buf.Length() > 0)
                    {
                    buf.Append(_L(" "));
                    }
                buf.Append(tmp);
                }
            
            iTestScripter->SetResultDescription(buf);
            RDebug::Print( _L("SetDescription to [%S]"), &buf );
            }
            break;
        case TTestKeywords::ESendPointerEvent:
     	   {
     	   if ( iTestScripter->TestModuleIf().UITesting() == true )
     		   {
     		   continueTask = SendPointerEventL( aItem );
     		   }
     	   else
     		   {
     		   __TRACE( KError, (_L("ExecuteLineL: Can't execute sendpointerevent in non s60 environment. Check if .cfg file name contains ui_ prefix and UITestingSupport= YES entry is defined in TestFrameworkd.ini")));
     		   iRunErrorMessage = KErrMsgSendPointerEventNotSupported;
     		   User::Leave( KErrNotSupported );    		   
     		   }

     	   // Check if it was global or local sendpointerevent
     	   if ( !continueTask )
     		   {
 	           // Stop execution after pointer event is send and wait until it is handled
 	           SetActive();
     		   }           
     	   }
     	   break;
        default:
            {
            continueTask = ExecuteCommandL( aKeyword, aItem );
            }
            break;
        }

    __TRACE( KMessage, (_L("ExecuteLineL: TestCase line executed")));

    return continueTask;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: PressKeyL

     Description: Send key press event to AppUi
  
     Parameters:    CStifItemParser* aItem: in: script line
     
     Return Values: None

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::PressKeyL( CStifItemParser* aItem )
	{
    _LIT( KErrMsgSendKeyEventInvalidParameterValue, "PressKey: Invalid parameter %s value " );
    _LIT( KErrMsgSendKeyEventInvalidParameter, "PressKey: Invalid parameter %s" );    
    
    _LIT( KKeyCodeParameter, "keycode=" );
    _LIT( KKeyScanCodeParameter, "keyscancode=" );
    _LIT( KModifierParameter, "modifier=" );
    _LIT( KRepeatsParameter, "repeats=" );
    
    _LIT( KModeGlobal, "global" );
    _LIT( KModeLocal, "local" );
    
    TBool globalMode = EFalse;
	TUint keyCode = 0; 
	TInt keyScanCode = 0; 
	TUint keyModifiers = 0; 
	TInt keyRepeats = 0;
		
	TPtrC parameter;
		
	CUiEnvProxy* uiEnvProxy = iTestScripter->TestModuleIf().GetUiEnvProxy();
	
	while( aItem->GetNextString( parameter ) == KErrNone )
		{
		if ( parameter.Find( KKeyCodeParameter ) == 0 )
			{
			TPtrC parameterValue = parameter.Right( parameter.Length() - KKeyCodeParameter().Length() );
			TLex parameterValueParser( parameterValue );
			if ( parameterValueParser.Val( keyCode ) != KErrNone )
				{
					if ( uiEnvProxy->ParseKeyCode( parameterValue, keyCode ) != KErrNone )
						{
						__TRACE( KError, (_L("SendKeyEvent: Invalid parameter value")));
						TBuf<128> errMsg;
						errMsg.Format( KErrMsgSendKeyEventInvalidParameterValue, &KKeyCodeParameter() );
						iRunErrorMessage = errMsg;
						User::Leave( KErrArgument );    		   				
						}
				}
			}
		else if ( parameter.Find( KKeyScanCodeParameter ) == 0 )
			{
			TPtrC parameterValue = parameter.Right( parameter.Length() - KKeyScanCodeParameter().Length() );
			TLex parameterValueParser( parameterValue );
			if ( parameterValueParser.Val( keyScanCode ) != KErrNone )
				{
				if ( uiEnvProxy->ParseKeyScanCode( parameterValue, keyScanCode ) != KErrNone )
					{
					__TRACE( KError, (_L("SendKeyEvent: Invalid parameter value")));
					TBuf<128> errMsg;
					errMsg.Format( KErrMsgSendKeyEventInvalidParameterValue, &KKeyCodeParameter() );
					iRunErrorMessage = errMsg;
					User::Leave( KErrArgument );    		   				
					}
				}
			}
		else if ( parameter.Find( KModifierParameter ) == 0 )
			{
			TPtrC parameterValue = parameter.Right( parameter.Length() - KModifierParameter().Length() );
			TLex parameterValueParser( parameterValue );
			if ( parameterValueParser.Val( keyModifiers ) != KErrNone )
				{
				if ( uiEnvProxy->ParseModifier( parameterValue, keyModifiers ) != KErrNone )
					{
					__TRACE( KError, (_L("SendKeyEvent: Invalid parameter value")));
					TBuf<128> errMsg;
					errMsg.Format( KErrMsgSendKeyEventInvalidParameterValue, &KModifierParameter() );
					iRunErrorMessage = errMsg;
					User::Leave( KErrArgument );    		   				
					}
				}
			}
		else if ( parameter.Find( KRepeatsParameter ) == 0 )
			{
			TPtrC parameterValue = parameter.Right( parameter.Length() - KRepeatsParameter().Length() );
			TLex parameterValueParser( parameterValue );
			if ( parameterValueParser.Val( keyRepeats ) != KErrNone )
				{
				__TRACE( KError, (_L("SendKeyEvent: Invalid parameter value")));
				TBuf<128> errMsg;
				errMsg.Format( KErrMsgSendKeyEventInvalidParameterValue, &KRepeatsParameter() );
				iRunErrorMessage = errMsg;
				User::Leave( KErrArgument );    		   				
				}
			}
		else if ( parameter == KModeGlobal )
			{
			globalMode = ETrue;
			}
		else if ( parameter == KModeLocal )
			{
			globalMode = EFalse;
			}
		else if ( uiEnvProxy->ParseKeyCode( parameter, keyCode ) == KErrNone ) 
			{			
			}			
		else if ( parameter.Length() == 1 )
			{
			keyCode = parameter[ 0 ];
			}
		else
			{
			__TRACE( KError, (_L("PressKey: Invalid parameter")));
			TBuf<128> errMsg;
			errMsg.Format( KErrMsgSendKeyEventInvalidParameter, &parameter );
			iRunErrorMessage = errMsg;
			User::Leave( KErrArgument );   			
			}
		}
	
	if ( globalMode )
		{
		uiEnvProxy->PressKeyL( keyCode, keyScanCode, keyModifiers, keyRepeats );			
		}
	else
		{
		uiEnvProxy->PressKeyL( &iStatus, keyCode, keyScanCode, keyModifiers, keyRepeats );
		}
		
	return globalMode;
	}

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: TypeTextL

     Description: Sends text to AppUi
  
     Parameters:    CStifItemParser* aItem: in: script line
     
     Return Values: None

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::TypeTextL( CStifItemParser* aItem )
	{
	_LIT( KErrMsgTypeTextInvalidParameter, "TypeText: Invalid parameter %S" );
	_LIT( KErrMsgTypeTextNotEnoughParameters, "TypeText: Not enough parameters" );
	
	TBool globalMode = EFalse;
	
	TPtrC command;
	TPtrC text;
	aItem->SetParsingType( CStifItemParser::EQuoteStyleParsing );
	
	// Read first parameter
	TInt ret = aItem->GetNextString( command );
	
	if ( ret != KErrNone )
		{
		__TRACE( KError, (_L("TypeText: Not enough parameters")));
		iRunErrorMessage = KErrMsgTypeTextNotEnoughParameters;
		User::Leave( ret );
		}
	
	// Read second parameter
	ret = aItem->GetNextString( text );
	
	// Check if second can be read. if yes then check if first parameters is
	// 'global' parameter
	if ( ret != KErrNone )
		{
		// normal type text
		text.Set( command );
		iTestScripter->TestModuleIf().GetUiEnvProxy()->TypeTextL( &iStatus, text );
		globalMode = EFalse;
		}
	else if ( command == _L("global") )
		{
		iTestScripter->TestModuleIf().GetUiEnvProxy()->TypeTextL( text );
		globalMode = ETrue;
		}
	else
		{
		__TRACE( KError, (_L("TypeText: Invalid parameter")));
		TBuf<128> errMsg;
		errMsg.Format( KErrMsgTypeTextInvalidParameter, &command );
		iRunErrorMessage = errMsg;
		User::Leave( KErrNotSupported );
		}
		
	return globalMode;
	}

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: SendPointerEventL

     Description: Send key press event to AppUi
  
     Parameters:    CStifItemParser* aItem: in: script line
     
     Return Values: None

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::SendPointerEventL( CStifItemParser* aItem )
	{
	// Error messages
    _LIT( KErrMsgNotEnoughParameters, "SendPointerEvent: Not enough parameters" );
    _LIT( KErrMsgPointerEventTypeNotDefined, "SendPointerEvent: Pointer event type not defined" );
    _LIT( KErrMsgInvalidUnknownPointerEventType, "SendPointerEvent: Invalid/Unknown pointer event type %S" );
    _LIT( KErrMsgPositionXNotDefined, "SendPointerEvent: x co-ordinate is not defined" );
    _LIT( KErrMsgPositionYNotDefined, "SendPointerEvent: y co-ordinate is not defined" );
    _LIT( KErrMsgPositionXInvalidValue, "SendPointerEvent: Invalid value of x co-ordinate" );
    _LIT( KErrMsgPositionYInvalidValue, "SendPointerEvent: Invalid value of y co-ordinate" );

    // Parameters keywords
    _LIT( KModeGlobal, "global" );
    _LIT( KModeLocal, "local" );
    
    TBool globalMode = EFalse;
	TUint eventType = 0; 
	TPoint position( 0, 0 );
	TPoint parentPosition( 0, 0 );
		
	TPtrC parameter;

	CUiEnvProxy* uiEnvProxy = iTestScripter->TestModuleIf().GetUiEnvProxy();

	// Get first parameter
	if ( aItem->GetNextString( parameter ) != KErrNone )
		{
		__TRACE( KError, (_L("SendPointerEvent: Not enough parameters")));
		iRunErrorMessage = KErrMsgNotEnoughParameters;
		User::Leave( KErrArgument );   			
		}
	else
		{
		TBool modeSelected = EFalse;
		// Check if global/local mode is defined
		if ( parameter == KModeLocal )
			{
			globalMode = false;
			modeSelected = ETrue;
			}
		else if ( parameter == KModeGlobal )
			{
			globalMode = true;
			modeSelected = ETrue;
			}
		
		if ( modeSelected )
			{
			// First parameter was mode so we need get next parameter which should 
			// contain event type
			if ( aItem->GetNextString( parameter ) != KErrNone )
				{
				__TRACE( KError, (_L("SendPointerEvent: Pointer event type not defined")));
				iRunErrorMessage = KErrMsgPointerEventTypeNotDefined;
				User::Leave( KErrArgument );   			
				}
			}
		}
	
	// Parse event type
	if ( uiEnvProxy->ParsePointerEventType( parameter, eventType ) != KErrNone )
		{
		__TRACE( KError, (_L("PressKey: Invalid/Unknown pointer event type %S"), &parameter ));
		iRunErrorMessage.Format( KErrMsgInvalidUnknownPointerEventType, &parameter );
		User::Leave( KErrArgument );   			
		}
	
	if ( aItem->GetNextString( parameter ) != KErrNone )
		{
		__TRACE( KError, (_L("SendPointerEvent: x co-ordinate is not defined")));
		iRunErrorMessage = KErrMsgPositionXNotDefined;
		User::Leave( KErrArgument );   			
		}

	TLex parameterParser( parameter );
	if ( parameterParser.Val( position.iX ) != KErrNone )
		{
		__TRACE( KError, (_L("SendPointerEvent: Invalid value of x co-ordinate")));
		iRunErrorMessage = KErrMsgPositionXInvalidValue;
		User::Leave( KErrArgument );   			
		}

	if ( aItem->GetNextString( parameter ) != KErrNone )
		{
		__TRACE( KError, (_L("SendPointerEvent: y co-ordinate is not defined")));
		iRunErrorMessage = KErrMsgPositionYNotDefined;
		User::Leave( KErrArgument );   			
		}

	parameterParser.Assign( parameter );
	if ( parameterParser.Val( position.iY ) != KErrNone )
		{
		__TRACE( KError, (_L("SendPointerEvent: Invalid value of y co-ordinate")));
		iRunErrorMessage = KErrMsgPositionYInvalidValue;
		User::Leave( KErrArgument );   			
		}		
		
	// Send pointer event
	if ( globalMode )
		{
		uiEnvProxy->SendPointerEventL( eventType, position );			
		}
	else
		{
		uiEnvProxy->SendPointerEventL( &iStatus, eventType, position );
		}

	return globalMode;
	}


/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteEventL

     Description: Executes event command script line
  
     Parameters:    TDesC& aKeyword: in: keyword string
                    CStifItemParser* aItem: in: script line
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteEventL( TDesC& aKeyword,
                                  CStifItemParser* aItem )
    {
    _LIT( KErrMsgEventNameNotDefined, "%S : Event name not defined" );
    _LIT( KErrMsgUnknowKeyword, "Unknow keyword %S" );
    
    TInt ret = KErrNone;
    TInt key = TTestKeywords::Parse( aKeyword, TTestKeywords::Keyword );
    TBool continueTask = ETrue;
     
    TPtrC eventName;
    // read eventname
    ret = aItem->GetNextString( eventName );
    if ( ret != KErrNone )
    	{
    	iRunErrorMessage.Format( KErrMsgEventNameNotDefined, &aKeyword );
    	User::Leave( ret );
    	}
    
    TEventIf event;
    event.SetName( eventName );
    
    __TRACE( KMessage, (_L("%S %S"), &aKeyword, &eventName));
    iTestScripter->TestModuleIf().Printf( KPrintPriLow, _L("Event"), 
        _L("%S %S"), &aKeyword, &eventName);

    switch( key )
        {
        case TTestKeywords::ERequest:
            {
            HBufC* eName = eventName.AllocLC();
            User::LeaveIfError( iEventArray.Append( eName ) );
            CleanupStack::Pop( eName );
            event.SetType( TEventIf::EReqEvent );
            ret = iTestScripter->TestModuleIf().Event( event );
            }
            break;
        case TTestKeywords::EWait:
            {
            event.SetType( TEventIf::EWaitEvent );
            iTestScripter->TestModuleIf().Event( event, iStatus );
            SetActive();
            continueTask = EFalse;
            }
            break;
        case TTestKeywords::ERelease:
            {
            event.SetType( TEventIf::ERelEvent );
            ret = iTestScripter->TestModuleIf().Event( event );
            if( ret == KErrNone )
                {
                TPtrC eName;
                TInt count = iEventArray.Count();
                for( TInt i = 0; i < count; i++ )
                    {
                    eName.Set( iEventArray[i]->Des() );
                    if( eName == eventName )
                        {
                        HBufC* tmp = iEventArray[i];
                        iEventArray.Remove(i);
                        delete tmp;
                        break;
                        }
                    }
                }
            }
            break;
        case TTestKeywords::ESet:
            {
            event.SetType( TEventIf::ESetEvent );
            
            TPtrC tmp;
            // Get optional set argument
            if( aItem->GetNextString( tmp ) == KErrNone )
                {                      
                // Parse optional set argument
                if( tmp == _L("state") )
                    {
                    __TRACE( KMessage, (_L("State event")));
                    event.SetEventType( TEventIf::EState );
                    }
                else
                    {
                    __TRACE( KError, 
                        (_L("Unknown argument for set %S"), 
                        &tmp));
                    }
                }      
            
            ret = iTestScripter->TestModuleIf().Event( event );
            }
            break;
        case TTestKeywords::EUnset:
            {
            event.SetType( TEventIf::EUnsetEvent );
            event.SetEventType( TEventIf::EState );
            ret = iTestScripter->TestModuleIf().Event( event );
            }
            break;
        default:
            __TRACE( KError, (_L("Unknown keyword %S"), &aKeyword));
            iRunErrorMessage.Format( KErrMsgUnknowKeyword, &aKeyword );
            User::Leave( KErrGeneral );
            break;
        }
    if( ret != KErrNone )
        {
        TName resultDescr;
        resultDescr.Format(_L("Event %S returned error: %d"), &aKeyword, ret);
        iTestScripter->UpdateTestCaseResult(ret, resultDescr);
        // Stops execution from CTestRunner::RunError
        User::Leave( KErrGeneral );
        }

    return continueTask;

    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteCommandL

     Description: Executes script line

     Parameters:    TDesC& aObject: in: object name 
                    CStifItemParser* aItem: in: script line

     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution

     Errors/Exceptions: Leaves on error situations.

     Status: Proposal

-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteCommandL( TDesC& aObject, CStifItemParser* aItem)
    {
    _LIT( KErrMsgUnknownObjOrKeyword, "Unknow keyword/object %S" );
    
    TTestObjectBase* obj = iTestScripter->GetObject ( aObject );    
    if( obj == NULL )
        {
        iRunErrorMessage.Format( KErrMsgUnknownObjOrKeyword, &aObject );
        User::Leave( KErrGeneral );
        }

    if( iTestCaseResults.Count() == 0 )
        {
        // KErrNone is the default result expected if nothing else is given 
        User::LeaveIfError( iTestCaseResults.Append( KErrNone ) );
        }

    // OOM heap testing with FAILNEXT
    if( iHeapFailNext > 0 )
        {
        __TRACE( KPrint, ( 
            _L( "OOM FAILNEXT is used with count value: %d " ),
            iHeapFailNext) );
        User::__DbgSetAllocFail( RHeap::EUser, RHeap::EFailNext, iHeapFailNext  );
        }
    // OOM heap testing with SETFAIL
    if( iHeapSetFailValue > 0 )
        {
        __TRACE( KPrint, (
            _L("OOM SETFAIL is used with type:[%d] and value(rate):[%d]"),
            iHeapSetFailType, iHeapSetFailValue ) );
        User::__DbgSetAllocFail( RHeap::EUser, iHeapSetFailType, iHeapSetFailValue );
        }

    TInt commandResult = KErrNone;
    TRAP( commandResult, commandResult = obj->RunMethodL( *aItem ) );

    
    // Reset OOM macro immediately(Otherwise other allocations
    // will be blocked)
    User::__DbgSetAllocFail( RHeap::EUser, RHeap::ENone, 1 );

    // OOM test class's build block handling
    if( iTestScripter->iOOMIgnoreFailure )
        {
        __TRACE( KPrint, ( 
            _L( "----------< OOM related test information >----------" ) ) );
        if( iHeapFailNext > 0 )
            {
            __TRACE( KPrint, ( 
                _L( "'oomignorefailure' is:[ON] and 'failnextvalue' count is:[%d]" ),
                iHeapFailNext ) );
            }
        if( iHeapSetFailValue > 0 )
            {
            __TRACE( KPrint, ( 
                _L( "'oomignorefailure' is:[ON], 'setfailtype' type is:[%d] and value is:[%d]" ),
                iHeapSetFailType, iHeapSetFailValue ) );
            }
        __TRACE( KPrint, ( 
            _L( "Test class build block execution with result:[%d]" ),
            commandResult ) );
        __TRACE( KPrint, ( 
            _L( "----------------------------------------------------" ) ) );

        // Continue testing, despite the memory error situations
        iTestCaseResults.Reset();
        return ETrue; // Continue script executions
        }

    // Normal test class's build block handling
    if( iTestCaseResults.Find( commandResult ) < 0 ) 
        {
        __TRACE( KError, ( _L("Command for [%S] failed (%d)"), 
                &aObject, commandResult ));

        // Added more logging for this special case
        if( commandResult == KErrNone )
            {
            __TRACE( KError, (_L("Test fails with result KErrGeneral, because execution result was KErrNone and expected results given with 'allownextresult' were Symbian's error codes (<0)")));    
            }
           
        iTestScripter->UpdateTestCaseResult(commandResult, _L("CTestRunner::ExecuteCommandL returned error"));

        // Stops execution from CTestRunner::RunError
        User::Leave( KErrGeneral );
        }
        
    iTestCaseResults.Reset();
    // Reset test case allow result to CTestModuleIf side too. This is
    // used in TAL-TA5L macro handling.
    User::LeaveIfError( 
        iTestScripter->TestModuleIf().ResetAllowResult() );
        
    return ETrue;
    }
    
/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: SetRunnerActive

     Description: Set CTestRunner active and complete.

     Parameters:    None.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft

-------------------------------------------------------------------------------
*/
void CTestRunner::SetRunnerActive()
     {
     __TRACEFUNC();
     // Update state
     iState = ERunnerRunning;

     iStatus = KRequestPending;
     TRequestStatus* rs = &iStatus;
     SetActive();
     User::RequestComplete( rs, KErrNone );

     }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: OOMIgnoreFailure

     Description: Handles 'oomignorefailure' keyword parsing.
  
     Parameters: CStifItemParser* aItem: in: Item object for parsing.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestRunner::OOMIgnoreFailureL( CStifItemParser* aItem )
    {
    _LIT( KerrMsgOOMIgnoreFailureUnknownArgument, "OOMIgnoreFailure : Invalid argument %S" );
    _LIT( KerrMsgOOMIgnoreFailureNoArgument, "OOMIgnoreFailure : Parameter not defined" );
    
    __TRACEFUNC();

    TPtrC tmp;
    // Get result
    if( aItem->GetNextString( tmp ) == KErrNone )
        {                      
        if( tmp == _L("ON") || tmp == _L("on") )
            {
            __TRACE( KMessage, (_L("OOM related 'oomignorefailure': ON")));
            iTestScripter->iOOMIgnoreFailure = ETrue;
            }
        else if( tmp == _L("OFF") || tmp == _L("off") )
            {
            __TRACE( KMessage, (_L("OOM related 'oomignorefailure': OFF")));
            iTestScripter->iOOMIgnoreFailure = EFalse;
            }
        else
            {
            __TRACE( KError, 
                (_L("Unknown argument for 'oomignorefailure': [%S]"), 
                &tmp));
            iRunErrorMessage.Format( KerrMsgOOMIgnoreFailureUnknownArgument, &tmp );
            User::Leave( KErrArgument );
            }
        }
    else
        {
        __TRACE( KError, ( _L( "Unknown argument for 'oomignorefailure'" ) ) );
        iRunErrorMessage = KerrMsgOOMIgnoreFailureNoArgument;
        User::Leave( KErrArgument );
        }
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: OOMHeapFailNext

     Description: Handles 'oomheapfailnext' keyword parsing.
  
     Parameters: CStifItemParser* aItem: in: Item object for parsing.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestRunner::OOMHeapFailNextL( CStifItemParser* aItem )
    {
    _LIT( KErrMsgHeapFailNextNoArgument, "HeapFailNext : Parameters is not defined or has invalid value" );
    _LIT( KErrMsgHeapFailNextInvalidValue, "HeapFailNext : Argument value can't be <0" );
    __TRACEFUNC();

    TInt heapfailNext( 0 );
    // If LOOP_COUNTER is used the GetNextInt returns a correct
    // value(Because of the LOOP_COUNTER implementation).
    if( aItem->GetNextInt( heapfailNext ) == KErrNone )
        {
        if( heapfailNext < 0 )
        	{
        	__TRACE( KError, ( _L( "Argument value must be >0" ) ) );
            iRunErrorMessage = KErrMsgHeapFailNextInvalidValue;
            User::Leave( KErrArgument );        	
        	}
        // OOM heap's FAILNEXT range is from 1 to n
        iHeapFailNext = heapfailNext + 1;
        __TRACE( KMessage, ( _L( "'oomheapfailnext' count value(rate): %d" ),
            iHeapFailNext ) );
        }
    else
        {
        __TRACE( KError, ( _L( "Unknown argument for 'oomheapfailnext'" ) ) );
        iRunErrorMessage = KErrMsgHeapFailNextNoArgument;
        User::Leave( KErrArgument );
        }
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: OOMHeapSetFail

     Description: Handles 'oomheapsetfail' keyword parsing.
  
     Parameters: CStifItemParser* aItem: in: Item object for parsing.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestRunner::OOMHeapSetFailL( CStifItemParser* aItem )
    {
    _LIT( KErrMsgOOMHeapSetFailTypeNotDefined, "OOMHeapSetFail : Parameter type is not defined" );
    _LIT( KErrMsgOOMHeapSetFailRateNotDefined, "OOMHeapSetFail : Parameter rate is not defined" );
    _LIT( KErrMsgOOMHeapSetFailTypeInvalidValue, "OOMHeapSetFail : Parameter type has invalid value" );
    _LIT( KErrMsgOOMHeapSetFailRateInvalidValue, "OOMHeapSetFail : Parameter rate can't be <0" );
    __TRACEFUNC();

    TPtrC type;
    TInt value( 0 );

    // NOTE: If error in type or value parsing => return without changing
    //       iHeapSetFailType or iHeapSetFailValue valiables.

    // Get type
    if( aItem->GetNextString( type ) != KErrNone )
        {
        __TRACE( KError, ( 
            _L( "Unknown argument for 'oomheapsetfail' type" ) ) );
        iRunErrorMessage = KErrMsgOOMHeapSetFailTypeNotDefined;
        User::Leave( KErrArgument );
        }
    // Get value(rate)
    if( aItem->GetNextInt( value ) != KErrNone )
        {
        __TRACE( KError, ( 
            _L( "Unknown argument for 'oomheapsetfail' value(rate)" ) ) );
        iRunErrorMessage = KErrMsgOOMHeapSetFailRateNotDefined;
        User::Leave( KErrArgument );
        }    
    
    // All parsing operations are passed, get type and value

    // Get type
    if( type == _L("random") )
        {
        __TRACE( KMessage, (_L("'oomheapsetfail' type: random")));
        iHeapSetFailType = RHeap::ERandom;
        }
    else if( type == _L("truerandom") )
        {
        __TRACE( KMessage, (_L("'oomheapsetfail' type: truerandom")));
        iHeapSetFailType = RHeap::ETrueRandom;
        }
    else if( type == _L("deterministic") )
        {
        __TRACE( KMessage, (_L("'oomheapsetfail' type: deterministic")));
        iHeapSetFailType = RHeap::EDeterministic;
        }
    else if( type == _L("none") )
        {
        __TRACE( KMessage, (_L("'oomheapsetfail' type: none")));
        iHeapSetFailType = RHeap::ENone;
        }
    else if( type == _L("failnext") )
        {
        __TRACE( KMessage, (_L("'oomheapsetfail' type: failnext")));
        iHeapSetFailType = RHeap::EFailNext;
        }
    else
        {
        __TRACE( KError, 
            ( _L( "Unknown argument for 'oomheapsetfail' type: [%S]" ),
            &type ) );
        iRunErrorMessage = KErrMsgOOMHeapSetFailTypeInvalidValue;
        User::Leave( KErrArgument );
        }

    if( value < 0 )
    	{
        __TRACE( KError, _L( "'oomheapsetfail' value(rate) can't be <0" ) );    	    	
        iRunErrorMessage = KErrMsgOOMHeapSetFailRateInvalidValue;
        User::Leave( KErrArgument );
    	}
    // Get value
    // OOM heap's SETFAIL range is from 1 to n
    iHeapSetFailValue = value + 1;
    __TRACE( KMessage, ( 
        _L( "'oomheapsetfail' value(rate): %d" ), iHeapSetFailValue ) );

    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: OOMHeapToNormal

     Description: Initialize all OOM related variables.
  
     Parameters: None.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestRunner::OOMHeapToNormal()
    {
    __TRACEFUNC();

    if( iTestScripter )
        {
        // This should initialize also
        iTestScripter->iOOMIgnoreFailure = EFalse;
        }

    iHeapFailNext = 0;
    iHeapSetFailType = RHeap::ENone;
    iHeapSetFailValue = 0;

    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: TestInterference

     Description: Starts test interference.
  
     Parameters: CStifItemParser* aItem: in: Item object for parsing.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestRunner::TestInterferenceL( CStifItemParser* aItem )
    {
    _LIT( KErrMsgTestInterferenceNameNotDefined, "TestInterference : Parameter name is not defined" );
    _LIT( KErrMsgTestInterferenceCommandNotDefined, "TestInterference : Parameter command is not defined" );
    _LIT( KErrMsgTestInterferenceCommandInvalidValue, "TestInterference : Parameter command has invalid value" );
    __TRACEFUNC();
    TPtrC name;
    TPtrC command;

    // Get name
    if( aItem->GetNextString( name ) != KErrNone )
        {
        __TRACE( KError, ( 
            _L( "Unknown argument for 'testinterference' name" ) ) );
        iRunErrorMessage = KErrMsgTestInterferenceNameNotDefined;
        User::Leave( KErrArgument ); // Error in parsing => Leave
        }
    // Get command
    if( aItem->GetNextString( command ) != KErrNone )
        {
        __TRACE( KError, ( 
            _L( "Unknown argument for 'testinterference' command" ) ) );
        iRunErrorMessage = KErrMsgTestInterferenceCommandNotDefined;
        User::Leave( KErrArgument ); // Error in parsing => Leave
        }

    if( command == _L( "start" ) )
        {
        __TRACE( KMessage, ( _L( "'testinterference' called, starts test interference") ) );
        StartInterferenceL( name, aItem );
        }
    else if( command == _L( "stop" ) )
        {
        __TRACE( KMessage, ( _L( "'testinterference' called, stops and releases test interference") ) );
        StopInterferenceL( name );
        }
    else
        {
        __TRACE( KError, ( 
            _L( "Unknown command for 'testinterference'[%S]" ), &command ) );
        iRunErrorMessage = KErrMsgTestInterferenceCommandInvalidValue;
        User::Leave( KErrArgument ); // Error in parsing => Leave
        }

    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: StartInterference

     Description: 
  
     Parameters: CStifItemParser* aItem: in: Item object for parsing.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestRunner::StartInterferenceL( TDesC& aName, CStifItemParser* aItem )
    {
    _LIT( KErrMsgStartInterferenceCategoryNotDefined, "TestInterference : Parameter category is not defined" );
    _LIT( KErrMsgStartInterferenceCategoryInvalidValue, "TestInterference : Parameter category has invalid value" );
    _LIT( KErrMsgStartInterferenceTypeNotDefined, "TestInterference : Parameter type is not defined" );
    _LIT( KErrMsgStartInterferenceTypeInvalidValue, "TestInterference : Parameter type has invalid value" );
    _LIT( KErrMsgStartInterferenceIdleNotDefined, "TestInterference : Parameter idle is not defined or has invalid value" );
    _LIT( KErrMsgStartInterferenceIdleInvalidValue, "TestInterference : Parameter idle has invalid value" );
    _LIT( KErrMsgStartInterferenceActiveNotDefined, "TestInterference : Parameter active is not defined or has invalid value" );
    _LIT( KErrMsgStartInterferenceActiveInvalidValue, "TestInterference : Parameter active has invalid value" );
    
    __TRACEFUNC();
    TPtrC category;
    TPtrC type;
    TInt idle( 0 );
    TInt active( 0 );
    TInt priority( 0 );

    // Get category
    if( aItem->GetNextString( category ) != KErrNone )
        {
        __TRACE( KError, ( 
            _L( "Unknown argument for 'testinterference' category" ) ) );
        iRunErrorMessage = KErrMsgStartInterferenceCategoryNotDefined;
        User::Leave( KErrArgument ); // Error in parsing => Leave
        }
    // Get type
    if( aItem->GetNextString( type ) != KErrNone )
        {
        __TRACE( KError, ( 
            _L( "Unknown argument for 'testinterference' type" ) ) );
        iRunErrorMessage = KErrMsgStartInterferenceTypeNotDefined;
        User::Leave( KErrArgument ); // Error in parsing => Leave
        }

    TPtrC timeStr;
    TReal time;
    // Get idle time
    if( aItem->GetNextString( timeStr ) != KErrNone )
        {        
        __TRACE( KError, ( 
            _L( "Unknown argument for 'testinterference' idle" ) ) );
        iRunErrorMessage = KErrMsgStartInterferenceIdleNotDefined;
        User::Leave( KErrArgument ); // Error in parsing => Leave
        }
    if ( TLex( timeStr ).Val( time, '.' ) != KErrNone )
    	{
        __TRACE( KError, ( 
            _L( "Unknown argument for 'testinterference' idle" ) ) );
        iRunErrorMessage = KErrMsgStartInterferenceIdleNotDefined;
        User::Leave( KErrArgument ); // Error in parsing => Leave    	
    	}
    // Convert idle time from milli to micro seconds
   	idle = static_cast<TInt>( time * 1000.0);

    if( idle < 0 )
    	{
        __TRACE( KError, ( 
                _L( "Argument 'testinterference' idle can't be <0" ) ) );
    	iRunErrorMessage = KErrMsgStartInterferenceIdleInvalidValue;
    	User::Leave( KErrArgument );
    	}
   // Get active time
    if( aItem->GetNextString( timeStr ) != KErrNone )
        {
        __TRACE( KError, ( 
            _L( "Unknown argument for 'testinterference' active" ) ) );
        iRunErrorMessage = KErrMsgStartInterferenceActiveNotDefined;
        User::Leave( KErrArgument ); // Error in parsing => Leave
        }
    if ( TLex( timeStr ).Val( time, '.' ) != KErrNone )
    	{
        __TRACE( KError, ( 
            _L( "Unknown argument for 'testinterference' idle" ) ) );
        iRunErrorMessage = KErrMsgStartInterferenceIdleNotDefined;
        User::Leave( KErrArgument ); // Error in parsing => Leave    	
    	}
    
    // Convert active time from milli to micro seconds
   	active = static_cast<TInt>( time * 1000.0 );
   	
    if( active < 0 )
    	{
        __TRACE( KError, ( 
                _L( "Argument 'testinterference' active can't be <0" ) ) );
    	iRunErrorMessage = KErrMsgStartInterferenceActiveInvalidValue;
    	User::Leave( KErrArgument );
    	}
    
   // Get priority
    if( aItem->GetNextInt( priority ) != KErrNone )
        {
        // Log information only do not return. Priority value is optional.
        __TRACE( KInit, ( 
            _L( "Unknown argument for 'testinterference' priority or value not given, default priority will be used" ) ) );
        }    

    MSTIFTestInterference* interference = NULL;

    // Get category
    if( category == _L("activeobject") )
        {
        interference = MSTIFTestInterference::NewL( iTestScripter,
                            MSTIFTestInterference::EActiveObject );
        }
    else if( category == _L("thread") )
        {
        interference = MSTIFTestInterference::NewL( iTestScripter,
                            MSTIFTestInterference::EThread );
        }
    else
        {
        __TRACE( KError, ( 
            _L( "Unknown argument for 'testinterference' category[%S]" ), &category ) );
        iRunErrorMessage = KErrMsgStartInterferenceCategoryInvalidValue;
        User::Leave( KErrArgument ); // Error in parsing => Leave
        }

    // Create object that include test interference information and append this
    // to array.
    TTestInterference* object = new (ELeave) TTestInterference();
    object->iName = aName;
    object->iInterference = interference;

    // Array for handling test interference between different objects
    TInt ret = iTestInterferenceArray.Append( object );
    if( ret != KErrNone )
        {
        delete object;
        __TRACE( KError, ( 
            _L( "CTestRunner::StartInterference: iTestInterferenceArray.Append fails:[%d]" ), ret ) );
        User::Leave( ret );
        }

    // Set priority if user given
    if( priority != 0 )
        {
        interference->SetPriority( priority );
        }

    // Get type
    if( type == _L("cpuload") )
        {
        interference->StartL( MSTIFTestInterference::ECpuLoadMicroSeconds, idle , active );
        }
    else if( type == _L("filesystemreadc") )
        {
        interference->StartL( MSTIFTestInterference::EFileSystemReadCMicroSeconds, idle , active );
        }
    else if( type == _L("filesystemreadd") )
        {
        interference->StartL( MSTIFTestInterference::EFileSystemReadDMicroSeconds, idle , active );
        }
    else if( type == _L("filesystemreade") )
        {
        interference->StartL( MSTIFTestInterference::EFileSystemReadEMicroSeconds, idle , active );
        }
    else if( type == _L("filesystemreadz") )
        {
        interference->StartL( MSTIFTestInterference::EFileSystemReadZMicroSeconds, idle , active );
        }
    else if( type == _L("filesystemwritec") )
        {
        interference->StartL( MSTIFTestInterference::EFileSystemWriteCMicroSeconds, idle , active );
        }
    else if( type == _L("filesystemwrited") )
        {
        interference->StartL( MSTIFTestInterference::EFileSystemWriteDMicroSeconds, idle , active );
        }
    else if( type == _L("filesystemwritee") )
        {
        interference->StartL( MSTIFTestInterference::EFileSystemWriteEMicroSeconds, idle , active );
        }
    else if( type == _L("filesystemfillandemptyc") )
        {
        interference->StartL( MSTIFTestInterference::EFileSystemFillAndEmptyCMicroSeconds, idle , active );
        }
    else if( type == _L("filesystemfillandemptyd") )
        {
        interference->StartL( MSTIFTestInterference::EFileSystemFillAndEmptyDMicroSeconds, idle , active );
        }
    else if( type == _L("filesystemfillandemptye") )
        {
        interference->StartL( MSTIFTestInterference::EFileSystemFillAndEmptyEMicroSeconds, idle , active );
        }
    else
        {
        __TRACE( KError, ( 
            _L( "Unknown argument for 'testinterference' type[%S]" ), &type ) );
        iRunErrorMessage = KErrMsgStartInterferenceTypeInvalidValue;
        User::Leave( KErrArgument ); // Error in parsing => Leave
        }

    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: StopInterference

     Description: Stops test interference.
  
     Parameters: TDesC& aName: in: Indicates right test interference object.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestRunner::StopInterferenceL( TDesC& aName )
    {    
    _LIT( KErrMsgStopInterference, "TestInterference : testinterference %S was not start" );
    __TRACEFUNC();
    
    TInt count = iTestInterferenceArray.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iTestInterferenceArray[i]->iName == aName )
            {
            // Found test module, return description
            iTestInterferenceArray[i]->iInterference->Stop();
            // Delete data
            delete iTestInterferenceArray[i];
            // Remove pointer to deleted data(Append())
            iTestInterferenceArray.Remove( i );
            // iTestMeasurementArray can contain only one type of measurement
            // so we can break when type is removed.
            return;
            }
        }
    __TRACE( KError, ( 
            _L( "'testinterference' type[%S] was not start" ), &aName ) );
    iRunErrorMessage.Format( KErrMsgStopInterference, &aName );
    User::Leave( KErrArgument ); // Error in parsing => Leave
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: MeasurementL

     Description: Starts test measurement.
  
     Parameters: CStifItemParser* aItem: in: Item object for parsing.

     Return Values: None.

     Errors/Exceptions: Leaves if StartBappeaMeasurementL() fails.
                        Leaves if StopBappeaMeasurementL() fails.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestRunner::MeasurementL( CStifItemParser* aItem )
    {
    _LIT( KErrMsgMeasurementCommandNotDefined, "TestMeasurement : Parameter command is not defined" );
    _LIT( KErrMsgMeasurementTypeNotDefined, "TestMeasurement : Parameter type is not defined" );
    _LIT( KErrMsgMeasurementUnknownType, "TestMeasurement : Unknown measurement type %S" );
    _LIT( KErrMsgMeasurementUnknownCommand, "TestMeasurement : Unknown command %S" );
    __TRACEFUNC();
    TPtrC type;
    TPtrC command;

    // Get command
    if( aItem->GetNextString( command ) != KErrNone )
        {
        __TRACE( KError, ( 
            _L( "Unknown argument for 'measurement' command" ) ) );
        iRunErrorMessage = KErrMsgMeasurementCommandNotDefined;
        User::Leave( KErrArgument ); // Error in parsing => Leave
        }
    // Get name
    if( aItem->GetNextString( type ) != KErrNone )
        {
        __TRACE( KError, ( 
            _L( "Unknown argument for 'measurement' type" ) ) );
        iRunErrorMessage = KErrMsgMeasurementTypeNotDefined;
        User::Leave( KErrArgument ); // Error in parsing => Leave
        }

    // Verify measurement type
    if( !( type == KParamMeasurement01 ||
                type == KParamMeasurement02 ||
                type == KParamMeasurement03 ||
                type == KParamMeasurement04 ||
                type == KParamMeasurement05 ||
                type == KParamMeasurementBappea ) )

        {
        __TRACE( KError, ( 
            _L( "Unknown measurement type:[%S]" ), &type ) );
        iRunErrorMessage.Format( KErrMsgMeasurementUnknownType, &type );
        User::Leave( KErrArgument ); // Error in types => Leave
        }

    // Verify command
   if( command == _L( "start" ) )
        {
        // START measurement's process
        __TRACE( KMessage, ( _L( "Start 'measurement' with '%S'"), &type ) );
        StartMeasurementL( type, aItem );
        }
    else if( command == _L( "stop" ) )
        {
        // STOP measurement's process
        __TRACE( KMessage, ( _L( "'Stop 'measurement' with '%S'"), &type ) );
        StopMeasurementL( type );
        }
    else
        {
        __TRACE( KError, ( 
            _L( "Unknown command for 'measurement' command:[%S] or type:[%S]" ), &command, &type ) );
        iRunErrorMessage.Format( KErrMsgMeasurementUnknownCommand, &command );
        User::Leave( KErrArgument ); // Error in commands => Leave
        }

    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: StartMeasurementL

     Description: Start measurement
  
     Parameters: const TDesC& aType: in: Plugin type.
                 CStifItemParser* aItem: in: Item object for parsing.

     Return Values: None.

     Errors/Exceptions: Leaves is measurement operation fails.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestRunner::StartMeasurementL( const TDesC& aType,
                                           CStifItemParser* aItem )
    {
    _LIT( KErrMsgMeasurementUnknownPlugin, "Measurement : Unknown measurement plugin %S" );
    _LIT( KErrMsgMeasurementStartFail, "Measurement : Measurement start fails" );
    __TRACEFUNC();

    CSTIFTestMeasurement* testMeasurement = NULL;

    // Get Measurement configuration info
    TPtrC configurationInfo( KNullDesC() );
    if( aItem->Remainder( configurationInfo ) != KErrNone )
        {
        __TRACE( KInit, ( 
            _L( "Using default path and file name for measurement configure" ) ) );
        } 

    if( aType == KParamMeasurement01 )
        {
        testMeasurement = CSTIFTestMeasurement::NewL( 
                    iTestScripter, 
                    CSTIFTestMeasurement::KStifMeasurementPlugin01,
                    configurationInfo );
        }
    else if( aType == KParamMeasurement02 )
        {
        testMeasurement = CSTIFTestMeasurement::NewL( 
                    iTestScripter, 
                    CSTIFTestMeasurement::KStifMeasurementPlugin02,
                    configurationInfo );
        }
    else if( aType == KParamMeasurement03 )
        {
        testMeasurement = CSTIFTestMeasurement::NewL( 
                    iTestScripter, 
                    CSTIFTestMeasurement::KStifMeasurementPlugin03,
                    configurationInfo );
        }
    else if( aType == KParamMeasurement04 )
        {
        testMeasurement = CSTIFTestMeasurement::NewL( 
                    iTestScripter, 
                    CSTIFTestMeasurement::KStifMeasurementPlugin04,
                    configurationInfo );
        }
    else if( aType == KParamMeasurement05 )
        {
        testMeasurement = CSTIFTestMeasurement::NewL( 
                    iTestScripter, 
                    CSTIFTestMeasurement::KStifMeasurementPlugin05,
                    configurationInfo );
        }
    else if( aType == KParamMeasurementBappea )
        {
        testMeasurement = CSTIFTestMeasurement::NewL( 
                    iTestScripter, 
                    CSTIFTestMeasurement::KStifMeasurementBappeaProfiler,
                    configurationInfo );
        }
    else
        {
        __TRACE( KError, ( _L( "Unknown plugin[%S] for 'measurement'" ), &aType ) );
        iRunErrorMessage.Format( KErrMsgMeasurementUnknownPlugin, &aType );        
        User::Leave( KErrArgument );
        }

    // Start test measurement
    TInt start_ret( KErrNone );
    start_ret = testMeasurement->Start();
    if( start_ret != KErrNone )
        {
        delete testMeasurement;
        //CleanupStack::PopAndDestroy( setting_buf );
        __TRACE( KError, ( 
            _L( "CTestRunner::StartMeasurementL(): Measurement Start() fails:[%d]" ), start_ret ) );
        iRunErrorMessage = KErrMsgMeasurementStartFail;
        User::Leave( start_ret );
        }

    TTestMeasurement* object = new (ELeave) TTestMeasurement();
    object->iName = aType;
    object->iMeasurement = testMeasurement;

    // Array for handling test measurement between different objects
    TInt ret = iTestMeasurementArray.Append( object );
    if( ret != KErrNone )
        {
        delete object;
        __TRACE( KError, ( 
            _L( "CTestRunner::StartMeasurementL(): iTestMeasurementArray.Append fails:[%d]" ), ret ) );
        User::Leave( ret );
        }

    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: StopMeasurementL

     Description: Stops test measurement.
  
     Parameters: None.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestRunner::StopMeasurementL( const TDesC& aType )
    {
    _LIT( KErrMsgMeasurementNotStarted, "Measurement : Measurement %S was not start" );
    __TRACEFUNC();

    TInt count = iTestMeasurementArray.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iTestMeasurementArray[i]->iName == aType )
            {
            // Found measurement module, stop
            iTestMeasurementArray[i]->iMeasurement->Stop();
            // Delete data
            delete iTestMeasurementArray[i];
            // Remove pointer to deleted data(Append())
            iTestMeasurementArray.Remove( i );
            // iTestMeasurementArray can contain only one type of measurement
            // so we can break when type is removed.
            return;
            }
        }

    __TRACE( KError, ( 
            _L( "CTestRunner::StopMeasurementL(): Measurement %S was not start" ), &aType ) );
    iRunErrorMessage.Format( KErrMsgMeasurementNotStarted, &aType  );
    User::Leave( KErrArgument );
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: AddTestCaseResultL

     Description: Adds new test case result. Used with 'allownextresult'
                  and 'allowerrorcodes' keywords.
  
     Parameters: CStifItemParser* aItem: in: Item object for parsing.

     Return Values: None.

     Errors/Exceptions: Leaves if iTestCaseResults.Append fails.
                        Leaves if aItem->GetNextInt() fails.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestRunner::AddTestCaseResultL( CStifItemParser* aItem )
    {
    _LIT( KErrMsgAllowNextResultInvalidValue, "No expected result value given or value has invalid format" );
    TInt result;

    TPtrC codeBuf;
    TBool found = EFalse;
    while( aItem->GetNextString( codeBuf ) == KErrNone )
    	{
    	TLex codeParser( codeBuf );
    	if ( codeParser.Val( result ) != KErrNone )
    		{
            __TRACE( KError, (_L("ExecuteLineL: No expected result value given")));
            iRunErrorMessage = KErrMsgAllowNextResultInvalidValue;
            User::Leave( KErrArgument );    		
    		}
    	else
    		{
    	    User::LeaveIfError( iTestCaseResults.Append( result ) );
    	    // Set test case allow result to CTestModuleIf side too. This is
    	    // used in TAL-TA5L macro handling.
    	    User::LeaveIfError( 
    	        iTestScripter->TestModuleIf().SetAllowResult( result ) );
    	    found = ETrue;
    		}
    	}

    if ( !found )
    	{
        __TRACE( KError, (_L("ExecuteLineL: No expected result value given")));
        iRunErrorMessage = KErrMsgAllowNextResultInvalidValue;
        User::Leave( KErrArgument );
    	}    
    }


/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CTestContinue class
    member functions. CTestContinue handles ContinueScript calls from 
    TestScripter.

-------------------------------------------------------------------------------
*/
// MACROS
#ifdef LOGGER
#undef LOGGER
#endif
#define LOGGER iTestScripter->iLog

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CTestContinue

     Method: CTestContinue

     Description: Default constructor

     Parameters: CTestRunner* aTestRunner: in: Backpointer to CTestRunner

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CTestContinue::CTestContinue( CTestScripter* aTestScripter,
                              TTestObject* aObject ): 
    // Executed with lowest priority, must be lower than CTesRunner priority 
    CActive(  CActive::EPriorityLow ),
    iTestScripter( aTestScripter ),
    iObject( aObject )
    {
    CActiveScheduler::Add( this );
    __TRACEFUNC();
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestContinue

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Parameters:    None

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestContinue::ConstructL()
    {
    
    iObject->iScript->EnableSignal( iStatus );
    SetActive();
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestContinue

     Method: NewL

     Description: Two-phased constructor.
          
     Parameters: CTestRunner* aTestRunner: in: Backpointer to CTestRunner

     Return Values: CTestContinue*: new object

     Errors/Exceptions: Leaves if new or ConstructL leaves

     Status: Draft
    
-------------------------------------------------------------------------------
*/

CTestContinue* CTestContinue::NewL( CTestScripter* aTestScripter,
                                    TTestObject* aObject )
    {
    CTestContinue* self = 
        new (ELeave) CTestContinue( aTestScripter, aObject );
     
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestContinue

     Method: ~CTestContinue

     Description: Destructor
     
     Parameters:    None

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/     

CTestContinue::~CTestContinue()
    {
    __TRACEFUNC();
    Cancel();
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestContinue

     Method: RunL

     Description: Derived from CActive, handles testcase execution.

     Parameters:    None.

     Return Values: None.

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestContinue::RunL()
    {

    __TRACEFUNC();
    __TRACE( KMessage, (_L("CTestContinue::RunL: [%d] "), iStatus.Int() ));

    if( iObject->Signal() ) 
        {
        // If OOM testing is ongoing ignore result check(given by user).
        if( !iTestScripter->iOOMIgnoreFailure )
            {
            // Erronous case RunError will called and test handling continue 
            // from there.
            if( iTestScripter->TestRunner().TestCaseResults().Count() == 0 )
                {
                // KErrNone is the default result expected 
                // if nothing else is given 
                User::LeaveIfError( iTestScripter->TestRunner().
                    TestCaseResults().Append( KErrNone ) );
                }
            
            if( iTestScripter->TestRunner().TestCaseResults().
                    Find( iStatus.Int() ) < 0 ) 
                {
                __TRACE( KError, ( _L("Command for [%S] failed (%d)"), 
                    &iObject->ObjectId(), iStatus.Int() ));


                if( iStatus.Int() == KErrNone )
                    {
                    User::Leave( KErrGeneral );
                    }
                else
                    {
                    User::Leave( iStatus.Int() );
                    }
                }
            }
        else
            {
            __TRACE( KMessage, ( 
                _L( "OOM test: 'oomignorefailure' is ON, signal result[%d] ignored" ),
                iStatus.Int() ));
            }

        iTestScripter->TestRunner().TestCaseResults().Reset();
        
         __TRACE( KMessage, (_L("CTestContinue::RunL: Set runner active ")));
        iTestScripter->iTestRunner->SetRunnerActive();   
        }
    else
        {
        // Signal called from test side but 'waittestclass' not yet processed
        iObject->iAsyncResult = iStatus.Int();
        }        
        
    iObject->iScript->EnableSignal( iStatus );
    SetActive();
    
    }
     
/*
-------------------------------------------------------------------------------

     Class: CTestContinue

     Method: DoCancel

     Description: Derived from CActive handles the Cancel

     Parameters:    None.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestContinue::DoCancel()
    {

    __TRACEFUNC();
    __TRACE( KMessage, (_L("CTestContinue::DoCancel")));

    iObject->iScript->CancelSignal();
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestContinue

     Method: RunError

     Description: Derived from CActive handles the Cancel

     Parameters:    None.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt CTestContinue::RunError( TInt aError )
    {

    // Return error from here, if none given from execution
    if( iTestScripter->iResult.iResult == KErrNone )
        {
        iTestScripter->UpdateTestCaseResult(aError, _L("CTestContinue::RunError"));
        }
    CActiveScheduler::Current()->Stop();
     
    return KErrNone;
    
    }
    
/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CDefinedValue class
    member functions. 
-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================
/*
-------------------------------------------------------------------------------

     Class: CDefinedValue

     Method: CDefinedValue

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.
     
     Parameters: None

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CDefinedValue::CDefinedValue()
    {
    }
     
/*
-------------------------------------------------------------------------------

     Class: CDefinedValue

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.

     Parameters:    TDesC& aName: in: Define name
                    TDesC& aValue: in: Define value

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CDefinedValue::ConstructL( TDesC& aName, TDesC& aValue )
    {    

    iNameBuf = aName.AllocLC();
    iName.Set( iNameBuf->Des() );
    iValueBuf = aValue.AllocLC();
    iValue.Set( iValueBuf->Des() );
    CleanupStack::Pop( iValueBuf );
    CleanupStack::Pop( iNameBuf );

    }

/*
-------------------------------------------------------------------------------

     Class: CDefinedValue

     Method: NewL

     Description: Two-phased constructor.
          
     Parameters:    TDesC& aName: in: Define name
                    TDesC& aValue: in: Define value

     Return Values: CDefinedValue*: new object

     Errors/Exceptions: Leaves if new or ConstructL leaves.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CDefinedValue* CDefinedValue::NewL( TDesC& aName, TDesC& aValue )
    {

    CDefinedValue* self = new (ELeave) CDefinedValue();
     
    CleanupStack::PushL( self );
    self->ConstructL( aName, aValue );
    CleanupStack::Pop();

    return self;

    }    
    
/*
-------------------------------------------------------------------------------

     Class: CDefinedValue

     Method: ~CDefinedValue

     Description: Destructor
     
     Parameters:    None

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/     
CDefinedValue::~CDefinedValue()
    {
    
    delete iValueBuf;
    delete iNameBuf;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CDefinedValue

     Method: SetValueL

     Description: Set new define value

     Parameters:    TDesC& aValue: in: Define value

     Return Values: None

     Errors/Exceptions: Leaves on error.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CDefinedValue::SetValueL( TDesC& aValue )
    {
    delete iValueBuf;
    iValueBuf = 0;
    iValueBuf = aValue.AllocLC();
    iValue.Set( iValueBuf->Des() );
    CleanupStack::Pop( iValueBuf );
    
    }

    
/*
-------------------------------------------------------------------------------

     Class: CDefinedValue

     Method: Name

     Description: Returns define name.

     Parameters:  None

     Return Values: TDesC: Define name

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TDesC& CDefinedValue::Name()
    { 
    return iName; 
    }
        
/*
-------------------------------------------------------------------------------

     Class: CDefinedValue

     Method: Value

     Description: Returns define value.

     Parameters:  None

     Return Values: TDesC: Define value

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TDesC& CDefinedValue::Value()
    { 
    return iValue; 
    }
    

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of TTestObjectBase class
    member functions. 
-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================
/*
-------------------------------------------------------------------------------

     Class: TTestObjectBase

     Method: TTestObjectBase

     Description: Constructor

     Parameters: None

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/    
TTestObjectBase::TTestObjectBase( TObjectType aType ) : 
    iAsyncResult( KErrNone ),
    iType( aType )
    {
    
    RDebug::Print( _L("TTestObjectBase::TTestObjectBase") );
    iName.Zero(); 
    
    }
    
    /*
-------------------------------------------------------------------------------

     Class: TTestObjectBase

     Method: ~TTestObjectBase

     Description: Destructor

     Parameters: None

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/    
TTestObjectBase::~TTestObjectBase()
    {
    
    RDebug::Print( _L("TTestObjectBase::~TTestObjectBase") );

    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of TTestObject class
    member functions. 
-------------------------------------------------------------------------------
*/
// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: TTestObject

     Method: TTestObject

     Description: Constructor

     Parameters: None

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/    
TTestObject::TTestObject() :
    TTestObjectBase( EObjectNormal ),
    iScript(0),
    iContinue(0),
    iCount(0)
    {

    RDebug::Print( _L("TTestObject::TTestObject") );

    }

/*
-------------------------------------------------------------------------------

     Class: TTestObject

     Method: ~TTestObject

     Description: Destructor

     Parameters: None

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TTestObject::~TTestObject()
    { 
    
    RDebug::Print( _L("TTestObject::~TTestObject") );
    delete iContinue; 
    delete iScript;
    
    }
        
/*
-------------------------------------------------------------------------------

     Class: TTestObject

     Method: RunMethodL

     Description: Run specified method from testclass.

     Parameters: CStifItemParser* aItem: in: itemparser

     Return Values: Symbian OS error code

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt TTestObject::RunMethodL( CStifItemParser& aItem )
    { 
    
    return iScript->RunMethodL( aItem ); 
    
    }
    
/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of TTestObjectKernel class
    member functions. 
-------------------------------------------------------------------------------
*/
// ================= MEMBER FUNCTIONS =========================================


/*
-------------------------------------------------------------------------------

     Class: TTestObjectKernel

     Method: TTestObjectKernel

     Description: Constructor

     Parameters: None

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/    
TTestObjectKernel::TTestObjectKernel() : 
    TTestObjectBase( EObjectKernel )
    {
    
    RDebug::Print( _L("TTestObjectKernel::TTestObjectKernel") );
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: TTestObjectKernel

     Method: ~TTestObjectKernel

     Description: Destructor

     Parameters: None

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TTestObjectKernel::~TTestObjectKernel()
    { 
    
    RDebug::Print( _L("TTestObjectKernel::~TTestObjectKernel") );
    
    iTestClass.Close(); 
    User::FreeLogicalDevice( iLddName );
    
    }
   
/*
-------------------------------------------------------------------------------

     Class: TTestObjectKernel

     Method: RunMethodL

     Description: Run specified method from kernel testclass.

     Parameters: CStifItemParser& aItem: in: itemparser

     Return Values: Symbian OS error code

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt TTestObjectKernel::RunMethodL( CStifItemParser& aItem )
    { 
    
    TPtrC method;
    TPtrC params;
    
    User::LeaveIfError( aItem.GetNextString( method ) );
    TInt ret = aItem.Remainder( params );
	if ( ret != KErrNone )
		{
		RDebug::Print( _L("No parameters given in test class method call") );
		}
    
    TInt result;
    TMethodResultDes resultDes;
          
    // Need to change descriptors from 16bit to 8bit for EKA2 kernel
    // because STIF Parser doesn't support 8bit parsing
    HBufC8 * myBuf1 = HBufC8::NewL( method.Length() );
    TPtr8 met8 = myBuf1->Des();   
    met8.Copy( method );
        
    HBufC8 * myBuf2 = HBufC8::NewL( params.Length() );
    TPtr8 par8 = myBuf2->Des();   
    par8.Copy( params );
    
    ret =  iTestClass.RunMethod( met8, par8, result, resultDes );

    delete myBuf1;
    delete myBuf2;
    
    if( ret != KErrNone )
        {
        return ret;
        }
    
    return result;                             
                                     
    };
    
// ================= OTHER EXPORTED FUNCTIONS =================================

/*
-------------------------------------------------------------------------------
    
     Function: LibEntryL

     Description: Polymorphic Dll Entry Point

     Parameters:    None.

     Return Values: CTestScripter*: pointer to new CTestScripter

     Errors/Exceptions: Leaves if NewL leaves.

     Status: Draft
     
-------------------------------------------------------------------------------
*/

EXPORT_C CTestScripter* LibEntryL()
    {
    return CTestScripter::NewL();
    
    }

// End of File
