/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <e32base.h>
#include <charconv.h>
#include <convutils.h>
                   
const TInt KNoPreviousCharacterSet=-1;
const TInt KDefaultCharacterSet = 0;
const TUint KControlCharacterEscape=0x1b;

#if defined(_DEBUG)
//It will cause performance problem with small KMaximumLengthOfIntermediateBuffer.
//Please use release version to test performance cases.
const TInt KMaximumLengthOfIntermediateBuffer=5;
#else
const TInt KMaximumLengthOfIntermediateBuffer=150;
#endif

struct SCnvConversionData;

_LIT(KLitPanicText, "CONVUTILS");

enum TPanic
	{
	EPanicBadInputConversionFlags1=1,
	EPanicBadInputConversionFlags2,
	EPanicBadInputConversionFlags3,
	EPanicBadNumberOfUnicodeElementsConsumed,
	EPanicAppendFlagViolated,
	EPanicBadNumberOfUnicodeCharactersConverted,
	EPanicBadNumberOfCharactersThatDroppedOut,
	EPanicLoopCounterOverRun1,
	EPanicLoopCounterOverRun2,
	EPanicDescriptorNotWholeNumberOfCharacters1,
	EPanicDescriptorNotWholeNumberOfCharacters2,
	EPanicDescriptorNotWholeNumberOfCharacters3,
	EPanicDescriptorNotWholeNumberOfCharacters4,
	EPanicBadStartOfNextEscapeSequence,
	EPanicInconsistentNumberOfForeignBytesRemaining,
	EPanicBadLengthOfRunToConvert1,
	EPanicBadLengthOfRunToConvert2,
	EPanicBadMethodPointer,
	EPanicBadMethodData1,
	EPanicBadMethodData2,
	EPanicBadMethodData3,
	EPanicBadMethodData4,
	EPanicBadNumberOfCharacterSets,
	EPanicBadConversionDataPointer1,
	EPanicBadConversionDataPointer2,
	EPanicBadConversionDataPointer3,
	EPanicBadFunctionPointer1,
	EPanicBadFunctionPointer2,
	EPanicBadFunctionPointer3,
	EPanicBadEscapeSequencePointer1,
	EPanicBadEscapeSequencePointer2,
	EPanicBadNumberOfStates,
	EPanicBadEscapeSequenceStart,
	EPanicBadNumberOfMethods,
	EPanicBadSurrogatePair1,
	EPanicBadSurrogatePair2,
	EPanicBadRemainderOfForeign,
	EPanicOutputDescriptorTooShortEvenToHoldEscapeSequenceToDefaultCharacterSet
	};

LOCAL_C void Panic(TPanic aPanic)
	{
	User::Panic(KLitPanicText, aPanic);
	}

/** Converts Unicode text into a complex foreign character set encoding. This 
is an encoding which cannot be converted simply by calling 
CCnvCharacterSetConverter::DoConvertFromUnicode(). It may be modal (e.g. JIS) 
or non-modal (e.g. Shift-JIS).

The Unicode text specified in aUnicode is converted using the array of 
conversion data objects (aArrayOfCharacterSets) provided by the plug-in for 
the complex character set encoding, and the converted text is returned in 
aForeign. Any existing contents in aForeign are overwritten.

Unlike CCnvCharacterSetConverter::DoConvertFromUnicode(), multiple character 
sets can be specified. aUnicode is converted using the first character conversion 
data object in the array. When a character is found which cannot be converted 
using that data, each character set in the array is tried in turn. If it cannot 
be converted using any object in the array, the index of the character is 
appended to aIndicesOfUnconvertibleCharacters and the character is replaced 
by aReplacementForUnconvertibleUnicodeCharacters.

If it can be converted using another object in the array, that object is used 
to convert all subsequent characters until another unconvertible character 
is found.

@param aDefaultEndiannessOfForeignCharacters The default endian-ness to use 
when writing the characters in the foreign character set. If an endian-ness 
for foreign characters is specified in the current conversion data object, 
then that is used instead and the value of 
aDefaultEndiannessOfForeignCharacters is ignored. 
@param aReplacementForUnconvertibleUnicodeCharacters The single character (one 
or more byte values) which is used to replace unconvertible characters. 
@param aForeign On return, contains the converted text in the non-Unicode 
character set. 
@param aUnicode The source Unicode text to be converted. 
@param aIndicesOfUnconvertibleCharacters On return, holds an ascending array 
of the indices of each Unicode character in the source text which could not 
be converted (because none of the target character sets have an equivalent 
character). 
@param aArrayOfCharacterSets Array of character conversion data objects, 
representing the character sets which comprise a complex character set 
encoding. These are used in sequence to convert the Unicode text. There must 
be at least one character set in this array and no character set may have any 
NULL member data, or a panic occurs. 
@return The number of unconverted characters left at the end of the input 
descriptor (e.g. because aForeign was not long enough to hold all the text), 
or a negative error value, as defined in CCnvCharacterSetConverter::TError. */
EXPORT_C TInt CnvUtilities::ConvertFromUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
		TDes8& aForeign, 
		const TDesC16& aUnicode, 
		CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters, 
		const TArray<SCharacterSet>& aArrayOfCharacterSets)
	{
	TUint notUsed;
	return ConvertFromUnicode(aDefaultEndiannessOfForeignCharacters, 
								aReplacementForUnconvertibleUnicodeCharacters, 
								aForeign, 
								aUnicode, 
								aIndicesOfUnconvertibleCharacters, 
								aArrayOfCharacterSets, 
								notUsed, 
								0);
	}

/** Converts Unicode text into a complex foreign character set encoding. This is 
an encoding which cannot be converted simply by a call to 
CCnvCharacterSetConverter::DoConvertFromUnicode(). It may be modal (e.g. JIS) 
or non-modal (e.g. Shift-JIS).

The Unicode text specified in aUnicode is converted using the array of conversion 
data objects (aArrayOfCharacterSets) provided by the plug-in for the complex 
character set encoding and the converted text is returned in aForeign. The 
function can either append to aForeign or overwrite its contents (if any).

Unlike CCnvCharacterSetConverter::DoConvertFromUnicode(), multiple character 
sets can be specified. aUnicode is converted using the first character conversion 
data object in the array. When a character is found which cannot be converted 
using that data, each character set in the array is tried in turn. If it cannot 
be converted using any object in the array, the index of the character is 
appended to aIndicesOfUnconvertibleCharacters and the character is replaced 
by aReplacementForUnconvertibleUnicodeCharacters.

If it can be converted using another object in the array, that object is used 
to convert all subsequent characters until another unconvertible character 
is found.

@param aDefaultEndiannessOfForeignCharacters The default endian-ness to use 
when writing the characters in the foreign character set. If an endian-ness 
for foreign characters is specified in the current conversion data object, 
then that is used instead and the value of 
aDefaultEndiannessOfForeignCharacters is ignored. 
@param aReplacementForUnconvertibleUnicodeCharacters The single character (one 
or more byte values) which is used to replace unconvertible characters. 
@param aForeign On return, contains the converted text in the non-Unicode 
character set. This may already contain some text. If it does, and if 
aInputConversionFlags specifies EInputConversionFlagAppend, then the converted 
text is appended to this descriptor. 
@param aUnicode The source Unicode text to be converted. 
@param aIndicesOfUnconvertibleCharacters On return, holds an ascending array 
of the indices of each Unicode character in the source text which could not 
be converted (because none of the target character sets have an equivalent 
character). 
@param aArrayOfCharacterSets Array of character set data objects. These are 
used in sequence to convert the Unicode text. There must be at least one 
character set in this array and no character set may have any NULL member 
data, or a panic occurs. 
@param aOutputConversionFlags If the input descriptor ended in a truncated 
sequence, e.g. the first half only of a Unicode surrogate pair, this returns 
with the EOutputConversionFlagInputIsTruncated flag set. 
@param aInputConversionFlags Specify 
CCnvCharacterSetConverter::EInputConversionFlagAppend to append the text to 
aForeign. Specify CCnvCharacterSetConverter::EInputConversionFlagAllowTruncatedInputNotEvenPartlyConsumable 
to prevent the function from returning the error-code EErrorIllFormedInput 
when the input descriptor consists of nothing but a truncated sequence. The 
CCnvCharacterSetConverter::EInputConversionFlagStopAtFirstUnconvertibleCharacter 
flag must not be set, otherwise a panic occurs. 
@return The number of unconverted characters left at the end of the input descriptor 
(e.g. because aForeign was not long enough to hold all the text), or a negative 
error value, as defined in CCnvCharacterSetConverter::TError. */
EXPORT_C TInt CnvUtilities::ConvertFromUnicode(
				CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
				const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
				TDes8& aForeign, 
				const TDesC16& aUnicode, 
				CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters, 
				const TArray<SCharacterSet>& aArrayOfCharacterSets, 
				TUint& aOutputConversionFlags, 
				TUint aInputConversionFlags)
	{
	__ASSERT_ALWAYS(~aInputConversionFlags&CCnvCharacterSetConverter::EInputConversionFlagStopAtFirstUnconvertibleCharacter, Panic(EPanicBadInputConversionFlags1));
	CheckArrayOfCharacterSets(aArrayOfCharacterSets);
	aOutputConversionFlags=0;
	TUint internalInputConversionFlags=aInputConversionFlags;
	if (~aInputConversionFlags&CCnvCharacterSetConverter::EInputConversionFlagAppend)
		{
		aForeign.SetLength(0);
		internalInputConversionFlags|=CCnvCharacterSetConverter::EInputConversionFlagAppend;
		}
	if (aUnicode.Length()==0)
		{
		return 0;
		}
	if (aForeign.MaxLength()==aForeign.Length()) // relies on the fact that aForeign's length has been set to zero if aInputConversionFlags does not have CCnvCharacterSetConverter::EInputConversionFlagAppend set
		{
		return aUnicode.Length();
		}
	TDes8* foreign=&aForeign;
	TPtr8 dummyForeign(NULL, 0, 0);
	if (aInputConversionFlags&CCnvCharacterSetConverter::EInputConversionFlagMustEndInDefaultCharacterSet)
		{
		TInt dummyMaximumLength =
			aForeign.MaxLength() - aArrayOfCharacterSets[KDefaultCharacterSet].iEscapeSequence->Length();
		__ASSERT_ALWAYS(dummyMaximumLength >= 0, 
			Panic(EPanicOutputDescriptorTooShortEvenToHoldEscapeSequenceToDefaultCharacterSet));
		dummyForeign.Set(const_cast <TUint8*> (aForeign.Ptr()),
						 aForeign.Length(), 
						 dummyMaximumLength);
		foreign=&dummyForeign;
		}
	const TInt numberOfCharacterSets=aArrayOfCharacterSets.Count();
	TInt numberOfUnicodeElementsConsumed=0;
	internalInputConversionFlags|=CCnvCharacterSetConverter::EInputConversionFlagStopAtFirstUnconvertibleCharacter; // this is not just an optimization - it ensures that "foreign" doesn't get filled up too much each time CCnvCharacterSetConverter::DoConvertFromUnicode is called
    TInt previousCharacterSet = aInputConversionFlags&CCnvCharacterSetConverter::EInputConversionFlagAssumeStartInDefaultCharacterSet?
        KDefaultCharacterSet : KNoPreviousCharacterSet;
	FOREVER
		{
		for (TInt presentCharacterSet=KDefaultCharacterSet;;)
			{
			__ASSERT_DEBUG(numberOfUnicodeElementsConsumed<=aUnicode.Length(), Panic(EPanicBadNumberOfUnicodeElementsConsumed));
			if (numberOfUnicodeElementsConsumed>=aUnicode.Length())
				{
				goto end;
				}
			const SCharacterSet& characterSet=aArrayOfCharacterSets[presentCharacterSet];
			const TInt oldNumberOfBytesInForeign=foreign->Length();
			if (numberOfUnicodeElementsConsumed>0)
				{
				internalInputConversionFlags|=CCnvCharacterSetConverter::EInputConversionFlagAllowTruncatedInputNotEvenPartlyConsumable;
				}
			CCnvCharacterSetConverter::TArrayOfAscendingIndices indicesOfUnconvertibleCharacters;
			const TInt returnValue=CCnvCharacterSetConverter::DoConvertFromUnicode(*characterSet.iConversionData, aDefaultEndiannessOfForeignCharacters, KNullDesC8, *foreign, aUnicode.Mid(numberOfUnicodeElementsConsumed), indicesOfUnconvertibleCharacters, aOutputConversionFlags, internalInputConversionFlags);
			if (returnValue<0)
				{
				return returnValue; // this is an error-code
				}
			__ASSERT_DEBUG(foreign->Length()>=oldNumberOfBytesInForeign, Panic(EPanicAppendFlagViolated));
			TInt indexOfFirstUnconvertibleCharacter;
			if (indicesOfUnconvertibleCharacters.NumberOfIndices()==0)
				{
				indexOfFirstUnconvertibleCharacter=-1;
				numberOfUnicodeElementsConsumed=aUnicode.Length()-returnValue;
				}
			else
				{
				indexOfFirstUnconvertibleCharacter=indicesOfUnconvertibleCharacters[0];
				numberOfUnicodeElementsConsumed+=indexOfFirstUnconvertibleCharacter;
				__ASSERT_DEBUG(numberOfUnicodeElementsConsumed+LengthOfUnicodeCharacter(aUnicode, numberOfUnicodeElementsConsumed)==aUnicode.Length()-returnValue, Panic(EPanicBadNumberOfUnicodeCharactersConverted));
				}
			if (indexOfFirstUnconvertibleCharacter!=0) // if at least one Unicode character at the start of CCnvCharacterSetConverter::DoConvertFromUnicode's input descriptor was convertible...
				{
				TBool gotoEnd = EFalse;
				if (foreign->Length()>oldNumberOfBytesInForeign)
					{
					TInt numberOfCharactersThatDroppedOut=0;
                     // Insert an escape sequence if this character set is different from the last one.
                     if (presentCharacterSet  != previousCharacterSet)
                        {
                        // Insert escape sequence (if requred) in front of the last encoded run of text.
                        // Note that this may cause some characters to drop out at the end.
                        (*characterSet.iConvertFromIntermediateBufferInPlace)(oldNumberOfBytesInForeign, *foreign, numberOfCharactersThatDroppedOut);
                        if (oldNumberOfBytesInForeign < foreign->Length())
                			previousCharacterSet = presentCharacterSet;
                        }
					numberOfUnicodeElementsConsumed-=numberOfCharactersThatDroppedOut;
					if (numberOfCharactersThatDroppedOut>0 )// if "foreign" has been filled to as much as it will hold...
						{
						gotoEnd = ETrue;
						}
					}
				if (indexOfFirstUnconvertibleCharacter<0) // if we've successfully converted up to the end of aUnicode (using *characterSet.iConversionData)...
					{
					gotoEnd = ETrue;
					}
				if (gotoEnd)
					{
					if ( aInputConversionFlags&CCnvCharacterSetConverter::EInputConversionFlagMustEndInDefaultCharacterSet
					    && previousCharacterSet != KDefaultCharacterSet
					    && previousCharacterSet != KNoPreviousCharacterSet)
					    {
					    aForeign.SetLength(foreign->Length());
    				    aForeign.Append(*aArrayOfCharacterSets[KDefaultCharacterSet].iEscapeSequence);
    				    foreign=NULL;
					    }
					goto end;
					}
				break;
				}
			__ASSERT_DEBUG(presentCharacterSet<numberOfCharacterSets, Panic(EPanicLoopCounterOverRun1));
			++presentCharacterSet;
			if (presentCharacterSet>=numberOfCharacterSets)
				{
				if ((foreign->MaxLength()-foreign->Length()<aReplacementForUnconvertibleUnicodeCharacters.Length()) ||
					(aIndicesOfUnconvertibleCharacters.AppendIndex(numberOfUnicodeElementsConsumed)!=CCnvCharacterSetConverter::TArrayOfAscendingIndices::EAppendSuccessful)) // the tests must be done in this order as AppendIndex must only be called if there is room for aReplacementForUnconvertibleUnicodeCharacters
					{
					goto end;
					}
				numberOfUnicodeElementsConsumed+=LengthOfUnicodeCharacter(aUnicode, numberOfUnicodeElementsConsumed);
				foreign->Append(aReplacementForUnconvertibleUnicodeCharacters);
				break;
				}
			}
		}
end:
	if (foreign!=NULL)
		{
		aForeign.SetLength(foreign->Length());
		foreign=NULL;
		}
	if ((numberOfUnicodeElementsConsumed==0) && (aOutputConversionFlags&CCnvCharacterSetConverter::EOutputConversionFlagInputIsTruncated) && (~aInputConversionFlags&CCnvCharacterSetConverter::EInputConversionFlagAllowTruncatedInputNotEvenPartlyConsumable))
		{
		return CCnvCharacterSetConverter::EErrorIllFormedInput;
		}
	return aUnicode.Length()-numberOfUnicodeElementsConsumed;
	}


/** Inserts an escape sequence into the descriptor.

This function is provided to help in the implementation of
ConvertFromUnicode() for modal character set encodings.
Each SCharacterSet object in the array passed to
ConvertFromUnicode() must have its
iConvertFromIntermediateBufferInPlace member assigned. To
do this for a modal character set encoding, implement a function whose
signature matches that of FConvertFromIntermediateBufferInPlace 
and which calls this function, passing all arguments unchanged, and 
specifying the character set's escape sequence and the number of bytes per 
character.

@param aStartPositionInDescriptor The byte position in aDescriptor at which 
the escape sequence is inserted. If the character set uses more than one byte 
per character, this position must be the start of a character, otherwise a 
panic occurs. 
@param aDescriptor The descriptor into which the escape sequence is inserted. 
@param aNumberOfCharactersThatDroppedOut The escape sequence is inserted into 
the start of aDescriptor and any characters that need to drop out to make 
room for the escape sequence (because the descriptor's maximum length was 
not long enough) drop out from the end of the buffer. This parameter indicates 
the number of characters that needed to drop out.
@param aEscapeSequence The escape sequence for the character set. 
@param aNumberOfBytesPerCharacter The number of bytes per character. */
EXPORT_C void CnvUtilities::ConvertFromIntermediateBufferInPlace(
					TInt aStartPositionInDescriptor, 
					TDes8& aDescriptor, 
					TInt& aNumberOfCharactersThatDroppedOut, 
					const TDesC8& aEscapeSequence, 
					TInt aNumberOfBytesPerCharacter)
	{
	const TInt lengthOfDescriptor=aDescriptor.Length();
	__ASSERT_ALWAYS((lengthOfDescriptor-aStartPositionInDescriptor)%aNumberOfBytesPerCharacter==0, Panic(EPanicDescriptorNotWholeNumberOfCharacters1));
	aNumberOfCharactersThatDroppedOut=(Max(0, aEscapeSequence.Length()-(aDescriptor.MaxLength()-lengthOfDescriptor))+(aNumberOfBytesPerCharacter-1))/aNumberOfBytesPerCharacter;
	const TInt lengthOfRunInCharacters=(lengthOfDescriptor-aStartPositionInDescriptor)/aNumberOfBytesPerCharacter;
	if (aNumberOfCharactersThatDroppedOut>=lengthOfRunInCharacters) // ">=" is correct (rather than ">") as if there's only room for the escape sequence we don't want to have it in the descriptor
		{
		aNumberOfCharactersThatDroppedOut=lengthOfRunInCharacters;
		aDescriptor.SetLength(aStartPositionInDescriptor);
		}
	else
		{
		aDescriptor.SetLength(lengthOfDescriptor-(aNumberOfCharactersThatDroppedOut*aNumberOfBytesPerCharacter));
		aDescriptor.Insert(aStartPositionInDescriptor, aEscapeSequence);
		}
	}


/**  Converts text from a modal foreign character set encoding into Unicode.

The non-Unicode text specified in aForeign is converted using 
the array of character set conversion objects (aArrayOfStates) 
provided by the plug-in, and the converted text is returned in 
aUnicode. The function can either append to aUnicode 
or overwrite its contents (if any), depending on the input conversion flags 
specified. The first element in aArrayOfStates is taken to be 
the default mode (i.e. the mode to assume by default if there is no preceding 
escape sequence).
 
@param aDefaultEndiannessOfForeignCharacters The default endian-ness of the 
foreign characters. If an endian-ness for foreign characters is specified 
in the conversion data, then that is used instead and the value of 
aDefaultEndiannessOfForeignCharacters is ignored. 
@param aUnicode On return, contains the text converted into Unicode. 
@param aForeign The non-Unicode source text to be converted. 
@param aState Used to store a modal character set encoding's current mode across 
multiple calls to ConvertToUnicode() on the same input descriptor. This argument 
should be passed the same object as passed to the plug-in's ConvertToUnicode() 
exported function.
@param aNumberOfUnconvertibleCharacters On return, contains the number of 
characters in aForeign which were not converted. Characters which cannot be 
converted are output as Unicode replacement characters (0xfffd). 
@param aIndexOfFirstByteOfFirstUnconvertibleCharacter On return, the index 
of the first byte of the first unconvertible character. For instance if the 
first character in the input descriptor (aForeign) could not be converted, 
then this parameter is set to the first byte of that character, i.e. zero. 
A negative value is returned if all the characters were converted. 
@param aArrayOfStates Array of character set conversion data objects, and their 
escape sequences ("modes"). There must be one or more modes in this array, 
none of the modes can have any NULL member data, and each mode's escape sequence 
must begin with KControlCharacterEscape (0x1b) or a panic occurs. 
@return The number of unconverted bytes left at the end of the input descriptor, 
or a negative error value, as defined in TError. */
EXPORT_C TInt CnvUtilities::ConvertToUnicodeFromModalForeign(
					CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
					TDes16& aUnicode, 
					const TDesC8& aForeign, 
					TInt& aState, 
					TInt& aNumberOfUnconvertibleCharacters, 
					TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter, 
					const TArray<SState>& aArrayOfStates)
	{
	TUint notUsed;
	return ConvertToUnicodeFromModalForeign(aDefaultEndiannessOfForeignCharacters, 
											aUnicode, 
											aForeign, 
											aState, 
											aNumberOfUnconvertibleCharacters, 
											aIndexOfFirstByteOfFirstUnconvertibleCharacter, 
											aArrayOfStates, 
											notUsed, 
											0);
	}

/** @param aDefaultEndiannessOfForeignCharacters The default endian-ness for 
the foreign characters. If an endian-ness for foreign characters is specified 
in the conversion data, then that is used instead and the value of 
aDefaultEndiannessOfForeignCharacters is ignored. 
@param aUnicode On return, contains the text converted into Unicode. 
@param aForeign The non-Unicode source text to be converted. 
@param aState Used to store a modal character set encoding's current mode 
across multiple calls to ConvertToUnicode() on the same input descriptor. This 
argument should be passed the same object as passed to the plug-in's 
ConvertToUnicode() exported function. 
@param aNumberOfUnconvertibleCharacters On return, contains the number of 
characters in aForeign which were not converted. Characters which cannot be 
converted are output as Unicode replacement characters (0xfffd). 
@param aIndexOfFirstByteOfFirstUnconvertibleCharacter On return, the index 
of the first byte of the first unconvertible character. For instance if the 
first character in the input descriptor (aForeign) could not be converted, 
then this parameter is set to the first byte of that character, i.e. zero. 
A negative value is returned if all the characters were converted. 
@param aArrayOfStates Array of character set conversion data objects, and their 
escape sequences. There must be one or more modes in this array, none of the 
modes can have any NULL member data, and each mode's escape sequence must 
begin with KControlCharacterEscape (0x1b) or a panic occurs. 
@param aOutputConversionFlags If the input descriptor ended in a truncated 
sequence, e.g. a part of a multi-byte character, aOutputConversionFlags 
returns with the EOutputConversionFlagInputIsTruncated flag set. 
@param aInputConversionFlags Specify 
CCnvCharacterSetConverter::EInputConversionFlagAppend to append the text to 
aUnicode. Specify EInputConversionFlagAllowTruncatedInputNotEvenPartlyConsumable 
to prevent the function from returning the error-code EErrorIllFormedInput 
when the input descriptor consists of nothing but a truncated sequence. The 
CCnvCharacterSetConverter::EInputConversionFlagStopAtFirstUnconvertibleCharacter 
flag must not be set, otherwise a panic occurs. 
@return The number of unconverted bytes left at the end of the input descriptor, 
or a negative error value, as defined in TError. */
EXPORT_C TInt CnvUtilities::ConvertToUnicodeFromModalForeign(
								CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
								TDes16& aUnicode, 
								const TDesC8& aForeign, 
								TInt& aState, 
								TInt& aNumberOfUnconvertibleCharacters, 
								TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter, 
								const TArray<SState>& aArrayOfStates, 
								TUint& aOutputConversionFlags, 
								TUint aInputConversionFlags)
	{
	__ASSERT_ALWAYS(~aInputConversionFlags&CCnvCharacterSetConverter::EInputConversionFlagStopAtFirstUnconvertibleCharacter, Panic(EPanicBadInputConversionFlags2));
	CheckArrayOfStates(aArrayOfStates);
	aNumberOfUnconvertibleCharacters=0;
	aIndexOfFirstByteOfFirstUnconvertibleCharacter=-1;
	aOutputConversionFlags=0;
	TUint internalInputConversionFlags=aInputConversionFlags;
	if (~aInputConversionFlags&CCnvCharacterSetConverter::EInputConversionFlagAppend)
		{
		aUnicode.SetLength(0);
		internalInputConversionFlags|=CCnvCharacterSetConverter::EInputConversionFlagAppend;
		}
	if (aForeign.Length()==0)
		{
		return 0;
		}
	if (aUnicode.MaxLength()==aUnicode.Length()) // relies on the fact that aUnicode's length has been set to zero if aInputConversionFlags does not have CCnvCharacterSetConverter::EInputConversionFlagAppend set
		{
		return aForeign.Length();
		}
	TPtrC8 remainderOfForeign(aForeign);
	TPtrC8 homogeneousRun;
	TInt numberOfForeignBytesConsumed=0;
	const SCnvConversionData* conversionData = NULL;
	const TInt startOfNextEscapeSequence=aForeign.Locate(KControlCharacterEscape);
	if (startOfNextEscapeSequence!=0) // if aForeign doesn't start with an escape sequence...
		{
		conversionData=(aState!=CCnvCharacterSetConverter::KStateDefault)? REINTERPRET_CAST(const SCnvConversionData*, aState): aArrayOfStates[0].iConversionData;
		if (startOfNextEscapeSequence==KErrNotFound)
			{
			homogeneousRun.Set(remainderOfForeign);
			remainderOfForeign.Set(NULL, 0);
			}
		else
			{
			__ASSERT_DEBUG(startOfNextEscapeSequence>0, Panic(EPanicBadStartOfNextEscapeSequence));
			homogeneousRun.Set(remainderOfForeign.Left(startOfNextEscapeSequence));
			remainderOfForeign.Set(remainderOfForeign.Mid(startOfNextEscapeSequence));
			}
		goto handleHomogeneousRun;
		}
	FOREVER
		{
		if (!NextHomogeneousForeignRun(conversionData, numberOfForeignBytesConsumed, homogeneousRun, remainderOfForeign, aArrayOfStates, aOutputConversionFlags))
			{
			goto end;
			}
handleHomogeneousRun:
		if (conversionData==NULL)
			{
			return CCnvCharacterSetConverter::EErrorIllFormedInput;
			}
		TInt numberOfUnconvertibleCharacters;
		TInt indexOfFirstByteOfFirstUnconvertibleCharacter;
		const TInt returnValue=CCnvCharacterSetConverter::DoConvertToUnicode(*conversionData, aDefaultEndiannessOfForeignCharacters, aUnicode, homogeneousRun, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter, aOutputConversionFlags, internalInputConversionFlags);
		if (returnValue<0)
			{
			return returnValue; // this is an error-code
			}
		if (numberOfUnconvertibleCharacters>0)
			{
			if (aNumberOfUnconvertibleCharacters==0)
				{
				aIndexOfFirstByteOfFirstUnconvertibleCharacter=numberOfForeignBytesConsumed+indexOfFirstByteOfFirstUnconvertibleCharacter;
				}
			aNumberOfUnconvertibleCharacters+=numberOfUnconvertibleCharacters;
			}
		numberOfForeignBytesConsumed+=homogeneousRun.Length();
		if (returnValue>0)
			{
			numberOfForeignBytesConsumed-=returnValue;
			goto end;
			}
		if (numberOfForeignBytesConsumed>0)
			{
			internalInputConversionFlags|=CCnvCharacterSetConverter::EInputConversionFlagAllowTruncatedInputNotEvenPartlyConsumable;
			}
		__ASSERT_DEBUG(remainderOfForeign==aForeign.Mid(numberOfForeignBytesConsumed), Panic(EPanicInconsistentNumberOfForeignBytesRemaining));
		}
end:
	if ((numberOfForeignBytesConsumed==0) && (aOutputConversionFlags&CCnvCharacterSetConverter::EOutputConversionFlagInputIsTruncated) && (~aInputConversionFlags&CCnvCharacterSetConverter::EInputConversionFlagAllowTruncatedInputNotEvenPartlyConsumable))
		{
		return CCnvCharacterSetConverter::EErrorIllFormedInput;
		}
	aState=REINTERPRET_CAST(TInt, conversionData);
	return aForeign.Length()-numberOfForeignBytesConsumed;
	}


/**  Converts text from a non-modal complex character set encoding (e.g. 
Shift-JIS or EUC-JP) into Unicode.The non-Unicode text specified in
aForeign is converted using the array of character set
conversion methods (aArrayOfMethods) provided by the
plug-in, and the converted text is returned in aUnicode.
Overwrites the contents, if any, of aUnicode.

@param aDefaultEndiannessOfForeignCharacters The default endian-ness of the 
foreign characters. If an endian-ness for foreign characters is specified 
in the conversion data, then that is used instead and the value of 
aDefaultEndiannessOfForeignCharacters is ignored. 
@param aUnicode On return, contains the text converted into Unicode. 
@param aForeign The non-Unicode source text to be converted. 
@param aNumberOfUnconvertibleCharacters On return, contains the number of 
characters in aForeign which were not converted. Characters which cannot be 
converted are output as Unicode replacement characters (0xfffd). 
@param aIndexOfFirstByteOfFirstUnconvertibleCharacter On return, the index 
of the first byte of the first unconvertible character. For instance if the 
first character in the input descriptor (aForeign) could not be converted, 
then this parameter is set to the first byte of that character, i.e. zero. 
A negative value is returned if all the characters were converted. 
@param aArrayOfMethods Array of conversion methods. There must be one or more 
methods in this array and none of the methods in the array can have any NULL 
member data or a panic occurs. 
@return The number of unconverted bytes left at the end of the input descriptor, 
or a negative error value, as defined in TError. */
EXPORT_C TInt CnvUtilities::ConvertToUnicodeFromHeterogeneousForeign(
					CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
					TDes16& aUnicode, 
					const TDesC8& aForeign, 
					TInt& aNumberOfUnconvertibleCharacters, 
					TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter, 
					const TArray<SMethod>& aArrayOfMethods)
	{
	TUint notUsed;
	return ConvertToUnicodeFromHeterogeneousForeign(
				aDefaultEndiannessOfForeignCharacters, 
				aUnicode, 
				aForeign, 
				aNumberOfUnconvertibleCharacters, 
				aIndexOfFirstByteOfFirstUnconvertibleCharacter, 
				aArrayOfMethods, 
				notUsed, 
				0);
	}

/** @param aDefaultEndiannessOfForeignCharacters The default endian-ness for the 
foreign characters. If an endian-ness for foreign characters is specified 
in the conversion data, then that is used instead and the value of 
aDefaultEndiannessOfForeignCharacters is ignored. 
@param aUnicode On return, contains the text converted into Unicode. 
@param aForeign The non-Unicode source text to be converted. 
@param aNumberOfUnconvertibleCharacters On return, contains the number of 
characters in aForeign which were not converted. Characters which cannot be 
converted are output as Unicode replacement characters (0xfffd). 
@param aIndexOfFirstByteOfFirstUnconvertibleCharacter On return, the index 
of the first byte of the first unconvertible character. For instance if the 
first character in the input descriptor (aForeign) could not be converted, 
then this parameter is set to the first byte of that character, i.e. zero. 
A negative value is returned if all the characters were converted. 
@param aArrayOfMethods Array of conversion methods. There must be one or more 
methods in this array and none of the methods in the array can have any NULL 
member data or a panic occurs. 
@param aOutputConversionFlags If the input descriptor ended in a truncated 
sequence, e.g. a part of a multi-byte character, aOutputConversionFlags 
returns with the EOutputConversionFlagInputIsTruncated flag set. 
@param aInputConversionFlags Specify 
CCnvCharacterSetConverter::EInputConversionFlagAppend to append the text to 
aUnicode. Specify EInputConversionFlagAllowTruncatedInputNotEvenPartlyConsumable 
to prevent the function from returning the error-code EErrorIllFormedInput 
when the input descriptor consists of nothing but a truncated sequence. The 
CCnvCharacterSetConverter::EInputConversionFlagStopAtFirstUnconvertibleCharacter 
flag must not be set, otherwise a panic occurs. 
@return The number of unconverted bytes left at the end of the input descriptor, 
or a negative error value, as defined in TError. */
EXPORT_C TInt CnvUtilities::ConvertToUnicodeFromHeterogeneousForeign(
						CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
						TDes16& aUnicode, 
						const TDesC8& aForeign, 
						TInt& aNumberOfUnconvertibleCharacters, 
						TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter, 
						const TArray<SMethod>& aArrayOfMethods, 
						TUint& aOutputConversionFlags, 
						TUint aInputConversionFlags)
	{
	__ASSERT_ALWAYS(~aInputConversionFlags&CCnvCharacterSetConverter::EInputConversionFlagStopAtFirstUnconvertibleCharacter, Panic(EPanicBadInputConversionFlags3));
	CheckArrayOfMethods(aArrayOfMethods);
	aNumberOfUnconvertibleCharacters=0;
	aIndexOfFirstByteOfFirstUnconvertibleCharacter=-1;
	aOutputConversionFlags=0;
	TUint internalInputConversionFlags=aInputConversionFlags;
	if (~aInputConversionFlags&CCnvCharacterSetConverter::EInputConversionFlagAppend)
		{
		aUnicode.SetLength(0);
		internalInputConversionFlags|=CCnvCharacterSetConverter::EInputConversionFlagAppend;
		}
	if (aForeign.Length()==0)
		{
		return 0;
		}
	if (aUnicode.MaxLength()==aUnicode.Length()) // relies on the fact that aUnicode's length has been set to zero if aInputConversionFlags does not have CCnvCharacterSetConverter::EInputConversionFlagAppend set
		{
		return aForeign.Length();
		}
	const TInt numberOfMethods=aArrayOfMethods.Count();
	TPtrC8 remainderOfForeign(aForeign);
	TInt numberOfForeignBytesConsumed=0;
	FOREVER
		{
		TInt lengthOfRunToConvert=0;
		const SMethod* method=NULL;
		for (TInt i=0;;)
			{
			method=&aArrayOfMethods[i];
			__ASSERT_DEBUG(method!=NULL, Panic(EPanicBadMethodPointer));
			lengthOfRunToConvert=(*method->iNumberOfBytesAbleToConvert)(remainderOfForeign);
			if (lengthOfRunToConvert<0)
				{
				return lengthOfRunToConvert; // this is an error-code
				}
			if (lengthOfRunToConvert>0)
				{
				break;
				}
			__ASSERT_DEBUG(i<numberOfMethods, Panic(EPanicLoopCounterOverRun2));
			++i;
			if (i>=numberOfMethods)
				{
				aOutputConversionFlags|=CCnvCharacterSetConverter::EOutputConversionFlagInputIsTruncated;
				goto end;
				}
			}
		TBuf8<KMaximumLengthOfIntermediateBuffer> intermediateBuffer;
		const TInt maximumUsableLengthOfIntermediateBuffer=ReduceToNearestMultipleOf(KMaximumLengthOfIntermediateBuffer, method->iNumberOfBytesPerCharacter);
		FOREVER
			{
			const TInt numberOfForeignBytesConsumedThisTime=Min(lengthOfRunToConvert, maximumUsableLengthOfIntermediateBuffer);
			intermediateBuffer=remainderOfForeign.Left(numberOfForeignBytesConsumedThisTime);
			__ASSERT_DEBUG((numberOfForeignBytesConsumedThisTime%method->iNumberOfBytesPerCharacter)==0, Panic(EPanicDescriptorNotWholeNumberOfCharacters2));
			(*method->iConvertToIntermediateBufferInPlace)(intermediateBuffer);
			__ASSERT_DEBUG((intermediateBuffer.Length()%method->iNumberOfCoreBytesPerCharacter)==0, Panic(EPanicDescriptorNotWholeNumberOfCharacters3));
			__ASSERT_DEBUG((intermediateBuffer.Length()/method->iNumberOfCoreBytesPerCharacter)*method->iNumberOfBytesPerCharacter==numberOfForeignBytesConsumedThisTime, Panic(EPanicBadMethodData1));
			TInt numberOfUnconvertibleCharacters;
			TInt indexOfFirstByteOfFirstUnconvertibleCharacter;
			const TInt returnValue=CCnvCharacterSetConverter::DoConvertToUnicode(*method->iConversionData, aDefaultEndiannessOfForeignCharacters, aUnicode, intermediateBuffer, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter, aOutputConversionFlags, internalInputConversionFlags);
			if (returnValue<0)
				{
				return returnValue; // this is an error-code
				}
			if (numberOfUnconvertibleCharacters>0)
				{
				if (aNumberOfUnconvertibleCharacters==0)
					{
					aIndexOfFirstByteOfFirstUnconvertibleCharacter=numberOfForeignBytesConsumed+indexOfFirstByteOfFirstUnconvertibleCharacter;
					}
				aNumberOfUnconvertibleCharacters+=numberOfUnconvertibleCharacters;
				}
			numberOfForeignBytesConsumed+=numberOfForeignBytesConsumedThisTime;
			if (returnValue>0)
				{
				__ASSERT_DEBUG((returnValue%method->iNumberOfCoreBytesPerCharacter)==0, Panic(EPanicDescriptorNotWholeNumberOfCharacters4));
				numberOfForeignBytesConsumed-=(returnValue/method->iNumberOfCoreBytesPerCharacter)*method->iNumberOfBytesPerCharacter;
				goto end;
				}
			if (numberOfForeignBytesConsumed>0)
				{
				internalInputConversionFlags|=CCnvCharacterSetConverter::EInputConversionFlagAllowTruncatedInputNotEvenPartlyConsumable;
				}
			remainderOfForeign.Set(aForeign.Mid(numberOfForeignBytesConsumed));
			lengthOfRunToConvert-=numberOfForeignBytesConsumedThisTime;
			__ASSERT_DEBUG(lengthOfRunToConvert>=0, Panic(EPanicBadLengthOfRunToConvert2));
			if (lengthOfRunToConvert<=0)
				{
				break;
				}
			}
		}
end:
	if ((numberOfForeignBytesConsumed==0) && (aOutputConversionFlags&CCnvCharacterSetConverter::EOutputConversionFlagInputIsTruncated) && (~aInputConversionFlags&CCnvCharacterSetConverter::EInputConversionFlagAllowTruncatedInputNotEvenPartlyConsumable))
		{
		return CCnvCharacterSetConverter::EErrorIllFormedInput;
		}
	return aForeign.Length()-numberOfForeignBytesConsumed;
	}

void CnvUtilities::CheckArrayOfCharacterSets(const TArray<SCharacterSet>& aArrayOfCharacterSets)
	{
	const TInt numberOfCharacterSets=aArrayOfCharacterSets.Count();
	__ASSERT_ALWAYS(numberOfCharacterSets>0, Panic(EPanicBadNumberOfCharacterSets));
	for (TInt i=0; i<numberOfCharacterSets; ++i)
		{
		const SCharacterSet& characterSet=aArrayOfCharacterSets[i];
		__ASSERT_ALWAYS(characterSet.iConversionData!=NULL, Panic(EPanicBadConversionDataPointer1));
		__ASSERT_ALWAYS(characterSet.iConvertFromIntermediateBufferInPlace!=NULL, Panic(EPanicBadFunctionPointer1));
		__ASSERT_ALWAYS(characterSet.iEscapeSequence!=NULL, Panic(EPanicBadEscapeSequencePointer1));
		}
	}

void CnvUtilities::CheckArrayOfStates(const TArray<SState>& aArrayOfStates)
	{
	const TInt numberOfStates=aArrayOfStates.Count();
	__ASSERT_ALWAYS(numberOfStates>0, Panic(EPanicBadNumberOfStates));
	for (TInt i=0; i<numberOfStates; ++i)
		{
		const SState& state=aArrayOfStates[i];
		__ASSERT_ALWAYS(state.iEscapeSequence!=NULL, Panic(EPanicBadEscapeSequencePointer2));
		__ASSERT_ALWAYS((*state.iEscapeSequence)[0]==KControlCharacterEscape, Panic(EPanicBadEscapeSequenceStart));
		__ASSERT_ALWAYS(state.iConversionData!=NULL, Panic(EPanicBadConversionDataPointer2));
		}
	}

void CnvUtilities::CheckArrayOfMethods(const TArray<SMethod>& aArrayOfMethods)
	{
	const TInt numberOfMethods=aArrayOfMethods.Count();
	__ASSERT_ALWAYS(numberOfMethods>0, Panic(EPanicBadNumberOfMethods));
	for (TInt i=0; i<numberOfMethods; ++i)
		{
		const SMethod& method=aArrayOfMethods[i];
		__ASSERT_ALWAYS(method.iNumberOfBytesAbleToConvert!=NULL, Panic(EPanicBadFunctionPointer2));
		__ASSERT_ALWAYS(method.iConvertToIntermediateBufferInPlace!=NULL, Panic(EPanicBadFunctionPointer3));
		__ASSERT_ALWAYS(method.iConversionData!=NULL, Panic(EPanicBadConversionDataPointer3));
		__ASSERT_ALWAYS(method.iNumberOfBytesPerCharacter>0, Panic(EPanicBadMethodData2));
		__ASSERT_ALWAYS(method.iNumberOfCoreBytesPerCharacter>0, Panic(EPanicBadMethodData3));
		__ASSERT_ALWAYS(method.iNumberOfCoreBytesPerCharacter<=method.iNumberOfBytesPerCharacter, Panic(EPanicBadMethodData4));
		}
	}

TInt CnvUtilities::LengthOfUnicodeCharacter(const TDesC16& aUnicode, TInt aIndex)
	{
	const TUint unicodeCharacter=aUnicode[aIndex];
	if ((unicodeCharacter>=0xd800) && (unicodeCharacter<=0xdbff)) // if the unicode character is the first half of a surrogate-pair...
		{
		__ASSERT_DEBUG(aIndex+1<aUnicode.Length(), Panic(EPanicBadSurrogatePair1));
#if defined(_DEBUG)
		const TUint secondHalfOfSurrogatePair=aUnicode[aIndex+1];
#endif
		__ASSERT_DEBUG((secondHalfOfSurrogatePair>=0xdc00) && (secondHalfOfSurrogatePair<=0xdfff), Panic(EPanicBadSurrogatePair2)); // this can be asserted as CCnvCharacterSetConverter::DoConvertFromUnicode should have returned an error value if this was a bad surrogate pair
		return 2;
		}
	return 1;
	}

TBool CnvUtilities::NextHomogeneousForeignRun(const SCnvConversionData*& aConversionData, TInt& aNumberOfForeignBytesConsumed, TPtrC8& aHomogeneousRun, TPtrC8& aRemainderOfForeign, const TArray<SState>& aArrayOfStates, TUint& aOutputConversionFlags)
	{
	__ASSERT_DEBUG((aRemainderOfForeign.Length()==0) || (aRemainderOfForeign[0]==KControlCharacterEscape), Panic(EPanicBadRemainderOfForeign));
	FOREVER
		{
		if (aRemainderOfForeign.Length()==0)
			{
			return EFalse;
			}
		const TInt numberOfStates=aArrayOfStates.Count();
		TInt i;
		for (i=0; i<numberOfStates; ++i)
			{
			const SState& state=aArrayOfStates[i];
			if (MatchesEscapeSequence(aNumberOfForeignBytesConsumed, aHomogeneousRun, aRemainderOfForeign, *state.iEscapeSequence))
				{
				aConversionData=state.iConversionData;
				goto foundState;
				}
			}
		for (i=0; i<numberOfStates; ++i)
			{
			if (IsStartOf(aRemainderOfForeign, *aArrayOfStates[i].iEscapeSequence))
				{
				// aRemainderOfForeign ends with a truncated escape sequence, so ConvertToUnicode cannot convert any more
				aOutputConversionFlags|=CCnvCharacterSetConverter::EOutputConversionFlagInputIsTruncated;
				return EFalse;
				}
			}
		// force ConvertToUnicode to return CCnvCharacterSetConverter::EErrorIllFormedInput
		aConversionData=NULL;
		return ETrue;
foundState:
		if (aHomogeneousRun.Length()>0)
			{
			return ETrue;
			}
		}
	}

TBool CnvUtilities::MatchesEscapeSequence(TInt& aNumberOfForeignBytesConsumed, TPtrC8& aHomogeneousRun, TPtrC8& aRemainderOfForeign, const TDesC8& aEscapeSequence)
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

TBool CnvUtilities::IsStartOf(const TDesC8& aStart, const TDesC8& aPotentiallyLongerDescriptor)
	{
	const TInt lengthOfStart=aStart.Length();
	return (aPotentiallyLongerDescriptor.Length()>=lengthOfStart) && (aPotentiallyLongerDescriptor.Left(lengthOfStart)==aStart);
	}

