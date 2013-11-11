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


#include <cmscontentinfo.h>
#include <asn1dec.h>
#include <asn1enc.h> 
#include "pkcs7asn1.h"
#include "cmsutils.h"

//
//Implementation of CMS ContentInfo Identity
//

EXPORT_C CCmsContentInfo* CCmsContentInfo::NewLC(const TDesC8& aRawData)
	{
	CCmsContentInfo* self = new (ELeave) CCmsContentInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aRawData);
	return self;			
	}

EXPORT_C CCmsContentInfo* CCmsContentInfo::NewL(const TDesC8& aRawData)
	{
	CCmsContentInfo* self = NewLC(aRawData);
	CleanupStack::Pop(self);
	return self;			
	}

EXPORT_C CCmsContentInfo* CCmsContentInfo::NewL(TCmsContentInfoType aContentInfoType, const TDesC8& aContentData)
	{
	CCmsContentInfo* self = NewLC(aContentInfoType, aContentData);
	CleanupStack::Pop(self);
	return self;					
	}

EXPORT_C CCmsContentInfo* CCmsContentInfo::NewLC(TCmsContentInfoType aContentInfoType, const TDesC8& aContentData)
	{
	CCmsContentInfo* self = new (ELeave) CCmsContentInfo(aContentInfoType, aContentData);
	CleanupStack::PushL(self);
	return self;					
	}
	
EXPORT_C CCmsContentInfo::~CCmsContentInfo()
	{
		
	}
CCmsContentInfo::CCmsContentInfo()
	{
	}

CCmsContentInfo::CCmsContentInfo(TCmsContentInfoType aContentInfoType, const TDesC8& aContentData)
:iContentType(aContentInfoType)
	{
	iContentData.Set(aContentData);
	}

void CCmsContentInfo::ConstructL(const TDesC8& aRawData)
	{
	const TInt minItems = 2;	// Must have OID
	const TInt maxItems = 2;	// Must have data
	CArrayPtr<TASN1DecGeneric>* contentInfo = PKCS7ASN1::DecodeSequenceLC(aRawData, minItems, maxItems);
	
	//Decode Content Type	
	iContentType=(TCmsContentInfoType)(CmsUtils::DecodeContentTypeL(contentInfo->At(0)));

	//Decode Content Data
	const TASN1DecGeneric* contentInfoAt1 = contentInfo->At(1);
	if ( contentInfoAt1->Tag() == 0 || contentInfoAt1->Class() == EContextSpecific )
		{
		TASN1DecGeneric decGen(contentInfoAt1->GetContentDER());
		decGen.InitL();
		if (iContentType==EContentTypeData)
			{
			if (decGen.Tag()!=EASN1OctetString || decGen.Class()!=EUniversal)
				{
				User::Leave(KErrArgument);	
				}
			else
				{
				iContentData.Set(decGen.GetContentDER());	
				}
			}
		else
			{
			iContentData.Set(decGen.Encoding());				
			}
		}
	else
		{
		User::Leave(KErrArgument);	
		}
		
	CleanupStack::PopAndDestroy(contentInfo);
	}
	
EXPORT_C CASN1EncSequence* CCmsContentInfo::EncodeASN1DERLC() const
	{
	// the root sequence contains the OID and the content data
	CASN1EncSequence* root = CASN1EncSequence::NewLC();
	
	//Encode the OID
	CASN1EncObjectIdentifier* oid = CmsUtils::EncodeContentTypeLC(iContentType);	 	
	root->AddAndPopChildL(oid);

	CASN1EncBase* enc(NULL);
	if (iContentType==EContentTypeData)
		{
		enc=CASN1EncOctetString::NewL(iContentData);			
		}
	else
		{
		//Encode the  Content
		//iContentData already encoded in sequence
		//so just rebuild the structure
		enc = CASN1EncEncoding::NewL(iContentData);
		}
	// Add [0] EXPLICT
	CASN1EncExplicitTag* Enc=CASN1EncExplicitTag::NewLC(enc, 0);
	root->AddAndPopChildL(Enc);					
		
	return root;
	}

EXPORT_C const TPtrC8 CCmsContentInfo::ContentData() const
	{
	return iContentData;	
	}

EXPORT_C TCmsContentInfoType CCmsContentInfo::ContentType() const
	{
	return iContentType;	
	}

	
//	
//Implementation of CMS EncapsulatedContentInfo Identity	
//
CEncapsulatedContentInfo* CEncapsulatedContentInfo::NewLC(const TDesC8& aRawData)
	{
	CEncapsulatedContentInfo* self = new (ELeave) CEncapsulatedContentInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aRawData);
	return self;			
	}

CEncapsulatedContentInfo* CEncapsulatedContentInfo::NewL(const TDesC8& aRawData)
	{
	CEncapsulatedContentInfo* self = NewLC(aRawData);
	CleanupStack::Pop(self);
	return self;			
	}

CEncapsulatedContentInfo* CEncapsulatedContentInfo::NewLC(TCmsContentInfoType aContentInfoType, TBool aIsEContentDataPresent, const TDesC8& aContentData)
	{
	CEncapsulatedContentInfo* self = new (ELeave) CEncapsulatedContentInfo(aContentInfoType, aIsEContentDataPresent, aContentData);
	CleanupStack::PushL(self);
	return self;					
	}

CEncapsulatedContentInfo* CEncapsulatedContentInfo::NewL(TCmsContentInfoType aContentInfoType, TBool aIsEContentDataPresent, const TDesC8& aContentData)
	{
	CEncapsulatedContentInfo* self = NewLC(aContentInfoType, aIsEContentDataPresent, aContentData);
	CleanupStack::Pop(self);
	return self;	
	}

CEncapsulatedContentInfo::CEncapsulatedContentInfo()
	{
	}

CEncapsulatedContentInfo::CEncapsulatedContentInfo(TCmsContentInfoType aContentInfoType, TBool aIsEContentDataPresent, const TDesC8& aContentData)
:iContentType(aContentInfoType),
 iIsContentDataPresent(aIsEContentDataPresent)
	{
	if (aIsEContentDataPresent)
		{
		iContentData.Set(aContentData);			
		}
	}

void CEncapsulatedContentInfo::ConstructL(const TDesC8& aRawData)
	{	
	const TInt minItems = 1;	// Must have OID
	const TInt maxItems = 2;	// Must have data
	CArrayPtr<TASN1DecGeneric>* contentInfo = PKCS7ASN1::DecodeSequenceLC(aRawData, minItems, maxItems);
	
	//Decode Content Type	
	iContentType=(TCmsContentInfoType)(CmsUtils::DecodeContentTypeL(contentInfo->At(0)));
	
	//Decode Content Data
	if(contentInfo->Count() == 2)
		{
		iIsContentDataPresent=ETrue;
		const TASN1DecGeneric* contentInfoAt1 = contentInfo->At(1);
		
		//Decode [0] Explicit
		if ( contentInfoAt1->Tag() == 0 || contentInfoAt1->Class() == EContextSpecific )
			{
			//Decode Wrapper Octet string	
			TASN1DecGeneric decGen(contentInfoAt1->GetContentDER());
			decGen.InitL();
			if(decGen.Tag() == EASN1OctetString && decGen.Class() == EUniversal)
				{
				iContentData.Set(decGen.GetContentDER());
				}
			else
				{
				//Wrapper is not an Octect String
				User::Leave(KErrArgument);
				}			
			}
		else
			{
			//Not [0] Explicit
			User::Leave(KErrArgument);
			}
		}
	else
		{
		//No optional data
		iContentData.Set(KNullDesC8());	
		}
	CleanupStack::PopAndDestroy(contentInfo);
	}	
	
	
CASN1EncSequence* CEncapsulatedContentInfo::EncodeASN1DERLC() const
	{
	// the root sequence contains the OID and the content data
	CASN1EncSequence* root = CASN1EncSequence::NewLC();
	
	//Encode the OID
	CASN1EncObjectIdentifier* oid = CmsUtils::EncodeContentTypeLC(iContentType);	 	
	root->AddAndPopChildL(oid);
	
	//Encode the Content
	if (iIsContentDataPresent)
		{
		//Wrapper Octect String
		CASN1EncOctetString* octetString=CASN1EncOctetString::NewL(iContentData);
		
		// Add [0] EXPLICT
		CASN1EncExplicitTag* Enc=CASN1EncExplicitTag::NewLC(octetString, 0);
		root->AddAndPopChildL(Enc);		
		}
			
	return root;
	}


EXPORT_C TBool CEncapsulatedContentInfo::IsContentDataPresent() const
	{
	return iIsContentDataPresent;	
	}

EXPORT_C const TPtrC8 CEncapsulatedContentInfo::ContentData() const
	{
	return iContentData;	
	}

EXPORT_C TCmsContentInfoType CEncapsulatedContentInfo::ContentType() const
	{
	return iContentType;	
	}















