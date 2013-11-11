/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Declares classes for producing PKCS#10 certificate requests.
*
*/


/**
 @file
 @publishedPartner
 @released 
*/

#ifndef __PKCS10_H__
#define __PKCS10_H__

#include <e32base.h>
#include <asn1enc.h>
#include <mctkeystore.h>
#include <signed.h>

class CX500DistinguishedName;
class CASN1EncBase;
class CPKCS10Attributes;
class CPKCS10KeyHelper;
class CMessageDigest;
class TX509KeyEncoder;

/** 
 * Class for making PKCS#10 Certificate Request objects.
 *
 * Keys are specified by a cryptotokens key info object - this means that this
 * API can only be used to generate cert requests for keys that are held in a
 * keystore on the device.
 * 
 */
class CPKCS10Request : public CActive
	{
public:
	/**
	 * Creates a new PKCS#10 request object.
	 * 
	 * @param aDN X500 distinguished name of the entity provided by caller.
	 *     Stored in iDN member variable. Ownership is not transferred.
	 * @param aKeyInfo The key info object of the key to sign the request with.
	 * 	   Does not take ownership.
	 * @param aAttr (Optional) The PKCS10 attributes to include in the request.
	 * 	   Takes ownership.
	 * @return A pointer to the newly allocated object.
	 */
	IMPORT_C static CPKCS10Request* NewL(const CX500DistinguishedName& aDN,
										 const CCTKeyInfo& aKeyInfo,
										 CPKCS10Attributes* aAttr = NULL);
	
	/**
	 * Creates a new PKCS#10 request object.
	 * 
	 * @param aDN X500 distinguished name of the entity provided by caller.
	 *     Stored in iDN member variable. Ownership is not transferred.
	 * @param aKeyInfo The key info object of the key to sign the request with.
	 * 	   Does not take ownership.
	 * @param aAttr (Optional) The PKCS10 attributes to include in the request.
	 * 	   Takes ownership.
	 * @return A pointer to the newly allocated object that is left on the
	 * 	   cleanup stack.
	 */
	IMPORT_C static CPKCS10Request* NewLC(const CX500DistinguishedName& aDN,
										 const CCTKeyInfo& aKeyInfo,
										 CPKCS10Attributes* aAttr = NULL);

	/**
	 * Destructs PKCS#10 object, deletes encoding buffer and attributes.
	 */
	IMPORT_C virtual ~CPKCS10Request();

public:

	/**
	 * Set the attributes to be encoded in the request. It replaces existing
	 * attributes, if any.
	 * @param aAttr The attributes - this object takes ownership.
	 */
	IMPORT_C void SetAttributes(CPKCS10Attributes* aAttr);

	/**
	 * Set the digest algorithm to use when signing the request.  If this method
	 * is not called, the default SHA-1 is used. 
	 *
	 * @param aDigest	For RSA keys, one of EMD2, EMD5 or ESHA1.  
	 * 					For DSA keys, ESHA1 is the only permitted value.
	 * @leave KErrArgument if the specified algorithm is not supported.
	 */
	IMPORT_C void SetDigestAlgL(TAlgorithmId aDigest);

	/**
	 * Set the distinguished name of the entity. It replaces existing
	 * name, if any.
	 * @param aDN X500 distinguished name of the entity provided by caller.
	 *     Stored in iDN member variable. Ownership is not transferred.
	 */
	IMPORT_C void SetDistinguishedNameL(const CX500DistinguishedName& aDN);

	/**
	 * Set the information of the key to sign with. It replaces existing
	 * key info, if any.
	 * @param aKeyInfo The key info object of the key to sign the request with.
	 * 	   Does not take ownership.
	 */
	IMPORT_C void SetKeyInfoL(const CCTKeyInfo& aKeyInfo);

	/**
	 * Create the ASN.1 DER encoding of the certificate request.  This is an
	 * asynchronous method. The Cancel() method can be called to cancel an
	 * outstanding request. This method can be called repeatedly to create 
	 * certificate requests after setting the various parameters. However an
	 * outstanding request must complete or be cancelled before calling this 
	 * method again.
	 * 
	 * 
	 * @param aResult	On successful completion, this points to a newly
	 * 					allocated buffer containing the encoded certificate request.
	 * @param aStatus	Asynchronous status notification 
	 */	
	IMPORT_C void CreateEncoding(HBufC8*& aResult, TRequestStatus& aStatus);
	
private:

	virtual void RunL();
	virtual TInt RunError(TInt aErr);
	virtual void DoCancel();

	enum TState
		{
		EIdle,
		EInitialize,
		EGetKeyStore,
		EGetPublicKey,
		EOpenSigner,
		ESign
		};

private:
	/** Private constructor that initializes essential member variables. */
	CPKCS10Request(const CX500DistinguishedName* aDN,
				   const CCTKeyInfo* aKeyInfo,
				   CPKCS10Attributes* aAttr);

	// Methods making ASN.1 encoding objects

	/**	
	 * Performs the actual ASN.1 encoding of the request without signing it.
	 * certRequestInfo is what gets signed with private key.
	 * @return Pointer to a newly allocated CASN1EncSequence object.
	 */
	CASN1EncSequence* MakeCertRequestInfoEncLC();

	/**
	 * Encodes desired certificate attributes into ASN1. Takes whatever 
	 * attributes are in the iAttributes and adds them below a 
	 * sequence. If there are no attributes stored, leaves the set empty.
	 * 
	 * The structure of the attribute node is as follows:
	 * @code
     * Context-specific[0]
     *    SEQUENCE-OF
     *      OID of the organization
     *      SET-OF
     *        SEQUENCE-OF (stored in iAttributes)
     *          SEQUENCE-OF
     *            OID of attribute
     *            OCTET STRING value
     *          SEQUENCE-OF
     *            OID of attribute
     *            OCTET STRING value
     *          ...
	 * @endcode
	 * @return Pointer to a newly allocated encoding object containing 
	 *     desired certificate attributes.
	 */
	CASN1EncBase* MakeAttrEncLC();

	/**
	 * Generates data to be signed.
	 */
	void EncodeTBSDataL();

	void CreateFinalEncodingL();

	void Reset();

private:
	const CX500DistinguishedName*	iDN;
	const CCTKeyInfo* 				iKeyInfo;
	CPKCS10Attributes*				iAttributes;
	TAlgorithmId					iDigestId;
	TRequestStatus*					iClientStatus;
	TState 							iState;
	HBufC8**						iResult;
	MCTKeyStore*					iKeyStore;
	CPKCS10KeyHelper*				iKeyHelper;
	HBufC8*							iExportedKey;
	HBufC8*							iTBSData;
	};

#endif
