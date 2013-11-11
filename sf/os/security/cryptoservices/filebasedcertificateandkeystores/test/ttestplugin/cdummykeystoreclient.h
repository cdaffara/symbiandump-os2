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


#ifndef __CDUMMYKEYSTORECLIENT_H__
#define __CDUMMYKEYSTORECLIENT_H__

#include <mctkeystoremanager.h>

/**
 * Implements the MCTKeyStoreManager interface.
 *
 * The class implements the keystore manager token interface for the software
 * keystore.  It will be created and owned by the unified keystore.
 */
NONSHARABLE_CLASS(CDummyKeyStoreClient) : public MCTKeyStoreManager
{
public://	One creator function for each token interface
	static MCTTokenInterface* NewKeyStoreInterfaceL(MCTToken& aToken);

public:
	virtual ~CDummyKeyStoreClient();
	void AddRef();

public:
//	*********************************************************************************	
//	MKeyStore
	virtual void List(RMPointerArray<CCTKeyInfo>& aKeys, const TCTKeyAttributeFilter& aFilter, TRequestStatus& aStatus);
	virtual void CancelList();
	virtual void GetKeyInfo(TCTTokenObjectHandle aHandle, CCTKeyInfo*& aInfo, TRequestStatus& aStatus);
	virtual void CancelGetKeyInfo();
	
	/** Open an RSA key for signing	*/
	virtual void Open(const TCTTokenObjectHandle& aHandle, 
					  MRSASigner*& aSigner,
					  TRequestStatus& aStatus);
	/** Open a DSA key for signing */
	virtual void Open(const TCTTokenObjectHandle& aHandle, 
					  MDSASigner*& aSigner, 
					  TRequestStatus& aStatus);
	/** Open a RSA key for private decryption */
	virtual void Open(const TCTTokenObjectHandle& aHandle, 
					  MCTDecryptor*& aDecryptor,
					  TRequestStatus& aStatus);
	/** Open a DH key for key agreement */
	virtual void Open(const TCTTokenObjectHandle& aHandle, 
					  MCTDH*& aDH, TRequestStatus& aStatus);


	/// Cancels an ongoing open request
	virtual void CancelOpen();
	
	/** Returns the public key in DER-encoded ASN-1 */
	virtual void ExportPublic(const TCTTokenObjectHandle& aHandle,
							  HBufC8*& aPublicKey,
							  TRequestStatus& aStatus);
	virtual void CancelExportPublic();

public:
//	*********************************************************************************	
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
//	*********************************************************************************
//	From MCTTokenInterface
	virtual MCTToken& Token();

public:
//	*********************************************************************************
	void RepudiableDSASign(const TCTTokenObjectHandle& aObject, const TDesC8& aDigest, CDSASignature*& aSignature, TRequestStatus& aStatus);
	void RepudiableRSASign(const TCTTokenObjectHandle& aObject, const TDesC8& aDigest, CRSASignature*& aSignature, TRequestStatus& aStatus);
	void Decrypt(const TCTTokenObjectHandle& aObject, const TDesC8& aCiphertext, TDes8& aPlaintext, TRequestStatus& aStatus);
	void DHPublicKey(const TCTTokenObjectHandle& aHandle, const TInteger& aN, const TInteger& aG, CDHPublicKey*& aX, TRequestStatus& aStatus);
	void DHAgree(const TCTTokenObjectHandle& aHandle, const CDHPublicKey& iY, HBufC8*& aAgreedKey, TRequestStatus& aStatus);

protected:
//	*********************************************************************************
//	From MCTTokenInterface
	virtual void DoRelease();
protected:
//	*********************************************************************************
	CDummyKeyStoreClient(MCTToken& aToken);
	virtual void ConstructL();

private:
	TInt iRefCount;
	
private:
	MCTToken& iToken;					///< The token we belong to
};

#endif	//	__CDUMMYKEYSTORECLIENT_H__
