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
#include "featmgrserver.h"
#include "featmgrconfiguration.h"

static TInt TheProcessHandleCount = 0;
static TInt TheThreadHandleCount = 0;
static TInt TheAllocatedCellsCount = 0;

#ifdef _DEBUG
static const TInt KBurstRate = 20;
#endif

void DestroyTestEnv();
///////////////////////////////////////////////////////////////////////////////////////

static RTest TheTest(_L("t_fmgrstartup"));

#ifdef EXTENDED_FEATURE_MANAGER_TEST
_LIT( KZOrgFeaturesFile, "Z:\\Private\\10205054\\features.dat" );
_LIT( KFeaturesFile, "C:\\Private\\102836E5\\features.dat" );
_LIT( KFeaturesDir, "C:\\Private\\102836E5\\" );
#endif

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
void CreateTestEnv()
    {
    //KFeaturesDir is defined only if EXTENDED_FEATURE_MANAGER_TEST is defined     
#ifdef EXTENDED_FEATURE_MANAGER_TEST    
    RFs fs;
    TInt err = fs.Connect();
    TEST2(err, KErrNone);
    
    err = fs.MkDir(KFeaturesDir);
    TEST (err == KErrNone || err == KErrAlreadyExists);
    
    (void)fs.Delete(KFeaturesFile);
    err = BaflUtils::CopyFile(fs, KZOrgFeaturesFile, KFeaturesDir);
    TEST2 (err, KErrNone);
    err = fs.SetAtt(KFeaturesFile, KEntryAttNormal, KEntryAttReadOnly);
    TEST2 (err, KErrNone);
    fs.Close();
#endif
    }

//Deletes all created test files.
void DestroyTestEnv()
    {
    //KFeaturesDir is defined only if EXTENDED_FEATURE_MANAGER_TEST is defined     
#ifdef EXTENDED_FEATURE_MANAGER_TEST
    RFs fs;
    TInt err = fs.Connect();
    if(err == KErrNone)
        {
        err = fs.Delete(KFeaturesFile);
        if (err != KErrNone)
            {
            RDebug::Print(_L("Warning: Clean test enviroment failed with error %d"), err);
            }
        }
    fs.Close();
#endif        
    }
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

static void OomPreStep(TInt aFailingAllocationNo)
    {
    aFailingAllocationNo = aFailingAllocationNo; //to silent the warning in urel build
    MarkHandles();
    MarkAllocatedCells();
    __UHEAP_MARK;
    __UHEAP_SETBURSTFAIL(RAllocator::EBurstFailNext, aFailingAllocationNo, KBurstRate);
    }

static void OomPostStep()
    {
    __UHEAP_RESET;
    __UHEAP_MARKEND;
    CheckAllocatedCells();
    CheckHandles();
    }

///////////////////////////////////////////////////////////////////////////////////////

static void CreateAndDestroyFeatMgrServerL()
    {
    CFeatMgrServer* server = CFeatMgrServer::NewLC(KServerCActivePriority);
    CleanupStack::PopAndDestroy(server);
    }

/**
@SYMTestCaseID          PDS-EFM-CT-4109
@SYMTestCaseDesc        
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void FeatMgrServerStartupOomTest()
    {
    TInt err = KErrNoMemory;
    TInt failingAllocationNo = 0;
    TheTest.Printf(_L("Iteration:\r\n"));
    while(err == KErrNoMemory)
        {
        TheTest.Printf(_L(" %d"), ++failingAllocationNo);
        OomPreStep(failingAllocationNo);
        TRAP(err, CreateAndDestroyFeatMgrServerL());
        OomPostStep();
        }
    if(err != KErrNoMemory)
        {
        TEST2(err, KErrNone);   
        }
    TheTest.Printf(_L("\r\n===OOM test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
    }

/**
@SYMTestCaseID          PDS-EFM-CT-4110
@SYMTestCaseDesc        
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void FeatMgrServerStartupFileIoTest()
    {
    RFs fs;
    TInt err = fs.Connect();
    TEST2(err, KErrNone);
    err = KErrNotFound;
    TInt cnt=1;
    for(;err<KErrNone;++cnt)
        {
        TheTest.Printf(_L("===Iteration %d. Simulated error:\r\n"), cnt);       
        for (TInt fsError=KErrNotFound;fsError>=KErrBadName;--fsError)
            {
//            if(fsError == KErrNotFound || fsError == KErrCorrupt || fsError == KErrPathNotFound || fsError == KErrEof)
//                {
//                continue;//The server code panics
//                }
            TheTest.Printf(_L("%d "), fsError);
            (void)fs.SetErrorCondition(fsError, cnt);
            TRAP(err, CreateAndDestroyFeatMgrServerL());
            (void)fs.SetErrorCondition(KErrNone);
            }
        TheTest.Printf(_L("\r\n"));
        }
    fs.Close();
    TheTest.Printf(_L("\r\n===File I/O error simulation test succeeded on iteration %d===\r\n"), cnt);
    }


void DoTestsL()
    {
    CActiveScheduler* scheduler = new CActiveScheduler;
    TEST(scheduler != NULL);
    CActiveScheduler::Install(scheduler);
    
    TheTest.Start(_L("@SYMTestCaseID:PDS-EFM-CT-4109 CFeatMgrServer::NewLC() OOM test"));
    FeatMgrServerStartupOomTest();

    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4110 CFeatMgrServer::NewLC() file I/O error simulation test"));
    FeatMgrServerStartupFileIoTest();
    
    delete scheduler;
    }

TInt E32Main()
    {
    TheTest.Title();
    
    CTrapCleanup* tc = CTrapCleanup::New();
    TheTest(tc != NULL);
    
    __UHEAP_MARK;
    
    CreateTestEnv();
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
