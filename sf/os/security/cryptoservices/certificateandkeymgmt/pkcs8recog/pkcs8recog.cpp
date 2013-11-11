/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include <asnpkcs.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

#include "pkcs8recog.h"

const TInt KRecognizerValue = 0x1020361C;
const TUid KUidMimeRecognizer = {KRecognizerValue};

_LIT8(KDataTypePkcs8KeyPair, "application/pkcs8");
_LIT8(KDataTypePkcs8EncryptedKeyPair, "application/pkcs8-encrypted");

const TInt KSupportedDataTypesNumber = 2;


// ----------------------------------------------------------------------------
// CApaPkcs8Recognizer
//

CApaPkcs8Recognizer::CApaPkcs8Recognizer()
	: CApaDataRecognizerType(KUidMimeRecognizer, CApaDataRecognizerType::ENormal)
	{
	iCountDataTypes = KSupportedDataTypesNumber;
	}

TUint CApaPkcs8Recognizer::PreferredBufSize()
	{
	return Max(KIsPKCS8DataMinLength, KIsEncryptedPKCS8DataMinLength);
	}

TDataType CApaPkcs8Recognizer::SupportedDataTypeL(TInt aIndex) const
	{
	__ASSERT_DEBUG(aIndex >= 0 && aIndex < KSupportedDataTypesNumber,
					User::Panic(_L("PKCS8RECOG"), 0));
	switch (aIndex)
		{
		case 0:
			return TDataType(KDataTypePkcs8KeyPair);
	
		case 1:
			return TDataType(KDataTypePkcs8EncryptedKeyPair);
			
		// Used to prevent warning about return paths not all returning a value
		default:
			return TDataType(KDataTypePkcs8KeyPair);
		}
	}

void CApaPkcs8Recognizer::DoRecognizeL(const TDesC& /*aName*/, const TDesC8& aBuffer)
	{
	// Ensure length is sufficient for checking type pkcs8
	if (aBuffer.Size() >= KIsPKCS8DataMinLength)
		{
		if (TASN1DecPKCS8::IsPKCS8Data(aBuffer))
			{
			iDataType = TDataType(KDataTypePkcs8KeyPair);
			iConfidence = ECertain;
			return;
			}
		}
		
	// Ensure length is sufficient for checking type pkcs8-encrypted	
	if (aBuffer.Size() >= KIsEncryptedPKCS8DataMinLength) 
		{
		if (TASN1DecPKCS8::IsEncryptedPKCS8Data(aBuffer))
			{
			iDataType = TDataType(KDataTypePkcs8EncryptedKeyPair);
			iConfidence = ECertain;
			return;
			}
		}
				
	// type not recognized
	}

CApaDataRecognizerType* CApaPkcs8Recognizer::CreateRecognizerL()
	{
	return new (ELeave) CApaPkcs8Recognizer();
	}

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x1020361B, CApaPkcs8Recognizer::CreateRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}	

