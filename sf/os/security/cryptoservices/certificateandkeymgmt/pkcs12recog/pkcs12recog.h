/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


/**
 @file
 @internalTechnology 
*/

#ifndef __PKCS12RECOG_H__

#include <apmrec.h>

/**
 A recogniser that recognises the following MIME types:
 application/x-pkcs12
 */
class CPkcs12Recognizer : public CApaDataRecognizerType
	{
	/** PKCS#12 recognizer panic codes */
	enum TPkcs12RecogPanic 
		{
		/** Data type index does not correspond to a mime-type */
		EPanicInvalidDataType
		};
		
public:
	/**
	Constructor
	*/
	CPkcs12Recognizer();

public: 
	/**
	Returns the preferred buffer size for PKCS#12 recognition
	@return preferred buffer size in bytes
	*/
	TUint PreferredBufSize();
	
	/**
	Allows a client to enumerate the supported mime-types.
	@param aIndex index of the mimetype to return
	@return mime-type corresponding to aIndex
	*/
	TDataType SupportedDataTypeL(TInt aIndex) const;
	
	static CApaDataRecognizerType* CreateRecognizerL();

private:
	// Implementation CApaDataRecognizerType::DoRecognizeL
	void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
	
	/**
	Checks whether the file name has a known PKCS#12 extension
	@param aName the file name to examine
	@return ETrue if the file extension looks is .pfx or .p12;
		    otherwise, EFalse is returned.
	*/
	TBool HasPkcs12Extension(const TDesC& aName);
		
	/**	
	Checks whether the buffer contains a valid PKCS#12 header.
	aBuffer buffer to recognise

	Expected ASN.1 sequence
	SEQ
		INTEGER  	// Version = 3
		SEQ		 	// authSafe - PKCS#7 ContentInfo
			OID		// ContentType (data or signed data)

	It is not practical to check further than this because the content
	field within the ContentInfo objects is optional and could be absent.
	@param aBuffer the buffer to check
	@return ETrue if the buffer contains a PKCS#12 header; 
			otherwise, EFalse is returned.	
	*/
	TBool DoRecognizeBufferL(const TDesC8& aBuffer);
	
	// There is no need to validate the lengths because the recogniser
	// checks the buffer size is at least as large as the minimum header
	// size	
	
	/**
	Checks that the data at the specified offset is a DER sequence tag
	and advances past the tag and it's length.
	
	@param aBuffer the buffer containing the DER sequence to validate
	@param aOffset the offset of the current byte within the buffer. This
	               is undefined if an error occurs.
	@return ETrue if a valid sequence tag & length is encountered;
			otherwise, EFalse is returned.
	*/
	TBool ConsumeSequenceL(const TDesC8& aBuffer, TUint& aOffset) const;

	/**
	Decodes a DER encoded integer at the specified offset and advances
	to the next element.
	Signed integers greater than 32 bits in length are not supported.	
	
	@param aBuffer the buffer containing the DER intger to decode
	@param aOffset the offset of the current byte within the buffer. This
	               is undefined if an error occurs.
	@param aIntVal the decoded integer value. This is undefined if an error occurs.
	@return ETrue if a valid integer is encountered; 
			otherwise, EFalse is returned.
	*/	
	TBool ConsumeIntegerL(const TDesC8& aBuffer, TUint& aOffset, TInt& aIntVal) const;
	
	/**
	Decodes a DER encoded length at the specified offset and advances
	to the start of the value.
	Lengths greater than 32 bits in length are not supported.
	
	@param aBuffer the buffer containing the length to decode.
	@param aOffset the offset of the current byte within the buffer. This
	               is undefined if an error occurs.
	@param aLength the decoded length value in octets. This is undefined if an error occurs.
	@return ETrue if the length is valid; otherwise, EFalse is returned.
	*/			
	TBool ConsumeLengthL(const TDesC8& aBuffer, TUint& aOffset, TInt& aLengthOctets) const;
	
	/**
	Decodes base256 encoded integer up to 4 bytes in length and advances
	past the data.
	Signed integers greater than 32 bits in length are not supported.
	
	@param aBuffer the buffer containing the octets to decode.
	@param aOffset the offset of the current byte within the buffer. This
	               is undefined if an error occurs.
	@param aLength the number of octets to decode (must be <= 4)
	@param aIntVal the decoded integer. This is undefined if an error occurs.
	*/		
	TBool ConsumeBase256L(const TDesC8& aBuffer, TUint& aOffset, TInt aLengthOctets, TInt& aIntVal) const;
	
	/**
	Calls panic with PKCS#12 recognizer category with the supplied panic code.
	@param aReason the panic code
	*/
	void Panic(TPkcs12RecogPanic aReason) const;
	};

#endif
