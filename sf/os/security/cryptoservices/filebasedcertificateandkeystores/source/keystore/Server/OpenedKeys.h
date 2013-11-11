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
* COpenedKey, abstract base class for performing crypto operations on keys
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __OpenedKeys_h__
#define __OpenedKeys_h__

#include "CKeyDataManager.h"
#include "keystorepassphrase.h"
#include "fsdatatypes.h"

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
#include <authserver/authclient.h>
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

class CRSAPrivateKey;
class CRSASignature;
class CDSAPrivateKey;
class CDSASignature;
class CDHParams;
class CDHPublicKey;
class CFSKeyStoreServer;

class COpenedKey : public CActive
	{
	
protected:
	enum TState
		{
		EIdle,
		EGetPassphrase,
		#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
		EDoAuthenticate,
		EAuthenticate,
		#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
		};
	
public:
	/** Factory function for creating COpenedKey-derived classes. */
	static COpenedKey* NewL(const CFileKeyData& aKeyData, TUid aType, const RMessage2& aMessage,
							CFileKeyDataManager& aKeyDataMan, CPassphraseManager& aPassMan);
	
	virtual ~COpenedKey();

	/** Returns the object's human-readable label */
	const TDesC& Label() const;

	/** Returns the key's handle so we can identifiy it. */
	TInt Handle() const;

	/// The type of opened key
	virtual TUid Type() const = 0;

protected:
	COpenedKey(const CFileKeyData& aKeyData, CFileKeyDataManager& aKeyDataMan, CPassphraseManager& aPassMan);
	void GetPassphrase(TRequestStatus& aStatus);

	// Methods supplied by derived classes

	/// The key algorithm this operation applies to
	virtual CKeyInfo::EKeyAlgorithm Algorithm() const = 0;
	/// The key usage required to perform this operation
	virtual TKeyUsagePKCS15 RequiredUsage() const = 0;
	/// Load the private key data
	virtual void ReadPrivateKeyL(RReadStream& aStream) = 0;
	/// Perform the operation, called from RunL
	virtual void PerformOperationL() = 0;
	/// Clean up, called after normal end error completion
	virtual void Cleanup() = 0;

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
protected:
	void AuthenticateL();
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
private:
	virtual void RunL();
	virtual TInt RunError(TInt aError);
	virtual void DoCancel();

private:
	void ConstructL(const RMessage2& aMessage);
	void CheckKeyL(const RMessage2& aMessage);
	void Complete(TInt aError);

private:
	const CFileKeyData&  iKeyData;
	CFileKeyDataManager& iKeyDataMan;
	CPassphraseManager&  iPassMan;
	HBufC*				 iLabel;
	
	CPassphrase*		 iPassphrase;
	CKeyInfo* 			iKeyInfo;
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	AuthServer::CIdentity* iUserIdentity;
	AuthServer::RAuthClient iAuthClient;
	AuthServer::CAuthExpression* iExpression;
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
protected:
	TState				 iState;
	TRequestStatus*		 iClientStatus;
	TBool				 iKeyRead;
	};

class CRSARepudiableSigner : public COpenedKey
	{
 public:
	CRSARepudiableSigner(const CFileKeyData& aKeyData, CFileKeyDataManager& aKeyDataMan, CPassphraseManager& aPassMan);
	~CRSARepudiableSigner();
	void Sign(const TDesC8& aPlaintext, CRSASignature*& aSignature, TRequestStatus& aStatus);

 private:
	virtual TUid Type() const;
	virtual CKeyInfo::EKeyAlgorithm Algorithm() const;
	virtual TKeyUsagePKCS15 RequiredUsage() const;
	virtual void ReadPrivateKeyL(RReadStream& aStream);
	virtual void PerformOperationL();
	virtual void Cleanup();

 private:
	CRSAPrivateKey* iPrivateKey;
	TPtrC8 			iPlaintext;
	CRSASignature** iSignaturePtr;	
	};

class CDSARepudiableSigner : public COpenedKey
	{
 public:
	CDSARepudiableSigner(const CFileKeyData& aKeyData, CFileKeyDataManager& aKeyDataMan, CPassphraseManager& aPassMan);
	~CDSARepudiableSigner();
	void Sign(const TDesC8& aPlaintext, CDSASignature*& aSignature, TRequestStatus& aStatus);

 private:	
	virtual TUid Type() const;
	virtual CKeyInfo::EKeyAlgorithm Algorithm() const;
	virtual TKeyUsagePKCS15 RequiredUsage() const;
	virtual void ReadPrivateKeyL(RReadStream& aStream);
	virtual void PerformOperationL();
	virtual void Cleanup();

 private:
	CDSAPrivateKey* iPrivateKey;
	TPtrC8 			iPlaintext;
	CDSASignature** iSignaturePtr;	
	};

class CFSRSADecryptor : public COpenedKey
	{
 public:
	CFSRSADecryptor(const CFileKeyData& aKeyData, CFileKeyDataManager& aKeyDataMan, CPassphraseManager& aPassMan);
	~CFSRSADecryptor();
	void Decrypt(const TDesC8& aCiphertext, HBufC8*&, TRequestStatus& aStatus);

 private:
	virtual TUid Type() const;
	virtual CKeyInfo::EKeyAlgorithm Algorithm() const;
	virtual TKeyUsagePKCS15 RequiredUsage() const;
 	virtual void ReadPrivateKeyL(RReadStream& aStream);
	virtual void PerformOperationL();
	virtual void Cleanup();
	
 private:
	CRSAPrivateKey* iPrivateKey;
	TPtrC8 			iCiphertext;
	HBufC8**		iPlaintextPtr;
	};

class CDHAgreement : public COpenedKey
	{
 public:
	CDHAgreement(const CFileKeyData& aKeyData, CFileKeyDataManager& aKeyDataMan, CPassphraseManager& aPassMan);
	~CDHAgreement();
	void PublicKey(CDHParams& aParameters, RInteger& aPublicKey, TRequestStatus& aStatus);
	void Agree(CDHPublicKey& aY, HBufC8*& aAgreedKey, TRequestStatus& aStatus);

 private:
	virtual TUid Type() const;
	virtual CKeyInfo::EKeyAlgorithm Algorithm() const;
	virtual TKeyUsagePKCS15 RequiredUsage() const;
	virtual void ReadPrivateKeyL(RReadStream& aStream);
	virtual void PerformOperationL();
	virtual void Cleanup();

 private:
	void DoPublicKeyL();
	void DoAgreeL();
	
	enum TDHState
		{
		EIdle,
		EPublicKey,
		EAgree
		};

 private:
	RInteger 	  iKey;
	TDHState	  iDHState;
	// For public key operation
	CDHParams* 	  iPKParams;
	RInteger* 	  iPKPublicKeyPtr;
	// For agree key operation
	CDHPublicKey* iAKPublicKey;
	HBufC8** 	  iAKAgreedKeyPtr;
	};

#endif
