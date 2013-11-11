// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <e32base.h>
#include <e32def.h>
#include <e32def_private.h>
#include <e32debug.h>
#include "symbianunittestlddctl.h"
#include "symbianunittestlddif.h"
#include "sutlogger.h"

const TInt KTestCaseNameLength = 256;

TInt RSymbianUnitTest::GetTestCaseCount(TInt* a1)
    {
    return RBusLogicalChannel::DoControl(RSymbianUnitTest::ETESTCOUNT, a1);
    }

TInt RSymbianUnitTest::ExecTestCase(TInt* a1, TAny* a2)
    {
    return RBusLogicalChannel::DoControl(RSymbianUnitTest::EEXECUTETESTCASE, a1, a2);
    }

TInt RSymbianUnitTest::GetTestCaseName(TInt* a1, TAny* a2)
    {
    return RBusLogicalChannel::DoControl(RSymbianUnitTest::ETESTCASENAME, a1, a2);
    }

TInt RSymbianUnitTest::Open(const TDesC& aName)
	{
	return DoCreate(aName,TVersion(0,0,1),KNullUnit,NULL,NULL);
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestLddCtl* CSymbianUnitTestLddCtl::NewLC(const TDesC& aDriverName)
    {
    CSymbianUnitTestLddCtl* self = new( ELeave )CSymbianUnitTestLddCtl();
    CleanupStack::PushL( self );
    self->ConstructL(aDriverName);
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestLddCtl* CSymbianUnitTestLddCtl::NewL(const TDesC& aDriverName)
    {
    CSymbianUnitTestLddCtl* self = CSymbianUnitTestLddCtl::NewLC(aDriverName);
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestLddCtl::CSymbianUnitTestLddCtl() 
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestLddCtl::ConstructL(const TDesC& aDriverName)
    {
    iDriverName = aDriverName.AllocL();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestLddCtl::~CSymbianUnitTestLddCtl()
    {
    delete iDriverName;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CSymbianUnitTestLddCtl::LoadDriver()
    {
    return User::LoadLogicalDevice(*iDriverName);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CSymbianUnitTestLddCtl::FreeDriver()
    {
    return User::FreeLogicalDevice(*iDriverName);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CSymbianUnitTestLddCtl::ExecuteLddTests(
        MSymbianUnitTestObserver& aObserver,
        CSymbianUnitTestResult& aResult)
    {

    TInt ret = iLddconn.Open(*iDriverName);
    if (ret != KErrNone)
        {
        return ret;
        }
    SUT_LOG_DEBUG("open ldd");

    TInt count;
    ret = iLddconn.GetTestCaseCount(&count);
    SUT_LOG_DEBUGF(_L("user space, test case count:%d"), count);
    if (ret != KErrNone)
        {
        return ret;
        }
    for (TInt i = 0; i < count; i++)
        {
        SUT_LOG_DEBUGF(_L("kernel ctl run test case index:%d"), i);
        TSUTTestCaseResult caseResult;
        TPckg<TSUTTestCaseResult> resultPckg(caseResult);
        
        ret = iLddconn.ExecTestCase(&i, (TAny*)&resultPckg);
        
        if (ret != KErrNone)
            {
            aResult.AddLeaveFromTestL(ret, 0);
            aObserver.TestFailed(_L("unknown"), _L8("fail to run kernel test cases"));
            }
        
        
        TBuf<KTestCaseNameLength> testName;
        testName.Copy(caseResult.iTestName);
        
        aResult.StartTestL( testName );
        aResult.EndTestL();
        
        if (caseResult.iRetCode != KErrNone)
                {
                RDebug::Printf("msg:%S line:%d file:%S", 
                        &caseResult.iFailureMessage,
                        caseResult.iLineNumber,
                        &caseResult.iFileName);
                aResult.AddAssertFailure(
                    caseResult.iFailureMessage,
                    caseResult.iLineNumber,
                    caseResult.iFileName);
                SUT_LOG_DEBUGF(_L("kernel test case failed:%S"), &testName);
                aObserver.TestFailed(testName, (const TDesC8&)caseResult.iFailureMessage);
                
                }
            else
               {
               SUT_LOG_DEBUGF(_L("kernel test passed:%S"), &testName);
               aObserver.TestPass(testName);
               }
        
        }
    
    RDebug::Printf("failure count:%d", aResult.Failures().Count());
    
    iLddconn.Close();
    SUT_LOG_DEBUG("close ldd");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CSymbianUnitTestLddCtl::GetTestCaseNames(CDesCArray& aTestCaseNames)
    {
    TInt ret = iLddconn.Open(*iDriverName);
    if (ret != KErrNone)
        {
        return ret;
        }
    SUT_LOG_DEBUG("open ldd");

    TInt count;
    ret = iLddconn.GetTestCaseCount(&count);
    SUT_LOG_DEBUGF(_L("user space, test case count:%d"), count);
    if (ret != KErrNone)
        {
        return ret;
        }
    
    for (TInt i = 0; i < count; i++)
        {
        TBuf8<KTestCaseNameLength> testCaseName;
        ret = iLddconn.GetTestCaseName(&i, (TAny*) &testCaseName);
        if (ret != KErrNone)
            {
            return ret;
            }
        TBuf<KTestCaseNameLength> testCaseName16;
        testCaseName16.Copy(testCaseName);
        aTestCaseNames.AppendL(testCaseName16);
        }
    iLddconn.Close();
    SUT_LOG_DEBUG("close ldd");
    return KErrNone;
    }

