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

#include <e32base.h>
#include <stringpool.h>
#include <wspdecoder.h>

// Constants
const TUint8 KWapQuote = 0x7F;
const TUint8 KCarryBitMask = 0x80;
#define KTopBitMask KCarryBitMask
const TUint8 KTop3BitSet = 0x70;
const TUint8 KQuoteChar = '\"';

// Panic category
_LIT(KUriPanicCategory,"WSPDECODER");


//***********************************************************************
//	TWspHeaderSegmenter
//**********************************************************************/


/**
  NextL iterates through the buffer.  Each call returns a TWspField in the paramater.

  @param aHeader Out - a TWspField containing the header <name,value> pair.  
  @warning The TWspField::iHdrName will be opened internally.
			It must be closed by the caller before this class is destroyed.
  @return	KErrNone if next field is returned
			KErrNotFound at the end of the buffer - no TWspField param returned
			KErrCorrupt if segmenting does not parse correctly
  @leave	RStringPool::OpenFStringL StringPool leave code if opening string fails.
*/
EXPORT_C TInt TWspHeaderSegmenter::NextL(TWspField& aHeader)
	{
	// have we run out of buffer?
	if (iOffset >= iBuffer.Length())
		return KErrNotFound;

	// Set decoder to current buffer
	TWspPrimitiveDecoder decoder(iBuffer.Mid(iOffset));
	TInt bufLen = 0;
	

	// Get the Field Name 
	switch(decoder.VarType())
		{
		case TWspPrimitiveDecoder::EString:
			{
			TPtrC8 name;
			bufLen = decoder.String(name);
			if (bufLen < KErrNone) return bufLen;
			aHeader.iHdrName = iPool.OpenFStringL(name);
			}
			break;

		case TWspPrimitiveDecoder::E7BitVal:
			{
			TUint8 name;
			bufLen = decoder.Val7Bit(name);
			if (bufLen < KErrNone) return bufLen;
			
			aHeader.iHdrName = iPool.StringF(name, iStringTable);
			}
			break;

		default:	// header name can't be anything else
			return KErrCorrupt;
		}
	
	// move our pointer past header name
	iOffset += bufLen;

	
	// Get the value buffer by figuring out the type, then set the pointer to span the entire
	// value.  Note - further parsing will happen later to pull out specific value data.
	switch(decoder.VarType())
		{
		case TWspPrimitiveDecoder::ELengthVal:
			{
			TInt len;
			bufLen = decoder.LengthVal(len);
			bufLen += len;		
			}
			break;
		case TWspPrimitiveDecoder::EQuotedString:
		case TWspPrimitiveDecoder::EString:
			{
			TPtrC8 strBuf;
			bufLen = decoder.String(strBuf);
			}
			break;
		case TWspPrimitiveDecoder::E7BitVal:
			bufLen = 1;
			break;
		default:
			return KErrCorrupt;
		}
	
	if (bufLen < 0) 
		return bufLen;

	if (iOffset + bufLen > iBuffer.Length())
          return KErrCorrupt;

	aHeader.iValBuffer.Set(iBuffer.Mid(iOffset, bufLen));
	iOffset += bufLen;
	return KErrNone;
	}


 //
 // WAP-WSP 8.4.1.2
 //

/**
	Looks at the byte currently pointed at in this buffer and returns the type.
	
	@return TWspHeaderType - the type of this data octet
*/
EXPORT_C TWspPrimitiveDecoder::TWspHeaderType TWspPrimitiveDecoder::VarType() const
	{
	TWspHeaderType type = ENotSet;
	
	// Check that the offset has not overflowed the buffer
	if( iOffset >= iBuffer.Length() )
		return type;

	TInt octet = iBuffer[iOffset];

	if (octet >= 0 && octet <= 31)
		type = ELengthVal;
	else if (octet == 34)
		type = EQuotedString;
	else if (octet >= 32 && octet <= 127)
		type = EString;
	else if (octet >= 128 && octet <= 255)
		type = E7BitVal;

	return type;
	}


/**
	Returns length of the data following this byte.  
	
	@pre iBuffer[iOffset] must be valid, VarType() == TWspHeaderType::ELengthVal
	@post internal offset gets updated to move past this primitive 
	@param aVal Out - the length encoded in this byte that indicates the size of the
		  	     data that follows.
	@return postive number indicating the number of bytes read from the buffer
			KErrCorrupt if data is not formatted correctly.
*/
EXPORT_C TInt TWspPrimitiveDecoder::LengthVal(TInt& aVal)
	{
	// have we run out of buffer?
	if (iOffset >= iBuffer.Length())
		return KErrCorrupt;

	TInt bufLen = 0;
	aVal = iBuffer[iOffset++];

	if (aVal == 31)
		{
		TUint32 uintVarLen = 0;
		bufLen = UintVar(uintVarLen);
		if (bufLen < KErrNone) return bufLen;
		aVal = (TInt)uintVarLen;
		}

	// add the 1 byte read at to get first aVal
	++bufLen;
	return bufLen;
	}


/**
   Returns a TPtrC holding the string the buffer currently points at without the NULL 
   termination. If the String type is a quoted string then the quotes are not included 
   in the returned buffer.
   
   @pre iBuffer[iOffset] must be valid, VarType() == TWspType::EString
   @post internal offset gets updated to move past this primitive 
   @param aString Out - the string
   @return postive number indicating the number of bytes read from the buffer
   		  KErrCorrupt if data is not formatted correctly.
*/
EXPORT_C TInt TWspPrimitiveDecoder::String(TPtrC8& aString)
	{
	TWspHeaderType type = VarType();
	if( type != EString && type != EQuotedString)
		return KErrCorrupt;

	TInt nullIndex = iBuffer.Mid(iOffset).Locate('\0');
	if( nullIndex == KErrNotFound )
		return KErrCorrupt;

	// Set buffer to data not including the NULL terminator
	TPtrC8 buf = iBuffer.Mid(iOffset, nullIndex);

	// is there a WAP Quote (0x7F) or a " at the start - step over it
	TInt bufferOffset = 0;
	const TUint8 firstByte = iBuffer[iOffset];
	if( firstByte == KQuoteChar || firstByte == KWapQuote )
		++bufferOffset;

	// Set the descriptor with the correct buffer segment
	aString.Set(buf.Mid(bufferOffset));

	// Step over the NULL
	++nullIndex;

	// update the offset
	iOffset += nullIndex;

	return nullIndex;
	} 

/**
	Returns a token, a short int or an octet value with the top bit cleared
	
	@pre iBuffer[iOffset] must be valid, VarType() == TWspHeaderType::E7BitVal
	@post internal offset gets updated to move past this primitive 
	@param aVal Out - the 7 bit value with top bit cleared
	@return postive number indicating the number of bytes read from the buffer
		    KErrCorrupt if data is not formatted correctly.
*/
EXPORT_C TInt TWspPrimitiveDecoder::Val7Bit(TUint8& aVal)
	{
	// have we run out of buffer?
	if (iOffset >= iBuffer.Length())
		return KErrCorrupt;

	aVal = (TUint8)(iBuffer[iOffset] & KWapQuote);
	++iOffset;

	// 1 byte read
	return 1;
	}


/**
	Returns an Integer - could be short or long.
	
	@pre iBuffer[iOffset] must be valid, VarType() == TWspHeaderType::ELengthVal or
		   VarType() == TWspHeaderType::E7BitVal
	@post internal offset gets updated to move past this primitive 
	@param aVal Out - the long int
	@return postive number indicating the number of bytes read from the buffer
			  KErrCorrupt if data is not formatted correctly.
*/
EXPORT_C TInt TWspPrimitiveDecoder::Integer(TUint32& aVal)
	{
	// have we run out of buffer?
	if (iOffset >= iBuffer.Length())
		return KErrCorrupt;

	TInt bufLen = 0;

	// read the first byte
	aVal = iBuffer[iOffset];
	
	// short integers have the top bit set
	if (aVal & KTopBitMask)
		{
		aVal &= KWapQuote;
		++iOffset;
		++bufLen;
		}
	else
		{
		bufLen = LongInt(aVal);
		}

	return bufLen;
	}

/**
	Returns a long int the buffer is currently pointing at. 
	
	@pre iBuffer[iOffset] must be valid, VarType() == TWspHeaderType::ELengthVal  
	@post internal offset gets updated to move past this primitive 
	@param aVal Out - the long int
	@return postive number indicating the number of bytes read from the buffer
		  KErrCorrupt if data is not formatted correctly.
*/
EXPORT_C TInt TWspPrimitiveDecoder::LongInt(TUint32& aVal)
	{
	// have we run out of buffer?
	if (iOffset >= iBuffer.Length())
		return KErrCorrupt;

	__ASSERT_DEBUG(aVal <= KMaxTUint, User::Panic(KUriPanicCategory, EWspDecoderLongIntOverflow));
	// initialize
	aVal = 0;

	// Get num bytes encoding [len] [byte1] ... [byten]
	// we are positioned at that location in the source descriptor
	TUint8 numBytes = 0;
	TInt bufLen = Val7Bit(numBytes);
	if (bufLen < KErrNone) return bufLen;

	// len can be up to 30 and verify we have enough buffer
	if (numBytes > 30 || numBytes > iBuffer.Mid(iOffset).Length())
		return KErrCorrupt;
	
	// Loop over the buffer, taking each byte and shifting it in count times.  
	for (TInt count = 0; count < numBytes ; ++count)
		aVal = (aVal << 8) + iBuffer[iOffset++];
	
	return (bufLen + numBytes);
	}

/**
	Returns a TUint32 
	
	@pre iBuffer[iOffset] must be valid, VarType() == TWspHeaderType::ELengthVal or
		   VarType() == TWspHeaderType::E7BitVal 
	@post internal offset gets updated to move past this primitive 
	@param aVal Out - the TUint32 decoded 
	@return postive number indicating the number of bytes read from the buffer
		    KErrCorrupt if data is not formatted correctly.
*/
EXPORT_C TInt TWspPrimitiveDecoder::UintVar(TUint32& aVal)
	{
	// have we run out of buffer?
	if (iOffset >= iBuffer.Length())
		return KErrCorrupt;

	// initialize return val
	aVal = 0;

	// maximum length for a uintvar is 5
	TInt lenLeft = Min(iBuffer.Mid(iOffset).Length(), 5);

	// get the first octet
	TUint8 byte = iBuffer[iOffset++];
	TInt numBytes = 1;

	--lenLeft;	

	// Check if any of the top 3 bits, ignoring the very top 'continue' bit, are set.  
	// Later if we see that this is a 5 byte number - we'll know it is corrupt.  
	// Encoding uses 7 bits/number 7x5=35 and we only support a maxiumum number 
	// of 32 bits.
	TBool topThreeBitsSet = byte & KTop3BitSet; 
	
	// copy over data from the byte into our return value (the top bit is a carry bit)
	aVal = byte & KWapQuote;

	// while the 'continue' bit is set and we have more data
	while ((byte & KCarryBitMask) && (lenLeft > 0))
		{
		// shift our last value up
		aVal <<= 7;
		// get the next byte
		byte = iBuffer[iOffset++];
		// copy it over to the lowest byte
		aVal |= byte & KWapQuote;
		--lenLeft;
		++numBytes;
		} 

	// last octet has continue bit set ... NOT allowed Or
	// this was encoded wrong - can't have a number bigger than 32 bits
	if ((byte & KCarryBitMask) || (numBytes == 5 && topThreeBitsSet))
		return KErrCorrupt;

	// number of bytes read
	return numBytes;
	}


/**
	Returns a formatted version string 
	
	@pre iBuffer[iOffset] must be valid, VarType() == TWspHeaderType::ELengthVal  
	@post internal offset gets updated to move past this primitive 
	@param aPool In - an opened string pool
	@param aVer Out - a formatted version string.  Caller must close this string.
	@return postive number indicating the number of bytes read from the buffer
		    KErrCorrupt if data is not formatted correctly.
*/
EXPORT_C TInt TWspPrimitiveDecoder::VersionL(RStringPool aPool, RStringF& aVer)
	{
	const TInt KMaxBufLength=5;
	TInt bufLen = 0;
	TInt byte = iBuffer[iOffset];
	if (!(byte & KTopBitMask))
		{
		TPtrC8 str;
		bufLen = String(str);
		if (bufLen < KErrNone) return KErrCorrupt;
		aVer = aPool.OpenFStringL(str);
		}
	else
		{
		// Major 0-7 , Minor 0-15 [xxx][yyyy]
		TUint8 val;
		bufLen = Val7Bit(val);
		if (bufLen < KErrNone) return KErrCorrupt;


		TInt minVer =  val & 0x0F;
		TInt majVer =  val & 0xF0;
		majVer >>= 4;

		if (majVer < 0 || majVer > 7)
			return KErrCorrupt;

		TBuf8<KMaxBufLength> buf;
		if (minVer == 0x0F)
			{
			_LIT8(KVersionFormat, "%D");
			buf.Format(KVersionFormat, majVer);
			}
		else
			{
			_LIT8(KVersionFormat, "%D.%D");
			buf.Format(KVersionFormat, majVer, minVer);
			}
			aVer = aPool.OpenFStringL(buf);
		}

	return bufLen;
	}

/**
	Returns a TDateTime offset from January 1, 1970 - WAP WSP Section 8.4.2.3 Panics if 
	the time val is greater then the maximum allowable integer size (32 bits).
	
	@pre iBuffer[iOffset] must be valid, VarType() == TWspHeaderType::ELengthVal  
	@post internal offset gets updated to move past this primitive 
	@param aDateTime Out - a WAP Date
	@return postive number indicating the number of bytes read from the buffer
			  KErrCorrupt if data is not formatted correctly.
*/
EXPORT_C TInt TWspPrimitiveDecoder::Date(TDateTime& aDateTime)
	{
	TUint32 secVal;
	TInt bufLen = LongInt(secVal);
	__ASSERT_ALWAYS(bufLen <= KMaxTInt, User::Panic(KUriPanicCategory, EWspDecoderDateOverflow));
	if (bufLen < KErrNone) return bufLen;

	TDateTime dt(1970,EJanuary,0,0,0,0,0);
	TTime time(dt);

	TInt sec = STATIC_CAST(TInt, secVal);
	time += TTimeIntervalSeconds(sec);
	aDateTime = time.DateTime();

	return bufLen;
	}
