/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/
 
#ifndef __CLIENTOPENEDKEYS_H__
#define __CLIENTOPENEDKEYS_H__

class CFSKeyStoreClient;
class CMessageDigest;

NONSHARABLE_CLASS(COpenedKey) : protected CBase
	{
 public:
	void SetObjectID(TInt aObjectId) 
		{iHandle.iObjectId = aObjectId;}

	void SetLabel(HBufC* aLabel)
		{iLabel = aLabel;}

	/**
	 * Virtual Release method visible though COpenedKey pointers - the
	 * implementation just calls the Release method from the mixin class in each
	 * case.
	 */
  	virtual void Release() = 0;

 protected:
	virtual ~COpenedKey();

 protected:
	TCTTokenObjectHandle iHandle;
	CFSKeyStoreClient* iClient;
	HBufC* iLabel;
	};

NONSHARABLE_CLASS(CRSARepudiableSigner) : public COpenedKey, public MRSASigner
	{
 public:
	static CRSARepudiableSigner* New(CFSKeyStoreClient* aClient);

	/** Returns the object's human-readable label */
	virtual const TDesC& Label() const;
	/** Returns a reference to the associated token */
	virtual MCTToken& Token() const;
	/** Returns a UID representing the type of the token object. The
		meanings of possible UIDs should be documented in the
		documentation for the interface that returns them. */
	virtual TUid Type() const;
	/** Returns a handle for the object. The primary purpose of the
		handle is to allow token objects to be 'passed' between
		processes. See TCTTokenObjectHandle for more details.
	*/
	virtual TCTTokenObjectHandle Handle() const;
		
/**	Sign some data. 
	The data is hashed before the signature is created using the SHA-1
	algorithm.
 	@param aPlaintext The string to be signed.
	@param aSignature The returned signature. This remains owned
	by the signer, and is valid until the signer is released or
	until the next Sign.
	@param aStatus Async status notification
*/
	virtual void SignMessage(const TDesC8& aPlaintext, CRSASignature*& aSignature, 
			  TRequestStatus& aStatus);


/**	Perform a raw signing operation.
	
	@param aDigest The data to be signed - this should be some form of hash of
	the actual message to be signed.  If the data is too long, this method will
	return KErrOverflow through aStatus.	
	@param aSignature The returned signature. This remains owned
	by the signer, and is valid until the signer is released or
	until the next Sign.
	@param aStatus Async status notification
*/
	virtual void Sign(const TDesC8& aData, CRSASignature*& aSignature, 
					  TRequestStatus& aStatus);


	/// Cancel an ongoing sign
	virtual void CancelSign();
	
	virtual void Release();

 private:
	CRSARepudiableSigner(CFSKeyStoreClient* aClient);
	virtual ~CRSARepudiableSigner();

 private:
	CMessageDigest* iDigest;
	HBufC8* iSignature;	
	};

NONSHARABLE_CLASS(CDSARepudiableSigner) : public COpenedKey, public MDSASigner
	{
 public:
	static CDSARepudiableSigner* New(CFSKeyStoreClient* aClient);
	
	/** Returns the object's human-readable label */
	virtual const TDesC& Label() const;
	/** Returns a reference to the associated token */
	virtual MCTToken& Token() const;
	/** Returns a UID representing the type of the token object. The
		meanings of possible UIDs should be documented in the
		documentation for the interface that returns them. */
	virtual TUid Type() const;
	/** Returns a handle for the object. The primary purpose of the
		handle is to allow token objects to be 'passed' between
		processes. See TCTTokenObjectHandle for more details.
	*/
	virtual TCTTokenObjectHandle Handle() const;
	
	/**	Sign some data. 

		@param aPlaintext The string to be signed. 
		@param aSignature The returned signature. This remains owned
		by the signer, and is valid until the signer is released or
		until the next Sign.
		@param aStatus Async status notification
	   */
	virtual void SignMessage(const TDesC8& aPlaintext, CDSASignature*& aSignature, 
					  TRequestStatus& aStatus);

	/**	Perform a raw signing operation.
	 
	    This method implements a raw sign, without the SHA1 hashing
	    mandated by the DSA specification. 

		@param aPlaintext The string to be signed - this should be some form of
		hash of the actual message to be signed.  If the data is too long, this
		method will return KErrOverflow through aStatus.
		@param aSignature The returned signature. This remains owned
		by the signer, and is valid until the signer is released or
		until the next Sign.
		@param aStatus Async status notification
	   */
	virtual void Sign(const TDesC8& aData, CDSASignature*& aSignature, 
					  TRequestStatus& aStatus);
	/// Cancel an ongoing sign
	virtual void CancelSign();
	
	virtual void Release();
	
 private:
	CDSARepudiableSigner(CFSKeyStoreClient* aClient);
	virtual ~CDSARepudiableSigner();

 private:
	CMessageDigest* iDigest;
	};

NONSHARABLE_CLASS(CFSRSADecryptor) : public COpenedKey, public MCTDecryptor
	{
 public:
	static CFSRSADecryptor* New(CFSKeyStoreClient* aClient);

	/** Returns the object's human-readable label */
	virtual const TDesC& Label() const;
	/** Returns a reference to the associated token */
	virtual MCTToken& Token() const;
	/** Returns a UID representing the type of the token object. The
		meanings of possible UIDs should be documented in the
		documentation for the interface that returns them. */
	virtual TUid Type() const;
	/** Returns a handle for the object. The primary purpose of the
		handle is to allow token objects to be 'passed' between
		processes. See TCTTokenObjectHandle for more details.
	*/
	virtual TCTTokenObjectHandle Handle() const;
	
	/** Do a private decrypt.
		@param aCiphertext The text to decrypt
		@param aPlaintext The returned plaintext
		@param aStatus Async status notification 
	*/
	virtual void Decrypt(const TDesC8& aCiphertext, TDes8& aPlaintext,
				 TRequestStatus& aStatus);
	/// Cancel an ongoing decrypt
	virtual void CancelDecrypt();
	virtual void Release();

 private:
	CFSRSADecryptor(CFSKeyStoreClient* aClient);
	virtual ~CFSRSADecryptor();
	};

NONSHARABLE_CLASS(CDHAgreement) : public COpenedKey, public MCTDH
	{
 public:
	static CDHAgreement* New(CFSKeyStoreClient* aClient);

	/** Returns the object's human-readable label */
	virtual const TDesC& Label() const;
	/** Returns a reference to the associated token */
	virtual MCTToken& Token() const;
	/** Returns a UID representing the type of the token object. The
		meanings of possible UIDs should be documented in the
		documentation for the interface that returns them. */
	virtual TUid Type() const;
	/** Returns a handle for the object. The primary purpose of the
		handle is to allow token objects to be 'passed' between
		processes. See TCTTokenObjectHandle for more details.
	*/
	virtual TCTTokenObjectHandle Handle() const;
	
	/** Returns the public key ('Big X') for the supplied set of parameters */
	virtual void PublicKey(const TInteger& aN, const TInteger& aG, 
						   CDHPublicKey*& aX,
						   TRequestStatus& aStatus);
	/** Agrees a session key given the public key of the other party */
	virtual void Agree(const CDHPublicKey& iY,
					   HBufC8*& aAgreedKey,
					   TRequestStatus& aStatus);
	/** Cancels either a PublicKey or Agree operation */
	virtual void CancelAgreement();
	virtual void Release();

 private:
	CDHAgreement(CFSKeyStoreClient* aClient);
	virtual ~CDHAgreement();
	};
#endif
