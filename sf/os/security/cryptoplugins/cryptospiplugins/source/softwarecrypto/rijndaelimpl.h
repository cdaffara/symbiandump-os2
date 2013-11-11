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


#ifndef	__RIJNDAELIMPL_H__
#define	__RIJNDAELIMPL_H__

/**
@file 
@internalComponent
@released
*/

#include <e32base.h>
#include <e32cmn.h>
#include "keys.h"
#include "symmetriccipherimpl.h"

/**
Plug-in class for AES (Rijndael) block cipher
*/
namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	
	NONSHARABLE_CLASS(CRijndaelImpl) : public CSymmetricBlockCipherImpl
		{
	public:
		/**
		Creates an instance of an AES (Rijndael) symmetric cipher plug-in.
		@param aKey The key
		@param aCryptoMode Whether to encrypt or decrypt
		@param aOperationMode The block cipher mode ECB, CBC, CTR etc
		@param aPadding The padding scheme to use None, SSLv3, PKCS#7
		@return A pointer to a CRijndaelImpl instance
		*/
		static CRijndaelImpl* NewL(const CKey& aKey, 
			TUid aCryptoMode, TUid aOperationMode, TUid aPadding);

		/**
		Creates an instance of an AES (Rijndael) symmetric cipher plug-in.
		A pointer to the plug-in instance is placed on the cleanup stack.
		@param aKey The key
		@param aCryptoMode Whether to encrypt or decrypt
		@param aOperationMode The block cipher mode ECB, CBC, CTR etc
		@param aPadding The padding scheme to use None, SSLv3, PKCS#7
		@return A pointer to a CRijndaelImpl instance
		*/
		static CRijndaelImpl* NewLC(const CKey& aKey, 
			TUid aCryptoMode,	TUid aOperationMode, TUid aPadding);

		// From CSymmetricCipherImpl
		TBool IsValidKeyLength(TInt aKeyBytes) const;
		TUid ImplementationUid() const;
		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		
		static CExtendedCharacteristics* CreateExtendedCharacteristicsL();
				
		/// Destructor
		~CRijndaelImpl();

	private:
		/**
		Constructor
		@param aOperationMode The mode of operation e.g. CBC
		@param aCryptoMode Whether to encrypt or decrypt
		@param aPaddingMode The padding mode to use. None, SSL, PKCS#7
		*/
		CRijndaelImpl(TUid aOperationMode, TUid aCryptoMode, TUid aPaddingMode);
			
		///	second phase of construction	
		void ConstructL(const CKey& aKey);
					
		/**
		Creates the key schedule iK from CBlockTransformationImpl::iKey.
		This should be called from ConstructL and Reset
		*/
		void SetKeySchedule();
		
		/**
		Setup the key schedule for encryption
		@param aKey The input key
		@param aKeySchedule The pointer to the key schedule buffer
		*/
		void SetEncryptKeySchedule(const TDesC8& aKey, TUint32* aKeySchedule);
		
		/**
		Setup the key schedule for decryption
		@param aKey The input key
		@param aKeySchedule The pointer to the key schedule buffer		
		*/
		void SetDecryptKeySchedule(const TDesC8& aKey, TUint32* aKeySchedule);

		// From CSymmetricBlockCipherImpl
		void TransformEncrypt(TUint8* aBuffer, TUint aNumBlocks);
		void TransformDecrypt(TUint8* aBuffer, TUint aNumBlocks);
				
	private:
	
		/** 
		The key-schedule size in WORDS
		
	 	The maximum size is (((KAESMaxBlockSize/4)+6)+1)*4
	 	*/
	 	static const TUint32 KKeyScheduleSize = 60;
	 	
	 	/// the key schedule
		TUint32 iK[KKeyScheduleSize];
		
		TInt iRounds;
		};
	}

#endif //__RIJNDAELIMPL_H__
