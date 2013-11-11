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


#include <pkcs7excert.h>
#include "pkcs7asn1.h"
#include <asn1dec.h>
#include <x509cert.h>
#include <asn1enc.h>

//Implementation of PKCS7 Extended Certificate Set

CPKCS7ExtendedCertificateOrCertificate* CPKCS7ExtendedCertificateOrCertificate::NewL(const TDesC8& aRawData)
	{
	CPKCS7ExtendedCertificateOrCertificate* self = new (ELeave) CPKCS7ExtendedCertificateOrCertificate();
	CleanupStack::PushL(self);
	self->ConstructL(aRawData);
	CleanupStack::Pop(self);
	return self;
	}

CPKCS7ExtendedCertificateOrCertificate::~CPKCS7ExtendedCertificateOrCertificate()
	{
	delete iCertificate;
	}

CPKCS7ExtendedCertificateOrCertificate::CPKCS7ExtendedCertificateOrCertificate()
	{
	}

void CPKCS7ExtendedCertificateOrCertificate::ConstructL(const TDesC8& aRawData)
	{
	TASN1DecGeneric decGen(aRawData);
	decGen.InitL();
		
	if (decGen.Tag()==EASN1Sequence && decGen.Class()==EUniversal)
		{
		// x509 certificates
		iCertificate = CX509Certificate::NewL(aRawData);
		}
	else if (decGen.Tag()==0 && decGen.Class()==EContextSpecific)
			{
			// extended certificates not supported
			User::Leave(KErrNotSupported);
			}
		else
			{
			User::Leave(KErrArgument);	
			}		
	}

EXPORT_C const CX509Certificate& CPKCS7ExtendedCertificateOrCertificate::Certificate(void) const
	{
	return *iCertificate;
	}





