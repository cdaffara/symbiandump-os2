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
#include <featurenotifier.h>

using namespace NFeature;

const TUid KNewFeatureUid = {0x7888ABCA}; 
const TUid KNewFeatureUid2 = {0x7888ABCB}; 

///////////////////////////////////////////////////////////////////////////////////////

static RTest TheTest(_L("t_fmgrnotify"));

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_LIT(KFeatureNoChange,          "FeatureNoChange");
_LIT(KFeatureStatusUpdated,     "FeatureStatusUpdated");
_LIT(KFeatureDataUpdated,       "FeatureDataUpdated");
_LIT(KFeatureStatusDataUpdated, "FeatureStatusDataUpdated");
_LIT(KFeatureRediscover,        "FeatureRediscover");
_LIT(KFeatureCreated,           "FeatureCreated");
_LIT(KFeatureDeleted,           "FeatureDeleted");

const TPtrC KNotificationText[] = {KFeatureNoChange(), KFeatureStatusUpdated(), KFeatureDataUpdated(), 
                                   KFeatureStatusDataUpdated(), KFeatureRediscover(), KFeatureCreated(), 
                                   KFeatureDeleted()};

class TTestFeatureObserver : public MFeatureObserver
    {
public:
    TTestFeatureObserver() :
        iType(static_cast <TFeatureChangeType> (-1)),
        iFeatureUid(KNullUid)    
        {
        }
    void SetExpected(TFeatureChangeType aType, TUid aFeatureUid)
        {
        iType = aType;
        iFeatureUid = aFeatureUid;    
        }
    void Reset()
        {
        iType = static_cast <TFeatureChangeType> (-1);
        iFeatureUid = KNullUid;    
        }
    virtual void HandleNotifyChange(TFeatureChangeType aType, TFeatureEntry aFeature)
        {
        TheTest.Printf(_L("=== HandleNotifyChange() called with aType=\"%S\" and aFeature.FeatureUid()=0x%X\r\n"), &KNotificationText[aType], aFeature.FeatureUid());
        CActiveScheduler::Stop();
        TEST2(aType, iType);
        TEST(aFeature.FeatureUid() == iFeatureUid);
        }
    virtual void HandleNotifyError(TInt aError)
        {
        TheTest.Printf(_L("=== HandleNotifyError() called with error=%d\r\n"), aError);
        CActiveScheduler::Stop();
        TEST2(aError, KErrNone);
        }
private:
        TFeatureChangeType  iType;
        TUid                iFeatureUid;    
    };

///////////////////////////////////////////////////////////////////////////////////////

void AddFeature(RFeatureControl& aCtrl, TUid aFeatureUid)
    {
    TBitFlags32 flags;
    flags.ClearAll();
    flags.Set(EFeatureSupported);
    flags.Set(EFeatureModifiable);
    TFeatureEntry fentry(aFeatureUid, flags, 0x0);
    TInt err = aCtrl.AddFeature(fentry);
    TEST2(err, KErrNone);
    }

void DeleteFeature(RFeatureControl& aCtrl, TUid aFeatureUid)
    {
    TInt err = aCtrl.DeleteFeature(aFeatureUid);
    TEST2(err, KErrNone);
    }

/**
@SYMTestCaseID          PDS-FEATMGR-CT-????
@SYMTestCaseDesc        
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void NotificationsTest1()
    {
    TTestFeatureObserver observer;
    CFeatureNotifier* notifier = NULL;
    TRAPD(err, notifier = CFeatureNotifier::NewL(observer));
    TEST2(err, KErrNone);
    //Request notification for feature with KNewFeatureUid uid.
    err = notifier->NotifyRequest(KNewFeatureUid);
    TEST2(err, KErrNone);
    err = notifier->NotifyRequest(KNewFeatureUid);
    TEST2(err, KErrAlreadyExists);
    
    RFeatureControl ctrl;
    err = ctrl.Connect();
    TEST2(err, KErrNone);
    //Add a feature with KNewFeatureUid uid and check the notification
    AddFeature(ctrl, KNewFeatureUid);
    observer.SetExpected(EFeatureFeatureCreated, KNewFeatureUid);
    CActiveScheduler::Start();
    //Set the feature status and data and check the notification
    err = ctrl.SetFeature(KNewFeatureUid, ETrue, 100);
    TEST2(err, KErrNone);
    observer.SetExpected(EFeatureStatusDataUpdated, KNewFeatureUid);
    CActiveScheduler::Start();
    //Set the feature data and check the notification
    err = ctrl.SetFeature(KNewFeatureUid, 200);
    TEST2(err, KErrNone);
    observer.SetExpected(EFeatureDataUpdated, KNewFeatureUid);
    CActiveScheduler::Start();
    //Enable the feature (it is already enabled) and check the notification
    err = ctrl.EnableFeature(KNewFeatureUid);
    TEST2(err, KErrNone);
    observer.SetExpected(EFeatureNoChange, KNewFeatureUid);//the feature is enabled - no status change
    CActiveScheduler::Start();
    //Disable the feature and check the notification
    err = ctrl.DisableFeature(KNewFeatureUid);
    TEST2(err, KErrNone);
    observer.SetExpected(EFeatureStatusUpdated, KNewFeatureUid);
    CActiveScheduler::Start();
    //Cancel notifications
    err = notifier->NotifyCancel(KNewFeatureUid);
    TEST2(err, KErrNone);
    err = notifier->NotifyCancel(KNewFeatureUid);
    TEST2(err, KErrNotFound);
    //Request notifications again    
    err = notifier->NotifyRequest(KNewFeatureUid);
    TEST2(err, KErrNone);
    //Delete the feature and check the notification
    DeleteFeature(ctrl, KNewFeatureUid);
    observer.SetExpected(EFeatureFeatureDeleted, KNewFeatureUid);
    CActiveScheduler::Start();
    //Cleanup
    ctrl.Close();
    delete notifier;
    }

/**
@SYMTestCaseID          PDS-FEATMGR-CT-????
@SYMTestCaseDesc        
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void NotificationsTest2()
    {
    TTestFeatureObserver observer;
    CFeatureNotifier* notifier = NULL;
    TRAPD(err, notifier = CFeatureNotifier::NewL(observer));
    TEST2(err, KErrNone);
    
    RFeatureControl ctrl;
    err = ctrl.Connect();
    TEST2(err, KErrNone);
    //Add two features
    AddFeature(ctrl, KNewFeatureUid);
    AddFeature(ctrl, KNewFeatureUid2);
    //Request notifications for the added features. One of them - duplicated in the array.
    RFeatureUidArray farray;
    err = farray.Append(KNewFeatureUid);
    TEST2(err, KErrNone);
    err = farray.Append(KNewFeatureUid2);
    TEST2(err, KErrNone);
    err = farray.Append(KNewFeatureUid);
    TEST2(err, KErrNone);
    //
    err = notifier->NotifyRequest(farray);
    TEST2(err, KErrNone);
    //Enable one of the features (already enabled) and check the notification
    err = ctrl.EnableFeature(KNewFeatureUid);
    TEST2(err, KErrNone);
    observer.SetExpected(EFeatureNoChange, KNewFeatureUid);//the feature is enabled - no status change
    CActiveScheduler::Start();
    //Disable the second feature and check the notification
    err = ctrl.DisableFeature(KNewFeatureUid2);
    TEST2(err, KErrNone);
    observer.SetExpected(EFeatureStatusUpdated, KNewFeatureUid2);
    CActiveScheduler::Start();
    //Cancel notifications for the second feature
    err = notifier->NotifyCancel(KNewFeatureUid2);
    TEST2(err, KErrNone);
    //Disable the first feature and check notification
    err = ctrl.DisableFeature(KNewFeatureUid);
    TEST2(err, KErrNone);
    observer.SetExpected(EFeatureStatusUpdated, KNewFeatureUid);
    CActiveScheduler::Start();
    //Cancel all notifications
    err = notifier->NotifyCancelAll();
    TEST2(err, KErrNone);
    err = notifier->NotifyCancelAll();
    TEST2(err, KErrNone);
    //Cleanup
    farray.Close();
    ctrl.Close();
    delete notifier;
    }

void DoTestsL()
    {
    CActiveScheduler* scheduler = new CActiveScheduler;
    TEST(scheduler != NULL);
    CActiveScheduler::Install(scheduler);
    
    TheTest.Start(_L("@SYMTestCaseID:PDS-EFM-CT-4066 Notifications test 1"));
    NotificationsTest1();

    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4067 Notifications test 2"));
    NotificationsTest2();
    
    delete scheduler;
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
