// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <utf.h>
#include <charconv.h>

#include <xml/plugins/charsetconverter.h>
#include <xml/xmlframeworkerrors.h>

using namespace Xml;

/**
The maximum number of bytes used for conversion at any time.
This is also used to size the necessary buffers used in the conversions.

@internalTechnology
*/
const TInt KMaxReadableBytes = 512;



LOCAL_C void DestroyHBufC16(TAny* aHBufC)
/**
This method is used when pointer reallocation is needed and the pointer needs to be 
cleaned via the cleanup stack.

@param				aHBufC the wide buffer.
@internalTechnology

*/
	{
	delete *static_cast<HBufC**>(aHBufC);
	}



LOCAL_C void DestroyHBufC8(TAny* aHBufC)
/**
This method is used when pointer reallocation is needed and the pointer needs to be 
cleaned via the cleanup stack.

@param				aHBufC the narrow buffer.
@internalTechnology

*/
	{
	delete *static_cast<HBufC8**>(aHBufC);
	}



CCharSetConverter::CCharSetConverter()
/**
Default Constructor

*/
	{
	// do nothing;
	}



/**
This method creates an instance of this class.
The framework is responsible for creating this object.

@leave ... One of the system wide error codes e.g. KErrNoMemory
@return	The new'ed object.
@internalTechnology
*/
CCharSetConverter* CCharSetConverter::NewL()
	{
	CCharSetConverter* self = new(ELeave) CCharSetConverter();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return(self);
	}



void CCharSetConverter::ConstructL()
/**
This method provides some construction of this object.

*/
	{
	iCnvCharacterSetConverter = CCnvCharacterSetConverter::NewL();
	User::LeaveIfError(iFs.Connect());
	iConversionBuffer = User::Heap().AllocL(KMaxReadableBytes);
	iConversionBufferSize = KMaxReadableBytes;
	}



CCharSetConverter::~CCharSetConverter()
/**
Destructor.
The framework is responsible for destroying this object.

@post				This object is properly destroyed.

*/
	{
	iFs.Close();
	delete iCnvCharacterSetConverter;
	delete iConversionBuffer;
	}



EXPORT_C void CCharSetConverter::PrepareCharConvL(TUint& aCharSetUid, const TDesC8& aEncoding)
/**
This method prepares CharConv to encode from the standard name.

@post				CharConv has been prepared.

@leave				KErrXmlUnsupportedCharacterSet - Charset not supported.
@leave				KErrXmlUnavailableCharacterSet - Charset not available

@param				aCharSetUid On return, contains the character set identifier 
					of the encoding.
@param				aEncoding the encoding to prepare for.
*/
	{
	// Get the charset uid
	if ((aCharSetUid = 
		iCnvCharacterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(aEncoding, iFs)) == 0)
		{
		User::Leave(KErrXmlUnsupportedCharacterSet);
		}


	// Prepare charconv to use this charset
	if (iCnvCharacterSetConverter->PrepareToConvertToOrFromL(aCharSetUid, iFs) == 
			CCnvCharacterSetConverter::ENotAvailable )
		{
		User::Leave(KErrXmlUnavailableCharacterSet); // Unavailable
		}
	}



EXPORT_C void CCharSetConverter::PrepareCharConvL(TUint& aCharSetUid, TInt aMibEnum)
/**
This method prepares CharConv to encode from the mib enum.

@post				CharConv has been prepared.

@leave				KErrXmlUnsupportedCharacterSet - Charset not supported.
@leave				KErrXmlUnavailableCharacterSet - Charset not available

@param				aCharSetUid On return, contains the character set identifier 
					of the encoding.
@param				aMibEnum The IANA specified mib enum for this encoding

@see				http://www.iana.org/assignments/character-sets
*/
	{
	// Get the charset uid
	if ((aCharSetUid = 
		iCnvCharacterSetConverter->ConvertMibEnumOfCharacterSetToIdentifierL(aMibEnum, iFs)) == 0)
		{
		User::Leave(KErrXmlUnsupportedCharacterSet);  // May want to try something else?
		}


	// Prepare charconv to use this charset
	if (iCnvCharacterSetConverter->PrepareToConvertToOrFromL(aCharSetUid, iFs) == 
			CCnvCharacterSetConverter::ENotAvailable )
		{
		User::Leave(KErrXmlUnavailableCharacterSet); // Unavailable
		}
	}



EXPORT_C TInt CCharSetConverter::ConvertToUnicodeL(TUint32 aSrcCharset, const TDesC8& aInputBuffer, 
														 HBufC16*& aUnicodeConversion)
/**
This method converts the given bytes to unicode.
If this function leaves, memory is cleaned up.
This overload allocates memory for the output itself.

@return				KErrNone if the conversion was succesfull
					or one of the error values defined in TError.

@leave				KErrXmlUnavailableCharacterSet - CharSet not available.

@param				aSrcCharset The character set encoding to convert from.
@param				aInputBuffer The characters to be converted.
@param				aUnicodeConversion On return, contains the unicode conversion.
*/
	{	
	if (iCnvCharacterSetConverter->PrepareToConvertToOrFromL(aSrcCharset, iFs) == 
			CCnvCharacterSetConverter::ENotAvailable)
		{
		User::Leave(KErrXmlUnavailableCharacterSet);
		}

	TInt maxLength = KMaxReadableBytes;	
	aUnicodeConversion = HBufC16::NewL(maxLength);	
	CleanupStack::PushL(TCleanupItem(DestroyHBufC16, &aUnicodeConversion));//push buffer's address


	TInt state = CCnvCharacterSetConverter::KStateDefault;
	TPtr16 remainingOutput(aUnicodeConversion->Des());
	TInt unconverted = iCnvCharacterSetConverter->ConvertToUnicode(remainingOutput, aInputBuffer, state);	

	// While there is still more data to convert
	while (0 < unconverted)	
		{
		// Resize the buffer to hold more data
		maxLength += KMaxReadableBytes;
		aUnicodeConversion = aUnicodeConversion->ReAllocL(maxLength);

		// Segment the writable area
		TInt outputLength = aUnicodeConversion->Length();
		TPtr16 remainingOutput1(&(aUnicodeConversion->Des())[0] + outputLength, 0, maxLength - outputLength);
		remainingOutput.Set(remainingOutput1);

		// Convert the data
		unconverted = iCnvCharacterSetConverter->ConvertToUnicode(remainingOutput, aInputBuffer.Right(unconverted), state);
		aUnicodeConversion->Des().SetLength(outputLength + remainingOutput.Length());
		}

	// Reallocate to a minimally-sized buffer	
	if (unconverted == 0)
		{
		aUnicodeConversion = aUnicodeConversion->ReAllocL(aUnicodeConversion->Length());
		}

	CleanupStack::Pop(&aUnicodeConversion);//destroy the object pointed by the buffer wherever it is since we have got hold of the pointer (buffer)'s address
	return unconverted;  // return error value if there is one.
	}


EXPORT_C TInt CCharSetConverter::ConvertToUnicodeL(TUint32 aSrcCharset, 
												   const TDesC8& aInput,
												   TPtr16& aOutput)
/**
This method converts the given bytes to unicode.
If this function leaves, memory is cleaned up.
This overload stores the conversion output in memory already allocated, for the sole use
of the TPtr versions of overloaded ConvertToUnicodeL and ConvertFromUnicodeL functions. You must make sure you
have finished with the output from a previous call to either (TPtr overload of) ConvertToUnicodeL
or ConvertFromUnicodeL before calling either again, as the previous output will be overwritten with
the new output.
This version is more efficient than the HBufC alternative and so should be used whenever possible.

@return				KErrNone if the conversion was succesfull
					or one of the error values defined in TError.

@leave				KErrXmlUnavailableCharacterSet - CharSet not available.

@param				aSrcCharset The character set encoding to convert from.
@param				aInput The characters to be converted.
@param				aOutput On return, contains the unicode conversion.
*/
	{	
	if (iCnvCharacterSetConverter->PrepareToConvertToOrFromL(aSrcCharset, iFs) == 
			CCnvCharacterSetConverter::ENotAvailable)
		{
		User::Leave(KErrXmlUnavailableCharacterSet);
		}

	TInt state = CCnvCharacterSetConverter::KStateDefault;

	// Set up output descriptor reference: "Payload" is iConversionBuffer (a TAny *), it's initial
	// length is zero (because it's empty) and it's initial maximum length is the maximum number of
	// unicode characters which will fit into the current size of iConversion buffer 	
	aOutput.Set((TUint16*)iConversionBuffer, 0, iConversionBufferSize/sizeof(TUint16));

	// Convert the data, returning the amount of characters that are unconverted, due to the output buffer being full
	TInt unconverted = iCnvCharacterSetConverter->ConvertToUnicode(aOutput, aInput, state);	

	// While there is still more data to convert
	while (0 < unconverted)	
		{
		TInt outputLength = aOutput.Length();

		// Resize the buffer to hold more data
		iConversionBufferSize += KMaxReadableBytes;
		
		iConversionBuffer = User::Heap().ReAllocL(iConversionBuffer,iConversionBufferSize);
		if (iConversionBuffer == NULL) 	
			User::Leave(KErrNoMemory);
		
		// Reconstruct the output descriptor to point to the new buffer, setting current
		// length (the number of characters we've converted so far) and maximum length
		// (the number of unicode characters which will fit into the newly extended 
		// iConversionBuffer) appropriately. 
		aOutput.Set((TUint16*)iConversionBuffer, outputLength, iConversionBufferSize/sizeof(TUint16));		

		// Construct a modifiable pointer descriptor pointing to the the writable area of
		// iConversionBuffer
		TPtr16 remainingOutput(((TUint16*)iConversionBuffer)+outputLength, 0, aOutput.MaxLength() - outputLength);

		// Try to convert another chunk of data
		unconverted = iCnvCharacterSetConverter->ConvertToUnicode(remainingOutput, aInput.Right(unconverted), state);
		
		// Update the length of the output buffer to include the data we just converted.
		aOutput.SetLength(remainingOutput.Length()+outputLength);
		}
		
	return unconverted;  // return error value if there is one.
	}


			
EXPORT_C TInt CCharSetConverter::ConvertFromUnicodeL(const TDesC16& aUnicodeConversion, 
														   TUint32 aDestCharset, HBufC8*& aOutputBuffer)
/**
This method converts the given unicode to the specified encoding.
If this function leaves, memory is cleaned up.
This overload allocates memory for the output itself.

@return				KErrNone if the conversion was succesfull
					or one of the error values defined in TError.
					
@leave				KErrXmlUnavailableCharacterSet - Charset not available.

@param				aUnicodeConversion The unicode to convert.
@param				aDestCharset The character set encoding to convert to.
@param				aOutputBuffer On return, contains the specified conversion.
*/
	{
	if (iCnvCharacterSetConverter->PrepareToConvertToOrFromL(aDestCharset, iFs) == 
			CCnvCharacterSetConverter::ENotAvailable)
		{
		User::Leave(KErrXmlUnavailableCharacterSet);
		}

	TInt maxLength = KMaxReadableBytes;	
	aOutputBuffer = HBufC8::NewL(maxLength);	
	CleanupStack::PushL(TCleanupItem(DestroyHBufC8, &aOutputBuffer));//push buffer's address

	TPtr8 remainingOutput(aOutputBuffer->Des());
	TInt unconverted = iCnvCharacterSetConverter->ConvertFromUnicode(remainingOutput, aUnicodeConversion);	

	// While there is still more data to convert
	while (0 < unconverted)	
		{
		// Resize the buffer to hold more data
		maxLength += KMaxReadableBytes;
		aOutputBuffer = aOutputBuffer->ReAllocL(maxLength);

		// Segment the writable area
		TInt outputLength = aOutputBuffer->Length();
		TPtr8 remainingOutput1(&(aOutputBuffer->Des())[0] + outputLength, 0, maxLength - outputLength);
		remainingOutput.Set(remainingOutput1);

		// Convert the data
		unconverted = iCnvCharacterSetConverter->ConvertFromUnicode(remainingOutput, aUnicodeConversion.Right(unconverted));
		aOutputBuffer->Des().SetLength(outputLength + remainingOutput.Length());
		}

	// Reallocate to a minimally-sized buffer	
	if (unconverted == 0)
		{
		aOutputBuffer = aOutputBuffer->ReAllocL(aOutputBuffer->Length());
		}

	CleanupStack::Pop(&aOutputBuffer);//destroy the object pointed by the buffer wherever it is since we have got hold of the pointer (buffer)'s address

	return unconverted;  // return error value if there is one.
	}


EXPORT_C TInt CCharSetConverter::ConvertFromUnicodeL(const TDesC16& aInput,
													 TUint32 aDestCharset,
													 TPtr8& aOutput)
/**
This method converts the given unicode to the specified encoding. 
If this function leaves, memory is cleaned up.
This overload stores the conversion output in memory already allocated, for the sole use
of the TPtr versions of overloaded ConvertToUnicodeL and ConvertFromUnicodeL functions. You must make sure you
have finished with the output from a previous call to either (TPtr overload of) ConvertToUnicodeL
or ConvertFromUnicodeL before calling either again, as the previous output will be overwritten with
the new output.
This version is more efficient than the HBufC alternative and so should be used whenever possible.

@return				KErrNone if the conversion was succesfull
					or one of the error values defined in TError.
					
@leave				KErrXmlUnavailableCharacterSet - Charset not available.

@param				aInput The unicode to convert.
@param				aDestCharset The character set encoding to convert to.
@param				aOutput The characters after conversion.
*/
	{
	if (iCnvCharacterSetConverter->PrepareToConvertToOrFromL(aDestCharset, iFs) == 
			CCnvCharacterSetConverter::ENotAvailable)
		{
		User::Leave(KErrXmlUnavailableCharacterSet);
		}
	// Set up output descriptor reference: "Payload" is iConversionBuffer (a TAny *), it's initial
	// length is zero (because it's empty).	
	aOutput.Set((TUint8*)iConversionBuffer, 0, iConversionBufferSize);	

	// Convert the data, returning the amount of characters that are unconverted, due to the output buffer being full
	TInt unconverted = iCnvCharacterSetConverter->ConvertFromUnicode(aOutput, aInput);	

	// While there is still more data to convert
	while (0 < unconverted)	
		{
		TInt outputLength = aOutput.Length();
		
		// Resize the buffer to hold the remaining data
		iConversionBufferSize += KMaxReadableBytes;
		
		iConversionBuffer = User::Heap().ReAllocL(iConversionBuffer,iConversionBufferSize);
		if (iConversionBuffer == NULL) 	
			User::Leave(KErrNoMemory);
		aOutput.Set((TUint8*)iConversionBuffer,iConversionBufferSize,iConversionBufferSize);
		
		// Construct a modifiable pointer descriptor pointing to the the writable area of
		// iConversionBuffer
		TPtr8 remainingOutput(((TUint8*)iConversionBuffer) + outputLength, 0, iConversionBufferSize - outputLength);

		// Try to convert another chunk of data
		unconverted = iCnvCharacterSetConverter->ConvertFromUnicode(remainingOutput, aInput.Right(unconverted));
	
		// Update the length of the output buffer to include the data we just converted.
		aOutput.SetLength(remainingOutput.Length()+outputLength);
		}
		
	return unconverted;  // return error value if there is one.
	}


EXPORT_C void CCharSetConverter::PrepareToConvertToOrFromL(TUint32 aCharSetUid)
/**
This method is a helper function that prepares CharConv for a conversion.

@see				CCnvCharacterSetConverter::PrepareToConvertToOrFromL
@post				CharConv is ready for the conversion or not.

@leave				KErrXmlUnavailableCharacterSet - Charset not available.

@param				aCharSetUid The character set encoding to convert to.
*/
	{
	if (iCnvCharacterSetConverter->PrepareToConvertToOrFromL(aCharSetUid, iFs) == 
			CCnvCharacterSetConverter::ENotAvailable )
		{
		User::Leave(KErrXmlUnavailableCharacterSet);
		}
	}



EXPORT_C void CCharSetConverter::ConvertCharacterSetIdentifierToStandardNameL(TUint32 aCharSetUid, 
																			  HBufC8*& aCharSet)
/**
This method is a helper function that obtains a standand character
encoding name from a character set identifer.

@see				CCnvCharacterSetConverter::ConvertCharacterSetIdentifierToStandardNameL

@leave				KErrXmlUnsupportedCharacterSet If the character set is not known.

@param				aCharSetUid The character set to obtain the name for.
@param				aCharSet On return holds the Internet-standard name
					or MIME name of the character set.
					The name is encoded in 8 bit ASCII. 
*/
	{
	if ((aCharSet = 
		iCnvCharacterSetConverter->
			ConvertCharacterSetIdentifierToStandardNameL(aCharSetUid, iFs)) == NULL)
		{
		User::Leave(KErrXmlUnsupportedCharacterSet);
		}
	}



EXPORT_C TInt CCharSetConverter::ConvertUcs4CharactersToEncodingL(TUint32* aUcs4Src, 
																  TInt aUcs4Count, 
																  TUint32 aDestCharset,
																  HBufC8*& aConversion)
/**
This method converts ucs-4 characters to the desired non-modal encoding.
aConversion should be NULL on calling of this function.
If this function leaves, memory is cleaned up.
There is no TPtr overload of this method, as currently it is only called a few times and so would not 
produce any noticable benefits.

@return				CCharSetConverter::ConvertFromUnicodeL.

@leave				KErrXmlBadCharacterConversion

@param				aUcs4Src list of ucs-4 characters.
@param				aUcs4Count number of ucs4 characters.
@param				aDestCharset the desired encoding.
@param				aConversion On return, points to the converted encoding.
*/
	{
	// convert ucs-4 to ucs-2

	// Find the length of the output
	TText16 buf[2];
	TInt length = 0;
	TUint32* src = NULL;

	for (src = aUcs4Src; src != (aUcs4Src + aUcs4Count); ++src)
		{
		// Convert a single character into the buffer, discard the result
		// but increase the length by the number of UTF16 codes output.
		length += Utf32ToUtf16(buf, *src) - buf;
		}

	HBufC16* utf16Out = HBufC16::NewL(length);
	CleanupStack::PushL(utf16Out);

	utf16Out->Des().SetLength(length);
    
	TText16* p = &((utf16Out->Des())[0]);
	

	// go through characters converting to ucs2.
	for (src = aUcs4Src; src != aUcs4Src + aUcs4Count; ++src)
		{
		// convert each ucs4 character
		 p = Utf32ToUtf16(p, *src);
		}

	// convert from ucs2 to desired encoding
	aConversion = NULL;
	TInt ret = 0;

	//HBufC overload of this method called, due to the need pass back the HBufC to the calling method
	ret = CCharSetConverter::ConvertFromUnicodeL(*utf16Out, aDestCharset, aConversion);
	CleanupStack::PushL(aConversion);

	if(ret > KErrNone)
		{
		// CharConv couldn't convert all the bytes. Character encoding may be truncated.
		User::Leave(KErrXmlBadCharacterConversion);
		}

	CleanupStack::Pop(aConversion);
	CleanupStack::PopAndDestroy(utf16Out);
	return(ret);
	}



TText16* CCharSetConverter::Utf32ToUtf16(TText16* aUtf16Out, TUint32 aUtf32)
/**
This method converts a ucs-4 character to unicode.

@return				Pointer to the next free byte in the output buffer.

@param				aUtf16Out On return, contains the unicode character conversion.
@param				aUtf32 The ucs-4 character
*/
	{
	if (aUtf32 <= 0xFFFF)
		{
		// UTF32 (or UCS4) should not have characters in the range 
		// D800-DBFF (high surrogate) and DC00-DFFF (low surrogate) in it,
		// as these are the surrogates that make up the extension mechanism for 
		// fitting Unicode into 16 bits.
		// In principle, surrogates in UCS-4 should be ignored. 
		// They are considered a bad thing because they might be an aliasing 
		// problem: one thing looking like another. 
		// In practice I don't think it is a problem here. 
		// If you like, you could reject any character between D800 to DFFF.

		// could weed out unpaired surrogates here, but...
		*aUtf16Out = static_cast<TText16>(aUtf32);
		return aUtf16Out + 1;
		}

	// A way to visualise the use of surrogate pairs is to imaging planes.
	// The surrogate is located on plane zero and identifies the actual plane
	// this character resides in.
	// This is why for supplementary characters we must insert the surrogates
	// so that charconv can convert correctly.
	// 
	// 0    D800 
	// |    | DFFF 
	// |    | | E000                                                                  10FFF
	// |    | | |                                                                     |
	// xxxxxxYxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	//       ^
	// For Utf32 this means nothing. 
	// 
	// For Utf16 if the following bit pattern is located then it corresponds to a 
	// supplementary character.
	// 
	//       ^
	// D800           DC00           DFFF
	// |              |              |
	// yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy
	//       High           Low
	// 
	// 1101 10.. ........         1101 11.. ........
	//        --10  bit--                --10  bit-- 
	// 
	// 
	// Add 10000 to both 10-bit values and the offset to the correct character is obtained.
	// 
	// 
	// So a test of this function would be to check that the value returned 
	// matches a utf-8 character encoding manually calculated from the original ucs4
	// value.


	// We have a supplementary character consists of 5 nibbles (20 bits) 
	// with no surrogates.
	// We have to insert the surrogate pair on the values minus 0x10000.
	// b0-b9 is the low order value, b10-b19 is the high order value.
	// b19....b10 b9....b0
	//   high       low
	//
	// Character values.
	// Basic			0x0-0xFFFF
	// Supplimentary	0x10000-0x10FFFF
	// so 0x10000 >> 10 = 0x43FF

	// To add the surrogate to the high order:
	//
	// ((utf32-0x10000)>>10)+0xD800
	// = (utf32>>10)-(0x10000>>10)+0xD800
	// = (utf32>>10)+(0xD800-0x40)
	// = (utf32>>10)+0xD7C0

	aUtf16Out[0] = static_cast<TText16>((aUtf32 >> 10)  + 0xD7C0);

	// To add the surrogate to the low order:
	//
	// ((utf32-0x10000) & 0x3FF)+0xDC00
	// = ((utf32 & 0x3FF) - (0x10000 & 0x3FF)) + 0xDC00
	// = ((utf32 & 0x3FF) - (0)) + 0xDC00
	// = (utf32 & 0x3FF) + 0xDC00

	aUtf16Out[1] = static_cast<TText16>(0xDC00 | (aUtf32 & 0x3FF));

	return aUtf16Out + 2;
	}
