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
* crypto keys definition - including symmetric & asymmetric keys
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef __CRYPTOAPI_KEYS_H__
#define __CRYPTOAPI_KEYS_H__

#include <e32base.h>
#include <cryptospi/cryptoparams.h>

namespace CryptoSpi
	{
	class CCryptoParam;
	/**
	The key property definition.
	*/
	class TKeyProperty
		{
	public:
		/**
		Algorithm UID of this Key
		*/
		TUid iAlgorithmUid;

		/**
		Implementation ID of the key generator
		*/
		TUid iImplementationUid;

		/**
		Key Type
		*/
		TUid iKeyType;

		/**
		Key Attribute
		*/
		TUid iKeyAttribute;
		};


	/**
	The key definition 
	*/
	NONSHARABLE_CLASS(CKey) : public CBase
		{
	public:

		/**
		Creates a new key from a key property and key parameters.
		@param aKeyProperty	The key property for the new key
		@param aKeyParameters	The list of the key parameters
		@return A pointer to a CKey instance
		*/
		IMPORT_C static CKey* NewL(const TKeyProperty& aKeyProperty, const CCryptoParams& aKeyParameters);

		/**
		Creates a new key from a key property and key parameters.
		Leave the Ckey pointer on the cleanup stack.
		@param aKeyProperty	The key property for the new key
		@param aKeyParameters	The list of the key parameters
		@return A pointer to a CKey instance
		*/
		IMPORT_C static CKey* NewLC(const TKeyProperty& aKeyProperty, const CCryptoParams& aKeyParameters);

		/**
		Creates a new key from a CKey object.
		@param aKey the source CKey object.
		@return A pointer to a CKey instance
		*/
		IMPORT_C static CKey* NewL(const CKey& aKey);

		/**
		Creates a new key from a CKey object.
		Leave the Ckey pointer on the cleanup stack.
		@param aKey the source CKey object.
		@return A pointer to a CKey instance
		*/		
		IMPORT_C static CKey* NewLC(const CKey& aKey);

		/**
		Destructor
		*/
		IMPORT_C ~CKey();

		/**
		Retrieve the key property
		@return The reference of the key property
		*/
		IMPORT_C const TKeyProperty& KeyProperty() const;
		
		/**
		Retrieve the Key Parameter according to the UID of the parameter
		@param aUid	The UID of the key parameter
		*/
		IMPORT_C const TInteger& GetBigIntL(TUid aUid) const;
		IMPORT_C TInt GetTIntL(TUid aUid) const;
		IMPORT_C const TDesC8& GetTDesC8L(TUid aUid) const;

		IMPORT_C TBool IsPresent(TUid aUid) const;

		/**
		Retrieve the key property and key parameter
		@param aKeyProperty	The key property
		@return The key parameters
		*/		
		IMPORT_C const CCryptoParams& KeyParameters() const;

	private:
		/**
		constructor
		*/
		CKey(const TKeyProperty& aKeyProperty);

		/**
		2nd Phase Constructor
		*/
		void ConstructL(const CCryptoParams& aKeyParameters);
						
	private:
		/**
		The Key Parameters of this object
		*/
		CCryptoParams* iKeyParameters;
		
		/**
		the key property
		*/
		TKeyProperty iKeyProperty;
		};

	} // namespace CryptoSpi

#endif //__CRYPTOAPI_KEYS_H__


