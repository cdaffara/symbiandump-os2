/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* HZ is defined in RFC 1843
*
*/


#include <e32std.h>
#include <charconv.h>
#include "gb2312.h"
#include <ecom/implementationproxy.h>
#include <charactersetconverter.h>

const TInt KIsInGbBlock=CCnvCharacterSetConverter::KStateDefault+1;
#if defined(_DEBUG)
const TInt KLengthOfIntermediateBuffer=6;
#else
const TInt KLengthOfIntermediateBuffer=150;
#endif

#if defined(_DEBUG)

_LIT(KLitPanicText, "HZ");

enum TPanic
	{
	EPanicTooManyMatchingIndicesFound=1,
	EPanicBadNumberOfBytesRequiredToBeAvailable,
	EPanicBadNumberOfBytesAvailable,
	EPanicBadNumberOfBytesThatCanBeMadeAvailable,
	EPanicBadNumberOfBytesMadeAvailable1,
	EPanicBadNumberOfBytesMadeAvailable2,
	EPanicBadDescriptorSubDivision1,
	EPanicBadDescriptorSubDivision2,
	EPanicBadDescriptorSubDivision3,
	EPanicBadDescriptorSubDivision4,
	EPanicBadPointers1,
	EPanicBadPointers2,
	EPanicBadPointers3,
	EPanicBadPointers4,
	EPanicBadPointers5,
	EPanicBadPointers6,
	EPanicBadPointers7,
	EPanicBadPointers8,
	EPanicBadPointers9,
	EPanicBadPointers10,
	EPanicBadPointers11,
	EPanicBadPointers12,
	EPanicStillInGbBlock,
	EPanicBadState,
	EPanicSplitBoundaryIsNotAsLateAsPossible1,
	EPanicSplitBoundaryIsNotAsLateAsPossible2,
	EPanicBadGb2312Index,
	EPanicBadHzIndex,
	EPanicBadTildeSequence,
	EPanicBadReturnValue1,
	EPanicBadReturnValue2,
	EPanicRemainderOfHzHasGotLonger
	};

LOCAL_C void Panic(TPanic aPanic)
	{
	User::Panic(KLitPanicText, aPanic);
	}

#endif

class CHZConverterImpl : public CCharacterSetConverterPluginInterface
	{

public:
	virtual const TDesC8& ReplacementForUnconvertibleUnicodeCharacters();

	virtual TInt ConvertFromUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
		TDes8& aForeign, 
		const TDesC16& aUnicode, 
		CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters);

	virtual TInt ConvertToUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		TDes16& aUnicode, 
		const TDesC8& aForeign, 
		TInt& aState, 
		TInt& aNumberOfUnconvertibleCharacters, 
		TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter);

	virtual TBool IsInThisCharacterSetL(
		TBool& aSetToTrue, 
		TInt& aConfidenceLevel, 
		const TDesC8& aSample);

	static CHZConverterImpl* NewL();
	virtual ~CHZConverterImpl();

private:
	CHZConverterImpl();

	};



const TDesC8& CHZConverterImpl::ReplacementForUnconvertibleUnicodeCharacters()
	{
	return CnvGb2312::ReplacementForUnconvertibleUnicodeCharacters();
	}

LOCAL_C void IncrementNumberOfUnicodeCharactersNotConverted(TInt aLengthOfUnicode, TInt& aNumberOfUnicodeCharactersNotConverted, CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters) // these seemingly haphazard order of these paramters is to match the position of the second and third parameters with the caller
	{
	++aNumberOfUnicodeCharactersNotConverted;
	const TInt indexOfUnicodeCharacterNowNotConverted=aLengthOfUnicode-aNumberOfUnicodeCharactersNotConverted;
#if defined(_DEBUG)
	TInt numberOfMatchingIndicesFound=0;
#endif
	for (TInt i=aIndicesOfUnconvertibleCharacters.NumberOfIndices()-1; i>=0; --i) // must iterate backwards as items from aIndicesOfUnconvertibleCharacters may be deleted
		{
		if (aIndicesOfUnconvertibleCharacters[i]==indexOfUnicodeCharacterNowNotConverted)
			{
			aIndicesOfUnconvertibleCharacters.Remove(i);
#if defined(_DEBUG)
			++numberOfMatchingIndicesFound;
#endif
			}
		}
	__ASSERT_DEBUG(numberOfMatchingIndicesFound<=1, Panic(EPanicTooManyMatchingIndicesFound));
	}

LOCAL_C void MakeAvailable(TInt aNumberOfBytesRequiredToBeAvailable, TInt& aNumberOfUnicodeCharactersNotConverted, CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters, TInt aLengthOfUnicode, const TUint8*& aPointerToLastUsedByte, TInt& aNumberOfBytesAvailable, TInt aNumberOfBytesThatCanBeMadeAvailable) // these seemingly haphazard order of these paramters is to match the position of the second to fourth parameters (inclusive) with the caller
// makes available as much of aNumberOfBytesRequiredToBeAvailable as it can, even if the final value (i.e. value on returning) of aNumberOfBytesAvailable<aNumberOfBytesRequiredToBeAvailable (i.e. it doesn't initially give up straight away and do nothing if aNumberOfBytesRequiredToBeAvailable>aNumberOfBytesThatCanBeMadeAvailable+aNumberOfBytesAvailable)
	{
	__ASSERT_DEBUG(aNumberOfBytesRequiredToBeAvailable>0, Panic(EPanicBadNumberOfBytesRequiredToBeAvailable));
	__ASSERT_DEBUG(aNumberOfBytesAvailable>=0, Panic(EPanicBadNumberOfBytesAvailable));
	__ASSERT_DEBUG(aNumberOfBytesThatCanBeMadeAvailable>=0, Panic(EPanicBadNumberOfBytesThatCanBeMadeAvailable));
	TInt numberOfBytesMadeAvailable=0;
	FOREVER
		{
		if (aNumberOfBytesAvailable>=aNumberOfBytesRequiredToBeAvailable)
			{
			break; // no more needs to be done
			}
		__ASSERT_DEBUG(numberOfBytesMadeAvailable<=aNumberOfBytesThatCanBeMadeAvailable, Panic(EPanicBadNumberOfBytesMadeAvailable1));
		if (numberOfBytesMadeAvailable>=aNumberOfBytesThatCanBeMadeAvailable)
			{
			break; // give up - no more can be done
			}
		const TInt numberOfBytesInCharacter=(*aPointerToLastUsedByte&0x80)? 2: 1;
		aPointerToLastUsedByte-=numberOfBytesInCharacter;
		aNumberOfBytesAvailable+=numberOfBytesInCharacter;
		numberOfBytesMadeAvailable+=numberOfBytesInCharacter;
		IncrementNumberOfUnicodeCharactersNotConverted(aLengthOfUnicode, aNumberOfUnicodeCharactersNotConverted, aIndicesOfUnconvertibleCharacters);
		}
	__ASSERT_DEBUG(numberOfBytesMadeAvailable<=aNumberOfBytesThatCanBeMadeAvailable, Panic(EPanicBadNumberOfBytesMadeAvailable2));
	}

LOCAL_C void ConvertFromGb2312ToHzInPlace(TDes8& aDescriptor, TInt& aNumberOfUnicodeCharactersNotConverted, CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters, TInt aLengthOfUnicode)
	{
	// it is legal for aDescriptor to be of length 0
	const TInt originalLengthOfDescriptor=aDescriptor.Length();
	if (originalLengthOfDescriptor>0)
		{
		TInt numberOfBytesAvailable=aDescriptor.MaxLength()-originalLengthOfDescriptor;
		TUint8* pointerToPreviousByte=CONST_CAST(TUint8*, aDescriptor.Ptr()-1);
		const TUint8* pointerToLastUsedByte=pointerToPreviousByte+originalLengthOfDescriptor;
		TBool isInGbBlock=EFalse;
		FOREVER
			{
			__ASSERT_DEBUG((pointerToLastUsedByte-(aDescriptor.Ptr()-1))+numberOfBytesAvailable==aDescriptor.MaxLength(), Panic(EPanicBadDescriptorSubDivision1));
			__ASSERT_DEBUG(pointerToPreviousByte<pointerToLastUsedByte, Panic(EPanicBadPointers1));
			const TUint currentByte=*(pointerToPreviousByte+1);
			if (currentByte&0x80)
				{
				if (!isInGbBlock)
					{
					MakeAvailable(4, aNumberOfUnicodeCharactersNotConverted, aIndicesOfUnconvertibleCharacters, aLengthOfUnicode, pointerToLastUsedByte, numberOfBytesAvailable, (pointerToLastUsedByte-pointerToPreviousByte)-2); // what's passed into the last parameter is not a typo - we do not want the two-byte character currently pointed to by (pointerToPreviousByte+1) to be made available
					if (numberOfBytesAvailable<4) // 4 bytes are required for the "~{" "~}" escape sequences (thus ensuring that at least a single double-byte character can be put into the GB-block)
						{
						break;
						}
					isInGbBlock=ETrue;
					Mem::Copy(pointerToPreviousByte+3, pointerToPreviousByte+1, pointerToLastUsedByte-pointerToPreviousByte);
					++pointerToPreviousByte;
					*pointerToPreviousByte='~';
					++pointerToPreviousByte;
					*pointerToPreviousByte='{';
					numberOfBytesAvailable-=2;
					pointerToLastUsedByte+=2;
					}
				++pointerToPreviousByte;
				*pointerToPreviousByte&=~0x80;
				__ASSERT_DEBUG(pointerToPreviousByte<pointerToLastUsedByte, Panic(EPanicBadPointers2));
				++pointerToPreviousByte;
				*pointerToPreviousByte&=~0x80;
				}
			else
				{
				if (isInGbBlock)
					{
closeGbBlock:
					isInGbBlock=EFalse;
					MakeAvailable(2, aNumberOfUnicodeCharactersNotConverted, aIndicesOfUnconvertibleCharacters, aLengthOfUnicode, pointerToLastUsedByte, numberOfBytesAvailable, pointerToLastUsedByte-pointerToPreviousByte);
					if (numberOfBytesAvailable<2) // 2 bytes are required for the "~}" escape sequence
						{
						IncrementNumberOfUnicodeCharactersNotConverted(aLengthOfUnicode, aNumberOfUnicodeCharactersNotConverted, aIndicesOfUnconvertibleCharacters);
						*(pointerToPreviousByte-1)='~';
						*pointerToPreviousByte='}';
						break;
						}
					Mem::Copy(pointerToPreviousByte+3, pointerToPreviousByte+1, pointerToLastUsedByte-pointerToPreviousByte);
					++pointerToPreviousByte;
					*pointerToPreviousByte='~';
					++pointerToPreviousByte;
					*pointerToPreviousByte='}';
					numberOfBytesAvailable-=2;
					pointerToLastUsedByte+=2;
					__ASSERT_DEBUG(pointerToPreviousByte<=pointerToLastUsedByte, Panic(EPanicBadPointers3));
					if (pointerToPreviousByte>=pointerToLastUsedByte)
						{
						break;
						}
					}
				if (currentByte=='~')
					{
					MakeAvailable(1, aNumberOfUnicodeCharactersNotConverted, aIndicesOfUnconvertibleCharacters, aLengthOfUnicode, pointerToLastUsedByte, numberOfBytesAvailable, (pointerToLastUsedByte-pointerToPreviousByte)-1); // what's passed into the last parameter is not a typo - we do not want the "~" currently pointed to by (pointerToPreviousByte+1) to be made available
					if (numberOfBytesAvailable<1) // 1 byte is required for the extra "~" character
						{
						break;
						}
					Mem::Copy(pointerToPreviousByte+2, pointerToPreviousByte+1, pointerToLastUsedByte-pointerToPreviousByte);
					++pointerToPreviousByte;
					*pointerToPreviousByte='~';
					numberOfBytesAvailable-=1;
					pointerToLastUsedByte+=1;
					}
				++pointerToPreviousByte;
				}
			__ASSERT_DEBUG(pointerToPreviousByte<=pointerToLastUsedByte, Panic(EPanicBadPointers4));
			if (pointerToPreviousByte>=pointerToLastUsedByte)
				{
				if (isInGbBlock)
					{
					goto closeGbBlock; // this is to share the code for closing the GB-block
					}
				break;
				}
			}
		__ASSERT_DEBUG(pointerToPreviousByte<=pointerToLastUsedByte, Panic(EPanicBadPointers5));
		if (pointerToPreviousByte<pointerToLastUsedByte)
			{
			__ASSERT_DEBUG((pointerToPreviousByte==pointerToLastUsedByte-1) || (pointerToPreviousByte==pointerToLastUsedByte-2), Panic(EPanicBadPointers6));
			numberOfBytesAvailable+=(pointerToLastUsedByte-pointerToPreviousByte);
			pointerToLastUsedByte=pointerToPreviousByte;
			IncrementNumberOfUnicodeCharactersNotConverted(aLengthOfUnicode, aNumberOfUnicodeCharactersNotConverted, aIndicesOfUnconvertibleCharacters);
			}
		//if it gets out from FOREVER, isInGbBlock could not be ETrue ~~~ so wouldn't need the assert
		//__ASSERT_DEBUG(!isInGbBlock, Panic(EPanicStillInGbBlock));
		aDescriptor.SetLength(aDescriptor.MaxLength()-numberOfBytesAvailable);
		__ASSERT_DEBUG(aDescriptor.Length()==pointerToLastUsedByte-(aDescriptor.Ptr()-1), Panic(EPanicBadDescriptorSubDivision2));
		}
	}

TInt CHZConverterImpl::ConvertFromUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
		TDes8& aForeign, 
		const TDesC16& aUnicode, 
		CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters)
	{
	TInt returnValue=CCnvCharacterSetConverter::DoConvertFromUnicode(CnvGb2312::ConversionData(), aDefaultEndiannessOfForeignCharacters, aReplacementForUnconvertibleUnicodeCharacters, aForeign, aUnicode, aIndicesOfUnconvertibleCharacters);
	if (returnValue<0)
		{
		return returnValue; // this is an error-code
		}
	ConvertFromGb2312ToHzInPlace(aForeign, returnValue, aIndicesOfUnconvertibleCharacters, aUnicode.Length());
	return returnValue;
	}

LOCAL_C TInt ConvertFromHzToHomogeneousGb2312(TBuf8<KLengthOfIntermediateBuffer>& aGb2312, TPtrC8& aHzBeingConsumed, TPtrC8& aRemainderOfHz, TInt& aState, TUint& aOutputConversionFlags)
	{
	// this function panics if aRemainderOfHz is of length 0
	TUint8* pointerToPreviousGb2312Byte=CONST_CAST(TUint8*, aGb2312.Ptr()-1);
	const TUint8* pointerToCurrentHzByte=aRemainderOfHz.Ptr();
	const TUint8* const pointerToLastHzByte=pointerToCurrentHzByte+(aRemainderOfHz.Length()-1);
	const TUint8* const pointerToLastHzByteToConvertThisTime=Min(pointerToLastHzByte, pointerToCurrentHzByte+(KLengthOfIntermediateBuffer-1));
	FOREVER
		{
		const TUint currentHzByte=*pointerToCurrentHzByte;
		if (currentHzByte=='~')
			{
			__ASSERT_DEBUG(pointerToCurrentHzByte<=pointerToLastHzByte, Panic(EPanicBadPointers7));
			if (pointerToCurrentHzByte>=pointerToLastHzByte)
				{
				aOutputConversionFlags|=CCnvCharacterSetConverter::EOutputConversionFlagInputIsTruncated;
				--pointerToCurrentHzByte;
				break;
				}
			++pointerToCurrentHzByte;
			const TUint nextHzByte=*pointerToCurrentHzByte;
			switch (nextHzByte)
				{
			case '{':
				if (aState==KIsInGbBlock)
					{
					return CCnvCharacterSetConverter::EErrorIllFormedInput;
					}
				aState=KIsInGbBlock;
				break;
			case '}':
				if (aState==CCnvCharacterSetConverter::KStateDefault)
					{
					return CCnvCharacterSetConverter::EErrorIllFormedInput;
					}
				aState=CCnvCharacterSetConverter::KStateDefault;
				break;
			case '~':
				++pointerToPreviousGb2312Byte;
				*pointerToPreviousGb2312Byte=STATIC_CAST(TUint8, currentHzByte);
				break;
			case 0x0a:
				break;
			default:
				return CCnvCharacterSetConverter::EErrorIllFormedInput;
				}
			}
		else
			{
			__ASSERT_DEBUG(pointerToCurrentHzByte<=pointerToLastHzByte, Panic(EPanicBadPointers8));
			if (pointerToCurrentHzByte>pointerToLastHzByteToConvertThisTime)
				{
				--pointerToCurrentHzByte;
				break;
				}
			if (aState==CCnvCharacterSetConverter::KStateDefault)
				{
				++pointerToPreviousGb2312Byte;
				*pointerToPreviousGb2312Byte=STATIC_CAST(TUint8, currentHzByte);
				}
			else
				{
				__ASSERT_DEBUG(aState==KIsInGbBlock, Panic(EPanicBadState));
				__ASSERT_DEBUG(pointerToCurrentHzByte<=pointerToLastHzByteToConvertThisTime, Panic(EPanicBadPointers9));
				if (pointerToCurrentHzByte>=pointerToLastHzByteToConvertThisTime)
					{
					aOutputConversionFlags|=CCnvCharacterSetConverter::EOutputConversionFlagInputIsTruncated;
					--pointerToCurrentHzByte;
					break;
					}
				++pointerToCurrentHzByte;
				++pointerToPreviousGb2312Byte;
				*pointerToPreviousGb2312Byte=STATIC_CAST(TUint8, currentHzByte|0x80);
				++pointerToPreviousGb2312Byte;
				*pointerToPreviousGb2312Byte=STATIC_CAST(TUint8, *pointerToCurrentHzByte|0x80);
				}
			}
		__ASSERT_DEBUG(pointerToCurrentHzByte<=pointerToLastHzByte, Panic(EPanicBadPointers10));
		if (pointerToCurrentHzByte>=pointerToLastHzByte)
			{
			break;
			}
		++pointerToCurrentHzByte;
		}
	aGb2312.SetLength((pointerToPreviousGb2312Byte+1)-aGb2312.Ptr());
	const TInt numberOfHzBytesBeingConsumed=(pointerToCurrentHzByte+1)-aRemainderOfHz.Ptr();
	aHzBeingConsumed.Set(aRemainderOfHz.Left(numberOfHzBytesBeingConsumed));
	aRemainderOfHz.Set(aRemainderOfHz.Mid(numberOfHzBytesBeingConsumed));
#if defined(_DEBUG)
	// AAA: check that if the split occurs on a boundary between some one-byte and some two-byte text, then aState corresponds to the state *after* the split (the code marked "BBB" relies on this)
	if (aRemainderOfHz.Length()>=2)
		{
		__ASSERT_DEBUG(aRemainderOfHz.Left(2)!=_L8("~{"), Panic(EPanicSplitBoundaryIsNotAsLateAsPossible1));
		__ASSERT_DEBUG(aRemainderOfHz.Left(2)!=_L8("~}"), Panic(EPanicSplitBoundaryIsNotAsLateAsPossible2));
		}
#endif
	return 0;
	}

LOCAL_C TInt Gb2312IndexToHzIndex(const TDesC8& aHz, TInt aGb2312Index, TBool aReturnMaximalHzIndex)
	{
	// this function panics if aHz is of length 0
	// aHz may start in either KIsInGbBlock or CCnvCharacterSetConverter::KStateDefault state, but it must *not* have any truncated sequences (i.e. "tilde <something>" sequence that is not complete, or part of a 2-byte character sequence) at either its start or its end
	__ASSERT_DEBUG(aGb2312Index>=0, Panic(EPanicBadGb2312Index));
	TInt hzIndex=0;
	TInt offsetFromGb2312IndexToHzIndex=0;
	const TUint8* const pointerToFirstHzByte=aHz.Ptr();
	const TUint8* pointerToCurrentHzByte=pointerToFirstHzByte;
	const TUint8* const pointerToLastHzByte=pointerToFirstHzByte+(aHz.Length()-1);
	FOREVER
		{
		const TInt newHzIndex=pointerToCurrentHzByte-pointerToFirstHzByte;
		const TInt candidateHzIndex=aGb2312Index+offsetFromGb2312IndexToHzIndex;
		__ASSERT_DEBUG(hzIndex<=candidateHzIndex, Panic(EPanicBadHzIndex));
		if (aReturnMaximalHzIndex? (newHzIndex>candidateHzIndex): (hzIndex>=candidateHzIndex))
			{
			break;
			}
		hzIndex=newHzIndex;
		if (*pointerToCurrentHzByte=='~')
			{
			__ASSERT_DEBUG(pointerToCurrentHzByte<=pointerToLastHzByte, Panic(EPanicBadPointers11));
			if (pointerToCurrentHzByte>=pointerToLastHzByte)
				{
				break;
				}
			++pointerToCurrentHzByte;
			const TUint currentHzByte=*pointerToCurrentHzByte;
			if (currentHzByte=='~')
				{
				++offsetFromGb2312IndexToHzIndex;
				}
			else
				{
				__ASSERT_DEBUG((currentHzByte=='{') || (currentHzByte=='}') || (currentHzByte==0x0a), Panic(EPanicBadTildeSequence));
				offsetFromGb2312IndexToHzIndex+=2;
				}
			}
		__ASSERT_DEBUG(pointerToCurrentHzByte<=pointerToLastHzByte, Panic(EPanicBadPointers12));
		if (pointerToCurrentHzByte>=pointerToLastHzByte)
			{
			break;
			}
		++pointerToCurrentHzByte;
		}
	return hzIndex;
	}

TInt CHZConverterImpl::ConvertToUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		TDes16& aUnicode, 
		const TDesC8& aForeign, 
		TInt& aState, 
		TInt& aNumberOfUnconvertibleCharacters, 
		TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
	{
	aUnicode.SetLength(0);
	TPtrC8 remainderOfHz(aForeign);
	TInt numberOfHzBytesConsumed=0;
	TUint outputConversionFlags=0;
	TUint inputConversionFlags=CCnvCharacterSetConverter::EInputConversionFlagAppend;
	const SCnvConversionData& gb2312ConversionData=CnvGb2312::ConversionData();
	FOREVER
		{
		__ASSERT_DEBUG(numberOfHzBytesConsumed+remainderOfHz.Length()==aForeign.Length(), Panic(EPanicBadDescriptorSubDivision3));
#if defined(_DEBUG)
		const TInt oldLengthOfRemainderOfHz=remainderOfHz.Length();
#endif
		TBuf8<KLengthOfIntermediateBuffer> gb2312;
		TPtrC8 hzBeingConsumed;
		const TInt returnValue1=ConvertFromHzToHomogeneousGb2312(gb2312, hzBeingConsumed, remainderOfHz, aState, outputConversionFlags);
		if (returnValue1<0)
			{
			return returnValue1; // this is an error-code
			}
		__ASSERT_DEBUG(returnValue1==0, Panic(EPanicBadReturnValue1));
		__ASSERT_DEBUG(hzBeingConsumed.Length()+remainderOfHz.Length()==oldLengthOfRemainderOfHz, Panic(EPanicRemainderOfHzHasGotLonger));
		if (hzBeingConsumed.Length()==0)
			{
			break;
			}
		TInt numberOfUnconvertibleCharacters;
		TInt indexOfFirstByteOfFirstUnconvertibleCharacter;
		const TInt returnValue2=CCnvCharacterSetConverter::DoConvertToUnicode(gb2312ConversionData, aDefaultEndiannessOfForeignCharacters, aUnicode, gb2312, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter, outputConversionFlags, inputConversionFlags);
		if (returnValue2<0)
			{
			return returnValue2; // this is an error-code
			}
		if (numberOfUnconvertibleCharacters>0)
			{
			if (aNumberOfUnconvertibleCharacters==0)
				{
				aIndexOfFirstByteOfFirstUnconvertibleCharacter=numberOfHzBytesConsumed+Gb2312IndexToHzIndex(hzBeingConsumed, indexOfFirstByteOfFirstUnconvertibleCharacter, EFalse);
				}
			aNumberOfUnconvertibleCharacters+=numberOfUnconvertibleCharacters;
			}
		if (returnValue2>0)
			{
			const TInt numberOfGb2312BytesConverted=gb2312.Length()-returnValue2;
			__ASSERT_DEBUG(numberOfGb2312BytesConverted>=0, Panic(EPanicBadReturnValue2));
			// don't call gb2312.SetLength(numberOfGb2312BytesConverted) as we want to access gb2312[numberOfGb2312BytesConverted] - in any case, gb2312's length is never going to be used again
			// don't bother re-setting remainderOfHz as it won't be used again
			numberOfHzBytesConsumed+=Gb2312IndexToHzIndex(hzBeingConsumed, numberOfGb2312BytesConverted, ETrue);
			aState=(gb2312[numberOfGb2312BytesConverted]&0x80)? KIsInGbBlock: CCnvCharacterSetConverter::KStateDefault; // BBB: if the split (between the text that was converted and the text that wasn't converted) occurs on a boundary between some one-byte and some two-byte text, then aState corresponds to the state *after* the split (the code marked "AAA" checks this) - this means that we set aState according to gb2312[numberOfGb2312BytesConverted] rather than gb2312[numberOfGb2312BytesConverted-1]
			break;
			}
		numberOfHzBytesConsumed+=hzBeingConsumed.Length();
		remainderOfHz.Set(aForeign.Mid(numberOfHzBytesConsumed));
		__ASSERT_DEBUG(numberOfHzBytesConsumed+remainderOfHz.Length()==aForeign.Length(), Panic(EPanicBadDescriptorSubDivision4));
		if (remainderOfHz.Length()==0)
			{
			break;
			}
		if (numberOfHzBytesConsumed>0)
			{
			inputConversionFlags|=CCnvCharacterSetConverter::EInputConversionFlagAllowTruncatedInputNotEvenPartlyConsumable;
			}
		}
	// N.B. remainderOfHz is in an undefined state by this point
	if ((numberOfHzBytesConsumed==0) && (outputConversionFlags&CCnvCharacterSetConverter::EOutputConversionFlagInputIsTruncated))
		{
		return CCnvCharacterSetConverter::EErrorIllFormedInput;
		}
	return aForeign.Length()-numberOfHzBytesConsumed;
	}

TBool CHZConverterImpl::IsInThisCharacterSetL(
		TBool& aSetToTrue, 
		TInt& aConfidenceLevel, 
		const TDesC8& aSample)
	{
	aSetToTrue=ETrue;
	TInt sampleLength = aSample.Length();
	TInt pairOfTilde=0;
	TInt occrenceOfNonHz=0;
	aConfidenceLevel = 50;
	// Hz encoding uses escape sequences...
	for (TInt i = 0; i < sampleLength; ++i)
		{
		if (aSample[i]>0x7e)
			occrenceOfNonHz++;
		if (aSample[i]==0x7e)
			{
			TInt increment1 = i+1;
			if (increment1 >= sampleLength)
				break;
			if ((aSample[increment1] == 0x7b)||(aSample[increment1] == 0x7d)||(aSample[increment1] == 0x7e))
				{
				pairOfTilde++;
				i++;
				}
			}
		}//for
	if (sampleLength)
		{
		TInt occurrenceOftilde =2*pairOfTilde*100/sampleLength;
		aConfidenceLevel=aConfidenceLevel-Max(0,(4-occurrenceOftilde));
		aConfidenceLevel += occurrenceOftilde;
		aConfidenceLevel -= ((occrenceOfNonHz*100)/sampleLength);
		}
	return ETrue;
	}

CHZConverterImpl* CHZConverterImpl::NewL()
	{
	CHZConverterImpl* self = new(ELeave) CHZConverterImpl();
	return self;
	}

CHZConverterImpl::~CHZConverterImpl()
	{
	}

CHZConverterImpl::CHZConverterImpl()
	{
	}

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10006065,	CHZConverterImpl::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
