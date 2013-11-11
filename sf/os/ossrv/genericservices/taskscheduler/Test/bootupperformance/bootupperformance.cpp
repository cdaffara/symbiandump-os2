//
// bootupperformance.cpp

// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// For testing Boot-up Performance Task Scheduler
// 
//

/**
 @file
 @internalComponent
 @test
*/
//
#include <e32base.h>
#include "e32debug.h"
#include <hal.h>
#include <csch_cli.h>
#include <bautils.h>
#include <f32file.h>
#include <e32test.h>

_LIT(KTestName,	"Boot-up Performance Task Scheduler");
RTest	TheTest(KTestName);

//For time counter
TInt fastTimerFreq;
TReal ticksPerMicroSec; 
TReal64 fsSessionMicroSecs;
TUint prevTime;
TUint timeDiff;

//file server session
RFs TheFs;

//Client side interface to the Task Scheduler
static RScheduler	TheScheduler;

//Backup file name of ask Scheduler
_LIT(KBackupFileName, "_:\\Private\\10005399\\SchedulesBackup.dat");
_LIT(KBackupDir, "_:\\Private\\10005399\\");

static TBuf<64> backupFileName;
static TBuf<32> backupDir;

/**
 * This function will create a dummy new task
 * It is used for test performance of Task Scheduler
 */ 
void CreateNewTask()
{
	TScheduleEntryInfo aScheduleInfo;
	aScheduleInfo.iIntervalType = EHourly;
	aScheduleInfo.iInterval = 1;			//every Hour
	aScheduleInfo.iStartTime = TTime(TDateTime(2020,EJuly,3,13,36,0,0));
	aScheduleInfo.iValidityPeriod = KMaxTInt;

	TInt err;
	err = TheScheduler.Connect();
	TheTest(err == KErrNone);
	
	TBuf<255>aFileName = _L("bootupperformance");

	TName  aName = _L("Performance Test");
	TTaskInfo aTaskInfo(0, aName, 1, -1);
	err = TheScheduler.Register( aFileName, 1 );
	TheTest(err == KErrNone);
	
	//only one event to add
	CArrayFixFlat<TScheduleEntryInfo>* array = new (ELeave) CArrayFixFlat<TScheduleEntryInfo>(1);
	CleanupStack::PushL( array );
	array->AppendL( aScheduleInfo );

	TSchedulerItemRef aItemRef;
	TheScheduler.CreatePersistentSchedule( aItemRef, *array );

	HBufC* aBuf = HBufC::NewLC(1);      
	TheScheduler.ScheduleTask( aTaskInfo, *aBuf, aItemRef, *array );
	
	CleanupStack::PopAndDestroy( 2 );
	TheScheduler.Close();
}

/**
 * This function Kill Task Scheduler if it is running
 * It is used for test performance of Task Scheduler
 */ 
void KillTaskScheduler()
	{
	TFindProcess findProcess (_L("Schexe.*"));
	TFullName result;
	while(findProcess.Next(result) == KErrNone )
		{
		TRequestStatus stat;
		RProcess processHandle;
		processHandle.Open (findProcess);
		processHandle.Kill(0);
		processHandle.Logon(stat);
		User::WaitForRequest(stat);
		processHandle.Close();
		User::After(1000000);
		}
	}

/**
 * This function will Start Task Scheduler directly calling Schexe.exe
 * It is used for test performance of Task Scheduler
 */ 	
void StartTaskScheduler()
	{
	TRequestStatus stat;
	RProcess server;
	_LIT(KScheduleServerExe, "Schexe");
	User::LeaveIfError(server.Create(KScheduleServerExe, _L("sysstartschexe")));
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	User::WaitForRequest(stat);		// wait for start or death
	}

//Tests
//===================================================================

/**
@SYMTestCaseID          SYSLIB-SCHSVR-PT-1865
@SYMTestCaseDesc        Measure the overhead to Start Task Scheduler from client
@SYMTestPriority        High
@SYMTestActions         Start Task Scheduler by calling Connect & Close
@SYMTestExpectedResults Tests must not fail
@SYMDEF                 DEF074891: Task Scheduler: Persistent Schedules not ReScheduled on Device Bootup
*/
void PerformanceTestTS1()
	{
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-PT-1865 Start Task Scheduler from client "));	
	
	//Kill the Task Scheduler if running 
	KillTaskScheduler();

//Test Case 1
	//Delete SchedulesBackup.dat file
	BaflUtils::DeleteFile(TheFs, backupFileName);
	TheFs.RmDir(backupDir);

	//Performance Test to start Task Scheduler first run
	prevTime = User::FastCounter();

	TheScheduler.Connect();
	TheScheduler.Close();
	
	timeDiff = User::FastCounter() - prevTime;
	fsSessionMicroSecs = timeDiff / (ticksPerMicroSec);
	TheTest.Printf(_L("\nTime to start Task Scheduler from client first run (no file)= %10.2lf microseconds\n"), fsSessionMicroSecs);

//Test Case 2
	//Performance Test to start Task Scheduler second run

	prevTime = User::FastCounter();

	TheScheduler.Connect();
	TheScheduler.Close();
	
	timeDiff = User::FastCounter() - prevTime;
	fsSessionMicroSecs = timeDiff / (ticksPerMicroSec);
	TheTest.Printf(_L("\nTime to start Task Scheduler from client second run (with file) = %10.2lf microseconds\n"), fsSessionMicroSecs);
	
	}
	
/**
@SYMTestCaseID          SYSLIB-SCHSVR-PT-1866
@SYMTestCaseDesc        Measure the overhead to Start Task Scheduler directly
@SYMTestPriority        High
@SYMTestActions         Start Task Scheduler exe directly and measure the performance
@SYMTestExpectedResults Tests must not fail
@SYMDEF                 DEF074891: Task Scheduler: Persistent Schedules not ReScheduled on Device Bootup
*/
void PerformanceTestTS2()
	{
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-PT-1866 Start Task Scheduler exe directly "));	
	
//Test Case 1	
	KillTaskScheduler();
	
	//Delete SchedulesBackup.dat file
	BaflUtils::DeleteFile(TheFs, backupFileName);
	TheFs.RmDir(backupDir);
	
	//Performance Test to start Task Scheduler first run
	prevTime = User::FastCounter();

	StartTaskScheduler();
	
	timeDiff = User::FastCounter() - prevTime;
	fsSessionMicroSecs = timeDiff / (ticksPerMicroSec);
	TheTest.Printf(_L("\nTime to start Task Schedular directly first run (no file) = %10.2lf microseconds\n"), fsSessionMicroSecs);

//Test Case 2
	//Performance Test to start Task Scheduler first run
	prevTime = User::FastCounter();

	StartTaskScheduler();
	
	timeDiff = User::FastCounter() - prevTime;
	fsSessionMicroSecs = timeDiff / (ticksPerMicroSec);
	TheTest.Printf(_L("\nTime to start Task Schedular directly second run (with file) = %10.2lf microseconds\n"), fsSessionMicroSecs);
	
//Test Case 3
	CreateNewTask();
	
	KillTaskScheduler();
	//Performance Test to start Task Scheduler first run
	prevTime = User::FastCounter();

	StartTaskScheduler();
	
	timeDiff = User::FastCounter() - prevTime;
	fsSessionMicroSecs = timeDiff / (ticksPerMicroSec);
	TheTest.Printf(_L("\nTime to Start Task Schedular with Schedule = %10.2lf microseconds\n"), fsSessionMicroSecs);

	}

/**
Runs all the tests.
*/
void RunTestsL()
	{
	PerformanceTestTS1();
	PerformanceTestTS2();
	}

GLDEF_C TInt E32Main() // main function called by E32
    {
    
  	CTrapCleanup* tc = CTrapCleanup::New();
  	__UHEAP_MARK;

	backupFileName.Copy(KBackupFileName);
	backupFileName[0] = RFs::GetSystemDriveChar();
	
	backupDir.Copy(KBackupDir);
	backupDir[0] = RFs::GetSystemDriveChar();
  	
  	TheTest.Start(_L("=== Start Task Scheduler tests \n"));
	TheTest.Title();
	HAL::Get(HALData::EFastCounterFrequency, fastTimerFreq);
	ticksPerMicroSec = 1.0E-6 * fastTimerFreq;
	
	User::LeaveIfError(TheFs.Connect());

	TRAPD(err,RunTestsL());
	TheTest(err == KErrNone);
	
	//Clean up 	Backup File
	BaflUtils::DeleteFile(TheFs, backupFileName);
	TheFs.RmDir(backupDir);
	TheFs.Close();
	TheTest.End();
	TheTest.Close();
	
	__UHEAP_MARKEND;
	delete tc;
	return 0; // and return
    }
