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
#include <s32file.h>

RTest TheTest(_L("t_sqlblob test"));

RSqlDatabase TheDb1;
RSqlDatabase TheDb2;
RSqlDatabase ThePrivateDb;

_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDbName1, "c:\\test\\t_blob1.db");
_LIT(KTestDbName2, "c:\\test\\t_blob2.db");
_LIT(KAttachedDbName, "attached_db");
_LIT(KPrivateSecureDb, "c:\\private\\1111C1CC\\ps.db"); 

const TInt KLargeDataBufLen = 2048;

// A buffer containing 2Kb of data.
// When it is used to write to a blob the data will exceed the size of the client buffer
// (which is 8 bytes in debug, 1.5Kb on target) and will be immediately transferred to the server
TBuf8<KLargeDataBufLen> TheLargeData;

///////////////////////////////////////////////////////////////////////////////////////
// Test database delete functions

void DeleteTestDbs()
	{
	TheDb1.Close();
	TheDb2.Close();
	ThePrivateDb.Close();
	(void)RSqlDatabase::Delete(KTestDbName1);
	(void)RSqlDatabase::Delete(KTestDbName2);
	(void)RSqlDatabase::Delete(KPrivateSecureDb);
	}

///////////////////////////////////////////////////////////////////////////////////////
// Test macros and functions

void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteTestDbs();
		TheTest(EFalse, aLine);
		}
	}
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DeleteTestDbs();
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////
// Test database create functions

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

void CreateTestDbs()
	{
	// Create t_blob1.db
	TInt err = TheDb1.Create(KTestDbName1);
	TEST2(err, KErrNone);
	
 	err = TheDb1.Exec(_L("CREATE TABLE table1(I INTEGER, T TEXT, B BLOB)"));	
	TEST(err >= 0);	
	
	// Create t_blob2.db
	err = TheDb2.Create(KTestDbName2);
	TEST2(err, KErrNone);

	err = TheDb2.Exec(_L("CREATE TABLE table2(int INTEGER, text TEXT, blob BLOB)"));	
	TEST(err >= 0);
		
	// Insert a blob value of 'FGFGFGFGFG' (10 characters in size)		
	err = TheDb2.Exec(_L("INSERT INTO table2 VALUES(1, 'Text Data', x'46474647464746474647')"));
	TEST2(err, 1);	
	
	// Create private secure db
	err = ThePrivateDb.Create(KPrivateSecureDb);
	TEST2(err, KErrNone);
	
	err = ThePrivateDb.Exec(_L("CREATE TABLE table3(age INTEGER, name TEXT, picture BLOB)"));	
	TEST(err >= 0);
	
	// Insert a blob value of 'ABABABABABABABA' (15 characters in size)		
	err = ThePrivateDb.Exec(_L("INSERT INTO table3 VALUES(31, 'John Smith', x'414241424142414241424142414241')"));
	TEST2(err, 1);		
	}
	
void CreateIndices()
	{
	TInt err = TheDb1.Exec(_L("CREATE INDEX textIdx1 on table1(T)"));	
	TEST(err >= 0);
	err = TheDb1.Exec(_L("CREATE INDEX blobIdx1 on table1(B)"));	
	TEST(err >= 0);
	
	err = TheDb2.Exec(_L("CREATE INDEX textIdx2 on table2(text)"));	
	TEST(err >= 0);
	err = TheDb2.Exec(_L("CREATE INDEX blobIdx2 on table2(blob)"));	
	TEST(err >= 0);
	}
	
void RemoveIndices()
	{
	TInt err = TheDb1.Exec(_L("DROP INDEX textIdx1"));	
	TEST(err >= 0);
	err = TheDb1.Exec(_L("DROP INDEX blobIdx1"));	
	TEST(err >= 0);

	err = TheDb2.Exec(_L("DROP INDEX textIdx2"));	
	TEST(err >= 0);
	err = TheDb2.Exec(_L("DROP INDEX blobIdx2"));	
	TEST(err >= 0);
	}
		
void AttachTestDb2()
	{
	TInt err = TheDb1.Attach(KTestDbName2, KAttachedDbName);
	TEST2(err, KErrNone);
	}
	
void FillLargeDataBuf(TChar aChar = 'Z')
	{
	TheLargeData.Fill(aChar, KLargeDataBufLen);	
	}
	
///////////////////////////////////////////////////////////////////////////////////////
// Unit test functions

void CheckBlobPropertiesL(TInt aBlobSize)
	{
	// Check properties of the last inserted blob 
		
	RSqlStatement stmt;
	CleanupClosePushL(stmt);
	TInt err = stmt.Prepare(TheDb1, _L("SELECT B FROM table1 WHERE ROWID == :Val"));
	TEST2(err, KErrNone);
	TInt paramIndex = stmt.ParameterIndex(_L(":Val"));
	TEST(paramIndex >= 0);	
	err = stmt.BindInt(paramIndex, TheDb1.LastInsertedRowId());
	TEST2(err, KErrNone);
	err = stmt.Next();
	TEST2(err, KSqlAtRow);
	
	// Check the 'declared column type' is ESqlBinary
	TSqlColumnType declColType;
	err = stmt.DeclaredColumnType(0, declColType);
	TEST2(declColType, ESqlBinary);
	// Check the 'runtime column type' is ESqlBinary
	TSqlColumnType colType = stmt.ColumnType(0);
	TEST2(colType, ESqlBinary);
	// Check the 'column size' is the size of the blob
	TInt blobSize = stmt.ColumnSize(0);
	TEST2(blobSize, aBlobSize);
	// Check the 'column value' is not 'NULL' (even for a zeroblob)
	TBool isNull = stmt.IsNull(0);
	TEST2(isNull, EFalse);
	// Check the 'column value' can be retrieved as a binary value
	TPtrC8 binaryPtr = stmt.ColumnBinaryL(0);
	TEST2(binaryPtr.Length(), aBlobSize);
	// Check the 'column value' cannot be retrieved as a text value (it is of type ESqlBinary)
	TPtrC textptr = stmt.ColumnTextL(0);
	TEST2(textptr.Length(), 0);	
	CleanupStack::PopAndDestroy(&stmt);
	}

void InsertBindZeroBlob(TInt aBlobSize)
	{
	// Insert a record that has had a zeroblob bound to it
	RSqlStatement stmt;
	TInt err = stmt.Prepare(TheDb1, _L("INSERT INTO table1 values(1, 'some text', :Val)"));
	TEST2(err, KErrNone);
	TInt paramIndex = stmt.ParameterIndex(_L(":Val"));
	TEST(paramIndex >= 0);	
	err = stmt.BindZeroBlob(paramIndex, aBlobSize);
	TEST2(err, KErrNone);
	err = stmt.Exec();
	stmt.Close();
	TEST2(err, 1);
	}
	
void InsertSQLiteZeroBlob(TInt aBlobSize)
	{		
	// Insert a record that contains the 'zeroblob()' function
	RSqlStatement stmt;
	TInt err = stmt.Prepare(TheDb1, _L("INSERT INTO table1 values(2, 'more text', zeroblob(:Val))"));
	TEST2(err, KErrNone);
	TInt paramIndex = stmt.ParameterIndex(_L(":Val")); 
	TEST(paramIndex >= 0);	
	err = stmt.BindInt(paramIndex, aBlobSize);
	TEST2(err, KErrNone);
	err = stmt.Exec();
	stmt.Close();
	TEST2(err, 1);
	}

void InsertBlobValueL(TInt aBlobSize)
	{	
	// Insert a record that contains an actual blob value
	HBufC8* binaryData = HBufC8::NewLC(aBlobSize);
	TPtr8 binaryDataPtr(binaryData->Des());
	for(TInt i = 0; i < aBlobSize/2; ++i)
		{
		binaryDataPtr.Append(_L8("DE"));
		}
		
	RSqlStatement stmt;
	CleanupClosePushL(stmt);
	TInt err = stmt.Prepare(TheDb1, _L("INSERT INTO table1 values(3, 'even more text', :Val)"));
	TEST2(err, KErrNone);
	TInt paramIndex = stmt.ParameterIndex(_L(":Val")); 
	TEST(paramIndex >= 0);	
	err = stmt.BindBinary(paramIndex, binaryDataPtr);
	TEST2(err, KErrNone);
	err = stmt.Exec();
	TEST2(err, 1);
	CleanupStack::PopAndDestroy(2); // stmt, binaryData		
	}	

void StreamBlob1L(TInt aBlobSize) 
	{	
	// Blob 1 is a zeroblob of size aBlobSize
	
	// Read (zero) data from the blob
	RSqlBlobReadStream rdStrm;
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TInt size = rdStrm.SizeL(); // check the blob's size
	TEST2(size, aBlobSize); 
	_LIT8(KFiveZeros, "\x0\x0\x0\x0\x0");
	_LIT8(KTwentyZeros, "\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0");
	TBuf8<50> data;
	rdStrm.ReadL(data, 5);
	TEST(data.Compare(KFiveZeros) == 0); // check 5 bytes of zero have been read
	rdStrm.ReadL(data, 20);
	TEST(data.Compare(KTwentyZeros) == 0);	// check 20 bytes of zero have been read
	CleanupStack::PopAndDestroy(&rdStrm);
	
	// Write some actual data to the blob
	RSqlBlobWriteStream wrStrm;
	CleanupClosePushL(wrStrm);
	wrStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	size = wrStrm.SizeL(); // check the blob's size
	TEST2(size, aBlobSize); 
	wrStrm.WriteL(_L8("AABBCCDDEE")); // write 10 bytes
	wrStrm.WriteL(_L8("FFGGHHIIJJ")); // write another 10 bytes
	wrStrm.WriteL(_L8("KKLLMMNNOOPPQQRRSSTTUUVVWWXX")); // write another 28 bytes
	wrStrm.CommitL();
	CleanupStack::PopAndDestroy(&wrStrm);
	
	// Read back some of the blob data
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	size = rdStrm.SizeL(); // check the blob's size
	TEST2(size, aBlobSize);
	rdStrm.ReadL(data, 4);
	TEST(data.Compare(_L8("AABB")) == 0); // check the first 4 bytes
	rdStrm.ReadL(data, 35);
	TEST(data.Compare(_L8("CCDDEEFFGGHHIIJJKKLLMMNNOOPPQQRRSST")) == 0); // check the next 35 bytes
	rdStrm.ReadL(data, 19);
	_LIT8(KTrailingZeros, "TUUVVWWXX\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0"); 
	TEST(data.Compare(KTrailingZeros) == 0); // check the next 19 bytes (which includes some of the original zero bytes)
	CleanupStack::PopAndDestroy(&rdStrm);
	}

void StreamText1L() 
	{	
	// Read data from the text column -
	// the database encoding is UTF-16 so the text is stored as UTF-16
	RSqlBlobReadStream rdStrm;
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table1"), _L("T"));
	TBuf<50> dataUTF16;
	rdStrm.ReadL(dataUTF16, 9);
	TEST(dataUTF16.Compare(_L("some text")) == 0); 
	CleanupStack::PopAndDestroy(&rdStrm);
	
	// Write some data to the text column (as UTF-16)
	RSqlBlobWriteStream wrStrm;
	CleanupClosePushL(wrStrm);
	wrStrm.OpenL(TheDb1, _L("table1"), _L("T"));
	wrStrm.WriteL(_L("new text!")); // can only write up to the original size of the data - 9 chars
	wrStrm.CommitL();
	CleanupStack::PopAndDestroy(&wrStrm);
	
	// Read back some of the text (as UTF-16)
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table1"), _L("T"));
	rdStrm.ReadL(dataUTF16, 9);
	TEST(dataUTF16.Compare(_L("new text!")) == 0);
	CleanupStack::PopAndDestroy(&rdStrm);
	
	// Write some data to the text column (as UTF-8)
	CleanupClosePushL(wrStrm);
	wrStrm.OpenL(TheDb1, _L("table1"), _L("T"));
	wrStrm.WriteL(_L8("try again"));
	wrStrm.CommitL();
	CleanupStack::PopAndDestroy(&wrStrm);
	
	// Read back some of the text (as UTF-8)
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table1"), _L("T"));
	TBuf8<50> dataUTF8;
	rdStrm.ReadL(dataUTF8, 9);
	TEST(dataUTF8.Compare(_L8("try again")) == 0);
	CleanupStack::PopAndDestroy(&rdStrm);
	}
	
void StreamBlob2L(TInt aBlobSize)
	{
	// Blob 2 is a zeroblob of size aBlobSize
	
	// Read (zero) data from the blob
	RSqlBlobReadStream rdStrm;
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TInt size = rdStrm.SizeL(); // check the blob's size
	TEST2(size, aBlobSize);
	_LIT8(KFifteenZeros, "\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0");
	_LIT8(KSixtyOneZeros, "\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0");
	TBuf8<200> data;
	rdStrm.ReadL(data, 15);
	TEST(data.Compare(KFifteenZeros) == 0); // check 15 bytes of zero have been read
	rdStrm.ReadL(data, 61);
	TEST(data.Compare(KSixtyOneZeros) == 0); // check 61 bytes of zero have been read
	size = rdStrm.SizeL(); // check the blob's size
	TEST2(size, aBlobSize); 
	CleanupStack::PopAndDestroy(&rdStrm); 

	// Write some actual data to the blob 
	RSqlBlobWriteStream wrStrm;
	CleanupClosePushL(wrStrm);
	wrStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	size = wrStrm.SizeL(); // check the blob's size
	TEST2(size, aBlobSize);
	wrStrm.WriteL(_L8("SOMENEWDATASOMENEWDATAS")); // write 23 bytes
	wrStrm.WriteL(_L8("OMENEWDATASOMENEWDATASOMENEWDATASOMENEWDATA")); // write another 43 bytes
	wrStrm.WriteL(_L8("SOMENEWDATASOMENEWDATASOMENEWDATASOMENEWDATASOMENEWDATASOMENEWDATASOMENE")); // write another 72 bytes
	wrStrm.WriteL(_L8("WDATASOMENEWDATA")); // write another 16 bytes
	size = wrStrm.SizeL(); // check the blob's size
	TEST2(size, aBlobSize); 
	wrStrm.CommitL();
	CleanupStack::PopAndDestroy(&wrStrm); 
	
	// Read back some of the blob data
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	size = rdStrm.SizeL(); // check the blob's size
	TEST2(size, aBlobSize);
	rdStrm.ReadL(data, 1);
	TEST(data.Compare(_L8("S")) == 0);  // check the first byte
	rdStrm.ReadL(data, 136);
	TEST(data.Compare(_L8("OMENEWDATASOMENEWDATASOMENEWDATASOMENEWDATASOMENEWDATASOMENEWDATASOMENEWDATASOMENEWDATASOMENEWDATASOMENEWDATASOMENEWDATASOMENEWDATASOMEN")) == 0); // check the next 136 bytes
	rdStrm.ReadL(data, 30);
	_LIT8(KTrailingZeros, "EWDATASOMENEWDATA\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0"); 
	TEST(data.Compare(KTrailingZeros) == 0); // check the next 30 bytes (which includes some of the original zero bytes)
	size = rdStrm.SizeL(); // check the blob's size
	TEST2(size, aBlobSize); 
	CleanupStack::PopAndDestroy(&rdStrm);	
	}
	
void StreamBlob3L(TInt aBlobSize)
	{
	// Blob 3 is a 'real' blob of value "DEDEDEDEDEDEDEDEDEDE"
	
	// Read some of the zero data
	RSqlBlobReadStream rdStrm;
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TInt size = rdStrm.SizeL(); // check the blob's size
	TEST2(size, aBlobSize);
	TBuf8<20> data;
	rdStrm.ReadL(data, 3);
	TEST(data.Compare(_L8("DED")) == 0); // check the first 3 bytes
	rdStrm.ReadL(data, 12);
	TEST(data.Compare(_L8("EDEDEDEDEDED")) == 0); // check the next 12 bytes
	size = rdStrm.SizeL(); // check the blob's size
	TEST2(size, aBlobSize); 
	CleanupStack::PopAndDestroy(&rdStrm); 	
	
	// Write some new data to the blob 
	RSqlBlobWriteStream wrStrm;
	CleanupClosePushL(wrStrm);
	wrStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	size = wrStrm.SizeL(); // check the blob's size
	TEST2(size, aBlobSize);
	wrStrm.WriteL(_L8("ABCDEF")); // write 6 bytes
	wrStrm.WriteL(_L8("GHIJKLMNOPQ")); // write another 11 bytes
	size = wrStrm.SizeL(); // check the blob's size
	TEST2(size, aBlobSize); 
	wrStrm.CommitL();
	CleanupStack::PopAndDestroy(&wrStrm);	
	
	// Read back some of the blob data
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	size = rdStrm.SizeL(); // check the blob's size
	TEST2(size, aBlobSize);
	rdStrm.ReadL(data, 2);
	TEST(data.Compare(_L8("AB")) == 0); // check the first 2 bytes
	rdStrm.ReadL(data, 7);
	TEST(data.Compare(_L8("CDEFGHI")) == 0); // check the next 7 bytes
	rdStrm.ReadL(data, 11);
	TEST(data.Compare(_L8("JKLMNOPQEDE")) == 0); // check the next 11 bytes
	size = rdStrm.SizeL(); // check the blob's size
	TEST2(size, aBlobSize); 
	CleanupStack::PopAndDestroy(&rdStrm);	

	// Seek to position - the blob value is now "ABCDEFGHIJKLMNOPQEDE"
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TStreamPos pos(9);
	rdStrm.Source()->SeekL(MStreamBuf::ERead, pos);
	rdStrm.ReadL(data, 2);
	TEST(data.Compare(_L8("JK")) == 0);
	rdStrm.ReadL(data, 5);
	TEST(data.Compare(_L8("LMNOP")) == 0);
	TStreamPos pos2(3);
	rdStrm.Source()->SeekL(MStreamBuf::ERead, pos2);
	rdStrm.ReadL(data, 4);
	TEST(data.Compare(_L8("DEFG")) == 0);
	TStreamPos pos3(21);
	TRAPD(err, rdStrm.Source()->SeekL(MStreamBuf::ERead, pos3));
	TEST2(err, KErrEof);
	TStreamPos pos4(18);
	rdStrm.Source()->SeekL(MStreamBuf::ERead, pos4);
	TRAP(err, rdStrm.ReadL(data, 3));
	TEST2(err, KErrEof);
	CleanupStack::PopAndDestroy(&rdStrm);	

	CleanupClosePushL(wrStrm);
	wrStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TStreamPos pos5(7);
	wrStrm.Sink()->SeekL(MStreamBuf::EWrite, pos5);
	wrStrm.WriteL(_L8("ZZZZZ"));
	wrStrm.WriteL(_L8("YYY"));
	TStreamPos pos6(17);
	wrStrm.Sink()->SeekL(MStreamBuf::EWrite, pos6);
	wrStrm.WriteL(_L8("XXX"));
	wrStrm.CommitL();
	wrStrm.Close();
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	rdStrm.ReadL(data, 20);
	TEST(data.Compare(_L8("ABCDEFGZZZZZYYYPQXXX")) == 0);
	rdStrm.Close();
	TStreamPos pos7(21);
	wrStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TRAP(err, wrStrm.Sink()->SeekL(MStreamBuf::EWrite, pos7));
	TEST2(err, KErrEof);
	TStreamPos pos8(18);
	wrStrm.Sink()->SeekL(MStreamBuf::EWrite, pos8);
	wrStrm.WriteL(_L8("TTT"));
	TRAP(err, wrStrm.CommitL());
	TEST2(err, KErrEof);
	wrStrm.Close();
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	rdStrm.ReadL(data, 20);
	TEST(data.Compare(_L8("ABCDEFGZZZZZYYYPQXXX")) == 0);
	CleanupStack::PopAndDestroy(2);	
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-4099
@SYMTestCaseDesc		Incremental blob tests on a database using streams.
						Insert a zeroblob using RSqlStatement::BindZeroBlob(), read and write to
					 	the blob using streams, also read and write to a text column using streams.
						Tests the RSqlBlobReadStream and RSqlBlobWriteStream methods.
@SYMTestPriority		Medium
@SYMTestActions			Execution of incremental blob stream operations on a database.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10411
                        REQ10418
*/	
void StreamBindZeroBlobTestL()
	{
	const TInt KBlobSize = 300;
	InsertBindZeroBlob(KBlobSize);
	CheckBlobPropertiesL(KBlobSize);
	StreamBlob1L(KBlobSize);
	StreamText1L();
	CheckBlobPropertiesL(KBlobSize);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4100
@SYMTestCaseDesc		Incremental blob tests on a database, using streams.
						Insert a zeroblob using the SQLite function zeroblob(), 
						read and write to the blob using streams.
						Tests the RSqlBlobReadStream and RSqlBlobWriteStream methods.
@SYMTestPriority		Medium
@SYMTestActions			Execution of incremental blob stream operations on a database.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10411
                        REQ10418
*/	
void StreamSqliteZeroBlobTestL()
	{
	const TInt KBlobSize = 1500;
	InsertSQLiteZeroBlob(KBlobSize);
	CheckBlobPropertiesL(KBlobSize);
	StreamBlob2L(KBlobSize);
	CheckBlobPropertiesL(KBlobSize);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4101
@SYMTestCaseDesc		Incremental blob tests on a database, using streams.
						Insert a real blob, read and write to the blob using streams.
						Tests the RSqlBlobReadStream and RSqlBlobWriteStream methods.
@SYMTestPriority		Medium
@SYMTestActions			Execution of incremental blob stream operations on a database.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10411
                        REQ10418
*/
void StreamRealBlobTestL()
	{
	const TInt KBlobSize = 20;
	InsertBlobValueL(KBlobSize);
	CheckBlobPropertiesL(KBlobSize);
	StreamBlob3L(KBlobSize);
	CheckBlobPropertiesL(KBlobSize);
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-4102
@SYMTestCaseDesc		Whole value blob retrieval tests on a database.
						Retrieve the whole value of a blob object in one go.
						Tests the TSqlBlob 'get' methods.
@SYMTestPriority		Medium
@SYMTestActions			Execution of whole value blob retrieval operations on a database.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10411
                        REQ10418
*/
void GetWholeBlob3L()
	{
	// Blob 3 is a 'real' blob of value "ABCDEFGZZZZZYYYPQXXX"
	const TInt KBlobSize = 20;
	
	// Get the whole content of Blob 3 in one go, using TSqlBlob::GetLC()
	HBufC8* wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table1"), _L("B"));
	TInt bufSize = wholeBuf->Size();
	TEST2(bufSize, KBlobSize);
	TEST(wholeBuf->Des().Compare(_L8("ABCDEFGZZZZZYYYPQXXX")) == 0);	
	CleanupStack::PopAndDestroy(wholeBuf); 

	// Get the whole content of Blob 3 in one go, using TSqlBlob::Get()
	HBufC8* buf = HBufC8::NewLC(KBlobSize);	
	TPtr8 bufPtr(buf->Des());	  
	TInt err = TSqlBlob::Get(TheDb1, _L("table1"), _L("B"), bufPtr);
	TEST2(err, KErrNone); 
	TEST(bufPtr.Compare(_L8("ABCDEFGZZZZZYYYPQXXX")) == 0);	
	CleanupStack::PopAndDestroy(buf); 
	
	// Get the whole content of Blob 3 in one go, using TSqlBlob::Get(),
	// ensuring that a buffer larger than the blob can be used
	HBufC8* largerBuf = HBufC8::NewLC(KBlobSize * 2);	
	TPtr8 largerBufPtr(largerBuf->Des());	  
	err = TSqlBlob::Get(TheDb1, _L("table1"), _L("B"), largerBufPtr);
	TEST2(err, KErrNone); 
	TEST(largerBufPtr.Compare(_L8("ABCDEFGZZZZZYYYPQXXX")) == 0);	
	CleanupStack::PopAndDestroy(largerBuf); 
	
	// Get the whole content of the blob in 2 chunks of 10 bytes
	HBufC8* streamBuf = HBufC8::NewLC(10);
	TPtr8 streamBufPtr(streamBuf->Des());
	RSqlBlobReadStream rdStrm;
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TInt size = rdStrm.SizeL();
	TEST2(size, KBlobSize);
	HBufC8* combinedData = HBufC8::NewLC(KBlobSize);
	TPtr8 combinedDataPtr(combinedData->Des());		
	for(TInt i = 2; i > 0 ; --i)
		{
		rdStrm.ReadL(streamBufPtr, 10);
		combinedDataPtr.Append(streamBufPtr);
		}
	TEST(combinedDataPtr.Compare(_L8("ABCDEFGZZZZZYYYPQXXX")) == 0);
	CleanupStack::PopAndDestroy(3); // combinedDataRead, rdStrm, streamBuf
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4104
@SYMTestCaseDesc		Whole value blob write tests on a database.
						Write the whole value of a blob object in one go.
						Tests the TSqlBlob 'set' methods.
@SYMTestPriority		Medium
@SYMTestActions			Execution of whole value blob write operations on a database.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10411
                        REQ10418
*/
void SetWholeBlob3L()
	{
	// Blob 3 is a 'real' blob of value "KKKKKKKKKKKKKKKKKEDE"
	TInt KBlobSize = 20;
	
	// Set the whole content of Blob 3 in one go
	HBufC8* dataBuf = HBufC8::NewLC(KBlobSize);
	TPtr8 dataPtr(dataBuf->Des());
	dataPtr.Append(_L8("CDCDCDCDCDCDCDCDCDCD"));
	TSqlBlob::SetL(TheDb1, _L("table1"), _L("B"), dataPtr);
	CleanupStack::PopAndDestroy(dataBuf); 
	
	// Check that the new blob data was written
	HBufC8* retrievedDataBuf = TSqlBlob::GetLC(TheDb1, _L("table1"), _L("B"));
	TInt blobLength = retrievedDataBuf->Size();
	TEST2(blobLength, KBlobSize);
	TEST(retrievedDataBuf->Des().Compare(_L8("CDCDCDCDCDCDCDCDCDCD")) == 0);		
	CleanupStack::PopAndDestroy(retrievedDataBuf);
	
	// Set the whole content of the blob in 2 chunks of 10 bytes
	RSqlBlobWriteStream wrStrm;
	CleanupClosePushL(wrStrm);
	wrStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TInt size = wrStrm.SizeL();
	TEST2(size, KBlobSize);		
	for(TInt i = 2; i > 0; --i)
		{
		wrStrm.WriteL(_L8("ZYZYZYZYZY")); 
		}
	CleanupStack::PopAndDestroy(&wrStrm);
	
	// Check that the new blob data was written
	retrievedDataBuf = TSqlBlob::GetLC(TheDb1, _L("table1"), _L("B"));
	blobLength = retrievedDataBuf->Size();
	TEST2(blobLength, KBlobSize);
	TEST(retrievedDataBuf->Des().Compare(_L8("ZYZYZYZYZYZYZYZYZYZY")) == 0);		
	CleanupStack::PopAndDestroy(retrievedDataBuf);
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-4106
@SYMTestCaseDesc		Blob read and write tests on an attached database.
						Performs streaming and whole value read and write operations
						on a blob in an attached database to ensure that the 
						RSqlBlobReadStream, RSqlBlobWriteStream and TSqlBlob methods 
						can be used on an attached database.
						Tests the RSqlBlobReadStream, RSqlBlobWriteStream and TSqlBlob methods.
@SYMTestPriority		Medium
@SYMTestActions			Execution of blob read and write operations on an attached database.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10411
                        REQ10418
*/
void AttachDbTestL()
	{
	// Attach test db 2 to test db 1
	AttachTestDb2();
	
	// Open a read stream on a blob in the attached database - 
	// the blob in the single record has a value of "FGFGFGFGFG"
	const TInt KAttachedBlobSize = 10;
	RSqlBlobReadStream rdStrm;
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), KAttachedDbName);
	TInt size = rdStrm.SizeL(); // check the blob's size
	TEST2(size, KAttachedBlobSize);
	TBuf8<20> data;
	rdStrm.ReadL(data, 2);
	TEST(data.Compare(_L8("FG")) == 0);	// check the first 2 bytes
	rdStrm.ReadL(data, 8);
	TEST(data.Compare(_L8("FGFGFGFG")) == 0); // check the next 8 bytes
	size = rdStrm.SizeL(); // check the blob's size
	TEST2(size, KAttachedBlobSize); 
	CleanupStack::PopAndDestroy(&rdStrm);	
	
	// Write some new data to the blob 
	RSqlBlobWriteStream wrStrm;
	CleanupClosePushL(wrStrm);
	wrStrm.OpenL(TheDb1, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), KAttachedDbName);
	size = wrStrm.SizeL(); // check the blob's size
	TEST2(size, KAttachedBlobSize);
	wrStrm.WriteL(_L8("LLLL")); // write 4 bytes
	wrStrm.WriteL(_L8("MMMMM")); // write another 5 bytes
	size = wrStrm.SizeL(); // check the blob's size
	TEST2(size, KAttachedBlobSize); 
	wrStrm.CommitL();
	CleanupStack::PopAndDestroy(&wrStrm);	
	
	// Read back some of the blob data
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), KAttachedDbName);
	size = rdStrm.SizeL(); // check the blob's size
	TEST2(size, KAttachedBlobSize);
	rdStrm.ReadL(data, 2);
	TEST(data.Compare(_L8("LL")) == 0); // check the first 2 bytes
	rdStrm.ReadL(data, 5);
	TEST(data.Compare(_L8("LLMMM")) == 0); // check the next 5 bytes
	rdStrm.ReadL(data, 3);
	TEST(data.Compare(_L8("MMG")) == 0); // check the next 3 bytes
	size = rdStrm.SizeL(); // check the blob's size
	TEST2(size, KAttachedBlobSize); 
	CleanupStack::PopAndDestroy(&rdStrm);	
	
	// Get the entire blob in the attached database
	HBufC8* wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), KAttachedDbName);
	TInt blobLength = wholeBuf->Length();
	TEST2(blobLength, KAttachedBlobSize);
	TEST(wholeBuf->Des().Compare(_L8("LLLLMMMMMG")) == 0);	
	CleanupStack::PopAndDestroy(wholeBuf); 
		
	TSqlBlob::Get(TheDb1, _L("table2"), _L("blob"), data, TheDb2.LastInsertedRowId(), KAttachedDbName);	 		 
	TEST(data.Compare(_L8("LLLLMMMMMG")) == 0);	

	// Set the entire blob in the attached database
	data.Zero();
	data.Append(_L8("STSTSTSTST"));
	TSqlBlob::SetL(TheDb1, _L("table2"), _L("blob"), data, TheDb2.LastInsertedRowId(), KAttachedDbName);
	wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), KAttachedDbName);
	TEST(wholeBuf->Des().Compare(_L8("STSTSTSTST")) == 0);	
	CleanupStack::PopAndDestroy(wholeBuf); 	
	}	

void BadParamReadStreamL()	
	{
	HBufC* tooLongName = HBufC::NewLC(KMaxFileName + 1);
	TPtr tooLongNameDes = tooLongName->Des();
	tooLongNameDes.Fill('A', KMaxFileName + 1);	

	// RSqlBlobReadStream::OpenL()	
	RSqlBlobReadStream rdStrm;
	CleanupClosePushL(rdStrm);
	TRAPD(err, rdStrm.OpenL(TheDb1, _L("table1"), _L("B"))); // a successful open (on a BLOB column)
	TEST2(err, KErrNone);
	rdStrm.Close();
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table1"), _L("T"))); // a successful open (on a TEXT column)
	TEST2(err, KErrNone);
	rdStrm.Close();
	TRAP(err, rdStrm.OpenL(TheDb2, _L("table1"), _L("B"))); // wrong db connection
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, rdStrm.OpenL(TheDb1, _L(""), _L("B"))); // empty table name
	TEST2(err, KErrBadName);
	TRAP(err, rdStrm.OpenL(TheDb1, tooLongNameDes, _L("B"))); // too long table name
	TEST2(err, KErrBadName);
	TRAP(err, rdStrm.OpenL(TheDb1, _L("invalidTableName"), _L("B"))); // invalid table name
	TEST2(err, KSqlErrGeneral);
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table1"), _L(""))); // empty column name
	TEST2(err, KErrBadName);
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table1"), tooLongNameDes)); // too long column name
	TEST2(err, KErrBadName);
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table1"), _L("invalidColumnName"))); // invalid column name
	TEST2(err, KSqlErrGeneral);
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table1"), _L("I"))); // invalid column type
	TEST2(err, KSqlErrGeneral);
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table1"), _L("B"), -12)); // illegal ROWID
	TEST2(err, KErrArgument);
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table1"), _L("B"), 99)); // invalid ROWID
	TEST2(err, KSqlErrGeneral);
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table1"), _L("B"), KSqlLastInsertedRowId, _L("main"))); // a successful open (on a BLOB column)
	TEST2(err, KErrNone);
	rdStrm.Close();
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table1"), _L("B"), KSqlLastInsertedRowId, _L(""))); // a successful open (on a BLOB column)
	TEST2(err, KErrNone);
	rdStrm.Close();
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table1"), _L("T"), KSqlLastInsertedRowId, _L("main"))); // a successful open (on a TEXT column)
	TEST2(err, KErrNone);
	rdStrm.Close();
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table1"), _L("T"), KSqlLastInsertedRowId, _L(""))); // a successful open (on a TEXT column)
	TEST2(err, KErrNone);
	rdStrm.Close();
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // a successful open (on a BLOB column)
	TEST2(err, KErrNone);
	rdStrm.Close();
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table2"), _L("text"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // a successful open (on a TEXT column)
	TEST2(err, KErrNone);
	rdStrm.Close();
	TRAP(err, rdStrm.OpenL(TheDb2, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // wrong db connection
	TEST2(err, KSqlErrGeneral);
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), tooLongNameDes)); // too long attached db name
	TEST2(err, KErrBadName);
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), _L("invalidAttachedDbName"))); // invalid attached db name
	TEST2(err, KSqlErrGeneral);
	TRAP(err, rdStrm.OpenL(TheDb1, _L("invalidTableName"), _L("blob"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // invalid table name
	TEST2(err, KSqlErrGeneral);
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table2"), _L("invalidColumnName"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // invalid column name
	TEST2(err, KSqlErrGeneral);
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table2"), _L("int"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // invalid column type
	TEST2(err, KSqlErrGeneral);
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table2"), _L("blob"), 64, KAttachedDbName)); // invalid ROWID
	TEST2(err, KSqlErrGeneral);
	CleanupStack::PopAndDestroy(&rdStrm);
	CleanupStack::PopAndDestroy(tooLongName);		
	
	// RSqlBlobReadStream::Source()::SeekL()
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TInt size = rdStrm.SizeL();
	TEST(size > 0);
	TStreamPos pos(-1);
	TRAP(err, rdStrm.Source()->SeekL(MStreamBuf::ERead, pos));
	TEST2(err, KErrEof);
	TStreamPos pos2(size - 1);
	TRAP(err, rdStrm.Source()->SeekL(MStreamBuf::ERead, pos2));
	TEST2(err, KErrNone);
	TStreamPos pos3(size + 1);
	TRAP(err, rdStrm.Source()->SeekL(MStreamBuf::ERead, pos3));
	TEST2(err, KErrEof);
	CleanupStack::PopAndDestroy(&rdStrm);
	
	// RSqlBlobReadStream::ReadL()
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	size = rdStrm.SizeL();
	TEST2(size, 20);
	TBuf8<20> dataBuf;
	TBuf8<50> tooBigDataBuf;	
	TRAP(err, rdStrm.ReadL(tooBigDataBuf));
	TEST2(err, KErrEof);
	rdStrm.Close();
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TRAP(err, rdStrm.ReadL(tooBigDataBuf, size + 1));
	TEST2(err, KErrEof);
	rdStrm.Close();
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TRAP(err, rdStrm.ReadL(tooBigDataBuf, TChar('J'))); // doesn't find 'J' so tries to fill the buffer to its max size
	TEST2(err, KErrEof);
	CleanupStack::PopAndDestroy(&rdStrm);
	}
	
void BadParamWriteStreamL()
	{
	HBufC* tooLongName = HBufC::NewLC(KMaxFileName + 1);
	TPtr tooLongNameDes = tooLongName->Des();
	tooLongNameDes.Fill('A', KMaxFileName + 1);	
	
	// RSqlBlobWriteStream::OpenL()
	RSqlBlobWriteStream wrStrm;
	CleanupClosePushL(wrStrm);
	TRAPD(err, wrStrm.OpenL(TheDb1, _L("table1"), _L("B"))); // a successful open (on a BLOB column)
	TEST2(err, KErrNone);
	wrStrm.Close();
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table1"), _L("T"))); // a successful open (on a TEXT column)
	TEST2(err, KErrNone);
	wrStrm.Close();
	TRAP(err, wrStrm.OpenL(TheDb2, _L("table1"), _L("B"))); // wrong db connection
	TEST2(err, KSqlErrGeneral);
	TRAP(err, wrStrm.OpenL(TheDb1, _L(""), _L("B"))); // empty table name
	TEST2(err, KErrBadName);
	TRAP(err, wrStrm.OpenL(TheDb1, tooLongNameDes, _L("B"))); // too long table name
	TEST2(err, KErrBadName);
	TRAP(err, wrStrm.OpenL(TheDb1, _L("invalidTableName"), _L("B"))); // invalid table name
	TEST2(err, KSqlErrGeneral);
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table1"), _L(""))); // empty column name
	TEST2(err, KErrBadName);
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table1"), tooLongNameDes)); // too long column name
	TEST2(err, KErrBadName);
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table1"), _L("invalidColumnName"))); // invalid column name
	TEST2(err, KSqlErrGeneral);
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table1"), _L("I"))); // invalid column type
	TEST2(err, KSqlErrGeneral);
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table1"), _L("B"), 0)); // illegal ROWID
	TEST2(err, KErrArgument);
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table1"), _L("B"), 99)); // invalid ROWID
	TEST2(err, KSqlErrGeneral);
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table1"), _L("B"), KSqlLastInsertedRowId, _L("main"))); // a successful open (on a BLOB column)
	TEST2(err, KErrNone);
	wrStrm.Close();
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table1"), _L("B"), KSqlLastInsertedRowId, _L(""))); // a successful open (on a BLOB column)
	TEST2(err, KErrNone);
	wrStrm.Close();
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table1"), _L("T"), KSqlLastInsertedRowId, _L("main"))); // a successful open (on a TEXT column)
	TEST2(err, KErrNone);
	wrStrm.Close();
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table1"), _L("T"), KSqlLastInsertedRowId, _L(""))); // a successful open (on a TEXT column)
	TEST2(err, KErrNone);
	wrStrm.Close();
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // a successful open (on a BLOB column)
	TEST2(err, KErrNone);
	wrStrm.Close();
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table2"), _L("text"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // a successful open (on a TEXT column)
	TEST2(err, KErrNone);
	wrStrm.Close();
	TRAP(err, wrStrm.OpenL(TheDb2, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // wrong db connection
	TEST2(err, KSqlErrGeneral);
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), tooLongNameDes)); // too long attached db name
	TEST2(err, KErrBadName);
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), _L("invalidAttachedDbName"))); // invalid attached db name
	TEST2(err, KSqlErrGeneral);
	TRAP(err, wrStrm.OpenL(TheDb1, _L("invalidTableName"), _L("blob"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // invalid table name
	TEST2(err, KSqlErrGeneral);
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table2"), _L("invalidColumnName"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // invalid column name
	TEST2(err, KSqlErrGeneral);
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table2"), _L("int"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // invalid column type
	TEST2(err, KSqlErrGeneral);
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table2"), _L("blob"), 64, KAttachedDbName)); // invalid ROWID
	TEST2(err, KSqlErrGeneral);
	CleanupStack::PopAndDestroy(&wrStrm);
	CleanupStack::PopAndDestroy(tooLongName);

	// RSqlBlobWriteStream::Sink()::SeekL()
	CleanupClosePushL(wrStrm);
	wrStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TInt size = wrStrm.SizeL();
	TEST(size > 0);
	TStreamPos pos(-1);
	TRAP(err, wrStrm.Sink()->SeekL(MStreamBuf::ERead, pos));
	TEST2(err, KErrEof);
	TStreamPos pos2(size - 1);
	TRAP(err, wrStrm.Sink()->SeekL(MStreamBuf::ERead, pos2));
	TEST2(err, KErrNone);
	TStreamPos pos3(size + 1);
	TRAP(err, wrStrm.Sink()->SeekL(MStreamBuf::ERead, pos3));
	TEST2(err, KErrEof);
	CleanupStack::PopAndDestroy(&wrStrm);
	
	// RSqlBlobWriteStream::WriteL()
	CleanupClosePushL(wrStrm);
	wrStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	size = wrStrm.SizeL();
	TEST2(size, 20);
	TBuf8<50> tooBigDataBuf;
	tooBigDataBuf.Fill('B', 50);	
	TRAP(err, wrStrm.WriteL(tooBigDataBuf));
#ifdef _DEBUG
	TEST2(err, KErrEof);
#else
	TEST2(err, KErrNone);
	TRAP(err, wrStrm.CommitL());
	TEST2(err, KErrEof);
#endif
	wrStrm.Close();
	wrStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TRAP(err, wrStrm.WriteL(tooBigDataBuf, size + 1));
#ifdef _DEBUG
	TEST2(err, KErrEof);
#else
	TEST2(err, KErrNone);
	TRAP(err, wrStrm.CommitL());
	TEST2(err, KErrEof);
#endif
	CleanupStack::PopAndDestroy(&wrStrm);
	}
	
void BadParamGetL()
	{
	HBufC* tooLongName = HBufC::NewLC(KMaxFileName + 1);
	TPtr tooLongNameDes = tooLongName->Des();
	tooLongNameDes.Fill('A', KMaxFileName + 1);	

	// TSqlBlob::GetLC()	
	HBufC8* wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table1"), _L("B")); // a successful get (on a BLOB column)
	TInt blobLength = wholeBuf->Length();
	TEST(blobLength > 0);
	CleanupStack::PopAndDestroy(wholeBuf); 
	wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table1"), _L("T")); // a successful get (on a TEXT column)
	blobLength = wholeBuf->Length();
	TEST(blobLength > 0);
	CleanupStack::PopAndDestroy(wholeBuf); 
	TRAPD(err, wholeBuf = TSqlBlob::GetLC(TheDb2, _L("table1"), _L("B"))); // wrong db connection
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb1, _L(""), _L("B"))); // empty table name
	TEST2(err, KErrBadName); 
	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb1, tooLongNameDes, _L("B"))); // too long table name
	TEST2(err, KErrBadName); 
	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb1, _L("invalidTableName"), _L("B"))); // invalid table name
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table1"), _L(""))); // empty column name
	TEST2(err, KErrBadName); 
	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table1"), tooLongNameDes)); // too long column name
	TEST2(err, KErrBadName); 
	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table1"), _L("invalidColumnName"))); // invalid column name
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table1"), _L("I"))); // invalid column type
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table1"), _L("B"), -2)); // illegal ROWID
	TEST2(err, KErrArgument); 
	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table1"), _L("B"), 3731)); // invalid ROWID
	TEST2(err, KSqlErrGeneral); 
	wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table1"), _L("B"), TheDb1.LastInsertedRowId(), _L("main")); // a successful get (on a BLOB column)
	blobLength = wholeBuf->Length();
	TEST(blobLength > 0);
	CleanupStack::PopAndDestroy(wholeBuf); 
	wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table1"), _L("B"), TheDb1.LastInsertedRowId(), _L("")); // a successful get (on a BLOB column)
	blobLength = wholeBuf->Length();
	TEST(blobLength > 0);
	CleanupStack::PopAndDestroy(wholeBuf); 
	wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table1"), _L("T"), TheDb1.LastInsertedRowId(), _L("main")); // a successful get (on a TEXT column)
	blobLength = wholeBuf->Length();
	TEST(blobLength > 0);
	CleanupStack::PopAndDestroy(wholeBuf); 
	wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table1"), _L("T"), TheDb1.LastInsertedRowId(), _L("")); // a successful get (on a TEXT column)
	blobLength = wholeBuf->Length();
	TEST(blobLength > 0);
	CleanupStack::PopAndDestroy(wholeBuf); 
	wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), KAttachedDbName); // a successful get (on a BLOB column)
	blobLength = wholeBuf->Length();
	TEST(blobLength > 0);
	CleanupStack::PopAndDestroy(wholeBuf); 
	wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table2"), _L("text"), TheDb2.LastInsertedRowId(), KAttachedDbName); // a successful get (on a TEXT column)
	blobLength = wholeBuf->Length();
	TEST(blobLength > 0);
	CleanupStack::PopAndDestroy(wholeBuf);
	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb2, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // wrong db connection
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb1, _L("invalidTableName"), _L("blob"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // invalid table name
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table2"), _L("invalidColumnName"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // invalid column name
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table2"), _L("int"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // invalid column type
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table2"), _L("blob"), 345, KAttachedDbName)); // invalid ROWID
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), _L("invalidAttachedDbName"))); // invalid attached db name
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), tooLongNameDes)); // too long attached db name
	TEST2(err, KErrBadName); 

	// TSqlBlob::Get()
	HBufC8* buf = HBufC8::NewLC(50);
	TPtr8 bufPtr(buf->Des());	  	
	err = TSqlBlob::Get(TheDb1, _L("table1"), _L("B"), bufPtr); // a successful get (on a BLOB column)
	TEST2(err, KErrNone);
	err = TSqlBlob::Get(TheDb1, _L("table1"), _L("T"), bufPtr); // a successful get (on a TEXT column)
	TEST2(err, KErrNone); 
	err = TSqlBlob::Get(TheDb2, _L("table1"), _L("B"), bufPtr); // wrong db connection
	TEST2(err, KSqlErrGeneral); 
	err = TSqlBlob::Get(TheDb1, _L(""), _L("B"), bufPtr); // empty table name
	TEST2(err, KErrBadName); 
	err = TSqlBlob::Get(TheDb1, tooLongNameDes, _L("B"), bufPtr); // too long table name
	TEST2(err, KErrBadName);
	err = TSqlBlob::Get(TheDb1, _L("invalidTableName"), _L("B"), bufPtr); // invalid table name
	TEST2(err, KSqlErrGeneral); 
	err = TSqlBlob::Get(TheDb1, _L("table1"), _L(""), bufPtr); // empty column name
	TEST2(err, KErrBadName); 
	err = TSqlBlob::Get(TheDb1, _L("table1"), tooLongNameDes, bufPtr); // too long column name
	TEST2(err, KErrBadName); 
	err = TSqlBlob::Get(TheDb1, _L("table1"), _L("invalidColumnName"), bufPtr); // invalid column name
	TEST2(err, KSqlErrGeneral); 
	err = TSqlBlob::Get(TheDb1, _L("table1"), _L("I"), bufPtr); // invalid column type
	TEST2(err, KSqlErrGeneral); 
	err = TSqlBlob::Get(TheDb1, _L("table1"), _L("B"), bufPtr, 0); // illegal ROWID
	TEST2(err, KErrArgument); 
	err = TSqlBlob::Get(TheDb1, _L("table1"), _L("B"), bufPtr, 3731); // invalid ROWID
	TEST2(err, KSqlErrGeneral); 
	err = TSqlBlob::Get(TheDb1, _L("table1"), _L("B"), bufPtr, TheDb1.LastInsertedRowId(), _L("main")); // a successful get (on a BLOB column)
	TEST2(err, KErrNone); 
	err = TSqlBlob::Get(TheDb1, _L("table1"), _L("B"), bufPtr, TheDb1.LastInsertedRowId(), _L("")); // a successful get (on a BLOB column)
	TEST2(err, KErrNone); 
	err = TSqlBlob::Get(TheDb1, _L("table1"), _L("T"), bufPtr, TheDb1.LastInsertedRowId(), _L("main")); // a successful get (on a TEXT column)
	TEST2(err, KErrNone); 
	err = TSqlBlob::Get(TheDb1, _L("table1"), _L("T"), bufPtr, TheDb1.LastInsertedRowId(), _L("")); // a successful get (on a TEXT column)
	TEST2(err, KErrNone); 
	err = TSqlBlob::Get(TheDb1, _L("table2"), _L("blob"), bufPtr, TheDb2.LastInsertedRowId(), KAttachedDbName); // a successful get (on a BLOB column)
	TEST2(err, KErrNone); 
	err = TSqlBlob::Get(TheDb1, _L("table2"), _L("text"), bufPtr, TheDb2.LastInsertedRowId(), KAttachedDbName); // a successful get (on a TEXT column)
	TEST2(err, KErrNone); 
	err = TSqlBlob::Get(TheDb2, _L("table2"), _L("blob"), bufPtr, TheDb2.LastInsertedRowId(), KAttachedDbName); // wrong db connection
	TEST2(err, KSqlErrGeneral); 
	err = TSqlBlob::Get(TheDb1, _L("invalidTableName"), _L("blob"), bufPtr, TheDb2.LastInsertedRowId(), KAttachedDbName); // invalid table name
	TEST2(err, KSqlErrGeneral); 
	err = TSqlBlob::Get(TheDb1, _L("table2"), _L("invalidColumnName"), bufPtr, TheDb2.LastInsertedRowId(), KAttachedDbName); // invalid column name
	TEST2(err, KSqlErrGeneral); 
	err = TSqlBlob::Get(TheDb1, _L("table2"), _L("int"), bufPtr, TheDb2.LastInsertedRowId(), KAttachedDbName); // invalid column type
	TEST2(err, KSqlErrGeneral); 
	err = TSqlBlob::Get(TheDb1, _L("table2"), _L("blob"), bufPtr, 345, KAttachedDbName); // invalid ROWID
	TEST2(err, KSqlErrGeneral); 
	err = TSqlBlob::Get(TheDb1, _L("table2"), _L("blob"), bufPtr, TheDb2.LastInsertedRowId(), _L("invalidAttachedDbName")); // invalid attached db name
	TEST2(err, KSqlErrGeneral); 
	err = TSqlBlob::Get(TheDb1, _L("table2"), _L("blob"), bufPtr, TheDb2.LastInsertedRowId(), tooLongNameDes); // too long attached db name
	TEST2(err, KErrBadName);
	CleanupStack::PopAndDestroy(buf);	
	
	CleanupStack::PopAndDestroy(tooLongName);
	}

void BadParamSetL()
	{
	HBufC* tooLongName = HBufC::NewLC(KMaxFileName + 1);
	TPtr tooLongNameDes = tooLongName->Des();
	tooLongNameDes.Fill('A', KMaxFileName + 1);	
	
	// TSqlBlob::SetL()
	TRAPD(err, TSqlBlob::SetL(TheDb1, _L("table1"), _L("B"), _L8("twenty characters !!"))); // a successful set (on a BLOB column)
	TEST2(err, KErrNone); 
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table1"), _L("T"), _L8("twenty characters !!"))); // a successful set (on a TEXT column)
	TEST2(err, KErrNone); 
	TRAP(err, TSqlBlob::SetL(TheDb2, _L("table1"), _L("B"), _L8("twenty characters..."))); // wrong db connection
	TEST2(err, KSqlErrGeneral);
	TRAP(err, TSqlBlob::SetL(TheDb1, _L(""), _L("B"), _L8("twenty characters..."))); // empty table name
	TEST2(err, KErrBadName); 
	TRAP(err, TSqlBlob::SetL(TheDb1, tooLongNameDes, _L("B"), _L8("twenty characters..."))); // too long table name
	TEST2(err, KErrBadName);   
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("invalidTableName"), _L("B"), _L8("twenty characters..."))); // invalid table name
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table1"), _L(""), _L8("twenty characters..."))); // empty column name
	TEST2(err, KErrBadName); 
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table1"), tooLongNameDes, _L8("twenty characters..."))); // too long column name
	TEST2(err, KErrBadName); 
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table1"), _L("invalidColumnName"), _L8("twenty characters..."))); // invalid column name
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table1"), _L("I"), _L8("twenty characters..."))); // invalid column type
	TEST2(err, KSqlErrGeneral);
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table1"), _L("B"), _L8("twenty characters..."), -3));  // illegal ROWID
	TEST2(err, KErrArgument);
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table1"), _L("B"), _L8("twenty characters..."), 1113));  // invalid ROWID
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table1"), _L("B"), _L8("!!!twenty characters"), TheDb1.LastInsertedRowId(), _L("main"))); // a successful set (on a BLOB column)
	TEST2(err, KErrNone); 
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table1"), _L("B"), _L8("!!!twenty characters"), TheDb1.LastInsertedRowId(), _L(""))); // a successful set (on a BLOB column)
	TEST2(err, KErrNone); 
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table1"), _L("T"), _L8("!!!twenty characters"), TheDb1.LastInsertedRowId(), _L("main"))); // a successful set (on a TEXT column)
	TEST2(err, KErrNone); 
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table1"), _L("T"), _L8("!!!twenty characters"), TheDb1.LastInsertedRowId(), _L(""))); // a successful set (on a TEXT column)
	TEST2(err, KErrNone); 
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table2"), _L("blob"), _L8("10 chars!!"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // a successful set (on a BLOB column)
	TEST2(err, KErrNone); 
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table2"), _L("text"), _L8("10 chars!!"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // a successful set (on a TEXT column)
	TEST2(err, KErrNone); 
	TRAP(err, TSqlBlob::SetL(TheDb2, _L("table2"), _L("blob"), _L8("10 chars.."), TheDb2.LastInsertedRowId(), KAttachedDbName)); // wrong db connection
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("invalidTableName"), _L("blob"), _L8("10 chars.."), TheDb2.LastInsertedRowId(), KAttachedDbName)); // invalid table name
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table2"), _L("invalidColumnName"), _L8("10 chars.."), TheDb2.LastInsertedRowId(), KAttachedDbName)); // invalid column name
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table2"), _L("int"), _L8("10 chars.."), TheDb2.LastInsertedRowId(), KAttachedDbName)); // invalid column type
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table2"), _L("blob"), _L8("10 chars.."), 13, KAttachedDbName)); // invalid ROWID
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table2"), _L("blob"), _L8("10 chars.."), TheDb2.LastInsertedRowId(), _L("invalidAttachedDbName"))); // invalid attached db name
	TEST2(err, KSqlErrGeneral); 	
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table2"), _L("blob"), _L8("10 chars.."), TheDb2.LastInsertedRowId(), tooLongNameDes)); // too long attached db name
	TEST2(err, KErrBadName); 
	
	CleanupStack::PopAndDestroy(tooLongName);
	}
	
void BadParamBindZeroBlobL()
	{
	// RSqlStatement::BindZeroBlob()
	RSqlBlobReadStream rdStrm;
	CleanupClosePushL(rdStrm);
	RSqlStatement stmt;
	CleanupClosePushL(stmt);
	TInt err = stmt.Prepare(TheDb2, _L("INSERT INTO table2 values(1, 'dummy text', :Val)"));
	TEST2(err, KErrNone);
	TInt paramIndex = stmt.ParameterIndex(_L(":Val"));
	TEST(paramIndex >= 0);	
	err = stmt.BindZeroBlob(paramIndex, -1); // a negative blob size
	TEST2(err, KErrNone);
	err = stmt.Exec();
	TEST2(err, 1);
	stmt.Reset();
	rdStrm.OpenL(TheDb2, _L("table2"), _L("blob"));
	TInt size = rdStrm.SizeL(); // check the blob's size is 0 (0 is used if a negative number was specified)
	TEST2(size, 0);
	rdStrm.Close();
	err = stmt.BindZeroBlob(paramIndex, 0); // a blob size of zero
	TEST2(err, KErrNone);
	err = stmt.Exec();
	TEST2(err, 1);
	stmt.Reset();
	rdStrm.OpenL(TheDb2, _L("table2"), _L("blob"));
	size = rdStrm.SizeL(); // check the blob's size is 0
	TEST2(size, 0);
	// For subsequent test purposes make the last inserted record have a zeroblob > 0 size
	err = stmt.BindZeroBlob(paramIndex, 1);
	TEST2(err, KErrNone);
	err = stmt.Exec();
	TEST2(err, 1);
	CleanupStack::PopAndDestroy(&stmt);		
	CleanupStack::PopAndDestroy(&rdStrm);	
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-4107
@SYMTestCaseDesc		Bad parameter tests for the methods of RSqlBlobReadStream, 
						RSqlBlobWriteStream, TSqlBlob and RSqlStatement::BindZeroBlob().
						Tests that the correct error code is returned when a bad parameter
						is used in a call to one of the methods.
						Tests the RSqlBlobReadStream, RSqlBlobWriteStream and TSqlBlob methods
						and RSqlStatement::BindZeroBlob().
@SYMTestPriority		High
@SYMTestActions			Execution of bad parameter tests.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10411
                        REQ10418
*/
void BadParamTestL()
	{
	BadParamReadStreamL();	
	BadParamWriteStreamL();	
	BadParamGetL();
	BadParamSetL();	
	BadParamBindZeroBlobL();
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4108
@SYMTestCaseDesc		Indexed column tests for the methods of RSqlBlobReadStream
						and RSqlBlobWriteStream, to ensure that a blob or text column
						that is indexed cannot be written to but can be read from 
						(an SQLite restriction).
@SYMTestPriority		Medium
@SYMTestActions			Execution of read and write operations on an indexed column in a database.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10411
                        REQ10418
*/	
void IndexedColumnTestL()
	{
	// Create an index on the BLOB column and on the TEXT column in the main and attached databases
	CreateIndices();

	// Attempt to open a write stream on an indexed BLOB and an indexed TEXT column - this should not be permitted.
	// (This is an SQLite restriction, but having an index on a large BLOB or a TEXT column is highly unlikely!)
	RSqlBlobWriteStream wrStrm;
	CleanupClosePushL(wrStrm);
	TRAPD(err, wrStrm.OpenL(TheDb1, _L("table1"), _L("B"))); // indexed BLOB column
	TEST2(err, KSqlErrGeneral);
	wrStrm.Close();
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table1"), _L("T"))); // indexed TEXT column
	TEST2(err, KSqlErrGeneral);
	wrStrm.Close();
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // indexed BLOB column
	TEST2(err, KSqlErrGeneral);
	wrStrm.Close();
	TRAP(err, wrStrm.OpenL(TheDb1, _L("table2"), _L("text"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // indexed TEXT column
	TEST2(err, KSqlErrGeneral);
	CleanupStack::PopAndDestroy(&wrStrm);
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table1"), _L("B"), _L8("twenty characters !!"))); // indexed BLOB column
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table1"), _L("T"), _L8("twenty characters !!"))); // indexed TEXT column
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table2"), _L("blob"), _L8("10 chars!!"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // indexed BLOB column
	TEST2(err, KSqlErrGeneral); 
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table2"), _L("text"), _L8("10 chars!!"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // indexed TEXT column
	TEST2(err, KSqlErrGeneral);
	
	// Attempt to open a read stream on an indexed BLOB and an indexed text COLUMN - this should be permitted
	RSqlBlobReadStream rdStrm;
	CleanupClosePushL(rdStrm);
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table1"), _L("B"))); // a successful open (on an indexed BLOB column)
	TEST2(err, KErrNone);
	rdStrm.Close();
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table1"), _L("T"))); // a successful open (on an indexed TEXT column)
	TEST2(err, KErrNone);
	rdStrm.Close();
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // a successful open (on an indexed BLOB column)
	TEST2(err, KErrNone);
	rdStrm.Close();
	TRAP(err, rdStrm.OpenL(TheDb1, _L("table2"), _L("text"), TheDb2.LastInsertedRowId(), KAttachedDbName)); // a successful open (on an indexed TEXT column)
	TEST2(err, KErrNone);
	CleanupStack::PopAndDestroy(&rdStrm);
	HBufC8* wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table1"), _L("B")); // a successful get (on an indexed BLOB column)
	TInt blobLength = wholeBuf->Length();
	TEST(blobLength > 0);
	CleanupStack::PopAndDestroy(wholeBuf); 
	wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table1"), _L("T")); // a successful get (on an indexed TEXT column)
	blobLength = wholeBuf->Length();
	TEST(blobLength > 0);
	CleanupStack::PopAndDestroy(wholeBuf); 
	wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table2"), _L("blob"), TheDb2.LastInsertedRowId(), KAttachedDbName); // a successful get (on an indexed BLOB column)
	blobLength = wholeBuf->Length();
	TEST(blobLength > 0);
	CleanupStack::PopAndDestroy(wholeBuf); 	
	wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table2"), _L("text"), TheDb2.LastInsertedRowId(), KAttachedDbName); // a successful get (on an indexed TEXT column)
	blobLength = wholeBuf->Length();
	TEST(blobLength > 0);
	CleanupStack::PopAndDestroy(wholeBuf);
	HBufC8* buf = HBufC8::NewLC(50);
	TPtr8 buffPtr(buf->Des());	  	
	err = TSqlBlob::Get(TheDb1, _L("table1"), _L("B"), buffPtr); // a successful get (on an indexed BLOB column)
	TEST2(err, KErrNone); 
	err = TSqlBlob::Get(TheDb1, _L("table1"), _L("T"), buffPtr); // a successful get (on an indexed TEXT column)
	TEST2(err, KErrNone); 
	err = TSqlBlob::Get(TheDb1, _L("table2"), _L("blob"), buffPtr, TheDb2.LastInsertedRowId(), KAttachedDbName); // a successful get (on an indexed BLOB column)
	TEST2(err, KErrNone); 
	err = TSqlBlob::Get(TheDb1, _L("table2"), _L("text"), buffPtr, TheDb2.LastInsertedRowId(), KAttachedDbName); // a successful get (on an indexed TEXT column)
	TEST2(err, KErrNone);
	CleanupStack::PopAndDestroy(buf);
	
	RemoveIndices();
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-4109
@SYMTestCaseDesc		'End of file' tests for the methods of RSqlBlobReadStream
						RSqlBlobWriteStream and TSqlBlob, to ensure that a client cannot
						read beyond the end of a blob object or write beyond the end of a 
						blob object and that an appropriate error code is returned.
@SYMTestPriority		High
@SYMTestActions			Execution of read and write operations beyond the end of a blob object.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10411
                        REQ10418
*/	
void EofTestL()
	{
	// Use Blob 3 in table1, whose value is "!!!twenty characters"
	
	// Read Eof tests

	const TInt KBlobSize = 20;	
	RSqlBlobReadStream rdStrm;
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TInt size = rdStrm.SizeL(); 
	TEST2(size, KBlobSize);
	TBuf8<50> data;
	TRAPD(err, rdStrm.ReadL(data, KBlobSize));
	TEST2(err, KErrNone); 
	TEST(data.Compare(_L8("!!!twenty characters")) == 0);
	TRAP(err, rdStrm.ReadL(data, 1));
	TEST2(err, KErrEof);
	rdStrm.Close();
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TRAP(err, rdStrm.ReadL(data, 21));
	TEST2(err, KErrEof); 
	rdStrm.Close();
	
	HBufC8* exactSizeBuf = HBufC8::NewLC(KBlobSize);	
	TPtr8 exactSizeBufPtr(exactSizeBuf->Des());	  	  
	err = TSqlBlob::Get(TheDb1, _L("table1"), _L("B"), exactSizeBufPtr);
	TEST2(err, KErrNone); 
	TEST(exactSizeBufPtr.Compare(_L8("!!!twenty characters")) == 0);
	CleanupStack::PopAndDestroy(exactSizeBuf);	
	
	HBufC8* tooSmallBuf = HBufC8::NewLC(KBlobSize/2);	
	TPtr8 tooSmallBufPtr(tooSmallBuf->Des());	  	  			  		  	  
	err = TSqlBlob::Get(TheDb1, _L("table1"), _L("B"), tooSmallBufPtr);
	TEST2(err, KErrOverflow); 	
	CleanupStack::PopAndDestroy(tooSmallBuf);	
	
	// Write Eof tests
	
	RSqlBlobWriteStream wrStrm;
	CleanupClosePushL(wrStrm);
	wrStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	size = wrStrm.SizeL();
	TEST2(size, KBlobSize);
	// For debug builds: This 20 bytes will be written to the blob straight away
	// because 20 bytes is more than the 8 byte buffer
	// For release builds: This 20 bytes will NOT be written to the blob straight away
	// because there is room for it to be stored in the 1.5K byte buffer
	TRAP(err, wrStrm.WriteL(_L8("a twenty char string"))); 
	TEST2(err, KErrNone);
	// For debug/release builds: This 1 byte will NOT be written to the blob straight away
	// because there is room for it to be stored in the 8 byte/1.5K buffer
	TRAP(err, wrStrm.WriteL(_L8("a")));
	TEST2(err, KErrNone);
	// For debug builds: The CommitL() call will cause an attempt to write the 1 byte 
	// in the buffer to the blob - however, the stream's write position indicates 
	// that the write would be beyond the end of the blob and so an error occurs
	// For release builds: The CommitL() call will cause an attempt to write the 21 bytes 
	// in the buffer to the blob - however, 21 bytes is larger than the size of the blob
	// and so an error occurs
	TRAP(err, wrStrm.CommitL());
	TEST2(err, KErrEof);	
	wrStrm.Close();
	// For debug builds: Check that the blob value is now "a twenty char string"
	// For release builds: Check that the blob value is still "!!!twenty characters"
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TRAP(err, rdStrm.ReadL(data, KBlobSize));
	TEST2(err, KErrNone); 
#ifdef _DEBUG	
	TEST(data.Compare(_L8("a twenty char string")) == 0);
#else	
	TEST(data.Compare(_L8("!!!twenty characters")) == 0);
#endif
	rdStrm.Close();
	
	TheDb1.Exec(_L("BEGIN"));
	wrStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	// For debug/release builds: These 3 bytes will be stored in the 8 byte/1.5K buffer
	TRAP(err, wrStrm.WriteL(_L8("eee"))); 
	TEST2(err, KErrNone);
	// For debug builds: These 25 bytes will not fit in the 8 byte buffer and so an attempt 
	// will be made to write these 25 bytes and the 3 buffered bytes to the blob - 
	// however, the size of the blob is only 20 and so an error occurs
	// For release builds: These 25 bytes will be stored in the 1.5K buffer
	TRAP(err, wrStrm.WriteL(_L8("fffffffffffffffffffffffff"))); 
#ifdef _DEBUG
	TEST2(err, KErrEof);
	wrStrm.Close();
	TheDb1.Exec(_L("ROLLBACK"));

	// Check that the blob value is still "a twenty char string"
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TRAP(err, rdStrm.ReadL(data, KBlobSize));
	TEST2(err, KErrNone); 
	TEST(data.Compare(_L8("a twenty char string")) == 0);
	rdStrm.Close();
#else
	TEST2(err, KErrNone);
	// For release builds: The CommitL() call will cause an attempt to write the 28 bytes 
	// in the buffer to the blob - however, 28 bytes is larger than the size of the blob
	// and so an error occurs
	TRAP(err, wrStrm.CommitL());
	TEST2(err, KErrEof);
	wrStrm.Close();
	TheDb1.Exec(_L("ROLLBACK"));
	
	// Check that the blob value is still "!!!twenty characters"
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TRAP(err, rdStrm.ReadL(data, KBlobSize));
	TEST2(err, KErrNone); 
	TEST(data.Compare(_L8("!!!twenty characters")) == 0);
	rdStrm.Close();
#endif

	TheDb1.Exec(_L("BEGIN"));
	wrStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TRAP(err, wrStrm.WriteL(_L8("a string that is longer than 20 characters"))); 	
#ifdef _DEBUG	
	TEST2(err, KErrEof);
	wrStrm.Close();
	TheDb1.Exec(_L("ROLLBACK"));
	wrStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TRAP(err, wrStrm.WriteL(_L8("ggg"))); 
	TEST2(err, KErrNone);
	TRAP(err, wrStrm.CommitL());
	TEST2(err, KErrNone);
	CleanupStack::PopAndDestroy(&wrStrm);	
	// Check that the blob value is now "gggwenty char string"
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TRAP(err, rdStrm.ReadL(data, KBlobSize));
	TEST2(err, KErrNone); 
	TEST(data.Compare(_L8("gggwenty char string")) == 0);
	rdStrm.Close();
#else
	TEST2(err, KErrNone);
	TRAP(err, wrStrm.CommitL());
	TEST2(err, KErrEof);
	wrStrm.Close();
	TheDb1.Exec(_L("ROLLBACK"));
	wrStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TRAP(err, wrStrm.WriteL(_L8("hhh"))); 
	TEST2(err, KErrNone);
	TRAP(err, wrStrm.CommitL());
	TEST2(err, KErrNone);
	CleanupStack::PopAndDestroy(&wrStrm);		
	// Check that the blob value is now "hhhtwenty characters"
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TRAP(err, rdStrm.ReadL(data, KBlobSize));
	TEST2(err, KErrNone); 
	TEST(data.Compare(_L8("hhhtwenty characters")) == 0);
	rdStrm.Close();
#endif

	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table1"), _L("B"), _L8("a twenty char string")));
	TEST2(err, KErrNone); 
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TRAP(err, rdStrm.ReadL(data, KBlobSize));
	TEST2(err, KErrNone); 
	TEST(data.Compare(_L8("a twenty char string")) == 0);
	rdStrm.Close();

	TheDb1.Exec(_L("BEGIN"));
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table1"), _L("B"), _L8("a string that is longer than 20 characters")));
	TEST2(err, KErrEof);
	TheDb1.Exec(_L("ROLLBACK"));
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TRAP(err, rdStrm.ReadL(data, KBlobSize));
	TEST2(err, KErrNone); 
	TEST(data.Compare(_L8("a twenty char string")) == 0);
	rdStrm.Close();
	
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table1"), _L("B"), _L8("less than 20")));
	TEST2(err, KErrNone); 
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	TRAP(err, rdStrm.ReadL(data, KBlobSize));
	TEST2(err, KErrNone); // changing only part of the blob data is permitted
	TEST(data.Compare(_L8("less than 20r string")) == 0);
	rdStrm.Close();

	CleanupStack::PopAndDestroy(&rdStrm);	
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-4110
@SYMTestCaseDesc		General blob read and write tests on a private secure database,
						to ensure that there are no security issues with the methods of 
						RSqlBlobReadStream, RSqlBlobWriteStream and TSqlBlob.
@SYMTestPriority		High
@SYMTestActions			Execution of read and write operations on a private secure database.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10411
                        REQ10418
						REQ5794
*/	
void PrivateSecureDbTestL()
	{
	// The blob has the value 'ABABABABABABABA'
	
	// Read data from the blob
	const TInt KBlobSize = 15;
	RSqlBlobReadStream rdStrm;
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(ThePrivateDb, _L("table3"), _L("picture"));
	TInt size = rdStrm.SizeL(); // check the blob's size
	TEST2(size, KBlobSize); 
	TBuf8<50> data;
	rdStrm.ReadL(data, 5);
	TEST(data.Compare(_L8("ABABA")) == 0); // check 5 bytes have been read
	rdStrm.ReadL(data, 8);
	TEST(data.Compare(_L8("BABABABA")) == 0);	// check the next 8 bytes have been read
	rdStrm.SizeL(); // check the blob's size
	TEST2(size, KBlobSize); 
	CleanupStack::PopAndDestroy(&rdStrm);

	HBufC8* wholeBuf = TSqlBlob::GetLC(ThePrivateDb, _L("table3"), _L("picture"));
	TInt blobLength = wholeBuf->Length();
	TEST2(blobLength, KBlobSize);
	TEST(wholeBuf->Des().Compare(_L8("ABABABABABABABA")) == 0);	
	CleanupStack::PopAndDestroy(wholeBuf); 
	
	HBufC8* buf = HBufC8::NewLC(KBlobSize);	
	TPtr8 bufPtr(buf->Des());	  
	TInt err = TSqlBlob::Get(ThePrivateDb, _L("table3"), _L("picture"), bufPtr);
	TEST2(err, KErrNone); 
	TEST(bufPtr.Compare(_L8("ABABABABABABABA")) == 0);	
	CleanupStack::PopAndDestroy(buf); 
	
	// Write data to the blob 
	RSqlBlobWriteStream wrStrm;
	CleanupClosePushL(wrStrm);
	wrStrm.OpenL(ThePrivateDb, _L("table3"), _L("picture"));
	size = wrStrm.SizeL(); // check the blob's size
	TEST2(size, KBlobSize); 
	wrStrm.WriteL(_L8("AABBCC")); // write 6 bytes
	wrStrm.WriteL(_L8("DD")); // write another 2 bytes
	wrStrm.WriteL(_L8("EEFFG")); // write another 5 bytes
	size = wrStrm.SizeL(); // check the blob's size
	TEST2(size, KBlobSize); 
	wrStrm.CommitL();
	CleanupStack::PopAndDestroy(&wrStrm);
	
	// Check that the new blob data was written
	HBufC8* retrievedDataBuf = TSqlBlob::GetLC(ThePrivateDb, _L("table3"), _L("picture"));
	blobLength = retrievedDataBuf->Size();
	TEST2(blobLength, KBlobSize);
	TEST(retrievedDataBuf->Des().Compare(_L8("AABBCCDDEEFFGBA")) == 0);		
	CleanupStack::PopAndDestroy(retrievedDataBuf);
	
	HBufC8* dataBuf = HBufC8::NewLC(KBlobSize);
	TPtr8 dataPtr(dataBuf->Des());
	dataPtr.Append(_L8("CDCDCDCDCDCDCDC"));
	TSqlBlob::SetL(ThePrivateDb, _L("table3"), _L("picture"), dataPtr);
	CleanupStack::PopAndDestroy(dataBuf); 
	
	// Check that the new blob data was written
	retrievedDataBuf = TSqlBlob::GetLC(ThePrivateDb, _L("table3"), _L("picture"));
	blobLength = retrievedDataBuf->Size();
	TEST2(blobLength, KBlobSize);
	TEST(retrievedDataBuf->Des().Compare(_L8("CDCDCDCDCDCDCDC")) == 0);		
	CleanupStack::PopAndDestroy(retrievedDataBuf);
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-4111
@SYMTestCaseDesc		Concurrent blob read and write tests using the 
						methods of RSqlBlobReadStream and RSqlBlobWriteStream.
						Tests that read and write operations on different blobs
						can happen concurrently and that read operations on the
						same blob from different streams can happen concurrently.
@SYMTestPriority		Medium
@SYMTestActions			Execution of concurrent blob read and write operations on a database.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10411
                        REQ10418
*/	
void ConcurrentReadAndWriteTestL()
	{	
	// Insert a zeroblob of size 2Kb
	InsertSQLiteZeroBlob(KLargeDataBufLen);
	// Insert a zeroblob of size 4Kb 
	InsertBindZeroBlob(4 * 1024);
	
	// Handles on different blobs
	
	// Write and read from different blobs
	RSqlBlobReadStream rdStrm;
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"), TheDb1.LastInsertedRowId() - 1);

	RSqlBlobWriteStream wrStrm;
	CleanupClosePushL(wrStrm);
	wrStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	
	wrStrm.WriteL(_L8("GHIJKL")); // blob2 is not updated in cache (as client buffer not full)
	TBuf8<100> data;
	rdStrm.ReadL(data, 6);
	_LIT8(KSixZeros, "\x0\x0\x0\x0\x0\x0");
	TEST(data.Compare(KSixZeros) == 0);		
	wrStrm.CommitL(); // blob2 update is not committed (as the rdStrm handle is open)
	wrStrm.Close();
	rdStrm.ReadL(data, 6);
	TEST(data.Compare(KSixZeros) == 0);	
	rdStrm.Close();	// the blob2 update is committed
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"), TheDb1.LastInsertedRowId() - 1);
	rdStrm.ReadL(data, 2); // read 2 bytes
	_LIT8(KTwoZeros, "\x0\x0");
	TEST(data.Compare(KTwoZeros) == 0);	
	wrStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	wrStrm.WriteL(TheLargeData); // blob2 is updated in the cache	
	rdStrm.ReadL(data, 10); // read the next 10 bytes
	_LIT8(KTenZeros, "\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0");
	TEST(data.Compare(KTenZeros) == 0);	
	rdStrm.Close();
	wrStrm.CommitL(); // the blob2 update is committed
	CleanupStack::PopAndDestroy(2);
	
	// Write to different blobs via different streams
	CleanupClosePushL(wrStrm);
	wrStrm.OpenL(TheDb1, _L("table1"), _L("B"), TheDb1.LastInsertedRowId() - 1);

	RSqlBlobWriteStream wrStrm2;
	CleanupClosePushL(wrStrm2);
	wrStrm2.OpenL(TheDb1, _L("table1"), _L("B"));
	
	wrStrm.WriteL(_L8("ABABABABABABAB"));
	wrStrm2.WriteL(_L8("CDCDCD"));
	wrStrm.WriteL(_L8("EFEF"));
	wrStrm.Close();
	wrStrm2.Close(); // the blob1 update is committed and the blob2 update is committed 
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"), TheDb1.LastInsertedRowId() - 1);
	rdStrm.ReadL(data, 18);
	TEST(data.Compare(_L8("ABABABABABABABEFEF")) == 0);	
	rdStrm.Close();
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	rdStrm.ReadL(data, 6);
	TEST(data.Compare(_L8("CDCDCD")) == 0);	
	rdStrm.Close();
	CleanupStack::PopAndDestroy(3); 
	
	// Read from different blobs via different streams
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"), TheDb1.LastInsertedRowId() - 1);

	RSqlBlobReadStream rdStrm2;
	CleanupClosePushL(rdStrm2);
	rdStrm2.OpenL(TheDb1, _L("table1"), _L("B"));

	rdStrm.ReadL(data, 2);
	TEST(data.Compare(_L8("AB")) == 0);		
	rdStrm2.ReadL(data, 3);
	TEST(data.Compare(_L8("CDC")) == 0);	
	rdStrm.ReadL(data, 15);
	TEST(data.Compare(_L8("ABABABABABABEFE")) == 0);		
	rdStrm2.ReadL(data, 2);
	TEST(data.Compare(_L8("DC")) == 0);	
	CleanupStack::PopAndDestroy(2); 
	
	// Handles on the same blob
	// NOTE: using different stream objects on the same blob is only
	// safe when all of the stream objects are read streams - writing to 
	// the same blob from different streams or writing and reading from 
	// the same blob at the same time has undefined behaviour
	
	// Read from the same blob (blob2) via different streams
	CleanupClosePushL(wrStrm);
	wrStrm.OpenL(TheDb1, _L("table1"), _L("B"));
	wrStrm.WriteL(_L8("MNOPQR"));
	CleanupStack::PopAndDestroy(); // the blob2 update is committed 

	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb1, _L("table1"), _L("B"));

	CleanupClosePushL(rdStrm2);
	rdStrm2.OpenL(TheDb1, _L("table1"), _L("B"));

	rdStrm.ReadL(data, 2);
	TEST(data.Compare(_L8("MN")) == 0);		
	rdStrm2.ReadL(data, 3);
	TEST(data.Compare(_L8("MNO")) == 0);	
	rdStrm.ReadL(data, 10);
	TEST(data.Compare(_L8("OPQRZZZZZZ")) == 0);		
	rdStrm2.ReadL(data, 15);
	TEST(data.Compare(_L8("PQRZZZZZZZZZZZZ")) == 0);	
	CleanupStack::PopAndDestroy(2); 
	}
	
void UTF16TextL(TInt aTextSize)
	{
	// The text value is "test", size 8 bytes in UTF-16, aTextSize = 8
	
	// Try to get the whole content of the text as UTF-8, using TSqlBlob::GetLC()
	HBufC8* wholeBuf = TSqlBlob::GetLC(TheDb1, _L("table1"), _L("T"));
	TInt bufSize = wholeBuf->Size(); // get the number of bytes in the buffer
	TEST2(bufSize, aTextSize);
	CleanupStack::PopAndDestroy(wholeBuf); 
	
	// Try to get the whole content of the text as UTF-8, using TSqlBlob::Get()
	HBufC8* buf = HBufC8::NewLC(aTextSize);	
	TPtr8 bufPtr(buf->Des());	  
	TInt err = TSqlBlob::Get(TheDb1, _L("table1"), _L("T"), bufPtr);
	TEST2(err, KErrNone); 
	bufSize = buf->Size(); // get the number of bytes in the buffer
	TEST2(bufSize, aTextSize);
	CleanupStack::PopAndDestroy(buf); 

	// Try to set the whole content of the text as UTF-8, using TSqlBlob::SetL()
	HBufC8* dataBuf = HBufC8::NewLC(aTextSize);
	TPtr8 dataPtr(dataBuf->Des());
	dataPtr.Append(_L8("OPOPOPOP"));
	TRAP(err, TSqlBlob::SetL(TheDb1, _L("table1"), _L("T"), dataPtr));
	TEST2(err, KErrNone); // can set 8 UTF-8 characters as this is 8 bytes
	CleanupStack::PopAndDestroy(dataBuf); 	
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4112
@SYMTestCaseDesc		UTF-16 text read and write tests using the UTF-8 methods of 
						TSqlBlob.
@SYMTestPriority		Medium
@SYMTestActions			Execution of UTF-16 text read and write operations using 
						UTF-8 descriptors.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10411
                        REQ10418
*/
void UTF16FormatTestL()
	{	
	// Insert a record with a UTF-16 text value (text is encoded in UTF-16 by default) 
	TInt err = TheDb1.Exec(_L("INSERT INTO table1 VALUES(1, 'test', x'46474647464746474647')"));
	TEST2(err, 1);
	const TInt KTextSize = 8; // 8 bytes (4 UTF-16 characters)
	
	// Now read and write the UTF-16 text value using UTF-8 methods
	UTF16TextL(KTextSize);	
	}
	
/**
@SYMTestCaseID			SYSLIB-SQL-UT-4114
@SYMTestCaseDesc		Storing a big blob test, to ensure that by using the methods 
						of RSqlBlobReadStream and RSqlBlobWriteStream larger blobs can 
						be stored in practice than in previous versions of Symbian SQL.
						Creates a 18Mb zeroblob and then writes data into it and reads
						the data back.
						Also tests the TSqlBlob APIs to store and retrieve a large blob.
						Note that the test will use 18Mb blob only in WINSCW builds.
						Otherwise the used blob size is 3Mb.
@SYMTestPriority		Medium
@SYMTestActions			Execution of creating a 18Mb zeroblob, writing data to it and 
						reading it back.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ10410
*/
void BigBlobTestL()
	{
	// In this test we create a zeroblob big enough to hold a 18MB blob.
	// 18MB is larger than the server could previously read or write,
	// due to the server heap limit of 6MB (WINSCW builds).
	// This test will prove that a 18MB blob can be written and read
	// using the new APIs but not with the old APIs
		
#if defined __WINS__ ||	defined __WINSCW__
	const TInt KBigBlobSize = 18 * 1024 * 1024;
#else
	const TInt KBigBlobSize = 3 * 1024 * 1024;
#endif
		
	// Create a zeroblob big enough to hold a 36MB blob
	TInt err = TheDb2.Exec(_L("BEGIN"));
	TEST(err >= 0);
	
	RSqlStatement stmt;
	err = stmt.Prepare(TheDb2, _L("INSERT INTO table2 values(435, 'big blob test', :Val)"));
	TEST2(err, KErrNone);
	TInt paramIndex = stmt.ParameterIndex(_L(":Val"));
	TEST(paramIndex >= 0);	
	err = stmt.BindZeroBlob(paramIndex, KBigBlobSize);
	TEST2(err, KErrNone);
	err = stmt.Exec();	
	stmt.Close();
	if(err == KErrDiskFull)	
		{
		(void)TheDb2.Exec(_L("ROLLBACK"));
		TheTest.Printf(_L("==== The disk is full. The test cannot be completed!\r\n"));
		RFs fs;
		err = fs.Connect();
		if(err == KErrNone)
			{
			TVolumeInfo vinfo;
			err = fs.Volume(vinfo, EDriveC);
			if(err == KErrNone)
				{
				TheTest.Printf(_L("==== Free disk space=%d\r\n"), vinfo.iFree);
				}
			fs.Close();
			}
		return;	
		}
	TEST2(err, 1);
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// Fill a buffer with KBigBlobSize/KBlobPartCnt bytes of data 
	// (the test application's heap may be too small to allocate a KBigBlobSize bytes buffer)
    const TInt KBlobWrPartCnt = 16;// 1/16 part of the blob will be written at once using streaming API
	const TInt KBufferSize1 = KBigBlobSize / KBlobWrPartCnt;
	HBufC8* blobWrBuf = HBufC8::NewLC(KBufferSize1);
	TPtr8 blobWrChunk(blobWrBuf->Des());
	blobWrChunk.Fill('Z', KBufferSize1);	
	
	// Write KBigBlobSize bytes to the blob in the inserted record
	RSqlBlobWriteStream wrStrm;
	CleanupClosePushL(wrStrm);
	wrStrm.OpenL(TheDb2, _L("table2"), _L("blob"));
	TInt size = wrStrm.SizeL(); // check the blob's size
	TEST2(size, KBigBlobSize);
	for(TInt i1=0;i1<KBlobWrPartCnt;++i1)
	    {
        TRAP(err, wrStrm.WriteL(blobWrChunk)); // write KBufferSize1 bytes of data
        TEST2(err, KErrNone);
	    }
	wrStrm.CommitL();
	CleanupStack::PopAndDestroy(&wrStrm);
	
	err = TheDb2.Exec(_L("COMMIT"));
	TEST2(err, 1);
	
    /////////////////////////////////////////////////////////////////////////////////////////////////////
	// Read the big blob value back from the record in KBigBlobSize/6 chunks
    const TInt KBlobRdPartCnt = 24;// 1/24 part of the blob will be read at once using streaming API
	const TInt KBufferSize2 = KBigBlobSize / KBlobRdPartCnt;
	HBufC8* blobRdBuf = HBufC8::NewLC(KBufferSize2);
	TPtr8 blobRdBufPtr(blobRdBuf->Des());
	RSqlBlobReadStream rdStrm;
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(TheDb2, _L("table2"), _L("blob"));
	size = rdStrm.SizeL(); // check the blob's size
	TEST2(size, KBigBlobSize);
	for(TInt i2=0;i2<KBlobRdPartCnt;++i2)
	    {
        rdStrm.ReadL(blobRdBufPtr, KBufferSize2);
        TEST(blobRdBufPtr.Compare(blobWrChunk.Left(KBufferSize2)) == 0); // check the first KBigBlobSize/KBlobRdPartCnt bytes
	    }
	TRAP(err, rdStrm.ReadL(blobRdBufPtr, 1));
	TEST2(err, KErrEof); // check that there is no more data to be read
	CleanupStack::PopAndDestroy(2, blobRdBuf); // rdStrm, blobRdBuf
		
    /////////////////////////////////////////////////////////////////////////////////////////////////////
	// Try to read the whole KBigBlobSize blob value using the old API
	err = stmt.Prepare(TheDb2, _L("SELECT blob FROM table2 WHERE ROWID == :Val"));
	TEST2(err, KErrNone);
	paramIndex = stmt.ParameterIndex(_L(":Val"));
	TEST(paramIndex >= 0);	
	err = stmt.BindInt(paramIndex, TheDb2.LastInsertedRowId());
	TEST2(err, KErrNone);
	// Check that the blob retrieval fails (because there is
	// not enough server-side memory to load it into the VDBE)
	err = stmt.Next();
#if defined __WINS__ ||	defined __WINSCW__
	TEST2(err, KErrNoMemory);
#else	
	TEST2(err, KSqlAtRow);
#endif
	stmt.Close();
	
    /////////////////////////////////////////////////////////////////////////////////////////////////////
	// Try to write another KBigBlobSize bytes big blob value using the old API.
    // Check that the at some point the blob write fails (because there is
    // not enough server-side memory to store the whole KBigBlobSize bytes).
	err = stmt.Prepare(TheDb2, _L("INSERT INTO table2 values(99, 'text', :Val)"));
	TEST2(err, KErrNone);	
	paramIndex = stmt.ParameterIndex(_L(":Val"));
	TEST(paramIndex >= 0);
	RSqlParamWriteStream strm;
	err = strm.BindBinary(stmt, paramIndex);
	TEST2(err, KErrNone);
	for(TInt i3=0;i3<KBlobWrPartCnt && err==KErrNone;++i3)
	    {
	    TRAP(err, strm.WriteL(blobWrChunk, KBufferSize1)); 
	    }
#if defined __WINS__ ||	defined __WINSCW__
	TEST2(err, KErrNoMemory);
#else	
	TEST2(err, KErrNone);
#endif
	strm.Close();
	stmt.Close();
	CleanupStack::PopAndDestroy(blobWrBuf);
	blobWrBuf = NULL;

    /////////////////////////////////////////////////////////////////////////////////////////////////////
	// Use the TSqlBlob APIs to insert another big blob of size 85Kb
	// (to test the block algorithm used by TSqlBlob 'set')
	const TInt KBigBlobSize2 = 85 * 1024;
	err = TheDb2.Exec(_L("BEGIN"));
    TEST(err >= 0);
	
	err = stmt.Prepare(TheDb2, _L("INSERT INTO table2 values(189, 'another big blob', :Val)"));
	TEST2(err, KErrNone);
	paramIndex = stmt.ParameterIndex(_L(":Val"));
	TEST(paramIndex >= 0);	
	err = stmt.BindZeroBlob(paramIndex, KBigBlobSize2);
	TEST2(err, KErrNone);
	err = stmt.Exec();	
	TEST2(err, 1);
	stmt.Close();

	blobWrBuf = HBufC8::NewLC(KBigBlobSize2);
	blobWrChunk.Set(blobWrBuf->Des());
	blobWrChunk.SetLength(KBigBlobSize2);
    blobWrChunk.Fill('F');
    TPtr8 p((TUint8*)blobWrChunk.Ptr() + blobWrChunk.Length() / 2, blobWrChunk.Length() / 2);
    p.Fill('E');// blobWrBuf now contains 42.5Kb of 'E's followed by 42.5Kb of 'F's

	TRAP(err, TSqlBlob::SetL(TheDb2, _L("table2"), _L("blob"), blobWrChunk));
	TEST2(err, KErrNone);
	
	err = TheDb2.Exec(_L("COMMIT"));
	TEST2(err, 1);

	// Read the blob value back from the record	
	// (to test the block algorithm used by TSqlBlob 'get')
	HBufC8* buf = TSqlBlob::GetLC(TheDb2, _L("table2"), _L("blob"));
	TEST(buf->Des().Compare(blobWrChunk) == 0);
    CleanupStack::PopAndDestroy(buf);
    buf = NULL;
	
	buf = HBufC8::NewLC(KBigBlobSize2);
	blobRdBufPtr.Set(buf->Des());
	err = TSqlBlob::Get(TheDb2, _L("table2"), _L("blob"), blobRdBufPtr);	
	TEST2(err, KErrNone);
	TEST(blobRdBufPtr.Compare(blobWrChunk) == 0);
	
	CleanupStack::PopAndDestroy(2, blobWrBuf); // buf, blobWrBuf
	}
	
/**
@SYMTestCaseID          PDS-SQL-CT-4194
@SYMTestCaseDesc        The test opens a test database, creates a table with a blob column and inserts one record.
						Then the test uses RSqlBlobWriteStream to modify the blob column content.
                        MStreamBuf::SeekL() is used to modify the blob data at specific positions.
                        Then the test uses RSqlBlobReadStream object to read the just written blob data.
                        MStreamBuf::SeekL() is used to read the column content at specific positions 
                        (the same positions used during the blob write operation). The read byte values must
                        match the written byte values.
@SYMTestPriority        High
@SYMTestActions         RSqlBlobReadStream and RSqlBlobWriteStream - MStreamBuf::SeekL() test.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF145028
*/  
void StreamSeekTestL()
	{
    TInt rc = TheDb1.Exec(_L("CREATE TABLE A(Fld1 INTEGER, Fld2 BLOB)"));
    TEST(rc >= 0);
    
    //Write a record to the database using a blob stream. MStreamBuf::SeekL() is used to modify the content at a specific position.
    rc = TheDb1.Exec(_L("INSERT INTO A(Fld1, Fld2) VALUES(1, zeroblob(256))"));
    TEST2(rc, 1);
    RSqlBlobWriteStream strm1;
    CleanupClosePushL(strm1);
    strm1.OpenL(TheDb1, _L("A"), _L("Fld2"));
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
    
    strm1buf->SeekL(MStreamBuf::EWrite, EStreamEnd, -KData().Length());
    strm1buf->WriteL(KData().Ptr(), KData().Length());
    
    strm1buf->SeekL(MStreamBuf::EWrite, EStreamEnd, -4 * KStreamOffset);
    strm1buf->WriteL(&KByte, 1);
    
    strm1.CommitL();
    CleanupStack::PopAndDestroy(&strm1);
    
    //Read the record using a blob stream. MStreamBuf::SeekL() is used to read the content at a specific position.
    RSqlBlobReadStream strm2;
    CleanupClosePushL(strm2);
    strm2.OpenL(TheDb1, _L("A"), _L("Fld2"));

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
	}

void DoTestsL()
	{	
	CreateTestDbs();

	// Insert a zeroblob using RSqlStatement::BindZeroBlob() and read and write to it using streams
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4099: Stream BindZeroBlob() test"));
	StreamBindZeroBlobTestL();
			
	// Insert a zeroblob using SQLite's zeroblob() function and read and write to it using streams
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4100: Stream zeroblob() test"));
	StreamSqliteZeroBlobTestL();
		
	// Insert a record containing a 'real' blob and read and write to it using streams
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4101: Stream real blob test"));
	StreamRealBlobTestL();
	
	// Get a whole blob object
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4102: Whole value blob retrieval test"));
	GetWholeBlob3L();
		
	// Set a whole blob object
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4104: Whole value blob set test"));
	SetWholeBlob3L();
		
	// Attached database test
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4106: Attached database test"));
	AttachDbTestL();
	
	// Bad parameter test
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4107: Bad parameter test"));
	BadParamTestL();
	
	// Indexed column test
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4108: Indexed column test"));
	IndexedColumnTestL();
	
	// End Of File test
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4109: Eof test"));
	EofTestL();
	
	// Private secure database test
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4110: Private secure db test"));	
	PrivateSecureDbTestL();
	
	// Concurrent read and write test
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4111: Concurrent read and write test"));	
	ConcurrentReadAndWriteTestL();
	
	// UTF-16 read and write test
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4112: UTF-16 format test"));	
	UTF16FormatTestL();
	
	// Big blob test
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4114: Big blob test"));
	BigBlobTestL();
		
	TheTest.Next(_L(" @SYMTestCaseID:PDS-SQL-CT-4194: Blob streams. MStreamBuf::SeekL() test"));
	StreamSeekTestL();
		
	DeleteTestDbs();
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	
	__UHEAP_MARK;
		
	CreateTestDir();
	DeleteTestDbs();
	FillLargeDataBuf();
	TRAPD(err, DoTestsL());
	TEST2(err, KErrNone);

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}

