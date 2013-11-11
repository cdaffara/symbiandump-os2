/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __ASYMMETRICCIPHERIMPL_H__
#define __ASYMMETRICCIPHERIMPL_H__

/**
@file 
@internalComponent
@released
*/

#include <e32base.h>
#include <cryptospi/cryptospidef.h>
#include <padding.h>
#include <cryptospi/asymmetriccipherplugin.h>
#include <cryptospi/keys.h>
#include "../../../source/common/inlines.h"

/**
 * Abstract base class for symmetric cipher plug-ins.
 */
namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	
	NONSHARABLE_CLASS(CAsymmetricCipherImpl) : public CBase, public MAsymmetricCipher
		{
	public:
		
		// Override MPlugin virtual functions
		void Close();
		void Reset(); // Always call reset in super-class if you override this
		TAny* GetExtension(TUid aExtensionId);
		void GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics);
		// End of MPlugin
		
		// Override MAsymmetricCipherBase virtual functions
		void SetKeyL(const CKey& aKey);             // override DoSetKeyL instead
		void SetCryptoModeL(TUid aCryptoMode);      // override DoSetCryptoModeL instead
		void SetPaddingModeL(TUid aPaddingMode);    // override DoSetPaddingModeL instead
		virtual TInt GetMaximumInputLengthL() const;
		virtual TInt GetMaximumOutputLengthL() const;
		// End of MAsymmetricCipherBase
		
		// Override MAsymmetricCipher virtual functions
		virtual void ProcessL(const TDesC8& aInput, TDes8& aOutput) = 0;
		// End of MAsymmetricCipher
		
		/// Destructor
		~CAsymmetricCipherImpl();
		
	protected:
		/**
		Constructor
		@param aCryptoMode Whether to encrypt or decrypt
		@param aPaddingMode The padding mode
		*/
		CAsymmetricCipherImpl(TUid aCryptoMode, TUid aPaddingMode);

		/**
		Second phase of construction. Always call ConstructL in the super-class
		if you override this method.
		
		@param aKey The key to initialise the cipher with.
		*/
		virtual void ConstructL(const CKey& aKey);
		
		/**
		Implemented by each cipher subclass to determine whether the
		specified key length is valid for that cipher.
		This is called by ConstructL and SetKeyL
		@param aKeyBytes The key length in bytes to verify.
		@return ETrue if key length is acceptable
		*/
		virtual TBool IsValidKeyLengthL(TInt aKeyBytes) const = 0;
		
		/**
		Helper function implemented by concrete cipher sub-class that allows
		GetCharacteristicsL to return the correct characteristics object.
		@return The implemention uid
		*/
		virtual TUid ImplementationUid() const = 0;
		
		/**
		Validates and sets the crypto mode (iCryptoMode)
		@param aCryptoMode The crypto mode
		*/	
		virtual void DoSetCryptoModeL(TUid aCryptoMode);
		
		/**
		Extracts the raw key from aKey and sets iKey and iKeyBytes
		The key length is also checked to meet export restrictions and
		to ensure that it is appropriate for the cipher.
		@param aKey The key
		*/
		virtual void DoSetKeyL(const CKey& aKey);
		
		/**
		Validates and sets the padding mode (iPaddingMode & iPadding)
		@param aPadding The desired padding mode
		*/
		virtual void DoSetPaddingModeL(TUid aPadding);
		
	private:
			
	protected:
		/// encryption or decryption
		TUid iCryptoMode;
		/// the current padding scheme
		TUid iPaddingMode;
		
		/// the key
		CKey* iKey;
		
		/// current padding scheme implementation
		CPadding* iPadding;
		};
	}

#endif	//	__ASYMMETRICCIPHERIMPL_H__
