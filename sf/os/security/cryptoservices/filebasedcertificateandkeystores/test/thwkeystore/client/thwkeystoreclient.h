/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements the MCTKeyStoreManager interface.
*
*/


/**
 * @file
 * @internalComponent
 * @released
 */
#ifndef CHARDWAREKEYSTORECLIENT_H
#define CHARDWAREKEYSTORECLIENT_H

#include <mctkeystoremanager.h>

#include "cryptoasymmetriccipherapi.h"
#include <asymmetric.h>
#include <cryptospi/cryptoparams.h>
#include "thwkeystoredefs.h"
#include "tcryptotokenhai.h"

/**
 * Implements the MCTKeyStoreManager interface.
 *
 * The class implements the keystore manager token interface for the software
 * keystore.  It will be created and owned by the unified keystore.
 */
NONSHARABLE_CLASS(CHardwareKeyStoreClient) : public CActive, public MCTKeyStoreManager
{
public:
	static MCTTokenInterface* NewKeyStoreInterfaceL(MCTToken& aToken);

public:
	virtual ~CHardwareKeyStoreClient();
	void AddRef();

public:
	void RunL();
	virtual void DoCancel();
	TInt RunError(TInt aError);
	
public:
//	MKeyStore
	virtual void List(RMPointerArray<CCTKeyInfo>& aKeys, const TCTKeyAttributeFilter& aFilter, TRequestStatus& aStatus);
	virtual void CancelList();
	virtual void GetKeyInfo(TCTTokenObjectHandle aHandle, CCTKeyInfo*& aInfo, TRequestStatus& aStatus);
	virtual void CancelGetKeyInfo();
	
	// Open an RSA key for signing
	virtual void Open(const TCTTokenObjectHandle& aHandle, 
					  MRSASigner*& aSigner,
					  TRequestStatus& aStatus);
	// Open a DSA key for signing
	virtual void Open(const TCTTokenObjectHandle& aHandle, 
					  MDSASigner*& aSigner, 
					  TRequestStatus& aStatus);
	// Open a RSA key for private decryption
	virtual void Open(const TCTTokenObjectHandle& aHandle, 
					  MCTDecryptor*& aDecryptor,
					  TRequestStatus& aStatus);
	// Open a DH key for key agreement
	virtual void Open(const TCTTokenObjectHandle& aHandle, 
					  MCTDH*& aDH, TRequestStatus& aStatus);


	// Cancels an ongoing open request
	virtual void CancelOpen();
	
	// Returns the public key in DER-encoded ASN-1
	virtual void ExportPublic(const TCTTokenObjectHandle& aHandle,
							  HBufC8*& aPublicKey,
							  TRequestStatus& aStatus);
	virtual void CancelExportPublic();

	virtual void Open(const TCTTokenObjectHandle& aHandle,
		                  CryptoSpi::CSigner*& aSigner,
		                  TRequestStatus& aStatus);
	
	virtual void Open(const TCTTokenObjectHandle& aHandle,
		                  CryptoSpi::CAsymmetricCipher*& asymmetricCipherObj,
		                  TRequestStatus& aStatus);
	
	virtual void Decrypt(const TCTTokenObjectHandle& aHandle,
                            const TDesC8& aCiphertext,
		                       HBufC8*& aPlaintextPtr,
		                       TRequestStatus& aStatus);
	
	virtual void Sign(const TCTTokenObjectHandle& aHandle,
                           const TDesC8& aPlaintext,
		                    CryptoSpi::CCryptoParams*& aSignature,
		                    TRequestStatus& aStatus);

public:
//	MCTKeyStoreManager
	virtual void CreateKey(CCTKeyInfo*& aReturnedKey, TRequestStatus& aStatus);
	virtual void CancelCreateKey();
	virtual void ImportKey(const TDesC8& aKey, CCTKeyInfo*& aReturnedKey, TRequestStatus& aStatus);
	virtual void CancelImportKey();
	virtual void ImportEncryptedKey(const TDesC8& aKey, CCTKeyInfo*& aReturnedKey, TRequestStatus& aStatus);
	virtual void CancelImportEncryptedKey();
	virtual void ExportKey(TCTTokenObjectHandle aHandle, HBufC8*& aKey, TRequestStatus& aStatus); 
	virtual void CancelExportKey();
	virtual void ExportEncryptedKey(TCTTokenObjectHandle aHandle, const CPBEncryptParms& aParams, HBufC8*& aKey, TRequestStatus& aStatus);
	virtual void CancelExportEncryptedKey();
	virtual void DeleteKey(TCTTokenObjectHandle aHandle, TRequestStatus& aStatus);
	virtual void CancelDeleteKey();	
	virtual void SetUsePolicy(TCTTokenObjectHandle aHandle, const TSecurityPolicy& aPolicy, TRequestStatus& aStatus);
	virtual void CancelSetUsePolicy();
	virtual void SetManagementPolicy(TCTTokenObjectHandle aHandle, const TSecurityPolicy& aPolicy, TRequestStatus& aStatus);
	virtual void CancelSetManagementPolicy();
	virtual void SetPassphraseTimeout(TInt aTimeout, TRequestStatus& aStatus);
	virtual void CancelSetPassphraseTimeout();	
	virtual void Relock(TRequestStatus& aStatus);
	virtual void CancelRelock();	

	void ReleaseObject(const TCTTokenObjectHandle& aObject);

public:
	/**
	 * this method os not a part of the framework, has been added so 
	 * that addition of keys to the hardware can be simulated for 
	 * testing purposes.
	 */ 
	void ImportKey(	TPtr aLabel, TPtr8 aPrivateKey, 
					TPtr8 aPublicKey , TRequestStatus& aStatus);

public:	
//	From MCTTokenInterface
	virtual MCTToken& Token();

protected:
//	From MCTTokenInterface
	virtual void DoRelease();

protected:
	CHardwareKeyStoreClient(MCTToken& aToken);
	virtual void ConstructL();

private:
	TInt iRefCount;
	MCTToken& iToken;					// The token we belong to
	THwKeystoreMessages iState;
	TRequestStatus* iClientStatus;
	TCTTokenObjectHandle iHandle;
	RPointerArray<CCTKeyInfo>* iKeys;
	TBuf8<256> iBuffer;
	CCryptoTokenHai* iCryptoTokenHai;
	HBufC8** iPublicKey;
	const TDesC8* iPtr; 
	TCTKeyAttributeFilter iFilter;
	union
	{
	CryptoSpi::CSigner** iSigner;
	CryptoSpi::CAsymmetricCipher** iAsymmetricCipher;
	CryptoSpi::CCryptoParams* iSignature;
	HBufC8** decryptedText;
	}iClientBuffer;
	
};

#endif	//	CHARDWAREKEYSTORECLIENT_H
