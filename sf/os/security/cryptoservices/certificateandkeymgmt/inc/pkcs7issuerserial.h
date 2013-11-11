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
 @publishedPartner
 @released
*/

#ifndef __PKCS7_ISSUER_AND_SERIAL_NUMBER_H__
#define __PKCS7_ISSUER_AND_SERIAL_NUMBER_H__

#include <e32base.h>

class CX500DistinguishedName;
class CX509SubjectKeyIdExt;
class CASN1EncSequence;
class CASN1EncBase;


/**
The issuer and serial number of a PKCS#7 signature.
*/
class CPKCS7IssuerAndSerialNumber : public CBase
	{
public:
	/**
	 * @internalComponent
	 *
	 * Creates a CMS issuer and serial number object as defined in RFC2630
	 * @param aIssuerName The distinguished name of the issuer. Ownership of
	 *	aIssuerName is transferred to the newly created object.
	 *	If construction fails, ownership is not transferred. The caller needs to push
	 *	aIssuerName onto the cleanup stack before calling this method and pop it off the
	 *	cleanup stack after successful construction.
	 * @param aSerialNumber The serial number of the issuer.
	 * @return The fully constructed object.
	 **/
	static CPKCS7IssuerAndSerialNumber* NewL(CX500DistinguishedName* aIssuerName, const TDesC8& aSerialNumber);

	/**
	 * @internalComponent
	 *
	 * Creates a CMS issuer and serial number object as defined in RFC2630
	 *	and leaves it on the cleanup stack.
	 * @param aIssuerName The distinguished name of the issuer. Ownership of
	 *	aIssuerName is transferred to the newly created object.
	 *	If construction fails, ownership is not transferred. The caller needs to push
	 *	aIssuerName onto the cleanup stack before calling this method and pop it off the
	 *	cleanup stack after successful construction.
	 * @param aSerialNumber The serial number of the issuer.
	 * @return The fully constructed object.
	 **/	
	static CPKCS7IssuerAndSerialNumber* NewLC(CX500DistinguishedName* aIssuerName, const TDesC8& aSerialNumber);
	
	/**
	 * @internalComponent
	 *
	 * Creates a CMS issuer and serial number object as defined in RFC2630.
	 * @param aRawData A buffer which holds the encoded issuer and serial number.
	 * @return The fully constructed object.
	 **/	
	static CPKCS7IssuerAndSerialNumber* NewL(const TDesC8& aRawData);
	
	/**
	Destructor.
	*/
	IMPORT_C ~CPKCS7IssuerAndSerialNumber();
	
	/**
	Returns the issuer's distinguished name.
	@return The issuer's distinguished name reference.
	*/
	IMPORT_C const CX500DistinguishedName& IssuerName() const;
	
	/**
	Returns the issuer's serial number.
	@return The issuer's serial number reference.
	*/
	IMPORT_C const TDesC8& SerialNumber() const;

	/**
	 * @internalComponent
	 * 
	 * Creates the ASN1 DER sequence of the signer identifier object
	 * and leaves it on the cleanup stack.
	 * @return  ASN1 DER sequence of this object.
	 */
	CASN1EncSequence* EncodeASN1DERLC() const;
		
private:
	/**
	Constructor
	*/
	CPKCS7IssuerAndSerialNumber(void);
	
	/**
	Copy constructor
	*/
	CPKCS7IssuerAndSerialNumber(const CPKCS7IssuerAndSerialNumber&);
	
	/**
	Second phase constructor
	@param aIssuerName the issuer name
	@param aSerialNumber the serial number
	*/
	void ConstructL(CX500DistinguishedName* aIssuerName, const TDesC8& aSerialNumber);

	/**
	Second phase constructor
	@param aRawData the raw data to be decoded.
	*/	
	void ConstructL(const TDesC8& aRawData);
	
private:
	/**
	Signer's distinguished name
	*/
	CX500DistinguishedName* iIssuerName;
	
	/**
	Signer's serial number
	*/
	HBufC8* iSerialNumber;
	};
	
#endif
