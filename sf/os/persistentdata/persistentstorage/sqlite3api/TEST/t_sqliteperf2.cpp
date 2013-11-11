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
//

#include <e32test.h>
#include <bautils.h>
#include <e32math.h>
#include <hal.h>
#include <sqlite3.h>
#include <string.h>
#include <stdio.h>
#include "sqliteTestUtl.h"

///////////////////////////////////////////////////////////////////////////////////////

enum TPerfTestType
	{
	EPerfTestMultiInsert,
	EPerfTestMultiUpdate,
	EPerfTestMultiDelete,
	EPerfTestMultiSelect,
	EPerfTestSingleInsert,
	EPerfTestSingleUpdate,
	EPerfTestSingleDelete,
	EPerfTestSingleSelect,
	//
	EPerfTestTypeCnt
	};

//The performance test case results are stored in the ThePerfTestResult array.
static TUint32 ThePerfTestResult[EPerfTestTypeCnt];

const char* const KTestName = "t_sqliteperf2 test";

_LIT(KTestDir,              "c:\\test\\");
_LIT(KTestDbName,           "c:\\test\\t_sqliteperf2.db");
const char KTestDbName8[] = "c:\\test\\t_sqliteperf2.db";

RFs TheFs;

sqlite3* TheDb = 0;

const TInt KInsertRecCnt = 1000;

const TInt KUpdateRecCnt = KInsertRecCnt / 10;
const TInt KDeleteRecCnt = KInsertRecCnt / 10;
const TInt KSelectRecCnt = KInsertRecCnt / 10;

const TInt KInsertRecIds[KInsertRecCnt] = 
	{
	18920,515,22944,32238,20164,9592,32360,29280,6219,26241,11175,15824,20409,14659,30084,4630,21646,24582,12976,21104,1834,
	23425,15260,9692,8601,12375,10729,2244,4784,9697,11296,8672,3929,25134,16803,24153,20011,1864,29411,6442,8510,
	13753,22996,20940,25149,6222,3742,13842,25788,23835,19606,850,25292,16319,25452,709,6093,2282,18597,9520,32132,
	10012,23884,452,24430,17583,15333,15427,14419,32535,8867,5733,3613,9065,19931,12410,16363,5146,22199,15696,31330,
	15369,27183,11607,17987,12992,27701,5379,25923,30199,23344,21106,28951,23642,1964,13903,28878,9613,29776,13302,22627,
	19308,29200,23769,9875,30871,19620,11042,5356,18478,6968,24474,3623,13017,2405,30409,24247,19724,11360,31071,21410,
	14985,14449,11218,15248,31526,10919,24905,32289,29904,15051,30137,15748,24272,10642,13133,13894,29261,16150,9996,20839,
	1042,3463,10422,7347,27538,19486,6638,7503,23337,24147,8350,12435,3493,23376,24728,28439,27909,10661,30841,17937,
	5761,829,17875,15070,24522,30097,13451,3294,16786,12827,7551,10629,19279,17166,30828,9903,1084,30788,25872,14132,
	23969,8312,20363,19901,6741,25269,13323,25046,20353,19470,17832,2884,25254,26000,31885,26520,10466,26229,20313,32260,
	2927,22170,7109,5486,21868,13620,646,12483,3808,30654,14965,9614,2300,30006,7917,201,32672,4488,32116,11762,
	29729,15691,26270,5218,9546,7595,1583,7899,18592,28838,18048,24928,26858,380,17188,20809,3569,16736,20170,17302,
	4485,22644,4197,9604,17856,17140,31627,9054,20623,20145,10267,12694,24685,3266,19747,11284,7794,2934,25345,32718,
	397,27789,12488,4761,26382,18661,23870,5223,10492,14594,27601,1587,19534,28970,14398,12407,21532,9774,4901,5160,
	9497,13128,3816,19950,11740,16127,8450,552,10058,28058,18992,5776,30968,29015,1760,29188,30287,32119,15445,4463,
	28916,14875,23282,10548,31499,8933,6568,22293,13214,12741,17781,17318,12867,11280,10795,30721,22009,30835,15292,31572,
	3942,19729,3864,24026,24900,15722,5499,10412,14873,3395,1057,31337,17181,29131,26894,24655,28321,1500,24135,3235,
	25831,21374,32065,19539,13111,5805,9882,32731,28975,7516,29191,7013,20571,26809,23182,29236,18950,29619,31477,20802,
	24399,17081,28282,11560,8063,8389,10692,26431,26777,11674,24602,28165,5683,8522,18643,15554,4741,26874,22797,32363,
	32192,233,17826,22470,24481,12207,24920,7375,5957,1731,11096,30043,32063,19520,8674,13563,27624,12777,11582,27171,
	8694,32106,14070,26763,13136,1505,16706,21206,31777,10908,4804,26482,18987,10020,19061,5311,12383,26747,28640,6259,
	14506,18339,21390,11837,26641,19807,21441,4520,18688,25406,22885,22339,28639,5588,8515,24831,11751,19531,29144,26164,
	6427,15141,9399,14619,2069,14441,19095,31165,3494,13964,29671,8668,31578,17533,10544,14318,7583,5880,23408,26316,
	14367,12518,27517,19297,16745,4135,31647,13200,22619,4367,2789,9725,7578,29700,12598,4035,27492,1432,14052,19895,
	18886,4597,19076,23280,29455,10110,18686,18567,31876,27827,19424,25243,30549,3413,29176,9880,7632,715,5665,28345,
	24789,15301,26717,30412,27018,26399,7460,7734,25423,26120,9980,2870,8693,10595,11850,4974,30762,32365,15208,3487,
	21048,3745,18167,28852,26864,5849,9335,25602,8416,6581,31509,9422,27085,4416,16579,25871,12893,25553,27534,1910,
	17427,8240,19756,23724,7040,17658,19669,17883,4280,5959,29525,24687,25604,19593,32094,7966,726,3000,26658,29733,
	6402,2476,11297,28885,16938,17610,6720,32335,28538,9186,3868,5004,21910,31832,28829,3232,30417,17282,10702,1855,
	7584,28498,6380,26752,23374,18997,9149,28641,24305,15930,26146,14492,23415,12016,25048,23766,7593,16124,1043,31819,
	13594,32373,32383,962,22216,7287,6554,22607,22467,19676,24738,9162,6103,6230,345,17791,32619,29371,22790,15143,
	5289,2975,21180,5656,27615,3368,31454,20664,30501,15646,27619,6199,31041,13378,30807,24190,8135,8161,13858,18530,
	2181,16808,20237,13853,8634,14978,5120,14324,9984,17555,31074,2379,2496,1599,11228,27564,21618,13040,22712,16520,
	26319,24391,26278,9758,12074,9500,6577,23454,3587,18625,13170,12431,6106,17287,25476,878,10561,22404,7224,5016,
	309,4243,8073,10858,21934,22171,22610,24486,5749,29973,21438,32024,21046,15304,28416,17786,29142,7383,3397,8854,
	15878,27646,27111,32426,25369,8985,23268,19728,26386,20454,6030,1566,17922,3851,6732,10385,14427,24032,8340,26842,
	8277,17586,17439,22908,4845,7342,13597,749,27536,13601,8437,19218,28258,8234,30110,15117,18122,18214,29772,22359,
	20272,9068,22352,15886,1541,28475,3897,32656,21508,25328,23360,25928,1976,21116,16611,6856,10815,9935,27928,4429,
	12589,5891,5198,21108,27699,15199,31650,24882,28266,14901,13547,16697,8948,13826,12168,22678,21371,7123,1568,29161,
	4208,10600,29255,18437,12973,30787,31557,2465,8280,536,17745,22794,15587,20156,20917,3376,951,681,1520,5078,
	5278,22108,22739,32389,14749,10621,17753,643,24374,31027,5357,22242,6921,5171,27844,6164,13191,5047,31615,2889,
	9720,22901,20304,11257,19906,22367,14268,4328,13897,5228,11362,13777,11327,15541,27712,17995,7128,22298,31944,9167,
	19254,4479,4957,12551,18927,22130,25701,13453,22721,26125,26783,22774,31105,30238,3254,28175,15189,7558,20326,5923,
	3807,4096,5132,28745,26210,22490,27546,3471,3847,6089,19570,7769,2177,22343,8226,27389,2907,5088,14373,25035,
	29096,27416,21853,23996,20037,554,11952,1646,1392,23339,29032,31060,3801,24280,13282,16801,7676,13202,138,1896,
	9391,1447,3335,14849,25591,9427,29106,14766,32544,6919,30029,17534,17677,8296,16626,10866,9164,4251,4169,3295,
	31341,17728,12798,11539,15637,31164,27762,3643,8014,7228,6833,28293,14497,12279,5541,22375,9395,15753,26061,30172,
	18314,14239,14321,1397,12526,13350,5256,1140,21483,20615,32673,4537,4780,27087,5988,29403,31809,29537,24569,16503,
	6047,9709,1701,11464,24552,6812,24630,6264,21997,9869,9890,27062,19186,2693,14827,22921,18609,19170,26693,11108,
	15009,17361,379,762,15479,19905,14735,12781,1560,7841,27168,3011,4132,9745,7756,24068,25172,5352,24787
	};

const TInt KUpdateRecIds[KUpdateRecCnt] = 
	{
	6638,22607,5776,31477,10815,6919,32116,31041,18609,7224,18167,12992,11751,22721,29619,21048,22170,30084,23408,26278,
	8437,9164,31509,552,16786,10544,29776,9604,26061,18314,23360,2884,11762,25134,31526,17826,29131,15753,12827,4035,5849,
	8073,25788,27789,7516,19905,18478,2465,5289,13601,4784,13453,7583,309,9725,16503,14321,30835,14849,29236,19539,27087,
	32365,29176,138,23642,9745,9880,28175,17555,12867,22921,17987,17318,15117,6554,17781,3569,23337,380,27085,24522,17583,
	27844,19676,21618,30137,9520,24272,1541,32718,6030,25604,28282,15333,20037,10858,31777,18048,16706
	};

const TInt KSelectRecIds[KSelectRecCnt] = 
	{
	5683,4367,1432,9725,15824,6030,16697,5379,26520,1447,21116,17728,2927,11362,30172,11228,32260,15930,25292,17753,12867,
	17534,12488,26164,3745,31164,30287,6230,16319,27909,6577,3471,5988,24272,2069,26270,21532,7769,23344,18987,6921,536,6164,
	30199,11108,3847,4597,28266,31509,31819,8510,12777,14449,31650,9996,7460,30412,24147,10908,19906,31876,24247,6732,5957,
	10815,17995,23337,7584,12383,26858,5665,11850,5749,5088,5486,643,13826,32094,15886,21910,4520,4630,7516,10110,24687,7558,
	22627,27171,14492,11175,18688,27546,27087,29772,20353,9520,15878,25046,15696,22921,
	};

const TInt KDeleteRecIds[KDeleteRecCnt] = 
	{
	20623,19095,26210,18048,8416,18167,13200,22944,1057,31337,17875,29973,1140,23374,24552,19931,13017,10385,10058,11360,
	30029,6581,22170,15824,2300,32389,25345,3494,17302,3807,19728,17832,11362,27909,30006,17361,28266,16319,22908,27564,
	13853,24026,17586,25269,646,15141,15753,24655,22644,9882,9725,26763,6968,29191,27087,26864,13620,23884,27789,8161,643,
	27062,1447,14506,29619,24153,8312,32544,31060,18530,13753,13826,8933,23408,20615,7013,18122,233,27712,536,3942,28852,
	22921,31876,9613,397,26717,18997,23835,15292,5891,30968,24685,30835,29537,15051,13128,31477,28416,8135
	};

const TInt KInsertRecId = 6184;
const TInt KUpdateRecId = 26658;
const TInt KSelectRecId = 9592;
const TInt KDeleteRecId = 24481;

const char KMultiInsertSql[] = "INSERT INTO Tbl VALUES(:Id, 9234567890, 99.998, 'TEXT VALUE', x'AABBCCDDEEFF')";
const char KMultiUpdateSql[] = "UPDATE Tbl SET D=11.113 WHERE I IN";
const char KMultiDeleteSql[] = "DELETE FROM Tbl WHERE I IN";
const char KMultiSelectSql[] = "SELECT I64,D,T,B FROM Tbl WHERE I IN";

const char KSingleInsertSql[] = "INSERT INTO Tbl VALUES(%d, 9234567890, 99.998, 'TEXT VALUE', x'AABBCCDDEEFF')";
const char KSingleUpdateSql[] = "UPDATE Tbl SET D=11.113 WHERE I=";
const char KSingleDeleteSql[] = "DELETE FROM Tbl WHERE I=";
const char KSingleSelectSql[] = "SELECT I64,D,T,B FROM Tbl WHERE I=";

//This buffer is used for SQL statement formatting. 2000 bytes should be enough as a buffer max length.
static char TheSqlBuf[2000];
//This buffer is used for printf related formatting. 500 characters should be enough.
static TBuf<500> ThePrintBuf;

#define UNUSED_VAR(a) (a) = (a)
#define UNUSED_PTR(a) a.Set(a)

///////////////////////////////////////////////////////////////////////////////////////

static void DeleteTestFiles()
	{
	(void)TheFs.Delete(KTestDbName);
	}

///////////////////////////////////////////////////////////////////////////////////////
static void PrintSqliteErrMsg()
	{
	if(TheDb)
		{
		const char* msg = sqlite3_errmsg(TheDb);
		TestPrintf("*** SQLite error message:");
		TestPrintf(msg);
		}
	}

//Test macros and functions
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteTestFiles();
		PrintSqliteErrMsg();
		TestTestLine(EFalse, aLine);
		}
	}
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DeleteTestFiles();
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		PrintSqliteErrMsg();
		TestTestLine(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

static TInt TheCounterFreq = -10000000;
const TInt KMicroSecIn1Sec = 1000000;

//Prints aFastCount parameter (converted to us)
static TInt FcDiff2Us(TUint32 aFastCount)
	{
	double v = ((double)aFastCount * KMicroSecIn1Sec) / (double)TheCounterFreq;
	TInt v2 = (TInt)v;
	return v2;
	}

static void GetFastCounterFrequency()
	{
	TEST2(HAL::Get(HAL::EFastCounterFrequency, TheCounterFreq), KErrNone);
	TBuf8<32> printString;
	printString.Format(_L8("Counter frequency=%d\r\n"), TheCounterFreq);
	TestPrintf((const char*)printString.PtrZ());
	}

TUint32 FastCounterValue(void)
	{
	return User::FastCounter();
	}

void StorePerfTestResult(TPerfTestType aType, TUint32 aStartTicks, TUint32 aEndTicks)
	{
	TInt64 diffTicks = (TInt64)aEndTicks - (TInt64)aStartTicks;
	if(diffTicks < 0)
		{
		diffTicks = KMaxTUint32 + diffTicks + 1;
		}
	ThePerfTestResult[aType] = (TUint32)diffTicks;
	}

static void PrintPerfTestResults()
	{
	TBuf8<256> printString;

	TInt r = FcDiff2Us(ThePerfTestResult[EPerfTestMultiInsert]);
	printString.Format(_L8("###Mutli Insert:  %8dus\r\n"), r);
	TestPrintf((const char*)printString.PtrZ());

	r = FcDiff2Us(ThePerfTestResult[EPerfTestMultiUpdate]);
	printString.Format(_L8("###Mutli Update:  %8dus\r\n"), r);
	TestPrintf((const char*)printString.PtrZ());

	r = FcDiff2Us(ThePerfTestResult[EPerfTestMultiSelect]);
	printString.Format(_L8("###Mutli Select:  %8dus\r\n"), r);
	TestPrintf((const char*)printString.PtrZ());

	r = FcDiff2Us(ThePerfTestResult[EPerfTestMultiDelete]);
	printString.Format(_L8("###Mutli Delete:  %8dus\r\n"), r);
	TestPrintf((const char*)printString.PtrZ());


	r = FcDiff2Us(ThePerfTestResult[EPerfTestSingleInsert]);
	printString.Format(_L8("##Single Insert:  %8dus\r\n"), r);
	TestPrintf((const char*)printString.PtrZ());

	r = FcDiff2Us(ThePerfTestResult[EPerfTestSingleUpdate]);
	printString.Format(_L8("##Single Update:  %8dus\r\n"), r);
	TestPrintf((const char*)printString.PtrZ());

	r = FcDiff2Us(ThePerfTestResult[EPerfTestSingleSelect]);
	printString.Format(_L8("##Single Select:  %8dus\r\n"), r);
	TestPrintf((const char*)printString.PtrZ());

	r = FcDiff2Us(ThePerfTestResult[EPerfTestSingleDelete]);
	printString.Format(_L8("##Single Delete:  %8dus\r\n"), r);
	TestPrintf((const char*)printString.PtrZ());
	}

///////////////////////////////////////////////////////////////////////////////////////

static void CreateTestDir()
    {
	TInt err = TheFs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

static void CreateTestDatabase()
	{
	(void)TheFs.Delete(KTestDbName);

	const char* ver = sqlite3_libversion();
	TestPrintf("*** SQLite library version:");
	TestPrintf(ver);

	TInt err = sqlite3_open(KTestDbName8, &TheDb);
	TEST2(err, SQLITE_OK);

	err = sqlite3_exec(TheDb, "CREATE TABLE Tbl(I INTEGER PRIMARY KEY, I64 BIGINT, D DOUBLE, T TEXT, B BINARY)", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	sqlite3_close(TheDb);
	TheDb = 0;
	}

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			PDS-SQLITE3-UT-4030
@SYMTestCaseDesc		SQLite library multi-insert performance test.
						The test inserts 1000 records in a single transaction and stores 
						the execution time for later use (comparison and printing).
						The main purpose of this test is - regression performance testing.
@SYMTestPriority		High
@SYMTestActions			SQLite library multi-insert performance test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10424
*/
static void SqliteMultiInsertTest()
	{
	const char* tail = 0;
	sqlite3_stmt* stmt = 0;

	TEST(!TheDb);
	TInt err = sqlite3_open(KTestDbName8, &TheDb);
	TEST2(err, SQLITE_OK);

	err = sqlite3_prepare(TheDb, KMultiInsertSql, -1, &stmt, &tail);
	TEST2(err, SQLITE_OK);
	
	TUint32 fc = FastCounterValue();
	err = sqlite3_exec(TheDb, "BEGIN", 0, 0, 0);
	TEST2(err, SQLITE_OK);

	for(TInt i=0;i<KInsertRecCnt;++i)
		{
		err = sqlite3_bind_int(stmt, 1, KInsertRecIds[i]);
		TEST2(err, SQLITE_OK);

		err = sqlite3_step(stmt);
		TEST2(err, SQLITE_DONE);
		
		err = sqlite3_reset(stmt);		
		TEST2(err, SQLITE_OK);
		}
		
	err = sqlite3_exec(TheDb, "COMMIT", 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	StorePerfTestResult(EPerfTestMultiInsert, fc, FastCounterValue());

	sqlite3_finalize(stmt);	
	sqlite3_close(TheDb);
	TheDb = 0;
	}

static void FormatSqlStmt(const char aSql[], const TInt aRecIds[], const TInt aRecCnt)
	{
	strcpy(TheSqlBuf, aSql);
	strcat(TheSqlBuf, "(");
	for(TInt i=0;i<aRecCnt;++i)
		{
		char tmp[10];
		sprintf(tmp, "%d,", aRecIds[i]);
		strcat(TheSqlBuf, tmp);
		}
	TheSqlBuf[strlen(TheSqlBuf) - 1] = ')';
	}

/**
@SYMTestCaseID			PDS-SQLITE3-UT-4031
@SYMTestCaseDesc		SQLite library multi-update performance test.
						The test updates 100 records and stores 
						the execution time for later use (comparison and printing).
						The main purpose of this test is - regression performance testing.
@SYMTestPriority		High
@SYMTestActions			SQLite library multi-update performance test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10424
*/
static void SqliteMultiUpdateTest()
	{
	TEST(!TheDb);
	TInt err = sqlite3_open(KTestDbName8, &TheDb);
	TEST2(err, SQLITE_OK);

	FormatSqlStmt(KMultiUpdateSql, KUpdateRecIds, KUpdateRecCnt);

	TUint32 fc = FastCounterValue();
	
	err = sqlite3_exec(TheDb, TheSqlBuf, 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	StorePerfTestResult(EPerfTestMultiUpdate, fc, FastCounterValue());
	
	TInt cnt = sqlite3_changes(TheDb);
	TEST2(cnt, KUpdateRecCnt);
	
	sqlite3_close(TheDb);
	TheDb = 0;
	}

/**
@SYMTestCaseID			PDS-SQLITE3-UT-4032
@SYMTestCaseDesc		SQLite library multi-select performance test.
						The test selects 100 records and stores 
						the execution time for later use (comparison and printing).
						The main purpose of this test is - regression performance testing.
@SYMTestPriority		High
@SYMTestActions			SQLite library multi-select performance test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10424
*/
static void SqliteMultiSelectTest()
	{
	const char* tail = 0;
	sqlite3_stmt* stmt = 0;
	
	TEST(!TheDb);
	TInt err = sqlite3_open(KTestDbName8, &TheDb);
	TEST2(err, SQLITE_OK);

	FormatSqlStmt(KMultiSelectSql, KSelectRecIds, KSelectRecCnt);

	err = sqlite3_prepare(TheDb, TheSqlBuf, -1, &stmt, &tail);
	TEST2(err, SQLITE_OK);
	
	TUint32 fc = FastCounterValue();
	
	TInt recCnt = 0;
	while((err = sqlite3_step(stmt)) == SQLITE_ROW)
		{
		__int64 i64;
		double d;
		const unsigned short* t;
		const unsigned char* b;
		
		i64 = sqlite3_column_int64(stmt, 0);
		UNUSED_VAR(i64);
		d = sqlite3_column_double(stmt, 1);
		UNUSED_VAR(d);
		t = (const unsigned short*)sqlite3_column_text16(stmt, 2);
		UNUSED_VAR(t);
		b = (const unsigned char*)sqlite3_column_blob(stmt, 3);
		UNUSED_VAR(b);
		++recCnt;
		}
	TEST2(err, SQLITE_DONE);
	TEST2(recCnt, KSelectRecCnt);
		
	StorePerfTestResult(EPerfTestMultiSelect, fc, FastCounterValue());
	
	sqlite3_finalize(stmt);	
	sqlite3_close(TheDb);
	TheDb = 0;
	}

/**
@SYMTestCaseID			PDS-SQLITE3-UT-4033
@SYMTestCaseDesc		SQLite library multi-delete performance test.
						The test deletes 100 records and stores 
						the execution time for later use (comparison and printing).
						The main purpose of this test is - regression performance testing.
@SYMTestPriority		High
@SYMTestActions			SQLite library multi-delete performance test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10424
*/
static void SqliteMultiDeleteTest()
	{
	TEST(!TheDb);
	TInt err = sqlite3_open(KTestDbName8, &TheDb);
	TEST2(err, SQLITE_OK);

	FormatSqlStmt(KMultiDeleteSql, KDeleteRecIds, KDeleteRecCnt);

	TUint32 fc = FastCounterValue();
	
	err = sqlite3_exec(TheDb, TheSqlBuf, 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	StorePerfTestResult(EPerfTestMultiDelete, fc, FastCounterValue());

	TInt cnt = sqlite3_changes(TheDb);
	TEST2(cnt, KDeleteRecCnt);
	
	sqlite3_close(TheDb);
	TheDb = 0;
	}
	
/**
@SYMTestCaseID			PDS-SQLITE3-UT-4034
@SYMTestCaseDesc		SQLite library single-insert performance test.
						The test inserts one record and stores 
						the execution time for later use (comparison and printing).
						The main purpose of this test is - regression performance testing.
@SYMTestPriority		High
@SYMTestActions			SQLite library single-insert performance test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10424
*/
static void SqliteSingleInsertTest()
	{
	TEST(!TheDb);
	TInt err = sqlite3_open(KTestDbName8, &TheDb);
	TEST2(err, SQLITE_OK);

	sprintf(TheSqlBuf, KSingleInsertSql, KInsertRecId);
	
	TUint32 fc = FastCounterValue();
	
	err = sqlite3_exec(TheDb, TheSqlBuf, 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	StorePerfTestResult(EPerfTestSingleInsert, fc, FastCounterValue());

	TInt cnt = sqlite3_changes(TheDb);
	TEST2(cnt, 1);
	
	sqlite3_close(TheDb);
	TheDb = 0;
	}	

/**
@SYMTestCaseID			PDS-SQLITE3-UT-4035
@SYMTestCaseDesc		SQLite library single-update performance test.
						The test updates one record and stores 
						the execution time for later use (comparison and printing).
						The main purpose of this test is - regression performance testing.
@SYMTestPriority		High
@SYMTestActions			SQLite library single-update performance test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10424
*/
static void SqliteSingleUpdateTest()
	{	
	TEST(!TheDb);
	TInt err = sqlite3_open(KTestDbName8, &TheDb);
	TEST2(err, SQLITE_OK);

	char tmp[10];
	sprintf(tmp, "%d", KUpdateRecId);
	strcpy(TheSqlBuf, KSingleUpdateSql);
	strcat(TheSqlBuf, tmp);
	
	TUint32 fc = FastCounterValue();
	
	err = sqlite3_exec(TheDb, TheSqlBuf, 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	StorePerfTestResult(EPerfTestSingleUpdate, fc, FastCounterValue());
	
	TInt cnt = sqlite3_changes(TheDb);
	TEST2(cnt, 1);
	
	sqlite3_close(TheDb);
	TheDb = 0;
	}

/**
@SYMTestCaseID			PDS-SQLITE3-UT-4036
@SYMTestCaseDesc		SQLite library single-select performance test.
						The test selects one record and stores 
						the execution time for later use (comparison and printing).
						The main purpose of this test is - regression performance testing.
@SYMTestPriority		High
@SYMTestActions			SQLite library single-select performance test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10424
*/
static void SqliteSingleSelectTest()
	{
	const char* tail = 0;
	sqlite3_stmt* stmt = 0;
	
	TEST(!TheDb);
	TInt err = sqlite3_open(KTestDbName8, &TheDb);
	TEST2(err, SQLITE_OK);

	char tmp[10];
	sprintf(tmp, "%d", KSelectRecId);
	strcpy(TheSqlBuf, KSingleSelectSql);
	strcat(TheSqlBuf, tmp);
	
	err = sqlite3_prepare(TheDb, TheSqlBuf, -1, &stmt, &tail);
	TEST2(err, SQLITE_OK);
	
	TUint32 fc = FastCounterValue();
	
	TInt recCnt = 0;
	while((err = sqlite3_step(stmt)) == SQLITE_ROW)
		{
		__int64 i64;
		double d;
		const unsigned short* t;
		const unsigned char* b;
		
		i64 = sqlite3_column_int64(stmt, 0);
		UNUSED_VAR(i64);
		d = sqlite3_column_double(stmt, 1);
		UNUSED_VAR(d);
		t = (const unsigned short*)sqlite3_column_text16(stmt, 2);
		UNUSED_VAR(t);
		b = (const unsigned char*)sqlite3_column_blob(stmt, 3);
		UNUSED_VAR(b);
		++recCnt;
		}
	TEST2(err, SQLITE_DONE);
	TEST2(recCnt, 1);

	StorePerfTestResult(EPerfTestSingleSelect, fc, FastCounterValue());

	sqlite3_finalize(stmt);	
	sqlite3_close(TheDb);
	TheDb = 0;
	}

/**
@SYMTestCaseID			PDS-SQLITE3-UT-4037
@SYMTestCaseDesc		SQLite library single-delete performance test.
						The test deletes one record and stores 
						the execution time for later use (comparison and printing).
						The main purpose of this test is - regression performance testing.
@SYMTestPriority		High
@SYMTestActions			SQLite library single-delete performance test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10424
*/
void SqliteSingleDeleteTest()
	{
	TEST(!TheDb);
	TInt err = sqlite3_open(KTestDbName8, &TheDb);
	TEST2(err, SQLITE_OK);

	char tmp[10];
	sprintf(tmp, "%d", KDeleteRecId);
	strcpy(TheSqlBuf, KSingleDeleteSql);
	strcat(TheSqlBuf, tmp);
	
	TUint32 fc = FastCounterValue();
	
	err = sqlite3_exec(TheDb, TheSqlBuf, 0, 0, 0);
	TEST2(err, SQLITE_OK);
	
	StorePerfTestResult(EPerfTestSingleDelete, fc, FastCounterValue());
	
	TInt cnt = sqlite3_changes(TheDb);
	TEST2(cnt, 1);
	
	sqlite3_close(TheDb);
	TheDb = 0;
	}

///////////////////////////////////////////////////////////////////////////////////////

static void DoTests()
	{
	TestStart("Get fast counter frequency");
	GetFastCounterFrequency();

	TestNext("SQLite: Create the test database");
	CreateTestDatabase();

	TBuf8<100> msgbuf;

	msgbuf.Format(_L8("@SYMTestCaseID:PDS-SQLITE3-UT-4030: Insert %d records in a single transaction"), KInsertRecCnt);
	TestNext((const char*)msgbuf.PtrZ());
	SqliteMultiInsertTest();

	msgbuf.Format(_L8("@SYMTestCaseID:PDS-SQLITE3-UT-4031: Update %d records in a single transaction"), KUpdateRecCnt);
	TestNext((const char*)msgbuf.PtrZ());
	SqliteMultiUpdateTest();

	msgbuf.Format(_L8("@SYMTestCaseID:PDS-SQLITE3-UT-4032: Select %d records"), KSelectRecCnt);
	TestNext((const char*)msgbuf.PtrZ());
	SqliteMultiSelectTest();

	msgbuf.Format(_L8("@SYMTestCaseID:PDS-SQLITE3-UT-4033: Delete %d records in a single transaction"), KDeleteRecCnt);
	TestNext((const char*)msgbuf.PtrZ());
	SqliteMultiDeleteTest();

	msgbuf.Format(_L8("@SYMTestCaseID:PDS-SQLITE3-UT-4034: Insert a single record"));
	TestNext((const char*)msgbuf.PtrZ());
	SqliteSingleInsertTest();

	msgbuf.Format(_L8("@SYMTestCaseID:PDS-SQLITE3-UT-4035: Update a single record"));
	TestNext((const char*)msgbuf.PtrZ());
	SqliteSingleUpdateTest();

	msgbuf.Format(_L8("@SYMTestCaseID:PDS-SQLITE3-UT-4036: Select a single record"));
	TestNext((const char*)msgbuf.PtrZ());
	SqliteSingleSelectTest();

	msgbuf.Format(_L8("@SYMTestCaseID:PDS-SQLITE3-UT-4037: Delete a single record"));
	TestNext((const char*)msgbuf.PtrZ());
	SqliteSingleDeleteTest();

	(void)TheFs.Delete(KTestDbName);

	PrintPerfTestResults();
	}

TInt E32Main()
	{
	TestOpen(KTestName);
	TestTitle();

	CTrapCleanup* tc = CTrapCleanup::New();

	__UHEAP_MARK;

	TInt err = TheFs.Connect();
	TestTest(err == KErrNone);

	CreateTestDir();
	DeleteTestFiles();
	DoTests();
	DeleteTestFiles();

	__UHEAP_MARKEND;

	TheFs.Close();
	TestEnd();
	TestClose();

	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
