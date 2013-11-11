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

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_sqlcollate test"));

_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDbName1, "c:\\test\\t_sqlcollate.db");

///////////////////////////////////////////////////////////////////////////////////////

void DeleteTestFiles()
	{
	RSqlDatabase::Delete(KTestDbName1);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteTestFiles();
		TheTest(EFalse, aLine);
		}
	}
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DeleteTestFiles();
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

void CreateTestDir()
    {
    RFs fs;
	TInt err = fs.Connect();
	TEST2(err, KErrNone);

	err = fs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	
	fs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////

//Compare aLeft and aRight strings using collated comparison, level 3, default collation method.
//This function is used when sorting the test names array in CollationTest1L().
TInt StrSortC3(const TPtrC& aLeft, const TPtrC& aRight)
	{
	return aLeft.CompareC(aRight, 3, NULL);	
	}

//Compare aLeft and aRight strings using collated comparison, level 1, default collation method.
//This function is used when sorting the test names array in CollationTest3L().
TInt StrSortC1(const TPtrC& aLeft, const TPtrC& aRight)
	{
	return aLeft.CompareC(aRight, 1, NULL);	
	}

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1609
@SYMTestCaseDesc		Create a table with a text column with default collation "CompareF"
						and insert some records there. 
						Test how the searching operations work executing some SELECT SQL statements.
@SYMTestPriority		High
@SYMTestActions			Testing database search operations using "CompareF" and "CompareC3" collations.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5907
*/	
void CollationTest1L()
	{
	RSqlDatabase db;
	TInt err = db.Create(KTestDbName1);
	TEST2(err, KErrNone);

	//Create test database
	RDebug::Print(_L("###Create test database\r\n"));
	_LIT(KCreateSql, "CREATE TABLE A(Name VARCHAR(100) COLLATE CompareF); CREATE INDEX AIdx ON A(Name COLLATE CompareF);");
	err = db.Exec(KCreateSql);
	TEST(err >= 0);

	//Insert some records. The column "Name" of each record contains the same name but the name characters are
	//variation of upper/lower case letters.
	RDebug::Print(_L("###Insert some records\r\n"));
	_LIT(KInsertSql, "INSERT INTO A(Name) VALUES(");
	//Collation sort order:    KNames[1] KNames[3] KNames[0] KNames[2]
	//Long "aaaa..." added to the end of each column value because SQLITE may use non-aligned strings
	//only when the string length is in [32..<cache_page_size>] interval.
	TPtrC KNames[] = {
		_L("aLex-aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"), 
		_L("ALeX-aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"), 
		_L("aleX-aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"), 
		_L("Alex-aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")};
	const TInt KInsertSqlStmtCnt = sizeof(KNames)/sizeof(KNames[0]);
	RArray<TPtrC> sortedNames;
	TLinearOrder<TPtrC> order(&StrSortC3);
	
	for(TInt i=0;i<KInsertSqlStmtCnt;++i)
		{
		err = sortedNames.InsertInOrder(KNames[i], order);
		TEST2(err, KErrNone);
		
		TBuf<128> sql(KInsertSql);
		sql.Append(_L("'"));
		sql.Append(KNames[i]);
		sql.Append(_L("')"));
		err = db.Exec(sql);
		TEST2(err, 1);
		}
	
	//The next "SELECT" statement must return a set containing all table records
	RDebug::Print(_L("###Select all records\r\n"));
	_LIT(KSelectSql1, "SELECT * FROM A WHERE NAME = 'alex-aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'");
	RSqlStatement stmt;
	err = stmt.Prepare(db, KSelectSql1);
	TEST2(err, KErrNone);	
	TInt recCount = 0;
	while(stmt.Next() == KSqlAtRow)
		{
		++recCount;
		TPtrC name = stmt.ColumnTextL(0);
		RDebug::Print(_L("%S\r\n"), &name);
		}
	stmt.Close();
	TEST(recCount == KInsertSqlStmtCnt);

	//The next "SELECT" statement must return a set containing all table records
	// this tests a LIKE clause with a bound parameter (with wildcards)	
	RDebug::Print(_L("###Select all records (LIKE with wildcard)\r\n"));
	_LIT(KSelectSql1a, "SELECT * FROM A WHERE NAME LIKE :Val");
	_LIT(KSearchString,"alex-aaaa%");
	err = stmt.Prepare(db, KSelectSql1a);
	TEST2(err, KErrNone);
	TInt idx=stmt.ParameterIndex(_L(":Val"));
	err=stmt.BindText(idx,KSearchString);
	TEST2(err, KErrNone);
	recCount = 0;
	while(stmt.Next() == KSqlAtRow)
		{
		++recCount;
		TPtrC name = stmt.ColumnTextL(0);
		RDebug::Print(_L("%S\r\n"), &name);
		}
	stmt.Close();
	TEST(recCount == KInsertSqlStmtCnt);

	//The next "SELECT" statement must return a set containing all table records
	// this tests a LIKE clause with a bound parameter (with no wildcards)
	RDebug::Print(_L("###Select all records (LIKE with no wildcard)\r\n"));
	_LIT(KSelectSql1b, "SELECT * FROM A WHERE NAME LIKE :Val");
	_LIT(KSearchStringA,
"alex-aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	err = stmt.Prepare(db, KSelectSql1b);
	idx=stmt.ParameterIndex(_L(":Val"));
	TEST2(err, KErrNone);
	err=stmt.BindText(idx,KSearchStringA);
	recCount = 0;
	while(stmt.Next() == KSqlAtRow)
		{
		++recCount;
		TPtrC name = stmt.ColumnTextL(0);
		RDebug::Print(_L("%S\r\n"), &name);
		}
	stmt.Close();
	TEST(recCount == KInsertSqlStmtCnt);

	//The next "SELECT" statement must return a row 
	// this tests a LIKE clause with a bound parameter and funny characters
	RDebug::Print(_L("###Select one records (LIKE with bound param with URL chars)\r\n"));
	err=db.Exec(_L("INSERT INTO A(Name) VALUES('http://a.b.c#d')"));
	TEST2(err,1);
	_LIT(KSelectSql1c, "SELECT * FROM A WHERE NAME LIKE :Val");
	_LIT(KSearchStringB,"http%");
	err = stmt.Prepare(db, KSelectSql1c);
	idx=stmt.ParameterIndex(_L(":Val"));
	TEST2(err, KErrNone);
	err=stmt.BindText(idx,KSearchStringB);
	recCount = 0;
	while(stmt.Next() == KSqlAtRow)
		{
		++recCount;
		TPtrC name = stmt.ColumnTextL(0);
		RDebug::Print(_L("%S\r\n"), &name);
		}
	stmt.Close();
	TEST(recCount == 1);

	
	//The next "SELECT" statement must return a set containing all table records, folded comparison used for sorting
	RDebug::Print(_L("###Select all records, folded string comparison\r\n"));
	_LIT(KSelectSql2, "SELECT * FROM A WHERE NAME = 'alex-aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa' ORDER BY NAME COLLATE CompareF");
	err = stmt.Prepare(db, KSelectSql2);
	TEST2(err, KErrNone);

	recCount = 0;
	for(TInt j=0;j<KInsertSqlStmtCnt;++j)
		{
		err = stmt.Next();
		TEST2(err, KSqlAtRow);
		++recCount;
		TPtrC name = stmt.ColumnTextL(0);
		RDebug::Print(_L("%S\r\n"), &name);
		TEST(name == KNames[j]);
		}
	stmt.Close();
	TEST(recCount == KInsertSqlStmtCnt);

	//The next "SELECT" statement must return a set containing all table records, collated comparison used for sorting
	RDebug::Print(_L("###Select all records, collated string comparison\r\n"));
	_LIT(KSelectSql3, "SELECT * FROM A ORDER BY NAME COLLATE CompareC3");
	err = stmt.Prepare(db, KSelectSql3);
	TEST2(err, KErrNone);

	for(TInt k=0;k<KInsertSqlStmtCnt;++k)
		{
		err = stmt.Next();
		TEST2(err, KSqlAtRow);
		TPtrC name = stmt.ColumnTextL(0);
		RDebug::Print(_L("%S\r\n"), &name);
		TEST(name == sortedNames[k]);
		}
		
	stmt.Close();
	
	//Cleanup	
	sortedNames.Close();
	db.Close();
	
	//To debug database reindexing
	err = db.Open(KTestDbName1);
	TEST2(err, KErrNone);
	db.Close();
	
	RDebug::Print(_L("###Delete test database\r\n"));
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1610
@SYMTestCaseDesc		Create a table with a text column with default collation "CompareC3"
						and insert some records there. 
						Test how the searching operations work executing some SELECT SQL statements.
@SYMTestPriority		High
@SYMTestActions			Testing database sorting operations using "CompareC3" collation.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5907
*/	
void CollationTest2L()
	{
	RSqlDatabase db;
	TInt err = db.Create(KTestDbName1);
	TEST2(err, KErrNone);

	//Create test database
	RDebug::Print(_L("###Create test database\r\n"));
	_LIT(KCreateSql, "CREATE TABLE A(Name VARCHAR(100) COLLATE CompareC3)");
	err = db.Exec(KCreateSql);
	TEST(err >= 0);

	//Insert some records.
	RDebug::Print(_L("###Insert some records\r\n"));
	_LIT(KInsertSql, "INSERT INTO A(Name) VALUES(");
	TPtrC KNames[] = {
		_L("aAaA"), 
		_L("AAaa"), 
		_L("aaAA"), 
		_L("aaaA")};
	const TInt KInsertSqlStmtCnt = sizeof(KNames)/sizeof(KNames[0]);

	for(TInt i=0;i<KInsertSqlStmtCnt;++i)
		{
		TBuf<128> sql(KInsertSql);
		sql.Append(_L("'"));
		sql.Append(KNames[i]);
		sql.Append(_L("')"));
		err = db.Exec(sql);
		TEST2(err, 1);
		}

	RSqlStatement stmt;
	
	//The next "SELECT" statement must return a set containing all table 
	//records which Name column value is bigger than "aaAA"
	RDebug::Print(_L("###Select all records, which Name column value is bigger than 'aaAA'\r\n"));
	_LIT(KSelectSql2, "SELECT * FROM A WHERE NAME > 'aaAA'");
	err = stmt.Prepare(db, KSelectSql2);
	TEST2(err, KErrNone);

	while((err = stmt.Next()) == KSqlAtRow)
		{
		TPtrC name = stmt.ColumnTextL(0);
		RDebug::Print(_L("%S\r\n"), &name);
		TInt res = name.CompareC(KNames[2], 3, NULL); 
		TEST(res > 0);
		}
	stmt.Close();
	TEST2(err, KSqlAtEnd);

	//Cleanup	
	db.Close();
	RDebug::Print(_L("###Delete test database\r\n"));
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1627
@SYMTestCaseDesc		Create a table with a text column "NAME" with default collation "CompareC0"
						and insert some records there. All inserted names are equal if compared at
						collation level 0, but some of them contain accented letters.
						Test how the searching operations work executing some SELECT SQL statements.
						Test how the sorting operations work executing some SELECT SQL statements.
@SYMTestPriority		High
@SYMTestActions			Testing database sorting operations using "CompareC0", "CompareC1", and "CompareC2" collations.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5907
*/	
void CollationTest3L()
	{
	RSqlDatabase db;
	TInt err = db.Create(KTestDbName1);
	TEST2(err, KErrNone);

	//Create test database
	RDebug::Print(_L("###Create test database\r\n"));
	_LIT(KCreateSql, "CREATE TABLE A(Name VARCHAR(100) COLLATE CompareC0)");
	err = db.Exec(KCreateSql);
	TEST(err >= 0);

	//Insert some records. Some of the inserted names have accented letters.
	//But all names are equal if compared at collation level 0.
	RDebug::Print(_L("###Insert some records\r\n"));
	_LIT(KInsertSql, "INSERT INTO A(Name) VALUES(");
	TBuf<10> name1(_L("Dvorak"));
	TBuf<10> name2;
	name2.SetLength(6);
	name2[0] = TChar('D'); 
	name2[1] = TChar('v'); 
	name2[2] = TChar('o'); 
	name2[3] = 0x0158;//LATIN SMALL LETTER R HACEK
	name2[4] = 0x00C1;//LATIN SMALL LETTER A ACUTE
	name2[5] = TChar('k');
	const TPtrC KNames[] = {name1, name2};
	const TInt KInsertSqlStmtCnt = sizeof(KNames)/sizeof(KNames[0]);
	RArray<TPtrC> sortedNames;
	TLinearOrder<TPtrC> order(&StrSortC1);

	//Insert the records. Also, check how many names are equal to 'dvorak' using collation level 0.
	_LIT(KTestName, "dvorak");
	TInt matchNameCnt = 0;
	for(TInt i=0;i<KInsertSqlStmtCnt;++i)
		{
		if(KNames[i].CompareC(KTestName, 0, NULL) == 0)
			{
			++matchNameCnt;	
			}

		err = sortedNames.InsertInOrder(KNames[i], order);
		TEST2(err, KErrNone);

		TBuf<128> sql(KInsertSql);
		sql.Append(_L("'"));
		sql.Append(KNames[i]);
		sql.Append(_L("')"));
		err = db.Exec(sql);
		TEST2(err, 1);
		}

	//The next "SELECT" statement must return a set, which record count must be matchNameCnt.
	RDebug::Print(_L("###Select all records, collated string comparison, level 0\r\n"));
	_LIT(KSelectSql2, "SELECT * FROM A WHERE NAME = 'dvorak'");
	RSqlStatement stmt;
	err = stmt.Prepare(db, KSelectSql2);
	TEST2(err, KErrNone);

	TInt recCount = 0;
	while((err = stmt.Next()) == KSqlAtRow)
		{
		TPtrC name = stmt.ColumnTextL(0);
		RDebug::Print(_L("%S\r\n"), &name);
		TEST(name == KNames[recCount]);
		++recCount;
		}
	stmt.Close();
	TEST(recCount == matchNameCnt);

	//The next "SELECT" statement must return an ordered set containing all table records.
	RDebug::Print(_L("###Select all records, collated string comparison, level 1\r\n"));
	_LIT(KSelectSql3, "SELECT * FROM A WHERE NAME = 'dvorak' ORDER BY NAME COLLATE CompareC1 DESC");
	err = stmt.Prepare(db, KSelectSql3);
	TEST2(err, KErrNone);

	for(TInt k=0;k<KInsertSqlStmtCnt;++k)
		{
		err = stmt.Next();
		TEST2(err, KSqlAtRow);
		TPtrC name = stmt.ColumnTextL(0);
		RDebug::Print(_L("%S %S\r\n"), &name, &sortedNames[k]);
		TEST(name == sortedNames[KInsertSqlStmtCnt - k - 1]);//descending order
		}
	stmt.Close();

	//CompareC2 collation used in the SELECT statement
	err = stmt.Prepare(db, _L("SELECT NAME FROM A WHERE NAME = 'Dvorak' COLLATE CompareC2"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TPtrC name3 = stmt.ColumnTextL(0);
	TEST(name3 == name1);
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();

	//Cleanup	
	sortedNames.Close();
	db.Close();
	RDebug::Print(_L("###Delete test database\r\n"));
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1760
@SYMTestCaseDesc		Creating a test table with a text field. Inserting some records there and
						testing how LIKE operator works. The test cases include: accented text column values,
						using '_' and '%' wild cards, using ESCAPE clause, blank pattern string, blank ESCAPE
						string, multi-character ESCAPE string.
@SYMTestPriority		High
@SYMTestActions			Testing SQL LIKE operator and ESCAPE clause.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5907
*/
void LikeTest1()
	{
	RSqlDatabase db;
	TInt err = db.Create(KTestDbName1);
	TEST2(err, KErrNone);
	//Create a test table and insert some records
	err = db.Exec(_L("CREATE TABLE A(Id INTEGER PRIMARY KEY, Name TEXT)"));
	TEST(err >= 0);
	err = db.Exec(_L("INSERT INTO A(Id, Name) VALUES(1, 'Dvorak')"));
	TEST2(err, 1);
	err = db.Exec(_L("INSERT INTO A(Id, Name) VALUES(2, 'Dvorák')"));
	TEST2(err, 1);
	//Create a statement object and issue a SELECT SQL statement + LIKE clause
	//Test case 1 = full name search with LIKE
	RSqlStatement stmt;
	err = stmt.Prepare(db, _L("SELECT COUNT(*) FROM A WHERE Name LIKE 'DVORAK'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TInt cnt = stmt.ColumnInt(0);
	TEST2(cnt, 2);
	stmt.Close();
	//Test case 2 = wild card used sequence character in the search pattern + LIKE
	err = stmt.Prepare(db, _L("SELECT COUNT(*) FROM A WHERE Name LIKE '%RA%'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	cnt = stmt.ColumnInt(0);
	TEST2(cnt, 2);
	stmt.Close();
	//Test case 3 = wild card character used in the search pattern + LIKE
	err = stmt.Prepare(db, _L("SELECT COUNT(*) FROM A WHERE Name LIKE 'DV___K'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	cnt = stmt.ColumnInt(0);
	TEST2(cnt, 2);
	stmt.Close();
	//Insert one more record
	err = db.Exec(_L("INSERT INTO A(Id, Name) VALUES(3, 't_sqltest')"));
	TEST2(err, 1);
	//Test case 4 = wild card character used in the search pattern + LIKE + ESCAPE
	err = stmt.Prepare(db, _L("SELECT COUNT(*) FROM A WHERE Name LIKE 't/_sqlte__' ESCAPE '/'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	cnt = stmt.ColumnInt(0);
	TEST2(cnt, 1);
	stmt.Close();
	//Test case 5 = wild card character used in the search pattern + LIKE + ESCAPE without an escape character
	err = stmt.Prepare(db, _L("SELECT COUNT(*) FROM A WHERE Name LIKE 't/_sqltest' ESCAPE ''"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST(err != KErrNone);
	TEST2(::SqlRetCodeClass(err), ESqlDbError);
	TPtrC errMsg = db.LastErrorMessage();
	RDebug::Print(_L("!! error=\"%S\"\r\n"), &errMsg);
	stmt.Close();
	//Test case 6 = wild card character used in the search pattern + LIKE + ESCAPE with more than one escape characters
	err = stmt.Prepare(db, _L("SELECT COUNT(*) FROM A WHERE Name LIKE 't/_sqltest' ESCAPE '1234'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST(err != KErrNone);
	TEST2(::SqlRetCodeClass(err), ESqlDbError);
	errMsg.Set(db.LastErrorMessage());
	RDebug::Print(_L("!! error=\"%S\"\r\n"), &errMsg);
	stmt.Close();
	//Test case 7 = blank pattern string
	err = stmt.Prepare(db, _L("SELECT COUNT(*) FROM A WHERE Name LIKE ''"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	cnt = stmt.ColumnInt(0);
	TEST2(cnt, 0);
	stmt.Close();
	
	//Cleanup	
	db.Close();
	RDebug::Print(_L("###Delete test database\r\n"));
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1761
@SYMTestCaseDesc		Verifying that all 'LIKE + ESCAPE' test cases which work with the old DBMS component,
						pass successfully with the new SQL component. The test includes positive and negative test cases.
						Not all negative test cases from the old DBMS pass, because the DBMS supports limited implementation of
						the ESCAPE clause. Here the old negative tests were converted to a positive test cases.
@SYMTestPriority		High
@SYMTestActions			Testing SQL LIKE operator and ESCAPE clause.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5907
*/
void LikeTest2()
	{
	RSqlDatabase db;
	TInt err = db.Create(KTestDbName1);
	TEST2(err, KErrNone);
	//Create a test table
	err = db.Exec(_L("CREATE TABLE A(Fld1 TEXT, Fld2 TEXT)"));
	TEST(err >= 0);
	//Insert some records
	err = db.Exec(_L("INSERT INTO A(Fld1, Fld2) VALUES('ACDC\\','BLAH')"));	// Rec1
	TEST2(err, 1);
	err = db.Exec(_L("INSERT INTO A(Fld1) VALUES('ABCDEFGH')"));			// Rec2
	TEST2(err, 1);
	err = db.Exec(_L("INSERT INTO A(Fld1) VALUES('A_CDEFGH')"));			// Rec3
	TEST2(err, 1);
	err = db.Exec(_L("INSERT INTO A(Fld1) VALUES('A_%_CDEFGH')"));			// Rec4
	TEST2(err, 1);
	err = db.Exec(_L("INSERT INTO A(Fld1) VALUES('A%CDEFGH')"));			// Rec5
	TEST2(err, 1);
	err = db.Exec(_L("INSERT INTO A(Fld1, Fld2) VALUES('ADCDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOP','ADCB')"));	//Rec6
	TEST2(err, 1);
	err = db.Exec(_L("INSERT INTO A(Fld1) VALUES('XZD\\FZX')"));			// Rec7
	TEST2(err, 1);
	//Prepare SELECT query, step and check the record set content
	TPtrC res;
	//Test 1 - only Rec1 satisfies the WHILE condition
	RSqlStatement stmt;
	err = stmt.Prepare(db, _L("SELECT Fld2 FROM A WHERE  Fld1 LIKE 'ACDC\\' AND Fld2 LIKE '%BL%'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, res);
	TEST2(err, KErrNone);
	TEST(res == _L("BLAH"));
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Test 2 - only Rec5 satisfies the WHILE condition
	err = stmt.Prepare(db, _L("SELECT Fld1 FROM A WHERE  Fld1 LIKE '%A\\%C%' ESCAPE '\\'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, res);
	TEST2(err, KErrNone);
	TEST(res == _L("A%CDEFGH"));
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Test 3 - only Rec3 satisfies the WHILE condition
	err = stmt.Prepare(db, _L("SELECT Fld1 FROM A WHERE  Fld1 LIKE '%A\\_C%' ESCAPE '\\'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, res);
	TEST2(err, KErrNone);
	TEST(res == _L("A_CDEFGH"));
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Test 4 - only Rec4 satisfies the WHILE condition
	err = stmt.Prepare(db, _L("SELECT Fld1 FROM A WHERE  Fld1 LIKE '%A\\_\\%\\_C%' ESCAPE '\\'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, res);
	TEST2(err, KErrNone);
	TEST(res == _L("A_%_CDEFGH"));
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Test 5 - only Rec6 satisfies the WHILE condition
	err = stmt.Prepare(db, _L("SELECT Fld1, Fld2 FROM A WHERE  Fld1 LIKE '%1234%'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, res);
	TEST2(err, KErrNone);
	TEST(res == _L("ADCDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOP"));
	err = stmt.ColumnText(1, res);
	TEST2(err, KErrNone);
	TEST(res == _L("ADCB"));
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Test 6 - only Rec1 satisfies the WHILE condition
	err = stmt.Prepare(db, _L("SELECT Fld1, Fld2 FROM A WHERE Fld1 LIKE '%AC%' AND Fld2 LIKE '_LA_'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, res);
	TEST2(err, KErrNone);
	TEST(res == _L("ACDC\\"));
	err = stmt.ColumnText(1, res);
	TEST2(err, KErrNone);
	TEST(res == _L("BLAH"));
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Test 7 - only Rec1 satisfies the WHILE condition
	err = stmt.Prepare(db, _L("SELECT Fld1, Fld2 FROM A WHERE Fld1 LIKE 'NOTINTABLE' OR Fld2 LIKE '_LA_'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, res);
	TEST2(err, KErrNone);
	TEST(res == _L("ACDC\\"));
	err = stmt.ColumnText(1, res);
	TEST2(err, KErrNone);
	TEST(res == _L("BLAH"));
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Test 8 - only Rec6 satisfies the WHILE condition
	err = stmt.Prepare(db, _L("SELECT Fld1, Fld2 FROM A WHERE  Fld1 LIKE '%ADC%' AND Fld2 LIKE 'ADC_'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, res);
	TEST2(err, KErrNone);
	TEST(res == _L("ADCDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOPQRSTUVWXYZ123456789ACDEFGHIJKLMNOP"));
	err = stmt.ColumnText(1, res);
	TEST2(err, KErrNone);
	TEST(res == _L("ADCB"));
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Test 9 - only Rec5 satisfies the WHILE condition
	err = stmt.Prepare(db, _L("SELECT Fld1 FROM A WHERE  Fld1 LIKE '%\\%C%' ESCAPE '\\'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, res);
	TEST2(err, KErrNone);
	TEST(res == _L("A%CDEFGH"));
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Test 10 - only Rec7 satisfies the WHILE condition
	err = stmt.Prepare(db, _L("SELECT Fld1 FROM A WHERE  Fld1 LIKE '%D\\\\%' ESCAPE '\\'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, res);
	TEST2(err, KErrNone);
	TEST(res == _L("XZD\\FZX"));
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Test 11 - only Rec4 satisfies the WHILE condition
	err = stmt.Prepare(db, _L("SELECT * FROM A WHERE  Fld1 LIKE '%A\\__\\_C%' ESCAPE '\\'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, res);
	TEST2(err, KErrNone);
	TEST(res == _L("A_%_CDEFGH"));
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Test 12 - only Rec5 satisfies the WHILE condition
	err = stmt.Prepare(db, _L("SELECT * FROM A WHERE  Fld1 LIKE '%A%\\%C%' ESCAPE '\\'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, res);
	TEST2(err, KErrNone);
	TEST(res == _L("A%CDEFGH"));
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Test 13 - only Rec2 satisfies the WHILE condition
	err = stmt.Prepare(db, _L("SELECT * FROM A WHERE  Fld1 LIKE 'ABC%' ESCAPE '\\'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	err = stmt.ColumnText(0, res);
	TEST2(err, KErrNone);
	TEST(res == _L("ABCDEFGH"));
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Test 14 - there is no record satisfying the WHILE condition
	err = stmt.Prepare(db, _L("SELECT * FROM A WHERE  Fld1 LIKE 'A_C' ESCAPE '\\'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Test 15 - there is no record satisfying the WHILE condition
	err = stmt.Prepare(db, _L("SELECT * FROM A WHERE  Fld1 LIKE 'A%C' ESCAPE '\\'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Test 16 - there is no record satisfying the WHILE condition
	err = stmt.Prepare(db, _L("SELECT * FROM A WHERE  Fld1 LIKE '_A\\_C' ESCAPE '\\'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Test 17 - there is no record satisfying the WHILE condition
	err = stmt.Prepare(db, _L("SELECT * FROM A WHERE  Fld1 LIKE '_A\\_C_' ESCAPE '\\'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Test 18 - there is no record satisfying the WHILE condition
	err = stmt.Prepare(db, _L("SELECT * FROM A WHERE  Fld1 LIKE 'ABC' ESCAPE '\\'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Test 19 - there is no record satisfying the WHILE condition
	err = stmt.Prepare(db, _L("SELECT * FROM A WHERE  Fld1 LIKE '%ABC' ESCAPE '\\'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtEnd);
	stmt.Close();
	//Cleanup	
	db.Close();
	RDebug::Print(_L("###Delete test database\r\n"));
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

void DoTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1609 Folding & Collation test 1 "));
	CollationTest1L();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1610 Folding & Collation test 2 "));
	CollationTest2L();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1627 Collation test 3 "));
	CollationTest3L();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1760 LIKE & ESCAPE test 1 "));
	LikeTest1();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1761 LIKE & ESCAPE test 2 "));
	LikeTest2();
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();

	__UHEAP_MARK;
	
	CreateTestDir();
	DeleteTestFiles();
	TRAPD(err, DoTestsL());
	DeleteTestFiles();
	TEST2(err, KErrNone);

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;
	
	User::Heap().Check();
	return KErrNone;
	}
