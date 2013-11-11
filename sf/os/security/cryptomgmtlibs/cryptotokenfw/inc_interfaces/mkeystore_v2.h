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
* MKeystore.h
*
*/


/**
 @file 
 @publishedPartner
 @released
*/
 
#ifndef __MKEYSTORE_H__
#define __MKEYSTORE_H__

#include <ct.h>

class CDSASignature;
class CRSASignature;
class CDHParameters;
class CDHPublicKey;
class TInteger;

class CCTKeyInfo;
struct TCTKeyAttributeFilter;

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
namespace CryptoSpi
    {
    class CSigner;
    class CAsymmetricCipher;
    class CCryptoParams;
    }
#endif /* SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT */

/**
 * A template for signer objects.  It isn't possible to use a base class as the
 * signature objects created are not related.
 *
 * This template is be instantiated with a CRSASignature* as the
 * signature class for RSA signatures and with a CDSASignature* as the Signature
 * for DSA.
 * 
 */
template <class Signature> class MCTSigner : public MCTTokenObject
	{
 public:
	/**
	 * Sign some data.
	 * 
	 * The data is hashed before the signature is created using the SHA-1
	 * algorithm.
	 * 
	 * @param aPlaintext The string to be signed.
	 * @param aSignature The returned signature.  A new signature object is
	 *     created which is owned by the caller.
	 *     
	 * @capability Dependent Requires the caller to have any capabilities specified in the
	 *     key use security policy.
	 * @leave KErrPermissionDenied If the caller does not conform to the key use
	 *     security policy.
	 * @leave KErrBadPassphrase If the user failed to enter the correct passphrase.
	 */
	virtual void SignMessage(const TDesC8& aPlaintext, Signature& aSignature, 
							 TRequestStatus& aStatus) = 0;

	/**
	 * Perform a raw signing operation.
	 *	
	 * @param aPlaintext The string to be signed - this should be some form of
	 *     hash of the actual message to be signed.   In order to generate valid PKCS#1 v1.5 signature 
	 *     aPlainText should consist of ASN.1 encoded digest algorithm ID and hash as described in RFC2313.
	 *     If the data is too long,  this method will return KErrOverflow through aStatus.
	 * @param aSignature The returned signature.  A new signature object is
	 *     created which is owned by the caller.
	 * @capability Dependent Requires the caller to have any capabilities specified in the
	 *     key use security policy.
	 * @leave KErrPermissionDenied If the caller does not conform to the key use
	 *     security policy.
	 * @leave KErrBadPassphrase If the user failed to enter the correct passphrase.
	 */
	virtual void Sign(const TDesC8& aPlaintext, Signature& aSignature, 
					  TRequestStatus& aStatus) = 0;

	/** Cancel an ongoing Sign() or SignMessage() operation. */
	virtual void CancelSign() = 0;
	
 protected:
	inline MCTSigner(MCTToken& aToken);
	virtual ~MCTSigner() = 0;
	};

/**
 * An RSA signer object.
 * 
 */
typedef MCTSigner<CRSASignature*> MRSASigner;

/**
 * A DSA signer object.
 * 
 */
typedef MCTSigner<CDSASignature*> MDSASigner; 

/**
 * A Decryptor. To do a private decrypt, you need to get one of these
 * objects.
 * 
 */
class MCTDecryptor : public MCTTokenObject
	{
 public:
	/**
	 * Do a private decrypt.
	 * 
	 * @param aCiphertext The data to decrypt
	 * @param aPlaintext The returned plaintext
	 * @capability Dependent Requires the caller to have any capabilities specified in the
	 *     key use security policy.
	 * @leave KErrPermissionDenied If the caller does not conform to the key use
	 *     security policy.
	 * @leave KErrBadPassphrase If the user failed to enter the correct passphrase.
	 */
	virtual void Decrypt(const TDesC8& aCiphertext, TDes8& aPlaintext,
				 TRequestStatus& aStatus) = 0;
	
	/** Cancel an ongoing Decrypt() operation. */
	virtual void CancelDecrypt() = 0;
	
 protected:
	inline MCTDecryptor(MCTToken& aToken);
	inline virtual ~MCTDecryptor() = 0;
	};

/**
 * A Diffie-Hellman key agreement object.
 * 
 */
class MCTDH : public MCTTokenObject
	{
 public:
	/**
	 * Returns the public key ('Big X') for the supplied set of parameters.
	 * 
	 * @param aN The DH modulus parameter.
	 * @param aG The DH generator parameter.
	 * @param aX The returned public key.  A new object is created which is
	 *     owned by the caller.
	 * @capability Dependent Requires the caller to have any capabilities specified in the
	 *     key use security policy.
	 * @leave KErrPermissionDenied If the caller does not conform to the key use
	 *     security policy.
	 * @leave KErrBadPassphrase If the user failed to enter the correct passphrase.
	 */
	virtual void PublicKey(const TInteger& aN, const TInteger& aG, 
						   CDHPublicKey*& aX,
						   TRequestStatus& aStatus) = 0;
	
	/**
	 * Agrees a session key.
	 * 
	 * @param aY The public key of the other party.
	 * @param aAgreedKey The returned key.  A new object is created which is
	 * owned by the caller.
	 * @capability Dependent Requires the caller to have any capabilities specified in the
	 *     key use security policy.
	 * @leave KErrPermissionDenied If the caller does not conform to the key use
	 *     security policy.
	 * @leave KErrBadPassphrase If the user failed to enter the correct passphrase.
	 */
	virtual void Agree(const CDHPublicKey& aY,
					   HBufC8*& aAgreedKey,
					   TRequestStatus& aStatus) = 0;
	
	/** Cancels either a PublicKey() or Agree() operation */
	virtual void CancelAgreement() = 0;
	
 protected:
	inline MCTDH(MCTToken& aToken);
	inline virtual ~MCTDH() = 0;
	};

/**
 * Defines the interface for a read-only key store.
 *
 * This prvides the API for the client to query the keys and open objects
 * allowing crypto operations to be performed.
 *
 * This documentation describes the security policy that must be enforced by
 * implementations of the interface.
 * 
 */
class MKeyStore
	{
public:
	/**
	 * Listing keys
	 */
	
	/** 
	 * List all the keys in the store that match the filter.
	 *
	 * @param aKeys An array to which the returned keys are appended
	 * @param aFilter a filter controlling which keys are returned
	 * @param aStatus This will be completed with the final status code
	 * @capability ReadUserData requires the caller to have ReadUserData capability
	 * @leave KErrPermissionDenied if the caller does not have ReadUserData capability
	 */
	virtual void List(RMPointerArray<CCTKeyInfo>& aKeys, 
					  const TCTKeyAttributeFilter& aFilter, 
					  TRequestStatus& aStatus) = 0;
	
	/** Cancel an ongoing List() operation */
	virtual void CancelList() = 0;
			
	/**
	 * Getting a key given a TCTTokenObjectHandle
	 */
	 
	/**
	 * Retrieves a key given its handle.
	 * 
	 * @param aHandle The handle of the required key
	 * @param aInfo The returned key info
	 * @param aStatus Async request notification
	 * @capability Dependent Requires the caller to have any capabilities specified in the
	 *     key use security policy.
	 * @leave KErrPermissionDenied If the caller does not conform to the key use
	 *     security policy.
	 * @leave KErrNotFound If the key the handle referes to does not exist. 
	 */
	virtual void GetKeyInfo(TCTTokenObjectHandle aHandle, CCTKeyInfo*& aInfo,
							TRequestStatus& aStatus) = 0;
	
	/** Cancel an ongoing GetKeyInfo() operation */
	virtual void CancelGetKeyInfo() = 0;
	
	/**
	 * Opening keys
	 */
	
	/**
	 * Open an RSA key for signing
	 * 
	 * @param aHandle The handle of the key to be opened.  This must be the
	 *     handle of an RSA key on this store that is usable for signing by
	 *     this process or the operation will fail.
	 * @param aSigner The returned signer object.
	 * @param aStatus Asynchronous request notification.
	 * @capability Dependent Requires the caller to have any capabilities specified in the
	 *     key use security policy.
	 * @leave KErrPermissionDenied If the caller does not conform to the key use
	 *     security policy.
	 * @leave KErrNotFound If the key the handle referes to does not exist.
	 * @leave KErrKeyAlgorithm If the key is not an RSA key.
	 * @leave KErrKeyUsage If the key doesn't have sign usage.
	 * @leave KErrKeyValidity If the key is not currently valid.
	 */
	virtual void Open(const TCTTokenObjectHandle& aHandle, 
					  MRSASigner*& aSigner,
					  TRequestStatus& aStatus) = 0;
	
	/**
	 * Open a DSA key for signing
	 * 
	 * @param aHandle The handle of the key to be opened.  This must be the
	 *     handle of a DSA key on this store that is usable by this process
	 *     for signing or the operation will fail.
	 * @param aSigner The returned signer object
	 * @param aStatus Asynchronous request notification.
	 * 
	 * @capability Dependent Requires the caller to have any capabilities specified in the
	 *     key use security policy.
	 * @leave KErrPermissionDenied If the caller does not conform to the key use
	 *     security policy.
	 * @leave KErrNotFound If the key the handle referes to does not exist.
	 * @leave KErrKeyAlgorithm If the key is not a DSA key.
	 * @leave KErrKeyUsage If the key doesn't have sign usage.
	 * @leave KErrKeyValidity If the key is not currently valid.
	 */
	virtual void Open(const TCTTokenObjectHandle& aHandle, 
					  MDSASigner*& aSigner, 
					  TRequestStatus& aStatus) = 0;
	
	/**
	 * Open a RSA key for private decryption
	 * 
	 * @param aHandle The handle of the key to be opened.  This must be the
	 *     handle of a RSA key on this store that is usable by this process
	 *     for decryption or the operation will fail.
	 * @param aDecryptor The returned decryptor object
	 * @param aStatus Asynchronous request notification.
	 * 
	 * @capability Dependent Requires the caller to have any capabilities specified in the
	 *     key use security policy.
	 * @leave KErrPermissionDenied If the caller does not conform to the key use
	 *     security policy.
	 * @leave KErrNotFound If the key the handle referes to does not exist.
	 * @leave KErrKeyAlgorithm If the key is not an RSA key.
	 * @leave KErrKeyUsage If the key doesn't have decrypt usage.
	 * @leave KErrKeyValidity If the key is not currently valid.
	 */
	virtual void Open(const TCTTokenObjectHandle& aHandle, 
					  MCTDecryptor*& aDecryptor,
					  TRequestStatus& aStatus) = 0;
	
	/**
	 * Open a DH key for key agreement
	 * 
	 * @param aHandle The handle of the key to be opened.  This must be the
	 *     handle of a DH key on this store that is usable by this process
	 *     for decryption or the operation will fail.
	 * @param aDH The returned agreement object
	 * @param aStatus Asynchronous request notification.
	 * 
	 * @capability Dependent Requires the caller to have any capabilities specified in the
	 *     key use security policy.
	 * @leave KErrPermissionDenied If the caller does not conform to the key use
	 *     security policy.
	 * @leave KErrNotFound If the key the handle referes to does not exist.
	 * @leave KErrKeyAlgorithm If the key is not a DH key.
	 * @leave KErrKeyUsage If the key doesn't have derive usage.
	 * @leave KErrKeyValidity If the key is not currently valid.
	 */
	virtual void Open(const TCTTokenObjectHandle& aHandle, 
					  MCTDH*& aDH, TRequestStatus& aStatus) = 0;

	/** Cancels an ongoing Open() operation */
	virtual void CancelOpen() = 0;
	
	/**
	 * Exporting keys
	 */

	/**
	 * Get the public half of a key pair.
	 *
	 * The key is returned in DER-encoded ASN-1. The format is that of the X509
	 * SubjectPublicKeyInfo type.
	 *
	 * <p>For RSA keys, the format is:</p>
	 * <pre>
	 * SEQUENCE-OF
	 *     SEQUENCE-OF
	 *         OID of the encryption algorithm (KRSA)
	 *         NULL
	 *     BIT STRING encoded public key.
	 * </pre>
	 *
	 * <p>For DSA keys, the format is:</p>
	 * <pre>
	 * SEQUENCE-OF
	 *     SEQUENCE-OF
	 *         OID dsa (1.2.840.10040.4.1)
	 *         SEQUENCE-OF
	 *             INTEGER p
	 *             INTEGER q
	 *             INTEGER g
	 *     BIT STRING
	 *         INTEGER public value (y)
	 * </pre>
	 *
	 * @param aHandle The handle of the key.
	 * @param aPublicKey A pointer to a buffer.  This will be set to a newly
	 *     created buffer containing the exported key data.  The caller is
	 *     responsible for deleting the buffer.
	 * @capability Dependent Requires the caller to have any capabilities specified in the
	 *     key use security policy.
	 * @leave KErrPermissionDenied If the caller does not conform to the key use
	 *     security policy.
	 * @leave KErrNotFound If the key the handle referes to does not exist.
	 * @leave KErrKeyAlgorithm If the key is not an RSA or DSA key.
	 */
	virtual void ExportPublic(const TCTTokenObjectHandle& aHandle,
							  HBufC8*& aPublicKey,
							  TRequestStatus& aStatus) = 0;
	
	/** Cancels an ongoing ExportPublic() operation */
	virtual void CancelExportPublic() = 0;

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    /**
     * Opens a key for signing. This function returns a CryptoSPI
     * signer object. SignL() method should be invoked on this object
     * to perform the signing operation.
     *
     * @param aHandle The handle of the key to be opened.
     * @param aSigner The returned CryptoSPI signer object.
     * @param aStatus Returns the status of asynchronous operation, 
     * possible values of which are given below:-
     * KErrNone if successful, otherwise a system wide error 
     * code (in such a case signer object is not allocated). The 
     * most likely error codes are:-
     * - KErrNotSupported Default value, used if licensee does not
     * provide an implementation.
     * - KErrPermissionDenied If the caller does not conform to
     * the key use security policy.
     * - KErrNotFound If the key the handle referes to does not
     * exist.
     * - KErrKeyUsage If the key doesn't have sign usage.
     * - KErrKeyValidity If the key is not currently valid.
     * - KErrKeySize If the key length is too small.
     * - KErrKeyAccess If an invalid combination of key access
     * flags were specified.
     * 
     * @capability Requires the caller to have the capabilities
     * specified in the key use security policy.
     */
	virtual void Open(const TCTTokenObjectHandle& /*aHandle*/,
	                  CryptoSpi::CSigner*& /*aSigner*/,
	                  TRequestStatus& aStatus)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,KErrNotSupported);
		}

    /**
     * Opens a key for decryption. This function returns a CryptoSPI
     * signer object. ProcessL() method should be invoked on this object
     * to perform the decryption operation.
     *
     * @param aHandle The handle of the key to be opened.
     * @param aAsymmetricCipher The returned CryptoSPI cipher object.
     * @param aStatus Returns the status of asynchronous operation,
     * possible values of which are given below:-
     * KErrNone if successful, otherwise a system wide error 
     * code (in such a case cipher object is not allocated). The 
     * most likely error codes are:-
     * - KErrNotSupported Default value, used if licensee does not
     * provide an implementation.
     * - KErrPermissionDenied If the caller does not conform to
     * the key use security policy.
     * - KErrNotFound If the key the handle referes to does not
     * exist.
     * - KErrKeyUsage If the key doesn't have sign usage.
     * - KErrKeyValidity If the key is not currently valid.
     * - KErrKeySize If the key length is too small.
     * - KErrKeyAccess If an invalid combination of key access
     * flags were specified.
     *
     * @capability Requires the caller to have the capabilities
     * specified in the key use security policy.
     */
	virtual void Open(const TCTTokenObjectHandle& /*aHandle*/,
	                  CryptoSpi::CAsymmetricCipher*& /*aAsymmetricCipher*/,
	                  TRequestStatus& aStatus)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,KErrNotSupported);
		}

	/**
	 * This function takes a token handle and encrypted text as input
	 * and stores the decrypted text as one of the output parameters.
	 * This API would should be used by the licensees who want to perform
	 * decryption operation inside the hardware without using CryptoSPI.
	 *
	 * @param aHandle The handle of the key to be used for decryption.
     * @param aCiphertext Contains the encrypted text which has to be
     * decrypted.
     * @param aPlaintextPtr This contains the decrypted text. Caller
     * should take responsibility of this pointer. Derived classes 
     * should never take ownership of the passed pointer.
     * @param aStatus Returns the status of asynchronous operation,
     * possible values are given below:-
     * KErrNone if successful, otherwise a system wide error 
     * code (in such a case aPlaintextPtr is not allocated). The 
     * most likely error codes are:-
     * - KErrNotSupported Default value, used if licensee does not
     * provide an implementation.
     * - KErrPermissionDenied If the caller does not conform to
     * the key use security policy.
     * - KErrNotFound If the key the handle referes to does not
     * exist.
     * - KErrKeyUsage If the key doesn't have sign usage.
     * - KErrKeyValidity If the key is not currently valid.
     * - KErrKeySize If the key length is too small.
     * - KErrKeyAccess If an invalid combination of key access
     * flags were specified.
     * 
     * @capability Requires the caller to have the capabilities
     * specified in the key use security policy.
     */
	virtual void Decrypt(const TCTTokenObjectHandle& /*aHandle*/,
                         const TDesC8& /*aCiphertext*/,
	                     HBufC8*& /*aPlaintextPtr*/,
	                     TRequestStatus& aStatus)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,KErrNotSupported);
		}

	/**
	 * This function takes a token handle and plain text as input and
	 * returns the signature as one of the output parameters. This API
	 * would enable the licensees to sign a text by just having a handle
	 * to key. The key can be stored in the hardware and does not come
	 * out at all. This API should be used by the licensees who want to
	 * perform signing operation inside the hardware without using
	 * CryptoSPI.
	 *
	 * @param aHandle The handle of the key to be used for decryption.
	 * @param aPlainText Text which has to be signed.
	 * @param aSignature The cryptoSPI signature. Caller
	 * should take responsibility of this pointer. Derived classes 
     * should never take ownership of the passed pointer.
	 * @param aStatus Returns the status of asynchronous operation,
	 * possible values are:-
	 * KErrNone if successful, otherwise a system wide error 
     * code (in such a case aSignature is not allocated). The 
     * most likely error codes are:-
     * - KErrNotSupported Default value, used if licensee does not
     * provide an implementation.
     * - KErrPermissionDenied If the caller does not conform to
     * the key use security policy.
     * - KErrNotFound If the key the handle referes to does not
     * exist.
     * - KErrKeyUsage If the key doesn't have sign usage.
     * - KErrKeyValidity If the key is not currently valid.
     * - KErrKeySize If the key length is too small.
     * - KErrKeyAccess If an invalid combination of key access
     * flags were specified.
     * 
	 * @capability Requires the caller to have the capabilities
	 * specified in the key use security policy.
	 */
	virtual void Sign(	const TCTTokenObjectHandle& /*aHandle*/,
                        const TDesC8& /*aPlainText*/,
	                    CryptoSpi::CCryptoParams*& /*aSignature*/,
	                    TRequestStatus& aStatus)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,KErrNotSupported);
		}
	
#endif /* SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT */

	};


template <class Signature> inline MCTSigner<Signature>::MCTSigner(MCTToken& aToken)
		: MCTTokenObject(aToken)
	{
	}

template <class Signature> inline MCTSigner<Signature>::~MCTSigner()
	{
	}

inline MCTDecryptor::MCTDecryptor(MCTToken& aToken)
		: MCTTokenObject(aToken)
	{
	};

inline MCTDecryptor::~MCTDecryptor()
	{
	};

inline MCTDH::MCTDH(MCTToken& aToken)
		: MCTTokenObject(aToken)
	{
	};

inline MCTDH::~MCTDH()
	{
	};

#endif // __MKEYSTORE_H__
