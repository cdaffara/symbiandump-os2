// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <s32file.h>

///////////////////////////////////////////////////////////////////////////////////////

#define UNUSED_VAR(a) (a) = (a)

RTest TheTest(_L("t_sqlsecurity1 test"));

_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDbName, "c:[98765432]t_sqlsecurity1_1.db");
_LIT(KTestDbName2, "c:\\test\\t_sqlsecurity1_2.db");
_LIT(KTestDbName3, "c:[98765432]t_sqlsecurity1_3.db");
_LIT(KTestDbName4, "c:\\private\\98765432\\t_sqlsecurity1_4.db");
_LIT(KTestDbName5, "|00110022|33005500|c:");
_LIT(KTestFile, "c:\\test\\policy.strm");

_LIT(KDb1, "C:[98765432]t_nosettings.db");
_LIT(KDb2, "C:[98765432]t_emptysettings.db");
_LIT(KDb3, "C:[98765432]t_invversion.db");
_LIT(KDb4, "C:[98765432]t_nocollation.db");
_LIT(KDb5, "C:[98765432]t_longcollation.db");
_LIT(KDb6, "C:[98765432]t_nodefaultpolicy.db");
_LIT(KDb7, "C:[98765432]t_invobject.db");
_LIT(KDb8, "C:[98765432]t_2defaultpolicies.db");

const TSecurityPolicy KDefaultPolicy(ECapabilityDRM);
const TSecurityPolicy KPolicy1(ECapabilityTrustedUI, ECapabilityDiskAdmin);
const TSecurityPolicy KPolicy2(ECapabilityNetworkControl, ECapabilityPowerMgmt, ECapabilityCommDD);
const TSecurityPolicy KPolicy3(ECapabilityTCB, ECapabilityWriteUserData, ECapabilityUserEnvironment);

_LIT(KTableName1, "A-tbl");
_LIT(KTableName2, "B-table");
_LIT(KTableName3, "T-CCCCCCCCCCCCCC");
_LIT(KTableName11, "A-TBL");

RFs TheFs;

///////////////////////////////////////////////////////////////////////////////////////

void DeleteTestFiles()
	{
	(void)TheFs.Delete(KTestFile);
	(void)RSqlDatabase::Delete(KTestDbName4);
	(void)RSqlDatabase::Delete(KTestDbName3);
	(void)RSqlDatabase::Delete(KTestDbName2);
	(void)RSqlDatabase::Delete(KTestDbName);
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

void CreateTestEnv()
    {
	TInt err = TheFs.Connect();
	TEST2(err, KErrNone);

	err = TheFs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	err = TheFs.CreatePrivatePath(EDriveC);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

///////////////////////////////////////////////////////////////////////////////////////

TBool operator==(const TSecurityPolicy& aLeft, const TSecurityPolicy& aRight)
	{
	return aLeft.Package() == aRight.Package();
	}

void CreateTestSecurityPolicy(RSqlSecurityPolicy& aSecurityPolicy)
	{
	TInt err = aSecurityPolicy.Create(KDefaultPolicy);
	TEST2(err, KErrNone);

	err = aSecurityPolicy.SetDbPolicy(RSqlSecurityPolicy::ESchemaPolicy, KPolicy1);
	TEST2(err, KErrNone);
	err = aSecurityPolicy.SetDbPolicy(RSqlSecurityPolicy::EWritePolicy, KPolicy3);
	TEST2(err, KErrNone);

	err = aSecurityPolicy.SetPolicy(RSqlSecurityPolicy::ETable, KTableName1, RSqlSecurityPolicy::EReadPolicy, KPolicy1);
	TEST2(err, KErrNone);
	err = aSecurityPolicy.SetPolicy(RSqlSecurityPolicy::ETable, KTableName1, RSqlSecurityPolicy::EWritePolicy, KPolicy2);
	TEST2(err, KErrNone);

	err = aSecurityPolicy.SetPolicy(RSqlSecurityPolicy::ETable, KTableName2, RSqlSecurityPolicy::EReadPolicy, KPolicy1);
	TEST2(err, KErrNone);
	err = aSecurityPolicy.SetPolicy(RSqlSecurityPolicy::ETable, KTableName3, RSqlSecurityPolicy::EReadPolicy, KPolicy2);
	TEST2(err, KErrNone);

	err = aSecurityPolicy.SetPolicy(RSqlSecurityPolicy::ETable, KTableName2, RSqlSecurityPolicy::EWritePolicy, KPolicy1);
	TEST2(err, KErrNone);

	err = aSecurityPolicy.SetPolicy(RSqlSecurityPolicy::ETable, KTableName3, RSqlSecurityPolicy::EWritePolicy, KPolicy3);
	TEST2(err, KErrNone);
	}

void CheckTestSecurityPolicy(const RSqlSecurityPolicy& aSecurityPolicy)
	{
	//Database policies
	TSecurityPolicy policy = aSecurityPolicy.DbPolicy(RSqlSecurityPolicy::ESchemaPolicy);
	TEST(policy == KPolicy1);
	policy = aSecurityPolicy.DbPolicy(RSqlSecurityPolicy::EReadPolicy);
	TEST(policy == KDefaultPolicy);
	policy = aSecurityPolicy.DbPolicy(RSqlSecurityPolicy::EWritePolicy);
	TEST(policy == KPolicy3);
	//Table  policies
	policy = aSecurityPolicy.Policy(RSqlSecurityPolicy::ETable, KTableName11, RSqlSecurityPolicy::EReadPolicy);
	TEST(policy == KPolicy1);
	policy = aSecurityPolicy.Policy(RSqlSecurityPolicy::ETable, KTableName2, RSqlSecurityPolicy::EReadPolicy);
	TEST(policy == KPolicy1);
	policy = aSecurityPolicy.Policy(RSqlSecurityPolicy::ETable, KTableName3, RSqlSecurityPolicy::EReadPolicy);
	TEST(policy == KPolicy2);
	policy = aSecurityPolicy.Policy(RSqlSecurityPolicy::ETable, KTableName1, RSqlSecurityPolicy::EWritePolicy);
	TEST(policy == KPolicy2);
	policy = aSecurityPolicy.Policy(RSqlSecurityPolicy::ETable, KTableName3, RSqlSecurityPolicy::EWritePolicy);
	TEST(policy == KPolicy3);
	policy = aSecurityPolicy.Policy(RSqlSecurityPolicy::ETable, KTableName2, RSqlSecurityPolicy::EWritePolicy);
	TEST(policy == KPolicy1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1635
@SYMTestCaseDesc		RSqlSecurityPolicy test.
						Tests RSqlSecurityPolicy API.
@SYMTestPriority		High
@SYMTestActions			RSqlSecurityPolicy test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/
void SqlSecurityPolicyTest()
	{
	RSqlSecurityPolicy securityPolicy;
	TInt err = securityPolicy.Create(KDefaultPolicy);
	TEST2(err, KErrNone);

	//Empty RSqlSecurityPolicy object tests
		//Database policies
	TSecurityPolicy policy = securityPolicy.DbPolicy(RSqlSecurityPolicy::ESchemaPolicy);
	TEST(policy == KDefaultPolicy);
	policy = securityPolicy.DbPolicy(RSqlSecurityPolicy::EReadPolicy);
	TEST(policy == KDefaultPolicy);
	policy = securityPolicy.DbPolicy(RSqlSecurityPolicy::EWritePolicy);
	TEST(policy == KDefaultPolicy);
		//Table policies
	policy = securityPolicy.Policy(RSqlSecurityPolicy::ETable, _L("A-Tbl"), RSqlSecurityPolicy::EReadPolicy);
	TEST(policy == KDefaultPolicy);
	policy = securityPolicy.Policy(RSqlSecurityPolicy::ETable, _L("B-Tbl"), RSqlSecurityPolicy::EWritePolicy);
	TEST(policy == KDefaultPolicy);
		//
	securityPolicy.Close();

	//Tests with initialized RSqlSecurityPolicy object
	CreateTestSecurityPolicy(securityPolicy);
	//Check the created and initialized object
	CheckTestSecurityPolicy(securityPolicy);

	//Externalize, then internalize and check the security policy object
	RFileWriteStream out;
	err = out.Replace(TheFs, KTestFile, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	TRAP(err, securityPolicy.ExternalizeL(out));
	TEST2(err, KErrNone);
	TRAP(err, out.CommitL());
	out.Close();
	TEST2(err, KErrNone);

	securityPolicy.Close();
	err = securityPolicy.Create(KDefaultPolicy);
	TEST2(err, KErrNone);
	RFileReadStream in;
	err = in.Open(TheFs, KTestFile, EFileRead);
	TEST2(err, KErrNone);
	TRAP(err, securityPolicy.InternalizeL(in));
	in.Close();
	TEST2(err, KErrNone);
	CheckTestSecurityPolicy(securityPolicy);

	//Change some security policies and check that the RqlSecurityPolicy content was properly updated
	err = securityPolicy.SetPolicy(RSqlSecurityPolicy::ETable, KTableName11, RSqlSecurityPolicy::EReadPolicy, KPolicy3);
	TEST2(err, KErrNone);
	policy = securityPolicy.Policy(RSqlSecurityPolicy::ETable, KTableName1, RSqlSecurityPolicy::EReadPolicy);
	TEST(policy == KPolicy3);
	policy = securityPolicy.Policy(RSqlSecurityPolicy::ETable, KTableName11, RSqlSecurityPolicy::EWritePolicy);
	TEST(policy == KPolicy2);
	//
	securityPolicy.Close();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1637
@SYMTestCaseDesc		RSqlDatabase::Create() test.
						Tests RSqlDatabase::Create() call when the request is for creation of a secure database.
@SYMTestPriority		High
@SYMTestActions			RSqlDatabase::Create() test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/
void CreateSecureDatabaseTest()
	{
	RSqlSecurityPolicy securityPolicy;

	//Create and initialize RSqlSecurityPolicy object.
	CreateTestSecurityPolicy(securityPolicy);

	//Create secure database using the data from securityPolicy object.
	RSqlDatabase db;
	TInt err = db.Create(KTestDbName, securityPolicy);
	TEST2(err, KErrNone);
	securityPolicy.Close();

	//Check that the database security policy matches the policy used when the database was created.
	err = db.GetSecurityPolicy(securityPolicy);
	TEST2(err, KErrNone);
	CheckTestSecurityPolicy(securityPolicy);
	securityPolicy.Close();

	db.Close();

	//Reopen the database and check the security policies
	err = db.Open(KTestDbName);
	TEST2(err, KErrNone);

	//Check that the database security policy matches the policy used when the database was created.
	err = db.GetSecurityPolicy(securityPolicy);
	TEST2(err, KErrNone);
	CheckTestSecurityPolicy(securityPolicy);
	securityPolicy.Close();

	db.Close();

	err = RSqlDatabase::Delete(KTestDbName);
	TEST2(err, KErrNone);

	//An attempt to create a secure database specifying the full database path
	CreateTestSecurityPolicy(securityPolicy);
	err = db.Create(KTestDbName2, securityPolicy);
	securityPolicy.Close();
	TEST2(err, KErrArgument);

	//An attempt to create a non-secure database formatting the database file name as <drive:><dbFileName>
	err = db.Create(KTestDbName);
	TEST2(err, KErrArgument);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1643
@SYMTestCaseDesc		SQL server private path in database file name test.
						Verify that SQL API returns appropriate error, if an attempt is made to create, open
						or delete a secure database, with the full path specified in the database file name.
@SYMTestPriority		High
@SYMTestActions			SQL server private path in database file name test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/
void PrivatePathTest()
	{
	RSqlSecurityPolicy securityPolicy;
	CreateTestSecurityPolicy(securityPolicy);

	RSqlDatabase db;

	TInt err = db.Create(_L("C:\\PrIVATE\\10281E17\\[98765432]A.DB"), securityPolicy);
	TEST2(err, KErrArgument);
	err = db.Create(_L("C:\\PRIVATE\\10281E17\\[98765432]A.DB"));
	TEST2(err, KErrArgument);
	err = db.Open(_L("C:\\PRIVATE\\10281E17\\[98765432]A.DB"));
	TEST2(err, KErrArgument);
	err = db.Open(_L("C:\\PRIvaTe\\10281E17\\[98765432]A.DB"));
	TEST2(err, KErrArgument);

	err = db.Create(KTestDbName2);
	TEST2(err, KErrNone);
	err = db.Attach(_L("C:\\PRIVATe\\10281E17\\[98765432]A.DB"), _L("Db"));
	TEST2(err, KErrArgument);
	err = db.Attach(_L("C:\\PRIVAtE\\10281E17\\[98765432]A.DB"), _L("Db"));
	TEST2(err, KErrArgument);
	//This is an attempt to attach a database from the application's private data cage
	err = db.Attach(KTestDbName4, _L("Db"));
	TEST2(err, KErrNotFound);//There is no "Db" database file in the application's private data cage
	db.Close();

	err = RSqlDatabase::Delete(_L("C:\\pRIVATE\\10281E17\\[98765432]A.DB"));
	TEST2(err, KErrArgument);
	err = RSqlDatabase::Delete(_L("C:\\PRIvATE\\10281E17\\[98765432]A.DB"));
	TEST2(err, KErrArgument);

	//This is an attempt to create a database in the application's private data cage
	err = db.Create(KTestDbName4);
	TEST2(err, KErrNone);
	db.Close();
	err = RSqlDatabase::Delete(KTestDbName4);
	TEST2(err, KErrNone);

	err = db.Create(KTestDbName5);
	TEST2(err, KErrArgument);

	securityPolicy.Close();
	}

//This test is here to check that the construction of the second database object on the server side passes
//without problems, if there is already one other secure database object (authorizer callback possible problems
//during the construction phase)
//Shortly: the test checks how the construction of the secure database object 2 might be influenced by
//         the already existing secure database object 1.
void TwoSecureDbTest()
	{
	RSqlSecurityPolicy securityPolicy;
	CreateTestSecurityPolicy(securityPolicy);

	RSqlDatabase db1;
	TInt err = db1.Create(KTestDbName, securityPolicy);
	TEST2(err, KErrNone);

	RSqlDatabase db2;
	err = db2.Create(KTestDbName3, securityPolicy);
	TEST2(err, KErrNone);

	securityPolicy.Close();
	db2.Close();
	db1.Close();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3508
@SYMTestCaseDesc		Test for DEF109100: SQL, code coverage for TSqlDbSysSettings is low.
						The test attempts to create a secure shared database with a security policy
						specified for the system tables, which is not allowed.
@SYMTestPriority		High
@SYMTestActions			Test for DEF109100: SQL, code coverage for TSqlDbSysSettings is low.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF109100
*/
void StoreSysTableSecurityPolicyTest()
	{
	_LIT(KSecurityTable, "symbian_security");
	_LIT(KSettingsTable, "symbian_settings");

	//Case 1: "symbian_security" table
	RSqlSecurityPolicy sp;
	TInt err = sp.Create(KDefaultPolicy);
	TEST2(err, KErrNone);
	err = sp.SetDbPolicy(RSqlSecurityPolicy::ESchemaPolicy, KPolicy1);
	TEST2(err, KErrNone);
	err = sp.SetPolicy(RSqlSecurityPolicy::ETable, KSecurityTable, RSqlSecurityPolicy::EReadPolicy, KPolicy1);
	TEST2(err, KErrNone);
	(void)RSqlDatabase::Delete(KTestDbName);
	RSqlDatabase db;
	err = db.Create(KTestDbName, sp);
	TEST2(err, KErrArgument);
	sp.Close();

	//Case 2: "symbian_settings" table
	err = sp.Create(KDefaultPolicy);
	TEST2(err, KErrNone);
	err = sp.SetDbPolicy(RSqlSecurityPolicy::ESchemaPolicy, KPolicy1);
	TEST2(err, KErrNone);
	err = sp.SetPolicy(RSqlSecurityPolicy::ETable, KSettingsTable, RSqlSecurityPolicy::EReadPolicy, KPolicy1);
	TEST2(err, KErrNone);
	err = RSqlDatabase::Delete(KTestDbName);
	TEST2(err, KErrNotFound);
	err = db.Create(KTestDbName, sp);
	TEST2(err, KErrArgument);
	sp.Close();

	err = RSqlDatabase::Delete(KTestDbName);
	TEST2(err, KErrNotFound);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3509
@SYMTestCaseDesc		Test for DEF109100: SQL, code coverage for TSqlDbSysSettings is low.
						The test has 5 sub-cases, where the test attempts to open
						a secure shared database with:
						- no settings table;
						- empty settings table;
						- invalid version number of the settings;
						- NULL collation dll name, stored in the settings;
						- too long collation dll name, stored in the settings;
@SYMTestPriority		High
@SYMTestActions			Test for DEF109100: SQL, code coverage for TSqlDbSysSettings is low.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF109100
*/
void LoadSysSettingsTest()
	{
	//Case 1: no "symbian_settings" table. No error returned, because the table is recreated.
	RSqlDatabase db;
	TInt err = db.Open(KDb1);
	TEST2(err, KErrNone);
	db.Close();

	//Case 2: "symbian_settings" table is empty.
	err = db.Open(KDb2);
	TEST2(err, KErrGeneral);
	db.Close();

	//Case 3: Invalid version number in "symbian_settings" table.
	err = db.Open(KDb3);
	TEST2(err, KErrGeneral);
	db.Close();

	//Case 4: NULL collation dll name in "symbian_settings" table.
	err = db.Open(KDb4);
	TEST2(err, KErrGeneral);
	db.Close();

	//Case 5: Too long collation dll name in "symbian_settings" table.
	err = db.Open(KDb5);
	TEST2(err, KErrOverflow);
	db.Close();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3510
@SYMTestCaseDesc		Test for DEF109100: SQL, code coverage for TSqlDbSysSettings is low.
						The test has 3 sub-cases, where the test attempts to open
						a secure shared database with:
						- missing default policy in the security policy table;
						- invalid database object type in the security policy table;
						- 2 default policies in the security policy table;
@SYMTestPriority		High
@SYMTestActions			Test for DEF109100: SQL, code coverage for TSqlDbSysSettings is low.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF109100
*/
void LoadSecurityPolicyTest()
	{
	//Case 1: missing default policy.
	RSqlDatabase db;
	TInt err = db.Open(KDb6);
	TEST2(err, KErrGeneral);
	db.Close();

	//Case 2: invalid database object type.
	err = db.Open(KDb7);
	TEST2(err, KErrGeneral);
	db.Close();

	//Case 3: 2 default policies.
	err = db.Open(KDb8);
	TEST2(err, KErrGeneral);
	db.Close();
	}

void DoTests()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1635 RSqlSecurityPolicy test "));
	SqlSecurityPolicyTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1637 Create secure database test "));
	CreateSecureDatabaseTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1643 Sql server private path in the file name test "));
	PrivatePathTest();

	TheTest.Next(_L("Two secure databases at the same time test"));
	TwoSecureDbTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3508 DEF109100 - 1 - SQL, code coverage for TSqlBufRIterator,TSqlAttachDbRefCounter,TSqlDbSysSettings is low "));
	StoreSysTableSecurityPolicyTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3509 DEF109100 - 2 - SQL, code coverage for TSqlBufRIterator,TSqlAttachDbRefCounter,TSqlDbSysSettings is low "));
	LoadSysSettingsTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3510 DEF109100 - 3 - SQL, code coverage for TSqlBufRIterator,TSqlAttachDbRefCounter,TSqlDbSysSettings is low "));
	LoadSecurityPolicyTest();
	}

TInt E32Main()
	{
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();

	__UHEAP_MARK;

	CreateTestEnv();
	DeleteTestFiles();
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
