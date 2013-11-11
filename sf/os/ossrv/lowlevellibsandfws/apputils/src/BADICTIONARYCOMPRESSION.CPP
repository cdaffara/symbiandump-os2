// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <baflpan.h>
#include "BADICTIONARYCOMPRESSION.H"

RDictionaryCompressionBitStream::RDictionaryCompressionBitStream() :
	iNumberOfBitsUsedForDictionaryTokens(0),
	iOffsetToFirstBit(-1),
	iOffsetToCurrentBit(-1),
	iOffsetOnePastLastBit(-1),
	iOwnsBitBuffer(EFalse),
	iBuffer(NULL),
	iAssertObj()
	{
	}

void RDictionaryCompressionBitStream::OpenL(
				TInt aNumberOfBitsUsedForDictionaryTokens,
				TInt aOffsetToFirstBit,
				TInt aOffsetOnePastLastBit,
				TBool aTransferringOwnershipOfBuffer,
				TUint8* aBuffer,
				const TBaAssert& aAssertObj)
	{
	iNumberOfBitsUsedForDictionaryTokens = aNumberOfBitsUsedForDictionaryTokens;
	iOffsetToFirstBit = aOffsetToFirstBit;
	iOffsetToCurrentBit = aOffsetToFirstBit;
	iOffsetOnePastLastBit = aOffsetOnePastLastBit;
	iOwnsBitBuffer = aTransferringOwnershipOfBuffer;
	iBuffer = aBuffer;
	iAssertObj = aAssertObj;

	iAssertObj.AssertDebL(aBuffer!=NULL,EBafPanicNullPointer);
	iAssertObj.AssertDebL(aOffsetToFirstBit>=0,EBafPanicNegativeOffsetToFirstBit1);
	iAssertObj.AssertDebL(aOffsetToFirstBit<=aOffsetOnePastLastBit,EBafPanicNegativeLengthOfBitBuffer);
	}

void RDictionaryCompressionBitStream::Close()
	{
	if (iOwnsBitBuffer)
		{
		iOwnsBitBuffer=EFalse;
		delete [] iBuffer;
		}
	iBuffer=NULL;
	}

TBool RDictionaryCompressionBitStream::EndOfStreamL() const
	{
	__ASSERT_DEBUG(iBuffer!=NULL,Panic(EBafPanicNotConstructed1));
	iAssertObj.AssertDebL(iOffsetToFirstBit>=0,EBafPanicNegativeOffsetToFirstBit2);
	iAssertObj.AssertDebL(iOffsetToCurrentBit>=iOffsetToFirstBit,EBafPanicBadCurrentBitPosition1);
	iAssertObj.AssertDebL(iOffsetToCurrentBit<=iOffsetOnePastLastBit,EBafPanicBadCurrentBitPosition2);
	return iOffsetToCurrentBit>=iOffsetOnePastLastBit;
	}

TInt RDictionaryCompressionBitStream::IndexOfDictionaryEntryL()
	{
	// increments the current bit-position if it returns a value >=0; returns KErrNotFound if the next thing in the stream is plain data rather than the index of a dictionary entry
	__ASSERT_DEBUG(iBuffer!=NULL,Panic(EBafPanicNotConstructed2));
	iAssertObj.AssertDebL(!EndOfStreamL(),EBafPanicEndOfStream1);
	if (!CurrentBitIsOn())
		{
		++iOffsetToCurrentBit;
		return ReadIntegerL(iNumberOfBitsUsedForDictionaryTokens);
		}
	return KErrNotFound;
	}

void RDictionaryCompressionBitStream::ReadL(TDes8& aBufferToAppendTo,TBool aCalypsoFileFormat)
	{
	// can only be called if IndexOfDictionaryEntry returned a negative value
	__ASSERT_DEBUG(iBuffer!=NULL,Panic(EBafPanicNotConstructed3));
	iAssertObj.AssertDebL(!EndOfStreamL(),EBafPanicEndOfStream2);
	TInt numberOfConsecutivePrefixBits=0;
	TInt i;
	for (i=0; i<4; ++i)
		{
		const TBool currentBitIsOn=CurrentBitIsOn();
		++iOffsetToCurrentBit; // increment this regardless whether the current bit is on
		if (!currentBitIsOn)
			{
			break;
			}
		++numberOfConsecutivePrefixBits;
		}
	iAssertObj.AssertDebL(numberOfConsecutivePrefixBits>0,EBafPanicBadNumberOfConsecutivePrefixBits1);
	iAssertObj.AssertDebL(numberOfConsecutivePrefixBits<=4,EBafPanicBadNumberOfConsecutivePrefixBits2);
	TInt numberOfBytesToRead;
	if (numberOfConsecutivePrefixBits==3)
		{
		numberOfBytesToRead=3+ReadIntegerL(3);
		}
	else if (numberOfConsecutivePrefixBits==4)
		{
		numberOfBytesToRead=ReadIntegerL(8);
		if (!aCalypsoFileFormat)
			{
			numberOfBytesToRead+=3+(1<<3);
			}
		}
	else
		{
		numberOfBytesToRead=numberOfConsecutivePrefixBits;
		}
	const TInt numberOfBitsOffByteBoundary=iOffsetToCurrentBit%8;
	const TUint8* currentByte=iBuffer+(iOffsetToCurrentBit/8);
	iAssertObj.AssertDebL(
		(numberOfBytesToRead + aBufferToAppendTo.Length()) <= aBufferToAppendTo.MaxLength(),
		EBafPanicBufLength);
	for (i=0; i<numberOfBytesToRead; ++i, ++currentByte)
		{
		TUint byte=*currentByte;
		iAssertObj.AssertDebL(numberOfBitsOffByteBoundary>=0,EBafPanicBadNumberOfBitsOffByteBoundary1);
		if (numberOfBitsOffByteBoundary>0)
			{
			byte>>=numberOfBitsOffByteBoundary;
			byte|=(*(currentByte+1)<<(8-numberOfBitsOffByteBoundary));
			byte&=0xff;
			}
		aBufferToAppendTo.Append(byte);
		}
	iOffsetToCurrentBit+=numberOfBytesToRead*8;
	iAssertObj.AssertDebL(numberOfBitsOffByteBoundary==iOffsetToCurrentBit%8,EBafPanicBadNumberOfBitsOffByteBoundary2);
	}

TBool RDictionaryCompressionBitStream::CurrentBitIsOn() const
	{
	// does not increment the current bit-position
	__ASSERT_DEBUG(iBuffer!=NULL,Panic(EBafPanicNotConstructed4));
	return iBuffer[iOffsetToCurrentBit/8]&(1<<(iOffsetToCurrentBit%8));
	}

TUint RDictionaryCompressionBitStream::ReadIntegerL(TInt aNumberOfBits)
	{
	// increments the current bit-position
	__ASSERT_DEBUG(iBuffer!=NULL,Panic(EBafPanicNotConstructed5));
	TInt integer=0;
	TInt numberOfBitsLeftToRead=aNumberOfBits;
	FOREVER
		{
		const TInt offsetToFirstBitToReadInCurrentByte=iOffsetToCurrentBit%8;
		const TInt offsetOnePastLastBitToReadInCurrentByte=Min(8,offsetToFirstBitToReadInCurrentByte+numberOfBitsLeftToRead);
		const TInt numberOfBitsReadFromCurrentByte=offsetOnePastLastBitToReadInCurrentByte-offsetToFirstBitToReadInCurrentByte;
		iAssertObj.AssertDebL(numberOfBitsReadFromCurrentByte>0,EBafPanicBadNumberOfBitsReadFromCurrentByte);
		const TUint bitsReadFromCurrentByte=((iBuffer[iOffsetToCurrentBit/8]>>offsetToFirstBitToReadInCurrentByte)&((1<<numberOfBitsReadFromCurrentByte)-1));
		integer|=(bitsReadFromCurrentByte<<(aNumberOfBits-numberOfBitsLeftToRead));
		iOffsetToCurrentBit+=numberOfBitsReadFromCurrentByte;
		numberOfBitsLeftToRead-=numberOfBitsReadFromCurrentByte;
		iAssertObj.AssertDebL(numberOfBitsLeftToRead>=0,EBafPanicBadNumberOfBitsLeftToRead);
		if (numberOfBitsLeftToRead<=0)
			{
			break;
			}
		}
	return integer;
	}

