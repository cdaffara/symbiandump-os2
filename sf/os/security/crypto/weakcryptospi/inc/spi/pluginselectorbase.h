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
* plugin selector interface
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef __CRYPTOAPI_PLUGINSELECTORBASE_H__
#define __CRYPTOAPI_PLUGINSELECTORBASE_H__

#include <e32base.h>


namespace CryptoSpi
	{
	class CHash;
	class CRandom;
	class CSymmetricCipher;
	class CAsymmetricCipher;
	class CSigner;
	class CVerifier;
	class CKeyAgreement;
	class CKeyPairGenerator;
	class CAsyncHash;
	class CAsyncRandom;
	class CAsyncSymmetricCipher;
	class CAsyncAsymmetricCipher;
	class CAsyncSigner;
	class CAsyncVerifier;
	class CAsyncKeyAgreement;
	class CAsyncKeyPairGenerator;
	class CKey;
	class CCryptoParams;
	

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	class CMac;
	class CAsyncMac;
#endif
	
	/**
	Base class for the selectors
	*/
	class MPluginSelector
		{
	public:
		/**
		Destructor
		*/
		inline virtual ~MPluginSelector() = 0;
		
		/**
		 * @deprecated
		 * 
		 * Create a new instance of a hash object
		 * 
		 * @param 	aHash The pointer to CHash
		 * @param 	aAlgorithmUid The specific hash algorithm e.g. MD2, SHA1
		 * @param 	aOperationMode The operation mode of the hash e.g. Hash mode, Hmac mode
		 * @param 	aKey The key for Hmac mode, which should be NULL in Hash mode
		 * @param 	aAlgorithmParams The parameters that are specific to a particular 
		 * 			algorithm. This is for extendibility and will normally be null.
		 * @leave 	KErrNone if successful; otherwise, a system wide error code.
		 */
		virtual void CreateHashL(CHash*& aHash,
								TUid aAlgorithmUid,
								TUid aOperationMode,
								const CKey* aKey,
								const CCryptoParams* aAlgorithmParams) = 0;
		
		/**
		Creates a new instance of a Random object.

		@param aRandom A reference to a pointer that should be set to point to the new CRandom object.
		@param aAlgorithmUid The algorithm to use
		@param aAlgorithmParams Parameters that are specific this algorithm.
	    @leave KErrNone if successful; otherwise, a system wide error code.
		*/		
		virtual void CreateRandomL(CRandom*& aRandom,
									TUid aAlgorithmUid,
									const CCryptoParams* aAlgorithmParams) = 0;
		
		/**
		Creates a new synchronous instance of a symmetric cipher
		
		@param aCipher	A reference to a pointer that should be set to point to the new symmetric object.
		@param aAlgorithmUid The algorithm to use
		@param aKey The encryption/decryption key.
		@param aCryptoMode The Symmetric cipher mode.
		@param aOperationMode The Symmetric cipher operation mode.
		@param aPaddingMode The Symmetric cipher padding mode.
		@param aAlgorithmParams Parameters that are specific to a particular algorithm. This is for extendibility and will normally be null.
		@leave	KErrNone if successful; otherwise, a system wide error code.
		*/
		virtual void CreateSymmetricCipherL(CSymmetricCipher*& aCipher,
											TUid aAlgorithmUid,
											const CKey& aKey,
											TUid aCryptoMode,
											TUid aOperationMode,
											TUid aPaddingMode,
											const CCryptoParams* aAlgorithmParams) = 0;

		/**
		Creates a new instance of an asymmetric cipher
		
		@param aCipher A reference to a pointer that should be set to point to the new asymmetric cipher object.
		@param aAlgorithmUid The asymmetric cipher algorithm to use (e.g. KRsaCipherUid)
		@param aKey The encryption/decryption key.
		@param aCryptoMode whether to encrypt or decrypt
		@param aPaddingMode The padding mode to use
		@param aAlgorithmParams Parameters that are specific to a particular algorithm. This is for extendibility and will normally be null.
		@leave	KErrNone if successful; otherwise, a system wide error code.
		*/
		virtual void CreateAsymmetricCipherL(CAsymmetricCipher*& aCipher,
											TUid aAlgorithmUid,
											const CKey& aKey,
											TUid aCryptoMode,
											TUid aPaddingMode,									
											const CCryptoParams* aAlgorithmParams) = 0;

		/**
		Creates a new instance of a synchronous signer.

		@param aSigner A reference to a pointer that should be set to point to the new signer object.
		@param aAlgorithmUid The algorithm to use.
		@param aKey The signing key.
		@param aPaddingMode The padding mode of the signer.
		@param aAlgorithmParams Parameters that are specific to a particular algorithm. This is for extendibility and will normally be null.
		@leave	KErrNone if successful; otherwise, a system wide error code.
		*/
		virtual void CreateSignerL(CSigner*& aSigner,
									TUid aAlgorithmUid,
									const CKey& aKey,
									TUid aPaddingMode,
									const CCryptoParams* aAlgorithmParams) = 0;

		/**
		Creates a new instance of a verifier.
		
		@param aVerifier A reference to a pointer that should be set to point to the new verifier object.
		@param aAlgorithmUid The algorithm to use
		@param aKey The key to verify the signature with.
		@param aPaddingMode The padding mode of the signer.
		@param aAlgorithmParams Parameters that are specific to a particular algorithm. This is for extendibility and will normally be null.
		@leave	KErrNone if successful; otherwise, a system wide error code.
		*/
		virtual void CreateVerifierL(CVerifier*& aVerifier,
									TUid aAlgorithmUid,
									const CKey& aKey,
									TUid aPaddingMode,
									const CCryptoParams* aAlgorithmParams) = 0;

		/**
		Creates a new instance of a key pair generator.

		@param aKeyPairGenerator A reference to a pointer that should be set to point to the new asymmetric key pair generator object.
		@param aKeyAlgorithmUid	The algorithm UID
		@param aAlgorithmParams	The parameters that are specific to a particular algorithm. This is for extendibility and will normally be null.
		@leave	KErrNone if successful; otherwise, a system wide error code.
		*/
		virtual void CreateKeyPairGeneratorL(CKeyPairGenerator*& aKeyPairGenerator,
											TUid aKeyAlgorithmUid,
											const CCryptoParams* aAlgorithmParams) = 0;


		/**
		Creates a new instance of a key agreement system.

		@param aKeyAgreement A reference to a pointer that should be set to point to the new key agreement object.
		@param aAlgorithmUid The algorithm to use
		@param aPrivateKey The private key to combine with the other parties public key during the agreement.
		@param aAlgorithmParams The parameters that are specific to a particular algorithm. This is for extendibility and will normally be null.
		@leave	KErrNone if successful; otherwise, a system wide error code.
		*/
		virtual void CreateKeyAgreementL(CKeyAgreement*& aKeyAgreement,
										TUid aAlgorithmUid,
										const CKey& aPrivateKey,
										const CCryptoParams* aAlgorithmParams) = 0;

		/**
		 * @deprecated
		 * 
		 * Create a new instance of a asynchronous hash object
		 *
		 * @param 	aHash The pointer to CHash
		 * @param 	aAlgorithmUid The specific hash algorithm e.g. MD2, SHA1
		 * @param 	aOperationMode The operation mode of the hash e.g. Hash mode, Hmac mode
		 * @param 	aKey The key for Hmac mode, which should be NULL in Hash mode
		 * @param 	aAlgorithmParams The parameters that are specific to a particular 
		 * 			algorithm. This is for extendibility and will normally be null.
		 * @leave	KErrNone if successful; otherwise, a system wide error code.
		 */										
		virtual void CreateAsyncHashL(CAsyncHash*& aHash,
										TUid aAlgorithmUid,
										TUid aOperationMode,
										const CKey* aKey,
										const CCryptoParams* aAlgorithmParams) = 0;
		/**
		Creates a new instance of a asynchronous random object.

		@param aRandom A reference to a pointer that should be set to point to the new CRandom object.
		@param aAlgorithmUid The algorithm to use
		@param aAlgorithmParams Parameters that are specific this algorithm.
		@leave	KErrNone if successful; otherwise, a system wide error code.
		*/
		virtual void CreateAsyncRandomL(CAsyncRandom*& aRandom,
										TUid aAlgorithmUid,
										const CCryptoParams* aAlgorithmParams) = 0;
		/**
		Creates a new synchronous instance of a asynchronous symmetric cipher
		
		@param aCipher	A reference to a pointer that should be set to point to the new symmetric object.
		@param aAlgorithmUid The algorithm to use
		@param aKey The encryption/decryption key.
		@param aCryptoMode The Symmetric cipher mode.
		@param aOperationMode The Symmetric cipher operation mode.
		@param aPaddingMode The Symmetric cipher padding mode.
		@param aAlgorithmParams Parameters that are specific to a particular algorithm. This is for extendibility and will normally be null.
		@leave	KErrNone if successful; otherwise, a system wide error code.
		*/
		virtual void CreateAsyncSymmetricCipherL(CAsyncSymmetricCipher*& aCipher,
										TUid aAlgorithmUid,
										const CKey& aKey,
										TUid aCryptoMode,
										TUid aOperationMode,
										TUid aPaddingMode,
										const CCryptoParams* aAlgorithmParams) = 0;

		/**
		Creates a new instance of an asynchronous asymmetric cipher
		
		@param aCipher A reference to a pointer that should be set to point to the new asymmetric cipher object.
		@param aAlgorithmUid The asymmetric cipher algorithm to use (e.g. KRsaCipherUid)
		@param aKey The encryption/decryption key.
		@param aCryptoMode whether to encrypt or decrypt
		@param aPaddingMode The padding mode to use
		@param aAlgorithmParams Parameters that are specific to a particular algorithm. This is for extendibility and will normally be null.
		@leave	KErrNone if successful; otherwise, a system wide error code.
		*/
		virtual void CreateAsyncAsymmetricCipherL(CAsyncAsymmetricCipher*& aCipher,
										TUid aAlgorithmUid,
										const CKey& aKey,
										TUid aCryptoMode,
										TUid aPaddingMode,																						
										const CCryptoParams* aAlgorithmParams) = 0;
		/**
		Creates a new instance of a asynchronous signer.

		@param aSigner A reference to a pointer that should be set to point to the new signer object.
		@param aAlgorithmUid The algorithm to use.
		@param aKey The signing key.
		@param aPaddingMode The padding mode of the signer.
		@param aAlgorithmParams Parameters that are specific to a particular algorithm. This is for extendibility and will normally be null.
		@leave	KErrNone if successful; otherwise, a system wide error code.
		*/
		virtual void CreateAsyncSignerL(CAsyncSigner*& aSigner,
										TUid aAlgorithmUid,
										const CKey& aKey,
										TUid aPaddingMode,
										const CCryptoParams* aAlgorithmParams) = 0;
		/**
		Creates a new instance of a asynchronous verifier.
		
		@param aVerifier A reference to a pointer that should be set to point to the new verifier object.
		@param aAlgorithmUid The algorithm to use
		@param aKey The key to verify the signature with.
		@param aPaddingMode The padding mode of the signer.
		@param aAlgorithmParams Parameters that are specific to a particular algorithm. This is for extendibility and will normally be null.
		@leave	KErrNone if successful; otherwise, a system wide error code.
		*/		
		virtual void CreateAsyncVerifierL(CAsyncVerifier*& aVerifier,
										TUid aAlgorithmUid,
										const CKey& aKey,
										TUid aPaddingMode,
										const CCryptoParams* aAlgorithmParams) = 0;

		/**
		Creates a new instance of a asynchronous key pair generator.

		@param aKeyPairGenerator A reference to a pointer that should be set to point to the new asymmetric key pair generator object.
		@param aKeyAlgorithmUid	The algorithm UID
		@param aAlgorithmParams	The parameters that are specific to a particular algorithm. This is for extendibility and will normally be null.
		@leave	KErrNone if successful; otherwise, a system wide error code.
		*/
		virtual void CreateAsyncKeyPairGeneratorL(CAsyncKeyPairGenerator*& aKeyPairGenerator,
										TUid aAlgorithmUid,
										const CCryptoParams* aAlgorithmParams) = 0;

		/**
		Creates a new instance of a asynchronous key agreement system.

		@param aKeyAgreement A reference to a pointer that should be set to point to the new key agreement object.
		@param aAlgorithmUid The algorithm to use
		@param aPrivateKey The private key to combine with the other parties public key during the agreement.
		@param aAlgorithmParams The parameters that are specific to a particular algorithm. This is for extendibility and will normally be null.
		@leave	KErrNone if successful; otherwise, a system wide error code.
		*/
		virtual void CreateAsyncKeyAgreementL(CAsyncKeyAgreement*& aKeyAgreement,
										TUid aAlgorithmUid,
										const CKey& aPrivateKey,
										const CCryptoParams* aAlgorithmParams) = 0;		

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT		

		/**
		 * Create a CMac instance (for software based MAC plug-in dll implementation)
		 *
		 * @param aMac           	The pointer to CMac. This will be initialised with 
		 * 						  	the plug-in implementation of the desired MAC algorithm.
		 * @param aAlgorithmUid  	The specific MAC algorithm desired for evaluation of MAC value.
		 *                       	e.g. MD2, SHA1 or AES-XCBC-MAC-96, AES-XCBC-PRF-128
		 * @param aKey           	Symmetric key for calculating message authentication code value. 
		 * @param aAlgorithmParams  The parameters those are specific to a particular MAC algorithm.
		 * 							This is for extendibility and will normally be null.                     	
		 * @leave	              	KErrNone if successful; otherwise, leaves with a system wide error code.
		 */
		virtual void CreateMacL(CMac*& aMac,
								const TUid aAlgorithmUid,
								const CKey& aKey,
								const CCryptoParams* aAlgorithmParams) = 0;
				
		/**
		 * Create a CAsyncMac instance (for hardware based MAC plug-in dll implementation)
		 *
		 * @param aMac           	The pointer to CMac. This will be initialised with 
		 * 						  	the plug-in implementation of the desired MAC algorithm.
		 * @param aAlgorithmUid  	The specific MAC algorithm desired for evaluation of MAC value.
		 *                       	e.g. MD2, SHA1 or AES-XCBC-MAC-96, AES-XCBC-PRF-128
		 * @param aKey           	Symmetric key for calculating message authentication code value. 
		 * @param aAlgorithmParams  The parameters those are specific to a particular MAC algorithm.
		 * 							This is for extendibility and will normally be null.                     	
		 * @leave	             	KErrNone if successful; otherwise, leaves with a system wide error code.
		 */
		virtual void CreateAsyncMacL(CAsyncMac*& aMac,
								const TUid aAlgorithmUid,
								const CKey& aKey,
								const CCryptoParams* aAlgorithmParams) = 0;
		
		/**
		 * Create a CHash instance
		 * 
		 * @param 	aHash The pointer to CHash
		 * @param 	aAlgorithmUid The specific hash algorithm e.g. MD2, SHA1, MD4
		 * @param 	aAlgorithmParams The parameters that are specific to a particular 
		 * 			algorithm. This is for extendibility and will normally be null.
		 * @leave 	KErrNone if successful; otherwise, a system wide error code.
		 */
		virtual void CreateHashL(CHash*& aHash,
								TUid aAlgorithmUid,
								const CCryptoParams* aAlgorithmParams) = 0;

		/**
		 * Create a CAsyncHash instance
		 * 	
		 * @param 	aAsyncHash The pointer to CAsyncHash
		 * @param 	aAlgorithmUid The specific hash algorithm e.g. MD2, SHA1, MD4
		 * @param 	aAlgorithmParams The parameters that are specific to a particular 
		 * 			algorithm. This is for extendibility and will normally be null.
		 * @leave 	KErrNone if successful; otherwise, a system wide error code.
		 */
		virtual void CreateAsyncHashL(CAsyncHash*& aAsyncHash, 
								TUid aAlgorithmUid,
								const CCryptoParams* aAlgorithmParams) = 0;
		
#endif		
		};
			
	MPluginSelector::~MPluginSelector()
		{
		}
	}
#endif //__CRYPTOAPI_PLUGINSELECTORBASE_H__


