// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <d32dbms.h>
#include <e32math.h>
#include <e32test.h>
#include <f32file.h>
#include <s32buf.h>

#include "crccheck.h"

#undef __UHEAP_MARK
#undef __UHEAP_MARKEND
#define __UHEAP_MARK
#define __UHEAP_MARKEND

LOCAL_D TDBMS_CRCChecks TheCrcChecker;

#ifndef __linux__ //No CRC test on LINUX
#ifdef __TOOLS2__
const TPtrC	KCrcRecord=_L("\\epoc32\\winscw\\c\\dbms-tst\\T_DEFECT.CRC");
#else
const TPtrC	KCrcRecord=_L("C:\\dbms-tst\\T_DEFECT.CRC");
#endif
#endif


LOCAL_D RTest 			TheTest (_L ("t_defect.exe"));
LOCAL_D CTrapCleanup*	TheTrapCleanup = NULL;
#ifndef __TOOLS2__
LOCAL_D RSemaphore 		TheWaitToStartSem;
LOCAL_D RSemaphore 		TheWaitForThreadsReadySem;
#endif
LOCAL_D RFs 			TheFs;
LOCAL_D RDbs			TheDbs1, TheDbs2;
LOCAL_D RDbNamedDatabase TheDb1, TheDb2;

_LIT (KName,			"ConnectTestThread_");
_LIT (KStart,			"Starting thread %x.\n");
_LIT (KConnect,			"Thread %x: Waiting to connect...\n");
_LIT (KConSuccess,		"Thread %x: Connection succeeded.\n");
_LIT (KConFailed,		"Thread %x: Connection failed. Error %d.\n");
_LIT (KStatus,			"Status of thread %x is %d.\n");
 
 
_LIT(KTable, 			"TABLE");
_LIT(KColName,			"Fld");
_LIT(KCol2Name, 		"Fld2");

#ifndef __TOOLS2__
const TPtrC KDbName=_L("C:\\dbms-tst\\TESTDB22.DB");
#else
const TPtrC KDbName=_L(".\\dbms-tst\\TESTDB22.DB");
#endif

_LIT(KSQLInsert1, 		"INSERT INTO TABLE (Fld, Fld2) VALUES ('ACDC\\','BLAH')");
_LIT(KSQLInsert2, 		"INSERT INTO TABLE   (Fld) VALUES ('ABCDEFGH')");
_LIT(KSQLInsert3, 		"INSERT INTO TABLE   (Fld) VALUES ('A?CDEFGH')");
_LIT(KSQLInsert4, 		"INSERT INTO TABLE   (Fld) VALUES ('A?*?CDEFGH')");
_LIT(KSQLInsert5, 		"INSERT INTO TABLE   (Fld) VALUES ('A*CDEFGH')");
_LIT(KSQLInsert6, 		"INSERT INTO TABLE   (Fld, Fld2) VALUES ('ADCDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOP','ADCB')");
_LIT(KSQLInsert7, 		"INSERT INTO TABLE   (Fld) VALUES ('XZD\\FZX')");

_LIT(KSqlRequestGranularity, "SELECT Fld FROM test WHERE (Fld LIKE '1' AND Fld LIKE '2') AND Fld LIKE '3'  AND Fld LIKE '4'  AND Fld LIKE '5'  AND Fld LIKE '6'");

_LIT(KText16Name, 		"text16");
_LIT(KTableName, 		"test");
_LIT(KIndexName, 		"test_index");
_LIT(KMaxStringFormat, 	"%0256d");

void TestCleanup()
	{
	TheDb2.Close();
	TheDb1.Close();
#ifndef __TOOLS2__
	TheDbs2.Close();
	TheDbs1.Close();
#endif
	(void)TheFs.Delete(KDbName);
	TheFs.Close();
	TheCrcChecker.GenerateCrcL(KDbName);
	}

//-----------------------------------------------------------------------------
//
// Test macros and functions.
//
//-----------------------------------------------------------------------------
// If (!aValue) then the test will be panicked, the test data files will be 
// deleted.
LOCAL_C void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TestCleanup();
		TheTest(EFalse, aLine);
		}
	}


// If (aValue != aExpected) then the test will be panicked, the test data files
// will be deleted.
LOCAL_C void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TestCleanup();
		TheTest(EFalse, aLine);
		}
	}
		
	
//Use these to test conditions.
#define TEST(arg)                ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check((aValue), (aExpected), __LINE__)
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct TTest
	{
	const TText* result;
	const TText* query;
	};
		
const TTest KQuery[]=
	{
	{_S("ACDC\\"),_S("SELECT Fld FROM TABLE WHERE  Fld LIKE 'ACDC\\' AND Fld2 LIKE '*BL*'")},
	{_S("A*CDEFGH"),_S("SELECT * FROM TABLE WHERE  Fld LIKE '*A\\*C*' ESCAPE '\\'")},
	{_S("A?CDEFGH"),_S("SELECT * FROM TABLE WHERE  Fld LIKE '*A\\?C*' ESCAPE '\\'")},
	{_S("A?*?CDEFGH"),_S("SELECT * FROM TABLE WHERE  Fld LIKE '*A\\?\\*\\?C*' ESCAPE '\\'")},
	{_S("ADCDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOP"),_S("SELECT * FROM TABLE WHERE  Fld LIKE '*1234*'")},	
	{_S("BLAH"),_S("SELECT Fld2 FROM TABLE WHERE  Fld LIKE '*AC*' AND Fld2 LIKE '?LA?'")},
	{_S("BLAH"),_S("SELECT Fld2 FROM TABLE WHERE  Fld LIKE 'NOTINTABLE' OR Fld2 LIKE '?LA?'")},
	{_S("ADCDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOP"),_S("SELECT * FROM TABLE WHERE  Fld LIKE '*ADC*' AND Fld2 LIKE 'ADC?'")},	
	{_S("A*CDEFGH"),_S("SELECT Fld FROM TABLE WHERE Fld LIKE '*\\*C*' ESCAPE '\\'")},
	{_S("XZD\\FZX"),_S("SELECT Fld FROM TABLE WHERE Fld LIKE '*D\\\\*' ESCAPE '\\'")}
	};

const TTest KBadQuery[]=
	{
	{_S(""),_S("SELECT * FROM TABLE WHERE  Fld LIKE 'A?C' ESCAPE '\\'")},
	{_S(""),_S("SELECT * FROM TABLE WHERE  Fld LIKE 'A*C' ESCAPE '\\'")},
	{_S(""),_S("SELECT * FROM TABLE WHERE  Fld LIKE '?A\\?C' ESCAPE '\\'")},
	{_S(""),_S("SELECT * FROM TABLE WHERE  Fld LIKE '?A\\?C?' ESCAPE '\\'")},
	{_S(""),_S("SELECT * FROM TABLE WHERE  Fld LIKE '*A\\??\\?C*' ESCAPE '\\'")},
	{_S(""),_S("SELECT * FROM TABLE WHERE  Fld LIKE '*A*\\*C*' ESCAPE '\\'")},
	{_S(""),_S("SELECT * FROM TABLE WHERE  Fld LIKE 'ABC' ESCAPE '\\'")},
	{_S(""),_S("SELECT * FROM TABLE WHERE  Fld LIKE 'ABC*' ESCAPE '\\'")},
	{_S(""),_S("SELECT * FROM TABLE WHERE  Fld LIKE '*ABC' ESCAPE '\\'")}
	};	

const TInt KNumQueries = 10;
const TInt KNumBadQueries = 9;
const TInt KThreadCount	= 3;
const TInt KOneSecond 	= 1000000;
const TInt KDbNameLen   = 255;
const TInt KMaxColName = 32;

#ifndef __TOOLS2__
static void DoDbmsConnectThreadSubFunctionL (TInt aThreadNumber)
	{
	// The session	
	RDbs TheDbSession;
	CleanupClosePushL (TheDbSession);

	RDebug::Print (KStart (), aThreadNumber);
	RDebug::Print (KConnect (), aThreadNumber);
	
 	// Signal the main thread to continue
 	TheWaitForThreadsReadySem.Signal (1);

	// Wait until we are signalled
	TheWaitToStartSem.Wait ();	

	// Connect to Dbms
	TInt r = TheDbSession.Connect ();

	if (r == KErrNone)
		{
		RDebug::Print (KConSuccess (), aThreadNumber);
		TheDbSession.Close ();
		}
	else
		{
		RDebug::Print (KConFailed (), aThreadNumber, r);
		User::Leave (r);
		}

	CleanupStack::PopAndDestroy (1); // session
	}

static TInt DoDbmsConnectThread (TAny* aThreadNumber)
	{
	__UHEAP_MARK;

	CTrapCleanup* trapCleanup = CTrapCleanup::New ();
	__ASSERT_ALWAYS (trapCleanup!=NULL, User::Invariant ());

	TInt* threadNumber = static_cast <TInt*> (aThreadNumber);
	TRAPD (err, DoDbmsConnectThreadSubFunctionL (*threadNumber));
	
	delete trapCleanup;
	
	__UHEAP_MARKEND;

	return err;		
	}
#endif

#ifndef TOOLS2_VARIANT
/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0644
@SYMTestCaseDesc        Test for defect no 44697
@SYMTestPriority        Medium
@SYMTestActions         Test for defect fixes
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Defect_DEF44697L ()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-DBMS-CT-0644 Defect_DEF44697L "));

	__UHEAP_MARK;
	
  	// find out the number of open handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread ().HandleCount (startProcessHandleCount, startThreadHandleCount);

	/////////////////////
	// The Test Begins...
	
 	// Create semaphores
 	::CleanupClosePushL (::TheWaitForThreadsReadySem);
 	User::LeaveIfError (::TheWaitForThreadsReadySem.CreateLocal (0));

 	::CleanupClosePushL (::TheWaitToStartSem);
 	User::LeaveIfError (::TheWaitToStartSem.CreateLocal (0));

 	// Create the threads.
 	RThread createTestThread_1;
 	RThread createTestThread_2;
 	RThread createTestThread_3;
 	
 	TBuf<100> thread_name;
	TInt KThreadNumber1 = 1;
	TInt KThreadNumber2 = 2;
	TInt KThreadNumber3 = 3;
 	
 	// Create the first test thread______________________________
	thread_name = KName ();
	thread_name.AppendNum (KThreadNumber1);
 		
 	User::LeaveIfError (
 		createTestThread_1.Create (thread_name, 
								   (TThreadFunction) DoDbmsConnectThread,
								   KDefaultStackSize,
								   KMinHeapSize,
								   KMinHeapSize,
								   &KThreadNumber1));
 	
 	// Default priority of Main thread is EPriorityNormal								   
 	createTestThread_1.SetPriority(EPriorityMore);
 	
 	TheTest.Printf (_L ("%S thread started\n"), &thread_name);
	 	
 	// Request notification when the thread dies.
 	TRequestStatus threadStatus_1;
 	createTestThread_1.Logon (threadStatus_1);

	//___________________________________________________________

 	// Create the second test thread______________________________
	thread_name = KName ();
	thread_name.AppendNum (KThreadNumber2);
 		
 	User::LeaveIfError (
 		createTestThread_2.Create (thread_name, 
								   (TThreadFunction) DoDbmsConnectThread,
								   KDefaultStackSize,
								   KMinHeapSize,
								   KMinHeapSize,
								   &KThreadNumber2));
 					
  	// Default priority of Main thread is EPriorityNormal								   
 	createTestThread_2.SetPriority(EPriorityMore);
				   
 	TheTest.Printf (_L ("%S thread started\n"), &thread_name);
	 	
 	// Request notification when the tread dies.
 	TRequestStatus threadStatus_2;
 	createTestThread_2.Logon (threadStatus_2);

	//___________________________________________________________

 	// Create the third test thread______________________________
	thread_name = KName ();
	thread_name.AppendNum (KThreadNumber3);
 		
 	User::LeaveIfError (
 		createTestThread_3.Create (thread_name, 
								   (TThreadFunction) DoDbmsConnectThread,
								   KDefaultStackSize,
								   KMinHeapSize,
								   KMinHeapSize,
								   &KThreadNumber3));
 		
  	// Default priority of Main thread is EPriorityNormal								   
 	createTestThread_3.SetPriority(EPriorityMore);
							   
 	TheTest.Printf (_L ("%S thread started\n"), &thread_name);
	 	
 	// Request notification when the tread dies.
 	TRequestStatus threadStatus_3;
 	createTestThread_3.Logon (threadStatus_3);

	//___________________________________________________________

	TheTest (threadStatus_1.Int () == KRequestPending);
	TheTest (threadStatus_2.Int () == KRequestPending);
	TheTest (threadStatus_3.Int () == KRequestPending);

 	// Make threads eligible for execution
 	createTestThread_1.Resume ();
 	createTestThread_2.Resume ();
 	createTestThread_3.Resume ();

	// The way this works is that the main thread blocks until all
	// the test threads are ready (semaphore 1) and then signals them
	// (semaphore 2).
	//
	// 1: Main thread Waits for ALL test threads to become ready.
	// 2: Main thread Signals ALL test threads to run.
	//
	// 1: Test thread Signals Main thread
	// 2: Test thread Waits for Main thread
	//
	// There is still a slight race condition between the 
	// test thread signalling (semaphore 1) and then waiting 
	// (semaphore 2) which is why we use both higher priority test
	// threads and a timer.
	//
	// The problems come with the way Time slicing works due to
	// other threads of higher priority being run.
	//
	// Higher priority: Ensures the test thread runs before the 
	// the main thread.
	//
	// Timer: Safeguards when multiple core processors are being used.
	//
	// The Higher priority fixes the problem on single core processors
	// and multiple cores processors (SMP) where each core can run a
	// thread.
	// 
	// It should also ensure that if the system is so busy that it 
	// affects the test thread execution, the test thread will still
	// get to the Wait state before the Main thread can Signal.
	// 
	// However, on multiple cores the Main thread may run at the same 
	// time as the test thread, so we need to make sure that when the
	// test thread Signals it can acheive its Wait state before the 
	// Main thread Signals. For example, if the timer has elapsed on the
	// Main thread and the sytem is busy, the test thread should still
	// run before the Main thread due to it higher priority.
	//
	// We also have to think about things like priority inheritance
	// where a thread that has a handle on a Mutex inherits the same 
	// priority as a thread Waiting on it. This shouldn't happen for 
	// Semaphores as there is no one handle, i.e. no critical section.
	//
	// This higher priority inheritance will take affect when a low 
	// priority thread that has a handle on the Mutex blocks because of
	// another medium priority running thread. So in effect a high 
	// priority thread Waiting on this Mutex is also blocked.
	//
	// It is also worth noting that on EKA1 emulator, scheduling is
	// performed by windows. On EKA2 emulator scheduling is performed
	// by Symbian so that it is the same as hardware.
	
	TheWaitForThreadsReadySem.Wait();			
	TheWaitForThreadsReadySem.Wait();			
	TheWaitForThreadsReadySem.Wait();			
	
  	// Sleep for a while to allow threads to block on the semaphore
  	User::After (KOneSecond<<2); // 4 seconds
	
 	// Signal all the threads to continue
 	TheWaitToStartSem.Signal (KThreadCount);
 	
 	// Wait for all threads to complete, don't care on the order.
	User::WaitForRequest (threadStatus_1);
	User::WaitForRequest (threadStatus_2);
	User::WaitForRequest (threadStatus_3);
	
	TheTest.Printf (KStatus, KThreadNumber1, threadStatus_1.Int ());
	TheTest.Printf (KStatus, KThreadNumber2, threadStatus_2.Int ());
	TheTest.Printf (KStatus, KThreadNumber3, threadStatus_3.Int ());
	
	TheTest (threadStatus_1.Int () == KErrNone);
	TheTest (threadStatus_2.Int () == KErrNone);
	TheTest (threadStatus_3.Int () == KErrNone);
	
	CleanupStack::PopAndDestroy (&::TheWaitToStartSem);
	CleanupStack::PopAndDestroy (&::TheWaitForThreadsReadySem);
	
	// The Test Ends...
	/////////////////////

	// check that no handles have leaked
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread ().HandleCount (endProcessHandleCount, endThreadHandleCount);

	TheTest (startThreadHandleCount == endThreadHandleCount);

	__UHEAP_MARKEND;
	}
#endif // !TOOLS2_VARIANT
	
	
	
// Test for LIKE Predicate for EDbColLongText16
LOCAL_C void LikePredicateDbColLongText16TestL()
 	{
 	TheTest.Next (_L ("LikePredicateDbColLongText16TestL"));
 	//Creating database

	RFs fsSession;
    User::LeaveIfError(fsSession.Connect());
    CleanupClosePushL(fsSession);
	RDbNamedDatabase database;
	User::LeaveIfError(database.Replace(fsSession, KDbName));
	CleanupClosePushL(database);
	
	//Create table

	CDbColSet* columns= CDbColSet::NewLC();
			
	TDbCol name(KColName,EDbColLongText16,KDbNameLen);
	name.iAttributes = TDbCol::ENotNull;
	
	TDbCol name2(KCol2Name,EDbColLongText16,KDbNameLen);

	columns->AddL(name);
	columns->AddL(name2);
   	User::LeaveIfError (database.CreateTable (KTable, *columns));
	CleanupStack::PopAndDestroy(); // columns
    
    // Insert values into table
    TInt error =	database.Execute(KSQLInsert1);
    TheTest(error>=0);
    error =database.Execute(KSQLInsert2);
    TheTest(error>=0);
    error =database.Execute(KSQLInsert3);
    TheTest(error>=0);
	error =	database.Execute(KSQLInsert4);
	TheTest(error>=0);
	error =	database.Execute(KSQLInsert5);
	TheTest(error>=0);
	error = database.Execute(KSQLInsert6);
	TheTest(error>=0);
	error = database.Execute(KSQLInsert7);
	TheTest(error>=0);
	
		
	TheTest.Next(_L("Test for valid LIKE predicate queries"));
	
	
	for(TInt i =0;i<KNumQueries;++i)
		{
		RDebug::Print(_L("Executing statement: %s \n"),(KQuery[i].query));
		RDbView view;
		view.Prepare(database, TDbQuery(TPtrC(KQuery[i].query), EDbCompareFolded), view.EReadOnly);
		view.EvaluateAll();
		view.FirstL();      
		typedef TBuf<256> TScriptLine;
		TInt count =0;
		while (view.AtRow())
			{
			view.GetL();
			count++;
			RDbColReadStream rd;
			rd.OpenLC(view,1);
			TScriptLine text;
			rd.ReadL(text,view.ColLength(1));
			CleanupStack::PopAndDestroy();
			RDebug::Print(_L("Expected result: %s Actual Result: %S\n"),(KQuery[i].result),&text);
			TInt err = text.Compare(TPtrC(KQuery[i].result));
			TheTest(err ==0);
			view.NextL();
			}
		view.Close();		
		}
	
	
	// test for illegal statements, check they return KErrArgument
	TheTest.Next(_L("Test that illegal queries return KErrArgument"));	
	
		
	for(TInt j =0;j<KNumBadQueries;++j)
		{
		RDebug::Print(_L("Executing illegal statement: %s \n"),(KBadQuery[j].query));
		RDbView view;
		TInt prepErr = view.Prepare(database, TDbQuery(TPtrC(KBadQuery[j].query), EDbCompareFolded), view.EReadOnly);
		TheTest(prepErr==KErrArgument);
		view.Close();		
		}
        
    CleanupStack::PopAndDestroy(&database); // database
    CleanupStack::PopAndDestroy(&fsSession); // fsSession
 	}
 	
 	
 	
 //  for LIKE Predicate for EDbColLongText8	
LOCAL_C void LikePredicateDbColLongText8TestL()
 	{
	TheTest.Next (_L ("LikePredicate DbColLongText8 TestL"));

 	//Creating database

	RFs      fsSession;
    User::LeaveIfError(fsSession.Connect());
    CleanupClosePushL(fsSession);
	RDbNamedDatabase database;
	User::LeaveIfError(database.Replace(fsSession, KDbName));
	CleanupClosePushL(database);
		
	//Create table

	CDbColSet* columns= CDbColSet::NewLC();
			
	TDbCol name(KColName,EDbColLongText8,KDbNameLen);
	name.iAttributes = TDbCol::ENotNull;
		
	TDbCol name2(KCol2Name,EDbColLongText8,KDbNameLen);

	columns->AddL(name);
	columns->AddL(name2);
   	
	User::LeaveIfError (database.CreateTable (KTable, *columns));
	CleanupStack::PopAndDestroy(); // columns
    
	// Insert values into the table    
 	TInt error =	database.Execute(KSQLInsert1);
    TheTest(error>=0);
    error =database.Execute(KSQLInsert2);
    TheTest(error>=0);
    error =database.Execute(KSQLInsert3);
    TheTest(error>=0);
	error =	database.Execute(KSQLInsert4);
	TheTest(error>=0);
	error =	database.Execute(KSQLInsert5);
	TheTest(error>=0);
	error = database.Execute(KSQLInsert6);
	TheTest(error>=0);
	error = database.Execute(KSQLInsert7);
	TheTest(error>=0);
			
	TheTest.Next(_L("Test for valid LIKE predicate queries"));
	
		
	for(TInt i =0;i<KNumQueries;++i)
		{
		RDebug::Print(_L("Executing statement: %s \n"),(KQuery[i].query));
		RDbView view;
		TInt prepErr = view.Prepare(database, TDbQuery(TPtrC(KQuery[i].query), EDbCompareFolded), view.EReadOnly);
		TheTest(prepErr>=0);
		TInt evErr = view.EvaluateAll();
		TheTest(evErr==0);
		TBuf8<256> result;
		TBuf8<256> colname;
		result.Copy(TPtrC(KQuery[i].result));
		view.FirstL(); 
		
		while (view.AtRow())
			{
			view.GetL();
			RDbColReadStream rd;
			rd.OpenLC(view,1);
			rd.ReadL(colname,view.ColLength(1));
			CleanupStack::PopAndDestroy();
			RDebug::Print(_L("Expected result: %S Actual Result: %S\n"),&result,&colname);
			TInt err = colname.CompareF(result);
			TheTest(err ==0);
				
			view.NextL();
			}
					
		view.Close();		
		}
		
		// test for illegal statements, check they return KErrArgument
	TheTest.Next(_L("Test that illegal queries return KErrArgument"));	
	
	
		
	for(TInt j =0;j<KNumBadQueries;++j)
		{
		RDebug::Print(_L("Executing illegal statement: %s \n"),(KBadQuery[j].query));	
		RDbView view;
		TInt prepErr = view.Prepare(database, TDbQuery(TPtrC(KBadQuery[j].query), EDbCompareFolded), view.EReadOnly);
		TheTest(prepErr==KErrArgument);
	
		view.Close();		
		}
	       
   	CleanupStack::PopAndDestroy(&database); // database
    CleanupStack::PopAndDestroy(&fsSession); // fsSession
 	}



// Test for LIKE Predicate for EDbColText
LOCAL_C void LikePredicateDbColTextTestL()
 	{
 	TheTest.Next (_L ("LikePredicate DbColText TestL"));
 	//Creating database
	
	RFs      fsSession;
    User::LeaveIfError(fsSession.Connect());
    CleanupClosePushL(fsSession);
	RDbNamedDatabase database;
	User::LeaveIfError(database.Replace(fsSession, KDbName));
	CleanupClosePushL(database);
	
	//Create table

	CDbColSet* columns= CDbColSet::NewLC();
			
	TDbCol name(KColName,EDbColText,KDbNameLen);
	name.iAttributes = TDbCol::ENotNull;
	
	TDbCol name2(KCol2Name,EDbColText,KDbNameLen);

	columns->AddL(name);
	columns->AddL(name2);
		
	User::LeaveIfError (database.CreateTable (KTable, *columns));
	CleanupStack::PopAndDestroy(); // columns
   
 	// Insert values into the table
    TInt error =	database.Execute(KSQLInsert1);
    TheTest(error>=0);
    error =database.Execute(KSQLInsert2);
    TheTest(error>=0);
    error =database.Execute(KSQLInsert3);
    TheTest(error>=0);
	error =	database.Execute(KSQLInsert4);
	TheTest(error>=0);
	error =	database.Execute(KSQLInsert5);
	TheTest(error>=0);
	error = database.Execute(KSQLInsert6);
	TheTest(error>=0);
	error = database.Execute(KSQLInsert7);
	TheTest(error>=0);
		

	TheTest.Next(_L("Test for valid LIKE predicate queries"));
	
		
	for(TInt i =0;i<KNumQueries;++i)
		{
		RDebug::Print(_L("Executing statement: %s \n"),(KQuery[i].query));
		RDbView view;
		TInt prepErr = view.Prepare(database, TDbQuery(TPtrC(KQuery[i].query), EDbCompareFolded), view.EReadOnly);
		if(TPtrC(KQuery[i].result).Length() == 0)
			{
			TheTest(prepErr != KErrNone);
			continue;	
			}
				
		view.EvaluateAll();
		TBufC<256> colname;
		TBufC<256> res;
		view.FirstL();      
		while (view.AtRow())
			{
			view.GetL();
			colname = view.ColDes(1);
			res= KQuery[i].result;
			RDebug::Print(_L("Expected result: %s Actual Result: %S\n"),(KQuery[i].result),&colname);
			TInt err =	colname.Compare(TPtrC(KQuery[i].result));	
			TheTest(err ==0);
			view.NextL();
			}
		view.Close();		
		}
	
	// test for illegal statements, check they return KErrArgument
	TheTest.Next(_L("Test that illegal queries return KErrArgument"));	
	
	for(TInt j =0;j<KNumBadQueries;++j)
		{
		RDebug::Print(_L("Executing illegal statement: %s \n"),(KBadQuery[j].query));
		RDbView view;
		TInt prepErr = view.Prepare(database, TDbQuery(TPtrC(KBadQuery[j].query), EDbCompareFolded), view.EReadOnly);
		TheTest(prepErr==KErrArgument);
		view.Close();		
		}
        
    CleanupStack::PopAndDestroy(&database); // database
    CleanupStack::PopAndDestroy(&fsSession); // fsSession
    }
 	
 	
 	
 /**
@SYMTestCaseID 			SYSLIB-DBMS-UT-1592
@SYMTestCaseDesc     	Testing limited-ESCAPE-clause
@SYMTestPriority     	High
@SYMTestActions      	Execute DBMS query with ESCAPE-clause
@SYMTestExpectedResults The test should not fail.
@SYMDEF INC076370 
*/ 	
LOCAL_C void Defect_INC076370 ()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-UT-1592 Defect INC076370 "));
	LikePredicateDbColTextTestL(); //EDbColText
	LikePredicateDbColLongText16TestL(); //EDbColLongText16
	LikePredicateDbColLongText8TestL();	//EDbColLongText8	
	}
	
 /**
@SYMTestCaseID 			SYSLIB-DBMS-UT-1667
@SYMTestCaseDesc     	Testing RdbRowSet::DeleteL() with EDbColLongText16 type columns
@SYMTestPriority     	High
@SYMTestActions      	Create a table with a EDbColLongText16 type column and then use
						RdbRowSet::DeleteL() to delete the current row.
@SYMTestExpectedResults The test should not fail.
@SYMDEF INC083027
*/ 	
LOCAL_C void Defect_INC083027 ()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-UT-1667 Defect INC083027 "));
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());

	// create database
	RDbNamedDatabase database;
	CleanupClosePushL(database);
	User::LeaveIfError(database.Replace(fs, _L("c:\\test.db")));

	CDbColSet* columns = CDbColSet::NewLC();
	const TInt maxTextLength = 256;
	TDbCol text16Col(KText16Name, EDbColLongText16, maxTextLength);
	columns->AddL(text16Col);

	TBuf<KMaxColName> targetColName;
	targetColName = KText16Name;
	
	// create table
	User::LeaveIfError(database.CreateTable(KTableName, *columns));
	
	//create index
	CDbKey* key = CDbKey::NewLC();
	TInt keyLength = 122;
	TDbKeyCol keyCol(targetColName, keyLength);
	key->AddL(keyCol);
	User::LeaveIfError(database.CreateIndex(KIndexName, KTableName, *key));
	CleanupStack::PopAndDestroy(2);		// key and columns
	
	//insert rows
	HBufC* sqlQueryBuf = HBufC::NewLC(512);
	TPtr sqlQuery(sqlQueryBuf->Des());
	_LIT(KSQLInsertFormat, "SELECT %S FROM %S");
	sqlQuery.Format(KSQLInsertFormat, &targetColName, &KTableName);
	
	RDbView insertview;
	User::LeaveIfError(insertview.Prepare(database, TDbQuery(sqlQuery), RDbView::EInsertOnly));
	
	HBufC* tmpBuf = HBufC::NewLC(maxTextLength);
	TPtr maxString(tmpBuf->Des());
	maxString.Format(KMaxStringFormat, 0);
	insertview.InsertL();
	insertview.SetColL(1, maxString);
	insertview.PutL();
	insertview.Close();
	
	//delete the row
	RDbView deleteview;
	User::LeaveIfError(deleteview.Prepare(database, TDbQuery(sqlQuery), RDbView::EUpdatable));
	User::LeaveIfError(deleteview.EvaluateAll());

	while (deleteview.NextL())
		{
		deleteview.GetL();
		TRAPD(err , deleteview.DeleteL());
		TheTest(err==KErrNone);
		}
	deleteview.Close();
	
	CleanupStack::PopAndDestroy(2);  // tmpBuf, sqlQueryBuf
    CleanupStack::PopAndDestroy(&database); // database
    CleanupStack::PopAndDestroy(&fs); // fs
	}

 /**
@SYMTestCaseID          SYSLIB-DBMS-UT-1894
@SYMTestCaseDesc        Testing memory handling in CSqlMultiNode::Concatenate() 
@SYMTestPriority        Medium
@SYMTestActions         Execute a special request to a database which will trigger CSqlMultiNode::Concatenate(), and the size of one of the SQL nodes will be divisible by the CSqlMultiNode granularity
@SYMTestExpectedResults The test should not fail or panic.
@SYMDEF INC093657
*/        
LOCAL_C void Defect_INC093657L ()
   {
   TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-UT-1894 Defect INC093657 "));
   RFs fs;
   CleanupClosePushL(fs);
   User::LeaveIfError(fs.Connect());

   // create database
   RDbNamedDatabase database;
   CleanupClosePushL(database);
   User::LeaveIfError(database.Replace(fs, _L("c:\\test.db")));
   
   CDbColSet* columns = CDbColSet::NewLC();
   const TInt maxTextLength = 256;
   TDbCol column(KColName, EDbColLongText16, maxTextLength);
   columns->AddL(column);
   
   // create table
   User::LeaveIfError(database.CreateTable(KTableName, *columns));
   CleanupStack::PopAndDestroy();  // columns
   
   //execute a pointless request that is intended to detect subtle memory corruptions in CSqlMultiNode::Concatenate
   RDbView view;   
   TInt err = view.Prepare(database, TDbQuery(KSqlRequestGranularity));   
   
   TheTest(err==KErrNone);   
   
   view.Close();
   database.Destroy();
   
   CleanupStack::PopAndDestroy(&database); // database
   CleanupStack::PopAndDestroy(&fs); // fs
   }

/**
@SYMTestCaseID			SYSLIB-DBMS-UT-3467
@SYMTestCaseDesc		Test for DEF105615 "DBMS, CDbColSet::operator[](TDbColNo) operator may access an invalid memory".
						The test creates a table with 3 coluumns and a multi-column key (3 columns). The column
						names length is such that when RDbRowSet::ColSetL() is called for retrieving the column
						names, the CDbColSet array data member will make just a single memory allocation, where
						all TDbCol elements will be stored. Then the test repeats 100 times, the following statements:
							<retrieve a colset>;
							<create a copy of colset's last TDbCol element using TDbCol's copy constructor>;
							<create a copy of colset's last TDbCol element using TDbCol's "=" operator>;
						If the test uses the compiler generated TDbCol's copy constructor and "=" operator,
						the test crashes at some iteration, because an invalid memory region is accessed and
						the crash is: KERN-EXEC 3.
						The same test is repeated for TDbKeyCol's copy constructor and "=" operator.
@SYMTestPriority		High
@SYMTestActions			Test for DEF105615 "DBMS, CDbColSet::operator[](TDbColNo) operator may access an invalid memory".
@SYMTestExpectedResults The test must not fail
@SYMDEF					DEF105615
*/
void DEF105615()
	{
   	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-UT-3467 DEF105615 DBMS, CDbColSet::operator[](TDbColNo) operator may access an invalid memory"));
	RFs fs;
	TInt err = fs.Connect();
	TheTest(err == KErrNone);
	
	RDbNamedDatabase db;
	err = db.Replace(fs, KDbName);
	TheTest(err == KErrNone);

	const TInt KColCnt = 3;

	err = db.Execute(_L("CREATE TABLE A(A1234567890 INTEGER, B1234567890 INTEGER, C12345 INTEGER)"));
	TheTest(err == KErrNone);
	err = db.Execute(_L("CREATE INDEX I1 ON A(A1234567890, B1234567890, C12345)"));
	TheTest(err == KErrNone);
	
	RDbTable tbl;
	err = tbl.Open(db, _L("A"));
	TheTest(err == KErrNone);

	//It is very hard to reproduce the problem, because the memory region after the memory, occupied by 
	//CDbColSet's array, may be valid. That is the reason the test is repeated in a loop KTestCnt times, 
	//where every  ColSetL() call will allocate a new block of memory for its array and at some point TDbCol's 
	//copy constructor and "=" operator may try to access an invalid memory area, if the compiler generated
	//ones are used.
	const TInt KTestCnt = 100;
	TInt i;
	CDbColSet* colset[KTestCnt];
	for(i=0;i<KTestCnt;++i)
		{
		TRAP(err, colset[i] = tbl.ColSetL());
		TheTest(err == KErrNone);
		TDbCol lastCol = (*colset[i])[KColCnt];		//copy constructor
		lastCol = (*colset[i])[KColCnt];			//"=" operator
		}
	for(i=0;i<KTestCnt;++i)
		{
		delete colset[i];
		}
	
	tbl.Close();

	//The same test is repeated for TDbKeyCol's copy constructor and "=" operator
	CDbKey* key[KTestCnt];	
	for(i=0;i<KTestCnt;++i)
		{
		TRAP(err, key[i] = db.KeyL(_L("I1"), _L("A")));
		TheTest(err == KErrNone);
		TDbKeyCol lastKeyCol = (*key[i])[KColCnt - 1];	//copy constructor
		lastKeyCol = (*key[i])[KColCnt - 1];			//"=" operator
		}
	for(i=0;i<KTestCnt;++i)
		{
		delete key[i];
		}

	db.Close();
	err = fs.Delete(KDbName);
	TheTest(err == KErrNone);
	fs.Close();
	}
   
/**
@SYMTestCaseID			SYSLIB-DBMS-UT-3469
@SYMTestCaseDesc		Test for DEF105615 "DBMS, CDbColSet::operator[](TDbColNo) operator may access an invalid memory".
						The test creates TDbCol and TDbKeyCol objects, creates their copies using copy constructors
						and "=" operators and checks that the copies were constructed correctly,
@SYMTestPriority		High
@SYMTestActions			Test for DEF105615 "DBMS, CDbColSet::operator[](TDbColNo) operator may access an invalid memory".
@SYMTestExpectedResults The test must not fail
@SYMDEF					DEF105615
*/
void DEF105615_2()
	{
   	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-UT-3469 DEF105615 DBMS, CDbColSet::operator[](TDbColNo) operator may access an invalid memory - 2 "));
   	
   	const TDbColType KColType = EDbColText16;
   	const TInt KMaxColLen = 73;
   	const TUint KColAttributes = TDbCol::ENotNull;
   	_LIT(KColName, "Name");
   	
   	TDbCol srcDbCol(KColName, EDbColText16, KMaxColLen);
   	srcDbCol.iAttributes = KColAttributes;

	//TDbCol - copy constructor   	
   	TDbCol dbColCopy1(srcDbCol);
   	TheTest(dbColCopy1.iType == srcDbCol.iType && dbColCopy1.iType == KColType);
   	TheTest(dbColCopy1.iMaxLength == srcDbCol.iMaxLength && dbColCopy1.iMaxLength == KMaxColLen);
   	TheTest(dbColCopy1.iAttributes == srcDbCol.iAttributes && dbColCopy1.iAttributes == KColAttributes);
   	TheTest(dbColCopy1.iName == srcDbCol.iName && dbColCopy1.iName == KColName);

	//TDbCol - "=" operator
   	TDbCol dbColCopy2;
   	dbColCopy2 = srcDbCol;
   	TheTest(dbColCopy2.iType == srcDbCol.iType && dbColCopy2.iType == KColType);
   	TheTest(dbColCopy2.iMaxLength == srcDbCol.iMaxLength && dbColCopy2.iMaxLength == KMaxColLen);
   	TheTest(dbColCopy2.iAttributes == srcDbCol.iAttributes && dbColCopy2.iAttributes == KColAttributes);
   	TheTest(dbColCopy2.iName == srcDbCol.iName && dbColCopy2.iName == KColName);

	//TDbCol - self assignment
	srcDbCol = srcDbCol;
   	TheTest(srcDbCol.iType == KColType);
   	TheTest(srcDbCol.iMaxLength == KMaxColLen);
   	TheTest(srcDbCol.iAttributes == KColAttributes);
   	TheTest(srcDbCol.iName == KColName);

   	const TInt KKeyLen = 29;
   	const TDbKeyCol::TOrder KKeyOrder = TDbKeyCol::EDesc;
   	_LIT(KKeyName, "Name22");

   	TDbKeyCol srcDbKeyCol(KKeyName, KKeyLen, KKeyOrder);
   	
	//TDbKeyCol - copy constructor   	
   	TDbKeyCol dbKeyColCopy1(srcDbKeyCol);
   	TheTest(dbKeyColCopy1.iOrder == srcDbKeyCol.iOrder && dbKeyColCopy1.iOrder == KKeyOrder);
   	TheTest(dbKeyColCopy1.iLength == srcDbKeyCol.iLength && dbKeyColCopy1.iLength == KKeyLen);
   	TheTest(dbKeyColCopy1.iName == srcDbKeyCol.iName && dbKeyColCopy1.iName == KKeyName);
   	
	//TDbKeyCol - "=" operator
   	TDbKeyCol dbKeyColCopy2;
   	dbKeyColCopy2 = srcDbKeyCol;
   	TheTest(dbKeyColCopy2.iOrder == srcDbKeyCol.iOrder && dbKeyColCopy2.iOrder == KKeyOrder);
   	TheTest(dbKeyColCopy2.iLength == srcDbKeyCol.iLength && dbKeyColCopy2.iLength == KKeyLen);
   	TheTest(dbKeyColCopy2.iName == srcDbKeyCol.iName && dbKeyColCopy2.iName == KKeyName);

	//TDbKeyCol - self assignment
	srcDbKeyCol = srcDbKeyCol;
   	TheTest(srcDbKeyCol.iOrder == KKeyOrder);
   	TheTest(srcDbKeyCol.iLength == KKeyLen);
   	TheTest(srcDbKeyCol.iName == KKeyName);
   	}
   
/**
@SYMTestCaseID          SYSLIB-DBMS-UT-3413
@SYMTestCaseDesc        Testing that "incremental update" operations running in one connection does not 
						interfere with database operations executed from a second connection
@SYMTestPriority        High
@SYMTestActions         Create a test database with one table and insert some records there (> 100).
						Create 2 database connections.
						Open that database from connection 1 and execute an incremental update operation
						in a transaction. At the same time try to open and close the same table from 
						connection 2, mixing these operations with the RDbUpdate::Next() calls from 
						connection 1. So the call pattern should be:
						@code
						RDbUpdate dbUpdate;
						....
						while((err = dbUpdate.Next()) > 0)		//from "Conenction 1"
							{
							RDbTable tbl;
							err = tbl.Open(TheDb2, _L("A"));	//from "Conenction 2"
							...
							}
						@endcode						
@SYMTestExpectedResults The test should not fail or panic.
@SYMDEF INC101720
*/
void Defect_INC101720()
	{
    TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-UT-3413 "));
	//Create the test database
	TInt err = TheDb1.Replace(TheFs, KDbName);
	TEST2(err, KErrNone);
	TheDb1.Close();
	TheCrcChecker.GenerateCrcL(KDbName);

	//Establish the first database connection
#ifndef __TOOLS2__
	err = TheDbs1.Connect();
	TEST2(err, KErrNone);
#endif
	err = TheDb1.Open(TheDbs1, KDbName);
	TEST2(err, KErrNone);
	//Create a test table and fill the table with enough test records (> 100)
	err = TheDb1.Execute(_L("CREATE TABLE A(Id COUNTER, Id2 INTEGER, Name LONG VARCHAR)"));	
	TEST2(err, KErrNone);
	const TInt KTestRecCount = 200;
	err = TheDb1.Begin();	
	TEST2(err, KErrNone);
	for(TInt i=0;i<KTestRecCount;++i)
		{
		_LIT(KSqlFmtStr, "INSERT INTO A(Id2, Name) VALUES(%d, 'TestNameString')");
		TBuf<100> sql;
		// We can't use this because Math::Random() may return different numbers on 
		// different platform, which will lead to different output that result in 
		// CRC test failing.
		// TUint32 id = Math::Random() % KTestRecCount;
		TUint32 id = (i^0x55555555) % KTestRecCount;

		sql.Format(KSqlFmtStr, id + 1);
		err = TheDb1.Execute(sql);
		TEST2(err, 1);
		}
	err = TheDb1.Commit();	
	TEST2(err, KErrNone);
	//Establish a second connection with the same test database
#ifndef __TOOLS2__
	err = TheDbs2.Connect();
	TEST2(err, KErrNone);
#endif
	err = TheDb2.Open(TheDbs2, KDbName);
	TEST2(err, KErrNone);
	//The test: Conenction 1 - "incremental update" operation.
	//			Connection 2 - "open table/close table" operations mixed with the incremental Next-s.
	//Expectation: The test must not fail.
	err = TheDb1.Begin();
	TEST2(err, KErrNone);
	RDbUpdate dbUpdate;
	err = dbUpdate.Execute(TheDb1, _L("UPDATE A SET Name = 'ModifiedNameString' WHERE Id2 > 10"));
	TEST2(err, KErrNone);
	TInt step = 0;
	while((err = dbUpdate.Next()) > 0)
		{
		++step;
		RDbTable tbl;
		err = tbl.Open(TheDb2, _L("A"));
		TEST2(err, KErrNone);
		tbl.Close();
		}
	TEST(step > 1);//just to be sure that the test executes dbUpdate.Next() more than once
	TEST2(err, KErrNone);
	dbUpdate.Close();
	err = TheDb1.Commit();
	TEST2(err, KErrNone);
	//Cleanup
	TheDb2.Close();
#ifndef __TOOLS2__
	TheDbs2.Close();
#endif
	TheDb1.Close();
#ifndef __TOOLS2__
	TheDbs1.Close();
#endif
	TheCrcChecker.GenerateCrcL(KDbName);
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-UT-3484
@SYMTestCaseDesc        DBMS Hindi collation doesn't work on long text fields.  
@SYMTestPriority        Medium
@SYMTestActions         This test is to check that DBMS correctly sorts columns using Collation, when 
						the columns are of type EDbColLongText16.  Previous implementations split the 
						strings to be compared into chunks, however this could cause it to be sorted 
						incorrectly if it was split on a combining or accent character.  This fault
						occurs on the default locale as well as Hindi.  Test steps:
						* Create a database table and adds several unicode strings to EDbColLongText16 
						  column in table.  One set of strings have an ascii character followed by 
						  an accent (e + ') and the other set have the combined equivilant ascii 
						  character (è).  These should have the same sort order,however if are split 
						  then will compare differently.
						* Sort the columns using EDbCompareCollated
						* Check that the columns were sorted in the correct order
@SYMTestExpectedResults The columns should get sorted into ascending order correctly
@SYMDEF INC107268
*/
void Defect_INC107268L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-UT-3484 Defect INC107268 - DBMS Hindi collation doesn't work on long text fields"));
     
     // some unicode characters
     const TChar Ka(0x0061); 		// a    
     const TChar Kb(0x0062); 		// b
     const TChar Ke(0x0065);			// e
     const TChar Kgrave(0x0060); 	// ' (grave)
     const TChar Kegrave(0x00e8);  	// e with grave
     const TChar K1(0x0031);			// 1
     const TChar K2(0x0032);			// 2
 
  	// the maximum characters in a EDbColLongText16 string before dbms stops storing
  	// the string inline, and we need to read it from a stream (see TBlobKey).
  	const TInt KInlineLimit = 127;
  	
  	// maximum number of characters buffered in TBlobKey when string stored out of line.
  	// (see TBlobKey::ETruncSize which is in bytes)
  	const TInt KTruncLimit = 16; 
 
  	const TInt KMaxStringSize = 256;
  	
  	TBuf<KMaxStringSize> inLineBoundryA;
  	TBuf<KMaxStringSize> inLineBoundryB;
  	TBuf<KMaxStringSize> truncBoundryA;
  	TBuf<KMaxStringSize> truncBoundryB;
  	TBuf<KMaxStringSize> padding;
     
     // this string will be stored inline.  It should sort to be < stringB
     inLineBoundryA.Fill(Ka, KInlineLimit-2);
     inLineBoundryA.Append(Kegrave);
     inLineBoundryA.Append(K1);
     
     // this string is just over the break point, so *is* truncated. 
     // this is expected to get sorted incorrecly as combining character is split off (negative test case)
     inLineBoundryB.Fill(Ka, KInlineLimit-2);
     inLineBoundryB.Append(Ke);
     inLineBoundryB.Append(Kgrave);
     inLineBoundryB.Append(K2);
         
     padding.Fill(Kb, KInlineLimit);
     
     // this string is longger that KInlineLimit so is stored out of line
     truncBoundryA.Fill(Kb, KTruncLimit-2);
     truncBoundryA.Append(Kegrave);
     truncBoundryA.Append(K1);
     truncBoundryA.Append(padding);
     
     // this string has combining characters that fall on boundry of ETruncSize value (32 bytes)
     truncBoundryB.Fill(Kb, KTruncLimit-2);
     truncBoundryB.Append(Ke);
     truncBoundryB.Append(Kgrave);
     truncBoundryB.Append(K2);
     truncBoundryB.Append(padding);
     
     
     // e and '(grave) characters seperately
     TBuf<3> e_grave; 
     e_grave.Append( Ke );
     e_grave.Append( Kgrave );    
     
     // e with grave character - this should sort the same as e_grave
     TBuf<3> egrave; 
     egrave.Append( Kegrave );
     
     TBuf<1> nullString;
     
 	e_grave.Append(K2); // make e_grave sort second
     egrave.Append(K1);  // make egrave sort first
     
     // Check with database
     _LIT(KPosLmLandmarkTable, "lmt_landmark");
     _LIT(KPosLmLandmarkIdCol, "lmc_lmid");
     _LIT(KPosLmNameCol, "lmc_name");
         
     TInt err = TheDb1.Replace( TheFs, KDbName );
     TEST2 (err, KErrNone);
 	CleanupClosePushL(TheDb1);
         
     CDbColSet* columns = CDbColSet::NewLC();
     TDbCol idField( KPosLmLandmarkIdCol, EDbColUint32 );
     idField.iAttributes |= TDbCol::EAutoIncrement;
     columns->AddL( idField );
     columns->AddL( TDbCol( KPosLmNameCol, EDbColLongText16 ) ); // Works with EDbColText16.  Defect only for EDbColLongText16.
     
     err = TheDb1.CreateTable( KPosLmLandmarkTable, *columns );
     TEST2 (err, KErrNone);   
     CleanupStack::PopAndDestroy(columns);
     
     RDbTable table;
     err = table.Open( TheDb1, KPosLmLandmarkTable );
     TEST2 (err, KErrNone);
     CleanupClosePushL(table);
 	
 	// add rows to table
   	table.InsertL();
     table.SetColL( 2, egrave); // row 0 - sorted 8th
     table.PutL();
 
 	table.InsertL();
     table.SetColL( 2, e_grave ); // row 1 - sorted 9th
     table.PutL();
    
     table.InsertL();
     table.SetColL( 2, inLineBoundryA ); // row 2 - sorted 3rd (incorrectly - negative test case)
     table.PutL();
     
     table.InsertL();
     table.SetColL( 2, inLineBoundryB ); // row 3 - sorted 2nd (incorrectly - negative test case)
     table.PutL();
     
     table.InsertL();
     table.SetColL( 2, nullString ); // row 4 - sorted 1st
     table.PutL();
   
   	table.InsertL();
     table.SetColL( 2, truncBoundryB ); // row 5 - sorted 5th
     table.PutL();
     
     table.InsertL();
     table.SetColL( 2, truncBoundryA ); // row 6 - sorted 4th
     table.PutL();
     
 
     CleanupStack::PopAndDestroy(); // table.close()
 
 	// do an sql select with Order By to sort columns
     _LIT(KPosLmSqlSelectOrderByString, "SELECT %S, %S FROM %S ORDER BY %S");
     TBuf<200> sql;
     sql.Format( KPosLmSqlSelectOrderByString, 
                 &KPosLmLandmarkIdCol,
                 &KPosLmNameCol,
                 &KPosLmLandmarkTable,
                 &KPosLmNameCol);
     
     RDbView view;
     CleanupClosePushL(view);
     err = view.Prepare( TheDb1, TDbQuery( sql, EDbCompareCollated ) );
     TEST2 (err, KErrNone);
     err = view.EvaluateAll();
     TEST2 (err, KErrNone);
         
     // Now check that view is ordered correctly  
     const TUint32 ExpectedOrder[] = {4,3,2,6,5,0,1};
     TInt x = 0;
     while (view.NextL())
         {
         view.GetL();
         TEST2(view.ColUint32(1), ExpectedOrder[x]); // check we got the expected order
         x++;          
         }
     TEST2(x, 7); // check we got the right number of values
     CleanupStack::PopAndDestroy(2); // TheDb1.Close(); view.Close()
	TheCrcChecker.GenerateCrcL(KDbName);

 	}
 
   
LOCAL_C void DoTestsL ()
	{
	__UHEAP_MARK;
	CleanupClosePushL(TheFs);
	Defect_INC076370();
	Defect_INC083027();

//	Defect_DEF44697L ();  // Use of three RThreads, not possible for tools2.

	Defect_INC093657L(); 
	Defect_INC101720(); 
// These two tests don't work on 9.3.
	DEF105615();
	DEF105615_2();

	Defect_INC107268L();
	CleanupStack::PopAndDestroy(); // TheFs.Close()
	__UHEAP_MARKEND;
	}

GLDEF_C TInt E32Main ()
	{
	__UHEAP_MARK;
	TheTest.Title ();
	TheTest.Start (_L ("Verify Defect Fixes"));
	
	TheTrapCleanup = CTrapCleanup::New ();
	__ASSERT_ALWAYS (TheTrapCleanup!=NULL, User::Invariant ());

	TInt err = TheFs.Connect();
	TheTest(err == KErrNone);
	
	TRAP (err,DoTestsL ());
	TheTest (err==KErrNone);

	//Wait some time, because DBMS server won't be destroyed right after the last DBMS session
	//being clossed.
	TheTest.Printf(_L("Wait DBMS server shutdown...\n"));
	User::After(7000000);

#ifndef __linux__
#ifndef __TOOLS2__
	TRAPD(lc, err = TheCrcChecker.DumpCrcRecordsL(KCrcRecord));
	TheTest(err==KErrNone);
	TheTest(lc==KErrNone);
#else
	TRAPD(lc, err = TheCrcChecker.ValidateCrcRecordsL(KCrcRecord));
	TPtrC errmsg;
	TheCrcChecker.ErrorReportL(err, errmsg);
	RDebug::Print(errmsg);
	TheTest(err==KErrNone || err==TDBMS_CRCChecks::ECrcCheckOk);
#endif
#endif

	delete TheTrapCleanup;

	TheTest.End ();
	TheTest.Close ();

	__UHEAP_MARKEND;
	return (0);
	}
