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
* Implements CFSKeyStoreServer  
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CFSKEYSTORESERVER_H__
#define __CFSKEYSTORESERVER_H__

#include <mctkeystoremanager.h>
#include <s32file.h>
#include <secdlg.h>

#include "fstokencliserv.h"

class CDecPKCS8Data;
class CKeyCreator;
class CFileKeyData;
class CPassphrase;
class CPassphraseManager;
class CFileKeyDataManager;

class CRSARepudiableSigner;
class CDSARepudiableSigner;
class CFSRSADecryptor;
class CDHAgreement;
class COpenedKey;
class CKeyInfo;
class CKeyStoreSession;
class CKeyStoreConduit;

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER

#include <authserver/authclient.h>

class CKeyStoreCenrep;

#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

/**	Server side implementation of keystore interfaces as an active object. */
class CFSKeyStoreServer : public CActive
{
private:
	enum TCurrentAction
		{
		EIdle,
		ECreateKeyCreate,
 		ECreateKeyFinal,
		EImportKey,
		EExportKey,
		EExportEncryptedKey,
		EDoExportKey,
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
		ECreateUserKey,
		EImportUserKey,
		ESetAuthenticationPolicy,
		ESetAuthPolicy,
		EDoImportKey,
		EDoSetAuthenticationPolicy,
		EAuthenticate,
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
		EImportOpenPrivateStream,
		EExportKeyGetPassphrase,
		EExportEncryptedKeyGetPassphrase,
		EChangePassphrase,
		EChangePassphraseClearCached,
		EAuthOpen,
		EGetPassphrase,

		};

	/**
	 * The operations that can create a new key, used by CheckKeyAttributes.
	 */
	enum TNewKeyOperation
		{
		ENewKeyCreate,
		ENewKeyImportPlaintext,
		ENewKeyImportEncrypted
		};

public:
	static CFSKeyStoreServer* NewL();
	virtual ~CFSKeyStoreServer();
	CKeyStoreSession* CreateSessionL();
	void RemoveSession(CKeyStoreSession& aSession);
	void ServiceRequestL(const RMessage2& aMessage, CKeyStoreSession& aSession);
public:
	// For MKeyStore
	void ListL(const TCTKeyAttributeFilter& aFilter, RPointerArray<CKeyInfo>& aKeys);
	void GetKeyInfoL(TInt aObjectId, CKeyInfo*& aInfo);
	COpenedKey* OpenKeyL(TInt aHandle, TUid aOpenedKeyType);
	void ExportPublicL(TInt aObjectId, TDes8& aOut);
	TInt GetKeyLengthL(TInt aObjectId);
public:
	// For MCTKeyStoreManager
	void CreateKey(CKeyInfo& aReturnedKey, TRequestStatus& aStatus);
	void CancelCreateKey();
	void ImportKey(const TDesC8& aKey, CKeyInfo& aReturnedKey, TBool aIsEncrypted, TRequestStatus& aStatus);
	void CancelImportKey();
	void ExportKey(TInt aObjectId, const TPtr8& aKey, TRequestStatus& aStatus); 
	void CancelExportKey();
	void ExportEncryptedKey(TInt aObjectId, const TPtr8& aKey, CPBEncryptParms& aParams, TRequestStatus& aStatus);
	void CancelExportEncryptedKey();
	void DeleteKeyL(TInt aObjectId);
	void SetUsePolicyL(TInt aObjectId, const TSecurityPolicy& aPolicy);
	void SetManagementPolicyL(TInt aObjectId, const TSecurityPolicy& aPolicy);
	void CheckRangeL(TInt aFreshness);
	
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	void CreateUserKey(	CKeyInfo& aReturnedKey,
						const TDesC& aAuthString,
						TInt aFreshness,
						TRequestStatus& aStatus);

	void ImportUserKey(	const TDesC8& aKey, CKeyInfo& aReturnedKey, 
						TBool aIsEncrypted,
						const TDesC& aAuthString,
						TInt aFreshness,
						TRequestStatus& aStatus);
	
	void SetAuthenticationPolicy(	
								TInt aObjectId, 
								HBufC* aAuthString,
								TInt aFreshness, 
								TRequestStatus& aStatus);

	HBufC* AuthExpressionL( TInt aObjectId);
	TInt FreshnessL(TInt aObjectId);
	CKeyInfo* KeyDetailsLC(TInt aObjectId);

#else
	// For MCTAuthenticationObject
	void ChangePassphrase(TRequestStatus& aStatus);
	void CancelChangePassphrase();
	void AuthOpen(TRequestStatus& aStatus);
	void CancelAuthOpen();
	void AuthClose();
	void Relock();
	TInt GetTimeRemainingL();
	void SetTimeoutL(TInt aTimeout);
	TInt GetTimeout();
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
private:
	CFSKeyStoreServer();
	void ConstructL();

private:
	//	From CActive
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	
private:
	CFSKeyStoreServer(const CFSKeyStoreServer&);			//	No copying
	CFSKeyStoreServer& operator=(const CFSKeyStoreServer&);	//	No copying
private:
	CPassphraseManager* CreatePassphraseManagerLC();
	TInt CheckKeyAttributes(CKeyInfo& aKey, TNewKeyOperation aOp);
	TInt CheckKeyAlgorithmAndSize(CKeyInfo& aKey);
	void DoCreateKeyL();
	void DoStoreKeyL();
	void DoImportKeyL();
	void DoExportKeyL();
	void CompleteKeyExportL(TBool encrypted=EFalse);
	TInt CheckCreateKeyPolicy();
	void Cleanup();
	TInt CheckImportKeyPolicy(TNewKeyOperation aKeyOperation);
	void CheckExportKeyPolicyL();
	void CompleteClientRequest(TInt aCompletionCode);
		
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	template <class T> inline void EncryptAndStoreL(const T& aKey, RStoreWriteStream& aStream );
	void StoreKeyL(const TDesC8& aKeyData, RStoreWriteStream& aStream);
	
	AuthServer::CIdentity* SyncAuthenticateLC(const TDesC& aAuthExpression, TInt aFreshness);
	void AsyncAuthenticateL(const TDesC& aAuthString,
							TInt aFreshness);
	void SetDefaultAuthPolicyL();
	void WriteAuthenticationPolicyL();
#else	
	void GetKeystorePassphrase(TCurrentAction aNextState);
	void OpenPrivateStream();
	void DoChangePassphrase();
	void RemoveCachedPassphrases(TStreamId aStreamId);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
private:
	void PKCS8ToKeyL(CDecPKCS8Data* aPKCS8Data);
	TBool KeyMatchesFilterL(const CKeyInfo& aInfo,
							const TCTKeyAttributeFilter& aFilter);
	
	
private:
	CFileKeyDataManager* iKeyDataManager;
	CKeyStoreConduit* iConduit;
	RPointerArray<CKeyStoreSession> iSessions;
	const RMessage2* iMessage;		///< The request currently being processed, not owned by us
	CKeyStoreSession* iSession;    	///< The session of the request being processed, not owned through here
	TRequestStatus* iCallerRequest;
	TCurrentAction iAction;
	TCurrentAction iNextAction;
private:
	CKeyInfo* iKeyInfo;
	CKeyCreator* iKeyCreator;
	
	CPBEncryptParms* iPbeParams;    // Not owned by this object
	
	TPtr8 iExportBuf;
	TBool iExportingKeyEncrypted;

	TInt iObjectId;
	TPtrC8 iPKCS8Data;
	TBool iImportingEncryptedKey;
	TPINValue iPassword;
	const CFileKeyData* iKeyData;
	CPassphrase* iPassphrase;
	
#ifdef 	SYMBIAN_KEYSTORE_USE_AUTH_SERVER

private:
	AuthServer::RAuthClient iAuthClient;
	AuthServer::CIdentity* iUserIdentity;
	AuthServer::TIdentityId iIdentityId;
	CKeyStoreCenrep* iKeyStoreCenrep;
	HBufC* iAuthString;
	TInt iFreshness;
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
};

#endif	//	__CFSKEYSTORESERVER_H__
