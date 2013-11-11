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
* verifier abstract interface
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __CRYPTOAPI_VERIFIER_H__
#define __CRYPTOAPI_VERIFIER_H__

#include <cryptospi/signerplugin.h>

namespace CryptoSpi
	{

	/**
	The Verifier definition. Intended to allow plug-ins
	to implement extensible signature verifier functionality, and to work with all
	known existing  signature algorithms, e.g. DSA, RSA etc
	*/

	class MVerifier : public MSignatureBase
		{
	public:

		/**
		Verify the signature
		@param aInput	The hash of the message to be verified
		@param aSignature	The signature of the hash
		@param aVerificationResult	Indicates the success or failure of the verification
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual void VerifyL(const TDesC8& aInput, const CCryptoParams& aSignature, TBool& aVerificationResult) = 0;

		/**
		Unsign the signature
		@param aOutput	The unsigned hash
		@param aSignature	The signature of the hash
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual void InverseSignL(HBufC8*& aOutput, const CCryptoParams& aSignature) = 0;
		};

	class MAsyncVerifier : public MSignatureBase
		{
	public:

		/**
		Verify the signature
		@param aInput	The hash of the message to be verified
		@param aSignature	The signature of the hash
		@param aVerificationResult	Indicates the success or failure of the verification
		@param aRequestStatus
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual void VerifyL(const TDesC8& aInput, const CCryptoParams& aSignature, TBool& aVerificationResult, TRequestStatus& aRequestStatus) = 0;

		/**
		Unsign the signature
		@param aOutput	The unsigned hash
		@param aSignature	The signature of the hash
		@param aRequestStatus the request status.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual void InverseSignL(HBufC8*& aOutput, const CCryptoParams& aSignature, TRequestStatus& aRequestStatus) = 0;
		/**
		Cancel the outstanding request
		*/
		virtual void Cancel() = 0;
		};

	} //namespace CryptoSpi

#endif //__CRYPTOAPI_VERIFIER_H__

