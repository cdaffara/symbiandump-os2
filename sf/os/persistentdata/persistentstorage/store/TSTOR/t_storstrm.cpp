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
const TInt KTestExpandSize=0x20;
const TInt KTestGranularity=0x02;

// This is a path specification and should not be used as is
_LIT(KFileLocationSpec, "Z:\\STOR-TST\\T_STRM.DAT");

LOCAL_D RTest test(_L("t_storstrm"));
LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RFs TheFs;
LOCAL_D CFileStore* TheStore;
LOCAL_D CBufStore* TheBufStore;
LOCAL_D RStoreWriteStream TheSink;
LOCAL_D RStoreReadStream TheSource;

class THexEncodeFilter : public TStreamFilter
	{
public:
	void Set(MStreamBuf* aBuf,TInt aMode);
private:
	TInt Capacity(TInt aMaxLength);
	TInt FilterL(TAny* aPtr,TInt aMaxLength,const TUint8*& aFrom,const TUint8* anEnd);
	void DoSynchL();
//
	static TText8 HexDigit(TUint aNibble);
	static TText8 HexMSN(TUint aByte);
	static TText8 HexLSN(TUint aByte);
private:
	TChar iSpare;
	};

class RHexDumpStream : public RWriteStream
	{
public:
	RHexDumpStream() {}
	RHexDumpStream(RWriteStream& aStream);
	void Open(RWriteStream& aStream);
private:
	THexEncodeFilter iFilter;
	};
class RHexLoadStream : public RReadStream
	{
public:
	RHexLoadStream() {}
	RHexLoadStream(RReadStream& aStream);
	void Open(RReadStream& aStream);
private:
	THexEncodeFilter iFilter;
	};

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1205
@SYMTestCaseDesc	    Streaming in and out tests
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for copying and filtering
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testFixL(CArrayFix< TBuf<16> >& aFix)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1205 Test streaming in and out "));
	TBuf<16> aa(_L("aa"));
	aFix.AppendL(aa);
	TBuf<16> bb(_L("bbbb"));
	aFix.AppendL(bb);
	TStreamId id=TheSink.CreateL(*TheStore);
	TheSink<<aFix;
	TBuf<16> cc(_L("cccccc"));
	aFix.AppendL(cc);
	TheSink.Close();
	id=TheSink.CreateL(*TheStore);
	TheSink<<aFix;
	TheSink.Close();
	TheStore->SetRootL(id);
	TheStore->CommitL();
	aFix.Reset();
	TheSource.OpenL(*TheStore,TheStore->Root());
	TheSource>>aFix;
	TheSource.Close();
	test(aFix[0]==aa&&aFix[1]==bb&&aFix[2]==cc);
	test.Printf(_L("%S %S %S\n"),&aa,&bb,&cc);
	test.Printf(_L("%S %S %S\n"),&aFix[0],&aFix[1],&aFix[2]);
//
// Test copying and filtering.
//
	CBufSeg* bufp=CBufSeg::NewL(KTestExpandSize);
	CleanupStack::PushL(bufp);
//
	TheSource.OpenL(*TheStore,id);
	id=TheSink.CreateL(*TheStore);
//
	RBufWriteStream bout(*bufp);
	RHexDumpStream hout(bout);
	hout.WriteL(TheSource,10);
	RHexLoadStream hin(TheSource);
	hin.ReadL(bout,10);
	bout.CommitL();
//
	RBufReadStream bin(*bufp);
	bin.ReadL(TheSink);
//
	TheSink.Close();
	TheSource.Close();
//
	CleanupStack::PopAndDestroy();
	
	}

/**
@SYMTestCaseID          PDS-STORE-CT-4018
@SYMTestCaseDesc	    Testing RStoreWriteStream::Append API
@SYMTestPriority 	    High
@SYMTestActions  	    Open file for append, add data to Store
@SYMTestExpectedResults Data is succesfully added to store
@SYMDEF                 DEF135804
*/
LOCAL_C void testAppendL()
	{
	test.Next(_L("PDS-STORE-CT-4018: Testing Append API"));
	TheBufStore=CBufStore::NewLC(KTestExpandSize);
	
	//TStreamId id=TheSink.CreateL(*TheBufStore);
	TStreamId id = TheBufStore->ExtendL();
	TRAPD(r, TheSink.AppendL(*TheBufStore, id));
	test(r == KErrNone);
	TBuf<16> inbuf(_L("appendL"));
	TheSink<<inbuf;
	TheSink.CommitL();
	TheSink.Close();
	
	
	TBuf<16> outbuf;
	TheSource.OpenL(*TheBufStore,id);
	TheSource>>outbuf;
	TheSource.Close();
	test(inbuf==outbuf);
	
	test.Printf(_L("Write Buf: %S\n"),&inbuf);
	test.Printf(_L("Read Buf: %S\n"),&outbuf);
	
	CleanupStack::PopAndDestroy(TheBufStore);
	
	}

RHexDumpStream::RHexDumpStream(RWriteStream& aStream)
	{
	Open(aStream);
	}

void RHexDumpStream::Open(RWriteStream& aStream)
	{
	iFilter.Set(aStream.Sink(),THexEncodeFilter::EWrite);
	RWriteStream::Attach(&iFilter);
	}

RHexLoadStream::RHexLoadStream(RReadStream& aStream)
	{
	Open(aStream);
	}

void RHexLoadStream::Open(RReadStream& aStream)
	{
	iFilter.Set(aStream.Source(),THexEncodeFilter::ERead);
	RReadStream::Attach(&iFilter);
	}

void THexEncodeFilter::Set(MStreamBuf* aBuf,TInt aMode)
	{
	TStreamFilter::Set(aBuf,aMode);
	iSpare=TChar(0);
	}

TInt THexEncodeFilter::Capacity(TInt aMaxLength)
	{
	__ASSERT_DEBUG(aMaxLength>0,User::Panic(_L("aargh!"),0));
	return (iSpare.Eos()?aMaxLength+1:aMaxLength)>>1;
	}

TInt THexEncodeFilter::FilterL(TAny* aPtr,TInt aMaxLength,const TUint8*& aFrom,const TUint8* anEnd)
	{
	__ASSERT_DEBUG(aMaxLength>0,User::Panic(_L("aargh!"),0));
	TPtr8 hex((TText8*)aPtr,aMaxLength);
	const TUint8* bin=aFrom;
	if (!iSpare.Eos())
		{
		hex.Append(iSpare);
		aMaxLength--;
		}
	TInt n=Min(aMaxLength>>1,anEnd-bin);
	for (const TUint8* end=bin+n;bin<end;bin++)
		{
		TUint8 byte=*bin;
		hex.Append(HexMSN(byte));
		hex.Append(HexLSN(byte));
		}
	if (aMaxLength>(n<<1)&&bin<anEnd)
		{
		TUint8 byte=*bin++;
		hex.Append(HexMSN(byte));
		iSpare=HexLSN(byte);
		}
	else
		iSpare=TChar(0);
	__ASSERT_DEBUG(hex.Length()==hex.MaxLength()||bin==anEnd,User::Panic(_L("aargh!"),0));
	aFrom=bin;
	return hex.Length();
	}

void THexEncodeFilter::DoSynchL()
	{
	if (IsCommitted())
		return;
//
	if (!iSpare.Eos())
		{
		TText8 c=TText8(iSpare);
		iSpare=TChar(0);
		EmitL(&c,1);
		}
	TStreamFilter::DoSynchL();
	}

TText8 THexEncodeFilter::HexDigit(TUint aNibble)
	{
	return TText8(aNibble+(aNibble<10?'0':('a'-10)));
	}

TText8 THexEncodeFilter::HexMSN(TUint aByte)
	{
	return HexDigit(aByte>>4);
	}

TText8 THexEncodeFilter::HexLSN(TUint aByte)
	{
	return HexDigit(aByte&0x0f);
	}

LOCAL_C void doMainL()
	{
	test.Start(_L("Creating Store"));
	TParsePtrC parse(KFileLocationSpec);
	
//	TheStore=CBufStore::NewLC(KTestExpandSize);
	TheStore=CDirectFileStore::ReplaceLC(TheFs,parse.NameAndExt(),EFileRead|EFileWrite);
	TheStore->SetTypeL(TUidType(KDirectFileStoreLayoutUid,TUid::Uid('H'|('E'<<8)|('X'<<16))));

	test.Next(_L("Creating CArrayFixFlat"));
	CArrayFixFlat< TBuf<16> >* pFixFlat=new(ELeave) CArrayFixFlat< TBuf<16> >(KTestGranularity);
	CleanupStack::PushL(pFixFlat);
	testFixL(*pFixFlat);
	
	testAppendL();
	
	CleanupStack::PopAndDestroy(2,TheStore);
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
// Test the streaming framework.
//
GLDEF_C TInt E32Main()
    {
	test.Title();
	setupTestDirectory();
	setupCleanup();
	__UHEAP_MARK;
//
	TRAPD(r,doMainL());
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
	TheFs.Close();
	test.Close();
	return 0;
    }

