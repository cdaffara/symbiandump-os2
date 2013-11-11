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
* Declares key helper classes for PKCS#10 that perform the algorithm dependant work.
*
*/


/**
 @file
 @internalComponent
 @released 
*/

#ifndef __PKCS10KEYHELPER_H__
#define __PKCS10KEYHELPER_H__

#include <mctkeystore.h>
#include <x509keys.h>
#include "x509keyencoder.h"

class CASN1EncBase;
class CASN1EncSequence;
class CASN1EncBitString;

/**
 * Abstract base class defines the interface for PKCS#10 key helpers.
 *
 * This class is fairly dumb, and provides a generic interface for various
 * keystore functionality.  The methods are called from the RunL of
 * CPKCS10Request.
 *
 * The implementation decodes the public key to create a public key object -
 * this is pretty wasteful, as the first thing we're going to do is re-encode it
 * again, mostly in exactly the same format.  However it's simpler and less
 * error-prome to do it this way.
 */
class CPKCS10KeyHelper : public CBase
	{
 public:

	/**
	 * Create appropriate subclass of CPKCS10KeyHelper depending on key
	 * alogorithm.
	 *
	 * @param aKeyStore The keystore to use - this object takes ownership.
	 * @param aKeyInfo The key to use.
	 */	
	static CPKCS10KeyHelper* CreateKeyHelperL(MCTKeyStore& aKeyStore,
											  const CCTKeyInfo& aKeyInfo,
											  const TDesC8& aExportedKey,
											  const TAlgorithmId aDigestId);

	virtual ~CPKCS10KeyHelper();

 public:

	void FetchPublicKey(TRequestStatus& aStatus);
	void CancelFetchPublicKey();

	virtual void OpenSigner(TRequestStatus& aStatus) = 0;
	virtual void CancelOpenSigner() = 0;

	virtual void SignDigestL(const TDesC8& aDigest, TRequestStatus& aStatus) = 0;
	virtual void CancelSignDigest() = 0;

	virtual CASN1EncBase* EncodeKeyLC();
	virtual CASN1EncSequence* EncodeSignatureAlgorithmLC();
	virtual CASN1EncBitString* EncodeSignatureLC() = 0;
	
 protected:

	CPKCS10KeyHelper(MCTKeyStore& aKeyStore, const CCTKeyInfo& aKeyInfo);

	virtual void CreateKeyEncoderL(const TDesC8& aExportedKey,
								   const TAlgorithmId aDigestId) = 0;
	
	CASN1EncBase* DigestInfoLC(const TDesC8& digest);

 protected:

	MCTKeyStore&		iKeyStore;
	const CCTKeyInfo&	iKeyInfo;
	TX509KeyEncoder*	iKeyEncoder;
	};

/**
 * Implementation of PKCS#10 key helper for RSA keys.
 */
class CPKCS10RSAKeyHelper : public CPKCS10KeyHelper
	{
 public:

	CPKCS10RSAKeyHelper(MCTKeyStore& aKeyStore, const CCTKeyInfo& aKeyInfo);
	virtual ~CPKCS10RSAKeyHelper();

 private:

	virtual void OpenSigner(TRequestStatus& aStatus);
	virtual void CancelOpenSigner();

	virtual void SignDigestL(const TDesC8& aDigest, TRequestStatus& aStatus);
	virtual void CancelSignDigest();	

	virtual void CreateKeyEncoderL(const TDesC8& aExportedKey, const TAlgorithmId aDigestId);
	virtual CASN1EncBitString* EncodeSignatureLC();

private:

	CRSAPublicKey*		iPublicKey;
	MRSASigner*			iRSASigner;
	CRSASignature*		iRSASignature;
	HBufC8* 			iDigestBuf;
	};

/**
 * Implementation of PKCS#10 key helper for DSA keys.
 */
class CPKCS10DSAKeyHelper : public CPKCS10KeyHelper
	{
 public:

	CPKCS10DSAKeyHelper(MCTKeyStore& aKeyStore, const CCTKeyInfo& aKeyInfo);
	virtual ~CPKCS10DSAKeyHelper();

 private:
	
	virtual void OpenSigner(TRequestStatus& aStatus);
	virtual void CancelOpenSigner();

	virtual void SignDigestL(const TDesC8& aDigest, TRequestStatus& aStatus);
	virtual void CancelSignDigest();	

	virtual void CreateKeyEncoderL(const TDesC8& aExportedKey, const TAlgorithmId aDigestId);
	virtual CASN1EncSequence* EncodeSignatureAlgorithmLC();
	virtual CASN1EncBitString* EncodeSignatureLC();

 private:

 	CDSAPublicKey* 		iPublicKey;
	MDSASigner*			iDSASigner;
	CDSASignature*		iDSASignature;
	};

#endif
