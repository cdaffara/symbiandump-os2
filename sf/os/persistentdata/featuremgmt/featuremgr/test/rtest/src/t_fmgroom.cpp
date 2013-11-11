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
#include <bautils.h>
#include <featmgr.h>
#include <featureuids.h>
#include <featurecontrol.h>
#include <featdiscovery.h>
#include <featurenotifier.h>
#include "../../../src/clientdll/featmgrresourcetester.h"

using namespace NFeature;


const TInt KInvalidFeatureId    = 90901671;
const TUid KInvalidFeatureUid   = {KInvalidFeatureId};

static TInt TheProcessHandleCount = 0;
static TInt TheThreadHandleCount = 0;
static TInt TheAllocatedCellsCount = 0;

#ifdef _DEBUG
static const TInt KBurstRate = 20;
#endif

enum TFeatMgrOomTestMode
    {
    EFeatMgrOomServerTestMode,
    EFeatMgrOomClientTestMode,
    };

///////////////////////////////////////////////////////////////////////////////////////

static RTest TheTest(_L("t_fmgroom"));



///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine)
    {
    if(!aValue)
        {
        RDebug::Print(_L("*** Expression evaluated to false. Line %d\r\n"), aLine);
        TheTest(EFalse, aLine);
        }
    }
void Check2(TInt aValue, TInt aExpected, TInt aLine)
    {
    if(aValue != aExpected)
        {
        RDebug::Print(_L("*** Expected: %d, got: %d. Line %d\r\n"), aExpected, aValue, aLine);
        TheTest(EFalse, aLine);
        }
    }
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

static void MarkHandles()
    {
    RThread().HandleCount(TheProcessHandleCount, TheThreadHandleCount);
    }

static void MarkAllocatedCells()
    {
    TheAllocatedCellsCount = User::CountAllocCells();
    }

static void CheckAllocatedCells()
    {
    TInt allocatedCellsCount = User::CountAllocCells();
    TEST2(allocatedCellsCount, TheAllocatedCellsCount);
    }

static void CheckHandles()
    {
    TInt endProcessHandleCount;
    TInt endThreadHandleCount;
    
    RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

    TEST2(TheProcessHandleCount, endProcessHandleCount);
    TEST2(TheThreadHandleCount, endThreadHandleCount);
    }

static void OomPreStep(TInt aFailingAllocationNo, TFeatMgrOomTestMode aMode = EFeatMgrOomClientTestMode)
    {
    if(aMode == EFeatMgrOomClientTestMode)
        {
        MarkHandles();
        MarkAllocatedCells();
        __UHEAP_MARK;
        __UHEAP_SETBURSTFAIL(RAllocator::EBurstFailNext, aFailingAllocationNo, KBurstRate);
        }
    else
        {
        TFeatMgrResourceTester::Mark();
        TFeatMgrResourceTester::SetHeapFailure(RHeap::EBurstFailNext, aFailingAllocationNo);
        }
    }

static void OomPostStep(TFeatMgrOomTestMode aMode = EFeatMgrOomClientTestMode)
    {
    if(aMode == EFeatMgrOomClientTestMode)
        {
        __UHEAP_RESET;
        __UHEAP_MARKEND;
        CheckAllocatedCells();
        CheckHandles();
        }
    else
        {
        TFeatMgrResourceTester::SetHeapFailure(RHeap::ENone, 0);
        TFeatMgrResourceTester::Check();
        }
    }

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID          PDS-EFM-CT-4068
@SYMTestCaseDesc        Include test case 4069 too
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void ControlOpenOomTest(TBool aUseConnect)
    {
    TInt err = KErrNoMemory;
    TInt failingAllocationNo = 0;
    RFeatureControl ctrl;
    TheTest.Printf(_L("Iteration:\r\n"));
    while(err == KErrNoMemory)
        {
        TheTest.Printf(_L(" %d"), ++failingAllocationNo);
        OomPreStep(failingAllocationNo);
        err = aUseConnect ? ctrl.Connect() : ctrl.Open();
        ctrl.Close();
        OomPostStep();
        }
    if(err != KErrNoMemory)
        {
        TEST2(err, KErrNone);   
        }
    TheTest.Printf(_L("\r\n===OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
    }

/**
@SYMTestCaseID          PDS-EFM-CT-4070
@SYMTestCaseDesc        Include test case 4071-4077 too
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void ControlFeatureSupportedOomTest(TBool aUseUid, TBool aInvalidFeature, TFeatMgrOomTestMode aMode)
    {
    RFeatureControl ctrl;
    TInt err = ctrl.Open();
    TEST2(err, KErrNone);
    
    const TUid KFeatureUid(aInvalidFeature ? KInvalidFeatureUid : KConnectivity);
    TFeatureEntry fentry(KFeatureUid);
    
    err = KErrNoMemory;
    TInt failingAllocationNo = 0;
    TheTest.Printf(_L("Iteration:\r\n"));
    while(err == KErrNoMemory)
        {
        TheTest.Printf(_L(" %d"), ++failingAllocationNo);
        OomPreStep(failingAllocationNo, aMode);
        err = aUseUid ? ctrl.FeatureSupported(KFeatureUid) : ctrl.FeatureSupported(fentry);
        OomPostStep(aMode);
        }
    ctrl.Close();
    if(err != KErrNoMemory)
        {
        TEST2(err, aInvalidFeature ? KErrNotFound : KFeatureSupported);   
        }
    TheTest.Printf(_L("\r\n===OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
    }

/**
@SYMTestCaseID          PDS-EFM-CT-4078
@SYMTestCaseDesc        Include test case 4079 too
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void ControlListSupportedFeaturesOomTest(TFeatMgrOomTestMode aMode)
    {
    RFeatureControl ctrl;
    TInt err = ctrl.Open();
    TEST2(err, KErrNone);
    
    err = KErrNoMemory;
    TInt failingAllocationNo = 0;
    TheTest.Printf(_L("Iteration:\r\n"));
    while(err == KErrNoMemory)
        {
        TheTest.Printf(_L(" %d"), ++failingAllocationNo);
        OomPreStep(failingAllocationNo, aMode);
        const TInt KGranularity = 1;
        RFeatureUidArray farray(KGranularity);
        err = ctrl.ListSupportedFeatures(farray);
        if(err != KErrNoMemory)
            {
            TEST2(err, KErrNone);
            TInt count = farray.Count();
            TheTest.Printf(_L("===Features count: %d\r\n"), count);
            TEST(count > 0);
            }
        farray.Close();
        OomPostStep(aMode);
        }
    ctrl.Close();
    TheTest.Printf(_L("\r\n===OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
    }

/**
@SYMTestCaseID          PDS-EFM-CT-4080
@SYMTestCaseDesc        Include test case 4081 too
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void ControlFeaturesSupportedOomTest(TFeatMgrOomTestMode aMode)
    {
    RFeatureControl ctrl;
    TInt err = ctrl.Open();
    TEST2(err, KErrNone);
    RFeatureUidArray farray;
    err = ctrl.ListSupportedFeatures(farray);
    TEST2(err, KErrNone);
    
    err = KErrNoMemory;
    TInt failingAllocationNo = 0;
    TheTest.Printf(_L("Iteration:\r\n"));
    while(err == KErrNoMemory)
        {
        TheTest.Printf(_L(" %d"), ++failingAllocationNo);
        OomPreStep(failingAllocationNo, aMode);
        RFeatureArray farray2;
        err = KErrNone;
        for(TInt i=0;i<farray.Count() && err==KErrNone;++i)
            {
            err = farray2.Append(farray[i]); 
            }
        if(err == KErrNone)
            {
            err = ctrl.FeaturesSupported(farray2);
            }
        if(err != KErrNoMemory)
            {
            TEST2(err, KErrNone);
            TInt count = farray2.Count();
            TheTest.Printf(_L("===Features count: %d\r\n"), count);
            TEST(count > 0);
            }
        farray2.Close();
        OomPostStep(aMode);
        }
    farray.Close();
    ctrl.Close();
    TheTest.Printf(_L("\r\n===OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
    }

/**
@SYMTestCaseID          PDS-EFM-CT-4082
@SYMTestCaseDesc        Include test case 4083 too
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void ControlAddFeatureOomTest(TFeatMgrOomTestMode aMode)
    {
    RFeatureControl ctrl;
    TInt err = ctrl.Open();
    TEST2(err, KErrNone);

    const TUid KNewFeatureUid = {0x7888ABCE}; 
    TBitFlags32 flags;
    flags.Set(EFeatureSupported);
    flags.Set(EFeatureModifiable);    
    flags.Set(EFeaturePersisted);
    TFeatureEntry fentry(KNewFeatureUid, flags, 0x0);
    
    err = KErrNoMemory;
    TInt failingAllocationNo = 0;
    TheTest.Printf(_L("Iteration:\r\n"));
    while(err == KErrNoMemory)
        {
        TheTest.Printf(_L(" %d"), ++failingAllocationNo);
        OomPreStep(failingAllocationNo, aMode);
        err = ctrl.AddFeature(fentry);
        OomPostStep(aMode);
        if(err == KErrNoMemory)
            {
            err = ctrl.EnableFeature(fentry.FeatureUid());
            TEST2(err, KErrNotFound);
            }
        }
    TEST2(err, KErrNone);
    err = ctrl.DeleteFeature(fentry.FeatureUid());
    TEST2(err, KErrNone);
    
    ctrl.Close();
    TheTest.Printf(_L("\r\n===OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
    }

/**
@SYMTestCaseID          PDS-EFM-CT-4084
@SYMTestCaseDesc        Include test case 4085 too
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void ControlDeleteFeatureOomTest(TFeatMgrOomTestMode aMode)
    {
    RFeatureControl ctrl;
    TInt err = ctrl.Open();
    TEST2(err, KErrNone);

    const TUid KNewFeatureUid = {0x7888ABCE}; 
    TBitFlags32 flags;
    flags.Set(EFeatureSupported);
    flags.Set(EFeatureModifiable);
    TFeatureEntry fentry(KNewFeatureUid, flags, 0x0);
    err = ctrl.AddFeature(fentry);
    TEST2(err, KErrNone);
    
    err = KErrNoMemory;
    TInt failingAllocationNo = 0;
    TheTest.Printf(_L("Iteration:\r\n"));
    while(err == KErrNoMemory)
        {
        TheTest.Printf(_L(" %d"), ++failingAllocationNo);
        OomPreStep(failingAllocationNo, aMode);
        err = ctrl.DeleteFeature(fentry.FeatureUid());
        OomPostStep(aMode);
        if(err == KErrNoMemory)
            {
            err = ctrl.EnableFeature(fentry.FeatureUid());
            TEST2(err, KErrNone);
            }
        }
    TEST2(err, KErrNone);
    
    ctrl.Close();
    TheTest.Printf(_L("\r\n===OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TTestFeatureObserver : public MFeatureObserver
    {
    public: 
        virtual void HandleNotifyChange(TFeatureChangeType /*aType*/, TFeatureEntry /*aFeature*/)
            {
            }
        virtual void HandleNotifyError(TInt /*aError*/)
            {
            }
    };

/**
@SYMTestCaseID          PDS-EFM-CT-4086
@SYMTestCaseDesc        Include test case 4087 too
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void NotifierNewLOomTest(TFeatMgrOomTestMode aMode)
    {
    CActiveScheduler* scheduler = new CActiveScheduler;
    TEST(scheduler != NULL);
    CActiveScheduler::Install(scheduler);
    
    RFeatureControl ctrl;
    TInt err = ctrl.Open();
    TEST2(err, KErrNone);
    
    TTestFeatureObserver observer;
    err = KErrNoMemory;
    TInt failingAllocationNo = 0;
    TheTest.Printf(_L("Iteration:\r\n"));
    while(err == KErrNoMemory)
        {
        TheTest.Printf(_L(" %d"), ++failingAllocationNo);
        OomPreStep(failingAllocationNo, aMode);
        CFeatureNotifier* notifier = NULL;
        TRAP(err, notifier = CFeatureNotifier::NewL(observer));
        delete notifier;
        OomPostStep(aMode);
        }
    TEST2(err, KErrNone);
    ctrl.Close();
    delete scheduler;
    TheTest.Printf(_L("\r\n===OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
    }

/**
@SYMTestCaseID          PDS-EFM-CT-4088
@SYMTestCaseDesc        Include test case 4089-4091 too
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void NotifierNotifyRequestOomTest(TBool aInvalidFeature, TFeatMgrOomTestMode aMode)
    {
    CActiveScheduler* scheduler = new CActiveScheduler;
    TEST(scheduler != NULL);
    CActiveScheduler::Install(scheduler);
    
    RFeatureControl ctrl;
    TInt err = ctrl.Open();
    TEST2(err, KErrNone);
    
    TTestFeatureObserver observer;
    CFeatureNotifier* notifier = NULL;
    TRAP(err, notifier = CFeatureNotifier::NewL(observer));
    TEST2(err, KErrNone);

    const TUid KFeatureUid(aInvalidFeature ? KInvalidFeatureUid : KConnectivity);
    
    err = KErrNoMemory;
    TInt failingAllocationNo = 0;
    TheTest.Printf(_L("Iteration:\r\n"));
    while(err == KErrNoMemory)
        {
        TheTest.Printf(_L(" %d"), ++failingAllocationNo);
        OomPreStep(failingAllocationNo, aMode);
        err = notifier->NotifyRequest(KFeatureUid);
        (void)notifier->NotifyCancelAll();
        OomPostStep(aMode);
        }
    TEST2(err, KErrNone);
    delete notifier;
    ctrl.Close();
    delete scheduler;
    TheTest.Printf(_L("\r\n===OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
    }

/**
@SYMTestCaseID          PDS-EFM-CT-4092
@SYMTestCaseDesc        Include test case 4093 too
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void NotifierNotifyRequestsOomTest(TFeatMgrOomTestMode aMode)
    {
    CActiveScheduler* scheduler = new CActiveScheduler;
    TEST(scheduler != NULL);
    CActiveScheduler::Install(scheduler);
    
    RFeatureControl ctrl;
    TInt err = ctrl.Open();
    TEST2(err, KErrNone);
    
    TTestFeatureObserver observer;
    CFeatureNotifier* notifier = NULL;
    TRAP(err, notifier = CFeatureNotifier::NewL(observer));
    TEST2(err, KErrNone);

    RFeatureUidArray features;
    err = features.Append(KConnectivity);
    TEST2(err, KErrNone);
    err = features.Append(KFax);
    TEST2(err, KErrNone);
    err = features.Append(KConnectivity);
    TEST2(err, KErrNone);
    err = features.Append(KLocationManagement);
    TEST2(err, KErrNone);
    
    err = KErrNoMemory;
    TInt failingAllocationNo = 0;
    TheTest.Printf(_L("Iteration:\r\n"));
    while(err == KErrNoMemory)
        {
        TheTest.Printf(_L(" %d"), ++failingAllocationNo);
        OomPreStep(failingAllocationNo, aMode);
        err = notifier->NotifyRequest(features);
        (void)notifier->NotifyCancelAll();
        OomPostStep(aMode);
        }
    TEST2(err, KErrNone);
    features.Close();
    delete notifier;
    ctrl.Close();
    delete scheduler;
    TheTest.Printf(_L("\r\n===OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
    }



void DoTestsL()
    {
    TheTest.Start(_L("@SYMTestCaseID:PDS-EFM-CT-4068 RFeatureControl::Connect() OOM test"));
    ControlOpenOomTest(ETrue);
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4069 RFeatureControl::Open() OOM test"));
    ControlOpenOomTest(EFalse);
    
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4070 RFeatureControl::FeatureSupported(UID) + valid feature, client side OOM test"));
    ControlFeatureSupportedOomTest(ETrue, EFalse, EFeatMgrOomClientTestMode);
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4071 RFeatureControl::FeatureSupported(UID) + valid feature, server side OOM test"));
    ControlFeatureSupportedOomTest(ETrue, EFalse, EFeatMgrOomServerTestMode);
    
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4072 RFeatureControl::FeatureSupported() + valid feature, client side OOM test"));
    ControlFeatureSupportedOomTest(EFalse, EFalse, EFeatMgrOomClientTestMode);
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4073 RFeatureControl::FeatureSupported() + valid feature, server side OOM test"));
    ControlFeatureSupportedOomTest(EFalse, EFalse, EFeatMgrOomServerTestMode);
    
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4074 RFeatureControl::FeatureSupported(UID) + invalid feature, client side OOM test"));
    ControlFeatureSupportedOomTest(ETrue, ETrue, EFeatMgrOomClientTestMode);
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4075 RFeatureControl::FeatureSupported(UID) + invalid feature, server side OOM test"));
    ControlFeatureSupportedOomTest(ETrue, ETrue, EFeatMgrOomServerTestMode);
    
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4076 RFeatureControl::FeatureSupported() + invalid feature, client side OOM OOM test"));
    ControlFeatureSupportedOomTest(EFalse, ETrue, EFeatMgrOomClientTestMode);
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4077 RFeatureControl::FeatureSupported() + invalid feature, server side OOM OOM test"));
    ControlFeatureSupportedOomTest(EFalse, ETrue, EFeatMgrOomServerTestMode);
    
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4078 RFeatureControl::ListSupportedFeatures(), client side OOM test"));
    ControlListSupportedFeaturesOomTest(EFeatMgrOomClientTestMode);
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4079 RFeatureControl::ListSupportedFeatures(), server side OOM test"));
    ControlListSupportedFeaturesOomTest(EFeatMgrOomServerTestMode);
    
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4080 RFeatureControl::FeaturesSupported(), client side OOM test"));
    ControlFeaturesSupportedOomTest(EFeatMgrOomClientTestMode);
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4081 RFeatureControl::FeaturesSupported(),server side OOM test"));
    ControlFeaturesSupportedOomTest(EFeatMgrOomServerTestMode);
    
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4082 RFeatureControl::AddFeature(), client side OOM test"));
    ControlAddFeatureOomTest(EFeatMgrOomClientTestMode);
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4083 RFeatureControl::AddFeature(), server side OOM test"));
    ControlAddFeatureOomTest(EFeatMgrOomServerTestMode);
    
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4084 RFeatureControl::DeleteFeature(), client side OOM test"));
    ControlDeleteFeatureOomTest(EFeatMgrOomClientTestMode);
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4085 RFeatureControl::DeleteFeature(), server side OOM test"));
    ControlDeleteFeatureOomTest(EFeatMgrOomServerTestMode);

    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4086 CFeatureNotifier::NewL(), client side OOM test"));
    NotifierNewLOomTest(EFeatMgrOomClientTestMode);
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4087 CFeatureNotifier::NewL(), server side OOM test"));
    NotifierNewLOomTest(EFeatMgrOomServerTestMode);
    
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4088 CFeatureNotifier::NotifyRequest(), valid feature, client side OOM test"));
    NotifierNotifyRequestOomTest(ETrue, EFeatMgrOomClientTestMode);
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4089 CFeatureNotifier::NotifyRequest(), valid feature, server side OOM test"));
    NotifierNotifyRequestOomTest(ETrue, EFeatMgrOomServerTestMode);
    
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4090 CFeatureNotifier::NotifyRequest(), invalid feature, client side OOM test"));
    NotifierNotifyRequestOomTest(EFalse, EFeatMgrOomClientTestMode);
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4091 CFeatureNotifier::NotifyRequest(), invalid feature, server side OOM test"));
    NotifierNotifyRequestOomTest(EFalse, EFeatMgrOomServerTestMode);

    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4092 CFeatureNotifier::NotifyRequest(<array>), valid feature, client side OOM test"));
    NotifierNotifyRequestsOomTest(EFeatMgrOomClientTestMode);
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4093 CFeatureNotifier::NotifyRequest(<array>), valid feature, server side OOM test"));
    NotifierNotifyRequestsOomTest(EFeatMgrOomServerTestMode);
    }

TInt E32Main()
    {
    TheTest.Title();
    
    CTrapCleanup* tc = CTrapCleanup::New();
    TheTest(tc != NULL);
    
    __UHEAP_MARK;
    
    TRAPD(err, DoTestsL());
    TEST2(err, KErrNone);

    __UHEAP_MARKEND;
    
    TheTest.End();
    TheTest.Close();
    
    delete tc;

    User::Heap().Check();
    return KErrNone;
    }
