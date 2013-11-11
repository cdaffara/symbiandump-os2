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

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

#if !defined(__BADICTIONARYCOMPRESSION_H__)
#define __BADICTIONARYCOMPRESSION_H__

#include "BaAssert.h"

/**
This class implements a stream of bits (least significant bit first) as used by the code reading dictionary-compressed resource-files
@internalComponent
*/
class RDictionaryCompressionBitStream
	{
public:
	RDictionaryCompressionBitStream();
	void OpenL(
		TInt aNumberOfBitsUsedForDictionaryTokens,
		TInt aOffsetToFirstBit,
		TInt aOffsetOnePastLastBit,
		TBool aTransferringOwnershipOfBuffer,
		TUint8* aBuffer,
		const TBaAssert& aAssertObj);
	void Close();
	TBool EndOfStreamL() const;
	TInt IndexOfDictionaryEntryL(); // increments the current bit-position if it returns a value >=0; returns a negative value if the next thing in the stream is plain data rather than the index of a dictionary entry
 	void ReadL(TDes8& aBufferToAppendTo,TBool aCalypsoFileFormat); // can only be called if IndexOfDictionaryEntry returned a negative value
private:
	TBool CurrentBitIsOn() const; // does not increment the current bit-position
	TUint ReadIntegerL(TInt aNumberOfBits); // increments the current bit-position
private:
	TInt iNumberOfBitsUsedForDictionaryTokens;
	TInt iOffsetToFirstBit;
	TInt iOffsetToCurrentBit;
	TInt iOffsetOnePastLastBit;
	TBool iOwnsBitBuffer;
	TUint8* iBuffer;
	TBaAssert iAssertObj;
	};

#endif

