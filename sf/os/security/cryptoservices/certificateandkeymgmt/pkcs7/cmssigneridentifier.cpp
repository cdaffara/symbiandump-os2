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


#include <cmssigneridentifier.h> 
#include <asn1dec.h>
#include <asn1enc.h>

//
// Implementation of CCmsSignerIdentifier
//

CCmsSignerIdentifier* CCmsSignerIdentifier::NewL(CPKCS7IssuerAndSerialNumber* aIssuerAndSerialNumber)
	{
	CCmsSignerIdentifier* self = NewLC(aIssuerAndSerialNumber);
	CleanupStack::Pop(self);
	return self;		
	}

CCmsSignerIdentifier* CCmsSignerIdentifier::NewLC(CPKCS7IssuerAndSerialNumber* aIssuerAndSerialNumber)
	{
	if (!aIssuerAndSerialNumber)
		{
		User::Leave(KErrArgument);	
		}
	CCmsSignerIdentifier* self = new (ELeave) CCmsSignerIdentifier(aIssuerAndSerialNumber);
	CleanupStack::PushL(self);
	return self;		
	}

CCmsSignerIdentifier* CCmsSignerIdentifier::NewL(HBufC8* aSubjectKeyIdExt)
	{
	CCmsSignerIdentifier* self = NewLC(aSubjectKeyIdExt);
	CleanupStack::Pop(self);
	return self;
	}
	
CCmsSignerIdentifier* CCmsSignerIdentifier::NewLC(HBufC8* aSubjectKeyIdExt)
	{
	if (!aSubjectKeyIdExt)
		{
		User::Leave(KErrArgument);	
		}
	CCmsSignerIdentifier* self = new (ELeave) CCmsSignerIdentifier(aSubjectKeyIdExt);
	CleanupStack::PushL(self);
	return self;				
	}
	
CCmsSignerIdentifier* CCmsSignerIdentifier::NewL(const TDesC8& aRawData)
	{
	CCmsSignerIdentifier* self = NewLC(aRawData);
	CleanupStack::Pop(self);
	return self;					
	}

CCmsSignerIdentifier* CCmsSignerIdentifier::NewLC(const TDesC8& aRawData)
	{
	CCmsSignerIdentifier* self = new (ELeave) CCmsSignerIdentifier();
	CleanupStack::PushL(self);
	self->ConstructL(aRawData);
	return self;					
	}

CCmsSignerIdentifier::CCmsSignerIdentifier(CPKCS7IssuerAndSerialNumber* aIssuerAndSerialNumber) : iSignerIdentifierType(EIssuerAndSerialNumber)
	{
	iIssuerAndSerialNumber=aIssuerAndSerialNumber;
	}

CCmsSignerIdentifier::CCmsSignerIdentifier(HBufC8* aSubjectKeyIdExt) : iSignerIdentifierType(ESubjectKeyIdentifier)
	{
	iSubjectKeyIdExt=aSubjectKeyIdExt;
	}

EXPORT_C CCmsSignerIdentifier::~CCmsSignerIdentifier()
	{
	delete iIssuerAndSerialNumber;
	delete iSubjectKeyIdExt;
	}

CASN1EncBase* CCmsSignerIdentifier::EncodeASN1DERLC() const
	{
	switch (iSignerIdentifierType)
		{
	case EIssuerAndSerialNumber:
		{
		return iIssuerAndSerialNumber->EncodeASN1DERLC();
		}
		
	case ESubjectKeyIdentifier:
		{
		CASN1EncBase* subKey = CASN1EncOctetString::NewLC(*iSubjectKeyIdExt);
		subKey->SetTag(0);
		return subKey;
		}
		
	default:
		User::Leave(KErrNotSupported);
		}
	
	//Should not arrive here
	return NULL;	
	}
	
EXPORT_C const CPKCS7IssuerAndSerialNumber* CCmsSignerIdentifier::IssuerAndSerialNumber() const
	{
	return iIssuerAndSerialNumber;
	}

EXPORT_C TInt CCmsSignerIdentifier::SignerIdentifierType() const
	{
	return iSignerIdentifierType;
	}

CCmsSignerIdentifier::CCmsSignerIdentifier()
	{
	}

void CCmsSignerIdentifier::ConstructL(const TDesC8& aRawData)
	{
	TASN1DecGeneric decGen(aRawData);
	decGen.InitL();
	
	if(decGen.Tag() == EASN1Sequence && decGen.Class()==EUniversal)
		{
		iSignerIdentifierType=EIssuerAndSerialNumber;
		iIssuerAndSerialNumber=CPKCS7IssuerAndSerialNumber::NewL(aRawData);			
		}
	else if(decGen.Tag() == 0 && decGen.Class()==EContextSpecific)
			{
			iSignerIdentifierType=ESubjectKeyIdentifier;
			iSubjectKeyIdExt=decGen.GetContentDER().AllocL();				
			}
		 else
			 {
			 User::Leave(KErrArgument);	
			 }	
	}

EXPORT_C const TDesC8& CCmsSignerIdentifier::SubjectKeyIdentifier() const
	{
	return *iSubjectKeyIdExt;
	}




