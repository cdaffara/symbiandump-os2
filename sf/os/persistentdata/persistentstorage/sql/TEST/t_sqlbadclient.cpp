// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32math.h>
#include <sqldb.h>
#include "SqlUtil.h"
#include "SqlSrvStartup.h" //KSqlMajorVer, KSqlMinorVer, KSqlBuildVer

///////////////////////////////////////////////////////////////////////////////////////

static RFs TheFs;
RTest TheTest(_L("t_sqlbadclient test"));
_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDbName1, "c:\\test\\t_sqlbadclient.db");
_LIT(KTestDbName2, "c:[1111D1C1]t_sqlbadclient.db");

#if defined __WINS__ || defined __WINSCW__
const TInt KTestIterCount = 5000;
#else
const TInt KTestIterCount = 4000;
#endif
const TInt KMaxDesArgLen = 1000;
enum TArgType 
	{
	EIntArgType, 
	ETextArgType, 
	EBinArgType, 
	ELastArgType
	};

//////////////////////////////////////////////////////////////////////////////////////

//If the SQL server crashes and the test receives KErrServerTerminated error, then the 
//next set will contain the last:
// - iteration number;
// - handle type;
// - function code;
// - handle;
// - IPC arguments values;
struct TThreadData
	{
	TInt				iIteration;
	TSqlSrvHandleType 	iHandleType;
	TInt 				iFunction;
	TInt 				iHandle;
	TArgType 			iArgType[KMaxMessageArguments];
	TInt 				iIntArg[KMaxMessageArguments];
	TBuf<KMaxDesArgLen> iTextArg[KMaxMessageArguments];
	TBuf8<KMaxDesArgLen> iBinArg[KMaxMessageArguments];
	TInt64 				iSeed;
	};
//////////////////////////////////////////////////////////////////////////////////////

_LIT(KPanicCategory, "SrvTerm");
_LIT(KPanicCategory2, "InvArg");
const TInt KPanicCode = 1111;
const TInt KPanicCode2 = 2222;

///////////////////////////////////////////////////////////////////////////////////////

//Deletes all created test files.
void DeleteTestFiles()
	{
	RSqlDatabase::Delete(KTestDbName1);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine, TBool aPrintThreadName = EFalse)
	{
	if(!aValue)
		{
		DeleteTestFiles();
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
		DeleteTestFiles();
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

///////////////////////////////////////////////////////////////////////////////////////

//Creates file session instance and the test directory
void CreateTestEnv()
    {
	TInt err = TheFs.Connect();
	TEST2(err, KErrNone);

	err = TheFs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

//Starts the SQL server process.
TInt StartSqlServer()
	{
	const TUid KSqlSrvUid3 = {0x10281E17};//The same UID is in SqlSrv.mmp file
	const TUidType serverUid(KNullUid, KNullUid, KSqlSrvUid3);
	_LIT(KSqlSrvImg, "SqlSrv");//SQL server image name
	RProcess server;
	TInt err = server.Create(KSqlSrvImg, KNullDesC, serverUid);
	if(err != KErrNone)
		{
		return err;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if(stat != KRequestPending)
		{
		server.Kill(0);		// abort startup
		}
	else
		{
		server.Resume();	// logon OK - start the server
		}
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	err = (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return err;
	}

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////        RTestSqlDbSession          //////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//Test client SQL session class
class RTestSqlDbSession : public RSessionBase
	{
public:	
	TInt Connect(const TVersion& aVersion);
	void Close();
	TInt SendReceive(TInt aFunction);
	TInt SendReceive(TInt aFunction, const TIpcArgs& aArgs);

private:
	TInt DoCreateSession(const TVersion& aVersion);
	};
	
TInt RTestSqlDbSession::Connect(const TVersion& aVersion)
	{
	TInt err = DoCreateSession(aVersion);
	if(err == KErrNone)
		{
		TIpcArgs ipcArgs(KTestDbName1().Length(), &KTestDbName1(), 0, 0);
		err = SendReceive(ESqlSrvDbOpen, ipcArgs);
		}
	if(err != KErrNone && err != KErrAlreadyExists)
		{
		Close();	
		}
	return err;
	}

void RTestSqlDbSession::Close()
	{
	if(Handle())
		{
		(void)SendReceive(ESqlSrvDbClose);
		}
	RSessionBase::Close();
	}

TInt RTestSqlDbSession::SendReceive(TInt aFunction)
	{
	return RSessionBase::SendReceive(aFunction);	
	}
	
TInt RTestSqlDbSession::SendReceive(TInt aFunction, const TIpcArgs& aArgs)
	{
	return RSessionBase::SendReceive(aFunction, aArgs);	
	}

TInt RTestSqlDbSession::DoCreateSession(const TVersion& aVersion)
	{
	const TInt KTimesToRetryConnection = 2;
	TInt retry = KTimesToRetryConnection;
	_LIT(KSqlSrvName, "!SQL Server");//SqlDb server name
	for(;;)
		{
		TInt err = CreateSession(KSqlSrvName, aVersion);
		if(err != KErrNotFound && err != KErrServerTerminated)
			{
			return err;
			}
		if(--retry == 0)
			{
			return err;
			}
		err = ::StartSqlServer();
		if(err != KErrNone && err != KErrAlreadyExists)
			{
			return err;
			}
		}
	}
	
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

TInt SendReceive(RTestSqlDbSession aSession, TSqlSrvHandleType aHandleType, TInt aFunction, TInt aHandle, TIpcArgs& aArgs)
	{
	return aSession.SendReceive(::MakeMsgCode(static_cast <TSqlSrvFunction> (aFunction), aHandleType, aHandle), aArgs);
	}

TInt SendReceive(RTestSqlDbSession aSession, TInt aFunction, TIpcArgs& aArgs)
	{
	return aSession.SendReceive(aFunction, aArgs);
	}

void PrintIterationCount(TInt aIteration)
	{
	if((aIteration % 100) == 0)
		{
		TTime time;
		time.HomeTime();
		TDateTime dt = time.DateTime();
		TBuf<16> tbuf;
		tbuf.Format(_L("%02d:%02d:%02d.%06d"), dt.Hour(), dt.Minute(), dt.Second(), dt.MicroSecond());
		RDebug::Print(_L("-----[%S] Test iterations: %d\r\n"), &tbuf, aIteration);
		}
	}

//Worker thread function.
//It behaves as a malicious client. Connects to the SQL server. In each test iteration generates some random values
//for the function number, handle, handle type, IPC arguments. Then sends a command to the server using these
//randomly generated values. If the server crashes and the thread function receives KErrServerTerminated error,
//then the thread kills itself and the main thread will get KPanicCategory and KPanicCode as a reason for the
//worker thread's death. The last set of randomly generated values will be stored in the memory, pointed by aData argument.
TInt ThreadFunc1(void* aData)
	{
	__UHEAP_MARK;
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TTEST(tc != NULL);

	TThreadData* p = static_cast <TThreadData*> (aData);
	TTEST(p != NULL);
	TThreadData& data = *p;

	TVersion sqlSoftwareVersion(KSqlMajorVer, KSqlMinorVer, KSqlBuildVer);
	RTestSqlDbSession sess;
	TInt err = sess.Connect(sqlSoftwareVersion);
	TTEST2(err, KErrNone);

	while(++data.iIteration <= KTestIterCount)
		{
		PrintIterationCount(data.iIteration);
		TIpcArgs args;
		do
			{
			data.iFunction = Math::Rand(data.iSeed) % (ESqlSrvStreamClose + 1);//ESqlSrvStreamClose - the last server message number)
			}
		while(data.iFunction >= ESqlSrvResourceMark && data.iFunction <= ESqlSrvSetHeapFailure);
		for(TInt i=0;i<KMaxMessageArguments;++i)
			{
			//Initialize arguments
			data.iArgType[i] = static_cast <TArgType> (Math::Rand(data.iSeed) % ELastArgType);
			switch(data.iArgType[i])
				{
				case EIntArgType:
					data.iIntArg[i] = Math::Rand(data.iSeed) % 9711;
					args.Set(i, data.iIntArg[i]);
					break;
				case ETextArgType:
					{
					TInt len = Math::Rand(data.iSeed) % KMaxDesArgLen;	
					data.iTextArg[i].SetLength(len);
					args.Set(i, &data.iTextArg[i]);
					}
					break;
				case EBinArgType:
					{
					TInt len = Math::Rand(data.iSeed) % KMaxDesArgLen;	
					data.iBinArg[i].SetLength(len);
					args.Set(i, &data.iBinArg[i]);
					}
					break;
				default:
					User::Panic(KPanicCategory2, KPanicCode2);
					break;
				}
			}
		//Send arguments
		User::SetJustInTime(EFalse);
		TInt err = KErrNone;
		if((Math::Rand(data.iSeed) % 5) == 0)	//Pass a handle (statement or stream)
			{
			data.iHandleType = (Math::Rand(data.iSeed) % 2) ? ESqlSrvStatementHandle : ESqlSrvStreamHandle;
			data.iHandle = Math::Rand(data.iSeed) % 64;
			err = ::SendReceive(sess, data.iHandleType, data.iFunction, data.iHandle, args);
			}
		else
			{
			err = ::SendReceive(sess, data.iFunction, args);
			}
		if(err == KErrServerTerminated)
			{
			User::Panic(KPanicCategory, KPanicCode);
			}
		User::SetJustInTime(ETrue);
		}

	sess.Close();

	delete tc;	
	
	__UHEAP_MARKEND;
	
	return KErrNone;		
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1769
@SYMTestCaseDesc		In a loop, where the loop iterations are less than KTestIterCount (5000 at the moment), 
						the test creates a public shared database and a worker thread, which will behave as 
						malicious client. If the worker thread crashes the SQL server, then the worker thread
						dies notifying the main thread about the SQL server crash. The main thread prints the 
						values used in the last IPC call and crashes the test.
@SYMTestPriority		High
@SYMTestActions			SQL, Malicious client simulation test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
						REQ10405
						REQ10407
*/	
void BadClientTest()
	{
	TThreadData* p = new TThreadData;
	TEST(p != NULL);
	TThreadData& data = *p;
	data.iFunction = 0;
	TTime now;
	now.UniversalTime();
	data.iSeed = now.Int64();
	
	_LIT(KThreadName, "WorkThrd");
	
	for(data.iIteration=0;data.iIteration<KTestIterCount;++data.iIteration)
		{
		PrintIterationCount(data.iIteration);
		//Create/open the test database
		RSqlDatabase db;
		TInt err = db.Create(KTestDbName1);
		TEST(err == KErrNone || err == KErrAlreadyExists);
		if(err == KErrNone)
			{
			err = db.Exec(_L8("CREATE TABLE A(Id INTEGER); INSERT INTO A(Id) VALUES(1); INSERT INTO A(Id) VALUES(2);"));	
			TEST(err >= 0);
			}
		db.Close();
		//Run the malicious client (one worker theread which will try to crash the SQL server)
		RThread thread;
		TEST2(thread.Create(KThreadName, &ThreadFunc1, 0x2000, 0x1000, 0x10000, &data, EOwnerProcess), KErrNone);
		TRequestStatus status;
		thread.Logon(status);
		TEST2(status.Int(), KRequestPending);
		thread.Resume();
		User::WaitForRequest(status);
		User::SetJustInTime(ETrue);	// enable debugger panic handling
		if(thread.ExitType() == EExitPanic)
			{
			if(thread.ExitReason() == KPanicCode)
				{
				TheTest.Printf(_L("##Server terminated!\r\n"));
				TheTest.Printf(_L("##Iteration=%d, Handle type(hex)=%X, Function(hex)=%X, Handle=%d\r\n"), data.iIteration, data.iHandleType, data.iFunction, data.iHandle);
				for(TInt i=0;i<KMaxMessageArguments;++i)
					{
					switch(data.iArgType[i])
						{
						case EIntArgType:
							TheTest.Printf(_L("##Arg %d, Integer, value=%d\r\n"), i, data.iIntArg[i]);
							break;
						case ETextArgType:
							TheTest.Printf(_L("##Arg %d, Text,    length=%d\r\n"), i, data.iTextArg[i].Length());
							break;
						case EBinArgType:
							TheTest.Printf(_L("##Arg %d, Binary,  length=%d\r\n"), i, data.iBinArg[i].Length());
							break;
						default:
							TheTest.Printf(_L("##Arg %d, Invalid argument type: %d\r\n"), i, data.iArgType[i]);
							break;
						}
					}
				TEST(0);
				}
			}
		thread.Close();
		}
	User::SetJustInTime(ETrue);	// enable debugger panic handling
	delete p;
	}

RSqlSecurityPolicy CreateSecurityPolicy()
	{
	RSqlSecurityPolicy securityPolicy;
	const TSecurityPolicy KDefaultPolicy(TSecurityPolicy::EAlwaysPass);
	TInt err = securityPolicy.Create(KDefaultPolicy);
	TEST2(err, KErrNone);
	return securityPolicy;
	}

void DoBadNameTest(RSqlDatabase& aDb)
	{
	TFileName invalidFileName1;
	invalidFileName1.SetLength(KMaxFileName);
	//Initialise the first character so that it is not treated as an empty descriptor by the sever
	invalidFileName1[0] = TChar(204); //0xCC
	TFileName invalidFileName2;
	invalidFileName2.SetLength(KMaxFileName);
	invalidFileName2.Fill(TChar(204));//0xCC
	
	TInt err = aDb.Attach(invalidFileName1, invalidFileName2);
	TheTest.Printf(_L("Attach, invalid database name-1, err=%d\r\n"), err);
	TEST(err != KErrNone);
	err = aDb.Attach(invalidFileName2, invalidFileName1);
	TheTest.Printf(_L("Attach, invalid database name-2, err=%d\r\n"), err);
	TEST(err != KErrNone);
	err = aDb.Attach(KTestDbName2, invalidFileName2);
	TheTest.Printf(_L("Attach, invalid database name-3, err=%d\r\n"), err);
	TEST(err != KErrDied);
	if(err == KErrNone)
		{
		err = aDb.Detach(invalidFileName2);
		TheTest.Printf(_L("Detach, invalid database name-3, err=%d\r\n"), err);
		TEST2(err, KErrNone);
		}
	err = aDb.Attach(KTestDbName2, invalidFileName1);
	TheTest.Printf(_L("Attach, invalid database name-4, err=%d\r\n"), err);
	TEST(err != KErrDied);
	if(err == KErrNone)
		{
		err = aDb.Detach(invalidFileName1);
		TheTest.Printf(_L("Detach, invalid database name-4, err=%d\r\n"), err);
		TEST2(err, KErrNone);
		}
	
	RSqlDatabase::TSize size;

	err = aDb.Size(size, invalidFileName1);
	TheTest.Printf(_L("Size, invalid database name-1, err=%d\r\n"), err);
	TEST(err != KErrNone);
	err = aDb.Size(size, invalidFileName2);
	TheTest.Printf(_L("Size, invalid database name-2, err=%d\r\n"), err);
	TEST(err != KErrNone);

	err = aDb.Compact(RSqlDatabase::EMaxCompaction, invalidFileName1);
	TheTest.Printf(_L("Compact, invalid database name-1, err=%d\r\n"), err);
	TEST(err != KErrNone);
	err = aDb.Compact(RSqlDatabase::EMaxCompaction, invalidFileName2);
	TheTest.Printf(_L("Compact, invalid database name-2, err=%d\r\n"), err);
	TEST(err != KErrNone);

	RSqlStatement stmt;
	err = stmt.Prepare(aDb, _L("SELECT * FROM A"));
	TEST2(err, KErrNone);
	
	err = stmt.ColumnIndex(invalidFileName1);
	TheTest.Printf(_L("ColumnIndex, invalid column name-1, err=%d\r\n"), err);
	TEST(err != KErrNone);
	err = stmt.ColumnIndex(invalidFileName2);
	TheTest.Printf(_L("ColumnIndex, invalid column name-2, err=%d\r\n"), err);
	TEST(err != KErrNone);
	err = stmt.ParameterIndex(invalidFileName1);
	TheTest.Printf(_L("ParameterIndex, invalid parameter name-1, err=%d\r\n"), err);
	TEST(err != KErrNone);
	err = stmt.ParameterIndex(invalidFileName2);
	TheTest.Printf(_L("ParameterIndex, invalid parameter name-2, err=%d\r\n"), err);
	TEST(err != KErrNone);

	stmt.Close();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4048
@SYMTestCaseDesc		Bad database names - robustness test.
						The test defines file names with invalid content and attempts to use 
						these file names with the SQL API. The test should not cause crashes 
						in the SQL server.
@SYMTestPriority		High
@SYMTestActions			Bad database names - robustness test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10405
                        REQ10407
*/
void BadNameTest()
	{
	TFileName invalidFileName1;
	invalidFileName1.SetLength(KMaxFileName);
	TFileName invalidFileName2;
	invalidFileName2.SetLength(KMaxFileName / 2);
	
	//Initialise the first character so that it is not treated as an empty descriptor by the sever
	invalidFileName1[0] = TChar(204); //0xCC
	invalidFileName2[0] = TChar(204); //0xCC
	
	(void)RSqlDatabase::Delete(KTestDbName1);
	(void)RSqlDatabase::Delete(KTestDbName2);
		
	RSqlDatabase db;
	TInt err = db.Create(invalidFileName1);
	TEST(err != KErrNone);
	err = db.Create(invalidFileName2);
	TEST(err != KErrNone);

	err = db.Open(invalidFileName1);
	TEST(err != KErrNone);
	err = db.Open(invalidFileName2);
	TEST(err != KErrNone);

	err = db.Create(KTestDbName1);
	TEST2(err, KErrNone);
	err = db.Exec(_L("CREATE TABLE A(I INTEGER)"));
	TEST(err >= 0);
	db.Close();
	
	RSqlSecurityPolicy sp = CreateSecurityPolicy();
	err = db.Create(KTestDbName2, sp);
	TEST2(err, KErrNone);
	sp.Close();
	err = db.Exec(_L("CREATE TABLE A(I INTEGER)"));
	TEST(err >= 0);
	db.Close();

	TheTest.Printf(_L("Bad names test - public shared database\r\n"));
	err = db.Open(KTestDbName1);	
	TEST2(err, KErrNone);
	DoBadNameTest(db);
	db.Close();

	TheTest.Printf(_L("Bad names test - public secure shared database\r\n"));
	err = db.Open(KTestDbName2);	
	TEST2(err, KErrNone);
	DoBadNameTest(db);
	db.Close();
	
	(void)RSqlDatabase::Delete(KTestDbName2);
	(void)RSqlDatabase::Delete(KTestDbName1);
	
	err = RSqlDatabase::Copy(invalidFileName1, invalidFileName2);
	TheTest.Printf(_L("Copy database, err=%d\r\n"), err);
	TEST(err != KErrNone);
	err = RSqlDatabase::Delete(invalidFileName1);
	TheTest.Printf(_L("Delete database-1, err=%d\r\n"), err);
	TEST(err != KErrNone);
	err = RSqlDatabase::Delete(invalidFileName2);
	TheTest.Printf(_L("Delete database-2, err=%d\r\n"), err);
	TEST(err != KErrNone);
	}

/**
@SYMTestCaseID          PDS-SQL-CT-4200
@SYMTestCaseDesc        Invalid sql software version test.
@SYMTestPriority        High
@SYMTestActions			The test verifies that the SQL server checks that the software version of SQL sessions
						to be created is less or equal to the current version of the server software. 
						If that is not true then the SQL server does not create the session annd returns KErrNotSupported.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF145236
*/  
void InvalidSoftwareVersionTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName1);
	RSqlDatabase db;
	TInt err = db.Create(KTestDbName1);
	TEST2(err, KErrNone);
	db.Close();
	
	//Smaller version number
	TVersion sqlSoftwareVersion1(1, 0, 0);
	RTestSqlDbSession sess1;
	err = sess1.Connect(sqlSoftwareVersion1);
	sess1.Close();
	TEST2(err, KErrNone);
	
	//Bigger version number 1
	TVersion sqlSoftwareVersion2(1, 97, 3);
	RTestSqlDbSession sess2;
	err = sess2.Connect(sqlSoftwareVersion2);
	TEST2(err, KErrNotSupported);
	
	//Bigger version number 2
	TVersion sqlSoftwareVersion3(78, 0, 1);
	RTestSqlDbSession sess3;
	err = sess3.Connect(sqlSoftwareVersion3);
	TEST2(err, KErrNotSupported);

	//The current version number
	TVersion sqlSoftwareVersion4(KSqlMajorVer, KSqlMinorVer, KSqlBuildVer);
	RTestSqlDbSession sess4;
	err = sess4.Connect(sqlSoftwareVersion4);
	sess4.Close();
	TEST2(err, KErrNone);
	
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

void DoTests()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1769 Bad client test "));
	BadClientTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4048 Bad names test"));
	BadNameTest();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4200 Invalid software version test"));
	InvalidSoftwareVersionTest();
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	
	__UHEAP_MARK;
	
	DeleteTestFiles();
	CreateTestEnv();
	DoTests();
	DeleteTestFiles();
	TheFs.Close();

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
