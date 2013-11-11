// Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <s32mem.h>
#include <e32test.h>

const TInt KTestCleanupStack=0x20;

// This is a path specification and should not be used as is
_LIT(KFileLocationSpec, "Z:\\STOR-TST\\T_FSTRM.DAT");
const TUint8* KTestData=_S8("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
const TInt KTestLength=36;
const TInt KTestTotal=KTestLength*(KTestLength+1);
const TPtrC8 KTestDes(KTestData,KTestLength);

LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RTest test(_L("t_storfstrm"));
LOCAL_D RFs TheFs;
LOCAL_D TFileName TheTempFile;
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1162
@SYMTestCaseDesc	    Tests for writing to a stream.
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for writing to a stream
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testWriteL(RWriteStream& aStream)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1162 Writing... "));
	for (TInt i=0;i<=KTestLength;++i)
		{
		aStream.WriteL(KTestDes,i);
		aStream.WriteL(&KTestData[i],KTestLength-i);
		}
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1163
@SYMTestCaseDesc	    Tests for reading from a stream.
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for reading from a stream
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testReadL(RReadStream& aStream)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1163 Reading... "));
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
@SYMTestCaseID          SYSLIB-STORE-CT-1164
@SYMTestCaseDesc	    Tests for skipping data on a stream.
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for skipping data while reading from a stream
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testSkipL(RReadStream& aStream)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1164 Skipping... "));
	for (TInt i=0;i<=KTestLength;++i)
		{
		aStream.ReadL(i);
		aStream.ReadL(KTestLength-i);
		}
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1165
@SYMTestCaseDesc	    Tests a stream is at end-of-file.
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for end of file while reading from a stream
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testEofL(RReadStream& aStream)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1165 At end "));
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
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1166
@SYMTestCaseDesc	    Tests for writing to a file stream.
						Tests RFileWriteStream::WriteL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for writing to replaced,temporary,opened,created file.
						Tests for creating an already existing file.
						Tests for panic while deleting a file.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testWriteL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1166 Writing to attached file "));

	
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	RFile file;
	if (file.Replace(TheFs,parse.NameAndExt(),EFileWrite)!=KErrNone)
		test.Panic(_L("Replacing file"));
	RFile f=file;
	RFileWriteStream out(f);
	testWriteL(out);
	out.CommitL();
	out.Attach(file);
	testWriteL(out);
	out.Close();
//
	test.Next(_L("Writing to replaced file"));
	test(out.Replace(TheFs,parse.NameAndExt(),EFileWrite)==KErrNone);
	testWriteL(out);
	out.Close();
//
	test.Next(_L("Writing to temp file"));
	test(out.Temp(TheFs,parse.DriveAndPath(),TheTempFile,EFileWrite)==KErrNone);
	testWriteL(out);
	out.CommitL();
	out.Close();
//
	test.Next(_L("Writing to opened file"));
	test(out.Open(TheFs,parse.NameAndExt(),EFileWrite)==KErrNone);
	testWriteL(out);
//
	test.Next(_L("Failing to create existing file"));
	test(out.Create(TheFs,TheTempFile,EFileWrite)==KErrAlreadyExists);
	testWriteL(out);
	out.Close();
	if (TheFs.Delete(parse.NameAndExt())!=KErrNone)
		test.Panic(_L("Deleting file"));
//
	test.Next(_L("Writing to created file"));
	test(out.Create(TheFs,parse.NameAndExt(),EFileWrite)==KErrNone);
	testWriteL(out);
	out.CommitL();
	out.Close();
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1167
@SYMTestCaseDesc	    Tests reading from a file stream.
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for reading from an attached file
						Attempt for reading from opened file
						Attempt for reading from temp file.Test for end of file error
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testReadL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1167 Reading from attached file "));

	TParsePtrC parse(KFileLocationSpec);
	
	RFile file;
	if (file.Open(TheFs,parse.NameAndExt(),EFileRead)!=KErrNone)
		test.Panic(_L("Opening file"));
	RFile f=file;
	RFileReadStream in(f);
	testReadL(in);
	testEofL(in);
	in.Attach(file);
	testReadL(in);
	testEofL(in);
	in.Close();
//
	test.Next(_L("Reading from opened file"));
	test(in.Open(TheFs,parse.NameAndExt(),EFileRead)==KErrNone);
	testReadL(in);
	testEofL(in);
	in.Close();
//
	test.Next(_L("Reading from temp file"));
	test(in.Open(TheFs,TheTempFile,EFileRead)==KErrNone);
	testReadL(in);
	testEofL(in);
	in.Close();
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1168
@SYMTestCaseDesc	    Skipping on a file stream test
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for skipping data while reading from a stream
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testSkipL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1168 Skipping using small transfers "));

	TParsePtrC parse(KFileLocationSpec);
	
	RFileReadStream in;
	test(in.Open(TheFs,parse.NameAndExt(),EFileRead)==KErrNone);
	testSkipL(in);
	testEofL(in);
	in.Close();
//
	test.Next(_L("Skipping using a single big transfer"));
	test(in.Open(TheFs,parse.NameAndExt(),EFileRead)==KErrNone);
	in.ReadL(KTestTotal);
	testEofL(in);
	in.Close();
	}
/**
@SYMTestCaseID          SYSLIB-STORE-CT-1169
@SYMTestCaseDesc	    Copying from one file stream to another test
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for copying using small transfers.
                        Attempt for copying until end of file.
						Attempt for end of file error
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testCopyL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1169 Copying using small transfers "));

	TParsePtrC parse(KFileLocationSpec);
	
	RFileReadStream in;
	RFileWriteStream out;
	test(in.Open(TheFs,parse.NameAndExt(),EFileRead)==KErrNone);
	test(out.Replace(TheFs,TheTempFile,EFileWrite)==KErrNone);
	testCopyL(out,in);
	testEofL(in);
	out.CommitL();
	out.Close();
	in.Close();
	test(in.Open(TheFs,TheTempFile,EFileRead)==KErrNone);
	testReadL(in);
	testEofL(in);
	in.Close();
//
	test.Next(_L("Copying using a single big transfer"));
	test(out.Replace(TheFs,TheTempFile,EFileWrite)==KErrNone);
	test(in.Open(TheFs,parse.NameAndExt(),EFileRead)==KErrNone);
	in.ReadL(out,KTestTotal);
	testEofL(in);
	in.Close();
	test(in.Open(TheFs,parse.NameAndExt(),EFileRead)==KErrNone);
	out.WriteL(in,KTestTotal);
	testEofL(in);
	in.Close();
	out.CommitL();
	out.Close();
	test(in.Open(TheFs,TheTempFile,EFileRead)==KErrNone);
	testReadL(in);
	testReadL(in);
	testEofL(in);
	in.Close();
//
	test.Next(_L("Copying until end of file"));
	test(out.Replace(TheFs,TheTempFile,EFileWrite)==KErrNone);
	test(in.Open(TheFs,parse.NameAndExt(),EFileRead)==KErrNone);
	in.ReadL(out);
	testEofL(in);
	in.Close();
	test(in.Open(TheFs,parse.NameAndExt(),EFileRead)==KErrNone);
	out.WriteL(in);
	testEofL(in);
	in.Close();
	out.CommitL();
	out.Close();
	test(in.Open(TheFs,TheTempFile,EFileRead)==KErrNone);
	testReadL(in);
	testReadL(in);
	testEofL(in);
	in.Close();
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

class RTestReadStream : public RReadStream
	{
public:
	RTestReadStream(MStreamBuf* aSource) :
		RReadStream(aSource)
		{
		}
	void Attach(MStreamBuf* aSource)
		{
		RReadStream::Attach(aSource);
		}
	void Detach()
		{
		RReadStream::Detach();
		}
	};

class RTestWriteStream : public RWriteStream
	{
public:
	RTestWriteStream(MStreamBuf* aSink) :
		RWriteStream(aSink)
		{
		}
	void Attach(MStreamBuf* aSink)
		{
		RWriteStream::Attach(aSink);
		}
	void Detach()
		{
		RWriteStream::Detach();
		}
	};

/**
@SYMTestCaseID          PDS-STORE-CT-4064
@SYMTestCaseDesc        RReadStream, RWriteStream, Pop() and Detach() test.
@SYMTestActions			The test calls Pop() and Detach() methods of RReadStream and RWriteStream classes.         
@SYMTestPriority        High
@SYMTestExpectedResults Test must not fail
*/
void StreamDetachTestL()
	{
	test.Next(_L("@SYMTestCaseID:PDS-STORE-CT-4064: RReadStream, RWriteStream, Pop() and Detach() test"));
	
	TBuf8<100> buf;
	TDesBuf desBuf;
	desBuf.Set(buf);
	MStreamBuf* mbuf = &desBuf; 
	
	_LIT8(KStr, "1234567890");
	
	RTestWriteStream wstrm(mbuf);
	wstrm.PushL();
	wstrm.Detach();
	wstrm.Attach(mbuf);
	TRAPD(err, wstrm.WriteL(KStr));
	test(err == KErrNone);
	TRAP(err, wstrm.CommitL());
	test(err == KErrNone);
	wstrm.Pop();
	wstrm.Close();

	RTestReadStream rstrm(mbuf);
	rstrm.PushL();
	rstrm.Detach();
	rstrm.Attach(mbuf);
	TBuf8<100> buf2;
	TRAP(err, rstrm.ReadL(buf2, KStr().Length()));
	test(err == KErrNone);
	rstrm.Pop();
	rstrm.Close();
	
	test(KStr() == buf2);
	}

//
// Test file-based streams.
//
GLDEF_C TInt E32Main()
    {
	test.Title();
	setupTestDirectory();
	setupCleanup();
	__UHEAP_MARK;
//
	test.Start(_L("Test file-based streams"));
	TRAPD(r,testWriteL());
	test(r==KErrNone);
	TRAP(r,testReadL());
	test(r==KErrNone);
	TRAP(r,testSkipL());
	test(r==KErrNone);
	TRAP(r,testCopyL());
	test(r==KErrNone);
	TRAP(r, StreamDetachTestL());
	test(r==KErrNone);

	//deletion of data files must be before call to .End() - DEF047652
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	::DeleteDataFile(parse.FullName());

	test.End();
//
	__UHEAP_MARKEND;

	delete TheTrapCleanup;
	if (TheFs.Delete(TheTempFile)!=KErrNone)
		test.Panic(_L("Deleting temp file"));
	TheFs.Close();
	test.Close();
	return 0;
    }

