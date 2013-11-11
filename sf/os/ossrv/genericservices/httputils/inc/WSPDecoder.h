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

/**
 @file WSPDecoder.h
 @publishedAll
 @deprecated
*/

#ifndef	__WSPDECODER_H__
#define	__WSPDECODER_H__

#include <e32base.h>
#include <stringpool.h>


/** 
* This file contains the following classes:
*	TWspField 
*	TWspHeaderSegmenter 
*	TWspPrimitiveDecoder  
*	
*/


/**
enum DecoderPanic
@publishedAll
@deprecated
*/
enum TWspDecoderPanic
	{
	/**
	LongIntOverflow
	*/
	EWspDecoderLongIntOverflow,
	/**
	DateOverflow
	*/
	EWspDecoderDateOverflow
	};

/**
TWspField class holds the pair <HeaderName, ValueBuffer>
This is a simple class that associates these values.
No ownership is handed to this class.  It is up to user of this class to
open and close all resources used.
@publishedAll
@deprecated
*/
class TWspField 
	{
public:

	inline TWspField();

	inline TWspField(RStringF aHdrName, TPtrC8 aValBuf);

	/**
	The header name of a opened RStringF - *Not Owned*
	This is externally opened.  It must be externally 
	closed.
	*/
	RStringF iHdrName;
	
	/** The raw value buffer - Note: Not owned by this */
	TPtrC8	 iValBuffer;
	};

/** 
TWspHeaderSegmenter segments a WSP buffer into WSP header/value pairs.  
It detects boundaries between header/values based on the WAP-WSP spec.
	- To construct, buffer and string pool is passed in
	- Call to NextL() to iterate through the buffer - this returns a TWspField
	- NextL() returns KErrNotFound when done

@publishedAll
@deprecated
*/ 
class TWspHeaderSegmenter
	{
public:

	inline TWspHeaderSegmenter(RStringPool aPool, const TStringTable& aStringTable, TPtrC8 aBuffer); 

	IMPORT_C TInt NextL(TWspField& aWspHeader);

	inline TInt Offset() const;

private:
	/** Raw buffer that will be segmented - Not Owned */
	TPtrC8		iBuffer;

	/** Segment offset into the buffer. */
	TInt		iOffset;

	/** Opened string pool to use with the string table already loaded - Not Owned */
	RStringPool iPool;

	/** The string table to use in the string pool - Not Owned */
	const TStringTable& iStringTable;
	};

/** 
Decoder for WSP Primitves - WAP-WSP Section 8.4.1
@publishedAll
@deprecated
*/ 
class TWspPrimitiveDecoder 
	{
  public:

	/**
	* TWspHeaderType describe the types from WAP-WSP Section 8.4.1.2
	*/
	 enum TWspHeaderType
		{
		/**
		The type has not been set 
		*/
		ENotSet,		
		/**
		0-31 -  octet is a value length 
		*/
		ELengthVal,		
		/**
		34 - value is a quoted text string, terminated by a Null
		*/
		EQuotedString,	
		/** 
		32-127 - value is a text string, terminated by a Null
		*/
		EString,		
		/**
		128-255 - encoded 7 bit value, this header has no more data	
		*/
		E7BitVal		
		};

			
	inline TWspPrimitiveDecoder(TPtrC8 aBuffer);
	
	IMPORT_C TWspHeaderType VarType() const;
	
	IMPORT_C TInt LengthVal(TInt& aVal);
	
	IMPORT_C TInt String(TPtrC8& aString);


	IMPORT_C TInt Val7Bit(TUint8& aVal);
	
	IMPORT_C TInt Integer(TUint32& aVal);

	IMPORT_C TInt LongInt(TUint32& aVal);

	IMPORT_C TInt UintVar(TUint32& aVal);

	IMPORT_C TInt VersionL(RStringPool aPool, RStringF& aVer);

	IMPORT_C TInt Date(TDateTime& aDateTime);
  
  private:
	/** The raw buffer */
	TPtrC8 iBuffer;

	/** The current offset */
	TInt   iOffset;
	};

/**
  Constructor
  
 */
inline TWspField::TWspField()
	{
	}

/**
  Constructor
  
  @param aHdrName In - The Header Name.  This must be an opened RStringF
  @param aValBuf In - the Buffer containing the header value in its raw format
*/
inline TWspField::TWspField(RStringF aHdrName, TPtrC8 aValBuf) : 
		iHdrName(aHdrName), 
		iValBuffer(aValBuf) 
	{
	}

/**
  Constructor

  @param aPool In - an opened RStringPool - owned by the caller
  @param aStringTable In - the string table in the string pool to use
  @param aBuffer In - the buffer containing the WSP header data - owned by the caller
  @pre The string table must be opened with the WSP Sting constants table
*/
inline TWspHeaderSegmenter::TWspHeaderSegmenter(RStringPool aPool, const TStringTable& aStringTable, TPtrC8 aBuffer) : 
		iBuffer(aBuffer), 
		iOffset(0),
		iPool(aPool),
		iStringTable(aStringTable)
	{
	}

/**
  Offset	returns the current offset into the buffer being parsed.
  
  @return	TInt offset value. It will point to beginning of next segmented field.
 			If NextL has not been called it will be set to 0. The beginning of the buffer.
 			If buffer has been completely parsed, will return KErrNotFound.
*/
inline TInt TWspHeaderSegmenter::Offset() const
	{
	return (iOffset < iBuffer.Length()) ? iOffset : KErrNotFound;	
	}

/**
  Constructor
  
  @param aBuffer In - the buffer containing the value in its raw format
*/
inline TWspPrimitiveDecoder::TWspPrimitiveDecoder(TPtrC8 aBuffer) : 
		iBuffer(aBuffer), 
		iOffset(0) 
	{
	}


#endif // __WSPDECODER_H__


