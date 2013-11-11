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
#include <bautils.h>
#include <sqldb.h>
#include <e32math.h>
#include "SqlBur.h"

//CSqlSrvTestBurInterface - test implementation of the MSqlSrvBurInterface, implemented in the production code by the SQL server.
class CSqlSrvTestBurInterface : public CBase, public MSqlSrvBurInterface
	{
	public:
		static CSqlSrvTestBurInterface* New();
		virtual ~CSqlSrvTestBurInterface();
		virtual RFs& Fs();
		virtual void GetBackUpListL(TSecureId aUid, TDriveNumber aDrive, RArray<HBufC*>& aFileList);
		
	private:
		void Construct();
		
	private:
		RFs iFs;
		
	};

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_sqlbur2"));

#ifdef _DEBUG
static const TInt KBurstRate = 100;
#endif

TDriveNumber KTestDrive = EDriveC;
_LIT(KTestDir, "c:\\test\\");
CActiveScheduler* TheScheduler = NULL;
CSqlSrvTestBurInterface* TheSqlSrvTestBurInterface = NULL;
TInt TestModeSqlBurError = KErrNone;//The CSqlBurEventMonitor code will set the error here

///////////////////////////////////////////////////////////////////////////////////////

void DestroyTestEnv()
	{
	delete TheSqlSrvTestBurInterface;
	TheSqlSrvTestBurInterface = NULL;		
	
	delete TheScheduler;
	TheScheduler = NULL;
	
	(void)RProperty::Delete(KSqlBurPropertyCategoryUid, KSqlBurBackupRestoreKey);  
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DestroyTestEnv();
		RDebug::Print(_L("*** Boolean expression evaluated to false.\r\n"));
		TheTest(EFalse, aLine);
		}
	}
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DestroyTestEnv();
		RDebug::Print(_L("*** Expected error: %d, got: %d.\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

CSqlSrvTestBurInterface* CSqlSrvTestBurInterface::New()
	{
	CSqlSrvTestBurInterface* self = new CSqlSrvTestBurInterface;
	TEST(self != NULL);
	self->Construct();
	return self;
	}

void CSqlSrvTestBurInterface::Construct()
	{
	TInt err = iFs.Connect();
	TEST2(err, KErrNone);

	err = iFs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	err = iFs.CreatePrivatePath(KTestDrive);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

CSqlSrvTestBurInterface::~CSqlSrvTestBurInterface()
	{
	iFs.Close();
	}

RFs& CSqlSrvTestBurInterface::Fs()
	{
	return iFs;
	}

//No-op. Not needed in this test app.
void CSqlSrvTestBurInterface::GetBackUpListL(TSecureId, TDriveNumber, RArray<HBufC*>&)
	{
	TEST(EFalse);
	}

///////////////////////////////////////////////////////////////////////////////////////

void CreateTestEnv()
    {
	TheScheduler = new CActiveScheduler;
	TEST(TheScheduler != NULL);
	CActiveScheduler::Install(TheScheduler);
	
	TheSqlSrvTestBurInterface = CSqlSrvTestBurInterface::New();
	TEST(TheSqlSrvTestBurInterface != NULL);
	
	TInt err = RProperty::Define(KSqlBurPropertyCategoryUid, KSqlBurBackupRestoreKey, 0);  
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			PDS-SQL-UT-4233
@SYMTestCaseDesc		CSqlBurEventMonitor object creation - OOM test
						The test runs CSqlBurEventMonitor::NewL() in an OOM loop.
@SYMTestActions			CSqlBurEventMonitor object creation - OOM test
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
*/
void SqlBurEventMonitorOomTest()
	{
	TInt err = KErrNoMemory;
	TInt failingAllocationNo = 0;
	TheTest.Printf(_L("Iteration:\r\n"));
	while(err == KErrNoMemory)
		{
		TheTest.Printf(_L(" %d"), ++failingAllocationNo);
		
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);
		__UHEAP_MARK;
		__UHEAP_SETBURSTFAIL(RAllocator::EBurstFailNext, failingAllocationNo, KBurstRate);

		CSqlBurEventMonitor* monitor = NULL;
		TRAP(err, monitor = CSqlBurEventMonitor::NewL(*TheSqlSrvTestBurInterface));
		delete monitor;
		
		__UHEAP_RESET;
		__UHEAP_MARKEND;
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);
		
		TEST2(startProcessHandleCount, endProcessHandleCount);
		TEST2(startThreadHandleCount, endThreadHandleCount);
		}
	TEST2(err, KErrNone);
	TheTest.Printf(_L("\r\n=== OOM Test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4234
@SYMTestCaseDesc		CSqlBurEventMonitor functional test
						The test sets the backup & restore property status and then checks
						how the backup & restore property monitor (CSqlBurEventMonitor) reacts to the event.
@SYMTestActions			CSqlBurEventMonitor functional test
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
*/
void SqlBurEventMonitorFunctionalTest()
	{
	CSqlBurEventMonitor* monitor = NULL;
	TRAPD(err, monitor = CSqlBurEventMonitor::NewL(*TheSqlSrvTestBurInterface));
	TEST2(err, KErrNone);
	TEST(!monitor->ActiveBackupClient());
	TEST(!monitor->SqlBurCallback());
	//Set the property to conn::EBURBackupFull, conn::EBURBackupPartial, conn::EBURRestoreFull, conn::EBURRestorePartial, 
	//then start the scheduler. CSqlBurEventMonitor::RunL() gets called and 
	//CSqlBurCallback and CActiveBackupClient interfaces get created (if the interfaces do exist, they are destroyed first).
	TInt burPropertyStatus[] = {conn::EBURBackupFull, conn::EBURBackupPartial, conn::EBURRestoreFull, conn::EBURRestorePartial};
	for(TInt i=0;i<(sizeof(burPropertyStatus)/sizeof(burPropertyStatus[0]));++i)
		{
		err = RProperty::Set(KSqlBurPropertyCategoryUid, KSqlBurBackupRestoreKey, burPropertyStatus[i]);
		TEST2(err, KErrNone);
		TestModeSqlBurError = KErrNone;
		if(i > 0)
			{
			__UHEAP_MARK;
			}
		CActiveScheduler::Start();
		if(i > 0)
			{
			__UHEAP_MARKEND;
			}
		TEST2(TestModeSqlBurError, KErrNone);
		TEST(monitor->ActiveBackupClient() != NULL);
		TEST(monitor->SqlBurCallback() != NULL);
		}
	//Set the property to conn::EBURUnset, start the scheduler. CSqlBurEventMonitor::RunL() gets called 
	//and CSqlBurCallback interface gets destroyed.
	err = RProperty::Set(KSqlBurPropertyCategoryUid, KSqlBurBackupRestoreKey, conn::EBURUnset);
	TEST2(err, KErrNone);
	TestModeSqlBurError = KErrNone;
	CActiveScheduler::Start();
	TEST2(TestModeSqlBurError, KErrNone);
	TEST(!monitor->ActiveBackupClient());
	TEST(!monitor->SqlBurCallback());
	//Set the property to conn::EBURNormal, start the scheduler. CSqlBurEventMonitor::RunL() gets called. 
	//CSqlBurCallback interface has been destroyed alread. No memory deallocations should be made during this call.
	err = RProperty::Set(KSqlBurPropertyCategoryUid, KSqlBurBackupRestoreKey, conn::EBURNormal);
	TEST2(err, KErrNone);
	__UHEAP_MARK;
	TestModeSqlBurError = KErrNone;
	CActiveScheduler::Start();
	TEST2(TestModeSqlBurError, KErrNone);
	TEST(!monitor->ActiveBackupClient());
	TEST(!monitor->SqlBurCallback());
	__UHEAP_MARKEND;
	//Set the property, then delete it. CSqlBurEventMonitor::RunL() should get called, but the call should
	//fail because the property does not exist.
	err = RProperty::Set(KSqlBurPropertyCategoryUid, KSqlBurBackupRestoreKey, conn::EBURBackupFull);
	TEST2(err, KErrNone);
	err = RProperty::Delete(KSqlBurPropertyCategoryUid, KSqlBurBackupRestoreKey);  
	TEST2(err, KErrNone);
	__UHEAP_MARK;
	TestModeSqlBurError = KErrNone;
	CActiveScheduler::Start();
	TEST2(TestModeSqlBurError, KErrNotFound);
	TEST(!monitor->ActiveBackupClient());
	TEST(!monitor->SqlBurCallback());
	__UHEAP_MARKEND;
	//Restore the property
	err = RProperty::Define(KSqlBurPropertyCategoryUid, KSqlBurBackupRestoreKey, 0);  
	TEST2(err, KErrNone);
	//
	delete monitor;
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4235
@SYMTestCaseDesc		CSqlBurEventMonitor::RunL() - OOM test
						The test sets the backup & restore property status and then checks
						how the backup & restore property monitor (CSqlBurEventMonitor) reacts to the event.
						The test is performed in an OOM loop.
@SYMTestActions			CSqlBurEventMonitor::RunL() - OOM test
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
*/
void SqlBurEventMonitorRunOomTest()
	{
	CSqlBurEventMonitor* monitor = NULL;
	TRAPD(err, monitor = CSqlBurEventMonitor::NewL(*TheSqlSrvTestBurInterface));
	
	err = KErrNoMemory;
	TInt failingAllocationNo = 0;
	TheTest.Printf(_L("Iteration:\r\n"));
	while(err == KErrNoMemory)
		{
		TheTest.Printf(_L(" %d"), ++failingAllocationNo);
		
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);
		__UHEAP_MARK;
		__UHEAP_SETBURSTFAIL(RAllocator::EBurstFailNext, failingAllocationNo, KBurstRate);

		TEST(!monitor->ActiveBackupClient());
		TEST(!monitor->SqlBurCallback());
		//Set the property, start the scheduler. CSqlBurEventMonitor::RunL() gets called and CSqlBurCallback
		//interface gets created.
		err = RProperty::Set(KSqlBurPropertyCategoryUid, KSqlBurBackupRestoreKey, conn::EBURBackupFull);
		if(err == KErrNone)
			{
			TestModeSqlBurError = KErrNone;
			CActiveScheduler::Start();
			err = TestModeSqlBurError;
			if(err == KErrNone)
				{
				TEST(monitor->ActiveBackupClient() != NULL);
				TEST(monitor->SqlBurCallback() != NULL);
				//Destroy the SQL backup & restore callback
				err = RProperty::Set(KSqlBurPropertyCategoryUid, KSqlBurBackupRestoreKey, conn::EBURNormal);
				TestModeSqlBurError = KErrNone;
				CActiveScheduler::Start();
				err = TestModeSqlBurError;
				if(err == KErrNone)
					{
					TEST(!monitor->ActiveBackupClient());
					TEST(!monitor->SqlBurCallback());
					}
				}
			}
		
		__UHEAP_RESET;
		__UHEAP_MARKEND;
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);
		
		TEST2(startProcessHandleCount, endProcessHandleCount);
		TEST2(startThreadHandleCount, endThreadHandleCount);
		}
	TEST2(err, KErrNone);
	TheTest.Printf(_L("\r\n=== OOM Test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
	delete monitor;
	}

void DoTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:PDS-SQL-UT-4233 CSqlBurEventMonitor object creation - OOM test"));
	SqlBurEventMonitorOomTest();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-UT-4234 CSqlBurEventMonitor functional test"));
	SqlBurEventMonitorFunctionalTest();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-UT-4235 CSqlBurEventMonitor::RunL() - OOM test"));
	SqlBurEventMonitorRunOomTest();
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
