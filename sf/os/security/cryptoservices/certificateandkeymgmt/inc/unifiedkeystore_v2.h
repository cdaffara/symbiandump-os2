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
* UNIFIEDKEYSTORE.H
* The unified key store implementation
*
*/


/**
 @file 
 @publishedPartner
 @released
*/
 
#ifndef __UNIFIEDKEYSTORE_H__
#define __UNIFIEDKEYSTORE_H__

#include <f32file.h>
#include <e32base.h>

#include <mctkeystoremanager.h>

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
namespace CryptoSpi
    {
    class CSigner;
    class CAsymmetricCipher;
    class CCryptoParams;
    }
#endif

/**
 * Unified KeyStore panics 
 *
 * @publishedPartner
 * @released
 */
enum TUnifiedKeyStorePanic
	{
	EUnexpectedInitialise		= 1,	///< Unexpected initialise
	EArrayAccessOutOfBounds		= 2,	///< Array access out of bounds
	ETokensArrayAlreadyInUse	= 3,	///< Tokens array already in use
	EUnrecognisedState			= 4,	///< Unrecognised state
	};

/**
 * The unified key store.
 * 
 * This class provides a key store whose contents are the sum of the contents of
 * all key store implementations on the device.  It is intended a single point
 * of access for clients wishing to access key stores.
 *
 * Since this class is intended for widespread use, capability checks relating
 * to key access are documented here even though the checks are actually made in
 * the individual key store implementations.
 * 
 * @publishedPartner
 * @released
 */
NONSHARABLE_CLASS(CUnifiedKeyStore) : public CActive, public MKeyStore
	{
 public:	
	/**
	 * Creates a new CUnifiedKeyStore object.
	 * 
	 * @param aFs	A file server session. It must already be open.
	 * @return		A pointer to an instance of the CUnifiedKeyStore class.
	 */
	IMPORT_C static CUnifiedKeyStore* NewL(RFs& aFs);
	
	/**
	 * Creates a new CUnifiedKeyStore object and and puts a pointer to the new object 
	 * onto the cleanup stack.
	 * 
	 * @param aFs	A file server session. It must already be open.
	 * @return		A pointer to an instance of the CUnifiedKeyStore class.
	 */
	IMPORT_C static CUnifiedKeyStore* NewLC(RFs& aFs);

	/**
	 * The destructor destroys all the resources owned by this object.
	 */
	IMPORT_C ~CUnifiedKeyStore();

	/**
	 * Initialises the manager.
	 * 
	 * It must be called after the manager has been constructed and before any call 
	 * to the manager functions.
	 * 
	 * This is an asynchronous request.
	 * 
	 * @param aStatus	The request status object; contains the result of the Initialize() 
	 * 					request when complete. Set to KErrCancel if any outstanding request is cancelled.
	 */
	IMPORT_C void Initialize(TRequestStatus& aStatus);

	/** 
	 * Cancels an ongoing Initialize() operation.
	 *
	 * The operation completes with KErrCancel.
	 */
	IMPORT_C void CancelInitialize();
	
public:  // Implementation of MKeyStore interface
	virtual void List(RMPointerArray<CCTKeyInfo>& aKeys, const TCTKeyAttributeFilter& aFilter, TRequestStatus& aStatus);
	virtual void CancelList();
	virtual void GetKeyInfo(TCTTokenObjectHandle aHandle, CCTKeyInfo*& aInfo,TRequestStatus& aStatus);
	virtual void CancelGetKeyInfo();
	virtual void Open(const TCTTokenObjectHandle& aHandle, 
					  MRSASigner*& aSigner,
					  TRequestStatus& aStatus);
	virtual void Open(const TCTTokenObjectHandle& aHandle, 
					  MDSASigner*& aSigner, 
					  TRequestStatus& aStatus);
	virtual void Open(const TCTTokenObjectHandle& aHandle, 
					  MCTDecryptor*& aDecryptor,
					  TRequestStatus& aStatus);
	virtual void Open(const TCTTokenObjectHandle& aHandle, 
					  MCTDH*& aDH, TRequestStatus& aStatus);
	virtual void CancelOpen();
	virtual void ExportPublic(const TCTTokenObjectHandle& aHandle,
							  HBufC8*& aPublicKey,
							  TRequestStatus& aStatus);
	virtual void CancelExportPublic();

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
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
#endif

public:		//	For MCTKeyStoreManager except those (CreateKey, ImportKey, ImportEncryptedKey)
			//	that require a caller-specified store
	
	/**
	 * Exports a key pair in the clear.
	 *
	 * The key is exported as DER-encoded PKCS#8 data.
	 *
	 * @param aHandle	The handle of the key to export
	 * @param aKey		A reference to a HBufC8 pointer.  The pointer will be set to
	 *					a newly allocated buffer containing the key data.  It is the caller's
	 *					responsibility to delete this buffer.
	 * @param aStatus	The request status object; contains the result of the ExportKey() request 
	 *					when complete. Set to KErrCancel if any outstanding request is cancelled.
	 *				
	 * @capability ReadUserData		Requires the caller to have ReadUserData capability
	 * @leave KErrPermissionDenied	If the caller does not have ReadUserData capability, 
	 *								or is not the owner of the key.
	 * @leave KErrNotFound			If the key the handle referes to does not exist.
	 * @leave KErrKeyAccess			If the sensitive flag is set for the key, or the
	 *								exportable flag is not set.
	 * @leave KErrKeyAlgorithm		If this type of key cannot be exported.
	 */
	IMPORT_C void ExportKey(TCTTokenObjectHandle aHandle, HBufC8*& aKey, TRequestStatus& aStatus);

	/** 
	 * Cancels an ongoing ExportKey() operation.
	 *
	 * The operation completes with KErrCancel.
	 */
	IMPORT_C void CancelExportKey();

	/**
	 * Exports an encrypted key pair.
	 *
	 * The key is exported as DER-encoded PKCS#5/PKCS#8 data.
	 *
	 * @param aHandle	The handle of the key to export
	 * @param aKey		A reference to a HBufC8 pointer.  The pointer will be set to
	 *					a newly allocated buffer containing the key data.
	 * @param aParams	The PBE encryption parameters to use when encrypting the key.
	 * @param aStatus	The request status object; contains the result of the ExportEncryptedKey() request 
	 *					when complete. Set to KErrCancel if any outstanding request is cancelled.
	 * 
	 * @capability ReadUserData		Requires the caller to have ReadUserData capability
	 * @leave KErrPermissionDenied	If the caller does not have ReadUserData capability,
	 *								or is not the owner of the key.
	 * @leave KErrNotFound			If the key the handle referes to does not exist. 
	 * @leave KErrKeyAccess			If the exportable flag is not set for the key.
	 * @leave KErrKeyAlgorithm		If this type of key cannot be exported.
	 */
	IMPORT_C void ExportEncryptedKey(TCTTokenObjectHandle aHandle, const CPBEncryptParms& aEncryptParams, 
									 HBufC8*& aKey, TRequestStatus& aStatus);

	/** 
	 * Cancels an ongoing ExportEncryptedKey() operation.
	 *
	 * The operation completes with KErrCancel.
	 */
	IMPORT_C void CancelExportEncryptedKey();

	/**
	 * Deletes a key.
	 * 
	 * @param aHandle	The handle of the key to delete
	 * @param aStatus	The request status object; contains the result of the DeleteKey() request 
	 *					when complete. Set to KErrCancel if any outstanding request is cancelled.
	 *	
	 * @capability WriteUserData	Requires the caller to have WriteUserData capability
	 * @leave KErrPermissionDenied	If the caller does not have WriteUserData capability,
	 *								or is not the owner of the key.
	 * @leave KErrNotFound			If the key the handle referes to does not exist. 
	 * @leave KErrAccessDenied		If the calling process is not allowed to delete the key.
	 * @leave KErrInUse				If another client is currently using the key.
	 */
	IMPORT_C void DeleteKey(TCTTokenObjectHandle aHandle, TRequestStatus& aStatus);

	/** 
	 * Cancels an ongoing DeleteKey() operation.
	 *
	 * The operation completes with KErrCancel.
	 */
	IMPORT_C void CancelDeleteKey();

	/**
	 * Sets the security policy for key use.
	 *
	 * Specifies which processes are allowed to use the key for cryptographic
	 * operations.
	 *
	 * @param aHandle	The handle of the key
	 * @param aPolicy	The new security policy.
	 * @param aStatus	The request status object; contains the result of the SetUsePolicy() request 
	 *					when complete. Set to KErrCancel if any outstanding request is cancelled.
	 *	
	 * @capability WriteUserData	Requires the caller to have WriteUserData capability
	 * @leave KErrPermissionDenied	If the caller does not have WriteUserData capability,
	 *								or is not the owner of the key.
	 * @leave KErrNotFound			If the key the handle referes to does not exist.
	 */
	IMPORT_C void SetUsePolicy(TCTTokenObjectHandle aHandle,
							  const TSecurityPolicy& aPolicy,
							  TRequestStatus& aStatus);

	/** 
	 * Cancels an ongoing SetUsePolicy() operation.
	 *
	 * The operation completes with KErrCancel.
	 */
	IMPORT_C void CancelSetUsePolicy();

	/**
	 * Sets the security policy for key management.
	 *
	 * Specifies which processes are allowed to perform management operations on
	 * the key.
	 *
	 * @param aHandle	The handle of the key
	 * @param aPolicy	The new security policy.
	 * @param aStatus	The request status object; contains the result of the SetManagementPolicy() request 
	 *					when complete. Set to KErrCancel if any outstanding request is cancelled.
	 *	
	 * @capability WriteUserData	Requires the caller to have WriteUserData capability
	 * @leave KErrPermissionDenied	If the caller does not have WriteUserData capability,
	 *								or is not the owner of the key.
	 * @leave KErrNotFound			If the key the handle referes to does not exist.
	 */
	IMPORT_C void SetManagementPolicy(TCTTokenObjectHandle aHandle,
									  const TSecurityPolicy& aPolicy,
									  TRequestStatus& aStatus);

	/** 
	 * Cancels an ongoing SetManagementPolicy() operation.
	 *
	 * The operation completes with KErrCancel.
	 */
	IMPORT_C void CancelSetManagementPolicy();

	/**
	 * Sets the passphrase timeout for all keys owned by this process.
	 * 
	 * @param aTimeout	The timeout in seconds. 0 means that the passphrase is
	 *     				always asked for, and -1 means that it is never expired
	 * @param aStatus	The request status object; contains the result of the SetPassphraseTimeout() request 
	 *					when complete. Set to KErrCancel if any outstanding request is cancelled.
	 *	
	 * @capability WriteUserData	Requires the caller to have WriteUserData capability
	 * @leave KErrPermissionDenied	If the caller does not have WriteUserData capability,
	 *								or is not the owner of the key
	 * @leave KErrArgument			If the timeout specified is invalid.
	 */
	IMPORT_C void SetPassphraseTimeout(TInt aTimeout, TRequestStatus& aStatus);

	/** 
	 * Cancels an ongoing SetPassphraseTimeout() operation. 
	 *
	 * The operation completes with KErrCancel.
	 */
	IMPORT_C void CancelSetPassphraseTimeout();

	/** 
	 * Re-locks the entire store (i.e., forget the passphrase).
	 *
	 * @param aStatus	The request status object; contains the result of the Relock() request 
	 *					when complete. Set to KErrCancel if any outstanding request is cancelled.
	 */
	IMPORT_C void Relock(TRequestStatus& aStatus);
	
	/** 
	 * Cancels an ongoing Relock() operation.
	 *
	 * The operation completes with KErrCancel.
	 */
	IMPORT_C void CancelRelock();
	
public:
	/**
	 * Generates a new key pair.
	 *
	 * For the software key store, the owner of the new key is set to the
	 * calling process.  Users can subsequently be added by calling SetUsers().
	 *	
	 * @param aKeyStoreIndex  The index of the key store manager in which to
	 *                        create the key.  Must be between zero and
	 *	                      KeyStoreMangerCount() exclusive.		
	 * @param aUsage		  The key usage flags in the PKCS#15 format.
	 * @param aSize		      The size of the key in bits.
	 * @param aLabel		  A textual label for the key.
	 * @param aAlgorithm	  The type of key.
	 * @param aAccessType     The key access type - a bitfield specifying key
	 *	                      access requirements.  Allowed values are zero, or
	 *	                      a comination of CCTKeyInfo::EKeyAccess::ESenstive
	 *	                      and CCTKeyInfo::EKeyAccess::EExtractable
	 * @param aStartDate	  The start of the validity period.
	 * @param aEndDate		  The end of the validity period.	
	 * @param aKeyInfoOut     A pointer that is set to a newly created key info
	 *	                      object on successful completion.
	 * @param aStatus		  The request status object; contains the result of
	 *						  the CreateKey() request when complete. Set to 
	 *						  KErrCancel if any outstanding request is cancelled.
	 *   
	 * @capability WriteUserData	Requires the caller to have WriteUserData capability
	 * @leave KErrPermissionDenied	If the caller does not have WriteUserData capability
	 * @leave KErrKeyUsage			If the key usage flags are not valid or not
	 *								consistent with the key algorithm.
	 * @leave KErrKeyValidity		If the validity start and end dates are specified
	 *								but do not form a valid time period.
	 * @panic						If aKeyStoreIndex does not specify a valid keystore manager.
	 */
	IMPORT_C void CreateKey(TInt aKeyStoreIndex, TKeyUsagePKCS15 aUsage,TUint aSize, 
							const TDesC& aLabel, CCTKeyInfo::EKeyAlgorithm aAlgorithm, 
							TInt aAccessType, TTime aStartDate, TTime aEndDate, 
							CCTKeyInfo*& aKeyInfoOut, TRequestStatus& aStatus);
	
	/** 
	 * Cancels an ongoing CreateKey() operation. 
	 *
	 * The operation completes with KErrCancel.
	 */
	IMPORT_C void CancelCreateKey();

	/**
	 * Imports a key pair.
	 *
	 * For the software key store, the owner of the new key is set to the
	 * calling process.  Users can subsequently be added by calling SetUsers().
	 *
	 * The key data should be in PKCS#8 format.  Both encrypted and cleartext
	 * versions are allowed.
	 *
	 * @param aKeyStoreIndex  The index of the key store manager in which to
	 *	                      create the key.  Must be between zero and
	 *	                      KeyStoreMangerCount() exclusive.			
	 * @param aKeyData		  The key data to import, ASN.1 DER encoded PKCS#8.
	 * @param aUsage		  The key usage flags in the PKCS#15 format.
	 * @param aLabel		  A textual label for the key.
	 * @param aAccessType     The key access type - a bitfield specifying key
	 *	                      access requirements.  Allowed values are zero, or
	 *	                      a comination of CCTKeyInfo::EKeyAccess::ESenstive
	 *	                      and CCTKeyInfo::EKeyAccess::EExtractable
	 * @param aStartDate	  The start of the validity period.
	 * @param aEndDate		  The end of the validity period.	
	 * @param aKeyInfoOut     A pointer that is set to a newly created key info
	 *	                      object on successful completion.
	 * @param aStatus		  The request status object; contains the result of
	 *						  the ImportKey() request when complete. Set to 
	 *						  KErrCancel if any outstanding request is cancelled.
	 *	                      
	 * @capability WriteUserData	Requires the caller to have WriteUserData capability
	 * @leave KErrPermissionDenied	If the caller does not have WriteUserData capability
	 * @leave KErrKeyUsage			If the key usage flags are not valid or not
	 *								consistent with the key algorithm.
	 * @leave KErrKeyValidity		If the validity start and end dates are specified
	 *								but do not form a valid time period.
	 * @leave KErrArgument			If the key data cannot be parsed.
	 * @panic						If aKeyStoreIndex does not specify a valid keystore manager.
	 */
	IMPORT_C void ImportKey(TInt aKeyStoreIndex, const TDesC8& aKeyData,
							TKeyUsagePKCS15 aUsage, const TDesC& aLabel, 
							TInt aAccessType, TTime aStartDate, TTime aEndDate, 
							CCTKeyInfo*& aKeyInfoOut, TRequestStatus& aStatus);
	
	/** 
	 * Cancels an ongoing ImportKey() operation. 
	 *
	 * The operation completes with KErrCancel.
	 */
	IMPORT_C void CancelImportKey();
	
public:
	
	/**
	 * Gets the number of available read-only key stores.
	 * 
	 * @return	The number of available read-only key stores.
	 */
	IMPORT_C TInt KeyStoreCount() const;
	
	/**
	 * Gets a read-only interface to a key store.
	 * 
	 * @param aIndex				An ordinal number that identifies the key store.
	 * @return						A read-only interface to the key store specified by aIndex.
	 * 
	 * @panic CUnifiedKeyStore 2    If aIndex is out of range, ie it is greater
	 * 								than or equal to the value returned by KeyStoreCount().
	 */
	IMPORT_C MCTKeyStore& KeyStore(TInt aIndex);
	
	/**
	 * Gets the number of available read-write key stores.
	 * 
	 * @return	The number of key stores that are open for read-write access.
	 */
	IMPORT_C TInt KeyStoreManagerCount() const;
	
	/**
	 * Gets a read-write interface to the store specified by aIndex.
	 * 
	 * @param aIndex				An ordinal number that identifies the key store.
	 * @return						A read-write interface to the key store specified by aIndex.
	 * 
	 * @panic CUnifiedKeyStore 2    If aIndex s out of range, ie it is greater than
	 * 								or equal to the value returned by KeyStoreManagerCount().
	 */
	IMPORT_C MCTKeyStoreManager& KeyStoreManager(TInt aIndex);

#ifdef SYMBIAN_AUTH_SERVER
public:
	/**
		 * Generates a new key pair. The creation of key is for currently authenticated 
		 * user. If currently there is no authenticated user then authentication of an user 
		 * would be required.
		 *
		 * For the software key store, the owner of the new key is set to the
		 * calling process.  Users can subsequently be added by calling SetUsers().
		 *	
		 * @param aKeyStoreIndex  			The index of the key store manager in which to
		 *                        			create the key.  Must be between zero and
		 *	                      			KeyStoreMangerCount() exclusive.		
		 * @param aUsage		  			The key usage flags in the PKCS#15 format.
		 * @param aSize		      			The size of the key in bits.
		 * @param aLabel		  			A textual label for the key.
		 * @param aAlgorithm	  			The type of key.
		 * @param aAccessType     			The key access type - a bitfield specifying key
		 *	                      			access requirements.  Allowed values are zero, or
		 *	                      			a comination of CCTKeyInfo::EKeyAccess::ESenstive
		 *	                      			and CCTKeyInfo::EKeyAccess::EExtractable
		 * @param aStartDate	  			The start of the validity period.
		 * @param aEndDate		  			The end of the validity period.	
		 * @param aAuthenticationString		The expression through which a user can be authenticated.
		 * 									Currently this should correspond to one of the alias values
		 * 									set by the licensee for authentication server configuration.
		 * @param aFreshness				The validity to be considered for an already authenticated 
		 * 									identity. Specification is in seconds.	
		 * @param aKeyInfoOut     			A pointer that is set to a newly created key info
		 *	                      			object on successful completion.
		 * @param aStatus		  			Final status of the operation. 
		 *   
		 * @capability WriteUserData		Requires the caller to have WriteUserData capability
		 * @leave KErrPermissionDenied		If the caller does not have WriteUserData capability
		 * @leave KErrKeyUsage				If the key usage flags are not valid or not
		 *									consistent with the key algorithm.
		 * @leave KErrKeyValidity			If the validity start and end dates are specified
		 *									but do not form a valid time period.
		 * @leave KErrAuthenticationFailure	If the user authentication fails.
		 * @leave ...						Any of the system wide error code.
		 * @panic							If aKeyStoreIndex does not specify a valid keystore manager.
	*/

	IMPORT_C void CreateKey(TInt aKeyStoreIndex, TKeyUsagePKCS15 aUsage,TUint aSize, 
							const TDesC& aLabel, CCTKeyInfo::EKeyAlgorithm aAlgorithm, 
							TInt aAccessType, TTime aStartDate, TTime aEndDate,
							const TDesC& aAuthenticationString, TInt aFreshness,
							CCTKeyInfo*& aKeyInfoOut, TRequestStatus& aStatus) ;

	/**
		 * Imports a key pair. The import of key is for currently authenticated 
		 * user. If currently there is no authenticated user then authentication 
		 * of an user would be required.
		 *
		 * For the software key store, the owner of the new key is set to the
		 * calling process.  Users can subsequently be added by calling SetUsers().
		 *
		 * The key data should be in PKCS#8 format.  Both encrypted and cleartext
		 * versions are allowed.
		 *
		 * @param aKeyStoreIndex  			The index of the key store manager in which to
		 *	                      			create the key.  Must be between zero and
		 *	                      			KeyStoreMangerCount() exclusive.			
		 * @param aKeyData		  			The key data to import, ASN.1 DER encoded PKCS#8.
		 * @param aUsage		  			The key usage flags in the PKCS#15 format.
		 * @param aLabel		  			A textual label for the key.
		 * @param aAccessType     			The key access type - a bitfield specifying key
		 *	                      			access requirements.  Allowed values are zero, or
		 *	                      			a comination of CCTKeyInfo::EKeyAccess::ESenstive
		 *	                      			and CCTKeyInfo::EKeyAccess::EExtractable
		 * @param aStartDate	  			The start of the validity period.
		 * @param aEndDate		  			The end of the validity period.
		 * @param aAuthenticationString		The expression through which a user can be authenticated.
		 * 									Currently this should correspond to one of the alias values
		 * 									set by the licensee for authentication server configuration.
		 * @param aFreshness				The validity to be considered for an already authenticated 
		 * 									identity. Specification is in seconds.	
		 * @param aKeyInfoOut     			A pointer that is set to a newly created key info
		 *	                      			object on successful completion.
		 * @param aStatus		  			Final status of the operation. 
		 *	                      
		 * @capability WriteUserData	Requires the caller to have WriteUserData capability
		 * @leave KErrPermissionDenied	If the caller does not have WriteUserData capability
		 * @leave KErrKeyUsage			If the key usage flags are not valid or not
		 *								consistent with the key algorithm.
		 * @leave KErrKeyValidity		If the validity start and end dates are specified
		 *								but do not form a valid time period.
		 * @leave KErrArgument			If the key data cannot be parsed.
		 * @panic						If aKeyStoreIndex does not specify a valid keystore manager.
	*/
	
	IMPORT_C void ImportKey(	TInt aKeyStoreIndex, const TDesC8& aKeyData,
								TKeyUsagePKCS15 aUsage, const TDesC& aLabel, 
								TInt aAccessType, TTime aStartDate, TTime aEndDate, 
								const TDesC& aAuthenticationString, TInt aFreshness,
								CCTKeyInfo*& aKeyInfoOut, TRequestStatus& aStatus);
	
	/**
	 *  Set the authentication policy for an already existing key in the store.
	 * 
	 * @param aHandle					The handle of the key whose policy is to be changed.
	 * @param aAuthenticationString		The expression associated to this key.
	 * @param aFreshness				The validity associated to this key.
	 * 									Specification is in seconds.
	 * @param aStatus					Final status of the operation.  
	*/
	
	IMPORT_C void SetAuthenticationPolicy(	const TCTTokenObjectHandle aHandle, 
											const TDesC& aAuthenticationString,
											TInt aFreshness,					
											TRequestStatus& aStatus);

	/**
	 *  Retrieve authentication policy for an already existing key in the store.
	 * 
	 * @param aHandle					The handle of the key whose policy is to be retrieved.
	 * @param aAuthenticationString		The expression associated to this key. The memory would
	 * 									be allocated at the server side.
	 * @param aFreshness				The validity associated to this key.
	 * 									Specification is in seconds.
	 * @param aStatus					Final status of the operation. 
	*/
	
	IMPORT_C void GetAuthenticationPolicy(	const TCTTokenObjectHandle aHandle, 
											HBufC*& aAuthenticationString,
											TInt& aFreshness,					
											TRequestStatus& aStatus);
#endif // SYMBIAN_AUTH_SERVER
	
private:
	CUnifiedKeyStore(RFs& aFs);
	void ConstructL();
private:	//	From CActive
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
private:
	enum TState
		{
		EIdle,
		EInitializeGetTokenList,
		EInitializeGetToken,
		EInitialiseGetKeyManagerInterface,
		EInitializeGetKeyUserInterface,
		EInitializeGetKeyUserInterfaceFinished,
		EInitializeFinished,
	//	----------------------------------------------
		EList,
		EGetKeyInfo,
		EOpen,
	//	----------------------------------------------
		ECreateKey,
		EImportKey,
		EImportKeyEncrypted,
		EExportKey,
		EExportEncryptedKey,
		EExportPublic,
		EDeleteKey,
		ESetUsePolicy,
		ESetManagementPolicy,
		ESetPassphraseTimeout,
		ERelock,
		ESetAuthenticationPolicy,
		EGetAuthenticationPolicy
		};
private:
	void StartAsyncOperation(TState aState, TRequestStatus& aStatus);
	void DoInitializeL();
	TBool DoOpen(const TCTTokenObjectHandle& aHandle, 
				 TRequestStatus& aStatus);
	void PrepareToCreateKeyL(TInt aKeyStoreIndex,
							TKeyUsagePKCS15 aUsage, TUint aSize, 
							const TDesC& aLabel,
							CCTKeyInfo::EKeyAlgorithm aAlgorithm,
							TInt aAccessType,
							TTime aStartDate, TTime aEndDate,  
							TRequestStatus& aStatus);
	/**
	 * A synchronous method to find the key store given a token object handle.
	 * Returns NULL if none found.
	 */
	MCTKeyStore* FindKeyStore(const TCTTokenObjectHandle& aHandle);
	/**
	 * A synchronous method to find the key store manager given a token object
	 * handle.  Returns NULL if none found.
	 */
	MCTKeyStoreManager* FindKeyStoreManager(const TCTTokenObjectHandle& aHandle);
	/** Complete the user's request and clean up state. */
	void Complete(TInt aError);
	/** Clean up state. */
	void Cleanup();
	/** Cancel the outstanding request. */
	void CancelOutstandingRequest();
private:
	/**
	 * A wrapper around a keystore interface that remebers whether it is a
	 * readonly or manager interface.
	 */
	class CKeyStoreIF
	{
	public:
		CKeyStoreIF(MCTTokenInterface*, TBool);
		~CKeyStoreIF();
	public:
		inline MCTTokenInterface* KeyStore() const {return (iKeyStore);};
		inline TBool IsKeyManager() const {return (iIsKeyManager);};
	private:
		CKeyStoreIF(){};
	private:
		MCTTokenInterface* iKeyStore;
		TBool iIsKeyManager;
	};
private:
	RFs& iFs;
	TState iState;
	TRequestStatus* iOriginalRequestStatus;
	RPointerArray<CKeyStoreIF> iKeyStoresHolder;

	RCPointerArray<CCTTokenTypeInfo> iTokenTypes;
	TInt iIndexTokenTypes;
	MCTTokenType* iTokenType;	
	MCTToken* iToken;
	MCTTokenInterface* iTokenInterface;
	TUid iRequestUid;	
	RCPointerArray<HBufC> iTokens;
	TInt iIndexTokens;
	
	MCTKeyStore* iKeyStore;               ///< The key store in use by the current operation or NULL
	MCTKeyStoreManager* iKeyStoreManager; ///< The key store manager in use by the current operation or NULL
	
	RMPointerArray<CCTKeyInfo>* iKeyInfos;
	TCTKeyAttributeFilter* iFilter;
	CCTKeyInfo* iKeyInfo;
	HBufC8* iKeyData;
	CCTKeyInfo** iKeyInfoOut; 			 ///< Pointer to client's key info pointer
	CPBEncryptParms* iPbeParams;     // PBE parameters for encrypted key export

	TInt iIndex;
	TInt iNewTimeout;
};

#endif
