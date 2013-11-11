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

#include <d32dbms.h>
#include <s32file.h>
#include <e32test.h>
#include <e32math.h>
#include <s32mem.h>
#include <hal.h>

static RTest TheTest(_L("t_dbperf3"));
static CTrapCleanup* 	TheTrapCleanup;
static RFs				TheFs;
static RDbs 			TheDbs;
static RDbNamedDatabase TheDatabase;
static RDbRowSet 		TheRowSet, TheRowSet2;
static RFile 			TheTestFile;
static TFileName 		TheDatabaseFileName;

#define COUNT_OF(array) (sizeof(array)/sizeof(array[0]))

const TInt KTrackRecordCount = 200;
const TInt KCategoryRecordCount = 50;
const TInt KStatsRecordCount = 50;
const TInt KTestBlobSize = 4096;

//_LIT(KCreateTrackTable, "CREATE TABLE TRACKS (id INTEGER, marked_2_play INTEGER, category_id INTEGER, artist_last_name CHAR(15) NOT NULL,artist_first_name CHAR(15) NOT NULL, title CHAR(16) NOT NULL,download_site CHAR(30) NOT NULL,band_name CHAR(20) NOT NULL,origin CHAR(16),autostart INTEGER, init_volume INTEGER, music_file LONG VARCHAR)");
_LIT(KCreateTrackIndex, "CREATE INDEX IDX1 ON TRACKS(id,marked_2_play,category_id)");
_LIT(KCreateTrackTable, "CREATE TABLE TRACKS (id INTEGER, marked_2_play INTEGER, category_id INTEGER, artist_last_name CHAR(15) NOT NULL,artist_first_name CHAR(15) NOT NULL, title CHAR(16) NOT NULL,download_site CHAR(30) NOT NULL,band_name CHAR(20) NOT NULL,origin CHAR(16),autostart INTEGER, init_volume INTEGER)");
_LIT(KCreateTrackTable2, "CREATE TABLE TRACKS2 (id INTEGER, music_file LONG VARCHAR)");
//_LIT(KCreateTrackIndex2, "CREATE INDEX IDX4 ON TRACKS2(id)");

//_LIT(KTrackTable,"TRACKS");

_LIT(KId,"id");
_LIT(KLastName,"artist_last_name");
_LIT(KFirstName,"artist_first_name");
_LIT(KTitle,"title");
_LIT(KDownloadSite,"download_site");
_LIT(KBandName,"band_name");
_LIT(KOrigin,"origin");
_LIT(KAutoStart,"autostart");
_LIT(KInitVolume,"init_volume");
_LIT(KMarked2Play,"marked_2_play");
_LIT(KCategoryId,"category_id");
_LIT(KMusicFile,"music_file");


//category Table LITS
_LIT(KCreateCategoryTable, "CREATE TABLE CATEGORY (category_id INTEGER,category_name CHAR(20),genre INTEGER)");
_LIT(KCreateCategoryIndex, "CREATE INDEX IDX2 ON CATEGORY(category_id)");

//_LIT(KCategoryTable,"CATEGORY");

//KCategoryId defined for category table	
_LIT(KCategoryName,"category_name");
_LIT(KGenre,"genre");


//STATS Table LITS
_LIT(KCreatestatsTable, "CREATE TABLE STATS (category_id INTEGER, no_of_tracks INTEGER, no_autostart INTEGER,no_manualstart INTEGER,no_marked_2_play INTEGER, no_unmarked_2_play INTEGER, size_of_musicfiles INTEGER)");
_LIT(KCreatestatsIndex, "CREATE UNIQUE INDEX IDX3 ON STATS(category_id)");

//_LIT(KStatsTable,"STATS");

//KCategoryId defined for category table
_LIT(KNoOfTracks,"no_of_tracks");
_LIT(KNoMarked2Play,"no_marked_2_play");
_LIT(KNoUnmarked2Play,"no_unmarked_2_play");
_LIT(KNoAutostart,"no_autostart");
_LIT(KNoManualStart,"no_manualstart");
_LIT(KSizeOfMusicFiles,"size_of_musicfiles");

//////////////////////////////////////////////////////

static TInt TheCounterFreq = -10000000;
const TInt KMicroSecIn1Sec = 1000000;

TUint32 CalcTickDiff(TUint32 aStartTicks, TUint32 aEndTicks)
	{
	TInt64 diffTicks = (TInt64)aEndTicks - (TInt64)aStartTicks;
	if(diffTicks < 0)
		{
		diffTicks = KMaxTUint32 + diffTicks + 1;
		}
	return (TUint32)diffTicks;
	}

//Prints aFastCount parameter (converted to us)
void PrintFcDiffAsUs(const TDesC& aFormatStr, TUint32 aFastCount)
	{
	double v = ((double)aFastCount * KMicroSecIn1Sec) / (double)TheCounterFreq;
	TInt v2 = (TInt)v;
	TheTest.Printf(aFormatStr, v2);
	}

///////////////////////////////////////////////////////////////////////////////////////

//Delete "aFullName" file.
static void DeleteFile(const TDesC& aFullName)
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		TEntry entry;
		if(fsSession.Entry(aFullName, entry) == KErrNone)
			{
			err = fsSession.SetAtt(aFullName, 0, KEntryAttReadOnly);
			if(err != KErrNone) 
				{
				TheTest.Printf(_L("Error %d changing \"%S\" file attributes.\n"), err, &aFullName);
				}
			err = fsSession.Delete(aFullName);
			if(err != KErrNone) 
				{
				TheTest.Printf(_L("Error %d deleting \"%S\" file.\n"), err, &aFullName);
				}
			}
		fsSession.Close();
		}
	else
		{
		TheTest.Printf(_L("Error %d connecting file session. File: %S.\n"), err, &aFullName);
		}
	}

///////////////////////////////////////////////////////////////////////////////////////

static void CloseAll()
	{
	TheRowSet2.Close();
	TheRowSet.Close();
	TheDatabase.Close();
	TheDbs.Close();
	TheFs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Tests macros and functions.
//If (!aValue) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		CloseAll();
		DeleteFile(TheDatabaseFileName);
		TheTest(EFalse, aLine);
		}
	}
//If (aValue != aExpected) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		TheTest.Printf(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		CloseAll();
		DeleteFile(TheDatabaseFileName);
		TheTest(EFalse, aLine);
		}
	}
//Use these to test conditions.
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

void PrintFileSize()
	{
	RDbDatabase::TSize s = TheDatabase.Size();
	TheTest.Printf(_L("####FileSize: %d\r\n"), s.iSize);
	}

///////////////////////////////////////////////////////////////////////////////////////

void FillRandomData(TDes& aData, TInt64 aSeed)
	{
	aData.Zero();
	for (TInt i=0; i<aData.MaxLength(); ++i)
		{
		// add next character (we stick to lowercase alphabet for now)
		aData.Append(TChar(Math::FRand(aSeed)*25 + 'a'));	
		}
	}

void FillRandomData(TDes& aData)
	{
	// get random seed
	TTime time;
	time.UniversalTime();
	TInt64 seed = time.Int64();
	// do the filling
	FillRandomData(aData, seed);
	}

///////////////////////////////////////////////////////////////////////////////////////

void CreateDatabase()
	{
	TInt err = TheDatabase.Replace(TheFs, TheDatabaseFileName);
	TEST2(err, KErrNone);
	TheDatabase.Close();
	err = TheDbs.Connect();
	TEST2(err, KErrNone);
	err = TheDatabase.Open(TheDbs, TheDatabaseFileName);
	TEST2(err, KErrNone);
	err = TheDatabase.Execute(KCreateTrackTable);
	TEST2(err, KErrNone);
	err = TheDatabase.Execute(KCreateTrackIndex);
	TEST2(err, KErrNone);
	err = TheDatabase.Execute(KCreateTrackTable2);
	TEST2(err, KErrNone);
	//err = TheDatabase.Execute(KCreateTrackIndex2);
	//TEST2(err, KErrNone);
	err = TheDatabase.Execute(KCreateCategoryTable);
	TEST2(err, KErrNone);
	err = TheDatabase.Execute(KCreateCategoryIndex);
	TEST2(err, KErrNone);
	err = TheDatabase.Execute(KCreatestatsTable);
	TEST2(err, KErrNone);
	err = TheDatabase.Execute(KCreatestatsIndex);
	TEST2(err, KErrNone);
	//err = TheDatabase.Compact();
	//TEST2(err, KErrNone);
	}

void InsertTrackTableL()
	{	
	HBufC* randomDataBuf = HBufC::NewLC(KTestBlobSize);
	TPtr randomData(randomDataBuf->Des());
	FillRandomData(randomData);
	
	RDbView view;
	TInt err = view.Prepare(TheDatabase, _L("select * from TRACKS"), view.EInsertOnly);
	TEST2(err, KErrNone);
	TheRowSet = view;	
	
	CDbColSet* colSet = TheRowSet.ColSetL();
	const TInt KIdIdx = colSet->ColNo(KId);
	const TInt KLastNameIdx = colSet->ColNo(KLastName);
	const TInt KFirstNameIdx = colSet->ColNo(KFirstName);
	const TInt KTitleIdx = colSet->ColNo(KTitle);
	const TInt KDownloadSiteIdx = colSet->ColNo(KDownloadSite);
	const TInt KBandNameIdx = colSet->ColNo(KBandName);
	const TInt KOriginIdx = colSet->ColNo(KOrigin);
	const TInt KAutoStartIdx = colSet->ColNo(KAutoStart);
	const TInt KInitVolumeIdx = colSet->ColNo(KInitVolume);
	const TInt KMarkedToPlayIdx = colSet->ColNo(KMarked2Play);
	const TInt KCategoryIdIdx = colSet->ColNo(KCategoryId);
	//const TInt KMusicFileIdx = colSet->ColNo(KMusicFile);
	delete colSet;
	colSet = NULL;

	err = TheDatabase.Begin();
	TEST2(err, KErrNone);
	
	for (TInt ii=1;ii<=KTrackRecordCount;++ii)
		{
		TheRowSet.InsertL();
		TheRowSet.SetColL(KIdIdx, ii);
		TheRowSet.SetColL(KLastNameIdx, _L("Dummy"));
		TheRowSet.SetColL(KFirstNameIdx,_L("Dummy"));
		TheRowSet.SetColL(KTitleIdx,_L("Dummy"));
		TheRowSet.SetColL(KDownloadSiteIdx,_L("Dummy"));
		TheRowSet.SetColL(KBandNameIdx,_L("Dummy"));
		TheRowSet.SetColL(KOriginIdx,_L("Dummy"));
		TheRowSet.SetColL(KAutoStartIdx,(ii%2));
		TheRowSet.SetColL(KInitVolumeIdx,(ii%2));
		TheRowSet.SetColL(KMarkedToPlayIdx,(ii%2));
		TheRowSet.SetColL(KCategoryIdIdx,(ii%KCategoryRecordCount));

		//RDbColWriteStream musicfile;
		//musicfile.OpenLC(TheRowSet, KMusicFileIdx);
		//musicfile.WriteL(randomData,KTestBlobSize);
		//musicfile.CommitL();
		//CleanupStack::PopAndDestroy(&musicfile);

		TheRowSet.PutL();
		}

	err = TheDatabase.Commit();
	TEST2(err, KErrNone);

	//err = TheDatabase.Compact();
	//TEST2(err, KErrNone);

	TheRowSet.Close();	

	////////////////////////////////////////////////////////////////////////////////////////////////

	err = view.Prepare(TheDatabase, _L("select * from TRACKS2"), view.EInsertOnly);
	TEST2(err, KErrNone);
	TheRowSet = view;	
	
	colSet = TheRowSet.ColSetL();
	const TInt KIdIdx2 = colSet->ColNo(KId);
	const TInt KMusicFileIdx2 = colSet->ColNo(KMusicFile);
	delete colSet;

	err = TheDatabase.Begin();
	TEST2(err, KErrNone);

	for (TInt ii=1;ii<=KTrackRecordCount;++ii)
		{
		TheRowSet.InsertL();
		TheRowSet.SetColL(KIdIdx2, ii);
		
		RDbColWriteStream musicfile;
		musicfile.OpenLC(TheRowSet, KMusicFileIdx2);
		musicfile.WriteL(randomData,KTestBlobSize);
		musicfile.CommitL();
		CleanupStack::PopAndDestroy(&musicfile);
		
		TheRowSet.PutL();
		}

	err = TheDatabase.Commit();
	TEST2(err, KErrNone);

	//err = TheDatabase.Compact();
	//TEST2(err, KErrNone);

	TheRowSet.Close();	

	CleanupStack::PopAndDestroy(randomDataBuf);
	}
	
void InsertCategoryTableL()
	{
	RDbView view;
	TInt err = view.Prepare(TheDatabase, _L("select * from CATEGORY"), view.EInsertOnly);
	TEST2(err, KErrNone);
	TheRowSet = view;	

	CDbColSet* colSet = TheRowSet.ColSetL();
	const TInt KCategoryIdIdx = colSet->ColNo(KCategoryId);
	const TInt KCategoryNameIdx = colSet->ColNo(KCategoryName);
	const TInt KGenreIdx = colSet->ColNo(KGenre);
	delete colSet;

	err = TheDatabase.Begin();
	TEST2(err, KErrNone);

	for (TInt ii=1;ii<=KCategoryRecordCount;++ii)
		{
		TheRowSet.InsertL();
		TheRowSet.SetColL(KCategoryIdIdx, ii);
		TheRowSet.SetColL(KCategoryNameIdx, _L("History"));
		TheRowSet.SetColL(KGenreIdx,(ii*500));
		TheRowSet.PutL();
		}

	err = TheDatabase.Commit();
	TEST2(err, KErrNone);
	
	//err = TheDatabase.Compact();
	//TEST2(err, KErrNone);

	TheRowSet.Close();	
	}
	
void InsertStatsTableL()
	{
	RDbView view;
	TInt err = view.Prepare(TheDatabase, _L("select * from STATS"), view.EInsertOnly);
	TEST2(err, KErrNone);
	TheRowSet = view;	

	CDbColSet* colSet = TheRowSet.ColSetL();
	const TInt KCategoryIdIdx = colSet->ColNo(KCategoryId);
	const TInt KTrackCntIdx = colSet->ColNo(KNoOfTracks);
	const TInt KMarkedToPlayCntIdx = colSet->ColNo(KNoMarked2Play);
	const TInt KUnmarkedToPlayCntIdx = colSet->ColNo(KNoUnmarked2Play);
	const TInt KAutoStartCntIdx = colSet->ColNo(KNoAutostart);
	const TInt KManualStartCntIdx = colSet->ColNo(KNoManualStart);
	const TInt KSizeMusicFilesIdx = colSet->ColNo(KSizeOfMusicFiles);
	delete colSet;

	TInt default_Stat = 0;

	err = TheDatabase.Begin();
	TEST2(err, KErrNone);
	
	for (TInt ii=0;ii<KStatsRecordCount;++ii)
		{
		TheRowSet.InsertL();
		TheRowSet.SetColL(KCategoryIdIdx, ii);
		TheRowSet.SetColL(KTrackCntIdx,default_Stat);
		TheRowSet.SetColL(KMarkedToPlayCntIdx,default_Stat);
		TheRowSet.SetColL(KUnmarkedToPlayCntIdx,default_Stat);
		TheRowSet.SetColL(KAutoStartCntIdx,default_Stat);
		TheRowSet.SetColL(KManualStartCntIdx,default_Stat);
		TheRowSet.SetColL(KSizeMusicFilesIdx,default_Stat);
		TheRowSet.PutL();
		}

	err = TheDatabase.Commit();
	TEST2(err, KErrNone);
	
	//err = TheDatabase.Compact();
	//TEST2(err, KErrNone);

	TheRowSet.Close();	
	}
	
void FillDatabaseL()
	{
	InsertTrackTableL();
	InsertCategoryTableL();	
	InsertStatsTableL();
	}

void DestroyDatabase()
	{
	TheRowSet.Close();
	TheDatabase.Close();
	TheDbs.Close();
	TInt err = TheFs.Delete(TheDatabaseFileName);
	TEST2(err, KErrNone);
	}

///////////////////////////////////////////////////////////////////////////////////////

void GetFastCounterFrequency()
	{
	TEST2(HAL::Get(HAL::EFastCounterFrequency, TheCounterFreq), KErrNone);
	TheTest.Printf(_L("Counter frequency=%d\r\n"), TheCounterFreq);
	}

///////////////////////////////////////////////////////////////////////////////////////

void SelectTracksL(TInt aCount, RArray<TInt>& aTracks)
	{
	TUint32 fc = User::FastCounter();
	RDbView view;
	TUint32 fc2 = User::FastCounter();
	TInt err = view.Prepare(TheDatabase, _L("select id from TRACKS"), view.EReadOnly);
	TEST2(err, KErrNone);
	PrintFcDiffAsUs(_L("###\"Prepare()\",time=%d us\r\n"), CalcTickDiff(fc2, User::FastCounter()));
	fc2 = User::FastCounter();
	err = view.EvaluateAll();
	TEST2(err, KErrNone);
	PrintFcDiffAsUs(_L("###\"EvaluateAll()\",time=%d us\r\n"), CalcTickDiff(fc2, User::FastCounter()));
	TheRowSet = view;

	TUint32 diff1 = 0, diff2 = 0;
	TInt count = 0;	
	while(count < aCount)
		{
		fc2 = User::FastCounter();
		if(!TheRowSet.NextL())
			{
			break;	
			}
		diff1 += CalcTickDiff(fc2, User::FastCounter());	
		fc2 = User::FastCounter();
		TheRowSet.GetL();
		diff2 += CalcTickDiff(fc2, User::FastCounter());	
		aTracks.Append(TheRowSet.ColInt(1));
		++count;
		}
	TEST2(count, aCount);
	PrintFcDiffAsUs(_L("###\"Total NextL()\",time=%d us\r\n"), diff1);
	PrintFcDiffAsUs(_L("###\"Total GetL()\",time=%d us\r\n"), diff2);
	
	fc2 = User::FastCounter();
	TheRowSet.Close();	
	PrintFcDiffAsUs(_L("###\"Close()\",time=%d us\r\n"), CalcTickDiff(fc2, User::FastCounter()));
	PrintFcDiffAsUs(_L("###\"SELECT FROM TRACKS\",time=%d us\r\n"), CalcTickDiff(fc, User::FastCounter()));
	}

/** 
@SYMTestCaseID          PDS-DBMS-UT-4009
@SYMTestCaseDesc        DBMS performance tests.
@SYMTestPriority        High
@SYMTestActions        	The test opens the test database and:
						- selects the ids of the tracks to be deleted and collects them into an array;
						- deletes the recods with matching track ids from TRACK table;
						- deletes the recods with matching track ids from TRACK2 table;
						The execution times are printed out.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ7141
*/
void DeleteTracksL()
	{
	TheTest.Printf(_L("Record count: %d\r\n"), KTrackRecordCount);
	
	RArray<TInt> tracks;
	tracks.ReserveL(KTrackRecordCount);
	CleanupClosePushL(tracks);
	SelectTracksL(KTrackRecordCount, tracks);
	//
	_LIT(KDeleteSql, "DELETE FROM tracks  WHERE id>=%d AND id<=%d");

	TBuf<100> sql;
	sql.Format(KDeleteSql, tracks[0], tracks[tracks.Count() - 1]);

	TUint32 fc2 = User::FastCounter();

	TInt err = TheDatabase.Begin();
	TEST2(err, KErrNone);
	
	TUint32 fc = User::FastCounter();
	TInt rc = TheDatabase.Execute(sql);
	PrintFcDiffAsUs(_L("###\"DELETE FROM TRACKS\",time=%d us\r\n"), CalcTickDiff(fc, User::FastCounter()));
	TEST2(rc, KTrackRecordCount);
	TheTest.Printf(_L("Deleted record count: %d\r\n"), rc);

	sql.Replace(12, 6, _L("TRACKS2"));
	fc = User::FastCounter();
	rc = TheDatabase.Execute(sql);
	PrintFcDiffAsUs(_L("###\"DELETE FROM TRACKS2\",time=%d us\r\n"), CalcTickDiff(fc, User::FastCounter()));
	TEST2(rc, KTrackRecordCount);
	TheTest.Printf(_L("Deleted record count: %d\r\n"), rc);

	err = TheDatabase.Commit();
	TEST2(err, KErrNone);

	PrintFcDiffAsUs(_L("###Total \"DELETE FROM TRACKS\",time=%d us\r\n"), CalcTickDiff(fc2, User::FastCounter()));
	
	CleanupStack::PopAndDestroy(&tracks);
	}

///////////////////////////////////////////////////////////////////////////////////////

//This test checks how RDbDatabase::Commit() works if there are some active RDbRowSet objects - 
//read-only and updatable. The expectation is that the Commit() call won't fail, the RDbRowSet objects
//retain their pre-commit positions.
void CommitTestL()
	{
	//Create 2 test tables, insert some records in the first table
	TheDatabase.Close();
	TInt err = TheDatabase.Replace(TheFs, TheDatabaseFileName);
	TEST2(err, KErrNone);
	err = TheDatabase.Execute(_L("CREATE TABLE AA1(Id INTEGER)"));
	TEST2(err, KErrNone);
	err = TheDatabase.Execute(_L("CREATE TABLE AA2(Id INTEGER)"));
	TEST2(err, KErrNone);
	err = TheDatabase.Execute(_L("INSERT INTO AA1(Id) VALUES(1)"));
	TEST2(err, 1);
	err = TheDatabase.Execute(_L("INSERT INTO AA1(Id) VALUES(2)"));
	TEST2(err, 1);
	err = TheDatabase.Execute(_L("INSERT INTO AA1(Id) VALUES(3)"));
	TEST2(err, 1);
	//Begin transaction
	err = TheDatabase.Begin();
	TEST2(err, KErrNone);
	//Prepare read-only view and call FirstL() (TheRowSet object)
	RDbView view;
	err = view.Prepare(TheDatabase, _L("select * from AA1"), RDbRowSet::EReadOnly);
	TEST2(err, KErrNone);
	TheRowSet = view;
	err = view.EvaluateAll();	//DBMS can use FirstL() without the EvaluateAll() call in this case
	TEST2(err, KErrNone);
	TBool rc = TheRowSet.FirstL();
	TEST(rc);
	//Prepare updatable view and call NextL() (TheRowSet2 object)
	err = view.Prepare(TheDatabase, _L("select * from AA1"), RDbRowSet::EUpdatable);
	TEST2(err, KErrNone);
	TheRowSet2 = view;
	err = view.EvaluateAll();	//DBMS can use NextL() without the EvaluateAll() call in this case
	TEST2(err, KErrNone);
	rc = TheRowSet2.FirstL();
	TEST(rc);
	rc = TheRowSet2.NextL();
	TEST(rc);
	//Execute one INSERT statement
	err = TheDatabase.Execute(_L("INSERT INTO AA2(Id) VALUES(1)"));
	TEST2(err, 1);
	//Commit transaction
	err = TheDatabase.Commit();
	TEST2(err, KErrNone);
	//Check the retrieved by TheRowSet record 
	TheRowSet.GetL();
	TEST2(TheRowSet.ColInt(1), 1);
	//Check the retrieved by TheRowSet2 record 
	TheRowSet2.GetL();
	TEST2(TheRowSet2.ColInt(1), 2);
	//Cleanup
	TheRowSet2.Close();
	TheRowSet.Close();
	TheDatabase.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////
	
void DoTestL()
	{
	TheTest.Start(_L("Get fast counter frequency"));
	GetFastCounterFrequency();
	
	TheTest.Next(_L("Create&Fill test database"));
	CreateDatabase();
	FillDatabaseL();

	TheTest.Next(_L(" @SYMTestCaseID:PDS-DBMS-UT-4009 Delete tracks"));
	DeleteTracksL();

	TheTest.Next(_L("Commit() test (not a performance test)"));
	CommitTestL();
	}

///////////////////////////////////////////////////////////////////////////////////////

//Usage: "t_dbperf3 [<drive letter>:]"
TInt E32Main()
    {
	TheTest.Title();
	
	TheTrapCleanup = CTrapCleanup::New();
	TEST(TheTrapCleanup != NULL);
	
	//Construct test database file name
	_LIT(KTestDatabase, "c:\\dbms-tst\\t_dbperf3.db");
	TFileName fname;
	User::CommandLine(fname);
	TParse parse;
	parse.Set(fname, &KTestDatabase, 0);
	const TDesC& dbFilePath = parse.FullName();
	TheDatabaseFileName.Copy(dbFilePath);
	TheTest.Printf(_L("Test database: %S\r\n"), &TheDatabaseFileName);

	__UHEAP_MARK;

	TInt err = TheFs.Connect();
	TEST2(err, KErrNone);
	err = TheFs.MkDir(TheDatabaseFileName);
	TheTest.Printf(_L("MkDir(): err=%d\r\n"), err);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	DeleteFile(TheDatabaseFileName);

	TRAP(err, DoTestL());
	TEST2(err, KErrNone);

	CloseAll();
	DeleteFile(TheDatabaseFileName);

	__UHEAP_MARKEND;

	TheTest.End();
	TheTest.Close();
	
	delete TheTrapCleanup;
	return KErrNone;
    }
