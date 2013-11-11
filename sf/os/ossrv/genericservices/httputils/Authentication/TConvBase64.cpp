// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TBase64.cpp
// 
//

#include <tconvbase64.h>

/**
TBase64 constructor
*/
EXPORT_C TBase64::TBase64(): iShiftStored(0), iMaskShiftStored(ESix)
	{}
	
	
/**
	Encodes an ASCII string to Base64 string.

	@param aSrcString The source string in ASCII code.
	@param rDestString The destination string with converted base64 values.
	@return Number of characters in the source string that were not Encoded.
*/	
EXPORT_C TInt TBase64::Encode(const TDesC8& aSrcString, TDes8& rDestString)
	{
    // Clears the destination string
    rDestString.Zero();
    // Initialise variables
    const TUint8* srcStringPtr=aSrcString.Ptr();
    const TUint8* srcStringEnd=aSrcString.Length()+srcStringPtr;
    TUint8* destStringPtr=(TUint8*)rDestString.Ptr();
    TUint8* destStringPtrBase=destStringPtr;
    TInt character=0;
    TUint8 encodedChar=0;
    TInt charStorage=0;
    TInt maskShift=EZero;
    TInt destStringCharNum = 0;

    while(srcStringPtr<=srcStringEnd)
	    {
        // maskShift is used as a char read counter
	    if(maskShift==ESix)
            {
	        // If the 3rd char read is also the last char then the while loop
            // is broken on the next check.
            if(srcStringPtr==srcStringEnd)
            	srcStringPtr++;
            maskShift=EZero;
            character=0;   
            }
        else
            {
            if(srcStringPtr==srcStringEnd)
	            character=0;
            else
		        character=*srcStringPtr;
 
			srcStringPtr++;
		    // Shifts charStorage ready for the next char
	        charStorage=charStorage<<8;
            maskShift+=ETwo;
            }
        charStorage=charStorage|character;
        // Shifts the mask to the correct bit location
        // Masks (AND's) the valid bits from charStorage
        // Shifts the valid bits into the low order 8bits
        // Converts to BASE64 char, Casts the result to an unsigned char (which it should be ?....I hope)
        encodedChar=(TUint8)Base64ToAscii[((charStorage>>maskShift)&ESixBitMask)];

        *destStringPtr++=encodedChar;
        destStringCharNum++;

        // Add a CRLF every KMaxB64EncodedCharsPerLine characters so as not to exceed the line length
        // limitation specified in RFC 2822.
        if (destStringCharNum == KMaxB64EncodedCharsPerLine)
	        {
            destStringCharNum = 0;
            *destStringPtr++ = '\r';
            *destStringPtr++ = '\n';
            }
		}
     
    // Check for not enough chars and pad if required
    if (maskShift==EFour)
	    {
        *destStringPtr++=KImcvConvEquals;
        *destStringPtr++=KImcvConvEquals;
        }
    else
        if(maskShift==ESix)
	        *destStringPtr++=KImcvConvEquals;   
            
    rDestString.SetLength((TInt)(destStringPtr-destStringPtrBase));
    return ((TInt)(srcStringPtr-srcStringEnd));
	}

/**
	Decodes the Base64 string to ASCII pattern.

	@param aSrcString The source string in Base64 codeset.
	@param rDestString The destination string with converted ASCII code values.
	@return ETrue if aSrcString is not long enough to decode fully, resulting in the storage of
		the last character and requiring another aSrcString (poss 0 length) to be passed to it to clear this character. 
		Returns EFalse if the line was decoded OK or the end of the encoded file is reached ie "="
*/	
EXPORT_C TBool TBase64::Decode(const TDesC8& aSrcString, TDes8& rDestString)
	{
	TInt decodedInt=0;
	TInt8 offsetChar=0;
	TUint8 decodedChar=0;
	 
	// Clears the destination string
	rDestString.Zero();

	// Initialise variables
	const TUint8* srcStringPtr=aSrcString.Ptr();
	const TUint8* srcStringEnd=aSrcString.Length()+srcStringPtr;
	TUint8* destStringPtr=(TUint8*)rDestString.Ptr();
	TUint8* destStringPtrBase=destStringPtr;

	TInt maskShift=iMaskShiftStored;
	TInt shiftStorage=iShiftStored;
	
	// Main character process loop
	while(srcStringPtr<srcStringEnd)	
		{
		offsetChar=(TInt8)(*srcStringPtr-KImcvLookUpStartOffset);
		srcStringPtr++;

		// Check for valid B64 character		
		if((offsetChar>=0)&&(offsetChar<80))
			{
			// Read in next character and B64 decode
			decodedInt=AsciiToBase64[offsetChar];

			// Exits when a PAD char is reached
			if(decodedInt==EPadChar)
				{
				rDestString.SetLength((TInt)(destStringPtr-destStringPtrBase));
				return EFalse;
				}

			// Ensures the first 2 chars of 4 are received before processing
			if(maskShift==ESix)
				maskShift=EFour;
			else
				{
				shiftStorage=shiftStorage<<ESix;
				shiftStorage=shiftStorage|decodedInt;
				decodedChar=(TUint8)((shiftStorage>>maskShift)&EEightBitMask);
				
				if((maskShift-=ETwo)<EZero)
					maskShift=ESix; 
				
				*destStringPtr++=decodedChar;
				}
			shiftStorage=decodedInt;
			}
		}
	iShiftStored=shiftStorage;
	iMaskShiftStored=maskShift;
	
	rDestString.SetLength((TInt)(destStringPtr-destStringPtrBase));
	
	return maskShift<ESix;
	}


/**
    Encodes an ASCII string to Base64 string.

    @param aSrcString The source string in ASCII.
    @param aDestString The destination string with converted base64 values.
    @param aLineLength The maximum line length of the encoded base64 values.
         A CR/LF sequence will be added after these many characters. 
         The default value is -1, which means no CR/LF is added to output. The encoding is compliant with RFC 4648 
    @return Number of characters in the source string that were not encoded.
*/  
EXPORT_C TInt TBase64::PortableEncode(const TDesC8& aSrcString, TDes8& aDestString, TInt aLineLength)
    {
    // Clears the destination string
    aDestString.Zero();
    // Initialise variables
    const TUint8* srcStringPtr=aSrcString.Ptr();
    const TUint8* srcStringEnd=aSrcString.Length()+srcStringPtr;
    TUint8* destStringPtr=(TUint8*)aDestString.Ptr();
    TUint8* destStringPtrBase=destStringPtr;
    TInt character=0;
    TUint8 encodedChar=0;
    TInt charStorage=0;
    TInt maskShift=EZero;
    TInt destStringCharNum = 0;

    while(srcStringPtr<=srcStringEnd)
        {
        // maskShift is used as a char read counter
        if(maskShift==ESix)
            {
            // If the 3rd char read is also the last char then the while loop
            // is broken on the next check.
            if(srcStringPtr==srcStringEnd)
                srcStringPtr++;
            maskShift=EZero;
            character=0;   
            }
        else
            {
            if(srcStringPtr==srcStringEnd)
                character=0;
            else
                character=*srcStringPtr;
 
            srcStringPtr++;
            // Shifts charStorage ready for the next char
            charStorage=charStorage<<8;
            maskShift+=ETwo;
            }
        charStorage=charStorage|character;
        // Shifts the mask to the correct bit location
        // Masks (AND's) the valid bits from charStorage
        // Shifts the valid bits into the low order 8bits
        // Converts to BASE64 char, Casts the result to an unsigned char (which it should be ?....I hope)
        encodedChar=(TUint8)Base64ToAscii[((charStorage>>maskShift)&ESixBitMask)];

        *destStringPtr++=encodedChar;
        destStringCharNum++;

        // Add a CRLF every aLineLength number of characters 
        if (destStringCharNum == aLineLength)
            {
            destStringCharNum = 0;
            *destStringPtr++ = '\r';
            *destStringPtr++ = '\n';
            }
        }
     
    // Check for not enough chars and pad if required
    if (maskShift==EFour)
        {
        *destStringPtr++=KImcvConvEquals;
        *destStringPtr++=KImcvConvEquals;
        }
    else
        if(maskShift==ESix)
            *destStringPtr++=KImcvConvEquals;   
            
    aDestString.SetLength((TInt)(destStringPtr-destStringPtrBase));
    return ((TInt)(srcStringPtr-srcStringEnd));
    }
