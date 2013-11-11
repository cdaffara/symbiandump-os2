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
* Description:  
*
*/

#include "symbianunittestrunner.h"
#include "symbianunittestuicallback.h"
#include "symbianunittestoutputformatter.h"
#include "symbianunittestoutputfactory.h"
#include "symbianunittestresult.h"
#include "symbianunittestversion.h"
#include "sutlogger.h"
#include "symbianunittestlddctl.h"
#include <symbianunittestinterface.h>
#include <e32uid.h>


// Failures while loading dll:
_LIT( KFailedToFindDll, "Failed to find DLL:\n\"%S\"\n" );
_LIT( KFailedToLoadLDD, "Failed to load LDD:\n\"%S\"\n" );
_LIT( KFailedToRunLDD, "Failed to run LDD tests:\n\"%S\"\n" );
_LIT( KNonCompatibleUIDs, "Cannot use DLL with non-compatible UIDs!\n" );
_LIT( KExportFuncNotFound, "Cannot find EXPORT function from test DLL!\n" );
_LIT( KLogVersion, "SymbianUnitTest v%d.%d.%d" );
_LIT( KLogFinish, "SymbianUnitTest finished" );

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C CSymbianUnitTestRunner* CSymbianUnitTestRunner::NewLC( 
    MSymbianUnitTestUiCallBack& aUiCallBack )
    {
    CSymbianUnitTestRunner* self = 
        new( ELeave )CSymbianUnitTestRunner( aUiCallBack );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C CSymbianUnitTestRunner* CSymbianUnitTestRunner::NewL( 
    MSymbianUnitTestUiCallBack& aUiCallBack )
    {
    CSymbianUnitTestRunner* self = CSymbianUnitTestRunner::NewLC( aUiCallBack );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestRunner::CSymbianUnitTestRunner( 
    MSymbianUnitTestUiCallBack& aUiCallBack )
    : iUiCallBack( aUiCallBack )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestRunner::ConstructL()
    {
    iResult = CSymbianUnitTestResult::NewL();
    //init logger
    TBuf<50> version;
    version.Format(KLogVersion, SUT_MAJOR_VERSION, SUT_MINOR_VERSION, SUT_BUILD_VERSION);
    SUT_LOG_START(version);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C CSymbianUnitTestRunner::~CSymbianUnitTestRunner()
    {
    delete iResult;
    }

// -----------------------------------------------------------------------------
// From MSymbianUnitTestObserver
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestRunner::TestPass(const TDesC& aTestCaseName)
    {
    iUiCallBack.TestPass(aTestCaseName);
    }

// -----------------------------------------------------------------------------
// From MSymbianUnitTestObserver
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestRunner::TestFailed(const TDesC& aTestCaseName, const TDesC8& aErrMsg)
    {
    iUiCallBack.TestFailed(aTestCaseName, aErrMsg);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSymbianUnitTestRunner::TestCount()
    {
    return iTestCount;
    }

EXPORT_C TInt CSymbianUnitTestRunner::TestCountL(const MDesCArray& aTestDllNames, const MDesCArray& aTestCaseNames)
    {
    TInt dllCnt = aTestDllNames.MdcaCount(); 
    if (dllCnt == 0)
        {
        return 0;
        }
    TInt caseCnt = aTestCaseNames.MdcaCount();
    if (caseCnt != 0)
        {
        return caseCnt;
        }
    else
        {
        for ( TInt i = 0; i < dllCnt; i++)
            {
            CDesCArray* testCaseNames = new (ELeave) CDesCArrayFlat(1);
            CleanupStack::PushL(testCaseNames);
            this->TestCaseNamesL(aTestDllNames.MdcaPoint(i), *testCaseNames);
            caseCnt += testCaseNames->Count();
            CleanupStack::PopAndDestroy(testCaseNames);
            }
        return caseCnt;
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTestRunner::ExecuteTestsL(
    const MDesCArray& aTestDllNames,
    TBool aMemoryAllocationFailureSimulation,
    const TDesC& aOutputFileName,
    const TDesC& aOutputFormat,
    const CDesCArray& aTestCaseNames,
    TInt  aTimeout )
    {
    iTestCount = 0;
    
    MSymbianUnitTestInterface::TFailureSimulation failureSimulation = 
        MSymbianUnitTestInterface::ENoFailureSimulation;
    if ( aMemoryAllocationFailureSimulation )
        {
        failureSimulation = 
            MSymbianUnitTestInterface::EMemAllocFailureSimulation;
        }
    
    for ( TInt i = 0; i < aTestDllNames.MdcaCount(); i++ )
        {
        TPtrC16 testDllName( aTestDllNames.MdcaPoint( i ) );
        if (testDllName.Find(_L(".ldd"))>0)
            {
            ExecuteLddTestsL(testDllName, aTestCaseNames, aTimeout);
            }
        else
            {
            //it is dll test
            ExecuteDllTestsL(testDllName, failureSimulation, aTestCaseNames, aTimeout);
            }
        }
    
    CSymbianUnitTestOutputFormatter* outputFormatter = 
        SymbianUnitTestOutputFactory::CreateOutputLC( 
            aOutputFileName, aOutputFormat );
    outputFormatter->PrintL( *iResult );
    CleanupStack::PopAndDestroy( outputFormatter );    
    SUT_LOG_INFO(KLogFinish);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSymbianUnitTestRunner::FailedTestCount()
    {
    return iResult->Failures().Count();
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTestRunner::TestCaseNamesL(
    const TDesC& aTestDllName, CDesCArray& aTestCaseNames)
    {
    
    if (aTestDllName.Find(_L(".ldd"))>0)
        {
        //it is kernel ldd test
        return GetLddTestCaseNamesL(aTestDllName, aTestCaseNames);
        }
    else
        {
        //it is dll test
        return GetDllTestCaseNamesL(aTestDllName, aTestCaseNames);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestRunner::GetDllTestCaseNamesL(
        const TDesC& aTestDllName, CDesCArray& aTestCaseNames)
    {
    RLibrary library;
    TInt ret;
    ret = library.Load(aTestDllName);
    if (ret != KErrNone)
        {
        iUiCallBack.InfoMsg(KFailedToFindDll, aTestDllName);
        SUT_LOG_FORMAT(KFailedToFindDll, &aTestDllName);
        User::Leave(ret);
        }
    CleanupClosePushL(library);
    // The second UID of the dll to be used must be compatible
    if (library.Type()[1] != KSymbianUnitTestDllUid)
        {
        iUiCallBack.InfoMsg(KNonCompatibleUIDs);
        User::Leave(KErrNotFound);
        }
    TLibraryFunction entryFunction = library.Lookup(1);
    if (!entryFunction)
        {
        iUiCallBack.InfoMsg(KExportFuncNotFound);
        User::Leave(KErrNotFound);
        }

    MSymbianUnitTestInterface* test =
            reinterpret_cast<MSymbianUnitTestInterface*> (entryFunction());
    
    TCleanupItem cleanupItem(DeleteTest, test);
    CleanupStack::PushL(cleanupItem);
    
    test->TestCaseNamesL(aTestCaseNames);
    CleanupStack::Pop(); // cleanupItem
    CleanupStack::PopAndDestroy(&library);
  
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestRunner::GetLddTestCaseNamesL(
        const TDesC& aTestLddName, CDesCArray& aTestCaseNames)
    {
    CSymbianUnitTestLddCtl* lddCtl = CSymbianUnitTestLddCtl::NewLC(
            aTestLddName);
    TInt ret = lddCtl->LoadDriver();
    if (ret != KErrNone)
        {
        iUiCallBack.InfoMsg(KFailedToLoadLDD, aTestLddName);
        SUT_LOG_FORMAT(KFailedToLoadLDD, &aTestLddName);
        User::Leave(ret);
        }

    ret = lddCtl->GetTestCaseNames(aTestCaseNames); 
    if (ret != KErrNone)
        {
        iUiCallBack.InfoMsg(KFailedToRunLDD, aTestLddName);
        SUT_LOG_FORMAT(KFailedToRunLDD, &aTestLddName);
        User::Leave(ret);
        }
    lddCtl->FreeDriver();
    CleanupStack::PopAndDestroy(lddCtl);
    SUT_LOG_FORMAT(_L(" case name count:%d\n"), aTestCaseNames.Length());
    for (TInt i = 0; i < aTestCaseNames.Length(); i++)
        {
            SUT_LOG_FORMAT(_L(" case:%S\n"), &aTestCaseNames[i]);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestRunner::DeleteTest( TAny* aTest )
    {
    MSymbianUnitTestInterface* test = 
        reinterpret_cast< MSymbianUnitTestInterface* >( aTest );
    delete test;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestRunner::ExecuteDllTestsL(const TDesC& aTestDllName, MSymbianUnitTestInterface::TFailureSimulation aFailureSimulation,
        const CDesCArray& aTestCaseNames, TInt  aTimeout)
    {
    RLibrary library;
    TInt ret;
    ret = library.Load(aTestDllName);
    if (ret != KErrNone)
        {
        iUiCallBack.InfoMsg(KFailedToFindDll, aTestDllName);
        SUT_LOG_FORMAT(KFailedToFindDll, &aTestDllName);
        User::Leave(ret);
        }
    CleanupClosePushL(library);
    // The second UID of the dll to be used must be compatible
    if (library.Type()[1] != KSymbianUnitTestDllUid)
        {
        iUiCallBack.InfoMsg(KNonCompatibleUIDs);
        User::Leave(KErrNotFound);
        }
    TLibraryFunction entryFunction = library.Lookup(1);
    if (!entryFunction)
        {
        iUiCallBack.InfoMsg(KExportFuncNotFound);
        User::Leave(KErrNotFound);
        }

    MSymbianUnitTestInterface* test =
            reinterpret_cast<MSymbianUnitTestInterface*> (entryFunction());
    TCleanupItem cleanupItem(DeleteTest, test);
    CleanupStack::PushL(cleanupItem);
    iTestCount += test->TestCaseCount();
    test->ExecuteL(*this, *iResult, aFailureSimulation, aTestCaseNames,
            aTimeout);
    CleanupStack::Pop(); // cleanupItem
    CleanupStack::PopAndDestroy(&library);
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestRunner::ExecuteLddTestsL(const TDesC& aTestLddName, const CDesCArray& /*aTestCaseNames*/,
        TInt  /*aTimeout*/)
    {
    CSymbianUnitTestLddCtl* lddCtl = CSymbianUnitTestLddCtl::NewLC( aTestLddName );
    TInt ret = lddCtl->LoadDriver();
    if (ret != KErrNone)
        {
        iUiCallBack.InfoMsg(KFailedToLoadLDD, aTestLddName);
        SUT_LOG_FORMAT(KFailedToLoadLDD, &aTestLddName);
        User::Leave(ret);
        }
    
    ret = lddCtl->ExecuteLddTests(*this, *iResult);
    if (ret != KErrNone)
            {
            iUiCallBack.InfoMsg(KFailedToRunLDD, aTestLddName);
            SUT_LOG_FORMAT(KFailedToRunLDD, &aTestLddName);
            User::Leave(ret);
            }
    lddCtl->FreeDriver();
    CleanupStack::PopAndDestroy( lddCtl );
    }
