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
* keyagreement.h
* Key Agreement Abstract Interface
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __CRYPTOAPI_KEYAGEEMENT_H__
#define __CRYPTOAPI_KEYAGEEMENT_H__

#include <cryptospi/cryptoplugin.h>


/**
The keyagreement definition. Intended to allow plug-ins
to implement extensible keyagreement functionality, and to work with all
known existing keyagreement algorithms, e.g. DiffieHellman, ECDH etc. A
plug-in implementing this interface should expect the private key of one of 
the parties to be stored in it via SetKeyL() and then the public key of the 
other party to be used in the call to AgreeL().
*/

namespace CryptoSpi
	{
	class CCryptoParams;

	class MKeyAgreement : public MPlugin
		{
	public:
		/**
		Set the private key for this keyagreement. Reset() is called to reinitialize the keyAgreement
		@param aSelfPrivateKey	The private key of one of the parties
		@param aParams	The parameters shared between both parties

		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual void SetKeyL(const CKey& aSelfPrivateKey, const CCryptoParams* aParams) = 0;
		
		/**
		Performs the key agreement operation.
		@param aOtherPublicKey	The public key of the other party
		@param aParams	The parameters shared between both parties
		@return A pointer to a CKey instance containing the agreed key

		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual CKey* AgreeL(const CKey& aOtherPublicKey, const CCryptoParams* aParams) = 0;
		};


	class MAsyncKeyAgreement : public MPlugin
		{
	public:

		/**
		Set the private key for this keyagreement. Reset() is called to reinitialize the keyAgreement
		@param aSelfPrivateKey	The private key of one of the parties
		@param aParams	The parameters shared between both parties
 
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual void SetKeyL(const CKey& aSelfPrivateKey, const CCryptoParams* aParams) = 0;

		/**
		Performs the key agreement operation.
		@param aOtherPublicKey	The public key of the other party
		@param aKey A pointer to a CKey instance containing the agreed key
		@param aParams	The parameters shared between both parties
		@param aRequestStatus 

		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual void AgreeL(const CKey& aOtherPublicKey, CKey& aKey, const CCryptoParams* aParams, TRequestStatus& aRequestStatus) = 0;

		/**
		Cancel the outstanding request
		*/	
		virtual void Cancel() = 0;
		};

	}
#endif //__CRYPTOAPI_KEYAGEEMENT_H__
