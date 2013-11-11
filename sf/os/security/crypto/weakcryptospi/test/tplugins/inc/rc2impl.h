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


#ifndef	__RC2_H__
#define	__RC2_H__

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
Plug-in class for RC2 block cipher
*/
namespace SoftwareCrypto
	{
	NONSHARABLE_CLASS(CRc2Impl) : public CSymmetricBlockCipherImpl
		{
	public:
	
		/**
		Creates an instance of an RC2 symmetric cipher plug-in.
		@param aKey The key
		@param aCryptoMode Whether to encrypt or decrypt
		@param aOperationMode The block cipher mode ECB, CBC, CTR etc
		@param aPadding The padding scheme to use None, SSLv3, PKCS#7
		@param aEffectiveKeyLenBits The effective key length in bits
		@return A pointer to a CRc2Impl instance
		*/
		static CRc2Impl* NewL(const CryptoSpi::CKey& aKey, 
			TUid aCryptoMode, TUid aOperationMode, TUid aPadding, TInt aEffectiveKeyLenBits);

		/**
		Creates an instance of an RC2 symmetric cipher plug-in.
		A pointer to the plug-in instance is placed on the cleanup stack.
		@param aKey The key
		@param aCryptoMode Whether to encrypt or decrypt
		@param aOperationMode The block cipher mode ECB, CBC, CTR etc
		@param aPadding The padding scheme to use None, SSLv3, PKCS#7
		@param aEffectiveKeyLenBits The effective key length in bits
		@return A pointer to a CRc2Impl instance
		*/
		static CRc2Impl* NewLC(const CryptoSpi::CKey& aKey, 
			TUid aCryptoMode, TUid aOperationMode, TUid aPadding, TInt aEffectiveKeyLenBits);
		
		// From CSymmetricCipherImpl
		TBool IsValidKeyLength(TInt aKeyBytes) const;
		TUid ImplementationUid() const;
		TInt GetKeyStrength() const;
				
		/// Destructor
		~CRc2Impl();
		
		/** SSL Effective Key Length Compatibility - for compatibility with SSL */
		static const TUint KDefaultEffectiveKeyLenBits = 1024;

	private:
		/**
		Constructor
		@param aOperationMode The mode of operation e.g. CBC
		@param aCryptoMode Whether to encrypt or decrypt
		@param aPaddingMode The padding mode to use. None, SSL, PKCS#7
		*/
		CRc2Impl(TUid aOperationMode, TUid aCryptoMode, TUid aPaddingMode);
			
		///	second phase of construction	
		void ConstructL(const CKey& aKey, TInt aEffectiveKeyLenBits);
					
		/**
		Expands the key (iKey) to iEffectiveKeyLenBits and stores the result in iK
		*/
		void SetKeySchedule();
		
		// From CSymmetricBlockCipherImpl
		void TransformEncrypt(TUint8* aBuffer, TUint aNumBlocks);
		void TransformDecrypt(TUint8* aBuffer, TUint aNumBlocks);		

	private:		
		static const TUint8 KRc2BlockBytes = 8;
		static const TInt KRc2MaxKeySizeBytes = 128;	
		static const TInt KRc2ExpandedKeyLen = 64;
		/**
	 	 The expanded key buffer.
		 Each iK[i] is a 16-bit word.
	 	 */
		TUint16 iK[KRc2ExpandedKeyLen];	//	128 bytes		
		
		/** The effective key length in bits */
		TInt iEffectiveKeyLenBits;	
		};
	}

#endif //__RC2_H__
