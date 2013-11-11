// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Note: Only runs on hardware as you cannot remove MMC card on emulator
// 
//

#include <e32test.h>
#include <bautils.h>
#include <s32stor.h>
#include <sqldb.h>

///////////////////////////////////////////////////////////////////////////////////////

static RFs TheFs;
RTest TheTest(_L("t_sqlattach2 test"));
RSqlDatabase TheDb;
RSqlDatabase TheDb2;
_LIT(KTestMMCDir, "c:\\test\\");
_LIT(KTestDir, "E:\\test\\");
_LIT(KTestDatabase1, "c:\\test\\t_sqlattach2_1.db");
_LIT(KTestMMCDatabase1, "E:\\test\\t_sqlattach2_2.db");
_LIT(KTestMMCDatabase2, "E:\\test\\t_sqlattach2_3.db");


_LIT(KTxtInsertMMCTxt," Insert MMC card\r\n");
_LIT(KTxtRemoveMMCTxt," Remove MMC card\r\n");
_LIT(KTxtPressAnyKey," [press any key to continue]\r\n");
_LIT(KTxtDefectTitle,"DEF116630  SQL, Missing test scenario: Open db, attach db on external media, remove media");


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		RDebug::Print(_L("*** Line %d\r\n"), aLine);
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Line %d, Expected error: %d, got: %d\r\n"), aLine, aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

//Creates file session instance and the test directory
void CreateTestEnv()
    {
	TInt err = TheFs.Connect();
	TEST2(err, KErrNone);

	err = TheFs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	err = TheFs.MkDir(KTestMMCDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}


/**
@SYMTestCaseID			SYSLIB-SQL-CT-4021
@SYMTestCaseDesc		Determine the result of union select operations on an external media located attached database
						when the external media is removed.
@SYMTestPriority		Medium

@SYMTestActions			Create database one on internal media and database two on external media.
						Insert records into both databases.
						Attach database two which resides on MMC card to database one.
						Prepare a union SELECT sql statement1 to retrieve records from the internal and attached database with database two listed before database one.
						Prepare a union SELECT sql statement2 to retrieve records from the internal and attached database with database one listed before database two
						Pauses and prompts the removal of the MMC card.
						Exec union select statement1 across databases.
						Exec union select statement2 across databases.

@SYMTestExpectedResults Databases are created and populated successfully.
						Database are attached sucessfully.
						Union statement1 operations return KErrNotReady
						Union statement2 operations return one record from internal database.
@SYMDEF					DEF116630
*/
void SqlAttachMMCDb2InternalDbUnionTestL()
	{

	CConsoleBase* console=Console::NewL(KTxtDefectTitle,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	console->Printf(KTxtInsertMMCTxt);
	console->Printf(KTxtPressAnyKey);
	console->Getch(); // get and ignore character

	//Initially remove any preexisting database;
	(void)RSqlDatabase::Delete(KTestMMCDatabase1);
	(void)RSqlDatabase::Delete(KTestDatabase1);

	TInt err = TheDb.Create(KTestDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L8("CREATE TABLE A(Id INTEGER)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L8("INSERT INTO A(Id) VALUES(1)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L8("INSERT INTO A(Id) VALUES(2)"));
	TEST2(err, 1);
	TheDb.Close();

	err = TheDb.Create(KTestMMCDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L8("CREATE TABLE B(N INTEGER)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(10)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(20)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(30)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(40)"));
	TEST2(err, 1);
	TheDb.Close();

	err = TheDb.Open(KTestDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Attach(KTestMMCDatabase1, _L("Db2"));
	TEST2(err, KErrNone);

	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L8("SELECT * FROM B union all Select * from A"));
	TEST2(err, KErrNone);

	RSqlStatement stmt2;
	err = stmt2.Prepare(TheDb, _L8("SELECT * FROM A union all Select * from B"));
	TEST2(err, KErrNone);

	console->Printf(KTxtRemoveMMCTxt);
	console->Printf(KTxtPressAnyKey);
	console->Getch(); // get and ignore character

	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	stmt.Close();

	err = stmt2.Next();
	TEST2(err, KSqlAtRow);
	stmt2.Close();

	err = TheDb.Detach(_L("Db2"));
	TEST2(err, KErrNone);

	TheDb.Close();

	CleanupStack::PopAndDestroy(); // close console
	(void)RSqlDatabase::Delete(KTestDatabase1);
	}


/**
@SYMTestCaseID			SYSLIB-SQL-CT-4022
@SYMTestCaseDesc		Determine the result of operations on an external media located attached database
						when the external media is removed.

@SYMTestPriority		Medium
@SYMTestActions			Create database one on internal media and database two on external media.
						Insert records into both databases.
						Attach database two which resides on MMC card to database one.
						Prepare a SELECT sql statement to retrieve records from the attached database two.
						Retrieve a record from database 2.
						Pauses and prompts the removal of the MMC card.
						Attempt to insert two records into database two.
						Prepare a SELECT sql statement to retrieve records from the attached database one.
						Attempt to select a record from dabase one.
						Attempt to insert record into database one.
						Reinsert the MMC card.
						Attempt to insert record in database one.
						Attempt to insert a record in database two.

@SYMTestExpectedResults Databases are created and populated successfully.
						Database are attached sucessfully.
						Select operations on database one are successful
						After MMC removal
						Disk write for database two return KErrNotReady.
						Select operations on database two are successful
						Select operations on database one are successful
						Write operations for database one are successful.
						After MMC re-insert
						Inserting a record into database one are succesful.
						Inserting a record into database two are successful

@SYMDEF					DEF116630
*/
void SqlAttachMMCDb2InternalDbTestL()
	{

	CConsoleBase* console=Console::NewL(KTxtDefectTitle,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	console->Printf(KTxtInsertMMCTxt);
	console->Printf(KTxtPressAnyKey);
	console->Getch(); // get and ignore character

	//Initially remove any preexisting database;
	(void)RSqlDatabase::Delete(KTestMMCDatabase1);
	(void)RSqlDatabase::Delete(KTestDatabase1);

	TInt err = TheDb.Create(KTestDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L8("CREATE TABLE A(Id INTEGER)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L8("INSERT INTO A(Id) VALUES(1)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L8("INSERT INTO A(Id) VALUES(2)"));
	TEST2(err, 1);
	TheDb.Close();

	err = TheDb.Create(KTestMMCDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L8("CREATE TABLE B(N INTEGER)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(10)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(20)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(30)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(40)"));
	TEST2(err, 1);
	TheDb.Close();

	err = TheDb.Open(KTestDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Attach(KTestMMCDatabase1, _L("Db2"));
	TEST2(err, KErrNone);

	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L8("SELECT * FROM B"));
	TEST2(err, KErrNone);

	err = stmt.Next();
	TEST2(err, KSqlAtRow);

	console->Printf(KTxtRemoveMMCTxt);
	console->Printf(KTxtPressAnyKey);
	console->Getch(); // get and ignore character

	err = stmt.Next();
	TEST2(err, KSqlAtRow);

	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(40)"));
	TEST2(err, KErrNotReady);


	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(50)"));
	TEST2(err, KErrNotReady);


	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	stmt.Close();
//Check that you can still perform operations on internal media based database
	RSqlStatement stmt2;
	err = stmt2.Prepare(TheDb, _L8("SELECT * FROM A"));
	TEST2(err, KErrNone);
	err = stmt2.Next();
	TEST2(err, KSqlAtRow);

	err = TheDb.Exec(_L8("INSERT INTO A(Id) VALUES(40)"));
	TEST2(err, 1);

	err = stmt2.Next();
	TEST2(err, KSqlAtRow);
	stmt2.Close();

	//Check that databases start working again when mmc card reinserted.
	console->Printf(KTxtInsertMMCTxt);
	console->Printf(KTxtPressAnyKey);
	console->Getch(); // get and ignore character

	err = TheDb.Detach(_L("Db2"));
	TEST2(err, KErrNone);

	TheDb.Close();

	err = TheDb2.Open(KTestDatabase1);
	TEST2(err, KErrNone);
	err = TheDb2.Attach(KTestMMCDatabase1, _L("Db2"));
	TEST2(err, KErrNone);


	err = TheDb2.Exec(_L8("INSERT INTO A(Id) VALUES(50)"));
	TEST2(err, 1);
	err = TheDb2.Exec(_L8("INSERT INTO B(N) VALUES(50)"));
	TEST2(err, 1);

	err = TheDb2.Detach(_L("Db2"));
	TEST2(err, KErrNone);

	TheDb2.Close();
	CleanupStack::PopAndDestroy(); // close console
	(void)RSqlDatabase::Delete(KTestMMCDatabase1);
	(void)RSqlDatabase::Delete(KTestDatabase1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-4023
@SYMTestCaseDesc		Determine the result of multiple disk write operations on an internal and
						external media located database when the attached external media is removed.
@SYMTestPriority		Medium

@SYMTestActions			Create database 1 on internal media and database two on external media.
						Insert records into both databases.
						Attaches database 2 which resides on MMC card to database 1.
						Prepare a SELECT sql statement to retrieve records from database 1.
						Retrieve a record from database 1.
						Pause and prompts the removal of the MMC card.
						Attempt to insert a record into database one.
						Attempt to select records from dabase one.
						Attempt to prepare a select statement on database two.
						Attempt to retrieve a record from database two.
						Attempts to insert record into database two.
@SYMTestExpectedResults Databases are created and populated successfully.
						Database are attached sucessfully.
						Prepare and select operations on database one are successful
						MMC Removed
						Write operations on database one are successful.
						Select operations on database one are successful.
						Prepare statement on database two are successful
						Disk read/write operations on database two return KErrNotReady.

@SYMDEF					DEF116630
*/
void SqlAttachMMCDb2InternalDbAndDoDiskOpsTestL()
	{

	CConsoleBase* console=Console::NewL(KTxtDefectTitle,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	console->Printf(KTxtInsertMMCTxt);
	console->Printf(KTxtPressAnyKey);
	console->Getch(); // get and ignore character

	//Initially remove any preexisting database;
	(void)RSqlDatabase::Delete(KTestMMCDatabase1);
	(void)RSqlDatabase::Delete(KTestDatabase1);

	TInt err = TheDb.Create(KTestDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L8("CREATE TABLE A(Id INTEGER)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L8("INSERT INTO A(Id) VALUES(1)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L8("INSERT INTO A(Id) VALUES(2)"));
	TEST2(err, 1);
	TheDb.Close();

	err = TheDb.Create(KTestMMCDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L8("CREATE TABLE B(N INTEGER)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(10)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(20)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(30)"));
	TEST2(err, 1);
	TheDb.Close();

	err = TheDb.Open(KTestDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Attach(KTestMMCDatabase1, _L("Db2"));
	TEST2(err, KErrNone);

	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L8("SELECT * FROM A"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);

	console->Printf(KTxtRemoveMMCTxt);
	console->Printf(KTxtPressAnyKey);
	console->Getch(); // get and ignore character

	err = TheDb.Exec(_L8("INSERT INTO A(Id) VALUES(40)"));
	TEST2(err, 1);

	err = stmt.Next();
	TEST2(err, KSqlAtRow);

	stmt.Close();

//Check that you can still perform operations on external media based database
	RSqlStatement stmt2;
	err = stmt2.Prepare(TheDb, _L8("SELECT * FROM B"));
	TEST2(err, KErrNone);
	err = stmt2.Next();
	TEST2(err,KSqlAtRow);

	stmt2.Close();

	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(40)"));
	TEST2(err, KErrNotReady);

	err = TheDb.Detach(_L("Db2"));
	TEST2(err, KErrNone);

	TheDb.Close();
	CleanupStack::PopAndDestroy(); // close console
	(void)RSqlDatabase::Delete(KTestDatabase1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-4024
@SYMTestCaseDesc		Determine the result of operations on an internal media located attached database
						to a external media database
						when the external media is removed.
@SYMTestPriority		Medium

@SYMTestActions			Create database 1 on internal media and database two on external media.
						Insert records into both databases.
						Attempt to Attach database 1 to database two which resides on MMC card.
						Attempt to Prepare a SELECT statement to retrieve records from the attached database 2.
						Attempt to retrieve record from database two.
						Pause and prompts the removal of the MMC card.
						Attempt to SELECT a record from database two.
						Attempt to insert a record into database two.
						Attempt to SELECT a record from database two.
						Attempt to prepare a select statement on database one.
						Attempt to select records from dabase one.
						Attempt to insert record into database one.

@SYMTestExpectedResults Databases are created and populated successfully.
						Database are attached sucessfully.
						Select operations on database two are successful
						MMC Removed
						Select operation on database two is successful
						Operations involving a disk write for database 2 return KErrNotReady.
						Select operation on database two is successful
						Prepare select operations on database on is successful
						Select operations on database one are successful
						Insert operations on database one are successful
@SYMDEF					DEF116630
*/
void SqlAttachInternalDb2MMCDbTestL()
	{

	CConsoleBase* console=Console::NewL(KTxtDefectTitle,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	console->Printf(KTxtInsertMMCTxt);
	console->Printf(KTxtPressAnyKey);
	console->Getch(); // get and ignore character

	//Initially remove any preexisting database;
	(void)RSqlDatabase::Delete(KTestMMCDatabase1);
	(void)RSqlDatabase::Delete(KTestDatabase1);

	TInt err = TheDb.Create(KTestDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L8("CREATE TABLE A(Id INTEGER)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L8("INSERT INTO A(Id) VALUES(1)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L8("INSERT INTO A(Id) VALUES(2)"));
	TEST2(err, 1);
	TheDb.Close();

	err = TheDb.Create(KTestMMCDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L8("CREATE TABLE B(N INTEGER)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(10)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(20)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(30)"));
	TEST2(err, 1);
	TheDb.Close();

	err = TheDb.Open(KTestMMCDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Attach(KTestDatabase1, _L("Db2"));
	TEST2(err, KErrNone);

	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L8("SELECT * FROM B"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);

	console->Printf(KTxtRemoveMMCTxt);
	console->Printf(KTxtPressAnyKey);
	console->Getch(); // get and ignore character

	err = stmt.Next();
	TEST2(err, KSqlAtRow);

	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(40)"));
	TEST2(err, KErrNotReady);

	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	stmt.Close();

//Check that you can still perform operations on internal media based database
	RSqlStatement stmt2;
	err = stmt2.Prepare(TheDb, _L8("SELECT * FROM A"));
	TEST2(err, KErrNone);
	err = stmt2.Next();
	TEST2(err, KSqlAtRow);

	err = TheDb.Exec(_L8("INSERT INTO A(Id) VALUES(40)"));
	TEST2(err, 1);

	err = stmt2.Next();
	TEST2(err, KSqlAtRow);
	stmt2.Close();

	err = TheDb.Detach(_L("Db2"));
	TEST2(err, KErrNone);

	TheDb.Close();
	CleanupStack::PopAndDestroy(); // close console
	(void)RSqlDatabase::Delete(KTestDatabase1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-4025
@SYMTestCaseDesc		Determine the result of operations on an external media located attached database
						to another external media database when the external media is removed.
@SYMTestPriority		Medium
@SYMTestActions			Create database one and database two on external media.
						Insert records into both databases.
						Attach database two to database one
						Prepare a select statement on database two.
						Retrieve a record from database two.
						Pause and prompts the removal of the MMC card.
						Retrieve a record from database two.
						Attempt to insert a record into database two.
						Attempt to insert another record into database two.
						Retrieve a record from database two.
						Prepare a select statement on database one.
						Attempt to retrieve record from database one.
						Attempt to insert a record into database one.
						Attempt to retrieve record from database one.

@SYMTestExpectedResults Databases are created and populated successfully.
						Database are attached sucessfully.
						Select operations on database two are successful
						MMC Removed.
						Read operations are successful on database two.
						First Insert operation return KErrNotReady for database two.
						Second Insert operation return KErrNotReady for database two.
						Read operations are still successful on database two.
						Read operations are still successful on database one.
						Insert operation returns KErrNotReady.
						Read operations are still successful on database one.
@SYMDEF				DEF116630
*/
void SqlAttachMMCDb12MMCDb2TestL()
	{

	CConsoleBase* console=Console::NewL(KTxtDefectTitle,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	console->Printf(KTxtInsertMMCTxt);
	console->Printf(KTxtPressAnyKey);
	console->Getch(); // get and ignore character

	//Initially remove any preexisting database;
	(void)RSqlDatabase::Delete(KTestMMCDatabase1);
	(void)RSqlDatabase::Delete(KTestMMCDatabase2);

	TInt err = TheDb.Create(KTestMMCDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L8("CREATE TABLE A(Id INTEGER)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L8("INSERT INTO A(Id) VALUES(1)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L8("INSERT INTO A(Id) VALUES(2)"));
	TEST2(err, 1);
	TheDb.Close();

	err = TheDb.Create(KTestMMCDatabase2);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L8("CREATE TABLE B(N INTEGER)"));
	TEST(err >= 0);
	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(10)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(20)"));
	TEST2(err, 1);
	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(30)"));
	TEST2(err, 1);
	TheDb.Close();

	err = TheDb.Open(KTestMMCDatabase1);
	TEST2(err, KErrNone);
	err = TheDb.Attach(KTestMMCDatabase2, _L("Db2"));
	TEST2(err, KErrNone);

	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L8("SELECT * FROM B"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);

	console->Printf(KTxtRemoveMMCTxt);
	console->Printf(KTxtPressAnyKey);
	console->Getch(); // get and ignore character

	err = stmt.Next();
	TEST2(err, KSqlAtRow);

	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(40)"));
	TEST2(err, KErrNotReady);

	err = TheDb.Exec(_L8("INSERT INTO B(N) VALUES(50)"));
	TEST2(err, KErrNotReady);

	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	stmt.Close();

//Check the error conditions when you attempt operations on the other database
	RSqlStatement stmt2;
	err = stmt2.Prepare(TheDb, _L8("SELECT * FROM A"));
	TEST2(err, KErrNone);
	err = stmt2.Next();
	TEST2(err, KSqlAtRow);

	err = TheDb.Exec(_L8("INSERT INTO A(Id) VALUES(40)"));
	TEST2(err, KErrNotReady);

	err = stmt2.Next();
	TEST2(err, KSqlAtRow);
	stmt2.Close();

	TheDb.Close();

	//Initially remove any preexisting database;
	(void)RSqlDatabase::Delete(KTestMMCDatabase1);
	(void)RSqlDatabase::Delete(KTestMMCDatabase2);

	CleanupStack::PopAndDestroy(); // close console
	}



void DoTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-4022 \"SQL against attached MMC db when MMC removed\" test "));
	SqlAttachMMCDb2InternalDbTestL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-4024 SQL against MMC db with attached internal database when MMC removed  "));
	SqlAttachInternalDb2MMCDbTestL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-4025 SQL against MMC based db with attached MMC based database when MMC removed  "));
	SqlAttachMMCDb12MMCDb2TestL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-4023 SQL against internal database with attached MMC db when MMC removed "));
	SqlAttachMMCDb2InternalDbAndDoDiskOpsTestL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-4021 SQL UNION against internal database with attached MMC db when MMC removed "));
	SqlAttachMMCDb2InternalDbUnionTestL();

	}

TInt E32Main()
	{
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();

	__UHEAP_MARK;
	CreateTestEnv();
	TRAPD(err, DoTestsL());
	TheFs.Close();
	TEST2(err, KErrNone);

	__UHEAP_MARKEND;

	TheTest.End();
	TheTest.Close();

	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
