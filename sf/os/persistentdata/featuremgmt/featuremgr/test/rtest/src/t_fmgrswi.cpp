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
#include <f32file.h>
#include <sacls.h>
#include <e32property.h>
#include <featmgr.h>
#include <featureuids.h>
#include <featurecontrol.h>
#include <featdiscovery.h>
#include "../src/inc/featmgrconfiguration.h"

///////////////////////////////////////////////////////////////////////////////////////

static RTest TheTest(_L("t_fmgrswi"));

const TUid KNewFeatureUid = {0x7888ABC2}; 

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

TInt KillProcess(const TDesC& aProcessName)
    {
    TFullName name;
    //RDebug::Print(_L("Find and kill \"%S\" process.\n"), &aProcessName);
    TBuf<64> pattern(aProcessName);
    TInt length = pattern.Length();
    pattern += _L("*");
    TFindProcess procFinder(pattern);

    while (procFinder.Next(name) == KErrNone)
        {
        if (name.Length() > length)
            {//If found name is a string containing aProcessName string.
            TChar c(name[length]);
            if (c.IsAlphaDigit() ||
                c == TChar('_') ||
                c == TChar('-'))
                {
                // If the found name is other valid application name
                // starting with aProcessName string.
                //RDebug::Print(_L(":: Process name: \"%S\".\n"), &name);
                continue;
                }
            }
        RProcess proc;
        if (proc.Open(name) == KErrNone)
            {
            proc.Kill(0);
            //RDebug::Print(_L("\"%S\" process killed.\n"), &name);
            }
        proc.Close();
        }
    return KErrNone;
    }

/**
@SYMTestCaseID          PDS-EFM-CT-4111
@SYMTestCaseDesc        The test demonstrates that there is a problem in the FeatMgr code that processes SWI events.
                        Test steps:
                        1) The test sets KSAUidSoftwareInstallKeyValue property in order to simulate that SWI has started.
                        2) The test calls RFeatureControl::SWIStart() to notify the server that SWI started.
                        3) The test adds a new feature. Since this happens during the SWI process the new feature
                           will be cached by the FeatMgr server.
                        4) The test simulates a file I/O error to happen on iteration #4. The failure should occur
                           at the moment when the server tries to persist the new feature that is in the cache.
                        5) The test notifies the server that SWI has completed: KSAUidSoftwareInstallKeyValue value set
                           and RFeatureControl::SWIEnd() called.
                           When the server receives the "end of SWI" notification, the server will try to persist
                           the cached new feature. But because of the simulated file I/O error the server will fail
                           to store the new feature to the features.dat file. But in accordance with the current
                           design of the server, no error will be reported back to the client.
                        6) The test kills the FeatMgr server. The server cache is gone.
                        7) The test restarts the server and attempts to request the new feature. The new feature
                           is missing.
@SYMTestPriority        High
@SYMTestActions         FeatMgr SWI test with simulated file I/O error during SWI.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void SWItest()
    {
    RFs fs;
    TInt err = fs.Connect();
    TEST2(err, KErrNone);
    //
    RFeatureControl ctrl;
    err = ctrl.Open();
    TEST2(err, KErrNone);
    //Simulate SWI start
    err = RProperty::Set(KUidSystemCategory, KSAUidSoftwareInstallKeyValue, ESASwisInstall);
    TEST2(err, KErrNone);
    //Notify FeatMgr server that SWI started
    err = ctrl.SWIStart();
    TEST2(err, KErrNone);
    //Add a new persistent feature (using the same SWI connection) 
    TBitFlags32 flags;
    flags.ClearAll();
    flags.Set(EFeatureSupported);
    flags.Set(EFeatureModifiable);
    flags.Set(EFeaturePersisted);
    TFeatureEntry fentry(KNewFeatureUid, flags, 9876);
    err = ctrl.AddFeature(fentry);
    TEST2(err, KErrNone);
    //Simulate file I/O error
    (void)fs.SetErrorCondition(KErrGeneral, 4);
    //Complete the SWI simulation
    err = RProperty::Set(KUidSystemCategory, KSAUidSoftwareInstallKeyValue, ESASwisInstall | ESASwisStatusSuccess);
    TEST2(err, KErrNone);
    //Notify FeatMgr server that SWI completed
    err = ctrl.SWIEnd();
    TEST2(err, KErrNone);
    //Cleanup
    ctrl.Close();
    (void)fs.SetErrorCondition(KErrNone);
    fs.Close();
    //Kill FeatMgr server
    err = KillProcess(KServerProcessName);
    TEST2(err, KErrNone);
    //Open new connection
    err = ctrl.Open();
    TEST2(err, KErrNone);
    //The feature should be there
    TFeatureEntry fentry2(KNewFeatureUid);
    err = ctrl.FeatureSupported(fentry2);
    TEST2(err, KFeatureSupported);
    TEST2(fentry2.FeatureData(), fentry.FeatureData());
    //Cleanup
    err = ctrl.DeleteFeature(KNewFeatureUid);
    TEST2(err, KErrNone);
    ctrl.Close();
    }

void DoTestsL()
    {
    CActiveScheduler* scheduler = new CActiveScheduler;
    TEST(scheduler != NULL);
    CActiveScheduler::Install(scheduler);
    
    TheTest.Start(_L("@SYMTestCaseID:PDS-EFM-CT-4111 SWI test"));
    SWItest();
    
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
