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


#include <cmscertchoice.h>
#include "pkcs7asn1.h"
#include <asn1dec.h>
#include <x509cert.h>
#include <asn1enc.h>
#include "cmsutils.h"

//Implementation of CMS Certificate Choice

CCmsCertificateChoice* CCmsCertificateChoice::NewL(const TDesC8& aRawData)
	{
	CCmsCertificateChoice* self = new (ELeave) CCmsCertificateChoice();
	CleanupStack::PushL(self);
	self->ConstructL(aRawData);
	CleanupStack::Pop(self);
	return self;
	}

CCmsCertificateChoice::~CCmsCertificateChoice()
	{
	delete iCertificate;
	delete iEncodedAttributeCertificate;
	}

CCmsCertificateChoice::CCmsCertificateChoice()
	{
	}

void CCmsCertificateChoice::ConstructL(const TDesC8& aRawData)
	{
	TASN1DecGeneric decGen(aRawData);
	decGen.InitL();
		
	if (decGen.Tag()==EASN1Sequence && decGen.Class()==EUniversal)
		{
		// x509 certificates
		iCertificateType=ECertificateX509;
		iCertificate = CX509Certificate::NewL(aRawData);
		}
	else if (decGen.Tag()==1 && decGen.Class()==EContextSpecific)
			{
			// Attribute certificate
			iCertificateType=ECertificateAttribute;
			CASN1EncEncoding* enc=CASN1EncEncoding::NewLC(aRawData, EASN1Sequence, EUniversal);
			iEncodedAttributeCertificate=CmsUtils::CreateDEREncodingLC(*enc);
			CleanupStack::Pop(iEncodedAttributeCertificate);
			CleanupStack::PopAndDestroy(enc);
			}
		else if (decGen.Tag()==0 && decGen.Class()==EContextSpecific)
				{
				// extended certificates not supported
				iCertificateType=ECertificateExtendedCerificate;
				User::Leave(KErrNotSupported);
				}
			else
				{
				User::Leave(KErrArgument);	
				}		
	}

EXPORT_C const CX509Certificate& CCmsCertificateChoice::Certificate(void) const
	{
	return *iCertificate;
	}

CCmsCertificateChoice* CCmsCertificateChoice::NewLC(const CX509Certificate& aCertificate)
	{	
	CCmsCertificateChoice* self = new (ELeave) CCmsCertificateChoice();
	CleanupStack::PushL(self);
	self->ConstructL(aCertificate);
	return self;
	}

CCmsCertificateChoice* CCmsCertificateChoice::NewL(const CX509Certificate& aCertificate)
	{
	CCmsCertificateChoice* self = CCmsCertificateChoice::NewLC(aCertificate);
	CleanupStack::Pop();
	return self;
	}

	
CCmsCertificateChoice* CCmsCertificateChoice::NewLC(TCertificateType aCertType, const TDesC8& aEncodedCertificate)
	{		
	CCmsCertificateChoice* self = new (ELeave) CCmsCertificateChoice();
	CleanupStack::PushL(self);
	self->ConstructL(aCertType, aEncodedCertificate);
	return self;
	}

CCmsCertificateChoice* CCmsCertificateChoice::NewL(TCertificateType aCertType, const TDesC8& aEncodedCertificate)
	{
	CCmsCertificateChoice* self = CCmsCertificateChoice::NewLC(aCertType, aEncodedCertificate);
	CleanupStack::Pop();
	return self;
	}

void CCmsCertificateChoice::ConstructL(const CX509Certificate& aCertificate)
	{
	iCertificateType=ECertificateX509;
	iCertificate=CX509Certificate::NewL(aCertificate);
	}

void CCmsCertificateChoice::ConstructL(TCertificateType aCertType, const TDesC8& aEncodedCertificate)
	{
	iCertificateType=aCertType;
	if (aCertType==ECertificateAttribute)
		{
		iEncodedAttributeCertificate=aEncodedCertificate.AllocL();	
		}
	else if (aCertType==ECertificateX509)
			{
			iCertificate=CX509Certificate::NewL(aEncodedCertificate);	
			}
		 else
			 {
			 User::Leave(KErrNotSupported);	
			 }
	}
	

EXPORT_C const HBufC8* CCmsCertificateChoice::AttributeCertificate() const
	{
	return iEncodedAttributeCertificate;
	}

CASN1EncEncoding* CCmsCertificateChoice::EncodeASN1DERLC() const
	{
	CASN1EncEncoding* enc(NULL);
	if (iCertificateType==ECertificateX509)
		{
		enc=CASN1EncEncoding::NewLC(iCertificate->Encoding());
		}
	else if (iCertificateType==ECertificateAttribute)
			{
			TASN1DecGeneric decGen(*iEncodedAttributeCertificate);
			decGen.InitL();
			if (decGen.Tag()!=EASN1Sequence)
				{
				User::Leave(KErrArgument);	
				}
			enc=CASN1EncEncoding::NewLC(*iEncodedAttributeCertificate, 1, EContextSpecific);
			}
		else
			{
			//Extended Certificate
			User::Leave(KErrNotSupported);
			}
	return enc;
	}

EXPORT_C CCmsCertificateChoice::TCertificateType CCmsCertificateChoice::CertificateType()
	{
	return iCertificateType;
	}














