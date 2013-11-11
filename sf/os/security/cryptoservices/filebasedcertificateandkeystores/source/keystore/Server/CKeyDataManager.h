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
* Implements CFileKeyData and CFileKeyDataManager 
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CKEYDATAMANAGER_H__
#define __CKEYDATAMANAGER_H__

#include <s32file.h>
#include <ct/tcttokenobjecthandle.h>
#include <ct.h>

class CKeyInfo;
class CPassphraseManager;
class CPassphrase;

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
#include <authserver/authtypes.h>
#include <e32property.h>
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

#ifdef KEYTOOL
#include <authserver/authtypes.h>
#include <e32property.h>
#endif // KEYTOOL

/**
 * Server side in-memory representation of a key for file key store management
 * purposes. Objects of this type are stored in an in-memory array for quick
 * keystore lookup.
 */
class CFileKeyData : public CBase
	{
public:
	/// Create a new key data object
	static CFileKeyData* NewLC(TInt aObjectId, const TDesC& aLabel, TStreamId aInfoData,
							   TStreamId aPassphraseId, TStreamId aPublicData, TStreamId aPrivateData);
    /// Read a key data object from a stream
	static CFileKeyData* NewL(RStoreReadStream& aReadStream);
	
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	static CFileKeyData* NewLC(	TInt aObjectId, const TDesC& aLabel, 
								TStreamId aInfoData, TStreamId aPublicData, 
								TStreamId aPrivateData, AuthServer::TIdentityId aIdentityId);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
#ifdef KEYTOOL
	static CFileKeyData* CreateOldKeyL(RStoreReadStream& aReadStream);
#endif // KEYTOOL
public:
	~CFileKeyData();	
public:
	/// Write out the key data to a stream
	void ExternalizeL(RWriteStream&) const;
#ifdef KEYTOOL
	void ExternalizeWithAuthL(RWriteStream& aWriteStream) ;
#endif // KEYTOOL
public:
	inline const TDesC& Label() const;
	inline TInt32 Handle() const;
	inline TStreamId InfoDataStreamId() const;
	inline TStreamId PassphraseStreamId() const;
	inline TStreamId PublicDataStreamId() const;
	inline TStreamId PrivateDataStreamId() const;
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	inline AuthServer::TIdentityId Identity() const;
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
#ifdef KEYTOOL
	inline void SetInfoDataStreamId( TStreamId& aInfoDataStreamId );
	inline void SetPublicDataStreamId( TStreamId& aPublicDataStreamId );
	inline void SetPrivateDataStreamId( TStreamId& aPrivateDataStreamId );
	inline AuthServer::TIdentityId Identity() const;
#endif // KEYTOOL
private:
	CFileKeyData(TInt aObjectId, TStreamId aInfoData, TStreamId aPassphraseId,
				 TStreamId aPublicData, TStreamId aPrivateData);

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	CFileKeyData(	TInt aObjectId, TStreamId aInfoData,  
					TStreamId aPublicData, TStreamId aPrivateData, 
					AuthServer::TIdentityId aIdentityId);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

#ifdef KEYTOOL
	CFileKeyData(	TInt aObjectId, TStreamId aInfoData,  
					TStreamId aPublicData, TStreamId aPrivateData, 
					AuthServer::TIdentityId aIdentityId);
#endif // KEYTOOL

	CFileKeyData();
	void ConstructL(const TDesC& aLabel);
	void InternalizeL(RReadStream&);
#ifdef KEYTOOL
	void InternalizeOldKeyL(RReadStream& aReadStream);
#endif // KEYTOOL
	
private:
	TInt iObjectId;				///< Data to identify the key
	TStreamId iInfoData;		///< ID of stream holding publicly available data for key 
	TStreamId iPassphraseId;    ///< ID of stream holding passphrase data (not currently used)
	TStreamId iPublicKeyData;	///< ID of stream holding public key data
	TStreamId iPrivateKeyData;	///< ID of stream holding private key data
	HBufC* iLabel;				///< Key label data
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	AuthServer::TIdentityId iIdentityId;
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
#ifdef KEYTOOL
	AuthServer::TIdentityId iIdentityId;
#endif // KEYTOOL
};

inline const TDesC& CFileKeyData::Label() const
	{
	return *iLabel;
	}

inline TInt32 CFileKeyData::Handle() const
	{
	return iObjectId;
	}

inline TStreamId CFileKeyData::InfoDataStreamId() const
	{
	return iInfoData;
	}

inline TStreamId CFileKeyData::PassphraseStreamId() const
	{
	return iPassphraseId;
	}

inline TStreamId CFileKeyData::PublicDataStreamId() const
	{
	return iPublicKeyData;
	}

inline TStreamId CFileKeyData::PrivateDataStreamId() const
	{
	return iPrivateKeyData;
	}

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
inline AuthServer::TIdentityId CFileKeyData::Identity() const
	{
	return iIdentityId;
	}
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

#ifdef KEYTOOL
	inline void CFileKeyData::SetInfoDataStreamId( TStreamId& aInfoDataStreamId )
		{
		iInfoData = aInfoDataStreamId;
		}
	
	inline void CFileKeyData::SetPublicDataStreamId( TStreamId& aPublicDataStreamId )
		{
		iPublicKeyData = aPublicDataStreamId;
		}
	
	inline void CFileKeyData::SetPrivateDataStreamId( TStreamId& aPrivateDataStreamId )
		{
		iPrivateKeyData = aPrivateDataStreamId;
		}
	
#endif // KEYTOOL
/**
 * Access the server file store of all keys and key data. The only class to
 * access the store, which maintains store integrity When a new key is created,
 * it is represented by a CFileKeyData object and added to the array.  
 */
class CFileKeyDataManager : public CBase
{
public:
	static CFileKeyDataManager* NewL();
	~CFileKeyDataManager();
public:	
	CPassphraseManager* CreatePassphraseManagerLC();
	void AddL(const CFileKeyData*);
	void RemoveL(TInt aObjectId);
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	TBool IsKeyAlreadyInStore(const TDesC& aKeyLabel, AuthServer::TIdentityId aIdentity) const;
#else
	TBool IsKeyAlreadyInStore(const TDesC& aKeyLabel) const;
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
public:
	/// Get the id of the default passphrase, or KNullStreamId if it doesn't exist yet.
	TStreamId DefaultPassphraseId() const;
	/// Create a new key data object for a key create/import and leave it one the cleanup stack
	const CFileKeyData* CreateKeyDataLC(const TDesC& aLabel, TStreamId aPassphrase);
	
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	const CFileKeyData* CreateKeyDataLC(const TDesC& aLabel, AuthServer::TIdentityId aIdentity);
	TUint32 CachedIdentity();
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	///	Reads the info data for a given key, returning a new CKeyInfo that's on the cleanup stack
	CKeyInfo* ReadKeyInfoLC(const CFileKeyData& aKeyData) const;
	
	/// Writes key info data for a key
	void WriteKeyInfoL(const CFileKeyData& aKeyData, const CKeyInfo& aKeyInfo);
	/// Writes key info data and reverts changes to the store if if leaves
	void SafeWriteKeyInfoL(const CFileKeyData& aKeyData, const CKeyInfo& aKeyInfo);
	// Methods for opening data streams for a key
	void OpenPublicDataStreamLC(const CFileKeyData& aKeyData, RStoreWriteStream& aStream);
	void OpenPublicDataStreamLC(const CFileKeyData& aKeyData, RStoreReadStream& aStream) const;
	void OpenPrivateDataStreamLC(const CFileKeyData& aKeyData, CPassphrase& aPassphrase, RStoreReadStream& aStream);	
	
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	void OpenPrivateDataStreamLC(const CFileKeyData& aKeyData, RStoreWriteStream& aStream);
	void OpenPrivateDataStreamLC(const CFileKeyData& aKeyData, RStoreReadStream& aStream) const;
#else
	void OpenPrivateDataStreamLC(const CFileKeyData& aKeyData, CPassphrase& aPassphrase, RStoreWriteStream& aStream);

#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	/*
	 * not currently implemented, but might be someday
	/// Create a new passphrase by prompting the user and pass back its id.
	void CreatePassphrase(CPassphraseManager& aPassMan, TStreamId& aIdOut, TRequestStatus& aStatus);
	/// Remove an existing passphrase.  Leaves if the passphrase is used by any key.
	void RemovePassphraseL(TStreamId aId);
	 */
	
public:
	TInt Count() const;
	const CFileKeyData* operator[](TInt aIndex) const;
	const CFileKeyData* Lookup(TInt aObjectId) const;
public:
	/**
	 * Get the passphrase timeout.  A timeout of zero indicates
	 * that passphrases are never cached.  A timeout of -1 means cache until an
	 * explicit close operation occurs.
	 */
	TInt GetPassphraseTimeout() const;
	void SetPassphraseTimeoutL(TInt aTimeout);
private:
	CFileKeyDataManager();
	void ConstructL();
private:											//	Manages access to store
	void OpenStoreL();
	void OpenStoreInFileL(const TDesC& aFile);
	void CreateStoreInFileL(const TDesC& aFile);
private:
	void OpenInfoDataStreamLC(const CFileKeyData& aKeyData, RStoreWriteStream&);
	static void RevertStore(TAny* aStore);			//	Cleanupitem
	void WriteKeysToStoreL();
	void UpdateStoreL();
	TStreamId CreateWriteStreamL();
	void ReadPassphraseTimeoutL();
	void WritePassphraseTimeoutL();
	void CompactStore();
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	const HBufC8* DecryptKey(const TDesC8& aEncryptedKey);
	void WriteAuthDetailsL( RStoreWriteStream& aInfoStream, const CKeyInfo& aKeyInfo );
	void ReadAuthDetailsL( RStoreReadStream& aInfoStream, CKeyInfo& aKeyInfo ) const;
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
private:
	RFile iFile;
	RFs iFs;
	CPermanentFileStore* iFileStore;
	TStreamId iRootStreamId;	 		///< Root of the store
	TStreamId iInfoStreamId;	 		///< Stream that contains list of key data
	
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	RProperty iIdentityId;
#else
	TStreamId iPassStreamId;	 		///< Stream for the default passphrase
	TStreamId iTimeoutStreamId;  		///< Stream for timeout data
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
private:
	TInt iKeyIdentifier;
	RPointerArray<const CFileKeyData> iKeys;	///< In memory representation of keys in the store
	TInt iTimeout;						///< The passphrase timeout
	
};

#endif
