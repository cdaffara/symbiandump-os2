// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <f32file.h>
#include <e32test.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <hal.h>
#include <utf.h>				//CnvUtfConverter
#include "sqlite3.h"
#include "SqliteSymbian.h"

#define UNUSED_VAR(a) a = (a)

//Forward declaration
struct Mem;

static RTest 			TheTest(_L ("t_sqlite.exe"));
static CTrapCleanup*	TheTrapCleanup = NULL;
static RFs 				TheFs;

//File-local buffer used for converted to UTF16 (from UTF8) strings or
//for a temporary file name buffer.
static TUint16 TheFileName[KMaxFileName + 1];

_LIT(KTestDir, "c:\\test\\");

const char KDbName1[] = "c:\\test\\t_sqlite1.db";
const char KDbName2[] = "c:\\test\\t_sqlite2.db";
_LIT(KContactsFile, "c:\\test\\Contacts.DB");

sqlite3 *TheDb1 = NULL, *TheDb2 = NULL;

static TBuf<4096> TheBuf1;
static TBuf<4096> TheBuf2;
static TBuf8<512> TheBuf3;

static TInt TheCmpFCallCnt = 0;
static TInt TheCmpCCallCnt = 0;

_LIT(KSimpleContactsSqlFile, "z:\\test\\add_simple_contacts.sql");

//In order to be able to compile the test, the following variables are defined (used inside the OS porting layer, when _SQLPROFILER macro is defined)
#ifdef _SQLPROFILER
TInt TheSqlSrvProfilerFileRead = 0;
TInt TheSqlSrvProfilerFileWrite = 0;
TInt TheSqlSrvProfilerFileSync = 0;
TInt TheSqlSrvProfilerFileSetSize = 0;
#endif

///////////////////////////////////////////////////////////////////////////////////////

//Creates TPtrC object which points to the unicode presentation of aData.
//aData argument is expected to point to UTF8 encoded, zero terminated string.
//The function returns a TPtrC, non-zero terminated object pointing to the unicode presentation of aData.
//If the length of the returned TPtrC object is 0 - it means that unicode conversion of aData failed, 
//because the aData is too long or is NULL. 
//Max allowed aData length is KMaxFileName (excluding terminating 0 character).
static TPtrC ConvertToUtf16(const char *aData)
	{
	TPtrC ptr(0, 0);
	if(aData)
		{
		TInt len = mbstowcs(reinterpret_cast <wchar_t*> (TheFileName), aData, KMaxFileName + 1);
		//Check converted string length. If it is longer than KMaxFileName characters, then aData is too long.
		if(len > 0 && len <= KMaxFileName)
			{
			ptr.Set(TheFileName, len);
			}
		}
	return ptr;
	}

///////////////////////////////////////////////////////////////////////////////////////

static void DeleteTestFiles()
	{
   	sqlite3_close(TheDb2);
   	sqlite3_close(TheDb1);
   	
	TheFs.Delete(ConvertToUtf16(KDbName2));
	TheFs.Delete(ConvertToUtf16(KDbName1));
	TheFs.Delete(KContactsFile);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteTestFiles();
		TheTest(EFalse, aLine);
		}
	}
static void Check(TInt aValue, TInt aExpected, TInt aLine)
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

static void CreatePrivateDirs()
	{
	RFs fs;
	TEST2(fs.Connect(), KErrNone);
	for(TInt i=0;i<('Z'-'A');++i)
		{
		fs.CreatePrivatePath(i);
		}
	fs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////

static void PrintConfig(const TDesC& aDbFilePath)
	{
	RDebug::Print(_L("================= Configuration ================\r\n"));	
	RDebug::Print(_L("Cache page size %dK, pages %d, total %dK\r\n"), SQLITE_DEFAULT_PAGE_SIZE/1024, SQLITE_DEFAULT_CACHE_SIZE, SQLITE_DEFAULT_PAGE_SIZE * SQLITE_DEFAULT_CACHE_SIZE/1024);	
	RDebug::Print(_L("Temp cache page size %dK, pages %d, total %dK\r\n"), SQLITE_DEFAULT_PAGE_SIZE/1024, SQLITE_DEFAULT_TEMP_CACHE_SIZE, SQLITE_DEFAULT_PAGE_SIZE * SQLITE_DEFAULT_TEMP_CACHE_SIZE/1024);	
	_LIT(K1, "On");
	_LIT(K2, "Off");
	RDebug::Print(_L("Autovacuum: %S\r\n"), SQLITE_DEFAULT_AUTOVACUUM ? &K1 : &K2);	
	#ifdef SQLITE_DEBUG
		RDebug::Print(_L("Debug: On\r\n"));	
	#else
		RDebug::Print(_L("Debug: Off\r\n"));	
	#endif
	RDebug::Print(_L("Db file: %S\r\n"), &aDbFilePath);
	RDebug::Print(_L("================================================\r\n"));	
	}
	
///////////////////////////////////////////////////////////////////////////////////////

static TInt callback(void */*NotUsed*/, TInt argc, char **argv, char **azColName)
	{
	TInt i;
	for(i=0; i<argc; i++)
		{
		TPtrC8 colName((const TUint8*)azColName[i]);
		TPtrC8 colVal((const TUint8*)(argv[i] ? argv[i] : "NULL"));
		TheBuf2.Copy(colName);
		TheBuf2.Append(_L(" = "));
		TheBuf1.Copy(colVal);
		TheBuf2.Append(TheBuf1);
    	RDebug::Print(_L("%S\r\n"), &TheBuf2);
		}
  	RDebug::Print(_L("\n"));
  	return 0;
	}

static TInt Compare(void*, TInt size1, const void* p1, TInt size2, const void* p2)
	{
	TPtrC8 ptr1((TUint8*)p1, size1);
	TPtrC8 ptr2((TUint8*)p2, size2);
	
	TInt res = ptr1.CompareF(ptr2);
	return res;		
	}

static TInt Compare2(void*, TInt size1, const void* p1, TInt size2, const void* p2)
	{
	TPtrC8 ptr1((TUint8*)p1, size1);
	TPtrC8 ptr2((TUint8*)p2, size2);
	
	TInt res = -ptr1.Compare(ptr2);
	return res;		
	}


static void Func(sqlite3_context* ctx, TInt nCnt, sqlite3_value** pp)
	{
	for(TInt i=0;i<nCnt;++i)
		{
		TInt v = sqlite3_value_int(*pp++);
		v = v;
		}
	sqlite3_result_int(ctx, 564);		
	}

///////////////////////////////////////////////////////////////////////////////////////

//Create database, table, view, INSERT, SELECT, DELETE sql statements, register collations, 
//user defined functions, CREATE INDEX, CREATE VIEW sql statements, GROUP BY, HAVING, EXCEPT,...
static void DoTests1()
	{
	char *zErrMsg = 0;
	TInt rc;

	rc = sqlite3_open(KDbName1, &TheDb1);
  	if(rc)
  		{
  		TPtrC p = ConvertToUtf16(sqlite3_errmsg(TheDb1));
  		RDebug::Print(_L("Can't open database, err %d, msg: %S\n"), rc, &p);
    	TEST(0);
  		}

	rc = sqlite3_create_collation(TheDb1, "Clt1", SQLITE_UTF8, NULL, &Compare);
  	if(rc)
  		{
  		TPtrC p = ConvertToUtf16(sqlite3_errmsg(TheDb1));
  		RDebug::Print(_L("Can't create collation, err %d, msg: %S\n"), rc, &p);
    	TEST(0);
  		}

	rc = sqlite3_create_collation(TheDb1, "Clt2", SQLITE_UTF8, NULL, &Compare2);
  	if(rc)
  		{
  		TPtrC p = ConvertToUtf16(sqlite3_errmsg(TheDb1));
  		RDebug::Print(_L("Can't create collation, err %d, msg: %S\n"), rc, &p);
    	TEST(0);
  		}

	rc = sqlite3_create_function(TheDb1, "Func", 2, SQLITE_UTF8, NULL, Func, NULL, NULL);
  	if(rc)
  		{
  		TPtrC p = ConvertToUtf16(sqlite3_errmsg(TheDb1));
  		RDebug::Print(_L("Can't create UDF, err %d, msg: %S\n"), rc, &p);
    	TEST(0);
  		}

	rc = sqlite3_exec(TheDb1, "\
	             BEGIN TRANSACTION;\
	             CREATE TABLE Pn(Id INTEGER PRIMARY KEY AUTOINCREMENT,\
	                             Name TEXT NOT NULL,\
	                             Surname TEXT NOT NULL COLLATE Clt1);\
	             CREATE INDEX PnSurname ON Pn(Surname);\
	             CREATE TABLE Addr(Id INTEGER PRIMARY KEY AUTOINCREMENT,\
	                               PnId INTEGER,\
	                               CityId INTEGER,\
	                               Location TEXT DEFAULT 'address');\
	             CREATE INDEX AddrPnId ON Addr(PnId);\
	             CREATE INDEX AddrCityId ON Addr(CityId);\
	             CREATE TABLE City(ID INTEGER PRIMARY KEY AUTOINCREMENT, Name TEXT Default 'None');\
	             CREATE TABLE Shop(Id INTEGER, Name TEXT);\
	             CREATE TABLE Sales(ShopId INTEGER, M MONEY, D TIMESTAMP, D1 DATE, T1 TIME);\
	             CREATE TRIGGER TrgPnDel BEFORE DELETE ON Pn \
	             BEGIN \
					SELECT Func(2,8);\
	                SELECT CASE WHEN ((SELECT COUNT(*) FROM Addr WHERE Addr.PnId = old.Id) > 0) \
	                            THEN RAISE (ABORT, 'Foreign key constraint violation!!!') \
	                END;\
	             END;\
	             CREATE VIEW V1 AS SELECT * FROM Pn;\
	             CREATE VIEW V2 AS SELECT * FROM Addr;\
	             CREATE VIEW V3 AS SELECT * FROM Sales;\
	             CREATE VIEW V4 AS SELECT * FROM Shop;\
	             COMMIT TRANSACTION;", callback, 0, &zErrMsg);
	             
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
    	TEST(0);
		}

	rc = sqlite3_exec(TheDb1, 
	"BEGIN TRANSACTION;\
	 INSERT INTO Pn(Name, Surname) VALUES('Aaaa1', 'Aaaa2');\
	 INSERT INTO Pn(Name, Surname) VALUES('Bbbb1', 'Bbbb2');\
	 INSERT INTO Pn(Name, Surname) VALUES('Cccc1', 'Cccc2');\
	 INSERT INTO Pn(Name, Surname) VALUES('Dddd1', 'Dddd2');\
	 INSERT INTO City(Name) VALUES('London');\
	 INSERT INTO City(Name) VALUES('Manchester');\
	 INSERT INTO Addr(PnId, CityId, Location) SELECT Pn.Id, City.Id, 'Here' FROM Pn, City WHERE Pn.Surname = 'Cccc2' AND City.Name = 'London';\
	 INSERT INTO Addr(PnId, CityId, Location) SELECT Pn.Id, City.Id, 'There' FROM Pn, City WHERE Pn.Surname = 'Bbbb2' AND City.Name = 'Manchester';\
	 INSERT INTO Shop(Id, Name) VALUES(1, 'Shop-1');\
	 INSERT INTO Shop(Id, Name) VALUES(2, 'Shop-2');\
	 INSERT INTO Shop(Id, Name) VALUES(3, 'Shop-3');\
	 INSERT INTO Sales(ShopId, M, D, D1, T1) VALUES(1, 123.0, '2005-01-01', CURRENT_DATE, '12:34:17');\
	 INSERT INTO Sales(ShopId, M, D, D1, T1) VALUES(2, 100.0, '2005-01-01', '2005-01-27', '12:34:18');\
	 INSERT INTO Sales(ShopId, M, D, D1, T1) VALUES(2, 200.0, '2005-01-02', '2005-01-28', '12:34:19');\
	 INSERT INTO Sales(ShopId, M, D, D1, T1) VALUES(3, 200.0, '2005-01-03', '2005-01-29', '12:34:23');\
	 COMMIT TRANSACTION;", callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
    	TEST(0);
		}

	rc = sqlite3_exec(TheDb1, "SELECT COUNT(*) FROM Shop", callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
    	TEST(0);
		}

	for(TInt i=0;i<500;++i)
		{
		_LIT8(KSqlStmt, "INSERT INTO Shop(Id, Name) VALUES(%d, 'Shop-%d')\x0");
		TheBuf3.Format(KSqlStmt, i+1, i+1);
		rc = sqlite3_exec(TheDb1, (const char*)TheBuf3.Ptr(), callback, 0, &zErrMsg);
		if(rc != SQLITE_OK)
			{
	  		TPtrC p = ConvertToUtf16(zErrMsg);
	  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
	    	sqlite3_free(zErrMsg);
    		TEST(0);
			}
		}

	_LIT8(KSqlStmt2, "DELETE FROM Shop WHERE Id > 400\x0");
	rc = sqlite3_exec(TheDb1, (const char*)KSqlStmt2().Ptr(), callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
   		TEST(0);
		}

	_LIT8(KSqlStmt3, "SELECT COUNT(*) FROM Shop\x0");
	rc = sqlite3_exec(TheDb1, (const char*)KSqlStmt3().Ptr(), callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
   		TEST(0);
		}

	rc = sqlite3_exec(TheDb1, "SELECT Pn.Surname, Addr.Location, City.Name FROM Pn \
	      INNER JOIN Addr ON Pn.Id = Addr.PnId \
	      INNER JOIN City ON Addr.CityId = City.Id \
	      ORDER BY Surname COLLATE Clt2", callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
   		TEST(0);
		}
	      
	rc = sqlite3_exec(TheDb1, "SELECT * FROM Addr", callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
   		TEST(0);
		}
	
	rc = sqlite3_exec(TheDb1, "SELECT * FROM Sales WHERE D1 > '2005-08-27'", callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
   		TEST(0);
		}
	
	rc = sqlite3_exec(TheDb1, "SELECT V1.* FROM V1 EXCEPT SELECT V1.* FROM V1 WHERE V1.Id = 2", callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
   		TEST(0);
		}

	rc = sqlite3_exec(TheDb1, "SELECT V3.D, SUM(V3.M) FROM V3 GROUP BY V3.D HAVING SUM(V3.M) > 210.0", callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
   		TEST(0);
		}
	
	rc = sqlite3_exec(TheDb1, "SELECT V4.Name, SUM(V3.M) FROM V4, V3 WHERE V4.Id = V3.ShopId GROUP BY V4.Id", callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
   		TEST(0);
		}

	sqlite3_close(TheDb1);
	TheFs.Delete(ConvertToUtf16(KDbName1));
	}

//Generic SQL tests.
static void DoTests2()
	{
	TheFs.Delete(ConvertToUtf16(KDbName1));
	
	char *zErrMsg = 0;
	TInt rc;

	rc = sqlite3_open(KDbName1, &TheDb1);
  	if(rc != SQLITE_OK)
  		{
  		TPtrC p = ConvertToUtf16(sqlite3_errmsg(TheDb1));
  		RDebug::Print(_L("Can't open database, err %d, msg: %S\n"), rc, &p);
    	TEST(0);
  		}
  		
	rc = sqlite3_exec(TheDb1, "PRAGMA auto_vacuum = 1", callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
   		TEST(0);
		}
	
	rc = sqlite3_exec(TheDb1, "\
	             BEGIN TRANSACTION;\
	             CREATE TABLE Shop(Id INTEGER, Name TEXT);\
	             CREATE INDEX ShopName ON Shop(Name);\
	             COMMIT TRANSACTION;", callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
   		TEST(0);
		}
		
	rc = sqlite3_exec(TheDb1, "BEGIN TRANSACTION", callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
   		TEST(0);
		}
	
	for(TInt i=0;i<1000;++i)
		{
		_LIT8(KSqlStmt, "INSERT INTO Shop(Id, Name) VALUES(%d, 'Shop-%d')\x0");
		TheBuf3.Format(KSqlStmt, i, i);
		rc = sqlite3_exec(TheDb1, (const char*)TheBuf3.Ptr(), callback, 0, &zErrMsg);
		if(rc != SQLITE_OK)
			{
	  		TPtrC p = ConvertToUtf16(zErrMsg);
	  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
	    	sqlite3_free(zErrMsg);
	   		TEST(0);
			}
		}
		
	rc = sqlite3_exec(TheDb1, "COMMIT TRANSACTION", callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
   		TEST(0);
		}

	rc = sqlite3_exec(TheDb1, "SELECT COUNT(*) FROM Shop", callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
   		TEST(0);
		}

	rc = sqlite3_exec(TheDb1, "BEGIN TRANSACTION", callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
   		TEST(0);
		}
	
	rc = sqlite3_exec(TheDb1, "DELETE FROM Shop WHERE Id > 100", callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
   		TEST(0);
		}
		
	rc = sqlite3_exec(TheDb1, "COMMIT TRANSACTION", callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
   		TEST(0);
		}

	rc = sqlite3_exec(TheDb1, "SELECT COUNT(*) FROM Shop", callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
   		TEST(0);
		}

	sqlite3_close(TheDb1);
	TheFs.Delete(ConvertToUtf16(KDbName1));
	}

//Accented column names test
static void AccentedColumnNamesTestL()
	{
	TheFs.Delete(ConvertToUtf16(KDbName1));

	//Open database
	TPtrC tmp = ConvertToUtf16(KDbName1);
	TBuf<KMaxFileName + 1> fname;
	fname.Copy(tmp);
	TInt rc = sqlite3_open16(fname.PtrZ(), &TheDb1);//!!!!16-bit encoding!!!!!
  	if(rc != SQLITE_OK)
  		{
  		TPtrC p = ConvertToUtf16(sqlite3_errmsg(TheDb1));
  		RDebug::Print(_L("Can't open database, err %d, msg: %S\n"), rc, &p);
    	TEST(0);
  		}
	
	//Create table
	_LIT(KSqlStrZ, "CREATE TABLE abc(col_\u00C4 integer, col_A text)\0");
//	_LIT(KSqlStrZ, "CREATE TABLE abc(col_a integer, col_A text)\0");
//	_LIT(KSqlStrZ, "CREATE TABLE abc(col_Ä integer, col_ä text)\0");
//	_LIT(KSqlStrZ, "CREATE TABLE abc(col_Ä integer, col_A\x308 text)\0");
	sqlite3_stmt* stmtHandle = NULL;
	const void* stmtTailZ = NULL;
	rc = sqlite3_prepare16_v2(TheDb1, KSqlStrZ().Ptr(), -1, &stmtHandle, &stmtTailZ);
	if(rc != SQLITE_OK)
		{
		const void* errMsgZ = sqlite3_errmsg16(TheDb1);
		TPtrC msg(reinterpret_cast <const TText16*> (errMsgZ), wcslen(reinterpret_cast <const wchar_t*> (errMsgZ)));
		RDebug::Print(_L("'sqlite3_prepare16_v2()' failed, err %d, error msg: \"%S\"\r\n"), rc, &msg);
    	TEST(0);
		}
	rc = sqlite3_step(stmtHandle);
	TEST(rc == SQLITE_DONE);

	sqlite3_finalize(stmtHandle);
	stmtHandle = NULL;

	//Select from the table
	_LIT(KSqlStrZ2, "SELECT * FROM abc WHERE :prm_\u00C4 = col_\u00C4 and :prm_\u00E4 = col_A and :prm_A = col_A and :prm_a = col_\u00C4\0");
	rc = sqlite3_prepare16_v2(TheDb1, KSqlStrZ2().Ptr(), -1, &stmtHandle, &stmtTailZ);
	if(rc != SQLITE_OK)
		{
		const void* errMsgZ = sqlite3_errmsg16(TheDb1);
		TPtrC msg(reinterpret_cast <const TText16*> (errMsgZ), wcslen(reinterpret_cast <const wchar_t*> (errMsgZ)));
		RDebug::Print(_L("'sqlite3_prepare16_v2()' failed, err %d, error msg: \"%S\"\r\n"), rc, &msg);
    	TEST(0);
		}

	//parameter checks
	TBuf<16> prmNames[5];
 	TInt prmCount = sqlite3_bind_parameter_count(stmtHandle);
 	for(TInt i=1;i<=prmCount;++i)
 		{
 		const char* prmNameZ = sqlite3_bind_parameter_name(stmtHandle, i);
		TPtrC8 name8(reinterpret_cast <const TUint8*> (prmNameZ), strlen(prmNameZ));
		HBufC* name = CnvUtfConverter::ConvertToUnicodeFromUtf8L(name8);
		prmNames[i] = *name;
		delete name;
 		}
 	TInt prmIndex0_1 = sqlite3_bind_parameter_index(stmtHandle, ":prm_Ä");
 	prmIndex0_1 = prmIndex0_1;
 	TInt prmIndex1_1 = sqlite3_bind_parameter_index(stmtHandle, ":prm_ä");
 	prmIndex1_1 = prmIndex1_1;
 	TInt prmIndex2_1 = sqlite3_bind_parameter_index(stmtHandle, ":prm_A");
 	prmIndex2_1 = prmIndex2_1;
 	TInt prmIndex3_1 = sqlite3_bind_parameter_index(stmtHandle, ":prm_a");
 	prmIndex3_1 = prmIndex3_1;
	
	TBuf8<16> name8;
	
	HBufC8* name = CnvUtfConverter::ConvertFromUnicodeToUtf8L(prmNames[1]);
	name8 = *name;
	delete name;
 	TInt prmIndex0_2 = sqlite3_bind_parameter_index(stmtHandle, (const char*)name8.PtrZ());
 	prmIndex0_2 = prmIndex0_2;
 	
	name = CnvUtfConverter::ConvertFromUnicodeToUtf8L(prmNames[2]);
	name8 = *name;
	delete name;
 	TInt prmIndex1_2 = sqlite3_bind_parameter_index(stmtHandle, (const char*)name8.PtrZ());
 	prmIndex1_2 = prmIndex1_2;
 	
	name = CnvUtfConverter::ConvertFromUnicodeToUtf8L(prmNames[3]);
	name8 = *name;
	delete name;
 	TInt prmIndex2_2 = sqlite3_bind_parameter_index(stmtHandle, (const char*)name8.PtrZ());
 	prmIndex2_2 = prmIndex2_2;
 	
	name = CnvUtfConverter::ConvertFromUnicodeToUtf8L(prmNames[4]);
	name8 = *name;
	delete name;
 	TInt prmIndex3_2 = sqlite3_bind_parameter_index(stmtHandle, (const char*)name8.PtrZ());
 	prmIndex3_2 = prmIndex3_2;
	
	//Column checks
	TPtrC colName1(_L("col_\u00C4"));
	TPtrC colName2(_L("col_A"));
	
	const void* p = sqlite3_column_name16(stmtHandle, 0);
	TPtrC pp1((const TUint16*)p, wcslen(reinterpret_cast <const wchar_t*> (p)));
	TEST(colName1 == pp1);
	
	p = sqlite3_column_name16(stmtHandle, 1);
	TPtrC pp2((const TUint16*)p, wcslen(reinterpret_cast <const wchar_t*> (p)));
	TEST(colName2 == pp2);
		
	sqlite3_finalize(stmtHandle);
	
	sqlite3_close(TheDb1);
	TheFs.Delete(ConvertToUtf16(KDbName1));
	}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////              "Add Contacts" test case                  ////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//Read SQL file - 8-bit, zero-terminated string
static char* ReadSQL2(const TDesC& aSqlFileName)
	{
	RFile file;
	TEST2(file.Open(TheFs, aSqlFileName, EFileRead), KErrNone);
	
	TInt size = 0;
	TEST2(file.Size(size), KErrNone);
	
	char* sql = new char [size + 1];
	TEST(sql != NULL);
	
	TPtr8 ptr((TUint8*)sql, size + 1);
	TEST2(file.Read(ptr, size), KErrNone);
	*(sql + size) = 0;

	file.Close();
	return sql;
	}

//Read SQL file - 16-bit, zero-terminated string
static HBufC* ReadSQL16(const TDesC& aSqlFileName)
	{
	RFile file;
	TEST2(file.Open(TheFs, aSqlFileName, EFileRead), KErrNone);
	
	TInt size = 0;
	TEST2(file.Size(size), KErrNone);

	char* sql = new char [size];
	TEST(sql != NULL);
	TPtr8 ptr((TUint8*)sql, size);
	TEST2(file.Read(ptr, size), KErrNone);
	
	HBufC* sql16 = HBufC::New(size + 1);
	TEST(sql16 != NULL);
	TPtr16 ptr16 = sql16->Des();
	ptr16.Copy(ptr);
	ptr16.Append(TChar(0));

	delete [] sql;
	file.Close();
	return sql16;
	}

//Prints file size in bytes
static void PrintFileSize(const TDesC& aPath)
	{
	RFile file;
	TEST2(file.Open(TheFs, aPath, EFileRead), KErrNone);
	TInt size = 0;
	TEST2(file.Size(size), KErrNone);
	file.Close();
	RDebug::Print(_L("File \"%S\", size: %d\r\n"), &aPath, size);
	}

//Executes 8-bit SQL statement	
static void ExecSql(sqlite3* aDbHandle, const char* aSqlStmt, const TDesC& aMsg)
	{
	TEST(aDbHandle != NULL);
	TEST(aSqlStmt != NULL);
	
	char* errMsg = NULL;
	TTime t1;
	t1.UniversalTime();
	TInt rc = sqlite3_exec(aDbHandle, aSqlStmt, callback, 0, &errMsg);
	if(rc != SQLITE_OK)
		{
		TPtrC8 ptr8((const TUint8*)errMsg, strlen(errMsg));
		TheBuf1.Copy(ptr8);
		RDebug::Print(_L("'sqlite3_exec()' failed, err %d, error msg: \"%S\"\t\n"), rc, &TheBuf1);
		TEST(0);
		}
	TTime t2;
	t2.UniversalTime();
	TTimeIntervalMicroSeconds diffTime = t2.MicroSecondsFrom(t1); 
	diffTime = diffTime.Int64() / 1000;
	RDebug::Print(_L("%S, time: %d ms\r\n"), &aMsg, (TInt)diffTime.Int64());
	}

//This function searches aString argument for ';' occurences.
//Every time when it finds a ';' character, the function places a 0 right after the ';' and
//tests the just created, zero-terminated substring if it is a comlpete SQL statement.
//If it is a SQL statement, the function returns it and modifies aString argument to point right after the found
//SQL string. If it is not SQL statement, the function will continue the searching.
//If there is no ';' inside aString argument, the function returns the same string as a return result and
//modifies aString argument - sets it to TPtr(NULL, 0, 0).
//
//The function expects aString argument to be zero-terminated.
static TPtrC GetFirstSqlStmt(TPtr& aString)
	{
	const TChar KDelimitier(';');
	TPtr str(const_cast <TUint16*> (aString.Ptr()), aString.Length(), aString.Length());
	TInt afterDelimitierPos = 0;
	TInt pos;
	while((pos = str.Locate(KDelimitier) + 1) > 0 && pos < str.Length())
		{
		//There is a possibility that the string which terminates with the found ';' is SQL statement.
		//Zero terminate the string placing a zero right after ';' character and test it using sqlite3_complete16()
		//call. If it is not SQL string, restore the original character and continue searching.
		afterDelimitierPos += pos;
		TChar ch = aString[afterDelimitierPos];
		aString[afterDelimitierPos] = 0;
		TInt res = sqlite3_complete16(aString.Ptr());
		aString[afterDelimitierPos] = ch;
		if(res)
			{
			str.Set(const_cast <TUint16*> (aString.Ptr()), afterDelimitierPos, afterDelimitierPos);	
			//Replace the found ';' character with 0.
			str[afterDelimitierPos - 1] = 0;
			aString.Set(const_cast <TUint16*> (aString.Ptr()) + afterDelimitierPos, aString.Length() - afterDelimitierPos, aString.Length() - afterDelimitierPos);
			return str;
			}
		str.Set(const_cast <TUint16*> (str.Ptr()) + pos, str.Length() - pos, str.Length() - pos);	
		}
	//aString argument does not contain valid SQL statement or there is no ';' character inside aString.
	//Set aString to TPtr(NULL, 0, 0) and return the original string.
	aString.Set(NULL, 0, 0);
	return str;
	}
	
//Executes 16-bit SQL statement	
static void ExecSql16(sqlite3* aDbHandle, TDes& aSqlStmtZ, const TDesC& aMsg)
	{
	TEST(aDbHandle != NULL);
		
	TTime t1;
	t1.UniversalTime();
	
	TPtr sqlLeftZ(const_cast <TUint16*> (aSqlStmtZ.Ptr()), aSqlStmtZ.Length(), aSqlStmtZ.Length());
	while(sqlLeftZ.Length() > 0)
		{
		TPtrC sql = GetFirstSqlStmt(sqlLeftZ);
		sqlite3_stmt* stmtHandle = NULL;
		const void* stmtTailZ = NULL;
		TInt err = sqlite3_prepare16_v2(aDbHandle, sql.Ptr(), -1, &stmtHandle, &stmtTailZ);
		__ASSERT_ALWAYS(!stmtTailZ || User::StringLength((const TUint16*)stmtTailZ) == 0, User::Invariant());
		if(stmtHandle)
			{
			if(err == SQLITE_OK)
				{
				while((err = sqlite3_step(stmtHandle)) == SQLITE_ROW)
					{
					}
				}
			sqlite3_finalize(stmtHandle);
			}
		if(err != SQLITE_DONE && err != SQLITE_OK)
			{
			const void* errMsgZ = sqlite3_errmsg16(aDbHandle);
			TPtrC msg(reinterpret_cast <const TText16*> (errMsgZ), wcslen(reinterpret_cast <const wchar_t*> (errMsgZ)));
			RDebug::Print(_L("'sqlite3_exec16()' failed, err %d, error msg: \"%S\"\t\n"), err, &msg);
			TEST(0);
			}
		}
	TTime t2;
	t2.UniversalTime();
	TTimeIntervalMicroSeconds diffTime = t2.MicroSecondsFrom(t1); 
	diffTime = diffTime.Int64() / 1000;
	RDebug::Print(_L("%S, time: %d ms\r\n"), &aMsg, (TInt)diffTime.Int64());
	}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////              "Search" test case                        ////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

struct TPnName
	{
	TPnName(const TDesC8& aFirstName, const TDesC8& aSurname) :
		iFirstName(aFirstName),
		iSurname(aSurname)
		{
		}
	const TPtrC8 iFirstName;
	const TPtrC8 iSurname;
	};

const TInt KNamesCnt = 100;
	
const TPnName KNames[KNamesCnt] = 
	{
		TPnName(_L8("Kauh"), _L8("Mollel")),
		TPnName(_L8("Be"), _L8("Balcalertthawnd")),
		TPnName(_L8("Joba"), _L8("Hah")),
		TPnName(_L8("Mal"), _L8("Sinianna")),
		TPnName(_L8("Alip"), _L8("Hanittrinke")),
		TPnName(_L8("Ris"), _L8("Aba")),
		TPnName(_L8("Nirindrilo"), _L8("Oangah")),
		TPnName(_L8("An"), _L8("Mck")),
		TPnName(_L8("Ris"), _L8("Jat")),
		TPnName(_L8("Ja"), _L8("R")),
		TPnName(_L8("Pary"), _L8("Sawngethwnes")),
		TPnName(_L8("Main"), _L8("Stonstc")),
		TPnName(_L8("Joldan"), _L8("Misonialonss")),
		TPnName(_L8("Ja"), _L8("Beetth")),
		TPnName(_L8("An"), _L8("Magill")),
		TPnName(_L8("Ste"), _L8("Hakegstolbebilance")),
		TPnName(_L8("Laelefattal"), _L8("Bume")),
		TPnName(_L8("Anortoausl"), _L8("Kenoonssssoninals")),
		TPnName(_L8("Sthnill"), _L8("Huere")),
		TPnName(_L8("Elatandy"), _L8("Miadhelbi")),
		TPnName(_L8("Nevieohageridik"), _L8("Baronirgeriallyemo")),
		TPnName(_L8("Dertrry"), _L8("Miches")),
		TPnName(_L8("Tan-"), _L8("Sonagutlly")),
		TPnName(_L8("Mazianer"), _L8("Wi")),
		TPnName(_L8("Kesadrin"), _L8("Swixohar")),
		TPnName(_L8("Juhnn"), _L8("Vezuins")),
		TPnName(_L8("Geri"), _L8("Okun-Mamar")),
		TPnName(_L8("Jol"), _L8("Hadir")),
		TPnName(_L8("Lon"), _L8("Fonernginire")),
		TPnName(_L8("Brrk"), _L8("El")),
		TPnName(_L8("So"), _L8("Thanas")),
		TPnName(_L8("Timon"), _L8("Matarol")),
		TPnName(_L8("Clicartif"), _L8("Sandhinth")),
		TPnName(_L8("Dan"), _L8("Brl")),
		TPnName(_L8("An"), _L8("Danss")),
		TPnName(_L8("Y"), _L8("Gianstes")),
		TPnName(_L8("Gralilas"), _L8("Beny")),
		TPnName(_L8("Vamean"), _L8("Matesstel")),
		TPnName(_L8("Ch"), _L8("Inrinez")),
		TPnName(_L8("Ra"), _L8("Lusieing")),
		TPnName(_L8("Gerik"), _L8("Mawoshar")),
		TPnName(_L8("Nobrd"), _L8("Kerokilirtsoug")),
		TPnName(_L8("Norichnik"), _L8("Balmo")),
		TPnName(_L8("Anddra"), _L8("Fit")),
		TPnName(_L8("Maily"), _L8("Tanyerohetsphinbr")),
		TPnName(_L8("Frsa"), _L8("Huntorrenerkh")),
		TPnName(_L8("Gi"), _L8("Spandaveees")),
		TPnName(_L8("Jollminenipaninderal"), _L8("Vartzury")),
		TPnName(_L8("Ankshr"), _L8("Terawloleral")),
		TPnName(_L8("An"), _L8("La")),
		TPnName(_L8("Ma"), _L8("Brnd")),
		TPnName(_L8("Sonerdalmon"), _L8("Bo")),
		TPnName(_L8("Nis"), _L8("Tapeworrt")),
		TPnName(_L8("Shand"), _L8("Hacllik")),
		TPnName(_L8("San"), _L8("Sh")),
		TPnName(_L8("Mico"), _L8("Javiaros")),
		TPnName(_L8("Hub"), _L8("Warey")),
		TPnName(_L8("Mambew"), _L8("Maw")),
		TPnName(_L8("Honik"), _L8("Fantscerstetoringu")),
		TPnName(_L8("Da"), _L8("Saneelur")),
		TPnName(_L8("Aberecalahayondorttelin"), _L8("Futtesesoxok")),
		TPnName(_L8("Dor"), _L8("Lelek")),
		TPnName(_L8("Matin"), _L8("Fure")),
		TPnName(_L8("Niasietolf"), _L8("Jonones")),
		TPnName(_L8("Das"), _L8("Hoeonds")),
		TPnName(_L8("Anchn"), _L8("Svss")),
		TPnName(_L8("Dor"), _L8("Bolunatrk")),
		TPnName(_L8("Casah"), _L8("Brilllundonsssoug")),
		TPnName(_L8("Iapew"), _L8("Bagukak")),
		TPnName(_L8("Lieni"), _L8("MoncNicel")),
		TPnName(_L8("Adewalyary"), _L8("Buradesorobbrerans")),
		TPnName(_L8("Tos"), _L8("Gis")),
		TPnName(_L8("Vi"), _L8("Berk")),
		TPnName(_L8("Jorya"), _L8("upmarone")),
		TPnName(_L8("Iatew"), _L8("Hend")),
		TPnName(_L8("Liag"), _L8("Brsmall")),
		TPnName(_L8("Al"), _L8("Spahay")),
		TPnName(_L8("El"), _L8("Sy")),
		TPnName(_L8("Pary"), _L8("Trl")),
		TPnName(_L8("Br"), _L8("Usouroneis")),
		TPnName(_L8("Sirnilly"), _L8("Olay")),
		TPnName(_L8("Fell"), _L8("Bouphies")),
		TPnName(_L8("Man"), _L8("Haz")),
		TPnName(_L8("Dare"), _L8("Was")),
		TPnName(_L8("Fahnahopephrtex"), _L8("Gat")),
		TPnName(_L8("Har"), _L8("Handfffebinneickiasse")),
		TPnName(_L8("Gerlai"), _L8("Boravirg")),
		TPnName(_L8("Miss"), _L8("Us")),
		TPnName(_L8("Caushatattoatot"), _L8("Wes")),
		TPnName(_L8("Eizicay"), _L8("Gunbss")),
		TPnName(_L8("Pan"), _L8("Hilesertatickesobss-")),
		TPnName(_L8("Anaw"), _L8("Mangar")),
		TPnName(_L8("Korba"), _L8("Siansolan")),
		TPnName(_L8("Darl"), _L8("Haginijelso")),
		TPnName(_L8("Ral"), _L8("Veddddkisocackeluisowowone")),
		TPnName(_L8("La"), _L8("Wawethl")),
		TPnName(_L8("Y"), _L8("Wisonkend")),
		TPnName(_L8("Evimiat"), _L8("JondepssooncClille")),
		TPnName(_L8("Rin"), _L8("DulatoliacKark")),
		TPnName(_L8("Shegeiew"), _L8("Ass"))
	};
	
/////////////////////////////////////////////////////////////
//  16-bit strings
/////////////////////////////////////////////////////////////

static TInt CmpF16(void*, TInt size1, const void* p1, TInt size2, const void* p2)
	{
	++TheCmpFCallCnt;
	
	TPtrC16 ptr1((TUint16*)p1, size1/2);
	TPtrC16 ptr2((TUint16*)p2, size2/2);
	
	TInt res = ptr1.CompareF(ptr2);
	return res;		
	}

static TInt CmpC16(void*, TInt size1, const void* p1, TInt size2, const void* p2)
	{
	++TheCmpCCallCnt;
	
	TPtrC16 ptr1((TUint16*)p1, size1/2);
	TPtrC16 ptr2((TUint16*)p2, size2/2);
	TInt res = ptr1.CompareC(ptr2);
	return res;		
	}

static void SearchDbTest16(const TDesC& aDbFilePath)
	{
	TheFs.Delete(aDbFilePath);
	
	TBuf<KMaxFileName + 1> fname;
	fname.Copy(aDbFilePath);
	//Open database
	RDebug::Print(_L("Open database\r\n"));
	sqlite3 *dbHandle = NULL;
	TInt rc = sqlite3_open16(fname.PtrZ(), &dbHandle);//!!!!16-bit encoding!!!!!
	if(rc != SQLITE_OK)
		{
		const void* errMsgZ = sqlite3_errmsg16(dbHandle);
		TPtrC msg(reinterpret_cast <const TText16*> (errMsgZ), wcslen(reinterpret_cast <const wchar_t*> (errMsgZ)));
		RDebug::Print(_L("'sqlite3_open()' failed, file %S, err %d, error msg: \"%S\"\t\n"), &aDbFilePath, rc, &msg);
		TEST(0);
		}
	TEST(dbHandle != NULL);
	//Create "CompareF" collation	
	RDebug::Print(_L("Create \"CompareF\" collation\r\n"));
	_LIT(KCmpF, "CmpF16\x0");
	rc = sqlite3_create_collation16(dbHandle, (const char*)(KCmpF().Ptr()), SQLITE_UTF16 | SQLITE_UTF16_ALIGNED, NULL, &CmpF16);
  	if(rc)
  		{
		const void* errMsgZ = sqlite3_errmsg16(dbHandle);
		TPtrC msg(reinterpret_cast <const TText16*> (errMsgZ), wcslen(reinterpret_cast <const wchar_t*> (errMsgZ)));
		RDebug::Print(_L("Err=%S\r\n"), &msg);
		TEST(0);
  		}
	//Create "CompareC" collation	
	RDebug::Print(_L("Create \"CompareC\" collation\r\n"));
	_LIT(KCmpC, "CmpC16\x0");
	rc = sqlite3_create_collation16(dbHandle, (const char*)(KCmpC().Ptr()), SQLITE_UTF16 | SQLITE_UTF16_ALIGNED, NULL, &CmpC16);
  	if(rc)
  		{
		const void* errMsgZ = sqlite3_errmsg16(dbHandle);
		TPtrC msg(reinterpret_cast <const TText16*> (errMsgZ), wcslen(reinterpret_cast <const wchar_t*> (errMsgZ)));
		RDebug::Print(_L("Err=%S\r\n"), &msg);
		TEST(0);
  		}
	//Create database schema
	TheCmpFCallCnt = TheCmpCCallCnt = 0;
	RDebug::Print(_L("Create database schema\r\n"));
	HBufC16* createSqlZ = ReadSQL16(_L("z:\\test\\contacts_schema_to_vendors.sql"));
	TPtr sql = createSqlZ->Des();
	ExecSql16(dbHandle, sql, _L("Create schema"));
	delete createSqlZ;
	RDebug::Print(_L("CmpF() call cnt %d, CmpC() call cnt %d\r\n"), TheCmpFCallCnt, TheCmpCCallCnt);
	//Add 1001 "simple" contacts
	TheCmpFCallCnt = TheCmpCCallCnt = 0;
	RDebug::Print(_L("Add 1001 \"simple\" contacts\r\n"));
	HBufC16* addSqlZ = ReadSQL16(KSimpleContactsSqlFile);
	RDebug::Print(_L("--\r\n"));
	sql.Set(addSqlZ->Des());
	ExecSql16(dbHandle, sql, _L("Add simple contacts"));
	delete addSqlZ;
	RDebug::Print(_L("CmpF() call cnt %d, CmpC() call cnt %d\r\n"), TheCmpFCallCnt, TheCmpCCallCnt);
	//Print the number of records
	RDebug::Print(_L("Print the number of records\r\n"));
	TBuf<40> testSql(_L("SELECT COUNT(*) FROM CONTACTS"));
	testSql.Append(TChar(0));
	ExecSql16(dbHandle, testSql, _L("--"));
	
	//Create index: "First name, Last name"
	TheCmpFCallCnt = TheCmpCCallCnt = 0;
	RDebug::Print(_L("Create index: \"First name, Last name\"\r\n"));
	TBuf<100> createIndexStmt(_L("CREATE INDEX Idx1 ON identitytable(cm_firstname COLLATE CmpC16, cm_lastname COLLATE CmpC16)"));
	createIndexStmt.Append(TChar(0));
	ExecSql16(dbHandle, createIndexStmt, _L("Create index"));
	RDebug::Print(_L("CmpF() call cnt %d, CmpC() call cnt %d\r\n"), TheCmpFCallCnt, TheCmpCCallCnt);

	RDebug::Print(_L("Close database\r\n"));
   	sqlite3_close(dbHandle);

	PrintFileSize(aDbFilePath);
	}

/////////////////////////////////////////////////////////////
//  8-bit strings
/////////////////////////////////////////////////////////////

static TInt CmpF8(void*, TInt size1, const void* p1, TInt size2, const void* p2)
	{
	++TheCmpFCallCnt;
	TPtrC8 ptr1((TUint8*)p1, size1);
	TPtrC8 ptr2((TUint8*)p2, size2);
	
	TInt res = ptr1.CompareF(ptr2);
	return res;		
	}

static TInt CmpC8(void*, TInt size1, const void* p1, TInt size2, const void* p2)
	{
	TPtrC8 ptr1((TUint8*)p1, size1);
	TPtrC8 ptr2((TUint8*)p2, size2);
	TInt res = ptr1.CompareC(ptr2);
	return res;		
	}

static void SearchDbTest8(const TDesC& aDbFilePath, const TDesC& aAddContactsFile, const TDesC& aMsg)
	{
	TheFs.Delete(aDbFilePath);
	
	TBuf8<KMaxFileName + 1> fname;
	fname.Copy(aDbFilePath);
	
	RDebug::Print(_L("%S\r\n"), &aMsg);
	RDebug::Print(_L("Open database\r\n"));
	sqlite3 *dbHandle = NULL;
	TInt rc = sqlite3_open((const char*)fname.PtrZ(), &dbHandle);
	if(rc != SQLITE_OK)
		{
		const void* errMsgZ = sqlite3_errmsg16(dbHandle);
		TPtrC msg(reinterpret_cast <const TText16*> (errMsgZ), wcslen(reinterpret_cast <const wchar_t*> (errMsgZ)));
		RDebug::Print(_L("'sqlite3_open()' failed, file %S, err %d, error msg: \"%S\"\r\n"), &aDbFilePath, rc, &msg);
		TEST(0);
		}
	TEST(dbHandle != NULL);
	
	RDebug::Print(_L("Create 'CompareF' collation\r\n"));
	_LIT8(KCmpF, "CmpF8\x0");
	rc = sqlite3_create_collation(dbHandle, (const char*)(KCmpF().Ptr()), SQLITE_UTF8, NULL, &CmpF8);
  	if(rc)
  		{
		const void* errMsgZ = sqlite3_errmsg16(dbHandle);
		TPtrC msg(reinterpret_cast <const TText16*> (errMsgZ), wcslen(reinterpret_cast <const wchar_t*> (errMsgZ)));
		RDebug::Print(_L("'sqlite3_create_collation()' failed, file %S, err %d, error msg: \"%S\"\r\n"), &aDbFilePath, rc, &msg);
		TEST(0);
  		}

	RDebug::Print(_L("Create 'CompareC' collation\r\n"));
	_LIT8(KCmpC, "CmpC8\x0");
	rc = sqlite3_create_collation(dbHandle, (const char*)(KCmpC().Ptr()), SQLITE_UTF8, NULL, &CmpC8);
  	if(rc)
  		{
		const void* errMsgZ = sqlite3_errmsg16(dbHandle);
		TPtrC msg(reinterpret_cast <const TText16*> (errMsgZ), wcslen(reinterpret_cast <const wchar_t*> (errMsgZ)));
		RDebug::Print(_L("'sqlite3_create_collation()' failed, file %S, err %d, error msg: \"%S\"\r\n"), &aDbFilePath, rc, &msg);
		TEST(0);
  		}

	RDebug::Print(_L("Create database schema\r\n"));
	char* createSqlZ = ReadSQL2(_L("z:\\test\\contacts_schema_to_vendors.sql"));
	ExecSql(dbHandle, createSqlZ, _L("Create schema"));
	delete [] createSqlZ;

	RDebug::Print(_L("Add 1001 contacts\r\n"));
	char* addSqlZ = ReadSQL2(aAddContactsFile);
	ExecSql(dbHandle, addSqlZ, _L("Add contacts"));
	delete [] addSqlZ;

	RDebug::Print(_L("Print the number of records\r\n"));
	const char testSql[] = {"SELECT COUNT(*) FROM CONTACTS"};
	ExecSql(dbHandle, testSql, _L("SELECT COUNT(*)"));
	
	RDebug::Print(_L("Create index (using 'CompareF' collation): 'FirstName, Surname'\r\n"));
	_LIT8(KCreateIndexStmt, "CREATE INDEX Idx1 ON identitytable(cm_firstname COLLATE CmpF8, cm_lastname COLLATE CmpF8)\x0");
	ExecSql(dbHandle, (const char*)(KCreateIndexStmt().Ptr()), _L("Create index"));
	RDebug::Print(_L("CompareF() called %d times\r\n"), TheCmpFCallCnt);

/* BEGIN OF - TEST CASE 1 "Select all contacts which first name begins with 'a' " */	

	RDebug::Print(_L("Prepare 'Select all contacts where the first name begins with 'A'' SQL string\r\n"));
	_LIT8(KSearchStmt, "SELECT cm_firstname, cm_lastname FROM identitytable WHERE cm_firstname LIKE 'A%'\x0");
	sqlite3_stmt* stmtHandle = NULL;
	const char* stmtTailZ = NULL;
	TTime t1;
	t1.UniversalTime();
	rc = sqlite3_prepare_v2(dbHandle, (const char*)(KSearchStmt().Ptr()), -1, &stmtHandle, &stmtTailZ);
	TEST2(rc, SQLITE_OK);
	TEST(stmtHandle != NULL);
	TEST(!stmtTailZ || strlen(stmtTailZ) == 0);
	TTime t2;
	t2.UniversalTime();
	TTimeIntervalMicroSeconds diffTime = t2.MicroSecondsFrom(t1); 
	diffTime = diffTime.Int64() / 1000;
	TInt t = (TInt)diffTime.Int64();
	RDebug::Print(_L("'Prepare SQL statement' time: %d ms\r\n"), t);

	RDebug::Print(_L("Step the prepared SQL statement\r\n"));
	TInt totalCmpFCnt = 0;
	t1.UniversalTime();
	TheCmpFCallCnt = 0;
	TInt recordCnt = 0;
	while((rc = sqlite3_step(stmtHandle)) == SQLITE_ROW)
		{
		//const TUint8* firstName = sqlite3_column_text(stmtHandle, 0);
		//TPtrC8 p(firstName, strlen((const char*)firstName));
		//TBuf<100> p1; p1.Copy(p);
		//const TUint8* surname = sqlite3_column_text(stmtHandle, 1);
		//p.Set(surname, strlen((const char*)surname));
		//TBuf<100> p2; p2.Copy(p);
		//RDebug::Print(_L("Found rec: %S, %S\r\n"), &p1, &p2);
		++recordCnt;
		}
	totalCmpFCnt += TheCmpFCallCnt;
	TEST(rc == SQLITE_OK || rc == SQLITE_DONE);
	t2.UniversalTime();
	diffTime = t2.MicroSecondsFrom(t1); 
	diffTime = diffTime.Int64() / 1000;
	t = (TInt)diffTime.Int64();
	RDebug::Print(_L("'Stepping' time: %d ms, found records: %d\r\n"), t, recordCnt);
	RDebug::Print(_L("Total 'search' ('CompareF' used) operations=%d\r\n"), totalCmpFCnt);

	sqlite3_finalize(stmtHandle);
	stmtHandle = NULL;

/* END OF - TEST CASE 1 "Select all contacts which first name begins with 'a' " */	

/*	BEGIN OF - TEST CASE 2 "Do 100 searches in 1001 contacts" */

	RDebug::Print(_L("Prepare 'SELECT FirstName, Surname...' SQL string\r\n"));
	_LIT8(KSearchStmt2, "SELECT cm_firstname, cm_lastname FROM identitytable WHERE cm_firstname = :Prm1 AND cm_lastname = :Prm2\x0");
	stmtHandle = NULL;
	stmtTailZ = NULL;
	t1.UniversalTime();
	rc = sqlite3_prepare_v2(dbHandle, (const char*)(KSearchStmt2().Ptr()), -1, &stmtHandle, &stmtTailZ);
	TEST2(rc, SQLITE_OK);
	TEST(stmtHandle != NULL);
	TEST(!stmtTailZ || strlen(stmtTailZ) == 0);
	t2.UniversalTime();
	diffTime = t2.MicroSecondsFrom(t1); 
	diffTime = diffTime.Int64() / 1000;
	t = (TInt)diffTime.Int64();
	RDebug::Print(_L("'Prepare SQL statement' time: %d ms\r\n"), t);
	
	TInt idxPrm1 = sqlite3_bind_parameter_index(stmtHandle, ":Prm1");
	TEST(idxPrm1 > 0);
	TInt idxPrm2 = sqlite3_bind_parameter_index(stmtHandle, ":Prm2");
	TEST(idxPrm2 > 0);
	
	RDebug::Print(_L("Do %d searches using the prepared SQL statement\r\n"), KNamesCnt);
	totalCmpFCnt = 0;
	t1.UniversalTime();
	for(TInt i=0;i<KNamesCnt;++i)
		{
		const TDesC8& firstName = KNames[i].iFirstName;
		rc = sqlite3_bind_text(stmtHandle, idxPrm1, (const char*)firstName.Ptr(), firstName.Length(), SQLITE_STATIC);
		TEST2(rc, SQLITE_OK);
		
		const TDesC8& surname = KNames[i].iSurname;
		rc = sqlite3_bind_text(stmtHandle, idxPrm2, (const char*)surname.Ptr(), surname.Length(), SQLITE_STATIC);
		TEST2(rc, SQLITE_OK);
		
		TheCmpFCallCnt = 0;
		TInt recordCnt = 0;
		while((rc = sqlite3_step(stmtHandle)) == SQLITE_ROW)
			{
			++recordCnt;
			}
		totalCmpFCnt += TheCmpFCallCnt;
		TEST(recordCnt == 1);
		rc = sqlite3_reset(stmtHandle);
		TEST2(rc, SQLITE_OK);
		}
	t2.UniversalTime();
	diffTime = t2.MicroSecondsFrom(t1); 
	diffTime = diffTime.Int64() / 1000;
	t = (TInt)diffTime.Int64();
	RDebug::Print(_L("'Search' time: %d ms\r\n"), t);
	RDebug::Print(_L("Total 'search' ('CompareF' used) operations=%d, average_per_iter=%d\r\n"), totalCmpFCnt, totalCmpFCnt/KNamesCnt);

	sqlite3_finalize(stmtHandle);
	stmtHandle = NULL;
	
/* END OF - TEST CASE 2 "Do 100 searches in 1001 contacts" */	

	t1.UniversalTime();
	sqlite3_finalize(stmtHandle);
	t2.UniversalTime();
	diffTime = t2.MicroSecondsFrom(t1); 
	diffTime = diffTime.Int64() / 1000;
	t = (TInt)diffTime.Int64();
	RDebug::Print(_L("'Finalize SQL statement' time: %d ms\r\n"), t);

   	sqlite3_close(dbHandle);
	PrintFileSize(aDbFilePath);
	}

static void TwoDatabasesTest()
	{
	(void)sqlite3SymbianLastOsError();
	sqlite3 *dbHandle1 = NULL;
	TInt rc1 = sqlite3_open(KDbName1, &dbHandle1);
	if(rc1 != SQLITE_OK)
		{
		rc1 = sqlite3SymbianLastOsError();
		}

	(void)sqlite3SymbianLastOsError();
	sqlite3 *dbHandle2 = NULL;
	TInt rc2 = sqlite3_open(KDbName1, &dbHandle2);
	if(rc2 != SQLITE_OK)
		{
		rc2 = sqlite3SymbianLastOsError();
		}
	
   	sqlite3_close(dbHandle2);

   	sqlite3_close(dbHandle1);
	
	(void)TheFs.Delete(ConvertToUtf16(KDbName1));
	
	TEST2(rc1, KErrNone);
	TEST2(rc2, KErrInUse);//EFileRead | EFileWrite file open mode!
	}

static void QuickTest()
	{
	(void)TheFs.Delete(ConvertToUtf16(KDbName1));

	sqlite3 *dbHandle1 = NULL;
	TInt err = sqlite3_open(KDbName1, &dbHandle1);
	TEST2(err, SQLITE_OK);

	err = sqlite3_exec(dbHandle1, "CREATE TABLE A(Fld1 INTEGER, Fld2 TEXT);", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	sqlite3_stmt* stmtHandle = NULL;
	_LIT(KSql1, "INSERT INTO A(Fld1, Fld2) VALUES(1, :Val)\x0");
	err = sqlite3_prepare16_v2(dbHandle1, KSql1().Ptr(), -1, &stmtHandle, NULL);
	TEST2(err, SQLITE_OK);

	_LIT(KPrmData, "U012");
	err = sqlite3_bind_text16(stmtHandle, 1, (const void*)KPrmData().Ptr(), KPrmData().Length() * sizeof(TText), SQLITE_STATIC);
	TEST2(err, SQLITE_OK);

	while((err = sqlite3_step(stmtHandle)) == SQLITE_ROW)
		{
		}
	if(err == SQLITE_ERROR)	//It may be "out of memory" problem
		{
		err = sqlite3_reset(stmtHandle);
		__ASSERT_ALWAYS(err != SQLITE_OK, User::Invariant());
		}

   	sqlite3_finalize(stmtHandle);
   	
	_LIT(KSql2, "SELECT * FROM A WHERE Fld1 = 1\x0");
	err = sqlite3_prepare16_v2(dbHandle1, KSql2().Ptr(), -1, &stmtHandle, NULL);
	TEST2(err, SQLITE_OK);

	err = sqlite3_step(stmtHandle);
	TEST2(err, SQLITE_ROW);

	const void* data = sqlite3_column_text16(stmtHandle, 1);
	UNUSED_VAR(data);
	TInt charLength  = sqlite3_column_bytes16(stmtHandle, 1) / sizeof(TUint16);
	UNUSED_VAR(charLength);
   	
   	sqlite3_finalize(stmtHandle);
   	
   	sqlite3_close(dbHandle1);

	(void)TheFs.Delete(ConvertToUtf16(KDbName1));
	}

//////////////////////////////////////////////////////////////////////////////////////////////////

static void TemdDbTest()
	{
	sqlite3 *dbHandle = NULL;
	TInt rc = sqlite3_open(KDbName1, &dbHandle);
	if(rc != SQLITE_OK)
		{
		const void* errMsgZ = sqlite3_errmsg16(dbHandle);
		TPtrC msg(reinterpret_cast <const TText16*> (errMsgZ), wcslen(reinterpret_cast <const wchar_t*> (errMsgZ)));
		RDebug::Print(_L("'sqlite3_open()' failed, err %d, error msg: \"%S\"\r\n"), rc, &msg);
		TEST(0);
		}
	TEST(dbHandle != NULL);

	char *zErrMsg = 0;

	_LIT8(KSql1, "CREATE TEMP TABLE A(F1 INTEGER)\x0");
	rc = sqlite3_exec(dbHandle, reinterpret_cast <const char*> (KSql1().Ptr()), callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
    	TEST(0);
		}

	_LIT8(KSql2, "INSERT INTO A(F1) VALUES(2)\x0");
	rc = sqlite3_exec(dbHandle, reinterpret_cast <const char*> (KSql2().Ptr()), callback, 0, &zErrMsg);
	if(rc != SQLITE_OK)
		{
  		TPtrC p = ConvertToUtf16(zErrMsg);
  		RDebug::Print(_L("SQL error %d, msg: %S\n"), rc, &p);
    	sqlite3_free(zErrMsg);
    	TEST(0);
		}

   	sqlite3_close(dbHandle);
	(void)TheFs.Delete(ConvertToUtf16(KDbName1));
	}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

static void CreateTestDir()
    {
    RFs fs;
	TInt err = fs.Connect();
	TEST2(err, KErrNone);

	err = fs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	
	fs.Close();
	}

static void DoTestsL()
	{
	TheTest.Next(_L("Attempt to open twice the same database file in the same thread"));
	TwoDatabasesTest();

	TheTest.Next(_L("TEMP database test"));
	TemdDbTest();

	TheTest.Next(_L("Quick test"));
	QuickTest();

	TheTest.Next(_L("SQLite tests 1"));
	DoTests1();
	
	TheTest.Next(_L("SQLite tests 2"));
	DoTests2();

	TheTest.Next(_L("Accented column names"));
	AccentedColumnNamesTestL();
	
	TFileName fname;
	User::CommandLine(fname);
	TParse parse;
	parse.Set(fname, &KContactsFile, 0);
	const TDesC& dbFilePath = parse.FullName();
	
	PrintConfig(dbFilePath);

	TheTest.Next(_L("String searching tests. 16-bit strings. Simple contacts"));
	SearchDbTest16(dbFilePath);

	TheTest.Next(_L("String searching tests. 8-bit strings. Simple contacts"));
	SearchDbTest8(dbFilePath, KSimpleContactsSqlFile, _L("Search simple contacts"));
	}

TInt E32Main()
	{
	TheTest.Title();
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-LEGACY-T_SQLITE-0001 SQLite tests "));
	
	TheTrapCleanup = CTrapCleanup::New ();
	__ASSERT_ALWAYS(TheTrapCleanup != NULL, User::Invariant());

	__UHEAP_MARK;

	CreateTestDir();
	
	TheTest(TheFs.Connect() == KErrNone);
	DeleteTestFiles();

	CreatePrivateDirs();

	//Init sqlite library
	sqlite3SymbianLibInit();

	TRAPD(err, DoTestsL());
	
	sqlite3SymbianLibFinalize();

	CloseSTDLIB();
	
	DeleteTestFiles();
	
	TEST2(err, KErrNone);
	
	TheFs.Close();

	__UHEAP_MARKEND;

	delete TheTrapCleanup;


	TheTest.End();
	TheTest.Close();


	return KErrNone;
	}
