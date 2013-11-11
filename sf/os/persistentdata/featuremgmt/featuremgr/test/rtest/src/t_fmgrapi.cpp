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
#include <featmgr.h>
#include <featureuids.h>
#include <featurecontrol.h>
#include <featdiscovery.h>

using namespace NFeature;

const TInt KInvalidFeatureId1    = 90901671;
const TInt KInvalidNegFeatureId2 = -90901671;
const TUid KInvalidFeatureUid1  = {KInvalidFeatureId1};

///////////////////////////////////////////////////////////////////////////////////////

static RTest TheTest(_L("t_fmgrapi"));
TUid SupportedFeature   = KConnectivity;  //Defaulted to KConnectivity, but we will re-assign it in FeatureTestPreparation().
TUid SupportedFeature2  = KSip;          //Defaulted to KSip, but we will re-assign it in FeatureTestPreparation().

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
@SYMTestCaseID          PDS-EFM-CT-4059
@SYMTestCaseDesc        
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void FeatureSupportedTestL()
    {
    FeatureManager::InitializeLibL();
    FeatureManager::InitializeLibL();
    
    //Feature, default present
    TBool rc = FeatureManager::FeatureSupported(KConnectivity.iUid);
    TEST(rc);
    //Feature, default not present
    rc = FeatureManager::FeatureSupported(KPrint.iUid);
    TEST(rc);
    
    //Ivalid feature UID
    rc = FeatureManager::FeatureSupported(KInvalidFeatureId1);
    TEST(!rc);
    //Ivalid feature UID - negative
    rc = FeatureManager::FeatureSupported(KInvalidNegFeatureId2);
    TEST(!rc);

    FeatureManager::UnInitializeLib();  
    FeatureManager::UnInitializeLib();  
    FeatureManager::UnInitializeLib();//it should be safe to call UnInitializeLib() even without matching InitializeLibL() call   
    }

/**
@SYMTestCaseID          PDS-EFM-CT-4060
@SYMTestCaseDesc        
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void FeatureControlTest1()
    {
    RFeatureControl ctrl;
    TInt err = ctrl.Open();
    TEST2(err, KErrNone);
    /////////////////////////////////////////////////////////////
    RFeatureUidArray farray;
    err = ctrl.ListSupportedFeatures(farray);
    TEST2(err, KErrNone);
    TheTest.Printf(_L("RFeatureControl::ListSupportedFeatures()\r\n"));
    for(TInt i=0;i<farray.Count();++i)
        {
        TheTest.Printf(_L("  Feature: %08X\r\n"), farray[i].iUid);
        }
    /////////////////////////////////////////////////////////////
    RFeatureArray farray2;
    for(TInt i=0;i<farray.Count();++i)
        {
        err = farray2.Append(farray[i]); 
        TEST2(err, KErrNone);
        }
    
    err = ctrl.FeaturesSupported(farray2);
    TEST2(err, KErrNone);

    TheTest.Printf(_L("RFeatureControl::FeaturesSupported()\r\n"));
    for(TInt i=0;i<farray2.Count();++i)
        {
        TheTest.Printf(_L("  Feature: %08X, Flags: %08X, Data: %08X\r\n"), farray2[i].FeatureUid(), farray2[i].FeatureFlags().iFlags, farray2[i].FeatureData());
        }
    
    RFeatureArray farray3;
    err = ctrl.FeaturesSupported(farray3);
    TEST2(err, KErrArgument);
    
    /////////////////////////////////////////////////////////////
    farray2.Close();
    farray.Close();
    ctrl.Close();
    ctrl.Close();//It should be safe to call Close() again
    }

/** 
@SYMTestCaseID          PDS-EFM-CT-4061
@SYMTestCaseDesc        
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void FeatureControlTest2()
    {
    RFeatureControl ctrl;
    TInt err = ctrl.Open();
    TEST2(err, KErrNone);
    /////////////////////////////////////////////////////////////
    RFeatureUidArray farray;
    err = ctrl.ListSupportedFeatures(farray);
    TEST2(err, KErrNone);
    for(TInt i=0;i<farray.Count();++i)
        {
        err = ctrl.FeatureSupported(farray[i]);
        TEST2(err, KFeatureSupported);
        
        TFeatureEntry feat(farray[i]);
        err = ctrl.FeatureSupported(feat);
        TEST2(err, KFeatureSupported);
        }

    err = ctrl.FeatureSupported(TUid::Uid(KInvalidFeatureId1));
    TEST2(err, KErrNotFound);

    TFeatureEntry feat1(TUid::Uid(KInvalidFeatureId1));
    err = ctrl.FeatureSupported(feat1);
    TEST2(err, KErrNotFound);

    const TUint KFlags = 0x0134357;
    const TUint KData = 0xAB5234;
    TFeatureEntry feat2(TUid::Uid(KInvalidFeatureId1), KFlags, KData);
    err = ctrl.FeatureSupported(feat2);
    TEST2(err, KErrNotFound);
    
    /////////////////////////////////////////////////////////////
    farray.Close();
    ctrl.Close();
    }

/** 
@SYMTestCaseID          PDS-EFM-CT-4062
@SYMTestCaseDesc        
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void FeatureControlTest3()
    {
    RFeatureControl ctrl;
    TInt err = ctrl.Open();
    TEST2(err, KErrNone);
    
    //Retrieve a uid list of all supported features
    RFeatureUidArray farray;
    err = ctrl.ListSupportedFeatures(farray);
    TEST2(err, KErrNone);
    //Retrieve a TFeaureEntry list of all supported features
    RFeatureArray farray2;
    for(TInt i=0;i<farray.Count();++i)
        {
        err = farray2.Append(farray[i]); 
        TEST2(err, KErrNone);
        }
    err = ctrl.FeaturesSupported(farray2);
    TEST2(err, KErrNone);
    //Play with EnableFeature()/DisableFeature() calls. If the EFeatureModifiable flag is not set then 
    //the feature cannot be enabled or disabled.
    for(TInt i=0;i<farray2.Count();++i)
        {
        const TFeatureEntry& fentry = farray2[i];
        TBitFlags32 flags = fentry.FeatureFlags();
        
        err = ctrl.DisableFeature(fentry.FeatureUid());
        TEST2(err, flags.IsSet(EFeatureModifiable) ? KErrNone : KErrAccessDenied);
        
        err = ctrl.EnableFeature(fentry.FeatureUid());
        TEST2(err, flags.IsSet(EFeatureModifiable) ? KErrNone : KErrAccessDenied);
        }
    //It is impossible to set non-existing feature
    const TUid KNewFeatureUid = {0x7888ABCD}; 
    err = ctrl.SetFeature(KNewFeatureUid, ETrue, 0x0);
    TEST2(err, KErrNotFound);
    //It is impossible to set non-modifiable feature
    TBitFlags32 flags = farray2[0].FeatureFlags();
    err = ctrl.SetFeature(farray2[0].FeatureUid(), ETrue, 0x0);
    TEST2(err, flags.IsSet(EFeatureModifiable) ? KErrNone : KErrAccessDenied);
    //Add new feature
    flags.ClearAll();
    flags.Set(EFeatureSupported);
    flags.Set(EFeatureModifiable);
    TFeatureEntry fentry(KNewFeatureUid, flags, 0x0);
    err = ctrl.AddFeature(fentry);
    TEST2(err, KErrNone);
    //Retrieve the new feature and check flags
    err = ctrl.FeatureSupported(fentry);
    TEST2(err, KFeatureSupported);
    flags = fentry.FeatureFlags();
    TEST(flags.IsSet(EFeatureSupported));
    TEST(flags.IsSet(EFeatureModifiable));
    //Now, it should be possible to set the new feature - it is modifiable.
    err = ctrl.SetFeature(fentry.FeatureUid(), ETrue, 0x0);
    TEST2(err, KErrNone);
    err = ctrl.SetFeature(fentry.FeatureUid(), EFalse, 0x0);
    TEST2(err, KErrNone);
    //Enable/disable
    err = ctrl.EnableFeature(fentry.FeatureUid());
    TEST2(err, KErrNone);
    err = ctrl.DisableFeature(fentry.FeatureUid());
    TEST2(err, KErrNone);
    //Delete the added feature
    err = ctrl.DeleteFeature(fentry.FeatureUid());
    TEST2(err, KErrNone);
    err = ctrl.DeleteFeature(fentry.FeatureUid());
    TEST2(err, KErrNotFound);
    
    /////////////////////////////////////////////////////////////
    farray2.Close();
    farray.Close();
    ctrl.Close();
    }

/** 
@SYMTestCaseID          PDS-EFM-CT-4063
@SYMTestCaseDesc        
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void FeatureDiscoveryTest1L()
    {
    TBool rc = CFeatureDiscovery::IsFeatureSupportedL(KInvalidFeatureId1);
    TEST(!rc);
    rc = CFeatureDiscovery::IsFeatureSupportedL(SupportedFeature.iUid);
    TEST(rc);
    
    rc = CFeatureDiscovery::IsFeatureSupportedL(KInvalidFeatureUid1);
    TEST(!rc);
    rc = CFeatureDiscovery::IsFeatureSupportedL(SupportedFeature);
    TEST(rc);

    CFeatureDiscovery* fdiscovery = CFeatureDiscovery::NewLC();
    
    rc = fdiscovery->IsSupported(KInvalidFeatureId1);
    TEST(!rc);
    rc = fdiscovery->IsSupported(SupportedFeature.iUid);
    TEST(rc);

    rc = fdiscovery->IsSupported(KInvalidFeatureUid1);
    TEST(!rc);
    rc = fdiscovery->IsSupported(SupportedFeature);
    TEST(rc);
    
    CleanupStack::PopAndDestroy(fdiscovery);
    }

void DoFeatureDiscoveryTest2L(TBool aStaticMethodsUsed)
    {
    CFeatureDiscovery* fdiscovery = NULL;
    if(!aStaticMethodsUsed)
        {
        fdiscovery = CFeatureDiscovery::NewL();
        }
    //////////////////////////////////////////////////////////
    //A test with a set: one valid and one invalid feature 
    TFeatureSet fset;
    TInt err = fset.Append(SupportedFeature);
    TEST2(err, KErrNone);
    err = fset.Append(KInvalidFeatureUid1);
    TEST2(err, KErrNone);
    TBool rc = fset.IsFeatureSupported(SupportedFeature);
    TEST(!rc);
    rc = fset.IsFeatureSupported(KInvalidFeatureUid1);
    TEST(!rc);
    if(aStaticMethodsUsed)
        {
        TRAP(err, CFeatureDiscovery::FeaturesSupportedL(fset));
        }
    else
        {
        err = fdiscovery->FeaturesSupported(fset);
        }
    TEST2(err, KErrNone);
    rc = fset.IsFeatureSupported(SupportedFeature);
    TEST(rc);
    rc = fset.IsFeatureSupported(KInvalidFeatureUid1);
    TEST(!rc);
    rc = fset.AreAllFeaturesSupported();
    TEST(!rc);
    //////////////////////////////////////////////////////////
    //A test with an empty set 
    TFeatureSet fset2;
    rc = fset2.IsFeatureSupported(SupportedFeature);
    TEST(!rc);
    rc = fset2.IsFeatureSupported(KInvalidFeatureUid1);
    TEST(!rc);
    if(aStaticMethodsUsed)
        {
        TRAP(err, CFeatureDiscovery::FeaturesSupportedL(fset2));
        }
    else
        {
        err = fdiscovery->FeaturesSupported(fset2);
        }
    TEST2(err, KErrArgument);
    rc = fset2.IsFeatureSupported(SupportedFeature);
    TEST(!rc);
    rc = fset2.IsFeatureSupported(KInvalidFeatureUid1);
    TEST(!rc);
    rc = fset2.AreAllFeaturesSupported();
    TEST(rc);//because fset2 is empty
    //////////////////////////////////////////////////////////
    //A test with a set: two valid features 
    TFeatureSet fset3;
    err = fset3.Append(SupportedFeature);
    TEST2(err, KErrNone);
    err = fset3.Append(SupportedFeature2);
    TEST2(err, KErrNone);
    if(aStaticMethodsUsed)
        {
        TRAP(err, CFeatureDiscovery::FeaturesSupportedL(fset3));
        }
    else
        {
        err = fdiscovery->FeaturesSupported(fset3);
        }
    TEST2(err, KErrNone);
    rc = fset3.IsFeatureSupported(SupportedFeature);
    TEST(rc);
    rc = fset3.IsFeatureSupported(SupportedFeature2);
    TEST(rc);
    rc = fset3.AreAllFeaturesSupported();
    TEST(rc);
    //////////////////////////////////////////////////////////
    delete fdiscovery;
   }

/** 
@SYMTestCaseID          PDS-EFM-CT-4064
@SYMTestCaseDesc        
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void FeatureDiscoveryTest2L()
    {
    DoFeatureDiscoveryTest2L(ETrue);
    DoFeatureDiscoveryTest2L(EFalse);
    }

void FeatureTestPreparation()
    {
    RFeatureControl ctrl;
    TInt err = ctrl.Open();
    TEST2(err, KErrNone);
    /////////////////////////////////////////////////////////////
    RFeatureUidArray farray;
    TheTest.Printf(_L("Getting supported feature for CFeatureDiscovery test\r\n"));
    err = ctrl.ListSupportedFeatures(farray);
    if (err == KErrNone && farray.Count() > 1)
        {
        SupportedFeature = farray[0]; //Take the first supported value
        SupportedFeature2 = farray[1]; //Take the first supported value
        }
    else
        {
        TheTest.Printf(_L("Warning: Failed to get supported feature for testing. CFeatureDiscovery tests are going to fail\r\n"));
        }
    farray.Close();
    ctrl.Close();
    }

void DoTestsL()
    {
    FeatureTestPreparation(); //Preparation for the test.
    TheTest.Start(_L("@SYMTestCaseID:PDS-EFM-CT-4059 FeatureManager::FeatureSupported() test"));
    FeatureSupportedTestL();
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4060 RFeatureControl tests-1"));
    FeatureControlTest1();
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4061 RFeatureControl tests-2"));
    FeatureControlTest2();
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4062 RFeatureControl tests-3"));
    FeatureControlTest3();
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4063 CFeatureDiscovery tests-1"));
    FeatureDiscoveryTest1L();
    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4064 CFeatureDiscovery & TFeatureSet tests"));
    FeatureDiscoveryTest2L();
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
