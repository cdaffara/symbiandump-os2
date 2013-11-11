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
* Key Pair Generator Abstract interface
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __CRYPTOAPI_KEYPAIRGENERATORPLUGIN_H__
#define __CRYPTOAPI_KEYPAIRGENERATORPLUGIN_H__

#include <cryptospi/cryptoplugin.h>

namespace CryptoSpi
	{
	class CKeyPair;
	/**
	The Key Pair Generator definition. Intended to allow plug-ins
	to implement extensible Key Pair Generator functionality, and
	to work with all known existing Key pair Generator algorithms
	*/


	class MKeyPairGenerator : public MPlugin
		{
	public:

		/**
		Generates a key pair
		@param aKeySize	The algorithm-specific metric as a number of bits
		@param aKeyParameters	The list of algorithm-specific parameters used by the generator to generate the key pair
		@param aKeyPair	The generated key pair

		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual void GenerateKeyPairL(TInt aKeySize, const CCryptoParams& aKeyParameters, CKeyPair*& aKeyPair) = 0;
		};


	class MAsyncKeyPairGenerator : public MPlugin
		{
	public:

		/**
		Generates a key pair
		@param aKeySize	The algorithm-specific metric as a number of bits
		@param aKeyParameters	The list of algorithm-specific parameters used by the generator to generate the key pair
		@param aKeyPair	The generated key pair
		@param aRequestStatus

		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual void GenerateKeyPairL(TInt aKeySize, const CCryptoParams& aKeyParameters, CKeyPair*& aKeyPair, TRequestStatus& aRequestStatus) = 0;

		/**
		Cancel the outstanding request
		*/
		virtual void Cancel() = 0;

		};

	} // namespace CryptoSpi

#endif //__CRYPTOAPI_KEYPAIRGENERATORPLUGIN_H__
