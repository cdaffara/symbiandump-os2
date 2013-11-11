// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
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

#include <e32test.h>
#include <featmgr.h>
#include <featureuids.h>
#include <featurecontrol.h>
#include <featdiscovery.h>

using namespace NFeature;

const TInt KInvalidFeatureId1    = 90901671;
const TUid KInvalidFeatureUid1  = {KInvalidFeatureId1};

///////////////////////////////////////////////////////////////////////////////////////
//////  Note: This test has no platsec capabilities. It should not be possible to /////
//////        call platsec protected FeatMgr methods.                             /////
///////////////////////////////////////////////////////////////////////////////////////

static RTest TheTest(_L("t_fmgrsecurity1"));

///////////////////////////////////////////////////////////////////////////////////////

//Deletes all created test files.
void DestroyTestEnv()
    {
    }

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine)
    {
    if(!aValue)
        {
        DestroyTestEnv();
        RDebug::Print(_L("*** Expression evaluated to false. Line %d\r\n"), aLine);
        TheTest(EFalse, aLine);
        }
    }
void Check2(TInt aValue, TInt aExpected, TInt aLine)
    {
    if(aValue != aExpected)
        {
        DestroyTestEnv();
        RDebug::Print(_L("*** Expected: %d, got: %d. Line %d\r\n"), aExpected, aValue, aLine);
        TheTest(EFalse, aLine);
        }
    }
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID          PDS-FEATMGR-CT-????
@SYMTestCaseDesc        
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 ????
*/
void FeatureControlPlatSecTest()
    {
    RFeatureControl ctrl;
    TInt err = ctrl.Open();
    TEST2(err, KErrNone);

    err = ctrl.EnableFeature(KConnectivity);
    TEST2(err, KErrPermissionDenied);
    err = ctrl.EnableFeature(KInvalidFeatureUid1);
    TEST2(err, KErrPermissionDenied);
    
    err = ctrl.DisableFeature(KConnectivity);
    TEST2(err, KErrPermissionDenied);
    err = ctrl.DisableFeature(KInvalidFeatureUid1);
    TEST2(err, KErrPermissionDenied);

    err = ctrl.SetFeature(KConnectivity, ETrue, 0);
    TEST2(err, KErrPermissionDenied);
    err = ctrl.SetFeature(KInvalidFeatureUid1, ETrue, 0);
    TEST2(err, KErrPermissionDenied);
    err = ctrl.SetFeature(KConnectivity, 0);
    TEST2(err, KErrPermissionDenied);
    err = ctrl.SetFeature(KInvalidFeatureUid1, 0);
    TEST2(err, KErrPermissionDenied);

    TFeatureEntry fentry;
    err = ctrl.AddFeature(fentry);
    TEST2(err, KErrPermissionDenied);
    err = ctrl.DeleteFeature(KConnectivity);
    TEST2(err, KErrPermissionDenied);
    err = ctrl.DeleteFeature(KInvalidFeatureUid1);
    TEST2(err, KErrPermissionDenied);
    
    err = ctrl.SWIStart();
    TEST2(err, KErrPermissionDenied);
    err = ctrl.SWIEnd();
    TEST2(err, KErrPermissionDenied);
    
    ctrl.Close();
    }

void DoTestsL()
    {
    TheTest.Start(_L("@SYMTestCaseID:PDS-EFM-CT-4108 RFeatureControl platsec test"));
    FeatureControlPlatSecTest();
    }

TInt E32Main()
    {
    TheTest.Title();
    
    CTrapCleanup* tc = CTrapCleanup::New();
    TheTest(tc != NULL);
    
    __UHEAP_MARK;
    
    TRAPD(err, DoTestsL());
    DestroyTestEnv();
    TEST2(err, KErrNone);

    __UHEAP_MARKEND;
    
    TheTest.End();
    TheTest.Close();
    
    delete tc;

    User::Heap().Check();
    return KErrNone;
    }
