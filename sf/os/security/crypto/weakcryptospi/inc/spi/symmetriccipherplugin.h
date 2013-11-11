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
* symmetric cipher abstract interface
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __CRYPTOAPI_SYMMETRICCIPHER_H__
#define __CRYPTOAPI_SYMMETRICCIPHER_H__

#include <cryptospi/cryptoplugin.h>

namespace CryptoSpi
	{
	class CKey;
	/**
	The Symmetric Cipher Base definition. Intended to allow plug-ins
	to implement extensible symmetric cipher functionality, and to work with all
	known existing symmetric algorithms, e.g. DES, 3DES, ADES, RC2, ARC4 etc
	*/
	class MSymmetricCipherBase : public MPlugin
		{
	public:

		/**
		Set the key of this cipher. Reset() is called to reinitialise the cipher.
		@param aKey	The symmetric key.
		@leave KErrArgument if aKey is not of the expected type.
		@leave KErrNotSupported if the key is not of valid length.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual void SetKeyL(const CKey& aKey) = 0;

		/**
		Set the operation mode of this cipher
		@param aOperationMode	The operation mode e.g. CBC, ECB etc
		@leave KErrNotSupported if the operation mode is not supported.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual void SetOperationModeL(TUid aOperationMode) = 0;

		/**
		Set the crypto mode of this cipher. Reset() is called to reinitialise the cipher.
		@param aCryptoMode	The crypto mode e.g encryption, decryption
		@leave KErrNotSupported if the crypto mode is not supported.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual void SetCryptoModeL(TUid aCryptoMode) = 0;

		/**
		Set padding Mode of this cipher. Reset() is called to reinitialise the cipher.
		@param aPaddingMode	The padding mode e.g. SSLv3, PKCS7
		@leave KErrNotSupported if the padding mode is not supported.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual void SetPaddingModeL(TUid aPaddingMode) = 0;

		/**
		Set the initialization vector of this cipher. Reset() is called to reinitialise the cipher.
		@param aIv	The initialization vector.
		@leave KErrNotSupported if the current mode of operation does not support this.
		@leave KErrArgument If the length of the Iv is not equal to the block size.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual void SetIvL(const TDesC8& aIv) = 0;

		/**
		Returns the maximum length that an output buffer would need to be in order to hold the result
		of the next process operation, given the input length aInputLength and the internal
		state of the state of the cipher.
		@param aInputLength	The length of the input to process
		@return The length of the output buffer
		*/
		virtual TInt MaxOutputLength(TInt aInputLength) const = 0;

		/**
		Returns the maximum length that an output buffer would need to be in order to hold the result
		of the final processing operation, given the input length aInputLength and the
		internal state of the cipher.
		@param aInputLength	The length of input to process
		@return The length of the output buffer
		*/
		virtual TInt MaxFinalOutputLength(TInt aInputLength) const = 0;

		/**
		Returns the block size in bits. For stream ciphers (e.g. ARC4) the block size
		is defined to be 8 bits.
		@return The block size in bits
		*/
		virtual TInt BlockSize() const = 0;

		/**
		Returns the size of the current key in bits.
		@return The size of the current key in bits
		*/
		virtual TInt KeySize() const = 0;
		};


	class MSymmetricCipher : public MSymmetricCipherBase
		{
	public:
		/**
		Encrypts or decrypts aInput and appends the result to aOutput.
		@param aInput	The input data to be processed.
		@param aOutput	The resulting processed data appended to aOutput.	
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.		
		*/
		virtual void ProcessL(const TDesC8& aInput, TDes8& aOutput) = 0;

		/**
		Pads aInput to be block aligned using the underlying padding system, then
		encrypts or decrypts the input data, and appends the result to aOutput
		@param aInput  The input buffer to be encrypted or decrypted.
		@param aOutput The resulting, padded, processed data is appended to aOutput.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput) = 0;
		};


	class MAsyncSymmetricCipher : public MSymmetricCipherBase
		{
	public:

		/**
		Encrypts or decrypts aInput and appends the result to aOutput asynchronously
		@param aInput	The input data to be processed.
		@param aOutput	The resulting processed data appended to aOutput.		
		@param aRequestStatus
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual void ProcessL(const TDesC8& aInput, TDes8& aOutput, TRequestStatus& aRequestStatus) = 0;

		/**
		Asynchronously Pads aInput to be block aligned using the underlying padding system, the
		encrypts or decrypts the input data, and appends the result to aOutput
		@param aInput  The input buffer to be encrypted or decrypted.
		@param aOutput The resulting, padded, processed data is appended to aOutput.
		@param aRequestStatus
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput, TRequestStatus& aRequestStatus) = 0;

		/**
		Cancel the outstanding request
		*/
		virtual void Cancel() = 0;
		};

	} // namespace CryptoSpi

#endif //__CRYPTOAPI_SYMMETRICCIPHER_H__
