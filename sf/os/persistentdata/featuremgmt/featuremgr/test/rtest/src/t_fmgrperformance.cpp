// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <hal.h>
#include <featmgr.h>
#include <featureuids.h>
#include <featurecontrol.h>
#include <featdiscovery.h>

using namespace NFeature;

TInt TheFastCounterFreq = 0;

const TInt KInvalidFeatureId1 = 90901671;
const TUid KInvalidFeatureUid1 = {KInvalidFeatureId1};
const TUid KNewFeatureUid = {0x7888ABC1}; 

///////////////////////////////////////////////////////////////////////////////////////

static RTest TheTest(_L("t_fmgrperformance"));

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

TInt TimeDiffUs(TUint32 aStartTicks, TUint32 aEndTicks)
    {
    if(TheFastCounterFreq == 0)
        {
        TEST2(HAL::Get(HAL::EFastCounterFrequency, TheFastCounterFreq), KErrNone);
        TheTest.Printf(_L("===Fast counter frequency = %d Hz\r\n"), TheFastCounterFreq);
        }
    TInt64 diffTicks = (TInt64)aEndTicks - (TInt64)aStartTicks;
    if(diffTicks < 0)
        {
        diffTicks = KMaxTUint32 + diffTicks + 1;
        }
    const TInt KMicroSecIn1Sec = 1000000;
    TInt us = (diffTicks * KMicroSecIn1Sec) / TheFastCounterFreq;
    return us;
    }

void PrintTime(const TDesC& aFmt, TUint32 aStartTicks, TUint32 aEndTicks)
    {
    TInt us = TimeDiffUs(aStartTicks, aEndTicks);
    TheTest.Printf(aFmt, us);
    }

/**
@SYMTestCaseID          PDS-EFM-CT-4106
@SYMTestCaseDesc        
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void FeatureControlTest()
    {
    TFeatureEntry fentry;
    
    TUint32 start = User::FastCounter();
    RFeatureControl ctrl;
    
    TInt err = ctrl.Open();
    TEST2(err, KErrNone);
    TUint32 end = User::FastCounter();
    PrintTime(_L("===1 RFeatureControl::Open(), time=%d us\r\n"), start, end);
    
    //The second RFeatureControl::Open() call is "free", because only one connection per thread is kept in TLS 
    RFeatureControl ctrl2;
    start = User::FastCounter();
    err = ctrl2.Open();
    TEST2(err, KErrNone);
    end = User::FastCounter();
    PrintTime(_L("===2 RFeatureControl::Open(), time=%d us\r\n"), start, end);
    
    //
    start = User::FastCounter();
    ctrl2.Close();
    end = User::FastCounter();
    PrintTime(_L("===2 RFeatureControl::Close(), time=%d us\r\n"), start, end);
    
    //
    TBitFlags32 flags;
    flags.ClearAll();
    flags.Set(EFeatureSupported);
    flags.Set(EFeatureModifiable);
    
    fentry = TFeatureEntry(KNewFeatureUid, flags, 0x0);
    start = User::FastCounter();
    err = ctrl.AddFeature(fentry);
    TEST2(err, KErrNone);
    end = User::FastCounter();
    PrintTime(_L("===RFeatureControl::AddFeature(), time=%d us\r\n"), start, end);
    //
    start = User::FastCounter();
    err = ctrl.FeatureSupported(KNewFeatureUid);
    TEST2(err, 1);
    end = User::FastCounter();
    PrintTime(_L("===RFeatureControl::FeatureSupported(TUid), time=%d us\r\n"), start, end);
    //
    start = User::FastCounter();
    err = ctrl.FeatureSupported(KInvalidFeatureUid1);
    TEST2(err, KErrNotFound);
    end = User::FastCounter();
    PrintTime(_L("===RFeatureControl::FeatureSupported(invalid TUid), time=%d us\r\n"), start, end);
    //
    fentry = TFeatureEntry(KNewFeatureUid);
    start = User::FastCounter();
    err = ctrl.FeatureSupported(fentry);
    TEST2(err, 1);
    end = User::FastCounter();
    PrintTime(_L("===RFeatureControl::FeatureSupported(TFeatureEntry), time=%d us\r\n"), start, end);
    //
    RFeatureArray farray;
    err = farray.Append(TFeatureEntry(KNewFeatureUid));
    TEST2(err, KErrNone);
    err = farray.Append(TFeatureEntry(KInvalidFeatureUid1));
    TEST2(err, KErrNone);
    err = farray.Append(TFeatureEntry(KConnectivity));
    TEST2(err, KErrNone);
    err = farray.Append(TFeatureEntry(KUsb));
    TEST2(err, KErrNone);
    start = User::FastCounter();
    err = ctrl.FeaturesSupported(farray);
    end = User::FastCounter();
    PrintTime(_L("===RFeatureControl::FeaturesSupported(), time=%d us\r\n"), start, end);
    TEST2(farray.Count(), 3);//KInvalidFeatureUid1 should have been removed from the array
    farray.Close();
    //
    start = User::FastCounter();
    err = ctrl.DisableFeature(KNewFeatureUid);
    TEST2(err, KErrNone);
    end = User::FastCounter();
    PrintTime(_L("===1 RFeatureControl::DisableFeature(), time=%d us\r\n"), start, end);
    //Disable the same feature again
    start = User::FastCounter();
    err = ctrl.DisableFeature(KNewFeatureUid);
    TEST2(err, KErrNone);
    end = User::FastCounter();
    PrintTime(_L("===2 RFeatureControl::DisableFeature(already disabled), time=%d us\r\n"), start, end);
    //
    start = User::FastCounter();
    err = ctrl.DisableFeature(KFax);
    TEST2(err, KErrAccessDenied);
    end = User::FastCounter();
    PrintTime(_L("===3 RFeatureControl::DisableFeature(access denied), time=%d us\r\n"), start, end);
    //
    start = User::FastCounter();
    err = ctrl.EnableFeature(KNewFeatureUid);
    TEST2(err, KErrNone);
    end = User::FastCounter();
    PrintTime(_L("===1 RFeatureControl::EnableFeature(), time=%d us\r\n"), start, end);
    //Enable the same feature again
    start = User::FastCounter();
    err = ctrl.EnableFeature(KNewFeatureUid);
    TEST2(err, KErrNone);
    end = User::FastCounter();
    PrintTime(_L("===2 RFeatureControl::EnableFeature(already enabled), time=%d us\r\n"), start, end);
    //
    start = User::FastCounter();
    err = ctrl.EnableFeature(KFax);
    TEST2(err, KErrAccessDenied);
    end = User::FastCounter();
    PrintTime(_L("===3 RFeatureControl::EnableFeature(access denied), time=%d us\r\n"), start, end);
    //
    start = User::FastCounter();
    err = ctrl.SetFeature(KNewFeatureUid, EFalse, 100);
    TEST2(err, KErrNone);
    end = User::FastCounter();
    PrintTime(_L("===1 RFeatureControl::SetFeature(), time=%d us\r\n"), start, end);
    //
    start = User::FastCounter();
    err = ctrl.SetFeature(KNewFeatureUid, 200);
    TEST2(err, KErrNone);
    end = User::FastCounter();
    PrintTime(_L("===2 RFeatureControl::SetFeature(), time=%d us\r\n"), start, end);
    //
    RFeatureUidArray farray2;
    start = User::FastCounter();
    err = ctrl.ListSupportedFeatures(farray2);
    TEST2(err, KErrNone);
    end = User::FastCounter();
    PrintTime(_L("===RFeatureControl::ListSupportedFeatures(), time=%d us\r\n"), start, end);
    TEST(farray2.Count() > 0);
    farray2.Close();
    //
    start = User::FastCounter();
    ctrl.DeleteFeature(KNewFeatureUid);
    end = User::FastCounter();
    PrintTime(_L("===RFeatureControl::DeleteFeature(), time=%d us\r\n"), start, end);
    //
    start = User::FastCounter();
    ctrl.Close();
    end = User::FastCounter();
    PrintTime(_L("===1 RFeatureControl::Close(), time=%d us\r\n"), start, end);
    }

/**
@SYMTestCaseID          PDS-EFM-CT-4107
@SYMTestCaseDesc        
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void FeatureManagerTestL()
    {
    TUint32 start = User::FastCounter();
    FeatureManager::InitializeLibL();
    TUint32 end = User::FastCounter();
    PrintTime(_L("===FeatureManager::InitializeLibL(server already loaded by the previous test), time=%d us\r\n"), start, end);
    //
    start = User::FastCounter();
    TBool rc = FeatureManager::FeatureSupported(KConnectivity.iUid);
    TEST(rc);
    end = User::FastCounter();
    PrintTime(_L("===FeatureManager::FeatureSupported(), time=%d us\r\n"), start, end);
    //
    start = User::FastCounter();
    FeatureManager::UnInitializeLib();  
    end = User::FastCounter();
    PrintTime(_L("===FeatureManager::UnInitializeLib(), time=%d us\r\n"), start, end);
    }

void DoTestsL()
    {
    TheTest.Start(_L("@SYMTestCaseID:PDS-EFM-CT-4106 RFeatureControl performance test"));
    FeatureControlTest();

    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4107 FeatureManager performance test"));
    FeatureManagerTestL();
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
