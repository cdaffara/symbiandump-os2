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
* crypto symmetric cipher application interface
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __CRYPTOAPI_SYMMETRICCIPHERAPI_H__
#define __CRYPTOAPI_SYMMETRICCIPHERAPI_H__

#include <e32base.h>
#include <cryptospi/cryptobaseapi.h>

namespace CryptoSpi
	{
	class MSymmetricCipherBase;
	class MSymmetricCipher;
	class MAsyncSymmetricCipher;
	class CKey;
	class CCryptoParams;
	
	NONSHARABLE_CLASS(CSymmetricCipherBase) : public CCryptoBase
		{
	public:
		/**
		Destructor
		*/		
		virtual ~CSymmetricCipherBase();
		
		/**
		Set the key of this cipher. Reset() is called to reinitialise the cipher.
		@param aKey	The symmetric key.
		@leave KErrArgument if aKey is not of the expected type.
		@leave KErrNotSupported if the key is not of valid length.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		IMPORT_C void SetKeyL(const CKey& aKey);

		/**
		Set the operation mode of this cipher. Reset() is called to reinitialise the cipher.
		@param aOperationMode	The operation mode e.g. CBC, ECB etc
		@leave KErrNotSupported if the operation mode is not supported.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		IMPORT_C void SetOperationModeL(TUid aOperationMode);

		/**
		Set the crypto mode of this cipher. Reset() is called to reinitialise the cipher.
		@param aCryptoMode	The crypto mode e.g encryption, decryption
		@leave KErrNotSupported if the crypto mode is not supported.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		IMPORT_C void SetCryptoModeL(TUid aCryptoMode);

		/**
		Set padding mode of this cipher. Reset() is called to reinitialise the cipher.
		@param aPaddingMode	The padding mode e.g. SSLv3, PKCS7
		@leave KErrNotSupported if the padding mode is not supported.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		IMPORT_C void SetPaddingModeL(TUid aPaddingMode);

		/**
		Set the initialization vector of this cipher. Reset() is called to reinitialise the cipher.
		@param aIV The initialization vector.
		@leave KErrNotSupported if the current mode of operation does not support this.
		@leave KErrArgument If the length of the Iv is not equal to the block size.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		IMPORT_C void SetIvL(const TDesC8& aIv);

		/**
		Returns the maximum length that an output buffer would need to be in order to hold the result 
		of the next process operation, given the input length inputLen and the internal
		state of the state of the cipher.
		@param aInputLength	The length of the input to process
		@return The length of the output buffer
		*/
		IMPORT_C TInt MaxOutputLength(TInt aInputLength);
		
		/**
		Returns the maximum length that an output buffer would need to be in order to hold the result 
		of the next processfinal operation, given the input length inputLen and the
		internal state of the cipher.
		@param aInputLength	The length of input to process
		@return The length of the output buffer
		*/
		IMPORT_C TInt MaxFinalOutputLength(TInt aInputLength);

		/**
		Returns the block size in bits. For stream ciphers this is defined to be
		8-bits.
		@return The block size in bits
		*/
		IMPORT_C TInt BlockSize();
		
		/**
		Returns the size of the current key in bits.
		@return The size of the current key in bits
		*/
		IMPORT_C TInt KeySize();

	protected:
		/**
		 * @internalComponent
		 *
		 * Constructor
		 **/
		CSymmetricCipherBase(MSymmetricCipherBase* aSymmetricCipher, TInt aHandle);		
		};


	/**
	Symmetric Cipher API, which wraps a synchronous Symmetric Cipher plugin implementation
	*/
	NONSHARABLE_CLASS(CSymmetricCipher) : public CSymmetricCipherBase
		{
	public:

		/**
		 * @internalComponent
		 *
		 * Create a CSymmetricCipher instance from the given MSymmetricCipher instance
		 * @param aSymmetricCipher an Sync Symmetric Cipher plugin instance
		 * @return A pointer to a CSymmetricCipher instance
		 **/
		static CSymmetricCipher* NewL(MSymmetricCipher* aSymmetricCipher, TInt aHandle);

		/**
		Destructor
		*/
		IMPORT_C ~CSymmetricCipher();

		/**
		Encrypts or decrypts aInput and appends the result to aOutput.
		@param aInput	The input data to be processed.
		@param aOutput	The resulting processed data appended to aOutput.		
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		IMPORT_C void ProcessL(const TDesC8& aInput, TDes8& aOutput);

		/**
		Pads aInput to be block aligned using the underlying padding system, then
		encrypts or decrypts the input data, and appends the result to aOutput
		@param aInput  The input buffer to be encrypted or decrypted.
		@param aOutput The resulting, padded, processed data is appended to aOutput.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		IMPORT_C void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput);
		
	private:
		/**
		Constructor
		*/
		CSymmetricCipher(MSymmetricCipher* aSymmetricCipher, TInt aHandle);
		};

	/**
	Async Symmetric Cipher API, which wraps an asynchronous Symmetric Cipher plugin implementation
	*/
	NONSHARABLE_CLASS(CAsyncSymmetricCipher) : public CSymmetricCipherBase
		{
	public:

		/**
		 * @internalComponent
		 *
		 * Create a CAsyncSymmetricCipher instance from the given MAsyncSymmetricCipher instance
		 * @param aAsyncSymmetricCipher an async Symmetric Cipher plugin instance
		 * @return A pointer to a CAsyncSymmetricCipher instance
		 **/
		static CAsyncSymmetricCipher* NewL(MAsyncSymmetricCipher* aAsyncSymmetricCipher, TInt aHandle);

		/**
		Destructor
		*/
		IMPORT_C ~CAsyncSymmetricCipher();

		/**
		Encrypts or decrypts aInput and appends the result to aOutput asynchronously
		@param aInput	The input data to be processed.
		@param aOutput	The resulting processed data appended to aOutput.		
		@param aRequestStatus
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		IMPORT_C void ProcessL(const TDesC8& aInput, TDes8& aOutput, TRequestStatus& aRequestStatus);

		/**
		Asynchronously pads aInput to be block aligned using the underlying padding system,
		encrypts or decrypts the input data, and appends the result to aOutput
		@param aInput  The input buffer to be encrypted or decrypted.
		@param aOutput The resulting, padded, processed data is appended to aOutput.
		@param aRequestStatus
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		IMPORT_C void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput, TRequestStatus& aRequestStatus);

		/**
		Cancel the outstanding request
		*/
		IMPORT_C void Cancel();
		
	private:
		/**
		Construtor
		*/
		CAsyncSymmetricCipher(MAsyncSymmetricCipher* aAsyncSymmetricCipher, TInt aHandle);
		};


	/**
	This Factory to create synchronous and asynchronous symmetric cipher instances
	*/
	class CSymmetricCipherFactory
		{
	public:

		/**
		Creates a new synchronous instance of a symmetric cipher
		
		@param aCipher	A reference to a pointer that should be set to point to the new symmetric object.
		@param aAlgorithmUid The algorithm to use
		@param aKey The encryption/decryption key.
		@param aCryptoMode The Symmetric cipher mode.
		@param aOperationMode The Symmetric cipher operation mode.
		@param aPaddingMode The Symmetric cipher padding mode.
		@param aAlgorithmParams Parameters that are specific to a particular algorithm. This is for extendibility and will normally be null.
		@return KErrNone if successful; otherwise, a system wide error code.
		*/
		IMPORT_C static void CreateSymmetricCipherL(CSymmetricCipher*& aCipher,
													TUid aAlgorithmUid,
													const CKey& aKey,
													TUid aCryptoMode,
													TUid aOperationMode,
													TUid aPaddingMode,
													const CCryptoParams* aAlgorithmParams);

		/**
		Creates a new asynchronous instance of a symmetric cipher

		@param aAsyncCipher A reference to a pointer that should be set to point to the new symmetric object.
		@param aAlgorithmUid The algorithm to use
		@param aKey The encryption/decryption key.
		@param aCryptoMode The Symmetric cipher mode.
		@param aOperationMode The Symmetric cipher operation mode.
		@param aPaddingMode The Symmetric cipher padding mode.
		@param aAlgorithmParams Parameters that are specific to a particular algorithm. This is for extendibility and will normally be null.
		@return KErrNone if successful; otherwise, a system wide error code.
		*/
		IMPORT_C static void CreateAsyncSymmetricCipherL(CAsyncSymmetricCipher*& aAsyncCipher,
														TUid aAlgorithmUid,
														const CKey& aKey,
														TUid aCryptoMode,
														TUid aOperationMode,
														TUid aPaddingMode,
														const CCryptoParams* aAlgorithmParams);

		};
	}

#endif //__CRYPTOAPI_SYMMETRICCIPHERAPI_H__
