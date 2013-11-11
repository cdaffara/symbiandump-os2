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

#include <s32mem.h>
#include <e32test.h>
#include <s32btree.h>
#include "UB_STD.H"

const TInt KTestCleanupStack=0x20;
const TInt KTestExpandSize=0x20;
const TInt KTestGranularity=0x02;

const TUint8 KMidTUint8=KMaxTUint8/2;
const TUint16 KMidTUint16=KMaxTUint16/2;
const TUint32 KMidTUint32=KMaxTUint32/2;

LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RTest test(_L("t_storconv"));


template <class T1,class T2>
void testCopyL(T1& aCopy,const T2& anOriginal)
	{
	CBufSeg* buf=0;
	TRAPD(r,buf=CBufSeg::NewL(KTestExpandSize));
	if (r!=KErrNone)
		test.Panic(_L("Allocating buffer"));
//
// Write anOriginal out to the buffer.
//
	RBufWriteStream out;
	out.Append(*buf);
	TRAP(r,out<<anOriginal);
	test(r==KErrNone);
	TRAP(r,out.CommitL());
	if (r!=KErrNone)
		test.Panic(_L("Committing write stream"));
//
// Read anOriginal in from the buffer.
//
	RBufReadStream in(*buf);
	TRAP(r,in>>aCopy);
	test(r==KErrNone);
//
// See if it's consumed the lot.
//
	TUint8 b;
	test(in.Source()->ReadL(&b,1)==0);
//
	delete buf;
	}

//
// Clone aDes using memory-based streams.
//
template <class T>
HBufC8* testClone_HBufC8L(const T& aDes,TInt aMaxLength)
	{
	CBufSeg* buf=0;
	TRAPD(r,buf=CBufSeg::NewL(KTestExpandSize));
	if (r!=KErrNone)
		test.Panic(_L("Allocating buffer"));
//
// Write anOriginal out to the buffer.
//
	RBufWriteStream out;
	out.Append(*buf);
	TRAP(r,out<<aDes);
	test(r==KErrNone);
	TRAP(r,out.CommitL());
	if (r!=KErrNone)
		test.Panic(_L("Committing write stream"));
//
// Read anOriginal in from the buffer.
//
	RBufReadStream in(*buf);
	HBufC8* clone=NULL;
	TRAP(r,clone=HBufC8::NewL(in,aMaxLength));
	if (aMaxLength<aDes.Length())
		test(r==KErrOverflow);
	else
		{
		test(r==KErrNone);
		test(clone!=NULL);
//
// See if it's consumed the lot.
//
		TUint8 b;
		test(in.Source()->ReadL(&b,1)==0);
		}
//
	delete buf;
	return clone;
	}

//
// Clone aDes using memory-based streams.
//
template <class T>
HBufC16* testClone_HBufC16L(const T& aDes,TInt aMaxLength)
	{
	CBufSeg* buf=0;
	TRAPD(r,buf=CBufSeg::NewL(KTestExpandSize));
	if (r!=KErrNone)
		test.Panic(_L("Allocating buffer"));
//
// Write anOriginal out to the buffer.
//
	RBufWriteStream out;
	out.Append(*buf);
	TRAP(r,out<<aDes);
	test(r==KErrNone);
	TRAP(r,out.CommitL());
	if (r!=KErrNone)
		test.Panic(_L("Committing write stream"));
//
// Read anOriginal in from the buffer.
//
	RBufReadStream in(*buf);
	HBufC16* clone=NULL;
	TRAP(r,clone=HBufC16::NewL(in,aMaxLength));
	if (aMaxLength<aDes.Length())
		test(r==KErrOverflow);
	else
		{
		test(r==KErrNone);
		test(clone!=NULL);
//
// See if it's consumed the lot.
//
		TUint8 b;
		test(in.Source()->ReadL(&b,1)==0);
		}
//
	delete buf;
	return clone;
	}

//
// Clone aDes exactly using memory-based streams.
//
template <class T>
HBufC8* testClone_HBufC8L(const T& aDes)
	{
	return testClone_HBufC8L(aDes,aDes.Length());
	}

//
// Clone aDes exactly using memory-based streams.
//
template <class T>
HBufC16* testClone_HBufC16L(const T& aDes)
	{
	return testClone_HBufC16L(aDes,aDes.Length());
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1207
@SYMTestCaseDesc	    Streaming signed integers test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for streaming 8,16,32,64 bit signed integers.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testIntL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1207 Streaming TInt8 "));
	TInt8 int8=0;
	testCopyL(int8,TInt8(KMinTInt8));
	test(int8==KMinTInt8);
	testCopyL(int8,TInt8(KMinTInt8+1));
	test(int8==KMinTInt8+1);
	testCopyL(int8,TInt8(-2));
	test(int8==-2);
	testCopyL(int8,TInt8(-1));
	test(int8==-1);
	testCopyL(int8,TInt8(0));
	test(int8==0);
	testCopyL(int8,TInt8(1));
	test(int8==1);
	testCopyL(int8,TInt8(2));
	test(int8==2);
	testCopyL(int8,TInt8(KMaxTInt8-1));
	test(int8==KMaxTInt8-1);
	testCopyL(int8,TInt8(KMaxTInt8));
	test(int8==KMaxTInt8);
//
	test.Next(_L("Streaming TInt16"));
	TInt16 int16=0;
	testCopyL(int16,TInt16(KMinTInt16));
	test(int16==KMinTInt16);
	testCopyL(int16,TInt16(KMinTInt16+1));
	test(int16==KMinTInt16+1);
	testCopyL(int16,TInt16(-2));
	test(int16==-2);
	testCopyL(int16,TInt16(-1));
	test(int16==-1);
	testCopyL(int16,TInt16(0));
	test(int16==0);
	testCopyL(int16,TInt16(1));
	test(int16==1);
	testCopyL(int16,TInt16(2));
	test(int16==2);
	testCopyL(int16,TInt16(KMaxTInt16-1));
	test(int16==KMaxTInt16-1);
	testCopyL(int16,TInt16(KMaxTInt16));
	test(int16==KMaxTInt16);
//
	test.Next(_L("Streaming TInt32"));
	TInt32 int32=0;
	testCopyL(int32,TInt32(KMinTInt32));
	test(int32==KMinTInt32);
	testCopyL(int32,TInt32(KMinTInt32+1));
	test(int32==KMinTInt32+1);
	testCopyL(int32,TInt32(-2));
	test(int32==-2);
	testCopyL(int32,TInt32(-1));
	test(int32==-1);
	testCopyL(int32,TInt32(0));
	test(int32==0);
	testCopyL(int32,TInt32(1));
	test(int32==1);
	testCopyL(int32,TInt32(2));
	test(int32==2);
	testCopyL(int32,TInt32(KMaxTInt32-1));
	test(int32==KMaxTInt32-1);
	testCopyL(int32,TInt32(KMaxTInt32));
	test(int32==KMaxTInt32);
//
	test.Next(_L("Streaming TInt64"));
	TInt64 int64=0;
	testCopyL(int64,KMinTInt64);
	test(int64==KMinTInt64);
	testCopyL(int64,KMinTInt64+1);
	test(int64==KMinTInt64+1);
	testCopyL(int64,TInt64(-2));
	test(int64==-2);
	testCopyL(int64,TInt64(-1));
	test(int64==-1);
	testCopyL(int64,TInt64(0));
	test(int64==0);
	testCopyL(int64,TInt64(1));
	test(int64==1);
	testCopyL(int64,TInt64(2));
	test(int64==2);
	testCopyL(int64,KMaxTInt64-1);
	test(int64==KMaxTInt64-1);
	testCopyL(int64,KMaxTInt64);
	test(int64==KMaxTInt64);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1208
@SYMTestCaseDesc	    Streaming unsigned integers test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for streaming 8,16,32 bit unsigned integers.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testUintL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1208 Streaming TUint8 "));
   	TUint8 uint8=KMidTUint8;
	testCopyL(uint8,TUint8(0));
	test(uint8==0);
	testCopyL(uint8,TUint8(1));
	test(uint8==1);
	testCopyL(uint8,TUint8(KMidTUint8-2));
	test(uint8==KMidTUint8-2);
	testCopyL(uint8,TUint8(KMidTUint8-1));
	test(uint8==KMidTUint8-1);
	testCopyL(uint8,TUint8(KMidTUint8));
	test(uint8==KMidTUint8);
	testCopyL(uint8,TUint8(KMidTUint8+1));
	test(uint8==KMidTUint8+1);
	testCopyL(uint8,TUint8(KMidTUint8+2));
	test(uint8==KMidTUint8+2);
	testCopyL(uint8,TUint8(KMaxTUint8-1));
	test(uint8==KMaxTUint8-1);
	testCopyL(uint8,TUint8(KMaxTUint8));
	test(uint8==KMaxTUint8);
//
	test.Next(_L("Streaming TUint16"));
   	TUint16 uint16=KMidTUint16;
	testCopyL(uint16,TUint16(0));
	test(uint16==0);
	testCopyL(uint16,TUint16(1));
	test(uint16==1);
	testCopyL(uint16,TUint16(KMidTUint16-2));
	test(uint16==KMidTUint16-2);
	testCopyL(uint16,TUint16(KMidTUint16-1));
	test(uint16==KMidTUint16-1);
	testCopyL(uint16,TUint16(KMidTUint16));
	test(uint16==KMidTUint16);
	testCopyL(uint16,TUint16(KMidTUint16+1));
	test(uint16==KMidTUint16+1);
	testCopyL(uint16,TUint16(KMidTUint16+2));
	test(uint16==KMidTUint16+2);
	testCopyL(uint16,TUint16(KMaxTUint16-1));
	test(uint16==KMaxTUint16-1);
	testCopyL(uint16,TUint16(KMaxTUint16));
	test(uint16==KMaxTUint16);
//
	test.Next(_L("Streaming TUint32"));
   	TUint32 uint32=KMidTUint32;
	testCopyL(uint32,TUint32(0));
	test(uint32==0);
	testCopyL(uint32,TUint32(1));
	test(uint32==1);
	testCopyL(uint32,TUint32(KMidTUint32-2));
	test(uint32==KMidTUint32-2);
	testCopyL(uint32,TUint32(KMidTUint32-1));
	test(uint32==KMidTUint32-1);
	testCopyL(uint32,TUint32(KMidTUint32));
	test(uint32==KMidTUint32);
	testCopyL(uint32,TUint32(KMidTUint32+1));
	test(uint32==KMidTUint32+1);
	testCopyL(uint32,TUint32(KMidTUint32+2));
	test(uint32==KMidTUint32+2);
	testCopyL(uint32,TUint32(KMaxTUint32-1));
	test(uint32==KMaxTUint32-1);
	testCopyL(uint32,TUint32(KMaxTUint32));
	test(uint32==KMaxTUint32);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1209
@SYMTestCaseDesc	    Streaming Real numbers test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for streaming TReal32,TReal64 bit unsigned integers.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testRealL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1209 Streaming TReal32 "));
	TReal32 real32=TReal32(0);
	testCopyL(real32,TReal32(-31416.3));
	test(real32==TReal32(-31416.3));
	testCopyL(real32,TReal32(-0.0001));
	test(real32==TReal32(-0.0001));
	testCopyL(real32,TReal32(0));
	test(real32==TReal32(0));
	testCopyL(real32,TReal32(0.0001));
	test(real32==TReal32(0.0001));
	testCopyL(real32,TReal32(31416.3));
	test(real32==TReal32(31416.3));
//
	test.Next(_L("Streaming TReal64"));
	TReal64 real64=TReal64(0);
	testCopyL(real64,TReal64(-31416.3));
	test(real64==TReal64(-31416.3));
	testCopyL(real64,TReal64(-0.0001));
	test(real64==TReal64(-0.0001));
	testCopyL(real64,TReal64(0));
	test(real64==TReal64(0));
	testCopyL(real64,TReal64(0.0001));
	test(real64==TReal64(0.0001));
	testCopyL(real64,TReal64(31416.3));
	test(real64==TReal64(31416.3));
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1210
@SYMTestCaseDesc	    Streaming TPoint test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for copying two TPoint objects and test for integrity of copied object
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testPointL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1210 Streaming TPoint "));
	TPoint point=TPoint(0,0);
	testCopyL(point,TPoint(13,65));
	test(point==TPoint(13,65));
	testCopyL(point,TPoint(1,-1));
	test(point==TPoint(1,-1));
	testCopyL(point,TPoint(0,0));
	test(point==TPoint(0,0));
	testCopyL(point,TPoint(7,-666));
	test(point==TPoint(7,-666));
	testCopyL(point,TPoint(-13,-13));
	test(point==TPoint(-13,-13));
	testCopyL(point,TPoint(KMinTInt,KMaxTInt));
	test(point==TPoint(KMinTInt,KMaxTInt));
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1211
@SYMTestCaseDesc	    Streaming TSize objects test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for copying two TSize objects and test for integrity of copied object
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testSizeL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1211 Streaming TSize "));
	TSize point=TSize(0,0);
	testCopyL(point,TSize(13,65));
	test(point==TSize(13,65));
	testCopyL(point,TSize(1,-1));
	test(point==TSize(1,-1));
	testCopyL(point,TSize(0,0));
	test(point==TSize(0,0));
	testCopyL(point,TSize(7,-666));
	test(point==TSize(7,-666));
	testCopyL(point,TSize(-13,-13));
	test(point==TSize(-13,-13));
	testCopyL(point,TSize(KMinTInt,KMaxTInt));
	test(point==TSize(KMinTInt,KMaxTInt));
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1212
@SYMTestCaseDesc	    Streaming TRect objects test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for copying two TRect objects and test for integrity of copied object
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testRectL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1212 Streaming TRect"));
	TRect point=TRect(0,0,0,0);
	testCopyL(point,TRect(13,65,84,72));
	test(point==TRect(13,65,84,72));
	testCopyL(point,TRect(-13,-65,84,72));
	test(point==TRect(-13,-65,84,72));
	testCopyL(point,TRect(0,0,0,0));
	test(point==TRect(0,0,0,0));
	testCopyL(point,TRect(-1,1,1,-1));
	test(point==TRect(-1,1,1,-1));
	testCopyL(point,TRect(KMinTInt,KMinTInt,KMaxTInt,KMaxTInt));
	test(point==TRect(KMinTInt,KMinTInt,KMaxTInt,KMaxTInt));
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1213
@SYMTestCaseDesc	    Streaming descriptors tests
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for copying TDes8,TDes16 descriptors
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testDesL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1213 Streaming TDes8 "));
  	TBuf8<0x100> des8,copy8;
	des8=_L8("test1");
	testCopyL(copy8,des8);
	test(copy8==des8);
	des8.Append(_L8(" add some more text"));
	testCopyL(copy8,des8);
	test(copy8==des8);
	des8.SetMax();
	des8.Fill('?');
	testCopyL(copy8,des8);
	test(copy8==des8);
	des8.Zero();
	testCopyL(copy8,des8);
	test(copy8==des8);
//
 	test.Next(_L("Streaming TDes16"));
  	TBuf16<0x100> des16,copy16;
	des16=_L16("test1");
	testCopyL(copy16,des16);
	test(copy16==des16);
	des16.Append(_L16(" add some more text"));
	testCopyL(copy16,des16);
	test(copy16==des16);
	des16.SetMax();
	des16.Fill('?');
	testCopyL(copy16,des16);
	test(copy16==des16);
	des16.Zero();
	testCopyL(copy16,des16);
	test(copy16==des16);
//
	test.Next(_L("Streaming out TDes8 and streaming in TDes16"));
	des8=_L8("test1");
	testCopyL(copy16,des8);
	copy8.Copy(copy16);
	test(copy8==des8);
	des8.Append(_L8(" add some more text"));
	testCopyL(copy16,des8);
	copy8.Copy(copy16);
	test(copy8==des8);
	des8.SetMax();
	des8.Fill('?');
	testCopyL(copy16,des8);
	copy8.Copy(copy16);
	test(copy8==des8);
	des8.Zero();
	testCopyL(copy16,des8);
	copy8.Copy(copy16);
	test(copy8==des8);
//
	test.Next(_L("Streaming out TDes16 and streaming in TDes8"));
	des16=_L16("test1");
	testCopyL(copy8,des16);
	copy16.Copy(copy8);
	test(copy16==des16);
	des16.Append(_L16(" add some more text"));
	testCopyL(copy8,des16);
	copy16.Copy(copy8);
	test(copy16==des16);
	des16.SetMax();
	des16.Fill('?');
	testCopyL(copy8,des16);
	copy16.Copy(copy8);
	test(copy16==des16);
	des16.Zero();
	testCopyL(copy8,des16);
	copy16.Copy(copy8);
	test(copy16==des16);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1214
@SYMTestCaseDesc	    Streaming HBufC test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by cloning and check for the integrity of the cloned object.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testHBufCL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1214 Streaming HBufC8 "));
  	TBuf8<0x100> des8,copy8;
  	HBufC8* buf8;
	des8=_L8("test1");
	buf8=testClone_HBufC8L(des8);
	test(*buf8==des8);
	User::Free(buf8);
	des8.Append(_L8(" add some more text"));
	buf8=testClone_HBufC8L(des8);
	test(*buf8==des8);
	User::Free(buf8);
	des8.SetMax();
	des8.Fill('?');
	buf8=testClone_HBufC8L(des8);
	test(*buf8==des8);
	User::Free(buf8);
	des8.Zero();
	buf8=testClone_HBufC8L(des8);
	test(*buf8==des8);
	User::Free(buf8);
//
 	test.Next(_L("Streaming HBufC16"));
  	TBuf16<0x100> des16,copy16;
  	HBufC16* buf16;
	des16=_L16("test1");
	buf16=testClone_HBufC16L(des16);
	test(*buf16==des16);
	User::Free(buf16);
	des16.Append(_L16(" add some more text"));
	buf16=testClone_HBufC16L(des16);
	test(*buf16==des16);
	User::Free(buf16);
	des16.SetMax();
	des16.Fill('?');
	buf16=testClone_HBufC16L(des16);
	test(*buf16==des16);
	User::Free(buf16);
	des16.Zero();
	buf16=testClone_HBufC16L(des16);
	test(*buf16==des16);
	User::Free(buf16);
//
	test.Next(_L("Streaming out TDes8 and streaming in HBufC16"));
	des8=_L8("test1");
	buf16=testClone_HBufC16L(des8);
	copy8.Copy(*buf16);
	test(copy8==des8);
	User::Free(buf16);
	des8.Append(_L8(" add some more text"));
	buf16=testClone_HBufC16L(des8);
	copy8.Copy(*buf16);
	test(copy8==des8);
	User::Free(buf16);
	des8.SetMax();
	des8.Fill('?');
	buf16=testClone_HBufC16L(des8);
	copy8.Copy(*buf16);
	test(copy8==des8);
	User::Free(buf16);
	des8.Zero();
	buf16=testClone_HBufC16L(des8);
	copy8.Copy(*buf16);
	test(copy8==des8);
	User::Free(buf16);
//
	test.Next(_L("Streaming out TDes16 and streaming in HBufC8"));
	des16=_L16("test1");
	buf8=testClone_HBufC8L(des16);
	copy16.Copy(*buf8);
	test(copy16==des16);
	User::Free(buf8);
	des16.Append(_L16(" add some more text"));
	buf8=testClone_HBufC8L(des16);
	copy16.Copy(*buf8);
	test(copy16==des16);
	User::Free(buf8);
	des16.SetMax();
	des16.Fill('?');
	buf8=testClone_HBufC8L(des16);
	copy16.Copy(*buf8);
	test(copy16==des16);
	User::Free(buf8);
	des16.Zero();
	buf8=testClone_HBufC8L(des16);
	copy16.Copy(*buf8);
	test(copy16==des16);
	User::Free(buf8);
//
	test.Next(_L("Overflowing and over-allocating HBufC8"));
	des8.SetMax();
	buf8=testClone_HBufC8L(des8,0x80);
	test(buf8==NULL);
	buf8=testClone_HBufC8L(des8,0x200);
	test(buf8!=NULL);
	test(User::AllocLen(buf8)==sizeof(TBufC8<0x100>));
	User::Free(buf8);
//
	test.Next(_L("Overflowing and over-allocating HBufC16"));
	des16.SetMax();
	buf16=testClone_HBufC16L(des16,0x80);
	test(buf16==NULL);
	buf16=testClone_HBufC16L(des16,0x200);
	test(buf16!=NULL);
	test(User::AllocLen(buf16)==sizeof(TBufC16<0x100>));
	User::Free(buf16);
}

//
// Compare two buffers.
//
TBool operator==(const CBufBase& aBuf,const CBufBase& anotherBuf)
	{
	TInt s=aBuf.Size();
	if (s!=anotherBuf.Size())
		return EFalse;
//
	for (TInt i=0;i<s;)
		{
		TPtr8 p1=((CBufBase&)aBuf).Ptr(i);
		TPtr8 p2=((CBufBase&)anotherBuf).Ptr(i);
		TInt n=Min(p1.Size(),p2.Size());
		if (Mem::Compare(p1.Ptr(),n,p2.Ptr(),n)!=0)
			return EFalse;
//
		i+=n;
		}
	return ETrue;
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1215
@SYMTestCaseDesc	    Streaming CBufFlat,CBufSeg buffers test
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for copying two buffer objects
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testBufL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1215 Streaming CBufFlat "));
	const TText8* data=_S8("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	CBufBase* bufOrg=CBufFlat::NewL(36);
	CleanupStack::PushL(bufOrg);
	CBufBase* buf=CBufFlat::NewL(36);
	CleanupStack::PushL(buf);
	bufOrg->InsertL(0,data,36);
	testCopyL(*buf,*bufOrg);
	test(*buf==*bufOrg);
	buf->Delete(0,10);
	testCopyL(*buf,*bufOrg);
	test(*buf==*bufOrg);
	buf->Delete(0,10);
	buf->InsertL(26,data,10);
	testCopyL(*buf,*bufOrg);
	test(*buf==*bufOrg);
	buf->InsertL(0,data+10,26);
	testCopyL(*buf,*bufOrg);
	test(*buf==*bufOrg);
	CleanupStack::PopAndDestroy(2);
//
	test.Next(_L("Streaming CBufSeg"));
	bufOrg=CBufSeg::NewL(7);
	CleanupStack::PushL(bufOrg);
	buf=CBufSeg::NewL(11);
	CleanupStack::PushL(buf);
	bufOrg->InsertL(0,data,36);
	testCopyL(*buf,*bufOrg);
	test(*buf==*bufOrg);
	buf->Delete(0,10);
	testCopyL(*buf,*bufOrg);
	test(*buf==*bufOrg);
	buf->Delete(0,10);
	buf->InsertL(26,data,10);
	testCopyL(*buf,*bufOrg);
	test(*buf==*bufOrg);
	testCopyL(*buf,*bufOrg);
	test(*buf==*bufOrg);
	CleanupStack::PopAndDestroy(2);
	}

template <class T>
TBool operator==(const CArrayFix<T>& anArray,const CArrayFix<T>& anotherArray);
#if defined(__GCC32__)
template <class T>
inline TBool operator==(const CArrayFixFlat<T>& anArray,const CArrayFixFlat<T>& anotherArray)
	{return (const CArrayFix<T>&)anArray==(const CArrayFix<T>&)anotherArray;}
#endif

//
// Compare two arrays.
//
template <class T>
TBool operator==(const CArrayFix<T>& anArray,const CArrayFix<T>& anotherArray)
	{
	TInt n=anArray.Count();
	if (n!=anotherArray.Count())
		return EFalse;
//
	for (TInt i=0;i<n;i++)
		{
		if (anArray[i]!=anotherArray[i])
			return EFalse;
		}
	return ETrue;
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1216
@SYMTestCaseDesc	    Streaming fixed arrays test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by copying two fixed array objects.Tests for the integrity of the data.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testArrayFixL()
	{
//*	test.Next(_L("Streaming CArrayFix<TUint32>"));
//#pragma message ( __FILE__ " : 'testArrayFix()' not entirely implemented")
//
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1216 Streaming CArrayFix<TDes> "));
	TBuf<16> des[]={_L("aa"),_L("bbbb"),_L("cccccc")};
	CArrayFixFlat< TBuf<16> >* fixDesOrg=new(ELeave) CArrayFixFlat< TBuf<16> >(KTestGranularity);
	CleanupStack::PushL(fixDesOrg);
	CArrayFixFlat< TBuf<16> >* fixDes=new(ELeave) CArrayFixFlat< TBuf<16> >(KTestGranularity);
	CleanupStack::PushL(fixDes);
	fixDesOrg->AppendL(des+1,2);
	testCopyL(*fixDes,*fixDesOrg);
	test(*fixDes==*fixDesOrg);
	fixDesOrg->InsertL(0,des[0]);
	testCopyL(*fixDes,*fixDesOrg);
	test(*fixDes==*fixDesOrg);
	CleanupStack::PopAndDestroy(2);
	}

/**
@SYMTestCaseID          PDS-STORE-CT-4011
@SYMTestCaseDesc	    Test for US_SHARE.CPP file. RShareReadStream.
@SYMTestPriority 	    High
@SYMTestActions  	    Test for creating and opening, read, write functions in RShareReadStream.
@SYMTestExpectedResults Creation and opening must not fail. 
						Written data, and read data should be equal. 
@SYMDEF                 DEF135804
*/
LOCAL_C void testFileL()
	{

	_LIT8(KTestString, "Test String test");
	test.Next(_L("PDS-STORE-CT-4011 testUS_SHARE_FileL"));
	
	HBufC8* rwbuf = HBufC8::NewLC(1024*10);
	HBufC8* buf = HBufC8::NewLC(1024);
	TPtr8 des = buf->Des();
	TDesBuf tdb;
	TPtr8 des2 = rwbuf->Des();
	tdb.Set( des2 );
	
	TStreamExchange se(&tdb);
	
	RShareWriteStream wstream(se);
	wstream.WriteL(KTestString);
	wstream.CommitL();
	wstream.Close();
	
	RShareReadStream rstream(se);
	rstream.ReadL(des, buf->Length());
	rstream.Close();
	
	test( des.Compare(KTestString) );

	RShareWriteStream wstream2;
	wstream2.Open(se);
	wstream2.WriteL(KTestString);
	wstream2.CommitL();
	wstream2.Close();

	RShareReadStream rstream2;
	rstream2.Open(se);
	rstream2.ReadL(des, buf->Length());
	rstream2.Close();

	test(des.Compare(KTestString));
	
	CleanupStack::PopAndDestroy(2, rwbuf);
	
	}

/**
@SYMTestCaseID          PDS-STORE-CT-4012
@SYMTestCaseDesc	    Test for US_SHARE.CPP file. RShareBuf.
@SYMTestPriority 	    High
@SYMTestActions  	    Writing, reading and seeking in RShareBuf
@SYMTestExpectedResults Written data and read data (after seek to the beggining of the bufer)
						should equal. Read, write and seek operation must not fail.
@SYMDEF                 DEF135804
*/
class RShareBufTest : public RShareBuf
	{
	public:
	TInt DoReadL(TAny* aPtr,TInt aMaxLength){return RShareBuf::DoReadL(aPtr, aMaxLength);}
	TInt DoReadL(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus){return RShareBuf::DoReadL(aDes, aMaxLength, aStatus);}
	TStreamTransfer DoReadL(MStreamInput& anInput,TStreamTransfer aTransfer){return RShareBuf::DoReadL(anInput, aTransfer);}
	void DoWriteL(const TAny* aPtr,TInt aLength){RShareBuf::DoWriteL(aPtr, aLength);}
	TInt DoWriteL(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus){return RShareBuf::DoWriteL(aDes,aMaxLength, aStatus);}
	TStreamTransfer DoWriteL(MStreamOutput& anOutput,TStreamTransfer aTransfer){return RShareBuf::DoWriteL(anOutput, aTransfer);}
	TStreamPos DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset){return RShareBuf::DoSeekL(aMark,aLocation,anOffset);}
	};

LOCAL_C void testFileRShareBufL()
{
	_LIT8(KTestString, "Test String test very long to have something to seek for");
	HBufC8* rwbuf = HBufC8::NewLC(1024*10);
	HBufC8* buf = HBufC8::NewLC(1024);
	TDesBuf tdb;
	TPtr8 des2 = rwbuf->Des();
	tdb.Set( des2 );
	
	TStreamExchange se(&tdb);
	
	test.Next(_L("PDS-STORE-CT-4012 RShareBuf tests"));
	RShareBufTest rsb;
	rsb.Release();
	rsb.Open(se, RShareBuf::ERead|RShareBuf::EWrite);
	rsb.WriteL(KTestString().Ptr(), KTestString().Length());
	TInt dupa = rsb.SizeL();
	
	TStreamPos spos = rsb.DoSeekL(RShareBuf::ERead, EStreamBeginning, 20);
	test(spos.Offset() == 20);
	spos = rsb.DoSeekL(RShareBuf::ERead, EStreamMark, 5);
	test(spos.Offset()== 25);
	spos = rsb.DoSeekL(RShareBuf::EWrite, EStreamEnd, -5);
	test(spos.Offset()==rsb.SizeL()-5 );
	
	TUint8* tempBuf = new TUint8[1024];
	CleanupStack::PushL(tempBuf);
	test(tempBuf != NULL);
	for(TInt i=0;i<1024;i++)
		tempBuf[i]=0;
	
	spos = rsb.DoSeekL(RShareBuf::ERead, EStreamBeginning, 0);
	test(spos.Offset() == 0);
	TInt bytesread = rsb.DoReadL((void*)tempBuf, rsb.SizeL());
	test(bytesread == rsb.SizeL());
	buf->Des().Copy(tempBuf);
	test(buf->Des().Compare(KTestString)==0);
	
	spos = rsb.DoSeekL(RShareBuf::EWrite, EStreamBeginning, 0);
	test(spos.Offset() == 0);
	
	TRequestStatus status;
	rsb.DoWriteL(KTestString(), KTestString().Length(), status);
	User::WaitForRequest(status);
	test(status == KErrNone);
	
	buf->Des().Zero();
	TPtr8 pbuf2 = buf->Des();
	
	spos = rsb.DoSeekL(RShareBuf::ERead, EStreamBeginning, 0);
	test(spos.Offset() == 0);
	bytesread = rsb.ReadL(pbuf2, buf->Des().MaxLength(), status);
	test(bytesread == buf->Des().MaxLength());
	User::WaitForRequest(status);
	test(status == KErrNone);
	test(buf->Des().Compare(KTestString)==0);
	
	/**
	 * Unable to test those functions:
	 * TStreamTransfer DoWriteL(MStreamOutput& anOutput,TStreamTransfer aTransfer)
	 * TStreamTransfer DoReadL(MStreamInput& anInput,TStreamTransfer aTransfer);
	 * because all MStreamOutput/MStreamInput derived class are not exported
	 * thus it can't be used outside theis DLL
	 */
	
	rsb.Close();
	CleanupStack::PopAndDestroy(3, rwbuf);
}

/**
@SYMTestCaseID          PDS-STORE-CT-4013
@SYMTestCaseDesc	    Test for US_HOST.CPP file. TStreamExchange and TStreamMark.
@SYMTestPriority 	    High
@SYMTestActions  	    Read and write on TStreamExchange. Reading and writing on TStreamMark.
@SYMTestExpectedResults Read and write operation should not leave.
@SYMDEF                 DEF135804
*/
LOCAL_C void testTStreamExchangeAndMarkL()
{
	test.Next(_L("PDS-STORE-CT-4013 testTStreamExchangeAndMarkL"));
	_LIT8(KTestString, "Test String test very long to have something to seek for");
	HBufC8* rwbuf = HBufC8::NewLC(1024*10);
	HBufC8* buf = HBufC8::NewLC(1024);
	TDesBuf tdb;
	TPtr8 des2 = rwbuf->Des();
	tdb.Set( des2 );
	TStreamExchange se(&tdb);
	RShareBufTest rsb;
	rsb.Open(se, RShareBuf::ERead|RShareBuf::EWrite);
	test( se.HostL() == &tdb );
	
	TStreamMark sm(0);
	TInt bytesprocessed;
	sm.WriteL(se, KTestString().Ptr(),KTestString().Length() );
	TRequestStatus rstatus;
	
	TStreamMark sm2(0);
	bytesprocessed = sm2.WriteL(se, KTestString(), KTestString().Length(), rstatus);
	test (bytesprocessed == KTestString().Length());
	User::WaitForRequest(rstatus);
	test(rstatus == KErrNone);
	
	TBool rc = sm == sm2;
	test(!rc);
	rc = sm == (const TStreamMark*)&sm2;
	test(!rc);
	
	rc = sm != sm2;
	test(rc);
	rc = sm != (const TStreamMark*)&sm2;
	test(rc);
	
	rc = sm.IsWith(se);
	test(!rc);
	rc = sm2.IsWith(se);
	test(rc);
	
	TStreamMark sm3(0);
	TPtr8 des = buf->Des();
	bytesprocessed = sm3.ReadL(se, des, des.MaxLength(), rstatus);
	RDebug::Printf("Written: %d, Length: %d",bytesprocessed,des.MaxLength());
	test (bytesprocessed == des.MaxLength());
	User::WaitForRequest(rstatus);
	test(rstatus == KErrNone);
	
	TStreamMark sm4(0);
	buf->Des().Zero();
	des.Set(buf->Des());
	bytesprocessed = sm4.ReadL(se, des, rstatus);
	RDebug::Printf("Written: %d, Length: %d",bytesprocessed,des.MaxLength());
	test (bytesprocessed == des.MaxLength());
	sm4.ExtractL(se);
	User::WaitForRequest(rstatus);
	test(rstatus == KErrNone);
	
	TStreamMark sm5(0);
	bytesprocessed = sm5.WriteL(se, KTestString(), rstatus);
	RDebug::Printf("Written: %d, Length: %d",bytesprocessed,KTestString().Length());
	test (bytesprocessed == KTestString().Length());
	User::WaitForRequest(rstatus);
	test(rstatus == KErrNone);
	sm5.ExtractL(se);
	
	rsb.Close();
	CleanupStack::PopAndDestroy(2, rwbuf);
}

const TInt KMinTestHeapSize = 0x10000;
const TInt KMaxTestHeapSize = 0x100000;

/**
 * Helper function for PDS-STORE-CT-4014
 */
LOCAL_C void DoTBtreeKeyPanicL(TInt aCase)
	{
	//those will panic, and should be tested if they will panic
	switch(aCase)
		{
		case 0:
			{
			TBtreeKey key27(0,ECmpTUint);
			}
		case 1:
			{
			TBtreeKey key23(0,ECmpTInt);
			}
		case 2:
			{
			TBtreeKey key9(0,ECmpCollated);
			}
		case 3:
			{
			TBtreeKey key6(0,ECmpFolded);
			}
		case 4:
			{
			TBtreeKey key3(0,ECmpNormal);
			}
		case 5:
			{
			TBtreeKey key91(0,ECmpCollated,10);
			}
		case 6:
			{
			TBtreeKey key61(0,ECmpFolded,10);
			}
		case 7:
			{
			TBtreeKey key31(0,ECmpNormal,10);
			}
		}
	}
/**
 * Helper function for PDS-STORE-CT-4014
 */
LOCAL_C TInt DoPanicingThread(TAny* aTestCase)
	{
	User::SetJustInTime(EFalse);	// disable debugger panic handling
	TInt tcase = *((TInt*)aTestCase);
	TRAP_IGNORE(DoTBtreeKeyPanicL(tcase));		
	return 0;
	}

/**
@SYMTestCaseID          PDS-STORE-CT-4014
@SYMTestCaseDesc	    Test for UB_KEY.CPP file. TBtreeKey constructors, comparators, function Between().
@SYMTestPriority 	    High
@SYMTestActions  	    Create instance of TBtreeKey using all available constructors with 
						all possible parameters. Run comparation and between functions.
@SYMTestExpectedResults Objects must be created properly. Comparators must return proper values.
						MBtreeKey Between function, not fully implemented. See comments inside test.
@SYMDEF                 DEF135804
*/
LOCAL_C void testMBtreeKeyL()
{
	test.Next(_L("PDS-STORE-CT-4014 MBtreeKey Constructors"));
	TBtreeKey key;
	TBtreeKey key_1(0);
	TBtreeKey key2(0, ECmpTInt16);
	//cmp text

	for(TInt i=0;i<8;i++)
		{
		RThread thread;
		TInt tcase = i;
		TRequestStatus rst;
		TFileName name;
		name.Copy(_L("MyPanic "));
		name.AppendNum(i);
		TInt err = thread.Create(name, DoPanicingThread, KDefaultStackSize, KMinTestHeapSize, KMaxTestHeapSize, &tcase, EOwnerThread);
		test(err == KErrNone);
		TRequestStatus status;
		thread.Logon(status);
		test(status.Int() == KRequestPending);
		thread.Resume();
		User::WaitForRequest(status);
		User::SetJustInTime(ETrue);	// enable debugger panic handling
		test(thread.ExitType() == EExitPanic);
		test(thread.ExitReason() == EInvalidKeyComparison );
		RDebug::Printf("Thread %d paniced as design with correct panic code", i);
		}
	
	test.Next(_L("MBtreeKey Comparators"));
	//prepare 8bit descriptor
	TBuf8<50> textKey1;
	TBuf8<50> textKey2;
	textKey1.Copy(_L("  Ala ma kota"));
	textKey2.Copy(_L("  Ala ma kota"));
	TUint8* ptr = const_cast<TUint8*>(textKey1.PtrZ());
	*ptr = textKey1.Length();
	ptr = const_cast<TUint8*>(textKey2.PtrZ());
	*ptr = textKey2.Length();
	
	//prepare 16bit descriptor
	TBuf16<50> text16Key1;
	TBuf16<50> text16Key2;
	text16Key1.Copy(_L("  Ala ma kota"));
	text16Key2.Copy(_L("  Ala ma kota"));
	TUint16* ptr16 = const_cast<TUint16*>(text16Key1.PtrZ());
	*ptr16 = text16Key1.Length();
	ptr16 = const_cast<TUint16*>(text16Key2.PtrZ());
	*ptr16 = text16Key2.Length();

	
	TBtreeKey key4(0,ECmpNormal8);
	test(key4.Compare(textKey1.PtrZ(),textKey2.PtrZ()) == 0);	
	TBtreeKey key4_1(0,ECmpNormal8,13);
	test(key4_1.Compare(textKey1.PtrZ(),textKey2.PtrZ()) == 0);
	
	TBtreeKey key5(0,ECmpNormal16);
	test(key5.Compare(text16Key1.PtrZ(),text16Key2.PtrZ()) == 0);
	TBtreeKey key5_1(0,ECmpNormal16,13);
	test(key5_1.Compare(text16Key1.PtrZ(),text16Key2.PtrZ()) == 0);

	TBtreeKey key7(0,ECmpFolded8);
	test(key7.Compare(textKey1.PtrZ(),textKey2.PtrZ()) == 0);
	TBtreeKey key7_1(0,ECmpFolded8,13);
	test(key7_1.Compare(textKey1.PtrZ(),textKey2.PtrZ()) == 0);
	
	TBtreeKey key8(0,ECmpFolded16);
	test(key8.Compare(text16Key1.PtrZ(),text16Key2.PtrZ()) == 0);
	TBtreeKey key8_1(0,ECmpFolded16,13);
	test(key8_1.Compare(text16Key1.PtrZ(),text16Key2.PtrZ()) == 0);

	TBtreeKey key10(0,ECmpCollated8);
	test(key10.Compare(textKey1.PtrZ(),textKey2.PtrZ()) == 0);
	TBtreeKey key10_1(0,ECmpCollated8,13);
	test(key10_1.Compare(textKey1.PtrZ(),textKey2.PtrZ()) == 0);
	
	TBtreeKey key11(0,ECmpCollated16);
	test(key11.Compare(text16Key1.PtrZ(),text16Key2.PtrZ()) == 0);
	TBtreeKey key11_1(0,ECmpCollated16,13);
	test(key11_1.Compare(text16Key1.PtrZ(),text16Key2.PtrZ()) == 0);
	
	//cmp numeric
	TInt8 int8_1 = 123;
	TInt8 int8_2 = 123;
	TInt16 int16_1 = 32000;
	TInt16 int16_2 = 32000;
	TInt32 int32_1 = 2147480000; 
	TInt32 int32_2 = 2147480000;
	TInt64 int64_1 = 9223372036854770000; 
	TInt64 int64_2 = 9223372036854770000;
	
	TBtreeKey key20(0,ECmpTInt8);
	test( key20.Compare(&int8_1, &int8_2)==0);
	TBtreeKey key21(0,ECmpTInt16);
	test( key21.Compare(&int16_1, &int16_2)==0);
	TBtreeKey key22(0,ECmpTInt32);
	test( key22.Compare(&int32_1, &int32_2)==0);

	TBtreeKey key24(0,ECmpTUint8);
	test( key24.Compare(&int8_1, &int8_2)==0);
	TBtreeKey key25(0,ECmpTUint16);
	test( key25.Compare(&int16_1, &int16_2)==0);
	TBtreeKey key26(0,ECmpTUint32);
	test( key26.Compare(&int32_1, &int32_2)==0);
	TBtreeKey key28(0,ECmpTInt64);
	test( key28.Compare(&int64_1, &int64_2)==0);
	
	//one is bigger
	int32_1+=4;
	int64_1+=54;
	test( key22.Compare(&int32_1, &int32_2)==1);
	test( key22.Compare(&int32_2, &int32_1)==-1);
	test( key26.Compare(&int32_1, &int32_2)==1);
	test( key26.Compare(&int32_2, &int32_1)==-1);
	test( key28.Compare(&int64_1, &int64_2)==1);
	test( key28.Compare(&int64_2, &int64_1)==-1);
	
	//prepare second 16bit descriptor to be greater than first one
	text16Key2.Copy(_L("  Ala mb kotb"));
	ptr16 = const_cast<TUint16*>(text16Key2.PtrZ());
	*ptr16 = text16Key2.Length();
	
	//prepare second 8bit descriptor to be greater than first one	
	textKey2.Copy(_L("  Ala mb kotb"));
	ptr = const_cast<TUint8*>(textKey2.PtrZ());
	*ptr = textKey2.Length();
	
	//testing Between function
	TBtreePivot pivot;
	
	test.Next(_L("MBtreeKey Between function, not fully implemented. See comments."));
	/**
	 * For all tests bellow we should made tests if
	 * pivot > textKey1 && pivot < textKey2
	 * but function between is not properly implemented yet
	 * then we don't check result
	 */
	
	//TBtreeKey key4(0,ECmpNormal8);
	key4.Between(textKey1.PtrZ(),textKey2.PtrZ(),pivot);
	//TBtreeKey key4_1(0,ECmpNormal8,13);
	key4_1.Between(textKey1.PtrZ(),textKey2.PtrZ(),pivot);
	
	//TBtreeKey key5(0,ECmpNormal16);
	key5.Between(text16Key1.PtrZ(),text16Key2.PtrZ(),pivot);
	//TBtreeKey key5_1(0,ECmpNormal16,13);
	key5_1.Between(text16Key1.PtrZ(),text16Key2.PtrZ(),pivot);
	
	//TBtreeKey key7(0,ECmpFolded8);
	key7.Between(textKey1.PtrZ(),textKey2.PtrZ(),pivot);
	//TBtreeKey key7_1(0,ECmpFolded8,13);
	key7_1.Between(textKey1.PtrZ(),textKey2.PtrZ(),pivot);
		
	//TBtreeKey key8(0,ECmpFolded16);
	key8.Between(text16Key1.PtrZ(),text16Key2.PtrZ(),pivot);
	//TBtreeKey key8_1(0,ECmpFolded16,13);
	key8_1.Between(text16Key1.PtrZ(),text16Key2.PtrZ(),pivot);
		
	//TBtreeKey key10(0,ECmpCollated8);
	key10.Between(textKey1.PtrZ(),textKey2.PtrZ(),pivot);
	//TBtreeKey key10_1(0,ECmpCollated8,13);
	key10_1.Between(textKey1.PtrZ(),textKey2.PtrZ(),pivot);
	
	//TBtreeKey key11(0,ECmpCollated16);
	key11.Between(text16Key1.PtrZ(),text16Key2.PtrZ(),pivot);
	//TBtreeKey key11_1(0,ECmpCollated16,13);
	key11_1.Between(text16Key1.PtrZ(),text16Key2.PtrZ(),pivot);
	
	//TBtreeKey key20(0,ECmpTInt8);
	key20.Between(&int8_1, &int8_2,pivot);
	//TBtreeKey key21(0,ECmpTInt16);
	key21.Between(&int16_1, &int16_2,pivot);
	//TBtreeKey key22(0,ECmpTInt32);
	key22.Between(&int32_1, &int32_2,pivot);

	//TBtreeKey key24(0,ECmpTUint8);
	key24.Between(&int8_1, &int8_2,pivot);
	//TBtreeKey key25(0,ECmpTUint16);
	key25.Between(&int16_1, &int16_2,pivot);
	//TBtreeKey key26(0,ECmpTUint32);
	key26.Between(&int32_1, &int32_2,pivot);
	//TBtreeKey key28(0,ECmpTInt64);
	key28.Between(&int64_1, &int64_2,pivot);
	
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
// Test streaming conversions.
//
GLDEF_C TInt E32Main()
    {
	test.Title();
	setupCleanup();
	__UHEAP_MARK;
//
	test.Start(_L("Test streaming conversions"));
	TRAPD(r,testIntL());
	test(r==KErrNone);
	TRAP(r,testUintL());
	test(r==KErrNone);
	TRAP(r,testRealL());
	test(r==KErrNone);
	TRAP(r,testDesL());
	test(r==KErrNone);
	TRAP(r,testHBufCL());
	test(r==KErrNone);
	TRAP(r,testBufL());
	test(r==KErrNone);
	TRAP(r,testArrayFixL());
	test(r==KErrNone);
	TRAP(r,testPointL());
	test(r==KErrNone);
	TRAP(r,testSizeL());
	test(r==KErrNone);
	TRAP(r,testRectL());
	test(r==KErrNone);
	//
	TRAP(r,testFileL());
	test(r==KErrNone);
	TRAP(r,testFileRShareBufL());
	test(r==KErrNone);	
	TRAP(r,testTStreamExchangeAndMarkL());
	test(r==KErrNone);	
	TRAP(r,testMBtreeKeyL());
	test(r==KErrNone);	
	test.End();
//
	__UHEAP_MARKEND;
	delete TheTrapCleanup;
	test.Close();
	return 0;
    }

