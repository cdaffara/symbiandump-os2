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

#define __SQLSRV_STRINGS__
#include "SqlSrvStrings.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RFs TheFs;
RSqlDatabase TheDb;
TParse TheFileNameParse;
RTest TheTest(_L("t_sqlauthorizer test"));

const TSecurityPolicy KPolicy1(ECapabilityReadUserData, ECapabilityNetworkControl,  ECapabilityWriteUserData);
const TSecurityPolicy KPolicy2(ECapabilityReadUserData);

_LIT8(KPragma , 		"PRAGMA %S.%S\x0");
_LIT8(KPragmaWithParam, "PRAGMA %S.%S%S\x0");

_LIT(KPublicDir,  "C:\\test\\");
_LIT(KPrivateDir, "C:\\private\\21212129\\");

_LIT(KPublicDbName,	 "C:\\test\\t_sqlauthorizer.db");
_LIT(KSecureDbName,  "C:[21212129]t_sqlauthorizer.db");
_LIT(KPrivateDbName, "C:\\private\\21212129\\t_sqlauthorizer.db");
_LIT(KAttachDb, "AttachedDB");

_LIT(KCreateTable1, "CREATE TABLE A(Id INTEGER,Name TEXT)");
_LIT(KCreateTable2, "CREATE TABLE B(Id INTEGER,Address TEXT, Num INTEGER, FOREIGN KEY (Address) REFERENCES A(id))");
_LIT(KCreateIndex, 	"CREATE INDEX Index1 ON B (Num)");
_LIT(KInsertTable, 	"INSERT INTO A VALUES(1, 'AAA')");

TBuf8<KMaxFileName + 64> TheSql;
const TInt KNumPragma = 34;

//////////////////////////////////////////// Input List  ///////////////////////////////////////////////
struct TInputList
	{
	const TPtrC8 iPragmaName;
	const TPtrC8 iReadParam;
	const TPtrC8 iWriteParam;
	};

const TInputList KInputList [KNumPragma] = 
	{
	////////////////////////////////////////////////////////////////////////////////////////////////
	//	Pragma Name	  	  		 |Read Parameter   |Write Parameter			|Pragma Command    
	////////////////////////////////////////////////////////////////////////////////////////////////
		{KAutoVacuum(), 		  KNullDesC8(), 	_L8(" = 0")}, 			//auto_vacuum
		{KCacheSize(),			  KNullDesC8(), 	_L8(" = 32")},			//cache_size
		{KCaseSensitiveLike(), 	  KNullDesC8(), 	_L8(" = 0")},			//case_sensitive_like
		{KCountChanges(), 	   	  KNullDesC8(), 	_L8(" = 1")},			//count_changes
		{KDefaultCacheSize(), 	  KNullDesC8(), 	_L8(" = 32")},			//DefaultAccess_cache_size	
		{KEmptyResultCallbacks(), KNullDesC8(), 	_L8(" = 0")},			//empty_result_callbacks
		{KEncoding(), 			  KNullDesC8(), 	_L8(" = \"UTF-8\"")},	//encoding
		{KFullColumnNames(), 	  KNullDesC8(), 	_L8(" = 0")},			//full_column_names
		{KFullfsync(), 			  KNullDesC8(), 	_L8(" = 1")},			//fullfsync
		{KIncrementalVacuum(), 	  KNullDesC8(), 	_L8(" = 0")},			//incremental_vacuum
		{KJournalMode(), 		  KNullDesC8(), 	_L8(" = DELETE")},		//journal_mode
		{KJournalSizeLimit(),	  KNullDesC8(), 	_L8(" = 1000000")},		//journal_size_limit
		{KLegacyFileFormat(), 	  KNullDesC8(), 	_L8(" = ON")},			//legacy_file_format
		{KLockingMode(), 		  KNullDesC8(), 	_L8(" = Normal")},		//locking_mode
		{KPageSize(), 			  KNullDesC8(), 	_L8(" = 2048")},		//page_size		
		{KMaxPageCount(), 		  KNullDesC8(), 	_L8(" = 32")},			//max_page_count
		{KReadUncommitted(), 	  KNullDesC8(), 	_L8(" = 1")},			//read_uncommitted
		{KShortColumnNames(), 	  KNullDesC8(), 	_L8(" = 0")},			//short_column_names
		{KSynchronousFlag(), 	  KNullDesC8(), 	_L8(" = NORMAL")}, 		//synchronous
		{KTempStore(), 			  KNullDesC8(), 	_L8(" = MEMORY")},		//temp_store	
		{KTempStoreDirectory(),   KNullDesC8(), 	_L8(" = TEMP")}, 		//temp_store_directory
		{KDatabaseList(), 		  KNullDesC8(), 	KNullDesC8()},			//database_list
		{KForeignKeyList(), 	  _L8("(B)"), 		KNullDesC8()},			//foreign_key_list
		{KFreelistCount(), 		  KNullDesC8(), 	KNullDesC8()},			//freelist_count
		{KIndexInfo(), 			  _L8("(Index1)"), 	KNullDesC8()},			//index_info		
		{KIndexIist(), 			  _L8("(B)"), 		KNullDesC8()},			//index_list
		{KPageCount(), 			  KNullDesC8(), 	KNullDesC8()},			//page_count
		{KTableInfo(), 			  _L8("(B)"), 		KNullDesC8()},			//table_info
		{KSchemaVersion(), 		  KNullDesC8(), 	_L8(" = 10")},			//schema_version
		{KUserVersion(), 		  KNullDesC8(), 	_L8(" = 10")},			//user_version	
		{KIntegrityCheck(), 	  KNullDesC8(), 	_L8(" = 1")},			//integrity_check
		{KParserTrace(), 		  KNullDesC8(), 	_L8(" = ON")},			//parser_trace
		{KVdbeTrace(), 			  KNullDesC8(), 	_L8(" = ON")},			//vdbe_trace
		{KdbeListing(), 		  KNullDesC8(), 	_L8(" = ON")},			//vdbe_listing
	};

//////////////////////////////////////////////// Expected results ///////////////////////////////////////////////////////////

enum TAccessType
	{
	EReadParam,
	EWriteParam,
	};

enum TDatabaseType
	{
	ENonSecure,
	ESecure,
	};

enum TAccessResult
	{
	EOk = KErrNone,
	EIgnore,
	EDeny,
	ENoCheck
	};


// Table listing the expected for each pragma command for the following access conditions:-
// 1) Pragma Read with non-secure database
// 2) Pragma Write with non-secure database
// 3) Pragma Read with secure database
// 4) Pragma Write with secure database
const TInt KResultsTable[KNumPragma][4] = 
	{
		////////////////////////////////////////////////////////////////////////////////////
		//		NON_SECURE	  	|			SECURE		|
		//W/out Pram |With Param|W/out Pram |With Param |Pragma Command 
		///////////////////////////////////////////////////////////////////////////////////
		{EOk,		EIgnore,	EDeny, 		EDeny }, 	//auto_vacuum
		{EOk,		EIgnore,	EDeny,		EDeny }, 	//cache_size
		{EOk,		ENoCheck,	EDeny,		EDeny },	//case_sensitive_like  
		{EOk,		EOk,		EDeny,		EDeny }, 	//count_changes
		{EOk,		EIgnore,	EDeny,		EDeny }, 	//cache_size
		{EOk,		EIgnore,	EDeny,		EDeny }, 	//empty_result_callbacks
		{EOk,		EIgnore,	EDeny,		EDeny },  	//encoding
		{EOk,		EOk,		EDeny,		EDeny },	//full_column_names
		{EOk,		EIgnore,	EDeny,		EDeny },	//fullfsync
		{EIgnore,	EIgnore,	EDeny,		EDeny },	//incremental_vacuum
		{EIgnore,	EIgnore,	EDeny,		EDeny },	//journal_mode
		{EIgnore,	EIgnore,	EDeny,		EDeny }, 	//journal_size_limit
		{EOk,		EIgnore,	EDeny,		EDeny }, 	//legacy_file_format
		{EOk,		EIgnore,	EDeny,		EDeny },	//locking_mode
		{EOk,		EIgnore,	EDeny,		EDeny },	//page_size
		{EIgnore,	EIgnore,	EDeny,		EDeny },	//max_page_count
		{EOk,		EIgnore,	EDeny,		EDeny },	//read_uncommitted
		{EOk,		EOk,		EDeny,		EDeny },	//short_column_names
		{EOk,		EIgnore,	EDeny,		EDeny },	//synchronous
		{EOk,		EIgnore,	EDeny,		EDeny },	//temp_store
		{EOk,		ENoCheck,	EDeny,		EDeny },	//temp_store_directory
		{EOk,		EOk,		EDeny,		EDeny },	//database_list
		{ENoCheck,	EOk,		EDeny,		EDeny },	//foreign_key_list
		{EOk,		EIgnore,	EDeny,		EDeny },	//freelist_count
		{ENoCheck,	EOk,		EDeny,		EDeny },	//index_info
		{ENoCheck,	EOk,		EDeny,		EDeny },	//index_list
		{EOk,		EIgnore,	EDeny,		EDeny },	//page_count
		{ENoCheck,	EOk,		EDeny,		EDeny },	//table_info
		{EOk,		EIgnore,	EDeny,		EDeny },	//schema_version
		{EOk,		EIgnore,	EDeny,		EDeny },	//user_version
		{EOk,		EIgnore,	EDeny,		EDeny },	//integrity_check
		{EOk,		EIgnore,	EDeny,		EDeny },	//parser_trace
		{EOk,		EIgnore,	EDeny,		EDeny },	//vdbe_trace
		{EOk,		EIgnore,	EDeny,		EDeny },	//vdbe_trace
	};

///////////////////////////////////////////////////////////////////////////////////////
//Deletes all created test files.
void DeleteTestFiles()
	{	
	(void)RSqlDatabase::Delete(KPrivateDbName);
	(void)RSqlDatabase::Delete(KSecureDbName);
	(void)RSqlDatabase::Delete(KPublicDbName);
	}

///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteTestFiles();
		RDebug::Print(_L("*** Line %d\r\n"), aLine);
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DeleteTestFiles();
		RDebug::Print(_L("*** Line %d, Expected error: %d, got: %d\r\n"), aLine, aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//Creates file session instance and the test directory
void CreateTestEnv()
    {
	TInt err = TheFs.Connect();
	TEST2(err, KErrNone);

	err = TheFs.MkDir(KPublicDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	
	err = TheFs.MkDir(KPrivateDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//Create and setup all the database test files

//Perform common setup steps for all databases
void SetupTestDb()
	{
	TInt err = TheDb.Exec(KCreateTable1);
	TEST(err >= KErrNone);

	err = TheDb.Exec(KCreateTable2);
	TEST(err >= KErrNone);

	err = TheDb.Exec(KCreateIndex);
	TEST(err >= KErrNone);

	err = TheDb.Exec(KInsertTable);
	TEST(err >= KErrNone);	
	}

void CreateTestDBs()
	{
	RSqlSecurityPolicy policy; //required for secure database
	
	//Create Private Database
	TInt err = TheDb.Create(KPublicDbName);
	TEST2(err, KErrNone);
	
	SetupTestDb();
	
	TheDb.Close();

	//Create Public Database
	err = TheDb.Create(KPrivateDbName);
	TEST2 (err, KErrNone);
	
	SetupTestDb();
	
	TheDb.Close();
	
	//Create Secure Database
	err = policy.Create(TSecurityPolicy(TSecurityPolicy::EAlwaysPass));
	TEST2 (err, KErrNone);
	err = policy.SetDbPolicy(RSqlSecurityPolicy::EReadPolicy, KPolicy1);
	TEST2 (err, KErrNone);
	err = policy.SetDbPolicy(RSqlSecurityPolicy::EWritePolicy, KPolicy2);
	TEST2 (err, KErrNone);
	err = TheDb.Create(KSecureDbName,policy);
	TEST2 (err, KErrNone);
	
	SetupTestDb();
	
	policy.Close();
	TheDb.Close();	
	}

void PragmaTests(const TAccessType aAccessType, TDatabaseType aDbType, TBool aAttached)
	{
	TBuf8<10> databaseName;
	TPtrC8 param;
	TInt index = 0;
	//Depending if we are 
	aAttached ? databaseName.Copy(_L8("AttachedDB")) : databaseName.Copy(_L8("main"));
	
	if(aDbType == ENonSecure)
		{
		index = (aAccessType == EReadParam) ? 0 : 1;
		}
	else
		{
		index = (aAccessType == EReadParam) ? 2 : 3;
		}
	
	for(TInt i=0;i<KNumPragma;i++)
		{
//////////////////////////////////// Formatting the pragma string ////////////////////////////////////////
//Check to see if a parameter value is needed for input
		
		(aAccessType == EReadParam) ? param.Set(KInputList[i].iReadParam) : param.Set(KInputList[i].iWriteParam);
		if (param != KNullDesC8)
			{
			//Formatting Pragma Strings with Parameters
			TheSql.Format(KPragmaWithParam, &databaseName, &KInputList[i].iPragmaName, &param);
			}
		else
			{
			//Formatting Pragma Strings without Parameters
			TheSql.Format(KPragma, &databaseName, &KInputList[i].iPragmaName);
			}

//////////////////// Executing the pragma and checking the results against the "ResultsTable" ////////////
					
			RSqlStatement stmt;
			TInt err = stmt.Prepare(TheDb, TheSql);
			RDebug::Printf("Testing the following pragma command - %s",TheSql.Ptr());
			switch (KResultsTable[i][index])
				{
				case EOk:
				case EIgnore:
					TEST2(err, KErrNone);
					break;
				case EDeny:
					TEST2(err, KErrPermissionDenied);
					break;
				case ENoCheck:
					break;
				default:
					TEST(KErrNone);
				}
			stmt.Close();
		}
	}

//This function will perform the following to test authorizer:-
// 1) Setup the connection - either using a single or attached database
// 2) Test the authorizer using pragma 'writes'
// 3) Test the authorizer using pragma 'reads'  
void PerformPragmaTest(const TDesC16& aDatabaseName, TDatabaseType aDbType, TBool aAttached)
	{
	TInt err = 0;
	
	if(aAttached)
		{
		if (aDatabaseName.Compare(KSecureDbName) == 0)
			{
			TheDb.Open(KPublicDbName);
			TEST2(err, KErrNone);
			}
		else
			{
			TheDb.Open(KSecureDbName);
			TEST2(err, KErrNone);
			}
		
		err = TheDb.Attach(aDatabaseName,KAttachDb);
		TEST2(err, KErrNone);
		}
	else
		{
		err = TheDb.Open(aDatabaseName);
		TEST2(err, KErrNone);
		}
	
	//Testing Pragma Reads
	PragmaTests(EReadParam, aDbType, aAttached);

	//Testing Pragma Writes
	PragmaTests(EWriteParam, aDbType, aAttached);
	
	TheDb.Close();
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4122
@SYMTestCaseDesc		Test for DEF129581: All Pragmas are allowed to be executed on non-secure SQL databases.
						Execute all SQL pragmas to test the permissions returned from the CSqlSrvDatabase::AuthorizeCallback
						function is correct.
						The test attempts to execute pragmas for the following cases:-
						1) Without paramter (also known as 'Read' access)
						2) With parameter (also known as 'Write' access)
						The database tested in this case is a public database. 
@SYMTestPriority		High
@SYMTestActions			Test for DEF129581: All Pragmas are allowed to be executed on non-secure SQL databases.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF129581
*/
void PublicDbPragmaTest()
	{
	PerformPragmaTest(KPublicDbName(), ENonSecure, EFalse);
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4123
@SYMTestCaseDesc		Test for DEF129581: All Pragmas are allowed to be executed on non-secure SQL databases.
						Execute all SQL pragmas to test the permissions returned from the CSqlSrvDatabase::AuthorizeCallback
						function is correct.
						The test attempts to execute pragmas for the following cases:-
						1) Without paramter (also known as 'Read' access)
						2) With parameter (also known as 'Write' access)
						The database tested in this case is a private database. 
@SYMTestPriority		High
@SYMTestActions			Test for DEF129581: All Pragmas are allowed to be executed on non-secure SQL databases.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF129581
*/
void PrivateDbPragmaTest()
	{
	PerformPragmaTest(KPrivateDbName(), ENonSecure, EFalse);
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4124
@SYMTestCaseDesc		Test for DEF129581: All Pragmas are allowed to be executed on non-secure SQL databases.
						Execute all SQL pragmas to test the permissions returned from the CSqlSrvDatabase::AuthorizeCallback
						function is correct.
						The test attempts to execute pragmas for the following cases:-
						1) Without paramter (also known as 'Read' access)
						2) With parameter (also known as 'Write' access)
						The database tested in this case is a secure database. 
@SYMTestPriority		High
@SYMTestActions			Test for DEF129581: All Pragmas are allowed to be executed on non-secure SQL databases.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF129581
*/
void SecureDbPragmaTest()
	{
	PerformPragmaTest(KSecureDbName(), ESecure, EFalse);
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4125
@SYMTestCaseDesc		Test for DEF129581: All Pragmas are allowed to be executed on non-secure SQL databases.
						Execute all SQL pragmas to test the permissions returned from the CSqlSrvDatabase::AuthorizeCallback
						function is correct.
						The test attempts to execute pragmas for the following cases:-
						1) Without paramter (also known as 'Read' access)
						2) With parameter (also known as 'Write' access)
						The database tested in this case is a public attached database.  
@SYMTestPriority		High
@SYMTestActions			Test for DEF129581: All Pragmas are allowed to be executed on non-secure SQL databases.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF129581
*/
void PublicAttDbPragmaTest()
	{
	PerformPragmaTest(KPublicDbName(), ENonSecure, ETrue);
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4126
@SYMTestCaseDesc		Test for DEF129581: All Pragmas are allowed to be executed on non-secure SQL databases.
						Execute all SQL pragmas to test the permissions returned from the CSqlSrvDatabase::AuthorizeCallback
						function is correct.
						The test attempts to execute pragmas for the following cases:-
						1) Without paramter (also known as 'Read' access)
						2) With parameter (also known as 'Write' access)
						The database tested in this case is a private attached database. 
@SYMTestPriority		High
@SYMTestActions			Test for DEF129581: All Pragmas are allowed to be executed on non-secure SQL databases.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF129581
*/
void PrivateAttDbPragmaTest()
	{
	PerformPragmaTest(KPrivateDbName(), ENonSecure, ETrue);
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4127
@SYMTestCaseDesc		Test for DEF129581: All Pragmas are allowed to be executed on non-secure SQL databases.
						Execute all SQL pragmas to test the permissions returned from the CSqlSrvDatabase::AuthorizeCallback
						function is correct.
						The test attempts to execute pragmas for the following cases:-
						1) Without paramter (also known as 'Read' access)
						2) With parameter (also known as 'Write' access)
						The database tested in this case is a secure attached database. 
@SYMTestPriority		High
@SYMTestActions			Test for DEF129581: All Pragmas are allowed to be executed on non-secure SQL databases.
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF129581
*/
void SecureAttDbPragmaTest()
	{
	PerformPragmaTest(KSecureDbName(), ESecure, ETrue);
	}

void DoTests()
	{
	TheTest.Start(_L("@SYMTestCaseID:PDS-SQL-UT-4122 Pragma Testing with public databases"));
	PublicDbPragmaTest();
	
	TheTest.Next(_L("@SYMTestCaseID:PDS-SQL-UT-4123 Pragma Testing with private databases"));
	PrivateDbPragmaTest();
	
	TheTest.Next(_L("@SYMTestCaseID:PDS-SQL-UT-4124 Pragma Testing with secure databases"));
	SecureDbPragmaTest();
	
	TheTest.Next(_L("@SYMTestCaseID:PDS-SQL-UT-4125 Pragma Testing with attached public databases"));
	PublicAttDbPragmaTest();
	
	TheTest.Next(_L("@SYMTestCaseID:PDS-SQL-UT-4126 Pragma Testing with attached private databases"));
	PrivateAttDbPragmaTest();
	
	TheTest.Next(_L("@SYMTestCaseID:PDS-SQL-UT-4127 Pragma Testing with attached secure databases"));
	SecureAttDbPragmaTest();
	}

TInt E32Main()
	{
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	
	__UHEAP_MARK;
	
	CreateTestEnv();
	DeleteTestFiles();
	CreateTestDBs();
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
