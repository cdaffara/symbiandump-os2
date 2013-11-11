/*
* Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "tScriptTests.h"
#include "t_testhandler.h"
#include "t_certstoretests.h"
#include "t_certstoreactions.h"

#include <e32std.h>
#include <f32file.h>
#include <ecom/ecom.h>


///////////////

#include "t_testsetup.h"
#include "t_testactionspec.h"
#include "t_input.h"
#include "t_certstoreactionmemfail.h"
#include "tcancel.h"
#include "t_message.h"
#include "tScriptSetup.h"
#include "tHardcodedSetup.h"
#include "t_testhandler.h"
#include "t_output.h"
#include "tTestSpec.h"
#include "Ttesthandlersettings.h"

/**
*	--------------------------------------
*	How this test works:
*	--------------------------------------
*
*	RUN THIS TEST IN CONFIGURATION 1 (FILECERTSTORE.DLL SHOULD BE THE ONLY 
*	ECOM PLUGIN IN Z:\SYSTEM\LIBS\PLUGINS).  IF WAPCERTSTORE.DLL IS PRESENT
*	THIS TEST WILL HANG!!!
*
*	This test is designed to stress concurrent access to the filecertstore
*	by multiple threads, and to check that the integrity of the store is
*	retained.
*
*	The test consists of 3 scripts (certstoreconcurrent1-3.txt) which test various
*	possible accesses to the filebased store (cert add, delete, set trust, applications
*	and certificate retreival.  There are 27 possible ways to combine the 3 scripts.
*	For each of these combinations, 3 threads are started and each is assigned a
*	separate test handler and one of the scripts for that combination.  The threads
*	then run together, accessing the store concurrently. 
*	
*	BECAUSE THE THREADS ARE RUNNING CONCURRENTLY, IT IS NOT POSSIBLE TO PREDICT
*	THE RESULTS OF EACH TEST, EG IT IS NOT POSSIBLE TO DETERMINE WHETHER THE 
*	CERTIFICATE THAT ONE THREAD WISHES TO DELETE IS ACTUALLY PRESENT IN THE STORE
*	AT THAT TIME, SINCE ANOTHER THREAD MAY HAVE REMOVED IT.  BECAUSE OF THIS 
*	THE SCRIPTS ARE MARKED WITH A testconcurrent FLAG TO INDICATE THAT THE FAIL
*	RESULTS SHOULD BE DISREGARDED.  The results for each script are written to a
*	separate file in EPOC directory \tcertstoreconcurrent\ on system drive thus by the end of the
*	the test there are 81 such log files in the directory.
*
*	Following the 27 combinations of 3 threads, the test then runs a standard
*	tcertstore test using one of the scripts used in general certstore testing
*	(the script is determined by the command line for the entire test).  This
*	runs in a single thread so results can be predicted.  Thus we check that 
*	filecertstore integrity is maintained.  The log file for these tests are
*	placed in EPOC system drive and should be inspected for errors as part of the
*	testing procedure.
*
*	Thus to run these tests, the following command line should be used:
*	tcertstoreconcurrent \tcertstore\scripts\unifiedcertstore2-conf1.txt \tcertstoreconcurrent1.log
*	,the script and log file being on system drive.This runs script unifiedcertstore2-conf1 after the 
*	threaded tests and logs	the test results to tcertstoreconcurrent1.log
*/

//	3 scripts available, switch between them
const static TText* scripts[] = {	_S("dummy for zero element"),
									_S("\\tcertstoreconcurrent\\scripts\\certstoreconcurrent1.txt"), 
									_S("\\tcertstoreconcurrent\\scripts\\certstoreconcurrent2.txt"),
									_S("\\tcertstoreconcurrent\\scripts\\certstoreconcurrent3.txt")};


const TInt KMaxIterations = 27;

const TInt scriptCombinations[] = {	1,1,1,	1,1,2,	1,1,3,
									1,2,1,	1,2,2,	1,2,3,
									1,3,1,	1,3,2,	1,3,3,
									2,1,1,	2,1,2,	2,1,3,
									2,2,1,	2,2,2,	2,2,3,
									2,3,1,	2,3,2,	2,3,3,
									3,1,1,	3,1,2,	3,1,3,
									3,2,1,	3,2,2,	3,2,3,
									3,3,1,	3,3,2,	3,3,3};

class TThreadData
	{
public:
	void InitialiseL(TInt aIteration, TInt aScriptNum);
public:	
	TPtrC iScriptFile;
	TFileName iLogFile;
	};

void TThreadData::InitialiseL(TInt aIteration, TInt aThreadNum)
	{
	ASSERT(aIteration >= 0 && aIteration < KMaxIterations);
	ASSERT(aThreadNum >= 1 && aThreadNum <= 3);

	TInt script = scriptCombinations[aIteration * 3 + aThreadNum - 1];		
	
	// Set script file
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysdriveName (sysDrive.Name());
	TBuf <60> scriptFile (sysdriveName);
	scriptFile.Append(scripts[script]);
	iScriptFile.Set(scriptFile);

	// Set log file
	iLogFile.Zero();
	TBuf<80> scriptName (sysdriveName);
	scriptName.Append(_L("\\tcertstoreconcurrent\\iteration%02d_thread%d_script%d.txt"));
	TBuf<80> buf ;
	buf.Format(scriptName,aIteration, aThreadNum, script); 
	iLogFile.Append(buf);
	}


LOCAL_C TInt ThreadEntryPoint(TAny* aArg)
{
	__UHEAP_MARK;

	CTrapCleanup* cleanup=CTrapCleanup::New();

	TThreadData* data = static_cast<TThreadData*>(aArg);
	ASSERT(data);

	TRAPD(r, DoTests(data->iScriptFile, data->iLogFile, ETrue));
	//TRAPD(r, PerformTests(TestTypes(), data->iScriptFile, data->iLogFile));

	ASSERT( (r==KErrNone) || (r==KErrInUse) );

	delete cleanup;
	
	__UHEAP_MARKEND;

	return (r);
}

/** Start a thread. */
LOCAL_D void StartThreadL(RThread& aThread, TThreadData& aData, TInt aIteration, TInt aThreadNum, TRequestStatus& aStatus)
	{
	aData.InitialiseL(aIteration, aThreadNum);

	TBuf<32> threadName;
	threadName.Format(_L("iteration%02d_thread%d"), aIteration, aThreadNum);
	
	RHeap* heap = User::ChunkHeap(NULL, KMinHeapSize, 0x100000);
	User::LeaveIfNull(heap);
	User::LeaveIfError(aThread.Create(threadName, ThreadEntryPoint, KDefaultStackSize, heap, (TAny*)&aData));	
	aStatus = KRequestPending;
	aThread.Logon(aStatus);
	aThread.Resume();
	}

//	Kicks off each thread for multiple concurrent certstore access
LOCAL_D TBool DoThreadedTestsL(CConsoleBase* console, HBufC* logFileName, TBool wait)
{
 	RFs myfs;
 	CleanupClosePushL(myfs);
 
 	RFile logfile;
 	CleanupClosePushL(logfile);
 
 	Output* out;
 	
 	User::LeaveIfError(myfs.Connect());
 	User::LeaveIfError(logfile.Replace(myfs, *logFileName, EFileWrite));
 
 	out = new (ELeave) FileOutput(logfile);
 	CleanupStack::PushL(out);

	TInt failureCount = 0;
	
	for (TInt i = 0 ; i < KMaxIterations; ++i)
		{
		console->Printf(_L("Iteration %d \n"), i);
		out->writeNewLine();
		out->writeString(_L("Iteration "));
		out->writeNum(i);
		out->writeNewLine();

		RThread thread1;
		TThreadData data1;
		TRequestStatus status1;
		StartThreadL(thread1, data1, i, 1, status1);

		RThread thread2;
		TThreadData data2;
		TRequestStatus status2;
		StartThreadL(thread2, data2, i, 2, status2);

		RThread thread3;
		TThreadData data3;
		TRequestStatus status3;
		StartThreadL(thread3, data3, i, 3, status3);
		
		User::WaitForRequest(status1);
		User::WaitForRequest(status2);
		User::WaitForRequest(status3);

		TExitType exit1 = thread1.ExitType();
		TExitType exit2 = thread2.ExitType();
		TExitType exit3 = thread3.ExitType();
		
		if (exit1 != EExitKill) 
			{
			console->Printf(_L("ERROR: Thread 1 exited with exit type: %d \n"), exit1);
 			out->writeString(_L("ERROR: Thread 1 exited with exit type: "));
			out->writeNum(exit1);
	 		out->writeNewLine();
			failureCount++;
			}
		if (exit2 != EExitKill) 		
			{
			console->Printf(_L("ERROR: Thread 2 exited with exit type: %d \n"), exit2);
 			out->writeString(_L("ERROR: Thread 2 exited with exit type: "));
			out->writeNum(exit2);
	 		out->writeNewLine();			
			failureCount++;
			}
		if (exit3 != EExitKill) 
			{
			console->Printf(_L("ERROR: Thread 3 exited with exit type: %d \n"), exit3);
 			out->writeString(_L("ERROR: Thread 2 exited with exit type: "));
			out->writeNum(exit3);
	 		out->writeNewLine();			
			failureCount++;
			}
				
		thread1.Heap()->Close();
		thread2.Heap()->Close();
		thread3.Heap()->Close();
		
		thread1.Close();
		thread2.Close();
		thread3.Close(); 

		User::LeaveIfError(status1.Int());
		User::LeaveIfError(status2.Int());
		User::LeaveIfError(status3.Int());		
	}
	if (failureCount > 0) 
 		{
 		out->writeNewLine();
		console->Printf(_L("\n %d tests failed out of %d \n"), failureCount, (KMaxIterations*3));
 		out->writeNewLine();
 		out->writeNum(failureCount);
 		out->writeString(_L(" tests failed out of "));
 		out->writeNum(KMaxIterations*3);
 		out->writeNewLine();		
 		}
 	if (wait) 
 		{
		console->Printf(_L("\n Press any key to continue \n"));		
		console->Getch();
 		}
 	CleanupStack::PopAndDestroy(out);
 	CleanupStack::PopAndDestroy(&logfile);
 	CleanupStack::PopAndDestroy(&myfs); 	
 	if (failureCount>0) 
 		{
 		return EFalse;
 		}
 	return ETrue;
}


/**
 * Extracts the nPos command line argument.
 */
LOCAL_D HBufC* GetArgument(TInt nPos)
	{
	HBufC *argv = HBufC::NewLC(User::CommandLineLength());
	TPtr cmd(argv->Des());
	User::CommandLine(cmd);

	TLex arguments(cmd);

	// finds nth parameter 
	while(nPos && !arguments.Eos())
		{
		TPtrC token = arguments.NextToken();
		if(token.Length() > 0)		
			nPos--;
		}

	HBufC* result = NULL;
	if(!arguments.Eos())
		{
		TPtrC testfile(arguments.NextToken());

		if(testfile.Length() > 0)
			result = testfile.AllocL();
		};

	// no parameter found, but must return something so..
	if(!result)
		result = HBufC::NewL(0);

	CleanupStack::PopAndDestroy(argv);

	return result;
	}

/**
 * This function sets up a console, a log file and checks
 * whether we need to wait for a key pressed after test 
 * completion.
 * First DoThreadedTestsL is called, if everything is ok
 * it return ETrue and we move on to the standard tests.
 * If something went amiss (return EFalse) we skip 
 * the standard test and return. 
 */
LOCAL_D void SetupAndRunTests() 
{

	CConsoleBase* console = Console::NewL(_L("Test code"), TSize(KConsFullScreen, KConsFullScreen));
	
	HBufC* logFile = GetArgument(1);
	
	if (logFile->Length()==0) 
		{
		_LIT(defaultLog, "\\tcertstore.log");
		TDriveUnit sysDrive (RFs::GetSystemDrive());
		TDriveName sysdriveName (sysDrive.Name());
		TBuf <18> fileName (sysdriveName);
		fileName.Append(defaultLog);
		logFile->ReAlloc(18);
		TPtr16 plog = logFile->Des();
		plog.Append(fileName);
		}
		
	HBufC* wait = GetArgument(2);

	TBool waitAfterCompletion = EFalse;
	if (wait->Find(_L("-w")) != KErrNotFound) 
		{
		waitAfterCompletion = ETrue;
		}
	
	TBool res = EFalse;	
	TRAPD(err, res = DoThreadedTestsL(console, logFile, waitAfterCompletion));
	
	if (res) 
		{
		// Now run a normal tcertstore test to check store integrity
		TRAP(err, DoTests());
		}
		
	delete console;
	delete wait;
	delete logFile;
}

GLDEF_C TInt E32Main()
{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	
	TRAPD(err, SetupAndRunTests());
	
	REComSession::FinalClose();

	delete cleanup;

	__UHEAP_MARKEND;
	return 0;
}
