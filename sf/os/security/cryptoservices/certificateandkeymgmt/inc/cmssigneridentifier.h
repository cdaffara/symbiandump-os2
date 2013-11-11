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

#ifndef CMSSIGNERIDENTIFIER_H
#define CMSSIGNERIDENTIFIER_H

#include <e32base.h>
#include <pkcs7issuerserial.h>

class CX500DistinguishedName;
class CX509SubjectKeyIdExt;
class CASN1EncSequence;
class CASN1EncBase;


/**
 A representation of a RFC2630 (SignerIdentifier) entity.
 */
class CCmsSignerIdentifier : public CBase
	{
public:

	/**
	The SignerIdentifier type.
	*/
	enum TSignerIdentifierType
		{
		/** The SignerIdentifier type is issuerAndSerialNumber. */
		EIssuerAndSerialNumber,
		/** The SignerIdentifier type is subjectKeyIdentifier. */
		ESubjectKeyIdentifier
		};

	/**
	 * @internalComponent
	 * 
	 * Creates a CMS SignerIdentifier as defined in RFC2630.
	 * @param aIssuerAndSerialNumber An IssuerAndSerialNumber instance used to construct
	 *	this object. Ownership of aIssuerAndSerialNumber is transferred to the newly
	 *	created CCmsSignerIdentifier object. If construction fails, ownership is 
	 *	not transferred. The caller needs to push aIssuerAndSerialNumber onto the cleanup stack before calling 
	 *	this method, and pop it off the cleanup stack after successful construction.
	 *  @return The fully constructed object.
	 **/
	static CCmsSignerIdentifier* NewL(CPKCS7IssuerAndSerialNumber* aIssuerAndSerialNumber);
	
	/**
	 * @internalComponent
	 *
	 * Creates a CMS SignerIdentifier as defined in RFC2630, and leaves it on the cleanup stack.
	 * @param aIssuerAndSerialNumber An IssuerAndSerialNumber instance used to construct
	 *	this object. Ownership of aIssuerAndSerialNumber is transferred to the newly
	 *	created CCmsSignerIdentifier object. If construction fails, ownership is 
	 *	not transferred. The caller needs to push aIssuerAndSerialNumber onto the cleanup stack before calling 
	 *	this method, and pop it off the cleanup stack after successful construction.
	 * @return The fully constructed object.
	 **/	
	static CCmsSignerIdentifier* NewLC(CPKCS7IssuerAndSerialNumber* aIssuerAndSerialNumber);
	
	/**
	 * @internalComponent
	 * 
	 * Creates a CMS SignerIdentifier as defined in RFC2630.
	 * @param aSubjectKeyIdExt A buffer that contains the subject key identifier.
	 *	Ownership of aSubjectKeyIdExt is transferred to the newly
	 *	created CCmsSignerIdentifier object. If construction fails, ownership is 
	 *	not transferred. The caller needs to push aSubjectKeyIdExt onto the cleanup stack before calling 
	 *	this method, and pop it off the cleanup stack after successful construction.
	 * @return The fully constructed object.
	 **/
	static CCmsSignerIdentifier* NewL(HBufC8* aSubjectKeyIdExt);
	
	/**
	 * @internalComponent
	 * 
	 * Creates a CMS SignerIdentifier as defined in RFC2630, and leaves it on the cleanup stack.
	 * @param aSubjectKeyIdExt A buffer that contains the subject key identifier.
	 *	Ownership of aSubjectKeyIdExt is transferred to the newly
	 *	created CCmsSignerIdentifier object. If construction fails, ownership is 
	 *	not transferred. The caller needs to push aSubjectKeyIdExt onto the cleanup stack before calling 
	 *	this method, and pop it off the cleanup stack after successful construction.
	 * @return The fully constructed object
	 **/		
	static CCmsSignerIdentifier* NewLC(HBufC8* aSubjectKeyIdExt);
	
	/**
	 * @internalComponent
	 *
	 * Creates a CMS SignerIdentifier as defined in RFC2630.
	 * @param aRawData A buffer that contains the encoded CMS signer identifier.
	 * @return The fully constructed object.
	 **/			
	static CCmsSignerIdentifier* NewL(const TDesC8& aRawData);
	
	/**
	 * @internalComponent
	 *
	 * Creates a CMS SignerIdentifier as defined in RFC2630, and leaves it on the cleanup stack.
	 * @param aRawData A buffer that contains the encoded CMS signer identifier.
	 * @return The fully constructed object.
	 **/				
	static CCmsSignerIdentifier* NewLC(const TDesC8& aRawData);
	
	/**
	Destructor
	*/
	IMPORT_C ~CCmsSignerIdentifier();
	
	/**
	 * @internalComponent
	 *
	 * Creates the ASN1 sequence of the signer identifier object and leaves it on the cleanup stack.
	 * @return ASN1 sequence of this object.
	 */	
	CASN1EncBase* EncodeASN1DERLC() const;

	/**
	Returns the issuer and serial name.
	@return The CPKCS7IssuerAndSerialNumber reference. NULL if the signer 
	identifier type is subjectKeyIdentifier. The signer identifier type can be retrieved
	using SignerIdentifierType().
	*/		
	IMPORT_C const CPKCS7IssuerAndSerialNumber* IssuerAndSerialNumber() const;
	
	/**
	Returns the subject key identifier.
	@return The subject key identifier reference. KNullDesC8 if the signer 
	identifier type is issuerAndSerialNumber. The signer identifier type can 
	be retrieved using SignerIdentifierType().
	*/	
	IMPORT_C const TDesC8& SubjectKeyIdentifier() const;
	
	/**
	Returns the type of the signer identifier.
	@return The type of the signer identifier. See the TSignerIdentifierType enum.
	*/		
	IMPORT_C TInt SignerIdentifierType() const;
	
protected:
	/**
	 * @internalComponent
	 * 
	 * Default constructor
	 **/
	CCmsSignerIdentifier();
	
	/**
	 * @internalComponent
	 *
	 * Constructor for encoding.
	 * @param aIssuerAndSerialNumber The issuer and serial number.
	 **/
	CCmsSignerIdentifier(CPKCS7IssuerAndSerialNumber* aIssuerAndSerialNumber);
	
	/**
	 * @internalComponent
	 *
	 * Constructor for encoding
	 * @param aSubjectKeyIdExt the subject key identifier extension
	 **/	
	CCmsSignerIdentifier(HBufC8* aSubjectKeyIdExt);
	
	/**
	 * @internalComponent
	 *
	 * Second phase constructor for encoding
	 * @param aRawData the raw data
	 **/		
	void ConstructL(const TDesC8& aRawData);

private:
	/**
	The type of the signer identifier.
	*/
	TSignerIdentifierType iSignerIdentifierType;
	
	/**
	The issuer and serial number
	*/	
	CPKCS7IssuerAndSerialNumber* iIssuerAndSerialNumber;
	
	/**
	The subject key identifier.
	*/
	HBufC8* iSubjectKeyIdExt;
	};	
	
#endif //CMSSIGNERIDENTIFIER_H
