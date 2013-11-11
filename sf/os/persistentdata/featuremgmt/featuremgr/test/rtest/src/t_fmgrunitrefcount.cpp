// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32debug.h>
#include <bautils.h>
#include <featmgr/featurecontrol.h>
#include <featmgr/featmgr.h>
#include <featdiscovery.h>
#include "featmgrtlsdata.h"

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_fmgrunitrefcount test"));

const TUid KDummyFeatUid = {0x12345678};

///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine)
    {
    if(!aValue)
        {
        //DeleteTestFiles();
        RDebug::Print(_L("*** Line %d\r\n"), aLine);
        TheTest(EFalse, aLine);
        }
    }

void Check2(TInt aValue, TInt aExpected, TInt aLine)
    {
    if(aValue != aExpected)
        {
        RDebug::Print(_L("*** Line %d, Expected error: %d, got: %d\r\n"), aLine, aExpected, aValue);
        TheTest(EFalse, aLine);
        }
    }
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

/**
@SYMTestCaseID          PDS-EFM-UT-4112
@SYMTestCaseDesc        Unit test for client reference counting.
@SYMTestPriority        High
@SYMTestActions         Initialise FeatureManager and check the client reference count
                        Connect using RFeatureControl and check the client reference count
                        Uninitialise FeatureManager and check the client reference count
                        Close RFeatureControl and check the client reference count
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/  
void TestRefCountFeatureManagerL()
    {
        RFeatureControl featCtrl;    
        
        CleanupClosePushL(featCtrl);
        FeatureManager::InitializeLibL();
        TEST2 (GetClientCount(), 1); // Client count should be 1 at this point
        
        featCtrl.Connect();
        TEST2 (GetClientCount(), 2); // Client count should be 2 at this point
        
        // Both should return same result
        TEST2 (FeatureManager::FeatureSupported(KDummyFeatUid.iUid), featCtrl.FeatureSupported(KDummyFeatUid)==KFeatureSupported);
        
        FeatureManager::UnInitializeLib();
        TEST2 (GetClientCount(), 1); // Client count should be 1 at this point
        
        CleanupStack::PopAndDestroy(&featCtrl);
        TEST2 (GetClientCount(), 0); // Client count should be 0 at this point
    }

/**
@SYMTestCaseID          PDS-EFM-UT-4113
@SYMTestCaseDesc        Unit test for client reference counting.
@SYMTestPriority        High
@SYMTestActions         Create CFeatureDiscovery object and check the client reference count
                        Connect using RFeatureControl and check the client reference count
                        Delete the CFeatureDiscovery object and check the client reference count
                        Close RFeatureControl and check the client reference count
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/  
void TestRefCountCFeatureDiscoveryL()
    {
        RFeatureControl featCtrl;
        CleanupClosePushL(featCtrl);
        CFeatureDiscovery* featDisc = CFeatureDiscovery::NewLC();    
        TEST2 (GetClientCount(), 1); // Client count should be 1 at this point
        featCtrl.Connect();
        TEST2 (GetClientCount(), 2); // Client count should be 2 at this point

        // Both should return same result
        TEST2 (featDisc->IsSupported(KDummyFeatUid), featCtrl.FeatureSupported(KDummyFeatUid)==KFeatureSupported);
        
        CleanupStack::PopAndDestroy(featDisc);
        TEST2 (GetClientCount(), 1); // Client count should be 1 at this point
        
        CleanupStack::PopAndDestroy(&featCtrl);
        TEST2 (GetClientCount(), 0); // Client count should be 0 at this point
    }

///////////////////////////////////////////////////////////////////////////////////////

void DoTestsL()
	{
    TheTest.Start(_L(" @SYMTestCaseID:PDS-EFM-UT-4112 Client Reference Count using FeatureManager"));
    TestRefCountFeatureManagerL();
    TheTest.Next(_L(" @SYMTestCaseID:PDS-EFM-UT-4113 Client Reference Count using CFeatureDiscovery"));
    TestRefCountCFeatureDiscoveryL();
    
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	
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
