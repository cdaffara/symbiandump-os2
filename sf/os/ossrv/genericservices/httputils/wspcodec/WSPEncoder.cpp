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

#include <wspencoder.h>

//constants
//
const TUint8 KWSPQuoteCharacter	= 0x7F; // QUOTE character as specified in the WSP BNF. 
const TUint8 KWSPQuote			= 0x22; // The regular quote character ".
const TUint8 KCarryBitMask		= 0x80; // Continue bit set
#define KTopBitMask KCarryBitMask		// Mask for checking top bit
const TUint KUintVarIndicator	= 31;	// Byte value indicating a UIntVar follows.
const TUint KUIntVarOctetShift	= 7;	// Octet shift required processing a UnIntVar
const TUint KLongIntOctetShift	= 8;	// Octet shift required processing a LongInt
const TInt KDesArrayGranularity = 6;	// Granularity of descriptor array
_LIT8(KWspStringTerminator, "\0");
_LIT8(KTxtSeparators, "()<>@,;:\\\"/[]?={} "); // Separator characters as defined in RFC2616

// Panic category
//
_LIT(KWspCodecPanicCategory,"WSPCODEC"); 

/**
	Static factory constructor.
	
	@leave			KErrNoMemory
	@return		returns a Pointer to fully constructed CWspHeaderEncoder object.
*/
EXPORT_C CWspHeaderEncoder* CWspHeaderEncoder::NewL()
	{
	CWspHeaderEncoder* self = CWspHeaderEncoder::NewLC();
	CleanupStack::Pop(self);
	return self;
	}
	
/**
 	Static factory constructor.
	
	@leave			KErrNoMemory
	@return		returns a Pointer to fully constructed CWspHeaderEncoder object on the Heap.
*/
EXPORT_C CWspHeaderEncoder* CWspHeaderEncoder::NewLC()
	{
	CWspHeaderEncoder* self = new (ELeave) CWspHeaderEncoder();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/** 
	Default constructor. 
*/
CWspHeaderEncoder::CWspHeaderEncoder()
	{
	}
	
/**
	 Default destructor
*/
EXPORT_C CWspHeaderEncoder::~CWspHeaderEncoder()			
	{
	iArray.ResetAndDestroy();
	}

/** 
	Standard second phase construction. 
*/
void CWspHeaderEncoder::ConstructL()			
	{
	// Create new buffer;
	CDesC8Array* buffer = new (ELeave) CDesC8ArrayFlat(KDesArrayGranularity);
	CleanupStack::PushL(buffer);
	User::LeaveIfError(iArray.Append(buffer));
	CleanupStack::Pop(buffer);
	}
	
/**
  Starts a new encoded header. 
 
  @param 			aToken	field name being encoded as a Token value.
  @leave			KErrNoMemory
*/
EXPORT_C void CWspHeaderEncoder::StartHeaderL(TUint8 aToken)			
	{
	__ASSERT_DEBUG(iTotalLength==0,User::Panic(KWspCodecPanicCategory, EWspCodecPanicStartHeaderCalledTwice));
	AddShortIntL(aToken);
	}
	
/**
  Starts a new encoded header.

  @param 	aString	Fieldname parameter is encoded as a TextString.
  @leave	KErrNoMemory
*/	
EXPORT_C void CWspHeaderEncoder::StartHeaderL(const TDesC8& aString)
	{
	__ASSERT_DEBUG(iTotalLength==0,User::Panic(KWspCodecPanicCategory, EWspCodecPanicStartHeaderCalledTwice));
	AddTextStringL(aString);
	}

/**
  Starts a new encoded header.

  @param 	aString	Fieldname parameter is encoded as a TextString.
  @leave	KErrNotSupported
*/
EXPORT_C void CWspHeaderEncoder::StartHeaderL(const RStringF /* aString */ )
	{
	__ASSERT_DEBUG(iTotalLength==0,User::Panic(KWspCodecPanicCategory, EWspCodecPanicStartHeaderCalledTwice));
	User::Leave(KErrNotSupported);
	}


/**
 	Completes and returns encoded field 8 bit buffer. This method will panic if an 
 	EndValueLengthL() is not called after a StartValueLength().
	
	Note: 
	The final buffer containing the entire encoded header is constructed.
	Returns buffer containing the encoded field constructed 
	from the first call to StartHeaderL. 
					
	@return		Pointer to buffer containing the entire encoded field.
	 			Responsibility for deallocating the memory is also passed.
	@pre 		The function StartHeaderL should have been called.
	@post		Encoder is reset ready to be used again.
	@leave		HBufC8::NewL leaves, if the new 8 bit heap descriptor cannot be created.

*/
EXPORT_C HBufC8* CWspHeaderEncoder::EndHeaderL()
	{
	__ASSERT_DEBUG(iArray.Count()==1,User::Panic(KWspCodecPanicCategory, EWspCodecPanicEndValueLengthNotCalled));
	// concatenate array elements and return.

	HBufC8* outputBuffer = HBufC8::NewL(iTotalLength);

	CDesC8Array* desc = iArray[0];
	TInt count = desc->Count();
	for (TInt jj=0; jj<count; ++jj) 
		{
		(outputBuffer->Des()).Append((*desc)[jj]);
		}

	desc->Reset();
	iTotalLength=0;
	return outputBuffer;
	}

/**
  Encodes input Integer value and adds it to the encoded field. Choice of encoded 
  form dependent on the size of the input.Either ShortInt or LongInt method chosen.
					
  @param 		aInt	Integer value to be encoded.
  @pre			StartHeaderL needs to have been called.
  @leave		KErrNoMemory
*/
EXPORT_C void CWspHeaderEncoder::AddIntegerL(const TUint aInt)
	{
	// Determine if its a short or longInt we want
	(aInt < KTopBitMask) ? AddShortIntL((TUint8) aInt) : AddLongIntL(aInt);
	}

/**
  Encodes input and adds it to the encoded field. Encodes parameter value using WSP 
  ShortInt method.
 					
  @param 			aValue	value to be encoded.
  @pre				StartHeaderL needs to have been called.
  @leave			KErrNoMemory
*/
EXPORT_C void CWspHeaderEncoder::AddShortIntL(const TUint8 aValue)
	{
	const TInt arrayCount=iArray.Count();
	__ASSERT_DEBUG(arrayCount>0,User::Panic(KWspCodecPanicCategory, EWspCodecPanicStartHeaderLNotCalled));
	CDesC8Array* desc=iArray[arrayCount-1];


	// ShortInt shoud be a character 127 or less. With highest bit set to 1.
	TUint8 shortInt = TWspPrimitiveEncoder::ShortInt(aValue);

	desc->AppendL(TPtrC8(&shortInt, 1));
	++iTotalLength;
	}


/**
  Encodes input and adds it to the encoded field. For short length the value must
  be between octet 0 - 31.
 	
  @param 		aValue	value to be encoded.
  @pre			StartHeaderL needs to have been called.
  @leave		KErrNoMemory, KErrOverflow if the value is greater than 31
*/
EXPORT_C void CWspHeaderEncoder::AddShortLengthL(const TUint8 aValue)
	{
	const TInt arrayCount=iArray.Count();
	__ASSERT_DEBUG(arrayCount>0,User::Panic(KWspCodecPanicCategory, EWspCodecPanicStartHeaderLNotCalled));
	CDesC8Array* desc=iArray[arrayCount-1];

	// Check if the value is in the correct range ie octet 0-31
	if(aValue > KUintVarIndicator)
		User::Leave(KErrOverflow);

	desc->AppendL(TPtrC8(&aValue, 1));
	++iTotalLength;
	}

/**
  Encodes input and adds it to the encoded field. Encodes parameter value using WSP 
  LongInt method.
 					
  @param 			aValue	value to be encoded.
  @pre				StartHeaderL needs to have been called.
  @leave			KErrNoMemory
*/
EXPORT_C void CWspHeaderEncoder::AddLongIntL(const TUint32 aValue)
	{
	const TInt arrayCount=iArray.Count();
	__ASSERT_DEBUG(arrayCount>0,User::Panic(KWspCodecPanicCategory, EWspCodecPanicStartHeaderLNotCalled));
	CDesC8Array* desc=iArray[arrayCount-1];

	HBufC8* buf = TWspPrimitiveEncoder::LongIntL(aValue);
	CleanupStack::PushL(buf);

	desc->AppendL(*buf);
	iTotalLength+=buf->Length();

	CleanupStack::PopAndDestroy(buf);
	}
	
/**
  Encodes input and adds it to the encoded field. Encodes parameter value using WSP 
  UIntVar method.
 					
  @param 		aInt	value to be encoded.
  @pre			StartHeaderL needs to have been called.
  @leave		KErrNoMemory
*/
EXPORT_C void CWspHeaderEncoder::AddUintVarL(const TUint aInt)
	{
	const TInt arrayCount=iArray.Count();
	__ASSERT_DEBUG(arrayCount>0,User::Panic(KWspCodecPanicCategory, EWspCodecPanicStartHeaderLNotCalled));
	CDesC8Array* desc=iArray[arrayCount-1];

	HBufC8*	buf = TWspPrimitiveEncoder::UintVarL(aInt);
	CleanupStack::PushL(buf);

	desc->AppendL(*buf);
	iTotalLength+=buf->Length();

	CleanupStack::PopAndDestroy(buf);
	}

/**
  Encodes input and adds it to the encoded field. Encodes parameter value using WSP
  TextString method.
 				   
  @param 			aText		value to be encoded.
  @pre				StartHeaderL needs to have been called.
  @leave			KErrNoMemory
*/
EXPORT_C void CWspHeaderEncoder::AddTextStringL(const TDesC8& aText)
	{	
	const TInt arrayCount=iArray.Count();
	__ASSERT_DEBUG(arrayCount>0,User::Panic(KWspCodecPanicCategory, EWspCodecPanicStartHeaderLNotCalled));
	CDesC8Array* desc=iArray[arrayCount-1];

	HBufC8*	buf = TWspPrimitiveEncoder::TextStringL(aText);
	CleanupStack::PushL(buf);
	desc->AppendL(*buf);
	iTotalLength+=buf->Length();
	CleanupStack::PopAndDestroy(buf);
	}

/**
  Encodes input and adds it to the encoded field. Encodes parameter value using WSP
  TextString method.
  	   	
  @param 			aText		value to be encoded.
  @pre				StartHeaderL needs to have been called.
  @leave			KErrNoMemory
*/
EXPORT_C void CWspHeaderEncoder::AddTextStringL(const RString& /* aText */)
	{
	User::Leave(KErrNotSupported);
	}
	
/**
  Encodes input and adds it to the encoded field.Encodes parameter value using WSP 
  Date method.
 				   
  @param 			aDate		value to be encoded.
  @pre				StartHeaderL needs to have been called.
  @leave			KErrNoMemory
*/
EXPORT_C void CWspHeaderEncoder::AddDateL(const TDateTime aDate)
	{
	const TInt arrayCount=iArray.Count();
	__ASSERT_DEBUG(arrayCount>0,User::Panic(KWspCodecPanicCategory, EWspCodecPanicStartHeaderLNotCalled));
	CDesC8Array* desc=iArray[arrayCount-1];

	HBufC8* buf = TWspPrimitiveEncoder::DateL(aDate);
	CleanupStack::PushL(buf);
	desc->AppendL(*buf);
	iTotalLength+=buf->Length();
	CleanupStack::PopAndDestroy(buf);
	}
	
/**
  Encodes input and adds it to the encoded field. Adds value as-is to the encoded field.
 				   
  @param 	aToken	parameter added without encodeing. Should be a valid WSP token, 
  a 8 bit number > 0x7F (i.e. top bit set).
  @pre 		StartHeaderL and StartValueLengthL should have been called.
  @post		EndValueLengthL needs to be called subsequently.
*/
EXPORT_C void CWspHeaderEncoder::AddTokenL(const TUint8 aToken)
	{
	const TInt arrayCount=iArray.Count();
	__ASSERT_DEBUG(arrayCount>0,User::Panic(KWspCodecPanicCategory, EWspCodecPanicStartHeaderLNotCalled));
	CDesC8Array* desc=iArray[arrayCount-1];

	TUint8 shortInt = (TUint8) (aToken); 
	desc->AppendL(TPtrC8(&shortInt, 1));
	++iTotalLength;
	}
	
/**
  Encodes input and adds it to the encoded field. Encodes parameter value using WSP 
  TokenText method.
 				  
  @param 		aTokenText	value to be encoded.
  @leave		KErrNoMemory
*/
EXPORT_C void CWspHeaderEncoder::AddTokenTextL(const TDesC8& aTokenText)
	{
	const TInt arrayCount=iArray.Count();
	__ASSERT_DEBUG(arrayCount>0,User::Panic(KWspCodecPanicCategory, EWspCodecPanicStartHeaderLNotCalled));

	// Step through token text passed in and ensure there are no invalid characters
	const TInt tokenTextLength = aTokenText.Length();
	for( TInt ii = 0; ii<tokenTextLength; ++ii)
		{
		TUint8 currentChar = aTokenText[ii];
		if( KTxtSeparators().Locate(currentChar) != KErrNotFound )
			User::Leave(KErrCorrupt);
		}
	// Token text does not contain any invalid characters
	HBufC8* buf = TWspPrimitiveEncoder::TextStringL(aTokenText);
	CleanupStack::PushL(buf);
	CDesC8Array* desc=iArray[arrayCount-1];
	desc->AppendL(*buf);
	iTotalLength += buf->Length();
	CleanupStack::PopAndDestroy(buf);
	}

/**
  Encodes input and adds it to the encoded field. Adds value as-is to the encoded field.
 					
  @param 			aData		value to be encoded.
  @leave			KErrNoMemory
*/
EXPORT_C void CWspHeaderEncoder::AddDataL(const TDesC8& aData)
	{
	const TInt arrayCount=iArray.Count();
	__ASSERT_DEBUG(arrayCount>0,User::Panic(KWspCodecPanicCategory, EWspCodecPanicStartHeaderLNotCalled));
	CDesC8Array* desc=iArray[arrayCount-1];
	desc->AppendL(aData);
	iTotalLength += aData.Length();
	}

/**
  From calling this function, the length in bytes of all encodings added subsequently will
  be calculated and stored as part of the encoded string, as specified in WSP spec.Can be nested. i.e.
  @code
	 encoder->StartHeaderL();
	 encoder->StartValueLengthL();
	 encoder->StartValueLengthL();
	 encoder->AddLongIntL();
	 encoder->EndValueLengthL();
	 encoder->AddTextStringL();
	 encoder->EndValueLengthL();
	 HBufC8* output = encoder->EndHeaderL();
  @endcode
 	
  @pre 			StartHeaderL should have been called.
  @post			EndValueLengthL needs to be called subsequently.
  @leave		KErrNoMemory
*/
EXPORT_C void CWspHeaderEncoder::StartValueLengthL()
	{
	// Create new buffer;
	CDesC8Array* buffer = new (ELeave) CDesC8ArrayFlat(KDesArrayGranularity);
	CleanupStack::PushL(buffer);
	User::LeaveIfError(iArray.Append(buffer));
	CleanupStack::Pop(buffer);
	}

/**
  Needs to be called at the point in the construction of a header when ValueLength 
  can be calculated.
 					  
  @pre 			StartHeaderL and StartValueLengthL should have been called.
  @post			ValueLength has been calculated and added, together with the
 				encoded header, to the internal representation of the header buffer.
  @leave		KErrNoMemory
*/
EXPORT_C void CWspHeaderEncoder::EndValueLengthL()
	{
	const TInt arrayCount=iArray.Count();
	__ASSERT_ALWAYS(arrayCount>1,User::Panic(KWspCodecPanicCategory, EWspCodecPanicStartValueLengthNotCalled));

	// Calculate the length of the current buffer.
	// and append it onto the previous buffer. [length value, then data]
	TUint32 valueLength=0;


	CDesC8Array* desc=iArray[arrayCount-1]; // current descriptor being dealt with
	CDesC8Array* parentDesc=iArray[arrayCount-2]; // parent descriptor to which it must be added.

	TInt buffersToAdd=desc->Count();
	TInt ii=buffersToAdd;

	// Check the length of all parameters (not the first element in tha array, the field name)
	while (ii)
		valueLength+=(*desc)[--ii].Length();
	
	// Remove desc from array. Will have to delete also.
	iArray.Remove(arrayCount-1);
	CleanupStack::PushL(desc);

	// Depending of size of the length save as number or UintVar
	if (valueLength < KUintVarIndicator)
		{
		// Value length represented by an 8 bit number.
		AddShortLengthL( (TUint8) valueLength);
		}
	else
		{
		// Value length represented by an 8bit value indicating a UIntVar follows,
		// followed by a UIntVar
		AddShortLengthL( (TUint8) KUintVarIndicator);
		AddUintVarL(valueLength);
		}

	// Add field value, parameters etc.
	ii=0;
	while (ii<buffersToAdd)
		parentDesc->AppendL((*desc)[ii++]);

	CleanupStack::PopAndDestroy(desc);
	}




//**********************************************************************************

/**
  Takes a TUint8 parameter, and sets the top bit. As specified for the WSP ShortInt 
  encoding method.
 					
  @param 		aValue number to be encoded.
  @return 		Output, encoded as a TUint8, representation of the header buffer.
 				If input greater that 127 (invalid input), returns 0
*/
EXPORT_C TUint8 TWspPrimitiveEncoder::ShortInt(const TUint8 aValue)
	{
	// ShortInt should be a character 127 or less. With highest bit set to 1.
	return (aValue > KWSPQuoteCharacter) ? (TUint8) 0: (TUint8) (aValue | KTopBitMask); 
	}

/**
  Takes a TUint32 parameter and encodes it using the WSP specified LongInt method.
  	
  @param 			aValue number to be encoded.
  @return 			Output, encoded HBufC8 buffer. 
  @leave			KErrNoMemory
*/
EXPORT_C HBufC8* TWspPrimitiveEncoder::LongIntL(const TUint32 aValue)
	{
	// Consists of size and up to number of 30 bytes.
	// for a TInt32 the maximum is 4 bytes long.
	// Check size of number, to determine number of bytes needed to store it.

	TUint8 size = 0; // maximum value is 4 with a 32bit integer
	TUint32 value=aValue;
	do {
		++size;
		value >>=KLongIntOctetShift; ; // shift by 8 bits.
		} while (value>0);

	HBufC8* output = HBufC8::NewL(size+1);
	TPtr8 outPtr(output->Des());

	outPtr.Append(size);
	TInt ii = size;
	while (ii-- >0) 
		{		
		outPtr.Append( (TUint8) (aValue>>ii*KLongIntOctetShift) );
		}

	return output;
	}

/**
  Takes a TUint32 parameter and encodes it using the WSP specified UintVar method.
  					
  @param 			aInt number to be encoded.
  @return 			Output, encoded HBufC8 buffer. 
  @leave			KErrNoMemory
*/
EXPORT_C HBufC8* TWspPrimitiveEncoder::UintVarL(const TUint32 aInt)
	{
	TUint8 size = 0; // maximum value is 5 with a 32bit integer
	TUint32 value=aInt;
	do {
		++size;
		value >>=KUIntVarOctetShift; ; // shift by 7 bits.
		} while (value>0);

	HBufC8* output = HBufC8::NewL(size);
	TPtr8 outPtr(output->Des());

	TInt ii = size; 
	while (--ii > 0)
		{
		outPtr.Append( (TUint8)(aInt>>(KUIntVarOctetShift*(ii))  & KWSPQuoteCharacter) | KCarryBitMask); 
		} 

	// Finally the first 7 bits, last octet, do not set first bit.
	outPtr.Append( (TUint8)(aInt & KWSPQuoteCharacter) ); // Add even if 0 value.

	return output;
	}

/**
  Takes a RString parameter and encodes it using the WSP specified TextString method.
  					
  @param 			aText string to be encoded.
  @return 			Output, encoded HBufC8 buffer. 
  @leave			KErrNoMemory
*/
EXPORT_C HBufC8* TWspPrimitiveEncoder::TextStringL(const RString  /* aText*/ )
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

/**
  Takes a TDesC8 parameter and encodes it using the WSP specified TextString method.
  					
  @param 			aText string to be encoded.
  @return 			Output, encoded HBufC8 buffer. 
  @leave			KErrNoMemory
*/
EXPORT_C HBufC8* TWspPrimitiveEncoder::TextStringL(const TDesC8& aText)
	{
	HBufC8* output=NULL;
	TInt stringLength = aText.Length();
	TUint8 firstChar = 0;
	TUint8 lastChar = 0;
	if(stringLength>0)
		{
		firstChar = aText[0];
		lastChar = aText[stringLength-1];
		}

	TPtr8 outPtr(NULL,0);
	if (firstChar > KWSPQuoteCharacter)
		{
		// Apply WSP rule: first character of the string not 7bit add QuoteCharacter.
		// Add the quote character and include space for the NULL character
		stringLength+=2;
		output =  HBufC8::NewL(stringLength); 
		outPtr.Set(output->Des());

		outPtr.Append(KWSPQuoteCharacter);
		outPtr.Append(aText);
		}
	else if (firstChar==KWSPQuote && lastChar==KWSPQuote)
		{
		// Apply WSP rule: if quoted string, remove the closing quote
		output =  HBufC8::NewL(stringLength); 
		outPtr.Set(output->Des());
		outPtr.Append(aText);
		outPtr.SetLength(stringLength-1);
		}
	else
		{
		stringLength+=1; // terminating NULL char
		output =  HBufC8::NewL(stringLength); 
		outPtr.Set(output->Des());
		outPtr.Append(aText);
		}

	// Terminate string with 0x00
	outPtr.Append(KWspStringTerminator);
	return output;
	}

/**
  Takes a TDateTime parameter and encodes it using the WSP specified Date encoding method.
  
  @param 			aDate value to be encoded.
  @return 			Output, encoded HBufC8 buffer. 
  @leave			KErrNoMemory
*/
EXPORT_C HBufC8* TWspPrimitiveEncoder::DateL(const TDateTime aDate)
	{
	TTime baseTime(TDateTime(1970,EJanuary,0,0,0,0,0));
	TTime dateTime(aDate);
	TTimeIntervalSeconds interval;
	dateTime.SecondsFrom(baseTime, interval);

	return LongIntL(interval.Int());
	}

