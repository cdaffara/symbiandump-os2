// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32file.h>
#include <e32test.h>

const TInt KTestCleanupStack=0x20;

// This is a path specification and should not be used as is
_LIT(KFileLocationSpec, "Z:\\STOR-TST\\T_FBUF.DAT");
const TUint8* KTestData=_S8("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
const TUint8* KTestDataLong=_S8("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0");
const TUint8* KTestData10Chars = _S8("0123456789");

const TInt KTestLength=36;
const TInt KTestTotal=KTestLength*(KTestLength+1);
const TPtrC8 KTestDes(KTestData,KTestLength);
const TPtrC8 KTestDesLong(KTestDataLong,KTestLength+1);
const TInt KTestFileBufSize=7;
const TInt KTestWriteBufSize=5;
const TInt KTestReadBufSize=3;
const TInt KTestSkipBufSize=2;
const TInt KTestCopyBufSize=1;

LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RTest test(_L("t_storfbuf"));
LOCAL_D RFs TheFs;
LOCAL_D TFileName TheTempFile;

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1136
@SYMTestCaseDesc	    Writing to a stream buffer test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for RWriteStream::WriteL() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testWriteL(RWriteStream& aStream)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1136 Writing... "));
	for (TInt i=0;i<=KTestLength;++i)
		{
		aStream.WriteL(KTestDes,i);
		aStream.WriteL(&KTestData[i],KTestLength-i);
		}
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1137
@SYMTestCaseDesc	    Reading from a stream buffer test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for RReadStream::ReadL() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testReadL(RReadStream& aStream)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1137 Reading... "));
	for (TInt i=KTestLength;i>=0;--i)
		{
		TBuf8<KTestLength+1> buf;
		aStream.ReadL(buf,i);
		test(buf.Length()==i);
		buf.SetMax();
		aStream.ReadL(&buf[i],KTestLength-i);
		buf.SetLength(KTestLength);
		test(buf==KTestDes);
		}
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1138
@SYMTestCaseDesc	    Skipping data on a stream buffer test.
@SYMTestPriority 	    High
@SYMTestActions  	    Tests skipping data while reading from a stream
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testSkipL(RReadStream& aStream)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1138 Skipping... "));
	for (TInt i=0;i<=KTestLength;++i)
		{
		aStream.ReadL(i);
		aStream.ReadL(KTestLength-i);
		}
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1139
@SYMTestCaseDesc	    Tests a stream buffer is at end-of-file.
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for end of file while reading from a stream buffer
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testEofL(RReadStream& aStream)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1139 At end "));
	TUint8 b;
	test(aStream.Source()->ReadL(&b,1)==0);
	}

//
// Test copying from one stream to another.
//
LOCAL_C void testCopyL(RWriteStream& aWriteStream,RReadStream& aReadStream)
	{
	test.Next(_L("Copying"));
	for (TInt i=KTestLength;i>=0;--i)
		{
		aWriteStream.WriteL(aReadStream,i);
		aReadStream.ReadL(aWriteStream,KTestLength-i);
		}
	}

//
// Test writing to a stream buffer.
//
LOCAL_C void testWriteL(MStreamBuf& aBuf)
	{
	RWriteStream stream(&aBuf);
	testWriteL(stream);
	}

//
// Test reading from a stream buffer.
//
LOCAL_C void testReadL(MStreamBuf& aBuf)
	{
	RReadStream stream(&aBuf);
	testReadL(stream);
	}

//
// Test skipping data on a stream buffer.
//
LOCAL_C void testSkipL(MStreamBuf& aBuf)
	{
	RReadStream stream(&aBuf);
	testSkipL(stream);
	}

//
// Test a stream buffer is at end-of-file.
//
LOCAL_C void testEofL(MStreamBuf& aBuf)
	{
	RReadStream stream(&aBuf);
	testEofL(stream);
	}

//
// Test copying from one stream buffer to another.
//
LOCAL_C void testCopyL(MStreamBuf& aSink,MStreamBuf& aSource)
	{
	RWriteStream out(&aSink);
	RReadStream in(&aSource);
	testCopyL(out,in);
	}
/**
@SYMTestCaseID          PDS-STORE-CT-4027
@SYMTestCaseDesc	    Writing to a stream buffer test
@SYMTestPriority 	    High
@SYMTestActions  	    Validate the MStreamBuf Write API
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF135804
*/
LOCAL_C void testStreamBufWriteL(MStreamBuf& aBuf)
	{
	test.Next(_L(" @SYMTestCaseID:PDS-STORE-CT-4027 Writing... "));
	TRequestStatus status;
	aBuf.Write(KTestDes,status);
	User::WaitForRequest(status);
	
	aBuf.SeekL(aBuf.EWrite,EStreamBeginning,0);
	aBuf.WriteL(KTestDes,status);
	User::WaitForRequest(status);
	
	}

/**
@SYMTestCaseID          PDS-STORE-CT-4026
@SYMTestCaseDesc	    Reading from a stream buffer test
@SYMTestPriority 	    High
@SYMTestActions  	    Validate the MStreamBuf Read API
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF135804
*/
LOCAL_C void testStreamBufReadL(MStreamBuf& aBuf)
	{
	test.Next(_L(" @SYMTestCaseID:PDS-STORE-CT-4026 Reading... "));
	TRequestStatus status;
	
	TBuf8<KTestLength+1> buf;
	aBuf.Read(buf, KTestLength, status);
	User::WaitForRequest(status);
	test(buf.Length()==KTestLength);
	test(buf==KTestDes);
	
	aBuf.SeekL(aBuf.ERead,EStreamBeginning,0);
	aBuf.ReadL(buf, status);
	User::WaitForRequest(status);
	test(buf.Length()== KTestLength+1);
	test(buf==KTestDesLong);

	aBuf.SeekL(aBuf.ERead,EStreamBeginning,0);
	aBuf.Read(buf, status);
	User::WaitForRequest(status);
	test(buf.Length()== KTestLength+1);
	test(buf==KTestDesLong);

	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1140
@SYMTestCaseDesc	    Write to a file buffer test.
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for writing to an attached file,a replaced file,a temporary file,an opened file
						Tests for replacing an existing file,test for deleting a file.
						Tests for writing to a new file buffer after the deletion of old file.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testWriteL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1140 Writing to attached file "));

	
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	
	
	
	RFile file;
	if (file.Replace(TheFs,parse.NameAndExt(),EFileWrite)!=KErrNone)
		test.Panic(_L("Replacing file"));
	RFileBuf rbuf(KTestWriteBufSize);
	RFile file2 = file;//Keep a copy in file2, because Attach() will clear the passed RFile parameter
	rbuf.Attach(file);
	rbuf.Detach();
	rbuf.Reattach(file2);
	testStreamBufWriteL(rbuf);
	rbuf.SynchL();
	rbuf.Close();
	if (TheFs.Delete(parse.NameAndExt())!=KErrNone)
			test.Panic(_L("Deleting file"));

	if (file.Replace(TheFs,parse.NameAndExt(),EFileWrite)!=KErrNone)
			test.Panic(_L("Replacing file"));
	RFileBuf buf(KTestWriteBufSize);
	buf.Attach(file);
	testWriteL(buf);
	buf.SynchL();
	buf.Reset(KTestFileBufSize);
	test(buf.SizeL()==KTestTotal);
	test(buf.SeekL(buf.EWrite,EStreamBeginning,KTestTotal/2)==KStreamBeginning+KTestTotal/2);
	testWriteL(buf);
	buf.Close();
//
	test.Next(_L("Writing to replaced file"));
	test(buf.Replace(TheFs,parse.NameAndExt(),EFileWrite)==KErrNone);
	testWriteL(buf);
	buf.SynchL();
	buf.File().Close();
	buf.Detach();
//
	test.Next(_L("Writing to temp file"));
	test(buf.Temp(TheFs,parse.DriveAndPath(),TheTempFile,EFileWrite)==KErrNone);
	testWriteL(buf);
	buf.SynchL();
	buf.File().Close();
	buf.Close();
//
	test.Next(_L("Writing to opened file"));
	test(buf.Open(TheFs,parse.NameAndExt(),EFileRead|EFileWrite)==KErrNone);
	testWriteL(buf);
//
	test.Next(_L("Failing to create existing file"));
	test(buf.Create(TheFs,TheTempFile,EFileWrite)==KErrAlreadyExists);
//
	test.Next(_L("Write overtaking read mark"));
	test(buf.SeekL(buf.ERead,KTestLength)==KStreamBeginning+KTestLength);
	RReadStream in(&buf);
	in.ReadL(1);
	test(buf.TellL(buf.ERead)==KStreamBeginning+KTestLength+1);
	test(buf.SeekL(buf.EWrite,EStreamBeginning)==KStreamBeginning);
	testWriteL(buf);
	buf.Close();
	if (TheFs.Delete(parse.NameAndExt())!=KErrNone)
		test.Panic(_L("Deleting file"));
//
	test.Next(_L("Writing to created file"));
	test(buf.Create(TheFs,parse.NameAndExt(),EFileWrite)==KErrNone);
	testWriteL(buf);
	buf.SynchL();
	buf.Close();
	

	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1141
@SYMTestCaseDesc	    Reading from a file buffer test.
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for reading from a file buffer.Tests for panic while opening a file.
                        Tests for reading from opened file,and a temporary file.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testReadL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1141 Reading from attached file "));

	TParsePtrC parse(KFileLocationSpec);
	
	RFile file;
	if (file.Open(TheFs,parse.NameAndExt(),EFileRead)!=KErrNone)
		test.Panic(_L("Opening file"));
	RFileBuf buf(KTestReadBufSize);
	buf.Attach(file);
	test(buf.SeekL(buf.ERead,EStreamBeginning,KTestTotal)==KStreamBeginning+KTestTotal);
	testEofL(buf);
	test(buf.SeekL(buf.ERead,EStreamBeginning)==KStreamBeginning);
	testReadL(buf);
	testEofL(buf);
	buf.SynchL();
	buf.Reset(KTestFileBufSize);
	buf.SeekL(buf.ERead|buf.EWrite,KStreamBeginning);
	testReadL(buf);
	testEofL(buf);
	buf.SynchL();
	buf.Reset(KTestFileBufSize);
	buf.SeekL(buf.ERead|buf.EWrite,KStreamBeginning);
	testStreamBufReadL(buf);
	buf.Close();
	buf.Close();
//
	test.Next(_L("Reading from opened file"));
	test(buf.Open(TheFs,parse.NameAndExt(),EFileRead|EFileWrite)==KErrNone);
	test(buf.SeekL(buf.ERead,EStreamEnd)==KStreamBeginning+KTestTotal);
	testEofL(buf);
	test(buf.SeekL(buf.ERead,EStreamEnd,-KTestTotal)==KStreamBeginning);
	testReadL(buf);
	testEofL(buf);
//
	test.Next(_L("Read overtaking write mark"));
	test(buf.SeekL(buf.EWrite,KTestLength)==KStreamBeginning+KTestLength);
	RWriteStream out(&buf);
	out.WriteL(KTestData,1);
	test(buf.TellL(buf.EWrite)==KStreamBeginning+KTestLength+1);
	test(buf.SeekL(buf.ERead,EStreamBeginning)==KStreamBeginning);
	testReadL(buf);
	testEofL(buf);
	buf.Close();
//
	test.Next(_L("Reading from temp file"));
	test(buf.Open(TheFs,TheTempFile,EFileRead)==KErrNone);
	test(buf.SeekL(buf.ERead,EStreamMark,KTestTotal)==KStreamBeginning+KTestTotal);
	testEofL(buf);
	test(buf.SeekL(buf.ERead,-KTestTotal)==KStreamBeginning);
	testReadL(buf);
	testEofL(buf);
	buf.Close();
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1142
@SYMTestCaseDesc	    Tests for skipping on a file buffer.
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for skipping data while reading.Test for end of file error
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testSkipL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1142 Skipping using small transfers "));

	TParsePtrC parse(KFileLocationSpec);
	
	RFileBuf buf(KTestSkipBufSize);
	test(buf.Open(TheFs,parse.NameAndExt(),EFileRead)==KErrNone);
	testSkipL(buf);
	testEofL(buf);
	buf.SynchL();
	buf.Reset(KTestFileBufSize);
	buf.SeekL(buf.ERead|buf.EWrite,KStreamBeginning);
	testSkipL(buf);
	testEofL(buf);
	buf.Close();
//
	test.Next(_L("Skipping using a single big transfer"));
	test(buf.Open(TheFs,parse.NameAndExt(),EFileRead)==KErrNone);
	RReadStream in(&buf);
	in.ReadL(KTestTotal);
	testEofL(buf);
	buf.Close();
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1143
@SYMTestCaseDesc	    Tests for copying within a single file buffer
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for copying using a different buffer sizes.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testCopyL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1143 Copying using small transfers "));

	TParsePtrC parse(KFileLocationSpec);
	
	RFileBuf buf(KTestCopyBufSize);
	test(buf.Open(TheFs,TheTempFile,EFileRead|EFileWrite)==KErrNone);
	testCopyL(buf,buf);
	testEofL(buf);
	buf.SynchL();
	buf.Reset(KTestFileBufSize);
	buf.SeekL(buf.ERead,KStreamBeginning);
	testCopyL(buf,buf);
	buf.SeekL(buf.ERead,KStreamBeginning);
	testReadL(buf);
	testReadL(buf);
	testEofL(buf);
	buf.Close();
//
	test.Next(_L("Copying using a single big transfer"));
	test(buf.Open(TheFs,parse.NameAndExt(),EFileRead|EFileWrite)==KErrNone);
	RWriteStream out(&buf);
	RReadStream in(&buf);
	in.ReadL(out,KTestTotal);
	testEofL(buf);
	buf.SeekL(buf.ERead|buf.EWrite,KStreamBeginning);
	out.WriteL(in,KTestTotal);
	testEofL(buf);
	buf.SeekL(buf.ERead|buf.EWrite,KStreamBeginning);
	testReadL(buf);
	testEofL(buf);
	buf.Close();
//
	test.Next(_L("Copying until end of file"));
	test(buf.Open(TheFs,parse.NameAndExt(),EFileRead|EFileWrite)==KErrNone);
	in.ReadL(out);
	testEofL(buf);
	buf.SeekL(buf.ERead|buf.EWrite,KStreamBeginning);
	out.WriteL(in);
	testEofL(buf);
	buf.SeekL(buf.ERead|buf.EWrite,KStreamBeginning);
	testReadL(buf);
	testEofL(buf);
	buf.Close();
	}

/**
@SYMTestCaseID          SYSLIB-STORE-UT-4005
@SYMTestCaseDesc	    DEF118202 - STORE component, RFileBuf, truncate, file position set incorrectly.
						The test creates an RFileBuf object and writes 30 bytes to the underlying file then
						commits. The file size is checked and should be 30 bytes.
						Then the test writes to the file another 20 bytes, commits and sets the file size to
						be 30 bytes. Then the test checks that the file size is really 30 bytes using
						the RFielBuf::SizeL() function.
@SYMTestPriority 	    High
@SYMTestActions  	    DEF118202 - STORE component, RFileBuf, truncate, file position set incorrectly.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF118202
*/
LOCAL_C void DEF118202L()
	{ 
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-UT-4005 DEF118202 - STORE component, RFileBuf, truncate, file position set incorrectly "));

	TParsePtrC parse(KFileLocationSpec);
	
	RFileBuf buf;
	CleanupClosePushL(buf);
	if(buf.Replace(TheFs, parse.NameAndExt(), EFileWrite) != KErrNone)
	     test.Panic(_L("Replacing file"));
	TInt size0 = buf.SizeL();
	test(size0 == 0);

	buf.WriteL(KTestData10Chars, 10);
	buf.WriteL(KTestData10Chars, 10);
	buf.WriteL(KTestData10Chars, 10);
	buf.SynchL();
	TInt size1 = buf.SizeL();

	buf.WriteL(KTestData10Chars, 10);
	buf.WriteL(KTestData10Chars, 10);
	buf.SynchL();

	buf.SetSizeL(30);
	buf.SynchL();
	TInt size2 = buf.SizeL();

	CleanupStack::PopAndDestroy(&buf);
	if (TheFs.Delete(parse.NameAndExt()) != KErrNone)
		test.Panic(_L("Deleting file"));

	test(size1 == size2);
	}

/**
@SYMTestCaseID          PDS-STORE-CT-4017
@SYMTestCaseDesc	    Operation read / write
@SYMTestPriority 	    High
@SYMTestActions  	    Reading and writing RFileBuf using protected API.
@SYMTestExpectedResults Data written must equal data read.
@SYMDEF                 DEF135804
*/

class RFileBufNext: public RFileBuf
	{
	public:
		TInt DoReadL(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus)
			{
			return RFileBuf::DoReadL(aDes,aMaxLength,aStatus);
			}
		TInt DoWriteL(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus)
			{
			return RFileBuf::DoWriteL(aDes,aMaxLength,aStatus);
			}
		TInt Avail(TArea anArea) const
			{
			return TStreamBuf::Avail(anArea);
			}
		TUint8* End(TArea anArea) const
			{
			return TStreamBuf::End(anArea);
			}
		void SetEnd(TArea anArea,TUint8* anEnd)
			{
			TStreamBuf::SetEnd(anArea, anEnd);
			}
	};

LOCAL_C void testReadWriteL()
	{
	test.Next(_L("@SYMTestCaseID PDS-STORE-CT-4017"));
	_LIT(KFileStore2, "c:\\STOR-TST\\T_FILESTORE.DAT");
	_LIT8(KTestTextBuf, "Ala ma kota, Ola ma psa a osa lata.");
	RFileBufNext fbuf;
	RFs fs;
	test( fs.Connect() == KErrNone );
	fs.Delete(KFileStore2);
	test( fbuf.Create(fs, KFileStore2, EFileWrite ) == KErrNone );
	TRequestStatus status;
	TRAPD(err, fbuf.DoWriteL(KTestTextBuf(),KTestTextBuf().Length(),status) );
	test(err == KErrNone);
	User::WaitForRequest(status);
	TRAP(err, fbuf.SeekL(MStreamBuf::ERead|MStreamBuf::EWrite, 0) );
	test(err == KErrNone);
	TBuf8<250> temp;
	fbuf.DoReadL(temp,temp.MaxLength(),status);
	User::WaitForRequest(status);
	test(temp.Compare(KTestTextBuf)==0);
	
	TInt av = fbuf.Avail(MStreamBuf::ERead);
	av = fbuf.Avail(MStreamBuf::EWrite);
	TUint8* end = fbuf.End(MStreamBuf::ERead);
	fbuf.SetEnd(MStreamBuf::ERead, end-1);
	end = fbuf.End(MStreamBuf::EWrite);
	fbuf.SetEnd(MStreamBuf::EWrite, end-1);
	
	fbuf.Close();
	fs.Delete(KFileStore2);
	fs.Close();
	}

//
// Prepare the test directory.
//
LOCAL_C void setupTestDirectory()
    {
	TInt r=TheFs.Connect();
	test(r==KErrNone);
//
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	
	r=TheFs.MkDir(parse.DriveAndPath());
	test(r==KErrNone||r==KErrAlreadyExists);
	r=TheFs.SetSessionPath(parse.DriveAndPath());
	test(r==KErrNone);
	}

//
// Initialise the cleanup stack.
//
LOCAL_C void setupCleanup()
    {
	TheTrapCleanup=CTrapCleanup::New();
	test(TheTrapCleanup!=NULL);
	TRAPD(r,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)1);\
		test(r==KErrNone);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	test(r==KErrNone);
	}

LOCAL_C void DeleteDataFile(const TDesC& aFullName)
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		TEntry entry;
		if(fsSession.Entry(aFullName, entry) == KErrNone)
			{
			RDebug::Print(_L("Deleting \"%S\" file.\n"), &aFullName);
			err = fsSession.SetAtt(aFullName, 0, KEntryAttReadOnly);
			if(err != KErrNone)
				{
				RDebug::Print(_L("Error %d changing \"%S\" file attributes.\n"), err, &aFullName);
				}
			err = fsSession.Delete(aFullName);
			if(err != KErrNone)
				{
				RDebug::Print(_L("Error %d deleting \"%S\" file.\n"), err, &aFullName);
				}
			}
		fsSession.Close();
		}
	else
		{
		RDebug::Print(_L("Error %d connecting file session. File: %S.\n"), err, &aFullName);
		}
	}

//
// Test file-based stream buffer.
//
GLDEF_C TInt E32Main()
    {
	test.Title();
	setupTestDirectory();
	setupCleanup();
	__UHEAP_MARK;

	test.Start(_L("Test file-based stream buffer"));
	TRAPD(r,testWriteL());
	test(r==KErrNone);
	TRAP(r,testReadL());
	test(r==KErrNone);
	TRAP(r,testSkipL());
	test(r==KErrNone);
	TRAP(r,testCopyL());
	test(r==KErrNone);
	TRAP(r, DEF118202L());
	test(r==KErrNone);
	TRAP(r, testReadWriteL());
	test(r==KErrNone);
	
	
	//deletion of data files must be before call to .End() - DEF047652
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	::DeleteDataFile(parse.FullName());

	test.End(); 
	
	__UHEAP_MARKEND;

	delete TheTrapCleanup;
	if (TheFs.Delete(TheTempFile)!=KErrNone)
		test.Panic(_L("Deleting temp file"));
	TheFs.Close();
	test.Close();
	return 0;
    }

