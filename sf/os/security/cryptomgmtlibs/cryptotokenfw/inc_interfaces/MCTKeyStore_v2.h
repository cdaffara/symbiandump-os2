/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* MCTKeystore.h
*
*/


/**
 @file 
 @publishedPartner
 @released
*/
 
#ifndef __MCTKEYSTORE_H__
#define __MCTKEYSTORE_H__

#include <securitydefs.h>
#include <mkeystore.h>
#include <s32file.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mctkeystoreuids.h>
#endif

/** The UID of the filekey store */
const TInt KTokenTypeFileKeystore = 0x101F7333;

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS

/** The type ID of CCTKeyInfo objects */
const TUid KKeyInfoUID = {0x101F5152};
const TUid KNonRepudiationSignerUID = {0x101F7A40};
const TUid KKeyStoreAuthObjectUID = {0x101FE681};

#endif

class MCTAuthenticationObject;

/**
 * Defines the interface for a read-only key store token.
 *
 * All the details are defined in MKeyStore as they are shared by the unified
 * key store.
 * 
 * The class adds no extra member functions or data. 
 * 
 */
class MCTKeyStore : public MCTTokenInterface, public MKeyStore
	{
	};

/**
 * Base class for CCTKeyInfo.
 *
 */
class CKeyInfoBase : protected CBase
	{
public:

	/** Key algorithms. */
	enum EKeyAlgorithm
		{
		EInvalidAlgorithm	= 0,
		ERSA				= 1,
		EDSA				= 2,
		EDH					= 3,
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		EECC                = 4,
#endif
		};

	/** Flags for key access bitfield. */
	enum EKeyAccess
		{
		EInvalidAccess		= 0x00,
		ESensitive			= 0x01,
		EExtractable		= 0x02,
		EAlwaysSensitive	= 0x04,
		ENeverExtractable	= 0x08,
		ELocal				= 0x10
		};

public:
	
	inline TKeyIdentifier ID() const;						///< The ID (SHA-1 hash) of the key 
	inline TKeyUsagePKCS15 Usage() const;					///< The key usage 
	inline TUint Size() const;								///< The size of the key 
	inline const TDesC& Label() const;						///< The key's label.
	inline const TSecurityPolicy& UsePolicy() const; 		///< The security policy for key use 
	inline const TSecurityPolicy& ManagementPolicy() const; ///< The security policy for key management 
	inline EKeyAlgorithm Algorithm() const;					///< The key algorithm

	/**
	 * The key access type. The return code is bitfield made up of 0 or more
	 * values from EKeyAccess ORed together.
	 */
	inline TInt AccessType() const;

	/**
	 * Returns whether the key is native.
	 *
	 * A native key is one where operations on the key are performed on the same
	 * hardware as the the key is stored. For instance, if a key that is stored
	 * on secure hardware but calculations are carried out on the main
	 * processor, it isn't native.
	 */
	inline TBool Native() const;
	
	inline TTime StartDate() const;					///< The start time, or TTime(0) if not set
	inline TTime EndDate() const;					///< The end time, or TTime(0) if not set
	inline const TDesC8& PKCS8AttributeSet() const;	///< The PKCS#8 attribute set.

	/**
	 * Externalizes the key data to stream
	 */
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

public:

	/**
	 * Get the handle of the key.  This is a identifier that uniquely identifies
	 * the key within one store, and is used to construct the token handle.
	 */
	inline TInt HandleID() const;

	/**
	 * Set the handle of the key.  Called by the token when a key is created.
	 *
	 * @param aHandle The new handle of the key
	 */
	inline void SetHandle(TInt aHandle);

	/**
	 * Called by the token when a key is created, to set the key identifier
	 * (SHA-1 hash) of the new key
	 *
	 * @param aId The newly generated SHA-1 hash
	 */
	inline void SetIdentifier(TKeyIdentifier aId);

	/**
	 * Set the size of a key.
	 */
	inline void SetSize(TUint aSize);

	/**
	 * Set the key algorithm.
	 */
	inline void SetAlgorithm(EKeyAlgorithm aAlg);

protected:

	/**
	 * Protected constructor, called by derived classes.
	 */
	IMPORT_C CKeyInfoBase(TKeyIdentifier aID,
						  TKeyUsagePKCS15 aUsage,
						  TUint aSize,
						  HBufC* aLabel,
						  TInt aHandle,
						  const TSecurityPolicy& aUsePolicy,
						  const TSecurityPolicy& aManagementPolicy,
						  EKeyAlgorithm aAlgorithm,
						  TInt aAccessType,
						  TBool aNative,
						  TTime aStartDate,
						  TTime aEndDate,
						  HBufC8* aPKCS8AttributeSet);

	/**
	 * Protected constructor, called by derived classes.
	 */
	IMPORT_C CKeyInfoBase();

	/**
	 * Second phase constructor.  Called by derived classes' NewL methods.
	 */
	IMPORT_C void ConstructL();

	/**
	 * Second phase constructor.  Called by derived classes' NewL methods.
	 */
	IMPORT_C void ConstructL(RReadStream& aIn);

	/**
	 * Destructor is protected so CCTKeyInfo can force users to call Release.
	 */
	IMPORT_C ~CKeyInfoBase();

private:

	/**
	 * Internalizes key data from a stream.
	 */
	void InternalizeL(RReadStream& aStream);
	
protected:
	TKeyIdentifier iID;					///< The ID of the key
	TKeyUsagePKCS15 iUsage;				///< The usage of the key 
	TUint iSize;						///< The size of the key 
	HBufC* iLabel;						///< The identifying label of the key
	TInt iHandle;						///< The handle of the key
	TSecurityPolicy iUsePolicy;			///< The secutity policy for key use
	TSecurityPolicy iManagementPolicy;	///< The secutity policy for key management
	EKeyAlgorithm iAlgorithm;			///< The key type (RSA, DSA etc)
	TInt iAccessType;					///< Key sensitivity
	TBool iNative;						///< Key is native (cryptographic operations are carried out on the store)
	TTime iStartDate;					///< Key Start Date 
	TTime iEndDate;						///< Key end date 
	HBufC8* iPKCS8AttributeSet;			///< Attributes as a DER-encoded set
	};

/**
 * Information about a key, as returned by MCTKeyStore::List.
 *
 */
class CCTKeyInfo : public CKeyInfoBase, public MCTTokenObject
	{
public:

	/**
	 * 
	 * Creates a CCTKeyInfo from constituents.  This is called by the unified
	 * key store, and should not be called directly.
	 *
	 * @param aID The SHA1 hash of the key
	 * @param aUsage The usage of the key
	 * @param aSize The size of the key in bytes
	 * @param aProtector A protector object if the key is protected by a PIN.
	 * This may be NULL if the protector is not known.
	 * @param aLabel The label of the key (takes ownership).
	 * @param aToken The token the key is in
	 * @param aHandle The object ID part of the object handle; an
	 * integer that is unique amongst keys in this token.
	 * @param aUsePolicy The security policy for key use
	 * @param aManagementPolicy The security policy for key management
	 * @param aAlgorithm The key algorithm (RSA, DSA or Diffie-Hellman)
	 * @param aAccessType The access type of the key
	 * @param aNative	Defines whether the key is native
	 * @param aStartDate The key validity start date
	 * @param aEndDate The key validity end date
	 * @param aPKCS8AttributeSet (optional) The DER encoded PKCS8 attribute set
	 * (takes ownership).
	 *
	 * @leave KErrKeyUsage If the key usage flags are not valid or not
	 * consistent with the key algorithm.
	 * @leave KErrKeyValidity If the validity start and end dates are specified
	 * but do not form a valid time period.
	 */
	IMPORT_C static CCTKeyInfo* NewL(TKeyIdentifier aID,
									 TKeyUsagePKCS15 aUsage,
									 TUint aSize,
									 MCTAuthenticationObject* aProtector,
									 HBufC* aLabel,
									 MCTToken& aToken,
									 TInt aHandle,
									 const TSecurityPolicy& aUsePolicy,
									 const TSecurityPolicy& aManagementPolicy,
									 EKeyAlgorithm aAlgorithm,
									 TInt aAccessType,
									 TBool aNative,
									 TTime aStartDate,
									 TTime aEndDate,
									 HBufC8* aPKCS8AttributeSet = NULL);

	/**
	 * Creates a new KeyInfo from a stream.
	 *
	 * @param aStream The stream to read the key data from
	 * @param aToken The token that the key is in
	 *
	 * @leave KErrKeyUsage If the key usage flags are not valid or not
	 * consistent with the key algorithm.
	 * @leave KErrKeyValidity If the validity start and end dates are specified
	 * but do not form a valid time period.
	 */
	IMPORT_C static CCTKeyInfo* NewL(RReadStream& aStream, MCTToken& aToken);

public:

	/**
	 * The PIN (or other authentication object) that protects the key, or NULL
	 * if not set.  This object is owned by key store.
	 */
	inline MCTAuthenticationObject* Protector() const;

	/**
	 * Sets the authentication object for this key.  The object's Release method
	 * will be called by the destructor, allowing for refence counting of auth
	 * objects to be implemented if desired.
	 */
	inline void SetProtector(MCTAuthenticationObject& aProtector);
	
	/** The CT handle to this object. */
	inline operator TCTTokenObjectHandle() const;

public:

	// from MCTTokenObject

	/** The label of the key */
 	virtual const TDesC& Label() const;

	/** The token the key is in */
	virtual MCTToken& Token() const;

	/** Returns KKeyInfoUID to indicate this is a key info object */
	virtual TUid Type() const;

	/**
	 * A handle for the key. This can be used to identify a key to
	 * another process.
	 */
	virtual TCTTokenObjectHandle Handle() const;

private:

	CCTKeyInfo(TKeyIdentifier aID,
			   TKeyUsagePKCS15 aUsage,
			   TUint aSize,
			   MCTAuthenticationObject* aProtector,
			   HBufC* aLabel,
			   MCTToken& aToken,
			   TInt aHandle,
			   const TSecurityPolicy& aUsePolicy,
			   const TSecurityPolicy& aManagementPolicy,
			   EKeyAlgorithm aAlgorithm,
			   TInt aAccessType,
			   TBool aNative,
			   TTime aStartDate,
			   TTime aEndDate,
			   HBufC8* aPKCS8AttributeSet);

	CCTKeyInfo(MCTToken& aToken);

	~CCTKeyInfo();

private:

	/** The token the key is in*/
	MCTToken& iToken;

	/** The protector object of the key. This pointer is not owned by the class */
	MCTAuthenticationObject* iProtector;
};

/**
 * A filter to specify which keys should be returned from the store by
 * MCTKeyStore::List.
 *
 */
struct TCTKeyAttributeFilter
	{
	enum TPolicyFilter
		{
		EAllKeys,
		EUsableKeys,
		EManageableKeys,
		EUsableOrManageableKeys
		};

	/** Constructor */
	IMPORT_C TCTKeyAttributeFilter();

	/**
	 * The hash of the key we're looking for. A zero-length descriptor means
	 * 'don't care
	 */
	TKeyIdentifier iKeyId;

	/**
	 * The required usage of the key. A key must match any of the usages
	 * specified. Use EAllUsages to return all usages.
	 */
	TKeyUsagePKCS15 iUsage;

	/**
	 * Filter returned keys by the operations the calling process is allowed to
	 * perform on them (as determined by the security policies set on them).
	 * 
	 * The default is EUsableKeys.  Note that if this is to set to KAllKeys, all
	 * keys will be returned, including those that are unusable by the calling
	 * process.
	 */
	TPolicyFilter iPolicyFilter;

	/** The algorithm. EInvalidAlgorithm indicates 'don't care */
	CCTKeyInfo::EKeyAlgorithm iKeyAlgorithm;
	};

#include "mctkeystore.inl"

#endif // __MCTKEYSTORE_H__
