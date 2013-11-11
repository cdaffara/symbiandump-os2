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
* Description: This module contains implementation of CUIStore 
* and CModule class member functions.
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32svr.h>
#include <f32file.h>
#include <e32uid.h>
#include <collate.h>

#include <StifLogger.h>

#include "Logging.h"
#include <stifinternal/UIStore.h>
#include <stifinternal/UIEngine.h>
#include <stifinternal/UIStoreContainer.h>
#include "UIStorePopup.h"
#include <stifinternal/UIEngineContainer.h>


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
//@spe _LIT(KNameTxt,"TEST FRAMEWORK");
//@spe _LIT(KNameBase,"BASE");
_LIT( KUIStore, "CUiStore" );
_LIT( KUIStoreIf, "CUiStoreIf" );
//_LIT( KUIStoreDefaultDir, "C:\\TestFramework\\" );

_LIT( KUIStoreSetStart, "[TestSetStart]" );
_LIT( KUIStoreSetEnd, "[TestSetEnd]" );
_LIT( KUIStoreSetCaseStart, "[TestSetCaseStart]" );
_LIT( KUIStoreSetCaseEnd, "[TestSetCaseEnd]" );
_LIT( KUIStoreCaseModuleName, "ModuleName=" );
_LIT( KUIStoreTestCaseTitle, "Title=" );
_LIT( KUIStoreTestCaseFile, "TestCaseFile="); 
_LIT( KUIStoreTestCaseNum, "TestCaseNum=");
_LIT( KUIStoreCaseExpectedResult, "ExpectedResult=");
_LIT( KUIStoreCasePriority, "Priority="); 
_LIT( KUIStoreCaseTimeout, "Timeout=");
_LIT( KUIStoreLastStartedCaseIndex, "LastStartedCaseIndex=");


_LIT( KUIStoreDefaultRebootFile, "TestFrameworkUIReboot.txt" );
_LIT( KUIStoreStartTest, "[StartTestCase]" );
_LIT( KUIStoreStartTestEnd, "[StartTestCaseEnd]" );

_LIT( KUIStoreCaseStatus, "TestCaseStatus=");
_LIT( KUIStoreCaseExecutionResult, "TestCaseExecutionResult=");
_LIT( KUIStoreCaseResult, "TestCaseResult=");
_LIT( KUIStoreCaseStartTime, "TestCaseStartTime=");
_LIT( KUIStoreCaseEndTime, "TestCaseEndTime=");

const TInt KRcpHeaderLen = 16;        
const TInt KFixedStartedCaseIndex = 10;

// MACROS
#ifdef LOGGER
#undef LOGGER
#endif
#define LOGGER iLogger

// LOCAL CONSTANTS AND MACROS
static const TUid KUidTestModule = { 0x101FB3E7 };

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// ==================== LOCAL FUNCTIONS =======================================

// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: NewL

    Description: Construct the CUIStore class

    Parameters: None

    Return Values: CUIStore*                    New object

    Errors/Exceptions: Leaves if memory allocation fails or
                       ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIStore* CUIStore::NewL( CUIStoreIf* aUIStoreIf )
    {
    
    CUIStore* self = new ( ELeave ) CUIStore( aUIStoreIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;

    }


/*
-------------------------------------------------------------------------------

    Class: CUIStore

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
void CUIStore::ConstructL( )
    {
    }


/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: CUIStore

    Description: Constructor.
    Initialize non-zero member variables.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIStore::CUIStore( CUIStoreIf* aUIStoreIf ):
    iUIStoreIf( aUIStoreIf ),
    iUpdateNeeded( ETrue ),
    iActivePopupPriority( EPopupPriorityLow )
    {
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: ~CUIStore

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIStore::~CUIStore( )
    {	

    iFileList.ResetAndDestroy();
    iFileList.Close(); 
    iStartedTestSets.ResetAndDestroy();
    iStartedTestSets.Close();  

    iStartedTestCases.ResetAndDestroy();
    iStartedTestCases.Close();
    
    iTestSets.ResetAndDestroy();
    iTestSets.Close();  

    iTestCases.ResetAndDestroy();
    iTestCases.Close();
    
    iPopups.ResetAndDestroy();
    iPopups.Close();
        
    delete iUIEngine;    
    iUIEngine = 0;

    }
/*
-------------------------------------------------------------------------------

    Class: CStifTFwIf

    Method: Open

    Description: Open test engine.

    Parameters: TDesC& aTestFrameworkIni: in: Initialization file to Test Framework

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C  TInt CUIStore::Open( const TDesC& aTestFrameworkIni )
    {
    
    if( aTestFrameworkIni.Length() > KMaxFileName )
        {
        return KErrArgument;
        }
    
    TRAPD( err, 
        iUIEngine = CUIEngine::NewL( this );
        );
    if( err != KErrNone )
        {
        return err;
        }
    
    TFileName ini( aTestFrameworkIni );
    // Check given ini file
    TRAP( err, CheckIniL( ini ) );
    
    // Store folder of initialization file to open it again when looking for filters
    RDebug::Print(_L("CUIStore stores name of ini file [%S]"), &ini);
    iTestFrameworkIni.Copy(ini);
    
    return iUIEngine->Open( ini );
    
    }


/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: Close

    Description: Close test engine.

    Parameters: None

    Return Values: TInt KErrNone: Always returned KErrNone

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CUIStore::Close()
    {
    
    TInt ret = KErrNone;
    if( iUIEngine != NULL )
        {
        ret = iUIEngine->Close();
    
        delete iUIEngine;    
        iUIEngine = 0;
        }
        
    return ret;

    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: AddTestModule

    Description: Add test module to module list of test engine

    Parameters: TDesC& aModuleName: in: Testmodule, which is added to module list
                TDesC& aIniFile: in: Initialization file to the test module

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CUIStore::AddTestModule( const TDesC& aModuleName, 
                                       const TDesC& aIniFile )
    {

    TInt ret = iUIEngine->AddTestModule( aModuleName, aIniFile );
         
    RefreshAllCases();
    
    return ret;
         
    }


/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: RemoveTestModule

    Description: Add test module to module list of test engine

    Parameters: TDesC& aModuleName: in: Testmodule, which is removed of module list

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CUIStore::RemoveTestModule( const TDesC& aModuleName )
    {

    TInt ret = iUIEngine->RemoveTestModule( aModuleName );
     
    RefreshAllCases();
    
    return ret;
    
    }


/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: AddTestCaseFile

    Description: Add test case file to test case file list of test engine

    Parameters: TDesC& aModuleName: in: Testmodule, which own test cases of test case list.
                TDesC& aCaseFile: in: Test case list, which is added to test case list

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CUIStore::AddTestCaseFile( const TDesC& aModuleName, 
                                         const TDesC& aCaseFile )
    {

    TInt ret = iUIEngine->AddTestCaseFile( aModuleName, aCaseFile );
     
    RefreshAllCases();
    
    return ret;
    
    }


/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: RemoveTestCaseFile

    Description: Remove test case file of test case file list of test engine

    Parameters: TDesC& aModuleName: in: Testmodule, which own test cases of test case list
                TDesC& aCaseFile: in: Test case list, which is removed of test case list

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CUIStore::RemoveTestCaseFile( const TDesC& aModuleName, 
                                            const TDesC& aCaseFile )
    {   
    
    TInt ret = iUIEngine->RemoveTestCaseFile( aModuleName, aCaseFile );
         
    RefreshAllCases();
    
    return ret;
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: StartTestCase

    Description: Start selected test case identified with CTestInfo.
                anIndex contains index in StartedTestCase array, 
                which is valid only during execution of calling 
                function.

    Parameters: const CTestInfo& aTestInfo: in: Test case info
                TInt& anIndex: out: Index to StartedTestCaseArray returned 

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CUIStore::StartTestCase( const CTestInfo& aTestInfo, 
                                       TInt& anIndex )
    {
    
    CUIEngineContainer* container = NULL;
    TInt ret = iUIEngine->StartTestCase( container, aTestInfo );
    
    if( ret != KErrNone )
        {
        return ret;
        }
    
    CStartedTestCase* testCase = NULL;
    TRAP( ret,
        testCase = CStartedTestCase::NewL( aTestInfo, *container );
       );
    if( ret != KErrNone )
        {
        iUIEngine->AbortStartedTestCase( container );
        return ret;
        }
        
    ret = iStartedTestCases.Append( testCase );
    if( ret != KErrNone )
        {
        iUIEngine->AbortStartedTestCase( container );
        delete testCase;
        return ret;
        }
    
    anIndex =  iStartedTestCases.Find( testCase );
    
    if( anIndex < 0 )
        {
        User::Panic( KUIStore, KErrNotFound );
        }
    
    iUIStoreIf->Update( testCase, testCase->Status() ); 
                
    return KErrNone;
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: TestCases

    Description: Return array of existing test cases.

    Parameters: RRefArray<CTestInfo>& aTestCases: out: Array of test cases
                TDesC& aTestModule: in: Test module name (optional)
                TDesC& aTestCaseFile: in: Test case file name (optional) 

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CUIStore::TestCases( RRefArray<CTestInfo>& aTestCases, 
                                   const TDesC& aTestModule,
                                   const TDesC& aTestCaseFile ) 
    {
    
    TInt ret = UpdateCases();
    if( ret != KErrNone )
        {
        return ret;
        }
    TInt count = iTestCases.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( ( aTestModule.Length() > 0 ) &&
            ( iTestCases[i]->ModuleName() != aTestModule ) )
            {
            continue;
            }
        else if( ( aTestCaseFile.Length() > 0 ) &&
                 ( iTestCases[i]->TestCaseFile() != aTestCaseFile ) )
            {
            continue;
            }
        aTestCases.Append( *iTestCases[i] );
        }
    
    return KErrNone;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: StartedTestCaseL

    Description: Return started (running/runned) test case

    Parameters: CStartedTestCase& aTestCase: out: Test case information
                TInt anIndex: in: test case index in StartedTestCaseArray

    Return Values: Reference to CStartedTestCase object

    Errors/Exceptions: Leaves if anIndex out of range

    Status: Draft

-------------------------------------------------------------------------------
*/      
EXPORT_C CStartedTestCase& CUIStore::StartedTestCaseL( TInt anIndex )
    {
    
    TInt count = iStartedTestCases.Count();
    if( ( anIndex >= count ) ||
        ( anIndex < 0 ) )
        {
        User::Leave( KErrNotFound );
        }
        
    return *iStartedTestCases[anIndex];
         
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: StartedTestCases

    Description: Return started (running/runned) test cases

    Parameters: RRefArray<CStartedTestCase>& aTestCases: out: array of test cases
                TExecutionStatus aStatus: in: test case status (optional)
                TDesC& aTestModule: in: test module name (optional)
                TDesC& aTestCaseFile: in: test case file name (optional)

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/            
EXPORT_C TInt CUIStore::StartedTestCases( RRefArray<CStartedTestCase>& aTestCases,
                                          TInt aStatus,
                                          const TDesC& aTestModule,
                                          const TDesC& aTestCaseFile )
    {
    
    TInt count = iStartedTestCases.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( ( aTestModule.Length() > 0 ) &&
            ( iStartedTestCases[i]->TestInfo().ModuleName() != aTestModule ) )
            {
            continue;
            }
        else if( ( aTestCaseFile.Length() > 0 ) &&
                 ( iStartedTestCases[i]->TestInfo().TestCaseFile() != aTestCaseFile ) )
            {
            continue;
            }
        else if( ( aStatus != CUIStoreIf::EStatusAll) &&
                 !( iStartedTestCases[i]->Status() & aStatus ) )
            {
            continue;
            }
        if( aTestCases.Append( *iStartedTestCases[i] ) != KErrNone )
            {
            return KErrNoMemory;
            }
        }
    
    
    return KErrNone;
    
    }
            
/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: Modules

    Description: Return modules array

    Parameters: RRefArray<CModule>& aTestModules: out: modules array

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/     
EXPORT_C TInt CUIStore::Modules( RRefArray<TDesC>& aTestModules )
    {

    TInt ret = UpdateCases();
    if( ret != KErrNone )
        {
        return ret;
        }

    // Go through test cases and search if there are new modules 
    // and add them to aTestModules array 
    TInt caseCount = iTestCases.Count();
    TInt moduleCount = 0;
    TInt caseIndex = 0;
    TInt moduleIndex = 0;
    for ( caseIndex = 0; caseIndex < caseCount; caseIndex++ )
        {
        // First check modules
        moduleCount = aTestModules.Count();
        for ( moduleIndex = 0; moduleIndex < moduleCount; moduleIndex++ )
            {
            if( iTestCases[caseIndex]->ModuleName() == 
                aTestModules[moduleIndex] )
                {
                break;
                }
            }
        if ( moduleIndex == moduleCount )
            {
            // New module found
            if( aTestModules.Append( iTestCases[caseIndex]->ModuleName() ) !=
                KErrNone )
                {
                return KErrNoMemory;
                } 
            }
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: TestCaseFiles

    Description: Return test case files

    Parameters: RRefArray<TDesC>& aTestCaseFiles: out: Array of test case files
                TDesC& aTestModule: in: test module name (optional)

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/     
EXPORT_C TInt CUIStore::TestCaseFiles( RRefArray<TDesC>& aTestCaseFiles,
                                       const TDesC& aTestModule  )
    {
    
    TInt ret = UpdateCases();
    if( ret != KErrNone )
        {
        return ret;
        }

    // Go through test cases and search if there are new test case files 
    // for specified test module and add them to aTestCaseFiles array 
    TInt caseCount = iTestCases.Count();
    TInt fileCount = 0;
    TInt caseIndex = 0;
    TInt fileIndex = 0;
    for ( caseIndex = 0; caseIndex < caseCount; caseIndex++ )
        {
        if( ( aTestModule.Length() > 0 ) &&
            ( aTestModule != iTestCases[caseIndex]->ModuleName() ) )
            {
            // Test case file is not for specified module
            continue;
            }
            
        // First check modules
        fileCount = aTestCaseFiles.Count();
        for ( fileIndex = 0; fileIndex < fileCount; fileIndex++ )
            {
            if( iTestCases[caseIndex]->TestCaseFile() == 
                aTestCaseFiles[fileIndex] )
                {
                break;
                }
            }
        if ( fileIndex == fileCount )
            {
            // New test case file found
            if( aTestCaseFiles.Append( iTestCases[caseIndex]->TestCaseFile() ) !=
                KErrNone )
                {
                return KErrNoMemory;
                } 
            }
        }

    return KErrNone;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: LoadAllModules

    Description: Loads all TestFramework test modules from \system\libs\ directories 
                of all drives. In Secure Platform from \sys\bin directories.

    Parameters: None
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/ 
EXPORT_C TInt CUIStore::LoadAllModules()
    {
    
    RPointerArray<TDesC> testModules;
    
    TInt ret = ListAllModules( testModules );
    if( ret == KErrNone )
        {
        for (TInt i= 0; i < testModules.Count(); i++)
            {
            iLogger->Log( _L("Add test module: %S"), testModules[i] );
            ret = iUIEngine->AddTestModule( *testModules[i], KNullDesC );
            if( ret != KErrNone )
                {
                iLogger->Log( _L("Add test module %S failed %d"), 
                    testModules[i], ret );
                break;
                }
            }
        }
    RefreshAllCases();
    
    testModules.ResetAndDestroy();
    testModules.Close();
    
    return ret;

    }    

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: ListAllModules

    Description: Lists all TestFramework test modules from \system\libs\ directories 
                of all drives. In Secure Platform from \sys\bin directories.

    Parameters: None
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/ 
EXPORT_C TInt CUIStore::ListAllModules( RPointerArray<TDesC>& aModuleNames )
    {
    
    TRAPD( err, ListAllModulesL( aModuleNames ); );
    return err;
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: ListAllModulesL

    Description: Lists all TestFramework test modules from \system\libs\ directories 
                of all drives. In Secure Platform from \sys\bin directories.

    Parameters: None
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/ 
void CUIStore::ListAllModulesL( RPointerArray<TDesC>& aModuleNames )
    {

    TFileName libDirectory;
    RFs fsSession;
    TFileName fullName;

    // Connect to file server
	User::LeaveIfError( fsSession.Connect() ); // Start session
	CleanupClosePushL( fsSession );

    TDriveList drivelist; 
	User::LeaveIfError( fsSession.DriveList(drivelist) );
	// A TDriveList (the list of available drives), is an array of 
	// 26 bytes. Each byte with a non zero value signifies that the 
	// corresponding drive is available.

    // 0x10000079, 0x1000008d, 0x101FB3E7
    TUidType anEntryUid( KDynamicLibraryUid, 
                         KSharedLibraryUid, 
                         KUidTestModule ); 
    TInt driveNumber; 
	TChar driveLetter;

    CDir* testModules = NULL;
    
	for( driveNumber=EDriveA; driveNumber<=EDriveZ; driveNumber++ )
		{
		if( !drivelist[driveNumber] ) 
			{
			// If drive-list entry is zero, drive is not available
			continue;
			}
        User::LeaveIfError(fsSession.DriveToChar(driveNumber,driveLetter));

        libDirectory.Zero();
        libDirectory.Append( driveLetter );
        libDirectory.Append( _L(":\\sys\\bin\\*") );
        
        iLogger->Log( _L("Searching modules from %S"), &libDirectory );

        fsSession.GetDir( libDirectory, anEntryUid, ESortNone, testModules );

        if( !testModules )
            {
            // Continue if no test modules found
            continue;
            }
        TInt count = testModules->Count();
        for (TInt i= 0; i < count; i++)
            {           
            fullName = (*testModules)[i].iName;
            
            fullName.LowerCase();
            // Remove optional '.DLL' from file name
            TParse parse;
            parse.Set( fullName, NULL, NULL );
    
            if ( parse.Ext() == _L(".dll") )
                {
                const TInt len = parse.Ext().Length();
                fullName.Delete( fullName.Length()-len, len );
                }
                        
            // Exclude internal test modules (testcombiner, testscripter, suevent)
            _LIT(KTestCombiner, "testcombiner");
            _LIT(KTestScripter, "testscripter");
            _LIT(KSUEvent, "suevent");
            if(fullName != KTestCombiner && fullName != KSUEvent && fullName != KTestScripter)
                { 
                HBufC* name = fullName.AllocLC();
                iLogger->Log( _L("Found: %S"), name );
                User::LeaveIfError( aModuleNames.Append( name ) );
                CleanupStack::Pop( name );
                }
            }
        delete testModules;
        testModules = NULL;
		}
    
    CleanupStack::PopAndDestroy(); // fsSession 
    
    } 
    
/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: CreateTestSet

    Description: Create new test set.

    Parameters: TDesC& aSetName: in: test set name (Max length is KMaxName)
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/ 
EXPORT_C TInt CUIStore::CreateTestSet( const TDesC& aSetName )
    {
    
    TPtrC setName;
    TFileName tmp;
    TInt ret = ParseTestSetName( aSetName, setName, tmp );
    if( ret != KErrNone )
        {
        return ret;
        }
        
    CTestSetInfo* setInfo = NULL;
    if( FindSetByName( setName, setInfo ) == KErrNone )
        {
        return KErrAlreadyExists;
        }
    TRAPD( err, 
        setInfo = CTestSetInfo::NewL( setName );
        );
    if( err != KErrNone )
        {
        return err;
        }
    if( iTestSets.Append( setInfo ) != KErrNone )
        {
        delete setInfo;
        return KErrNoMemory;
        }
            
    return KErrNone;
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: RemoveTestSet

    Description: Remove active test set.

    Parameters: TDesC& aSetName: in: test set name (Max length is KMaxName)
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/ 
EXPORT_C TInt CUIStore::RemoveTestSet( const TDesC& aSetName )
    {
    TInt err = UnloadTestSet( aSetName );
    if ( err != KErrNone )
        {
        return err;
        }
    
    TFileName setfile;
    setfile.Append(KUIStoreDefaultDir);
    setfile.Append(aSetName);
    RFs fs;
    err = fs.Connect();
    if( err != KErrNone )
        {
        fs.Close();
        return err;
        }
    err = fs.Delete( setfile );
    if ( err != KErrNone )
        {
        fs.Close();
        return err;    
        }
    
    return KErrNone;   
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: UnloadTestSet

    Description: Unloads active test set.

    Parameters: TDesC& aSetName: in: test set name (Max length is KMaxName)
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/ 
EXPORT_C TInt CUIStore::UnloadTestSet( const TDesC& aSetName )
    {    
    TPtrC setName;
    TFileName tmp;
    TInt ret = ParseTestSetName( aSetName, setName, tmp );
    if( ret != KErrNone )
        {
        return ret;
        }
    
    TInt count = iTestSets.Count();
    TInt index = 0;
    for( ; index < count; index++ )
        {
        if( iTestSets[index]->Name() == setName )
            {
            break;
            }
        }
    if( index == count ) 
        { 
        return KErrNotFound;
        }

    CTestSetInfo* setInfo = iTestSets[index];
    iTestSets.Remove( index );
    
    // If started test set keeps info about currently removed test set
    // then remove also this info
    CStartedTestSet *stset;
    for(index = 0; index < iStartedTestSets.Count(); index++)
        {
        stset = iStartedTestSets[index];
        if(stset->GetOriginalTestSet() == setInfo)
            {
            stset->NullOriginalTestSet();
            }
        }
    
    delete setInfo;
    
    return KErrNone;    
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: TestSets

    Description: Query test sets.

    Parameters:RRefArray<CTestSetInfo>& aSetInfos: out: list of test sets
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/ 
EXPORT_C TInt CUIStore::TestSets( RRefArray<CTestSetInfo>& aSetInfos )
    {
    
    TInt count = iTestSets.Count();
    for( TInt i=0; i<count; i++ )
        {
        if( aSetInfos.Append( *iTestSets[i] ) != KErrNone )
            {
            return KErrNoMemory;
            }
        }
    return KErrNone;
    
    }       

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: TestSets

    Description: Query test sets.

    Parameters:RRefArray<CTestSetInfo>& aSetInfos: out: list of test sets
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/ 
EXPORT_C const CTestSetInfo& CUIStore::TestSetL( const TDesC& aSetName  )
    {
    
    TPtrC setName;
    TFileName tmp;
    User::LeaveIfError( ParseTestSetName( aSetName, setName, tmp ) );
        
    CTestSetInfo* setInfo = NULL;
    User::LeaveIfError( FindSetByName( setName, setInfo ) );
    return *setInfo;
    
    }                
        
/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: AddToTestSet

    Description: Add test case to test set.

    Parameters: TDesC& aSetName: out: test set name
                CTestInfo& aTestInfo: in: test case to add
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/ 
EXPORT_C TInt CUIStore::AddToTestSet( const TDesC& aSetName, 
                                      const CTestInfo& aTestInfo )
    {
    
    TPtrC setName;
    TFileName tmp;
    TInt ret = ParseTestSetName( aSetName, setName, tmp );
    if( ret != KErrNone )
        {
        return ret;
        }
        
    CTestSetInfo* setInfo = NULL;
    ret = FindSetByName( setName, setInfo );
    if( ret != KErrNone )
        {
        return ret;
        }
    
    return setInfo->AddTestCase( aTestInfo );
        
    }       

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: InsertToTestSet

    Description: Insert test case to test set.

    Parameters: TDesC& aSetName: out: test set name
                CTestInfo& aTestInfo: in: test case to add
                TInt aPos: in: position to add
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/ 
EXPORT_C TInt CUIStore::InsertToTestSet( const TDesC& aSetName, 
                                         const CTestInfo& aTestInfo,
                                         TInt aPos  )
    {
    
    TPtrC setName;
    TFileName tmp;
    TInt ret = ParseTestSetName( aSetName, setName, tmp );
    if( ret != KErrNone )
        {
        return ret;
        }
        
    CTestSetInfo* setInfo = NULL;
    ret = FindSetByName( setName, setInfo );
    if( ret != KErrNone )
        {
        return ret;
        }
    
    return setInfo->InsertTestCase( aTestInfo, aPos );
        
    }       
    
/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: RemoveFromTestSet

    Description: Remove test case from test set.

    Parameters: TDesC& aSetName: out: test set name
                CTestInfo& aTestInfo: in: test case to remove
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CUIStore::RemoveFromTestSet( const TDesC& aSetName, 
                                           const CTestInfo& aTestInfo )
    {
    
    TPtrC setName;
    TFileName tmp;
    TInt ret = ParseTestSetName( aSetName, setName, tmp );
    if( ret != KErrNone )
        {
        return ret;
        }
        
    CTestSetInfo* setInfo = NULL;
    ret = FindSetByName( setName, setInfo );
    if( ret != KErrNone )
        {
        return ret;
        }
    
    return setInfo->RemoveTestCase( aTestInfo );
    
    }       

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: SaveTestSet

    Description: Save test set. Deprecated, SaveTestSet2 to be used instead 

    Parameters: TDesC&:  out: test set name
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CUIStore::SaveTestSet( const TDesC& /* aSetName */ )
    {  
        
    return KErrNotSupported;
    
    }  

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: SaveTestSet2

    Description: Save test set.

    Parameters: TDes& aSetName: out: test set name
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CUIStore::SaveTestSet2( TDes& aSetName )
    {
    
    RRefArray<TDesC> testsets;
    
    GetTestSetsList(testsets);
    TBool isexist(EFalse);
    for(TInt i=0;i<testsets.Count();i++)
    {
       if(testsets[i]==aSetName)
           {
           isexist=ETrue;
           break;
           }
    
    }
    testsets.Reset();
    if(!isexist)
    {
        TTime current;
        TDateTime date_rep;
        current.HomeTime();
        date_rep = current.DateTime();
        TBuf<32> currSetName;
        _LIT(f_ext,".set");
        
        //create "test set name" string
        currSetName.AppendNum(date_rep.Year());
        currSetName.Append('_');
        currSetName.AppendNum(date_rep.Month()+1); // Incrimination necessary, because Day and Month fields of TDateTime class are 0 based
        currSetName.Append('_');
        currSetName.AppendNum(date_rep.Day()+1);
        currSetName.Append('_');
        currSetName.AppendNum(date_rep.Hour());
        currSetName.Append('_');
        currSetName.AppendNum(date_rep.Minute());
        currSetName.Append('_');
        currSetName.AppendNum(date_rep.Second());
        currSetName.Append(f_ext);
        
        for(TInt i=0;i<iTestSets.Count();i++)
        {
            if(iTestSets[i]->Name()==aSetName)
                {
                iTestSets[i]->RenameTestSet(currSetName);
                
                }
        
        }
        
        
        
        aSetName.Zero();
        aSetName.Copy(currSetName);
       
        
    
    }
    TPtrC setName;
    TFileName tmp;

    

    TInt ret = ParseTestSetName( aSetName, setName, tmp );
    if( ret != KErrNone )
        {
        return ret;
        }
        
    CTestSetInfo* setInfo = NULL;
    ret = FindSetByName( setName, setInfo );
    if( ret != KErrNone )
        {
        return ret;
        }
     
    TRAPD( err,
        SaveTestSetL( *setInfo, aSetName );
        );
    
   
    
    return err;
    
    }  

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: LoadTestSet

    Description: Load test set.

    Parameters: TDesC& aSetName: out: test set name
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CUIStore::LoadTestSet( const TDesC& aSetName )
    {
    TPtrC setName;
    TFileName tmp;
    TInt ret = ParseTestSetName( aSetName, setName, tmp );
    if( ret != KErrNone )
        {
        return ret;
        }
    CTestSetInfo* setInfo= NULL;
    
    FindSetByName( setName, setInfo );
    if(setInfo != NULL)
        {
        return KErrNone;
        }
    
    TRAPD( err, 
        setInfo = CTestSetInfo::NewL( aSetName );
        );
    if( err != KErrNone )
        {
        return err;
        }
    if( iTestSets.Append( setInfo ) != KErrNone )
        {
        delete setInfo;
        return KErrNoMemory;
        }

    TRAP( err,
        LoadTestSetL( setName, aSetName );
        );
    
     if( err != KErrNone && err != KErrAlreadyExists )
        {
        RemoveTestSet( setName );
        }
            
    return err;
    
    }       

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: UpdateTestSet

    Description: Updates (on storage) earlier saved test set.

    Parameters: CTestSetInfo& aSetInfo: in: test set
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CUIStore::UpdateTestSet(CTestSetInfo& aSetInfo)
    {
    TRAPD(err,
          UpdateTestSetL(aSetInfo, aSetInfo.Name());
         );
         
    return err;
    }  
        
/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: StartTestSet

    Description: Start selected test set identified with CTestSetInfo.

    Parameters: const CTestSetInfo& aTestSetInfo: in: Started test set
                TInt& anIndex: out index in StartedTestSet array, 
                    which is valid only during execution of calling 
                    function.
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/      
EXPORT_C TInt CUIStore::StartTestSet( const CTestSetInfo& aTestSetInfo,
                                      TInt& anIndex,
                                      CStartedTestSet::TSetType aType )
    {
    return StartTestSet(aTestSetInfo, anIndex, aType, EFalse);    
    }       

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: StartTestSet

    Description: Start selected test set identified with CTestSetInfo.

    Parameters: const CTestSetInfo& aTestSetInfo: in: Started test set
                TInt& anIndex: out index in StartedTestSet array, 
                    which is valid only during execution of calling 
                    function.
                TSetType aType: sequential or paraller
                TBool aNotExecutedCasesMode: only test case which haven't been
                    executed yet
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/      
EXPORT_C TInt CUIStore::StartTestSet( const CTestSetInfo& aTestSetInfo,
                                      TInt& anIndex,
                                      CStartedTestSet::TSetType aType,
                                      TBool aNotStartedCasesMode )
    {
    
    TInt ret = KErrNone;
    
    CStartedTestSet* set = NULL;
    TRAPD( err, 
        set = CStartedTestSet::NewL( this, aTestSetInfo, aType );
        );
    if( err != KErrNone )
        {
        return err;
        }

    if( iStartedTestSets.Append( set ) != KErrNone )
        {
        delete set;
        return KErrNoMemory;
        } 

    // Set mode in which only still not executed test cases will be run.
    // It applies only to sequential execution.
    set->SetNotStartedCasesMode(aNotStartedCasesMode);
    
    ret = set->StartNext();
        
    anIndex = iStartedTestSets.Find( set );
    if( anIndex < 0 )
        {
        User::Panic( KUIStore, KErrNotFound );
        }
    // Check that testset starting was successful
    if( ret != KErrNone )
        {
        iStartedTestSets.Remove( anIndex );
        anIndex = KErrNotFound; // safety
        delete set;
        return ret;
        }
    
    return KErrNone;
    
    }       

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: AbortTestSet

    Description: Abort running test set.

    Parameters: None
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CUIStore::AbortTestSet( CStartedTestSet& aSetInfo )
    {
        
    return aSetInfo.Abort();
    
    } 


/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: StartedTestSetL

    Description: Return started (running/runned) test set.

    Parameters: TInt anIndex: out index in StartedTestSet array 
                        
    Return Values: CStartedTestSet&: reference to test set

    Errors/Exceptions: Leaves on error.

    Status: Draft

-------------------------------------------------------------------------------
*/  
EXPORT_C CStartedTestSet& CUIStore::StartedTestSetL( TInt anIndex)
    {
    
    if( anIndex < 0 ||
        anIndex >= iStartedTestSets.Count() )
        {
        User::Leave( KErrNotFound );
        }
    
    return *iStartedTestSets[ anIndex ];
    
    }       


/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: StartedTestSets

    Description: Return started (running/runned) test cases

    Parameters: RRefArray<CStartedTestSet>& aTestCases: out: list of test sets
                TInt aStatus: in: status of queried sets
                        
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CUIStore::StartedTestSets( RRefArray<CStartedTestSet>& aTestSets,
                                         TInt aStatus )
    {    
    
    TInt count = iStartedTestSets.Count();
    
    for( TInt i = 0; i < count; i++ )
        {
        if( ( aStatus != CUIStoreIf::ESetAll) &&
            !( iStartedTestSets[i]->Status() & aStatus ) )
            {
            continue;
            }
        if( aTestSets.Append( *iStartedTestSets[i] ) != KErrNone )
            {
            return KErrNoMemory;
            }
        }        
        
    return KErrNone;
    
    }           

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: LoadSavedTestCases

    Description: Load saved testcases.

    Parameters: None
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CUIStore::LoadSavedTestCases()
    {
    
    TRAPD( err,
        LoadExecutedTestCasesL();
        );
    
    return err;
    
    }       
/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: GetTestSetsList

    Description: Returns menu item text.

    Parameters: const TInt                :in:      Menu index

    Return Values: const TDesC&                     Menu line text

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CUIStore::GetTestSetsList( RRefArray<TDesC>& aArray )
    {
    RFs fileReader;
    CDir* dirContents = NULL;
    _LIT(KSetPattern,".set");
    TInt entNum = 0;
    TInt ret = 0;
    HBufC* fName = NULL;

    
    ret = fileReader.Connect();

    if (ret != KErrNone)
    	return ret;
    
    
    ret = fileReader.GetDir(KUIStoreDefaultDir,
    						KEntryAttNormal | KEntryAttHidden | KEntryAttSystem,
    						ESortByName | EDirsFirst | EAscending,
    						dirContents);
    fileReader.Close();	
    if (ret != KErrNone)
    	{    		
	    delete dirContents;
	    return ret;
	    }
    entNum = dirContents->Count();
    for (int i = 0;i<entNum;i++)
    	{
    	if ((!dirContents->operator[](i).IsDir())&&
    		 dirContents->operator[](i).iName.Find(KSetPattern)!=KErrNotFound)
    			{
    			  TRAPD( err, fName = HBufC::NewL(64) );		       			             
    			  if( err != KErrNone )
    			    {
    			      delete dirContents;
    			      return err;
    			    }    		 	
    		    *fName =  dirContents->operator[](i).iName;
    		 	ret = iFileList.Append(fName);
    		 	if (ret != KErrNone)
    		 		{
    					delete fName;
    					delete dirContents;
    					return ret;	
    		 		}
    		 	ret = aArray.Append(*fName);
			 	if (ret != KErrNone)
    		 		{
    					delete fName;
    					delete dirContents;
    					return ret;	
    		 		}

    			}    
    	}
    delete dirContents;
    return KErrNone; 

    }


/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: FindSetByName

    Description: Finds test set by name.

    Parameters: const TDesC& aSetName: in: set name 
                CTestSetInfo*& aSetInfo: out: pointer to test set
                        
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIStore::FindSetByName( const TDesC& aSetName, 
                              CTestSetInfo*& aSetInfo )
    {
    
    TInt count = iTestSets.Count();
    for( TInt i=0; i<count; i++ )
        {
        if( iTestSets[i]->Name() == aSetName )
            {
            aSetInfo = iTestSets[i];
            return KErrNone;
            }
        }
    
    return KErrNotFound;
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: FindSetByCase

    Description: Finds test set by name.

    Parameters:  const CStartedTestCase* aTestCase: in: running test case 
                 CStartedTestSet*& aSet: out: set running test case
                        
    Return Values: KErrNotFound: test case is not runned by any test set
                   KErrNone: test case was runned by aSet
                   

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIStore::FindStartedSetByCase( const CStartedTestCase* aTestCase, 
                                     CStartedTestSet*& aSet  )
    {
    
    TInt count = iStartedTestSets.Count();
    for( TInt i=0; i<count; i++ )
        {
        if( iStartedTestSets[i]->IsRunning( aTestCase ) )
            {
            aSet = iStartedTestSets[i];
            return KErrNone;
            }
        }
    
    return KErrNotFound;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: LoadTestSetL

    Description: Load test set.

    Parameters: TDesC& aSetName: out: test set name
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIStore::LoadTestSetL( const TDesC& aSetName, const TDesC& /*aSetFileName*/ )
    {
    
    TPtrC tmp;
    TInt num;
    TInt high;
    TInt64 interval;
    
    CStifParser* parser = CStifParser::NewL( KUIStoreDefaultDir, aSetName );

        
    CleanupStack::PushL( parser );
    
    CStifSectionParser* section = 
        parser->SectionL( KUIStoreSetStart, KUIStoreSetEnd );
    CleanupStack::PushL( section );
    
    CStifItemParser* item;

    // Get started test case (if possible)
    TUint lastStartedCaseIndex = 0;
    item = section->GetItemLineL(KUIStoreLastStartedCaseIndex);
    if(item)
        {
        CleanupStack::PushL(item);
        TInt r = item->GetInt(KUIStoreLastStartedCaseIndex, lastStartedCaseIndex);
        CleanupStack::PopAndDestroy(item);
        if(r != KErrNone)
            {
            __TRACE(KInit, (_L("Could not read [%S] from test set file. Result [%d]."), &KUIStoreLastStartedCaseIndex, r));
            }
        }
    else
        {
        __TRACE(KInit, (_L("Could not find [%S] from test set file."), &KUIStoreLastStartedCaseIndex));
        }

    
    CTestSetInfo* setInfo = NULL;
    User::LeaveIfError( FindSetByName( aSetName, setInfo ) );
    
    // Update started case
    setInfo->SetLastStartedCaseIndex(lastStartedCaseIndex);
    
    CTestInfo* testInfo = CTestInfo::NewL();
    CleanupStack::PushL( testInfo );    

    CStifSectionParser* subSection = 
        section->SubSectionL( KUIStoreSetCaseStart, KUIStoreSetCaseEnd );
    
    while( subSection )
        {
        CleanupStack::PushL( subSection );    
        
        // Get module name
        // Mandatory, leave if not found
        User::LeaveIfError( 
            subSection->GetLine( KUIStoreCaseModuleName, tmp, ENoTag ) );
        testInfo->SetModuleName( tmp );
    
        // Get test case title
        // Mandatory, leave if not found
        User::LeaveIfError( 
            subSection->GetLine( KUIStoreTestCaseTitle, tmp, ENoTag ) );
        testInfo->SetTestCaseTitle( tmp );
        
        // Get test case file
        num = subSection->GetLine( KUIStoreTestCaseFile, tmp, ENoTag );
        if( ( num == KErrNone ) &&
            ( tmp.Length() > 0 ) )
            {
            // Optional
            testInfo->SetTestCaseFile( tmp );
            }
        
        // Get test case number
        item = subSection->GetItemLineL( KUIStoreTestCaseNum );
        CleanupStack::PushL( item );
        // Mandatory, leave if not found
        User::LeaveIfError( item->GetInt( KUIStoreTestCaseNum, num ));
        testInfo->SetTestCaseNumber( num );
        CleanupStack::PopAndDestroy( item );    
        
        // Get test case priority
        item = subSection->GetItemLineL( KUIStoreCasePriority );
        CleanupStack::PushL( item );
        // Mandatory, leave if not found
        User::LeaveIfError( item->GetInt( KUIStoreCasePriority, num ));
        testInfo->SetPriority( num );
        CleanupStack::PopAndDestroy( item ); 
        
        // Get test case timeout
        item = subSection->GetItemLineL( KUIStoreCaseTimeout );
        CleanupStack::PushL( item );
        // Mandatory, leave if not found
        User::LeaveIfError( item->GetInt( KUIStoreCaseTimeout, num ));
        User::LeaveIfError( item->GetNextInt( high ));
        //@js<--remove--> interval.Set( high, num );
        interval = MAKE_TINT64( high, num );
        TTimeIntervalMicroSeconds timeout( interval );
        testInfo->SetTimeout( timeout );
        CleanupStack::PopAndDestroy( item ); 
        
        User::LeaveIfError( setInfo->AddTestCase( *testInfo ) );
        
        CleanupStack::PopAndDestroy( subSection );    
        subSection = 
            section->NextSubSectionL( KUIStoreSetCaseStart, KUIStoreSetCaseEnd );
        } 
        
    CleanupStack::PopAndDestroy( testInfo );    
    CleanupStack::PopAndDestroy( section );
    CleanupStack::PopAndDestroy( parser );
    
    const RRefArray<const CTestInfo>& testCases = setInfo->TestCases();
    LoadTestModulesAndTestCaseFilesL( testCases );
     
    }     
    
/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: SaveTestSetL

    Description: Save test set.

    Parameters: CTestSetInfo& aSetInfo: in: test set
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CUIStore::SaveTestSetL( CTestSetInfo& aSetInfo, const TDesC& aSetFileName )
    {
    //Extract path
    TParse p;
    p.Set(aSetFileName, NULL, NULL);
    TPtrC path =  p.DriveAndPath(); // gives path for test set
    TPtrC fn = p.NameAndExt(); // gives filename with extension
    if(path.Length() == 0)
        {
        path.Set(KUIStoreDefaultDir);
        }

    //Create file server
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    //Create or open file
    RFile file;
    TFileName filename = path;
    filename.Append(fn);
    
    TInt r = file.Replace(fs, filename, EFileWrite);
    if(r != KErrNone)
        {
        User::Leave(r);
        }
    else
        {
        CleanupClosePushL(file);
        
        RBuf buffer;
        buffer.CreateL(256);
        CleanupClosePushL(buffer);
        
        // Saving
        buffer.Format(_L("%S"), &KUIStoreSetStart);
        WriteLineL(file, buffer);

        // Saving test set causes reset of index
        aSetInfo.SetLastStartedCaseIndex(0);
        buffer.Format(_L("%S "), &KUIStoreLastStartedCaseIndex);
        buffer.AppendNumFixedWidth(aSetInfo.GetLastStartedCaseIndex(), EDecimal, KFixedStartedCaseIndex);
        WriteLineL(file, buffer);

        // Saving test cases
        TInt count = aSetInfo.TestCases().Count();
        for(TInt i = 0; i < count; i++)
            {
            WriteLineL(file, KNullDesC);
            buffer.Format(_L("%S"), &KUIStoreSetCaseStart);
            WriteLineL(file, buffer);
            
            buffer.Format(_L("%S %S"), &KUIStoreCaseModuleName, &aSetInfo.TestCases()[i].ModuleName());
            WriteLineL(file, buffer);
            buffer.Format(_L("%S %S"), &KUIStoreTestCaseTitle, &aSetInfo.TestCases()[i].TestCaseTitle());
            WriteLineL(file, buffer);
            if(aSetInfo.TestCases()[i].TestCaseFile().Length() > 0)
                {
                buffer.Format(_L("%S %S"), &KUIStoreTestCaseFile, &aSetInfo.TestCases()[i].TestCaseFile());
            WriteLineL(file, buffer);
                }
            buffer.Format(_L("%S %d"), &KUIStoreTestCaseNum, aSetInfo.TestCases()[i].TestCaseNum());
            WriteLineL(file, buffer);
            buffer.Format(_L("%S %d"), &KUIStoreCasePriority, aSetInfo.TestCases()[i].Priority());
            WriteLineL(file, buffer);
            buffer.Format(_L("%S %d %d"), &KUIStoreCaseTimeout, I64LOW(aSetInfo.TestCases()[i].Timeout().Int64()), 
                                                                I64HIGH(aSetInfo.TestCases()[i].Timeout().Int64()));
            WriteLineL(file, buffer);
            buffer.Format(_L("%S %d"), &KUIStoreCaseExpectedResult, aSetInfo.TestCases()[i].ExpectedResult());
            WriteLineL(file, buffer);
            buffer.Format(_L("%S"), &KUIStoreSetCaseEnd);    
            WriteLineL(file, buffer);
            }

        WriteLineL(file, KNullDesC);
        buffer.Format(_L("%S"), &KUIStoreSetEnd);
        WriteLineL(file, buffer);

        CleanupStack::PopAndDestroy(&buffer);
        CleanupStack::PopAndDestroy(&file);
        }
    CleanupStack::PopAndDestroy(&fs);        
    }           

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: UpdateTestSetL

    Description: Updates information in test set file.

    Parameters: CTestSetInfo& aSetInfo: in: test set
    
    Return Values: Symbian OS error code

    Errors/Exceptions: Leaves when writing to file fails
                       Leaves when file.seek fails
                       Leaves when can't connect to file server

    Status: Approved

-------------------------------------------------------------------------------
*/
void CUIStore::UpdateTestSetL(CTestSetInfo& aSetInfo, const TDesC& aSetFileName)
    {
    // Get path
    TParse p;
    p.Set(aSetFileName, NULL, NULL);
    TPtrC path = p.DriveAndPath(); // gives path for test set
    TPtrC fn = p.NameAndExt(); // gives filename with extension
    if(path.Length() == 0)
        {
        path.Set(KUIStoreDefaultDir);
        }

    //Create file server
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    //Create or open file
    RFile file;
    TFileName filename = path;
    filename.Append(fn);
    TInt foundpos = KErrNotFound;
    
    TInt r = file.Open(fs, filename, EFileWrite);
    if(r != KErrNone)
        {
        User::Leave(r);
        }
    else
        {
        CleanupClosePushL(file);

        //Search for line
        RBuf buffer;
        buffer.CreateL(256);
        CleanupClosePushL(buffer);

        //Prepare file
        TInt pos = 0;
        User::LeaveIfError(file.Seek(ESeekStart, pos));
        
        //Read file
        ReadLineL(file, buffer);
        while(buffer.Length() > 0)
            {
            // Keep remembering current position
            if(buffer.Find(KUIStoreLastStartedCaseIndex) == 0)
                {
                foundpos = pos;
                break;
                }
            // What is current position
            pos = 0;
            User::LeaveIfError(file.Seek(ESeekCurrent, pos));
            // Read next line from file
            ReadLineL(file, buffer);
            }

        if(foundpos != KErrNotFound)
            {
            // Position was found. Just update that line (save index of last 
            // started test case)
            RBuf8 b;
            b.CreateL(40);
            CleanupClosePushL(b);

            b.Copy(KUIStoreLastStartedCaseIndex);
            b.Append(_L8(" "));
            b.AppendNumFixedWidth(aSetInfo.GetLastStartedCaseIndex(), EDecimal, KFixedStartedCaseIndex);
            User::LeaveIfError(file.Seek(ESeekStart, foundpos));
            User::LeaveIfError(file.Write(b));
            CleanupStack::PopAndDestroy(&b);
            }

        CleanupStack::PopAndDestroy(&buffer);
        CleanupStack::PopAndDestroy(&file);
        }
    CleanupStack::PopAndDestroy(&fs);        
    
    if(foundpos == KErrNotFound)
        {
        // Position of KUIStoreLastStartedCaseIndex could not be found.
        // Store the whole file.
        SaveTestSetL(aSetInfo, aSetFileName);
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: ReadLineL

    Description: Read the whole line from the file. If there is enough space,
                 the whole content of line will be returned in buffer.

    Parameters: RFile& file: in: file to be read
                TDes8& buffer: out: buffer to be returned
    
    Return Values: None

    Errors/Exceptions: Leaves if RFile::Read method fails

    Status: Approved

-------------------------------------------------------------------------------
*/
void CUIStore::ReadLineL(RFile &file, TDes& buffer)
    {
    TBuf8<1> c;
    TBuf<1> c16;
    buffer.Copy(KNullDesC);
    
    User::LeaveIfError(file.Read(c));
    while(c.Length() > 0)
        {
        // There is still place to write to the dest buffer
        if(buffer.Length() < buffer.MaxLength())
            {
            c16.Copy(c);
            buffer.Append(c16);
            }
        // Stop reading if end of line
        if(c[0] == 0x0A)
            {
            break;
            }
        User::LeaveIfError(file.Read(c));
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: WriteLineL

    Description: Write given line to the file and adds end of line.

    Parameters: RFile& file: in: file to be written
                TDesC& buffer: in: buffer to be written
    
    Return Values: None

    Errors/Exceptions: Leaves if RFile::Write method fails

    Status: Approved

-------------------------------------------------------------------------------
*/
void CUIStore::WriteLineL(RFile &file, const TDesC& buffer)
    {
    if(buffer.Length() > 0)
        {
        // Create 8-bit buffer
        RBuf8 buf;
        buf.CreateL(buffer.Length());
        CleanupClosePushL(buf);
    
        buf.Copy(buffer);
    
        // Write buffer to file + end of line
        User::LeaveIfError(file.Write(buf));

        // Delete 8-bit buffer
        CleanupStack::PopAndDestroy(&buf);
        }

    // Write end of line
    TBuf8<2> eoline;
    eoline.Copy(_L("\r\n"));
    User::LeaveIfError(file.Write(eoline));
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: LoadTestModulesL

    Description: Load test modules. 

    Parameters: CTestSetInfo& aSetInfo: in: test set
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIStore::LoadTestModulesAndTestCaseFilesL( 
    const RRefArray<const CTestInfo>& aTestCases )
    {
    
    RRefArray<TDesC> testCaseFiles;
    RRefArray<TDesC> testModules;
    CleanupClosePushL( testCaseFiles );
    CleanupClosePushL( testModules );
    
    User::LeaveIfError( Modules( testModules ) );
    
    TInt cCount = aTestCases.Count();
    TInt mCount = 0;
    TInt mInd=0;
    TInt fCount = 0;
    TInt fInd=0;
    for( TInt cInd=0; cInd<cCount; cInd++ )
        {
        mCount = testModules.Count();
        for( mInd=0; mInd<mCount; mInd++ )
            {
            if( aTestCases[cInd].ModuleName() == testModules[mInd] )
                {
                // Test module already loaded
                break;
                }
            }
        if( mInd == mCount )
            {
            // Not found, load test module
            if( AddTestModule( aTestCases[cInd].ModuleName() ) == KErrNone )
                {
                User::LeaveIfError( 
                    testModules.Append( aTestCases[cInd].ModuleName() ) );
                }
            }                
        if( aTestCases[cInd].TestCaseFile().Length() == 0 )
            {
            // Test case file is not used, continue
            continue;
            }    
        testCaseFiles.Reset();
        User::LeaveIfError( 
            TestCaseFiles( testCaseFiles, aTestCases[cInd].ModuleName() ));
        fCount = testCaseFiles.Count();
        for( fInd=0; fInd<fCount; fInd++ )
            {
            if( aTestCases[cInd].TestCaseFile() == testCaseFiles[fInd] )
                {
                // Testcasefile already loaded
                break;
                }
            }  
        if( fInd == fCount )
            {  
            // Load test module
            if( AddTestCaseFile( aTestCases[cInd].ModuleName(),
                                 aTestCases[cInd].TestCaseFile() ) == KErrNone )
                {
                User::LeaveIfError( 
                    testCaseFiles.Append( aTestCases[cInd].TestCaseFile() ) );
                }
            }
        }   

    CleanupStack::PopAndDestroy(); // testModules
    CleanupStack::PopAndDestroy(); // testCaseFiles
        
    }
                               
/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: TestExecuted

    Description: Test case executed callback from UI engine.
    
    Parameters:  CUIEngineContainer* const aContainer: in: Execution container
                 TFullTestResult& aFullTestResult: in: test result
    
    Return Values: None
    
    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/    
void CUIStore::TestExecuted ( CUIEngineContainer* aContainer,
                              TFullTestResult& aFullTestResult )
    {
    
    CStartedTestCase* testCase = NULL;
    
    if( FindByContainer( aContainer, testCase ) != KErrNone )
        {
        __TRACE( KError, ( CStifLogger::EError, _L("CUIStore::TestExecuted: Not found")));
        return;
        }
    CStartedTestSet* set = NULL;
    TInt setStatus = 0;
    if( FindStartedSetByCase( testCase, set ) == KErrNone )
        {
        setStatus = set->Status();
        set->TestCompleted( testCase, aFullTestResult );        
        setStatus |= set->Status();
        }
        
    // Check and delete all pending popup windows for test case
    TInt count = iPopups.Count();
    for( TInt i = 0; i<count; i++ )
        {
        if( iPopups[i]->Container() == aContainer )
            {
            delete iPopups[i];
            iPopups.Remove( i );
            }
        }
        
    // Get old status 
    TInt status = testCase->Status();
    // Set result
    testCase->SetResult( aFullTestResult );
    // Get new status 
    status |= testCase->Status();
    // Add set status flags
    status |= setStatus;
    
    iUIStoreIf->Update( testCase, status ); 
    
    return;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: PrintProg

    Description: Progress information from test case execution, 
            callback from UI engine.
    
    Parameters:  CUIEngineContainer* const aContainer: in: Execution container
                 TTestProgress& aProgress: in: print info
    
    Return Values: Symbian OS error code

    
    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/ 
TInt CUIStore::PrintProg ( CUIEngineContainer* aContainer,
                           TTestProgress& aProgress )
    {
    if( aContainer == NULL )
        {
        return KErrArgument;
        }
    if( ( aProgress.iDescription.Length() == 0 ) &&
        ( aProgress.iText.Length() == 0 ) )
    
        {
        return KErrArgument;
        }
        
    CStartedTestCase* testCase = NULL;
    
    if( FindByContainer( aContainer, testCase ) != KErrNone )
        {
        __TRACE( KError, ( CStifLogger::EError, _L("CUIStore::TestExecuted: Not found")));
        return KErrNotFound;
        }

    TInt position = KErrNotFound;
    TInt smallPos = KErrNotFound;

    // Search the array to find the position
    const TInt count = testCase->PrintArray().Count();
    for (TInt i = 0; i < count; i++)
        {

        // Check if that item is already on list
        if ( testCase->PrintArray()[i]->iDescription == aProgress.iDescription &&
             testCase->PrintArray()[i]->iPosition    == aProgress.iPosition )
            {
            // Set it to be updated
            position = i;
            break;
            }

        // Found a smaller priority item from list
        if ( aProgress.iPosition <  testCase->PrintArray()[i]->iPosition )
            {              
            smallPos = i;
            break;
            }
        }

    // Either update item in array or add new item to array
    if ( position != KErrNotFound )
        {
        // Replace existing text
        testCase->PrintArray()[position]->ReplaceTextL( aProgress );
        }
    else 
        {
        CTestProgress* prog = NULL;
        TRAPD( err,
            // Allocate new
            prog = CTestProgress::NewL( aProgress );
            );
        if( err != KErrNone )
            {
            return err;
            }
        if ( smallPos != KErrNotFound )
            {
            if( testCase->PrintArray().Insert( prog, smallPos ) != KErrNone )
                {
                delete prog;
                return KErrNoMemory;
                }
            }
        else 
            {
            if( testCase->PrintArray().Append( prog ) != KErrNone )
                {
                delete prog;
                return KErrNoMemory;
                }
            }
        }
        
    iUIStoreIf->Update( testCase, CUIStoreIf::EPrintUpdate ); 
    
    return KErrNone;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: PrintProg

    Description: Error information from Test framework, 
            callback from UI engine.
    
    Parameters:  CUIEngineContainer* const aContainer: in: Execution container
                 TErrorNotification& aError: in: error info
    
    Return Values: Symbian OS error code
    
    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/    
TInt CUIStore::ErrorPrint( TErrorNotification& aError )
    {

    if( aError.iText.Length() == 0 )
        {
        return KErrArgument;
        }
    
    iUIStoreIf->Error( aError );
        
    return KErrNone;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: RemoteMsg

    Description: Remote protocol control messages handling.

    Parameters: const TDesC& aMessage: in: Remote message

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIStore::RemoteMsg( CUIEngineContainer* aContainer,
                          const TDesC& aMessage)
    {

    if( ( aMessage.Length() == 0 ) ||
        ( aContainer == NULL ) )
        {
        return KErrArgument;
        }
    
    TInt ret = KErrNone;

    TInt error = KErrNone;
    CStifTFwIfProt* msg = NULL;
    CStifTFwIfProt* resp = NULL;
    TRAPD( err, msg = CStifTFwIfProt::NewL(); );
    if( err != KErrNone )
        {
        return err;
        }
    TRAP( err, resp = CStifTFwIfProt::NewL(); );
    if( err != KErrNone )
        {
        delete msg;
        return err;
        }
    
    // Parse received message
    TRAP( err, 
        error = msg->SetL( aMessage ) 
    );
    if( err != KErrNone )
        {
        __TRACE( KError, ( _L( "CUIStore::RemoteMsg: message header parsing failed [%d]"), err ) );
        delete msg;
        delete resp;
        return err;
        }

    // Create response 
    TRAP( err, resp->CreateL(); );
    if( err != KErrNone )
        {
        delete msg;
        delete resp;
        return err;
        }                

    resp->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgResponse );
    
    if( error != KErrNone )
        {
        __TRACE( KError, ( _L( "CUIStore::RemoteMsg: message parsing failed [%d]"), error ) );
        resp->AppendId( SETID( (TInt32)DevId(), 0 ) );
        resp->AppendId( msg->SrcId() );
        resp->Append( CStifTFwIfProt::MsgType, msg->iMsgType );
        resp->Append( CStifTFwIfProt::RespParam, CStifTFwIfProt::ERespResult, error );
        
        aContainer->RemoteReceive( resp->Message() );
        
        // Error reported with protocol message, return success
        delete msg;
        delete resp;
        return KErrNone;
        }                    
    
    TBool sendResp = ETrue;
                        
    switch( msg->iMsgType )
        {
        case CStifTFwIfProt::EMsgReserve:
            {
            // Check IDs
            if( ( msg->SrcDevId() == 0 ) ||
                ( msg->SrcTestId() == 0 ) )
                {
                __TRACE( KError, ( _L( "CUIStore::RemoteMsg: reserve for illegal srcid received") ) );
                error = KErrGeneral;
                }
            if( msg->DstId() != 0 ) 
                {
                // Not a broadcast 
                if( ( msg->DstDevId() != DevId() ) ||
                    ( msg->DstTestId() != 0 ) )  
                    {
                    __TRACE( KError, ( _L( "CUIStore::RemoteMsg: reserve for illegal dstid received") ) );
                    error = KErrGeneral;
                    }
                }                
                
            resp->AppendId( SETID( (TInt32)DevId(), 0 ) );
            resp->AppendId( msg->SrcId() );
            resp->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgReserve );
            if( error != KErrNone )
                {
                resp->Append( CStifTFwIfProt::RespParam, CStifTFwIfProt::ERespResult, error );
                }
            }
            break;
        case CStifTFwIfProt::EMsgRelease:
            {
            // Check protocol ids
            if( ( msg->SrcDevId() == 0 ) ||
                ( msg->SrcTestId() == 0 ) ||
                ( msg->DstTestId() != 0 ) )
                {
                __TRACE( KError, ( _L( "CUIStore::RemoteMsg: release for illegal srcid or dstid received") ) );
                error = KErrGeneral;
                }   
            else if( msg->DstDevId() != DevId() ) 
                {
                __TRACE( KError, ( _L( "CUIStore::RemoteMsg: release for illegal dstid received") ) );
                error = KErrNotFound;
                }   

            resp->AppendId( msg->DstId() );
            resp->AppendId( msg->SrcId() );
            resp->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRelease );
            
            if( error != KErrNone )
                {
                resp->Append( CStifTFwIfProt::RespParam, CStifTFwIfProt::ERespResult, error );
                }            
            }
            break;
        case CStifTFwIfProt::EMsgRemote:
            {
            // Check protocol ids
            if( ( msg->SrcDevId() == 0 ) ||
                ( msg->SrcTestId() == 0 ) ||
                ( msg->DstDevId() == 0 ) )
                {
                __TRACE( KError, ( _L( "CUIStore::RemoteMsg: remote for illegal srcid or dstid received") ) );
                error = KErrGeneral;
                }
            else
                {               
                __TRACE( KError, ( _L( "CUIStore::RemoteMsg: received remote call") ) );
                
                error = MsgRemote( aContainer, *msg, *resp );
                }
                
            if( error != KErrNone )
                {
                resp->AppendId( msg->DstId() );
                resp->AppendId( msg->SrcId() );
                resp->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRemote );
                resp->Append( msg->iCmdDes );
                resp->Append( CStifTFwIfProt::RespParam,     
                             CStifTFwIfProt::ERespResult, 
                             error );
                }
            else
                {
                sendResp = EFalse;
                }
                                
            }
            break;
        case CStifTFwIfProt::EMsgResponse:            
        default:
            __TRACE( KError, ( _L( "CUIStore::RemoteMsg: invalid message")) );
            ret = KErrNotSupported;
        }

    if( ( ret == KErrNone ) && sendResp ) 
         {
        aContainer->RemoteReceive( resp->Message() );
        }
        
    delete msg;
    delete resp;
    
    return ret;
    
    }


/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: MsgRemote

    Description: Remote command
    
    Parameters: const TDesC& aMessage: in:

    Return Values: KErrNotSupported

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIStore::MsgRemote( CUIEngineContainer* aContainer, 
                          CStifTFwIfProt& aReq, 
                          CStifTFwIfProt& aResp )
    {
    
    TInt ret = KErrNone;    
        
    switch( aReq.iCmdType )
        {
        case CStifTFwIfProt::ECmdRun:
            ret = MsgRemoteRun( aContainer, aReq, aResp );
            break;
        case CStifTFwIfProt::ECmdPause:
        case CStifTFwIfProt::ECmdResume:
        case CStifTFwIfProt::ECmdCancel:
            ret = MsgRemoteTestCtl( aContainer, aReq, aResp );
            break;
        case CStifTFwIfProt::ECmdRequest:
        case CStifTFwIfProt::ECmdRelease:
            ret = MsgRemoteEventCtl( aContainer, aReq, aResp );            
            break;
        case CStifTFwIfProt::ECmdSendReceive:
            {
            ret = MsgRemoteSendReceive( aContainer, aReq, aResp );
            break;
            }
        default:
            {
            CStifTFwIfProt* resp = NULL;
            TRAPD( err, 
                resp = CStifTFwIfProt::NewL(); 
                resp->CreateL();
                );
            if( err != KErrNone )
                {
                delete resp;
                return err;
                }
            resp->SetMsgType( CStifTFwIfProt::EMsgResponse );
            resp->SetSrcId( aReq.DstId() );
            resp->SetDstId( aReq.SrcId() );
            resp->SetRespType( CStifTFwIfProt::EMsgRemote );
            // Command type must be set separately, 
            // because it is unspecified in this case
            resp->iCmdType = aReq.iCmdType;
            resp->Append( aReq.iCmdDes );
                
            ret = RemotePopup( aContainer, aReq.Message(), resp );

            if( ret != KErrNone )
                {
                delete resp;
                }
            }
            break;
        }
    
    return ret;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: AtsRemoteRun

    Description: Remote run message
    
    Parameters: const TDesC& aMessage: in:

    Return Values: KErrNotSupported

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/        
TInt CUIStore::MsgRemoteRun( CUIEngineContainer* aContainer,
                             CStifTFwIfProt& aReq, 
                             CStifTFwIfProt& /* aResp */)
    {
    
    TInt ret = KErrNone;    
    TUint16 testid = 0;
    
    if( ( aReq.DstDevId() == 0 ) ||
        ( aReq.DstTestId() != 0 ) ) 
        {
        // Protocol violation
        __TRACE( KError, ( _L( "AtsReceive: remote run for illegal dstid received") ) );
        return KErrGeneral;
        }

    if ( aReq.DstDevId() != DevId() ) 
        {
        // Not our protocol message
        __TRACE( KError, ( _L( "AtsReceive: remote run for illegal dstdevid received") ) );
        return KErrNotFound;
        }

    
    if( aReq.iModule.Length() == 0 )
        {
        __TRACE( KError, ( _L("No mandatory test module name given as run parameter") ) );        
        ret = KErrNotFound;
        }    
    else if( aReq.iTestCaseNumber < 0 )
        {
        __TRACE( KError, ( _L("No mandatory test case number given as run parameter") ) );        
        ret = KErrNotFound;
        }    
    
    testid = 1;
    CStifTFwIfProt* resp = NULL;
    TRAPD( err, 
        resp = CStifTFwIfProt::NewL(); 
        resp->CreateL();
        );
    if( err != KErrNone )
        {
        delete resp;
        return err;
        }

    CStifTFwIfProt* resp2 = NULL;
    TRAP( err, 
        resp2 = CStifTFwIfProt::NewL(); 
        resp2->CreateL();
        );
    if( err != KErrNone )
        {
        delete resp2;
        return err;
        }              

    resp->SetMsgType( CStifTFwIfProt::EMsgResponse );
    resp->SetSrcId( SETID( (TInt32)DevId(), testid ) );
    resp->SetDstId( aReq.SrcId() );
    resp->SetRespType( CStifTFwIfProt::EMsgRemote );
    resp->SetCmdType( CStifTFwIfProt::ECmdRun );        

    resp2->SetMsgType( CStifTFwIfProt::EMsgResponse );
    resp2->SetSrcId( SETID( (TInt32)DevId(), testid ) );
    resp2->SetDstId( aReq.SrcId() );
    resp2->SetRespType( CStifTFwIfProt::EMsgRemote );
    resp2->SetCmdType( CStifTFwIfProt::ECmdRun );        
   
    // Remote run started popup call
    ret = RemotePopup( aContainer, aReq.Message(), resp, EPopupPriorityHighest );
    if( ret != KErrNone )
        {
        delete resp;
        }
    
    // Remote run result popup call
    ret = RemotePopup( aContainer, aReq.Message(), resp2 );
    if( ret != KErrNone )
        {
        delete resp2;
        }
    
    return ret;            
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: MsgRemoteTestCtl

    Description: Remote test control message
    
    Parameters: const TDesC& aMessage: in:

    Return Values: KErrNotSupported

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/        
TInt CUIStore::MsgRemoteTestCtl( CUIEngineContainer* aContainer,
                                 CStifTFwIfProt& aReq, 
                                 CStifTFwIfProt& /*aResp*/ )
    {
    
    TInt ret = KErrNone;    
   
    if( ( aReq.DstDevId() == 0 ) ||
        ( aReq.DstTestId() == 0 ) ) 
        {
        // Protocol violation
        __TRACE( KError, ( _L( "AtsReceive: remote test control for illegal dstid received") ) );
        return KErrGeneral;
        }

    if ( aReq.DstDevId() != DevId() ) 
        {
        // Not our protocol message
        __TRACE( KError, ( _L( "AtsReceive: remote test control for illegal dstdevid received") ) );
        return KErrNotFound;
        }
    
    CStifTFwIfProt* resp = NULL;
    TRAPD( err, 
        resp = CStifTFwIfProt::NewL(); 
        resp->CreateL();
        );
    if( err != KErrNone )
        {
        delete resp;
        return err;
        }
        
    resp->SetMsgType( CStifTFwIfProt::EMsgResponse );
    resp->SetSrcId( aReq.DstId() );
    resp->SetDstId( aReq.SrcId() );
    resp->SetRespType( CStifTFwIfProt::EMsgRemote );
    resp->SetCmdType( aReq.iCmdType );
    
    ret = RemotePopup( aContainer, aReq.Message(), resp );

    if( ret != KErrNone )
        {
        delete resp;
        }

    return ret;
    
    }    

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: MsgRemoteEventCtl

    Description: Remote run message
    
    Parameters: const TDesC& aMessage: in:

    Return Values: KErrNotSupported

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/        
TInt CUIStore::MsgRemoteEventCtl( CUIEngineContainer* aContainer,
                                  CStifTFwIfProt& aReq, 
                                  CStifTFwIfProt& aResp )
    {
    
    if( ( aReq.DstDevId() == 0 ) ||
        ( aReq.DstTestId() != 0 ) ) 
        {
        // Protocol violation
        __TRACE( KError, ( _L( "AtsReceive: remote event control for illegal dstid received") ) );
        return KErrGeneral;
        }

    if ( aReq.DstDevId() != DevId() ) 
        {
        // Not our protocol message
        __TRACE( KError, ( _L( "AtsReceive: remote event control for illegal dstdevid received") ) );
        return KErrNotFound;
        }
        
    if( aReq.iEventName.Length() == 0 )
        {
        __TRACE( KError, ( _L("No event name given") ) );        
        return KErrNotFound;
    }
    
    TInt ret = KErrNone;
                    
    switch( aReq.iCmdType )
        {
        case CStifTFwIfProt::ECmdRequest:
            {
            // Send event active information
            CStifTFwIfProt* resp = NULL;
            TRAP( ret, 
                resp = CStifTFwIfProt::NewL(); 
                resp->CreateL(); );
            if( ret != KErrNone )
                {
                delete resp;
                return ret;
                }
                
            resp->SetMsgType( CStifTFwIfProt::EMsgResponse );
            resp->SetSrcId( aReq.DstId() );
            resp->SetDstId( aReq.SrcId() );
            resp->SetRespType( CStifTFwIfProt::EMsgRemote );
            resp->SetCmdType( CStifTFwIfProt::ECmdRequest );
            resp->Append( CStifTFwIfProt::EventStatus, 
                          CStifTFwIfProt::EEventSet );
            resp->Append( aReq.iEventName );
            ret = RemotePopup( aContainer, aReq.Message(), resp, EPopupPriorityNormal );                

            if( ret != KErrNone )
                {
                delete resp;
                }
                    
            aResp.AppendId( aReq.DstId() );
            aResp.AppendId( aReq.SrcId() );
            aResp.Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRemote );
            aResp.Append( CStifTFwIfProt::CmdType, CStifTFwIfProt::ECmdRequest );
            if( ret == KErrNone )
                {
                aResp.Append( CStifTFwIfProt::EventStatus, 
                              CStifTFwIfProt::EEventActive );
                aResp.Append( aReq.iEventName );
                }
            else 
                {
                aResp.Append( CStifTFwIfProt::EventStatus, 
                              CStifTFwIfProt::EEventError );
                aResp.Append( aReq.iEventName );                              
                aResp.Append( CStifTFwIfProt::EventStatusParams, 
                             CStifTFwIfProt::EEventResult,
                             ret );
                }

            ret = aContainer->RemoteReceive( aResp.Message() );    
            ret = KErrNone;          
            }
            break;
        case CStifTFwIfProt::ECmdRelease:
            {
            // Check and delete all pending event popup windows for test case
            TInt count = iPopups.Count();
            for( TInt i = 0; i<count; i++ )
                {
                if( ( iPopups[i]->Container() == aContainer ) &&
                    iPopups[i]->IsEventPopup() )
                    {
                    delete iPopups[i];
                    iPopups.Remove( i );
                    }
                }
                
            aResp.AppendId( aReq.DstId() );
            aResp.AppendId( aReq.SrcId() );
            aResp.Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRemote );
            aResp.Append( CStifTFwIfProt::CmdType, CStifTFwIfProt::ECmdRelease );
            aResp.Append( aReq.iEventName );

            ret = aContainer->RemoteReceive( aResp.Message() );    
            ret = KErrNone;          
            
            }
            break;
        default:
            return KErrNotSupported;
        }
        
    return ret;
    
    }    

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: MsgRemoteSendReceive

    Description: Asynchronous remote sendreceive message
    
    Parameters: CUIEngineContainer* aContainer: in:
                CStifTFwIfProt& aReq: in
                CStifTFwIfProt& aResp: in

    Return Values: KErrNotSupported

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/        
TInt CUIStore::MsgRemoteSendReceive( CUIEngineContainer* aContainer,
                             CStifTFwIfProt& aReq, 
                             CStifTFwIfProt& aResp )
    {
    
    TInt ret = KErrNone;    
    // TUint16 testid = 0;
    
    if( ( aReq.DstDevId() == 0 ) ||
        ( aReq.DstTestId() != 0 ) ) 
        {
        // Protocol violation
        __TRACE( KError, ( _L( "AtsReceive: remote run for illegal dstid received") ) );
        return KErrGeneral;
        }

    if ( aReq.DstDevId() != DevId() ) 
        {
        // Not our protocol message
        __TRACE( KError, ( _L( "AtsReceive: remote run for illegal dstdevid received") ) );
        return KErrNotFound;
        }

    // testid = 1;
    CStifTFwIfProt* resp = NULL;
    TRAPD( err, 
        resp = CStifTFwIfProt::NewL(); 
        resp->CreateL();
        );
    if( err != KErrNone )
        {
        delete resp;
        return err;
        }
        
    resp->SetMsgType( CStifTFwIfProt::EMsgResponse );
    //resp->SetSrcId( SETID( DevId(), testid ) );
    resp->SetSrcId( aReq.DstId() );
    resp->SetDstId( aReq.SrcId() );
    resp->SetRespType( CStifTFwIfProt::EMsgRemote );
    resp->SetCmdType( CStifTFwIfProt::ECmdSendReceive );        
   
    ret = RemotePopup( aContainer, aReq.Message(), resp );
    if( ret != KErrNone )
        {
        delete resp;
        }
        
    aResp.AppendId( aReq.DstId() );
    aResp.AppendId( aReq.SrcId() );
    aResp.Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRemote );
    aResp.Append( CStifTFwIfProt::CmdType, CStifTFwIfProt::ECmdSendReceive );
    if( ret != KErrNone )
        {
        aResp.Append( CStifTFwIfProt::RunStatus, 
                      CStifTFwIfProt::ERunError );
        aResp.Append( CStifTFwIfProt::RunStatusParams, 
                      CStifTFwIfProt::ERunResult, ret );
        }
    else
        {
        aResp.Append( CStifTFwIfProt::RunStatus, 
                      CStifTFwIfProt::ERunStarted );
        }

    // Send response
    aContainer->RemoteReceive( aResp.Message() );
    
    // Response is created, return success
    ret = KErrNone;        
    
    return ret;
            
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: GoingToReboot

    Description: Reboot indication handling.

    Parameters: CUIEngineContainer* aContainer: in: Container

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIStore::GoingToReboot( CUIEngineContainer* /* aContainer */,
                              TRequestStatus& aStatus )
    {
    
    _LIT( KDateString,"%H%T%S.%C" ); 
       
    const TInt KTimeFieldLength = 30;
    TBuf<KTimeFieldLength> time;
    
    // Store info 
    CStifLogger* logger = CStifLogger::NewL( KUIStoreDefaultDir,
                                             KUIStoreDefaultRebootFile,
                                             CStifLogger::EData,
                                             CStifLogger::EFile,
                                             ETrue,
                                             EFalse,
                                             EFalse,
                                             EFalse,
                                             EFalse,
                                             ETrue );
    
    CleanupStack::PushL( logger );
    TInt count = iStartedTestCases.Count();
    for( TInt index=0; index<count; index++ )
        {
        logger->Log( _L("%S\r\n"), &KUIStoreStartTest );
        
        // First test case info
        logger->Log( _L("%S %S\r\n"), &KUIStoreCaseModuleName, 
            &iStartedTestCases[index]->TestInfo().ModuleName() );
        logger->Log( _L("%S %S\r\n"), &KUIStoreTestCaseTitle, 
            &iStartedTestCases[index]->TestInfo().TestCaseTitle() );
        if( iStartedTestCases[index]->TestInfo().TestCaseFile().Length() > 0 )
            {
            logger->Log( _L("%S %S\r\n"), &KUIStoreTestCaseFile, 
                &iStartedTestCases[index]->TestInfo().TestCaseFile() );
            }
        logger->Log( _L("%S %d\r\n"), &KUIStoreTestCaseNum, 
            iStartedTestCases[index]->TestInfo().TestCaseNum() );
        logger->Log( _L("%S %d\r\n"), &KUIStoreCasePriority, 
            iStartedTestCases[index]->TestInfo().Priority() );
         //@js<--remove--> logger->Log( _L("%S %d %d\r\n"), &KUIStoreCaseTimeout,            
         //@js<--remove--> iStartedTestCases[index]->TestInfo().Timeout().Int64().Low(),
         //@js<--remove--> iStartedTestCases[index]->TestInfo().Timeout().Int64().High() );
        logger->Log( _L("%S %d %d\r\n"), &KUIStoreCaseTimeout,            
            I64LOW(iStartedTestCases[index]->TestInfo().Timeout().Int64()),
            I64HIGH(iStartedTestCases[index]->TestInfo().Timeout().Int64()));     
            
        logger->Log( _L("%S %d\r\n"), &KUIStoreCaseExpectedResult, 
            iStartedTestCases[index]->TestInfo().ExpectedResult() );    
        
        logger->Log( _L("%S %d\r\n"), &KUIStoreCaseStatus, 
            iStartedTestCases[index]->Status() );    

        logger->Log( _L("%S %d %d\r\n"), &KUIStoreCaseExecutionResult, 
            iStartedTestCases[index]->Result().iCaseExecutionResultType,
            iStartedTestCases[index]->Result().iCaseExecutionResultCode );    
        logger->Log( _L("%S %d %S\r\n"), &KUIStoreCaseResult, 
            iStartedTestCases[index]->Result().iTestResult.iResult,
            &iStartedTestCases[index]->Result().iTestResult.iResultDes );    
        
        // Start time
        iStartedTestCases[index]->Result().iStartTime.FormatL(
                    time, KDateString );
        logger->Log( _L("%S %S\r\n"), &KUIStoreCaseStartTime, &time );

        // Start time
        iStartedTestCases[index]->Result().iEndTime.FormatL(
                    time, KDateString );
        logger->Log( _L("%S %S\r\n"), &KUIStoreCaseEndTime, &time );

        logger->Log( _L("%S\r\n\r\n"), &KUIStoreStartTestEnd );
        }
    
    CleanupStack::PopAndDestroy( logger );
    
    // Show popup
    TKeyCode key;
    iUIStoreIf->PopupMsg( _L("GoingToReboot"), 
                          _L("Press any key to continue"), 
                          KRebootPopupTimeout, key, aStatus );
    
    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: LoadExecutedTestCasesL

    Description: Load all executed testcases saved before reboot.
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves on error

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIStore::LoadExecutedTestCasesL()
    {
    TPtrC tmp;
    TInt num = 0;
    TInt high = 0;
    TInt64 interval;
    TFullTestResult result;
    TInt status = 0;
    CStifItemParser* item = NULL;
    TInt ret = KErrNone;

    CStifParser* parser = NULL;
    TRAP( ret,
        parser = CStifParser::NewL( KUIStoreDefaultDir,
                                    KUIStoreDefaultRebootFile );
        );
    if( ret != KErrNone )
        {
        // reboot file not found
        return; 
        }
    CleanupStack::PushL( parser );
   
    __TRACE( KInit, ( _L( "Reboot file is found(%S%S). Reboot testing ongoing..." ), &KUIStoreDefaultDir, &KUIStoreDefaultRebootFile ) );

    CTestInfo* testInfo = NULL;    
     
    CStifSectionParser* section = 
        parser->SectionL( KUIStoreStartTest, KUIStoreStartTestEnd );
   
    while( section )
        {
        CleanupStack::PushL( section );    
        
        testInfo = CTestInfo::NewL();
        CleanupStack::PushL( testInfo ); 
    
        // Get module name
        // Mandatory, leave if not found
        User::LeaveIfError( 
            section->GetLine( KUIStoreCaseModuleName, tmp, ENoTag ) );
        testInfo->SetModuleName( tmp );
    
        // Get test case title
        // Mandatory, leave if not found
        User::LeaveIfError( 
            section->GetLine( KUIStoreTestCaseTitle, tmp, ENoTag ) );
        testInfo->SetTestCaseTitle( tmp );
        
        // Get test case file
        num = section->GetLine( KUIStoreTestCaseFile, tmp, ENoTag );
        if( ( num == KErrNone ) &&
            ( tmp.Length() > 0 ) )
            {
            // Optional
            testInfo->SetTestCaseFile( tmp );
            }
        else
            {
            testInfo->SetTestCaseFile( KNullDesC );
            }
        
        // Get test case number
        item = section->GetItemLineL( KUIStoreTestCaseNum );
        CleanupStack::PushL( item );
        // Mandatory, leave if not found
        User::LeaveIfError( item->GetInt( KUIStoreTestCaseNum, num ));
        testInfo->SetTestCaseNumber( num );
        CleanupStack::PopAndDestroy( item );    
        
        // Get test case priority
        item = section->GetItemLineL( KUIStoreCasePriority );
        CleanupStack::PushL( item );
        // Mandatory, leave if not found
        User::LeaveIfError( item->GetInt( KUIStoreCasePriority, num ));
        testInfo->SetPriority( num );
        CleanupStack::PopAndDestroy( item ); 
        
        // Get test case timeout
        item = section->GetItemLineL( KUIStoreCaseTimeout );
        CleanupStack::PushL( item );
        // Mandatory, leave if not found
        User::LeaveIfError( item->GetInt( KUIStoreCaseTimeout, num ));
        User::LeaveIfError( item->GetNextInt( high ));
        //interval.Set( high, num );
        interval = MAKE_TINT64( high, num );      
        TTimeIntervalMicroSeconds timeout( interval );
        testInfo->SetTimeout( timeout );
        CleanupStack::PopAndDestroy( item ); 
        
        // Get test case status
        item = section->GetItemLineL( KUIStoreCaseStatus );
        CleanupStack::PushL( item );
        // Mandatory, leave if not found
        User::LeaveIfError( item->GetInt( KUIStoreCaseStatus, status ));
        CleanupStack::PopAndDestroy( item ); 

        // Get test case execution result
        item = section->GetItemLineL( KUIStoreCaseExecutionResult );
        CleanupStack::PushL( item );
        // Mandatory, leave if not found
        User::LeaveIfError( item->GetInt( KUIStoreCaseExecutionResult, num ));
        result.iCaseExecutionResultType = ( TFullTestResult::TCaseExecutionResult) num;
        // Mandatory, leave if not found
        User::LeaveIfError( item->GetNextInt( result.iCaseExecutionResultCode ));
        CleanupStack::PopAndDestroy( item );
        
        // Get test case result
        item = section->GetItemLineL( KUIStoreCaseResult );
        CleanupStack::PushL( item );
        // Mandatory, leave if not found
        User::LeaveIfError( item->GetInt( KUIStoreCaseResult, 
                                          result.iTestResult.iResult ));
        // Not mandatory
        TBool first = ETrue;
        result.iTestResult.iResultDes.Zero();
        ret = item->GetNextString( tmp );
        while( ret == KErrNone )
            {
            if( result.iTestResult.iResultDes.Length() + tmp.Length() + 1 > result.iTestResult.iResultDes.MaxLength() )
                {
                User::Leave( KErrGeneral );
                }
            if(!first)
                result.iTestResult.iResultDes.Append(_L(" "));
            result.iTestResult.iResultDes.Append( tmp );
            first = EFalse;
            ret = item->GetNextString( tmp );
            }
        /*
        ret = item->GetNextString( tmp );
        if( ret == KErrNone )
            {
            if( tmp.Length() > result.iTestResult.iResultDes.MaxLength() )
                {
                User::Leave( KErrGeneral );
                }
            result.iTestResult.iResultDes.Copy( tmp );
            }
        */
        CleanupStack::PopAndDestroy( item ); 

        // Get test start time
        item = section->GetItemLineL( KUIStoreCaseStartTime );
        CleanupStack::PushL( item );
        // Mandatory, leave if not found
        User::LeaveIfError( item->GetString( KUIStoreCaseStartTime, tmp ));
        result.iStartTime.Set( tmp );
        CleanupStack::PopAndDestroy( item ); 

        // Get test end time
        item = section->GetItemLineL( KUIStoreCaseEndTime );
        CleanupStack::PushL( item );
        // Mandatory, leave if not found
        User::LeaveIfError( item->GetString( KUIStoreCaseEndTime, tmp ));
        result.iEndTime.Set( tmp );
        CleanupStack::PopAndDestroy( item ); 
             
        if( status != CUIStoreIf::EStatusRunning )
            {
            // Add executed test cases to list
            CStartedTestCase* startedCase = 
                new( ELeave )CStartedTestCase( testInfo, result, status );
            User::LeaveIfError( iStartedTestCases.Append( startedCase ) );            
            CleanupStack::Pop( testInfo );            

            // Fill data with test case info and send to test engine
            TTestInfo *info = new (ELeave) TTestInfo;
            CleanupStack::PushL(info);
            info->iModuleName.Copy(testInfo->ModuleName());
            info->iConfig.Copy(testInfo->TestCaseFile());
            info->iTestCaseInfo.iCaseNumber = testInfo->TestCaseNum();
            info->iTestCaseInfo.iTitle.Copy(testInfo->TestCaseTitle());
            info->iTestCaseInfo.iTimeout = testInfo->Timeout();
            info->iTestCaseInfo.iPriority = testInfo->Priority();
            
            iUIEngine->TestEngine().AddTestCaseResultToTestReport(*info, result, KErrNone);
            CleanupStack::PopAndDestroy(info);
            } 
        else
            {
            // Restart testcase that was running when reset was done
            CUIEngineContainer* container = NULL;
            User::LeaveIfError(  
                iUIEngine->StartTestCase( container, *testInfo ) );
            
            CStartedTestCase* testCase = NULL;
            TRAPD( retVal,
                testCase = CStartedTestCase::NewL( *testInfo, *container );
               );
            if( retVal != KErrNone )
                {
                iUIEngine->AbortStartedTestCase( container );
                User::Leave( retVal );
                }
                
            retVal = iStartedTestCases.Append( testCase );
            if( retVal != KErrNone )
                {
                iUIEngine->AbortStartedTestCase( container );
                delete testCase;
                User::Leave( retVal );
                }

            CleanupStack::PopAndDestroy( testInfo );

            }
        
        CleanupStack::PopAndDestroy( section );    
        section = 
            parser->NextSectionL( KUIStoreStartTest, KUIStoreStartTestEnd );
        } 
    
    CleanupStack::PopAndDestroy( parser );
    
    // Delete file
    RFs rf;
    TInt retVal = rf.Connect();
	if( retVal != KErrNone )
		{
		User::Leave( retVal );
		}
		
    TFileName file( KUIStoreDefaultDir );
    file.Append( KUIStoreDefaultRebootFile );
    rf.Delete( file );
    rf.Close();
    
    return;
    }    
/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: FindByContainer

    Description: Find test case with UIEngine Container pointer.
    
    Parameters:  CUIEngineContainer* const aContainer: in: Execution container
                 CStartedTestCase*& aTestCase: out: Testcase info
    
    Return Values: Symbian OS error code
    
    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/ 
TInt CUIStore::FindByContainer( CUIEngineContainer* const aContainer,
                                CStartedTestCase*& aTestCase )
    {
    
    TInt count = iStartedTestCases.Count();
    TInt index = 0;
    for( ; index < count; index++ )
        {
        if( ( iStartedTestCases[index]->Status() & 
              CUIStoreIf::EStatusRunning ) &&
            ( &iStartedTestCases[index]->UIEngineContainer() ==
              aContainer ) )
            {
            aTestCase = iStartedTestCases[index];
            return KErrNone;
            }
        }
    
    return KErrNotFound;
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: UpdateCases

    Description: Refreshs iTestCases array ie. fetches test cases from 
                test framework

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIStore::UpdateCases()
    {
    
    if( iUpdateNeeded )
        {
        TInt handle = iUIStoreIf->PopupMsg( 
            _L("Updating"), _L("Test cases"), 60 );
        iTestCases.ResetAndDestroy();
        TRAPD( ret, 
            ret = iUIEngine->GetTestCasesL( iTestCases );
            );
        iUpdateNeeded = EFalse;
        // Close popup
        iUIStoreIf->Close( handle );
        if( iTestCases.Count() == 0 )
            {
            iUIStoreIf->PopupMsg( _L("No test cases found"), 
                                  _L("e.g. check TestEngine log"), 
                                  3 );
            }
        return ret;
        }
    
    return KErrNone;
            
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: CheckIniL

    Description: Check ini file

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIStore::CheckIniL( TFileName& aIni )
    {
    
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    RFile file;
    TInt err = KErrNone;
    TErrorNotification error;
    error.iModule.Copy( KUIStore );

    TStifUtil::CorrectFilePathL( aIni );
    
    if ( aIni.Length() != 0 )
        {
        err = file.Open ( fs, aIni, EFileRead );

        if ( err == KErrNone )
            {
            // Use the file given as command line parameter
            iUIStoreIf->PopupMsg( _L("Using given ini-file"), 
                                  KNullDesC, 0 );
            file.Close();
            fs.Close(); 
            return;
            }
        else
            { 
            error.iText.Copy( _L("Can't open given ini-file") );
            iUIStoreIf->Error( error );  
            aIni.Zero();       
            }
        }
        
    // Try to locate default ini file from every drive
    TDriveList drivelist; 
	User::LeaveIfError( fs.DriveList(drivelist) );
	// A TDriveList (the list of available drives), is an array of 
	// 26 bytes. Each byte with a non zero value signifies that the 
	// corresponding drive is available.

    TInt driveNumber; 
	TChar driveLetter;

	for( driveNumber=EDriveA; driveNumber<=EDriveZ; driveNumber++ )
		{
		if( !drivelist[driveNumber] ) 
			{
			// If drive-list entry is zero, drive is not available
			continue;
			}
        User::LeaveIfError(
            fs.DriveToChar( driveNumber, driveLetter ));
        
        aIni.Zero();
        aIni.Append( driveLetter );
        aIni.Append( _L(":") );
        aIni.Append( KDefaultIni );

        // Try to open
        err = file.Open ( fs, aIni, EFileRead );

        if ( err == KErrNone )
            {
            // Use default file
            file.Close();
            TFileName info( _L("Using default ini-file ") );
            if( info.MaxLength()-info.Length() >  aIni.Length() )
                {
                // Show also filename if fits to descriptor
                info.Append( aIni );
                }
            iUIStoreIf->PopupMsg( info, KNullDesC, 0 );
            break;
            }
        }
    if( err != KErrNone )
        {
        iUIStoreIf->PopupMsg( _L("Starting without ini-file"), 
                              KNullDesC,  0);
        aIni.Zero();            
        }

    fs.Close();    

    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: ParseTestSetName

    Description: Parses test set name from test set filename.

    Parameters: const TDesC& aSetFileName: in: Test set filename
                TPtrC& aSetName: out: testset name
                TFileName& aFileName: in: filenamebuffer

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIStore::ParseTestSetName( const TDesC& aSetFileName, 
                                 TPtrC& aSetName,
                                 TFileName& aFileName )
    {
    if( aSetFileName.Length() > KMaxFileName )
        {
        return KErrArgument;
        }
    
    aFileName.Copy( aSetFileName );
    TParsePtr p( aFileName );
    aSetName.Set( p.NameAndExt() ); // gives test set name without path
    
    if( aSetName.Length() > KMaxName )
        {
        return KErrArgument;
        }
    
    return KErrNone;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: RemotePopup

    Description: Do remote message popup 

    Parameters: const TDesC& aReq: in: request

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIStore::RemotePopup( CUIEngineContainer* aContainer,
                            const TDesC& aMsg, 
                            CStifTFwIfProt* aResp,
                            TPopupPriority aPopupPriority )
    {
    TInt ret = KErrNone;
    
    CUIStorePopup* popup = NULL;
    TRAP( ret, popup = CUIStorePopup::NewL( this, aContainer, aResp, aPopupPriority, aMsg ); );
    if( ret != KErrNone )
        {
        return ret;
        }
    ret = iPopups.Append( popup );
    if( ret != KErrNone )
        {
        delete popup;
        return ret;
        }
                
    // We'll put new popup to the top if there are no popups active or if
    // currently active popup has lower priority than the new one               
    if( !iPopupActive || iActivePopupPriority > popup->GetPriority() ) 
        {       
        SetRemotePopupFromQueue();
        }

    return ret;
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: SetRemotePopupFromQueue

    Description: Prints the highest priority popup to the UI from the popup 
    queue

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft @js

-------------------------------------------------------------------------------
*/

void CUIStore::SetRemotePopupFromQueue()
    {
    CUIStorePopup* popup = NULL;
    
    if(iPopups.Count() > 0 )
        {        
        TInt highest = 0;
        
        // Let's get the highest priority popup and print it to the UI
        for( TInt x=0; x<iPopups.Count(); x++)
            {            
            if( iPopups[x]->GetPriority() < iPopups[highest]->GetPriority() )    
                {
                highest = x;
                }
            }
        
        popup = iPopups[highest];
        
        if( popup == NULL )
            {
            // Some weird error
            iPopupActive = EFalse;
            return;        
            }
        
        // Change the active popup priority        
        iActivePopupPriority = popup->GetPriority();             
                
        // Create proper popup        
        if( popup->GetPriority() == EPopupPriorityHighest )
            {                                 
            popup->Start( popup->GetMessage().Right( popup->GetMessage().Length() - KRcpHeaderLen ),
                _L("Press any key to send 'remote run started' message"));                                  
            }
        else if( popup->GetPriority() == EPopupPriorityNormal )
            {
            popup->Start( popup->GetMessage().Right( popup->GetMessage().Length() - KRcpHeaderLen ),
                _L("Press any key to set event") );        
            }
        else if( popup->GetPriority() == EPopupPriorityLow )
            {           
            popup->Start( popup->GetMessage().Right( popup->GetMessage().Length() - KRcpHeaderLen ),
                _L("Press 1-9 to return negative error value, a to switch to another remote run result popup, or any other key to return 0") );                                    
            }
            
        iPopupActive = ETrue;                                
        }
    }
/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: ShuffleRemoteRunResultPopups

    Description: Shuffle remote run result popups 

    Parameters: CUIStorePopup* aPopup (currently active popup)

    Return Values: None

    Errors/Exceptions: None

    Status: Draft @js

-------------------------------------------------------------------------------
*/
void CUIStore::ShuffleRemoteRunResultPopups( CUIStorePopup* aPopup )
    {    
    TInt nextPopupId = -1;
    
    for( TInt x=0; x<iPopups.Count(); x++)
        {            
        // Lets find the next remote run result priority popup
        if( iPopups[x]->GetPriority() == EPopupPriorityLow && iPopups[x] != aPopup )    
            {
            nextPopupId = x;
            break;
            }
        }
    
    if( nextPopupId == -1 )            
        {
        // We'll print error message popup, because there weren't any other remote run 
        // result popups active

        iUIStoreIf->PopupMsg( 
            _L("Error! There weren't any other remote run result popups active."), 
            _L(""), 
            5 );        
        User::After( 5000000 );
        iPopups.Append( aPopup );
        SetRemotePopupFromQueue();
        return;
        }

    // We'll add the popup to the end of the array    
    TInt ret = iPopups.Append( aPopup );
        
    if( ret != KErrNone )
        {
        delete aPopup;
        return;
        }
        
    // Print the next popup from queue
    SetRemotePopupFromQueue();
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: RemotePopup

    Description: Do remote message popup 

    Parameters: const TDesC& aReq: in: request

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIStore::RemotePopupComplete( CUIStorePopup* aPopup, 
                                    TInt aError,
                                    CUIEngineContainer* aContainer,
                                    CStifTFwIfProt* aResp,
                                    TKeyCode aKeyCode )
    {
    
    TInt ret = KErrNone;
    TInt index = iPopups.Find( aPopup );
    if( index < 0 )
        {
        User::Panic( KUIStore, KErrGeneral );
        }
    iPopups.Remove( index );   

    iPopupActive = EFalse;
    iActivePopupPriority = EPopupPriorityLow;

    if( aError != KErrNone )
        {
        ret = aError;
        }
    else
        {
        TChar c( aKeyCode );
        if( c.IsDigit() )
            {
            // Solve return value
            ret = -( aKeyCode - '0' );
            }
        else
            {
            ret = KErrNone;
            }
        }
    // If aResp is given, send it
    if( aResp )
        {     
        switch( aResp->iCmdType )
            {
            case CStifTFwIfProt::ECmdRun:
                {
                TChar response( aKeyCode );  
                RDebug::Print(_L("CUIStore::RemotePopupComplete: user pressed key %c"), (char)response);
                  
                if( aPopup->GetPriority() == EPopupPriorityHighest )
                    {                                        
                    // User has given response to remote run started- popup
                    aResp->Append( CStifTFwIfProt::RunStatus, 
                      CStifTFwIfProt::ERunStarted );                      
                    }                
                else if( aPopup->GetPriority() == EPopupPriorityLow &&  response=='a' )
                    {                                        
                    // User wants to change current remote run result popup to other
                    // remote run result popup. 
                    
                    ShuffleRemoteRunResultPopups( aPopup );                             
                    return KErrNone;
                    }
                else 
                    {                                    
                    // The test case result was given
                    aResp->Append( CStifTFwIfProt::RunStatus, 
                               CStifTFwIfProt::ERunReady );
                    if( ret != KErrNone )
                        {
                        aResp->Append( CStifTFwIfProt::RunStatusParams, 
                                       CStifTFwIfProt::ERunResult, ret );
                        }                   
                    }
                break;
                }
            case CStifTFwIfProt::ECmdSendReceive:
                {
                aResp->Append( CStifTFwIfProt::RunStatus, 
                               CStifTFwIfProt::ERunReady );
                if( ret != KErrNone )
                    {
                    aResp->Append( CStifTFwIfProt::RunStatusParams, 
                                   CStifTFwIfProt::ERunResult, ret );
                    }
                break;
                }
            case CStifTFwIfProt::ECmdPause:
            case CStifTFwIfProt::ECmdResume:
            case CStifTFwIfProt::ECmdCancel:
            case CStifTFwIfProt::ECmdRequest:
            case CStifTFwIfProt::ECmdRelease:
            default:
                if( ret != KErrNone )
                    {
                    aResp->Append( CStifTFwIfProt::RespParam, 
                                   CStifTFwIfProt::ERespResult, 
                                   ret );
                    }
                break;
            }
        
        ret = aContainer->RemoteReceive( aResp->Message() ); 
        }
                
    delete aPopup;      
    
    SetRemotePopupFromQueue();
    
    return ret;
              
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStore

    Method: ReadFiltersL

    Description: Reads filters from test framework initialization file.
        
    Parameters: RPointerArray<TDesC>& aFilters: array to be filled

    Return Values: None

    Errors/Exceptions: 

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C void CUIStore::ReadFiltersL(RPointerArray<TDesC>& aFilters)
    {
    // Clean array
    aFilters.ResetAndDestroy();
    
    // Locate file
    _LIT(KFilterSectionStart, "[Filters]");
    _LIT(KFilterSectionEnd, "[End_Filters]");
    _LIT(KFilterDefinition, "filter=");

    // Parse initialization file
    TInt err = KErrNone;
    CStifParser* parser;
    
    RDebug::Print(_L("STIF: Try to read filters from [%S]"), &iTestFrameworkIni);
    parser = CStifParser::NewL(KNullDesC, iTestFrameworkIni);
    CleanupStack::PushL(parser);

    // Parser created (file exists), create section parser
    CStifSectionParser* section;
    section = parser->SectionL(KFilterSectionStart, KFilterSectionEnd);
    if(section)
        {
        CleanupStack::PushL(section);
            
        // Get item lines
        CStifItemParser* item = section->GetItemLineL(KFilterDefinition, ENoTag);
            
        TPtrC ptr;
            
        while(item)
            {
            CleanupStack::PushL(item);
                
            // Read filter value
            err = item->GetString(KNullDesC, ptr);
            if(err == KErrNone)
                {
                HBufC* filter = ptr.AllocLC();
                User::LeaveIfError(aFilters.Append(filter));
                CleanupStack::Pop();
                }
                
            CleanupStack::PopAndDestroy(item);
            item = NULL;
            item = section->GetNextItemLineL(KFilterDefinition, ENoTag);
            }
            
        CleanupStack::PopAndDestroy(section);
        }
        
    // Clean
    CleanupStack::PopAndDestroy(parser);

    // If there are some filters added, first filter has to be "No filter"
    if(aFilters.Count() > 0)
        {
        RDebug::Print(_L("STIF: Filters loaded"));
        _LIT(KNoFilter, "No filter");
        HBufC* name = KNoFilter().AllocLC();
        User::LeaveIfError(aFilters.Insert(name, 0));
        CleanupStack::Pop(name);    
        }
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CUIStoreIf class member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CUIStoreIf

    Method: ConstructL

    Description: Second phase constructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if..

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C void CUIStoreIf::ConstructL( )
    {
    
    iUIStore = CUIStore::NewL( this );
    
    }


/*
-------------------------------------------------------------------------------

    Class: CUIStoreIf

    Method: CUIStoreIf

    Description: Constructor.
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C CUIStoreIf::CUIStoreIf( )
    {
    }


/*
-------------------------------------------------------------------------------

    Class: CUIStoreIf

    Method: ~CUIStoreIf

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C CUIStoreIf::~CUIStoreIf()
    {	
    
    delete iUIStore;
    iUIStore = NULL;
    
    }


/*
-------------------------------------------------------------------------------

    Class: CUIStoreIf

    Method: UIStore

    Description: Returns reference to CUIStore object, which handles test 
                 cases and test modules.
        
    Parameters: None

    Return Values: CUIStore reference

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C CUIStore& CUIStoreIf::UIStore()
    {
    
    __ASSERT_ALWAYS( iUIStore, User::Panic( KUIStoreIf, KErrNotFound ) );
        
    return *iUIStore;
        
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CTestSetInfo class member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestSetInfo

    Method: NewL

    Description: Construct the CTestSetInfo class

    Parameters: None

    Return Values: CTestSetInfo*                    New object

    Errors/Exceptions: Leaves if memory allocation fails or
                       ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestSetInfo* CTestSetInfo::NewL( const TDesC& aName )
    {
    
    CTestSetInfo* self = new ( ELeave ) CTestSetInfo();
    CleanupStack::PushL( self );
    self->ConstructL( aName );
    CleanupStack::Pop( self );

    return self;

    }
            
/*
-------------------------------------------------------------------------------

    Class: CTestSetInfo

    Method: ConstructL

    Description: Second phase constructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if..

    Status: Draft

-------------------------------------------------------------------------------
*/
void CTestSetInfo::ConstructL( const TDesC& aName )
    {
    
    iName = aName.AllocL();
    
    }


/*
-------------------------------------------------------------------------------

    Class: CTestSetInfo

    Method: CTestSetInfo

    Description: Constructor.
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestSetInfo::CTestSetInfo()
    {
    iLastStartedCaseIndex = 0;
    }


/*
-------------------------------------------------------------------------------

    Class: CTestSetInfo

    Method: ~CTestSetInfo

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestSetInfo::~CTestSetInfo()
    {	
    
    iTestCases.ResetAndDestroy();
    iTestCaseRefs.Reset();
    iTestCases.Close();
    iTestCaseRefs.Close();
    
    delete iName;
    iName = NULL;
    
    }
/*
-------------------------------------------------------------------------------

    Class: CTestSetInfo

    Method: AddTestCase

    Description: Add test case to test set.
        
    Parameters: const CTestInfo& aTestInfo: in: test info

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CTestSetInfo::AddTestCase( const CTestInfo& aTestInfo )
    {
    
    CTestInfo* testInfo = NULL;
    
    TRAPD( err, 
        testInfo = CTestInfo::NewL();
        testInfo->CopyL( aTestInfo );
        );
    if( err != KErrNone )
        {
        return err;
        }

    if( iTestCaseRefs.Append( *testInfo ) != KErrNone )
        {
        delete testInfo;
        return KErrNoMemory;        
        }
    if( iTestCases.Append( testInfo ) != KErrNone ) 
        {
        iTestCaseRefs.Remove( iTestCaseRefs.Count()-1 );
        delete testInfo;
        return KErrNoMemory;
        }

    return KErrNone;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestSetInfo

    Method: InsertTestCase

    Description: Insert test case to test set.
        
    Parameters: const CTestInfo& aTestInfo: in: test info
                TInt aPos: in: position to add

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CTestSetInfo::InsertTestCase( const CTestInfo& aTestInfo, TInt aPos )
    {
    
    if( ( aPos < 0 ) ||
        ( aPos >= iTestCases.Count() ) )
        {
        return KErrArgument;
        }
    
    CTestInfo* testInfo = NULL;
    
    TRAPD( err, 
        testInfo = CTestInfo::NewL();
        testInfo->CopyL( aTestInfo );
        );
    if( err != KErrNone )
        {
        return err;
        }
     
    if( iTestCaseRefs.Insert( *testInfo, aPos ) != KErrNone )
        {
        delete testInfo;
        return KErrNoMemory;        
        }
    if( iTestCases.Insert( testInfo, aPos ) != KErrNone )
        {
        iTestCaseRefs.Remove( aPos );
        delete testInfo;
        return KErrNoMemory;
        }
    
    return err;
        
    }
        
/*
-------------------------------------------------------------------------------

    Class: CTestSetInfo

    Method: RemoveTestCase

    Description: Remove test case from test set.
        
    Parameters: const CTestInfo& aTestInfo: in: test info

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CTestSetInfo::RemoveTestCase( const CTestInfo& aTestInfo )
    {
    
    TInt count = iTestCases.Count();
    for( TInt i=0; i<count; i++ )
        {
        if( aTestInfo == *iTestCases[i] )
            {
            CTestInfo* testInfo = iTestCases[i];
            iTestCases.Remove( i );
            iTestCaseRefs.Remove( i );
            delete testInfo;
            return KErrNone;
            }
        }

    return KErrNotFound;
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestSetInfo

    Method: CopyL

    Description: Remove test case from test set.
        
    Parameters: const CTestInfo& aTestInfo: in: test info

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CTestSetInfo::CopyL( const CTestSetInfo& aTestSetInfo )
    {
    
    iTestCaseRefs.Reset();
    iTestCases.ResetAndDestroy();
    
    TInt count = aTestSetInfo.TestCases().Count();
    for( TInt i=0; i<count; i++ )
        {
        User::LeaveIfError( AddTestCase( aTestSetInfo.TestCases()[i] ) );
        }
    
    delete iName;
    iName = 0;    
    iName = aTestSetInfo.Name().AllocL(); 
    
    iLastStartedCaseIndex = aTestSetInfo.iLastStartedCaseIndex;
         
    }    

/*
-------------------------------------------------------------------------------

    Class: CTestSetInfo

    Method: SetLastStartedCaseIndex

    Description: Sets the info which test case has been started lately.
        
    Parameters: TInt aCaseStarted: index of started test case

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestSetInfo::SetLastStartedCaseIndex(TUint aLastStartedCaseIndex)
    {
    iLastStartedCaseIndex = aLastStartedCaseIndex;
    }    

/*
-------------------------------------------------------------------------------

    Class: CTestSetInfo

    Method: GetLastStartedCaseIndex

    Description: Gets the info which test case has been started lately.
        
    Parameters: None

    Return Values: TInt: index of lately started test case

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TUint CTestSetInfo::GetLastStartedCaseIndex(void)
    {
    return iLastStartedCaseIndex;
    }    

/*
-------------------------------------------------------------------------------

    Class: CTestSetInfo

    Method: RenameTestSet

    Description:  rename test set
        
    Parameters: aTestSetName : new TestSetName

    Return Values: TInt: KErrNone if success.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestSetInfo::RenameTestSet(const TDesC& aTestSetName)
    {
    delete iName;
    iName=NULL;
    iName=aTestSetName.AllocL();
    }
// ================= OTHER EXPORTED FUNCTIONS =================================

// End of File
