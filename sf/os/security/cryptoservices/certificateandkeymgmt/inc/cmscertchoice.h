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
 @publishedPartner
 @released
*/


#ifndef CMSCERTCHOICE_H
#define CMSCERTCHOICE_H

#include <e32base.h>

class CX509Certificate;
class CASN1EncEncoding;

/**
An instance of a member of the RFC2630 CertificateChoices.
At present, only X509 certificates and attribute certificate are supported.
(not PKCS#6 extended certificates).
*/
class CCmsCertificateChoice : public CBase
	{
public:
	/**
	Certificate Types
	*/
	enum TCertificateType
		{
		/**
		X.509 Certificate.
		*/
		ECertificateX509,
		/**
		Extended Certificate.
		*/
		ECertificateExtendedCerificate,
		/**
		Attribute Certificate.
		*/
		ECertificateAttribute
		};
		
	/**
	 * @internalComponent
	 * 
	 * Creates a CertificateChoices as defined in RFC2630.
	 * @param aCertificate The X509 certificate used to build the CertificateChoices object.
	 * The newly created object will create a copy of aCertificate.	
	 * @return The fully constructed object.
	 **/
	static CCmsCertificateChoice* NewL(const CX509Certificate& aCertificate);

	/**
	 * @internalComponent
	 *
	 * Creates a CertificateChoices object as defined in RFC2630
	 * and leaves the object on the cleanup stack.
	 * @param aCertificate The X509 certificate used to build the CertificateChoices object.
	 * The newly created object will create a copy of aCertificate.	
	 * @return The fully constructed object.
	 **/
	static CCmsCertificateChoice* NewLC(const CX509Certificate& aCertificate);

	/**
	 * @internalComponent
	 *
	 * Creates a CertificateChoices object as defined in RFC2630.
	 * @param aCertType The encoded certificate type. ECertificateExtendedCerificate is not supported.
	 * @param aEncodedCertificate The encoded certificate used to build the CertificateChoices object.
	 * The newly created object will create a copy of aEncodedCertificate.
	 * @return The fully constructed object.
	 **/	
	static CCmsCertificateChoice* NewL(TCertificateType aCertType, const TDesC8& aEncodedCertificate);
	
	/**
	 * @internalComponent
	 * 
	 * Creates a CertificateChoices object as defined in RFC2630 and leaves the object on the cleanup stack.
	 * @param aCertType The encoded certificate type. ECertificateExtendedCerificate is not supported.
	 * @param aEncodedCertificate The encoded certificate used to build the CertificateChoices object.
	 * The newly created object will create a copy of aEncodedCertificate.
	 * @return The fully constructed object.
	 **/		
	static CCmsCertificateChoice* NewLC(TCertificateType aCertType, const TDesC8& aEncodedCertificate);
	
	/**
	 * @internalComponent
	 * 
	 * Creates a CertificateChoices object as defined in RFC2630.
	 * @param aRawData The encoded CertificateChoices object to be decoded.
	 * @return The fully constructed object.
	 **/			
	static CCmsCertificateChoice* NewL(const TDesC8& aRawData);
	
	/**
	Destructor
	*/
	virtual ~CCmsCertificateChoice();
	
	/**
	Returns the type of the CertificateChoices object
	@return The type of the certificate.
	*/
	IMPORT_C TCertificateType CertificateType();
	
	/**
	Returns the x509 certificate reference if the certificate is a X509 certificate.
	Callers need to check whether the certificate's type is X509 beforehand.
	@return The x509 certificate reference.
	*/
	IMPORT_C const CX509Certificate& Certificate(void) const;

	/**
	Returns the encoded certificate's buffer. If the certificate is not an
	attribute certificate, NULL is returned. Callers can also check whether 
	the certificate's type is X509 beforehand.
	@return A pointer to the encoded certificate buffer.
	*/
	IMPORT_C const HBufC8* AttributeCertificate() const;

	/**
	 * @internalComponent
	 *
	 * Creates the ASN1 DER sequence of the CertificateChoices object
	 * and leaves it on the cleanup stack.
	 * @return  ASN1 DER sequence of this object.
	 **/		
	CASN1EncEncoding* EncodeASN1DERLC() const;

private:
	/**
	Default Constructor
	*/
	CCmsCertificateChoice();
	
	
	/**
	Second phase constructor for decoding.
	@param aRawData the raw data to be decoded.
	*/
	void ConstructL(const TDesC8& aRawData);
	
	/**
	Second phase constructor for encoding.
	@param aCertificate the X509 certificate used to create the object
	*/
	void ConstructL(const CX509Certificate& aCertificate);

	/**
	Second phase constructor for encoding.
	@param aCertType The encoded certificate type. ECertificateExtendedCerificate is not supported
	@param aEncodedCertificate the encoded certificate used to create the object
	*/	
	void ConstructL(TCertificateType aCertType, const TDesC8& aEncodedCertificate);

	
private:
	/**
	The type the embedded certificate type
	*/
	TCertificateType iCertificateType;
	
	/**
	The X509 certificate pointer
	*/
	CX509Certificate* iCertificate;
	
	/**
	The attribute certificate buffer
	*/	
	HBufC8* iEncodedAttributeCertificate;
	};
	
#endif
