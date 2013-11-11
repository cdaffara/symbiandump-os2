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

#include "U32STD.H"
#include "U32FRAME.H"

const TInt KShiftCardinality8=1;
const TInt KShiftCardinality16=2;
const TInt KShiftCardinality32=3;
//
const TInt KDefaultIoBufSize=0xc00;
const TInt KFilterIoBufSize=0x100;
//
NONSHARABLE_CLASS(TNullInput) : public MStreamInput
	{
public:
	TInt PushL(const TAny* aPtr,TInt aMaxLength);
	TStreamTransfer ReadFromL(MStreamBuf& aSource,TStreamTransfer aTransfer);
	};
//
NONSHARABLE_CLASS(TSourceOutput) : public MStreamOutput
	{
public:
	inline TSourceOutput(MStreamBuf* aSource);
//
	TInt PullL(TAny* aPtr,TInt aMaxLength);
	TStreamTransfer WriteToL(MStreamBuf& aSink,TStreamTransfer aTransfer);
private:
	MStreamBuf* iSrc;
	};
//
NONSHARABLE_CLASS(TFilterInput) : public MStreamInput
	{
public:
	inline TFilterInput(TStreamFilter& aFilter,TAny* aPtr,TInt aMaxLength);
	inline TBool Done() const;
	inline TBool Eof() const;
	inline TInt Left() const;
//
	TInt PushL(const TAny* aPtr,TInt aMaxLength);
	TStreamTransfer ReadFromL(MStreamBuf& aSource,TStreamTransfer aTransfer);
private:
	TStreamFilter* iFltr;
	TUint8* iPtr;
	TInt iLeft;
	};
NONSHARABLE_CLASS(TFilterOutput) : public MStreamOutput
	{
public:
	inline TFilterOutput(TStreamFilter& aFilter,const TAny* aPtr,TInt aLength);
	inline TBool Done() const;
//
	TInt PullL(TAny* aPtr,TInt aMaxLength);
	TStreamTransfer WriteToL(MStreamBuf& aSink,TStreamTransfer aTransfer);
private:
	TStreamFilter* iFltr;
	const TUint8* iFrom;
	const TUint8* iEnd;
	};
//
NONSHARABLE_CLASS(TDelimitedInput8) : public MStreamInput
	{
public:
	TDelimitedInput8(TUint8* aPtr,TInt aLength,TChar aDelim);
	inline TUint8* Ptr() const;
	inline TInt Done() const;
//
	TInt PushL(const TAny* aPtr,TInt aMaxLength);
	TStreamTransfer ReadFromL(MStreamBuf& aSource,TStreamTransfer aTransfer);
private:
	TUint8* iPtr;
	TInt iLeft;
	TChar iDelim;
	};
NONSHARABLE_CLASS(TDelimitedInput16) : public MStreamInput
	{
public:
	TDelimitedInput16(TUint16* aPtr,TInt aLength,TChar aDelim);
	inline TUint16* Ptr() const;
	inline TInt Done() const;
//
	TInt PushL(const TAny* aPtr,TInt aMaxLength);
	TStreamTransfer ReadFromL(MStreamBuf& aSource,TStreamTransfer aTransfer);
private:
	TUint16* iPtr;
	TInt iLeft;
	TChar iDelim;
	};
//
enum TStreamPanic
	{
	EStreamNotOpen,
	EStreamReadLengthNegative,
	EStreamReadBeyondEnd,
	EStreamReadNoTransfer,
	EStreamReadInBreach,
	EStreamWriteLengthNegative,
	EStreamWriteBeyondEnd,
	EStreamWriteNoTransfer,
	EStreamWriteInBreach,
	EStreamDoesNotUnderstand,
	EStreamCannotRead,
	EStreamCannotWrite,
	EStreamCannotSeek,
	EStreamTransferNegative,
	EStreamPosInvalid,
	EStreamMarkInvalid,
	EStreamLocationInvalid,
	EStreamAreaInvalid,
	EStreamModeInvalid,
	EStreamUnderflowInBreach,
	EStreamOverflowInBreach,
	EStreamFilterInBreach,
	EStreamPushLengthNegative,
	EStreamPushNoTransfer,
	EStreamPushInBreach,
	EStreamPullLengthNegative,
	EStreamPullNoTransfer,
	EStreamPullInBreach,
	EStreamCardinalityOutOfRange,
	EStreamOffsetNegative,
	EStreamExtentNegative,
	EStreamTypeInvalid,
	EStreamCommitted
	};

GLREF_C void Panic(TStreamPanic aPanic);

#include "US_STD.INL"

