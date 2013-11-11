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
#include <e32math.h>
#include <bautils.h>
#include <hal.h>
#include <stdlib.h>
#include <sqldb.h>
#include "t_sqlcmdlineutil.h"
#include "SqlSrvStrings.h"
#include "sqlite3.h"
#include "SqliteSymbian.h"

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_sqlperformance test"));
RFs   TheFs;
TBuf<200> TheTestTitle;
TCmdLineParams TheCmdLineParams;
TBuf8<200> TheSqlConfigString;

_LIT(KUtf8,  "UTF8 ");
_LIT(KUtf16, "UTF16");

TFileName TheSecureDbName;
TFileName TheNonSecureDbName;
TFileName TheNonSecureDbName2;
TFileName TheNonSecureTmpDbName;
TFileName TheSglRecDbFileName;

_LIT(KSqlServerPrivateDir, "\\private\\10281e17\\");

_LIT(KCreateDbScript, "z:\\test\\contacts_schema_to_vendors.sql");
_LIT(KFillDbScript, "z:\\test\\add_simple_contacts.sql");

_LIT8(KCommitStr8, "COMMIT;");
_LIT16(KCommitStr16, "COMMIT;");

_LIT8(KUpdateSql8, "UPDATE IDENTITYTABLE SET CM_FIRSTNAME=:V1, CM_LASTNAME=:V2,CM_COMPANYNAME=:V3 WHERE PARENT_CMID=:ID");
_LIT16(KUpdateSql16, "UPDATE IDENTITYTABLE SET CM_FIRSTNAME=:V1, CM_LASTNAME=:V2,CM_COMPANYNAME=:V3 WHERE PARENT_CMID=:ID");

_LIT8(KUpdateSql2_8, "UPDATE IDENTITYTABLE SET CM_FIRSTNAME='%S%d',CM_LASTNAME='%S%d',CM_COMPANYNAME='%S%d' WHERE PARENT_CMID=%d");
_LIT16(KUpdateSql2_16, "UPDATE IDENTITYTABLE SET CM_FIRSTNAME='%S%d',CM_LASTNAME='%S%d',CM_COMPANYNAME='%S%d' WHERE PARENT_CMID=%d");

_LIT8(KSelectSql8, "SELECT CM_FIRSTNAME, CM_LASTNAME, CM_COMPANYNAME FROM IDENTITYTABLE WHERE PARENT_CMID > 50");
_LIT16(KSelectSql16, "SELECT CM_FIRSTNAME, CM_LASTNAME, CM_COMPANYNAME FROM IDENTITYTABLE WHERE PARENT_CMID > 50");

_LIT8(KDeleteSql8, "DELETE FROM IDENTITYTABLE WHERE PARENT_CMID > 50");
_LIT16(KDeleteSql16, "DELETE FROM IDENTITYTABLE WHERE PARENT_CMID > 50");

extern TPtrC GetFirstSqlStmt(TPtr& aString);

_LIT(KFirstName, "FirstName-");
_LIT(KLastName, "LastName-");
_LIT(KCompanyName, "CompanyName-");

_LIT(KFirstName2, "12345678-");
_LIT(KLastName2, "ABCDEFGHIJK-");
_LIT(KCompanyName2, "KKKKKKKKKK10-");

const char* KRawPrmName1 = ":V1";
const char* KRawPrmName2 = ":V2";
const char* KRawPrmName3 = ":V3";
const char* KRawPrmName4 = ":ID";

_LIT(KPrmName1, ":V1");	
_LIT(KPrmName2, ":V2");	
_LIT(KPrmName3, ":V3");	
_LIT(KPrmName4, ":ID");	

const TInt KTestTecordCount = 1000;
	
///////////////////////////////////////////////////////////////////////////////////////

void TestEnvDestroy()
	{
	(void)RSqlDatabase::Delete(TheNonSecureTmpDbName);
	(void)RSqlDatabase::Delete(TheNonSecureDbName2);
	(void)RSqlDatabase::Delete(TheNonSecureDbName);
	(void)RSqlDatabase::Delete(TheSecureDbName);
	TheFs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TestEnvDestroy();
		TheTest.Printf(_L("*** Line %d\r\n"), aLine);
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		TestEnvDestroy();
		TheTest.Printf(_L("*** Line %d, Expected error: %d, got: %d\r\n"), aLine, aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

void TestEnvInit()
    {
	TInt err = TheFs.Connect();
	TEST2(err, KErrNone);

	err = TheFs.MkDir(TheNonSecureDbName);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

//Reads a SQL file and returns the file content as HBUFC string.
//The caller is responsible for destroying the returned HBUFC object.
template <class HBUFC> HBUFC* ReadSqlScript(const TDesC& aSqlFileName)
	{
	RFile file;
	TEST2(file.Open(TheFs, aSqlFileName, EFileRead), KErrNone);
	
	TInt size = 0;
	TEST2(file.Size(size), KErrNone);
	
	HBufC8* sql = HBufC8::New(size);
	TEST(sql != NULL);
	
	TPtr8 ptr = sql->Des();
	TEST2(file.Read(ptr, size), KErrNone);

	file.Close();
	
	HBUFC* sql2 = HBUFC::New(size + 1);
	TEST(sql2 != NULL);
	sql2->Des().Copy(sql->Des());
	sql2->Des().Append(TChar(0));
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

//Prints aTicks parameter (converted to ms)
void PrintStats(TUint32 aStartTicks, TUint32 aEndTicks)
	{
	static TInt freq = 0;
	if(freq == 0)
		{
		TEST2(HAL::Get(HAL::EFastCounterFrequency, freq), KErrNone);
		}
	TInt64 diffTicks = (TInt64)aEndTicks - (TInt64)aStartTicks;
	if(diffTicks < 0)
		{
		diffTicks = KMaxTUint32 + diffTicks + 1;
		}
	const TInt KMicroSecIn1Sec = 1000000;
	TInt32 us = (diffTicks * KMicroSecIn1Sec) / freq;
	TheTest.Printf(_L("####Execution time: %d ms\r\n"), us / 1000);
	}
	
void PrintFileSize(const TDesC& aFileName)
	{
	TParse parse;
	parse.Set(aFileName, &KSqlServerPrivateDir(), 0);
	RFile file;
	TInt err = file.Open(TheFs, parse.FullName(), EFileRead);
	TEST2(err, KErrNone);
	TInt size = 0;
	err = file.Size(size);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("####FileSize: %d\r\n"), size);
	file.Close();
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////     SQL SERVER performance tests
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
enum TDbType {ENonSecureDb, ESecureDb};

//Template class offering Create() and Open() methods for creating/opening a RSqlDatabase object
template <TDbType TYPE> class TDbHelper
	{
public:	
	static void Create(const TDesC& aDbName);
	static RSqlDatabase Open(const TDesC& aDbName);
	};

//Creates aDb database schema.
void CreateDbSchema(RSqlDatabase& aDb)
	{
	HBufC8* createDbScript = ReadSqlScript<HBufC8>(KCreateDbScript());
	TInt err = aDb.Exec(createDbScript->Des());
	TEST(err >= 0);	
	delete createDbScript;
	}

//Explicit TDbHelper class specialization for creating/opening a nonsecure RSqlDatabase object
template <> class TDbHelper<ENonSecureDb>
	{
public:	
	static void Create(const TDesC& aDbName)
		{
		RSqlDatabase::Delete(aDbName);
		RSqlDatabase db;
		TInt err = db.Create(aDbName, &TheSqlConfigString);
		TEST2(err, KErrNone);
		CreateDbSchema(db);
		db.Close();
		}
	static RSqlDatabase Open(const TDesC& aDbName)
		{
		RSqlDatabase db;
		TInt err = db.Open(aDbName);
		TEST2(err, KErrNone);
		return db;
		}
	};

//Explicit TDbHelper class specialization for creating/opening a secure RSqlDatabase object
template <> class TDbHelper<ESecureDb>
	{
public:	
	static void Create(const TDesC& aDbName)
		{
		RSqlDatabase::Delete(aDbName);
		RSqlSecurityPolicy securityPolicy;
		TInt err = securityPolicy.Create(TSecurityPolicy(TSecurityPolicy::EAlwaysPass));
		TEST2(err, KErrNone);	
		securityPolicy.SetDbPolicy(RSqlSecurityPolicy::ESchemaPolicy, TSecurityPolicy(ECapabilityReadUserData, ECapabilityWriteUserData));
		securityPolicy.SetDbPolicy(RSqlSecurityPolicy::EWritePolicy, TSecurityPolicy(ECapabilityWriteUserData));
		securityPolicy.SetDbPolicy(RSqlSecurityPolicy::EReadPolicy, TSecurityPolicy(ECapabilityReadUserData));
		RSqlDatabase db;
		err = db.Create(aDbName, securityPolicy, &TheSqlConfigString);
		TEST2(err, KErrNone);	
		securityPolicy.Close();
		CreateDbSchema(db);
		db.Close();
		}
	static RSqlDatabase Open(const TDesC& aDbName)
		{
		RSqlDatabase db;
		TInt err = db.Open(aDbName);
		TEST2(err, KErrNone);
		return db;
		}
	};

//Executes SQL script
template <class HBUFC, class PTRC, class DESC> void ExecuteSqlScript(RSqlDatabase& aDb, const TDesC& aScriptFileName, const DESC& aCommitStr)
	{
	HBUFC* fillDbScript = ReadSqlScript<HBUFC>(aScriptFileName);
	TUint32 start = User::FastCounter();
	PTRC ptr(fillDbScript->Des());
	PTRC sql(GetNextTrans<PTRC, DESC>(ptr, aCommitStr));
	while(sql.Length() > 0)
		{
		TInt err = aDb.Exec(sql);
		if(err == KErrNoMemory)
			{
			TheTest.Printf(_L("###ERROR 'Out of memory'! The test cannot be completed!\r\n"));
			return;	
			}
		TEST(err > 0);	
		sql.Set(GetNextTrans<PTRC, DESC>(ptr, aCommitStr));
		}
	TUint32 end = User::FastCounter();
	PrintStats(start, end);
	delete fillDbScript;
	}
//Explicit ExecuteSqlScript() template instantiations.
template void ExecuteSqlScript<HBufC8, TPtrC8, TDesC8>(RSqlDatabase&, const TDesC&, const TDesC8&);
template void ExecuteSqlScript<HBufC16, TPtrC16, TDesC16>(RSqlDatabase&, const TDesC&, const TDesC16&);

///////////////////////////////////////////////////////////////////////////////////////

//"INSERT" test function
template <class HBUFC, class PTRC, class DESC, TDbType TYPE> void InsertTest(const TDesC& aDbFileName, const DESC& aCommitStr)
	{
	TheTest.Printf(_L("\"Insert\" test\r\n"));
	RSqlDatabase db = TDbHelper<TYPE>::Open(aDbFileName);
	ExecuteSqlScript<HBUFC, PTRC, DESC>(db, KFillDbScript, aCommitStr);
	db.Close();	
	}
//Explicit InsertTest() template instantiations.
template void InsertTest<HBufC8, TPtrC8, TDesC8, ENonSecureDb>(const TDesC&, const TDesC8&);
template void InsertTest<HBufC8, TPtrC8, TDesC8, ESecureDb>(const TDesC&, const TDesC8&);
template void InsertTest<HBufC16, TPtrC16, TDesC16, ENonSecureDb>(const TDesC&, const TDesC16&);
template void InsertTest<HBufC16, TPtrC16, TDesC16, ESecureDb>(const TDesC&, const TDesC16&);

//"UPDATE" test function (parametrized update)
template <class DESC, TDbType TYPE> void UpdateTest(const TDesC& aDbFileName, const DESC& aUpdateSql)
	{
	TheTest.Printf(_L("\"Update (parametrized)\" test\r\n"));
	RSqlDatabase db = TDbHelper<TYPE>::Open(aDbFileName);
	RSqlStatement stmt;
	TInt err = stmt.Prepare(db, aUpdateSql);
	TEST2(err, KErrNone);
	
	TInt firstNamePrmIdx = stmt.ParameterIndex(KPrmName1());
	TEST(firstNamePrmIdx >= 0);
	TInt lastNamePrmIdx = stmt.ParameterIndex(KPrmName2());
	TEST(lastNamePrmIdx >= 0);
	TInt companyNamePrmIdx = stmt.ParameterIndex(KPrmName3());
	TEST(companyNamePrmIdx >= 0);
	TInt idIdx = stmt.ParameterIndex(KPrmName4());
	TEST(idIdx >= 0);
	
	TUint32 start = User::FastCounter();
	for(TInt id=1;id<=KTestTecordCount;++id)
		{
		TBuf<20> buf;
		buf.Copy(KFirstName);
		buf.AppendNum(id);
		err = stmt.BindText(firstNamePrmIdx, buf);
		TEST2(err, KErrNone);
		buf.Copy(KLastName);
		buf.AppendNum(id);
		err = stmt.BindText(lastNamePrmIdx, buf);
		TEST2(err, KErrNone);
		buf.Copy(KCompanyName);
		buf.AppendNum(id);
		err = stmt.BindText(companyNamePrmIdx, buf);
		TEST2(err, KErrNone);
		err = stmt.BindInt(idIdx, id);
		TEST2(err, KErrNone);
		err = stmt.Exec();
		TEST(err > 0);	
		err = stmt.Reset();
		TEST2(err, KErrNone);
		}
	TUint32 end = User::FastCounter();
	PrintStats(start, end);
	stmt.Close();
	db.Close();
	}
//Explicit UpdateTest() template instantiations.
template void UpdateTest<TDesC8, ENonSecureDb>(const TDesC&, const TDesC8&);
template void UpdateTest<TDesC8, ESecureDb>(const TDesC&, const TDesC8&);
template void UpdateTest<TDesC16, ENonSecureDb>(const TDesC&, const TDesC16&);
template void UpdateTest<TDesC16, ESecureDb>(const TDesC&, const TDesC16&);

//"UPDATE" test function (without parameters) - SQL server
template <class BUF, class DESC, TDbType TYPE> void UpdateWPTest(const TDesC& aDbFileName, const DESC& aUpdateSql)
	{
	TheTest.Printf(_L("\"Update (without parameters)\" test\r\n"));
	RSqlDatabase db = TDbHelper<TYPE>::Open(aDbFileName);
	TBuf<200> fmtstr;
	fmtstr.Copy(aUpdateSql);
	
	TUint32 start = User::FastCounter();
	for(TInt id=1;id<=KTestTecordCount;++id)
		{
		TBuf<200> buf;
		buf.Format(fmtstr, &KFirstName2, id, &KLastName2, id, &KCompanyName2, id, id);
		BUF sql;
		sql.Copy(buf);
		TInt err = db.Exec(sql);
		TEST(err > 0);	
		}
	TUint32 end = User::FastCounter();
	PrintStats(start, end);
	db.Close();
	PrintFileSize(aDbFileName);
	}
//Explicit UpdateWPTest() template instantiations.
template void UpdateWPTest<TBuf8<200>, TDesC8, ENonSecureDb>(const TDesC&, const TDesC8&);
template void UpdateWPTest<TBuf8<200>, TDesC8, ESecureDb>(const TDesC&, const TDesC8&);
template void UpdateWPTest<TBuf16<200>, TDesC16, ENonSecureDb>(const TDesC&, const TDesC16&);
template void UpdateWPTest<TBuf16<200>, TDesC16, ESecureDb>(const TDesC&, const TDesC16&);

//"SELECT" test function
template <class DESC, TDbType TYPE> void SelectTest(const TDesC& aDbFileName, const DESC& aSelectSql)
	{
	TheTest.Printf(_L("\"Select\" test\r\n"));
	RSqlDatabase db = TDbHelper<TYPE>::Open(aDbFileName);
	RSqlStatement stmt;
	TInt err = stmt.Prepare(db, aSelectSql);
	TEST2(err, KErrNone);
	TUint32 start = User::FastCounter();
	while((err = stmt.Next()) == KSqlAtRow)
		{
		TBuf<20> buf;
		err = stmt.ColumnText(0, buf);			
		TEST2(err, KErrNone);
		TEST(buf.Length() > 0);
		err = stmt.ColumnText(1, buf);			
		TEST2(err, KErrNone);
		TEST(buf.Length() > 0);
		err = stmt.ColumnText(2, buf);			
		TEST2(err, KErrNone);
		TEST(buf.Length() > 0);
		}
	TEST2(err, KSqlAtEnd);
	TUint32 end = User::FastCounter();
	PrintStats(start, end);
	stmt.Close();
	db.Close();	
	}
//Explicit SelectTest() template instantiations.
template void SelectTest<TDesC8, ENonSecureDb>(const TDesC&, const TDesC8&);
template void SelectTest<TDesC8, ESecureDb>(const TDesC&, const TDesC8&);
template void SelectTest<TDesC16, ENonSecureDb>(const TDesC&, const TDesC16&);
template void SelectTest<TDesC16, ESecureDb>(const TDesC&, const TDesC16&);

//"DELETE" test function
template <class DESC, TDbType TYPE> void DeleteTest(const TDesC& aDbFileName, const DESC& aDeleteSql)
	{
	TheTest.Printf(_L("\"Delete\" test\r\n"));
	RSqlDatabase db = TDbHelper<TYPE>::Open(aDbFileName);
	TUint32 start = User::FastCounter();
	TInt err = db.Exec(aDeleteSql);
	TEST(err > 0);	
	TUint32 end = User::FastCounter();
	PrintStats(start, end);
	db.Close();	
	}
//Explicit SelectTest() template instantiations.
template void DeleteTest<TDesC8, ENonSecureDb>(const TDesC&, const TDesC8&);
template void DeleteTest<TDesC8, ESecureDb>(const TDesC&, const TDesC8&);
template void DeleteTest<TDesC16, ENonSecureDb>(const TDesC&, const TDesC16&);
template void DeleteTest<TDesC16, ESecureDb>(const TDesC&, const TDesC16&);
	
//Performance test function: INSERT, UPDATE, SELECT
template <class HBUFC, class PTRC, class DESC, TDbType TYPE> void PerformanceTest(const TDesC& aDbFileName,
																				  const DESC& aCommitStr,
																				  const DESC& aUpdateSql,
																				  const DESC& aSelectSql,
																				  const DESC& aDeleteSql)
	{
	CFileMan* fm = NULL;
	TRAPD(err, fm = CFileMan::NewL(TheFs));
	TEST2(err, KErrNone);
	
	TDbHelper<TYPE>::Create(aDbFileName);
	InsertTest<HBUFC, PTRC, DESC, TYPE>(aDbFileName, aCommitStr);
	PrintFileSize(aDbFileName);
	(void)fm->Copy(aDbFileName, TheNonSecureTmpDbName);
	
	UpdateTest<DESC, TYPE>(aDbFileName, aUpdateSql);
	PrintFileSize(aDbFileName);
	SelectTest<DESC, TYPE>(aDbFileName, aSelectSql);
	
	DeleteTest<DESC, TYPE>(aDbFileName, aDeleteSql);
	PrintFileSize(aDbFileName);
	
	(void)fm->Copy(TheNonSecureTmpDbName, aDbFileName);
	(void)fm->Delete(TheNonSecureTmpDbName);
	delete fm;
	}
//Explicit PerformanceTest() template instantiations.
template void PerformanceTest<HBufC8, TPtrC8, TDesC8, ENonSecureDb>(const TDesC&, const TDesC8&, const TDesC8&, const TDesC8&, const TDesC8&);
template void PerformanceTest<HBufC8, TPtrC8, TDesC8, ESecureDb>(const TDesC&, const TDesC8&, const TDesC8&, const TDesC8&, const TDesC8&);
template void PerformanceTest<HBufC16, TPtrC16, TDesC16, ENonSecureDb>(const TDesC&, const TDesC16&, const TDesC16&, const TDesC16&, const TDesC16&);
template void PerformanceTest<HBufC16, TPtrC16, TDesC16, ESecureDb>(const TDesC&, const TDesC16&, const TDesC16&, const TDesC16&, const TDesC16&);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////     SQLITE API used directly  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Template class offering Create() and Open() methods for creating/opening a sqlite3 handle.
template <TCmdLineParams::TDbEncoding TYPE> class TDbHelper2
	{
public:	
	static void Create(const TDesC& aDbName);
	static sqlite3* Open(const TDesC& aDbName);
	};

//If error - prints error message and panics the application
void PanicIfError(sqlite3* aDbHandle, TInt aErr)
	{
  	if(aErr != SQLITE_OK && aErr != SQLITE_DONE && aErr != SQLITE_ROW)
  		{
  		if(aDbHandle)
  			{
	  		TPtrC p((const TUint16*)sqlite3_errmsg16(aDbHandle));
	  		TheTest.Printf(_L("Database err %d, msg: %S\n\n"), aErr, &p);
			(void)sqlite3_close(aDbHandle);
  			}
    	TEST(0);
  		}
	}

//If error - prints error message and returns zero
TInt ReportIfError(sqlite3* aDbHandle, TInt aErr)
	{
  	if(aErr != SQLITE_OK && aErr != SQLITE_DONE && aErr != SQLITE_ROW)
  		{
  		if(aDbHandle)
  			{
	  		TPtrC p((const TUint16*)sqlite3_errmsg16(aDbHandle));
	  		TheTest.Printf(_L("Database err %d, msg: %S\n\n"), aErr, &p);
  			}
  		return ETrue;
  		}
  	return EFalse;
	}

//Explicit TDbHelper2 class specialization for creating/opening a database with UTF8 default encoding
template <> class TDbHelper2<TCmdLineParams::EDbUtf8>
	{
public:	
	static void Create(const TDesC& aDbFileName)
		{
		(void)TheFs.Delete(aDbFileName);
		HBufC8* dbName = HBufC8::New(aDbFileName.Length() + 1);
		TEST(dbName != NULL);
		dbName->Des().Copy(aDbFileName);
		sqlite3* dbHandle = NULL;
		TInt err = sqlite3_open((const char*)(dbName->Des().PtrZ()), &dbHandle);
		delete dbName;
		PanicIfError(dbHandle, err);
		HBufC8* createDbScript = ReadSqlScript<HBufC8>(KCreateDbScript());
		HBufC8* createDbScript2 = HBufC8::New(createDbScript->Des().Length() + 1);
		TEST(createDbScript2 != NULL);
		createDbScript2->Des().Copy(createDbScript->Des());
		delete createDbScript;
		createDbScript2->Des().Append(TChar(0));
		err = sqlite3_exec(dbHandle, (const char*)createDbScript2->Des().Ptr(), NULL, 0, NULL);
		PanicIfError(dbHandle, err);
		delete createDbScript2;
		(void)sqlite3_close(dbHandle);
		}
	static sqlite3* Open(const TDesC& aDbFileName)
		{
		HBufC8* dbName = HBufC8::New(aDbFileName.Length() + 1);
		TEST(dbName != NULL);
		dbName->Des().Copy(aDbFileName);
		sqlite3* dbHandle = NULL;
		TInt err = sqlite3_open((const char*)(dbName->Des().PtrZ()), &dbHandle);
		delete dbName;
		PanicIfError(dbHandle, err);
		return dbHandle;
		}
	};

//Explicit TDbHelper2 class specialization for creating/opening a database with UTF16 default encoding
template <> class TDbHelper2<TCmdLineParams::EDbUtf16>
	{
public:	
	static void Create(const TDesC& aDbFileName)
		{
		(void)TheFs.Delete(aDbFileName);
		HBufC16* dbName = HBufC16::New(aDbFileName.Length() + 1);
		TEST(dbName != NULL);
		dbName->Des().Copy(aDbFileName);
		sqlite3* dbHandle = NULL;
		TInt err = sqlite3_open16((const void*)(dbName->Des().PtrZ()), &dbHandle);
		delete dbName;
		PanicIfError(dbHandle, err);
		HBufC8* createDbScript = ReadSqlScript<HBufC8>(KCreateDbScript());
		HBufC8* createDbScript2 = HBufC8::New(createDbScript->Des().Length() + 1);
		TEST(createDbScript2 != NULL);
		createDbScript2->Des().Copy(createDbScript->Des());
		delete createDbScript;
		createDbScript2->Des().Append(TChar(0));
		err = sqlite3_exec(dbHandle, (const char*)createDbScript2->Des().Ptr(), NULL, 0, NULL);
		PanicIfError(dbHandle, err);
		delete createDbScript2;
		(void)sqlite3_close(dbHandle);
		}
	static sqlite3* Open(const TDesC& aDbFileName)
		{
		HBufC16* dbName = HBufC16::New(aDbFileName.Length() + 1);
		TEST(dbName != NULL);
		dbName->Des().Copy(aDbFileName);
		sqlite3* dbHandle = NULL;
		TInt err = sqlite3_open16((const void*)(dbName->Des().PtrZ()), &dbHandle);
		delete dbName;
		PanicIfError(dbHandle, err);
		return dbHandle;
		}
	};

template <class HBUFC, class PTRC, class DESC> void InsertTest2(sqlite3* aDbHandle, const TDesC& aScriptFileName, const DESC& aCommitStr);

//Explicit InsertTest2() template specialization for UTF8 encoded SQL strings
template <> void InsertTest2<HBufC8, TPtrC8, TDesC8>(sqlite3* aDbHandle, const TDesC& aScriptFileName, const TDesC8& aCommitStr)
	{
	TheTest.Printf(_L("\"Insert\" test\r\n"));
	HBufC8* fillDbScript = ReadSqlScript<HBufC8>(aScriptFileName);
	TUint32 start = User::FastCounter();
	TPtrC8 ptr(fillDbScript->Des());
	TPtrC8 sql(GetNextTrans<TPtrC8, TDesC8>(ptr, aCommitStr));
	while(sql.Length() > 0)
		{
		TUint8* p = (TUint8*)sql.Ptr();
		p[sql.Length() - 1] = 0;
		TInt err = sqlite3_exec(aDbHandle, (const char*)sql.Ptr(), NULL, 0, NULL);
		PanicIfError(aDbHandle, err);
		sql.Set(GetNextTrans<TPtrC8, TDesC8>(ptr, aCommitStr));
		}
	TUint32 end = User::FastCounter();
	PrintStats(start, end);
	delete fillDbScript;
	}

//Explicit InsertTest2() template specialization for UTF16 encoded SQL strings
template <> void InsertTest2<HBufC16, TPtrC16, TDesC16>(sqlite3* aDbHandle, const TDesC& aScriptFileName, const TDesC16& aCommitStr)
	{
	TheTest.Printf(_L("\"Insert\" test\r\n"));
	HBufC16* fillDbScript = ReadSqlScript<HBufC16>(aScriptFileName);
	TUint32 start = User::FastCounter();
	TPtrC16 ptr(fillDbScript->Des());
	TPtrC16 sql(GetNextTrans<TPtrC16, TDesC16>(ptr, aCommitStr));
	while(sql != KNullDesC16)
		{
		TPtr16 p((TUint16*)sql.Ptr(), sql.Length(), sql.Length());
		TPtrC16 current(KNullDesC16);
		while(p.Length() > 1) //"> 1" because it is a zero terminated string
			{
			current.Set(GetFirstSqlStmt(p));
			sqlite3_stmt* stmtHandle = NULL;
			const void* stmtTail = NULL;
			TInt err = sqlite3_prepare16_v2(aDbHandle, current.Ptr(), -1, &stmtHandle, &stmtTail);
			if(stmtHandle)	//stmtHandle can be NULL for statements like this: ";".
				{
				if(err == SQLITE_OK)
					{
					while((err = sqlite3_step(stmtHandle)) == SQLITE_ROW)
						{
						}
					}
				TInt err2 = sqlite3_finalize(stmtHandle);
				TEST2(err2, SQLITE_OK);
				}
			if(ReportIfError(aDbHandle, err))
				{
				delete fillDbScript;
				return;
				}
  			}//while(p.Length() > 1)
		sql.Set(GetNextTrans<TPtrC16, TDesC16>(ptr, aCommitStr));
		}//while(sql != KNullDesC16)
	TUint32 end = User::FastCounter();
	PrintStats(start, end);
	delete fillDbScript;
	}

template <class HBUFC, class DESC> void UpdateTest2(sqlite3* aDbHandle, const DESC& aUpdateSql);

//Explicit UpdateTest2() template specialization for UTF8 encoded SQL strings
template <> void UpdateTest2<HBufC8, TDesC8>(sqlite3* aDbHandle, const TDesC8& aUpdateSql)
	{
	TheTest.Printf(_L("\"Update\" test\r\n"));
	HBufC8* sql = HBufC8::New(aUpdateSql.Length() + 1);
	TEST(sql != NULL);
	sql->Des().Copy(aUpdateSql);
	sql->Des().Append(TChar(0));

	sqlite3_stmt* stmtHandle = NULL;
	const char* stmtTail = NULL;
	TInt err = sqlite3_prepare_v2(aDbHandle, (const char*)sql->Des().Ptr(), -1, &stmtHandle, &stmtTail);
	delete sql;
	PanicIfError(aDbHandle, err);
	TInt firstNamePrmIdx = sqlite3_bind_parameter_index(stmtHandle, KRawPrmName1);
	TEST(firstNamePrmIdx >= 0);
	TInt lastNamePrmIdx = sqlite3_bind_parameter_index(stmtHandle, KRawPrmName2);
	TEST(lastNamePrmIdx >= 0);
	TInt companyNamePrmIdx = sqlite3_bind_parameter_index(stmtHandle, KRawPrmName3);
	TEST(companyNamePrmIdx >= 0);
	TInt idIdx = sqlite3_bind_parameter_index(stmtHandle, KRawPrmName4);
	TEST(idIdx >= 0);
	
	TUint32 start = User::FastCounter();
	for(TInt id=1;id<=KTestTecordCount;++id)
		{
		TBuf8<20> buf1;
		buf1.Copy(KFirstName);
		buf1.AppendNum(id);
		buf1.Append(TChar(0));
		err = sqlite3_bind_text(stmtHandle, firstNamePrmIdx, (const char*)buf1.Ptr(), -1, SQLITE_STATIC);
		TEST2(err, SQLITE_OK);
		TBuf8<20> buf2;
		buf2.Copy(KLastName);
		buf2.AppendNum(id);
		buf2.Append(TChar(0));
		err = sqlite3_bind_text(stmtHandle, lastNamePrmIdx, (const char*)buf2.Ptr(), -1, SQLITE_STATIC);
		TEST2(err, SQLITE_OK);
		TBuf8<20> buf3;
		buf3.Copy(KCompanyName);
		buf3.AppendNum(id);
		buf3.Append(TChar(0));
		err = sqlite3_bind_text(stmtHandle, companyNamePrmIdx, (const char*)buf3.Ptr(), -1, SQLITE_STATIC);
		TEST2(err, SQLITE_OK);
		err = sqlite3_bind_int(stmtHandle, idIdx, id);
		TEST2(err, SQLITE_OK);
		while((err = sqlite3_step(stmtHandle)) == SQLITE_ROW)
			{
			}
		PanicIfError(aDbHandle, err);
		err = sqlite3_reset(stmtHandle);
		PanicIfError(aDbHandle, err);
		}
	TUint32 end = User::FastCounter();
	PrintStats(start, end);
	err = sqlite3_finalize(stmtHandle);
	TEST2(err, SQLITE_OK);
	}

//Explicit UpdateTest2() template specialization for UTF16 encoded SQL strings
template <> void UpdateTest2<HBufC16, TDesC16>(sqlite3* aDbHandle, const TDesC16& aUpdateSql)
	{
	TheTest.Printf(_L("\"Update\" test\r\n"));
	HBufC16* sql = HBufC16::New(aUpdateSql.Length() + 1);
	TEST(sql != NULL);
	sql->Des().Copy(aUpdateSql);
	sql->Des().Append(TChar(0));

	sqlite3_stmt* stmtHandle = NULL;
	const void* stmtTail = NULL;
	TInt err = sqlite3_prepare16_v2(aDbHandle, (const void*)sql->Des().Ptr(), -1, &stmtHandle, &stmtTail);
	delete sql;
	PanicIfError(aDbHandle, err);
	TInt firstNamePrmIdx = sqlite3_bind_parameter_index(stmtHandle, KRawPrmName1);
	TEST(firstNamePrmIdx >= 0);
	TInt lastNamePrmIdx = sqlite3_bind_parameter_index(stmtHandle, KRawPrmName2);
	TEST(lastNamePrmIdx >= 0);
	TInt companyNamePrmIdx = sqlite3_bind_parameter_index(stmtHandle, KRawPrmName3);
	TEST(companyNamePrmIdx >= 0);
	TInt idIdx = sqlite3_bind_parameter_index(stmtHandle, KRawPrmName4);
	TEST(idIdx >= 0);
	
	TUint32 start = User::FastCounter();
	for(TInt id=1;id<=KTestTecordCount;++id)
		{
		TBuf16<20> buf1;
		buf1.Copy(KFirstName);
		buf1.AppendNum(id);
		buf1.Append(TChar(0));
		err = sqlite3_bind_text16(stmtHandle, firstNamePrmIdx, (const void*)buf1.Ptr(), -1, SQLITE_STATIC);
		TEST2(err, SQLITE_OK);
		TBuf16<20> buf2;
		buf2.Copy(KLastName);
		buf2.AppendNum(id);
		buf2.Append(TChar(0));
		err = sqlite3_bind_text16(stmtHandle, lastNamePrmIdx, (const void*)buf2.Ptr(), -1, SQLITE_STATIC);
		TEST2(err, SQLITE_OK);
		TBuf16<20> buf3;
		buf3.Copy(KCompanyName);
		buf3.AppendNum(id);
		buf3.Append(TChar(0));
		err = sqlite3_bind_text16(stmtHandle, companyNamePrmIdx, (const void*)buf3.Ptr(), -1, SQLITE_STATIC);
		TEST2(err, SQLITE_OK);
		err = sqlite3_bind_int(stmtHandle, idIdx, id);
		TEST2(err, SQLITE_OK);
		while((err = sqlite3_step(stmtHandle)) == SQLITE_ROW)
			{
			}
		PanicIfError(aDbHandle, err);
		err = sqlite3_reset(stmtHandle);
		PanicIfError(aDbHandle, err);
		}
	TUint32 end = User::FastCounter();
	PrintStats(start, end);
	err = sqlite3_finalize(stmtHandle);
	TEST2(err, SQLITE_OK);
	}

//"UPDATE" test function (without parameters) - SQLITE
template <class DESC> void UpdateWPTest2(const TDesC& aDbName, const DESC& aUpdateSql);

//Explicit UpdateWPTest2() template specialization for UTF8 encoded SQL strings
template <> void UpdateWPTest2<TDesC8>(const TDesC& aDbName, const TDesC8& aUpdateSql)
	{
	TheTest.Printf(_L("\"Update (without parameters)\" test\r\n"));
	TBuf<200> fmtstr;
	fmtstr.Copy(aUpdateSql);

	sqlite3SymbianLibInit();
	sqlite3* dbHandle = TDbHelper2<TCmdLineParams::EDbUtf8>::Open(aDbName);
	
	TUint32 start = User::FastCounter();
	for(TInt id=1;id<=KTestTecordCount;++id)
		{
		TBuf<200> buf;
		buf.Format(fmtstr, &KFirstName2, id, &KLastName2, id, &KCompanyName2, id, id);
		TBuf8<200> sql;
		sql.Copy(buf);
		sql.Append(0);
		TInt err = sqlite3_exec(dbHandle, (const char*)sql.Ptr(), 0, 0, 0);
		TEST2(err, SQLITE_OK);
		}
	TUint32 end = User::FastCounter();
	PrintStats(start, end);
	
	TInt err2 = sqlite3_close(dbHandle);
	TEST2(err2, SQLITE_OK);
	sqlite3SymbianLibFinalize();
	CloseSTDLIB();
	}

//Explicit UpdateWPTest2() template specialization for UTF16 encoded SQL strings
template <> void UpdateWPTest2<TDesC16>(const TDesC& aDbName, const TDesC16& aUpdateSql)
	{
	TheTest.Printf(_L("\"Update (without parameters)\" test\r\n"));

	sqlite3SymbianLibInit();
	sqlite3* dbHandle = TDbHelper2<TCmdLineParams::EDbUtf16>::Open(aDbName);
	
	TUint32 start = User::FastCounter();
	for(TInt id=1;id<=KTestTecordCount;++id)
		{
		TBuf<200> sql;
		sql.Format(aUpdateSql, &KFirstName2, id, &KLastName2, id, &KCompanyName2, id, id);
		sql.Append(0);
		sqlite3_stmt* stmtHandle = NULL;
		const void* stmtTail = NULL;
		TInt err = sqlite3_prepare16_v2(dbHandle, sql.Ptr(), -1, &stmtHandle, &stmtTail);
		TEST2(err, SQLITE_OK);
		if(stmtHandle)	//stmtHandle can be NULL for statements like this: ";".
			{
			if(err == SQLITE_OK)
				{
				while((err = sqlite3_step(stmtHandle)) == SQLITE_ROW)
					{
					}
				}
			TInt err2 = sqlite3_finalize(stmtHandle);
			TEST2(err2, SQLITE_OK);
			TEST2(err, SQLITE_DONE);
			}
		}
	TUint32 end = User::FastCounter();
	PrintStats(start, end);
	
	TInt err2 = sqlite3_close(dbHandle);
	TEST2(err2, SQLITE_OK);
	sqlite3SymbianLibFinalize();
	CloseSTDLIB();
	}

template <class HBUFC, class DESC> void SelectTest2(sqlite3* aDbHandle, const DESC& aSelectSql);

//Explicit SelectTest2() template specialization for UTF8 encoded SQL strings
template <> void SelectTest2<HBufC8, TDesC8>(sqlite3* aDbHandle, const TDesC8& aSelectSql)
	{
	TheTest.Printf(_L("\"Select\" test\r\n"));
	HBufC8* sql = HBufC8::New(aSelectSql.Length() + 1);
	TEST(sql != NULL);
	sql->Des().Copy(aSelectSql);
	sql->Des().Append(TChar(0));

	sqlite3_stmt* stmtHandle = NULL;
	const char* stmtTail = NULL;
	TInt err = sqlite3_prepare_v2(aDbHandle, (const char*)sql->Des().Ptr(), -1, &stmtHandle, &stmtTail);
	delete sql;
	PanicIfError(aDbHandle, err);
	
	TUint32 start = User::FastCounter();
	while((err = sqlite3_step(stmtHandle)) == SQLITE_ROW)
		{
		TBuf8<20> buf;
		const unsigned char* p = sqlite3_column_text(stmtHandle, 0);
		buf.Copy(p, User::StringLength(p));
		TEST(buf.Length() > 0);
		p = sqlite3_column_text(stmtHandle, 1);
		buf.Copy(p, User::StringLength(p));
		TEST(buf.Length() > 0);
		p = sqlite3_column_text(stmtHandle, 2);
		buf.Copy(p, User::StringLength(p));
		TEST(buf.Length() > 0);
		}
	TEST2(err, SQLITE_DONE);
	TUint32 end = User::FastCounter();
	PrintStats(start, end);
	err = sqlite3_finalize(stmtHandle);
	TEST2(err, SQLITE_OK);
	}

//Explicit SelectTest2() template specialization for UTF16 encoded SQL strings
template <> void SelectTest2<HBufC16, TDesC16>(sqlite3* aDbHandle, const TDesC16& aSelectSql)
	{
	TheTest.Printf(_L("\"Select\" test\r\n"));
	HBufC16* sql = HBufC16::New(aSelectSql.Length() + 1);
	TEST(sql != NULL);
	sql->Des().Copy(aSelectSql);
	sql->Des().Append(TChar(0));

	sqlite3_stmt* stmtHandle = NULL;
	const void* stmtTail = NULL;
	TInt err = sqlite3_prepare16_v2(aDbHandle, (const void*)sql->Des().Ptr(), -1, &stmtHandle, &stmtTail);
	delete sql;
	PanicIfError(aDbHandle, err);
	
	TUint32 start = User::FastCounter();
	while((err = sqlite3_step(stmtHandle)) == SQLITE_ROW)
		{
		TBuf16<20> buf;
		const void* p = sqlite3_column_text16(stmtHandle, 0);
		buf.Copy((const TUint16*)p, User::StringLength((const TUint16*)p));
		TEST(buf.Length() > 0);
		p = sqlite3_column_text16(stmtHandle, 1);
		buf.Copy((const TUint16*)p, User::StringLength((const TUint16*)p));
		TEST(buf.Length() > 0);
		p = sqlite3_column_text16(stmtHandle, 2);
		buf.Copy((const TUint16*)p, User::StringLength((const TUint16*)p));
		TEST(buf.Length() > 0);
		}
	TEST2(err, SQLITE_DONE);
	TUint32 end = User::FastCounter();
	PrintStats(start, end);
	err = sqlite3_finalize(stmtHandle);
	TEST2(err, SQLITE_OK);
	}

template <class HBUFC, class DESC> void DeleteTest2(sqlite3* aDbHandle, const DESC& aDeleteSql);

//Explicit DeleteTest2() template specialization for UTF8 encoded SQL strings
template <> void DeleteTest2<HBufC8, TDesC8>(sqlite3* aDbHandle, const TDesC8& aDeleteSql)
	{
	TheTest.Printf(_L("\"Delete\" test\r\n"));
	HBufC8* sql = HBufC8::New(aDeleteSql.Length() + 1);
	TEST(sql != NULL);
	sql->Des().Copy(aDeleteSql);
	sql->Des().Append(TChar(0));
	TUint32 start = User::FastCounter();
	TInt err = sqlite3_exec(aDbHandle, (const char*)sql->Des().Ptr(), 0, 0, 0);
	TEST2(err, SQLITE_OK);
	TUint32 end = User::FastCounter();
	PrintStats(start, end);
	delete sql;
	}

//Explicit DeleteTest2() template specialization for UTF16 encoded SQL strings
template <> void DeleteTest2<HBufC16, TDesC16>(sqlite3* aDbHandle, const TDesC16& aDeleteSql)
	{
	TheTest.Printf(_L("\"Delete\" test\r\n"));
	HBufC16* sql = HBufC16::New(aDeleteSql.Length() + 1);
	TEST(sql != NULL);
	sql->Des().Copy(aDeleteSql);
	sql->Des().Append(TChar(0));
	TUint32 start = User::FastCounter();
	sqlite3_stmt* stmtHandle = NULL;
	const void* stmtTail = NULL;
	TInt err = sqlite3_prepare16_v2(aDbHandle, sql->Des().Ptr(), -1, &stmtHandle, &stmtTail);
	TEST2(err, SQLITE_OK);
	if(stmtHandle)	//stmtHandle can be NULL for statements like this: ";".
		{
		if(err == SQLITE_OK)
			{
			while((err = sqlite3_step(stmtHandle)) == SQLITE_ROW)
				{
				}
			}
		TInt err2 = sqlite3_finalize(stmtHandle);
		TEST2(err2, SQLITE_OK);
		TEST2(err, SQLITE_DONE);
		}
	TUint32 end = User::FastCounter();
	PrintStats(start, end);
	delete sql;
	}

template <TCmdLineParams::TDbEncoding TYPE, class HBUFC, class PTRC, class DESC> void PerformanceTest2(
																					   const TDesC& aDbFileName, 
																					   const DESC& aCommitStr,
																					   const DESC& aUpdateSql,
																					   const DESC& aSelectSql,
																					   const DESC& aDeleteSql)
	{
	CFileMan* fm = NULL;
	TRAPD(err, fm = CFileMan::NewL(TheFs));
	TEST2(err, KErrNone);
	
	sqlite3SymbianLibInit();
	TDbHelper2<TYPE>::Create(aDbFileName);
	
	sqlite3* dbHandle = TDbHelper2<TYPE>::Open(aDbFileName);
	InsertTest2<HBUFC, PTRC, DESC>(dbHandle, KFillDbScript(), aCommitStr);
	(void)sqlite3_close(dbHandle); dbHandle = NULL;
	PrintFileSize(aDbFileName);

	dbHandle = TDbHelper2<TYPE>::Open(aDbFileName);
	UpdateTest2<HBUFC, DESC>(dbHandle, aUpdateSql);
	(void)sqlite3_close(dbHandle); dbHandle = NULL;

	dbHandle = TDbHelper2<TYPE>::Open(aDbFileName);
	SelectTest2<HBUFC, DESC>(dbHandle, aSelectSql);
	(void)sqlite3_close(dbHandle); dbHandle = NULL;

	(void)fm->Copy(aDbFileName, TheNonSecureTmpDbName);
	
	dbHandle = TDbHelper2<TYPE>::Open(aDbFileName);
	DeleteTest2<HBUFC, DESC>(dbHandle, aDeleteSql);
	(void)sqlite3_close(dbHandle); dbHandle = NULL;
	PrintFileSize(aDbFileName);
	
	sqlite3SymbianLibFinalize();
	CloseSTDLIB();

	(void)fm->Copy(TheNonSecureTmpDbName, aDbFileName);
	(void)fm->Delete(TheNonSecureTmpDbName);
	delete fm;
	}
//Explicit PerformanceTest2() template instantiations.
template void PerformanceTest2<TCmdLineParams::EDbUtf8, HBufC8, TPtrC8, TDesC8>(const TDesC&, const TDesC8&, const TDesC8&, const TDesC8&, const TDesC8&);
template void PerformanceTest2<TCmdLineParams::EDbUtf16, HBufC8, TPtrC8, TDesC8>(const TDesC&, const TDesC8&, const TDesC8&, const TDesC8&, const TDesC8&);
template void PerformanceTest2<TCmdLineParams::EDbUtf8, HBufC16, TPtrC16, TDesC16>(const TDesC&, const TDesC16&, const TDesC16&, const TDesC16&, const TDesC16&);
template void PerformanceTest2<TCmdLineParams::EDbUtf16, HBufC16, TPtrC16, TDesC16>(const TDesC&, const TDesC16&, const TDesC16&, const TDesC16&, const TDesC16&);

void ColumnValueAccessTest()
	{
	_LIT(KColName1, "Column1");
	_LIT(KColName2, "Column2");
	_LIT(KColName3, "Column3");
	_LIT(KColName4, "Column4");
	const TInt KColCount = 4;
	TPtrC colNames[KColCount] = {KColName1(), KColName2(), KColName3(), KColName4()};
	TBuf<100> sql;
	
	//Create a test database
	RSqlDatabase db;
	TInt err = db.Create(TheNonSecureDbName2, &TheSqlConfigString);
	TEST2(err, KErrNone);
	_LIT(KCreateSql, "CREATE TABLE A(%S INTEGER, %S INTEGER, %S INTEGER, %S INTEGER)");
	sql.Format(KCreateSql, &colNames[0], &colNames[1], &colNames[2], &colNames[3]);
	err = db.Exec(sql);
	TEST(err >= 0);	
	//Insert a record in the test database
	_LIT(KInsertSql, "INSERT INTO A(%S, %S, %S, %S) VALUES(0, 1, 2, 3)");
	sql.Format(KInsertSql, &colNames[0], &colNames[1], &colNames[2], &colNames[3]);
	err = db.Exec(sql);
	TEST2(err, 1);
	//Retrieve the record
	RSqlStatement stmt;
	err = stmt.Prepare(db, _L8("SELECT * FROM A"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	
	//Init the random numbers generator
	TInt64 seed = (TInt64)&ColumnValueAccessTest;
	const TInt KTestCount = 20000;
	TInt val;
	
	//Test 1: retrieve column values using the column index
	TUint32 start = User::FastCounter();
	for(TInt i1=0;i1<KTestCount;++i1)	
		{
		TInt colIdx = Math::Rand(seed) % KColCount;
		val = stmt.ColumnInt(colIdx);
		TEST(val == colIdx);
		}
	TUint32 end = User::FastCounter();
	PrintStats(start, end);
	
	//Test 2: retrieve column values using the column name
	start = User::FastCounter();
	for(TInt i2=0;i2<KTestCount;++i2)	
		{
		TInt colNameIdx = Math::Rand(seed) % KColCount;
		TInt colIdx = stmt.ColumnIndex(colNames[colNameIdx]);
		val = stmt.ColumnInt(colIdx);
		TEST(val == colIdx);
		}
	end = User::FastCounter();
	PrintStats(start, end);
	
	//Cleanup
	stmt.Close();
	db.Close();
	(void)RSqlDatabase::Delete(TheNonSecureDbName2);
	}

///////////////////////////////////////////////////////////////////////////////////
////////////      "Single record" INSERT/UPDATE/DELETE tests       ////////////////
///////////////////////////////////////////////////////////////////////////////////

_LIT(KZDbName, "z:\\test\\default_avacon.dbSQL");

RSqlDatabase TheDb;

void OpenTestDb()
	{
	(void)RSqlDatabase::Delete(TheSglRecDbFileName);
	TInt err = RSqlDatabase::Copy(KZDbName, TheSglRecDbFileName);
	TEST2(err, KErrNone);
	err = TheDb.Open(TheSglRecDbFileName);
	TEST2(err, KErrNone);
	}
	
void CloseTestDb()
	{
	TheDb.Close();
	TInt err = RSqlDatabase::Delete(TheSglRecDbFileName);
	TEST2(err, KErrNone);
	}

void SingleUpdateTest()
	{
	OpenTestDb();
	
	_LIT(KUpdateSql, "UPDATE Table1 SET i2=41 WHERE i1 = 1");
	
	TUint32 start = User::FastCounter();
	TInt err = TheDb.Exec(KUpdateSql);
	TUint32 end = User::FastCounter();
	TEST2(err, 1);

	PrintStats(start, end);
	
	CloseTestDb();
	}

void SingleInsertTest()
	{
	OpenTestDb();

	_LIT(KInsertSql, "INSERT INTO Table1 (i1, i2, i3, i4, r5, t6, t7, t8, lt9, lb10) values(1001, 1, 2002, 1001, 1001.001001, '', 'attic attic baker', 'attic attic baker attic attic baker attic attic baker attic attic baker ', 'ATTIC ATTIC BAKER ATTIC ATTIC BAKER ATTIC ATTIC BAKER ATTIC ATTIC BAKER ', NULL)");
	
	TUint32 start = User::FastCounter();
	TInt err = TheDb.Exec(KInsertSql);
	TUint32 end = User::FastCounter();
	TEST2(err, 1);

	PrintStats(start, end);
	
	CloseTestDb();
	}

void SingleDeleteTest()
	{
	OpenTestDb();

	_LIT(KDeleteSql, "Delete from Table1 where i1 = 1");
	
	TUint32 start = User::FastCounter();
	TInt err = TheDb.Exec(KDeleteSql);
	TUint32 end = User::FastCounter();
	TEST2(err, 1);

	PrintStats(start, end);
	
	CloseTestDb();
	}
	
void SelectLikeQueryPerfTest()
{
	_LIT8(KServerConfigString1, "encoding =  \"UTF-8\"");
	//create a sql db file
	(void)RSqlDatabase::Delete(TheNonSecureDbName);
	TInt err = TheDb.Create(TheNonSecureDbName, &KServerConfigString1);
	TEST2(err, KErrNone);
	
	//Create a test table and insert some records
	err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER PRIMARY KEY, Name TEXT)"));
	TEST(err >= 0);
	
	//Add 100 records to the table
	for(TInt k=0;k<100;++k)
		{
		TBuf<100> sql;
		sql.Format(_L("INSERT INTO A VALUES(%d, 'Dvorák')"), k + 1);
		err = TheDb.Exec(sql);
		TEST2(err, 1);
		}
	
	//Update one record  
	err = TheDb.Exec(_L("UPDATE A SET Name='dvorak1' WHERE ID = 50"));
	TEST2(err, 1);

	TUint32 start = User::FastCounter();
	
	//Create a statement object and issue a SELECT SQL statement + LIKE clause
	//Test case 1 = full name search with LIKE
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb, _L("SELECT COUNT(*) FROM A WHERE Name LIKE 'DVORAK1'"));
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TInt cnt = stmt.ColumnInt(0);
	TEST2(cnt, 1);
	
	stmt.Close();
	
	TUint32 end = User::FastCounter();
	TEST2(err, 1);
	PrintStats(start, end);
	
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheNonSecureDbName);
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1648
@SYMTestCaseDesc		SQL database performance tests.
						Three test types used: "insert records", "update records" and "select records".
						The tests are executed on: secure database, non-secure database, client side database.
						Each test executed twice using UTF8 and UTF16 encoded SQL statements.
@SYMTestPriority		High
@SYMTestActions			SQL database performance tests.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
                        REQ5912
                        REQ10273
*/	
void DoTests()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1648 SQL performance tests"));
	
	TheTest.Printf(_L("Single \"update\" test\r\n"));
	SingleUpdateTest();
	
	TheTest.Printf(_L("Single \"insert\" test\r\n"));
	SingleInsertTest();
	
	TheTest.Printf(_L("Single \"delete\" test\r\n"));
	SingleDeleteTest();
	
	TheTestTitle.Format(_L("SERVER, UTF8 SQL strings, non-secure, encoding: \"%S\", page size: %d\r\n"), 
			TheCmdLineParams.iDbEncoding == TCmdLineParams::EDbUtf16 ? &KUtf16 : &KUtf8, TheCmdLineParams.iPageSize);
	TheTest.Printf(TheTestTitle);
	PerformanceTest<HBufC8, TPtrC8, TDesC8, ENonSecureDb>(TheNonSecureDbName, KCommitStr8(), KUpdateSql8(), KSelectSql8(), KDeleteSql8());

	TheTestTitle.Format(_L("SERVER, UTF8 SQL strings, non-secure, update test (without parameters), encoding: \"%S\", page size: %d\r\n"), 
			TheCmdLineParams.iDbEncoding == TCmdLineParams::EDbUtf16 ? &KUtf16 : &KUtf8, TheCmdLineParams.iPageSize);
	TheTest.Printf(TheTestTitle);
	UpdateWPTest<TBuf8<200>, TDesC8, ENonSecureDb>(TheNonSecureDbName, KUpdateSql2_8());

	TheTestTitle.Format(_L("SERVER, UTF8 SQL strings, secure, encoding: \"%S\", page size: %d\r\n"), 
			TheCmdLineParams.iDbEncoding == TCmdLineParams::EDbUtf16 ? &KUtf16 : &KUtf8, TheCmdLineParams.iPageSize);
	TheTest.Printf(TheTestTitle);
	PerformanceTest<HBufC8, TPtrC8, TDesC8, ESecureDb>(TheSecureDbName, KCommitStr8(), KUpdateSql8(), KSelectSql8(), KDeleteSql8());

	TheTest.Printf(_L("SQLITE, UTF8 encoded database, UTF8 SQL strings\r\n"));
	PerformanceTest2<TCmdLineParams::EDbUtf8, HBufC8, TPtrC8, TDesC8>(TheNonSecureDbName, KCommitStr8(), KUpdateSql8(), KSelectSql8(), KDeleteSql8());

	TheTest.Printf(_L("SQLITE, UTF8 encoded database, UTF8 SQL strings, update test (without parameters)\r\n"));
	UpdateWPTest2<TDesC8>(TheNonSecureDbName, KUpdateSql2_8());

	TheTest.Printf(_L("SQLITE, UTF16 encoded database, UTF8 SQL strings\r\n"));
	PerformanceTest2<TCmdLineParams::EDbUtf16, HBufC8, TPtrC8, TDesC8>(TheNonSecureDbName, KCommitStr8(), KUpdateSql8(), KSelectSql8(), KDeleteSql8());

	TheTestTitle.Format(_L("SERVER, UTF16 SQL strings, non-secure, encoding: \"%S\", page size: %d\r\n"), 
			TheCmdLineParams.iDbEncoding == TCmdLineParams::EDbUtf16 ? &KUtf16 : &KUtf8, TheCmdLineParams.iPageSize);
	TheTest.Printf(TheTestTitle);
	PerformanceTest<HBufC16, TPtrC16, TDesC16, ENonSecureDb>(TheNonSecureDbName, KCommitStr16(), KUpdateSql16(), KSelectSql16(), KDeleteSql16());
	
	TheTestTitle.Format(_L("SERVER, UTF16 SQL strings, non-secure, update test (without parameters), encoding: \"%S\", page size: %d\r\n"), 
			TheCmdLineParams.iDbEncoding == TCmdLineParams::EDbUtf16 ? &KUtf16 : &KUtf8, TheCmdLineParams.iPageSize);
	TheTest.Printf(TheTestTitle);
	UpdateWPTest<TBuf16<200>, TDesC16, ENonSecureDb>(TheNonSecureDbName, KUpdateSql2_16());

	TheTestTitle.Format(_L("SERVER, UTF16 SQL strings, secure, encoding: \"%S\", page size: %d\r\n"), 
			TheCmdLineParams.iDbEncoding == TCmdLineParams::EDbUtf16 ? &KUtf16 : &KUtf8, TheCmdLineParams.iPageSize);
	TheTest.Printf(TheTestTitle);
	PerformanceTest<HBufC16, TPtrC16, TDesC16, ESecureDb>(TheSecureDbName, KCommitStr16(), KUpdateSql16(), KSelectSql16(), KDeleteSql16());

	TheTest.Printf(_L("SQLITE, UTF8 encoded database, UTF16 SQL strings\r\n"));
	PerformanceTest2<TCmdLineParams::EDbUtf8, HBufC16, TPtrC16, TDesC16>(TheNonSecureDbName, KCommitStr16(), KUpdateSql16(), KSelectSql16(), KDeleteSql16());

	TheTest.Printf(_L("SQLITE, UTF16 encoded database, UTF16 SQL strings\r\n"));
	PerformanceTest2<TCmdLineParams::EDbUtf16, HBufC16, TPtrC16, TDesC16>(TheNonSecureDbName, KCommitStr16(), KUpdateSql16(), KSelectSql16(), KDeleteSql16());

	TheTest.Printf(_L("SQLITE, UTF16 encoded database, UTF16 SQL strings, update test (without parameters)\r\n"));
	UpdateWPTest2<TDesC16>(TheNonSecureDbName, KUpdateSql2_16());

	TheTest.Printf(_L("Accessing column value by index or by name\r\n"));
	ColumnValueAccessTest();
	
	TheTest.Printf(_L("Retrieving data from UTF8 Database using SELECT LIKE statements\r\n"));
	SelectLikeQueryPerfTest();

	}

TInt E32Main()
	{
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);

	GetCmdLineParamsAndSqlConfigString(TheTest, _L("t_sqlperformance"), TheCmdLineParams, TheSqlConfigString);
	_LIT(KSecureDbName, "c:[2121212A]t_perfdb.db");
	PrepareDbName(KSecureDbName, TheCmdLineParams.iDriveName, TheSecureDbName);
	_LIT(KNonSecureDbName, "c:\\test\\t_perfdb.db");
	PrepareDbName(KNonSecureDbName, TheCmdLineParams.iDriveName, TheNonSecureDbName);
	_LIT(KNonSecureDbName2, "c:\\test\\t_perfdb2.db");
	PrepareDbName(KNonSecureDbName2, TheCmdLineParams.iDriveName, TheNonSecureDbName2);
	_LIT(KNonSecureTmpDbName, "c:\\test\\tmp.db");
	PrepareDbName(KNonSecureTmpDbName, TheCmdLineParams.iDriveName, TheNonSecureTmpDbName);
	_LIT(KSglRecDbName, "c:\\test\\default_avacon.dbSQL");
	PrepareDbName(KSglRecDbName, TheCmdLineParams.iDriveName, TheSglRecDbFileName);

	TheTest.Printf(_L("==Databases: %S, %S, %S, %S, %S\r\n"), &TheSecureDbName, &TheNonSecureDbName, 
															  &TheNonSecureDbName2, &TheNonSecureTmpDbName, 
															  &TheSglRecDbFileName);
	
	__UHEAP_MARK;
	
	TestEnvInit();
	DoTests();
	TestEnvDestroy();

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
