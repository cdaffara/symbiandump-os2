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
* crypto random application interface
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef __CRYPTOAPI_RANDOMAPI_H__
#define __CRYPTOAPI_RANDOMAPI_H__

#include <cryptospi/cryptobaseapi.h>

namespace CryptoSpi
	{
	class MRandom;
	class MAsyncRandom;
	class CCryptoParams;
	/**
	Synchronous Random API, which wraps a synchronous Random plugin implementation
	*/
	NONSHARABLE_CLASS(CRandom) : public CCryptoBase
		{
	public:
		/**
		 * @internalComponent
		 * 
		 * Create a CRandom instance from the given MRandom instance
		 * @param aRandom	The random plugin instance
		 * @return A pointer to a CRandom instance
		 **/
		static CRandom* NewL(MRandom* aRandom, TInt aHandle);

		/**
		Destructor
		*/
		IMPORT_C ~CRandom();

		/**
		 * Generate enough random bytes to fill the supplied descriptor
		 * If there is not enough entropy available, or another error
		 * occurs (e.g. out of memory) then this method may leave.
		 * @param aDest	The returned generated random bytes filling the descriptor
		 */
		IMPORT_C void GenerateRandomBytesL(TDes8& aDest);

	private:
		
		/**
		Constructor
		*/
		CRandom(MRandom* aRandom, TInt aHandle);
		
		};


	/**
	Asynchronous Random API, which wraps an asynchronous Random plugin implementation
	*/

	NONSHARABLE_CLASS(CAsyncRandom) : public CCryptoBase
		{
	public:
		/**
		 * @internalComponent
		 * 
		 * Create a CRandom instance from the given MRandom instance
		 * @param aAsyncRandom		The async random plugin instance
		 * @return A pointer to a CAsyncRandom instance
		 **/
		static CAsyncRandom* NewL(MAsyncRandom* aAsyncRandom, TInt aHandle);

		/**
		Destructor
		*/
		IMPORT_C ~CAsyncRandom();

		/**
		 * Generate enough random bytes to fill the supplied descriptor
		 * If there is not enough entropy available, or another error
		 * occurs (e.g. out of memory) then this method may leave.
		 * @param aDest  The returned generated random bytes filling the descriptor
		 * @param aStatus
		 */
		IMPORT_C void GenerateRandomBytesL(TDes8& aDest, TRequestStatus& aStatus);

	private:
		
		/**
		Constructor
		*/
		CAsyncRandom(MAsyncRandom* aAsyncRandom, TInt aHandle);
		
		};


	/**
	the Factory to create synchronous and asynchronous hash instance 
	*/
	class CRandomFactory
		{

	public:
		/**
		Creates a new instance of a Random object.

		@param aRandom A reference to a pointer that should be set to point to the new CRandom object.
		@param aAlgorithmUid The algorithm to use
		@param aAlgorithmParams Parameters that are specific this algorithm.
		@return KErrNone if successful; otherwise, a system wide error code.
		*/
		IMPORT_C static void CreateRandomL(CRandom*& aRandom,
										TUid aAlgorithmUid,
										const CCryptoParams* aAlgorithmParams);

		/**
		Creates a new asynchronous instance of a Random object.

		@param aAsyncRandom A reference to a pointer that should be set to point to the new CAsyncRandom object.
		@param aAlgorithmUid The algorithm to use
		@param aAlgorithmParams Parameters that are specific this particular algorithm.
		@return KErrNone if successful; otherwise, a system wide error code.
		*/
		IMPORT_C static void CreateAsyncRandomL(CAsyncRandom*& aAsyncRandom,
											TUid aAlgorithmUid,
											const CCryptoParams* aAlgorithmParams);

		};
	}

#endif //__CRYPTOAPI_RANDOMAPI_H__
