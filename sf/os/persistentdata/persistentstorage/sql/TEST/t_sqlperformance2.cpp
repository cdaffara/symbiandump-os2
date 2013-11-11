// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "t_sqlcmdlineutil.h"

///////////////////////////////////////////////////////////////////////////////////////

RTest			TheTest(_L("t_sqlperformance2 test"));
RSqlDatabase 	TheDb;
TFileName		TheDbFileName;
RFs				TheFs;

TBuf<200> 		TheTestTitle;
TCmdLineParams 	TheCmdLineParams;
TBuf8<200> 		TheSqlConfigString;

_LIT(KUtf8,  "UTF8 ");
_LIT(KUtf16, "UTF16");

TInt TheBlobSize = 1024 * 256;

///////////////////////////////////////////////////////////////////////////////////////

void TestEnvDestroy()
	{
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
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
    //
    TInt driveNumber = -1;
	err = RFs::CharToDrive(TheCmdLineParams.iDriveName[0], driveNumber);
	TEST2(err, KErrNone);
	TDriveNumber driveNo = static_cast <TDriveNumber> (driveNumber);
	TDriveInfo driveInfo;
	err = TheFs.Drive(driveInfo, driveNo);
	TEST2(err, KErrNone);
    //Create the test directory
	err = TheFs.MkDirAll(TheDbFileName);
	TEST(err == KErrNone || err == KErrAlreadyExists);
    //Print drive info and the database name 
	_LIT(KType1, "Not present");
	_LIT(KType2, "Unknown");
	_LIT(KType3, "Floppy");
	_LIT(KType4, "Hard disk");
	_LIT(KType5, "CD ROM");
	_LIT(KType6, "RAM disk");
	_LIT(KType7, "Flash");
	_LIT(KType8, "ROM drive");
	_LIT(KType9, "Remote drive");
	_LIT(KType10,"NAND flash");
	_LIT(KType11,"Rotating media");
	TPtrC KMediaTypeNames[] = {KType1(), KType2(), KType3(), KType4(), KType5(), KType6(), KType7(), KType8(), KType9(), KType10(), KType11()};
	TheTest.Printf(_L("Drive %C: %S. File: \"%S\"\r\n"), 'A' + driveNo, &KMediaTypeNames[driveInfo.iType], &TheDbFileName);
	}
	
void PrintWriteTime(TTimeIntervalMicroSeconds aTime, TTimeIntervalMicroSeconds aWriteTime, TTimeIntervalMicroSeconds aCommitTime)
	{
	TheTest.Printf(_L("####Execution time: %d ms, Write: %d ms, Commit: %d ms\r\n"), 
		(TInt)(aTime.Int64() / 1000), (TInt)(aWriteTime.Int64() / 1000), (TInt)(aCommitTime.Int64() / 1000));
	}

void PrintReadTime(TTimeIntervalMicroSeconds aPrepareTime, TTimeIntervalMicroSeconds aNextTime, TTimeIntervalMicroSeconds aReadTime)
	{
	TInt executionTime = (TInt)(aPrepareTime.Int64() / 1000) + (TInt)(aNextTime.Int64() / 1000) + (TInt)(aReadTime.Int64() / 1000);
	TheTest.Printf(_L("####Execution time: %d ms, Prepare: %d ms, Next: %d ms, Read: %d ms\r\n"), 
		executionTime, (TInt)(aPrepareTime.Int64() / 1000), (TInt)(aNextTime.Int64() / 1000), (TInt)(aReadTime.Int64() / 1000));
	}

void PrintReadTime(TTimeIntervalMicroSeconds aOpenTime, TTimeIntervalMicroSeconds aReadTime)
	{
	TInt executionTime = (TInt)(aOpenTime.Int64() / 1000) + (TInt)(aReadTime.Int64() / 1000);
	TheTest.Printf(_L("####Execution time: %d ms, Open: %d ms, Read: %d ms\r\n"), 
		executionTime, (TInt)(aOpenTime.Int64() / 1000), (TInt)(aReadTime.Int64() / 1000));
	}
	
void PrintFileSize(RSqlDatabase& aDb)
	{
	TheTest.Printf(_L("####FileSize: %d\r\n"), aDb.Size());
	}
		
///////////////////////////////////////////////////////////////////////////////////////
	
void CreateTestDb()
	{
	(void)RSqlDatabase::Delete(TheDbFileName);
	TInt err = TheDb.Create(TheDbFileName, &TheSqlConfigString);
	TEST2(err, KErrNone);
	err = TheDb.Exec(_L8("CREATE TABLE A(B BLOB)"));
	TEST2(err, 1);
	}

void DoWriteBlobIncrL(const TDesC8& aData, 
					  TTime& aT1, 
					  TTime& aT2,
					  TTime& aT3,
					  TTime& aT4)
	{
	
	RSqlBlobWriteStream strm;
	CleanupClosePushL(strm);
	
	aT1.HomeTime();
	strm.OpenL(TheDb, _L("A"), _L("B"));	
	strm.WriteL(aData);
	aT2.HomeTime();
	
	aT3.HomeTime();
	strm.CommitL();
	aT4.HomeTime();
	
	CleanupStack::PopAndDestroy(&strm);
	}
	
void InsertZeroBlob(TBool aDoPrintTime = EFalse)
	{
	TBuf<100> sql;
	sql.Format(_L("INSERT INTO A VALUES(zeroblob(%d))"), TheBlobSize);
	
	TTime t1, t2;
	t1.HomeTime();
	TInt err = TheDb.Exec(sql);
	t2.HomeTime();
	TEST2(err, 1);
	TTimeIntervalMicroSeconds insertTime = t2.MicroSecondsFrom(t1);
	
	if(aDoPrintTime)
		{
		PrintWriteTime(insertTime, TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(0));
		}
	}
	
void InsertRealBlob()
	{
	HBufC8* data = HBufC8::New(TheBlobSize);
	TEST(data != NULL);
	TPtr8 dataptr = data->Des();
	dataptr.SetLength(TheBlobSize);
	dataptr.Fill(TChar('A'));
		
	RSqlStatement stmt;
	TInt err = stmt.Prepare(TheDb, _L8("INSERT INTO A VALUES(:Prm)"));
	TEST2(err, KErrNone);

	RSqlParamWriteStream strm;
	err = strm.BindBinary(stmt, 0);
	TEST2(err, KErrNone);
	
	TRAP(err, strm.WriteL(dataptr));
	TEST2(err, KErrNone);
	TRAP(err, strm.CommitL());
	TEST2(err, KErrNone);
	strm.Close();

	err = stmt.Exec();	
	TEST2(err, 1);			
	stmt.Close();	
	
	delete data;
	}
	
void InsertBlobIncr()
	{
	HBufC8* data = HBufC8::New(TheBlobSize);
	TEST(data != NULL);
	TPtr8 dataptr = data->Des();
	dataptr.SetLength(TheBlobSize);
	dataptr.Fill(TChar('B'));
	
	TTimeIntervalMicroSeconds totalTime, writeTime, commitTime;

	TBuf<100> sql;
	sql.Format(_L("INSERT INTO A VALUES(zeroblob(%d))"), TheBlobSize);

	TTime t1, t2, subt1, subt2, subt3, subt4;
	t1.HomeTime();
	
	TInt err = TheDb.Exec(_L8("BEGIN"));
	TEST(err >= 0);
	
	err = TheDb.Exec(sql);
	TEST2(err, 1);
	
	TRAP(err, DoWriteBlobIncrL(dataptr, subt1, subt2, subt3, subt4));
	TEST2(err, KErrNone);
	
	err = TheDb.Exec(_L8("COMMIT"));
	TEST(err >= 0);
		
	t2.HomeTime();
	totalTime = t2.MicroSecondsFrom(t1);
	
	writeTime = subt2.MicroSecondsFrom(subt1);
	commitTime = subt4.MicroSecondsFrom(subt3);
	
	PrintWriteTime(totalTime, writeTime, commitTime);
	
	delete data;
	}
	
void InsertBlobExec()
	{
	HBufC8* buf = HBufC8::New(TheBlobSize * 2 + 100);
	TEST(buf != NULL);
	TPtr8 sql = buf->Des();
	_LIT8(KStr, "INSERT INTO A VALUES(x'");
	sql.SetLength(TheBlobSize * 2 + KStr().Length());
	sql.Fill(TChar('A'));
	sql.Replace(0, KStr().Length(), KStr);
	sql.Append(_L8("')"));

	TTime t1, t2;
	
	t1.HomeTime();		
	TInt err = TheDb.Exec(sql);
	t2.HomeTime();
	TEST2(err, 1);

	TTimeIntervalMicroSeconds totalTime = t2.MicroSecondsFrom(t1);
	
	PrintWriteTime(totalTime, TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(0));
	
	delete buf;	
	}

void InsertBlobBindStreamPrm()
	{
	HBufC8* data = HBufC8::New(TheBlobSize);
	TEST(data != NULL);
	TPtr8 dataptr = data->Des();
	dataptr.SetLength(TheBlobSize);
	dataptr.Fill(TChar('A'));
	
	TTimeIntervalMicroSeconds totalTime, writeTime, commitTime;

	TTime t1, t2, t3, t4, t5, t6;
	t1.HomeTime();
				
	RSqlStatement stmt;
	TInt err = stmt.Prepare(TheDb, _L8("INSERT INTO A VALUES(:Prm)"));
	TEST2(err, KErrNone);

	RSqlParamWriteStream strm;
	err = strm.BindBinary(stmt, 0);
	TEST2(err, KErrNone);
	
	t3.HomeTime();
	TRAP(err, strm.WriteL(dataptr));
	t4.HomeTime();
	TEST2(err, KErrNone);
	
	t5.HomeTime();
	TRAP(err, strm.CommitL());
	t6.HomeTime();
	TEST2(err, KErrNone);
	
	err = stmt.Exec();	
	
	strm.Close();
	stmt.Close();	
	
	t2.HomeTime();
	TEST2(err, 1);
					
	totalTime = t2.MicroSecondsFrom(t1);
	
	writeTime = t4.MicroSecondsFrom(t3);
	commitTime = t6.MicroSecondsFrom(t5);
	
	PrintWriteTime(totalTime, writeTime, commitTime);
			
	delete data;	
	}
	
void UpdateBlobIncr()
	{
	HBufC8* data = HBufC8::New(TheBlobSize);
	TEST(data != NULL);
	TPtr8 dataptr = data->Des();
	dataptr.SetLength(TheBlobSize);
	dataptr.Fill(TChar('A'));
	
	TTimeIntervalMicroSeconds totalTime, writeTime, commitTime;

	TTime t1, t2, subt1, subt2, subt3, subt4;
	t1.HomeTime();
	
	TInt err = TheDb.Exec(_L8("BEGIN"));
	TEST(err >= 0);
		
	TRAP(err, DoWriteBlobIncrL(dataptr, subt1, subt2, subt3, subt4));
	TEST2(err, KErrNone);
	
	err = TheDb.Exec(_L8("COMMIT"));
	TEST(err >= 0);
		
	t2.HomeTime();
	totalTime = t2.MicroSecondsFrom(t1);
	
	writeTime = subt2.MicroSecondsFrom(subt1);
	commitTime = subt4.MicroSecondsFrom(subt3);
	
	PrintWriteTime(totalTime, writeTime, commitTime);
	
	delete data;
	}
	
void UpdateBlobExec()
	{
	HBufC8* buf = HBufC8::New(TheBlobSize * 2 + 100);
	TEST(buf != NULL);
	TPtr8 sql = buf->Des();
	_LIT8(KStr, "UPDATE A SET B=x'");
	sql.SetLength(TheBlobSize * 2 + KStr().Length());
	sql.Fill(TChar('B'));
	sql.Replace(0, KStr().Length(), KStr);
	sql.Append(_L8("'"));

	TTime t1, t2;
	t1.HomeTime();			
	TInt err = TheDb.Exec(sql);	
	t2.HomeTime();
	TEST2(err, 1);

	TTimeIntervalMicroSeconds totalTime = t2.MicroSecondsFrom(t1);
	
	PrintWriteTime(totalTime, TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(0));
	
	delete buf;	
	}

void UpdateBlobBindStreamPrm()
	{
	HBufC8* data = HBufC8::New(TheBlobSize);
	TEST(data != NULL);
	TPtr8 dataptr = data->Des();
	dataptr.SetLength(TheBlobSize);
	dataptr.Fill(TChar('B'));
	
	TTimeIntervalMicroSeconds totalTime, writeTime, commitTime;

	TTime t1, t2, t3, t4, t5, t6;
	t1.HomeTime();
				
	RSqlStatement stmt;
	TInt err = stmt.Prepare(TheDb, _L8("UPDATE A SET B=(:Prm)"));
	TEST2(err, KErrNone);

	RSqlParamWriteStream strm;
	err = strm.BindBinary(stmt, 0);
	TEST2(err, KErrNone);
	
	t3.HomeTime();;
	TRAP(err, strm.WriteL(dataptr));
	t4.HomeTime();
	TEST2(err, KErrNone);
	
	t5.HomeTime();
	TRAP(err, strm.CommitL());
	t6.HomeTime();
	TEST2(err, KErrNone);
	
	err = stmt.Exec();	

	strm.Close();		
	stmt.Close();	

	t2.HomeTime();
	TEST2(err, 1);
	
	totalTime = t2.MicroSecondsFrom(t1);
	
	writeTime = t4.MicroSecondsFrom(t3);
	commitTime = t6.MicroSecondsFrom(t5);
	
	PrintWriteTime(totalTime, writeTime, commitTime);
			
	delete data;	
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4084
@SYMTestCaseDesc		SQL, BLOB write, performance tests.
						Tests insertion and updates of BLOBs using the old
						APIs and the new RSqlBlobWriteStream APIs.
@SYMTestPriority		Medium
@SYMTestActions			Insertion and updates of blobs using the old and new APIs.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5912
*/
void BlobWriteTest()
	{	
	TheTest.Printf(_L("Blob size=%d Kb\r\n"), TheBlobSize / 1024);
	
	//Insert a blob
	TheTest.Printf(_L("==================================================================\r\n"));
	
	CreateTestDb();
	TheTest.Printf(_L("INSERT zeroblob - RSqlDatabase::Exec()\r\n"));
	PrintFileSize(TheDb);
	InsertZeroBlob(ETrue);
	PrintFileSize(TheDb);
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
				
	CreateTestDb();
	TheTest.Printf(_L("INSERT blob - RSqlParamWriteStream\r\n"));
	PrintFileSize(TheDb);
	InsertBlobBindStreamPrm();
	PrintFileSize(TheDb);
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
	
	CreateTestDb();
	TheTest.Printf(_L("INSERT blob - RSqlDatabase::Exec()\r\n"));
	PrintFileSize(TheDb);
	InsertBlobExec();
	PrintFileSize(TheDb);
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
	
	CreateTestDb();
	TheTest.Printf(_L("INSERT blob - RSqlBlobWriteStream + transaction\r\n"));
	PrintFileSize(TheDb);
	InsertBlobIncr();
	PrintFileSize(TheDb);
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
						
	// Update a blob 
	TheTest.Printf(_L("==================================================================\r\n"));
		
	CreateTestDb();
	TheTest.Printf(_L("UPDATE zeroblob - RSqlParamWriteStream\r\n"));
	PrintFileSize(TheDb);
	InsertZeroBlob();
	UpdateBlobBindStreamPrm();
	PrintFileSize(TheDb);
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
		
	CreateTestDb();
	TheTest.Printf(_L("UPDATE blob - RSqlParamWriteStream\r\n"));
	PrintFileSize(TheDb);
	InsertRealBlob();
	UpdateBlobBindStreamPrm();
	PrintFileSize(TheDb);
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
			
	CreateTestDb();
	TheTest.Printf(_L("UPDATE zeroblob - RSqlDatabase::Exec()\r\n"));
	PrintFileSize(TheDb);
	InsertZeroBlob();
	UpdateBlobExec();
	PrintFileSize(TheDb);
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
		
	CreateTestDb();
	TheTest.Printf(_L("UPDATE blob - RSqlDatabase::Exec()\r\n"));
	PrintFileSize(TheDb);
	InsertRealBlob();
	UpdateBlobExec();
	PrintFileSize(TheDb);
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
		
	CreateTestDb();
	TheTest.Printf(_L("UPDATE zeroblob - RSqlBlobWriteStream + transaction\r\n"));
	PrintFileSize(TheDb);
	InsertZeroBlob();
	UpdateBlobIncr();
	PrintFileSize(TheDb);
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
		
	CreateTestDb();
	TheTest.Printf(_L("UPDATE blob - RSqlBlobWriteStream + transaction\r\n"));
	PrintFileSize(TheDb);
	InsertRealBlob();
	UpdateBlobIncr();
	PrintFileSize(TheDb);
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);	
	}

void DoReadBlobIncrL(TDes8& aDes, TInt aMaxLength)
	{
	TTime t1, t2, t3, t4;
	
	TTimeIntervalMicroSeconds openTime, readTime; 
	
	RSqlBlobReadStream strm;
	CleanupClosePushL(strm);
	
	t1.HomeTime();
	strm.OpenL(TheDb, _L("A"), _L("B"), 1);
	t2.HomeTime();
	
	openTime = t2.MicroSecondsFrom(t1);
		
	t3.HomeTime();
	strm.ReadL(aDes, aMaxLength);
	t4.HomeTime();

	readTime = t4.MicroSecondsFrom(t3);
	
	PrintReadTime(openTime, readTime);

	CleanupStack::PopAndDestroy(&strm);
	}

void ReadBlobIncr()
	{
	HBufC8* data = HBufC8::New(TheBlobSize);
	TEST(data != NULL);
	TPtr8 dataptr = data->Des();
	
	TRAPD(err, DoReadBlobIncrL(dataptr, TheBlobSize));
	TEST2(err, KErrNone);
	TEST2(dataptr.Length(), TheBlobSize);
	
	delete data;
	}

void ReadBlobColDes()
	{
	HBufC8* data = HBufC8::New(TheBlobSize);
	TEST(data != NULL);
	TPtr8 dataptr = data->Des();
	
	TTime t1, t2, t3, t4, t5, t6;
	TTimeIntervalMicroSeconds prepareTime, nextTime, readTime;
	
	RSqlStatement stmt;
	t1.HomeTime();
	TInt err = stmt.Prepare(TheDb, _L8("SELECT B FROM A WHERE ROWID=1"));
	t2.HomeTime();
	TEST2(err, KErrNone);
	prepareTime = t2.MicroSecondsFrom(t1);

	t3.HomeTime();
	err = stmt.Next();
	t4.HomeTime();
	TEST2(err, KSqlAtRow);
	nextTime = t4.MicroSecondsFrom(t3);
	
	t5.HomeTime();
	err = stmt.ColumnBinary(0, dataptr);
	t6.HomeTime();
	TEST2(err, KErrNone);
	TEST2(dataptr.Length(), TheBlobSize);
	readTime = t6.MicroSecondsFrom(t5);
		
	PrintReadTime(prepareTime, nextTime, readTime);
	stmt.Close();
	
	delete data;
	}

void ReadBlobColPtr()
	{
	TTime t1, t2, t3, t4, t5, t6;
	TTimeIntervalMicroSeconds prepareTime, nextTime, readTime;
	
	RSqlStatement stmt;
	t1.HomeTime();
	TInt err = stmt.Prepare(TheDb, _L8("SELECT B FROM A WHERE ROWID=1"));
	t2.HomeTime();
	TEST2(err, KErrNone);
	prepareTime = t2.MicroSecondsFrom(t1);
	
	t3.HomeTime();
	err = stmt.Next();
	t4.HomeTime();
	TEST2(err, KSqlAtRow);
	nextTime = t4.MicroSecondsFrom(t3);
	
	TPtrC8 data;
	t5.HomeTime();
	err = stmt.ColumnBinary(0, data);
	t6.HomeTime();
	TEST2(err, KErrNone);
	TEST2(data.Length(), TheBlobSize);
	readTime = t6.MicroSecondsFrom(t5);

	PrintReadTime(prepareTime, nextTime, readTime);
	stmt.Close();
	}

void ReadBlobStreamCol()
	{
	HBufC8* data = HBufC8::New(TheBlobSize);
	TEST(data != NULL);
	TPtr8 dataptr = data->Des();
	
	TTime t1, t2, t3, t4, t5, t6;
	TTimeIntervalMicroSeconds prepareTime, nextTime, readTime;
	
	RSqlStatement stmt;
	t1.HomeTime();
	TInt err = stmt.Prepare(TheDb, _L8("SELECT B FROM A WHERE ROWID=1"));
	t2.HomeTime();
	TEST2(err, KErrNone);
	prepareTime = t2.MicroSecondsFrom(t1);

	t3.HomeTime();
	err = stmt.Next();
	t4.HomeTime();
	TEST2(err, KSqlAtRow);
	nextTime = t4.MicroSecondsFrom(t3);

	RSqlColumnReadStream strm;
	t5.HomeTime();
	err = strm.ColumnBinary(stmt, 0);
	TEST2(err, KErrNone);
	TRAP(err, strm.ReadL(dataptr, TheBlobSize));
	t6.HomeTime();
	TEST2(err, KErrNone);
	TEST2(dataptr.Length(), TheBlobSize);
	readTime = t6.MicroSecondsFrom(t5);

	strm.Close();
	stmt.Close();
	
	PrintReadTime(prepareTime, nextTime, readTime);
		
	delete data;
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-4085
@SYMTestCaseDesc		SQL, BLOB read, performance tests.
						Tests retrieval of BLOBs using the old
						APIs and the new RSqlBlobReadStream APIs.
@SYMTestPriority		Medium
@SYMTestActions			Retrieval of blobs using the old and new APIs.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5912
*/
void BlobReadTest()
	{
	TheTest.Printf(_L("Blob size=%d Kb\r\n"), TheBlobSize / 1024);
	
	// Insert a blob
	TheTest.Printf(_L("==================================================================\r\n"));
	TheTest.Printf(_L("Insert blob\r\n"));
		
	CreateTestDb();
	PrintFileSize(TheDb);
	InsertBlobExec();
	PrintFileSize(TheDb);
	TheDb.Close();
		
	// Read the blob
	TheTest.Printf(_L("==================================================================\r\n"));
		
	TheTest.Printf(_L("Read blob - RSqlBlobReadStream\r\n"));	
	TInt err = TheDb.Open(TheDbFileName);
	TEST2(err, KErrNone);
	ReadBlobIncr();
	TheDb.Close();

	TheTest.Printf(_L("Read blob - RSqlStatement::ColumnBinary(TInt, TDes8&)\r\n"));	
	err = TheDb.Open(TheDbFileName);
	TEST2(err, KErrNone);
	ReadBlobColDes();
	TheDb.Close();
		
	TheTest.Printf(_L("Read blob - RSqlStatement::ColumnBinary(TInt, TPtrC8&)\r\n"));	
	err = TheDb.Open(TheDbFileName);
	TEST2(err, KErrNone);
	ReadBlobColPtr();
	TheDb.Close();
		
	TheTest.Printf(_L("Read blob - RSqlColumnReadStream\r\n"));	
	err = TheDb.Open(TheDbFileName);
	TEST2(err, KErrNone);
	ReadBlobStreamCol();
	TheDb.Close();
		
	(void)RSqlDatabase::Delete(TheDbFileName);
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-4115
@SYMTestCaseDesc		SQL, sequential BLOB writes, performance tests.
						Tests sequentially writing 32Kb blocks to a 1.125Mb blob
						using the new RSqlBlobWriteStream APIs to examine
					    the write performance at different stages in the 
						sequence.
@SYMTestPriority		Medium
@SYMTestActions			Sequential writing of a blob using the new RSqlBlobWriteStream APIs.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5912
*/
void SequentialWriteTestL()
	{
	const TInt KBufLen = 32768; // 32Kb
	HBufC8* buf = HBufC8::NewL(KBufLen);
	TPtr8 dataPtr =	buf->Des();
	dataPtr.SetLength(KBufLen);
	dataPtr.Fill('A', KBufLen);	
	
	CreateTestDb();
	InsertZeroBlob(); // insert zeroblob of "TheBlobSize" size
		
	RSqlBlobWriteStream strm;
	strm.OpenL(TheDb, _L("A"), _L("B"));	
								
	// Sequentially write 32Kb blocks of data to the 
	// blob until the 1Mb cache is full and writes to the disk begin.
	// 32 * 32Kb = 1Mb = soft heap limit
	const TInt KItCount = TheBlobSize / KBufLen - 1;
	for(TInt i = 1; i <= KItCount; ++i)
		{
		TheTest.Printf(_L("***Iteration %d\r\n"), i);
		
		PrintFileSize(TheDb);
	
		TTimeIntervalMicroSeconds writeTime;
		TTime t1, t2;

		t1.HomeTime();
		strm.WriteL(dataPtr);
		t2.HomeTime();		

		writeTime = t2.MicroSecondsFrom(t1);
							
		PrintWriteTime(TTimeIntervalMicroSeconds(0), writeTime, TTimeIntervalMicroSeconds(0));		
		PrintFileSize(TheDb);		
		}	
			
	TTimeIntervalMicroSeconds commitTime;
	TTime t3, t4;
	t3.HomeTime();
	strm.CommitL();
	t4.HomeTime();	
	commitTime = t4.MicroSecondsFrom(t3);
		
	PrintWriteTime(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(0), commitTime);		
	PrintFileSize(TheDb);		
			
	strm.Close();		
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
		
	delete buf;
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-4116
@SYMTestCaseDesc		SQL, transaction sequential BLOB writes, performance tests.
						Tests sequentially writing 32Kb blocks to a 1.125Mb blob
						within a transaction, using the new RSqlBlobWriteStream APIs,
						to examine the write performance at different stages in the 
						sequence.
@SYMTestPriority		Medium
@SYMTestActions			Sequential writing of a blob within a transactions, using the 
						new RSqlBlobWriteStream APIs.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5912
*/
void TransSequentialWriteTestL()
	{
	const TInt KBufLen = 32768; // 32Kb
	HBufC8* buf = HBufC8::NewL(KBufLen);
	TPtr8 dataPtr =	buf->Des();
	dataPtr.SetLength(KBufLen);
	dataPtr.Fill('A', KBufLen);	
	
	CreateTestDb();
	InsertZeroBlob(); // insert zeroblob of "TheBlobSize" size
		
	RSqlBlobWriteStream strm;
	strm.OpenL(TheDb, _L("A"), _L("B"));
	
	TInt err = TheDb.Exec(_L8("BEGIN"));
	TEST(err >= 0);	
								
	// Sequentially write 32Kb blocks of data to the 
	// blob until the 1Mb cache is full and writes to the disk begin.
	// 32 * 32Kb = 1Mb = soft heap limit
	const TInt KItCount = TheBlobSize / KBufLen - 1;
	for(TInt i = 1; i <= KItCount; ++i)
		{
		TheTest.Printf(_L("***Iteration %d\r\n"), i);
		
		PrintFileSize(TheDb);
	
		TTimeIntervalMicroSeconds writeTime;
		TTime t1, t2;
		
		t1.HomeTime();
		strm.WriteL(dataPtr);
		t2.HomeTime();	

		writeTime = t2.MicroSecondsFrom(t1);
							
		PrintWriteTime(TTimeIntervalMicroSeconds(0), writeTime, TTimeIntervalMicroSeconds(0));		
		PrintFileSize(TheDb);		
		}	
		
	TTimeIntervalMicroSeconds commitTime;
	TTime t3, t4;
	
	t3.HomeTime();
	strm.CommitL();
	t4.HomeTime();	
	commitTime = t4.MicroSecondsFrom(t3);

	PrintWriteTime(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(0), commitTime);		
	PrintFileSize(TheDb);		
		
	TTime t5, t6;
	t5.HomeTime();	
	err = TheDb.Exec(_L8("COMMIT"));
	t6.HomeTime();	
	TEST(err >= 0);
	
	TTimeIntervalMicroSeconds transCommitTime = t6.MicroSecondsFrom(t5);		
	PrintWriteTime(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(0), transCommitTime);		
	PrintFileSize(TheDb);	
			
	strm.Close();		
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
		
	delete buf;
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4117
@SYMTestCaseDesc		SQL, whole BLOB write, performance tests.
						Tests writing a 256Kb data block to a 256Kb blob to examine the
						relative performance of the TSqlBlob and RSqlBlobWriteStream APIs.
@SYMTestPriority		Medium
@SYMTestActions			Whole update of a blob using the new TSqlBlob and RSqlBlobWriteStream APIs.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5912
*/
void WholeWriteTestL()
	{
	TInt bufLen = TheBlobSize; 
	HBufC8* buf = HBufC8::NewL(bufLen);
	TPtr8 dataPtr =	buf->Des();
	dataPtr.SetLength(bufLen);
	dataPtr.Fill('Z', bufLen);	
	
	CreateTestDb();
	InsertRealBlob(); // insert blob of "TheBlobSize" size
			
	TheTest.Printf(_L("***WholeWriteTestL - %dKb blob \r\n"), TheBlobSize / 1024);		
	PrintFileSize(TheDb);
	
	// TSqlBlob::Set
	TTimeIntervalMicroSeconds writeTime;
	TTime t1, t2, t3, t4;

	t1.HomeTime();
	TSqlBlob::SetL(TheDb, _L("A"), _L("B"), dataPtr);	
	t2.HomeTime();	
		
	writeTime = t2.MicroSecondsFrom(t1);
								
	PrintWriteTime(TTimeIntervalMicroSeconds(0), writeTime, TTimeIntervalMicroSeconds(0));		
	PrintFileSize(TheDb);
	
	// to avoid caching issues, close and re-create the database for the next part
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);		
	CreateTestDb();
	InsertRealBlob(); // insert blob of "TheBlobSize" size
	PrintFileSize(TheDb);

	// RSqlBlobWriteStream::WriteL
	t3.HomeTime();
	RSqlBlobWriteStream strm;
	CleanupClosePushL(strm);
	strm.OpenL(TheDb, _L("A"), _L("B"));	
	strm.WriteL(dataPtr);
	CleanupStack::PopAndDestroy(&strm);
	t4.HomeTime();	
		
	writeTime = t4.MicroSecondsFrom(t3);
							
	PrintWriteTime(TTimeIntervalMicroSeconds(0), writeTime, TTimeIntervalMicroSeconds(0));		
	PrintFileSize(TheDb);				
				
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
		
	delete buf;
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-4118
@SYMTestCaseDesc		SQL, transaction whole BLOB write, performance tests.
						Tests writing a 256Kb data block to a 256Kb blob, within a transaction,
						to examine the relative performance of the TSqlBlob and RSqlBlobWriteStream APIs.
@SYMTestPriority		Medium
@SYMTestActions			Whole update of a blob, within a transaction, using the new TSqlBlob and 
						RSqlBlobWriteStream APIs.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5912
*/
void TransWholeWriteTestL()
	{
	TInt bufLen = TheBlobSize;
	HBufC8* buf = HBufC8::NewL(bufLen);
	TPtr8 dataPtr =	buf->Des();
	dataPtr.SetLength(bufLen);
	dataPtr.Fill('Z', bufLen);	
	
	CreateTestDb();
	InsertRealBlob(); // insert blob of "TheBlobSize" size
	
	TheTest.Printf(_L("***TransWholeWriteTestL - %dKb blob\r\n"), TheBlobSize / 1024);	
	PrintFileSize(TheDb);
		
	// TSqlBlob::Set
	TTimeIntervalMicroSeconds writeTime;
	TTime t1, t2, t3, t4;

	t1.HomeTime();
	TInt err = TheDb.Exec(_L8("BEGIN"));
	TEST(err >= 0);		
	TSqlBlob::SetL(TheDb, _L("A"), _L("B"), dataPtr);			
	err = TheDb.Exec(_L8("COMMIT"));
	t2.HomeTime();	
	TEST(err >= 0);	

	writeTime = t2.MicroSecondsFrom(t1);
							
	PrintWriteTime(TTimeIntervalMicroSeconds(0), writeTime, TTimeIntervalMicroSeconds(0));		
	PrintFileSize(TheDb);	
	
	// to avoid caching issues, close and re-create the database for the next part
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
	CreateTestDb();
	InsertRealBlob(); // insert blob of "TheBlobSize" size
	PrintFileSize(TheDb);
	
	// RSqlBlobWriteStream::WriteL
	t3.HomeTime();
	err = TheDb.Exec(_L8("BEGIN"));
	TEST(err >= 0);	
	RSqlBlobWriteStream strm;
	CleanupClosePushL(strm);
	strm.OpenL(TheDb, _L("A"), _L("B"));	
	strm.WriteL(dataPtr);
	CleanupStack::PopAndDestroy(&strm);
	err = TheDb.Exec(_L8("COMMIT"));
	t4.HomeTime();
	TEST(err >= 0);	

	writeTime = t4.MicroSecondsFrom(t3);
							
	PrintWriteTime(TTimeIntervalMicroSeconds(0), writeTime, TTimeIntervalMicroSeconds(0));		
	PrintFileSize(TheDb);				
				
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
		
	delete buf;
	}	

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4119
@SYMTestCaseDesc		SQL, whole BLOB read, performance tests.
						Tests reading a 256Kb blob in one block to examine the
						relative performance of the TSqlBlob and RSqlBlobReadStream APIs.
@SYMTestPriority		Medium
@SYMTestActions			Whole retrieval of a blob using the new TSqlBlob and RSqlBlobReadStream APIs.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5912
*/	
void WholeReadTestL()
	{
	TInt bufLen = TheBlobSize; 
	HBufC8* buf = HBufC8::NewL(bufLen);
	TPtr8 dataPtr =	buf->Des();
	dataPtr.SetLength(bufLen);
	dataPtr.Fill('A', bufLen);	
	
	CreateTestDb();
	InsertRealBlob(); // insert blob of "TheBlobSize" size
	
	TheTest.Printf(_L("***WholeReadTestL - %dKb blob \r\n"), TheBlobSize / 1024);
	PrintFileSize(TheDb);
	
	// TSqlBlob::GetLC
	TTimeIntervalMicroSeconds readTime;
	TTime t1, t2, t3, t4, t5, t6;

	t1.HomeTime();
	HBufC8* readBuf = TSqlBlob::GetLC(TheDb, _L("A"), _L("B"));
	t2.HomeTime();
	TEST(readBuf->Des().Compare(buf->Des()) == 0);
	
	readTime = t2.MicroSecondsFrom(t1);
							
	PrintReadTime(TTimeIntervalMicroSeconds(0), readTime);
	PrintFileSize(TheDb);				
	CleanupStack::PopAndDestroy(readBuf);
	
	// to avoid caching issues, close and re-create the database for the next part
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
	CreateTestDb();
	InsertRealBlob(); // insert blob of "TheBlobSize" size
	PrintFileSize(TheDb);
	
	// TSqlBlob::Get
	HBufC8* preBuf = HBufC8::NewLC(bufLen);
	TPtr8 preBufPtr(preBuf->Des());
	t3.HomeTime();
	TInt err = TSqlBlob::Get(TheDb, _L("A"), _L("B"), preBufPtr);	
	t4.HomeTime();
	TEST2(err, KErrNone);
	TEST(preBufPtr.Compare(buf->Des()) == 0);

	readTime = t4.MicroSecondsFrom(t3);
							
	PrintReadTime(TTimeIntervalMicroSeconds(0), readTime);
	PrintFileSize(TheDb);				
	CleanupStack::PopAndDestroy(preBuf);
	
	// to avoid caching issues, close and re-create the database for the next part
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
	CreateTestDb();
	InsertRealBlob(); // insert blob of "TheBlobSize" size
	PrintFileSize(TheDb);
	
	// RSqlBlobReadStream::ReadL
	preBuf = HBufC8::NewLC(bufLen);	
	TPtr8 preBufP(preBuf->Des());
	t5.HomeTime();
	RSqlBlobReadStream strm;
	CleanupClosePushL(strm);
	strm.OpenL(TheDb, _L("A"), _L("B"));
	strm.ReadL(preBufP, bufLen);
	CleanupStack::PopAndDestroy(&strm);
	t6.HomeTime();
	TEST(preBufP.Compare(buf->Des()) == 0);
		
	readTime = t6.MicroSecondsFrom(t5);
							
	PrintReadTime(TTimeIntervalMicroSeconds(0), readTime);
	PrintFileSize(TheDb);	
	CleanupStack::PopAndDestroy(preBuf);
			
	TheDb.Close();
	(void)RSqlDatabase::Delete(TheDbFileName);
		
	delete buf;
	}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void DoTests()
	{
	TheTestTitle.Format(_L("@SYMTestCaseID:SYSLIB-SQL-UT-4084 SQL, BLOB write, performance tests, encoding: \"%S\", page size: %d\r\n"), 
			TheCmdLineParams.iDbEncoding == TCmdLineParams::EDbUtf16 ? &KUtf16 : &KUtf8, TheCmdLineParams.iPageSize);
	TheTest.Start(TheTestTitle);
	BlobWriteTest();
	
	TheTestTitle.Format(_L("@SYMTestCaseID:SYSLIB-SQL-UT-4085 SQL, BLOB read, performance tests, encoding: \"%S\", page size: %d\r\n"), 
			TheCmdLineParams.iDbEncoding == TCmdLineParams::EDbUtf16 ? &KUtf16 : &KUtf8, TheCmdLineParams.iPageSize);
	TheTest.Next(TheTestTitle);
	BlobReadTest();

	TheTest.Printf(_L("==================================================================\r\n"));
	
	// Bigger blob tests - only on hardware, release mode
#if !defined __WINS__ && !defined __WINSCW__ && !defined _DEBUG
	
	TheBlobSize = 1024 * 1024 + 128 * 1024;//1.125Mb 

	TheTestTitle.Format(_L("@SYMTestCaseID:SYSLIB-SQL-UT-4115 SQL, sequential BLOB writes, performance tests, encoding: \"%S\", page size: %d\r\n"), 
			TheCmdLineParams.iDbEncoding == TCmdLineParams::EDbUtf16 ? &KUtf16 : &KUtf8, TheCmdLineParams.iPageSize);
	TheTest.Next(TheTestTitle);
	TRAPD(err, SequentialWriteTestL());
	TEST2(err, KErrNone);
	
	TheTestTitle.Format(_L("@SYMTestCaseID:SYSLIB-SQL-UT-4116 SQL, transaction sequential BLOB writes, performance tests, encoding: \"%S\", page size: %d\r\n"), 
			TheCmdLineParams.iDbEncoding == TCmdLineParams::EDbUtf16 ? &KUtf16 : &KUtf8, TheCmdLineParams.iPageSize);
	TheTest.Next(TheTestTitle);
	TRAP(err, TransSequentialWriteTestL());
	TEST2(err, KErrNone);
		
	TheBlobSize = 256 * 1024 ; // 256Kb
		
	TheTestTitle.Format(_L("@SYMTestCaseID:SYSLIB-SQL-UT-4117 SQL, whole BLOB write, performance tests, encoding: \"%S\", page size: %d\r\n"), 
			TheCmdLineParams.iDbEncoding == TCmdLineParams::EDbUtf16 ? &KUtf16 : &KUtf8, TheCmdLineParams.iPageSize);
	TheTest.Next(TheTestTitle);
	TRAP(err, WholeWriteTestL());
	TEST2(err, KErrNone);
	
	TheTestTitle.Format(_L("@SYMTestCaseID:SYSLIB-SQL-UT-4118 SQL, transaction whole BLOB write, performance tests, encoding: \"%S\", page size: %d\r\n"), 
			TheCmdLineParams.iDbEncoding == TCmdLineParams::EDbUtf16 ? &KUtf16 : &KUtf8, TheCmdLineParams.iPageSize);
	TheTest.Next(TheTestTitle);
	TRAP(err, TransWholeWriteTestL());
	TEST2(err, KErrNone);
	
	TheTestTitle.Format(_L("@SYMTestCaseID:SYSLIB-SQL-UT-4119 SQL, whole BLOB read, performance tests, encoding: \"%S\", page size: %d\r\n"), 
			TheCmdLineParams.iDbEncoding == TCmdLineParams::EDbUtf16 ? &KUtf16 : &KUtf8, TheCmdLineParams.iPageSize);
	TheTest.Next(TheTestTitle);
	TRAP(err, WholeReadTestL());
	TEST2(err, KErrNone);
	
#endif//!defined __WINS__ && !defined __WINSCW__ && !defined _DEBUG
	}

TInt E32Main()
	{
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);

	__UHEAP_MARK;

	GetCmdLineParamsAndSqlConfigString(TheTest, _L("t_sqlperformance2"), TheCmdLineParams, TheSqlConfigString);
	_LIT(KDbName, "c:\\test\\t_sqlperformance2.db");
	PrepareDbName(KDbName, TheCmdLineParams.iDriveName, TheDbFileName);

	TheTest.Printf(_L("==Databases: %S\r\n"), &TheDbFileName); 
	
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
	

