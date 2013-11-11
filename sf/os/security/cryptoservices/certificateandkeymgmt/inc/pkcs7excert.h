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


/**
 @file
 @internalTechnology
 @released
*/


#ifndef __PKCS7_EXTENDED_CERTIFICATE_OR_CERTIFICATE_H__
#define __PKCS7_EXTENDED_CERTIFICATE_OR_CERTIFICATE_H__

#include <e32base.h>

class CX509Certificate;
class CASN1EncEncoding;


/**
An instance of a member of the RFC2315 ExtendedCertificatesAndCertificates.
At present, only X509 certificates are supported.
(not PKCS#6 extended certificates).
@internalTechnology
*/
class CPKCS7ExtendedCertificateOrCertificate : public CBase
	{
public:
	/**
	Creates an ExtendedCertificatesAndCertificates as defined in RFC2315.
	@param aRawData The encoded CertificateChoices object to be decoded.
	@return The fully constructed object.
	*/			
	static CPKCS7ExtendedCertificateOrCertificate* NewL(const TDesC8& aRawData);
	
	/**
	Destructor
	*/
	~CPKCS7ExtendedCertificateOrCertificate();
		
	/**
	Returns the x509 certificate reference if the certificate is a X509 certificate.
	Callers need to check whether the certificate's type is X509 beforehand.
	@return The x509 certificate reference.
	*/
	IMPORT_C const CX509Certificate& Certificate(void) const;

private:
	/**
	Default Constructor
	*/
	CPKCS7ExtendedCertificateOrCertificate();
	
	/**
	Copy Constructor
	@param the source object to be copied
	*/	
	CPKCS7ExtendedCertificateOrCertificate(const CPKCS7ExtendedCertificateOrCertificate&);
	
	/**
	Second phase constructor for decoding.
	@param aRawData the raw data to be decoded.
	*/
	void ConstructL(const TDesC8& aRawData);
	
private:	
	/**
	The X509 certificate pointer
	*/
	CX509Certificate* iCertificate;
	};


	
#endif
