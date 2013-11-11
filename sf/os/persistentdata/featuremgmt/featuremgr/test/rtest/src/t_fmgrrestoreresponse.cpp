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
#include <featurecontrol.h>
#include "t_fmgrbursim.h"

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_fmgrrestoreresponse"));

const TUint threadTimeout = 2000000;    // thread timeout = 2 seconds 

static RSemaphore MainThreadCrS;
static TBool featMgrIsResponsive = EFalse;

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine, TBool aPrintThreadName = EFalse)
    {
    if(!aValue)
        {
        //DeleteTestFiles();
        if(aPrintThreadName)
            {
            RThread th;
            TName name = th.Name();
            RDebug::Print(_L("*** Thread %S, Line %d\r\n"), &name, aLine);
            }
        else
            {
            RDebug::Print(_L("*** Line %d\r\n"), aLine);
            }
        TheTest(EFalse, aLine);
        }
    }

void Check2(TInt aValue, TInt aExpected, TInt aLine, TBool aPrintThreadName = EFalse)
    {
    if(aValue != aExpected)
        {
        //DeleteTestFiles();
        if(aPrintThreadName)
            {
            RThread th;
            TName name = th.Name();
            RDebug::Print(_L("*** Thread %S, Line %d Expected error: %d, got: %d\r\n"), &name, aLine, aExpected, aValue);
            }
        else
            {
            RDebug::Print(_L("*** Line %d, Expected error: %d, got: %d\r\n"), aLine, aExpected, aValue);
            }
        TheTest(EFalse, aLine);
        }
    }
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)
#define TTEST(arg) ::Check1((arg), __LINE__, ETrue)
#define TTEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__, ETrue)

// ------------------------- ------------------------- 
// setup and cleanup functions

TInt TestThreadL(void*)
    {
        __UHEAP_MARK;
        
        CTrapCleanup* tc = CTrapCleanup::New();
        RFeatureControl rfc;
        TTEST2( rfc.Connect(), KErrNone );
        
        // During restore, feature manager server should be responsive and return KErrServerBusy for write request 
        TInt err = rfc.EnableFeature( TUid::Uid(0x00000001) );
        TTEST2(err, KErrServerBusy);
		
        // During restore, feature manager server should be responsive and NOT return KErrServerBusy for read request
		err = rfc.FeatureSupported( TUid::Uid(0x00000001) );
		TTEST(err != KErrServerBusy);
		
        rfc.Close();
        featMgrIsResponsive = ETrue;
        RDebug::Print(_L("+++:TestThread: Query and Modification completed\r\n"));
        MainThreadCrS.Signal();
        delete tc;
        
        __UHEAP_MARKEND;
        
        return KErrNone;
    }
/**
@SYMTestCaseID          PDS-EFM-CT-4058
@SYMTestCaseDesc        Querying and modifying a feature during restore operation. 
                        Verify that a response is returned from the server during restore.
@SYMTestPriority        High
@SYMTestActions         Start simulating restore operation
                        Create a thread that will:
                        Modify a feature and verify that a response (KErrServerBusy) is received 
                        Query a feature and verify that a response is received (doesn't matter what the result is)
                        The thread should finished in less than 2 seconds.
                        Otherwise the test fail.          
@SYMTestExpectedResults Test must not fail
@SYMREQ                 
*/  
void TestRestoreResponseL()
    {
        _LIT(KThreadName, "RstTh");
        featMgrIsResponsive = EFalse;
        
        CFeatMgrBURSim* simulate = CFeatMgrBURSim::NewLC();
        RThread testThread;
        TRequestStatus testStatus;
        CleanupClosePushL( testThread );
        
        //Needs to ensure server is started before simulating backup operation
        RFeatureControl rfc;
        TTEST2( rfc.Connect(), KErrNone ); //This will start the server if not already started
        rfc.Close();
        
        simulate->Simulate_CheckRegFileL();
        
        // Simulate a restore
        RDebug::Print(_L("Simulating Restore of FeatMgr\r\n"));
        simulate->Simulate_StartRestoreL();

        TEST2( testThread.Create(KThreadName, &TestThreadL, 0x2000, 0x1000, 0x10000, NULL, EOwnerProcess), KErrNone );
        testThread.Logon(testStatus);
        TEST2( testStatus.Int(), KRequestPending );
        testThread.Resume();
        // Wait for 1.5 second for the query thread to finish. 
        RDebug::Print(_L("+++:MainThread: Wait for query and modification completion...\r\n"));
        MainThreadCrS.Wait(threadTimeout);
        // If query is responsive within the 1.5 second frame the following check should pass.
        TEST (featMgrIsResponsive);
        simulate->Simulate_EndRestoreL();
        
        CleanupStack::PopAndDestroy(&testThread);
        CleanupStack::PopAndDestroy(simulate);
    }

////////////////////////////////////////////////////////////////////////////////////
void DoTestsL()
	{
    MainThreadCrS.CreateLocal(0);
    
    TheTest.Start(_L(" @SYMTestCaseID:PDS-EFM-CT-4058 Restore Query and Modification Response"));
    TestRestoreResponseL();
    
    MainThreadCrS.Close();

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
