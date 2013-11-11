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
// Description:
//
#include <e32test.h>

#ifdef _SQLPROFILER

#include <bautils.h>
#include "FileBuf64.h"

///////////////////////////////////////////////////////////////////////////////////////

TBool TheOsCallTimeDetailedProfileEnabled = ETrue;//Needed because the RFileBuf64 source is included directly into this test
												  //nd the sql profiler is enabled (_SQLPROFILER is defined in the MMP file)

static RTest TheTest(_L("t_sqlfilebuf64 test"));
static RFs   TheFs;

_LIT(KTestDir, "c:\\test\\");
_LIT(KTestFile, "c:\\test\\t_sqlfilebuf64.bin");
_LIT(KTestFile2, "\\test\\t_sqlfilebuf64_2.bin");
_LIT(KTestFile3, "c:\\test\\t_sqlfilebuf64_3.bin");

static TBuf8<1024> TheBuf;
static TFileName TheDbName;

static TInt TheProcessHandleCount = 0;
static TInt TheThreadHandleCount = 0;
static TInt TheAllocatedCellsCount = 0;

#ifdef _DEBUG
static const TInt KBurstRate = 100;
#endif

enum TOomTestType
	{
	EOomCreateTest,	
	EOomOpenTest,	
	EOomTempTest
	};

//Used in read/write OOM tests
const TUint8 KChar = 'A';
const TInt KPageSize = 32768;

///////////////////////////////////////////////////////////////////////////////////////

void DeleteTestFiles()
	{
	if(TheDbName.Length() > 0)
		{
		(void)TheFs.Delete(TheDbName);
		}
	(void)TheFs.Delete(KTestFile3);
	(void)TheFs.Delete(KTestFile);
	}

void TestEnvDestroy()
	{
	DeleteTestFiles();
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
		RDebug::Print(_L("*** Line %d\r\n"), aLine);
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		TestEnvDestroy();
		RDebug::Print(_L("*** Line %d, Expected result: %d, got: %d\r\n"), aLine, aExpected, aValue);
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

	err = TheFs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

///////////////////////////////////////////////////////////////////////////////////////

static void MarkHandles()
	{
	RThread().HandleCount(TheProcessHandleCount, TheThreadHandleCount);
	}

static void MarkAllocatedCells()
	{
	TheAllocatedCellsCount = User::CountAllocCells();
	}

static void CheckAllocatedCells()
	{
	TInt allocatedCellsCount = User::CountAllocCells();
	TEST2(allocatedCellsCount, TheAllocatedCellsCount);
	}

static void CheckHandles()
	{
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	TEST2(TheProcessHandleCount, endProcessHandleCount);
	TEST2(TheThreadHandleCount, endThreadHandleCount);
	}

static void VerifyFileContent(const TDesC8& aPattern)
	{
	TheBuf.Zero();
	
	RFile64 file;
	TInt err = file.Open(TheFs, KTestFile, EFileShareReadersOrWriters);
	TEST2(err, KErrNone);

	TInt64 fsize;
	err = file.Size(fsize);
	TEST2(err, KErrNone);
	TEST2((TInt)fsize, aPattern.Length());
	
	err = file.Read(TheBuf, aPattern.Length());
	TEST2(err, KErrNone);
	
	file.Close();
	
	err = TheBuf.Compare(aPattern);
	TEST2(err, 0);
	}

static void VerifyFileContent(const TDesC8& aPattern, TInt64 aFilePos)
	{
	__ASSERT_DEBUG(aFilePos >= 0, User::Invariant());
	
	TheBuf.Zero();
	
	RFile64 file;
	TInt err = file.Open(TheFs, KTestFile, EFileShareReadersOrWriters);
	TEST2(err, KErrNone);

	err = file.Read(aFilePos, TheBuf, aPattern.Length());
	TEST2(err, KErrNone);
	
	file.Close();
	
	err = TheBuf.Compare(aPattern);
	TEST2(err, 0);
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4132
@SYMTestCaseDesc		RFileBuf64 write test 1.
						The test performs file write operations using RFileBuf64 class.
						The write positions are inside the buffer or right at the end of the buffer.
						The purpose of the test: to verify the logic of RFileBuf64::Write().
@SYMTestActions			RFileBuf64 write test 1.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMREQ					REQ12106
                        REQ12109
*/
void WriteTest1()
	{
	RFileBuf64 fbuf(1024);
	TInt err = fbuf.Create(TheFs, KTestFile, EFileWrite);
	TEST2(err, KErrNone); 
    fbuf.ProfilerReset();
    
    //Zero write request
	err = fbuf.Write(0, _L8(""));
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileWriteCount, 0);
	TEST2(fbuf.iFileWriteAmount, 0);
	TEST2(fbuf.iFileSizeCount, 0);

	//First write operation. After the operation the file buffer must countain 10 bytes.
	err = fbuf.Write(0, _L8("A123456789"));
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileWriteCount, 0);
	TEST2(fbuf.iFileWriteAmount, 0);
	TEST2(fbuf.iFileSizeCount, 1);

	//Second write operation. The offset is at the middle of the buffer.  Data length: 10;
	err = fbuf.Write(5, _L8("ZZZZZEEEEE"));
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileWriteCount, 0);
	TEST2(fbuf.iFileWriteAmount, 0);

	//Third write operation. The offset is at the end of the buffer.  Data length: 5;
	err = fbuf.Write(15, _L8("CCCCC"));
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileWriteCount, 0);
	TEST2(fbuf.iFileWriteAmount, 0);

	err = fbuf.Flush();
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileWriteCount, 1);
	TEST2(fbuf.iFileFlushCount, 1);
	TEST2(fbuf.iFileWriteAmount, 20);
	TEST2(fbuf.iFileSizeCount, 1);

	fbuf.Close();
	
	VerifyFileContent(_L8("A1234ZZZZZEEEEECCCCC"));

	(void)TheFs.Delete(KTestFile);
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4133
@SYMTestCaseDesc		RFileBuf64 write test 2.
						The test performs file write operations using RFileBuf64 class.
						The write positions are beyound the end of the file but within the buffer capacity.
						The purpose of the test: to verify the logic of RFileBuf64::Write().
@SYMTestActions			RFileBuf64 write test 2.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMREQ					REQ12106
                        REQ12109
*/
void WriteTest2()
	{
	RFileBuf64 fbuf(1024);
	TInt err = fbuf.Create(TheFs, KTestFile, EFileWrite);
	TEST2(err, KErrNone); 
    fbuf.ProfilerReset();

	//First write operation. After the operation the file buffer must countain 10 bytes.
	err = fbuf.Write(0, _L8("A123456789"));
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileWriteCount, 0);
	TEST2(fbuf.iFileWriteAmount, 0);
	TEST2(fbuf.iFileSizeCount, 1);

	//Second write operation. After the operation the file buffer must countain 10 + 10 zeros + 10 bytes.
	err = fbuf.Write(20, _L8("FFGGHHJJKK"));
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileWriteCount, 0);
	TEST2(fbuf.iFileWriteAmount, 0);
	TEST2(fbuf.iFileSizeCount, 1);

	err = fbuf.Flush();
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileWriteCount, 1);
	TEST2(fbuf.iFileFlushCount, 1);
	TEST2(fbuf.iFileWriteAmount, 30);
	TEST2(fbuf.iFileSizeCount, 1);

	fbuf.Close();
	
	TBuf8<30> pattern;
	pattern.Append(_L8("A123456789"));
	pattern.AppendFill(TChar(0), 10);
	pattern.Append(_L8("FFGGHHJJKK"));
	VerifyFileContent(pattern);
	
	(void)TheFs.Delete(KTestFile);
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4134
@SYMTestCaseDesc		RFileBuf64 write test 3.
						The test performs file write operations using RFileBuf64 class.
						The write position is before the start of the buffer but there is room for move.
						The purpose of the test: to verify the logic of RFileBuf64::Write().
@SYMTestActions			RFileBuf64 write test 3.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMREQ					REQ12106
                        REQ12109
*/
void WriteTest3()
	{
	//Iteration 1: The file length is 0, the first operation is "write beyond the end"
	//Iteration 2: The file length is 30, the first write operation is within the file.
	for(TInt i=0;i<2;++i)
		{
		RFileBuf64 fbuf(1024);
		TInt err = i == 0 ? fbuf.Create(TheFs, KTestFile, EFileWrite) : fbuf.Open(TheFs, KTestFile, EFileWrite);
		TEST2(err, KErrNone); 
	    fbuf.ProfilerReset();

		//First write operation. The offset is not 0.  Data length: 10;
		err = fbuf.Write(20, _L8("A123456789"));
		TEST2(err, KErrNone); 
		TEST2(fbuf.iFileWriteCount, 0);
		TEST2(fbuf.iFileWriteAmount, 0);
		TEST2(fbuf.iFileSizeCount, 1);
		
		//Second write operation. The offset is 0.  Data length: 20;
		err = fbuf.Write(0, _L8("AASSDDFFRR**********"));
		TEST2(err, KErrNone); 
		TEST2(fbuf.iFileWriteCount, 0);
		TEST2(fbuf.iFileWriteAmount, 0);
		TEST2(fbuf.iFileSizeCount, 1);

		err = fbuf.Flush();
		TEST2(err, KErrNone); 
		TEST2(fbuf.iFileWriteCount, 1);
		TEST2(fbuf.iFileFlushCount, 1);
		TEST2(fbuf.iFileWriteAmount, 30);
		TEST2(fbuf.iFileSizeCount, 1);

		fbuf.Close();

		VerifyFileContent(_L8("AASSDDFFRR**********A123456789"));
		}
	(void)TheFs.Delete(KTestFile);
	}	

/**
@SYMTestCaseID			PDS-SQL-UT-4135
@SYMTestCaseDesc		RFileBuf64 write test 4.
						The test performs file write operations using RFileBuf64 class and verifies that
						that the pending write data will be stored in the file when the buffer is closed.
						The purpose of the test: to verify the logic of RFileBuf64::Write().
@SYMTestActions			RFileBuf64 write test 4.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMREQ					REQ12106
                        REQ12109
*/
void WriteTest4()
	{
	RFileBuf64 fbuf(1024);
	TInt err = fbuf.Create(TheFs, KTestFile, EFileWrite);
	TEST2(err, KErrNone); 
    fbuf.ProfilerReset();

	// Data length: 10;
	err = fbuf.Write(0, _L8("A123456789"));
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileWriteCount, 0);
	TEST2(fbuf.iFileWriteAmount, 0);
	TEST2(fbuf.iFileSizeCount, 1);

	// Data length: 0;
	err = fbuf.Write(10, _L8(""));
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileWriteCount, 0);
	TEST2(fbuf.iFileWriteAmount, 0);
	TEST2(fbuf.iFileSizeCount, 1);

	fbuf.Close();
	
	VerifyFileContent(_L8("A123456789"));
	
	(void)TheFs.Delete(KTestFile);
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4136
@SYMTestCaseDesc		RFileBuf64 write test 5.
						The test performs file write operations using RFileBuf64 class.
						The data is written before the start of the file buffer and is too big to fit in the buffer.
						The purpose of the test: to verify the logic of RFileBuf64::Write().
@SYMTestActions			RFileBuf64 write test 5.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMREQ					REQ12106
                        REQ12109
*/
void WriteTest5()
	{
	RFileBuf64 fbuf(20);
	TInt err = fbuf.Create(TheFs, KTestFile, EFileWrite);
	TEST2(err, KErrNone); 
    fbuf.ProfilerReset();

	//First write operation. The offset is not 0.  Data length: 10;
	err = fbuf.Write(10, _L8("A123456789"));
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileWriteCount, 0);
	TEST2(fbuf.iFileWriteAmount, 0);
	TEST2(fbuf.iFileSizeCount, 1);

	//Second write operation. The offset is 0.  Data length: 12, i.e. within the buffer - should have no write to the disk.
	err = fbuf.Write(0, _L8("ZZXXCCVVBBNN"));
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileWriteCount, 0);
	TEST2(fbuf.iFileWriteAmount, 0);
	TEST2(fbuf.iFileSizeCount, 1);
	
	//Third write operation. The offet is 18. Data length: 5. The buffer should be written out to the file
	// after "ab" is appended to the buffer. The new buffe after being emptied should have data "cde".
	err = fbuf.Write(18, _L8("abcde"));
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileWriteCount, 1);
	TEST2(fbuf.iFileWriteAmount, 20);
	TEST2(fbuf.iFileSizeCount, 1);

	err = fbuf.Flush();
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileWriteCount, 2);
	TEST2(fbuf.iFileFlushCount, 1);
	TEST2(fbuf.iFileWriteAmount, 23);
	TEST2(fbuf.iFileSizeCount, 1);

	fbuf.Close();

	VerifyFileContent(_L8("ZZXXCCVVBBNN234567abcde"));
	
	(void)TheFs.Delete(KTestFile);
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4137
@SYMTestCaseDesc		RFileBuf64 write test 6.
						The test performs file write operations using RFileBuf64 class.
						The data is written before the start of the file buffer and is too big to fit in the buffer.
						The purpose of the test: to verify the logic of RFileBuf64::Write().
@SYMTestActions			RFileBuf64 write test 6.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMREQ					REQ12106
                        REQ12109
*/
void WriteTest6()
	{
	RFileBuf64 fbuf(20);
	TInt err = fbuf.Create(TheFs, KTestFile, EFileWrite);
	TEST2(err, KErrNone); 

	err = fbuf.Write(0, _L8("A123456789B123456789C123456789"));
	TEST2(err, KErrNone); 
	err = fbuf.Flush();
	TEST2(err, KErrNone); 
	fbuf.Close();
	VerifyFileContent(_L8("A123456789B123456789C123456789"));

	err = fbuf.Open(TheFs, KTestFile, EFileWrite);
	TEST2(err, KErrNone); 
    fbuf.ProfilerReset();

	//First write operation. The offset is not 0. Data length: 10;
	err = fbuf.Write(15, _L8("OOOOOOOOOO"));
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileWriteCount, 0);
	TEST2(fbuf.iFileWriteAmount, 0);
	TEST2(fbuf.iFileSizeCount, 1);

	//Second write operation. The offset is 0. Data length: 15;
	err = fbuf.Write(0, _L8("TTTTTTTTTTTTTTT"));
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileWriteCount, 1);
	TEST2(fbuf.iFileWriteAmount, 10);
	TEST2(fbuf.iFileSizeCount, 1);

	err = fbuf.Flush();
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileWriteCount, 2);
	TEST2(fbuf.iFileFlushCount, 1);
	TEST2(fbuf.iFileWriteAmount, 15 + 10);
	TEST2(fbuf.iFileSizeCount, 1);

	fbuf.Close();

	VerifyFileContent(_L8("TTTTTTTTTTTTTTTOOOOOOOOOO56789"));
	
	(void)TheFs.Delete(KTestFile);
	}

void PrepareReadTest()
	{
	RFile64 file;
	TInt err = file.Create(TheFs, KTestFile, EFileWrite);
	TEST2(err, KErrNone); 
	err = file.Write(_L8("A123456789ZZZZZZZZZZB-B-B-B-B-Y*Y*Y*Y*Y*"));
	TEST2(err, KErrNone); 
	err = file.Flush();
	TEST2(err, KErrNone); 
	file.Close();
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4138
@SYMTestCaseDesc		RFileBuf64 read test 1.
						The test performs file read operations using RFileBuf64 class.
						Tested "read" operations:
							- Zero max length request;
							- Too big read request;
							- Read beyond the end of the file;
						The purpose of the test: to verify the logic of RFileBuf64::Read().
@SYMTestActions			RFileBuf64 read test 1.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMREQ					REQ12106
                        REQ12109
*/
void ReadTest1()
	{
	const TInt KBufMaxSize = 20;// This is half the file size
	RFileBuf64 fbuf(KBufMaxSize);
	TInt err = fbuf.Open(TheFs, KTestFile, EFileWrite | EFileRead | EFileShareReadersOrWriters);
	TEST2(err, KErrNone);
    fbuf.ProfilerReset();
    
	//Zero max length request
    TUint8 buf1[1];
	TPtr8 ptr1(buf1, 0);
	err = fbuf.Read(0, ptr1);
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileReadCount, 0);
	TEST2(fbuf.iFileReadAmount, 0);
	TEST2(fbuf.iFileSizeCount, 0);

	//Too big request
	TBuf8<KBufMaxSize * 2> buf2;
	err = fbuf.Read(0, buf2);
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileReadCount, 1);
	TEST2(fbuf.iFileReadAmount, (KBufMaxSize * 2));
	TEST2(fbuf.iFileSizeCount, 1);
	VerifyFileContent(buf2);
	
	//Read beyond the end of the file
	err = fbuf.Read(2000, buf2);
	TEST2(err, KErrNone); 
	TEST2(buf2.Length(), 0); 

	//Write "5678" in the buffer, pos [4..8)
	err = fbuf.Write(4, _L8("5678"));
	TEST2(err, KErrNone); 

	//Too big request. There are pending data in the buffer.
	TBuf8<KBufMaxSize + 2> buf3;
	err = fbuf.Read(1, buf3);
	TEST2(err, KErrNone); 
	VerifyFileContent(buf3, 1);

	//Read from a non-zero file position to move the buffer start pos. The cached file pos will be 35 at the end.
	TBuf8<5> buf4;
	err = fbuf.Read(30, buf4);
	TEST2(err, KErrNone); 
	VerifyFileContent(buf4, 30);
	err = fbuf.Read(35, buf4);
	TEST2(err, KErrNone); 
	VerifyFileContent(buf4, 35);

	//Too big request. No pending data in the buffer. The file read pos is before the position of the cached data in the buffer.  
	err = fbuf.Read(10, buf3);
	TEST2(err, KErrNone); 
	VerifyFileContent(buf3, 10);
	
	fbuf.Close();
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4139
@SYMTestCaseDesc		RFileBuf64 read test 2.
						The test performs file read operations using RFileBuf64 class.
						Tested operations:
							- Non-buffered reads;
							- Buffered reads;
						The purpose of the test: to verify the logic of RFileBuf64::Read().
@SYMTestActions			RFileBuf64 read test 2.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMREQ					REQ12106
                        REQ12109
*/
void ReadTest2()
	{
	RFileBuf64 fbuf(1024);
	TInt err = fbuf.Open(TheFs, KTestFile, EFileWrite | EFileRead | EFileShareReadersOrWriters);
	TEST2(err, KErrNone); 
    fbuf.ProfilerReset();
    
    //1. Read bytes [0..20]
    TBuf8<20> buf1;
	err = fbuf.Read(0, buf1);
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileReadCount, 1);
	TEST2(fbuf.iFileReadAmount, 20);
	TEST2(fbuf.iFileSizeCount, 1);
    fbuf.ProfilerReset();
	VerifyFileContent(buf1, 0);
    
    //2. Read again, bytes [10..20]. They are not buffered.
    TBuf8<10> buf2;
	err = fbuf.Read(10, buf2);
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileReadCount, 1);
	TEST2(fbuf.iFileReadAmount, 10);
	TEST2(fbuf.iFileSizeCount, 0);
    fbuf.ProfilerReset();
	VerifyFileContent(buf2, 10);

    //3. Read again, bytes [20..30]. They are not buffered. But the file buffer will be populated, 
    //   because the file read position matches the guessed file read position from step 2.
	err = fbuf.Read(20, buf2);
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileReadCount, 1);
	TEST2(fbuf.iFileReadAmount, (10 * 2));
	TEST2(fbuf.iFileSizeCount, 0);
    fbuf.ProfilerReset();
	VerifyFileContent(buf2, 20);

	//4. Read again, bytes [25..35]. This is a buffered read operation.
	err = fbuf.Read(25, buf2);
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileReadCount, 0);
	TEST2(fbuf.iFileReadAmount, 0);
	TEST2(fbuf.iFileSizeCount, 0);
    fbuf.ProfilerReset();
	VerifyFileContent(buf2, 25);
	
	//5. Read again, bytes [15..25]. This is a non buffered read operation.
	err = fbuf.Read(15, buf2);
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileReadCount, 1);
	TEST2(fbuf.iFileReadAmount, 10);
	TEST2(fbuf.iFileSizeCount, 0);
    fbuf.ProfilerReset();
	VerifyFileContent(buf2, 15);

    //6. Read again, bytes [25..35]. This is a buffered read operation. The buffer from step 3 is still there.
	err = fbuf.Read(25, buf2);
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileReadCount, 0);
	TEST2(fbuf.iFileReadAmount, 0);
	TEST2(fbuf.iFileSizeCount, 0);
    fbuf.ProfilerReset();
	VerifyFileContent(buf2, 25);
	
    //7. Read again, bytes [35..45] - beyond the end of the file. 
    //   This is a buffered read operation. The buffer from step 3 is still there.
	err = fbuf.Read(35, buf2);
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileReadCount, 0);
	TEST2(fbuf.iFileReadAmount, 0);
	TEST2(fbuf.iFileSizeCount, 0);
	TEST2(buf2.Size(), 5);
    fbuf.ProfilerReset();
	VerifyFileContent(buf2, 35);
	
	fbuf.Close();
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4140
@SYMTestCaseDesc		RFileBuf64 read test 3.
						The test performs file read operations using RFileBuf64 class.
						Tested operations:
							- Non-buffered reads;
							- Buffered reads;
							- Part- buffered reads;
						The purpose of the test: to verify the logic of RFileBuf64::Read().
@SYMTestActions			RFileBuf64 read test 3.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMREQ					REQ12106
                        REQ12109
*/
void ReadTest3()
	{
	RFileBuf64 fbuf(1024);
	TInt err = fbuf.Open(TheFs, KTestFile, EFileWrite | EFileRead | EFileShareReadersOrWriters);
	TEST2(err, KErrNone); 
    fbuf.ProfilerReset();
    
    //1. Read bytes [0..10]. Non buffered.
    TBuf8<10> buf1;
	err = fbuf.Read(0, buf1);
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileReadCount, 1);
	TEST2(fbuf.iFileReadAmount, 10);
	TEST2(fbuf.iFileSizeCount, 1);
    fbuf.ProfilerReset();
	VerifyFileContent(buf1, 0);

    //2. Read bytes [10..20]. Non buffered. But the file buffer is populated, bytes [10..40].
	err = fbuf.Read(10, buf1);
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileReadCount, 1);
	TEST2(fbuf.iFileReadAmount, 30);
	TEST2(fbuf.iFileSizeCount, 0);
    fbuf.ProfilerReset();
	VerifyFileContent(buf1, 10);
	
    //3. Read bytes [25..35]. Buffered. Because the previous operation [2] performed a read-ahead operation.
	err = fbuf.Read(25, buf1);
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileReadCount, 0);
	TEST2(fbuf.iFileReadAmount, 0);
	TEST2(fbuf.iFileSizeCount, 0);
    fbuf.ProfilerReset();
	VerifyFileContent(buf1, 25);

    //4. Write bytes [20..30]. Buffered. Read buffer is gone, the file buffer contains the [20..30] file area.
	err = fbuf.Write(20, _L8("IIIIIQQQQQ"));
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileReadCount, 0);
	TEST2(fbuf.iFileReadAmount, 0);
	TEST2(fbuf.iFileWriteCount, 0);
	TEST2(fbuf.iFileWriteAmount, 0);
	TEST2(fbuf.iFileSizeCount, 0);
    fbuf.ProfilerReset();

    //5. Read bytes [25..35]. Part-buffered. Part of pending writes picked up. Then the buffer is flushed.
	err = fbuf.Read(25, buf1);
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileReadCount, 1);
	TEST2(fbuf.iFileReadAmount, 5);
	TEST2(fbuf.iFileWriteCount, 1);
	TEST2(fbuf.iFileWriteAmount, 10);
	TEST2(fbuf.iFileSizeCount, 0);
	
    fbuf.ProfilerReset();
    err = fbuf.Flush();
    TEST2(err, KErrNone);
    
    //All cached data should have been written to the file before the Flush() call.
	TEST2(fbuf.iFileReadCount, 0);
	TEST2(fbuf.iFileReadAmount, 0);
	TEST2(fbuf.iFileWriteCount, 0);
	TEST2(fbuf.iFileWriteAmount, 0);
	TEST2(fbuf.iFileSizeCount, 0);
	TEST2(fbuf.iFileFlushCount, 1);
    
    fbuf.ProfilerReset();
	VerifyFileContent(buf1, 25);

    //6. The buffer is empty after the last flush. Write bytes [0..10]. The file buffer contains the [0..10] file area.
	err = fbuf.Write(0, _L8("PPOOIIUUYY"));
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileReadCount, 0);
	TEST2(fbuf.iFileReadAmount, 0);
	TEST2(fbuf.iFileWriteCount, 0);
	TEST2(fbuf.iFileWriteAmount, 0);
	TEST2(fbuf.iFileSizeCount, 0);
    fbuf.ProfilerReset();
	
    //7. Read bytes [5..15]. Part buffered. Pending writes picked up. The content is written to the file.
	err = fbuf.Read(5, buf1);
	TEST2(err, KErrNone); 
	TEST2(fbuf.iFileReadCount, 1);
	TEST2(fbuf.iFileReadAmount, 5);
	TEST2(fbuf.iFileWriteCount, 1);
	TEST2(fbuf.iFileWriteAmount, 10);
	TEST2(fbuf.iFileSizeCount, 0);
    fbuf.ProfilerReset();
	VerifyFileContent(buf1, 5);
	
	fbuf.Close();
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4141
@SYMTestCaseDesc		RFileBuf64::SetReadAheadSize() test.
						The test iterates over all existing drives.
						For each R/W drive a test file is created using RFileBuf64 class.
						Then the test collects information regarding the block size, cluster size and 
						read buffer size and calls RFileBuf64::SetReadAheadSize() with these parameters
						to check how the read-ahead buffer size will be recalculated.
@SYMTestActions			RFileBuf64::SetReadAheadSize() test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMREQ					REQ12106
                        REQ12109
*/
void SetReadAheadSizeTest()
	{
	TheTest.Printf(_L("==================\r\n"));
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
	
	for(TInt drive=EDriveA;drive<=EDriveZ;++drive)
		{
		TDriveInfo driveInfo;
		TInt err = TheFs.Drive(driveInfo, drive);
		if(err == KErrNone)
			{
			TVolumeInfo vinfo;
			err = TheFs.Volume(vinfo, drive);
			if(err == KErrNone)
				{
				TVolumeIOParamInfo vparam;
				err = TheFs.VolumeIOParam(drive, vparam);
				TEST2(err, KErrNone);
				TBuf8<128> vinfoex8;
				err = TheFs.QueryVolumeInfoExt(drive, EFileSystemSubType, vinfoex8);
				TEST2(err, KErrNone);
				TPtrC vinfoex((const TUint16*)(vinfoex8.Ptr() + 8), vinfoex8[0]);
				TPtrC KMediaTypeNames[] = {KType1(), KType2(), KType3(), KType4(), KType5(), KType6(), KType7(), KType8(), KType9(), KType10(), KType11()};
				TheTest.Printf(_L("Drive: %C:, Type: %16.16S, File System: %8.8S, Size: %d Mb.\r\n"), 'A' + drive, &KMediaTypeNames[driveInfo.iType], &vinfoex, (TInt)(vinfo.iSize / (1024 * 1024)));
				TheTest.Printf(_L("            Size: %ld bytes.\r\n"), vinfo.iSize);
				TheTest.Printf(_L("       Block size=%d, Cluster size=%d, Read buffer size=%d.\r\n"), vparam.iBlockSize, vparam.iClusterSize, vparam.iRecReadBufSize);
				if(driveInfo.iType == EMediaRam || driveInfo.iType == EMediaHardDisk || driveInfo.iType == EMediaFlash || driveInfo.iType == EMediaNANDFlash)
				  	{
					TDriveUnit drvUnit(drive);
					TDriveName drvName = drvUnit.Name();
					TParse parse;
					parse.Set(KTestFile2, &drvName, NULL);
					TheDbName.Copy(parse.FullName());
					TRAP(err, BaflUtils::EnsurePathExistsL(TheFs, TheDbName));
					if(err == KErrNone || err == KErrAlreadyExists)
						{
						(void)TheFs.Delete(TheDbName);
						RFileBuf64 fbuf64(8 * 1024);
						err = fbuf64.Create(TheFs, TheDbName, EFileRead | EFileWrite);
						TEST2(err, KErrNone);
						TInt readAhead = fbuf64.SetReadAheadSize(vparam.iBlockSize, vparam.iRecReadBufSize);
						TheTest.Printf(_L("       Read-ahead size=%d.\r\n"), readAhead);
						fbuf64.Close();
						(void)TheFs.Delete(TheDbName);
						}
					else
						{
						TheTest.Printf(_L("Drive %C. BaflUtils::EnsurePathExistsL() has failed with err=%d.\r\n"), 'A' + drive, err);	
						}
					}
				}
			else
				{
				TheTest.Printf(_L("Drive %C. RFs::Volume() has failed with err=%d.\r\n"), 'A' + drive, err);	
				}
			}
		else
			{
			TheTest.Printf(_L("Drive %C. RFs::Drive() has failed with err=%d.\r\n"), 'A' + drive, err);	
			}
		}
	TheTest.Printf(_L("==================\r\n"));
	//
	RFileBuf64 fbuf64(8 * 1024);//buffer capacity = 8Kb
	
	//"ReadRecBufSize" defined and is power of two, the "BlockSize" is also defined and is power of two
	TInt err2 = fbuf64.Create(TheFs, TheDbName, EFileRead | EFileWrite);
	TEST2(err2, KErrNone);
	TInt blockSize = 4096; TInt readRecBufSize = 2048;
	TInt readAhead2 = fbuf64.SetReadAheadSize(blockSize, readRecBufSize);
	TEST2(readAhead2, readRecBufSize);
	fbuf64.Close();
	
	//"ReadRecBufSize" defined and is power of two but is less than the default read-ahead value
	err2 = fbuf64.Open(TheFs, TheDbName, EFileRead | EFileWrite);
	TEST2(err2, KErrNone);
	blockSize = 0; readRecBufSize = 128;
	readAhead2 = fbuf64.SetReadAheadSize(blockSize, readRecBufSize);
	TEST2(readAhead2, RFileBuf64::KDefaultReadAheadSize);
	fbuf64.Close();
	
	//"ReadRecBufSize" defined and is power of two but is bigger than the buffer capacity
	err2 = fbuf64.Open(TheFs, TheDbName, EFileRead | EFileWrite);
	TEST2(err2, KErrNone);
	blockSize = -10; readRecBufSize = fbuf64.iCapacity * 2;
	readAhead2 = fbuf64.SetReadAheadSize(blockSize, readRecBufSize);
	TEST2(readAhead2, fbuf64.iCapacity);
	fbuf64.Close();
	
	//"ReadRecBufSize" defined but is not power of two, "BlockSize" defined but is less than the default read-ahead value
	err2 = fbuf64.Open(TheFs, TheDbName, EFileRead | EFileWrite);
	TEST2(err2, KErrNone);
	blockSize = 512; readRecBufSize = 4000;
	readAhead2 = fbuf64.SetReadAheadSize(blockSize, readRecBufSize);
	TEST2(readAhead2, RFileBuf64::KDefaultReadAheadSize);
	fbuf64.Close();
	
	//"ReadRecBufSize" defined but is not power of two, "BlockSize" defined and is bigger than the default read-ahead value
	err2 = fbuf64.Open(TheFs, TheDbName, EFileRead | EFileWrite);
	TEST2(err2, KErrNone);
	blockSize = 4096; readRecBufSize = 4000;
	readAhead2 = fbuf64.SetReadAheadSize(blockSize, readRecBufSize);
	TEST2(readAhead2, blockSize);
	fbuf64.Close();

	//"ReadRecBufSize" defined but is not power of two, "BlockSize" defined and is bigger than the buffer capacity
	err2 = fbuf64.Open(TheFs, TheDbName, EFileRead | EFileWrite);
	TEST2(err2, KErrNone);
	blockSize = fbuf64.iCapacity * 2; readRecBufSize = 1;
	readAhead2 = fbuf64.SetReadAheadSize(blockSize, readRecBufSize);
	TEST2(readAhead2, fbuf64.iCapacity);
	fbuf64.Close();
	
	//"ReadRecBufSize" negative, "BlockSize" defined but is not power of two
	err2 = fbuf64.Open(TheFs, TheDbName, EFileRead | EFileWrite);
	TEST2(err2, KErrNone);
	blockSize = 1000; readRecBufSize = -2;
	readAhead2 = fbuf64.SetReadAheadSize(blockSize, readRecBufSize);
	TEST2(readAhead2, RFileBuf64::KDefaultReadAheadSize);
	fbuf64.Close();
	
	//"ReadRecBufSize" negative, "BlockSize" negative
	err2 = fbuf64.Open(TheFs, TheDbName, EFileRead | EFileWrite);
	TEST2(err2, KErrNone);
	blockSize = -1; readRecBufSize = -2;
	readAhead2 = fbuf64.SetReadAheadSize(blockSize, readRecBufSize);
	TEST2(readAhead2, RFileBuf64::KDefaultReadAheadSize);
	fbuf64.Close();
	//
	(void)TheFs.Delete(TheDbName);
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4142
@SYMTestCaseDesc		RFileBuf64 OOM test.
						The test calls RFileBuf64:Create(), RFileBuf64:Open() and RFileBuf64:Temp() in an OOM
						simulation loop and verifies that no memory is leaked.
@SYMTestActions			RFileBuf64 OOM test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMREQ					REQ12106
                        REQ12109
*/
void OomTest(TOomTestType aOomTestType)
	{
	(void)TheFs.Delete(KTestFile);
	
	if(aOomTestType == EOomOpenTest)
		{
		RFile64 file;
		TInt err2 = file.Create(TheFs, KTestFile, EFileWrite | EFileRead);	
		file.Close();
		TEST2(err2, KErrNone);
		}
	
	TFileName tmpFileName;
	TInt err = KErrNoMemory;
	TInt failingAllocationNo = 0;
	RFileBuf64 fbuf(1024);
	TheTest.Printf(_L("Iteration:\r\n"));
	while(err == KErrNoMemory)
		{
		TheTest.Printf(_L(" %d"), ++failingAllocationNo);
		
		MarkHandles();
		MarkAllocatedCells();
		
		__UHEAP_MARK;
		__UHEAP_SETBURSTFAIL(RAllocator::EBurstFailNext, failingAllocationNo, KBurstRate);
		
		switch(aOomTestType)
			{
			case EOomCreateTest:
				err = fbuf.Create(TheFs, KTestFile, EFileWrite | EFileRead);
				break;
			case EOomOpenTest:
				err = fbuf.Open(TheFs, KTestFile, EFileWrite | EFileRead);
				break;
			case EOomTempTest:
				{
				err = fbuf.Temp(TheFs, KTestDir, tmpFileName, EFileWrite | EFileRead);
				}
				break;
			default:
				TEST(0);
				break;
			}
		fbuf.Close();
		
		__UHEAP_RESET;
		__UHEAP_MARKEND;

		CheckAllocatedCells();
		CheckHandles();
		
		TEntry entry;
		if(err != KErrNoMemory)
			{
			TEST2(err, KErrNone);	
			}
		else if(aOomTestType == EOomCreateTest)
			{
			TInt err2 = TheFs.Entry(KTestFile, entry);
			TEST2(err2, KErrNotFound);
			}
		else if(aOomTestType == EOomTempTest)
			{
			if(tmpFileName.Size() > 0)
				{
				TInt err2 = TheFs.Entry(tmpFileName, entry);
				TEST2(err2, KErrNotFound);
				}
			}
		}
	TEST2(err, KErrNone);
	TheTest.Printf(_L("\r\n=== OOM Test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
	
	if(aOomTestType == EOomTempTest)
		{
		(void)TheFs.Delete(tmpFileName);
		}
	(void)TheFs.Delete(KTestFile);
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4195
@SYMTestCaseDesc		RFileBuf64::Create() file I/O error simulation test.
						The test calls RFileBuf64:Create() in a file I/O error simulation loop.
@SYMTestActions			RFileBuf64::Create() file I/O error simulation test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMDEF					DEF145198
*/
void CreateFileIoErrTest()
	{
    TInt err = KErrGeneral;
    TInt cnt = 0;
    for(;err<KErrNone;++cnt)
        {
        TheTest.Printf(_L("===Iteration %d. Simulated error:\r\n"), cnt);       
        for (TInt fsError=KErrNotFound;fsError>=KErrBadName;--fsError)
            {
            TheTest.Printf(_L("%d "), fsError);
        	__UHEAP_MARK;
            (void)TheFs.SetErrorCondition(fsError, cnt);
        	RFileBuf64 fbuf(1024);//buffer capacity = 1024 bytes
        	err = fbuf.Create(TheFs, KTestFile3, EFileRead | EFileWrite);
            (void)TheFs.SetErrorCondition(KErrNone);
            fbuf.Close();
            __UHEAP_MARKEND;
			TInt err2 = TheFs.Delete(KTestFile3);
			TInt expectedErr = err == KErrNone ? KErrNone : KErrNotFound;
			TEST2(err2, expectedErr);
            }
        TheTest.Printf(_L("\r\n"));
        }
    TheTest.Printf(_L("\r\n===File I/O error simulation test succeeded on iteration %d===\r\n"), cnt);
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4196
@SYMTestCaseDesc		RFileBuf64::Open() file I/O error simulation test.
						The test calls RFileBuf64:Open() in a file I/O error simulation loop.
@SYMTestActions			RFileBuf64::Open() file I/O error simulation test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMDEF					DEF145198
*/
void OpenFileIoErrTest()
	{
	RFileBuf64 fbuf(1024);//buffer capacity = 1024 bytes
	TInt err = fbuf.Create(TheFs, KTestFile3, EFileRead | EFileWrite);
	fbuf.Close();
	TEST2(err, KErrNone);
    err = KErrGeneral;
    TInt cnt = 0;
    for(;err<KErrNone;++cnt)
        {
        TheTest.Printf(_L("===Iteration %d. Simulated error:\r\n"), cnt);       
        for (TInt fsError=KErrNotFound;fsError>=KErrBadName;--fsError)
            {
            TheTest.Printf(_L("%d "), fsError);
        	__UHEAP_MARK;
            (void)TheFs.SetErrorCondition(fsError, cnt);
        	err = fbuf.Open(TheFs, KTestFile3, EFileRead | EFileWrite);
            (void)TheFs.SetErrorCondition(KErrNone);
            fbuf.Close();
            __UHEAP_MARKEND;
            }
        TheTest.Printf(_L("\r\n"));
        }
    TheTest.Printf(_L("\r\n===File I/O error simulation test succeeded on iteration %d===\r\n"), cnt);
	(void)TheFs.Delete(KTestFile3);
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4197
@SYMTestCaseDesc		RFileBuf64::Temp() file I/O error simulation test.
						The test calls RFileBuf64:Temp() in a file I/O error simulation loop.
@SYMTestActions			RFileBuf64::temp() file I/O error simulation test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMDEF					DEF145198
*/
void TempFileIoErrTest()
	{
    TInt err = KErrGeneral;
    TInt cnt = 0;
    for(;err<KErrNone;++cnt)
        {
        TheTest.Printf(_L("===Iteration %d. Simulated error:\r\n"), cnt);       
        for (TInt fsError=KErrNotFound;fsError>=KErrBadName;--fsError)
            {
            TheTest.Printf(_L("%d "), fsError);
        	__UHEAP_MARK;
            (void)TheFs.SetErrorCondition(fsError, cnt);
        	RFileBuf64 fbuf(1024);//buffer capacity = 1024 bytes
        	TFileName tmpFileName;
			err = fbuf.Temp(TheFs, KTestDir, tmpFileName, EFileWrite | EFileRead);
            (void)TheFs.SetErrorCondition(KErrNone);
            fbuf.Close();
            __UHEAP_MARKEND;
			TInt err2 = TheFs.Delete(tmpFileName);
			TInt expectedErr = err == KErrNone ? KErrNone : KErrNotFound;
			TEST2(err2, expectedErr);
            }
        TheTest.Printf(_L("\r\n"));
        }
    TheTest.Printf(_L("\r\n===File I/O error simulation test succeeded on iteration %d===\r\n"), cnt);
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4207
@SYMTestCaseDesc		RFileBuf64::Write() OOM test.
						The test calls RFileBuf64:Write() in an OOM
						simulation loop and verifies that no memory is leaked.
						The test also check that RFileBuf::DoSetCapacity() correctly operates in
						"out of memory" situation.
@SYMTestActions			RFileBuf64::Write() OOM test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMDEF					380056
*/
void WriteOomTest()
	{
	HBufC8* databuf = HBufC8::New(KPageSize);
	TEST(databuf != NULL);
	TPtr8 dataptr = databuf->Des();
	dataptr.SetLength(KPageSize);
	dataptr.Fill(TChar(KChar));
	
	TInt err = KErrNoMemory;
	TInt failingAllocationNo = 0;
	TheTest.Printf(_L("Iteration:\r\n"));
	while(err == KErrNoMemory)
		{
		TheTest.Printf(_L(" %d"), ++failingAllocationNo);

		(void)TheFs.Delete(KTestFile);
		
		MarkHandles();
		MarkAllocatedCells();
		
		__UHEAP_MARK;
		__UHEAP_SETBURSTFAIL(RAllocator::EBurstFailNext, failingAllocationNo, KBurstRate);

		const TInt KDefaultBufCapacity = 1024;
		RFileBuf64 fbuf(KDefaultBufCapacity);
		err = fbuf.Create(TheFs, KTestFile, EFileWrite | EFileRead);
		if(err == KErrNone)
			{
			err = fbuf.Write(0LL, dataptr);
			}
		fbuf.Close();
		
		__UHEAP_RESET;
		__UHEAP_MARKEND;

		CheckAllocatedCells();
		CheckHandles();
		}
	TEST2(err, KErrNone);
	RFile64 file;
	err = file.Open(TheFs, KTestFile, EFileRead);
	TEST2(err, KErrNone);
	dataptr.Zero();
	err = file.Read(dataptr);
	TEST2(err, KErrNone);
	file.Close();
	TEST2(dataptr.Length(), KPageSize);
	for(TInt i=0;i<KPageSize;++i)
		{
		TEST(dataptr[i] == KChar);
		}
	TheTest.Printf(_L("\r\n=== OOM Test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
	
	//The file is left undeleted - to be used in ReadOomTest().
	delete databuf;
	}

/**
@SYMTestCaseID			PDS-SQL-UT-4208
@SYMTestCaseDesc		RFileBuf64::Read() OOM test.
						The test calls RFileBuf64:Read() in an OOM
						simulation loop and verifies that no memory is leaked.
						The test also check that RFileBuf::DoSetCapacity() correctly operates in
						"out of memory" situation.
@SYMTestActions			RFileBuf64::Read() OOM test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMDEF					380056
*/
void ReadOomTest()
	{
	HBufC8* databuf = HBufC8::New(KPageSize);
	TEST(databuf != NULL);
	TPtr8 dataptr = databuf->Des();
	
	TInt err = KErrNoMemory;
	TInt failingAllocationNo = 0;
	TheTest.Printf(_L("Iteration:\r\n"));
	while(err == KErrNoMemory)
		{
		TheTest.Printf(_L(" %d"), ++failingAllocationNo);

		MarkHandles();
		MarkAllocatedCells();
		
		__UHEAP_MARK;
		__UHEAP_SETBURSTFAIL(RAllocator::EBurstFailNext, failingAllocationNo, KBurstRate);

		const TInt KDefaultBufCapacity = 1024;
		RFileBuf64 fbuf(KDefaultBufCapacity);
		err = fbuf.Open(TheFs, KTestFile, EFileRead);
		if(err == KErrNone)
			{
			err = fbuf.Read(0LL, dataptr);
			}
		fbuf.Close();
		
		__UHEAP_RESET;
		__UHEAP_MARKEND;

		CheckAllocatedCells();
		CheckHandles();
		}
	TEST2(err, KErrNone);
	RFile64 file;
	err = file.Open(TheFs, KTestFile, EFileRead);
	TEST2(err, KErrNone);
	dataptr.Zero();
	err = file.Read(dataptr);
	TEST2(err, KErrNone);
	file.Close();
	TEST2(dataptr.Length(), KPageSize);
	for(TInt i=0;i<KPageSize;++i)
		{
		TEST(dataptr[i] == KChar);
		}
	TheTest.Printf(_L("\r\n=== OOM Test succeeded at heap failure rate of %d ===\r\n"), failingAllocationNo);
	
	(void)TheFs.Delete(KTestFile);
	delete databuf;
	}

/**
@SYMTestCaseID          PDS-SQL-CT-4212
@SYMTestCaseDesc        RFileBuf64::Write() test.
                        The test performs file write operations using RFileBuf64 class.
                        Teh test sumilates the write operation at the conditions:
                        
                        1. There are 8 pages to be writted to the file.
                        2. Each page is 16 bytes
                        3. The size of RFileBuf64 is 4 pages, i.e. 64 bytes.
                        4. The order of write is not sequential.
 
@SYMTestActions         Write database pages.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority        High
*/
void TestSetSizeCounter()
    {
    const TInt KPageSize = 16;
    const TInt KBufSize = 4 * KPageSize;
    RFileBuf64 fbuf(KBufSize);
    (void)TheFs.Delete(KTestFile);
    TInt err = fbuf.Create(TheFs, KTestFile, EFileRead | EFileWrite);
    TEST2(err, KErrNone); 
    const TInt KMaxPage = 8;
    TUint8 fileData[KMaxPage][KPageSize];
    TPtrC8 pageData[KMaxPage];
    for(TInt i = 0;i <KMaxPage;++i)
        {
        Mem::Fill(fileData[i], KPageSize, TChar('a' + i));
        const TUint8* p = fileData[i]; 
        pageData[i].Set(p, KPageSize);
        }
    //
    fbuf.ProfilerReset();
    //Write the first 4 pages
    for (TInt ii = 0; ii < 4; ii++)
        {
        err = fbuf.Write(ii * KPageSize, pageData[ii]);
        TEST2(err, KErrNone);
        }
    //Write page #2
    err = fbuf.Write(2 * KPageSize, pageData[2]);
    TEST2(err, KErrNone);
    //
    TEST2(fbuf.iFileWriteCount, 0);
    TEST2(fbuf.iFileSetSizeCount, 0);
    //Write pages 5, 4, 6, 7
    err = fbuf.Write(5 * KPageSize, pageData[5]);
    TEST2(err, KErrNone);
    //
    TEST2(fbuf.iFileWriteCount, 1);
    TEST2(fbuf.iFileSetSizeCount, 0);
    //
    err = fbuf.Write(4 * KPageSize, pageData[4]);
    TEST2(err, KErrNone);
    err = fbuf.Write(6 * KPageSize, pageData[6]);
    TEST2(err, KErrNone);
    err = fbuf.Write(7 * KPageSize, pageData[7]);
    TEST2(err, KErrNone);
    //
    TEST2(fbuf.iFileWriteCount, 1);
    TEST2(fbuf.iFileSetSizeCount, 0);
    //
    err = fbuf.Flush();
    TEST2(err, KErrNone);
    //
    TEST2(fbuf.iFileWriteCount, 2);
    TEST2(fbuf.iFileSetSizeCount, 0);
    //
    fbuf.Close();
    (void)TheFs.Delete(KTestFile);
    }

///////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

//Panic thread function. 
//It will cast aData parameter to a TFunctor pointer and call it.
//The expectation is that the called function will panic and kill the panic thread.
TInt ThreadFunc(void* aData)
	{
	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);
	
	User::SetJustInTime(EFalse);	// disable debugger panic handling
	
	TFunctor* obj = reinterpret_cast<TFunctor*> (aData);
	TEST(obj != NULL);
	(*obj)();//call the panic function
	
	delete tc;
	
	return KErrNone;		
	}

//Panic test.
//PanicTest function will create a new thread - panic thread, giving it a pointer to the function which has to
//be executed and the expectation is that the function will panic and kill the panic thread.
//PanicTest function will check the panic thread exit code, exit category and the panic code.
void PanicTest(TFunctor& aFunctor, TExitType aExpectedExitType, const TDesC& aExpectedCategory, TInt aExpectedPanicCode)
	{
	RThread thread;
	_LIT(KThreadName,"SqlFileBufPanicThread");
	TEST2(thread.Create(KThreadName, &ThreadFunc, 0x2000, 0x1000, 0x10000, (void*)&aFunctor, EOwnerThread), KErrNone);
	
	TRequestStatus status;
	thread.Logon(status);
	TEST2(status.Int(), KRequestPending);
	thread.Resume();
	User::WaitForRequest(status);
	User::SetJustInTime(ETrue);	// enable debugger panic handling

	TEST2(thread.ExitType(), aExpectedExitType);
	TEST(thread.ExitCategory() == aExpectedCategory);
	TEST2(thread.ExitReason(), aExpectedPanicCode);
	
	CLOSE_AND_WAIT(thread);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////     Panic test functions    /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Panic when calling RFileBuf64::RFileBuf64() with an invalid buffer capacity value.
class TSqlFileBuf_InvalidCapacity : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RFileBuf64 fbuf(-8192);//panic here - "-8192" - negative buffer capacity
		}
	};
static TSqlFileBuf_InvalidCapacity TheSqlFileBuf_InvalidCapacity;

//Panic when calling RFileBuf64::Create() with an invalid file handle.
class TSqlFileBuf_InvalidFileHandle1 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RFileBuf64 fbuf(8192);
		RFs fs;
		fbuf.Create(fs, _L("aaa.db"), EFileRead);//panic here - invalid file handle
		}
	};
static TSqlFileBuf_InvalidFileHandle1 TheSqlFileBuf_InvalidFileHandle1;

//Panic when calling RFileBuf64::Create() with an invalid file name.
class TSqlFileBuf_InvalidFileName1 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RFileBuf64 fbuf(8192);
		RFs fs;
		TInt err = fs.Connect();
		TEST2(err, KErrNone);
		fbuf.Create(fs, KNullDesC, EFileRead);//panic here - invalid file name
		fs.Close();
		}
	};
static TSqlFileBuf_InvalidFileName1 TheSqlFileBuf_InvalidFileName1;

//Panic when calling RFileBuf64::Open() with an invalid file handle.
class TSqlFileBuf_InvalidFileHandle2 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RFileBuf64 fbuf(8192);
		RFs fs;
		fbuf.Open(fs, _L("aaa.db"), EFileRead);//panic here - invalid file handle
		}
	};
static TSqlFileBuf_InvalidFileHandle2 TheSqlFileBuf_InvalidFileHandle2;

//Panic when calling RFileBuf64::Open() with an invalid file name.
class TSqlFileBuf_InvalidFileName2 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RFileBuf64 fbuf(8192);
		RFs fs;
		TInt err = fs.Connect();
		TEST2(err, KErrNone);
		fbuf.Open(fs, KNullDesC, EFileRead);//panic here - invalid file name
		fs.Close();
		}
	};
static TSqlFileBuf_InvalidFileName2 TheSqlFileBuf_InvalidFileName2;

//Panic when calling RFileBuf64::Temp() with an invalid file handle.
class TSqlFileBuf_InvalidFileHandle3 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RFileBuf64 fbuf(8192);
		RFs fs;
		TFileName fname;
		fbuf.Temp(fs, _L("c:\\test"), fname, EFileRead);//panic here - invalid file handle
		}
	};
static TSqlFileBuf_InvalidFileHandle3 TheSqlFileBuf_InvalidFileHandle3;

//Panic when calling RFileBuf64::AdoptFromClient() with an invalid message handle.
class TSqlFileBuf_InvalidMessageHandle : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RFileBuf64 fbuf(8192);
		RMessage2 msg;
		fbuf.AdoptFromClient(msg, 0, 1);//panic here - invalid message handle
		}
	};
static TSqlFileBuf_InvalidMessageHandle TheSqlFileBuf_InvalidMessageHandle;

//Panic when calling RFileBuf64::Read() with an invalid file position.
class TSqlFileBuf_InvalidReadPos : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RFileBuf64 fbuf(8192);
		TBuf8<50> buf;
		fbuf.Read(-1024, buf);//panic here - invalid file position
		}
	};
static TSqlFileBuf_InvalidReadPos TheSqlFileBuf_InvalidReadPos;

//Panic when calling RFileBuf64::Write() with an invalid file position.
class TSqlFileBuf_InvalidWritePos : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RFileBuf64 fbuf(8192);
		TBuf8<50> buf;
		fbuf.Write(-1024, buf);//panic here - invalid file position
		}
	};
static TSqlFileBuf_InvalidWritePos TheSqlFileBuf_InvalidWritePos;

//Panic when calling RFileBuf64::SetSize() with an invalid file size.
class TSqlFileBuf_InvalidSize : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RFileBuf64 fbuf(8192);
		TBuf8<50> buf;
		fbuf.SetSize(-1024);//panic here - invalid file size
		}
	};
static TSqlFileBuf_InvalidSize TheSqlFileBuf_InvalidSize;

#endif //_DEBUG

/**
@SYMTestCaseID          PDS-SQL-UT-4236
@SYMTestCaseDesc        RFileBuf64 panic test.
						The test runs a thread. The thread will create a RFileBuf64 object
						and put the object in a situation where the file buffer cannot perform
						its duties anymore and will raise a panic. The test verifies that the file
						buffer implementation properly detects anomalities such as bad parameters,
						null handles, etc... 
@SYMTestActions         RFileBuf64 panic test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority        High
*/
void FileBufPanicTest()
	{
#ifdef _DEBUG
	_LIT(KPanicCategory, "FBuf64");
	PanicTest(TheSqlFileBuf_InvalidCapacity, EExitPanic, KPanicCategory, 1);
	PanicTest(TheSqlFileBuf_InvalidFileHandle1, EExitPanic, KPanicCategory, 7);
	PanicTest(TheSqlFileBuf_InvalidFileName1, EExitPanic, KPanicCategory, 10);
	PanicTest(TheSqlFileBuf_InvalidFileHandle2, EExitPanic, KPanicCategory, 7);
	PanicTest(TheSqlFileBuf_InvalidFileName2, EExitPanic, KPanicCategory, 10);
	PanicTest(TheSqlFileBuf_InvalidFileHandle3, EExitPanic, KPanicCategory, 7);
	PanicTest(TheSqlFileBuf_InvalidMessageHandle, EExitPanic, KPanicCategory, 8);
	PanicTest(TheSqlFileBuf_InvalidReadPos, EExitPanic, KPanicCategory, 4);
	PanicTest(TheSqlFileBuf_InvalidWritePos, EExitPanic, KPanicCategory, 4);
	PanicTest(TheSqlFileBuf_InvalidSize, EExitPanic, KPanicCategory, 5);
#else //_DEBUG
	TheTest.Printf(_L("This test can be run in _DEBUG mode only!"));
#endif//_DEBUG
	}

void DoTests()
	{
	TheTest.Start(_L(" @SYMTestCaseID:PDS-SQL-UT-4132 RFileBuf64 write test 1"));
	WriteTest1();
	TheTest.Next( _L(" @SYMTestCaseID:PDS-SQL-UT-4133 RFileBuf64 write test 2"));
	WriteTest2();
	TheTest.Next( _L(" @SYMTestCaseID:PDS-SQL-UT-4134 RFileBuf64 write test 3"));
	WriteTest3();
	TheTest.Next( _L(" @SYMTestCaseID:PDS-SQL-UT-4135 RFileBuf64 write test 4"));
	WriteTest4();
	TheTest.Next( _L(" @SYMTestCaseID:PDS-SQL-UT-4136 RFileBuf64 write test 5"));
	WriteTest5();
	TheTest.Next( _L(" @SYMTestCaseID:PDS-SQL-UT-4137 RFileBuf64 write test 6"));
	WriteTest6();
	TheTest.Next( _L("RFileBuf64 read test - preparation"));
	PrepareReadTest();
	TheTest.Next( _L(" @SYMTestCaseID:PDS-SQL-UT-4138 RFileBuf64 read test 1"));
	ReadTest1();
	TheTest.Next( _L(" @SYMTestCaseID:PDS-SQL-UT-4139 RFileBuf64 read test 2"));
	ReadTest2();
	TheTest.Next( _L(" @SYMTestCaseID:PDS-SQL-UT-4140 RFileBuf64 read test 3"));
	ReadTest3();

	TheTest.Next( _L(" @SYMTestCaseID:PDS-SQL-UT-4141 RFileBuf64::SetReadAheadSize() test"));
	SetReadAheadSizeTest();
	
	(void)TheFs.Delete(KTestFile);
	
	TheTest.Next( _L(" @SYMTestCaseID:PDS-SQL-UT-4142 RFileBuf64::Create() OOM test"));
	OomTest(EOomCreateTest);
	TheTest.Next( _L(" @SYMTestCaseID:PDS-SQL-UT-4142 RFileBuf64::Open() OOM test"));
	OomTest(EOomOpenTest);
	TheTest.Next( _L(" @SYMTestCaseID:PDS-SQL-UT-4142 RFileBuf64::Temp() OOM test"));
	OomTest(EOomTempTest);
	TheTest.Next( _L(" @SYMTestCaseID:PDS-SQL-UT-4207 RFileBuf64::Write() OOM test"));
	WriteOomTest();
	TheTest.Next( _L(" @SYMTestCaseID:PDS-SQL-UT-4208 RFileBuf64::Read() OOM test"));
	ReadOomTest();
	
	TheTest.Next( _L(" @SYMTestCaseID:PDS-SQL-UT-4195 RFileBuf64::Create() file I/O error simulation test"));
	CreateFileIoErrTest();
	TheTest.Next( _L(" @SYMTestCaseID:PDS-SQL-UT-4196 RFileBuf64::Open() file I/O error simulation test"));
	OpenFileIoErrTest();
	TheTest.Next( _L(" @SYMTestCaseID:PDS-SQL-UT-4197 RFileBuf64::Temp() file I/O error simulation test"));
	OpenFileIoErrTest();
	TheTest.Next( _L(" @SYMTestCaseID:PDS-SQL-CT-4212 RFileBuf64::Write() test"));
	TestSetSizeCounter();

	TheTest.Next( _L(" @SYMTestCaseID:PDS-SQL-UT-4236 RFileBuf64 panic test"));
	FileBufPanicTest();
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	
	__UHEAP_MARK;
	
	TestEnvInit();
	DeleteTestFiles();
	DoTests();
	TestEnvDestroy();

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
	
#else//_SQLPROFILER	

TInt E32Main()
	{
	TheTest.Title();
	
 	TheTest.Start(_L("This test works only if the test is built with _SQLPROFILER macro defined!"));
	TheTest.End();
	TheTest.Close();
	
	return KErrNone;
	}
	
#endif//_SQLPROFILER
