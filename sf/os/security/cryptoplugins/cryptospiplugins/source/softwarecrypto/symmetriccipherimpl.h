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
*
*/


#ifndef	__SYMMETRICCIPHERIMPL_H__
#define	__SYMMETRICCIPHERIMPL_H__

/**
@file 
@internalComponent
@released
*/

#include <e32base.h>
#include <e32cmn.h>
#include <cryptospi/cryptospidef.h>
#include <padding.h>
#include "symmetriccipherplugin.h"

/** The maximum block size supported (in bytes) */
const TUint KMaxBlockSizeSupported = 32;

/**
Abstract base class for symmetric cipher plug-ins.
*/
namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
		
	NONSHARABLE_CLASS(CSymmetricCipherImpl) : public CBase, public MSymmetricCipher
		{
	public:
		/**
		Implemented by each cipher subclass to determine whether the
		specified key length is valid for that cipher.
		This is called by ConstructL and SetKeyL
		@param aKeyLength The key length in bytes to verify.
		*/
		virtual TBool IsValidKeyLength(TInt aKeyBytes) const = 0;
		
		/**
		Helper function implemented by concrete cipher sub-class that 
		allows GetCharacteristicsL to return the correct characteristics object.
		@return The implemention uid
		*/
		virtual TUid ImplementationUid() const = 0;
		
		/**
		Gets the strength of the current key, needed to check whether the cipher
		may operate if strong cryptography is not enabled.
		@return The strength of the current key
		*/
		virtual TInt GetKeyStrength() const;
		
				
		// Override MPlugin virtual functions
		void Close();
		TAny* GetExtension(TUid aExtensionId);
		void GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics);
		// End of MPlugin
		
		// Override MSymmetricCipherBase virtual functions 
		TInt KeySize() const;
						
		/// Destructor
		~CSymmetricCipherImpl();

	protected:
		
		//Constructor
		CSymmetricCipherImpl();
		
		/**
		Second phase of construction. Always call ConstructL in the super-class
		if your override this method.
		
		@param aKey The key to initialise the cipher with.
		*/
		virtual void ConstructL(const CKey& aKey);		
		
		/**
		Extracts the raw symmetric key from a generic key object. The buffer
		is placed on the cleanup stack.
		
		@param aKey The key object
		@return A buffer containing the raw key value
		*/
		HBufC8* ExtractKeyDataLC(const CKey& aKey) const;	
	
		/**
		Zeros a buffer before deleting it to ensure that
		the contents will not be visible to another process if the page
		is re-used.
		@param aBuffer The pointer (possibly null) to the buffer to delete. This
		is set to null after deletion.
		*/
		void SecureDelete(HBufC8*& aBuffer);		
					
		/**
		Extracts the raw key from aKey and sets iKey and iKeyBytes
		The key length is also checked to meet export restrictions and
		to ensure that it is appropriate for the cipher.
		@param aKey The key
		*/
		virtual void DoSetKeyL(const CKey& aKey);
		
			
	protected:
		/// the key, extracted from a CKey object
		HBufC8* iKey;
		
		/// key size in bytes
		TUint iKeyBytes;
		
		};

	NONSHARABLE_CLASS(CSymmetricStreamCipherImpl) : public CSymmetricCipherImpl
		{
	public:
		// Destructor
		~CSymmetricStreamCipherImpl();
		
		// Override MSymmetricCipherBase virtual functions 
		TInt BlockSize() const;
		void SetKeyL(const CKey& aKey);		// override DoSetKeyL instead
		void SetCryptoModeL(TUid aCryptoMode);
		void SetOperationModeL(TUid aOperationMode);
		void SetPaddingModeL(TUid aPaddingMode);
		void SetIvL(const TDesC8& aIv);
		TInt MaxOutputLength(TInt aInputLength) const;
		TInt MaxFinalOutputLength(TInt aInputLength) const;
		// End of MSymmetricCipherBase
		
		// Override MSymmetricCipher virtual functions
		void ProcessL(const TDesC8& aInput, TDes8& aOutput);
		void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput);						
		// End of MSymmetricCipher 
	
	protected:
		// Constructor
		CSymmetricStreamCipherImpl();
		
		// Override CSymmetricCipherImpl virtual functions
		virtual void ConstructL(const CKey& aKey);

		/**	
		Performs an encryption or decryption on supplied data.
		@param aData	On input, data to be transformed; 
						on return, transformed data.
		*/
		virtual void DoProcess(TDes8& aData) = 0;
		};

	NONSHARABLE_CLASS(CSymmetricBlockCipherImpl) : public CSymmetricCipherImpl
		{
	public:	


		/**
		This function is invoked by SetKey and SetCryptoMode
		allowing the cipher sub-class to rebuild it's key schedule. 
		N.B. It is assumed that the key schedule is NOT modified
		by TransformEncrypt or TransformDecrypt
		*/
		virtual void SetKeySchedule() = 0;
		
		// Override MPlugin virtual functions		
		void Reset(); // Always call reset in super-class if you override this
		// End of MPlugin virtual functions

		// Override MSymmetricCipherBase virtual functions 
		TInt BlockSize() const;
		void SetKeyL(const CKey& aKey);  				// override DoSetKeyL instead
		void SetCryptoModeL(TUid aCryptoMode);			// override DoSetCryptoModeL instead
		void SetOperationModeL(TUid aOperationMode);	// override DoSetOperationMode instead		
		void SetPaddingModeL(TUid aPaddingMode);		// override DoSetPaddingModeL instead
		void SetIvL(const TDesC8& aIv);
		
		TInt MaxOutputLength(TInt aInputLength) const;
		TInt MaxFinalOutputLength(TInt aInputLength) const;
		// End of MSymmetricCipherBase

		// Override MSymmetricCipher virtual functions
		void ProcessL(const TDesC8& aInput, TDes8& aOutput);
		void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput);						
		// End of MSymmetricCipher

		/// Destructor
		~CSymmetricBlockCipherImpl();		
	protected:	
		/**
		Constructor
		@param aBlockBytes The block size in bytes
		@param aOperationMode The mode of operation e.g. CBC
		@param aCryptoMode Whether to encrypt or decrypt
		*/
		CSymmetricBlockCipherImpl(
			TUint8 aBlockBytes,
			TUid aOperationMode,
			TUid aCryptoMode,
			TUid aPaddingMode);
			
		// Override CSymmetricCipherImpl virtual functions
		virtual void ConstructL(const CKey& aKey);

		/**
		Validates and sets the crypto mode (iCryptoMode)
		@param aCryptoMode The crypto mode
		*/	
		virtual void DoSetCryptoModeL(TUid aCryptoMode);
		
		/**
		Validates and sets the operation mode (iOperationMode)
		@param aOperationMode The operation mode
		*/
		virtual void DoSetOperationModeL(TUid aOperationMode);
		
		/**
		Validates and sets the padding mode (iPaddingMode & iPadding)
		@param aPadding The desired padding mode
		*/
		virtual void DoSetPaddingModeL(TUid aPadding);
		
		void DoSetIvL(const TDesC8& aIv);

		inline void ModeEncryptStart(TUint8* aBuffer);
		inline void ModeEncryptEnd(TUint8* aBuffer);
		inline void ModeDecryptStart(TUint8* aBuffer);
		inline void ModeDecryptEnd(TUint8* aBuffer);

	private:
	
		/**
		Encrypts a number of blocks of data
		
		@param aBuffer The buffer containing exactly aNumBlocks of data to destructively encrypt
		@param aNumBlocks The number of blocks of data to encrypt
		*/
		virtual void TransformEncrypt(TUint8* aBuffer, TUint aNumBlocks) = 0;
		
		/**
		Decrypts a number of blocks of data
				
		@param aBuffer The buffer containing exactly aNumBlocks of data to destructively decrypt
		@param aNumBlocks The number of blocks of data to decrypt
		*/
		virtual void TransformDecrypt(TUint8* aBuffer, TUint aNumBlocks) = 0;		
			
		/// Pad the last block and encrypt
		void DoProcessFinalEncryptL(const TDesC8& aInput, TDes8& aOutput);
		
		/// Decrypt and unpad the last block
		void DoProcessFinalDecryptL(const TDesC8& aInput, TDes8& aOutput);		
		
		inline void Transform(TUint8* aBuffer, TUint aNumBlocks);

		void ProcessCtrL(const TDesC8& aInput, TDes8& aOutput);
		
	protected:
	
		/// block size in bytes, current largest block size is 16 bytes (AES)
		TUint8 iBlockBytes;	
		/// encryption or decryption
		TUid iCryptoMode;		
		/// The block cipher mode e.g. ECB, CBC
		TUid iOperationMode;
		/// the current padding scheme
		TUid iPaddingMode;
		
		/// the initialisation vector
		RBuf8 iIv;
		
		/// current padding scheme implementation
		CPadding* iPadding;
		/// buffer to store blocks
		RBuf8 iInputStore;
		/// buffer to store input / output of padding
		RBuf8 iPaddingBlock;

		/// The current block of cipher text - for CBC 
		TUint32* iCurrentCipherText;	
		/// A pointer to the current block of cipher text
		TUint8* iCurrentCipherTextPtr;		
		
		/** Used in both CBC and CTR mode. In CBC mode it stores the result of the last transform. In CTR mode 
		it stores the counter.*/
		TUint32* iRegister;	
		/** A pointer to iRegister.*/
		TUint8* iRegisterPtr;			
		
		/** Used in CTR mode to buffer plaintext during encryption.*/
		HBufC8* iBufferedPlaintext;
		/** Pointer to manipulate iBufferedPlaintext.*/
		TPtr8 iBufferedPlaintextPtr;
		
		/** CTR mode behaves like a stream cipher allowing arbitrary sized inputs to the encryption/decryption functions. 
		When handling an input whose length is not a multiple of the blocksize iCtrUnusedKeystream is used to buffer
		the unused portions of keystream for use in the next call. Cleared in Reset().*/
		HBufC8* iCtrUnusedKeystream;
		/** Pointer to manipulate iCtrUnusedKeystream.*/
		TPtr8 iCtrUnusedKeystreamPtr;
		};


	inline void CSymmetricBlockCipherImpl::Transform(TUint8* aBuffer, TUint aNumBlocks)
		{				
		if (iCryptoMode.iUid == KCryptoModeEncrypt)	//if in CTR mode always in crypto mode encrypt
			{				
			TransformEncrypt(aBuffer, aNumBlocks);
			}
		else if (iCryptoMode.iUid == KCryptoModeDecrypt)
			{				
			TransformDecrypt(aBuffer, aNumBlocks);
			}
		else 
			{
			ASSERT(EFalse);
			}
		}
			
	inline void CSymmetricBlockCipherImpl::ModeEncryptStart(TUint8* aBuffer)
		{
		if (iOperationMode.iUid == KOperationModeCBC)
			{			
			for (TInt i = 0; i < iBlockBytes; ++i)
				{
				aBuffer[i] ^= iRegisterPtr[i];
				}					
			}
		else if (iOperationMode.iUid == KOperationModeCTR)
			{
			iBufferedPlaintextPtr.Copy(aBuffer, iBlockBytes);
			Mem::Copy(aBuffer, iRegister, iBlockBytes);				
			}
		}		
	
	inline void CSymmetricBlockCipherImpl::ModeEncryptEnd(TUint8* aBuffer)
		{				
		if (iOperationMode.iUid == KOperationModeCBC)
			{
			for (TInt i = 0; i < iBlockBytes; ++i)
				{
				iRegisterPtr[i] = aBuffer[i]; 
				}													
			}
		else if (iOperationMode.iUid == KOperationModeCTR)
			{
			//XOR the plaintext with the keystream and increment counter
			for (TInt i = 0; i < iBlockBytes; ++i)
				{
				aBuffer[i] ^= iBufferedPlaintextPtr[i]; 
				}
			for (TInt i = iBlockBytes - 1; i >= 0; --i)
				{
				if (++(iRegisterPtr[i]) != 0) break;				
				}
			}										
		}		

	inline void CSymmetricBlockCipherImpl::ModeDecryptStart(TUint8* aBuffer)
		{
		__ASSERT_DEBUG((iOperationMode.iUid != KOperationModeCTR), User::Panic(_L("CSymmetricBlockCipherImpl.h"), 1));
		if (iOperationMode.iUid == KOperationModeCBC)
			{			
			for (TInt i = 0; i < iBlockBytes; ++i)
				{
				iCurrentCipherTextPtr[i] = aBuffer[i];
				}
			}
		}

	inline void CSymmetricBlockCipherImpl::ModeDecryptEnd(TUint8* aBuffer)
		{
		__ASSERT_DEBUG((iOperationMode.iUid != KOperationModeCTR), User::Panic(_L("CSymmetricBlockCipherImpl.h"), 2));		
		if (iOperationMode.iUid == KOperationModeCBC)
			{			
			// xor the output with the previous cipher text
			for (TInt i = 0; i < iBlockBytes; ++i)
				{
				aBuffer[i] ^= iRegisterPtr[i];
				iRegisterPtr[i] = iCurrentCipherTextPtr[i];
				}
			}	
		}		
	}						

#endif	//	__SYMMETRICCIPHERIMPL_H__
