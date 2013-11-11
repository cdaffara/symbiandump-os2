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
#include <e32test.h>

const TInt KTestCleanupStack=0x20;
const TUint8* KTestData=_S8("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
const TInt KTestLength=36;
const TInt KTestTotal=KTestLength*(KTestLength+1);
const TPtrC8 KTestDes(KTestData,KTestLength);
const TInt KTestExpandSize=0x20;
const TInt KTestCopyExpandSize=17;

LOCAL_D CTrapCleanup *TheTrapCleanup;
LOCAL_D RTest test(_L("t_stormemstrm"));
LOCAL_D TUint8 TheBuf[KTestTotal+1];

//
// Test writing to a stream.
//
LOCAL_C void testWriteL(RWriteStream& aStream)
	{
	test.Next(_L("Writing..."));
	for (TInt i=0;i<=KTestLength;++i)
		{
		aStream.WriteL(KTestDes,i);
		aStream.WriteL(&KTestData[i],KTestLength-i);
		}
	}

//
// Test reading from a stream.
//
LOCAL_C void testReadL(RReadStream& aStream)
	{
	test.Next(_L("Reading..."));
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

//
// Test skipping data on a stream.
//
LOCAL_C void testSkipL(RReadStream& aStream)
	{
	test.Next(_L("Skipping..."));
	for (TInt i=0;i<=KTestLength;++i)
		{
		aStream.ReadL(i);
		aStream.ReadL(KTestLength-i);
		}
	}

//
// Test a stream is at end-of-file.
//
LOCAL_C void testEofL(RReadStream& aStream)
	{
	test.Next(_L("At end"));
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
// Test writing to a memory stream.
//
LOCAL_C void testWriteL(TAny* aPtr)
	{
	test.Next(_L("Writing to constructed stream"));
	RMemWriteStream out(aPtr,KTestTotal);
	testWriteL(out);
	out.CommitL();
//
	test.Next(_L("Writing one byte too many"));
	TRAPD(r,out.WriteUint8L(0));
	test(r==KErrOverflow);
	out.Close();
//
	test.Next(_L("Over-writing massively"));
	out.Open(aPtr,KTestLength);
	TRAP(r,testWriteL(out));
	test(r==KErrOverflow);
	out.Open((TUint8*)aPtr+KTestLength,KTestTotal-KTestLength);
	TRAP(r,testWriteL(out));
	test(r==KErrOverflow);
//
	test.Next(_L("Writing to opened stream"));
	out.Open(aPtr,KTestTotal);
	testWriteL(out);
	}

//
// Test reading from a memory stream.
//
LOCAL_C void testReadL(const TAny* aPtr)
	{
	test.Next(_L("Reading from constructed stream"));
	RMemReadStream in(KTestData,KTestLength);
	TRAPD(r,testReadL(in));
	test(r==KErrEof);
	testEofL(in);
	in.Close();
//
	test.Next(_L("Reading from opened stream"));
	in.Open(aPtr,KTestTotal);
	testReadL(in);
	testEofL(in);
	}

//
// Test skipping on a memory stream.
//
LOCAL_C void testSkipL(const TAny* aPtr)
	{
	test.Next(_L("Skipping using small transfers"));
	RMemReadStream in(aPtr,KTestTotal);
	testSkipL(in);
	testEofL(in);
	in.Close();
//
	test.Next(_L("Skipping using a single big transfer"));
	in.Open(aPtr,KTestTotal);
	in.ReadL(KTestTotal);
	testEofL(in);
	}

//
// Test copying memory streams.
//
LOCAL_C void testCopyL(TAny* aPtr)
	{
	test.Next(_L("Copying using small transfers"));
	TUint8 buf[KTestTotal];
	RMemReadStream in(KTestData,KTestLength);
	RMemWriteStream out(buf,KTestTotal);
	TRAPD(r,testCopyL(out,in));
	test(r==KErrEof);
	testEofL(in);
	in.Open(aPtr,KTestTotal);
	TRAP(r,testCopyL(out,in));
	test(r==KErrOverflow);
	in.Open(buf,KTestTotal);
	testReadL(in);
	testEofL(in);
	in.Open(buf,KTestTotal);
	out.Open(aPtr,KTestTotal);
	testCopyL(out,in);
	testEofL(in);
	out.Close();
	in.Close();
	in.Open(aPtr,KTestTotal);
	testReadL(in);
	testEofL(in);
	in.Close();
//
	test.Next(_L("Copying using a single big transfer"));
	Mem::FillZ(buf,KTestTotal);
	in.Open(KTestData,KTestLength);
	out.Open(buf,KTestTotal);
	TRAP(r,in.ReadL(out,KTestTotal));
	test(r==KErrEof);
	testEofL(in);
	in.Open(aPtr,KTestTotal);
	TRAP(r,out.WriteL(in,KTestTotal));
	test(r==KErrOverflow);
	in.Open(buf,KTestTotal);
	testReadL(in);
	testEofL(in);
	in.Open(buf,KTestTotal);
	out.Open(aPtr,KTestTotal);
	in.ReadL(out,KTestTotal);
	testEofL(in);
	out.Close();
	in.Close();
	in.Open(aPtr,KTestTotal);
	testReadL(in);
	testEofL(in);
	in.Close();
//
	test.Next(_L("Copying until end of file"));
	Mem::FillZ(buf,KTestTotal);
	in.Open(KTestData,KTestLength);
	out.Open(buf,KTestTotal);
	out.WriteL(in);
	testEofL(in);
	in.Open(aPtr,KTestTotal);
	TRAP(r,in.ReadL(out));
	test(r==KErrOverflow);
	in.Open(buf,KTestTotal);
	testReadL(in);
	testEofL(in);
	in.Open(buf,KTestTotal);
	out.Open(aPtr,KTestTotal);
	out.WriteL(in);
	testEofL(in);
	out.CommitL();
	out.Close();
	in.Close();
	in.Open(aPtr,KTestTotal);
	testReadL(in);
	testEofL(in);
	in.Close();
	}

//
// Test writing to a descriptor stream.
//
LOCAL_C void testWriteL(TDes8& aDes)
	{
	test.Next(_L("Writing to constructed stream"));
	RDesWriteStream out(aDes);
	testWriteL(out);
//
	test.Next(_L("Writing one byte too many"));
	TRAPD(r,out.WriteUint8L(0));
	test(r==KErrOverflow);
	out.Close();
//
	test.Next(_L("Over-writing massively"));
	TPtr8 ptr((TUint8*)aDes.Ptr(),KTestLength);
	out.Open(ptr);
	TRAP(r,testWriteL(out));
	test(r==KErrOverflow);
	ptr.Set((TUint8*)aDes.Ptr()+KTestLength,KTestLength,KTestTotal-KTestLength);
	out.Open(ptr);
	TRAP(r,testWriteL(out));
	test(r==KErrOverflow);
//
	test.Next(_L("Writing to opened stream"));
	out.Open(aDes);
	testWriteL(out);
	out.CommitL();
	}

//
// Test reading from a descriptor stream.
//
LOCAL_C void testReadL(const TDesC8& aDes)
	{
	test.Next(_L("Reading from constructed stream"));
	RDesReadStream in(KTestDes);
	TRAPD(r,testReadL(in));
	test(r==KErrEof);
	testEofL(in);
	in.Close();
//
	test.Next(_L("Reading from opened stream"));
	in.Open(aDes);
	testReadL(in);
	testEofL(in);
	}

//
// Test skipping on a descriptor stream.
//
LOCAL_C void testSkipL(const TDesC8& aDes)
	{
	test.Next(_L("Skipping using small transfers"));
	RDesReadStream in(aDes);
	testSkipL(in);
	testEofL(in);
	in.Close();
//
	test.Next(_L("Skipping using a single big transfer"));
	in.Open(aDes);
	in.ReadL(KTestTotal);
	testEofL(in);
	}

//
// Test copying descriptor streams.
//
LOCAL_C void testCopyL(TDes8& aDes)
	{
	test.Next(_L("Copying using small transfers"));
	TBuf8<KTestTotal> buf;
	RDesReadStream in(KTestDes);
	RDesWriteStream out(buf);
	TRAPD(r,testCopyL(out,in));
	test(r==KErrEof);
	testEofL(in);
	in.Open(aDes);
	TRAP(r,testCopyL(out,in));
	test(r==KErrOverflow);
	out.CommitL();
	testEofL(in);
	in.Open(buf);
	testReadL(in);
	testEofL(in);
	in.Open(buf);
	out.Open(aDes);
	testCopyL(out,in);
	testEofL(in);
	out.Close();
	in.Close();
	in.Open(aDes);
	testReadL(in);
	testEofL(in);
	in.Close();
//
	test.Next(_L("Copying using a single big transfer"));
	buf.FillZ();
	in.Open(KTestDes);
	out.Open(buf);
	TRAP(r,in.ReadL(out,KTestTotal));
	test(r==KErrEof);
	testEofL(in);
	in.Open(aDes);
	TRAP(r,out.WriteL(in,KTestTotal));
	test(r==KErrOverflow);
	out.CommitL();
	in.Open(buf);
	testReadL(in);
	testEofL(in);
	in.Open(buf);
	out.Open(aDes);
	in.ReadL(out,KTestTotal);
	testEofL(in);
	out.Close();
	in.Close();
	in.Open(aDes);
	testReadL(in);
	testEofL(in);
	in.Close();
//
	test.Next(_L("Copying until end of file"));
	buf.FillZ();
	in.Open(KTestDes);
	out.Open(buf);
	out.WriteL(in);
	testEofL(in);
	in.Open(aDes);
	TRAP(r,in.ReadL(out));
	test(r==KErrOverflow);
	out.CommitL();
	in.Open(buf);
	testReadL(in);
	testEofL(in);
	in.Open(buf);
	out.Open(aDes);
	out.WriteL(in);
	testEofL(in);
	out.CommitL();
	out.Close();
	in.Close();
	in.Open(aDes);
	testReadL(in);
	testEofL(in);
	in.Close();
	}

//
// Test writing to a buffer stream.
//
LOCAL_C void testWriteL(CBufBase& aBuf)
	{
	test.Next(_L("Writing to constructed stream"));
	RBufWriteStream out(aBuf);
	testWriteL(out);
	out.CommitL();
//
	test.Next(_L("Writing to opened stream"));
	out.Open(aBuf,KTestLength);
	testWriteL(out);
//
	test.Next(_L("Writing to inserting stream"));
	out.Insert(aBuf,KTestLength);
	testWriteL(out);
	out.Close();
	out.Insert(aBuf,0);
	testWriteL(out);
//
	test.Next(_L("Writing to appending stream"));
	out.Append(aBuf);
	testWriteL(out);
//
	test.Next(_L("Writing to truncating stream"));
	out.Truncate(aBuf,KTestLength);
	testWriteL(out);
	out.Close();
	out.Truncate(aBuf);
	testWriteL(out);
	out.CommitL();
	}

//
// Test reading from a buffer stream.
//
LOCAL_C void testReadL(const CBufBase& aBuf)
	{
	test.Next(_L("Reading from constructed stream"));
	RBufReadStream in(aBuf,KTestLength);
	TRAPD(r,testReadL(in));
	test(r==KErrEof);
	testEofL(in);
	in.Close();
//
	test.Next(_L("Reading from opened stream"));
	in.Open(aBuf);
	testReadL(in);
	testEofL(in);
	}

//
// Test skipping on a buffer stream.
//
LOCAL_C void testSkipL(const CBufBase& aBuf)
	{
	test.Next(_L("Skipping using small transfers"));
	RBufReadStream in(aBuf);
	testSkipL(in);
	testEofL(in);
	in.Close();
//
	test.Next(_L("Skipping using a single big transfer"));
	in.Open(aBuf);
	in.ReadL(KTestTotal);
	testEofL(in);
	in.Close();
	}

//
// Test copying buffer streams.
//
LOCAL_C void testCopyL(CBufBase& aBuf)
	{
	test.Next(_L("Copying using small transfers"));
	CBufBase* buf=0;
	TRAPD(r,buf=CBufFlat::NewL(KTestCopyExpandSize));
	if (r!=KErrNone)
		test.Panic(_L("Allocating buffer"));
	RBufReadStream in(aBuf);
	RBufWriteStream out(*buf);
	TRAP(r,testCopyL(out,in));
	test(r==KErrEof);
	testEofL(in);
	in.Open(aBuf,KTestTotal-2*KTestLength);
	TRAP(r,testCopyL(out,in));
	test(r==KErrEof);
	out.Close();
	testEofL(in);
	in.Open(*buf);
	out.Open(aBuf);
	testCopyL(out,in);
	testEofL(in);
	out.CommitL();
	in.Open(aBuf);
	testReadL(in);
	testEofL(in);
	delete buf;
	TRAP(r,buf=CBufSeg::NewL(KTestCopyExpandSize));
	if (r!=KErrNone)
		test.Panic(_L("Allocating buffer"));
	in.Open(aBuf);
	out.Open(*buf);
	testCopyL(out,in);
	testEofL(in);
	out.CommitL();
	in.Open(*buf);
	out.Open(aBuf);
	testCopyL(out,in);
	testEofL(in);
	out.Close();
	in.Close();
	in.Open(aBuf);
	testReadL(in);
	testEofL(in);
	in.Close();
//
	test.Next(_L("Copying using a single big transfer"));
	in.Open(aBuf);
	out.Truncate(*buf,KTestLength);
	TRAP(r,in.ReadL(out,KTestTotal+KTestLength));
	test(r==KErrEof);
	testEofL(in);
	in.Open(aBuf,KTestTotal);
	TRAP(r,out.WriteL(in,KTestTotal));
	test(r==KErrEof);
	out.CommitL();
	testEofL(in);
	in.Open(*buf);
	testReadL(in);
	in.ReadL(KTestLength);
	testEofL(in);
	delete buf;
	TRAP(r,buf=CBufFlat::NewL(KTestExpandSize));
	if (r!=KErrNone)
		test.Panic(_L("Allocating buffer"));
	in.Open(aBuf);
	out.Open(*buf);
	testCopyL(out,in);
	testEofL(in);
	out.CommitL();
	in.Open(*buf);
	out.Open(aBuf);
	in.ReadL(out,KTestTotal);
	testEofL(in);
	out.Close();
	in.Close();
	in.Open(aBuf);
	testReadL(in);
	testEofL(in);
	in.Close();
//
	test.Next(_L("Copying until end of file"));
	in.Open(aBuf,KTestLength);
	out.Truncate(*buf);
	out.WriteL(in);
	testEofL(in);
	in.Open(aBuf,KTestTotal-KTestLength);
	in.ReadL(out);
	out.CommitL();
	testEofL(in);
	in.Open(*buf);
	testReadL(in);
	testEofL(in);
	delete buf;
	TRAP(r,buf=CBufSeg::NewL(KTestExpandSize));
	if (r!=KErrNone)
		test.Panic(_L("Allocating buffer"));
	in.Open(aBuf);
	out.Open(*buf);
	testCopyL(out,in);
	testEofL(in);
	out.CommitL();
	in.Open(*buf);
	out.Open(aBuf);
	out.WriteL(in);
	testEofL(in);
	out.Close();
	in.Close();
	in.Open(aBuf);
	testReadL(in);
	testEofL(in);
	in.Close();
	delete buf;
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1173
@SYMTestCaseDesc	    Tests for defect number FER56EJNS
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for MStreamBuf::SizeL() function returning zero length.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void test_defect_FER_56EJNS()
	{
	const TPtrC8 p;
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1173 "));
	// don't replace p with TPtrC8() because gcc interprets it as a function declaration.
	RDesReadStream s(p);
	TInt sz=-1;
	TRAPD(r,sz=s.Source()->SizeL());
	test (r==KErrNone);
	test (sz==0);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1174
@SYMTestCaseDesc	    Memory streams test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for writing,reading,skipping and copying on memory streams operations
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testMemL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1174 Test memory streams "));
	Mem::Fill(TheBuf,KTestTotal,'|');
	TheBuf[KTestTotal]=KMaxTUint8;
	testWriteL(TheBuf);
	testReadL(TheBuf);
	testSkipL(TheBuf);
	testCopyL(TheBuf);
	test(TheBuf[KTestTotal]==KMaxTUint8);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1175
@SYMTestCaseDesc	    Tests for read,write,copy operations on descriptor streams buffer
@SYMTestPriority 	    High
@SYMTestActions  	    Test for writing,reading,skipping and copying on descriptor streams operations
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testDesL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1175 Test descriptor streams "));
	Mem::Fill(TheBuf,KTestTotal,'|');
	TheBuf[KTestTotal]=KMaxTUint8;
	TPtr8 des(TheBuf,KTestTotal);
	testWriteL(des);
	test(des.Length()==KTestTotal);
	testReadL(des);
	testSkipL(des);
	des.SetLength(KTestTotal-KTestLength+1);
	testCopyL(des);
	test(des.Length()==KTestTotal);
	test(TheBuf[KTestTotal]==KMaxTUint8);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1176
@SYMTestCaseDesc	    Buffer streaming test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by writing,reading,skipping and copying on buffer streams
                        Tests for panic during creation of a new buffer
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testBufL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1176 Test buffer streams "));
	CBufBase* buf=0;
	TRAPD(r,buf=CBufFlat::NewL(KTestExpandSize));
	if (r!=KErrNone)
		test.Panic(_L("Allocating buffer"));
	testWriteL(*buf);
	test(buf->Size()==KTestTotal);
	testReadL(*buf);
	testSkipL(*buf);
	buf->ResizeL(KTestTotal-KTestLength);
	testCopyL(*buf);
	test(buf->Size()==KTestTotal);
	delete buf;
//
	TRAP(r,buf=CBufSeg::NewL(KTestExpandSize));
	if (r!=KErrNone)
		test.Panic(_L("Allocating buffer"));
	testWriteL(*buf);
	test(buf->Size()==KTestTotal);
	testReadL(*buf);
	testSkipL(*buf);
	buf->ResizeL(KTestTotal-KTestLength);
	testCopyL(*buf);
	test(buf->Size()==KTestTotal);
	delete buf;
	}

/**
DEF038720 - RBufReadStream Close/Re-Open doesn't return to initial open state

@SYMTestCaseID          SYSLIB-STORE-CT-1177
@SYMTestCaseDesc	    Tests for defect number DEF038720
						RBufReadStream Close/Re-Open doesn't return to initial open state
@SYMTestPriority 	    High
@SYMTestActions  	    Write to buffer stream and read back,asserts if any problem while reading the buffer
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void test_DEF038720L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1177 "));
	CBufFlat* buf = CBufFlat::NewL(0x20);
	CleanupStack::PushL(buf);
	buf->ExpandL(0, 0x20);

	RBufWriteStream writeStream(*buf);
	writeStream.PushL();
	writeStream.WriteInt32L(1);
	CleanupStack::PopAndDestroy(&writeStream);

	TInt temp;
	RBufReadStream readStream;
	for(TInt i=0; i<2; i++)
		{
		readStream.Open(*buf);
		readStream.PushL();
		temp = readStream.ReadInt32L();
		__ASSERT_ALWAYS(temp==1, User::Invariant());   //Fails here 2nd time, if the defect is not fixed
		CleanupStack::PopAndDestroy(&readStream);
		}

	CleanupStack::PopAndDestroy(buf);
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

//
// Test memory-based streams.
//
GLDEF_C TInt E32Main()
    {
	test.Title();
	setupCleanup();
	__UHEAP_MARK;
//
	test.Start(_L("Test memory-based streams"));
	TRAPD(r,testMemL());
	test(r==KErrNone);
	TRAP(r,testDesL());
	test(r==KErrNone);
	TRAP(r,testBufL());
	test(r==KErrNone);
//
	test.Next(_L("Test defect fixes"));
	test_defect_FER_56EJNS();
	TRAP(r,::test_DEF038720L());
	test(r==KErrNone);
	test.End();
//
	__UHEAP_MARKEND;
	delete TheTrapCleanup;
	test.Close();
	return 0;
    }

