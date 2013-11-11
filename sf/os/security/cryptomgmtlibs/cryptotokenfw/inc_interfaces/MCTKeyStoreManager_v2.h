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
* MKeystoreManager.h
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __MCTKEYSTOREMANAGER_H__
#define __MCTKEYSTOREMANAGER_H__

#include "mctkeystore.h"

/** */
const TInt KInterfaceKeyStoreManager = 0x101F7335;

class CPBEncryptParms;

/**
 * Defines the interface for a key store manager token.
 *
 * This documentation describes the security policy that must be enforced by
 * implementations of the interface.
 * 
 * @publishedPartner
 * @released
 */
class MCTKeyStoreManager : public MCTKeyStore
	{
public:
	/**
	 * Key creation
	 */
	
	/**
	 * Generates a new key pair and store it in the keystore.
	 *	
	 * @param aReturnedKey	This is filled by the caller with required
	 *						attributes, leaving the TKeyIdentifier iID and object handle iHandle
	 *						uninitialised - these values are set if the key is created successfully
	 * @param aStatus		This will be completed with the final status code
	 *	
	 * @capability WriteUserData	Requires the caller to have WriteUserData capability
	 * @leave KErrPermissionDenied	If the caller does not have WriteUserData capability
	 * @leave KErrAlreadyExists		If a key with the specified label already
	 *								exists in the keystore.
	 * @leave KErrKeySize			If the requested key size is not supported.
	 * @leave KErrKeyAccess			If an invalid combination of key access flags were specified.
	 * @leave KErrKeyValidity		If a validity period was specified, but the end
	 *								date was in the past.
	 */
	virtual void CreateKey(CCTKeyInfo*& aReturnedKey,
						   TRequestStatus& aStatus) = 0;

	/** Cancels an ongoing CreateKey() operation. */
	virtual void CancelCreateKey() = 0;
	
	/**
	 * Import keys
	 */
	
	/**
	 * Imports a cleartext key pair into the keystore.
	 *
	 * The import data is DER-encoded PKCS#8 format.
	 * 
	 * @param aKey			This is a descriptor representation of the PKCS#8 key data.
	 * @param aReturnedKey	This is filled by the caller with required
	 *						attributes, leaving the TKeyIdentifier iID and object handle iHandle
	 *						uninitialised - these values are set if the key is created successfully.
	 *	
	 * @capability WriteUserData	Requires the caller to have WriteUserData capability
	 * @leave KErrPermissionDenied	If the caller does not have WriteUserData capability
	 * @leave KErrAlreadyExists		If a key with the specified label already exists
	 *								in the keystore.
	 * @leave KErrKeySize			If the requested key size is not supported.
	 * @leave KErrKeyAccess			If an invalid combination of key access flags were specified.
	 * @leave KErrKeyValidity		If a validity period was specified, but the end
	 *								date was in the past.
	 * @leave KErrArgument			If there is an error parsing the key data.
	 */
	virtual void ImportKey(const TDesC8& aKey, 
						   CCTKeyInfo*& aReturnedKey,
						   TRequestStatus& aStatus) = 0;

	/** Cancels an ongoing ImportKey() operation. */
	virtual void CancelImportKey() = 0;

	/**
	 * Imports an encrypted key pair into the keystore.
	 *
	 * The import data is DER-encoded PKCS#5/PKCS#8 format.
	 * 
	 * @param aKey			This is a descriptor representation of the PKCS#8 key data
	 * @param aReturnedKey	This is filled by the caller with required
	 *						attributes, leaving the TKeyIdentifier iID and object handle iHandle
	 *						uninitialised - these values are set if the key is created successfully
	 *   
	 * @capability WriteUserData	Requires the caller to have WriteUserData capability
	 * @leave KErrPermissionDenied	If the caller does not have WriteUserData capability
	 * @leave KErrAlreadyExists		If a key with the specified label already exists
	 *								in the keystore.
	 * @leave KErrKeySize			If the requested key size is not supported.
	 * @leave KErrKeyAccess			If an invalid combination of key access flags were specified.
	 * @leave KErrKeyValidity		If a validity period was specified, but the end
	 *								date was in the past.
	 * @leave KErrArgument			If there is an error parsing the key data.
	 */
	virtual void ImportEncryptedKey(const TDesC8& aKey, 
									CCTKeyInfo*& aReturnedKey,
									TRequestStatus& aStatus) = 0;

	/** Cancels an ongoing ImportEncryptedKey() operation. */
	virtual void CancelImportEncryptedKey() = 0;

	/**
	 * Export keys
	 */
	
	/**
	 * Exports a key pair in the clear.
	 *
	 * The key is exported as DER-encoded PKCS#8 data.
	 *
	 * @param aHandle	The handle of the key to export
	 * @param aKey		A reference to a HBufC8 pointer.  The pointer will be set to
	 *					a newly allocated buffer containing the key data.  It is the caller's
	 *					responsibility to delete this buffer.
	 *				
	 * @capability Dependent 		Requires the caller to have any capabilities specified in the
	 *								key management security policy.
	 * @leave KErrPermissionDenied	If the caller does not conform to the key
	 *								management security policy.
	 * @leave KErrNotFound			If the key the handle referes to does not exist.
	 * @leave KErrKeyAccess			If the sensitive flag is set for the key, or the
	 *								exportable flag is not set.
	 * @leave KErrKeyAlgorithm		If this type of key cannot be exported.
	 */
	virtual void ExportKey(TCTTokenObjectHandle aHandle,
						   HBufC8*& aKey,
						   TRequestStatus& aStatus) = 0;
	
	/** Cancels an ongoing ExportKey() operation. */
	virtual void CancelExportKey() = 0;

	/**
	 * Exports an encrypted key pair.
	 *
	 * The key is exported as DER-encoded PKCS#5/PKCS#8 data.
	 *
	 * @param aHandle	The handle of the key to export
	 * @param aKey		A reference to a HBufC8 pointer.  The pointer will be set to
	 *					a newly allocated buffer containing the key data.
	 * @param aParams	The PBE encryption parameters to use when encrypting the key.
	 * 
	 * @capability Dependent 		Requires the caller to have any capabilities specified in the
	 *								key management security policy.
	 * @leave KErrPermissionDenied	If the caller does not conform to the key
	 *								management security policy.
	 * @leave KErrNotFound			If the key the handle referes to does not exist. 
	 * @leave KErrKeyAccess			If the exportable flag is not set for the key.
	 * @leave KErrKeyAlgorithm		If this type of key cannot be exported.
	 */
	virtual void ExportEncryptedKey(TCTTokenObjectHandle aHandle,
    							    const CPBEncryptParms& aEncryptParams,
									HBufC8*& aKey,
									TRequestStatus& aStatus) = 0;
									
	/** Cancels an ongoing ExportEncryptedKey() operation. */
	virtual void CancelExportEncryptedKey() = 0;

	/**
	 * Deletes a key.
	 * 
	 * @param aHandle	The handle of the key to delete
	 *	
	 * @capability Dependent 		Requires the caller to have any capabilities specified in the
	 *								key management security policy.
	 * @leave KErrPermissionDenied	If the caller does not conform to the key
	 *								management security policy.
	 * @leave KErrNotFound			If the key the handle referes to does not exist. 
	 * @leave KErrAccessDenied		If the calling process is not allowed to delete the key.
	 * @leave KErrInUse				If another client is currently using the key.
	 */
	virtual void DeleteKey(TCTTokenObjectHandle aHandle, 
						   TRequestStatus& aStatus) = 0;

	/** Cancels an ongoing DeleteKey() operation. */
	virtual void CancelDeleteKey() = 0;

	/**
	 * Sets the security policy for key use.
	 *
	 * Specifies which processes are allowed to use the key for cryptographic
	 * operations.
	 *
	 * @param aHandle	The handle of the key
	 * @param aPolicy	The new security policy.
	 *	
	 * @capability Dependent 		Requires the caller to have any capabilities specified in the
	 *								key management security policy.
	 * @leave KErrPermissionDenied	If the caller does not conform to the key
	 *								management security policy.
	 * @leave KErrNotFound			If the key the handle referes to does not exist.
	 */
	virtual void SetUsePolicy(TCTTokenObjectHandle aHandle,
							  const TSecurityPolicy& aPolicy,
							  TRequestStatus& aStatus) = 0;

	/** Cancels an ongoing SetUsePolicy() operation. */
	virtual void CancelSetUsePolicy() = 0;

	/**
	 * Sets the security policy for key management.
	 *
	 * Specifies which processes are allowed to perform management operations on
	 * the key.
	 *
	 * @param aHandle	The handle of the key
	 * @param aPolicy	The new security policy.
	 *	
	 * @capability Dependent 		Requires the caller to have any capabilities specified in the
	 *								current and new key management security policies.
	 * @leave KErrPermissionDenied	If the caller does not conform to the current
	 *								and new key management security policies.
	 * @leave KErrNotFound			If the key the handle referes to does not exist.
	 */
	virtual void SetManagementPolicy(TCTTokenObjectHandle aHandle,
									 const TSecurityPolicy& aPolicy,
									 TRequestStatus& aStatus) = 0;

	/** Cancels an ongoing SetManagementPolicy() operation. */
	virtual void CancelSetManagementPolicy() = 0;

	/**
	 * Sets the passphrase timeout for all keys owned by this process.
	 * 
	 * @param aTimeout	The timeout in seconds. 0 means that the passphrase is
	 *					always asked for, and -1 means that it is never expired
	 * @param aStatus	This will be completed with the final status code
	 *	
	 * @capability Dependent 		Requires the caller to have any capabilities specified in the
	 *								key management security policy.
	 * @leave KErrPermissionDenied	If the caller does not conform to the key
	 *								management security policy.
	 * @leave KErrArgument			If the timeout specified is invalid.
	 */
	virtual void SetPassphraseTimeout(TInt aTimeout, 
									  TRequestStatus& aStatus) = 0;

	/** Cancels an ongoing SetPassphraseTimeout() operation. */
	virtual void CancelSetPassphraseTimeout() = 0;

	/**
	 * Re-locks the entire store (i.e., forget the pasphrase) 
	 *
	 * @param aStatus	This will be completed with the final status code
	 */
	virtual void Relock(TRequestStatus& aStatus) = 0;

	/** Cancels an ongoing Relock() operation. */
	virtual void CancelRelock() = 0;

#ifdef SYMBIAN_AUTH_SERVER
	/**
	 * Generate a new key pair and store it in keystore. The key should be stored against an
	 * already authenticated user. If there is no user currently authenticated then authentication
	 * should be performed before creation of the key.
	 *	
	 * @param aReturnedKey				This is filled by the caller with required
	 *									attributes, leaving the TKeyIdentifier iID and object handle 
	 * 									iHandle	uninitialised - these values are set if the key is 
	 * 									created successfully.
 	 * @param aAuthenticationString		The expression through which a user can be authenticated.
	 * 									Currently this should correspond to one of the alias values
	 * 									set by the licensee for authentication server configuration.
	 * 									This should be stored against the created key.
	 * @param aFreshness				The validity to be considered for an already authenticated 
	 * 									identity. This should be stored against the created key. 
	 * 									Specification is in seconds.
	 * 
	 * @param aStatus					This will be completed with the final status code. Some specific
	 * 									codes are as follows.
	 * 									KErrPermissionDenied		If the caller does not have WriteUserData 
	 * 																capability.
	 * 									KErrAlreadyExists			If a key with the specified label already
	 *																exists in the keystore for the specific
	 * 																user.
	 * 									KErrKeySize					If the requested key size is not supported.
	 * 									KErrKeyAccess				If an invalid combination of key access 
	 * 																flags were specified.
	 * 									KErrKeyValidity				If a validity period was specified, but the 
	 * 																end date was in the past.
	 * 									KErrAuthenticationFailure	If the user authentication fails.
	 * 
	 * @see 							AuthServer::CAuthExpression
	 * 
	 * @capability WriteUserData	Should require the caller to have WriteUserData capability.
	 */
	virtual void CreateKey(	const TDesC& /*aAuthenticationString*/, 
							TInt /*aFreshness*/,
							CCTKeyInfo*& /*aReturnedKey*/,
							TRequestStatus& aStatus )
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,KErrNotSupported);
		}

	/**
	 * Imports a der-encoded PKCS#8 format key pair into the keystore. The key should be stored against an
	 * already authenticated user. If there is no user currently authenticated then authentication
	 * should be performed before creation of the key.
	 *
	 * 
	 * @param aKey						This is a descriptor representation of the PKCS#8 key data.
	 * @param aAuthenticationString		The expression through which a user can be authenticated.
	 * 									Currently this should correspond to one of the alias values
	 * 									set by the licensee for authentication server configuration.
	 * 									This should be stored against the created key.
	 * @param aFreshness				The validity to be considered for an already authenticated 
	 * 									identity. This should be stored against the created key.
	 * 									Specification is in seconds.
	 * @param aReturnedKey				This is filled by the caller with required
	 *									attributes, leaving the TKeyIdentifier iID and object handle 
	 * 									iHandle	uninitialised - these values are set if the key is 
	 * 									created successfully.
	 * @param aStatus					This will be completed with the final status code. Some specific
	 * 									codes are as follows.
	 * 									KErrPermissionDenied	If the caller does not have WriteUserData 
	 * 															capability.
	 * 									KErrAlreadyExists		If a key with the specified label already 
	 * 															exists in the keystore for the specific
	 * 															user.
	 * 									KErrKeySize				If the requested key size is not supported.
	 * 									KErrKeyAccess			If an invalid combination of key access flags 
	 * 															were specified.
	 * 									KErrKeyValidity			If a validity period was specified, but the end
	 *															date was in the past.
	 * 									KErrArgument			If there is an error parsing the key data.
	 * 									KErrAuthenticationFailure	If the user authentication fails.
	 * 
	 * @see 							AuthServer::CAuthExpression
	 * 
	 * @capability WriteUserData			Should require the caller to have WriteUserData capability
	*/
	virtual void ImportKey( const TDesC8& /*aKey*/, 
							const TDesC& /*aAuthenticationString*/, 
							TInt /*aFreshness*/, 
							CCTKeyInfo*& /*aReturnedKey*/, 
							TRequestStatus& aStatus )
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,KErrNotSupported);
		}

	/**
	 * Imports an encrypted key pair into the keystore. The key should be stored against an
	 * already authenticated user. If there is no user currently authenticated then authentication
	 * should be performed before creation of the key.
	 *
	 * The import data should be der-encoded PKCS#8 format encrypted in accordance to PKCS#5 
	 * specification.
	 * 
	 * @param aKey						This is a descriptor representation of the PKCS#8 key data.
	 * 									Encrypted in accordance to PKCS#5 specification.	
	 * @param aAuthenticationString		The expression through which a user can be authenticated.
	 * 									Currently this should correspond to one of the alias values
	 * 									set by the licensee for authentication server configuration.
	 * 									This should be stored against the created key.
	 * @param aFreshness				The validity to be considered for an already authenticated 
	 * 									identity. This should be stored against the created key.
	 * 									Specification is in seconds.
	 * @param aReturnedKey				This is filled by the caller with required
	 *									attributes, leaving the TKeyIdentifier iID and object handle 
	 * 									iHandle	uninitialised - these values are set if the key is 
	 * 									created successfully.
	 * @param aStatus					This will be completed with the final status code. Some specific
	 * 									codes are as follows.
	 * 									KErrPermissionDenied	If the caller does not have WriteUserData 
	 * 															capability.
	 * 									KErrAlreadyExists		If a key with the specified label already 
	 * 															exists in the keystore for the specific
	 * 															user.
	 * 									KErrKeySize				If the requested key size is not supported.
	 * 									KErrKeyAccess			If an invalid combination of key access flags 
	 * 															were specified.
	 * 									KErrKeyValidity			If a validity period was specified, but the end
	 *															date was in the past.
	 * 									KErrArgument			If there is an error parsing the key data.
	 * 									KErrAuthenticationFailure	If the user authentication fails.
	 * 
	 * @see 							AuthServer::CAuthExpression
	 * 
	 * @capability WriteUserData		Should require the caller to have WriteUserData capability
	*/
	virtual void ImportEncryptedKey( 	const TDesC8& /*aKey*/, 
										const TDesC& /*aAuthenticationString*/, 
										TInt /*aFreshness*/, 
										CCTKeyInfo*& /*aReturnedKey*/, 
										TRequestStatus& aStatus )
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNotSupported);
		}
	
	/**
	 *  Set the authentication policy for an already existing key in the store.
	 * 
	 * @param aHandle					The handle of the key whose policy is to be changed.
	 * @param aAuthenticationString		The expression associated to this key.
	 * @param aFreshness				The validity associated to this key.
	 * 									Specification is in seconds.
	 * @param aStatus					Final status of the operation.
	 * 									KErrAuthenticationFailure	If the user authentication fails.
	 * 
	 * @see 							AuthServer::CAuthExpression
	 *   
	*/
	virtual void SetAuthenticationPolicy(	const TCTTokenObjectHandle /*aHandle*/,
											const TDesC& /*aAuthenticationString*/,
											TInt /*aFreshness*/,
											TRequestStatus& aStatus)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNotSupported);
		}
	
	/**
	 *  Retrieve authentication policy for an already existing key in the store.
	 * 
	 * @param aHandle					The handle of the key whose policy is to be retrieved.
	 * @param aAuthenticationString		The expression associated to this key. The memory would
	 * 									be allocated at the server side.
	 * @param aFreshness				The validity associated to this key.
	 * 									Specification is in seconds.
	 * @param aStatus					Final status of the operation.
	 * 									KErrAuthenticationFailure	If the user authentication fails.
	 * 
	 * @see 							AuthServer::CAuthExpression
	 *  
	*/
	virtual void GetAuthenticationPolicy(	const TCTTokenObjectHandle /*aHandle*/,
											HBufC*& /*aAuthenticationString*/,
											TInt& /*aFreshness*/,
											TRequestStatus& aStatus)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNotSupported);
		}
#endif // SYMBIAN_AUTH_SERVER

};


#endif //	__MCTKEYSTOREMANAGER_H__
