/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedAll
 @released
*/

#ifndef __WTLSCERT_H__
#define __WTLSCERT_H__

#include <e32base.h>
#include <e32std.h>
#include <s32std.h>
#include <signed.h>
#include <unifiedcertstore.h>
#include <wtlsnames.h>


class CRSAPublicKey;
class CDSAPublicKey;
class CDSASignature;
class CDSAParameters;

const TInt KWTLSCertMaxDataElements = 6;

class CWTLSRSASignatureResult : public CRSASignatureResult
	{
public:
	IMPORT_C static CWTLSRSASignatureResult* NewL(const CAlgorithmIdentifier& aDigestAlgorithm, const TDesC8& aDigest);
	IMPORT_C static CWTLSRSASignatureResult* NewLC(const CAlgorithmIdentifier& aDigestAlgorithm, const TDesC8& aDigest);
	IMPORT_C virtual TBool VerifyL(const TDesC8& aResult);
private:
	void ConstructL(const CAlgorithmIdentifier& aDigestAlgorithm, const TDesC8& aDigest);
	};

class TWTLSKeyFactory : public TKeyFactory
	{
public:
	virtual CRSAPublicKey* RSAPublicKeyL(const TDesC8& aEncoding) const;
	virtual CRSASignatureResult* RSASignatureResultL(const CAlgorithmIdentifier& aDigestAlgorithm, TDesC8& aDigest) const;
	virtual CDSAPublicKey* DSAPublicKeyL(const CDSAParameters& aParams, const TDesC8& aEncoding) const;
	virtual CDSASignature* DSASignatureL(const TDesC8& aEncoding) const;
	virtual CDSAParameters* DSAParametersL(const TDesC8& aEncoding) const;
	virtual CDSAPublicKey* DSAPublicKeyL(const TDesC8& aParamsEncoding, const TDesC8& aEncoding) const;
	};

class CWTLSValidityPeriod : public CValidityPeriod
	{
public:
	IMPORT_C static CWTLSValidityPeriod* NewL(const TDesC8& aBinaryData);
	IMPORT_C static CWTLSValidityPeriod* NewLC(const TDesC8& aBinaryData);
	IMPORT_C static CWTLSValidityPeriod* NewL(const TDesC8& aBinaryData, TInt& aPos);
	IMPORT_C static CWTLSValidityPeriod* NewLC(const TDesC8& aBinaryData, TInt& aPos);
private:
	CWTLSValidityPeriod();
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	};

class CWTLSAlgorithmIdentifier : public CAlgorithmIdentifier
	{
public:
	IMPORT_C static CWTLSAlgorithmIdentifier* NewL(const TDesC8& aBinaryData);
	IMPORT_C static CWTLSAlgorithmIdentifier* NewLC(const TDesC8& aBinaryData);
	IMPORT_C static CWTLSAlgorithmIdentifier* NewL(const TDesC8& aBinaryData, TInt& aPos);
	IMPORT_C static CWTLSAlgorithmIdentifier* NewLC(const TDesC8& aBinaryData, TInt& aPos);
private:
	CWTLSAlgorithmIdentifier();
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	};

class CWTLSSigningAlgorithmIdentifier : public CSigningAlgorithmIdentifier
	{
public:
	IMPORT_C static CWTLSSigningAlgorithmIdentifier* NewL(const TDesC8& aBinaryData);
	IMPORT_C static CWTLSSigningAlgorithmIdentifier* NewLC(const TDesC8& aBinaryData);
	IMPORT_C static CWTLSSigningAlgorithmIdentifier* NewL(const TDesC8& aBinaryData, TInt& aPos);
	IMPORT_C static CWTLSSigningAlgorithmIdentifier* NewLC(const TDesC8& aBinaryData, TInt& aPos);
private:
	CWTLSSigningAlgorithmIdentifier();
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	};

class CWTLSSubjectPublicKeyInfo : public CSubjectPublicKeyInfo
	{
public:
	IMPORT_C static CWTLSSubjectPublicKeyInfo* NewL(const TDesC8& aBinaryData);
	IMPORT_C static CWTLSSubjectPublicKeyInfo* NewLC(const TDesC8& aBinaryData);
	IMPORT_C static CWTLSSubjectPublicKeyInfo* NewL(const TDesC8& aBinaryData, TInt& aPos);
	IMPORT_C static CWTLSSubjectPublicKeyInfo* NewLC(const TDesC8& aBinaryData, TInt& aPos);
private:
	CWTLSSubjectPublicKeyInfo();
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	};

_LIT(KWTLSTCAType, " T");
_LIT(KWTLSTCAValue, "ca");

class CWTLSCertificate : public CCertificate
	{
public:
	enum	//enum values for encoded data element positions in tbsCert data structure
		{	//these values are to be used as params to DataElementEncoding() function
		EVersionNumber = 0,
		EAlgorithmId = 1,
		EIssuerName = 2,
		EValidityPeriod = 3,
		ESubjectName = 4,
		ESubjectPublicKeyInfo = 5,
		};
		
	/**
	 * Creates a new CWTLSCertificate object from the specified buffer containing the binary coded representation.
	 *
	 * @param aBinaryData	The encoded binary representation.
	 * @return				The new CWTLSCertificate object.
	 */	
	IMPORT_C static CWTLSCertificate* NewL(const TDesC8& aBinaryData);

	/**
	 * Creates a new CWTLSCertificate object from the specified buffer containing the binary coded representation,
	 * and puts a pointer to it onto the cleanup stack.
	 *
	 * @param aBinaryData	The encoded binary representation.
	 * @return				The new CWTLSCertificate object.
	 */	
	IMPORT_C static CWTLSCertificate* NewLC(const TDesC8& aBinaryData);

	/**
	 * Creates a new CWTLSCertificate object from the specified buffer containing the binary coded representation.
	 *
	 * @param aBinaryData	The encoded binary representation.
	 * @param aPos			An offset into the descriptor, and is updated to the position at the end of the object.
	 * @return				The new CWTLSCertificate object.
	 */	
	IMPORT_C static CWTLSCertificate* NewL(const TDesC8& aBinaryData, TInt& aPos);

	/**
	 * Creates a new CWTLSCertificate object from the specified buffer containing the binary coded representation,
	 * and puts a pointer to it onto the cleanup stack.
	 *
	 * @param aBinaryData	The encoded binary representation.
	 * @param aPos			An offset into the descriptor, and is updated to the position at the end of the object.
	 * @return				The new CWTLSCertificate object.
	 */	
	IMPORT_C static CWTLSCertificate* NewLC(const TDesC8& aBinaryData, TInt& aPos);

	/**
	 * Creates a new CWTLSCertificate object from a stream.
	 *
	 * The stream must have been written using the corresponding ExternalizeL() function.
	 *
	 * @param aStream	The stream to be used as input.
	 * @return			The new CWTLSCertificate object.
	 */	
	IMPORT_C static CWTLSCertificate* NewL(RReadStream& aStream);

	/**
	 * Creates a new CWTLSCertificate object from a stream,
	 * and puts a pointer to it onto the cleanup stack.
	 *
	 * The stream must have been written using the corresponding ExternalizeL() function.
	 *
	 * @param aStream	The stream to be used as input.
	 * @return			The new CWTLSCertificate object.
	 */	
	IMPORT_C static CWTLSCertificate* NewLC(RReadStream& aStream);

	/**
	 * Creates a new CWTLSCertificate object from an existing one in the certificate store.
	 *
	 * @param aCert	The certificate to be copied.
	 * @return		The new CWTLSCertificate object.
	 */	
	IMPORT_C static CWTLSCertificate* NewL(const CWTLSCertificate& aCert);

	/**
	 * Creates a new CWTLSCertificate object from an existing one in the certificate store,
	 * and puts a pointer to it onto the cleanup stack.
	 *
	 * @param aCert	The certificate to be copied.
	 * @return		The new CWTLSCertificate object.
	 */	
	IMPORT_C static CWTLSCertificate* NewLC(const CWTLSCertificate& aCert);

	/**
	 * Destructor.
	 *
	 * Frees all resources owned by the object, prior to its destruction.
	 */
	IMPORT_C ~CWTLSCertificate();
	
	/**
	* Tests whether this WTLS certificate is the same as the specified WTLS certificate.
	*
	* This is not a simple as it sounds. For X.509 certificates equality means that the issuer name 
	* and serial number fields are the same. This guarantees equality since a CA must ensure that every 
	* certificate it issues has a unique serial number. But WTLS certificates do not include serial numbers, 
	* so there seems to be no definition of equality. This function uses a byte-for-byte comparison of the 
	* signatures on the certificates: this should not result in any false positives, but may give false 
	* negatives if CAs do naughty things like recertifying the same key (which they have been known to do).
	*
	* @param aCert	A WTLS certificate
	* @return		ETrue, if the parameter is the same certificate; EFalse, otherwise.
	*/
	IMPORT_C TBool IsEqualL(const CWTLSCertificate& aCert) const;
	
	//extra accessors
	
	/**
	 * Gets the version number of the WTLS certificate.
	 *
	 * @return	The version number of the certificate. Always returns 1.
	 */
	IMPORT_C TInt Version() const;
	
	/**
	 * Gets the name of the WTLS certificate's issuing authority.
	 *
	 * @return	A WTLS name:
	 *			@li	If the name is an X.500 DN, then if the name contains a Common name, that will be returned. 
	 * 				Otherwise, if the name contains an Organization name, that will be returned. Otherwise an empty 
	 * 				string will be returned. 
	 *			@li	If the name is of type text, then if the name is not a 'structured' name the entire string will 
	 *				be returned, otherwise the same procedure will be followed as for X.509 certificates.
	 * 			@li	If the name is null an empty string will be returned. 
	 */
	 // Unsupported -- If the name is a key hash or binary value the entire contents will be returned.
	IMPORT_C const CWTLSName& IssuerName() const;
	
	/**
	* Gets the name of the owner of the public key the WTLS certificate contains.
	*
	* @return A WTLS name.
	*/
	IMPORT_C const CWTLSName& SubjectName() const;
	IMPORT_C virtual TBool IsSelfSignedL() const;
	IMPORT_C virtual HBufC* SubjectL() const;
	IMPORT_C virtual HBufC* IssuerL() const;
	
	/**
	* Initialises the certificate from a stream. 
	*
	* This should not be called by client code; instead the static factory function above should be used. 
	* If a client is using the certstore component for storage then it should use CCertStore::AddL() for 
	* externalizing and CWTLSCert::NewL(CCertStore& aStore, const CCertStoreEntry& aEntry); for restoring.
	*
	* @param aStream 
	*/
	IMPORT_C void InternalizeL(RReadStream& aStream);
	
	/**
	* Gets the certificate's signed data.
	*
	* @return	A non-modifiable pointer descriptor representing the certificate's signed data.
	*/
	IMPORT_C const TPtrC8 SignedDataL() const;
	
	/**
	* Gets the encoding for a data element at the specified index.
	*
	* @param aIndex	The position of the encoded data element.
	*/
	IMPORT_C virtual const TPtrC8* DataElementEncoding(const TUint aIndex) const;
	
	/**
	* Tests whether a non-selfsigned certificate can be used to sign others.
	* 
	* Currently this only supports structured text variety of WTLS certificates.  
	* Other certificate types will return EFalse.
	*
	* @return	ETrue, if the certificate is capable of signing other certificates; otherwise, EFalse. 
	*/
	IMPORT_C TBool IsTCAL() const;
private:
	CWTLSCertificate();
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	void ConstructL(const CWTLSCertificate& aCertificate);
	void ConstructCertL(const TDesC8& aBinaryData, TInt& aPos);
	void InitEncodedDataElementsL();
	//private data
	TInt iVersion;
	CWTLSName* iIssuerName;
	CWTLSName* iSubjectName;
	TFixedArray<TPtrC8*, KWTLSCertMaxDataElements>* iDataElements;
	};

#endif
