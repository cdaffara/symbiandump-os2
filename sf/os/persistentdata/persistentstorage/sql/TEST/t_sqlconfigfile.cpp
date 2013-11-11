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
#include <sqldb.h>
//#include "SqlUtil.h"
#include "SqlSrvConfig.h"
#include "SqlResourceTester.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
/// This test works only if the whole SQL component is built with SYSLIBS_TEST macro defined! ///
/////////////////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_sqlconfigfile test"));

#ifdef SYSLIBS_TEST	

RFs TheFs;
RSqlDatabase TheDb;

_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDbName, "c:\\test\\t_sqlconfigfile.db");
_LIT(KSqlSrvConfigFile, "c:\\test\\t_sqlserver.cfg");
_LIT(KSqlSrvName, "sqlsrv.exe");

enum TConfigParamType {EPrmCacheSize, EPrmPageSize, EPrmDbEncoding};

//Default configuration parameter values, defined in ../test/sqlserver.cfg file
//(the same as the build-time configuration parameter values)
const TInt KDefaultPageSize = 1024;
const TInt KDefaultCacheSize = (TSqlSrvConfigParams::KDefaultSoftHeapLimitKb * 1024) / KDefaultPageSize;
const TSqlSrvConfigParams::TDbEncoding KDefaultEncoding = TSqlSrvConfigParams::EEncUtf16;

TInt KillProcess(const TDesC& aProcessName);

///////////////////////////////////////////////////////////////////////////////////////
// Destroy functions

TInt KillProcess(const TDesC& aProcessName)
	{
	TFullName name;
	//RDebug::Print(_L("Find and kill \"%S\" process.\n"), &aProcessName);
	TBuf<64> pattern(aProcessName);
	TInt length = pattern.Length();
	pattern += _L("*");
	TFindProcess procFinder(pattern);

	while (procFinder.Next(name) == KErrNone)
		{
		if (name.Length() > length)
			{//If found name is a string containing aProcessName string.
			TChar c(name[length]);
			if (c.IsAlphaDigit() ||
				c == TChar('_') ||
				c == TChar('-'))
				{
				// If the found name is other valid application name
				// starting with aProcessName string.
				//RDebug::Print(_L(":: Process name: \"%S\".\n"), &name);
				continue;
				}
			}
		RProcess proc;
		if (proc.Open(name) == KErrNone)
			{
			proc.Kill(0);
			//RDebug::Print(_L("\"%S\" process killed.\n"), &name);
			}
		proc.Close();
		}
	return KErrNone;
	}

void DestroyTestEnv()
	{
	TheDb.Close();
	(void)KillProcess(KSqlSrvName);
	(void)TheFs.Delete(KTestDbName);
	(void)TheFs.Delete(KSqlSrvConfigFile);
	TheFs.Close();
	}
	
///////////////////////////////////////////////////////////////////////////////////////
// Test macros and functions

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

// OOM test functions

static TInt TheHandleCount1;
static TInt TheHandleCount2;
static TInt TheAllocatedCellsCount;

void MarkHandles()
	{
	RThread().HandleCount(TheHandleCount1, TheHandleCount2);
	}

void CheckHandles()
	{
	TInt endHandleCount1;
	TInt endHandleCount2;

	RThread().HandleCount(endHandleCount1, endHandleCount2);

	TEST(TheHandleCount1 == endHandleCount1);
	TEST(TheHandleCount2 == endHandleCount2);
	}

void MarkAllocatedCells()
	{
	TheAllocatedCellsCount = User::CountAllocCells();
	}

void CheckAllocatedCells()
	{
	TInt allocatedCellsCount = User::CountAllocCells();
	TEST(allocatedCellsCount == TheAllocatedCellsCount);
	}

///////////////////////////////////////////////////////////////////////////////////////
// Set up functions

TInt DoCreateSecurityPolicy(RSqlSecurityPolicy& securityPolicy)
	{
	const TSecurityPolicy KDefaultPolicy(TSecurityPolicy::EAlwaysPass);
	if((KErrNone != securityPolicy.Create(KDefaultPolicy))
	   ||
	   (KErrNone != securityPolicy.SetDbPolicy(RSqlSecurityPolicy::ESchemaPolicy, KDefaultPolicy))
	   ||
	   (KErrNone != securityPolicy.SetDbPolicy(RSqlSecurityPolicy::EWritePolicy, KDefaultPolicy))
	   ||
	   (KErrNone != securityPolicy.SetDbPolicy(RSqlSecurityPolicy::EReadPolicy, KDefaultPolicy)))
		{
		return KErrGeneral;
		}
		
	return KErrNone;
	}
	
void SetupTestEnv()
    {
	TInt err = TheFs.Connect();
	TEST2(err, KErrNone);

	err = TheFs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	err = TheFs.CreatePrivatePath(EDriveC);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	(void)TheFs.Delete(KTestDbName);
	(void)TheFs.Delete(KSqlSrvConfigFile);
	}

///////////////////////////////////////////////////////////////////////////////////////
// Parameter check functions

TInt DoGetConfigParamValueL(RSqlDatabase& aDb, TConfigParamType aPrmType)
	{
	TSqlScalarFullSelectQuery q(aDb);
	TInt res = 0;
	switch(aPrmType)
		{
		case EPrmCacheSize:
			res = q.SelectIntL(_L8("PRAGMA cache_size"));
			break;
		case EPrmPageSize:
			res = q.SelectIntL(_L8("PRAGMA page_size"));
			break;
		case EPrmDbEncoding:
			{
			TBuf<20> dbEncodingText;
			res = q.SelectTextL(_L8("PRAGMA encoding"), dbEncodingText);
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

TInt GetConfigParamValue(RSqlDatabase& aDb, TConfigParamType aPrmType)
	{
	TInt res = 0;
	TRAPD(err, res = DoGetConfigParamValueL(aDb, aPrmType));
	TEST2(err, KErrNone);
	return res;
	}

void AssertConfigPrmValues(RSqlDatabase& aDb, TInt aExpectedCacheSize, TInt aExpectedPageSize, TInt aExpectedDbEncoding)
	{
	TInt cacheSize = GetConfigParamValue(aDb, EPrmCacheSize);
	TInt pageSize = GetConfigParamValue(aDb, EPrmPageSize);
	TInt dbEncoding = GetConfigParamValue(aDb, EPrmDbEncoding);
	TEST2(cacheSize, aExpectedCacheSize);
	TEST2(pageSize, aExpectedPageSize);
	TEST2(dbEncoding, aExpectedDbEncoding);
	}

///////////////////////////////////////////////////////////////////////////////////////
// Config file replacement functions

// File config strings are 16-bit.
void ReplaceConfigFile(const TDesC16& aConfig)
	{
	(void)KillProcess(KSqlSrvName);
	(void)TheFs.Delete(KSqlSrvConfigFile);
	RFile file;
	TInt err = file.Create(TheFs, KSqlSrvConfigFile, EFileRead | EFileWrite);
	TEST2(err, KErrNone);
	TPtrC8 p((const TUint8*)aConfig.Ptr(), aConfig.Length() * sizeof(TUint16));
	err = file.Write(p);
	file.Close();
	TEST2(err, KErrNone);
	}
	
///////////////////////////////////////////////////////////////////////////////////////
//

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3603
@SYMTestCaseDesc		Bad config file test
						The test creates bad config files like:
							- empty config file;
							- "\n" config file;
							- "\r\n" config file;
							- config file with comment lines only;
						Then the test restarts the SQL server and checks that the bad config file is detected and 
						appropriate error code - returned to the caller (during "database create" operation).
@SYMTestPriority		High
@SYMTestActions			Bad config file test
@SYMTestExpectedResults The test must not fail
@SYMREQ					REQ8162
*/
void BadCfgFileTest()
	{
	//Empty config file	
	ReplaceConfigFile(_L(""));
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrEof);//BC kept - an empty config file is treated as invalid
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"\n" config file	
	ReplaceConfigFile(_L("\n"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrEof);//BC compatible
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"\r\n" config file	
	ReplaceConfigFile(_L("\r\n"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrEof);//BC compatible
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"        \r\n" config file	
	ReplaceConfigFile(_L("        \r\n"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrEof);//BC compatible
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"  # \r\n" config file
	ReplaceConfigFile(_L("  # \r\n"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrEof);//BC compatible
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"  # \r\na=b\r\n" config file
	ReplaceConfigFile(_L("  # \r\na=b\r\n"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, KDefaultCacheSize, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"  # \r\n   a=b   \r\n" config file
	ReplaceConfigFile(_L("  # \r\n   a=b   \r\n"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, KDefaultCacheSize, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"  # \r\n   a=b  " config file
	ReplaceConfigFile(_L("  # \r\n   a=b  "));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, KDefaultCacheSize, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	(void)TheFs.Delete(KSqlSrvConfigFile);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3604
@SYMTestCaseDesc		Config file with bad parameters test
						The test creates config files with bad parameters like:
							- negative cache size;
							- non-numeric cache size value;
							- empty cache size value;
							- negative page size;
							- non-numeric page size value;
							- empty page size value;
							- negative soft heap limit size;
							- non-numeric soft heap limit value;
							- empty soft heap limit value;
							- too small soft heap limit value;
							- too big soft heap limit value;
							- negative free page threshold value;
							- empty free page threshold value;
							- non-numeric free page threshold value;
						Then the test restarts the SQL server and checks that the bad config file is detected and 
						appropriate error code - returned to the caller (during "database create" operation).
@SYMTestPriority		High
@SYMTestActions			Config file with bad parameters test
@SYMTestExpectedResults The test must not fail
@SYMREQ					REQ8162
                        REQ10271
*/
void BadCfgFileParametersTest()
	{
	/////////////// cache_size  ////////////////
	//"cache_size=-20;" config file
	ReplaceConfigFile(_L("cache_size=-20;"));
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"cache_size=456.90" config file
	ReplaceConfigFile(_L("cache_size=456.90"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"cache_size='dfjkhdfjk';" config file
	ReplaceConfigFile(_L("cache_size='dfjkhdfjk';"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"cache_size=;" config file
	ReplaceConfigFile(_L("cache_size=;"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	/////////////// page_size  ////////////////
	//"page_size=-55" config file
	ReplaceConfigFile(_L("page_size=-55"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"page_size=25.89" config file
	ReplaceConfigFile(_L("page_size=25.89"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);//BC compatible
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"page_size=gffgrtj" config file
	ReplaceConfigFile(_L("page_size=gffgrtj"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"page_size=" config file
	ReplaceConfigFile(_L("page_size="));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	////////   soft_heap_limit_kb    ///////////
	//"soft_heap_limit_kb=-10" config file
	ReplaceConfigFile(_L("soft_heap_limit_kb=-10"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"soft_heap_limit_kb=5" config file (bellow min limit - 8Kb when SYSLIBS_TEST macro is defined)
	ReplaceConfigFile(_L("soft_heap_limit_kb=5"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"soft_heap_limit_kb=8" config file (the min limit - 8Kb when SYSLIBS_TEST macro is defined)
	ReplaceConfigFile(_L("soft_heap_limit_kb=8"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"soft_heap_limit_kb=2000000000" config file (above max limit)
	ReplaceConfigFile(_L("soft_heap_limit_kb=2000000000"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"soft_heap_limit_kb=KMaxTInt/1024" config file (the max limit)
	TBuf<32> configBuf;
	configBuf.Copy(_L("soft_heap_limit_kb="));
	TInt maxSoftHeapLimit = KMaxTInt / 1024;
	configBuf.AppendNum(maxSoftHeapLimit);
	ReplaceConfigFile(configBuf);
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"soft_heap_limit_kb=sdfcvyua" config file
	ReplaceConfigFile(_L("soft_heap_limit_kb=sdfcvyua"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"soft_heap_limit_kb=" config file
	ReplaceConfigFile(_L("soft_heap_limit_kb="));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"soft_heap_limit_kb=1023.456" config file
	ReplaceConfigFile(_L("soft_heap_limit_kb=1023.456"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	////////   free_space_threshold_kb    ///////////
	//"free_space_threshold_kb=-10" config file
	ReplaceConfigFile(_L("free_space_threshold_kb=-10"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"free_space_threshold_kb=0" config file
	ReplaceConfigFile(_L("free_space_threshold_kb=0"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"free_space_threshold_kb=" config file
	ReplaceConfigFile(_L("free_space_threshold_kb="));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"free_space_threshold_kb=34.56" config file
	ReplaceConfigFile(_L("free_space_threshold_kb=34.56"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"free_space_threshold_kb=gfghfg" config file
	ReplaceConfigFile(_L("free_space_threshold_kb=gfghfg"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	/////////////////////////////////////////////
	(void)TheFs.Delete(KSqlSrvConfigFile);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3605
@SYMTestCaseDesc		Config parameters conflict test.
						1) The test creates a database with cache size parameter value specified in both the config file and the
							config string. The expectation is that the config string parameter will be used.
						2) The test creates a database with page size parameter value specified in both the config file and the
							config string. The expectation is that the config string parameter will be used.
						3) The test creates a database with encoding parameter value specified in both the config file and the
							config string. The expectation is that the config string parameter will be used.
						4) The test creates a database with soft heap limit value specified in both the config file and the
							config string. The expectation is that the database creation will fail (the soft heap limit
							cannot be configured using a config string).
						5) The test creates a database with free page threshold value specified in both the config file and the
							config string. The expectation is that the database creation will succeeds. The free page threshold
							value from the config file will be used.
@SYMTestPriority		High
@SYMTestActions			Config parameters conflict test
@SYMTestExpectedResults The test must not fail
@SYMREQ					REQ8162
                        REQ10271
*/
void CfgFileConflictTest()
	{
	//"cache_size=200" config file
	//"cache_size=100" client config string
	ReplaceConfigFile(_L("cache_size=200"));
	_LIT8(KConfigStr1, "cache_size=100");
	TInt err = TheDb.Create(KTestDbName, &KConfigStr1);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, 100, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"page_size=512" config file
	//"page_size=8192" client config string
	ReplaceConfigFile(_L("page_size=512"));
	_LIT8(KConfigStr2, "page_size=8192");
	err = TheDb.Create(KTestDbName, &KConfigStr2);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, (TSqlSrvConfigParams::KDefaultSoftHeapLimitKb * 1024) / 8192, 8192, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"encoding=UTF-16" config file
	//"encoding=UTF-8" client config string
	ReplaceConfigFile(_L("encoding=UTF-16"));
	_LIT8(KConfigStr3, "encoding=UTF-8");
	err = TheDb.Create(KTestDbName, &KConfigStr3);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, KDefaultCacheSize, KDefaultPageSize, TSqlSrvConfigParams::EEncUtf8);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"soft_heap_limit_kb=900" config file
	//"soft_heap_limit_kb=800" client config string
	ReplaceConfigFile(_L("soft_heap_limit_kb=900"));
	_LIT8(KConfigStr4, "soft_heap_limit_kb=800");
	err = TheDb.Create(KTestDbName, &KConfigStr4);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"free_space_threshold_kb=100" config file
	//"free_space_threshold_kb=200" client config string
	ReplaceConfigFile(_L("free_space_threshold_kb=100"));
	_LIT8(KConfigStr5, "free_space_threshold_kb=200");
	err = TheDb.Create(KTestDbName, &KConfigStr5);
	TEST2(err, KErrArgument);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	(void)TheFs.Delete(KSqlSrvConfigFile);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3606
@SYMTestCaseDesc		Soft Heap Limit - functional test.
						The test attempts to create a database with the soft heap limit value specified in the config file
						and different combinations of the page size and cache size parameters in both config file and client
						config string. The expectation is that when the cache size parameter value is not specified explicitly
						in the config file or in the config string, the cache size value will be calculated, using the soft
						heap limit and the database page size.
@SYMTestPriority		High
@SYMTestActions			Soft Heap Limit - functional test.
@SYMTestExpectedResults The test must not fail
@SYMREQ					REQ8162
*/
void SoftHeapLimitFunctionalTest1()
	{
	///////////////////// CREATE DATABASE /////////////////////////////////////////////////////////
	//"soft_heap_limit_kb=512" config file. (512 is the min soft heap limit value)
	//Expected result: the database cache size will be (512 * 1024)/page_size;
	ReplaceConfigFile(_L("soft_heap_limit_kb=512"));
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, (512 * 1024) / KDefaultPageSize, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"soft_heap_limit_kb=KMaxTInt/1024" config file. (KMaxTInt / 1024 is the max soft heap limit value)
	//Expected result: the database cache size will be KMaxTInt/page_size;
	TBuf<32> configBuf;
	configBuf.Copy(_L("soft_heap_limit_kb="));
	TInt maxSoftHeapLimit = KMaxTInt / 1024;
	configBuf.AppendNum(maxSoftHeapLimit);
	ReplaceConfigFile(configBuf);
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, KMaxTInt / KDefaultPageSize, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"soft_heap_limit_kb=512;page_size=2048" config file.
	//Expected result: the database cache size will be (512 * 1024)/2048. The page size value from the config file is used.
	ReplaceConfigFile(_L("soft_heap_limit_kb=512;page_size=2048"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, (512 * 1024) / 2048, 2048, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"soft_heap_limit_kb=512" config file.
	//"page_size=4096" client config string.
	//Expected result: the database cache size will be (512 * 1024)/4096. The page size value from the client config string is used.
	ReplaceConfigFile(_L("soft_heap_limit_kb=512;"));
	_LIT8(KConfigStr1, "page_size=4096");
	err = TheDb.Create(KTestDbName, &KConfigStr1);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, (512 * 1024) / 4096, 4096, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"soft_heap_limit_kb=512;page_size=8192" config file.
	//"page_size=2048" client config string.
	//Expected result: the database cache size will be (512 * 1024)/2048. The page size value from the client config string is used.
	ReplaceConfigFile(_L("soft_heap_limit_kb=512;page_size=8192"));
	_LIT8(KConfigStr2, "page_size=2048");
	err = TheDb.Create(KTestDbName, &KConfigStr2);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, (512 * 1024) / 2048, 2048, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"soft_heap_limit_kb=512;page_size=2048;encoding=UTF-8" config file.
	//"cache_size=100" client config string.
	//Expected result: the database cache size will be 100. The soft heap limit is not used for the cache size calculation.
	ReplaceConfigFile(_L("soft_heap_limit_kb=512;page_size=2048;encoding=UTF-8"));
	_LIT8(KConfigStr3, "cache_size=100");
	err = TheDb.Create(KTestDbName, &KConfigStr3);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, 100, 2048, TSqlSrvConfigParams::EEncUtf8);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	(void)TheFs.Delete(KSqlSrvConfigFile);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-3607
@SYMTestCaseDesc		Soft Heap Limit - functional test.
						The test attempts to open a database with the soft heap limit value specified in the config file
						and different combinations of the page size and cache size parameters in both config file and client
						config string. The expectation is that when the cache size parameter value is not specified explicitly
						in the config file or in the config string, the cache size value will be calculated, using the soft
						heap limit and the database page size (read from the database, not from the config file or string).
@SYMTestPriority		High
@SYMTestActions			Soft Heap Limit - functional test.
@SYMTestExpectedResults The test must not fail
@SYMREQ					REQ8162
*/
void SoftHeapLimitFunctionalTest2()
	{
	///////////////////// OPEN DATABASE /////////////////////////////////////////////////////////
	//"soft_heap_limit_kb=512;page_size=2048" config file.
	//Expected result: the database cache size will be (512 * 1024)/2048. The database page size value is used (not the built-time one).
	ReplaceConfigFile(_L("soft_heap_limit_kb=512;page_size=2048"));
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, (512 * 1024) / 2048, 2048, KDefaultEncoding);
	TheDb.Close();
	ReplaceConfigFile(_L("soft_heap_limit_kb=1024;page_size=8192"));
	err = TheDb.Open(KTestDbName);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, (1024 * 1024) / 2048, 2048, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"soft_heap_limit_kb=512" config file.
	//"page_size=4096" client config string.
	//Expected result: the database cache size will be (512 * 1024)/4096. The database page size value is used (not the built-time one).
	ReplaceConfigFile(_L("soft_heap_limit_kb=512"));
	_LIT8(KConfigStr1, "page_size=4096");
	err = TheDb.Create(KTestDbName, &KConfigStr1);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, (512 * 1024) / 4096, 4096, KDefaultEncoding);
	TheDb.Close();
	ReplaceConfigFile(_L("soft_heap_limit_kb=1024;page_size=8192"));
	err = TheDb.Open(KTestDbName);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, (1024 * 1024) / 4096, 4096, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	//"soft_heap_limit_kb=512" config file.
	//"page_size=4096" client config string when openning the database.
	//Expected result: the database cache size will be 512. The database page size value is used (the built-time one).
	ReplaceConfigFile(_L("soft_heap_limit_kb=512"));
	err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, 512, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	ReplaceConfigFile(_L("soft_heap_limit_kb=1024;page_size=512"));
	_LIT8(KConfigStr2, "page_size=4096");
	err = TheDb.Open(KTestDbName, &KConfigStr2);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, 1024, KDefaultPageSize, KDefaultEncoding);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	(void)TheFs.Delete(KSqlSrvConfigFile);
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-3608
@SYMTestCaseDesc		Soft Heap Limit - file I/O failure simulation test.
						The test creates a database with very small soft heap limit value (8Kb).
						Then the test attempts to insert a record in an explicit transaction while doing
						file I/O failure simulation.
@SYMTestPriority		High
@SYMTestActions			Soft Heap Limit - file I/O failure simulation test.
@SYMTestExpectedResults The test must not fail
@SYMREQ					REQ8162
                        REQ10271
*/
void FileIOFailureTest()
	{
	(void)RSqlDatabase::Delete(KTestDbName);
	ReplaceConfigFile(_L("soft_heap_limit_kb=8;cache_size=16;page_size=512;free_space_threshold_kb=100"));
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER,Name TEXT)"));
	TEST(err >= 0);
	TheDb.Close();
	err = -1;
	const TInt KTestRecCnt = 100;
	for(TInt cnt=0;err<KErrNone;++cnt)
		{		
		TheTest.Printf(_L("%d \r"), cnt);		
		err = TheDb.Open(KTestDbName);
		TEST2(err, KErrNone);
		TInt recCntBegin = 0;
		TSqlScalarFullSelectQuery q1(TheDb);
		TRAP(err, recCntBegin = q1.SelectIntL(_L("SELECT COUNT (*) FROM A")));
		TEST2(err, KErrNone);
		(void)TheFs.SetErrorCondition(KErrGeneral, cnt);
		err = TheDb.Exec(_L("BEGIN TRANSACTION"));
		if(err == KErrNone)
			{
			for(TInt i=0;i<KTestRecCnt;++i)
				{
				TBuf<300> sql;
				sql.Format(_L("INSERT INTO A(Id,Name) VALUES(%d, 'A1234567890B1234567890C1234567890D1234567890E1234567890F1234567890G1234567890H1234567890I1234567890J1234567890K1234567890L1234567890M1234567890N1234567890O1234567890P1234567890Q1234567890R1234567890')"), cnt);
				err = TheDb.Exec(sql);
				TEST(err == 1 || err < 0);
				if(err < 0)
					{
					break;
					}
				}
			if(err == 1)
				{
				err = TheDb.Exec(_L("COMMIT TRANSACTION"));
				}
			else if(TheDb.InTransaction()) //the transaction may have been rolled back automatically
				{
				err = TheDb.Exec(_L("ROLLBACK TRANSACTION"));
				}
			if(err == 0)
				{
				err = 1;	
				}
			}
		(void)TheFs.SetErrorCondition(KErrNone);
		if(err < 1)
			{
			TheDb.Close();//close the database to recover from the last error
			TInt err2 = TheDb.Open(KTestDbName);
			TEST2(err2, KErrNone);
			}
		TSqlScalarFullSelectQuery q2(TheDb);
		TInt recCntEnd = 0;
		TRAPD(err3, recCntEnd = q2.SelectIntL(_L("SELECT COUNT (*) FROM A")));
		TheDb.Close();
		TEST2(err3, KErrNone);
		//check the database content - all bets are off in a case of an I/O error. 
		//The new records may have actually been inserted.
		TEST(recCntEnd == recCntBegin || recCntEnd == (recCntBegin + KTestRecCnt));
		}
	(void)TheFs.SetErrorCondition(KErrNone);
	(void)RSqlDatabase::Delete(KTestDbName);
	(void)TheFs.Delete(KSqlSrvConfigFile);
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-3609
@SYMTestCaseDesc		Soft Heap Limit - OOM test.
						The test creates a database with very small soft heap limit value (8Kb).
						The the test attempts to insert a record in an explicit transaction while doing
						OOM simulation.
@SYMTestPriority		High
@SYMTestActions			Soft Heap Limit - OOM test.
@SYMTestExpectedResults The test must not fail
@SYMREQ					REQ8162
                        REQ10271
*/
void OOMtest()
	{
	(void)RSqlDatabase::Delete(KTestDbName);
	ReplaceConfigFile(_L("soft_heap_limit_kb=8;cache_size=16;page_size=512;free_space_threshold_kb=150"));
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(Id INTEGER,Name TEXT)"));
	TEST(err >= 0);
	TheDb.Close();

	const TInt KOomIterationCount = 1000;//Instead fo doing the OOM test while "err == KErrNoMemory", the test is
										 //performed KOomIterationCount times, because the "soft heap limit" will
										 //force the SQLite library to reuse some of the already allocated but not used pages.
	TInt failingAllocationNo = 0;
	while(failingAllocationNo < KOomIterationCount)
		{
		__UHEAP_MARK;

		const TInt KDelayedDbHeapFailureMask = 0x1000;
		TSqlResourceTester::SetDbHeapFailure(RHeap::EFailNext | KDelayedDbHeapFailureMask, ++failingAllocationNo);

		err = TheDb.Open(KTestDbName);
		TEST2(err, KErrNone);

		err = TheDb.Exec(_L("BEGIN TRANSACTION"));
		if(err == KErrNone)
			{
			const TInt KTestRecCnt = 4;
			for(TInt i=0;i<KTestRecCnt;++i)
				{
				err = TheDb.Exec(_L("INSERT INTO A(Id,Name) VALUES(1, 'A1234567890B1234567890C1234567890D1234567890E1234567890F1234567890G1234567890H1234567890I1234567890J1234567890K1234567890L1234567890M1234567890N1234567890O1234567890P1234567890Q1234567890R1234567890')"));
				if(err < 1)
					{
					break;
					}
				}
			if(err == 1)
				{
				err = TheDb.Exec(_L("COMMIT TRANSACTION"));
				}
			else if(TheDb.InTransaction()) //the transaction may have been rolled back automatically
				{
				err = TheDb.Exec(_L("ROLLBACK TRANSACTION"));
				}
			}
		
		TSqlResourceTester::SetDbHeapFailure(RHeap::ENone, 0);
	
		TheDb.Close();	

		TheTest.Printf(_L("%d/%d   \r"), failingAllocationNo, err);
		
		__UHEAP_MARKEND;
		
		TEST(err >= 0 || err == KErrNoMemory);
		}
	TEST(err >= 0);
	(void)RSqlDatabase::Delete(KTestDbName);
	(void)TheFs.Delete(KSqlSrvConfigFile);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4081
@SYMTestCaseDesc		Background compaction, free page threshold - functional test.
						The test creates a server config file, where the free page threshold is set to be 20 Kb.
						Then the test creates a database. The test inserts 40 pages (40 Kb) into the database, closes and 
						reopens the database. Then the test deletes some records from the database.
						But the space in the free pages is not big enough to kick-off the background compaction.
						The test checks that no compaction has occurred after the deletions.
						The test deletes more records and the free page threshold is reached.
						The test checks that after the last deletion the database really has been compacted.
@SYMTestPriority		Medium
@SYMTestActions			Background compaction, free page threshold - functional test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10271
*/
void FreePageThresholdTest()
	{
	const TInt KFreePageThresholdSrvCfgKb = 20;
	TBuf<50> cfgBuf1;
	cfgBuf1.Format(_L("free_space_threshold_kb=%d"), KFreePageThresholdSrvCfgKb);
	ReplaceConfigFile(cfgBuf1);
	
	const TInt KPageSize = 1024;
	TBuf8<50> cfgBuf2;
	cfgBuf2.Format(_L8("page_size=%d;"), KPageSize);
	//Create a database and insert some records. At the end the database size is bigger than the free pages threshold.
	(void)RSqlDatabase::Delete(KTestDbName);
	TInt err = TheDb.Create(KTestDbName, &cfgBuf2);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L("CREATE TABLE A(B BLOB)"));
	TEST2(err, 1);
	TBuf8<(KPageSize - 150) * 2> blob;
	blob.SetLength((KPageSize - 150) * 2);
	blob.Fill(TChar('A'));
	for(TInt i=0;i<KFreePageThresholdSrvCfgKb*2;++i)
		{
		TBuf8<KPageSize * 2> sql;
		sql.Format(_L8("INSERT INTO A VALUES(x'%S')"), &blob);
		err = TheDb.Exec(sql);
		TEST2(err, 1);
		}
	TheDb.Close();
	//Reopen the database and delete some records. The free spave is not big enough to kick-off the background compaction.
	err = TheDb.Open(KTestDbName);
	TEST2(err, KErrNone);
	for(TInt i=0;i<10;++i)
		{
		TBuf8<50> sql;
		sql.Format(_L8("DELETE FROM A WHERE ROWID=%d"), i + 1);
		err = TheDb.Exec(sql);
		TEST2(err, 1);
		}
	User::After(1000000);
	RSqlDatabase::TSize size;
	err = TheDb.Size(size);
	TEST2(err, KErrNone);
	TEST(size.iFree > 0);
	//Delete more records, the free page threshold is reached, the background compaction - kicked-off.
	for(TInt i=10;i<20;++i)
		{
		TBuf8<50> sql;
		sql.Format(_L8("DELETE FROM A WHERE ROWID=%d"), i + 1);
		err = TheDb.Exec(sql);
		TEST2(err, 1);
		}
	User::After(1000000);
	err = TheDb.Size(size);
	TEST2(err, KErrNone);
	TEST2(size.iFree, 0);
	//
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	(void)TheFs.Delete(KSqlSrvConfigFile);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4075
@SYMTestCaseDesc		Server configuration file, large string test.
						The test creates a server config file, where all parameters are used
						and checks the the parameter values are processed normally.
@SYMTestPriority		Medium
@SYMTestActions			Server configuration file, large string test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10271
*/
void LargeStringTest()
	{
	ReplaceConfigFile(_L("page_size=32768;cache_size=2048;encoding=UTF-16;soft_heap_limit_kb=2048;free_space_threshold_kb=100000000;compaction=background"));
	TInt err = TheDb.Create(KTestDbName);
	TEST2(err, KErrNone);
	AssertConfigPrmValues(TheDb, (2048 * 1024) / 32768, 32768, TSqlSrvConfigParams::EEncUtf16);
	TheDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName);
	}

void DoTests()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3603 Bad config file "));
	BadCfgFileTest();
 	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3604 Config file - bad parameters "));
	BadCfgFileParametersTest();
 	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3605 Config file - conflict test "));
	CfgFileConflictTest();
 	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3606 Soft heap limit - functional test (\"create database\") "));
 	SoftHeapLimitFunctionalTest1();
 	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3607 Soft heap limit - functional test (\"open database\") "));
 	SoftHeapLimitFunctionalTest2();
 	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3608 Soft heap limit - file I/O failure "));
    FileIOFailureTest();
 	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-3609 Soft heap limit - OOM failure "));
	OOMtest();
 	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4081 SQL server configuration file + free page threshold - functional test "));
 	FreePageThresholdTest();
 	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4075 SQL server configuration file + large string "));
 	LargeStringTest();
	}
	
#endif	//SYSLIBS_TEST

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	
	__UHEAP_MARK;

#ifdef SYSLIBS_TEST	
	TheTest.Start(_L("t_sqlconfigfile tests"));

	SetupTestEnv();
	DoTests();
 	DestroyTestEnv();
	
	TheTest.End();
#else
 	TheTest.Start(_L("This test works only if the whole SQL component is built with SYSLIBS_TEST macro defined!"));
	TheTest.End();
#endif	
	
	__UHEAP_MARKEND;
	
	TheTest.Close();
	
	delete tc;
	
	User::Heap().Check();
	return KErrNone;
	}
