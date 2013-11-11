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


#ifndef __KEYTOOL_COMMANDS_H_
#define __KEYTOOL_COMMANDS_H_

#include <e32base.h>
#include <ct/rmpointerarray.h>
#include <mctkeystore.h>
#include <unifiedkeystore.h>
#include <badesca.h>

#ifdef KEYTOOL
#include <authserver/authtypes.h>
#include <authserver/identity.h>
#include <authserver/authclient.h>
#endif // KEYTOOL

class CController;
class CKeyToolController;
#ifdef KEYTOOL
class CKeyInfo;
class CFileKeyData;
class CPermanentFileStore;
class CKeyStoreCenrep;
class CPassphrase;
#endif // KEYTOOL

/** 
 * This class wraps up the command line parameters
 * which affect the keystore command to be executed.
 * E.g. the key label we refer to, the key store to use, etc.
 */
class CKeyToolParameters : public CBase
	{
	public:
		static CKeyToolParameters* NewLC();
		~CKeyToolParameters();
#ifdef KEYTOOL
		enum TErrors
			{
			EMandatoryArgumentMissing=1,
			};
#endif // KEYTOOL
	enum TSetPolicy
		{
		ENone =0,
		ESetUserPolicy,
		ESetManagerPolicy,
		ERemoveUserPolicy,
		ESetAllUsersPolicy,
		};	
	protected:
		void ConstructL();

	private:
		CKeyToolParameters();

	public:
		TBool						iIsDetailed;
		HBufC* 						iPrivate;
		HBufC* 						iLabel;
		HBufC* 						iDefault;
		TInt 						iKeystoreIndex;
		TInt 						iCertstoreIndex;
		TKeyUsagePKCS15	 			iUsage;
		CKeyInfoBase::EKeyAccess	iAccess;
		TTime						iEndDate;
		RArray<TUid>				iUIDs;
		HBufC* 						iOwnerType;
		TBool 						iPageWise;
		TBool						iRemoveKey;
		TSetPolicy					iPolicy;
		HBufC*                      iIsDeletable;
#ifdef KEYTOOL
		HBufC*						iOldKeyFile;
		HBufC*						iNewKeyFile;
		HBufC* 						iAuthExpression;
		TInt						iFreshness;
		HBufC*						iPassphrase;
#endif // KEYTOOL
	};


/**
 * Interface that every keytool command class must implement.
 */
class CKeyToolCommand : public CActive
	{
	public:
		virtual void DoCommandL(CUnifiedKeyStore& aKeyStore, CKeyToolParameters* aParam) = 0;
		
	protected:
		CKeyToolCommand(CController* aController);

	protected:
		CController* iController;
		CKeyToolParameters* iParams; // we do not own this!		
	};
	
	
/**
 * Implements the keytool list action. This is an active object as listing is asynchronous.
 */
class CKeytoolUsage : public CKeyToolCommand
	{
public:
	static CKeytoolUsage* NewLC(CKeyToolController* aController);
	static CKeytoolUsage* NewL(CKeyToolController* aController);
	~CKeytoolUsage();
	void ConstructL();

public: // From CKeyToolCommand
	void DoCommandL(CUnifiedKeyStore& aKeyStore, CKeyToolParameters* aParam);
	
public: // From CActive
	void RunL();
	void DoCancel();
	
protected:
	CKeytoolUsage(CKeyToolController* aController);			
	};
	

/**
 * Implements the keytool list action. This is an active object as listing is asynchronous.
 */
class CKeytoolList : public CKeyToolCommand
	{
public:
	static CKeytoolList* NewLC(CKeyToolController* aController);
	static CKeytoolList* NewL(CKeyToolController* aController);
	~CKeytoolList();
	void ConstructL();

public: // From CKeyToolCommand
	void DoCommandL(CUnifiedKeyStore& aKeyStore, CKeyToolParameters* aParam);
	
public: // From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);	
	
protected:
	CKeytoolList(CKeyToolController* aController);

protected:
	RMPointerArray<CCTKeyInfo> iKeys;
	CUnifiedKeyStore* iKeyStore;
	TCTKeyAttributeFilter iFilter;
	MKeyStore* iKeyStoreImp;
	
	enum TState
		{
		EListKeys,
		EFinished,
		EIntermediate
		} iState;			
	};
	



/**
 * Implements the keytool import action.
 */
class CKeytoolImport : public CKeyToolCommand
	{
public:
	static CKeytoolImport* NewLC(CController* aController);
	static CKeytoolImport* NewL(CController* aController);
	~CKeytoolImport();
	void ConstructL();

public: // From CKeyToolCommand
	void DoCommandL(CUnifiedKeyStore& aKeyStore, CKeyToolParameters* aParam);
	
public: // From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
protected:	
	/**
	 * Given some (supposed) pkcs8 data, tells you whether we are dealing with 
	 * a PrivateKeyInfo or EncryptedPrivateKeyInfo ASN1 type.
	 */
	TBool IsPkcs8EncryptedL(TDesC8& aBinaryData);
	
private:
	CKeytoolImport(CController* aController);
	void Pkcs8PreprocessorL();
	
private:
	HBufC8* iKeyData;
	CCTKeyInfo* iKeyInfo;	
	};




/**
 * Implements the keytool remove action. 
 */
class CKeytoolRemove : public CKeytoolList
	{
public:
	static CKeytoolRemove* NewLC(CKeyToolController* aController);
	static CKeytoolRemove* NewL(CKeyToolController* aController);
	~CKeytoolRemove();
	
public: // From CActive
	void RunL();
	
protected:

private:
	CKeytoolRemove(CKeyToolController* aController);

private:
	RArray<CCTKeyInfo*> iKeyList;
	TInt idx;
	};



/**
 */
class CKeyToolListStores : public CKeyToolCommand
	{
public:
	static CKeyToolListStores* NewLC(CKeyToolController* aController);
	static CKeyToolListStores* NewL(CKeyToolController* aController);
	~CKeyToolListStores();
	void ConstructL();

public: // From CCertToolCommand
	void DoCommandL(CUnifiedKeyStore& aKeyStore, CKeyToolParameters* aParam);
	
public: // From CActive
	void RunL();
	void DoCancel();	
	
protected:
	CKeyToolListStores(CKeyToolController* aController);			
	
private:
	RArray<MCTKeyStore> iCertStoreList; 
	};


/**
 * Implements the keytool set policy action. 
 */
class CKeytoolSetPolicy : public CKeytoolList
	{
public:
	static CKeytoolSetPolicy* NewLC(CKeyToolController* aController);
	static CKeytoolSetPolicy* NewL(CKeyToolController* aController);
	~CKeytoolSetPolicy();
	
public: // From CActive
	void RunL();
	TInt RunError(TInt aError);
protected:

private:
	CKeytoolSetPolicy(CKeyToolController* aController);

private:
	CCTKeyInfo* iKey;
	TInt idx;
	};


#ifdef KEYTOOL

/**
 * Implements the migration of old keystore to the new keystore format 
 * which uses the authentication server. 
 */
class CKeytoolMigrateStore : public CActive
	{
	
private:
	enum TMigrateStoreState
		{
		EAfterAuthentication,
		
		};
public:
	static CKeytoolMigrateStore* NewLC(CKeyToolParameters* aParams);
	static CKeytoolMigrateStore* NewL(CKeyToolParameters* aParams);
	~CKeytoolMigrateStore();
	void ConstructL(CKeyToolParameters* aParams);

public: // From CKeyToolCommand
	void DoCommandL();

public: // From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);	
	
private:
	CKeytoolMigrateStore();
	void WriteKeyL(	const CKeyInfo& aKeyInfo, 
					RStoreWriteStream& aWriteInfoStream );

	void WriteAuthDetailsL( RStoreWriteStream& aWriteInfoStream );
	void RevertStore(TAny* aStore);
	void InitializeDefaultParams();
	void WriteKeyInformationL();
	template <class T> inline void EncryptAndStoreL(const T& aKey, RStoreWriteStream& aStream );
	void StoreKeyL(const TDesC8& aKeyData, RStoreWriteStream& aStream);
	template <class T> void RetrieveAndStorePublicKeyL( CFileKeyData* aKeyData, T* aPublicKey );
	template <class T> void RetrieveAndStorePrivateKeyL( CFileKeyData* aKeyData, T* aPrivateKey );
	static void CleanKeyInfo(TAny* aKeyInfo);
	
private:
	AuthServer::TIdentityId iIdentityId;
	AuthServer::RAuthClient iAuthClient; 
	CPermanentFileStore* iWriteFileStore;
	CPermanentFileStore* iReadFileStore;
	TStreamId iWriteLookupStreamId;
	CKeyToolParameters* iParams;
	RFs iFs;
	CKeyStoreCenrep* iKeyStoreCenrep;
	CPassphrase* iPassphrase;
	AuthServer::CIdentity* iUserIdentity;
	TMigrateStoreState iState;
	
	RPointerArray<CFileKeyData> iKeyList;
	};

#endif // KEYTOOL
#endif

