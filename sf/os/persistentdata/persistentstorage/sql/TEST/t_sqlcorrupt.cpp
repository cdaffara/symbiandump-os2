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
//
#include <e32test.h>
#include <bautils.h>
#include <sqldb.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "SqliteSymbian.h"

///////////////////////////////////////////////////////////////////////////////////////

RSqlDatabase TheDb;
RTest TheTest(_L("t_sqlcorrupt test"));

_LIT(KTestDir, "c:\\test\\");

_LIT(KDbName, "c:[08770000]t_sqlcorrupt.db");
_LIT(KFullDbName, "c:\\private\\10281E17\\[08770000]t_sqlcorrupt.db");

_LIT(KDbName2, "c:[08770000]t_sqlcorrupt2.db");
_LIT(KFullDbName2, "c:\\private\\10281E17\\[08770000]t_sqlcorrupt2.db");

RFs TheFs;

///////////////////////////////////////////////////////////////////////////////////////

void DestroyTestEnv()
	{
	TheDb.Close();
	(void)RSqlDatabase::Delete(KDbName2);
	(void)RSqlDatabase::Delete(KDbName);
	TheFs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DestroyTestEnv();
		RDebug::Print(_L("*** Boolean expression evaluated to false. Line %d\r\n"), aLine);
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DestroyTestEnv();
		RDebug::Print(_L("*** Line %d, Expected error: %d, got: %d\r\n"), aLine, aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

RSqlSecurityPolicy CreateTestSecurityPolicy()
	{
	RSqlSecurityPolicy policy;
	TInt err = policy.Create(TSecurityPolicy::EAlwaysPass);
	TEST2(err, KErrNone);

	err = policy.SetDbPolicy(RSqlSecurityPolicy::ESchemaPolicy, TSecurityPolicy::EAlwaysPass);
	TEST2(err, KErrNone);
	err = policy.SetDbPolicy(RSqlSecurityPolicy::EWritePolicy, TSecurityPolicy::EAlwaysPass);
	TEST2(err, KErrNone);
	err = policy.SetDbPolicy(RSqlSecurityPolicy::EReadPolicy, TSecurityPolicy::EAlwaysPass);
	TEST2(err, KErrNone);

	err = policy.SetPolicy(RSqlSecurityPolicy::ETable, _L("A"), RSqlSecurityPolicy::EWritePolicy, TSecurityPolicy::EAlwaysPass);
	TEST2(err, KErrNone);
	err = policy.SetPolicy(RSqlSecurityPolicy::ETable, _L("A"), RSqlSecurityPolicy::EReadPolicy, TSecurityPolicy::EAlwaysPass);
	TEST2(err, KErrNone);
	
	return policy;
	}

enum TDbEncoding
	{
	EDbEncUtf16,
	EDbEncUtf8,
	};

void DoCorruptedSecureDbTest(TDbEncoding aEncoding)
	{
	(void)RSqlDatabase::Delete(KDbName);
	
	RSqlSecurityPolicy policy = CreateTestSecurityPolicy();
	TInt err = KErrNone;
	if(aEncoding == EDbEncUtf16)
		{
		err = TheDb.Create(KDbName, policy);
		}
	else
		{
		_LIT8(KConfig, "encoding = \"UTF-8\"");
		err = TheDb.Create(KDbName, policy, &KConfig);
		}
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(I INTEGER); INSERT INTO A VALUES(10)"));
	TEST(err >= 0);
	TheDb.Close();
	policy.Close();

	CFileMan* fm = NULL;
	TRAP(err, fm = CFileMan::NewL(TheFs));
	TEST2(err, KErrNone);
	
	//Make a copy of the database
	err = fm->Copy(KFullDbName, KFullDbName2);
	TEST2(err, KErrNone);
	//Get the database file size and calculate the iterations count.
	TEntry entry;
	err = TheFs.Entry(KFullDbName, entry);
	TEST2(err, KErrNone);
	const TInt KCorruptBlockLen = 19;
	const TInt KIterationCnt = entry.iSize / KCorruptBlockLen;
	//
	TBuf8<KCorruptBlockLen> invalidData;
	invalidData.SetLength(KCorruptBlockLen);
	TChar invalidChar[] = {TChar((TUint)0xCC), TChar(0x00), TChar((TUint)-1), TChar(0x1D)};
	for(TInt j=0;j<(sizeof(invalidChar)/sizeof(invalidChar[0]));++j)
		{
		invalidData.Fill(invalidChar[j]);
		TheTest.Printf(_L("Invalid char: %X\r\n"), (TUint)(invalidChar[j]));
		for(TInt i=0;i<KIterationCnt;++i)
			{
			TheTest.Printf(_L("% 4d\r"), i + 1);
			//Corrupt the database
			err = fm->Copy(KFullDbName2, KFullDbName);
			TEST2(err, KErrNone);
			RFile file;
			err = file.Open(TheFs, KFullDbName, EFileRead | EFileWrite);
			TEST2(err, KErrNone);
			TInt fpos = i * KCorruptBlockLen;
			err = file.Write(fpos, invalidData);
			TEST2(err, KErrNone);
			file.Close();
			//Try to open the database and read the record
			TBool testPassed = EFalse;
			err = TheDb.Open(KDbName);
			if(err == KErrNone)
				{
				RSqlStatement stmt;
				err = stmt.Prepare(TheDb, _L("SELECT I FROM A"));
				if(err == KErrNone)
					{
					err = stmt.Next();
					if(err == KSqlAtRow)
						{
						TInt val = stmt.ColumnInt(0);
						if(val == 10)
							{
							testPassed = ETrue;
							err = KErrNone;
							}
						else
							{
							err = KErrGeneral;
							}
						}
					stmt.Close();
					}
				}

			TheDb.Close();
			(void)RSqlDatabase::Delete(KDbName);
			TheTest.Printf(_L("Iteration % 4d, err=%d\r\n"), i + 1, err);
			if(!testPassed)
				{
				TEST(err != KErrNone);
				}
			}//end of - for(TInt i=0;i<KIterationCnt;++i)
		TheTest.Printf(_L("\r\n"));
		}
	(void)fm->Delete(KFullDbName2);
	delete fm;
	}

/**
@SYMTestCaseID          PDS-SQL-CT-4202
@SYMTestCaseDesc        Invalid UTF16 encoded secure database test.
@SYMTestPriority        High
@SYMTestActions         The test creates 16-bit encoded secure database with one table and one record.
						Then the test simulates a database corruption by writing 19 bytes with random values
						from "pos" to "pos + 19", where "pos" is a valid db file position, incremented by 19
						at the end of each test iteration.
@SYMTestExpectedResults Test must not fail
*/  
void CorruptedSecureDbTest16()
	{
	DoCorruptedSecureDbTest(EDbEncUtf16);
	}

/**
@SYMTestCaseID          PDS-SQL-CT-4202
@SYMTestCaseDesc        Invalid UTF8 encoded secure database test.
@SYMTestPriority        High
@SYMTestActions         The test creates 8-bit encoded secure database with one table and one record.
						Then the test simulates a database corruption by writing 19 bytes with random values
						from "pos" to "pos + 19", where "pos" is a valid db file position, incremented by 19
						at the end of each test iteration.
@SYMTestExpectedResults Test must not fail
*/  
void CorruptedSecureDbTest8()
	{
	DoCorruptedSecureDbTest(EDbEncUtf8);
	}

const TInt KDbPolicyTypeTooSmall = 1;
const TInt KDbPolicyTypeTooBig = 2;
const TInt KDefaultPolicyWithObjName = 3;
const TInt KDbPolicyWithObjName = 4;
const TInt KInvObjPolicyWithObjName = 5;
const TInt KDefaultPolicyMoved = 6;
const TInt KDuplicatedDbPolicy = 7;
const TInt KTablePolicyTypeTooSmall = 8;
const TInt KTablePolicyTypeTooBig = 9;
const TInt KTablePolicyNameInvalid = 10;
const TInt KTwoDefaultPolicies = 11;

void CorruptDbPolicy(TInt aType)
	{
	TBuf8<200> dbName;
	dbName.Copy(KFullDbName());
	sqlite3* dbHandle = NULL;
	TInt err = sqlite3_open((const char*)(dbName.PtrZ()), &dbHandle);
	TEST2(err, SQLITE_OK);
	switch(aType)
		{
		case KDbPolicyTypeTooSmall: //Invalid database poicy type - less than the schema policy type
			err = sqlite3_exec(dbHandle, "UPDATE symbian_security SET PolicyType=-10 WHERE ObjectType=-1 AND PolicyType=0", NULL, 0, NULL);
			TEST2(err, SQLITE_OK);
			break;
		case KDbPolicyTypeTooBig: //Invalid database poicy type - bigger than the write policy type
			err = sqlite3_exec(dbHandle, "UPDATE symbian_security SET PolicyType=10 WHERE ObjectType=-1 AND PolicyType=0", NULL, 0, NULL);
			TEST2(err, SQLITE_OK);
			break;
		case KDefaultPolicyWithObjName: //Default database policy with valid object name 
			err = sqlite3_exec(dbHandle, "UPDATE symbian_security SET ObjectName='aaaa' WHERE ObjectType=-2", NULL, 0, NULL);
			TEST2(err, SQLITE_OK);
			break;
		case KDbPolicyWithObjName: //Database policy with valid object name 
			err = sqlite3_exec(dbHandle, "UPDATE symbian_security SET ObjectName='aaaa' WHERE ObjectType=-1", NULL, 0, NULL);
			TEST2(err, SQLITE_OK);
			break;
		case KInvObjPolicyWithObjName: //Invalid object type is with valid object name
			err = sqlite3_exec(dbHandle, "UPDATE symbian_security SET ObjectType=5, ObjectName='aaaa' WHERE ObjectType=0", NULL, 0, NULL);
			TEST2(err, SQLITE_OK);
			break;
		case KDefaultPolicyMoved: //The default security policy is the last in the table
			err = sqlite3_exec(dbHandle, "DELETE FROM symbian_security WHERE ObjectType=-2", NULL, 0, NULL);
			TEST2(err, SQLITE_OK);
			err = sqlite3_exec(dbHandle, "INSERT INTO symbian_security VALUES(20,-2,'',0, x'1122334455667788')", NULL, 0, NULL);
			TEST2(err, SQLITE_OK);
			break;
		case KDuplicatedDbPolicy: //Two database policies of the same type
			err = sqlite3_exec(dbHandle, "INSERT INTO symbian_security VALUES(9,-1,'',1, x'1122334455667788')", NULL, 0, NULL);
			TEST2(err, SQLITE_OK);
			break;
		case KTablePolicyTypeTooSmall: //Invalid table poicy type - less than the read policy type
			err = sqlite3_exec(dbHandle, "UPDATE symbian_security SET PolicyType=-10 WHERE ObjectType=0 AND PolicyType=1", NULL, 0, NULL);
			TEST2(err, SQLITE_OK);
			break;
		case KTablePolicyTypeTooBig: //Invalid table poicy type - less than the write policy type
			err = sqlite3_exec(dbHandle, "UPDATE symbian_security SET PolicyType=10 WHERE ObjectType=0 AND PolicyType=2", NULL, 0, NULL);
			TEST2(err, SQLITE_OK);
			break;
		case KTablePolicyNameInvalid:
			err = sqlite3_exec(dbHandle, "UPDATE symbian_security SET ObjectName='' WHERE ObjectType=0 AND PolicyType=2", NULL, 0, NULL);
			TEST2(err, SQLITE_OK);
			break;
		case KTwoDefaultPolicies:
			err = sqlite3_exec(dbHandle, "INSERT INTO symbian_security VALUES(9,-2,'',1, x'1122334455667788')", NULL, 0, NULL);
			TEST2(err, SQLITE_OK);
			break;
		default:
			TEST(0);
			break;
		}
	sqlite3_close(dbHandle);
	}

/**
@SYMTestCaseID          PDS-SQL-CT-4240
@SYMTestCaseDesc        Invalid settings in symbian_security table.
@SYMTestPriority        High
@SYMTestActions         The test creates a secure test database. Then the test runs a set of test 
						 iterations. On each itertaion the test makes onle of the stored security 
						 policies invalied (symbian_security table). Then the test attempts to open 
						 the database and checks the behaviour of the SQL server - whether an error
						 will be reproted back or not.
@SYMTestExpectedResults Test must not fail
*/  
void BadDbPolicyTest()
	{
	sqlite3SymbianLibInit();    
	
	RSqlSecurityPolicy policy = CreateTestSecurityPolicy();
	//
	(void)RSqlDatabase::Delete(KDbName);
	TInt err = TheDb.Create(KDbName, policy);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(I INTEGER); INSERT INTO A VALUES(10)"));
	TEST(err >= 0);
	TheDb.Close();
	policy.Close();
	//
	CFileMan* fm = NULL;
	TRAP(err, fm = CFileMan::NewL(TheFs));
	TEST2(err, KErrNone);
	//Make a copy of the database
	err = fm->Copy(KFullDbName, KFullDbName2);
	TEST2(err, KErrNone);
	//Invalid database policy - policy type value less than the schema policy type value
	CorruptDbPolicy(KDbPolicyTypeTooSmall);
	err = TheDb.Open(KDbName);
	TheDb.Close();
	TEST2(err, KErrGeneral);
	err = fm->Copy(KFullDbName2, KFullDbName);
	TEST2(err, KErrNone);
	//Invalid database policy - policy type value bigger than the write policy type value
	CorruptDbPolicy(KDbPolicyTypeTooBig);
	err = TheDb.Open(KDbName);
	TheDb.Close();
	TEST2(err, KErrGeneral);
	err = fm->Copy(KFullDbName2, KFullDbName);
	TEST2(err, KErrNone);
	//Invalid database policy - the default database policy is with valid object name
	CorruptDbPolicy(KDefaultPolicyWithObjName);
	err = TheDb.Open(KDbName);
	TheDb.Close();
	TEST2(err, KErrNone);
	err = fm->Copy(KFullDbName2, KFullDbName);
	TEST2(err, KErrNone);
	//Invalid database policy - database policy is with valid object name
	CorruptDbPolicy(KDbPolicyWithObjName);
	err = TheDb.Open(KDbName);
	TheDb.Close();
	TEST2(err, KErrNone);
	err = fm->Copy(KFullDbName2, KFullDbName);
	TEST2(err, KErrNone);
	//Invalid database policy - invalid object type is with valid object name
	CorruptDbPolicy(KInvObjPolicyWithObjName);
	err = TheDb.Open(KDbName);
	TheDb.Close();
	TEST2(err, KErrGeneral);
	err = fm->Copy(KFullDbName2, KFullDbName);
	TEST2(err, KErrNone);
	//Invalid database policy - the default security policy is the last in the table
	CorruptDbPolicy(KDefaultPolicyMoved);
	err = TheDb.Open(KDbName);
	TheDb.Close();
	TEST2(err, KErrNone);
	err = fm->Copy(KFullDbName2, KFullDbName);
	TEST2(err, KErrNone);
	//Invalid database policy - duplicated database policy
	CorruptDbPolicy(KDuplicatedDbPolicy);
	err = TheDb.Open(KDbName);
	TheDb.Close();
	TEST2(err, KErrGeneral);
	err = fm->Copy(KFullDbName2, KFullDbName);
	TEST2(err, KErrNone);
	//Invalid table policy - policy type value less than the read policy type value
	CorruptDbPolicy(KTablePolicyTypeTooSmall);
	err = TheDb.Open(KDbName);
	TheDb.Close();
	TEST2(err, KErrGeneral);
	err = fm->Copy(KFullDbName2, KFullDbName);
	TEST2(err, KErrNone);
	//Invalid table policy - policy type value bigger than the write policy type value
	CorruptDbPolicy(KTablePolicyTypeTooBig);
	err = TheDb.Open(KDbName);
	TheDb.Close();
	TEST2(err, KErrGeneral);
	err = fm->Copy(KFullDbName2, KFullDbName);
	TEST2(err, KErrNone);
	//Invalid table policy - emtpy string as a table name 
	CorruptDbPolicy(KTablePolicyNameInvalid);
	err = TheDb.Open(KDbName);
	TheDb.Close();
	TEST2(err, KErrGeneral);
	err = fm->Copy(KFullDbName2, KFullDbName);
	TEST2(err, KErrNone);
	//Two default policies
	CorruptDbPolicy(KTwoDefaultPolicies);
	err = TheDb.Open(KDbName);
	TheDb.Close();
	TEST2(err, KErrGeneral);
	err = fm->Copy(KFullDbName2, KFullDbName);
	TEST2(err, KErrNone);
	//
	(void)RSqlDatabase::Delete(KDbName);
	(void)fm->Delete(KFullDbName2);
	delete fm;
	
	sqlite3SymbianLibFinalize();
	CloseSTDLIB();
	}


void CreateTestEnv()
    {
    TInt err = TheFs.Connect();
    TEST2(err, KErrNone);

    err = TheFs.MkDir(KTestDir);
    TEST(err == KErrNone || err == KErrAlreadyExists);

    err = TheFs.CreatePrivatePath(EDriveC);
    TEST(err == KErrNone || err == KErrAlreadyExists);
    }

void DoTestsL()
	{
	TheTest.Start(_L("@SYMTestCaseID:PDS-SQL-CT-4202 Corrupted UTF16 encoded secure database test"));
	CorruptedSecureDbTest16();
	
	TheTest.Next(_L("@SYMTestCaseID:PDS-SQL-CT-4203 Corrupted UTF8 encoded secure database test"));
	CorruptedSecureDbTest8();
	
	TheTest.Next(_L("@SYMTestCaseID:PDS-SQL-CT-4240 Secure database with bad policy test"));
	BadDbPolicyTest();
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
