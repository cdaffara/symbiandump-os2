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

#include <s32mem.h>
#include <s32file.h>
#include <e32test.h>

const TInt KTestCleanupStack=0x20;
// This is a path specification and should not be used as is
_LIT(KFileLocationSpec, "Z:\\STOR-TST\\T_DELIM.DAT");

LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RTest test(_L("t_stordelim"));
LOCAL_D RFs TheFs;

LOCAL_C TInt ReadL(RReadStream& aStream,TUint8* aBuf,TInt aLength,TChar aDelim)
	{
	TPtr8 des(aBuf,aLength);
	aStream.ReadL(des,aDelim);
	return des.Length();
	}

LOCAL_C TInt ReadL(RReadStream& aStream,TUint16* aBuf,TInt aLength,TChar aDelim)
	{
	TPtr16 des(aBuf,aLength);
	aStream.ReadL(des,aDelim);
	return des.Length();
	}

LOCAL_C TPtrC8 KTestText8=_S8("One two three four five six seven eight nine ten eleven twelve.");
LOCAL_C TPtrC16 KTestText16=_S16("One two three four five six seven eight nine ten eleven twelve.");
LOCAL_C TPtrC8 KWhatever8=_S8("Whatever");
LOCAL_C TPtrC16 KWhatever16=_S16("Whatever");

const TInt KBufSize=0x20;

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1181
@SYMTestCaseDesc	    Tests for reading from 8-bit delimited input
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for RReadStream::ReadL() function.Check for KErrEof error flag
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testInput8L()
	{
	TUint8 buf[KBufSize];
	RDesReadStream strm(KWhatever8);
	TInt len=ReadL(strm,buf,KBufSize,'e');
	test (len==5);
	test (TPtrC8(buf,len)==_L8("Whate"));
	len=ReadL(strm,buf,KBufSize,'e');
	test (len==2);
	test (TPtrC8(buf,len)==_L8("ve"));
	len=ReadL(strm,buf,1,'q');
	test (len==1);
	test (TPtrC8(buf,len)==_L8("r"));
	TRAPD(r,ReadL(strm,buf,1,'r'));
	test (r==KErrEof);
	strm.Source()->SeekL(MStreamBuf::ERead,KStreamBeginning);
	TRAP(r,ReadL(strm,buf,KBufSize,'\n'));
	test (r==KErrEof);

	TParsePtrC parse(KFileLocationSpec);
	CFileStore* store=CDirectFileStore::ReplaceLC(TheFs,parse.NameAndExt(),EFileWrite|EFileRead);
	store->SetTypeL(store->Layout());
	RStoreWriteStream out;
	TStreamId id=out.CreateLC(*store);
	out.WriteL(KTestText8);
	out.CommitL();
	CleanupStack::PopAndDestroy();

// small buffer
	store->CommitL();
	store->Reset(11);
//
	RStoreReadStream in;
	in.OpenLC(*store,id);
	TInt tot=KTestText8.Length();
	TUint8 const* pp=KTestText8.Ptr();
	for (;;)
		{
		len=ReadL(in,buf,Min(tot,KBufSize),' ');
		test (len<=tot);
		test (Mem::Compare(buf,len,pp,len)==0);
		pp+=len;
		tot-=len;
		if (tot==0)
			break;
		test (buf[len-1]==' ');
		}
	test (buf[len-1]=='.');
	TRAP(r,ReadL(in,buf,1,' '));
	test (r==KErrEof);
	CleanupStack::PopAndDestroy();
//
	in.OpenLC(*store,id);
	tot=0;
	do
		{
		len=ReadL(in,buf,KBufSize,'.');
		tot+=len;
		} while (buf[len-1]!='.');
	test (tot==KTestText8.Length());
	TRAP(r,ReadL(in,buf,1,' '));
	test (r==KErrEof);
//
	CleanupStack::PopAndDestroy(2);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1182
@SYMTestCaseDesc	    Tests for reading from 16-bit delimited input.
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for RReadStream::ReadL() function.Check for KErrEof error flag.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testInput16L()
	{
	TUint16 buf[KBufSize];
	RDesReadStream strm(TPtrC8((TUint8 const*)KWhatever16.Ptr(),KWhatever16.Size()));
	TInt len=ReadL(strm,buf,KBufSize,'e');
	test (len==5);
	test (TPtrC16(buf,len)==_L16("Whate"));
	len=ReadL(strm,buf,KBufSize,'e');
	test (len==2);
	test (TPtrC16(buf,len)==_L16("ve"));
	len=ReadL(strm,buf,1,'q');
	test (len==1);
	test (TPtrC16(buf,len)==_L16("r"));
	TRAPD(r,ReadL(strm,buf,1,'r'));
	test (r==KErrEof);
	strm.Source()->SeekL(MStreamBuf::ERead,KStreamBeginning);
	TRAP(r,ReadL(strm,buf,KBufSize,'\n'));
	test (r==KErrEof);

	TParsePtrC parse(KFileLocationSpec);
	CFileStore* store=CDirectFileStore::OpenLC(TheFs,parse.NameAndExt(),EFileWrite|EFileRead);
	RStoreWriteStream out;
	TStreamId id=out.CreateLC(*store);
	out.WriteL(KTestText16);
	out.CommitL();
	CleanupStack::PopAndDestroy();

// small buffer
	store->CommitL();
	store->Reset(11);
//
	RStoreReadStream in;
	in.OpenLC(*store,id);
	TInt tot=KTestText16.Length();
	TUint16 const* pp=KTestText16.Ptr();
	for (;;)
		{
		len=ReadL(in,buf,Min(tot,KBufSize),' ');
		test (len<=tot);
		test (Mem::Compare(buf,len,pp,len)==0);
		pp+=len;
		tot-=len;
		if (tot==0)
			break;
		test (buf[len-1]==' ');
		}
	test (buf[len-1]=='.');
	TRAP(r,ReadL(in,buf,1,' '));
	test (r==KErrEof);
	CleanupStack::PopAndDestroy();
//
	in.OpenLC(*store,id);
	tot=0;
	do
		{
		len=ReadL(in,buf,KBufSize,'.');
		tot+=len;
		} while (buf[len-1]!='.');
	test (tot==KTestText16.Length());
	TRAP(r,ReadL(in,buf,1,' '));
	test (r==KErrEof);
//
	CleanupStack::PopAndDestroy(2);
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
// Test file-based streams.
//
GLDEF_C TInt E32Main()
    {
	test.Title();
	setupTestDirectory();
	setupCleanup();
	__UHEAP_MARK;
//
	test.Start(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1181 Test 8-bit delimited input "));
	TRAPD(r,testInput8L());
	test (r==KErrNone);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1182 Test 16-bit delimited input "));
	TRAP(r,testInput16L());
	test (r==KErrNone);

	//deletion of data files must be before call to .End() - DEF047652
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	::DeleteDataFile(parse.FullName());

	test.End();
//
	__UHEAP_MARKEND;

	delete TheTrapCleanup;
	TheFs.Close();
	test.Close();
	return 0;
    }

