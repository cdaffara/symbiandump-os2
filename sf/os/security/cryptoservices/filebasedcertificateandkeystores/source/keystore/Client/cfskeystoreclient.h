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
* Implements CFSKeyStoreClient
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CFSKEYSTORECLIENT_H__
#define __CFSKEYSTORECLIENT_H__

#include "CFSClient.h"
#include <mctkeystoremanager.h>
#include <ct/rmpointerarray.h>
#include "CKeyStoreAuthObject.h"

/** Maximum allowed size of digest to RSA sign (implied by max 2048 bit key length). */
const TInt KMaxRSADigestSize = 2048 / 8;

/** Maximum allowed size of digest to DSA sign (implied by 160 bit output of SHA1). */
const TInt KMaxDSADigestSize = 160 / 8;

class COpenedKey;
class MKeyInfoArray;
class CDHParams;

/**
 * Implements the MCTKeyStoreManager interface.  Inherits from CFSClient.
 *
 * The class implements the keystore manager token interface for the software
 * keystore.  It will be created and owned by the unified keystore.  Its main
 * function is to marshall user requests to the filetokens server and unmarshall
 * the results back again.
 */
NONSHARABLE_CLASS(CFSKeyStoreClient) : public CFSClient, public MCTKeyStoreManager
{
public://	One creator function for each token interface
	static MCTTokenInterface* NewKeyStoreUserInterfaceL(MCTToken& aToken, RFileStoreClientSession& aClient);
	static MCTTokenInterface* NewKeyStoreManagerInterfaceL(MCTToken& aToken, RFileStoreClientSession& aClient);
public:
	virtual ~CFSKeyStoreClient();
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

#ifdef SYMBIAN_AUTH_SERVER

	virtual void CreateKey(	const TDesC& aAuthenticationString, 
							TInt aFreshness,
							CCTKeyInfo*& aReturnedKey,
							TRequestStatus& aStatus );

	virtual void ImportKey( const TDesC8& aKey, 
							const TDesC& aAuthenticationString, 
							TInt aFreshness, CCTKeyInfo*& aReturnedKey, 
							TRequestStatus& aStatus );
			

	virtual void ImportEncryptedKey(const TDesC8& aKey, 
									const TDesC& aAuthenticationString, 
									TInt aFreshness, CCTKeyInfo*& aReturnedKey, 
									TRequestStatus& aStatus );
	
	virtual void SetAuthenticationPolicy(	const TCTTokenObjectHandle aHandle,
											const TDesC& aAuthenticationString,
											TInt aFreshness,
											TRequestStatus& aStatus);

	virtual void GetAuthenticationPolicy(	const TCTTokenObjectHandle aHandle,
											HBufC*& aAuthenticationString,
											TInt& aFreshness,
											TRequestStatus& aStatus);

#endif // SYMBIAN_AUTH_SERVER
	
	void ReleaseObject(const TCTTokenObjectHandle& aObject);
public:	
//	*********************************************************************************
//	From MCTTokenInterface
	virtual MCTToken& Token();
public:
//	*********************************************************************************
	void RepudiableDSASign(const TCTTokenObjectHandle& aObject, const TDesC8& aDigest, CDSASignature*& aSignature, TRequestStatus& aStatus);
	void CancelRepudiableDSASign();
	void RepudiableRSASign(const TCTTokenObjectHandle& aObject, const TDesC8& aDigest, CRSASignature*& aSignature, TRequestStatus& aStatus);
	void CancelRepudiableRSASign();
	void Decrypt(const TCTTokenObjectHandle& aObject, const TDesC8& aCiphertext, TDes8& aPlaintext, TRequestStatus& aStatus);
	void CancelDecrypt();
	void DHPublicKey(const TCTTokenObjectHandle& aHandle, const TInteger& aN, const TInteger& aG, CDHPublicKey*& aX, TRequestStatus& aStatus);
	void DHAgree(const TCTTokenObjectHandle& aHandle, const CDHPublicKey& iY, HBufC8*& aAgreedKey, TRequestStatus& aStatus);
	void CancelDH();

public:
	// Called by CKeyStoreAuthObject
	void ListProtectedObjects(RMPointerArray<MCTTokenObject>& aObjects, TRequestStatus& aStatus);
	void ChangeReferenceData(TRequestStatus &aStatus);
	void CancelChangeReferenceData();
	TUint32 AuthStatus() const;
	void AuthOpen(TRequestStatus& aStatus);
	void CancelAuthOpen();
	void AuthClose(TRequestStatus& aStatus);
	void TimeRemaining(TInt& aStime, TRequestStatus& aStatus);
	void SetTimeout(TInt aTime, TRequestStatus& aStatus);
	void Timeout(TInt& aTime, TRequestStatus& aStatus);
	
protected:
//	*********************************************************************************
//	From MCTTokenInterface
	virtual void DoRelease();
protected:
//	*********************************************************************************
//	From CActive
protected:	
	virtual void RunL();
	virtual TInt RunError(TInt aError);
	virtual void DoCancel();
protected:
//	*********************************************************************************
	CFSKeyStoreClient(TInt aUID, MCTToken& aToken, RFileStoreClientSession& aClient);
	virtual void ConstructL();
private:
	void DoListL(const TCTKeyAttributeFilter& aFilter, MKeyInfoArray& aOut);
	void DoGetKeyInfoL(TCTTokenObjectHandle aHandle, CCTKeyInfo*& aInfo);
	TInt DoOpenKey(TFSTokenMessages aMessage, const TCTTokenObjectHandle& aHandle, COpenedKey* aOpenedKey);
	void DoImportKey(TFSTokenMessages aMessage, const TDesC8& aKey, CCTKeyInfo*& aReturnedKey, TRequestStatus& aStatus);
	void DoImportUserKey(	TFSTokenMessages aMessage, 
							const TDesC8& aKey, 
							CCTKeyInfo*& aReturnedKey,
							const TDesC& aAuthExpression,
							TInt aFreshness,
							TRequestStatus& aStatus);
	void DoExportPublicL(const TCTTokenObjectHandle& aHandle, HBufC8*& aPublicKey);
	TInt MarshalKeyInfo(CCTKeyInfo& aKey);
	void Process(const TCTTokenObjectHandle& aHandle,const TDesC8& aText,TFSTokenMessages aMessage,TRequestStatus& aStatus,TInt aBufSize);
	void DoDHPublicKeyL(const TCTTokenObjectHandle& aHandle, const TInteger& aN, const TInteger& aG, CDHPublicKey*& aX, TRequestStatus& aStatus);
private:
	TInt AllocKeyBuffer(TInt aReqdSize);
	void FreeKeyBuffer();
	void UpdateKey();
	TUint GetKeySizeL(TInt aObjectId);
	void Cleanup();
private:
	TInt iRefCount;
	//	Data holders for server to write into
	TPckgBuf<TInt> iIntOut;
	
	TInt iObjectId; 		// Id of object passed to server

private:
	/**
	 * Pointers to client pointers, which we set to return created objects.
	 * Only one of these is ever in use at any one time, determined by the
	 * state.
	 */
	union 
		{
		TAny* iAny;						///< generic pointer, used to zero the contents of the union
		CCTKeyInfo** iKeyInfo;			///< key info object, used for create and import
		HBufC8** iBuffer;				///< generic buffer, used for export key and DH agree
		TDes8* iDecryptedText;			///< decypled plaintext, used for RSA decrypt
		CDSASignature** iDSASignature;  ///< DSA signature
		CRSASignature** iRSASignature;	///< RSA signature
		CDHPublicKey** iDHPublicKey;	///< DH public key
		} iClientPtr;
private:
	HBufC8* iKey;						///< Key data buffer for import/export
	HBufC8* iText;						///< Buffer used to hold text for decrypt and sign
	CKeyStoreAuthObject* iAuthObject;   ///< The single auth object that serves as protector for all keys
	CDHParams* iDHParams;				///< DH params for DH public key
	HBufC8* iPbeParamsBuf;				///< Buffer holding PBE parameters for encrypted key export

#ifdef SYMBIAN_AUTH_SERVER
	TBool iUseNewKeyServer;
	HBufC* iAuthExpression;
	TInt iFreshness;
#endif // SYMBIAN_AUTH_SERVER
	
};

#endif	//	__CFSKEYSTORECLIENT_H__
