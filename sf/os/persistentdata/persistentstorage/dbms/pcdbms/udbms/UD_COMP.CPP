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
// implementation for d32comp.h
// 
//

#include "UD_STD.H"
#include "D32COMP.H"

const TInt KCompareBufSize=0x200;	// 0.5K

// class Comp

TInt Comp::Compare(TReal32 aLeft,TReal32 aRight) __SOFTFP
	{
	return aLeft<aRight ? -1 : aLeft!=aRight;
	}

TInt Comp::Compare(TReal64 aLeft,TReal64 aRight) __SOFTFP
	{
	return aLeft<aRight ? -1 : aLeft!=aRight;
	}

TInt Comp::Compare(const TInt64& aLeft,const TInt64& aRight)
	{
	return aLeft<aRight ? -1 : aLeft!=aRight;
	}

TInt Comp::Compare8L(MStreamBuf& aBufL,MStreamBuf& aBufR,TInt aSize,const TTextOps& aTextOp)
	{
	const TInt KCompareBufLength=KCompareBufSize/sizeof(TText8);
	RReadStream left(&aBufL);
	RReadStream right(&aBufR);
	TText8 bufL[KCompareBufLength];
	TText8 bufR[KCompareBufLength];
	TInt read=KCompareBufLength;
	do
		{
		if (aSize<=read)
			read=aSize;
		left.ReadL(bufL,read);
		right.ReadL(bufR,read);
		TInt r=aTextOp.Compare(bufL,read,bufR,read);
		if (r!=0)
			return r;
		aSize-=KCompareBufLength;
		} while (aSize>0);
	return 0;
	}

TInt Comp::Compare8L(MStreamBuf& aBuf,const TText8* aPtr,TInt aSize,const TTextOps& aTextOp)
	{
	const TInt KCompareBufLength=KCompareBufSize/sizeof(TText8);
	RReadStream strm(&aBuf);
	TText8 buf[KCompareBufLength];
	TInt read=KCompareBufLength;
	do
		{
		if (aSize<read)
			read=aSize;
		strm.ReadL(buf,read);
		TInt r=aTextOp.Compare(buf,read,aPtr,read);
		if (r!=0)
			return r;
		aPtr+=KCompareBufLength;
		aSize-=KCompareBufLength;
		} while (aSize>0);
	return 0;
	}

TInt Comp::Compare8L(MStreamBuf& aBuf,TInt aSize,const TDesC8& aDes,const TTextOps& aTextOp)
	{
	const TText8* ptr=aDes.Ptr();
	TInt size=aDes.Length();
	TInt r=Compare8L(aBuf,ptr,Min(size,aSize),aTextOp);
	return r ? r : aSize-size;
	}

TInt Comp::Compare16L(MStreamBuf& aBufL,MStreamBuf& aBufR,TInt aSize,const TTextOps& aTextOp)
	{
	aSize >>= 1;
	const TInt KCompareBufLength=KCompareBufSize/sizeof(TText16);
	RReadStream left(&aBufL);
	RReadStream right(&aBufR);
	TText16 bufL[KCompareBufLength];
	TText16 bufR[KCompareBufLength];
	TInt read=KCompareBufLength;
	do
		{
		if (aSize<=read)
			read=aSize;
		left.ReadL(bufL,read);
		right.ReadL(bufR,read);
		TInt r=aTextOp.Order(bufL,read,bufR,read);
		if (r!=0)
			return r;
		aSize-=KCompareBufLength;
		} while (aSize>0);
	return 0;
	}

TInt Comp::Compare16L(MStreamBuf& aBuf,const TText16* aPtr,TInt aSize,const TTextOps& aTextOp)
	{
	aSize >>= 1;
	const TInt KCompareBufLength=KCompareBufSize/sizeof(TText16);
	RReadStream strm(&aBuf);
	TText16 buf[KCompareBufLength];
	TInt read=KCompareBufLength;
	do
		{
		if (aSize<read)
			read=aSize;
		strm.ReadL(buf,read);
		TInt r=aTextOp.Order(buf,read,aPtr,read);
		if (r!=0)
			return r;
		aPtr+=KCompareBufLength;
		aSize-=KCompareBufLength;
		} while (aSize>0);
	return 0;
	}

static TInt DoCompare16L(MStreamBuf& aBuf,const TText16* aPtr,TInt aLen,const TTextOps& aTextOp)
	{
	const TInt KCompareBufLength=KCompareBufSize/sizeof(TText16);
	RReadStream strm(&aBuf);
	TText16 buf[KCompareBufLength];
	TInt read=KCompareBufLength;
	do
		{
		if (aLen<read)
			read=aLen;
		strm.ReadL(buf,read);
		TInt r=aTextOp.Compare(buf,read,aPtr,read);
		if (r!=0)
			return r;
		aPtr+=KCompareBufLength;
		aLen-=KCompareBufLength;
		} while (aLen>0);
	return 0;
	}
	
TInt Comp::Compare16L(MStreamBuf& aBuf,TInt aSize,const TDesC16& aDes,const TTextOps& aTextOp)
	{
	const TText16* ptr=aDes.Ptr();
	TInt size=aDes.Length()<<1;
	TInt r=::DoCompare16L(aBuf,ptr,Min(size,aSize)>>1,aTextOp);
	return r ? r : aSize-size;
	}

