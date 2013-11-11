/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:       
*
*/


#include "PictographObserver.h"
#include <e32std.h>
#include <charconv.h>
#include <convdata.h>
#include <convutils.h>
#include "jisx0201.h"
#include "jisx0208.h"
#include "jisx0212.h"
#include "jisbase.h"
#include "featmgr/featmgr.h"

const TUint KControlCharacterEscape=0x1b;
const TUint KControlCharacterShiftOut=0x0e;
const TUint KControlCharacterShiftIn=0x0f;
const TUint KBitsForNonStandardStates=0x03;

_LIT8(KLit8EscapeSequenceForJisRoman, "\x1b\x28\x4a");
_LIT8(KLit8EscapeSequenceForJisRomanIncorrect, "\x1b\x28\x48");
_LIT8(KLit8EscapeSequenceForAscii, "\x1b\x28\x42");
_LIT8(KLit8EscapeSequenceForHalfWidthKatakana, "\x1b\x28\x49");
_LIT8(KLit8EscapeSequenceForJisC6226_1978, "\x1b\x24\x40");
_LIT8(KLit8EscapeSequenceForJisX0208_1983, "\x1b\x24\x42");
_LIT8(KLit8EscapeSequenceForJisX0208_199x, "\x1b\x26\x40\x1b\x24\x42");
_LIT8(KLit8EscapeSequenceForJisX0212_1990, "\x1b\x24\x28\x44");

typedef TInt (*FChangeState)(TInt aState);
typedef TInt (*FAppendConvertToUnicode)(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, TDes16& aUnicode, const TDesC8& aForeign, TInt& aState, TInt& aNumberOfUnconvertibleCharacters, TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter, const TArray<CnvUtilities::SState>& aArrayOfStates, TUint& aOutputConversionFlags, TUint aInputConversionFlags);

enum TNonStandardState // each of these values must fit into KBitsForNonStandardStates and each must also be non-zero
	{
	ENonStandardStateJis7=1,
	ENonStandardStateJis8
	};


LOCAL_D const SCnvConversionData::SVariableByteData::SRange halfWidthKatakana7VariableByteDataRange=
	{
	0x00,
	0xff,
	0,
	0
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange halfWidthKatakana7ToUnicodeDataRange=
	{
	0x21,
	0x5f,
	SCnvConversionData::SOneDirectionData::SRange::EOffset,
	0,
	0,
		{
		STATIC_CAST(TUint, 65344),
		0
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange unicodeToHalfWidthKatakana7DataRange=
	{
	0xff61,
	0xff9f,
	SCnvConversionData::SOneDirectionData::SRange::EOffset,
	1,
	0,
		{
		STATIC_CAST(TUint, -65344),
		0
		}
	};

LOCAL_D const SCnvConversionData halfWidthKatakana7ConversionData=
	{
	SCnvConversionData::EUnspecified,
		{
		1,
		&halfWidthKatakana7VariableByteDataRange
		},
		{
		1,
		&halfWidthKatakana7ToUnicodeDataRange
		},
		{
		1,
		&unicodeToHalfWidthKatakana7DataRange
		}
	};

#if defined(_DEBUG)

_LIT(KLitPanicText, "JISBASE_SHARED");

enum TPanic
	{
	EPanicNotAppending1=1,
	EPanicNotAppending2,
	EPanicNotAppending3,
	EPanicBadNonStandardState,
	EPanicBadPointers1,
	EPanicBadPointers2,
	EPanicBadPointers3,
	EPanicBadPointers4,
	EPanicBadFunctionPointer
	};

LOCAL_C void Panic(TPanic aPanic)
	{
	User::Panic(KLitPanicText, aPanic);
	}

#endif

TInt CnvJisBase::ChangeToNonStandardStateJis7(TInt aState)
	{
	return (aState&~KBitsForNonStandardStates)|ENonStandardStateJis7;
	}

TInt CnvJisBase::ChangeToNonStandardStateJis8(TInt aState)
	{
	return (aState&~KBitsForNonStandardStates)|ENonStandardStateJis8;
	}

TInt CnvJisBase::ChangeToStandardState(TInt)
	{
	return CCnvCharacterSetConverter::KStateDefault; // I actually thought that the correct behaviour for this would be to return "aState&~KBitsForNonStandardStates", but I asked Ken Lunde about it in an email and he said that after a run of JIS7 or JIS8, the bytes should always be interpreted as JIS-Roman
	}

TInt CnvJisBase::AppendConvertToUnicodeFromModalForeign(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, TDes16& aUnicode, const TDesC8& aModalForeign, TInt& aState, TInt& aNumberOfUnconvertibleCharacters, TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter, const TArray<CnvUtilities::SState>& aArrayOfStates, TUint& aOutputConversionFlags, TUint aInputConversionFlags)
	{
	__ASSERT_DEBUG(aInputConversionFlags&CCnvCharacterSetConverter::EInputConversionFlagAppend, Panic(EPanicNotAppending1));
	return CnvUtilities::ConvertToUnicodeFromModalForeign(aDefaultEndiannessOfForeignCharacters, aUnicode, aModalForeign, aState, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter, aArrayOfStates, aOutputConversionFlags, aInputConversionFlags);
	}

TInt CnvJisBase::AppendConvertToUnicodeFromJis7(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, TDes16& aUnicode, const TDesC8& aJis7, TInt&, TInt& aNumberOfUnconvertibleCharacters, TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter, const TArray<CnvUtilities::SState>&, TUint& aOutputConversionFlags, TUint aInputConversionFlags)
	{
	__ASSERT_DEBUG(aInputConversionFlags&CCnvCharacterSetConverter::EInputConversionFlagAppend, Panic(EPanicNotAppending2));
	return CCnvCharacterSetConverter::DoConvertToUnicode(halfWidthKatakana7ConversionData, aDefaultEndiannessOfForeignCharacters, aUnicode, aJis7, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter, aOutputConversionFlags, aInputConversionFlags);
	}

TInt CnvJisBase::AppendConvertToUnicodeFromJis8(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, TDes16& aUnicode, const TDesC8& aJis8, TInt&, TInt& aNumberOfUnconvertibleCharacters, TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter, const TArray<CnvUtilities::SState>&, TUint& aOutputConversionFlags, TUint aInputConversionFlags)
	{
	__ASSERT_DEBUG(aInputConversionFlags&CCnvCharacterSetConverter::EInputConversionFlagAppend, Panic(EPanicNotAppending3));
	return CCnvCharacterSetConverter::DoConvertToUnicode(CnvHalfWidthKatakana8::ConversionData(), aDefaultEndiannessOfForeignCharacters, aUnicode, aJis8, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter, aOutputConversionFlags, aInputConversionFlags);
	}

EXPORT_C TInt CnvJisBase::ConvertToUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, TDes16& aUnicode, const TDesC8& aForeign, TInt& aState, TInt& aNumberOfUnconvertibleCharacters, TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
	{
    TInt ret =KErrNone;
    TBool pictographsSupported = FeatureManager::FeatureSupported(KFeatureIdJapanesePicto);
    RArray<CnvUtilities::SState> states;
    if ( pictographsSupported )
        {        

        CnvUtilities::SState state;
        state.iEscapeSequence=&KLit8EscapeSequenceForJisRoman; // Jis-Roman is the default state, so it must come first in the array
        state.iConversionData=&CnvJisRoman::ConversionData();
        ret |= states.Append(state);
        state.iEscapeSequence=&KLit8EscapeSequenceForJisRomanIncorrect;
        state.iConversionData=&CnvJisRoman::ConversionData();
        ret |= states.Append(state);
        state.iEscapeSequence=&KLit8EscapeSequenceForAscii;
        state.iConversionData=&CCnvCharacterSetConverter::AsciiConversionData();
        ret |= states.Append(state);
    
        SetStatesForPictograph(states);

        state.iEscapeSequence=&KLit8EscapeSequenceForHalfWidthKatakana;
        state.iConversionData=&halfWidthKatakana7ConversionData;
        ret |= states.Append(state);
        state.iEscapeSequence=&KLit8EscapeSequenceForJisC6226_1978;
        state.iConversionData=&CnvJisX0208::ConversionData();
        ret |= states.Append(state);
        state.iEscapeSequence=&KLit8EscapeSequenceForJisX0208_1983;
        state.iConversionData=&CnvJisX0208::ConversionData();
        ret |= states.Append(state);
        state.iEscapeSequence=&KLit8EscapeSequenceForJisX0208_199x;
        state.iConversionData=&CnvJisX0208::ConversionData();
        ret |= states.Append(state);
        state.iEscapeSequence=&KLit8EscapeSequenceForJisX0212_1990;
        state.iConversionData=&CnvJisX0212::ConversionData();
        ret |= states.Append(state);
        }
    else
        {            
        CnvUtilities::SState state;
    	state.iEscapeSequence=&KLit8EscapeSequenceForJisRoman; // Jis-Roman is the default state, so it must come first in the array
    	state.iConversionData=&CnvJisRoman::ConversionData();
    	ret |= states.Append(state);
        state.iEscapeSequence=&KLit8EscapeSequenceForJisRomanIncorrect;
    	state.iConversionData=&CnvJisRoman::ConversionData();	
    	ret |= states.Append(state);
    	state.iEscapeSequence=&KLit8EscapeSequenceForAscii;
    	state.iConversionData=&CCnvCharacterSetConverter::AsciiConversionData();
    	ret |= states.Append(state);
    	state.iEscapeSequence=&KLit8EscapeSequenceForHalfWidthKatakana;
    	state.iConversionData=&halfWidthKatakana7ConversionData;
    	ret |= states.Append(state);
    	state.iEscapeSequence=&KLit8EscapeSequenceForJisC6226_1978;
    	state.iConversionData=&CnvJisX0208::ConversionData();
    	ret |= states.Append(state);
    	state.iEscapeSequence=&KLit8EscapeSequenceForJisX0208_1983;
    	state.iConversionData=&CnvJisX0208::ConversionData();
    	ret |= states.Append(state);
    	state.iEscapeSequence=&KLit8EscapeSequenceForJisX0208_199x;
    	state.iConversionData=&CnvJisX0208::ConversionData();
    	ret |= states.Append(state);
    	state.iEscapeSequence=&KLit8EscapeSequenceForJisX0212_1990;
    	state.iConversionData=&CnvJisX0212::ConversionData();
    	ret |= states.Append(state);
        }
    __ASSERT_DEBUG(!ret, User::Panic(_L("RArray append failure"), ret));
    
	const TArray<CnvUtilities::SState> arrayOfStates(states.Array());
	aUnicode.SetLength(0);
	const TUint8* const pointerToFirstByte=aForeign.Ptr();
	const TUint8* pointerToCurrentByte=pointerToFirstByte;
	const TUint8* pointerToStartOfNextRunToConvert=pointerToFirstByte;
	const TUint8* const pointerToLastByte=pointerToFirstByte+(aForeign.Length()-1);
	TUint outputConversionFlags=0;
	TUint inputConversionFlags=CCnvCharacterSetConverter::EInputConversionFlagAppend;
	FOREVER
		{
		FChangeState changeState=NULL;
		FAppendConvertToUnicode appendConvertToUnicode=NULL;
		TBool skipThisByte=EFalse;
		const TUint currentByte=*pointerToCurrentByte;
		switch (aState&KBitsForNonStandardStates)
			{
		case 0:
			if (currentByte==KControlCharacterShiftOut)
				{
				changeState=ChangeToNonStandardStateJis7;
				skipThisByte=ETrue;
				}
            else if (pictographsSupported && (currentByte==KControlCharacterShiftIn))
                {
                changeState=ChangeToStandardState;
                skipThisByte=ETrue;
                }
			else if (currentByte&0x80)
				{
				changeState=ChangeToNonStandardStateJis8;
				}
			appendConvertToUnicode=AppendConvertToUnicodeFromModalForeign;
			break;
		case ENonStandardStateJis7:
			if (currentByte==KControlCharacterEscape)
				{
				changeState=ChangeToStandardState; // it doesn't matter what function changeState is set to (as its return value won't actually be used), as long as changeState!=NULL so that the test below (after the end of this switch) passes
				}
			else if (currentByte==KControlCharacterShiftIn)
				{
				changeState=ChangeToStandardState;
				skipThisByte=ETrue;
				}
			else if (currentByte&0x80)
				{
				changeState=ChangeToNonStandardStateJis8;
				}
			appendConvertToUnicode=AppendConvertToUnicodeFromJis7;
			break;
		case ENonStandardStateJis8:
			if (currentByte==KControlCharacterEscape)
				{
				changeState=ChangeToStandardState; // it doesn't matter what function changeState is set to (as its return value won't actually be used), as long as changeState!=NULL so that the test below (after the end of this switch) passes
				}
			else if (currentByte==KControlCharacterShiftOut)
				{
				changeState=ChangeToNonStandardStateJis7;
				skipThisByte=ETrue;
				}
			else if ((currentByte&0x80)==0)
				{
				changeState=ChangeToStandardState;
				}
			appendConvertToUnicode=AppendConvertToUnicodeFromJis8;
			break;
#if defined(_DEBUG)
		default:
			Panic(EPanicBadNonStandardState);
			break;
#endif
			}
		__ASSERT_DEBUG(pointerToCurrentByte<=pointerToLastByte, Panic(EPanicBadPointers1));
		if ((pointerToCurrentByte>=pointerToLastByte) || (changeState!=NULL))
			{
			TBool lastIteration=EFalse;
			__ASSERT_DEBUG(pointerToCurrentByte>=pointerToStartOfNextRunToConvert, Panic(EPanicBadPointers2));
			if (changeState==NULL)
				{
				++pointerToCurrentByte; // this may make pointerToCurrentByte greater than pointerToLastByte
				lastIteration=ETrue;
				}
			if (pointerToCurrentByte>pointerToStartOfNextRunToConvert)
				{
				TPtrC8 runToConvert(pointerToStartOfNextRunToConvert, pointerToCurrentByte-pointerToStartOfNextRunToConvert);
				TInt numberOfUnconvertibleCharacters;
				TInt indexOfFirstByteOfFirstUnconvertibleCharacter;
				__ASSERT_DEBUG(appendConvertToUnicode!=NULL, Panic(EPanicBadFunctionPointer));
				const TInt returnValue=(*appendConvertToUnicode)(aDefaultEndiannessOfForeignCharacters, aUnicode, runToConvert, aState, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter, arrayOfStates, outputConversionFlags, inputConversionFlags);
				if (returnValue<0)
					{
                    states.Close();
					return returnValue; // this is an error-code
					}
				if (numberOfUnconvertibleCharacters>0)
					{
					if (aNumberOfUnconvertibleCharacters==0)
						{
						aIndexOfFirstByteOfFirstUnconvertibleCharacter=(pointerToStartOfNextRunToConvert-pointerToFirstByte)+indexOfFirstByteOfFirstUnconvertibleCharacter;
						}
					aNumberOfUnconvertibleCharacters+=numberOfUnconvertibleCharacters;
					}
				if (returnValue>0)
					{
					pointerToCurrentByte-=returnValue; // pointerToStartOfNextRunToConvert (which also needs adjusting in the same way) gets set below
					lastIteration=ETrue;
					changeState=NULL;
					skipThisByte=EFalse;
					}
				__ASSERT_DEBUG(pointerToCurrentByte>=pointerToFirstByte, Panic(EPanicBadPointers3));
				if (pointerToCurrentByte>pointerToFirstByte)
					{
					inputConversionFlags|=CCnvCharacterSetConverter::EInputConversionFlagAllowTruncatedInputNotEvenPartlyConsumable;
					}
				}
			if (changeState!=NULL)
				{
				aState=(*changeState)(aState);
				}
			if (skipThisByte)
				{
				if (pointerToCurrentByte==pointerToLastByte) // pointerToCurrentByte may already be greater than pointerToLastByte, in which case lastIteration will already be ETrue
					{
					lastIteration=ETrue;
					}
				++pointerToCurrentByte;
				}
			pointerToStartOfNextRunToConvert=pointerToCurrentByte;
			if (lastIteration) // check this first as pointerToCurrentByte may be greater than pointerToLastByte (but it will only be if lastIteration is EFalse)
				{
				break;
				}
			__ASSERT_DEBUG(pointerToCurrentByte<=pointerToLastByte, Panic(EPanicBadPointers4));
			if (pointerToCurrentByte>=pointerToLastByte)
				{
				break;
				}
			}
		++pointerToCurrentByte;
		}

    states.Close();
	// no checking with outputConversionFlags need to be done here
	return pointerToLastByte-(pointerToCurrentByte-1);
	}

EXPORT_C const SCnvConversionData& CnvJisBase::HalfWidthKatakana7ConversionData()
	{
	return halfWidthKatakana7ConversionData;
	}

EXPORT_C void CnvJisBase::IsCharacterJISBased(TInt& aConfidenceLevel, const TDesC8& aSample) 
	{
	// JIS is modal... so start off with a confidence of 0 and to begin with look 
	// for JIS escape sequences....Escape sequences defined above in the KLITs
	// For each escape sequence, increase the confidenceLevel ..... 
	aConfidenceLevel = 55;
	TInt jisRomanResult = 0;
	TInt asciiResult = 0;
	TInt jisX0208Result = 0;
	TInt jisC6226Result = 0;
	TInt jixX0212Result = 0;
	TInt hwKanaResult = 0;

	TInt EscSequences = 0;
	
	TInt sampleLength = aSample.Length();
	for (TInt i = 0; i < sampleLength; ++i)
		{
	
		// JIS is 7 bit encoding
		if((aSample[i]&0x80)!=0x00)
			{
			aConfidenceLevel=0;
			break;
			}
		// JIS supports the following character sets 
		if (i > jisC6226Result)
			{
			jisC6226Result=(aSample.Right(sampleLength-i)).Find(KLit8EscapeSequenceForJisC6226_1978);
			if (jisC6226Result!=KErrNotFound)
				EscSequences += 15; 
			}

		if (i > jisRomanResult)
			{
			jisRomanResult=(aSample.Right(sampleLength-i)).Find(KLit8EscapeSequenceForJisRoman);
			if (jisRomanResult!=KErrNotFound)
				EscSequences += 15; 
			}

		if (i > asciiResult)
			{
			asciiResult=(aSample.Right(sampleLength-i)).Find(KLit8EscapeSequenceForAscii);
			if (asciiResult!=KErrNotFound)
				EscSequences += 15; 
			}

		if (i > jisX0208Result)
			{
			jisX0208Result=(aSample.Right(sampleLength-i)).Find(KLit8EscapeSequenceForJisX0208_1983);
			if (jisX0208Result!=KErrNotFound)
				EscSequences += 15; 
			}

		if (i > jixX0212Result)
			{
			jixX0212Result=(aSample.Right(sampleLength-i)).Find(KLit8EscapeSequenceForJisX0212_1990);
			if (jixX0212Result!=KErrNotFound)
				EscSequences += 15; 
			}

		if (i > hwKanaResult)
			{
			hwKanaResult=(aSample.Right(sampleLength-i)).Find(KLit8EscapeSequenceForHalfWidthKatakana);
			if (hwKanaResult!=KErrNotFound)
				EscSequences += 15; 
			}
		}

	aConfidenceLevel = 0 < sampleLength?
		aConfidenceLevel + ((EscSequences*100)/sampleLength) : 90;
	aConfidenceLevel=(aConfidenceLevel >100)?100:aConfidenceLevel;
	}
