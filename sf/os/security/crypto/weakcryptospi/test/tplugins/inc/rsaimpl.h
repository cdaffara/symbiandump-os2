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


#ifndef __RSAIMPL_H__
#define __RSAIMPL_H__

/**
@file 
@internalComponent
@released
*/

#include <e32base.h>
#include <e32cmn.h>
#include <cryptospi/keys.h>
#include "asymmetriccipherimpl.h"

/**
 * Implementation of RSA encryption as described in PKCS#1 v1.5.
 */
namespace SoftwareCrypto
	{
	using namespace CryptoSpi;

	NONSHARABLE_CLASS(CRSAImpl) : public CAsymmetricCipherImpl
		{
	public:
	
		/**
		Creates an instance of an RSA asymmetric cipher plug-in.
		@param aKey The key
		@param aCryptoMode Whether to encrypt or decrypt
		@param aPadding The padding scheme to use None, SSLv3, PKCS#7
		@return A pointer to a CRSAImpl instance
		*/
		static CRSAImpl* NewL(TUid aImplementationUid, const CKey& aKey,
			TUid aCryptoMode, TUid aPadding);

		/**
		Creates an instance of an RSA asymmetric cipher plug-in.
		A pointer to the plug-in instance is placed on the cleanup stack.
		@param aKey The key
		@param aCryptoMode Whether to encrypt or decrypt
		@param aPadding The padding scheme to use None, SSLv3, PKCS#7
		@return A pointer to a CRSAImpl instance
		*/
		static CRSAImpl* NewLC(TUid aImplementationUid, const CKey& aKey,
			TUid aCryptoMode, TUid aPadding);

		// Override CAsymmetricCipherImpl virtual functions
		TUid ImplementationUid() const;
		TBool IsValidKeyLengthL(TInt aKeyBytes) const;
		TInt GetMaximumInputLengthL() const;
		TInt GetMaximumOutputLengthL() const;
		void ProcessL(const TDesC8& aInput, TDes8& aOutput);
		// End of CAsymmetricCipherImpl
		
		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		static CExtendedCharacteristics* CreateExtendedCharacteristicsL();

		// Destructor
		~CRSAImpl();

	private:
		/**
		Constructor
		@param aCryptoMode Whether to encrypt or decrypt
		@param aPaddingMode The padding mode to use. None, SSL, PKCS#7
		*/
		CRSAImpl(TUid aImplementationUid, TUid aCryptoMode, TUid aPaddingMode);
			
		/// second phase of construction
		void ConstructL(const CKey& aKey);
		
		/// for internal usage, called from ProcessL
		void EncryptL(const TDesC8& aInput, TDes8& aOutput) const;
		void DecryptL(const TDesC8& aInput, TDes8& aOutput) const;
		
	private:
	
		TUid iImplementationUid; 

		};
	}

#endif // __RSAIMPL_H__
