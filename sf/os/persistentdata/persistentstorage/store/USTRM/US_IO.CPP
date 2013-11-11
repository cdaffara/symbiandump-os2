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

#include "US_STD.H"

TInt TPtrInput::PushL(const TAny* aPtr,TInt aMaxLength)
//
// Accept the data, copying it to the buffer pointed to.
//
	{
	__ASSERT_DEBUG(aMaxLength>=0,Panic(EStreamPushLengthNegative));
	__ASSERT_DEBUG(aMaxLength>0,Panic(EStreamPushNoTransfer));
	iPtr=Mem::Copy(iPtr,aPtr,aMaxLength);
	return aMaxLength;
	}

TStreamTransfer TPtrInput::ReadFromL(MStreamBuf&,TStreamTransfer aTransfer)
//
// This input is passive.
//
	{
	__ASSERT_DEBUG(aTransfer>0,Panic(EStreamReadNoTransfer));
	return aTransfer;
	}

TInt TPtrOutput::PullL(TAny* aPtr,TInt aMaxLength)
//
// Produce data from the buffer pointed to.
//
	{
	__ASSERT_DEBUG(aMaxLength>=0,Panic(EStreamPullLengthNegative));
	__ASSERT_DEBUG(aMaxLength>0,Panic(EStreamPullNoTransfer));
	Mem::Copy(aPtr,iPtr,aMaxLength);
	iPtr+=aMaxLength;
	return aMaxLength;
	}

TStreamTransfer TPtrOutput::WriteToL(MStreamBuf&,TStreamTransfer aTransfer)
//
// This output is passive.
//
	{
	__ASSERT_DEBUG(aTransfer>0,Panic(EStreamWriteNoTransfer));
	return aTransfer;
	}

TInt TNullInput::PushL(const TAny*,TInt aMaxLength)
//
// Accept and discard the data.
//
	{
	__ASSERT_DEBUG(aMaxLength>=0,Panic(EStreamPushLengthNegative));
	__ASSERT_DEBUG(aMaxLength>0,Panic(EStreamPushNoTransfer));
	return aMaxLength;
	}

TStreamTransfer TNullInput::ReadFromL(MStreamBuf& aSource,TStreamTransfer aTransfer)
//
// Read and discard data from aSource using a temporary buffer.
//
	{
	__ASSERT_DEBUG(aTransfer>0,Panic(EStreamReadNoTransfer));
	do
		{
		TUint8 buf[KDefaultIoBufSize];
		TInt len=aSource.ReadL(buf,aTransfer[sizeof(buf)]);
		__ASSERT_DEBUG(len>=0&&len<=aTransfer[sizeof(buf)],Panic(EStreamReadInBreach));
		if (len==0)
			break;
//
		aTransfer-=len;
		} while (aTransfer>0);
	return aTransfer;
	}

TInt TSourceOutput::PullL(TAny* aPtr,TInt aMaxLength)
//
// Pull up to aMaxLength bytes of data from this output's source.
//
	{
	__ASSERT_DEBUG(aMaxLength>=0,Panic(EStreamPullLengthNegative));
	__ASSERT_DEBUG(aMaxLength>0,Panic(EStreamPullNoTransfer));
	__ASSERT_DEBUG(iSrc!=NULL,User::Invariant());
//
	TPtrInput input(aPtr);
	TInt len=iSrc->ReadL(input,aMaxLength);
	__ASSERT_DEBUG(len>=0&&len<=aMaxLength,Panic(EStreamReadInBreach));
	return len;
	}

TStreamTransfer TSourceOutput::WriteToL(MStreamBuf& aSink,TStreamTransfer aTransfer)
//
// Write data from this output's source to aSink using a temporary buffer.
//
	{
	__ASSERT_DEBUG(aTransfer>0,Panic(EStreamWriteNoTransfer));
	__ASSERT_DEBUG(iSrc!=NULL,User::Invariant());
	do
		{
		TUint8 buf[KDefaultIoBufSize];
		TInt len=iSrc->ReadL(buf,aTransfer[sizeof(buf)]);
		__ASSERT_DEBUG(len>=0&&len<=aTransfer[sizeof(buf)],Panic(EStreamReadInBreach));
		if (len==0)
			break;
//
		aSink.WriteL(buf,len);
		aTransfer-=len;
		} while (aTransfer>0);
	return aTransfer;
	}

TInt TFilterInput::PushL(const TAny* aPtr,TInt aMaxLength)
//
// Put up to aMaxLength bytes of data through the filter.
//
	{
	__ASSERT_DEBUG(aMaxLength>=0,Panic(EStreamPushLengthNegative));
	__ASSERT_DEBUG(aMaxLength>0,Panic(EStreamPushNoTransfer));
	__ASSERT_DEBUG(!Eof(),Panic(EStreamReadInBreach));
	__ASSERT_DEBUG(iFltr!=NULL&&iLeft>=0,User::Invariant());
	if (Done())
		return 0;
//
	const TUint8* from=(TUint8*)aPtr;
	TInt len=iFltr->FilterL(iPtr,iLeft,from,from+aMaxLength);
	__ASSERT_DEBUG(len>=0&&len<=iLeft,Panic(EStreamFilterInBreach));
	__ASSERT_DEBUG(from>=(TUint8*)aPtr&&from<=(TUint8*)aPtr+aMaxLength,Panic(EStreamFilterInBreach));
	__ASSERT_DEBUG(len==iLeft||from==(TUint8*)aPtr+aMaxLength,Panic(EStreamFilterInBreach));
	iPtr+=len;
	iLeft-=len;
	return from-(TUint8*)aPtr;
	}

TStreamTransfer TFilterInput::ReadFromL(MStreamBuf& aSource,TStreamTransfer aTransfer)
//
// Put data read from aSource through the filter using a temporary buffer.
//
	{
	__ASSERT_DEBUG(aTransfer>0,Panic(EStreamReadNoTransfer));
	__ASSERT_DEBUG(iFltr!=NULL&&iLeft>=0,User::Invariant());
	if (Done())
		return aTransfer;
//
	TUint8 buf[KFilterIoBufSize];
	TInt cap=aTransfer[Min(iFltr->Capacity(iLeft),sizeof(buf))];
	const TUint8* end=buf+(cap==0?0:aSource.ReadL(buf,cap));
	__ASSERT_DEBUG(end>=buf&&end<=buf+cap&&(!Eof()||end==buf),Panic(EStreamReadInBreach));
	const TUint8* from=buf;
	TInt len=iFltr->FilterL(iPtr,iLeft,from,end);
	__ASSERT_DEBUG(len>=0&&len<=iLeft,Panic(EStreamFilterInBreach));
	__ASSERT_DEBUG(from==end,Panic(EStreamFilterInBreach));
	if (end==buf && len==0)	// no input && no output, => end of stream
		iPtr=NULL;
	else
		iPtr+=len;
	iLeft-=len;
	return aTransfer-(from-buf);
	}

TInt TFilterOutput::PullL(TAny* aPtr,TInt aMaxLength)
//
// Pull up to aMaxLength bytes of data through the filter.
//
	{
	__ASSERT_DEBUG(aMaxLength>=0,Panic(EStreamPullLengthNegative));
	__ASSERT_DEBUG(aMaxLength>0,Panic(EStreamPullNoTransfer));
	__ASSERT_DEBUG(iFltr!=NULL&&iFrom!=NULL&&iFrom<=iEnd,User::Invariant());
	if (Done())
		return 0;
//
	__DEBUG(const TUint8* from=iFrom);
	TInt len=iFltr->FilterL(aPtr,aMaxLength,iFrom,iEnd);
	__ASSERT_DEBUG(len>=0&&len<=aMaxLength,Panic(EStreamFilterInBreach));
	__ASSERT_DEBUG(iFrom>=from&&iFrom<=iEnd,Panic(EStreamFilterInBreach));
	__ASSERT_DEBUG(len==aMaxLength||iFrom==iEnd,Panic(EStreamFilterInBreach));
	return len;
	}

TStreamTransfer TFilterOutput::WriteToL(MStreamBuf& aSink,TStreamTransfer aTransfer)
//
// Write data put through the filter to aSink using a temporary buffer.
//
	{
	__ASSERT_DEBUG(aTransfer>0,Panic(EStreamWriteNoTransfer));
	__ASSERT_DEBUG(iFltr!=NULL&&iFrom!=NULL&&iFrom<=iEnd,User::Invariant());
	if (Done())
		return aTransfer;
//
	TUint8 buf[KFilterIoBufSize];
	__DEBUG(const TUint8* from=iFrom);
	TInt len=iFltr->FilterL(buf,aTransfer[sizeof(buf)],iFrom,iEnd);
	__ASSERT_DEBUG(len>=0&&len<=aTransfer[sizeof(buf)],Panic(EStreamFilterInBreach));
	__ASSERT_DEBUG(iFrom>=from&&iFrom<=iEnd,Panic(EStreamFilterInBreach));
	__ASSERT_DEBUG(len==aTransfer[sizeof(buf)]||iFrom==iEnd,Panic(EStreamFilterInBreach));
	if (len>0)
		aSink.WriteL(buf,len);
	return aTransfer-len;
	}

TDelimitedInput8::TDelimitedInput8(TUint8* aPtr,TInt aLength,TChar aDelim)
	: iPtr(aPtr),iLeft(aLength),iDelim(aDelim)
	{}

TInt TDelimitedInput8::PushL(const TAny* aPtr,TInt aMaxLength)
//
// Push 8-bit text into this input buffer up to the first occurrence of the delimiter.
//
	{
	TInt len=Min(aMaxLength,iLeft);
	TInt d=TPtrC8((TUint8*)aPtr,len).Locate(iDelim)+1;
	if (d<=0)
		{
		d=len;
		iLeft-=len;
		}
	else
		iLeft=0;
	iPtr=(TUint8*)Mem::Copy(iPtr,aPtr,d);
	return d;
	}

TStreamTransfer TDelimitedInput8::ReadFromL(MStreamBuf& aSource,TStreamTransfer aTransfer)
//
// Read a single 8-bit text character from aSource, testing it against the delimiter.
//
	{
	if (Done())
		return aTransfer;
//
	RReadStream stream(&aSource);
	TUint8 c=stream.ReadUint8L();
	*iPtr++=c;
	if (c==(TUint8)iDelim)
		iLeft=0;
	else
		--iLeft;
	return aTransfer-sizeof(c);
	}

TDelimitedInput16::TDelimitedInput16(TUint16* aPtr,TInt aLength,TChar aDelim)
	: iPtr(aPtr),iLeft(aLength),iDelim(aDelim)
	{}

TInt TDelimitedInput16::PushL(const TAny* aPtr,TInt aMaxLength)
//
// Push 16-bit text into this input buffer up to the first occurrence of the delimiter.
// We cannot use TPtrC16 here as the data may not be half-word aligned
//
	{
	TInt len=Min(aMaxLength>>1,iLeft);
	iLeft-=len;
	TInt lbyte=iDelim&0xffu;
	TInt hbyte=iDelim>>8;
	const TUint8* p=static_cast<const TUint8*>(aPtr);
	TInt d=0;
	while (d<len)
		{
		if (p[d<<1]==lbyte && p[(d<<1)+1]==hbyte) // platform dependency
			{
			++d;		// include the delimiter
			iLeft=0;	// found a match
			break;
			}
		++d;
		}

	iPtr=(TUint16*)Mem::Copy(iPtr,aPtr,d<<1); // platform dependency
	return d<<1;
	}

TStreamTransfer TDelimitedInput16::ReadFromL(MStreamBuf& aSource,TStreamTransfer aTransfer)
//
// Read a single 16-bit text character from aSource, testing it against the delimiter.
//
	{
	if (Done())
		return aTransfer;
//
	RReadStream stream(&aSource);
	TUint16 c=stream.ReadUint16L();
//
	*iPtr++=c;
	if (c==(TUint16)iDelim)
		iLeft=0;
	else
		--iLeft;
	return aTransfer-sizeof(c);
	}

