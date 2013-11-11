// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "SqlSrvConfig.h"

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_sqlconfig test"));
_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDbName, "c:\\test\\t_sqlconfig_1.db");
_LIT(KTestDbName2, "c:\\test\\t_sqlconfig_2.db");
_LIT(KTestDbName3, "c:[1111C1C1]t_sqlconfig_3.db");
_LIT(KTestDbName4, "c:\\private\\1111C1C1\\t_sqlconfig_4.db");
_LIT(KSqlSrvConfigFile, "c:\\test\\t_sqlserver.cfg");
RFs TheFs;
RSqlDatabase TheDb;
RSqlDatabase TheDb2;

enum TConfigParamType {EPrmCacheSize, EPrmPageSize, EPrmDbEncoding};

//Default (build-time) configuration parameter values
const TInt KDefaultPageSize = 1024;
const TInt KDefaultCacheSize = (TSqlSrvConfigParams::KDefaultSoftHeapLimitKb * 1024) / KDefaultPageSize;
const TSqlSrvConfigParams::TDbEncoding KDefaultEncoding = TSqlSrvConfigParams::EEncUtf16;

///////////////////////////////////////////////////////////////////////////////////////

void DestroyTestEnv()
	{
	TheDb2.Close();
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName4);
	(void)RSqlDatabase::Delete(KTestDbName3);
	(void)RSqlDatabase::Delete(KTestDbName2);
	(void)RSqlDatabase::Delete(KTestDbName);
	(void)TheFs.Delete(KSqlSrvConfigFile);
	TheFs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DestroyTestEnv();
		TheTest(EFalse, aLine);
		}
	}
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DestroyTestEnv();
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

void SetupTestEnv()
    {
	TInt err = TheFs.Connect();
	TEST2(err, KErrNone);

	err = TheFs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	err = TheFs.CreatePrivatePath(EDriveC);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	(void)RSqlDatabase::Delete(KTestDbName3);
	(void)RSqlDatabase::Delete(KTestDbName2);
	(void)RSqlDatabase::Delete(KTestDbName);
	(void)TheFs.Delete(KSqlSrvConfigFile);//The test does not work if there is a test config file.
	}

TInt DoGetConfigParamValueL(RSqlDatabase& aDb, TConfigParamType aPrmType, const TDesC& aLogicalDbName = KNullDesC)
	{
	TBuf<100> sql;
	sql.Copy(_L("PRAGMA "));
	if(aLogicalDbName.Length() > 0)
		{
		sql.Append(aLogicalDbName);
		sql.Append(_L("."));
		}
	
	TSqlScalarFullSelectQuery q(aDb);
	TInt res = 0;
	switch(aPrmType)
		{
		case EPrmCacheSize:
			sql.Append(_L("cache_size"));
			res = q.SelectIntL(sql);
			break;
		case EPrmPageSize:
			sql.Append(_L("page_size"));
			res = q.SelectIntL(sql);
			break;
		case EPrmDbEncoding:
			{
			sql.Append(_L("encoding"));
			TBuf<20> dbEncodingText;
			res = q.SelectTextL(sql, dbEncodingText);
			TEST2(res, KErrNone);
			if(dbEncodingText.FindF(_L("UTF-16")) >= 0)
				{
				res = TSqlSrvConfigParams::EEncUtf16;	
				}
			else if(dbEncodingText.FindF(_L("UTF-8")) >= 0)
				{
				res = TSqlSrvConfigParams::EEncUtf8;	
				}
			else
				{
				TEST2(0, 1);
				}
			}
			break;
		default:
			TEST2(0, 1);
			break;
		}
	return res;
	}

TInt GetConfigParamValue(RSqlDatabase& aDb, TConfigParamType aPrmType, const TDesC& aLogicalDbName = KNullDesC)
	{
	TInt res = 0;
	TRAPD(err, res = DoGetConfigParamValueL(aDb, aPrmType, aLogicalDbName));
	TEST2(err, KErrNone);
	return res;
	}

void AssertConfigPrmValues(RSqlDatabase& aDb, TInt aExpectedCacheSize, TInt aExpectedPageSize, TInt aExpectedDbEncoding, 
		                   const TDesC& aLogicalDbName = KNullDesC)	
	{
	TInt cacheSize = GetConfigParamValue(aDb, EPrmCacheSize, aLogicalDbName);
	TInt pageSize = GetConfigParamValue(aDb, EPrmPageSize ,aLogicalDbName);
	TInt dbEncoding = GetConfigParamValue(aDb, EPrmDbEncoding, aLogicalDbName);
	TEST2(cacheSize, aExpectedCacheSize);
	TEST2(pageSize, aExpectedPageSize);
	TEST2(dbEncoding, aExpectedDbEncoding);
	}

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3436
@SYMTestCaseDesc		Test for DEF105928 "SQL server ignores config string parameters".
						The test attempts to create/open a database using different cache size/page size/
						database encoding configurations and then checks that the configuration parameters
						have been set properly and have the expected values.
@SYMTestPriority		High
@SYMTestActions			Test for DEF105928 "SQL server ignores config string parameters".
@SYMTestExpectedResults The test must not fail
@SYMDEF					DEF105928
*/
void CfgFunctionalTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. No config string. The default config parameters will be used.
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, KDefaultCacheSize, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. Cache size set.
	_LIT8(KCfgStr1, "cache_size=32");
	err = TheDb.Create(KTestDbName, &KCfgStr1);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, 32, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. Page size set.
	_LIT8(KCfgStr2, "page_size=2048");
	err = TheDb.Create(KTestDbName, &KCfgStr2);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, (TSqlSrvConfigParams::KDefaultSoftHeapLimitKb * 1024) / 2048, 2048, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. Cache & Page size set.
	_LIT8(KCfgStr3, "cache_size=256;page_size=4096");
	err = TheDb.Create(KTestDbName, &KCfgStr3);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, 256, 4096, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. Cache size & Page size & db encoding set.
	_LIT8(KCfgStr4, "cache_size=512;page_size=512;encoding=UTF-8");
	err = TheDb.Create(KTestDbName, &KCfgStr4);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, 512, 512, TSqlSrvConfigParams::EEncUtf8);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. Cache size & Page size & db encoding set.
	_LIT8(KCfgStr5, "cache_size=16;page_size=1024;encoding=UTF-16");
	err = TheDb.Create(KTestDbName, &KCfgStr5);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, 16, 1024, TSqlSrvConfigParams::EEncUtf16);
	TheDb.Close();
	//Open the database. Cache size set. The rest of parameter values must be the same as for KCfgStr5.
	_LIT8(KCfgStr6, "cache_size=80");
	err = TheDb.Open(KTestDbName, &KCfgStr6);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, 80, 1024, TSqlSrvConfigParams::EEncUtf16);
	TheDb.Close();
	//Open the database. Attempt to set the page size (impossible when opening a database).
	_LIT8(KCfgStr7, "page_size=2048");
	err = TheDb.Open(KTestDbName, &KCfgStr7);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, KDefaultCacheSize, 1024, TSqlSrvConfigParams::EEncUtf16);
	TheDb.Close();
	//Open the database. Attempt to set the encoding (impossible when opening a database).
	_LIT8(KCfgStr8, "encoding=UTF-8");
	err = TheDb.Open(KTestDbName, &KCfgStr8);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, KDefaultCacheSize, 1024, TSqlSrvConfigParams::EEncUtf16);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. Empty config string - 1
	_LIT8(KCfgStr9, "");
	err = TheDb.Create(KTestDbName, &KCfgStr9);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, KDefaultCacheSize, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. Empty config string - 2
	_LIT8(KCfgStr10, "       ");
	err = TheDb.Create(KTestDbName, &KCfgStr10);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, KDefaultCacheSize, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. Empty config string - 3
	_LIT8(KCfgStr11, " ; ;   ;;   ");
	err = TheDb.Create(KTestDbName, &KCfgStr11);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, KDefaultCacheSize, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3437
@SYMTestCaseDesc		Test for DEF105928 "SQL server ignores config string parameters".
						The test attempts to create/open a database using invalid configuration strings,
						too long configuration string, string with missing parameter values, string with
						invalid database encodings. The test expects the SQL server will report a failure
						and will refuse to execute the operation.
@SYMTestPriority		High
@SYMTestActions			Test for DEF105928 "SQL server ignores config string parameters".
@SYMTestExpectedResults The test must not fail
@SYMDEF					DEF105928
*/
void CfgNegativeTest()
	{
	//Create a database. Spelling problem in the parameter name. This is not reported as an error by the SQL server
	//(treated as unknown parameter)
	_LIT8(KCfgStr1, "casche_size = 32");
	TInt err = TheDb.Create(KTestDbName, &KCfgStr1);
	TEST2(err, KErrNone);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. Spelling problem in the parameter value. 
	_LIT8(KCfgStr2, "encoding = UTF-32");
	err = TheDb.Create(KTestDbName, &KCfgStr2);
	TEST2(err, KErrNone);//Invalid encoding: KErrNone, but the encoding will not be used
	AssertConfigPrmValues(TheDb, KDefaultCacheSize, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. Invalid config string.
	_LIT8(KCfgStr3, "dfgjkdfgkdfk; sfkgjhdfgjkdfk; dfgdfetrwer");
	err = TheDb.Create(KTestDbName, &KCfgStr3);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. Too long and invalid config string.
	_LIT8(KCfgStr4, "A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I123456789J123456789K123456789L123456789M123456789N123456789O123456789");
	err = TheDb.Create(KTestDbName, &KCfgStr4);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. No parameter value - 1.
	_LIT8(KCfgStr5, "page_size=");
	err = TheDb.Create(KTestDbName, &KCfgStr5);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. No parameter value - 2.
	_LIT8(KCfgStr6, "page_size=;");
	err = TheDb.Create(KTestDbName, &KCfgStr6);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. Non-number parameter value - 1.
	_LIT8(KCfgStr7, "page_size=aaa;");
	err = TheDb.Create(KTestDbName, &KCfgStr7);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. Non-number parameter value - 2.
	_LIT8(KCfgStr8, "cache_size=weyu34;");
	err = TheDb.Create(KTestDbName, &KCfgStr8);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. "soft_heap_limit_kb" in the config string.
	_LIT8(KCfgStr9, "soft_heap_limit_kb=512;");
	err = TheDb.Create(KTestDbName, &KCfgStr9);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. "free_space_threshold_kb" in the config string.
	_LIT8(KCfgStr10, "free_space_threshold_kb=256;");
	err = TheDb.Create(KTestDbName, &KCfgStr10);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3438
@SYMTestCaseDesc		Test for DEF105928 "SQL server ignores config string parameters".
						The test attempts to create a database with configuration string containing:
						negative or zero page size and cache size, too small or too big page size,
						page size, not power of two. The test expects the SQL server will detect those 
						invalid page size and cache size values and will refuse to execute the operation.
@SYMTestPriority		High
@SYMTestActions			Test for DEF105928 "SQL server ignores config string parameters".
@SYMTestExpectedResults The test must not fail
@SYMDEF					DEF105928
*/
void CfgInvalidCacheAndPageSizeTest()
	{
	//Create a database. Page size < 512.
	_LIT8(KCfgStr1, "page_size=128;");
	TInt err = TheDb.Create(KTestDbName, &KCfgStr1);
	TEST2(err, KErrNone);//Invalid page size: KErrNone expected, but the page size will not be set
	AssertConfigPrmValues(TheDb, KDefaultCacheSize, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. Page size > 32768.
	_LIT8(KCfgStr2, "page_size=65535;");
	err = TheDb.Create(KTestDbName, &KCfgStr2);
	TEST2(err, KErrNone);//Invalid page size: KErrNone expected, but the page size will not be set
	AssertConfigPrmValues(TheDb, KDefaultCacheSize, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. Page size is not power of two.
	_LIT8(KCfgStr3, "page_size=5000;");
	err = TheDb.Create(KTestDbName, &KCfgStr3);
	TEST2(err, KErrNone);//Invalid page size: KErrNone expected, but the page size will not be set
	AssertConfigPrmValues(TheDb, KDefaultCacheSize, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. Zero cache size.
	_LIT8(KCfgStr4, "cache_size=0");
	err = TheDb.Create(KTestDbName, &KCfgStr4);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, 0, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. Negative cache size.
	_LIT8(KCfgStr5, "cache_size=-32");
	err = TheDb.Create(KTestDbName, &KCfgStr5);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. Zero page size.
	_LIT8(KCfgStr6, "page_size=0");
	err = TheDb.Create(KTestDbName, &KCfgStr6);
	TEST2(err, KErrNone);//Invalid page size: KErrNone expected, but the page size will not be set
	AssertConfigPrmValues(TheDb, KDefaultCacheSize, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create a database. Negative page size.
	_LIT8(KCfgStr7, "page_size=-1024");
	err = TheDb.Create(KTestDbName, &KCfgStr7);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3439
@SYMTestCaseDesc		Test for DEF105928 "SQL server ignores config string parameters".
						The test creates two databases using configuration strings and checks that database 1
						configuration has no impact on database 2 configuration.
@SYMTestPriority		High
@SYMTestActions			Test for DEF105928 "SQL server ignores config string parameters".
@SYMTestExpectedResults The test must not fail
@SYMDEF					DEF105928
*/
void CfgCrossConnectionTest()
	{
	//Create database 1 with cache size = 40, page size = 2048.
	_LIT8(KCfgStr1, "cache_size = 40; page_size = 2048");
	TInt err = TheDb.Create(KTestDbName, &KCfgStr1);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, 40, 2048, KDefaultEncoding);
	//Without closing database 1, create database 2 without a configuration string.
	//Check that database 2 uses the default configuration parameters
	err = TheDb2.Create(KTestDbName2);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb2, KDefaultCacheSize, KDefaultPageSize, KDefaultEncoding);
	//Close database 2. Check the configuration parameters of database 1
	TheDb2.Close();
	AssertConfigPrmValues(TheDb, 40, 2048, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName2);
	(void)RSqlDatabase::Delete(KTestDbName);
	//Create 2 databases with different configuration parameters
	_LIT8(KCfgStr2_1, "cache_size = 50; page_size = 512; encoding = \"UTF-16\"");
	_LIT8(KCfgStr2_2, "cache_size = 80; page_size = 4096; encoding = \"UTF-8\"");
	err = TheDb.Create(KTestDbName, &KCfgStr2_1);
	TEST2(err, KErrNone);
	err = TheDb2.Create(KTestDbName2, &KCfgStr2_2);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, 50, 512, TSqlSrvConfigParams::EEncUtf16);
	AssertConfigPrmValues(TheDb2, 80, 4096, TSqlSrvConfigParams::EEncUtf8);
	TheDb2.Close();
	TheDb.Close();
	//Reopen the databases and check the configuration parameters
	err = TheDb.Open(KTestDbName);
	TEST2(err, KErrNone);
	err = TheDb2.Open(KTestDbName2);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, (TSqlSrvConfigParams::KDefaultSoftHeapLimitKb * 1024) / 512, 512, TSqlSrvConfigParams::EEncUtf16);
	AssertConfigPrmValues(TheDb2, (TSqlSrvConfigParams::KDefaultSoftHeapLimitKb * 1024) / 4096, 4096, TSqlSrvConfigParams::EEncUtf8);
	TheDb2.Close();
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName2);
	(void)RSqlDatabase::Delete(KTestDbName);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3440
@SYMTestCaseDesc		Test for DEF105928 "SQL server ignores config string parameters".
						The test creates a secure database with a configuration string. The test must not fail.
@SYMTestPriority		High
@SYMTestActions			Test for DEF105928 "SQL server ignores config string parameters".
@SYMTestExpectedResults The test must not fail
@SYMDEF					DEF105928
*/
void CfgSecureDbTest()
	{
	//Create
	TSecurityPolicy policy(TSecurityPolicy::EAlwaysPass);
	RSqlSecurityPolicy dbPolicy;
	TInt err = dbPolicy.Create(policy);
	TEST2(err, KErrNone);
	_LIT8(KCfgStr1, "cache_size = 80; page_size = 4096; encoding = UTF-8");
	err = TheDb.Create(KTestDbName3, dbPolicy, &KCfgStr1);
	TEST2(err, KErrNone);
	//Since it is a secure database, PRAGMAs cannot be executed (in order to assert the parameter values)
	//AssertConfigPrmValues(TheDb, 80, 4096, TSqlSrvConfig::EEncUtf8);
	TheDb.Close();	
	dbPolicy.Close();
	//Open
	_LIT8(KCfgStr2, "cache_size = 100");
	err = TheDb.Open(KTestDbName3, &KCfgStr2);
	TEST2(err, KErrNone);
	TheDb.Close();	
	(void)RSqlDatabase::Delete(KTestDbName3);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3441
@SYMTestCaseDesc		Test for DEF105928 "SQL server ignores config string parameters".
						The test creates a private database with a configuration string. The test must not fail.
@SYMTestPriority		High
@SYMTestActions			Test for DEF105928 "SQL server ignores config string parameters".
@SYMTestExpectedResults The test must not fail
@SYMDEF					DEF105928
*/
void CfgPrivateDbTest()
	{
	//Create
	_LIT8(KCfgStr1, "cache_size = 80; page_size = 4096; encoding = UTF-8");
	TInt err = TheDb.Create(KTestDbName4, &KCfgStr1);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, 80, 4096, TSqlSrvConfigParams::EEncUtf8);
	TheDb.Close();	
	//Open-1. The cache size can be changed.
	_LIT8(KCfgStr2, "cache_size = 100");
	err = TheDb.Open(KTestDbName4, &KCfgStr2);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, 100, 4096, TSqlSrvConfigParams::EEncUtf8);
	TheDb.Close();	
	//Open-2. The page size cannot be changed if the database does exist already.
	_LIT8(KCfgStr3, "page_size = 512");
	err = TheDb.Open(KTestDbName4, &KCfgStr3);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, (TSqlSrvConfigParams::KDefaultSoftHeapLimitKb * 1024) / 4096, 4096, TSqlSrvConfigParams::EEncUtf8);
	TheDb.Close();	
	(void)RSqlDatabase::Delete(KTestDbName4);
	}

/**
@SYMTestCaseID          SYSLIB-SQL-CT-3480
@SYMTestCaseDesc		Test for INC106788 - Cannot set SQLite page_size bigger than 4k (4096).
						The test attempts to create a database with page size 8K, 16K or 32K, which was not
						possible before (the default page size (1K) has been used).
@SYMTestPriority        High
@SYMTestActions			Test for INC106788 - Cannot set SQLite page_size bigger than 4k (4096).
@SYMTestExpectedResults The test should not fail or panic.
@SYMDEF INC106788
*/
void INC106788()
	{
	//Create a database with page size = 8192.
	(void)RSqlDatabase::Delete(KTestDbName);
	_LIT8(KCfgStr1, "page_size = 8192");
	TInt err = TheDb.Create(KTestDbName, &KCfgStr1);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, (TSqlSrvConfigParams::KDefaultSoftHeapLimitKb * 1024) / 8192, 8192, KDefaultEncoding);
	TheDb.Close();

	//Create a database with page size = 16384.
	(void)RSqlDatabase::Delete(KTestDbName);
	_LIT8(KCfgStr2, "page_size = 16384");
	err = TheDb.Create(KTestDbName, &KCfgStr2);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, (TSqlSrvConfigParams::KDefaultSoftHeapLimitKb * 1024) / 16384, 16384, KDefaultEncoding);
	TheDb.Close();
	
	//Create a database with page size = 32768.
	(void)RSqlDatabase::Delete(KTestDbName);
	_LIT8(KCfgStr3, "page_size = 32768");
	err = TheDb.Create(KTestDbName, &KCfgStr3);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, (TSqlSrvConfigParams::KDefaultSoftHeapLimitKb * 1024) / 32768, 32768, KDefaultEncoding);
	TheDb.Close();
	
	(void)RSqlDatabase::Delete(KTestDbName);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4049
@SYMTestCaseDesc		Database configuration string - injection test.
						The test attempts to create a database using a configuration string
						and passing a DELETE SQL statement as a compaction mode name.
						The database should be created successfully, the invalid compaction mode - ignored,
						the database system tables content shoud not be corrupted by the call.
@SYMTestPriority		High
@SYMTestActions			Database configuration string - injection test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10405
*/
void InjectionTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName);
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(I INTEGER); INSERT INTO A(I) VALUES(1);"));
	TEST(err > 0);
	TheDb.Close();
	
	_LIT8(KConfig1, "cache_size=128;DELETE FROM A");
	err = TheDb.Open(KTestDbName, &KConfig1);
	TEST2(err, KErrArgument);
	err = TheDb.Open(KTestDbName);
	TEST2(err, KErrNone);
	TSqlScalarFullSelectQuery query(TheDb);
	TInt recCount = 0;
	TRAP(err, recCount = query.SelectIntL(_L("SELECT COUNT(*) FROM A")));
	TEST2(err, KErrNone);
	TEST2(recCount, 1);
	TheDb.Close();

	_LIT8(KConfig2, "cache_size=256;compaction=DELETE FROM A;");
	err = TheDb.Open(KTestDbName, &KConfig2);
	TEST2(err, KErrNone);
	query.SetDatabase(TheDb);
	recCount = 0;
	TRAP(err, recCount = query.SelectIntL(_L("SELECT COUNT(*) FROM A")));
	TEST2(err, KErrNone);
	TEST2(recCount, 1);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);

	_LIT8(KConfig3, "cache_size=256;compaction=DELETE FROM symbian_settings;");
	err = TheDb.Create(KTestDbName, &KConfig3);
	TEST2(err, KErrNone);
	query.SetDatabase(TheDb);
	recCount = 0;
	TRAP(err, recCount = query.SelectIntL(_L("SELECT COUNT(*) FROM symbian_settings")));
	TEST2(err, KErrNone);
	TEST2(recCount, 1);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	}
	
/**
@SYMTestCaseID          PDS-SQL-UT-4152
@SYMTestCaseDesc		Test for DEF142305 - RSqlDatabase::Attach does not use cache_size configuration parameters.
						The test verifies that when a database is attached, the attached database cache size will be set
						based on the page size, soft heap limit and default cache size.
@SYMTestPriority        High
@SYMTestActions			Test for DEF142305 - RSqlDatabase::Attach does not use cache_size configuration parameters.
@SYMTestExpectedResults The test must not fail.
@SYMDEF INC106788
*/
void DEF142305()
	{
	_LIT8(KConfig, "cache_size=500");
	
	//Create main db
	TInt err = TheDb.Create(KTestDbName, &KConfig);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, 500, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	
	//Create non-secure db that will be attached 
	err = TheDb.Create(KTestDbName2, &KConfig);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, 500, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();

	//Create private db that will be attached 
	_LIT8(KConfig2, "page_size=2048");
	err = TheDb.Create(KTestDbName4, &KConfig2);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, (TSqlSrvConfigParams::KDefaultSoftHeapLimitKb * 1024) / 2048, 2048, KDefaultEncoding);//2048? - see KConfig2 string
	TheDb.Close();

	_LIT(KAttachDbName, "Attached");
	
	//Attach non-secure db
	err = TheDb.Open(KTestDbName, &KConfig);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, 500, KDefaultPageSize, KDefaultEncoding);
	err = TheDb.Attach(KTestDbName2, KAttachDbName);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, KDefaultCacheSize, KDefaultPageSize, KDefaultEncoding, KAttachDbName);
	err = TheDb.Detach(KAttachDbName);
	TEST2(err, KErrNone);
	TheDb.Close();
	
	//Attach private db
	err = TheDb.Open(KTestDbName, &KConfig);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, 500, KDefaultPageSize, KDefaultEncoding);
	err = TheDb.Attach(KTestDbName4, KAttachDbName);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, (TSqlSrvConfigParams::KDefaultSoftHeapLimitKb * 1024) / 2048, 2048, KDefaultEncoding, KAttachDbName);//2048? - see KConfig2 string
	err = TheDb.Detach(KAttachDbName);
	TEST2(err, KErrNone);
	TheDb.Close();
	
	(void)RSqlDatabase::Delete(KTestDbName4);
	(void)RSqlDatabase::Delete(KTestDbName2);
	(void)RSqlDatabase::Delete(KTestDbName);
	}

void DoTests()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3436 Sql config string - functional test "));
	CfgFunctionalTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3437 Sql config string - negative test "));
	CfgNegativeTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3438 Sql config string - invalid cache size and page size test "));
	CfgInvalidCacheAndPageSizeTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3439 Sql config string - cross connection test "));
	CfgCrossConnectionTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3440 Sql config string - secure database test "));
	CfgSecureDbTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3441 Sql config string - private database test "));
	CfgPrivateDbTest();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-3480 INC106788 - Cannot set SQLite page_size bigger than 4k (4096) "));
	INC106788();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4049 Config string - injection tests"));
	InjectionTest();
	TheTest.Next(_L("@SYMTestCaseID:PDS-SQL-UT-4152: DEF142305 RSqlDatabase::Attach does not use cache_size configuration parameters"));
	DEF142305();
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	
	__UHEAP_MARK;
	
	SetupTestEnv();
	DoTests();
	DestroyTestEnv();
	
	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;
	
	User::Heap().Check();
	return KErrNone;
	}
