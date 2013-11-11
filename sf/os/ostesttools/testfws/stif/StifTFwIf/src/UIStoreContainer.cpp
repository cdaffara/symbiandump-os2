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
* Description: This module contains implementation of 
* CStartedTestCase class member functions.
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32svr.h>

#include <stifinternal/UIEngine.h>
#include <stifinternal/UIEngineContainer.h>
#include <stifinternal/UIStoreContainer.h>
#include <stifinternal/UIStore.h>

#include "Logging.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS
#ifdef LOGGER
#undef LOGGER
#endif
_LIT( KStartedTestCase, "CStartedTestCase" );
_LIT( KStartedTestSet, "CStartedTestSet" );


// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ==================== LOCAL FUNCTIONS =======================================

// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CStartedTestCase

    Method: NewL

    Description: Create a testcase container.

    Parameters: CTestInfo* aTestInfo: in: Test case info
                CUIEngineContainer* aContainer: in: container

    Return Values: CStartedTestCase* : pointer to created object

    Errors/Exceptions: Leaves if memory allocation for object fails
                       Leaves if ConstructL leaves

    Status: Draft

-------------------------------------------------------------------------------
*/
CStartedTestCase* CStartedTestCase::NewL( const CTestInfo& aTestInfo, 
                                          CUIEngineContainer& aContainer )
    {

    CStartedTestCase* self = new ( ELeave ) CStartedTestCase( aContainer );
    CleanupStack::PushL( self );
    self->ConstructL( aTestInfo );
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CStartedTestCase

    Method: ConstructL

    Description: Second level constructor.

    Parameters: CTestInfo* aTestInfo: Test case info
    
    Return Values: None

    Errors/Exceptions: Leaves if CTestInfo copying fails

    Status: Draft

-------------------------------------------------------------------------------
*/
void CStartedTestCase::ConstructL( const CTestInfo& aTestInfo )
    {

    iTestInfo = CTestInfo::NewL();
    iTestInfo->CopyL( aTestInfo );

    }

/*
-------------------------------------------------------------------------------

    Class: CStartedTestCase

    Method: CStartedTestCase

    Description: Constructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CStartedTestCase::CStartedTestCase( CUIEngineContainer& aContainer ):
    iContainer( &aContainer ),
    iStatus( CUIStoreIf::EStatusRunning )
    {

    }
    
/*
-------------------------------------------------------------------------------

    Class: CStartedTestCase

    Method: CStartedTestCase

    Description: Private Constructor for internal use

    Parameters: CTestInfo* aTestInfo: in: new testinfo

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CStartedTestCase::CStartedTestCase( CTestInfo* aTestInfo, 
                                    TFullTestResult aResult, 
                                    TInt aStatus ):
    iContainer( 0 ),
    iTestInfo( aTestInfo ),
    iResult( aResult ),
    iStatus( aStatus )
    {

    }
    
/*
-------------------------------------------------------------------------------

    Class: CStartedTestCase

    Method: ~CStartedTestCase

    Description: Destructor.

    Cancels the asynchronous request, deallocate memory and closes the testcase.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CStartedTestCase::~CStartedTestCase()
    {
    
    delete iTestInfo;
    iPrints.ResetAndDestroy();
    iPrints.Close();
    
    }
 
/*
-------------------------------------------------------------------------------

    Class: CStartedTestCase

    Method: SetResult

    Description: Set the result

    Parameters: const TFullTestResult&     :in:      Result

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CStartedTestCase::SetResult(const TFullTestResult& aResult)
    {

    iResult = aResult;

    iStatus = CUIStoreIf::EStatusExecuted;

    switch( aResult.iCaseExecutionResultType )
        {
        case TFullTestResult::ECaseExecuted:
            if( aResult.iTestResult.iResult == KErrNone )
                {
                iStatus |= CUIStoreIf::EStatusPassed;
                }
            else
                {
                iStatus |= CUIStoreIf::EStatusFailed;
                }
            break;
        case TFullTestResult::ECaseCancelled:
        case TFullTestResult::ECaseTimeout:
        case TFullTestResult::ECaseSuicided:
            iStatus |= CUIStoreIf::EStatusAborted;
            break;
        default:
            iStatus |= CUIStoreIf::EStatusCrashed;
            break;
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CStartedTestCase

    Method: UIEngineContainer

    Description: Returns the UIEngineContainer

    Parameters: None

    Return Values: CUIEngineContainer& 

    Errors/Exceptions: Panics if called for testcase that is not running

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C CUIEngineContainer& CStartedTestCase::UIEngineContainer() const
    { 
    
    if( !( iStatus & CUIStoreIf::EStatusRunning ) ) 
        {
        __RDEBUG( _L("CStartedTestCase::UIEngineContainer for test case that is not running"));
        User::Panic( KStartedTestCase, KErrNotFound );
        }
    
    return *iContainer;
    
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CStartedTestSet class 
    member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CStartedTestSet

    Method: NewL

    Description: Create a testcase container.

    Parameters: const CTestSetInfo& aTestSetInfo: in: test set info
                CUIStore::TSetType aType: in: set type

    Return Values: CStartedTestSet* : pointer to created object

    Errors/Exceptions: Leaves if memory allocation for object fails
                       Leaves if ConstructL leaves

    Status: Draft

-------------------------------------------------------------------------------
*/
CStartedTestSet* CStartedTestSet::NewL( CUIStore* aUIStore, 
                                        const CTestSetInfo& aTestSetInfo,
                                        TSetType aType )
    {

    CStartedTestSet* self = new ( ELeave ) CStartedTestSet( aUIStore, aType );
    CleanupStack::PushL( self );
    self->ConstructL( aTestSetInfo );
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CStartedTestSet

    Method: ConstructL

    Description: Second level constructor.

    Parameters: const CTestSetInfo& aTestSetInfo: in: Test set info
    
    Return Values: None

    Errors/Exceptions: Leaves if CTestSetInfo copying fails

    Status: Draft

-------------------------------------------------------------------------------
*/
void CStartedTestSet::ConstructL( const CTestSetInfo& aTestSetInfo )
    {

    iTestSetInfo = CTestSetInfo::NewL( aTestSetInfo.Name() );
    iTestSetInfo->CopyL( aTestSetInfo );

    iOriginalTestSet = const_cast<CTestSetInfo*>(&aTestSetInfo);
    }

/*
-------------------------------------------------------------------------------

    Class: CStartedTestSet

    Method: CStartedTestSet

    Description: Constructor

    Parameters: CUIStore::TSetType aType: in: set type

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CStartedTestSet::CStartedTestSet( CUIStore* aUIStore, 
                                  TSetType aType ):
    iUIStore( aUIStore ),
    iResultSet( EFalse ),
    iType( aType ),
    iStatus( CUIStoreIf::ESetRunning ),
    iNext( 0 ),
    iRunningCases( 0 ),
    iOriginalTestSet ( NULL )
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CStartedTestSet

    Method: ~CStartedTestSet

    Description: Destructor.

    Cancels the asynchronous request, deallocate memory and closes the testcase.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CStartedTestSet::~CStartedTestSet()
    {
    
    delete iTestSetInfo;
    iTestSetInfo = 0;
    iTestCases.Reset();
    iTestCases.Close();
    
    }
 
/*
-------------------------------------------------------------------------------

    Class: CStartedTestSet

    Method: StartNext

    Description: Start next testcase. 
    
    Parameters: None

    Return Values: KErrNone: test case started
                   KErrNotFound: no test cases found.
                   Other Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CStartedTestSet::StartNext()
    {
    TInt ret = KErrNone;
    
    TInt count = iTestSetInfo->TestCases().Count();
    if( iNext >= count )
        {
        // All test cases running or completed, no more cases to start
        return KErrNotFound;
        }
    
    TInt index = 0;
    if( iType == ESetSequential )
        {
        // Get test case index to be executed (when in NotExecutedCasesMode)
        if(iNotStartedCasesMode)
            {
            iNext = iTestSetInfo->GetLastStartedCaseIndex();

            if(iNext >= count)
                {
                // All test cases running or completed, no more cases to start
                return KErrNotFound;
                }

            iTestSetInfo->SetLastStartedCaseIndex(iNext + 1);
            if(iOriginalTestSet)
                {
                iOriginalTestSet->SetLastStartedCaseIndex(iNext + 1);
                // Update only if original test set exists
                iUIStore->UpdateTestSet(*iOriginalTestSet);
                }
            }

        // Start next one and return
        ret = iUIStore->StartTestCase( iTestSetInfo->TestCases()[iNext],
                                       index );
        if( ret != KErrNone )
            {
            return ret;
            }
        iNext++;
        TRAPD( err,
            CStartedTestCase& testCase = iUIStore->StartedTestCaseL( index );
            ret = iTestCases.Append( testCase );
            );
        if( err != KErrNone )
            {
            User::Panic( KStartedTestSet, err );
            }
        }
    else
        {
        // Start all cases
        for( ; iNext<count; iNext++ )
            {
            ret = iUIStore->StartTestCase( iTestSetInfo->TestCases()[iNext],
                                           index );
            if( ret != KErrNone )
                {
                break;
                }
            TRAPD( err,
                CStartedTestCase& testCase = iUIStore->StartedTestCaseL( index );
                ret = iTestCases.Append( testCase );
                iRunningCases++;
                );
            if( err != KErrNone )
                {
                User::Panic( KStartedTestSet, err );
                }
            if( ret != KErrNone )
                {
                break;
                }
            } 
        }
    
    return ret;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CStartedTestSet

    Method: Abort

    Description: Start next testcase. 
    
    Parameters: None

    Return Values: KErrNone: test case started
                   KErrNotFound: no test cases found.
                   Other Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CStartedTestSet::Abort()
    {
    
    TInt ret = KErrNotFound;
    TInt count = iTestCases.Count();
    for( TInt i=0; i<count; i++ )
        {
        if( iTestCases[i].Status() & 
            CUIStoreIf::EStatusRunning )
            {
            ret = iTestCases[i].UIEngineContainer().CancelTest();
            if( ret != KErrNone )
                {
                break;
                }
            }
        }
    
    // Prevent execution of remaining cases
    count = iTestSetInfo->TestCases().Count();
    iNext = count;
    
    return ret;  
     
    }
    
    
/*
-------------------------------------------------------------------------------

    Class: CStartedTestSet

    Method: IsRunning

    Description: Check if test case running in this test set.
    
    Parameters: const CStartedTestCase* aTestCase: in: test case info

    Return Values: ETrue: This test set is running specified test case
                   EFalse: Not running

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TBool CStartedTestSet::IsRunning( const CStartedTestCase* aTestCase )
    {
    
    TInt count = iTestCases.Count();
    for( TInt i=0; i<count; i++ )
        {
        if( aTestCase == &iTestCases[i] )
            {
            return ETrue;
            }
        }
        
    return EFalse;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CStartedTestSet

    Method: TestCompleted

    Description: Test from this set completed. Start next or 
                set result for set.
    
    Parameters: const CStartedTestCase* aTestCase: in: test case info
                TFullTestResult aResult: in: test result

    Return Values:  Symbian OS error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CStartedTestSet::TestCompleted( const CStartedTestCase* aTestCase, 
                                     TFullTestResult aResult )  
    {
    
    TInt count = iTestCases.Count();
    TInt index = 0;
    for( ; index<count; index++ )
        {
        if( aTestCase == &iTestCases[index] )
            {
            break;
            }
        }
    if( index == count )
        {
        return KErrNotFound;
        }
    
    // Remove from running test cases
    iRunningCases--;
    
    if( iResultSet == EFalse )
        {
        // Check test results
        if( ( aResult.iCaseExecutionResultType != 
              TFullTestResult::ECaseExecuted ) ||
            ( aResult.iTestResult.iResult != KErrNone ) )
            {
            iResult = aResult; 
            iResultSet = ETrue;
            }
        }
        
    // Start next test case
    if( StartNext() == KErrNone )
        {
        // Test case was started
        return KErrNone;
        }
    
    if( iRunningCases > 0 )
        {
        // Test cases still running in test set
        return KErrNone;
        }
    
    iStatus = CUIStoreIf::ESetExecuted;
    if( iResultSet == EFalse )
        {  
        // Use the result of last test case all 
        // test cases where succesfull
        iResult = aResult;
        }
        
    // Set test case completed 
    if( ( iResult.iCaseExecutionResultType != 
          TFullTestResult::ECaseExecuted ) ||
        ( iResult.iTestResult.iResult != KErrNone ) )
        {
        iStatus = CUIStoreIf::ESetFailed;               
        }   
    else
        {
        iStatus = CUIStoreIf::ESetPassed;
        }
        
    return KErrNone;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CStartedTestSet

    Method: SetNotExecutedCasesMode

    Description: Sets mode in which only not executed tests will be run.
    
    Parameters: TBool aNotExecutedCasesMode: desired mode

    Return Values:  None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CStartedTestSet::SetNotStartedCasesMode(TBool aNotStartedCasesMode)
    {
    iNotStartedCasesMode = aNotStartedCasesMode;
    }
    
/*
-------------------------------------------------------------------------------

    Class: CStartedTestSet

    Method: GetOriginalTestSet

    Description: Returns pointer to original test set.
    
    Parameters: None

    Return Values:  Pointer to original test set.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestSetInfo* CStartedTestSet::GetOriginalTestSet()
    {
    return iOriginalTestSet;
    }
    
/*
-------------------------------------------------------------------------------

    Class: CStartedTestSet

    Method: NullOriginalTestSet

    Description: Sets pointer to original test set to NULL.
    
    Parameters: None

    Return Values:  None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CStartedTestSet::NullOriginalTestSet()
    {
    iOriginalTestSet = NULL;
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CTestProgress class 
    member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestProgress

    Method: NewL

    Description: Create CTestProgress.

    Parameters: TTestProgress& aProgress: in: info from test module

    Return Values: CTestProgress* : pointer to created object

    Errors/Exceptions: Leaves if memory allocation for object fails
                       Leaves if ConstructL leaves

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestProgress* CTestProgress::NewL( const TTestProgress& aProgress )
    {

    CTestProgress* self = new ( ELeave ) CTestProgress();
    CleanupStack::PushL( self );
    self->ConstructL( aProgress );
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestProgress

    Method: ConstructL

    Description: Second level constructor.

    Parameters: TTestProgress& aProgress: in: print info
    
    Return Values: None

    Errors/Exceptions: Leaves if TTestProgress copying fails

    Status: Draft

-------------------------------------------------------------------------------
*/
void CTestProgress::ConstructL( const TTestProgress& aProgress  )
    {
    
    iPosition = aProgress.iPosition;
    iDescriptionBuf = aProgress.iDescription.AllocL();
    iTextBuf = aProgress.iText.AllocL();
    iDescription.Set( iDescriptionBuf->Des() );
    iText.Set( iTextBuf->Des() );
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestProgress

    Method: ~CTestProgress

    Description: Destructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestProgress::~CTestProgress()
    {
    
    delete iTextBuf;
    delete iDescriptionBuf;
    
    }
 
/*
-------------------------------------------------------------------------------

    Class: CTestProgress

    Method: ReplaceTextL

    Description: Replace text with ne one.

    Parameters: const TTestProgress& aProgress: in: new info

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CTestProgress::ReplaceTextL( const TTestProgress& aProgress )
    {
    
    HBufC* tmp = iTextBuf;
    iTextBuf = 0;
    CleanupStack::PushL( tmp );
    iTextBuf = aProgress.iText.AllocL();
    iText.Set( iTextBuf->Des() );
    CleanupStack::PopAndDestroy( tmp );
    
    }

// End of File
