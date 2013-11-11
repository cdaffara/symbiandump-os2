/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef	__DESIMPL_H__
#define	__DESIMPL_H__

/**
@file 
@internalComponent
@released
*/

#include <e32base.h>
#include <e32cmn.h>
#include <cryptospi/keys.h>
#include "symmetriccipherimpl.h"

/**
Plug-in class for DES block cipher
*/
namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	
	NONSHARABLE_CLASS(CDesImpl) : public CSymmetricBlockCipherImpl
		{
	public:
	
		/**
		Creates an instance of a DES (Data Encryption Standard) symmetric cipher plug-in.
		@param aKey The key
		@param aCryptoMode whether to encrypt or decrypt
		@param aOperationMode The block cipher mode ECB, CBC, CTR etc
		@param aPadding The padding scheme to use None, SSLv3, PKCS#7
		@return A pointer to a CDesImpl instance
		*/
		static CDesImpl* NewL(TUid aImplementationUid, const CKey& aKey, 
			TUid aCryptoMode, TUid aOperationMode, TUid aPadding);

		/**
		Creates an instance of a DES (Data Encryption Standard) symmetric cipher plug-in.
		A pointer to the plug-in instance is placed on the cleanup stack.
		@param aKey The key
		@param aCryptoMode Whether to encrypt or decrypt
		@param aOperationMode T block cipher mode ECB, CBC, CTR etc
		@param aPadding The padding scheme to use None, SSLv3, PKCS#7
		@return A pointer to a CDesImpl instance
		*/
		static CDesImpl* NewLC(TUid aImplementationUid, const CKey& aKey, 
			TUid aCryptoMode,	TUid aOperationMode, TUid aPadding);
		
		// From CSymmetricCipherImpl
		TInt GetKeyStrength() const;
		TBool IsValidKeyLength(TInt aKeyBytes) const;
		TUid ImplementationUid() const;
		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		
		static CExtendedCharacteristics* CreateExtendedCharacteristicsL();

				
		/// Destructor
		~CDesImpl();		
	protected:
		/**
		Constructor
		@param aBlockBytes The block size in bytes (needed to allow
		3DES implementation to inherit from this class)
		@param aOperationMode The mode of operation e.g. CBC
		@param aCryptoMode Whether to encrypt or decrypt
		@param aPaddingMode The padding mode to use. None, SSL, PKCS#7
		*/
		CDesImpl(TUid aImplementationUid, TUint8 aBlockBytes, TUid aOperationMode, TUid aCryptoMode, TUid aPaddingMode);
		
		/**	
		Second phase of construction
		@param aKey The key object
		*/
		void ConstructL(const CKey& aKey);	
			
		// These protected functions are re-used by 3DES
		/**
		Transforms a block of data
		@param l The left half of the block to transform
		@param r The right half of the block to transform
		@param aKeySchedule The keyschedule to use for the transformation
		*/
		void DoTransform(TUint32& l, TUint32& r, const TUint32* aKeySchedule);
		
		/**
		Setup the key schedule for encryption
		@param aKey The input key
		@param aKeySchedule A pointer to the key schedule buffer
		*/
		void SetEncryptKeySchedule(const TDesC8& aKey, TUint32* aKeySchedule);
		
		/**
		Setup the key schedule for decryption
		@param aKey The input key
		@param aKeySchedule A pointer to the key schedule buffer		
		*/
		void SetDecryptKeySchedule(const TDesC8& aKey, TUint32* aKeySchedule);
		
		static const TUint8 KDesBlockBytes = 8;
		static const TUint8 KDesKeyBytes = 8;						

	private:														

		/**
		Creates the key schedule iK from CBlockTransformationImpl::iKey.
		This should be called from ConstructL and Reset
		*/
		void SetKeySchedule();			

		// From CSymmetricBlockCipherImpl
		void TransformEncrypt(TUint8* aBuffer, TUint aNumBlocks);
		void TransformDecrypt(TUint8* aBuffer, TUint aNumBlocks);
					
	private:
		
		TUid iImplementationUid; 
	
		/// Size of the key schedule in words
		static const TUint KKeyScheduleSize = 32;
		
		/// the key schedule
		TUint32 iK[KKeyScheduleSize];
		};
	}

#endif // __DESIMPL_H__
