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
* Implements CKeyStoreConduit
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CKEYSTORECONDUIT_H__
#define __CKEYSTORECONDUIT_H__

#include "fsserver.h"
#include <bigint.h>

class CDSASignature;
class CRSASignature;
class CFSKeyStoreServer;
class CKeyStoreSession;
class CRSARepudiableSigner;
class CDSARepudiableSigner;
class CFSRSADecryptor;
class CDHParams;
class COpenedKey;
class CKeyInfo;
class CPBEncryptParms;
class CDHPublicKey;

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
#include <authserver/identity.h>
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER


/**
 * The key store server object (CFSKeyStoreServer) owns one instance of this
 * object, which it calls for every client request.  This object unmarshalls the
 * client data and fulfills the request by calling back to the server object.
 * Any return information is then marshalled back to the client.
 */
class CKeyStoreConduit : public CActive
	{
 public:
	static CKeyStoreConduit* NewL(CFSKeyStoreServer& aServer);
	virtual ~CKeyStoreConduit();
 public:
	void ServiceRequestL(const RMessage2& aMessage, CKeyStoreSession& aSession);
 protected:	//	From CActive
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);
 private:
	CKeyStoreConduit(CFSKeyStoreServer& aServer);	
	void ConstructL();
 private:
	CKeyStoreConduit(const CKeyStoreConduit&);				//	No copying
	CKeyStoreConduit& operator=(const CKeyStoreConduit&);	//	No copying
 private:
	// For MKeyStore
	void ListL(const RMessage2& aMessage);
	void CancelList(const RMessage2& aMessage);
	void GetKeyInfoL(const RMessage2& aMessage);
	void CancelGetKeyInfo(const RMessage2& aMessage);
 private:
	// For MCTKeyStoreManager
	void CreateKeyL(const RMessage2& aMessage);
	void CancelCreateKey(const RMessage2& aMessage);
	void ImportKeyL(const RMessage2& aMessage);
	void CancelImportKey(const RMessage2& aMessage);
	void ExportKeyL(const RMessage2& aMessage); 
	void CancelExportKey(const RMessage2& aMessage);
	void ExportPublicL(const RMessage2& aMessage); 
	void CancelExportPublic(const RMessage2& aMessage);
	void CancelExportEncryptedKey(const RMessage2& aMessage);
	void DeleteKeyL(const RMessage2& aMessage);
	void SetUsePolicyL(const RMessage2& aMessage);
	void SetManagementPolicyL(const RMessage2& aMessage);
	void SetPassphraseTimeoutL(const RMessage2& aMessage);
	void RelockL(const RMessage2& aMessage);	
	void OpenKeyL(const RMessage2& aMessage, CKeyStoreSession& aSession, TUid aType);
	void CloseObjectL(const RMessage2& aMessage, CKeyStoreSession& aSession);
	
#ifdef SYMBIAN_AUTH_SERVER
	void IsKeyServerNewL(const RMessage2& aMessage);
#endif // SYMBIAN_AUTH_SERVER
	
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	void CreateUserKeyL(const RMessage2& aMessage);
	void ImportUserKeyL(const RMessage2& aMessage);
	void SetAuthenticationPolicyL(const RMessage2& aMessage);
	void GetAuthenticationPolicyL(const RMessage2& aMessage);
	
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
 private:
	// For MCTAuthenticationObject
	void ChangePassphrase(const RMessage2& aMessage);
	void CancelChangePassphrase(const RMessage2& aMessage);
	void AuthOpen(const RMessage2& aMessage);
	void CancelAuthOpen(const RMessage2& aMessage);
	void AuthClose(const RMessage2& aMessage);
	void GetTimeRemaining(const RMessage2& aMessage);
	void SetTimeout(const RMessage2& aMessage);
	void GetTimeout(const RMessage2& aMessage);
	void Relock(const RMessage2& aMessage);
 private:
	void GetKeyLengthL(const RMessage2& aMessage); 
	TUid GetClientUidL(const RMessage2& aMessage) const;
	void OpenKeyGetInfoL(const RMessage2& aMessage);
	void RepudiableDSASignL(const RMessage2& aMessage, CKeyStoreSession& aSession);
	void CancelDSASign(const RMessage2& aMessage, CKeyStoreSession& aSession);
	void RepudiableRSASignL(const RMessage2& aMessage, CKeyStoreSession& aSession);
	void CancelRSASign(const RMessage2& aMessage, CKeyStoreSession& aSession);
	void DecryptL(const RMessage2& aMessage, CKeyStoreSession& aSession);
	void CancelDecrypt(const RMessage2& aMessage, CKeyStoreSession& aSession);
	void DHPublicKeyL(const RMessage2& aMessage, CKeyStoreSession& aSession);
	void DoDHPublicKeyL(const RMessage2& aMessage, CKeyStoreSession& aSession);
	void FinishDHPublicKeyL();
	void DHAgreeL(const RMessage2& aMessage, CKeyStoreSession& aSession);
	void DoDHAgreeL(const RMessage2& aMessage, CKeyStoreSession& aSession);
	void FinishDHAgreeL();
	void CancelDH(const RMessage2& aMessage, CKeyStoreSession& aSession);
	COpenedKey* ProcessL(const RMessage2& aMessage, CKeyStoreSession& aSession,
						 const TUid& aCKeyInfoID, const TFSTokenMessages& aState, TPtr8& aPtr);
	void FinishOpenL(const RMessage2& aMessage, CKeyStoreSession& aSession, COpenedKey& aOpenedKey);
	void Cleanup();
 private:
	/** Encapsulate data about the current request. */
	class TAsyncRequest
		{
	public:
		void Set(TFSTokenMessages aRequest, const RMessage2& aMessage);
		void Complete(TInt aResult);
		void Cancel();
	public:
		inline TFSTokenMessages OutstandingRequest() { return iRequest; }
		inline const RMessage2& Message() { return iMessage ;}
	public:
		TAsyncRequest(TRequestStatus& aStatus);
		~TAsyncRequest();
	private:
		TFSTokenMessages iRequest;
		RMessage2 iMessage;
		TRequestStatus& iOwnerStatus;
		}; 
 private:
	CFSKeyStoreServer& iServer;				///< The server object used to execute client requests
	TAsyncRequest iCurrentRequest;			///< The client request we are currently servicing
	// unionize where possible
	HBufC8* iImportKey;						///< Buffer for key imported key data 
	CKeyInfo* iKeyInfo;						///< Key info, used by create, import and export
	HBufC8* iText;							///< Stores input data for sign and decrypt operations
	HBufC8* iExportBuf;						///< Holds the exported key data
	CPBEncryptParms* iPbeParams;            ///< Parameters for encrypted key export
	COpenedKey* iOpenedKey;					///< The key object for crypto operations
	CDSASignature* iDSASignature;			///< Signature to return for DSA sign operation
	CRSASignature* iRSASignature;			///< Signature to return for RSA sign operation
	CDHParams* iDHParams;					///< DH parameters for DU public key operation
	RInteger iDHPublicKeyOut;				///< DH public key to return to client
	CDHPublicKey* iDHPublicKey;				///< DH public key and params object for agree operation 
	HBufC8* iDHAgreedKeyOut;				///< Agreed key to return to client
	HBufC8* iPlaintext;						///< Buffer to hold decrypted data
	
	};

#endif	//	__CKEYSTORECONDUIT_H__
