/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     Implements the characterconversion plug-in
*                for ISCII characterset.
*
*/





#include <e32std.h>
#include <charconv.h>
#include <convgeneratedcpp.h>
#include <convutils.h>

//The maximum length of any intermediate buffer allocated for conversion.
const TInt KMaximumLengthOfIntermediateBuffer=5;
//The ISCII ATR code point, used for ISCII script switching mechanism.
const TUint KControlCharacterEscape=0xef;
//The number of Indic scripts supported by the plug-in.
//ISCII in general addresses all the Indic character sets.
const TUint KNumberOfIndicCharactersetsSupported = 1;
//The common reason for panic for all panics raised by the iscii plug-in
_LIT16(KPanicReason,"ISCII Plug-in Panic"); 
//The escape sequence for ISCII (ATR) is 0xEF and immidiate byte following 
//that is the script selection code for Devanagari.
_LIT8(KEscapeSequenceDevanagari,"\xef\x42");
//The sequence for Explicit Halant, in unicode it gets converted to VIRAMA+ZWNJ
_LIT8(KExplicitHalant,"\xe8\xe8");
//For supportiing ISCII to Unicode conversion of multi byte ISCII sequences, the
//sequence is converted to intermediate unused iscii code point.
_LIT8(KReplacementForExplicitHalant,"\xe8\xfc");
//The sequence for Soft Halant, in unicode it gets converted to VIRAMA+ZWJ
_LIT8(KSoftHalant,"\xe8\xe9");
//For supportiing ISCII to Unicode conversion of multi byte ISCII sequences, the
//sequence is converted to intermediate unused iscii code point.
_LIT8(KReplacementForSoftHalant,"\xe8\xfd");
//Devanagari character Om
_LIT8(KOm,"\xa1\xe9");
////For supportiing ISCII to Unicode conversion of multi byte ISCII sequences, the
//sequence is converted to intermediate unused iscii code point.
_LIT8(KReplacementForOm,"\xfe");
//Devanagari character Avagraha
_LIT8(KAvagraha,"\xea\xe9");
//For supportiing ISCII to Unicode conversion of multi byte ISCII sequences, the
//sequence is converted to intermediate unused iscii code point.
_LIT8(KReplacementForAvagraha,"\xff");

//Devanagari character VOCALIC RR
_LIT8(KVocalicRr,"\xaa\xe9");
//For supportiing ISCII to Unicode conversion of multi byte ISCII sequences, the
//sequence is converted to intermediate unused iscii code point.
_LIT8(KReplacementForVocalicRr,"\x80");
//Devanagari character VOCALIC LL
_LIT8(KVocalicLl,"\xa7\xe9");
//For supportiing ISCII to Unicode conversion of multi byte ISCII sequences, the
//sequence is converted to intermediate unused iscii code point.
_LIT8(KReplacementForVocalicLl,"\x81");
//Devanagari character VOCALIC L SIGN
_LIT8(KVocalicLSign,"\xdb\xe9");
//For supportiing ISCII to Unicode conversion of multi byte ISCII sequences, the
//sequence is converted to intermediate unused iscii code point.
_LIT8(KReplacementForVocalicLSign,"\x82");
//Devanagari character VOCALIC LL SIGN
_LIT8(KVocalicLlSign,"\xdc\xe9");
//For supportiing ISCII to Unicode conversion of multi byte ISCII sequences, the
//sequence is converted to intermediate unused iscii code point.
_LIT8(KReplacementForVocalicLlSign,"\x83");
//Devanagari character VOCALIC L
_LIT8(KVocalicL,"\xa6\xe9");
//For supportiing ISCII to Unicode conversion of multi byte ISCII sequences, the
//sequence is converted to intermediate unused iscii code point.
_LIT8(KReplacementForVocalicL,"\x84");
//Devanagari character VOCALIC RR SIGN
_LIT8(KVocalicRrSign,"\xdf\xe9");
//For supportiing ISCII to Unicode conversion of multi byte ISCII sequences, the
//sequence is converted to intermediate unused iscii code point.
_LIT8(KReplacementForVocalicRrSign,"\x85");

//Unconvertible ISCII character
_LIT8(KIsciiUnconvertibleCharacter,"\xeb");

enum TPanic
{
	//The panic raised by ConvertToUnicodeFromModalForeign_Internal() if the input
	//conversion flag is CCnvCharacterSetConverter::EInputConversionFlagStopAtFirstUnconvertibleCharacter
	EPanicBadInputConversionFlags=1,
	//Panic raised if the buffer does not start with the escape sequence 0xEF
	EPanicBadRemainderOfForeign,
	//Panic is raised if the length of the search buffer is greater than the length of the 
	//replacement buffer
	EPanicBadReplacementBuffer,
	//If the offset of start of the escape sequence is not an unsigned number.
	EPanicBadStartOfNextEscapeSequence
};

//The dummy datastructure for the dummy conversion data i.e. used for conversion if the 
//script selection code is not supported.
#define ARRAY_LENGTH(aArray) (sizeof(aArray)/sizeof((aArray)[0]))

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry keyedTable1616_foreignToUnicode_1[]=
	{
		{
		0xa0,
		0xfffd
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry keyedTable1616_unicodeToForeign_1[]=
	{
		{
		0xfffd,
		0xa0
		}
	};

LOCAL_D const SCnvConversionData::SVariableByteData::SRange foreignVariableByteDataRanges[]=
	{
		{
		0x00,
		0xff,
		0,
		0
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange foreignToUnicodeDataRanges[]=
	{
		{
		0x00,
		0x7f,
		SCnvConversionData::SOneDirectionData::SRange::EDirect,
		0,
		0,
			{
			0
			}
		},
		{
		0xa0,
		0xff,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616,
		0,
		0,
			{
			UData_SKeyedTable1616(keyedTable1616_foreignToUnicode_1)
			}
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange unicodeToForeignDataRanges[]=
	{
		{
		0x0000,
		0x007f,
		SCnvConversionData::SOneDirectionData::SRange::EDirect,
		1,
		0,
			{
			0
			}
		},
		{
		0x00a0,
		0xffff,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616,
		1,
		0,
			{
			UData_SKeyedTable1616(keyedTable1616_unicodeToForeign_1)
			}
		}
	};

//The dummy conversion data to be used for conversion if the iscii code sequence is not
//Devanagari (i.e. the script selection code is not 0x42 and something else.
//In this case the ISCII characters are converted to unconvertible characters.

LOCAL_D const SCnvConversionData conversionDataDummy=
	{
	SCnvConversionData::EFixedBigEndian,
		{
		ARRAY_LENGTH(foreignVariableByteDataRanges),
		foreignVariableByteDataRanges
		},
		{
		ARRAY_LENGTH(foreignToUnicodeDataRanges),
		foreignToUnicodeDataRanges
		},
		{
		ARRAY_LENGTH(unicodeToForeignDataRanges),
		unicodeToForeignDataRanges
		},
	NULL,
	NULL
	};



#ifdef EKA2

///////////////////////////////////////////////////////////////
// 3.1 Code

// INCLUDES
#include <ecom/implementationproxy.h>
#include <charactersetconverter.h>


/**
* The character conversion plug-in implementation for Iscii.
*
*  @lib ecom.lib
*  @since Series 60 3.1
*/

class CIsciiImplementation : public CCharacterSetConverterPluginInterface
{
    public:
    	//From CCharacterSetConverterPluginInterface
        virtual const TDesC8& ReplacementForUnconvertibleUnicodeCharacters();

	//From CCharacterSetConverterPluginInterface
        virtual TInt ConvertFromUnicode(
            CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
            const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
            TDes8& aForeign, 
            const TDesC16& aUnicode, 
            CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters );

	//From CCharacterSetConverterPluginInterface
        virtual TInt ConvertToUnicode(
            CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
            TDes16& aUnicode, 
            const TDesC8& aForeign, 
            TInt&, 
            TInt& aNumberOfUnconvertibleCharacters, 
            TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter );

	//From CCharacterSetConverterPluginInterface
        virtual TBool IsInThisCharacterSetL(
            TBool& aSetToTrue, 
            TInt& aConfidenceLevel, 
            const TDesC8& );

        static CIsciiImplementation* NewL();

        virtual ~CIsciiImplementation();
    private:
        CIsciiImplementation();
};

//Checks if a descriptor starts with another descriptor at the begining.
LOCAL_C TBool IsStartOf(const TDesC8& aEscapeSequence, const TDesC8& aBuffer)
{
	const TInt lengthOfStart=aEscapeSequence.Length();
	return (aBuffer.Length()>=lengthOfStart) && (aBuffer.Left(lengthOfStart)==aEscapeSequence);
}
// -----------------------------------------------------------------------------
// MatchesEscapeSequence()
//If the remainder of the foreign text starts with the passed escapesequence, modifies the remainder of the foreign text
//and  sets the homogeneous run buffer that uses the same conversion data.
//The homogeneous run buffer is the buffer that will use a single conversion data, it doesn't contain the attribute code
// neither it contains the script switching code.
//The aRemainderOfForeign buffer 

// Returns: ETrue: If the sequence contains the escape sequence
//              EFalse: If the sequence does not contain the escape sequence
//
// -----------------------------------------------------------------------------
//
LOCAL_C TBool MatchesEscapeSequence(TInt& aNumberOfForeignBytesConsumed, TPtrC8& aHomogeneousRun,
		TPtrC8& aRemainderOfForeign, const TDesC8& aEscapeSequence)
{
	const TInt lengthOfEscapeSequence=aEscapeSequence.Length();
	if (IsStartOf(aEscapeSequence, aRemainderOfForeign))
		{
		aRemainderOfForeign.Set(aRemainderOfForeign.Mid(lengthOfEscapeSequence));
		const TInt startOfNextEscapeSequence=aRemainderOfForeign.Locate(KControlCharacterEscape);
		if (startOfNextEscapeSequence==KErrNotFound)
			{
			aHomogeneousRun.Set(aRemainderOfForeign);
			aRemainderOfForeign.Set(NULL, 0);
			}
		else
			{
			aHomogeneousRun.Set(aRemainderOfForeign.Left(startOfNextEscapeSequence));
			aRemainderOfForeign.Set(aRemainderOfForeign.Mid(startOfNextEscapeSequence));
			}
		aNumberOfForeignBytesConsumed+=lengthOfEscapeSequence;
		return ETrue;
		}
	return EFalse;
}

// -----------------------------------------------------------------------------
// NextHomogeneousForeignRun()
//Matches the escape sequence of each of the elements of the SState array with the remainder of
//foreign text and if the escape sequence matches with the start of remainder of the foreign text, 
//then the conversion data is set to the conversion data corresponding to the escape sequence
//Also the homogeneous foreign text for conversion with the same escape sequence is set.

// Returns: ETrue: If length of the remainder of foreign buffer is nonzero.
//              EFalse: If length of the remainder of foreign buffer is zero.
//
// -----------------------------------------------------------------------------
//


LOCAL_C TBool NextHomogeneousForeignRun(const SCnvConversionData*& aConversionData, TInt& aNumberOfForeignBytesConsumed, TPtrC8& aHomogeneousRun, TPtrC8& aRemainderOfForeign, const TArray<CnvUtilities::SState>& aArrayOfStates, TUint& aOutputConversionFlags)
{
	TBool returnValue = EFalse;
	TBool foundState = EFalse;
	__ASSERT_DEBUG((aRemainderOfForeign.Length()==0) || (aRemainderOfForeign[0]==KControlCharacterEscape), User::Panic(KPanicReason,EPanicBadRemainderOfForeign));
	if (aRemainderOfForeign.Length()==0)
		{
		return returnValue;
		}
	const TInt numberOfStates=aArrayOfStates.Count();
	TInt i;
	for (i=0; i<numberOfStates; ++i)
		{
		const CnvUtilities::SState& state=aArrayOfStates[i];
		if (MatchesEscapeSequence(aNumberOfForeignBytesConsumed, aHomogeneousRun, aRemainderOfForeign, *state.iEscapeSequence))
			{
			aConversionData=state.iConversionData;
			foundState = ETrue;
			}
		}
	if(!foundState)
		{
		for (i=0; i<numberOfStates; ++i)
			{
			if (IsStartOf(aRemainderOfForeign, *aArrayOfStates[i].iEscapeSequence))
				{
				// aRemainderOfForeign ends with a truncated escape sequence, so ConvertToUnicode cannot convert any more
				aOutputConversionFlags|=CCnvCharacterSetConverter::EOutputConversionFlagInputIsTruncated;
				break;
				}
			}

		MatchesEscapeSequence(aNumberOfForeignBytesConsumed,aHomogeneousRun,aRemainderOfForeign,aRemainderOfForeign.Left(2));
		aConversionData = &conversionDataDummy;	
		returnValue = ETrue;
		}
		if (aHomogeneousRun.Length()>0)
			{
			returnValue = ETrue;
			}
		return returnValue;		
}
// -----------------------------------------------------------------------------
// ConvertFromUnicodeIntermediateBufferInPlace()
//Default implementation for conversion to the intermediate buffer
//It modifies the unicode buffer before it is converted back to iscii.
//The current implementation of iscii plug-in doesn't require any
//modification to the default implementation
// Returns: Nothing
//
// -----------------------------------------------------------------------------
//


LOCAL_C void ConvertFromUnicodeIntermediateBufferInPlace(TInt aStartPositionInDescriptor, TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut)
{
	CnvUtilities::ConvertFromIntermediateBufferInPlace(aStartPositionInDescriptor, aDescriptor, aNumberOfCharactersThatDroppedOut, KEscapeSequenceDevanagari, 1);
}

// -----------------------------------------------------------------------------
// DoFindAndModifyBuffer()
//Modifies the iscii buffer by replacing the search buffer with the replacement buffer.
//Introduced for handling multibyte iscii sequence.
//Takes the search buffer array and the replacement buffer arrays as input to it and replaces all
//the occurances of the search buffer with the corresponding replace buffer.
// Returns: Nothing
//
// -----------------------------------------------------------------------------
//

LOCAL_C void DoFindAndModifyBuffer(TDes8& aModifyBuffer,const TDesC8& aSearchBuffer,const TDesC8& aReplaceBuffer)
{
	FOREVER
	{
		TInt offset;
		__ASSERT_ALWAYS((aSearchBuffer.Length()>= aReplaceBuffer.Length()),User::Panic(KPanicReason,EPanicBadReplacementBuffer));
		if((offset = aModifyBuffer.Find(aSearchBuffer)) != KErrNotFound)
		{
			TUint8 *pointerToBuffer = const_cast<TUint8*> (aModifyBuffer.Ptr());
			Mem::Copy(pointerToBuffer+offset,aReplaceBuffer.Ptr(),aReplaceBuffer.Length());
			Mem::Copy(pointerToBuffer+offset+aReplaceBuffer.Length(),pointerToBuffer+offset+aSearchBuffer.Length(),aModifyBuffer.Length()-aSearchBuffer.Length()-offset);
			aModifyBuffer.SetLength(aModifyBuffer.Length() - aSearchBuffer.Length() + aReplaceBuffer.Length());
		}
		else
			break;
	}
	
}

// -----------------------------------------------------------------------------
// FindAndModifyBuffer()
//Modifies the iscii buffer by replacing the search buffer with the replacement buffer.
//Calls DoFindAndModifyBuffer() and supplies the search buffer and replacement buffer.
//Introduced for handling multibyte iscii sequence.
// Returns: Nothing
//
// -----------------------------------------------------------------------------
//

LOCAL_C void FindAndModifyBuffer(TDes8& aModifyBuffer)
{
	TInt ret = KErrNone;
    RArray<TPtrC8> searchBuffer;
	RArray<TPtrC8> replaceBuffer;
	
	//If the passed buffer contains the replacement buffer,
	//Then it should not get converted to respective Unicode
	//buffer rather it should get converted to replacement for
	//unconvertible character.
	
	ret |= searchBuffer.Append(KReplacementForExplicitHalant().Right(1));
	ret |= searchBuffer.Append(KReplacementForSoftHalant().Right(1));
	ret |= searchBuffer.Append(KReplacementForOm().Right(1));
	ret |= searchBuffer.Append(KReplacementForAvagraha().Right(1));
	
	ret |= searchBuffer.Append(KReplacementForVocalicRr().Right(1));
	ret |= searchBuffer.Append(KReplacementForVocalicLl().Right(1));
	ret |= searchBuffer.Append(KReplacementForVocalicLSign().Right(1));
	ret |= searchBuffer.Append(KReplacementForVocalicLlSign().Right(1));
	ret |= searchBuffer.Append(KReplacementForVocalicL().Right(1));
	ret |= searchBuffer.Append(KReplacementForVocalicRrSign().Right(1));
	
	//All normal search buffers
	ret |= searchBuffer.Append(KExplicitHalant().Mid(0));
	ret |= searchBuffer.Append(KSoftHalant().Mid(0));
	ret |= searchBuffer.Append(KOm().Mid(0));
	ret |= searchBuffer.Append(KAvagraha().Mid(0));
	
	ret |= searchBuffer.Append(KVocalicRr().Mid(0));
	ret |= searchBuffer.Append(KVocalicLl().Mid(0));
	ret |= searchBuffer.Append(KVocalicLSign().Mid(0));
	ret |= searchBuffer.Append(KVocalicLlSign().Mid(0));
	ret |= searchBuffer.Append(KVocalicL().Mid(0));
	ret |= searchBuffer.Append(KVocalicRrSign().Mid(0));
	
	//The replacement buffer for the odd cases to restrict the 
	//replacement buffers not to get converted to the corresponding 
	//unicode buffer
	
	ret |= replaceBuffer.Append(KIsciiUnconvertibleCharacter().Mid(0));
	ret |= replaceBuffer.Append(KIsciiUnconvertibleCharacter().Mid(0));
	ret |= replaceBuffer.Append(KIsciiUnconvertibleCharacter().Mid(0));
	ret |= replaceBuffer.Append(KIsciiUnconvertibleCharacter().Mid(0));
	
	ret |= replaceBuffer.Append(KIsciiUnconvertibleCharacter().Mid(0));
	ret |= replaceBuffer.Append(KIsciiUnconvertibleCharacter().Mid(0));
	ret |= replaceBuffer.Append(KIsciiUnconvertibleCharacter().Mid(0));
	ret |= replaceBuffer.Append(KIsciiUnconvertibleCharacter().Mid(0));
	ret |= replaceBuffer.Append(KIsciiUnconvertibleCharacter().Mid(0));
	ret |= replaceBuffer.Append(KIsciiUnconvertibleCharacter().Mid(0));
	
	//All normal replace buffers		
	ret |= replaceBuffer.Append(KReplacementForExplicitHalant().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForSoftHalant().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForOm().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForAvagraha().Mid(0));
	
	
	ret |= replaceBuffer.Append(KReplacementForVocalicRr().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForVocalicLl().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForVocalicLSign().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForVocalicLlSign().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForVocalicL().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForVocalicRrSign().Mid(0));
	
	__ASSERT_DEBUG(!ret, User::Panic(_L("RArray append failure"), ret));
			
	for(TInt counter=0;counter<searchBuffer.Count();counter++)
	{
		DoFindAndModifyBuffer(aModifyBuffer,searchBuffer[counter],replaceBuffer[counter]);
	}
	searchBuffer.Reset();
	replaceBuffer.Reset();
	
}

// -----------------------------------------------------------------------------
// DoNormalizeReturnValue()
//Modifies the return value(Number of bytes did not get converted) according to the modified
//intermediate buffer. Takes the modified intermediate buffer, return value, search and replace
//buffers. Searches for the replace buffer in the intermediate buffer and if found, it looks for
//the actual buffer corresponding to the replace buffer and adds the difference of the lengths
//of the actual to replace buffer to the return value.
// Returns: Nothing
//
// -----------------------------------------------------------------------------
//

LOCAL_C void DoNormalizeReturnValue(TUint& aReturnValue,const TDesC8& aBuffer,RArray<TPtrC8>& anArrayOfSearches,RArray<TPtrC8>& anArrayOfReplaces)
{
	TPtr8 buffer(const_cast<TUint8*>(aBuffer.Ptr()),aBuffer.Length(),aBuffer.Length());
	TUint count = anArrayOfSearches.Count();
	FOREVER
	{
		TBool flag = EFalse;
		for(TUint i=0;i<count;i++)
		{
			TPtrC8 searchBufferForComparison(buffer.Right(anArrayOfReplaces[i].Length()));
			TInt returnCompare = searchBufferForComparison.Compare(anArrayOfReplaces[i]);
			if(returnCompare == 0)
			{
				flag =ETrue;
				aReturnValue += (anArrayOfSearches[i].Length() - anArrayOfReplaces[i].Length());
				buffer=buffer.MidTPtr(0,buffer.Length()-anArrayOfReplaces[i].Length());
				break;
			}
		}
		
		if(buffer.Length() == 0)
		{
			break;
		}
		
		if(!flag)
		{
			buffer=buffer.MidTPtr(0,buffer.Length()-1);
		}
	}

}

// -----------------------------------------------------------------------------
// NormalizeReturnValue()
//Modifies the return value(Number of bytes did not get converted) according to the 
//replacements done to the iscii buffer before conversion.
//Internally calls DoNormalizeReturnValue() by suppling the search and replacement 
//buffers.

// Returns: Nothing
//
// -----------------------------------------------------------------------------
//

LOCAL_C void NormalizeReturnValue(TUint& aReturnValue,const TDesC8& aBuffer)
{
	TInt ret = KErrNone;
    RArray<TPtrC8> searchBuffer;
	RArray<TPtrC8> replaceBuffer;
	
	ret |= searchBuffer.Append(KExplicitHalant().Mid(0));
	ret |= searchBuffer.Append(KSoftHalant().Mid(0));
	ret |= searchBuffer.Append(KOm().Mid(0));
	ret |= searchBuffer.Append(KAvagraha().Mid(0));
	
	ret |= searchBuffer.Append(KVocalicRr().Mid(0));
	ret |= searchBuffer.Append(KVocalicLl().Mid(0));
	ret |= searchBuffer.Append(KVocalicLSign().Mid(0));
	ret |= searchBuffer.Append(KVocalicLlSign().Mid(0));
	ret |= searchBuffer.Append(KVocalicL().Mid(0));
	ret |= searchBuffer.Append(KVocalicRrSign().Mid(0));
	
	ret |= replaceBuffer.Append(KReplacementForExplicitHalant().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForSoftHalant().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForOm().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForAvagraha().Mid(0));
	
	ret |= replaceBuffer.Append(KReplacementForVocalicRr().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForVocalicLl().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForVocalicLSign().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForVocalicLlSign().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForVocalicL().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForVocalicRrSign().Mid(0));
	
	__ASSERT_DEBUG(!ret, User::Panic(_L("RArray append failure"), ret));
	
	DoNormalizeReturnValue(aReturnValue,aBuffer,searchBuffer,replaceBuffer);
	searchBuffer.Reset();
	replaceBuffer.Reset();
}

// -----------------------------------------------------------------------------
// HandleHomogeneousRun()
//Handles a homogeneous foreign buffer and converts the foreign buffer to unicode
//On return the aUnicode argument contains the converted unicode data.
//Also it sets the return value, returned from the conversion. The return value also
//takes into account if there is any buffer modification done before passing it to
//CCnvCharacterSetConverter::DoConvertToUnicode()
//buffers.

// Returns: Nothing
//
// -----------------------------------------------------------------------------
//

LOCAL_C void HandleHomogeneousRun(const SCnvConversionData*& aConversionData, 
							CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters,
							TDes16& aUnicode,
							const TDesC8& aHomogeneousForeign,
							TInt& aNumberOfUnconvertibleCharacters,
							TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter,
							TUint& aOutputConversionFlags,
							TUint aInputConversionFlags,TInt& aNumberOfForeignBytesConsumed,
							TInt& aReturnValue)
{
	TInt numberOfUnconvertibleCharacters;
	TInt indexOfFirstByteOfFirstUnconvertibleCharacter;
	TUint noOfConsumedBytes = 0;
	if(aConversionData == NULL)
		{
		aReturnValue = CCnvCharacterSetConverter::EErrorIllFormedInput;
		return;
		}
	aReturnValue = CCnvCharacterSetConverter::DoConvertToUnicode(*aConversionData,aDefaultEndiannessOfForeignCharacters,
															  aUnicode,aHomogeneousForeign,numberOfUnconvertibleCharacters,
															  indexOfFirstByteOfFirstUnconvertibleCharacter,aOutputConversionFlags,

	//The numberOfUnconvertibleCharacters and indexOfFirstByteOfFirstUnconvertibleCharacter are the values with
	//respect to the intermediate iscii buffer and original values aIndexOfFirstByteOfFirstUnconvertibleCharacter and 
	//aNumberOfUnconvertibleCharacters need to be adjusted accordingly.
	
	aInputConversionFlags);
	if(numberOfUnconvertibleCharacters>0)
		{
		if(aNumberOfUnconvertibleCharacters == 0)
			{
			aIndexOfFirstByteOfFirstUnconvertibleCharacter = aNumberOfForeignBytesConsumed + indexOfFirstByteOfFirstUnconvertibleCharacter;
			}
		aNumberOfUnconvertibleCharacters+=numberOfUnconvertibleCharacters;
		}
	noOfConsumedBytes = aHomogeneousForeign.Length();
	//To Check whether it is really required.
	NormalizeReturnValue(noOfConsumedBytes,aHomogeneousForeign);
	aNumberOfForeignBytesConsumed+=noOfConsumedBytes;
	if(aReturnValue>0)
		{
		TUint normalizedReturnValue = aReturnValue;
		
		//There original iscii buffer copied to an intermediate iscii buffer and then modified 
		//and is then passed for conversion. Now, after conversion, the return value needs to
		//be adjusted according to the original buffer. NormalizeReturnValue() does the 
		//same thing.
		
		NormalizeReturnValue(normalizedReturnValue,aHomogeneousForeign);
		aNumberOfForeignBytesConsumed-=normalizedReturnValue;
		aReturnValue=normalizedReturnValue;
		}
	
	//The HandleHomogeneousRun() method is called in a loop and once there is some
	//iscii codes converted to unicode, the ConvertToUnicode() should not return
	//CCnvCharacterSetConverter::EErrorIllFormedInput even though the conversion
	//method does not convert any of the iscii codes ppassed. To ensure that once the
	//first non-zero number of iscii codes are converted, the internal input conversion
	//flag is set to EInputConversionFlagAllowTruncatedInputNotEvenPartlyConsumable.
	
	if(aNumberOfForeignBytesConsumed>0)
		{
		aInputConversionFlags|=CCnvCharacterSetConverter::EInputConversionFlagAllowTruncatedInputNotEvenPartlyConsumable;
		}
	return;
}

// -----------------------------------------------------------------------------
// IsTruncatedDoubleByteIsciiSequence()
//Checks if anIsciiBuffer is a part of multi byte iscii sequence truncated in the middle.
//If it is a truncated sequence, then returns ETrue else returns EFalse.
//
// Returns: ETrue: If the intermediate input iscii buffer is truncated
//              EFalse: If the intermediate input iscii buffer is not truncated
//
// -----------------------------------------------------------------------------
//

LOCAL_C TBool IsTruncatedDoubleByteIsciiSequence(const TDesC8& anIsciiBuffer)
{
	RArray<TPtrC8> searchBuffer;
	if(anIsciiBuffer.Length () == 0)
		return EFalse;
	if(anIsciiBuffer[anIsciiBuffer.Length()-1] == 0xEF)
	return ETrue;
	
	TInt appendret = KErrNone;
	appendret |= searchBuffer.Append(KSoftHalant().Mid(0));
	appendret |= searchBuffer.Append(KOm().Mid(0));
	appendret |= searchBuffer.Append(KAvagraha().Mid(0));
	appendret |= searchBuffer.Append(KExplicitHalant().Mid(0));
	__ASSERT_DEBUG(!appendret, User::Panic(_L("RArray append failure"), appendret));
	
	TBool ret = EFalse;
	TBool isNotTruncated =EFalse;
	
	//First check if the intermediate iscii buffer is ending with a complete multi byte sequence.
	//If it ends with a complete multi byte sequence, no need to check if the last character of 
	//intermediate iscii is same as first character of multi byte iscii sequence. And return EFalse.
	for(TUint counter = 0;counter<searchBuffer.Count();counter++)
		{
		if(searchBuffer[counter].Compare(anIsciiBuffer.Right(searchBuffer[counter].Length())) == 0)
			{
			isNotTruncated = ETrue;
			break;
			}
		}
	//If the intermediate iscii buffer is not ending with a complete multi byte sequence, and the 
	//last code of the iscii buffer is a part of the multibyte sequence, then the iscii buffer is a 
	//truncated sequence and in that case return ETrue.
	
	for(TUint counter = 0;counter<searchBuffer.Count();counter++)
		{
		if(isNotTruncated)
			break;
		else if( (anIsciiBuffer[anIsciiBuffer.Length()-1] == searchBuffer[counter][0]))
			{
			ret =ETrue;
			break;
			}
		}
	searchBuffer.Reset();
	return ret;
}

// -----------------------------------------------------------------------------
// ReplacementForUnconvertibleUnicodeCharacters()
//Returns the replacement character for unconvertible unicode character.
//In the current implementation it is 0x1a (ASCII Substitute character)
//The default implementation calls ReplacementForUnconvertibleUnicodeCharacters_internal()
//in turn which is generated by cnvtool.
//
// Returns: Replacemt for unconvertible unicode characters (0x1a)
//
// -----------------------------------------------------------------------------
//

const TDesC8& CIsciiImplementation::ReplacementForUnconvertibleUnicodeCharacters()
	{
    return ReplacementForUnconvertibleUnicodeCharacters_internal();
	}

// -----------------------------------------------------------------------------
// ConvertFromUnicode()
//The main conversion function for converting from unicode to iscii
//Loaded and called by the character conversion framework.
//In turn calls CnvUtilities::ConvertFromUnicode() which is Symbian provide
//utility method for converting unicode to modal character codes.
//
// Returns: The numbet of unicode codes it could not convert.
//
// -----------------------------------------------------------------------------
//

TInt CIsciiImplementation::ConvertFromUnicode(
         CCnvCharacterSetConverter::TEndianness 
         aDefaultEndiannessOfForeignCharacters, 
         const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
         TDes8& aForeign, 
         const TDesC16& aUnicode, 
         CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters )
	{
	TFixedArray<CnvUtilities::SCharacterSet,KNumberOfIndicCharactersetsSupported> aArrayOfCharacterSets;
	aArrayOfCharacterSets[0].iConversionData = &conversionData;
	aArrayOfCharacterSets[0].iConvertFromIntermediateBufferInPlace = ConvertFromUnicodeIntermediateBufferInPlace;
	aArrayOfCharacterSets[0].iEscapeSequence = &KEscapeSequenceDevanagari();

		return CnvUtilities::ConvertFromUnicode(
				aDefaultEndiannessOfForeignCharacters, 
				aReplacementForUnconvertibleUnicodeCharacters, 
				aForeign, 
				aUnicode, 
				aIndicesOfUnconvertibleCharacters, 
				aArrayOfCharacterSets.Array());
	}

// -----------------------------------------------------------------------------
// ConvertToUnicode()
//The main conversion function for converting from iscii to unicode
//Loaded and called by the character conversion framework.
//To support some of the iscii characters, the input forign buffer is
//copied to an intermediate buffer and then is then modified and 
//CCnvCharactersetConverter::DoConvertToUnicode() is called with
//the modified buffer. For extensibility of iscii to other Indic languages
//it uses CnvUtilities::SState datastructure. CnvUtilities::SState is a
//Symbian defined class for modal charactersets. The escape sequence 
//is specified to ATR followed by the script selection code and the conversion
//data is specified to be the conversion for the particular script. For the time
//being only Devanagari with script selection code 0x42 is supported. If 
//any of the other script codes are used the conversion leads to unconvertible
//character i.e. 0xFFFD.
// Returns: The numbet of iscii codes it could not convert.
//
// -----------------------------------------------------------------------------
//

TInt CIsciiImplementation::ConvertToUnicode(
         CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
         TDes16& aUnicode, 
         const TDesC8& aForeign, 
         TInt& aState, 
         TInt& aNumberOfUnconvertibleCharacters, 
         TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter )
	{
	aNumberOfUnconvertibleCharacters = 0;
	TUint aOutputConversionFlags = CCnvCharacterSetConverter::KStateDefault;
	aIndexOfFirstByteOfFirstUnconvertibleCharacter=-1;
	TUint internalInputConversionFlags = 0;
	TInt numberOfForeignBytesConsumed=0;
	TPtrC8 remainderOfForeign(aForeign);
	TInt returnValue;
	TBool flag = EFalse;
	TBool isSkipMatchSequence = EFalse;
	const SCnvConversionData* convData;
	//Set the iscii conversion data and escape sequence for Devanagari.
	TFixedArray<CnvUtilities::SState,KNumberOfIndicCharactersetsSupported> modals;
	modals[0].iConversionData = &conversionData;
	modals[0].iEscapeSequence = &KEscapeSequenceDevanagari();

	aUnicode.SetLength(0);
	
	//The internal input conversion flag for conversion is set to CCnvCharacterSetConverter::EInputConversionFlagAppend
	//so that for each conversion in the conversion loop, the generated conversion buffer is appened to the aUnicode buffer.
	internalInputConversionFlags |= CCnvCharacterSetConverter::EInputConversionFlagAppend;
	if (aForeign.Length()==0)
	{
		return 0;
	}
	//Get the conversion data from the previous call else the conversion data is set to the default 
	//conversion data, i.e. Devanagari.
	convData=(aState!=CCnvCharacterSetConverter::KStateDefault)? REINTERPRET_CAST(const SCnvConversionData*, aState): modals[0].iConversionData;
	FOREVER
	{
		TBuf8<KMaximumLengthOfIntermediateBuffer> intermediateBuffer;
		TUint numberOfForeignBytesConsumedThisTime = 0;
		if((remainderOfForeign.Length() >=KMaximumLengthOfIntermediateBuffer) && (aUnicode.MaxLength()-aUnicode.Length() >=KMaximumLengthOfIntermediateBuffer))
		{
			numberOfForeignBytesConsumedThisTime = KMaximumLengthOfIntermediateBuffer;
			intermediateBuffer = remainderOfForeign.Left(numberOfForeignBytesConsumedThisTime);
			//If the intermediate buffer is a part of truncated buffer sequence but the
			//actual input buffer is not truncated then truncated sequence is not converted.
			//The intermediate buffer is modified so as not to contain the truncated sequence.
			
			flag = IsTruncatedDoubleByteIsciiSequence(intermediateBuffer);
			if(flag)
				{
					numberOfForeignBytesConsumedThisTime --;
					intermediateBuffer = remainderOfForeign.Left(numberOfForeignBytesConsumedThisTime);					
				}
			
		}
		else
		{
			flag = IsTruncatedDoubleByteIsciiSequence(remainderOfForeign.Left(aUnicode.MaxLength()-aUnicode.Length()));
			if(!flag)
				{
				numberOfForeignBytesConsumedThisTime = aUnicode.MaxLength()-aUnicode.Length();
				intermediateBuffer = remainderOfForeign.Left(numberOfForeignBytesConsumedThisTime);
				}
			else
				{
				if(aForeign.Length()>(numberOfForeignBytesConsumed+aUnicode.Length()))
					{
					numberOfForeignBytesConsumedThisTime = aUnicode.MaxLength()-aUnicode.Length()-1;
					intermediateBuffer = remainderOfForeign.Left(numberOfForeignBytesConsumedThisTime);
					break;
					}
				else
					{
					numberOfForeignBytesConsumedThisTime = aUnicode.MaxLength()-aUnicode.Length();
					intermediateBuffer = remainderOfForeign.Left(numberOfForeignBytesConsumedThisTime);
					}
				}
		}			
		
		//The input intermediate iscii buffer is modified with the search and replace
		//buffers. It is required for supporting multibyte iscii sequences.		
		FindAndModifyBuffer(intermediateBuffer);
		TPtrC8 remainderOfForeignInternal(intermediateBuffer);
		TPtrC8 homogeneousRun;
		const TInt startOfNextEscapeSequence=intermediateBuffer.Locate(KControlCharacterEscape);
		if (startOfNextEscapeSequence!=0) 
			{
			if (startOfNextEscapeSequence==KErrNotFound)
				{
				homogeneousRun.Set(remainderOfForeignInternal);
				remainderOfForeignInternal.Set(NULL, 0);
				}
			else
				{
				__ASSERT_DEBUG(startOfNextEscapeSequence>0, User::Panic(KPanicReason,EPanicBadStartOfNextEscapeSequence));
				homogeneousRun.Set(remainderOfForeignInternal.Left(startOfNextEscapeSequence));
				remainderOfForeignInternal.Set(remainderOfForeignInternal.Mid(startOfNextEscapeSequence));
				}
			isSkipMatchSequence = ETrue;
			}
		FOREVER
			{
			if(!isSkipMatchSequence)
				{
				if (!NextHomogeneousForeignRun(convData, numberOfForeignBytesConsumed, homogeneousRun, 
											remainderOfForeignInternal, modals.Array(), aOutputConversionFlags))
					{
					break;
					}
				}
			HandleHomogeneousRun( convData, aDefaultEndiannessOfForeignCharacters, aUnicode, homogeneousRun, aNumberOfUnconvertibleCharacters, 
								aIndexOfFirstByteOfFirstUnconvertibleCharacter,aOutputConversionFlags,internalInputConversionFlags,
								numberOfForeignBytesConsumed,returnValue);
			if(returnValue<0)
				{
				return returnValue;
				}
			isSkipMatchSequence = EFalse;
			}
		if(returnValue > 0)
			break;
		if ((!flag && (numberOfForeignBytesConsumedThisTime != KMaximumLengthOfIntermediateBuffer)) || (flag && (numberOfForeignBytesConsumedThisTime != (KMaximumLengthOfIntermediateBuffer-1) )))
			break;
		remainderOfForeign.Set(aForeign.Mid(numberOfForeignBytesConsumed));				
	}
	//If the number of iscii bytes consumed by the conversion is zero also the output conversion
	//flag is not set to EOutputConversionFlagInputIsTruncated, then return EErrorIllFormedInput.
	if ((numberOfForeignBytesConsumed==0) && (aOutputConversionFlags&CCnvCharacterSetConverter::EOutputConversionFlagInputIsTruncated))
		{
		return CCnvCharacterSetConverter::EErrorIllFormedInput;
		}
	//Set the conversion data sothat next time when ConvertToUnicode() is called,
	//will use the previous conversion data.
	aState=REINTERPRET_CAST(TInt, convData);
	return aForeign.Length()-numberOfForeignBytesConsumed;
    }

// -----------------------------------------------------------------------------
// IsInThisCharacterSetL()
//The method tells how probable it is that a sample piece of text is encoded in this character set.
//On return aConfidenceLevel, indicates how confident the function is about its return value. For
//iscii it is the default implementation and it does not implement the autodetect functionality.
//Loaded and called by the character conversion framework.
//
// Returns: EFalse: To tell CCnvCharacterSetConverter::AutoDetectCharacterSetL()  that the plug-in DLL
//                          is not implementing a function of this signature and is therefore empty.
//
// -----------------------------------------------------------------------------
//


//Default implementation for IsInThisCharacterSetL()

TBool CIsciiImplementation::IsInThisCharacterSetL(
         TBool& aSetToTrue, 
         TInt& aConfidenceLevel, 
         const TDesC8& )
	{
    	aSetToTrue = EFalse;
	aConfidenceLevel = 0;
	return EFalse;
	}

// -----------------------------------------------------------------------------
// NewL()
//Factory function for CIsciiImplementation(). Instantiates a CIsciiImplementation object on heap
//and returns the pointer to it.
//
// Returns: CIsciiImplementation*
//
// -----------------------------------------------------------------------------
//
	
CIsciiImplementation* CIsciiImplementation::NewL()
    {
    CIsciiImplementation* self = new(ELeave) CIsciiImplementation;
    return self;
    }

// -----------------------------------------------------------------------------
// CIsciiImplementation()
//default constructor, does nothing
//
// Returns: Nothing
//
// -----------------------------------------------------------------------------
//
CIsciiImplementation::CIsciiImplementation()
    {
    }

// -----------------------------------------------------------------------------
// ~CIsciiImplementation()
//default desstructor, does nothing
//
// Returns: Nothing
//
// -----------------------------------------------------------------------------
//

CIsciiImplementation::~CIsciiImplementation()
    {
    }

// ECOM CREATION FUNCTION
const TImplementationProxy ImplementationTable[] = 
    {
    // Used also in 0x1027508E.rss ( implementation_uid )
    IMPLEMENTATION_PROXY_ENTRY( 0x1027508E, CIsciiImplementation::NewL )
    };

// -----------------------------------------------------------------------------
// ImplementationGroupProxy()
//Returns a pointer to TImplementationProxy object which contains the implementation uid vs factory
//function table. Also on return sets the aTableCount to the number of entries in the table.
//
// Returns: TImplementationProxy*
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
#else

#include <convplug.h>

#ifndef EKA2
// -----------------------------------------------------------------------------
// E32Dll()
//For EKA1 this is the entry point for the DLL.
//
// Returns: KErrNone
//
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Dll(TDllReason)
{
	return KErrNone;
}
#endif

//Checks if a descriptor starts with another descriptor at the begining.
LOCAL_C TBool IsStartOf(const TDesC8& aEscapeSequence, const TDesC8& aBuffer)
{
	const TInt lengthOfStart=aEscapeSequence.Length();
	return (aBuffer.Length()>=lengthOfStart) && (aBuffer.Left(lengthOfStart)==aEscapeSequence);
}
// -----------------------------------------------------------------------------
// MatchesEscapeSequence()
//If the remainder of the foreign text starts with the passed escapesequence, modifies the remainder of the foreign text
//and  sets the homogeneous run buffer that uses the same conversion data.
//The homogeneous run buffer is the buffer that will use a single conversion data, it doesn't contain the attribute code
// neither it contains the script switching code.
//The aRemainderOfForeign buffer 

// Returns: ETrue: If the sequence contains the escape sequence
//              EFalse: If the sequence does not contain the escape sequence
//
// -----------------------------------------------------------------------------
//
LOCAL_C TBool MatchesEscapeSequence(TInt& aNumberOfForeignBytesConsumed, TPtrC8& aHomogeneousRun,
		TPtrC8& aRemainderOfForeign, const TDesC8& aEscapeSequence)
{
	const TInt lengthOfEscapeSequence=aEscapeSequence.Length();
	if (IsStartOf(aEscapeSequence, aRemainderOfForeign))
		{
		aRemainderOfForeign.Set(aRemainderOfForeign.Mid(lengthOfEscapeSequence));
		const TInt startOfNextEscapeSequence=aRemainderOfForeign.Locate(KControlCharacterEscape);
		if (startOfNextEscapeSequence==KErrNotFound)
			{
			aHomogeneousRun.Set(aRemainderOfForeign);
			aRemainderOfForeign.Set(NULL, 0);
			}
		else
			{
			aHomogeneousRun.Set(aRemainderOfForeign.Left(startOfNextEscapeSequence));
			aRemainderOfForeign.Set(aRemainderOfForeign.Mid(startOfNextEscapeSequence));
			}
		aNumberOfForeignBytesConsumed+=lengthOfEscapeSequence;
		return ETrue;
		}
	return EFalse;
}

// -----------------------------------------------------------------------------
// NextHomogeneousForeignRun()
//Matches the escape sequence of each of the elements of the SState array with the remainder of
//foreign text and if the escape sequence matches with the start of remainder of the foreign text, 
//then the conversion data is set to the conversion data corresponding to the escape sequence
//Also the homogeneous foreign text for conversion with the same escape sequence is set.

// Returns: ETrue: If length of the remainder of foreign buffer is nonzero.
//              EFalse: If length of the remainder of foreign buffer is zero.
//
// -----------------------------------------------------------------------------
//


LOCAL_C TBool NextHomogeneousForeignRun(const SCnvConversionData*& aConversionData, TInt& aNumberOfForeignBytesConsumed, TPtrC8& aHomogeneousRun, TPtrC8& aRemainderOfForeign, const TArray<CnvUtilities::SState>& aArrayOfStates, TUint& aOutputConversionFlags)
{
	TBool returnValue = EFalse;
	TBool foundState = EFalse;
	__ASSERT_DEBUG((aRemainderOfForeign.Length()==0) || (aRemainderOfForeign[0]==KControlCharacterEscape), User::Panic(KPanicReason,EPanicBadRemainderOfForeign));
	if (aRemainderOfForeign.Length()==0)
		{
		return returnValue;
		}
	const TInt numberOfStates=aArrayOfStates.Count();
	TInt i;
	for (i=0; i<numberOfStates; ++i)
		{
		const CnvUtilities::SState& state=aArrayOfStates[i];
		if (MatchesEscapeSequence(aNumberOfForeignBytesConsumed, aHomogeneousRun, aRemainderOfForeign, *state.iEscapeSequence))
			{
			aConversionData=state.iConversionData;
			foundState = ETrue;
			}
		}
	if(!foundState)
		{
		for (i=0; i<numberOfStates; ++i)
			{
			if (IsStartOf(aRemainderOfForeign, *aArrayOfStates[i].iEscapeSequence))
				{
				// aRemainderOfForeign ends with a truncated escape sequence, so ConvertToUnicode cannot convert any more
				aOutputConversionFlags|=CCnvCharacterSetConverter::EOutputConversionFlagInputIsTruncated;
				break;
				}
			}

		MatchesEscapeSequence(aNumberOfForeignBytesConsumed,aHomogeneousRun,aRemainderOfForeign,aRemainderOfForeign.Left(2));
		aConversionData = &conversionDataDummy;	
		returnValue = ETrue;
		}
		if (aHomogeneousRun.Length()>0)
			{
			returnValue = ETrue;
			}
		return returnValue;		
}
// -----------------------------------------------------------------------------
// ConvertFromUnicodeIntermediateBufferInPlace()
//Default implementation for conversion to the intermediate buffer
//It modifies the unicode buffer before it is converted back to iscii.
//The current implementation of iscii plug-in doesn't require any
//modification to the default implementation
// Returns: Nothing
//
// -----------------------------------------------------------------------------
//


LOCAL_C void ConvertFromUnicodeIntermediateBufferInPlace(TInt aStartPositionInDescriptor, TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut)
{
	CnvUtilities::ConvertFromIntermediateBufferInPlace(aStartPositionInDescriptor, aDescriptor, aNumberOfCharactersThatDroppedOut, KEscapeSequenceDevanagari, 1);
}

// -----------------------------------------------------------------------------
// DoFindAndModifyBuffer()
//Modifies the iscii buffer by replacing the search buffer with the replacement buffer.
//Introduced for handling multibyte iscii sequence.
//Takes the search buffer array and the replacement buffer arrays as input to it and replaces all
//the occurances of the search buffer with the corresponding replace buffer.
// Returns: Nothing
//
// -----------------------------------------------------------------------------
//

LOCAL_C void DoFindAndModifyBuffer(TDes8& aModifyBuffer,const TDesC8& aSearchBuffer,const TDesC8& aReplaceBuffer)
{
	FOREVER
	{
		TInt offset;
		__ASSERT_ALWAYS((aSearchBuffer.Length()>= aReplaceBuffer.Length()),User::Panic(KPanicReason,EPanicBadReplacementBuffer));
		if((offset = aModifyBuffer.Find(aSearchBuffer)) != KErrNotFound)
		{
			TUint8 *pointerToBuffer = const_cast<TUint8*> (aModifyBuffer.Ptr());
			Mem::Copy(pointerToBuffer+offset,aReplaceBuffer.Ptr(),aReplaceBuffer.Length());
			Mem::Copy(pointerToBuffer+offset+aReplaceBuffer.Length(),pointerToBuffer+offset+aSearchBuffer.Length(),aModifyBuffer.Length()-aSearchBuffer.Length()-offset);
			aModifyBuffer.SetLength(aModifyBuffer.Length() - aSearchBuffer.Length() + aReplaceBuffer.Length());
		}
		else
			break;
	}
	
}

// -----------------------------------------------------------------------------
// FindAndModifyBuffer()
//Modifies the iscii buffer by replacing the search buffer with the replacement buffer.
//Calls DoFindAndModifyBuffer() and supplies the search buffer and replacement buffer.
//Introduced for handling multibyte iscii sequence.
// Returns: Nothing
//
// -----------------------------------------------------------------------------
//

LOCAL_C void FindAndModifyBuffer(TDes8& aModifyBuffer)
{
	RArray<TPtrC8> searchBuffer;
	RArray<TPtrC8> replaceBuffer;
	
	TInt ret = KErrNone;
	ret |= searchBuffer.Append(KExplicitHalant().Mid(0));
	ret |= searchBuffer.Append(KSoftHalant().Mid(0));
	ret |= searchBuffer.Append(KOm().Mid(0));
	ret |= searchBuffer.Append(KAvagraha().Mid(0));
	
	ret |= replaceBuffer.Append(KReplacementForExplicitHalant().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForSoftHalant().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForOm().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForAvagraha().Mid(0));
	
	__ASSERT_DEBUG(!ret, User::Panic(_L("RArray append failure"), ret));
	
	for(TInt counter=0;counter<searchBuffer.Count();counter++)
	{
		DoFindAndModifyBuffer(aModifyBuffer,searchBuffer[counter],replaceBuffer[counter]);
	}
	searchBuffer.Reset();
	replaceBuffer.Reset();
	
}

// -----------------------------------------------------------------------------
// DoNormalizeReturnValue()
//Modifies the return value(Number of bytes did not get converted) according to the modified
//intermediate buffer. Takes the modified intermediate buffer, return value, search and replace
//buffers. Searches for the replace buffer in the intermediate buffer and if found, it looks for
//the actual buffer corresponding to the replace buffer and adds the difference of the lengths
//of the actual to replace buffer to the return value.
// Returns: Nothing
//
// -----------------------------------------------------------------------------
//

LOCAL_C void DoNormalizeReturnValue(TUint& aReturnValue,const TDesC8& aBuffer,RArray<TPtrC8>& anArrayOfSearches,RArray<TPtrC8>& anArrayOfReplaces)
{
	TPtr8 buffer(const_cast<TUint8*>(aBuffer.Ptr()),aBuffer.Length(),aBuffer.Length());
	TUint count = anArrayOfSearches.Count();
	FOREVER
	{
		TBool flag = EFalse;
		for(TUint i=0;i<count;i++)
		{
			TPtrC8 searchBufferForComparison(buffer.Right(anArrayOfReplaces[i].Length()));
			TInt returnCompare = searchBufferForComparison.Compare(anArrayOfReplaces[i]);
			if(returnCompare == 0)
			{
				flag =ETrue;
				aReturnValue += (anArrayOfSearches[i].Length() - anArrayOfReplaces[i].Length());
				buffer=buffer.MidTPtr(0,buffer.Length()-anArrayOfReplaces[i].Length());
				break;
			}
		}
		
		if(buffer.Length() == 0)
		{
			break;
		}
		
		if(!flag)
		{
			buffer=buffer.MidTPtr(0,buffer.Length()-1);
		}
	}

}

// -----------------------------------------------------------------------------
// NormalizeReturnValue()
//Modifies the return value(Number of bytes did not get converted) according to the 
//replacements done to the iscii buffer before conversion.
//Internally calls DoNormalizeReturnValue() by suppling the search and replacement 
//buffers.

// Returns: Nothing
//
// -----------------------------------------------------------------------------
//

LOCAL_C void NormalizeReturnValue(TUint& aReturnValue,const TDesC8& aBuffer)
{
	RArray<TPtrC8> searchBuffer;
	RArray<TPtrC8> replaceBuffer;
	TInt ret =KErrNone;
	ret |= searchBuffer.Append(KExplicitHalant().Mid(0));
	ret |= searchBuffer.Append(KSoftHalant().Mid(0));
	ret |= searchBuffer.Append(KOm().Mid(0));
	ret |= searchBuffer.Append(KAvagraha().Mid(0));
	
	ret |= replaceBuffer.Append(KReplacementForExplicitHalant().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForSoftHalant().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForOm().Mid(0));
	ret |= replaceBuffer.Append(KReplacementForAvagraha().Mid(0));
	
	__ASSERT_DEBUG(!ret, User::Panic(_L("RArray append failure"), ret));
	
	DoNormalizeReturnValue(aReturnValue,aBuffer,searchBuffer,replaceBuffer);
	searchBuffer.Reset();
	replaceBuffer.Reset();
}

// -----------------------------------------------------------------------------
// HandleHomogeneousRun()
//Handles a homogeneous foreign buffer and converts the foreign buffer to unicode
//On return the aUnicode argument contains the converted unicode data.
//Also it sets the return value, returned from the conversion. The return value also
//takes into account if there is any buffer modification done before passing it to
//CCnvCharacterSetConverter::DoConvertToUnicode()
//buffers.

// Returns: Nothing
//
// -----------------------------------------------------------------------------
//

LOCAL_C void HandleHomogeneousRun(const SCnvConversionData*& aConversionData, 
							CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters,
							TDes16& aUnicode,
							const TDesC8& aHomogeneousForeign,
							TInt& aNumberOfUnconvertibleCharacters,
							TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter,
							TUint& aOutputConversionFlags,
							TUint aInputConversionFlags,TInt& aNumberOfForeignBytesConsumed,
							TInt& aReturnValue)
{
	TInt numberOfUnconvertibleCharacters;
	TInt indexOfFirstByteOfFirstUnconvertibleCharacter;
	TUint noOfConsumedBytes = 0;
	if(aConversionData == NULL)
		{
		aReturnValue = CCnvCharacterSetConverter::EErrorIllFormedInput;
		return;
		}
	aReturnValue = CCnvCharacterSetConverter::DoConvertToUnicode(*aConversionData,aDefaultEndiannessOfForeignCharacters,
															  aUnicode,aHomogeneousForeign,numberOfUnconvertibleCharacters,
															  indexOfFirstByteOfFirstUnconvertibleCharacter,aOutputConversionFlags,

	//The numberOfUnconvertibleCharacters and indexOfFirstByteOfFirstUnconvertibleCharacter are the values with
	//respect to the intermediate iscii buffer and original values aIndexOfFirstByteOfFirstUnconvertibleCharacter and 
	//aNumberOfUnconvertibleCharacters need to be adjusted accordingly.
	
	aInputConversionFlags);
	if(numberOfUnconvertibleCharacters>0)
		{
		if(aNumberOfUnconvertibleCharacters == 0)
			{
			aIndexOfFirstByteOfFirstUnconvertibleCharacter = aNumberOfForeignBytesConsumed + indexOfFirstByteOfFirstUnconvertibleCharacter;
			}
		aNumberOfUnconvertibleCharacters+=numberOfUnconvertibleCharacters;
		}
	noOfConsumedBytes = aHomogeneousForeign.Length();
	//To Check whether it is really required.
	NormalizeReturnValue(noOfConsumedBytes,aHomogeneousForeign);
	aNumberOfForeignBytesConsumed+=noOfConsumedBytes;
	if(aReturnValue>0)
		{
		TUint normalizedReturnValue = aReturnValue;
		
		//There original iscii buffer copied to an intermediate iscii buffer and then modified 
		//and is then passed for conversion. Now, after conversion, the return value needs to
		//be adjusted according to the original buffer. NormalizeReturnValue() does the 
		//same thing.
		
		NormalizeReturnValue(normalizedReturnValue,aHomogeneousForeign);
		aNumberOfForeignBytesConsumed-=normalizedReturnValue;
		aReturnValue=normalizedReturnValue;
		}
	
	//The HandleHomogeneousRun() method is called in a loop and once there is some
	//iscii codes converted to unicode, the ConvertToUnicode() should not return
	//CCnvCharacterSetConverter::EErrorIllFormedInput even though the conversion
	//method does not convert any of the iscii codes ppassed. To ensure that once the
	//first non-zero number of iscii codes are converted, the internal input conversion
	//flag is set to EInputConversionFlagAllowTruncatedInputNotEvenPartlyConsumable.
	
	if(aNumberOfForeignBytesConsumed>0)
		{
		aInputConversionFlags|=CCnvCharacterSetConverter::EInputConversionFlagAllowTruncatedInputNotEvenPartlyConsumable;
		}
	return;
}

// -----------------------------------------------------------------------------
// IsTruncatedDoubleByteIsciiSequence()
//Checks if anIsciiBuffer is a part of multi byte iscii sequence truncated in the middle.
//If it is a truncated sequence, then returns ETrue else returns EFalse.
//
// Returns: ETrue: If the intermediate input iscii buffer is truncated
//              EFalse: If the intermediate input iscii buffer is not truncated
//
// -----------------------------------------------------------------------------
//

LOCAL_C TBool IsTruncatedDoubleByteIsciiSequence(const TDesC8& anIsciiBuffer)
{
	RArray<TPtrC8> searchBuffer;
	if(anIsciiBuffer.Length () == 0)
		return EFalse;
	if(anIsciiBuffer[anIsciiBuffer.Length()-1] == 0xEF)
	return ETrue;
	
	TInt appendret = KErrNone;
	appendret |= searchBuffer.Append(KSoftHalant().Mid(0));
	appendret |= searchBuffer.Append(KOm().Mid(0));
	appendret |= searchBuffer.Append(KAvagraha().Mid(0));
	appendret |= searchBuffer.Append(KExplicitHalant().Mid(0));
	__ASSERT_DEBUG(!ret, User::Panic(_L("RArray append failure"), ret));
	
	TBool ret = EFalse;
	TBool isNotTruncated =EFalse;
	
	//First check if the intermediate iscii buffer is ending with a complete multi byte sequence.
	//If it ends with a complete multi byte sequence, no need to check if the last character of 
	//intermediate iscii is same as first character of multi byte iscii sequence. And return EFalse.
	for(TUint counter = 0;counter<searchBuffer.Count();counter++)
		{
		if(searchBuffer[counter].Compare(anIsciiBuffer.Right(searchBuffer[counter].Length())) == 0)
			{
			isNotTruncated = ETrue;
			break;
			}
		}
	//If the intermediate iscii buffer is not ending with a complete multi byte sequence, and the 
	//last code of the iscii buffer is a part of the multibyte sequence, then the iscii buffer is a 
	//truncated sequence and in that case return ETrue.
	
	for(TUint counter = 0;counter<searchBuffer.Count();counter++)
		{
		if(isNotTruncated)
			break;
		else if( (anIsciiBuffer[anIsciiBuffer.Length()-1] == searchBuffer[counter][0]))
			{
			ret =ETrue;
			break;
			}
		}
	searchBuffer.Reset();
	return ret;
}

// -----------------------------------------------------------------------------
// ReplacementForUnconvertibleUnicodeCharacters()
//Returns the replacement character for unconvertible unicode character.
//In the current implementation it is 0x1a (ASCII Substitute character)
//The default implementation calls ReplacementForUnconvertibleUnicodeCharacters_internal()
//in turn which is generated by cnvtool.
//
// Returns: Replacemt for unconvertible unicode characters (0x1a)
//
// -----------------------------------------------------------------------------
//

EXPORT_C const TDesC8& ReplacementForUnconvertibleUnicodeCharacters()
	{
    return ReplacementForUnconvertibleUnicodeCharacters_internal();
	}

// -----------------------------------------------------------------------------
// ConvertFromUnicode()
//The main conversion function for converting from unicode to iscii
//Loaded and called by the character conversion framework.
//In turn calls CnvUtilities::ConvertFromUnicode() which is Symbian provide
//utility method for converting unicode to modal character codes.
//
// Returns: The numbet of unicode codes it could not convert.
//
// -----------------------------------------------------------------------------
//

EXPORT_C TInt ConvertFromUnicode(
         CCnvCharacterSetConverter::TEndianness 
         aDefaultEndiannessOfForeignCharacters, 
         const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
         TDes8& aForeign, 
         const TDesC16& aUnicode, 
         CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters )
	{
	TFixedArray<CnvUtilities::SCharacterSet,KNumberOfIndicCharactersetsSupported> aArrayOfCharacterSets;
	aArrayOfCharacterSets[0].iConversionData = &conversionData;
	aArrayOfCharacterSets[0].iConvertFromIntermediateBufferInPlace = ConvertFromUnicodeIntermediateBufferInPlace;
	aArrayOfCharacterSets[0].iEscapeSequence = &KEscapeSequenceDevanagari();

		return CnvUtilities::ConvertFromUnicode(
				aDefaultEndiannessOfForeignCharacters, 
				aReplacementForUnconvertibleUnicodeCharacters, 
				aForeign, 
				aUnicode, 
				aIndicesOfUnconvertibleCharacters, 
				aArrayOfCharacterSets.Array());
	}

// -----------------------------------------------------------------------------
// ConvertToUnicode()
//The main conversion function for converting from iscii to unicode
//Loaded and called by the character conversion framework.
//To support some of the iscii characters, the input forign buffer is
//copied to an intermediate buffer and then is then modified and 
//CCnvCharactersetConverter::DoConvertToUnicode() is called with
//the modified buffer. For extensibility of iscii to other Indic languages
//it uses CnvUtilities::SState datastructure. CnvUtilities::SState is a
//Symbian defined class for modal charactersets. The escape sequence 
//is specified to ATR followed by the script selection code and the conversion
//data is specified to be the conversion for the particular script. For the time
//being only Devanagari with script selection code 0x42 is supported. If 
//any of the other script codes are used the conversion leads to unconvertible
//character i.e. 0xFFFD.
// Returns: The numbet of iscii codes it could not convert.
//
// -----------------------------------------------------------------------------
//

EXPORT_C TInt ConvertToUnicode(
         CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
         TDes16& aUnicode, 
         const TDesC8& aForeign, 
         TInt& aState, 
         TInt& aNumberOfUnconvertibleCharacters, 
         TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter )
	{
	aNumberOfUnconvertibleCharacters = 0;
	TUint aOutputConversionFlags = CCnvCharacterSetConverter::KStateDefault;
	aIndexOfFirstByteOfFirstUnconvertibleCharacter=-1;
	TUint internalInputConversionFlags = 0;
	TInt numberOfForeignBytesConsumed=0;
	TPtrC8 remainderOfForeign(aForeign);
	TInt returnValue;
	TBool flag = EFalse;
	TBool isSkipMatchSequence = EFalse;
	const SCnvConversionData* convData;
	//Set the iscii conversion data and escape sequence for Devanagari.
	TFixedArray<CnvUtilities::SState,KNumberOfIndicCharactersetsSupported> modals;
	modals[0].iConversionData = &conversionData;
	modals[0].iEscapeSequence = &KEscapeSequenceDevanagari();

	aUnicode.SetLength(0);
	
	//The internal input conversion flag for conversion is set to CCnvCharacterSetConverter::EInputConversionFlagAppend
	//so that for each conversion in the conversion loop, the generated conversion buffer is appened to the aUnicode buffer.
	internalInputConversionFlags |= CCnvCharacterSetConverter::EInputConversionFlagAppend;
	if (aForeign.Length()==0)
	{
		return 0;
	}
	//Get the conversion data from the previous call else the conversion data is set to the default 
	//conversion data, i.e. Devanagari.
	convData=(aState!=CCnvCharacterSetConverter::KStateDefault)? REINTERPRET_CAST(const SCnvConversionData*, aState): modals[0].iConversionData;
	FOREVER
	{
		TBuf8<KMaximumLengthOfIntermediateBuffer> intermediateBuffer;
		TUint numberOfForeignBytesConsumedThisTime = 0;
		if((remainderOfForeign.Length() >=KMaximumLengthOfIntermediateBuffer) && (aUnicode.MaxLength()-aUnicode.Length() >=KMaximumLengthOfIntermediateBuffer))
		{
			numberOfForeignBytesConsumedThisTime = KMaximumLengthOfIntermediateBuffer;
			intermediateBuffer = remainderOfForeign.Left(numberOfForeignBytesConsumedThisTime);
			//If the intermediate buffer is a part of truncated buffer sequence but the
			//actual input buffer is not truncated then truncated sequence is not converted.
			//The intermediate buffer is modified so as not to contain the truncated sequence.
			
			flag = IsTruncatedDoubleByteIsciiSequence(intermediateBuffer);
			if(flag)
				{
					numberOfForeignBytesConsumedThisTime --;
					intermediateBuffer = remainderOfForeign.Left(numberOfForeignBytesConsumedThisTime);					
				}
			
		}
		else
		{
			flag = IsTruncatedDoubleByteIsciiSequence(remainderOfForeign.Left(aUnicode.MaxLength()-aUnicode.Length()));
			if(!flag)
				{
				numberOfForeignBytesConsumedThisTime = aUnicode.MaxLength()-aUnicode.Length();
				intermediateBuffer = remainderOfForeign.Left(numberOfForeignBytesConsumedThisTime);
				}
			else
				{
				if(aForeign.Length()>(numberOfForeignBytesConsumed+aUnicode.Length()))
					{
					numberOfForeignBytesConsumedThisTime = aUnicode.MaxLength()-aUnicode.Length()-1;
					intermediateBuffer = remainderOfForeign.Left(numberOfForeignBytesConsumedThisTime);
					break;
					}
				else
					{
					numberOfForeignBytesConsumedThisTime = aUnicode.MaxLength()-aUnicode.Length();
					intermediateBuffer = remainderOfForeign.Left(numberOfForeignBytesConsumedThisTime);
					}
				}
		}			
		
		//The input intermediate iscii buffer is modified with the search and replace
		//buffers. It is required for supporting multibyte iscii sequences.		
		FindAndModifyBuffer(intermediateBuffer);
		TPtrC8 remainderOfForeignInternal(intermediateBuffer);
		TPtrC8 homogeneousRun;
		const TInt startOfNextEscapeSequence=intermediateBuffer.Locate(KControlCharacterEscape);
		if (startOfNextEscapeSequence!=0) 
			{
			if (startOfNextEscapeSequence==KErrNotFound)
				{
				homogeneousRun.Set(remainderOfForeignInternal);
				remainderOfForeignInternal.Set(NULL, 0);
				}
			else
				{
				__ASSERT_DEBUG(startOfNextEscapeSequence>0, User::Panic(KPanicReason,EPanicBadStartOfNextEscapeSequence));
				homogeneousRun.Set(remainderOfForeignInternal.Left(startOfNextEscapeSequence));
				remainderOfForeignInternal.Set(remainderOfForeignInternal.Mid(startOfNextEscapeSequence));
				}
			isSkipMatchSequence = ETrue;
			}
		FOREVER
			{
			if(!isSkipMatchSequence)
				{
				if (!NextHomogeneousForeignRun(convData, numberOfForeignBytesConsumed, homogeneousRun, 
											remainderOfForeignInternal, modals.Array(), aOutputConversionFlags))
					{
					break;
					}
				}
			HandleHomogeneousRun( convData, aDefaultEndiannessOfForeignCharacters, aUnicode, homogeneousRun, aNumberOfUnconvertibleCharacters, 
								aIndexOfFirstByteOfFirstUnconvertibleCharacter,aOutputConversionFlags,internalInputConversionFlags,
								numberOfForeignBytesConsumed,returnValue);
			if(returnValue<0)
				{
				return returnValue;
				}
			isSkipMatchSequence = EFalse;
			}
		if(returnValue > 0)
			break;
		if ((!flag && (numberOfForeignBytesConsumedThisTime != KMaximumLengthOfIntermediateBuffer)) || (flag && (numberOfForeignBytesConsumedThisTime != (KMaximumLengthOfIntermediateBuffer-1) )))
			break;
		remainderOfForeign.Set(aForeign.Mid(numberOfForeignBytesConsumed));				
	}
	//If the number of iscii bytes consumed by the conversion is zero also the output conversion
	//flag is not set to EOutputConversionFlagInputIsTruncated, then return EErrorIllFormedInput.
	if ((numberOfForeignBytesConsumed==0) && (aOutputConversionFlags&CCnvCharacterSetConverter::EOutputConversionFlagInputIsTruncated))
		{
		return CCnvCharacterSetConverter::EErrorIllFormedInput;
		}
	//Set the conversion data sothat next time when ConvertToUnicode() is called,
	//will use the previous conversion data.
	aState=REINTERPRET_CAST(TInt, convData);
	return aForeign.Length()-numberOfForeignBytesConsumed;
    }

// -----------------------------------------------------------------------------
// IsInThisCharacterSetL()
//The method tells how probable it is that a sample piece of text is encoded in this character set.
//On return aConfidenceLevel, indicates how confident the function is about its return value. For
//iscii it is the default implementation and it does not implement the autodetect functionality.
//Loaded and called by the character conversion framework.
//
// Returns: EFalse: To tell CCnvCharacterSetConverter::AutoDetectCharacterSetL()  that the plug-in DLL
//                          is not implementing a function of this signature and is therefore empty.
//
// -----------------------------------------------------------------------------
//


//Default implementation for IsInThisCharacterSetL()

EXPORT_C TBool IsInThisCharacterSetL(
         TBool& aSetToTrue, 
         TInt& aConfidenceLevel, 
         const TDesC8& )
	{
    	aSetToTrue = EFalse;
	aConfidenceLevel = 0;
	return EFalse;
	}

EXPORT_C void Reserved_2()
	{
	}

EXPORT_C void Reserved_3()
	{
	}

EXPORT_C void Reserved_4()
	{
	}

EXPORT_C void Reserved_5()
	{
	}

EXPORT_C void Reserved_6()
	{
	}

EXPORT_C void Reserved_7()
	{
	}

EXPORT_C void Reserved_8()
	{
	}

#endif //EKA2
