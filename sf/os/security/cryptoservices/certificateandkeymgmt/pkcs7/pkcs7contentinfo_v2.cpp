/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

 
#include "pkcs7contentinfo_v2.h"
#include "pkcs7asn1.h"

EXPORT_C CPKCS7ContentInfo* CPKCS7ContentInfo::NewL(const TDesC8& aRawData)
	{
	CPKCS7ContentInfo* self = new (ELeave) CPKCS7ContentInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aRawData);
	CleanupStack::Pop(self);
	return self;	
	}

CPKCS7ContentInfo::~CPKCS7ContentInfo()
	{
	}

EXPORT_C CPKCS7ContentInfo::TContentInfoType CPKCS7ContentInfo::ContentType() const
	{
	return iContentType;
	}

EXPORT_C const TPtrC8 CPKCS7ContentInfo::ContentData() const
	{
	return iContentData;	
	}

CPKCS7ContentInfo::CPKCS7ContentInfo(void)
	{
	}

void CPKCS7ContentInfo::ConstructL(const TDesC8& aRawData)
	{  
	const TInt minItems = 1;	// Must have OID
	const TInt maxItems = 2;	// May have data

	CArrayPtr<TASN1DecGeneric>* contentInfo = PKCS7ASN1::DecodeSequenceLC(aRawData, minItems, maxItems);

    const TASN1DecGeneric* contentInfoAt0 = contentInfo->At(0);
	// Checks its a oid id
	if(contentInfoAt0->Tag()==EASN1ObjectIdentifier || contentInfoAt0->Class() == EUniversal)
		{
		// Gets the oid
		TASN1DecObjectIdentifier oidDec;
		HBufC* oidVal = oidDec.DecodeDERL(*contentInfo->At(0));
		CleanupStack::PushL(oidVal);
		// Data itself is optional so make sure its there
		if(contentInfo->Count() == 2)
			{
			const TASN1DecGeneric* contentInfoAt1 = contentInfo->At(1);
			if ( contentInfoAt1->Tag() == 0 || contentInfoAt1->Class() == EContextSpecific )
				{
				TASN1DecGeneric decGen(contentInfoAt1->GetContentDER());
				decGen.InitL();
	            if(*oidVal == KPkcs7DataOID)
					{
					// ContentData is OctetString if ContentType is Data
					if(decGen.Tag() == EASN1OctetString)   
						{
						iContentData.Set(decGen.GetContentDER()); 
						}
					else
						{
						User::Leave(KErrArgument);
						}
					}
				else
					{
					iContentData.Set(decGen.Encoding());
					}
				}
			}
		else
			{
			iContentData.Set(KNullDesC8());
			}
		// Checks if it is data OID.
		if(*oidVal == KPkcs7DataOID)
			{
			// The Content Type is indicated by an Integer.
			// Here if Content Type is equal to Data then,it is represented by 1
			// 1 indicates the numeric value of last element in the PKCS7 Data OID.
			iContentType = EContentTypeData;
			}
		else if(*oidVal == KPkcs7SignedDataOID)
			{
			// The Content Type is indicated by an Integer.
			// Here if Content Type is equal to SignedData then,it is represented by 2
			// 2 indicates the numeric value of last element in the PKCS7 SignedData OID.
			iContentType = EContentTypeSignedData;
			}
		else if(*oidVal == KPkcs7EnvelopedDataOID) 
			{
			// The Content Type is indicated by an Integer.
			// Here if Content Type is equal to EnvelopedData then,it is represented by 3.
			// 3 indicates the numeric value of last element in the PKCS7 EnvelopedData OID.
			iContentType = EContentTypeEnvelopedData;
			}
		else if(*oidVal == KPkcs7SignedAndEnvelopedDataOID)
			{
			// The Content Type is indicated by an Integer.
			// Here if Content Type is equal to SignedAndEnvelopedData then,it is represented by 4.
			// 4 indicates the numeric value of last element in the PKCS7 SignedAndEnvelopedData OID.
			iContentType = EContentTypeSignedAndEnvelopedData;
			}
		else if(*oidVal == KPkcs7DigestedDataOID)
			{
			// The Content Type is indicated by an Integer.
			// Here if Content Type is equal to DigestedData then,it is represented by 5.
			// 5 indicates the numeric value of last element in the PKCS7 DigestedData OID.
			iContentType = EContentTypeDigestedData;
			}
		else if(*oidVal == KPkcs7EncryptedDataOID)
			{
			// The Content Type is indicated by an Integer.
			// Here if Content Type is equal to EncryptedData then,it is represented by 6
			// 6 indicates the numeric value of last element in the PKCS7 EncryptedData OID.
			iContentType = EContentTypeEncryptedData;
			}
		else
			{
			User::Leave(KErrNotSupported);
			}
		CleanupStack::PopAndDestroy(oidVal);
		}
	else
		{
		User::Leave(KErrArgument);
		}
	CleanupStack::PopAndDestroy(contentInfo);
	}
