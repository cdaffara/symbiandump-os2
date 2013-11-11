/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @internalComponent
 @released
*/
#ifndef	__RIJNDAELIMPL_H__
#define	__RIJNDAELIMPL_H__
#include <e32base.h>
#include <e32cmn.h>
#include "keys.h"
#include "h4cipherimpl.h"
#include "cryptodriver.h"

/**
Plug-in class for AES (Rijndael) block cipher
*/
namespace HwCrypto
	{
	class __NOTSHARED CH4RijndaelImpl : public CH4CipherImpl
		{
	public:
	
		/**
		Creates an instance of an AES (Rijndael) symmetric cipher plug-in.
		@param aKey The key
		@param aCryptoMode Whether to encrypt or decrypt
		@param aOperationMode The block cipher mode ECB, CBC, CTR etc
		@param aPadding The padding scheme to use None, SSLv3, PKCS#7
		@return A pointer to a CH4RijndaelImpl instance
		*/
		static CH4RijndaelImpl* NewL(const CryptoSpi::CKey& aKey, 
			TUid aCryptoMode, TUid aOperationMode, TUid aPadding);

		/**
		Creates an instance of an AES (Rijndael) symmetric cipher plug-in.
		A pointer to the plug-in instance is placed on the cleanup stack.
		@param aKey The key
		@param aCryptoMode Whether to encrypt or decrypt
		@param aOperationMode The block cipher mode ECB, CBC, CTR etc
		@param aPadding The padding scheme to use None, SSLv3, PKCS#7
		@return A pointer to a CH4RijndaelImpl instance
		*/
		static CH4RijndaelImpl* NewLC(const CryptoSpi::CKey& aKey, 
			TUid aCryptoMode,	TUid aOperationMode, TUid aPadding);

		// From CSymmetricCipherImpl
		TBool IsValidKeyLength(TInt aKeyBytes) const;
		TUid ImplementationUid() const;

		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		
		static CExtendedCharacteristics* StaticGetExtendedCharacteristicsL();
		
		/// Destructor
		~CH4RijndaelImpl();

	private:
		/**
		Constructor
		@param aOperationMode The mode of operation e.g. CBC
		@param aCryptoMode Whether to encrypt or decrypt
		@param aPaddingMode The padding mode to use. None, SSL, PKCS#7
		*/
		CH4RijndaelImpl(TUid aOperationMode, TUid aCryptoMode, TUid aPaddingMode);
			
		///	second phase of construction	
		void ConstructL(const CryptoSpi::CKey& aKey);
					
		// From CH4CipherImpl
		virtual void DoSetupL();
		virtual void DoWriteL(const TUint8* aBuffer, TUint aNumBytes);
		virtual void DoReadL(TDes8 &aBuffer, TUint32 aLength);
				
	private:
		RCryptoDriver iDriver;
		TBool iDriverOpen;
		};
	}

#endif //__RIJNDAELIMPL_H__
