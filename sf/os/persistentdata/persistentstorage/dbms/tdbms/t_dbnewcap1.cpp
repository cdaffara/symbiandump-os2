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

//The test uses C:TESTDB.DB secure shared database, which creates tables A, B and C.
#include <e32test.h>
#include <d32dbms.h>
#include "t_dbplatsecutl.h"

//The .spd file has the following capabilities:
//	Schema	None
//	Read	SurroundingsDD
//	Write	UserEnvironment
const TUid KSecureDbUid = {0x12344321};

_LIT(KSecure,	"SECURE");
_LIT(KDbName,	"C:TestDB.DB");
_LIT(KTblNameA,	"A");
_LIT(KTblNameB,	"B");
_LIT(KTblNameC,	"C");

static RTest 				TheTest(_L("t_dbnewcap1"));
static RDbs 				TheDbs;
static RDbNamedDatabase 	TheDb;
static RDbTable 			TheTbl;

TDBSCUtils 	TheDbscUtils(TheTest, NULL);

static TColDef const KColumns[]=
	{
	{_S("ID"), EDbColInt32, TDbCol::ENotNull | TDbCol::EAutoIncrement},
	{_S("DATA1"), EDbColInt32, TDbCol::ENotNull},
	{_S("DATA2"), EDbColInt32, TDbCol::ENotNull},
	{0}
	};

/**
@SYMTestCaseID SYSLIB-DBMS-CT-1361
@SYMTestCaseDesc Checking to make sure the .spd file used in this test has been created correctly and
functions as it should.
@SYMTestPriority Medium
@SYMTestActions Creating a database and 3 tables (A, B & C), running 3 actions on it and then deleting
the database (5 actions in total). Whether the actions are allowed or not is decided on the capabilities
that the database has (stated in the MMP file) relative to the capabilities that are needed to perform
certain actions (stated in the .spd file).
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF065282
*/
static void DefectTestL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-1361 An app with \"UserEnvironment\" capabilities set\n "));

	TheTest.Printf(_L("Test 1: Create tables\n"));
	//The tests must pass, because the test app has "SCHEMA" capability
	CDbColSet* colset = TDBSCUtils::CreateColSetLC(KColumns);
	TInt err = TheDb.CreateTable(KTblNameA, *colset);
	TEST2(err, KErrNone);
	err = TheDb.CreateTable(KTblNameB, *colset);
	TEST2(err, KErrNone);
	err = TheDb.CreateTable(KTblNameC, *colset);
	TEST2(err, KErrNone);
	CleanupStack::PopAndDestroy(colset);

	TheTest.Printf(_L("Test 2: Opening the tables in insert-only mode\n"));
	//The tests must pass because the test app has "WRITE" capability
	err = TheTbl.Open(TheDb, KTblNameA, RDbRowSet::EInsertOnly);
	TEST2(err, KErrNone);
	TheTbl.Close();
	err = TheTbl.Open(TheDb, KTblNameB, RDbRowSet::EInsertOnly);
	TEST2(err, KErrNone);
	TheTbl.Close();
	err = TheTbl.Open(TheDb, KTblNameC, RDbRowSet::EInsertOnly);
	TEST2(err, KErrNone);
	TheTbl.Close();

	TheTest.Printf(_L("Test 3: An attempt to open tables in read-only mode\n"));
	//The tests must not pass because the test app does not have "READ" capability.
	err = TheTbl.Open(TheDb, KTblNameA, RDbRowSet::EReadOnly);
	TEST2(err, KErrPermissionDenied);
	err = TheTbl.Open(TheDb, KTblNameB, RDbRowSet::EReadOnly);
	TEST2(err, KErrPermissionDenied);
	err = TheTbl.Open(TheDb, KTblNameC, RDbRowSet::EReadOnly);
	TEST2(err, KErrPermissionDenied);
	TheTbl.Close();
	}

TInt E32Main()
    {
	__UHEAP_MARK;
	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);

	TInt err = TheDbs.Connect();
	TEST2(err, KErrNone);

	TBuf<32> format;
	format.Copy(KSecure);
	format.Append(KSecureDbUid.Name());

	//Make sure there is no previous instance of the database.
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDbName);
	TheTest.Printf(_L("Create a database\n"));
	err = TheDb.Create(TheDbs, KDbName, format);
	TEST2(err, KErrNone);

	TRAP(err, ::DefectTestL());
	TEST2(err, KErrNone);

	TheTbl.Close();
	TheDb.Close();
	// Remove the database that was created during the tests
	TheTest.Printf(_L("Delete the database\n"));
	err = TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDbName);
	TEST2(err, KErrNone);
	TheDbs.Close();

	TheTest.End();
	TheTest.Close();

	delete tc;

	__UHEAP_MARKEND;
	User::Heap().Check();
	return KErrNone;
    }
