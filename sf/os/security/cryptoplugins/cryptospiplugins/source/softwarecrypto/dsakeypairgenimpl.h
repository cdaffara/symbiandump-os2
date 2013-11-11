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


#ifndef __DSAKEYPAIRGENIMPL_H__
#define __DSAKEYPAIRGENIMPL_H__

/**
@file 
@internalComponent
@released
*/

#include <e32base.h>
#include <e32cmn.h>
#include "keypairgenimpl.h"
//#include <asymmetrickeys.h>

/**
 * Implementation of DSA key pair generation.
 */
namespace SoftwareCrypto
	{
	NONSHARABLE_CLASS(CDSAPrimeCertificate) : public CBase
		{
	public:
		/** 
		Creates a new DSA prime certificate from a specified 
		seed and counter value.
 
		@param aSeed	The seed from a DSA key generation process
		@param aCounter	The counter value from a DSA key generation process
		@return			A pointer to a CDSAPrimeCertificate instance
		*/
		static CDSAPrimeCertificate* NewL(const TDesC8& aSeed, TUint aCounter);

		/** 
		Creates a new DSA prime certificate from a specified 
		seed and counter value.
		
		The returned pointer is put onto the cleanup stack.
		
		@param aSeed		The seed from a DSA key generation process
		@param aCounter	The counter value from a DSA key generation process
		@return			A pointer to a CDSAPrimeCertificate instance
		*/
		static CDSAPrimeCertificate* NewLC(const TDesC8& aSeed, TUint aCounter);

		/**
		Gets the seed of the DSA prime certificate
		@return	The seed
		*/
		const TDesC8& Seed() const;
		
		/**
		Gets the counter value of the DSA prime certificate

		@return	The counter's value
		*/
		TUint Counter() const;
		
		/** 
		Destructor 
		*/
		~CDSAPrimeCertificate();
		
	protected:
		/** 
		Constructor
		@param aCounter	The DSA key generation counter
		*/
		CDSAPrimeCertificate(TUint aCounter);

		/**
		Default constructor
		*/
		CDSAPrimeCertificate();

		void ConstructL(const TDesC8& aSeed);
		
	protected:
		/** 
		The DSA key generation seed 
		*/
		const HBufC8* iSeed;
		/** 
		The DSA key generation counter 
		*/
		TUint iCounter;
	private:
		CDSAPrimeCertificate(const CDSAPrimeCertificate&);
		CDSAPrimeCertificate& operator=(const CDSAPrimeCertificate&);
		};
		
	NONSHARABLE_CLASS(CDSAKeyPairGenImpl) : public CKeyPairGenImpl
		{
	public:
	
		static CDSAKeyPairGenImpl* NewL();
		static CDSAKeyPairGenImpl* NewLC();

		// from MPlugin
		void Reset();

		// from MKeyPairGenerator
		void GenerateKeyPairL(TInt aKeySize, const CCryptoParams& aKeyParameters, CKeyPair*& aKeyPair);
		
		// Override CKeyPairGenImpl virtual functions
		TUid ImplementationUid() const;
		
		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		static CExtendedCharacteristics* CreateExtendedCharacteristicsL();
		
		// Destructor
		~CDSAKeyPairGenImpl();

		static TBool GeneratePrimesL(const TDesC8& aSeed,
									 TUint& aCounter, 
									 RInteger& aP, 
									 TUint aL, 
									 RInteger& aQ, 
									 TBool aUseInputCounter=EFalse);
									 
		static TBool ValidPrimeLength(TUint aPrimeBits);
		
	private:
		/**
		Constructor
		*/
		CDSAKeyPairGenImpl();
			
		/// second phase of construction
		void ConstructL();
		
		CDSAPrimeCertificate* iPrimeCertificate;
		};
	}

#endif // __DSAKEYPAIRGENIMPL_H__
