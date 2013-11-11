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
* Description: This module contains the implementation of 
* CTestCase class member functions.
*
*/

// INCLUDE FILES
#include "TestCombiner.h"
#include "TestCase.h"
#include "TestCaseNotify.h"
#include "Logging.h"
#include "StifPython.h"

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
#define LOGGER iTestCombiner->iLog

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CTestCase

     Method: CTestCase

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.

     Parameters: CTestCombiner* testCombiner: in: Backpointer to CTestCombiner
                 TDesC& aModuleName: in: name of the used test module
                 TDesC& aTestId: in: TestId for testcase
                 TInt aExpectedResult: in: expected result for the testcase
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/

CTestCase::CTestCase( CTestCombiner* testCombiner,
                      TInt aExpectedResult,
                      TFullTestResult::TCaseExecutionResult aCategory,
                      TCaseType aType,
                      CTCTestModule* aModule ): //--PYTHON
    CActive(  CActive::EPriorityStandard ),
    iType( aType ),
    iTestCombiner( testCombiner ),
    iState( ETestCaseIdle ),
    iExpectedResult( aExpectedResult ),
    iExpectedResultCategory( aCategory ),
    iTestModule( aModule ) //--PYTHON
    {
    
    CActiveScheduler::Add( this );   
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestCase

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.     

     Parameters: TDesC& aTestId: in: Test identifier

     Return Values: None.

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/

void CTestCase::ConstructL( TDesC& aTestId )
    {
    __ASSERT_ALWAYS( aTestId.Length() < KMaxFileName, User::Leave( KErrArgument ) );
    iTestId = aTestId.AllocL();
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestCase

     Method: ~CTestCase

     Description: Destructor
     
     Parameters: None.

     Return Values: None.

     Errors/Exceptions: None.
     
     Status: Proposal
    
-------------------------------------------------------------------------------
*/     

CTestCase::~CTestCase()
    {    

    Cancel();    
    delete iTestId;
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestCase

     Method: RunL

     Description: Derived from CActive. Handles the completed testcase.

     Parameters: None.

     Return Values: None.

     Errors/Exceptions: Leaves if completed with error. 

     Status: Proposal
    
-------------------------------------------------------------------------------
*/

void CTestCase::RunL()
    {
    __TRACE( KMessage, ( _L("CTestCase::RunL: [%d] "), iStatus.Int() ));
    
    Complete( iStatus.Int() );

    __TRACE( KMessage, ( _L( "CTestCase::RunL: Testcase completed" )));

    }

/*
-------------------------------------------------------------------------------

     Class: CTestCase

     Method: DoCancel

     Description: Derived from CActive, handles the Cancel.

     Parameters: None.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestCase::DoCancel()
    {
    __TRACE( KMessage, ( _L("CTestCase::DoCancel")));
    
    TRequestStatus* rs = &iStatus;
    User::RequestComplete( rs, KErrCancel );
    Complete( KErrCancel );
     
    }

/*
-------------------------------------------------------------------------------

     Class: CTestCase

     Method: Start

     Description: Start progress and event notifiers and set CTestCase active.
  
     Parameters: None.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestCase::StartL()
    {

    iState = ETestCaseRunning;
    iStatus = KRequestPending;
    SetActive();
     
    }

/*
-------------------------------------------------------------------------------

     Class: CTestCase

     Method: Complete

     Description: Completes testcase to CTestCombiner.
  
     Parameters: TInt aError: in: Completion error
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestCase::Complete( TInt aError )
    {

    __TRACE( KMessage, ( _L("CTestCase::Complete: %d "), aError ));
    iState = ETestCaseCompleted;
    
    iTestCombiner->Complete( this, aError );
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestCase

     Method: IsCompletelyFinished

     Description: Checks state.

     Parameters: None

     Return Values: TBool.

     Errors/Exceptions: None.

     Status: Proposal

-------------------------------------------------------------------------------
*/
TBool CTestCase::IsCompletelyFinished(void)
    {
    TBool result = (iState == CTestCase::ETestCaseCompleted);
    return result;
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CTCTestCase class 
    member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CTCTestCase

     Method: CTCTestCase

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.

     Parameters: CTestCombiner* testCombiner: in: Backpointer to CTestCombiner
                 TDesC& aModuleName: in: name of the used test module
                 TDesC& aTestId: in: TestId for testcase
                 TInt aExpectedResult: in: expected result for the testcase
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/

CTCTestCase::CTCTestCase( CTestCombiner* testCombiner,
                          TInt aExpectedResult,
                          TFullTestResult::TCaseExecutionResult aCategory,
                          CTCTestModule* aModule ): //--PYTHON
    CTestCase( testCombiner, aExpectedResult, aCategory, ECaseLocal, aModule ), //--PYTHON
    iResultPckg( iResult )
    {
    }

/*
-------------------------------------------------------------------------------

     Class: CTCTestCase

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.     

     Parameters: None. 

     Return Values: None.

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/

void CTCTestCase::ConstructL( TDesC& aModuleName,
                              TDesC& aTestId,
                              const TDesC& aTestCaseArguments )
    {
    __ASSERT_ALWAYS( aModuleName.Length() < KMaxFileName, User::Leave( KErrArgument ) );
    CTestCase::ConstructL( aTestId );
    iModuleName = aModuleName.AllocL();
    iTestCaseArguments = aTestCaseArguments.AllocL();
    }

/*
-------------------------------------------------------------------------------

     Class: CTCTestCase

     Method: NewL

     Description: Two-phased constructor.

     Parameters: CTestCombiner* testCombiner: in: Backpointer to CTestCombiner

     Return Values: CTCTestCase*: pointer to new CTCTestCase object

     Errors/Exceptions: Leaves if new or ConstructL leaves.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/

CTCTestCase* CTCTestCase::NewL( CTestCombiner* testCombiner,
                                TDesC& aModuleName,
                                TDesC& aTestId,
                                TInt aExpectedResult,
                                TFullTestResult::TCaseExecutionResult aCategory,
                                const TDesC& aTestCaseArguments,
                                CTCTestModule* aModule ) //--PYTHON
    {
    CTCTestCase* self = new (ELeave) CTCTestCase( testCombiner,
                                                   aExpectedResult,
                                                   aCategory,
                                                   aModule ); //--PYTHON
     
    CleanupStack::PushL( self );
    self->ConstructL( aModuleName, aTestId, aTestCaseArguments );
    CleanupStack::Pop();
    return self;
     
    }

/*
-------------------------------------------------------------------------------

     Class: CTCTestCase

     Method: ~CTCTestCase

     Description: Destructor
     
     Parameters: None.

     Return Values: None.

     Errors/Exceptions: None.
     
     Status: Proposal
    
-------------------------------------------------------------------------------
*/     

CTCTestCase::~CTCTestCase()
    {    
    Cancel();
    
    delete iEvent;
    
    // Unset all set events
    TInt count = iStateEventArray.Count();
    TInt i = 0;
    for( i = 0; i < count; i++ )
        {
		if( iStateEventArray[i] != NULL )
			{
			iStateEventArray[i]->SetType( TEventIf::EUnsetEvent );
			}
		if( iTestCombiner != NULL )
			{
			iTestCombiner->TestModuleIf().Event( *iStateEventArray[i] );
			}
        }
    iStateEventArray.ResetAndDestroy();
    iStateEventArray.Close();
    
    // to be sure..
    count = iEventArray.Count();
    TEventIf event;
    for( i = 0; i < count; i++ )
        {
        event.Copy( iEventArray[i]->Event() );
        event.SetType( TEventIf::ERelEvent );
		if( iTestCombiner != NULL )
			{
			iTestCombiner->TestModuleIf().Event( event );
			}
        }


    iEventArray.ResetAndDestroy();
    iEventArray.Close();
    
    delete iProgress;
    delete iModuleName;
    delete iTestCaseArguments;
    
    delete iCommand;
    }

/*
-------------------------------------------------------------------------------

     Class: CTCTestCase

     Method: TestCaseArguments

     Description: Get test case arguments

     Parameters: None.

     Return Values: Test case arguments.

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
const TDesC& CTCTestCase::TestCaseArguments() const 
    {
    if ( iTestCaseArguments !=NULL ) 
        {
        return *iTestCaseArguments; 
        }
    return KNullDesC;
    }

/*
-------------------------------------------------------------------------------

     Class: CTCTestCase

     Method: DoCancel

     Description: Derived from CActive, handles the Cancel.

     Parameters: None.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTCTestCase::DoCancel()
    {
    __TRACE( KMessage, ( _L("CTCTestCase::DoCancel")));
    iTestExecution.CancelAsyncRequest( ETestExecutionRunTestCase );
    Complete( KErrCancel );
     
    }    

/*
-------------------------------------------------------------------------------

     Class: CTCTestCase

     Method: Start

     Description: Start progress and event notifiers and set CTCTestCase active.
  
     Parameters: None.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTCTestCase::StartL()
    {
    iProgress = CTestProgressNotifier::NewL( iTestCombiner, this );
    iEvent = CTestEventNotifier::NewL( iTestCombiner, this );
    iCommand = CTestCommandNotifier::NewL(iTestCombiner, this);
    iState = ETestCaseRunning;
    SetActive();
     
    }

/*
-------------------------------------------------------------------------------

     Class: CTCTestCase

     Method: Complete

     Description: Start complete the testcase to CTestCombiner (Complete2 make
                  the final complete).
  
     Parameters: TInt aError: in: Completion error
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTCTestCase::Complete( TInt aError )
    {
    if( iState == ETestCaseRunning )
        {
        // Testcase is in running state, complete and delete notifiers
        if( aError != KErrNone )
            {
            iResult.iTestResult.iResult = aError;
            }
            
        iState = ETestCaseCompleted;
        __TRACE( KMessage, ( _L("CTCTestCase::Complete: %d "), aError ));
        
        }

	if( iProgress->IsPrintProgressFinished() )
        {
        // Print process is finished to UI => make final complete of
        // the test case
		Complete2();
        }

    }

/*
-------------------------------------------------------------------------------

     Class: CTCTestCase

     Method: Complete2

     Description: Testcase final complete to CTestCombiner.
  
     Parameters: None
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTCTestCase::Complete2()
    {
 	// to be sure..
    TInt count = iEventArray.Count();
    TEventIf event;
    for( TInt i = 0; i < count; i++ )
        {
        event.Copy( iEventArray[0]->Event() );
        event.SetType( TEventIf::ERelEvent );
        CTestEventNotify* eventEntry = iEventArray[0];
        iEventArray.Remove( 0 );
        delete eventEntry;
        iTestCombiner->TestModuleIf().Event( event );
        }
      
    iEventArray.ResetAndDestroy();
    iEventArray.Close();
    
    delete iEvent;
    iEvent = NULL;
    delete iProgress;
    iProgress = NULL;
    delete iCommand;
    iCommand = NULL;

    iTestCombiner->Complete( this );

    // Close execution handle
    iTestExecution.Close();
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTCTestCase

     Method: IsCompletelyFinished

     Description: Checks state and progress notifier.

     Parameters: None

     Return Values: TBool.

     Errors/Exceptions: None.

     Status: Proposal

-------------------------------------------------------------------------------
*/
TBool CTCTestCase::IsCompletelyFinished(void)
    {
    TBool result = (iState == CTCTestCase::ETestCaseCompleted);
	if(iProgress)
	    {
		result = result && (iProgress->IsPrintProgressFinished());
		}
    return result;
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CRemoteTestCase class 
    member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CRemoteTestCase

     Method: CRemoteTestCase

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.

     Parameters: CTestCombiner* testCombiner: in: Backpointer to CTestCombiner
                 TInt aExpectedResult: in: expected result for the testcase
                 TFullTestResult::TCaseExecutionResult aCategory: in: 
                    result category
     Return Values: None.

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/

CRemoteTestCase::CRemoteTestCase( CTestCombiner* testCombiner,
                          TInt aExpectedResult,
                          TFullTestResult::TCaseExecutionResult aCategory ):
    CTestCase( testCombiner, aExpectedResult, aCategory, ECaseRemote, NULL ), //--PYTHON
    iRemoteState( ECaseIdle ),
    iFreeSlave( EFalse ) 
    {
    }

/*
-------------------------------------------------------------------------------

     Class: CRemoteTestCase

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.     

     Parameters: None. 

     Return Values: None.

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/

void CRemoteTestCase::ConstructL( TDesC& aTestId )
    {
    CTestCase::ConstructL( aTestId );
    
    }

/*
-------------------------------------------------------------------------------

     Class: CRemoteTestCase

     Method: NewL

     Description: Two-phased constructor.

     Parameters: CTestCombiner* testCombiner: in: Backpointer to CTestCombiner

     Return Values: CRemoteTestCase*: pointer to new CRemoteTestCase object

     Errors/Exceptions: Leaves if new or ConstructL leaves.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/

CRemoteTestCase* CRemoteTestCase::NewL( CTestCombiner* testCombiner,
                                TDesC& aTestId, 
                                TInt aExpectedResult,
                                TFullTestResult::TCaseExecutionResult 
                                    aCategory )
    {
    CRemoteTestCase* self = new (ELeave) CRemoteTestCase( testCombiner, 
                                                   aExpectedResult,
                                                   aCategory );
     
    CleanupStack::PushL( self );
    self->ConstructL( aTestId );
    CleanupStack::Pop();
    return self;
     
    }

/*
-------------------------------------------------------------------------------

     Class: CRemoteTestCase

     Method: ~CRemoteTestCase

     Description: Destructor
     
     Parameters: None.

     Return Values: None.

     Errors/Exceptions: None.
     
     Status: Proposal
    
-------------------------------------------------------------------------------
*/     

CRemoteTestCase::~CRemoteTestCase()
    {        
    }
    
/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CTCTestModule class 
    member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CTCTestModule

     Method: CTCTestModule

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.

     Parameters: TDesC& aModule: in: module name
                 TDesC& aIni: in: inifile name
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
CTCTestModule::CTCTestModule( CTestCombiner* testCombiner ):
    iTestCombiner( testCombiner )
    {
    iTestCases = NULL;
    }

/*
-------------------------------------------------------------------------------

     Class: CTCTestModule

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.     

     Parameters: TDesC& aModule: in: Module name.
                 TDesC& aIni: in: Inifile name.
                 const TDesC& aConfigFile: in: Test case(config) file name. 

     Return Values: None.

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/

void CTCTestModule::ConstructL( TDesC& aModule,
                                TDesC& aIni,
                                const TDesC& aConfigFile )
    {
    __TRACE( KMessage, ( _L("CTCTestModule::ConstructL [%S]"), &aModule));
    
    __ASSERT_ALWAYS( aModule.Length() < KMaxFileName, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( aIni.Length() < KMaxFileName, User::Leave( KErrArgument ) );
    iModuleName = aModule.AllocL();
    iIniFile = aIni.AllocL();
    
    TFileName ini = *iIniFile;
    // Open handles
    __TRACE( KMessage, ( _L("CTCTestModule: Server connect")));    
    User::LeaveIfError( iTestServer.Connect( ModuleName(), aConfigFile ) ); 
    __TRACE( KMessage, ( _L("CTCTestModule: Module open")));
    User::LeaveIfError( iTestModule.Open( iTestServer, ini ) );
    
    iErrorPrinter = CTestErrorNotifier::NewL( iTestCombiner, this );
  
    }

/*
-------------------------------------------------------------------------------

     Class: CTCTestModule

     Method: NewL

     Description: Two-phased constructor.

     Parameters: None
     
     Return Values: TDesC& aModule: in: Module name.
                    TDesC& aIni: in: Inifile name.
                    const TDesC& aConfigFile: in: Test case(config) file name.
     
     Errors/Exceptions: Leaves if ConstructL leaves
                        Leaves if memory allocation fails
                        Leaves if illegal arguments are given

     Status: Proposal
    
-------------------------------------------------------------------------------
*/

CTCTestModule* CTCTestModule::NewL( CTestCombiner* testCombiner,
                                    TDesC& aModule, 
                                    TDesC& aIni,
                                    const TDesC& aConfigFile )
    {
    CTCTestModule* self = new (ELeave) CTCTestModule( testCombiner );
    
    CleanupStack::PushL( self );
    self->ConstructL( aModule, aIni, aConfigFile );
    CleanupStack::Pop();

    return self;
     
    }

/*

-------------------------------------------------------------------------------

     Class: CTCTestModule

     Method: ~CTCTestModule

     Description: Destructor
     
     Parameters:    None.

     Return Values: None.

     Errors/Exceptions: None.
     
     Status: Approved
    
-------------------------------------------------------------------------------
*/     

CTCTestModule::~CTCTestModule()
    {
    __TRACE( KMessage, ( _L("CTCTestModule::~CTCTestModule [%S]"), iModuleName));
    
    // delete error printer
    delete iErrorPrinter;
    // Close handles
    if( iTestServer.Handle() != 0 )
        {
        iTestModule.Close();
        iTestServer.Close();
        }
    delete iModuleName;
    delete iIniFile;
    delete iTestCases;
    }

/*
-------------------------------------------------------------------------------

     Class: CTCTestModule

     Method: GetTestCasesForCombiner

     Description: Get array of test cases (used to find test case number by title)
     
     Parameters:    const TDesC& aConfigFile: in: config file name.

     Return Values: None.

     Errors/Exceptions: None.
     
     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTCTestModule::GetTestCasesForCombiner(const TDesC& aConfigFile)
    {
    __TRACE(KMessage, (_L("Going to enumerate test cases for TestCombiner")));
    TName configFile(aConfigFile);

    // Enumerate test cases
    TCaseCount caseCount;
    TRequestStatus status;
    iTestModule.EnumerateTestCases(configFile, caseCount, status);
    User::WaitForRequest(status);

    // Check that enumerate succeeded
    if(status != KErrNone)
        {
        __TRACE(KMessage, (_L("Enumerating test cases from module returned error. Status %d."), status.Int()));
        return;
    	}
    __TRACE(KMessage, (_L("Enumeration for TestCombiner returned %d cases"), caseCount()));

    // Get test cases to buffer
    iTestCases = CFixedFlatArray<TTestCaseInfo>::NewL(caseCount());

    TInt ret = iTestModule.GetTestCases(*iTestCases);
    if(ret != KErrNone)
        {
        __TRACE(KMessage, (_L("Getting test cases from module returned error %d."), ret));
        return;
        }

    __TRACE(KMessage, (_L("Test cases for TestCombiner have been enumerated")));
    }


/*
-------------------------------------------------------------------------------

     Class: CTCTestModule

     Method: GetCaseNumByTitle

     Description: Finds test case index by title

     Parameters:    TDesC& aTitle: in: test case title.
                    TInt& aCaseNum: in out: test case index

     Return Values: KErrNone if everything is ok
                    KErrNotFound if there is no such test case

     Errors/Exceptions: None.

     Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTCTestModule::GetCaseNumByTitle(TDesC& aTitle, TInt& aCaseNum)
    {
    for(TInt i = 0; i < iTestCases->Count(); i++)
        {
        if((*iTestCases)[i].iTitle == aTitle)
            {
            aCaseNum = i;
            //TestScripter relies on 1-based indexing
            if((iModuleName->Find(KTestScripterName) != KErrNotFound) || (iModuleName->Find(KPythonScripter) != KErrNotFound) || (iModuleName->Find(KTestCombinerName) != KErrNotFound))
                aCaseNum++;
            return KErrNone;
            }
        }
    
    return KErrNotFound;
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CRemoteSendReceive class 
    member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CRemoteSendReceive

     Method: CRemoteSendReceive

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.

     Parameters: 

     Return Values: None.

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
CRemoteSendReceive::CRemoteSendReceive( CTestCombiner* aTestCombiner  ) :
    iTestCombiner( aTestCombiner )
    {
    // None

    }

/*
-------------------------------------------------------------------------------

     Class: CRemoteSendReceive

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.     

     Parameters: None. 

     Return Values: None.

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CRemoteSendReceive::ConstructL()
    {
    // None
    
    }

/*
-------------------------------------------------------------------------------

     Class: CRemoteSendReceive

     Method: NewL

     Description: Two-phased constructor.

     Parameters: 

     Return Values: CRemoteTestCase*: pointer to new CRemoteTestCase object

     Errors/Exceptions: Leaves if new or ConstructL leaves.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
CRemoteSendReceive* CRemoteSendReceive::NewL( CTestCombiner* aTestCombiner )
    {
    CRemoteSendReceive* self = new (ELeave) CRemoteSendReceive( aTestCombiner );
     
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
     
    }

/*
-------------------------------------------------------------------------------

     Class: CRemoteSendReceive

     Method: ~CRemoteSendReceive

     Description: Destructor
     
     Parameters: None.

     Return Values: None.

     Errors/Exceptions: None.
     
     Status: Proposal
    
-------------------------------------------------------------------------------
*/     
CRemoteSendReceive::~CRemoteSendReceive()
    {
    // None

    }

// End of File
