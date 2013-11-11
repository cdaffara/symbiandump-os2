// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <s32buf.h>				//MStreamBuf
#include <sqldb.h>

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_sqlapi test"));
_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDbName1, "c:\\test\\t_sqlapi1.db");
_LIT(KTestDbName2, "c:[1111CCCC]t_sqlapi2.db");
_LIT(KTestDbName3, "C:\\TEST\\t_sqlapi3.db");
_LIT(KTestDbName4, "C:[1111CCCC]D012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789.db");
_LIT(KTestDbName5, "C:\\TEST\\D012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789.db");
_LIT(KTestDbName6, "C:[1111CCCC]t_sqlapi6.db");
_LIT(KTestDbName7, "C:[1111CCCC]t_sqlapi7.db");
_LIT(KTestDbName8, "c:\\test\\t_sqlapi8.db");
_LIT(KTestDbName9, "c:\\private\\1111CCCC\\t_sqlapi9.db");
_LIT(KTestCfgDbName, "c:\\test\\t_sqlapi_cfg.db");
_LIT(KTestCfgDbName2, "c:[1111CCCC]t_sqlapi_cfg.db");
_LIT(KServerPrivateDir, "\\private\\10281e17\\");

_LIT(KDbName7, "C:\\TEST\\t_sqlapi7_2.db");

// used for the config test
_LIT8(KServerConfigString1, "   ;  cache_size =  1024 ; page_size =1024 ;encoding =  \"UTF-8\"   ");
_LIT8(KServerConfigString2, " badconfigstring ");
_LIT8(KServerConfigString3, " aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
_LIT8(KServerConfigString4, "");
_LIT8(KServerConfigString5, "dfgdfrgdkfjgjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj43w3wk4jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn");

const TUid KSecureUid = {0x1111CCCC};//The same as the UID in the MMP file

_LIT(KCreateDbScript, "z:\\test\\contacts_schema_to_vendors.sql");
_LIT(KFillDbScript, "z:\\test\\add_simple_contacts.sql");
_LIT8(KCommitStr8, "COMMIT;");
_LIT16(KCommitStr16, "COMMIT;");

///////////////////////////////////////////////////////////////////////////////////////

void DeleteTestFiles()
	{
	RSqlDatabase::Delete(KTestDbName1);
	RSqlDatabase::Delete(KTestDbName2);
	RSqlDatabase::Delete(KTestDbName3);
	RSqlDatabase::Delete(KTestDbName4);
	RSqlDatabase::Delete(KTestDbName5);
	RSqlDatabase::Delete(KTestDbName6);
	RSqlDatabase::Delete(KTestDbName7);
	RSqlDatabase::Delete(KTestDbName8);
	RSqlDatabase::Delete(KTestDbName9);
	RSqlDatabase::Delete(KTestCfgDbName);
	RSqlDatabase::Delete(KTestCfgDbName2);
	RSqlDatabase::Delete(KDbName7);
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

	err = fs.CreatePrivatePath(EDriveC);	
	TEST(err == KErrNone || err == KErrAlreadyExists);
	
	fs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////

template <class DES, class BUF> void ExecSqlStmtOnDb(RSqlDatabase& aDb, const TDesC8& aSqlString, 
												     TInt aExpectedError)
	{
	BUF sqlBuf;
	sqlBuf.Copy(aSqlString);
	
	TInt rc = aDb.Exec(sqlBuf);
	if(rc < 0 && SqlRetCodeClass(rc) == ESqlDbError)
		{
		TPtrC msg = aDb.LastErrorMessage();
		RDebug::Print(_L("Execute SQL error - '%S'\r\n"), &msg);
		}
	if(aExpectedError < 0)
		{
		TEST2(rc, aExpectedError);
		}
	else
		{
		TEST(rc >= 0);
		}
	}

template <class DES, class BUF>  RSqlStatement PrepareSqlStmt(RSqlDatabase& aDb, const TDesC8& aSqlString,
															  TInt aExpectedError)
	{
	BUF sqlBuf;
	sqlBuf.Copy(aSqlString);
	
	RSqlStatement stmt;
	TInt rc = stmt.Prepare(aDb, sqlBuf);
	if(rc != KErrNone && SqlRetCodeClass(rc) == ESqlDbError)
		{
		TPtrC msg = aDb.LastErrorMessage();
		RDebug::Print(_L("Execute SQL error - '%S'\r\n"), &msg);
		}
	TEST2(rc, aExpectedError);
	return stmt;
	}

void ExecSqlStmt(RSqlDatabase& aDb, RSqlStatement& aStmt, TInt aExpectedError)
	{
	TInt rc = aStmt.Exec();
	if(rc < 0 && SqlRetCodeClass(rc) == ESqlDbError)
		{
		TPtrC msg = aDb.LastErrorMessage();
		RDebug::Print(_L("Execute SQL error - '%S'\r\n"), &msg);
		}
	if(aExpectedError < 0)
		{
		TEST2(rc, aExpectedError);
		}
	else
		{
		TEST(rc >= 0);
		}
	}
		
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1601
@SYMTestCaseDesc		Create/Open/Close database tests. Invalid database names, very long database names,
						private databases, public databases, open twice database, create already
						existing database, open non-exisitng database, open corrupted database.
@SYMTestPriority		High
@SYMTestActions			Tests for RSqlDatabase::Create(), RSqlDatabase::Open() methods.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void OpenCloseDatabaseTest()
	{
	RSqlDatabase db;
	TSecurityPolicy defaultPolicy(TSecurityPolicy::EAlwaysPass);
	RSqlSecurityPolicy securityPolicy;
	TInt rc = securityPolicy.Create(defaultPolicy);
	TEST2(rc, KErrNone);
	
	//Secure shared database file on a non-existing drive (A:)
	_LIT(KDbPath1, "A:[1111CCCC]db1.db");
	rc = db.Create(KDbPath1, securityPolicy);
	TEST2(rc, KErrNotReady);
	db.Close();

	//Attempt to open a non-existing file.
	_LIT(KDbFName, "c:\\test\\nofile.db");
	rc = db.Open(KDbFName);
	TEST2(rc, KErrNotFound);
		
	//Zero length database file name.
	rc = db.Create(_L(""));
	TEST2(rc,  KErrBadName);
	db.Close();
	
	//Database file name containing only the drive name
	rc = db.Create(_L("C:"));
	TEST2(rc, KErrBadName);
	db.Close();
	
	//Database file name containing only the path, without the file name
	rc = db.Create(_L("C:\\TEST\\"));
	TEST2(rc, KErrBadName);
	db.Close();
	
	//Public shared database file on a non-existing drive (A:)	
	_LIT(KDbPath2, "A:\\test\\db1.db");
	rc = db.Create(KDbPath2);
	TEST2(rc, KErrNotReady);
	db.Close();

	// create database with good config specified
	rc = db.Create(KTestCfgDbName,&KServerConfigString1);
	TEST2(rc, KErrNone);
	db.Close();
	TInt rc2 = RSqlDatabase::Delete(KTestCfgDbName);
	TEST2(rc2, KErrNone);
	
	// create database with bad config specified
	rc = db.Create(KTestCfgDbName,&KServerConfigString2);
	TEST2(rc, KErrArgument);
	db.Close();

	// create database with long config specified
	rc = db.Create(KTestCfgDbName,&KServerConfigString3);
	TEST2(rc, KErrArgument);
	db.Close();

	// create database with empty config specified - not an error
	rc = db.Create(KTestCfgDbName,&KServerConfigString4);
	TEST2(rc, KErrNone);
	db.Close();
	rc2 = RSqlDatabase::Delete(KTestCfgDbName);
	TEST2(rc2, KErrNone);

    // create database with very long config specified
    rc = db.Create(KTestCfgDbName, &KServerConfigString5);
    TEST2(rc, KErrArgument);
    db.Close();
	
	//Secure shared database file on an existing drive (C:). 
	//Very long database file name (> 90 characters) but still a valid name.
	rc = db.Create(KTestDbName4, securityPolicy);
	db.Close();
	rc2 = RSqlDatabase::Delete(KTestDbName4);
	TEST2(rc, KErrNone);
	TEST2(rc2, KErrNone);

	//Secure shared database file on an existing drive (C:). 
	//Very long database file name (> 90 characters) but still a valid name.
	//With config
	rc = db.Create(KTestCfgDbName2, securityPolicy, &KServerConfigString1);
	db.Close();
	rc2 = RSqlDatabase::Delete(KTestCfgDbName2);
	TEST2(rc, KErrNone);
	TEST2(rc2, KErrNone);
	
	//Public shared database file on an existing drive (C:). 
	//Very long database file name (> 90 characters) but still a valid name.
	rc = db.Create(KTestDbName5);
	db.Close();
	rc2 = RSqlDatabase::Delete(KTestDbName5);
	TEST2(rc, KErrNone);
	TEST2(rc2, KErrNone);
	
	RFs fs;
	TEST2(fs.Connect(), KErrNone);
	TFileName privatePath;
	TEST2(fs.PrivatePath(privatePath), KErrNone);
	
	//Private shared database file on an existing drive (C:). 
	//Very long database file name.
	TBuf<50>filesysname;
	fs.FileSystemName(filesysname,(TInt) EDriveC);
    fs.Close();

	RDebug::Print(_L("file system name = %S"), &filesysname);
	TInt maxFileName = KMaxFileName -40;//"-40" because the SQLITE engine creates a journal file if begins
	                                                 //a transaction. The name of the journal file is 
	                                                 //"<dbFileName>-journal.<ext>". It is obvious that if the 
	                                                 //database file name is too long but still valid and its creation
	                                                 //succeeds, the journal file creation may fail because the journal
	                                                 //file name becomes too long

	if(filesysname.CompareF(_L("HVFS")) == 0)
	    {
        maxFileName = KMaxFileName -150;//The test will panic in PlatSim when the file name is too long. This line should be removed when platsim team fixes the file system defect.
	    }
	HBufC* dbPath = HBufC::New(maxFileName);
	TEST(dbPath != NULL);
	TPtr dbPathPtr = dbPath->Des();
	_LIT(KExt, ".DB");
	dbPathPtr.Copy(_L("C:"));
	dbPathPtr.Append(KSecureUid.Name());
	TInt len = maxFileName + 1 - (dbPathPtr.Length() + KExt().Length() + privatePath.Length());
	
	while(--len)
		{
        dbPathPtr.Append(TChar('A'));	
		}
	dbPathPtr.Append(KExt);	
	TEST(dbPathPtr.Length() == (maxFileName - privatePath.Length()));	
	rc = db.Create(dbPathPtr, securityPolicy);
	TEST2(rc, KErrNone);
	db.Close();
	rc2 = RSqlDatabase::Delete(dbPathPtr);
	TEST2(rc2, KErrNone);
	
	// Private database with config
	TBuf<KMaxFileName> cfgPath;
	cfgPath.Copy(_L("C:"));
	cfgPath.Append(KSecureUid.Name());
	cfgPath.Append(KExt);
	rc = db.Create(cfgPath,securityPolicy,&KServerConfigString1);
	db.Close();
	rc2 = RSqlDatabase::Delete(cfgPath);
	TEST2(rc, KErrNone);
	TEST2(rc2, KErrNone);
	
	//Public shared database file on an existing drive (C:). 
	//Very long database file name.
	dbPathPtr.Copy(_L("C:\\TEST\\D"));
	len = maxFileName + 1 - (dbPathPtr.Length() + KExt().Length());
	while(--len)
		{
        dbPathPtr.Append(TChar('A'));	
		}
	dbPathPtr.Append(KExt);	
	TEST(dbPathPtr.Length() == maxFileName);
	rc = db.Create(dbPathPtr);
	db.Close();
	rc2 = RSqlDatabase::Delete(dbPathPtr);
	
	delete dbPath;

	TEST2(rc, KErrNone);
	TEST2(rc2, KErrNone);

	//Create/Close/Open/Close secure shared database test
	rc = db.Create(KTestDbName6, securityPolicy);
	db.Close();
	rc2 = db.Open(KTestDbName6);
	db.Close();
	TInt rc3 = RSqlDatabase::Delete(KTestDbName6);
	TEST2(rc, KErrNone);
	TEST2(rc2, KErrNone);
	TEST2(rc3, KErrNone);
	
	//An attempt to create already existing secure shared file.	
	rc = db.Create(KTestDbName6, securityPolicy);
	db.Close();
	rc2 = db.Create(KTestDbName6, securityPolicy);
	db.Close();
	rc3 = RSqlDatabase::Delete(KTestDbName6);
	TEST2(rc, KErrNone);
	TEST2(rc2, KErrAlreadyExists);
	TEST2(rc3, KErrNone);
	
	//An attempt to open twice the same database file using different RSqlDatabase objects
	rc = db.Create(KTestDbName6, securityPolicy);
	RSqlDatabase db2;	
	rc2 = db2.Open(KTestDbName6);
	db2.Close();
	db.Close();
	rc3 = RSqlDatabase::Delete(KTestDbName6);
	TEST2(rc, KErrNone);
	TEST2(rc2, KErrNone);//-- KErrInUse -- in case EFileRead | EFileWrite file open mode!
	TEST2(rc3, KErrNone);

	//An attempt to create secure shared database file on a read-only drive (Z:)
	_LIT(KDbPath8, "Z:[1111CCCC]db1.db");
	rc = db.Create(KDbPath8, securityPolicy);
	TEST2(rc, KErrAccessDenied);
	db.Close();

	//An attempt to create non-secure shared database file on a read-only drive (Z:)
	_LIT(KDbPath8a, "Z:\\db1.db");
	rc = db.Create(KDbPath8a);
	TEST2(rc, KErrAccessDenied);
	db.Close();

	//An attempt to open non-existing secure shared database file on a read-only drive (Z:)
	rc = db.Open(KDbPath8);
	TEST(rc == KErrNotFound || rc == KErrPathNotFound);
	db.Close();

	//An attempt to open existing public shared database file on a read-only drive (Z:)
	_LIT(KDbPath9, "Z:\\TEST\\TestDb1.db");
	rc = db.Open(KDbPath9);
	TEST2(rc, KErrNone);
	db.Close();
	
	//An attempt to open corrupted public shared database file on a read-only drive (Z:)
	_LIT(KDbPath10, "Z:\\TEST\\CorruptDb.db");
	rc = db.Open(KDbPath10);
	// it will be KErrNotDb if SqlServer.cfg exists, else KErrNone if it doesn't
	// this is because we can detect a corrupt database when we attempt to
	// set the configuration. If there is no cfg file, then there will be no
	// attempt to set the pragmas and so the corrupt file is undetected
	TEST(rc==KSqlErrNotDb || rc==KErrNone);
	db.Close();

	//An attempt to open database with name containing non-convertible characters.
    TBuf<6> invName;
    invName.SetLength(6);
    invName[0] = TChar('c'); 
    invName[1] = TChar(':'); 
    invName[2] = TChar('\\'); 
    invName[3] = TChar(0xD800); 
    invName[4] = TChar(0xFC00); 
    invName[5] = TChar(0x0000);
	rc = db.Open(invName);
	db.Close();
	TEST(rc != KErrNone);

	//Copy the corrupted database file on drive C:
	TEST2(fs.Connect(), KErrNone);
	rc = BaflUtils::CopyFile(fs, KDbPath10, KTestDbName3);
	TEST2(rc, KErrNone);
	(void)fs.SetAtt(KTestDbName3, 0, KEntryAttReadOnly);
	fs.Close();

	//An attempt to open corrupted public shared database file on a drive (C:)
	rc = db.Open(KTestDbName3);
	TEST(rc == KSqlErrNotDb || rc == KErrNone);//Note: but it may be a different error code as well
	db.Close();
	(void)RSqlDatabase::Delete(KTestDbName3);
	
	//Create, Close, Open, Close and again Open database test
	rc = db.Create(KTestDbName2, securityPolicy);
	TEST2(rc, KErrNone);
	db.Close();
	rc = db.Open(KTestDbName2);
	TEST2(rc, KErrNone);
	db.Close();
	rc = db.Open(KTestDbName2);
	TEST2(rc, KErrNone);
	db.Close();

	//Open two connections to the same database.
	rc = db.Open(KTestDbName2);
	TEST2(rc, KErrNone);
	rc = db2.Open(KTestDbName2);
	TEST2(rc, KErrNone);
	db2.Close();
	db.Close();
	
	rc = RSqlDatabase::Delete(KTestDbName2);
	TEST2(rc, KErrNone);
	
	securityPolicy.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1602
@SYMTestCaseDesc		Setting database isolation level tests.
@SYMTestPriority		High
@SYMTestActions			Tests for RSqlDatabase::SetIsolationLevel() method.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void SetIsolationLevelTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName1);

	RSqlDatabase db;		
	TInt err = db.Create(KTestDbName1);
	TEST2(err, KErrNone);

	err = db.SetIsolationLevel(RSqlDatabase::EReadCommitted);
	TEST2(err, KErrNotSupported);
	
	err = db.SetIsolationLevel(RSqlDatabase::ERepeatableRead);
	TEST2(err, KErrNotSupported);
	
	err = db.SetIsolationLevel(RSqlDatabase::EReadUncommitted);
	TEST2(err, KErrNone);
	
	err = db.SetIsolationLevel(RSqlDatabase::ESerializable);
	TEST2(err, KErrNone);
	
	db.Close();
	
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1603
@SYMTestCaseDesc		Deleting database tests. Deleting non-existing database, opened database, 
						database on non-existing drive, zero-length database name, only path (no file name),
						rom drive based database.
@SYMTestPriority		High
@SYMTestActions			Tests for RSqlDatabase::Delete() method.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void DeleteDatabaseTest()
	{
	//An attempt to delete non-existing secure shared database
	_LIT(KDbName1, "C:[1111CCCC]EE__900000.adb");
	TInt err = RSqlDatabase::Delete(KDbName1);
	TEST2(err, KErrNotFound);

	//An attempt to delete non-existing public database
	_LIT(KDbName2, "C:\\TEST\\__900000.adb");
	err = RSqlDatabase::Delete(KDbName2);
	TEST2(err, KErrNotFound);
	
	//Zero length database file name.
	_LIT(KDbName3, "");
	err = RSqlDatabase::Delete(KDbName3);
	TEST2(err,  KErrBadName);
	
	//Database file name containing only the drive name
	_LIT(KDbName4, "C:");
	err = RSqlDatabase::Delete(KDbName4);
	TEST2(err, KErrBadName);
	
	//Database file name containing only the path, without the file name
	_LIT(KDbName5, "C:");
	err = RSqlDatabase::Delete(KDbName5);
	TEST2(err, KErrBadName);
	
	//Public shared database file on a non-existing drive (A:)	
	_LIT(KDbName6, "A:\\test\\db1.db");
	err = RSqlDatabase::Delete(KDbName6);
	TEST2(err, KErrNotReady);

	//An attempt to delete opened database.
	RSqlDatabase db;	
	err = db.Create(KDbName7);
	TEST2(err, KErrNone);
	err = RSqlDatabase::Delete(KDbName7);
	TEST2(err, KErrInUse);
	db.Close();
	err = RSqlDatabase::Delete(KDbName7);
	TEST2(err, KErrNone);

	//An attempt to delete existing public shared database file on a read-only drive (Z:)
	_LIT(KDbName8, "Z:\\TEST\\TestDb1.db");
	err = RSqlDatabase::Delete(KDbName8);
	TEST2(err, KErrAccessDenied);
	
	//Create secure database
	TSecurityPolicy defaultPolicy(TSecurityPolicy::EAlwaysPass);
	RSqlSecurityPolicy securityPolicy;
	err = securityPolicy.Create(defaultPolicy);
	TEST2(err, KErrNone);
	err = db.Create(KTestDbName7, securityPolicy);
	TEST2(err, KErrNone);
	db.Close();
	securityPolicy.Close();
	
	//Attempt to delete a secure public database.
	//The calling application has no rights to delete a file with that name from the server's 
	//private data cage.
   	err = RSqlDatabase::Delete(_L("C:[45454545]qq.db"));
	TEST2(err, KErrPermissionDenied);

	//Attempt to delete a secure public database. No drive specified.
	//The calling application has no rights to delete a file with that name from the server's 
	//private data cage.
	err = RSqlDatabase::Delete(_L("[45454545]qq.db"));
	TEST2(err, KErrPermissionDenied);

	//Attempt to delete secure database specifying the full database path
	TParse parse;
	parse.Set(KTestDbName7, &KServerPrivateDir(), 0);
	err = RSqlDatabase::Delete(parse.FullName());
	TEST2(err, KErrArgument);

	//Attempt to delete secure database specifying only the database name
	err = RSqlDatabase::Delete(parse.NameAndExt());
	//If C: is the system drive then the operation must pass.
	TEST2(err, KErrNone);
	}

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1640
@SYMTestCaseDesc		Copying database tests. Copying:
						- non-secure to non-secure database;
						- non-secure to secure database;
						- secure to non-secure database;
						- secure to secure database;
						- secure database, when the application is not the database creator (owner);
@SYMTestPriority		High
@SYMTestActions			Tests for RSqlDatabase::Copy() method.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void CopyDatabaseTest()
	{
	RSqlDatabase db;	

	//Create secure database
	TSecurityPolicy defaultPolicy(TSecurityPolicy::EAlwaysPass);
	RSqlSecurityPolicy securityPolicy;
	TInt err = securityPolicy.Create(defaultPolicy);
	TEST2(err, KErrNone);
	err = db.Create(KTestDbName7, securityPolicy);
	TEST2(err, KErrNone);
	db.Close();
	securityPolicy.Close();
	
	//Create non-secure database
	err = db.Create(KTestDbName1);
	TEST2(err, KErrNone);
	db.Close();

	//Copy non-secure to non-secure database	
	err = RSqlDatabase::Copy(KTestDbName1, KTestDbName8);
	TEST2(err, KErrNone);

	//Attempt to copy non-secure to secure database	
	err = RSqlDatabase::Copy(KTestDbName1, _L("C:[99999999]pkk.db"));
	TEST2(err, KErrPermissionDenied);

	//Attempt to copy secure to non-secure database	
	err = RSqlDatabase::Copy(KTestDbName7, _L("C:\\test\\asdf.db"));
	TEST2(err, KErrPermissionDenied);

	//Copy secure to secure database. The test application is the database owner.
	err = RSqlDatabase::Copy(KTestDbName7, KTestDbName4);
	TEST2(err, KErrNone);
	err = RSqlDatabase::Delete(KTestDbName4);
	TEST2(err, KErrNone);

	//Attempt to copy secure to secure database. The test application is not the database owner.
	err = RSqlDatabase::Copy(KTestDbName7, _L("C:[11111111]ff.db"));
	TEST2(err, KErrPermissionDenied);
	err = RSqlDatabase::Copy(_L("C:[11111111]ff.db"), _L("C:[22222222]ff.db"));
	TEST2(err, KErrPermissionDenied);
	err = RSqlDatabase::Copy(_L("C:[11111111]ff.db"), KTestDbName7);
	TEST2(err, KErrPermissionDenied);
	
	(void)RSqlDatabase::Delete(KTestDbName8);
	(void)RSqlDatabase::Delete(KTestDbName7);
	(void)RSqlDatabase::Delete(KTestDbName4);
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1604
@SYMTestCaseDesc		Create a table with two integer columns. The second column has a default value NULL.
						Check what will be the result of "column1 + column2" operation, if "column2" 
						value is NULL.
@SYMTestPriority		High
@SYMTestActions			Tests mathematical operations with ESqlNull column values.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void NullColumnValues()
	{
	(void)RSqlDatabase::Delete(KTestDbName1);
	
	RSqlDatabase db;		
	TInt err = db.Create(KTestDbName1);
	TEST2(err, KErrNone);

	err = db.Exec(_L8("CREATE TABLE test(int_fld integer, null_int_fld integer default null)"));
	TEST(err >= 0);

	err = db.Exec(_L8("INSERT INTO test(int_fld) values(200)"));
	TEST2(err, 1);

	RSqlStatement stmt;
	
	err = stmt.Prepare(db, _L8("SELECT * from test"));
	TEST2(err, KErrNone);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);

	TSqlColumnType colType = stmt.ColumnType(0);
	TEST(colType == ESqlInt);
	
	colType = stmt.ColumnType(1);
	TEST(colType == ESqlNull);
	
	TInt val = stmt.ColumnInt(0);
	TEST(val == 200);
	
	val = stmt.ColumnInt(1);
	TEST(val == 0);

	stmt.Close();
	
	err = stmt.Prepare(db, _L8("SELECT (int_fld + null_int_fld) as res from test"));
	TEST2(err, KErrNone);
	
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	
	colType = stmt.ColumnType(0);
	TEST(colType == ESqlNull);
	
	val = stmt.ColumnInt(0);
	TEST(val == 0);
	
	stmt.Close();
	db.Close();

	(void)RSqlDatabase::Delete(KTestDbName1);
	}

///////////////////////////////////////////////////////////////////////////////////////

_LIT8(KStmt1, "CREATE TABLE A1(Fld1 INTEGER, Fld2 DOUBLE);\
									   CREATE TABLE A2(Fld1 INTEGER, Fld2 DOUBLE);\
									   CREATE TRIGGER TrgA1Ins BEFORE Insert ON A1\
									   BEGIN\
									      INSERT INTO A2(Fld1, Fld2) VALUES(new.Fld1, new.Fld2);\
									   END;");
_LIT8(KStmt2, "INSERT INTO A1(Fld1, Fld2) VALUES(1, 2.0)");
_LIT8(KStmt3, "SELECT * FROM A2");
_LIT8(KStmt4, "INSERT INTO A1(Fld1, Fld2) VALUES(2, 4.0); UPDATE A2 SET Fld2 = 11.3 WHERE Fld1 = 2");
_LIT8(KStmt5, "");
_LIT8(KStmt6, "INSERT INTO A1(Fld1, Fld2) VALUESa(6, 234.0);");
_LIT8(KStmt7, "");
_LIT8(KStmt8, ";;;;;");
_LIT8(KStmt9, "INSERT INTO A1(Fld1, Fld2) VALUES(:v1, :v2)");

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1605
@SYMTestCaseDesc		SQL statements execution. Valid SQL statements. Empty SQL statement. 
						More than one SQL statements, separated with ";". SQL statement which syntax is
						incorrect. SQL statement with parameters. INSERT/SELECT/CREATE TABLE SQL statements.
@SYMTestPriority		High
@SYMTestActions			RSqlDatabase::Exec() tests, 16-bit and 8-bit SQL statements
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
template <class DES, class BUF> void ExecOnDbTest()
	{
	RSqlDatabase db;
	TInt rc = db.Create(KTestDbName1);
	TEST2(rc, KErrNone);

	//Create two tables and a trigger	              	  
	ExecSqlStmtOnDb<DES, BUF>(db, KStmt1, KErrNone); 

	//INSERT sql statement execution
	ExecSqlStmtOnDb<DES, BUF>(db, KStmt2, KErrNone); 

	//SELECT sql statement execution
	ExecSqlStmtOnDb<DES, BUF>(db, KStmt3, KErrNone); 

	//Executing more than one SQL statement in a single Exec() call.
	ExecSqlStmtOnDb<DES, BUF>(db, KStmt4, KErrNone); 

	//Executing zero length SQL statement.
	ExecSqlStmtOnDb<DES, BUF>(db, KStmt5, KErrNone); 
	
	//Executing a SQL statement with syntax errors.
	ExecSqlStmtOnDb<DES, BUF>(db, KStmt6, KSqlErrGeneral); 

	//Executing an empty SQL statement.
	ExecSqlStmtOnDb<DES, BUF>(db, KStmt7, KErrNone); 
	
	//Executing a SQL string, which does not have any SQL statements inside, but has valid syntax.
	ExecSqlStmtOnDb<DES, BUF>(db, KStmt8, KErrNone); 
	
	//Executing SQL statement with parameters. They will be set with NULL values, if not set explicitly.
	ExecSqlStmtOnDb<DES, BUF>(db, KStmt9, KErrNone); 
	
	db.Close();

	rc = RSqlDatabase::Delete(KTestDbName1);
	TEST2(rc, KErrNone);
	}

_LIT8(KStmt10, "");
_LIT8(KStmt11, ";   ;  ;;;");
_LIT8(KStmt12, "CREATE TABLE AAA(Fld1 INTEGER, Fld2 VARCHAR(100))");
_LIT8(KStmt13, "INSERT INTO AAA(Fld1, Fld2) VALUES(5, 'FldVal1-1234567890')");
_LIT8(KStmt14, "SELECT fld1, fld2 FROM AAA");
_LIT8(KStmt15, "SELECT fld2, fld1 FROM AAA");
_LIT8(KStmt16, "SELECT fld2, fld1 FROM AAA WHERE Fld1 > :Prm1 AND fld2 = :PRM2");
_LIT8(KStmt17, "INSERT INTO AAA(Fld1, Fld2) VALUES(:b, :a);");
_LIT8(KStmt18, "CREATE TABLE BBB(Fld1 INTEGER, Fld2 BIGINT, Fld3 DOUBLE, \
					                       Fld4 TEXT, Fld5 LONGBLOB, Fld6 TEXT NULL)");
_LIT8(KStmt19, "INSERT INTO BBB(Fld1, Fld2, Fld3, Fld4, Fld5, Fld6)\
	                                       VALUES(:V1, :V2, :V3, :V4, :V5, :V6)");
_LIT8(KStmt20, "SELECT * FROM BBB");
_LIT8(KStmt21, "SELECT fld1, fld2 FROM AAA;SELECT fld1, fld2 FROM AAA");

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1606
@SYMTestCaseDesc		Preparing SQL statements. Moving to the next record. Retrieving and verifying 
						the column types and values. Binding parameter values.
@SYMTestPriority		High
@SYMTestActions			RSqlStatement::Prepare(), RSqlStatement::Next() tests, 16-bit and 8-bit SQL statements.
						RSqlStatement::ColumnIndex(), RSqlStatement::Column<DataType>(), RSqlStatement::Bind<DataType>(),
						RSqlStatement::Column<DataType>().				
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
template <class DES, class BUF> void StatementTest()
	{
	RSqlDatabase db;	
	TInt rc = db.Create(KTestDbName1);
	TEST2(rc, KErrNone);

	//Executing an empty SQL statement.
	RSqlStatement stmt = PrepareSqlStmt<DES, BUF>(db, KStmt10, KErrArgument);
	stmt.Close();

	//Executing a SQL string, which does not have any SQL statements inside, but has valid syntax.
	stmt = PrepareSqlStmt<DES, BUF>(db, KStmt11, KErrArgument); 
	stmt.Close();
	
	//SQL statement without parameters. Create a table.
	stmt = PrepareSqlStmt<DES, BUF>(db, KStmt12, KErrNone);
	ExecSqlStmt(db, stmt, KErrNone);
	stmt.Close();

	//String containg more than one SQL statement.
	stmt = PrepareSqlStmt<DES, BUF>(db, KStmt21, KErrArgument); 
	stmt.Close();
	
	//SQL statement without parameters. Insert a record into the table.
	stmt = PrepareSqlStmt<DES, BUF>(db, KStmt13, KErrNone);
	ExecSqlStmt(db, stmt, KErrNone);
	stmt.Close();
		
	//Test RSqlStatement::ColumnIndex().
	stmt = PrepareSqlStmt<DES, BUF>(db, KStmt14, KErrNone);
	TInt idx1 = stmt.ColumnIndex(_L("FLD1"));
	TEST(idx1 == 0);
	TInt idx2 = stmt.ColumnIndex(_L("FlD2"));
	TEST(idx2 == 1);
	TInt idx3 = stmt.ColumnIndex(_L("fld3"));
	TEST(idx3 < 0);
	stmt.Close();
	
	stmt = PrepareSqlStmt<DES, BUF>(db, KStmt15, KErrNone);
	idx1 = stmt.ColumnIndex(_L("FLD1"));
	TEST(idx1 == 1);
	idx2 = stmt.ColumnIndex(_L("FlD2"));
	TEST(idx2 == 0);
	idx3 = stmt.ColumnIndex(_L("fld3"));
	TEST(idx3 < 0);
	
	//Test RSqlStatement::Column<DataType>() methods.
	TInt recCnt = 0;
	while((rc = stmt.Next()) == KSqlAtRow)
		{
		++recCnt;
		
		TInt intVal = stmt.ColumnInt(idx1);
		TEST(intVal == 5);
		
		//Integer column value retrieved as a text.
		TPtrC strVal1;
		TInt err = stmt.ColumnText(idx1, strVal1);
		TEST2(err, KErrNone);
		TEST(strVal1 == KNullDesC);
		
		//Text column value retrieved as a binary
		TBuf8<50> strVal3;
		err = stmt.ColumnBinary(idx2, strVal3);
		TEST2(err, KErrNone);
		TEST(strVal3 == KNullDesC8);
		
		TPtrC strVal2;
		err = stmt.ColumnText(idx2, strVal2);
		TEST2(err, KErrNone);
		TEST(strVal2 == _L("FldVal1-1234567890"));
				
		//Invalid column index. Panic: "SqlDb 5"
		//intVal = stmt.ColumnInt(1002);
		//intVal = stmt.ColumnInt(-24);
		}
	stmt.Close();
	TEST2(rc, KSqlAtEnd);
	TEST2(SqlRetCodeClass(rc), ESqlInformation);
	TEST(recCnt == 1);
	
	//Test RSqlStatement::Bind<DataType>() methods.
	stmt = PrepareSqlStmt<DES, BUF>(db, KStmt16, KErrNone);
	idx1 = stmt.ColumnIndex(_L("FLD1"));
	TEST(idx1 == 1);
	idx2 = stmt.ColumnIndex(_L("FlD2"));
	TEST(idx2 == 0);
	TInt prmIdx1 = stmt.ParameterIndex(_L(":prm1"));
	TEST(prmIdx1 == 0);
	TInt prmIdx2 = stmt.ParameterIndex(_L(":prm2"));
	TEST(prmIdx2 == 1);
	rc = stmt.BindInt(prmIdx1, -4);
	//No problem to bind whatever value type we like
	//rc = stmt.BindBinary(prmIdx1, KSqlStmt4());
	TEST2(rc, KErrNone);
	rc = stmt.BindText(prmIdx2, _L("FldVal1-1234567890"));
	TEST2(rc, KErrNone);
	//Test RSqlStatement::Column<DataType>() methods.
	recCnt = 0;
	while((rc = stmt.Next()) == KSqlAtRow)
		{
		++recCnt;
		TInt intVal = stmt.ColumnInt(idx1);
		TEST(intVal == 5);
		TPtrC strVal;
		TInt err = stmt.ColumnText(idx2, strVal);
		TEST2(err, KErrNone);
		TEST(strVal == _L("FldVal1-1234567890"));
		}
	stmt.Close();
	TEST2(rc, KSqlAtEnd);
	TEST2(SqlRetCodeClass(rc), ESqlInformation);
	TEST(recCnt == 1);
	
	//Test an INSERT SQL - prepare, bind, exec.
	stmt = PrepareSqlStmt<DES, BUF>(db, KStmt17, KErrNone);
	prmIdx1 = stmt.ParameterIndex(_L(":A"));
	TEST(prmIdx1 == 1);
	prmIdx2 = stmt.ParameterIndex(_L(":B"));
	TEST(prmIdx2 == 0);
	rc = stmt.BindInt(prmIdx1, 20);
	TEST2(rc, KErrNone);
	rc = stmt.BindText(prmIdx2, _L("FldVal2"));
	TEST2(rc, KErrNone);
	rc = stmt.Exec();
	TEST2(rc, 1);
	
	stmt.Close();
	
	//Create a table with INTEGER, INT64, REAL, TEXT, BINARY, NULL, field types
	stmt = PrepareSqlStmt<DES, BUF>(db, KStmt18, KErrNone);
	ExecSqlStmt(db, stmt, KErrNone);
	stmt.Close();
	
	enum {KLow = 1, KHigh = 2};
	//Insert (KHigh - KLow + 1) records
	stmt = PrepareSqlStmt<DES, BUF>(db, KStmt19, KErrNone);
	TInt v32 = 1024;
	TInt64 v64 = MAKE_TINT64(0x00FF00FF, 0x12345678);
	TReal vReal = 234.75;
	TBuf<10> vText;
	TBuf8<10> vBinary;
	for(TInt i=KLow;i<=KHigh;++i)
		{
		rc = stmt.BindInt(0, v32 * i);
		TEST2(rc, KErrNone);
		
		rc = stmt.BindInt64(1, v64 * i);
		TEST2(rc, KErrNone);
		
		rc = stmt.BindReal(2, vReal * i);
		TEST2(rc, KErrNone);
		
		vText.Copy(_L("TEXT"));
		vText.Append(TChar(i + '0'));
		rc = stmt.BindText(3, vText);
		TEST2(rc, KErrNone);

		vBinary.Copy(_L("BINARY"));
		vBinary.Append(TChar(i + '0'));
		rc = stmt.BindBinary(4, vBinary);
		TEST2(rc, KErrNone);

		rc = stmt.BindNull(5);
		TEST2(rc, KErrNone);

		rc = stmt.Exec();
		TEST2(rc, 1);

		rc = stmt.Reset();
		TEST2(rc, KErrNone);
		}
	stmt.Close();

	//Read and test (KHigh - KLow + 1) records
	stmt = PrepareSqlStmt<DES, BUF>(db, KStmt20, KErrNone);
	recCnt = 0;
	while((rc = stmt.Next()) == KSqlAtRow)
		{
		++recCnt;
		TInt v1 = stmt.ColumnInt(0);
		TEST(v1 == v32 * recCnt);

		TInt64 v2 = stmt.ColumnInt64(1);
		TEST(v2 == v64 * recCnt);

		TReal v3 = stmt.ColumnReal(2);
		TEST(Abs(v3 - vReal * recCnt) < 0.000001);
		
		vText.Copy(_L("TEXT"));
		vText.Append(TChar(recCnt + '0'));
		TPtrC v4;
		TInt err = stmt.ColumnText(3, v4);
		TEST2(err, KErrNone);
		TEST(v4 == vText);
		
		vBinary.Copy(_L("BINARY"));
		vBinary.Append(TChar(recCnt + '0'));
		TPtrC8 v5;
		err = stmt.ColumnBinary(4, v5);
		TEST2(err, KErrNone);
		TEST(v5 == vBinary);
		
		TBool b = stmt.IsNull(5);
		TEST(b != 0);
		}
	stmt.Close();
	TEST2(rc, KSqlAtEnd);
	TEST2(SqlRetCodeClass(rc), ESqlInformation);
	TEST(recCnt == (KHigh - KLow + 1));
	
	db.Close();

	rc = RSqlDatabase::Delete(KTestDbName1);
	TEST2(rc, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1607
@SYMTestCaseDesc		Retrieving long text column values test.
						Retrieving long column values using RSqlStatement::ColumnText() when the recipient buffer
						is not big enough.
@SYMTestPriority		High
@SYMTestActions			RSqlColumnReadStream::ColumnText() test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void ColumnTextStreamTest()
	{
	RSqlDatabase db;	
	TInt rc = db.Create(KTestDbName1);
	TEST2(rc, KErrNone);

	enum {KSqlBufSize = 64};
	
	//Create a table
	_LIT8(KSqlStmt1, "CREATE TABLE A(Fld1 INTEGER, Fld2 TEXT);");
	ExecSqlStmtOnDb<TDesC8, TBuf8<KSqlBufSize> >(db, KSqlStmt1(), KErrNone); 
	
	const TInt KTextLen = 3101;

	_LIT(KSqlStmt2, "INSERT INTO A(Fld1, Fld2) VALUES(");
	
	//Allocate a buffer for the SQL statement.
	HBufC* buf = HBufC::New(KSqlStmt2().Length() + KTextLen + 10);
	TEST(buf != NULL);

	TPtr sql = buf->Des();
	
	//Insert row 1
	
	const TChar KChar1('A');
	sql.Copy(KSqlStmt2);
	sql.Append(_L("1, '"));
	TInt i;
	for(i=0;i<KTextLen;++i)
		{
		sql.Append(KChar1);
		}
	sql.Append(_L("')"));

	rc = db.Exec(sql);
	TEST2(rc, 1);

	//Insert row 2
	
	const TChar KChar2('B');
	sql.Copy(KSqlStmt2);
	sql.Append(_L("2, '"));
	for(i=0;i<KTextLen;++i)
		{
		sql.Append(KChar2);
		}
	sql.Append(_L("')"));

	rc = db.Exec(sql);
	TEST2(rc, 1);

	//Prepare SELECT SQL statement
	_LIT8(KSqlStmt3, "SELECT * FROM A");
	RSqlStatement stmt = PrepareSqlStmt<TDesC, TBuf<KSqlBufSize> >(db, KSqlStmt3, KErrNone);

	//Move on row 1
	rc = stmt.Next();
	TEST2(rc, KSqlAtRow);

	//An attempt to read integer column using binary stream
	RSqlColumnReadStream strm2;
	rc = strm2.ColumnBinary(stmt, 0);
	strm2.Close();
	TEST2(rc, KErrArgument);

	//An attempt to read integer column using text stream
	rc = strm2.ColumnText(stmt, 0);
	strm2.Close();
	TEST2(rc, KErrArgument);
	
	//Read the long text column using a stream
	RSqlColumnReadStream columnStream;
	rc = columnStream.ColumnText(stmt, 1);
	columnStream.Close();
	TEST2(rc, KErrNone);

	//...and the leaving version of ColumnText() 
	TRAP(rc, columnStream.ColumnTextL(stmt, 1));
	TEST2(rc, KErrNone);
	
	TInt size = stmt.ColumnSize(1);
	TPtr colData = buf->Des();
	TRAP(rc, columnStream.ReadL(colData, size));
	columnStream.Close();
	TEST2(rc, KErrNone);
	
	//Check the column value
	for(i=0;i<KTextLen;++i)
		{
		TEST(colData[i] == (TUint8)KChar1);	
		}

	//Move on row 2
	rc = stmt.Next();
	TEST2(rc, KSqlAtRow);

	//Read row 2 using ColumnText(TInt aColumnIndex, TPtrC& aPtr).
	TPtrC colDataPtr;
	rc = stmt.ColumnText(1, colDataPtr);
	TEST2(rc, KErrNone);
	
	//Check the column value
	for(i=0;i<KTextLen;++i)
		{
		TEST(colDataPtr[i] == (TUint8)KChar2);	
		}

	//Read row 2 using ColumnText(TInt aColumnIndex, TDes& aDest).
	rc = stmt.ColumnText(1, colData);
	TEST2(rc, KErrNone);
	//Too small target buffer
	TBuf<3> buf1;
    rc = stmt.ColumnText(1, buf1);
    TEST2(rc, KErrOverflow);
	
	//Check the column value
	for(i=0;i<KTextLen;++i)
		{
		TEST(colData[i] == (TUint8)KChar2);	
		}
	
	//Read row 2 using a stream
	colData.Zero();
	rc = columnStream.ColumnText(stmt, 1);
	TEST2(rc, KErrNone);
	size = stmt.ColumnSize(1);
	TRAP(rc, columnStream.ReadL(colData, size));
	columnStream.Close();
	TEST2(rc, KErrNone);
	
	//Check the column value
	for(i=0;i<KTextLen;++i)
		{
		TEST(colData[i] == (TUint8)KChar2);	
		}
	
	//Read the column value using RSqlStatement::ColumnText(). 
	//The recipient buffer max length is smaller than the column value length.
	rc = stmt.Reset();
	TEST2(rc, KErrNone);
	rc = stmt.Next();
	TEST2(rc, KSqlAtRow);
	const TInt KBufMaxLen = 100;
	TBuf<KBufMaxLen> colBuf;
	rc = stmt.ColumnText(1, colBuf);
	TEST2(rc, KErrOverflow);
	//Check the column value
	for(i=0;i<KBufMaxLen;++i)
		{
		TEST(colBuf[i] == (TUint8)KChar1);	
		}
		
	stmt.Close();
	
	//Deallocate buf
	delete buf; 
	buf = NULL;
	
	db.Close();

	rc = RSqlDatabase::Delete(KTestDbName1);
	TEST2(rc, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1621
@SYMTestCaseDesc		Retrieving long binary column values test.
						Retrieving long column values using RSqlStatement::ColumnBinary() when the recipient buffer
						is not big enough.
@SYMTestPriority		High
@SYMTestActions			RSqlColumnReadStream::ColumnBinary() test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void ColumnBinaryStreamTest()
	{
	RSqlDatabase db;	
	TInt rc = db.Create(KTestDbName1);
	TEST2(rc, KErrNone);

	enum {KSqlBufSize = 64};
	
	//Create a table
	_LIT8(KSqlStmt1, "CREATE TABLE A(Fld1 INTEGER, Fld2 BLOB);");
	ExecSqlStmtOnDb<TDesC8, TBuf8<KSqlBufSize> >(db, KSqlStmt1(), KErrNone); 
	
	const TInt KDataLen = 3311;

	_LIT8(KSqlStmt2, "INSERT INTO A(Fld1, Fld2) VALUES(");
	
	//Allocate a buffer for the SQL statement.
	HBufC8* buf = HBufC8::New(KSqlStmt2().Length() + KDataLen * 2 + 10);//"* 2" - SQL statement with HEX values
	TEST(buf != NULL);

	TPtr8 sql = buf->Des();
	
	//Insert row 1

	const TUint8 KHexVal1 = 0x7E;
	_LIT8(KHexValStr1, "7E");
	sql.Copy(KSqlStmt2);
	sql.Append(_L8("1, x'"));
	TInt i;
	for(i=0;i<KDataLen;++i)
		{
		sql.Append(KHexValStr1);
		}
	sql.Append(_L8("')"));

	rc = db.Exec(sql);
	TEST2(rc, 1);

	//Insert row 2
	
	const TUint8 KHexVal2 = 0xA3;
	_LIT8(KHexValStr2, "A3");
	sql.Copy(KSqlStmt2);
	sql.Append(_L8("2, x'"));
	for(i=0;i<KDataLen;++i)
		{
		sql.Append(KHexValStr2);
		}
	sql.Append(_L8("')"));

	rc = db.Exec(sql);
	TEST2(rc, 1);

    //Insert row 3: the binary column length is just 2 bytes
    rc = db.Exec(_L("INSERT INTO A VALUES(3, x'A5D3')"));
    TEST2(rc, 1);
	
	//Prepare SELECT SQL statement
	_LIT8(KSqlStmt3, "SELECT * FROM A");
	RSqlStatement stmt = PrepareSqlStmt<TDesC8, TBuf8<KSqlBufSize> >(db, KSqlStmt3, KErrNone);

	//Move on row 1
	rc = stmt.Next();
	TEST2(rc, KSqlAtRow);

	//Read the long binary column using a stream
	RSqlColumnReadStream columnStream;
	rc = columnStream.ColumnBinary(stmt, 1);
	columnStream.Close();
	TEST2(rc, KErrNone);
	
	//...and the leaving version of ColumnBinary() 
	TRAP(rc, columnStream.ColumnBinaryL(stmt, 1));
	TEST2(rc, KErrNone);
	
	TInt size = stmt.ColumnSize(1);
	TPtr8 colData = buf->Des();
	TRAP(rc, columnStream.ReadL(colData, size));
	columnStream.Close();
	TEST2(rc, KErrNone);
	
	//Check the column value
	for(i=0;i<KDataLen;++i)
		{
		TUint8 val = colData[i];
		TEST(val = KHexVal1);	
		}

	//Move on row 2
	rc = stmt.Next();
	TEST2(rc, KSqlAtRow);

	//Read row 2 using ColumnBinary(TInt aColumnIndex, TPtrC8& aPtr).
	TPtrC8 colDataPtr;
	rc = stmt.ColumnBinary(1, colDataPtr);
	TEST2(rc, KErrNone);
	
	//Check the column value
	for(i=0;i<KDataLen;++i)
		{
		TUint8 val = colDataPtr[i];
		TEST(val = KHexVal2);	
		}

	//Read row 2 using ColumnBinary(TInt aColumnIndex, TDes8& aDest).
	rc = stmt.ColumnBinary(1, colData);
	TEST2(rc, KErrNone);
	
	//Check the column value
	for(i=0;i<KDataLen;++i)
		{
		TUint8 val = colData[i];
		TEST(val = KHexVal2);	
		}
	
	//Read row 2 using a stream
	colData.Zero();
	rc = columnStream.ColumnBinary(stmt, 1);
	TEST2(rc, KErrNone);
	size = stmt.ColumnSize(1);
	TRAP(rc, columnStream.ReadL(colData, size));
	columnStream.Close();
	TEST2(rc, KErrNone);
	
	//Check the column value
	for(i=0;i<KDataLen;++i)
		{
		TUint8 val = colData[i];
		TEST(val = KHexVal2);	
		}
		
	//Read the column value using RSqlStatement::ColumnBinary(). 
	//The recipient buffer max length is smaller than the column value length.
	rc = stmt.Reset();
	TEST2(rc, KErrNone);
	rc = stmt.Next();
	TEST2(rc, KSqlAtRow);
	const TInt KBufMaxLen = 100;
	TBuf8<KBufMaxLen> colBuf;
	rc = stmt.ColumnBinary(1, colBuf);
	TEST2(rc, KErrOverflow);
	//Check the column value
	for(i=0;i<KBufMaxLen;++i)
		{
		TUint8 val = colBuf[i];
		TEST(val = KHexVal2);	
		}
	
    //Move on row 3. The binary column value length is just 2 bytes.
    rc = stmt.Next();
    TEST2(rc, KSqlAtRow);
    rc = stmt.Next();
    TEST2(rc, KSqlAtRow);
    TBuf8<2> buf1;
    rc = stmt.ColumnBinary(1, buf1); 
    TEST2(rc, KErrNone);
    TEST2(buf1.Length(), 2);
    TBuf8<1> buf2;
    rc = stmt.ColumnBinary(1, buf2); 
    TEST2(rc, KErrOverflow);
    
	stmt.Close();
	
	//Deallocate buf
	delete buf; 
	buf = NULL;
	
	db.Close();

	rc = RSqlDatabase::Delete(KTestDbName1);
	TEST2(rc, KErrNone);
	}

/**
@SYMTestCaseID          PDS-SQL-CT-4191
@SYMTestCaseDesc        The test creates a test database and inserts one record using a stream.
                        MStreamBuf::SeekL() is used to modify the parameter data at specific positions.
                        Then the test executes a SELECT statement to read the just written record.
                        MStreamBuf::SeekL() is used to read the column content at specific positions 
                        (the same positions used during the record write operation). The read byte values must
                        match the written byte values.
@SYMTestPriority        High
@SYMTestActions         RSqlColumnReadStream::ColumnBinary() and RSqlParamWriteStream::BindBinary() - MStreamBuf::SeekL() test.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF145125
*/  
void StreamSeekTestL()
    {
    RSqlDatabase db;
    CleanupClosePushL(db);
    TInt rc = db.Create(KTestDbName1);
    TEST2(rc, KErrNone);
    rc = db.Exec(_L("CREATE TABLE A(Fld1 INTEGER, Fld2 BLOB)"));
    TEST(rc >= 0);
    //Write a record to the database using a stream. MStreamBuf::SeekL() is used to modify the content at a specific position.
    RSqlStatement stmt;
    CleanupClosePushL(stmt);
    rc = stmt.Prepare(db, _L("INSERT INTO A(Fld1, Fld2) VALUES(1, ?)"));
    TEST2(rc, KErrNone);
    
    RSqlParamWriteStream strm1;
    CleanupClosePushL(strm1);
    rc = strm1.BindBinary(stmt, 0);
    TEST2(rc, KErrNone);

    for(TInt i=0;i<256;++i)
        {
        strm1 << (TUint8)i;
        }
    
    const TInt KStreamOffset = 10;
    const TUint8 KByte = 'z';
    _LIT8(KData, "QWERTYUIOPASDFG");
    
    MStreamBuf* strm1buf = strm1.Sink();
    TEST(strm1buf != NULL);
    
    strm1buf->SeekL(MStreamBuf::EWrite, EStreamBeginning, 0);
    strm1buf->WriteL(&KByte, 1);
    
    strm1buf->SeekL(MStreamBuf::EWrite, EStreamMark, KStreamOffset);
    strm1buf->WriteL(&KByte, 1);
    
    strm1buf->SeekL(MStreamBuf::EWrite, EStreamEnd, 0);
    strm1buf->WriteL(KData().Ptr(), KData().Length());
    
    strm1buf->SeekL(MStreamBuf::EWrite, EStreamEnd, -4 * KStreamOffset);
    strm1buf->WriteL(&KByte, 1);
    
    strm1.CommitL();
    CleanupStack::PopAndDestroy(&strm1);
    
    rc = stmt.Exec();
    TEST2(rc, 1);
    CleanupStack::PopAndDestroy(&stmt);
    
    //Read the record using a stream. MStreamBuf::SeekL() is used to read the content at a specific position.
    CleanupClosePushL(stmt);
    rc = stmt.Prepare(db, _L("SELECT Fld2 FROM A WHERE Fld1 = 1"));
    TEST2(rc, KErrNone);
    rc = stmt.Next();
    TEST2(rc, KSqlAtRow);
    
    RSqlColumnReadStream strm2;
    CleanupClosePushL(strm2);
    rc = strm2.ColumnBinary(stmt, 0);
    TEST2(rc, KErrNone);

    TUint8 byte = 0;
    MStreamBuf* strm2buf = strm2.Source();
    TEST(strm1buf != NULL);
    
    strm2buf->SeekL(MStreamBuf::ERead, EStreamBeginning, 0);
    rc = strm2buf->ReadL(&byte, 1);
    TEST2(rc, 1);
    TEST2(byte, KByte);
    
    strm2buf->SeekL(MStreamBuf::ERead, EStreamMark, KStreamOffset);
    rc = strm2buf->ReadL(&byte, 1);
    TEST2(rc, 1);
    TEST2(byte, KByte);
    
    strm2buf->SeekL(MStreamBuf::ERead, EStreamEnd, -KData().Length());
    TUint8 buf[20];
    rc = strm2buf->ReadL(buf, KData().Length());
    TEST2(rc, KData().Length());
    TPtrC8 bufptr(buf, rc);
    TEST(bufptr == KData);
    
    strm2buf->SeekL(MStreamBuf::ERead, EStreamEnd, -4 * KStreamOffset);
    rc = strm2buf->ReadL(&byte, 1);
    TEST2(rc, 1);
    TEST2(byte, KByte);
    
    CleanupStack::PopAndDestroy(&strm2);
    CleanupStack::PopAndDestroy(&stmt);
    
    CleanupStack::PopAndDestroy(&db);
    rc = RSqlDatabase::Delete(KTestDbName1);
    TEST2(rc, KErrNone);
    }

/**
@SYMTestCaseID          PDS-SQL-CT-4174
@SYMTestCaseDesc        Test for DEF144937: SQL, SQL server, the code coverage can be improved in some areas.
@SYMTestPriority        High
@SYMTestActions         The test creates a test database with a table with 3 records.
                        The first record has a BLOB column with 0 length.
                        The second record has a BLOB column with length less than KSqlMaxDesLen
                        (in debug mode) in which case no IPC call is needed to be made in order 
                        to access the column value via stream.
                        The third record has a BLOB column with length exactly KSqlMaxDesLen 
                        in which case an IPC call will be made in order to retrieve the column value,
                        but the column value will be copied directly to the client - no stream object is created. 
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144937
*/  
void ColumnBinaryStreamTest2()
    {
    RSqlDatabase db;    
    TInt rc = db.Create(KTestDbName1);
    TEST2(rc, KErrNone);

    enum {KSqlBufSize = 128};
    
    //Create a table
    _LIT8(KSqlStmt1, "CREATE TABLE A(Fld1 INTEGER, Fld2 BLOB);");
    ExecSqlStmtOnDb<TDesC8, TBuf8<KSqlBufSize> >(db, KSqlStmt1(), KErrNone);
    
    //Insert one record where the BLOB length is 0. 
    //Insert second record where the BLOB length is smaller than the max inline column length - KSqlMaxDesLen.
    //Insert third record where the BLOB length is exactly the max inline column length - KSqlMaxDesLen.
    _LIT8(KSqlStmt2, "INSERT INTO A VALUES(1, '');INSERT INTO A VALUES(2, x'0102030405');INSERT INTO A VALUES(3, x'0102030405060708');");
    ExecSqlStmtOnDb<TDesC8, TBuf8<KSqlBufSize> >(db, KSqlStmt2(), KErrNone);
    
    RSqlStatement stmt;
    rc = stmt.Prepare(db, _L("SELECT Fld2 FROM A"));
    TEST2(rc, KErrNone);

    TBuf8<16> databuf;
    
    rc = stmt.Next();
    TEST2(rc, KSqlAtRow);
    //ColumnBinary() does not make an IPC call because the BLOB length is 0.
    RSqlColumnReadStream strm;
    rc = strm.ColumnBinary(stmt, 0);
    TEST2(rc, KErrNone);
    TRAP(rc, strm.ReadL(databuf, stmt.ColumnSize(0)));
    strm.Close();
    TEST2(rc, KErrNone);
    TEST2(databuf.Length(), 0);

    rc = stmt.Next();
    TEST2(rc, KSqlAtRow);
    //ColumnBinary() does not make an IPC call because the BLOB length is less than the max inline 
    //column length - KSqlMaxDesLen.
    rc = strm.ColumnBinary(stmt, 0);
    TEST2(rc, KErrNone);
    TRAP(rc, strm.ReadL(databuf, stmt.ColumnSize(0)));
    strm.Close();
    TEST2(rc, KErrNone);
    TEST(databuf == _L8("\x1\x2\x3\x4\x5"));

    rc = stmt.Next();
    TEST2(rc, KSqlAtRow);
    //ColumnBinary() makes an IPC call  (in _DEBUG mode) because:
    // - the column length is exactly KSqlMaxDesLen.  
    // - but at the same time the column length is equal to KIpcBufSize (in debug mode).
    rc = strm.ColumnBinary(stmt, 0);
    TEST2(rc, KErrNone);
    TRAP(rc, strm.ReadL(databuf, stmt.ColumnSize(0)));
    strm.Close();
    TEST2(rc, KErrNone);
    TEST(databuf == _L8("\x1\x2\x3\x4\x5\x6\x7\x8"));
    
    stmt.Close();
    db.Close();

    rc = RSqlDatabase::Delete(KTestDbName1);
    TEST2(rc, KErrNone);
    }

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1608
@SYMTestCaseDesc		Setting long text parameter values test.
@SYMTestPriority		High
@SYMTestActions			RSqlParamWriteStream::BindText() test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void TextParameterStreamTest()
	{
	RSqlDatabase db;	
	TInt rc = db.Create(KTestDbName1);
	TEST2(rc, KErrNone);

	enum {KSqlBufSize = 64};
	
	//Create a table
	_LIT8(KSqlStmt1, "CREATE TABLE A(Fld1 INTEGER, Fld2 TEXT);");
	ExecSqlStmtOnDb<TDesC8, TBuf8<KSqlBufSize> >(db, KSqlStmt1(), KErrNone); 

	const TInt KTextLen = 3001;

	_LIT(KSqlStmt2, "INSERT INTO A(Fld1, Fld2) VALUES(");
	
	//Allocate a buffer for the SQL statement.
	HBufC* buf = HBufC::New(KSqlStmt2().Length() + KTextLen + 10);
	TEST(buf != NULL);

	TPtr sql = buf->Des();
	
	//Insert a row

	const TChar KChar('g');
	sql.Copy(KSqlStmt2);
	sql.Append(_L("1, '"));
	TInt i;
	for(i=0;i<KTextLen;++i)
		{
		sql.Append(KChar);
		}
	sql.Append(_L("')"));

	rc = db.Exec(sql);
	TEST2(rc, 1);

	//Prepare parametrized SQL statement
	_LIT8(KSqlStmt3, "SELECT * FROM A WHERE Fld2 = :Val");
	RSqlStatement stmt = PrepareSqlStmt<TDesC, TBuf<KSqlBufSize> >(db, KSqlStmt3, KErrNone);

	//Open the parameter stream
	RSqlParamWriteStream paramStream;
	rc = paramStream.BindText(stmt, 0);
	paramStream.Close();
	TEST2(rc, KErrNone);

	//...and the leaving version of BindText()
	TRAP(rc, paramStream.BindTextL(stmt, 0));
	TEST2(rc, KErrNone);
	
	//Prepare and set the parameter value
	TPtr val = buf->Des();
	val.Zero();
	for(i=0;i<KTextLen;++i)
		{
		val.Append(KChar);
		}
	TRAP(rc, (paramStream.WriteL(val), paramStream.CommitL()));
	TEST2(rc, KErrNone);

	//Move on row 1
	rc = stmt.Next();
	TEST2(rc, KSqlAtRow);

	paramStream.Close();

	//Read the row using ColumnText(TInt aColumnIndex, TPtrC& aPtr).
	TPtrC colDataPtr;
	rc = stmt.ColumnText(1, colDataPtr);
	TEST2(rc, KErrNone);
	
	//Check the column value
	for(i=0;i<KTextLen;++i)
		{
		TEST(colDataPtr[i] == (TUint8)KChar);	
		}
	
	stmt.Close();

	//Deallocate buf
	delete buf; 
	buf = NULL;

	///////////////////////////////////////////////////////////////////////////////////////////////	
	//Open a "short" text parameter. The streaming API should work with "short" text parameters too.
	_LIT(KTextVal, "U012");

	//Delete all records
	_LIT8(KSqlStmt4, "DELETE FROM A");
	ExecSqlStmtOnDb<TDesC8, TBuf8<KSqlBufSize> >(db, KSqlStmt4(), KErrNone); 

	//Prepare INSERT SQL statement.	
	_LIT8(KSqlStmt5, "INSERT INTO A(Fld1, Fld2) VALUES(1, :Val)");
	stmt = PrepareSqlStmt<TDesC, TBuf<KSqlBufSize> >(db, KSqlStmt5, KErrNone);

	//Open the parameter stream
	rc = paramStream.BindText(stmt, 0);
	TEST2(rc, KErrNone);

	//Prepare and set the parameter value
	TRAP(rc, (paramStream.WriteL(KTextVal), paramStream.CommitL()));
	TEST2(rc, KErrNone);

	//Execute the prepared SQL statement
	rc = stmt.Exec();
	TEST2(rc, 1);

	paramStream.Close();
	stmt.Close();
	
	//Prepare SELECT SQL statement
	_LIT8(KSqlStmt6, "SELECT * FROM A WHERE Fld1 = 1");
	stmt = PrepareSqlStmt<TDesC, TBuf<KSqlBufSize> >(db, KSqlStmt6, KErrNone);

	//Move on row 1
	rc = stmt.Next();
	TEST2(rc, KSqlAtRow);

	///////////////////////////////////////////////////////////////////////////////////////////////	
	//Open a stream for a "short" text column. The streaming API should work with "short" text columns too.

	TBuf<20> columnVal;
	RSqlColumnReadStream columnStream;
	rc = columnStream.ColumnText(stmt, 1);
	TEST2(rc, KErrNone);
	TInt size = stmt.ColumnSize(1);
	TRAP(rc, columnStream.ReadL(columnVal, size));
	columnStream.Close();
	TEST2(rc, KErrNone);
	
	//Check the column value
	TEST(columnVal == KTextVal);
	
	stmt.Close();
	
	db.Close();

	rc = RSqlDatabase::Delete(KTestDbName1);
	TEST2(rc, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1622
@SYMTestCaseDesc		Setting long binary parameter values test.
@SYMTestPriority		High
@SYMTestActions			RSqlParamWriteStream::BindBinary() test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void BinaryParameterStreamTest()
	{
	//Create a test database
	RSqlDatabase db;	
	TInt rc = db.Create(KTestDbName1);
	TEST2(rc, KErrNone);

	enum {KSqlBufSize = 64};
	
	//Create a table
	_LIT8(KSqlStmt1, "CREATE TABLE A(Fld1 INTEGER, Fld2 BLOB);");
	ExecSqlStmtOnDb<TDesC8, TBuf8<KSqlBufSize> >(db, KSqlStmt1(), KErrNone); 

	const TInt KDataLen = 3731;

	_LIT8(KSqlStmt2, "INSERT INTO A(Fld1, Fld2) VALUES(");
	
	//Allocate a buffer for the SQL statement.
	HBufC8* buf = HBufC8::New(KSqlStmt2().Length() + KDataLen * 2 + 10);//"* 2" - SQL statement with HEX values
	TEST(buf != NULL);

	TPtr8 sql = buf->Des();
	
	//Insert a row

	const TUint8 KHexVal = 0xD3;
	_LIT8(KHexValStr, "D3");
	sql.Copy(KSqlStmt2);
	sql.Append(_L8("1, x'"));
	TInt i;
	for(i=0;i<KDataLen;++i)
		{
		sql.Append(KHexValStr);
		}
	sql.Append(_L8("')"));

	rc = db.Exec(sql);
	TEST2(rc, 1);

	//Prepare parametrized SQL statement
	_LIT8(KSqlStmt3, "SELECT * FROM A WHERE Fld2 = :Val");
	RSqlStatement stmt = PrepareSqlStmt<TDesC8, TBuf8<KSqlBufSize> >(db, KSqlStmt3, KErrNone);

	//Open the parameter stream
	RSqlParamWriteStream paramStream;
	rc = paramStream.BindBinary(stmt, 0);
	TEST2(rc, KErrNone);
	paramStream.Close();
    //Open the parameter stream with BindBinaryL()
    TRAP(rc, paramStream.BindBinaryL(stmt, 0));
    TEST2(rc, KErrNone);

	//Prepare and set the parameter value (NULL parameter value)
	TPtr8 prmVal = buf->Des();
	prmVal.SetLength(0);
	
	TRAP(rc, (paramStream.WriteL(prmVal), paramStream.CommitL()));
	TEST2(rc, KErrNone);

	rc = stmt.Next();
	TEST2(rc, KSqlAtEnd);
	
	paramStream.Close();
	stmt.Reset();
	
	//Prepare and set the parameter value (non-NULL binary value)
	rc = paramStream.BindBinary(stmt, 0);
	TEST2(rc, KErrNone);
	
	prmVal.SetLength(KDataLen);
	for(i=0;i<KDataLen;++i)
		{
		prmVal[i] = KHexVal;
		}
	TRAP(rc, (paramStream.WriteL(prmVal), paramStream.CommitL()));
	TEST2(rc, KErrNone);

	//Move on row 1
	rc = stmt.Next();
	TEST2(rc, KSqlAtRow);

	paramStream.Close();

	//Read the row using ColumnBinary(TInt aColumnIndex, TPtrC8& aPtr).
	TPtrC8 colDataPtr;
	rc = stmt.ColumnBinary(1, colDataPtr);
	TEST2(rc, KErrNone);
	
	//Check the column value
	for(i=0;i<KDataLen;++i)
		{
		TUint8 byte = colDataPtr[i];
		TEST(byte == KHexVal);	
		}
	
	stmt.Close();

	//Deallocate buf
	delete buf; 
	buf = NULL;


	///////////////////////////////////////////////////////////////////////////////////////////////	
	//Open a "short" binary parameter. The streaming API should work with "short" binary parameters too.
	_LIT8(KBinVal, "\x1\x2\x3\x4");

	//Delete all records
	_LIT8(KSqlStmt4, "DELETE FROM A");
	ExecSqlStmtOnDb<TDesC8, TBuf8<KSqlBufSize> >(db, KSqlStmt4(), KErrNone); 

	//Prepare INSERT SQL statement.	
	_LIT8(KSqlStmt5, "INSERT INTO A(Fld1, Fld2) VALUES(1, :Val)");
	stmt = PrepareSqlStmt<TDesC, TBuf<KSqlBufSize> >(db, KSqlStmt5, KErrNone);

	//Open the parameter stream
	rc = paramStream.BindBinary(stmt, 0);
	TEST2(rc, KErrNone);

	//Prepare and set the parameter value
	TRAP(rc, (paramStream.WriteL(KBinVal), paramStream.CommitL()));
	TEST2(rc, KErrNone);

	//Execute the prepared SQL statement
	rc = stmt.Exec();
	TEST2(rc, 1);

	paramStream.Close();
	stmt.Close();
	
	//Prepare SELECT SQL statement
	_LIT8(KSqlStmt6, "SELECT * FROM A WHERE Fld1 = 1");
	stmt = PrepareSqlStmt<TDesC, TBuf<KSqlBufSize> >(db, KSqlStmt6, KErrNone);

	//Move on row 1
	rc = stmt.Next();
	TEST2(rc, KSqlAtRow);

	///////////////////////////////////////////////////////////////////////////////////////////////	
	//Open a stream for a "short" binary column. The streaming API should work with "short" binary columns too.

	TBuf8<20> columnVal;
	RSqlColumnReadStream columnStream;
	rc = columnStream.ColumnBinary(stmt, 1);
	TEST2(rc, KErrNone);
	TInt size = stmt.ColumnSize(1);
	TRAP(rc, columnStream.ReadL(columnVal, size));
	columnStream.Close();
	TEST2(rc, KErrNone);
	
	//Check the column value
	TEST(columnVal == KBinVal);
	
	stmt.Close();
		
	db.Close();

	rc = RSqlDatabase::Delete(KTestDbName1);
	TEST2(rc, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1634
@SYMTestCaseDesc		RSqlStatement test - nameless parameter.
						Tests RSqlStatement behaviour if the prepared statement has nameless parameters.
@SYMTestPriority		High
@SYMTestActions			RSqlStatement test - nameless parameter.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void NamelessParameterTest()
	{
	//Create a test database
	RSqlDatabase db;	
	TInt rc = db.Create(KTestDbName1);
	TEST2(rc, KErrNone);

	rc = db.Exec(_L("CREATE TABLE A(F1 INTEGER, F2 INTEGER, F3 INTEGER)"));
	TEST(rc >= 0);

	RSqlStatement stmt;
	rc = stmt.Prepare(db, _L("SELECT * FROM A WHERE F1 = ? AND F2 = ? AND F3 = :Val"));
	TEST2(rc, KErrNone);

	TEST(stmt.ParameterIndex(_L("?0")) == 0);
	TEST(stmt.ParameterIndex(_L("?1")) == 1);
	TEST(stmt.ParameterIndex(_L(":VAL")) == 2);
		
	stmt.Close();

	db.Close();
	rc = RSqlDatabase::Delete(KTestDbName1);
	TEST2(rc, KErrNone);
	}

//Reads a SQL file and returns the file content as HBUFC string.
//The caller is responsible for destroying the returned HBUFC object.
template <class HBUFC> HBUFC* ReadSqlScript(const TDesC& aSqlFileName)
	{
	RFs fs;
	TEST2(fs.Connect(), KErrNone);
	
	RFile file;
	TEST2(file.Open(fs, aSqlFileName, EFileRead), KErrNone);
	
	TInt size = 0;
	TEST2(file.Size(size), KErrNone);
	
	HBufC8* sql = HBufC8::New(size);
	TEST(sql != NULL);
	
	TPtr8 ptr = sql->Des();
	TEST2(file.Read(ptr, size), KErrNone);

	file.Close();
	fs.Close();
	
	HBUFC* sql2 = HBUFC::New(size);
	TEST(sql2 != NULL);
	sql2->Des().Copy(sql->Des());
	delete sql;
	
	return sql2;
	}
//Explicit ReadSqlScript() template instantiations.
template HBufC8* ReadSqlScript<HBufC8>(const TDesC&);
template HBufC16* ReadSqlScript<HBufC16>(const TDesC&);

//Searches for the next aCommitStr appearance in aSqlScript string and returns a PTRC object holding
//the SQL strings from the beginning of aSqlScript till the aCommitStr (including it).
template <class PTRC, class DESC> PTRC GetNextTrans(PTRC& aSqlScript, const DESC& aCommitStr)
	{
	PTRC res(NULL, 0);
	TInt pos = aSqlScript.FindF(aCommitStr);
	if(pos >= 0)
		{
		pos += aCommitStr.Length();
		res.Set(aSqlScript.Left(pos));
		aSqlScript.Set(aSqlScript.Mid(pos));
		}
	return res;
	}
//Explicit GetNextTrans() template instantiations.
template TPtrC8 GetNextTrans<TPtrC8, TDesC8>(TPtrC8&, const TDesC8&);
template TPtrC16 GetNextTrans<TPtrC16, TDesC16>(TPtrC16&, const TDesC16&);

//Creates aDb database schema.
void CreateDbSchema(RSqlDatabase& aDb)
	{
	HBufC8* createDbScript = ReadSqlScript<HBufC8>(KCreateDbScript());
	TInt err = aDb.Exec(createDbScript->Des());
	TEST(err >= 0);	
	delete createDbScript;
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1768
@SYMTestCaseDesc		The test creates a database.
						Then the test executes 8-bit and 16-bit sql statements asynchronously
						(using asynchronous versions of RSqlDatabase::Exec() and RSqlStatement::Exec())
@SYMTestPriority		High
@SYMTestActions			SQL, Asynchronous sql statements execution.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void AsyncTest()
	{
	//////////////////////////////////////////////////////////////////////////
	//Asynchronous execution, 8-bit sql statements
	//Create the database
	RSqlDatabase db;
	TInt err = db.Create(KTestDbName1);
	TEST2(err, KErrNone);
	CreateDbSchema(db);
	//Read the sql script file and execute the statements 
	HBufC8* fillDbScript1 = ReadSqlScript<HBufC8>(KFillDbScript());
	TPtrC8 ptr1(fillDbScript1->Des());
	TPtrC8 sql1(GetNextTrans<TPtrC8, TDesC8>(ptr1, KCommitStr8()));
	TRequestStatus status;
	db.Exec(sql1, status);
    TEST2(status.Int(), KRequestPending);
    User::WaitForRequest(status); 
    TEST(status.Int() >= 0);
	delete fillDbScript1;
	//////////////////////////////////////////////////////////////////////////
	//Asynchronous execution, RSqlStatement::Exec().
	RSqlStatement stmt;
	
	err = stmt.Prepare(db, _L("UPDATE IDENTITYTABLE SET CM_FIRSTNAME=:V1"));
    TEST2(err, KErrNone);
    err = stmt.BindText(0, _L("+++first+++"));
    TEST2(err, KErrNone);
	stmt.Exec(status);
    TEST2(status.Int(), KRequestPending);
    User::WaitForRequest(status); 
    TEST(status.Int() >= 0);
	stmt.Close();
	
	err = stmt.Prepare(db, _L("UPDATE IDENTITYTABLE SET CM_FIRSTNAME='+++first+++'"));
    TEST2(err, KErrNone);
	stmt.Exec(status);
    TEST2(status.Int(), KRequestPending);
    User::WaitForRequest(status); 
    TEST(status.Int() >= 0);
	stmt.Close();
	
	//Verify the UPDATE operation
	err = stmt.Prepare(db, _L("SELECT COUNT(*) FROM identitytable WHERE cm_firstname = '+++first+++'"));
    TEST2(err, KErrNone);
    err = stmt.Next();
    TEST2(err, KSqlAtRow);
    TInt cnt = stmt.ColumnInt(0);
    TEST2(cnt, 64);
    stmt.Close();
	//Close and delete the database
	db.Close();
	err = RSqlDatabase::Delete(KTestDbName1);
	TEST2(err, KErrNone);
	//////////////////////////////////////////////////////////////////////////
	//Asynchronous execution, 16-bit sql statements
	//Create the database
	err = db.Create(KTestDbName1);
	TEST2(err, KErrNone);
	CreateDbSchema(db);
	//Read the sql script file and execute the statements 
	HBufC16* fillDbScript2 = ReadSqlScript<HBufC16>(KFillDbScript());
	TPtrC16 ptr2(fillDbScript2->Des());
	TPtrC16 sql2(GetNextTrans<TPtrC16, TDesC16>(ptr2, KCommitStr16()));
	db.Exec(sql2, status);
    TEST2(status.Int(), KRequestPending);
    User::WaitForRequest(status); 
    TEST(status.Int() >= 0);
	delete fillDbScript2;
	//Close and delete the database
	db.Close();
	err = RSqlDatabase::Delete(KTestDbName1);
	TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1816
@SYMTestCaseDesc		RSqlDatabase::Size() test.
						Call RSqlDatabase::Size() in various situations and check that the call does not fail.
@SYMTestPriority		High
@SYMTestActions			Call RSqlDatabase::Size() in various situations and check that the call does not fail.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ7141
*/
void SizeTest()
	{
	const TInt KTestDbCnt = 3;
	TPtrC dbNames[KTestDbCnt];
	dbNames[0].Set(KTestDbName1); //Non-secure shared database
	dbNames[1].Set(KTestDbName9); //Private database
	dbNames[2].Set(KTestDbName7); //Secure shared database
	
	TSecurityPolicy defaultPolicy(TSecurityPolicy::EAlwaysPass);
	RSqlSecurityPolicy securityPolicy;
	TInt rc = securityPolicy.Create(defaultPolicy);
	TEST2(rc, KErrNone);
	
	for(TInt i=0;i<KTestDbCnt;++i)
		{
		(void)RSqlDatabase::Delete(dbNames[i]);
		RSqlDatabase db;
		TInt rc = i == 2 ? db.Create(dbNames[i], securityPolicy) : db.Create(dbNames[i]);
		TEST2(rc, KErrNone);
		//Check database size
		TInt size1 = db.Size();
		TEST(size1 >= 0);
		//Insert some data and check the size again
		rc = db.Exec(_L("CREATE TABLE A(Id INTEGER, T TEXT)"));
		TEST(rc >= 0);
		rc = db.Exec(_L("INSERT INTO A VALUES(1, '1111111')"));
		TEST(rc >= 0);
		rc = db.Exec(_L("INSERT INTO A VALUES(2, '22222222222222')"));
		TEST(rc >= 0);
		TInt size2 = db.Size();
		TEST(size2 > size1);
		//Check the database size in a transaction
		rc = db.Exec(_L("BEGIN"));
		TEST(rc >= 0);
		TInt size3 = db.Size();
		TEST(size3 == size2);
		rc = db.Exec(_L("INSERT INTO A VALUES(3, '3333333333333333333333333333333333333333333333333333333333333')"));
		TEST(rc >= 0);
		rc = db.Exec(_L("INSERT INTO A VALUES(4, '4444444444444444444444444444444444444444444444444444444444444')"));
		TEST(rc >= 0);
		rc = db.Exec(_L("INSERT INTO A VALUES(5, '5555555555555555555555555555555555555555555555555555555555555')"));
		TEST(rc >= 0);
		TInt size4 = db.Size();
		TEST(size4 == size2);
		rc = db.Exec(_L("COMMIT"));
		TEST(rc >= 0);
		TInt size5 = db.Size();
		TEST(size5 == size2);
		//Cleanup
		db.Close();
		rc = RSqlDatabase::Delete(dbNames[i]);
		TEST2(rc, KErrNone);
		}
	securityPolicy.Close();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1817
@SYMTestCaseDesc		RSqlDatabase::InTransaction() test.
						Call RSqlDatabase::InTransaction() in various situations and check that the method reports 
						database's transaction state correctly.
@SYMTestPriority		High
@SYMTestActions			Call RSqlDatabase::InTransaction() in various situations and check that the method reports 
						database's transaction state correctly.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ7141
*/
void InTransactionTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName1);
	RSqlDatabase db;
	TInt rc = db.Create(KTestDbName1);
	TEST2(rc, KErrNone);
	//
	TBool state = db.InTransaction();
	TEST(!state);
	rc = db.Exec(_L("BEGIN"));
	TEST(rc >= 0);
	state = db.InTransaction();
	TEST(!!state);
	rc = db.Exec(_L("ROLLBACK"));
	TEST(rc >= 0);
	state = db.InTransaction();
	TEST(!state);
	//Cleanup
	db.Close();
	rc = RSqlDatabase::Delete(KTestDbName1);
	TEST2(rc, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4039
@SYMTestCaseDesc		RSqlDatabase::Size(TSize&) functional test.
						The test creates a non-secure shared database, private database, secure shared database
						and tests that the new RSqlDatabase::Size(TSize&) can be used on the created databases.
@SYMTestPriority		High
@SYMTestActions			RSqlDatabase::Size(TSize&) functional test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10407
*/
void SizeTest2()
	{
	const TInt KTestDbCnt = 3;
	TPtrC dbNames[KTestDbCnt];
	dbNames[0].Set(KTestDbName1); //Non-secure shared database
	dbNames[1].Set(KTestDbName9); //Private database
	dbNames[2].Set(KTestDbName7); //Secure shared database
	
	TSecurityPolicy defaultPolicy(TSecurityPolicy::EAlwaysPass);
	RSqlSecurityPolicy securityPolicy;
	TInt rc = securityPolicy.Create(defaultPolicy);
	TEST2(rc, KErrNone);
	
	_LIT8(KConfig, "compaction=background");
	
	for(TInt i=0;i<KTestDbCnt;++i)
		{
		(void)RSqlDatabase::Delete(dbNames[i]);
		RSqlDatabase db;
		TInt rc = i == 2 ? db.Create(dbNames[i], securityPolicy, &KConfig) : db.Create(dbNames[i], &KConfig);
		TEST2(rc, KErrNone);
		//Check database size
		RSqlDatabase::TSize size;
		rc = db.Size(size);
		TEST2(rc, KErrNone);
		TEST(size.iSize > 0);
		TEST2(size.iFree, 0);
		//Insert some data and check the size again
		rc = db.Exec(_L("CREATE TABLE A(Id INTEGER, T TEXT)"));
		TEST(rc >= 0);
		const TInt KTestRecCnt = 50;
		rc = db.Exec(_L("BEGIN"));
		TEST(rc >= 0);
		for(TInt k=0;k<KTestRecCnt;++k)
			{
			TBuf<100> sql;
			sql.Format(_L("INSERT INTO A VALUES(%d, 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA')"), k + 1);
			rc = db.Exec(sql);
			TEST2(rc, 1);
			}
		rc = db.Exec(_L("COMMIT"));
		TEST(rc >= 0);
		rc = db.Size(size);
		TEST2(rc, KErrNone);
		TEST(size.iSize > 0);
		TEST2(size.iFree, 0);
		//Delete the records and check the size again.
		rc = db.Exec(_L("DELETE FROM A"));
		rc = db.Size(size);
		TEST2(rc, KErrNone);
		TEST(size.iSize > 0);
		TEST(size.iFree > 0);
		//Cleanup
		db.Close();
		rc = RSqlDatabase::Delete(dbNames[i]);
		TEST2(rc, KErrNone);
		}
	securityPolicy.Close();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4040
@SYMTestCaseDesc		RSqlDatabase::Size(TSize&) on an attached database - functional test.
						The test creates a database and attaches another database. Then the test
						verifies that the new RSqlDatabase::Size(TSize&) method can be used on the attached 
						database. The test also calls the new method with an invalid attached database
						name and check that an appropriate error is reported.
@SYMTestPriority		High
@SYMTestActions			RSqlDatabase::Size(TSize&) on an attached database - functional test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10407
*/
void AttachedSizeTest2()
	{
	RSqlDatabase db;
	
	TInt err = db.Create(KTestDbName1);
	TEST2(err, KErrNone);
	err = db.Exec(_L("CREATE TABLE A(I INTEGER)"));
	TEST(err >= 0);
	db.Close();
	err = db.Create(KTestDbName8);
	TEST2(err, KErrNone);
	err = db.Exec(_L("CREATE TABLE B(J INTEGER)"));
	TEST(err >= 0);
	err = db.Exec(_L("BEGIN"));
	TEST(err >= 0);
	for(TInt i=0;i<1000;++i)
		{
		err = db.Exec(_L("INSERT INTO B(J) VALUES(1)"));
		TEST2(err, 1);
		}
	err = db.Exec(_L("COMMIT"));
	TEST(err >= 0);
	db.Close();
	
	err = db.Open(KTestDbName1);
	TEST2(err, KErrNone);
	_LIT(KAttachDbName, "B");
	err = db.Attach(KTestDbName8, KAttachDbName);
	TEST2(err, KErrNone);
	
	//Size(TSize&) - main and the attached databse
	RSqlDatabase::TSize	size1;
	err = db.Size(size1);
	TEST2(err, KErrNone);
	TEST(size1.iSize > 0);
	TEST(size1.iFree >= 0);
	RSqlDatabase::TSize	size2;
	err = db.Size(size2, KAttachDbName);
	TEST2(err, KErrNone);
	TEST(size2.iSize > 0);
	TEST(size2.iFree >= 0);
	TEST(size2.iSize > size1.iSize);

	//Very long attached database name
	TBuf<KMaxFileName + 10> longDbName;
	longDbName.SetLength(longDbName.MaxLength());
	err = db.Size(size1, longDbName);
	TEST2(err, KErrBadName);
	
	//An attempt to get the size of a non-existing attached database
	err = db.Size(size1, _L("TheDbDoesNotExist"));
	TEST2(err, KSqlErrGeneral);
	TPtrC msg = db.LastErrorMessage();
	TheTest.Printf(_L("Non-existing attached database, error message: %S\r\n"), &msg);
	
    //An attempt to get the size when the attached database name contains "bad" unicode characters (cannot be converted to UTF8)
    TBuf<2> dbName3;
    dbName3.SetLength(2);
    dbName3[0] = TChar(0xD800); 
    dbName3[1] = TChar(0xFC00); 
    err = db.Size(size1, dbName3);
    TEST2(err, KErrGeneral);
	
	err = db.Detach(KAttachDbName);
	TEST2(err, KErrNone);
	db.Close();
	(void)RSqlDatabase::Delete(KTestDbName8);
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4041
@SYMTestCaseDesc		RSqlDatabase::Size(TSize&) and different compaction modes - functional test.
						The test creates databases using all possible compaction modes and
						verifies that the new RSqlDatabase::Size(TSize&) method can be used on these 
						databases without any problems.
@SYMTestPriority		High
@SYMTestActions			RSqlDatabase::Size(TSize&) and different compaction modes - functional test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10407
*/
void DiffCompactModeSize2Test()
	{
	_LIT8(KAutoCompaction, "compaction = auto");
	RSqlDatabase db;
	TInt err = db.Create(KTestDbName1, &KAutoCompaction);
	TEST2(err, KErrNone);
	err = db.Exec(_L("CREATE TABLE A(I INTEGER)"));
	TEST(err >= 0);
	RSqlDatabase::TSize	size;
	err = db.Size(size);
	TEST2(err, KErrNone);
	TEST(size.iSize > 0);
	TEST(size.iFree >= 0);
	db.Close();
	(void)RSqlDatabase::Delete(KTestDbName1);
	
	_LIT8(KManualCompaction, "compaction = manual");
	err = db.Create(KTestDbName1, &KManualCompaction);
	TEST2(err, KErrNone);
	err = db.Exec(_L("CREATE TABLE A(I INTEGER)"));
	TEST(err >= 0);
	err = db.Size(size);
	TEST2(err, KErrNone);
	TEST(size.iSize > 0);
	TEST(size.iFree >= 0);
	db.Close();
	(void)RSqlDatabase::Delete(KTestDbName1);
	
	_LIT8(KBackgroundCompaction, "compaction = background");
	err = db.Create(KTestDbName1, &KBackgroundCompaction);
	TEST2(err, KErrNone);
	err = db.Exec(_L("CREATE TABLE A(I INTEGER)"));
	TEST(err >= 0);
	err = db.Size(size);
	TEST2(err, KErrNone);
	TEST(size.iSize > 0);
	TEST(size.iFree >= 0);
	db.Close();
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

/**
@SYMTestCaseID			PDS-SQL-CT-4205
@SYMTestCaseDesc		"PRAGMA count_changes" test.
						When "count_changes" pragma is ON, sqlite3_step() is called two times by the 
						SQL server, before getting the SQLITE_DONE return code.
						Everything else is the same (statement processing, etc.).
@SYMTestPriority		High
@SYMTestActions			"PRAGMA count_changes" test.
@SYMTestExpectedResults Test must not fail
*/
void CountChangesTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName1);
	RSqlDatabase db;
	TInt err = db.Create(KTestDbName1);
	TEST2(err, KErrNone);
	err = db.Exec(_L("CREATE TABLE A(I INTEGER)"));
	TEST(err >= 0);
	
	err = db.Exec(_L("PRAGMA count_changes=ON"));
	TEST(err >= 0);
	
	err = db.Exec(_L("INSERT INTO A VALUES(1)"));
	TEST2(err, 1);
	
	err = db.Exec(_L8("INSERT INTO A VALUES(2)"));
	TEST2(err, 1);
	
	RSqlStatement stmt;
	err = stmt.Prepare(db, _L("DELETE FROM A WHERE I>=1 AND I<=2"));
	TEST2(err, KErrNone);
	err = stmt.Exec();
	TEST2(err, 2);
	stmt.Close();
	
	err = db.Exec(_L("PRAGMA count_changes=OFF"));
	TEST(err >= 0);
	
	db.Close();
	(void)RSqlDatabase::Delete(KTestDbName1);
	}

void DoTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1601 Create/Open/Close database tests "));
	OpenCloseDatabaseTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1602 SetIsolationLevel() database tests "));	
	SetIsolationLevelTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1603 Delete database tests "));	
	DeleteDatabaseTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1640 Copy database tests "));	
	CopyDatabaseTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1604 Operations with NULL column values "));
	NullColumnValues();
	
	enum {KSqlBufSize = 1024};
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1605 RSqlDatabase::Exec() test. 16-bit SQL strings. "));	
	ExecOnDbTest<TDesC, TBuf<KSqlBufSize> >();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1605 RSqlDatabase::Exec() test. 8-bit SQL strings. "));	
	ExecOnDbTest<TDesC8, TBuf8<KSqlBufSize> >();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1606 RSqlStatement test. 16-bit SQL strings. "));	
	StatementTest<TDesC, TBuf<KSqlBufSize> >();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1606 RSqlStatement test. 8-bit SQL strings. "));
	StatementTest<TDesC8, TBuf8<KSqlBufSize> >();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1607 RSqlColumnReadStream test. Long text column "));
	ColumnTextStreamTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1621 RSqlColumnReadStream test. Long binary column "));
	ColumnBinaryStreamTest();

    TheTest.Next (_L(" @SYMTestCaseID:PDS-SQL-CT-4174 CSqlSrvSession::NewOutputStreamL() coverage test"));
	ColumnBinaryStreamTest2();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1608 RSqlParamWriteStream test. Long text parameter "));
	TextParameterStreamTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1622 RSqlParamWriteStream test. Long binary parameter "));
	BinaryParameterStreamTest();

    TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4191 MStreamBuf::SeekL() test"));
    StreamSeekTestL();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1634 RSqlStatement test. Nameless parameter "));
	NamelessParameterTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1768 Asynchronous execution tests "));
	AsyncTest();	
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1816 RSqlDatabase::Size() tests "));
	SizeTest();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1817 RSqlDatabase::InTransaction() tests "));
	InTransactionTest();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4039 RSqlDatabase::Size(RSqlDatabase::TSize&) tests"));
	SizeTest2();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4040 RSqlDatabase::Size(RSqlDatabase::TSize&) - attached database tests"));
	AttachedSizeTest2();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4041 RSqlDatabase::Size(RSqlDatabase::TSize&) - different compaction modes tests"));
	DiffCompactModeSize2Test();

	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4205 PRAGMA \"count_changes\" test"));
	CountChangesTest();
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
