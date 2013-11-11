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


#ifndef	__3DESIMPL_H__
#define	__3DESIMPL_H__

/**
@file 
@internalComponent
@released
*/

#include <e32base.h>
#include <e32cmn.h>
#include "keys.h"
#include "desimpl.h"

/**
Plug-in class for 3DES block cipher
*/
namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	
	NONSHARABLE_CLASS(C3DesImpl) : public CDesImpl
		{
	public:
		/**
		Creates an instance of a 3DES (Data Encryption Standard) symmetric cipher plug-in.
		@param aKey The key
		@param aCryptoMode Whether to encrypt or decrypt
		@param aOperationMode The block cipher mode ECB, CBC, CTR etc
		@param aPadding The padding scheme to use None, SSLv3, PKCS#7
		@return A pointer to a C3DesImpl instance
		*/
		static C3DesImpl* NewL(const CKey& aKey, 
			TUid aCryptoMode, TUid aOperationMode, TUid aPadding);

		/**
		Creates an instance of a 3DES (Data Encryption Standard) symmetric cipher plug-in.
		A pointer to the plug-in instance is placed on the cleanup stack.
		@param aKey The key
		@param aCryptoMode Whether to encrypt or decrypt
		@param aOperationMode The block cipher mode ECB, CBC, CTR etc
		@param aPadding The padding scheme to use None, SSLv3, PKCS#7
		@return A pointer to a C3DesImpl instance
		*/
		static C3DesImpl* NewLC(const CKey& aKey, 
			TUid aCryptoMode, TUid aOperationMode, TUid aPadding);
		
		// From CSymmetricCipherImpl
		TBool IsValidKeyLength(TInt aKeyBytes) const;
		TInt GetKeyStrength() const;
		TUid ImplementationUid() const;
		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		
		static CExtendedCharacteristics* CreateExtendedCharacteristicsL();
				
		/// 3Destructor
		~C3DesImpl();

	private:
		/**
		Constructor
		@param aOperationMode The mode of operation e.g. CBC
		@param aCryptoMode Whether to encrypt or decrypt
		@param aPaddingMode The padding mode to use. None, SSL, PKCS#7
		*/
		C3DesImpl(TUid aOperationMode, TUid aCryptoMode, TUid aPaddingMode);
			
		///	second phase of construction	
		void ConstructL(const CKey& aKey);						

		/**
		Creates the key schedules iK1, iK2, iK3 from CBlockTransformationImpl::iKey.
		This should be called from ConstructL and Reset
		*/
		void SetKeySchedule();

		// From CSymmetricBlockCipherImpl
		void TransformEncrypt(TUint8* aBuffer, TUint aNumBlocks);
		void TransformDecrypt(TUint8* aBuffer, TUint aNumBlocks);
		
	private:
		/// Size of the key schedule in words
		static const TUint KKeyScheduleSize = 32;
		
		/// the key schedules
		TUint32 iK1[KKeyScheduleSize];
		TUint32 iK2[KKeyScheduleSize];
		TUint32 iK3[KKeyScheduleSize];
		
		static const TUint8 K3DesBlockBytes = 8;
		static const TUint8 K3DesKeyBytes = 24;
		};
	}

#endif // __3DESIMPL_H__
