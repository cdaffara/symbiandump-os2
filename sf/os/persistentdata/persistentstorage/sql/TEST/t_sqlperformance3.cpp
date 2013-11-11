// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: MDS harvesting performance test
//
#include <e32test.h>
#include <e32math.h>
#include <bautils.h>
#include <hal.h>
#include <sqldb.h>
#include "t_sqlcmdlineutil.h"

RTest 			TheTest(_L("t_sqlperformance3 test"));
RSqlDatabase 	TheDb;

_LIT(KDbName, 	"c:\\test\\t_sqlperformance3.db");

TFileName		TheDbFileName;
TBuf<200> 		TheTestTitle;
TCmdLineParams 	TheCmdLineParams(TCmdLineParams::EDbUtf16, 4096, 10000);
TBuf8<200> 		TheSqlConfigString;

_LIT(KUtf8,  "UTF8 ");
_LIT(KUtf16, "UTF16");

TInt TheFastCounterFreq = 0;

const TInt KItemCnt = 1000;
const TInt KItemBlockSize = 100;

TInt TheIterations[100];
TInt TheIterationsCount = -1;

void CalcIterationsCount()
	{
	TheIterationsCount = KItemCnt / KItemBlockSize;
	TInt r = KItemCnt % KItemBlockSize;
	if(r > 0)
		{
		++TheIterationsCount;		
		}
	else
		{
		r = KItemBlockSize;
		}
	for(TInt i=0;i<(TheIterationsCount-1);++i)
		{
		TheIterations[i] = KItemBlockSize;
		}
	TheIterations[TheIterationsCount - 1] = r;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////

void TestEnvDestroy()
	{
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
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
		TSqlRetCodeClass cl = SqlRetCodeClass(aValue);
		if(cl == ESqlDbError)
			{
			TPtrC errmsg = TheDb.LastErrorMessage();
			TheTest.Printf(_L("*** SQLite err=\"%S\"\r\n"), &errmsg);
			}
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
	RFs fs;
	TInt err = fs.Connect();
	TEST2(err, KErrNone);
	err = fs.MkDirAll(TheDbFileName);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	fs.Close();
	}

TInt TimeDiffUs(TUint32 aStartTicks, TUint32 aEndTicks)
	{
	if(TheFastCounterFreq == 0)
		{
		TEST2(HAL::Get(HAL::EFastCounterFrequency, TheFastCounterFreq), KErrNone);
		}
	TInt64 diffTicks = (TInt64)aEndTicks - (TInt64)aStartTicks;
	if(diffTicks < 0)
		{
		diffTicks = KMaxTUint32 + diffTicks + 1;
		}
	const TInt KMicroSecIn1Sec = 1000000;
	TInt us = (diffTicks * KMicroSecIn1Sec) / TheFastCounterFreq;
	return us;
	}

void PrintTime(const TDesC& aFmt, TUint32 aStartTicks, TUint32 aEndTicks)
	{
	TInt us = TimeDiffUs(aStartTicks, aEndTicks);
	TheTest.Printf(aFmt, us);
	}

//=============================================================================
TInt TheCreateDbCreateConnTime = 0;
TInt TheCreateDbOpenConnTime = 0;
TInt TheLastItemIdTime = 0;
TInt TheTableExistsTime = 0;
TInt TheMdeTablesCreationTime = 0;
TInt TheLastInsertedRowIdTime = 0;
TInt TheCreateTables1Time = 0;
TInt TheCreateTables2Time = 0;
TInt TheCreateTables3Time = 0;
TInt TheTransactionTime = 0;
TInt TheDbCreateTime = 0;
TInt TheCreateTriggersTime = 0;
TInt TheInsertEventRelationTime = 0;
TInt TheDeleteObjPropertyTime = 0;
TInt TheInsertCol2PropTime = 0;
TInt TheHarvestTime = 0;
TInt TheHarvestSelectTime = 0;
TInt TheHarvestInsertObjImageTime = 0;
TInt TheHarvestSelect2Time = 0;
TInt TheHarvestInsertEventTime = 0;
TInt TheHarvestUpdateObjImageTime = 0;
TInt TheHarvestSelect3Time = 0;
TInt TheHarvestSelect4Time = 0;
TInt TheHarvestUpdateObjTime = 0;
//=============================================================================

void DoCreateDbFile()
	{
	TUint32 fc1 = User::FastCounter();
    TInt err = TheDb.Create(TheDbFileName, &TheSqlConfigString);
    if(err != KErrNone)
    	{
    	if(err == KErrAlreadyExists)
    		{
    		err = TheDb.Open(TheDbFileName, &TheSqlConfigString);
    		if(err == KErrNone)
    			{
    			TUint32 fc2 = User::FastCounter();
    			TheCreateDbCreateConnTime += TimeDiffUs(fc1, fc2);
    			}
    		}
    	}
    else
    	{
    	TUint32 fc2 = User::FastCounter();
		TheCreateDbOpenConnTime += TimeDiffUs(fc1, fc2);
    	}
    TEST2(err, KErrNone);
	}

void DoGetlastItemId()
	{
	_LIT(KGetLastMaxId, "SELECT max(seq) FROM SQLITE_SEQUENCE WHERE name!='symbian_security';");
	TUint32 fc1 = User::FastCounter();
	RSqlStatement stmt;
	TInt err = stmt.Prepare(TheDb, KGetLastMaxId);
	if(err == KErrNone)
		{
		(void)stmt.Next();
		}
	stmt.Close();
	TUint32 fc2 = User::FastCounter();
	TheLastItemIdTime += TimeDiffUs(fc1, fc2);
	}

TBool MdePreferencesCheck()
	{
	_LIT(KCheck, "SELECT COUNT(*) FROM MdE_Preferences;");
	TUint32 fc1 = User::FastCounter();
	RSqlStatement stmt;
	TInt err = stmt.Prepare(TheDb, KCheck);
	if(err == KErrNone)
		{
		err = stmt.Next();
		}
	stmt.Close();
	TUint32 fc2 = User::FastCounter();
	TheTableExistsTime += TimeDiffUs(fc1, fc2);
	return err == KSqlAtRow;
	}

void CreateMdeTables()
	{
	_LIT(KSql1, "CREATE TABLE MdE_Preferences(Key TEXT,Value NONE,ExtraValue LARGEINT,UNIQUE(Key,Value))");
	_LIT(KSql2, "CREATE TABLE MdS_Medias(MediaId INTEGER PRIMARY KEY,Drive INTEGER,PresentState INTEGER,Time LARGEINT)");
	_LIT(KSql3, "INSERT INTO MdE_Preferences(Key, Value, ExtraValue) VALUES(?, ?, ?)");
	
	TUint32 fc1 = User::FastCounter();
	
	TInt rc = TheDb.Exec(KSql1);
	TEST(rc >= 0);
	rc = TheDb.Exec(KSql2);
	TEST(rc >= 0);
	
	RSqlStatement stmt;
	rc = stmt.Prepare(TheDb, KSql3);
	TEST2(rc, KErrNone);
	rc = stmt.BindInt(0, 1);
	TEST2(rc, KErrNone);
	rc = stmt.BindInt(1, 2);
	TEST2(rc, KErrNone);
	rc = stmt.BindInt(2, 3);
	TEST2(rc, KErrNone);
	rc = stmt.Exec();
	TEST2(rc, 1);
	stmt.Close();

	TUint32 fc2 = User::FastCounter();
	TheMdeTablesCreationTime += TimeDiffUs(fc1, fc2);
	}

TInt64 LastInsertedRowId()
	{
	_LIT(KSql, "SELECT last_insert_rowid();");
	TUint32 fc1 = User::FastCounter();
	RSqlStatement stmt;
	TInt err = stmt.Prepare(TheDb, KSql);
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	TInt64 rowid = stmt.ColumnInt64(0); 
	stmt.Close();
	TUint32 fc2 = User::FastCounter();
	TheLastInsertedRowIdTime += TimeDiffUs(fc1, fc2);
	return rowid;
	}

void DoCreateTables1()
	{
	_LIT(KSql1, "CREATE TABLE IF NOT EXISTS NamespaceDef(NamespaceDefId INTEGER PRIMARY KEY,ReadOnly INTEGER,VendorId INTEGER,Name TEXT)");
	_LIT(KSql2, "CREATE TABLE IF NOT EXISTS ObjectDef(ObjectDefId INTEGER PRIMARY KEY,NamespaceDefId INTEGER NOT NULL,ParentDefId INTEGER NOT NULL,Flags INTEGER,Name TEXT)");
	_LIT(KSql3, "CREATE TABLE IF NOT EXISTS PropertyDef(PropertyDefId INTEGER PRIMARY KEY,ObjectDefId INTEGER NOT NULL,Flags INTEGER,Type INTEGER,MinValue NUMERIC,MaxValue NUMERIC,Name TEXT)");
	_LIT(KSql4, "CREATE TABLE IF NOT EXISTS RelationDef(RelationDefId INTEGER PRIMARY KEY,NamespaceDefId INTEGER NOT NULL,Name TEXT)");
	_LIT(KSql5, "CREATE TABLE IF NOT EXISTS EventDef(EventDefId INTEGER PRIMARY KEY,NamespaceDefId INTEGER NOT NULL,Priority INTEGER,Name TEXT)");

	TUint32 fc1 = User::FastCounter();
	
	TInt rc = TheDb.Exec(KSql1);
	TEST(rc >= 0);
	rc = TheDb.Exec(KSql2);
	TEST(rc >= 0);
	rc = TheDb.Exec(KSql3);
	TEST(rc >= 0);
	rc = TheDb.Exec(KSql4);
	TEST(rc >= 0);
	rc = TheDb.Exec(KSql5);
	TEST(rc >= 0);
	
	_LIT(KSql6, "INSERT INTO NamespaceDef(ReadOnly,VendorId,Name) Values(?,?,?);");
	RSqlStatement stmt;
	rc = stmt.Prepare(TheDb, KSql6);
	TEST2(rc, KErrNone);
	rc = stmt.BindInt(0, 1);
	TEST2(rc, KErrNone);
	rc = stmt.BindInt(1, 2);
	TEST2(rc, KErrNone);
	rc = stmt.BindText(2, _L("Nokia"));
	TEST2(rc, KErrNone);
	rc = stmt.Exec();
	TEST2(rc, 1);
	stmt.Close();

	TUint32 fc2 = User::FastCounter();
	TheCreateTables1Time += TimeDiffUs(fc1, fc2);
	}

void BeginTransaction()
	{
	TInt err = TheDb.Exec(_L("BEGIN"));
	TEST(err >= 0);
	}

void CommitTransaction()
	{
	TInt err = TheDb.Exec(_L("COMMIT"));
	TEST(err >= 0);
	}

void DoCreateTables2()
	{
	_LIT(KSql1, "CREATE TABLE IF NOT EXISTS Object1(ObjectId INTEGER PRIMARY KEY AUTOINCREMENT,ObjectDefId INTEGER NOT NULL,Flags INTEGER,MediaId LARGEINT,UsageCount LARGEINT DEFAULT 0,GuidHigh LARGEINT,GuidLow LARGEINT,URI TEXT NOT NULL COLLATE NOCASE,Origin INTEGER,Size LARGEINT,TimeOffset INTEGER,CreationDate LARGEINT,LastModifiedDate LARGEINT,ItemType TEXT,Title TEXT,UNIQUE(GuidHigh,GuidLow),UNIQUE(MediaId,URI))");
	_LIT(KSql2, "CREATE TABLE IF NOT EXISTS Relations1(RelationId INTEGER PRIMARY KEY AUTOINCREMENT,Flags INTEGER,RelationDefId INTEGER NOT NULL,LeftObjectId INTEGER NOT NULL,RightObjectId INTEGER NOT NULL,Parameter INTEGER,GuidHigh LARGEINT,GuidLow LARGEINT,LastModifiedDate LARGEINT)");
	_LIT(KSql3, "CREATE TRIGGER DeleteRelations1 BEFORE DELETE ON Object1 BEGIN DELETE FROM Relations1 WHERE LeftObjectId=OLD.ObjectId OR RightObjectId=OLD.ObjectId; END");
	_LIT(KSql4, "CREATE TABLE IF NOT EXISTS Event1(EventId INTEGER PRIMARY KEY AUTOINCREMENT,ObjectId INTEGER NOT NULL, EventDefId INTEGER NOT NULL, Timestamp INTEGER NOT NULL, Source TEXT, Participant TEXT)");
	_LIT(KSql5, "CREATE TRIGGER DeleteEvent1 BEFORE DELETE ON Object1 BEGIN DELETE FROM Event1 WHERE ObjectId=OLD.ObjectId; END");
	_LIT(KSql6, "CREATE TABLE IF NOT EXISTS TextSearch1(WordId INTEGER NOT NULL,ObjectId INTEGER NOT NULL,Position INTEGER)");
	_LIT(KSql7, "CREATE TRIGGER DeleteTextSearch1 BEFORE DELETE ON Object1 BEGIN DELETE FROM TextSearch1 WHERE ObjectId=OLD.ObjectId; END");
	_LIT(KSql8, "CREATE INDEX FreetextWordIdIndex1 ON TextSearch1(WordId)");
	_LIT(KSql9, "CREATE INDEX FreetextObjectIdIndex1 ON TextSearch1(ObjectId)");
	_LIT(KSql10, "CREATE TABLE IF NOT EXISTS TextSearchDictionary1(WordId INTEGER PRIMARY KEY,Word TEXT NOT NULL)");
	_LIT(KSql11, "CREATE TRIGGER InsertRelations1 INSERT ON Relations1 BEGIN UPDATE Object1 SET UsageCount = UsageCount + 1 WHERE ObjectId=NEW.LeftObjectId OR ObjectId=NEW.RightObjectId; END");
	_LIT(KSql12, "CREATE TRIGGER UpdateRelations1 UPDATE OF LeftObjectId, RightObjectId ON Relations1 WHEN OLD.Flags=NEW.Flags BEGIN UPDATE Object1 SET UsageCount = UsageCount - 1 WHERE ObjectId=OLD.LeftObjectId OR ObjectId=OLD.RightObjectId; UPDATE Object1 SET UsageCount = UsageCount + 1 WHERE ObjectId=NEW.LeftObjectId OR ObjectId=NEW.RightObjectId; END");
	_LIT(KSql13, "CREATE TRIGGER UpdateNotRemovedRelations1 UPDATE OF Flags ON Relations1 WHEN OLD.Flags!=NEW.Flags AND NOT NEW.Flags&3 BEGIN UPDATE Object1 SET UsageCount = UsageCount + 1 WHERE ObjectId=NEW.LeftObjectId OR ObjectId=NEW.RightObjectId; END");
	_LIT(KSql14, "CREATE TRIGGER UpdateRemovedRelations1 UPDATE OF Flags ON Relations1 WHEN OLD.Flags!=NEW.Flags AND NEW.Flags&3 AND NOT OLD.Flags&3 BEGIN UPDATE Object1 SET UsageCount = UsageCount - 1 WHERE ObjectId=OLD.LeftObjectId OR ObjectId=OLD.RightObjectId; END");
	_LIT(KSql15, "CREATE INDEX RelationsLeftObjectIdIndex1 ON Relations1(LeftObjectId)");
	_LIT(KSql16, "CREATE INDEX RelationsRightObjectIdIndex1 ON Relations1(RightObjectId)");
	_LIT(KSql17, "CREATE INDEX ObjectDefIndex1 ON Object1(ObjectDefId)");
	_LIT(KSql18, "CREATE TABLE IF NOT EXISTS Location1(ObjectId INTEGER NOT NULL,CellID LARGEINT,LocationAreaCode LARGEINT,Speed REAL,Direction REAL,Quality REAL,Latitude REAL,Longitude REAL,Altitude REAL,CountryCodeStr TEXT,NetworkCodeStr TEXT,Satellites TEXT,Country TEXT,CellName TEXT, UNIQUE(ObjectId))");
	_LIT(KSql19, "CREATE TABLE IF NOT EXISTS Calendar1(ObjectId INTEGER NOT NULL,CalendarType INTEGER,LocalUid LARGEINT,StartTime LARGEINT,EndTime LARGEINT,Location TEXT,vCalendar TEXT, UNIQUE(ObjectId))");
	_LIT(KSql20, "CREATE TABLE IF NOT EXISTS Contact1(ObjectId INTEGER NOT NULL,Number TEXT,EmailAddress TEXT,Company TEXT,JobTitle TEXT,Address TEXT,ContactURL TEXT,vCard TEXT, UNIQUE(ObjectId))");
	_LIT(KSql21, "CREATE TABLE IF NOT EXISTS Message1(ObjectId INTEGER NOT NULL,Received INTEGER,ToWhom TEXT,Sender TEXT, UNIQUE(ObjectId))");
	_LIT(KSql22, "CREATE TABLE IF NOT EXISTS Album1(ObjectId INTEGER NOT NULL,Ordered INTEGER,Type INTEGER, UNIQUE(ObjectId))");
	_LIT(KSql23, "CREATE TABLE IF NOT EXISTS Tag1(ObjectId INTEGER NOT NULL, UNIQUE(ObjectId))");
	_LIT(KSql24, "CREATE TABLE IF NOT EXISTS MediaObject1(ObjectId INTEGER NOT NULL,Protected INTEGER,DRM INTEGER,ThumbnailPresent INTEGER,Resized INTEGER,Sample INTEGER,Rating INTEGER,Privacy INTEGER,ResolutionUnit INTEGER,Bitrate INTEGER,Width INTEGER,Height INTEGER,Preinstalled INTEGER,AccessCount LARGEINT,DownloadID LARGEINT,Duration REAL,ReleaseDate LARGEINT,CaptureDate LARGEINT,Artist TEXT,Author TEXT,Copyright TEXT,Legal TEXT,Description TEXT,Comment TEXT,Genre TEXT, UNIQUE(ObjectId))");
	_LIT(KSql25, "CREATE TABLE IF NOT EXISTS Video1(ObjectId INTEGER NOT NULL,Protected INTEGER,DRM INTEGER,ThumbnailPresent INTEGER,Resized INTEGER,Sample INTEGER,Rating INTEGER,Privacy INTEGER,ResolutionUnit INTEGER,Bitrate INTEGER,Width INTEGER,Height INTEGER,Preinstalled INTEGER,AccessCount LARGEINT,DownloadID LARGEINT,Duration REAL,ReleaseDate LARGEINT,CaptureDate LARGEINT,AgeProfile INTEGER,AudioFourCC LARGEINT,RecordingFlags LARGEINT,LastPlayPosition REAL,Framerate REAL,Artist TEXT,Author TEXT,Copyright TEXT,Legal TEXT,Description TEXT,Comment TEXT,Genre TEXT,AudioLanguage TEXT, UNIQUE(ObjectId))");
	_LIT(KSql26, "CREATE TABLE IF NOT EXISTS Image1(ObjectId INTEGER NOT NULL,Protected INTEGER,DRM INTEGER,ThumbnailPresent INTEGER,Resized INTEGER,Sample INTEGER,Rating INTEGER,Privacy INTEGER,ResolutionUnit INTEGER,Bitrate INTEGER,Width INTEGER,Height INTEGER,Preinstalled INTEGER,AccessCount LARGEINT,DownloadID LARGEINT,Duration REAL,ReleaseDate LARGEINT,CaptureDate LARGEINT,FocalPlaneResolutionUnit INTEGER,ExposureProgram INTEGER,FocalLengthIn35mmFilm INTEGER,ISOSpeedRatings INTEGER,MeteringMode INTEGER,WhiteBalance INTEGER,Flash INTEGER,ColourSpace INTEGER,Orientation INTEGER,SamplesPerPixel INTEGER,BitsPerSample INTEGER,YCbCrPositioning INTEGER,ThumbCompression INTEGER,ThumbResolutionUnit INTEGER,ThumbOrientation INTEGER,FrameCount INTEGER,PixelXDimension LARGEINT,PixelYDimension LARGEINT,ExifVersion LARGEINT,ComponentsConfiguration LARGEINT,FlashPixVersion LARGEINT,ThumbXResolution LARGEINT,ThumbYResolution LARGEINT,FocalPlaneXResolution REAL,FocalPlaneYResolution REAL,ExposureTime REAL,ExposureBiasValue REAL,FNumber REAL,FocalLength REAL,ApertureValue REAL,MaxAperture REAL,ShutterSpeedValue REAL,XResolution REAL,YResolution REAL,DateTime LARGEINT,DateTimeDigitized LARGEINT,DateTimeOriginal LARGEINT,Artist TEXT,Author TEXT,Copyright TEXT,Legal TEXT,Description TEXT,Comment TEXT,Genre TEXT,Model TEXT,Make TEXT,RelatedSoundFile TEXT, UNIQUE(ObjectId))");
	_LIT(KSql27, "CREATE TABLE IF NOT EXISTS Audio1(ObjectId INTEGER NOT NULL,Protected INTEGER,DRM INTEGER,ThumbnailPresent INTEGER,Resized INTEGER,Sample INTEGER,Rating INTEGER,Privacy INTEGER,ResolutionUnit INTEGER,Bitrate INTEGER,Width INTEGER,Height INTEGER,Preinstalled INTEGER,AccessCount LARGEINT,DownloadID LARGEINT,Duration REAL,ReleaseDate LARGEINT,CaptureDate LARGEINT,Track INTEGER,SamplingFrequency REAL,Artist TEXT,Author TEXT,Copyright TEXT,Legal TEXT,Description TEXT,Comment TEXT,Genre TEXT,Album TEXT,Composer TEXT,OriginalArtist TEXT,AlbumArtist TEXT, UNIQUE(ObjectId))");
	_LIT(KSql28, "CREATE TABLE IF NOT EXISTS TrackLog1(ObjectId INTEGER NOT NULL,Length LARGEINT,StartTime LARGEINT,StopTime LARGEINT, UNIQUE(ObjectId))");
	
	const TPtrC KSql[] = 
		{
		KSql1(), KSql2(), KSql3(), KSql4(), KSql5(), KSql6(), KSql7(), KSql8(), KSql9(), KSql10(), 
		KSql11(), KSql12(), KSql13(), KSql14(), KSql15(), KSql16(), KSql17(), KSql18(), KSql19(), KSql20(), 
		KSql21(), KSql22(), KSql23(), KSql24(), KSql25(), KSql26(), KSql27(), KSql28() 
		};
	
	TUint32 fc1 = User::FastCounter();	
	for(TInt i=0;i<(sizeof(KSql)/sizeof(KSql[0]));++i)
		{
		TInt err = TheDb.Exec(KSql[i]);
		TEST(err >= 0);
		}
	TUint32 fc2 = User::FastCounter();
	TheCreateTables2Time += TimeDiffUs(fc1, fc2);
	}

void DoCreateTables3()
	{
	const TInt KInsObjDefRecCnt = 12;
	const TInt KInsPropDefRecCnt[KInsObjDefRecCnt] = {7, 11, 6, 7, 3, 2, 0, 24, 6, 40, 6, 3};
	TUint32 fc1 = User::FastCounter();	
	for(TInt i=0;i<KInsObjDefRecCnt;++i)
		{
		TBuf<10> objDefName;
		RSqlStatement stmt1;
		TInt err = stmt1.Prepare(TheDb, _L("INSERT INTO ObjectDef(NamespaceDefId, ParentDefId, Flags, Name) Values(?,?,?,?)"));
		TEST2(err, KErrNone);
		err = stmt1.BindInt(0, 1);
		TEST2(err, KErrNone);
		err = stmt1.BindInt(1, 2);
		TEST2(err, KErrNone);
		err = stmt1.BindInt(2, 3);
		TEST2(err, KErrNone);
		objDefName.Copy(_L("ObjDef"));
		objDefName.AppendNum(i + 1);
		err = stmt1.BindText(3, objDefName);
		TEST2(err, KErrNone);
		err = stmt1.Exec();
		TEST2(err, 1);
		stmt1.Close();
		TInt64 objdefid = LastInsertedRowId();
		for(TInt j=0;j<KInsPropDefRecCnt[i];++j)
			{
			TBuf<10> propDefName;
			RSqlStatement stmt2;
			err = stmt2.Prepare(TheDb, _L("INSERT INTO PropertyDef(ObjectDefId,Flags,Type,MinValue,MaxValue,Name) Values(?,?,?,?,?,?);"));
			TEST2(err, KErrNone);
			err = stmt2.BindInt64(0, objdefid);
			TEST2(err, KErrNone);
			err = stmt2.BindInt(1, 2);
			TEST2(err, KErrNone);
			err = stmt2.BindInt(2, 3);
			TEST2(err, KErrNone);
			err = stmt2.BindInt(3, 100);
			TEST2(err, KErrNone);
			err = stmt2.BindInt(4, 200);
			TEST2(err, KErrNone);
			propDefName.Copy(_L("PropDef"));
			propDefName.AppendNum(j + 1);
			err = stmt2.BindText(5, propDefName);
			TEST2(err, KErrNone);
			err = stmt2.Exec();
			TEST2(err, 1);
			stmt2.Close();
			LastInsertedRowId();
			}
		}
	TUint32 fc2 = User::FastCounter();
	TheCreateTables3Time += TimeDiffUs(fc1, fc2);
	}

void DoCreateTriggers()
	{
	_LIT(KSql1, "CREATE TRIGGER DeleteLocation1 DELETE ON Object1 BEGIN DELETE FROM Location1 WHERE ObjectId=OLD.ObjectId; END");
	_LIT(KSql2, "CREATE TRIGGER DeleteCalendar1 DELETE ON Object1 BEGIN DELETE FROM Calendar1 WHERE ObjectId=OLD.ObjectId; END");
	_LIT(KSql3, "CREATE TRIGGER DeleteContact1 DELETE ON Object1 BEGIN DELETE FROM Contact1 WHERE ObjectId=OLD.ObjectId; END");
	_LIT(KSql4, "CREATE TRIGGER DeleteMessage1 DELETE ON Object1 BEGIN DELETE FROM Message1 WHERE ObjectId=OLD.ObjectId; END");
	_LIT(KSql5, "CREATE TRIGGER DeleteAlbum1 DELETE ON Object1 BEGIN DELETE FROM Album1 WHERE ObjectId=OLD.ObjectId; END");
	_LIT(KSql6, "CREATE TRIGGER DeleteTag1 DELETE ON Object1 BEGIN DELETE FROM Tag1 WHERE ObjectId=OLD.ObjectId; END");
	_LIT(KSql7, "CREATE TRIGGER DeleteMediaObject1 DELETE ON Object1 BEGIN DELETE FROM MediaObject1 WHERE ObjectId=OLD.ObjectId; END");
	_LIT(KSql8, "CREATE TRIGGER DeleteVideo1 DELETE ON Object1 BEGIN DELETE FROM Video1 WHERE ObjectId=OLD.ObjectId; END");
	_LIT(KSql9, "CREATE TRIGGER DeleteImage1 DELETE ON Object1 BEGIN DELETE FROM Image1 WHERE ObjectId=OLD.ObjectId; END");
	_LIT(KSql10, "CREATE TRIGGER DeleteAudio1 DELETE ON Object1 BEGIN DELETE FROM Audio1 WHERE ObjectId=OLD.ObjectId; END");
	_LIT(KSql11, "CREATE TRIGGER DeleteTrackLog1 DELETE ON Object1 BEGIN DELETE FROM TrackLog1 WHERE ObjectId=OLD.ObjectId; END");
	
	const TPtrC KSql[] = 
		{
		KSql1(), KSql2(), KSql3(), KSql4(), KSql5(), KSql6(), KSql7(), KSql8(), KSql9(), KSql10(), KSql11() 
		};
	
	TUint32 fc1 = User::FastCounter();	
	for(TInt i=0;i<(sizeof(KSql)/sizeof(KSql[0]));++i)
		{
		TInt err = TheDb.Exec(KSql[i]);
		TEST(err >= 0);
		}
	TUint32 fc2 = User::FastCounter();
	TheCreateTriggersTime += TimeDiffUs(fc1, fc2);
	}

void DoInsertEventRelationDefs()
	{
	_LIT(KSql1, "INSERT INTO EventDef (NamespaceDefId, Priority, Name ) Values(?, ?, ?);");
	const TInt KSql1Cnt = 7;
	_LIT(KSql2, "INSERT INTO RelationDef (NamespaceDefId,Name) Values(?,?);");
	const TInt KSql2Cnt = 8;
	
	TUint32 fc1 = User::FastCounter();	
	
	for(TInt i=0;i<KSql1Cnt;++i)
		{
		TBuf<10> eventDefName;
		RSqlStatement stmt;
		TInt err = stmt.Prepare(TheDb, KSql1);
		TEST2(err, KErrNone);
		err = stmt.BindInt(0, 1);
		TEST2(err, KErrNone);
		err = stmt.BindInt(1, 2);
		TEST2(err, KErrNone);
		eventDefName.Copy(_L("EvtDef"));
		eventDefName.AppendNum(i + 1);
		err = stmt.BindText(2, eventDefName);
		TEST2(err, KErrNone);
		err = stmt.Exec();
		TEST2(err, 1);
		stmt.Close();
		LastInsertedRowId();
		}

	for(TInt i=0;i<KSql2Cnt;++i)
		{
		TBuf<10> relDefName;
		RSqlStatement stmt;
		TInt err = stmt.Prepare(TheDb, KSql2);
		TEST2(err, KErrNone);
		err = stmt.BindInt(0, 1);
		TEST2(err, KErrNone);
		relDefName.Copy(_L("RelDef"));
		relDefName.AppendNum(i + 1);
		err = stmt.BindText(1, relDefName);
		TEST2(err, KErrNone);
		err = stmt.Exec();
		TEST2(err, 1);
		stmt.Close();
		LastInsertedRowId();
		}
	
	TUint32 fc2 = User::FastCounter();
	TheInsertEventRelationTime += TimeDiffUs(fc1, fc2);
	}

void DoDeleteObjPropertyDef()
	{
	_LIT(KSql1, "DELETE FROM ObjectDef WHERE ObjectDefId=?;");
	_LIT(KSql2, "DELETE FROM PropertyDef WHERE ObjectDefId=?;");

	TUint32 fc1 = User::FastCounter();
	
	RSqlStatement stmt1;
	TInt err = stmt1.Prepare(TheDb, KSql1);
	TEST2(err, KErrNone);
	err = stmt1.BindInt(0, 0);
	TEST2(err, KErrNone);
	err = stmt1.Exec();
	TEST(err >= 0);
	stmt1.Close();
	
	RSqlStatement stmt2;
	err = stmt2.Prepare(TheDb, KSql2);
	TEST2(err, KErrNone);
	err = stmt2.BindInt(0, 0);
	TEST2(err, KErrNone);
	err = stmt2.Exec();
	TEST(err >= 0);
	stmt2.Close();
	
	TUint32 fc2 = User::FastCounter();
	TheDeleteObjPropertyTime += TimeDiffUs(fc1, fc2);
	}

void DoInsertCol2Prop()
	{
	_LIT(KSql1, "CREATE TABLE IF NOT EXISTS Col2Prop(ObjectDefId INTEGER NOT NULL,PropertyDefId INTEGER NOT NULL,ColumnId INTEGER NOT NULL)");
	TInt KCol2PropCnt = KItemCnt;
	_LIT(KSql2, "INSERT INTO Col2Prop(ObjectDefId,PropertyDefId,ColumnId) Values(?,?,?);");
	
	TUint32 fc1 = User::FastCounter();

	TInt err = TheDb.Exec(KSql1);
	TEST(err >= 0);
	for(TInt i=0;i<KCol2PropCnt;++i)
		{
		RSqlStatement stmt;
		err = stmt.Prepare(TheDb, KSql2);
		TEST2(err, KErrNone);
		err = stmt.BindInt(0, 10);
		TEST2(err, KErrNone);
		err = stmt.BindInt(1, 20);
		TEST2(err, KErrNone);
		err = stmt.BindInt(2, i);
		TEST2(err, KErrNone);
		err = stmt.Exec();
		TEST(err >= 0);
		stmt.Close();
		}
	
	TUint32 fc2 = User::FastCounter();
	TheInsertCol2PropTime += TimeDiffUs(fc1, fc2);
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4149
@SYMTestCaseDesc		MDE database creation performance test.
						The test creates the MDE database using the SQL statements from the 
						production code of the MetaDataEngine server. The time taken for the execution of
						SQL statements is measured and printed out.
@SYMTestActions			MDE database creation performance test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMDEF					DEF142327
*/
void CreateDb()
	{
	DoCreateDbFile();
	TheDb.Close();
	DoCreateDbFile();
	
	TUint32 fc1 = User::FastCounter();	
	
	////////////////////////////////
	DoGetlastItemId();
	MdePreferencesCheck();
	CreateMdeTables();
	////////////////////////////////
	TUint32 fc2 = User::FastCounter();	
	BeginTransaction();
	//
	DoCreateTables1();
	DoCreateTables2();
	DoCreateTables3();
	DoCreateTriggers();
	DoInsertEventRelationDefs();
	DoDeleteObjPropertyDef();
	DoInsertCol2Prop();
	//
	CommitTransaction();
	TUint32 fc3 = User::FastCounter();
	TheTransactionTime += TimeDiffUs(fc2, fc3);
	TheDbCreateTime += TimeDiffUs(fc1, fc3);
	}

void DoHarvestSelect()
	{
	TUint32 fc1 = User::FastCounter();	
	for(TInt i=0;i<TheIterationsCount;++i)
		{
		BeginTransaction();
		for(TInt j=0;j<TheIterations[i];++j)
			{
			_LIT(KSql, "SELECT ObjectId, Flags, LastModifiedDate, Size FROM Object1 WHERE NOT Flags&? AND (Flags&? OR Flags&?) AND MediaId=? AND URI=?;");
			RSqlStatement stmt;
			TInt err = stmt.Prepare(TheDb, KSql);
			TEST2(err, KErrNone);
			err = stmt.BindInt(0, 8);
			TEST2(err, KErrNone);
			err = stmt.BindInt(1, 16);
			TEST2(err, KErrNone);
			err = stmt.BindInt(2, 32);
			TEST2(err, KErrNone);
			TBuf<16> media;
			media.Copy(_L("media"));
			media.AppendNum(j + 1);
			err = stmt.BindText(3, media);
			TEST2(err, KErrNone);
			TBuf<16> uri;
			uri.Copy(_L("uri"));
			uri.AppendNum(j + 1);
			err = stmt.BindText(4, uri);
			TEST2(err, KErrNone);
			while((err = stmt.Next()) == KSqlAtRow)
				{
				}
			TEST2(err, KSqlAtEnd);
			stmt.Close();
			}
		CommitTransaction();
		}
	TUint32 fc2 = User::FastCounter();
	TheHarvestSelectTime += TimeDiffUs(fc1, fc2);
	}

void DoHarvestInsertObjImage()
	{
	_LIT(KSql1, "INSERT INTO Object1(ObjectId,ObjectDefId,Flags,MediaId,GuidHigh,GuidLow,URI,Origin,Size,CreationDate,LastModifiedDate) VALUES(?,?,?,?,?,?,?,?,?,?,?);");
	_LIT(KSql2, "INSERT INTO Image1(ObjectId) VALUES(?);");
	
	TUint32 fc1 = User::FastCounter();	
	TInt objdef = 0;
	for(TInt i=0;i<TheIterationsCount;++i)
		{
		BeginTransaction();
		for(TInt j=0;j<TheIterations[i];++j)
			{
			++objdef;
			RSqlStatement stmt1;
			TInt err = stmt1.Prepare(TheDb, KSql1);
			TEST2(err, KErrNone);
			err = stmt1.BindInt(0, objdef);
			TEST2(err, KErrNone);
			err = stmt1.BindInt(1, 9);
			TEST2(err, KErrNone);
			err = stmt1.BindInt(2, 48);
			TEST2(err, KErrNone);
			err = stmt1.BindInt(3, 3);
			TEST2(err, KErrNone);
			err = stmt1.BindInt(4, j + 1 + i * 100);
			TEST2(err, KErrNone);
			err = stmt1.BindInt(5, j + 1 + i * 10000);
			TEST2(err, KErrNone);
			TBuf<16> uri;
			uri.Copy(_L("URI-"));
			uri.AppendNum(j + 1 + i * 100);
			err = stmt1.BindText(6, uri);
			TEST2(err, KErrNone);
			err = stmt1.BindInt(7, 0);
			TEST2(err, KErrNone);
			err = stmt1.BindInt(8, 1000000);
			TEST2(err, KErrNone);
			err = stmt1.BindInt64(9, 32324234218723LL);
			TEST2(err, KErrNone);
			err = stmt1.BindInt64(10, 5675069785676565LL);
			TEST2(err, KErrNone);
			err = stmt1.Exec();
			TEST(err >= 0);
			stmt1.Close();

			RSqlStatement stmt2;
			err = stmt2.Prepare(TheDb, KSql2);
			TEST2(err, KErrNone);
			err = stmt2.BindInt(0, objdef);
			TEST2(err, KErrNone);
			err = stmt2.Exec();
			TEST(err >= 0);
			stmt2.Close();
			}
		CommitTransaction();
		}
	TUint32 fc2 = User::FastCounter();
	TheHarvestInsertObjImageTime += TimeDiffUs(fc1, fc2);
	}

void DoHarvestSelect2()
	{
	TUint32 fc1 = User::FastCounter();
	///////////////////////////////////////////////////////////////////////
	_LIT(KSql1, "SELECT count(*) FROM Object1 AS BO WHERE(NOT Flags&?)AND (NOT Flags&?) AND (((BO.ObjectId IN(SELECT ObjectId FROM Object1 WHERE((NOT Flags&?)AND (NOT Flags&?)AND (NOT Flags&?)AND ObjectDefId=?))OR BO.ObjectId IN(SELECT ObjectId FROM Object1 WHERE((NOT Flags&?)AND (NOT Flags&?)AND (NOT Flags&?)AND ObjectDefId=?)))AND BO.ObjectId IN(SELECT ObjectId FROM Object1 WHERE(Origin=?)))) ORDER BY LastModifiedDate DESC,BO.ObjectId DESC ;");
	RSqlStatement stmt1;
	TInt err = stmt1.Prepare(TheDb, KSql1);
	TEST2(err, KErrNone);
	err = stmt1.BindInt(0, 8);
	TEST2(err, KErrNone);
	err = stmt1.BindInt(1, 16);
	TEST2(err, KErrNone);
	err = stmt1.BindInt(2, 32);
	TEST2(err, KErrNone);
	err = stmt1.BindInt(3, 64);
	TEST2(err, KErrNone);
	err = stmt1.BindInt(4, 128);
	TEST2(err, KErrNone);
	err = stmt1.BindInt(5, 1);
	TEST2(err, KErrNone);
	err = stmt1.BindInt(6, 8);
	TEST2(err, KErrNone);
	err = stmt1.BindInt(7, 16);
	TEST2(err, KErrNone);
	err = stmt1.BindInt(8, 32);
	TEST2(err, KErrNone);
	err = stmt1.BindInt(9, 1);
	TEST2(err, KErrNone);
	err = stmt1.BindInt(10, 1);
	TEST2(err, KErrNone);
	while((err = stmt1.Next()) == KSqlAtRow)
		{
		}
	TEST2(err, KSqlAtEnd);
	stmt1.Close();
	///////////////////////////////////////////////////////////////////////
	_LIT(KSql2, "SELECT count(*) FROM Object1 AS BO WHERE(NOT Flags&?)AND (NOT Flags&?)AND ((BO.ObjectId IN(SELECT ObjectId FROM Object1 WHERE(Origin=?)))) ORDER BY CreationDate DESC,BO.ObjectId DESC ;");
	RSqlStatement stmt2;
	err = stmt2.Prepare(TheDb, KSql2);
	TEST2(err, KErrNone);
	err = stmt2.BindInt(0, 8);
	TEST2(err, KErrNone);
	err = stmt2.BindInt(1, 16);
	TEST2(err, KErrNone);
	err = stmt2.BindInt(2, 1);
	TEST2(err, KErrNone);
	while((err = stmt2.Next()) == KSqlAtRow)
		{
		}
	TEST2(err, KSqlAtEnd);
	stmt2.Close();
	///////////////////////////////////////////////////////////////////////
	_LIT(KSql3, "SELECT BO.* FROM Object1 AS BO WHERE(NOT Flags&?)AND (NOT Flags&?)AND ((BO.ObjectId IN(SELECT ObjectId FROM Object1 WHERE(Origin=?)))) ORDER BY CreationDate ASC,BO.ObjectId ASC ;");
	RSqlStatement stmt3;
	err = stmt3.Prepare(TheDb, KSql3);
	TEST2(err, KErrNone);
	err = stmt3.BindInt(0, 8);
	TEST2(err, KErrNone);
	err = stmt3.BindInt(1, 16);
	TEST2(err, KErrNone);
	err = stmt3.BindInt(2, 1);
	TEST2(err, KErrNone);
	while((err = stmt3.Next()) == KSqlAtRow)
		{
		}
	TEST2(err, KSqlAtEnd);
	stmt3.Close();
	/////////////////////////////////////////////////////////////////////////
	_LIT(KSql4, "SELECT count(*) FROM Object1 AS BO ,Album1 AS O ON BO.ObjectId=O.ObjectId WHERE(NOT Flags&?)AND (NOT Flags&?)  ORDER BY Title COLLATE NOCASE  ASC ;");
	RSqlStatement stmt4;
	err = stmt4.Prepare(TheDb, KSql4);
	TEST2(err, KErrNone);
	err = stmt4.BindInt(0, 8);
	TEST2(err, KErrNone);
	err = stmt4.BindInt(1, 16);
	TEST2(err, KErrNone);
	while((err = stmt4.Next()) == KSqlAtRow)
		{
		}
	TEST2(err, KSqlAtEnd);
	stmt4.Close();
	///////////////////////////////////////////////////////////////////////////
	_LIT(KSql5, "SELECT count(*) FROM Object1 AS BO WHERE(NOT Flags&?)AND (NOT Flags&?)AND (((BO.ObjectId IN(SELECT ObjectId FROM Object1 WHERE((NOT Flags&?)AND (NOT Flags&?)AND (NOT Flags&?)AND ObjectDefId=?))OR BO.ObjectId IN(SELECT ObjectId FROM Object1 WHERE((NOT Flags&?)AND (NOT Flags&?)AND (NOT Flags&?)AND ObjectDefId=?)))AND BO.ObjectId IN(SELECT ObjectId FROM Object1 WHERE(Origin!=?)))) ORDER BY CreationDate DESC,BO.ObjectId DESC ;");
	RSqlStatement stmt5;
	err = stmt5.Prepare(TheDb, KSql5);
	TEST2(err, KErrNone);
	err = stmt5.BindInt(0, 8);
	TEST2(err, KErrNone);
	err = stmt5.BindInt(1, 16);
	TEST2(err, KErrNone);
	err = stmt5.BindInt(2, 32);
	TEST2(err, KErrNone);
	err = stmt5.BindInt(3, 64);
	TEST2(err, KErrNone);
	err = stmt5.BindInt(4, 128);
	TEST2(err, KErrNone);
	err = stmt5.BindInt(5, 1);
	TEST2(err, KErrNone);
	err = stmt5.BindInt(6, 8);
	TEST2(err, KErrNone);
	err = stmt5.BindInt(7, 16);
	TEST2(err, KErrNone);
	err = stmt5.BindInt(8, 32);
	TEST2(err, KErrNone);
	err = stmt5.BindInt(9, 1);
	TEST2(err, KErrNone);
	err = stmt5.BindInt(10, 1);
	TEST2(err, KErrNone);
	while((err = stmt5.Next()) == KSqlAtRow)
		{
		}
	TEST2(err, KSqlAtEnd);
	stmt5.Close();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	_LIT(KSql6, "SELECT count(*) FROM Object1 AS BO ,Tag1 AS O ON BO.ObjectId=O.ObjectId WHERE(NOT Flags&?)AND (NOT Flags&?)AND ((O.ObjectId IN(SELECT ObjectId FROM Object1 WHERE((NOT Flags&?)AND (NOT Flags&?)AND (NOT Flags&?)AND UsageCount>=?)))) ORDER BY Title COLLATE NOCASE  ASC ;");	
	RSqlStatement stmt6;
	err = stmt6.Prepare(TheDb, KSql6);
	TEST2(err, KErrNone);
	err = stmt6.BindInt(0, 8);
	TEST2(err, KErrNone);
	err = stmt6.BindInt(1, 16);
	TEST2(err, KErrNone);
	err = stmt6.BindInt(2, 32);
	TEST2(err, KErrNone);
	err = stmt6.BindInt(3, 64);
	TEST2(err, KErrNone);
	err = stmt6.BindInt(4, 128);
	TEST2(err, KErrNone);
	err = stmt6.BindInt(5, 1);
	TEST2(err, KErrNone);
	while((err = stmt6.Next()) == KSqlAtRow)
		{
		}
	TEST2(err, KSqlAtEnd);
	stmt6.Close();
	
	TUint32 fc2 = User::FastCounter();
	TheHarvestSelect2Time += TimeDiffUs(fc1, fc2);
	}

void DoHarvestInsertEvent()
	{
	TUint32 fc1 = User::FastCounter();
	
	_LIT(KSql, "INSERT INTO Event1(EventId,ObjectId,EventDefId,Timestamp,Source,Participant) VALUES(?,?,?,?,?,?);");
	const TInt KEventCnt = KItemCnt;
	
	for(TInt i=0;i<KEventCnt;++i)
		{
		BeginTransaction();
		RSqlStatement stmt;
		TInt err = stmt.Prepare(TheDb, KSql);
		TEST2(err, KErrNone);
		err = stmt.BindInt(0, i + 1);
		TEST2(err, KErrNone);
		err = stmt.BindInt(1, i + 1);
		TEST2(err, KErrNone);
		err = stmt.BindInt(2, i + 1);
		TEST2(err, KErrNone);
		err = stmt.BindInt64(3, 329231202230LL);
		TEST2(err, KErrNone);
		err = stmt.BindInt(4, 1);
		TEST2(err, KErrNone);
		err = stmt.BindInt(5, 367);
		TEST2(err, KErrNone);
		stmt.Close();
		CommitTransaction();
		}
	
	TUint32 fc2 = User::FastCounter();
	TheHarvestInsertEventTime += TimeDiffUs(fc1, fc2);
	}

void DoHarvestUpdateObjImage()
	{
	TUint32 fc1 = User::FastCounter();
	
	_LIT(KSql1, "UPDATE Object1 SET Flags=? ,Origin=?,TimeOffset=?,ItemType=?,Title=? WHERE ObjectId=?;");
	_LIT(KSql2, "UPDATE Image1 SET Width=?,Height=?,BitsPerSample=?,FrameCount=? WHERE ObjectId=?;");
	
	for(TInt i=0;i<KItemCnt;++i)
		{
		BeginTransaction();
		
		RSqlStatement stmt1;
		TInt err = stmt1.Prepare(TheDb, KSql1);
		TEST2(err, KErrNone);
		err = stmt1.BindInt(0, 32);
		TEST2(err, KErrNone);
		err = stmt1.BindInt(1, 1);
		TEST2(err, KErrNone);
		err = stmt1.BindInt(2, 2);
		TEST2(err, KErrNone);
		err = stmt1.BindInt(3, 3);
		TEST2(err, KErrNone);
		TBuf<32> title;
		title.Copy(_L("Title-"));
		title.AppendNum(i + 1);
		err = stmt1.BindText(4, title);
		TEST2(err, KErrNone);
		err = stmt1.BindInt(5, i + 1);
		TEST2(err, KErrNone);
		err = stmt1.Exec();
		TEST2(err, 1);
		stmt1.Close();

		RSqlStatement stmt2;
		err = stmt2.Prepare(TheDb, KSql2);
		TEST2(err, KErrNone);
		err = stmt2.BindInt(0, 1000);
		TEST2(err, KErrNone);
		err = stmt2.BindInt(1, 1200);
		TEST2(err, KErrNone);
		err = stmt2.BindInt(2, 24);
		TEST2(err, KErrNone);
		err = stmt2.BindInt(3, 3);
		TEST2(err, KErrNone);
		err = stmt2.BindInt(4, i + 1);
		TEST2(err, KErrNone);
		err = stmt2.Exec();
		TEST2(err, 1);
		stmt2.Close();

		CommitTransaction();
		}
	
	TUint32 fc2 = User::FastCounter();
	TheHarvestUpdateObjImageTime += TimeDiffUs(fc1, fc2);
	}

void DoHarvestSelect3()
	{
	TUint32 fc1 = User::FastCounter();
	
	_LIT(KSql1, "SELECT BO.* ,O.* FROM Object1 AS BO ,Image1 AS O ON BO.ObjectId=O.ObjectId WHERE(NOT Flags&?)AND (NOT Flags&?)AND ((O.ObjectId IN(SELECT ObjectId FROM Object1 WHERE((NOT Flags&?)AND (NOT Flags&?)AND ObjectId=?))))LIMIT 1;");
	
	for(TInt i=0;i<KItemCnt;++i)
		{
		RSqlStatement stmt;
		TInt err = stmt.Prepare(TheDb, KSql1);
		TEST2(err, KErrNone);
		err = stmt.BindInt(0, 65536);
		TEST2(err, KErrNone);
		err = stmt.BindInt(1, 65536);
		TEST2(err, KErrNone);
		err = stmt.BindInt(2, 65536);
		TEST2(err, KErrNone);
		err = stmt.BindInt(3, 65536);
		TEST2(err, KErrNone);
		err = stmt.BindInt(4, i + 1);
		TEST2(err, KErrNone);
		TInt cnt = 0;
		while((err = stmt.Next()) == KSqlAtRow)
			{
			++cnt;
			}
		TEST2(err, KSqlAtEnd);
		TEST2(cnt, 1);
		stmt.Close();
		}

	TUint32 fc2 = User::FastCounter();
	TheHarvestSelect3Time += TimeDiffUs(fc1, fc2);
	}

void DoHarvestSelect4()
	{
	TUint32 fc1 = User::FastCounter();
	
	_LIT(KSql2, "SELECT BO.* FROM Object1 AS BO WHERE(NOT Flags&?)AND (NOT Flags&?)AND ((BO.ObjectId IN(SELECT ObjectId FROM Object1 WHERE((NOT Flags&?)AND (NOT Flags&?)AND (NOT Flags&?)AND ObjectId=?))));");
	
	for(TInt i=0;i<KItemCnt;++i)
		{
		RSqlStatement stmt;
		TInt err = stmt.Prepare(TheDb, KSql2);
		TEST2(err, KErrNone);
		err = stmt.BindInt(0, 65536);
		TEST2(err, KErrNone);
		err = stmt.BindInt(1, 65536);
		TEST2(err, KErrNone);
		err = stmt.BindInt(2, 65536);
		TEST2(err, KErrNone);
		err = stmt.BindInt(3, 65536);
		TEST2(err, KErrNone);
		err = stmt.BindInt(4, 65536);
		TEST2(err, KErrNone);
		err = stmt.BindInt(5, i + 1);
		TEST2(err, KErrNone);
		TInt cnt = 0;
		while((err = stmt.Next()) == KSqlAtRow)
			{
			++cnt;
			}
		TEST2(err, KSqlAtEnd);
		TEST2(cnt, 1);
		stmt.Close();
		}
	
	TUint32 fc2 = User::FastCounter();
	TheHarvestSelect4Time += TimeDiffUs(fc1, fc2);
	}

void DoHarvestUpdateObj()
	{
	TUint32 fc1 = User::FastCounter();

	_LIT(KSql, "UPDATE Object1 SET Flags=Flags&? WHERE ObjectId IN(?);");
	
	for(TInt i=0;i<KItemCnt;++i)
		{
		BeginTransaction();
		RSqlStatement stmt;
		TInt err = stmt.Prepare(TheDb, KSql);
		TEST2(err, KErrNone);
		err = stmt.BindInt(0, 32768);
		TEST2(err, KErrNone);
		err = stmt.BindInt(1, i + 1);
		TEST2(err, KErrNone);
		err = stmt.Exec();
		TEST2(err, 1);
		stmt.Close();
		CommitTransaction();
		}
	
	TUint32 fc2 = User::FastCounter();
	TheHarvestUpdateObjTime += TimeDiffUs(fc1, fc2);
	}

void Harvest()
	{
	TUint32 fc1 = User::FastCounter();	
	DoHarvestSelect();
	DoHarvestInsertObjImage();
	DoHarvestSelect2();
	DoHarvestSelect2();
	DoHarvestSelect2();
	DoHarvestInsertEvent();
	DoHarvestUpdateObjImage();
	DoHarvestSelect3();
	DoHarvestSelect4();
	DoHarvestUpdateObj();
	TUint32 fc2 = User::FastCounter();
	TheHarvestTime += TimeDiffUs(fc1, fc2);
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4150
@SYMTestCaseDesc		MDS harvesting performance test.
						The test reproduces the MDS harvesting actions with the SQL server,
						performed for 1000 images. The time taken for the execution of SQL statements 
						is measured and printed out.
@SYMTestActions			MDS harvesting performance test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMDEF					DEF142327
*/
void PrintResults()
	{
	TheTest.Printf(_L("==Fast counter frequency=%d Hz\r\n"), TheFastCounterFreq);
	TheTest.Printf(_L("==Create database, time=%d us\r\n"), TheCreateDbCreateConnTime);
	TheTest.Printf(_L("==Open database, time=%d us\r\n"), TheCreateDbOpenConnTime);
	TheTest.Printf(_L("=='SELECT max(seq) FROM SQLITE_SEQUENCE', time=%d us\r\n"), TheLastItemIdTime);
	TheTest.Printf(_L("=='SELECT COUNT(*) FROM MdE_Preferences', time=%d us\r\n"), TheTableExistsTime);
	TheTest.Printf(_L("==MDE tables creation, time=%d us\r\n"), TheMdeTablesCreationTime);
	TheTest.Printf(_L("=='SELECT last_insert_rowid()', time=%d us\r\n"), TheLastInsertedRowIdTime);
	TheTest.Printf(_L("==1.Create tables, time=%d us\r\n"), TheCreateTables1Time);
	TheTest.Printf(_L("==2.Create tables, time=%d us\r\n"), TheCreateTables2Time);
	TheTest.Printf(_L("==3.Create tables, time=%d us\r\n"), TheCreateTables3Time);
	TheTest.Printf(_L("==InsertEventRelation, time=%d us\r\n"), TheInsertEventRelationTime);
	TheTest.Printf(_L("==Create triggers, time=%d us\r\n"), TheCreateTriggersTime);
	TheTest.Printf(_L("==DeleteObjectProperty, time=%d us\r\n"), TheDeleteObjPropertyTime);
	TheTest.Printf(_L("==InsertCol2Property, time=%d us\r\n"), TheInsertCol2PropTime);
	TheTest.Printf(_L("==The create transaction, time=%d us\r\n"), TheTransactionTime);
	TheTest.Printf(_L("==Database create schema, time=%d us\r\n"), TheDbCreateTime);

	TheTest.Printf(_L("==HarvestSelect, time=%d us\r\n"), TheHarvestSelectTime);
	TheTest.Printf(_L("==HarvestInsertObjImage, time=%d us\r\n"), TheHarvestInsertObjImageTime);
	TheTest.Printf(_L("==HarvestSelect2, time=%d us\r\n"), TheHarvestSelect2Time);
	TheTest.Printf(_L("==HarvestSelect3, time=%d us\r\n"), TheHarvestSelect3Time);
	TheTest.Printf(_L("==HarvestSelect4, time=%d us\r\n"), TheHarvestSelect4Time);
	TheTest.Printf(_L("==HarvestInsertEvent, time=%d us\r\n"), TheHarvestInsertEventTime);
	TheTest.Printf(_L("==HarvestUpdateObjImage, time=%d us\r\n"), TheHarvestUpdateObjImageTime);
	TheTest.Printf(_L("==HarvestUpdateObj, time=%d us\r\n"), TheHarvestUpdateObjTime);
	TheTest.Printf(_L("==Harvest, time=%d us\r\n"), TheHarvestTime);
	}

void DoTestsL()
	{
	CalcIterationsCount();
	
	TheTestTitle.Format(_L("@SYMTestCaseID:PDS-SQL-UT-4149 Create database, encoding: \"%S\", page size: %d\r\n"), 
			TheCmdLineParams.iDbEncoding == TCmdLineParams::EDbUtf16 ? &KUtf16 : &KUtf8, TheCmdLineParams.iPageSize);
	TheTest.Start(TheTestTitle);
	CreateDb();

	TheTestTitle.Format(_L("@SYMTestCaseID:PDS-SQL-UT-4150 Harvest %d items, encoding: \"%S\", page size: %d\r\n"), 
			KItemCnt, TheCmdLineParams.iDbEncoding == TCmdLineParams::EDbUtf16 ? &KUtf16 : &KUtf8, TheCmdLineParams.iPageSize);
	TheTest.Next(TheTestTitle);
	Harvest();
	
	PrintResults();
	}

TInt E32Main()
	{
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	
	__UHEAP_MARK;

	GetCmdLineParamsAndSqlConfigString(TheTest, _L("t_sqlperformance3"), TheCmdLineParams, TheSqlConfigString);
	PrepareDbName(KDbName, TheCmdLineParams.iDriveName, TheDbFileName);

	TheTest.Printf(_L("==Databases: %S\r\n"), &TheDbFileName); 
	
	TestEnvDestroy();
	TestEnvInit();
	TRAPD(err, DoTestsL());
	TestEnvDestroy();
	TEST2(err, KErrNone);

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
