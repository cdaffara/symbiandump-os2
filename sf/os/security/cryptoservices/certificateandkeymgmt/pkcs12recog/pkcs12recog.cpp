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


#include "pkcs12recog.h"

#include <apmrec.h>
#include <apmstd.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <f32file.h>

/** UID for PKCS#12 mime-type recognizer */
static const TUint KPkcs12RecognizerImplementationId = 0x20001520;
static const TUid KUidMimePkcs12Recognizer = { KPkcs12RecognizerImplementationId };

/** 
Minimum buffer size assuming worst-case 3 byte lengths for variable
length fields. The file will never be smaller that this because it must
also contain either the MacData or be a signed object.

SEQ 5 bytes
INTEGER 3 bytes
SEQ 5 bytes
OID 11 bytes

total = 24
*/
static const TInt KPkcs12MinBufSize = 24;

/** Mime-type for PKCS#12 */
_LIT8(KDataTypePkcs12, "application/x-pkcs12");
/** The number of mime-types recognised */
static const TInt KSupportedDataTypesTotal = 1;

// ASN.1 / DER constants
/** DER encoding of an ASN.1 sequence tag */
static const TUint8 KDerSequenceOctet = 0x30;
/** DER encoding of an ASN.1 integer tag */
static const TUint8 KDerIntegerOctet = 0x02;
/** Expected PKCS#12 version number */
static const TInt KPkcs12Version = 3;

/**
DER encoding of PKCS#7 data content type OID
OID = 1.2.840.113549.1.7.1
*/		
static const TUint8 KPkcs7DataOid[] = 
	{
	0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x01
	};	
/** Length of encoded PKCS#7 data OID */	
static const TUint KPkcs7DataOidLen = sizeof(KPkcs7DataOid);		

/**
DER encoding of PKCS#7 signed data content type OID
OID = 1.2.840.113549.1.7.2
*/	
static const TUint8 KPkcs7SignedDataOid[] = 
	{
	0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x02
	};	
/** Length of encoded PKCS#7 signed data OID */	
static const TUint KPkcs7SignedDataOidLen = sizeof(KPkcs7SignedDataOid);	

/** PKCS#12 recognizer panic identifier */
_LIT(KPkcs12RecogPanic, "PKCS12RECOG");	
	
CPkcs12Recognizer::CPkcs12Recognizer()
	: CApaDataRecognizerType(KUidMimePkcs12Recognizer, CApaDataRecognizerType::ENormal)
	{
	iCountDataTypes = KSupportedDataTypesTotal;
	}

TUint CPkcs12Recognizer::PreferredBufSize()
	{
	return KPkcs12MinBufSize;
	}

TDataType CPkcs12Recognizer::SupportedDataTypeL(TInt aIndex) const
	{
	__ASSERT_DEBUG(aIndex >= 0 && aIndex < KSupportedDataTypesTotal,
					Panic(EPanicInvalidDataType));
	
	if (aIndex < 0 || aIndex >= KSupportedDataTypesTotal)
		{
		User::Leave(KErrArgument);
		}
						
	return TDataType(KDataTypePkcs12);
	}

void CPkcs12Recognizer::DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer)
	{
	__UHEAP_MARK;
	iConfidence = ENotRecognized;
	
	TPtrC8 pkcs12Buffer(aBuffer);
	TBuf8<KPkcs12MinBufSize> fileBuffer;
	
	if (aBuffer.Length() < KPkcs12MinBufSize)
		{										
		if (RFile* fh = FilePassedByHandleL()) 
			{
			User::LeaveIfError(fh->Read(0, fileBuffer, KPkcs12MinBufSize));
			}
		else 
			{
			// no file handle so attempt to read the file. This may
			// fail if the file is in a private directory
			RFs fs;
			User::LeaveIfError(fs.Connect());
			CleanupClosePushL(fs);
			RFile file;			
			TInt err = file.Open(fs, aName, EFileRead | EFileShareAny | EFileStream);
			
			// do nothing if the file fails to open, for any reason
			if (err == KErrNone)
				{
				CleanupClosePushL(file);
				User::LeaveIfError(file.Read(0, fileBuffer, KPkcs12MinBufSize));
				CleanupStack::PopAndDestroy(&file);
				}
			CleanupStack::PopAndDestroy(&fs);
			}		
		// buffer does not exist or is too small so attempt to 
		// read a buffer from the file		
		pkcs12Buffer.Set(fileBuffer);			
		}
		

	if (pkcs12Buffer.Length() > 0 && DoRecognizeBufferL(pkcs12Buffer))
		{
		iDataType = TDataType(KDataTypePkcs12);
		iConfidence = EProbable;
		if (HasPkcs12Extension(aName)) 
			{
			iConfidence = ECertain;
			}
		}
	__UHEAP_MARKEND;
 	}
	
TBool CPkcs12Recognizer::HasPkcs12Extension(const TDesC& aName)
	{
	_LIT(KPfxExt, ".pfx");
	_LIT(KP12Ext, ".p12");
	
	TBool match = EFalse;
	
	// It is not possible to use a TParse/TParsePtr here because the filename
	// supplied when a file-handle is passed is of the form ::filename.ext and
	// is not recognised as a valid filename
	if (aName.Length() > 4)
		{		
		const TPtrC ext = aName.Right(4);
		if (ext.CompareF(KPfxExt) == 0 || ext.CompareF(KP12Ext) == 0)
			{
			match = ETrue;
			}
		}
	return match;
	}

TBool CPkcs12Recognizer::DoRecognizeBufferL(const TDesC8& aBuffer)
 	{
 	TBool isPkcs12 = EFalse;
 	TUint offset = 0;
 	
 	if (aBuffer.Length() >= KPkcs12MinBufSize)
 		{
		// PFX
	 	if (ConsumeSequenceL(aBuffer, offset))
 			{
 			// Version
 			TInt version; 
			if (ConsumeIntegerL(aBuffer, offset, version)) 
				{
				if (version == KPkcs12Version)
					{
					// authSafe
					if (ConsumeSequenceL(aBuffer, offset))
						{
						const TPtrC8 dataOid(KPkcs7DataOid, KPkcs7DataOidLen);
						const TPtrC8 signedDataOid(KPkcs7SignedDataOid, KPkcs7SignedDataOidLen);
						// check whether content-type is data or signed data					
						if ((aBuffer.Mid(offset, KPkcs7DataOidLen).Compare(dataOid) == 0) ||
							(aBuffer.Mid(offset, KPkcs7SignedDataOidLen).Compare(signedDataOid) == 0))
							{
							isPkcs12 = ETrue;
							}
						}
					}
				}
 			} 		
 		}
 	return isPkcs12;
 	}

TBool CPkcs12Recognizer::ConsumeSequenceL(const TDesC8& aBuffer, TUint& aOffset) const
	{
	TBool isSequence = EFalse;
	if (aBuffer[aOffset] == KDerSequenceOctet)
		{
		// sequence tag found, skip tag and length
		aOffset++;
		TInt length;
		isSequence = ConsumeLengthL(aBuffer, aOffset, length);
		}
	return isSequence;
	}

TBool CPkcs12Recognizer::ConsumeLengthL(const TDesC8& aBuffer, TUint& aOffset, TInt& aLengthOctets) const
	{	
	TBool isValidLength = EFalse;	
	TUint8 lengthOctet = aBuffer[aOffset];
	if (lengthOctet & 0x80)
		{
		// The top bit is set so assume the length encoding is in long form
		TUint numOctets = lengthOctet & 0x7f;
		aOffset++;
		
		if (ConsumeBase256L(aBuffer, aOffset, numOctets, aLengthOctets))
			{			
			if (aLengthOctets >= 0)
				{
				// lengths must not be -ve
				isValidLength = ETrue;
				}
			}
		}
	else 
		{
		// Top bit zero so assume short form encoding i.e. one octet
		aLengthOctets = lengthOctet & 0x7f;
		aOffset++;
		isValidLength = ETrue;
		}
	return isValidLength;
	}
	
TBool CPkcs12Recognizer::ConsumeIntegerL(const TDesC8& aBuffer, TUint& aOffset, TInt& aIntVal) const
	{
	TBool isValidInteger = EFalse;
	if (aBuffer[aOffset] == KDerIntegerOctet) 
		{
		aOffset++;
		TInt length;
		if (ConsumeLengthL(aBuffer, aOffset, length))
			{			
			isValidInteger = ConsumeBase256L(aBuffer, aOffset, length, aIntVal);
			}
		}
	return isValidInteger;
	}

TBool CPkcs12Recognizer::ConsumeBase256L(const TDesC8& aBuffer, TUint& aOffset, TInt aLengthOctets, TInt& aIntVal) const
	{	
	TInt isValid = EFalse;
	if (aLengthOctets <= 4) 
		{
		aIntVal = 0;
		while (aLengthOctets-- > 0)
			{
			aIntVal <<= 8;
			aIntVal |= aBuffer[aOffset++];
			}
		isValid = ETrue;
		}
	return isValid;	
	}	

CApaDataRecognizerType* CPkcs12Recognizer::CreateRecognizerL()
	{
	return new (ELeave) CPkcs12Recognizer();
	}

static const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KPkcs12RecognizerImplementationId, CPkcs12Recognizer::CreateRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}	

void CPkcs12Recognizer::Panic(TPkcs12RecogPanic aReason) const
	{
	User::Panic(KPkcs12RecogPanic, aReason);
	}
