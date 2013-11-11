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
* crypto hash application interface
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef __CRYPTOAPI_HASHAPI_H__
#define __CRYPTOAPI_HASHAPI_H__

#include <e32base.h>
#include <cryptospi/cryptobaseapi.h>


namespace CryptoSpi
	{
	class MPlugin;
	class MHash;
	class MAsyncHash;
	class CCryptoParams;
	class CKey;

	/**
	Hash API, which wraps a synchronous Hash plugin implementation
	*/	
	NONSHARABLE_CLASS(CHash) : public CCryptoBase
		{
	public:
		/**
		 * @internalComponent
		 * 
		 * Create a CHash instance from the given MHash instance
		 * @param aHash	The hash plugin instance
		 * @return A pointer to a CHash instance
		 **/
		static CHash* NewL(MHash* aHash, TInt aHandle);
		
		/**
		Destructor
		*/
		IMPORT_C ~CHash();
		
        /**
        Adds aMessage to the internal representation of data to be hashed,
        then returns a TPtrC8 of the finalised hash of all the previously
        appended messages.
        @param aMessage  The data to be included in the hash.
        @return A descriptor pointer to the buffer containing the resulting hash.
        */
        IMPORT_C TPtrC8 Hash(const TDesC8& aMessage);    // Combination of Update and Final

        /**
        Adds data to the internal representation of messages to be hashed.
        @param aMessage	The data to be included in the hash.
        */
        IMPORT_C void Update(const TDesC8& aMessage);

        /**
        Produces a final hash value from all the previous updates of data
        to be hashed.
        @param aMessage	The data to be included in the hash.
        */
        IMPORT_C TPtrC8 Final(const TDesC8& aMessage);
        
		/**
		Creates a brand new reset CHash object containing no state
		information from the current object. This API is only to support  
		the old crypto API for BC reason. It is strongly recommended not to use this API.
	
	    To make a copy of a message digest with its internal state intact,
		see CopyL().
	
		@return A pointer to the new reset CHash object.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		IMPORT_C CHash* ReplicateL();		

		/** 
		Creates a new CHash object with the exact same state as
		the current object.This API is only to support  
		the old crypto API for BC reason. It is strongly recommended not to use this API.

		This function copies all internal state of the message digest.
		To create a new CHash object without the state of
		the current object, see ReplicateL().

		@return A pointer to the new CHash object.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		IMPORT_C CHash* CopyL();
				
		/**
		 * @deprecated
		 * 
		 * Set the key used for HMAC mode operation.
		 * @param aKey	The key for HMAC
		 * @leave KErrArgument if aKey is not of the expected type.
		 * @leave ...	Any of the crypto error codes defined in 
  				    	cryptospi_errs.h or any of the system-wide error codes.
		 */
		IMPORT_C void SetKeyL(const CKey& aKey);
			
		/**
		 * @deprecated
		 * 
		 * Set the operation mode, ie hash or hmac
		 * @param aOperationMode	The UID to identifiy the operation mode
		 * @leave KErrNotSupported if the specified operation mode is not supported.
		 * @leave ...	Any of the crypto error codes defined in 
  						cryptospi_errs.h or any of the system-wide error codes.
		 */		
		IMPORT_C void SetOperationModeL(TUid aOperationMode);
		
	private:
		/**
		Constructor
		*/
		CHash(MHash* aHash, TInt aHandle);
		};


	/**
	Asynchronous Hash API, which wraps an asynchronous Hash plugin implementation
	*/
	NONSHARABLE_CLASS(CAsyncHash) : public CCryptoBase
		{
	public:
		/**
		 * @internalComponent
		 * 
		 * Create a CAsyncHash instance from the given MAsyncHash instance
		 * @param aAsyncHash The async hash plugin instance
		 * @return A pointer to a CAsyncHash instance
		 **/
		static CAsyncHash* NewL(MAsyncHash* aAsyncHash, TInt aHandle);
		
		/**
		Destructor
		*/
		IMPORT_C ~CAsyncHash();
		
		/**
		Adds aMessage to the internal representation of data to be hashed,
		then returns a TPtrC8 of the finalised hash of all the previously
		appended messages.
		@param aMessage The data to be included in the hash.
		@param aHash A descriptor pointer to the buffer containing the hash result.
		@param aStatus
		*/
		IMPORT_C void Hash(const TDesC8& aMessage, TPtrC8& aHash, TRequestStatus& aStatus);

		/**
		Adds data to the internal representation of messages to be hashed.
		@param aMessage	The data to be included in the hash.
		@param aStatus
		*/
		IMPORT_C void Update(const TDesC8& aMessage, TRequestStatus& aStatus);

		/**
		Produces a final hash value from all the previous updates of data
		to be hashed.
		@param aMessage	The data to be included in the hash.
		@param aFinal A descriptor pointer to the buffer containing the hash result.
		@param aStatus
		@return A descriptor pointer to the buffer containing the resulting hash.
		*/
		IMPORT_C void Final(const TDesC8& aMessage, TPtrC8& aFinal, TRequestStatus& aStatus);

		/**
		Cancel the outstanding request
		*/
		IMPORT_C void Cancel();
		
		/**
		Creates a brand new reset CAsyncHash object containing no state
		information from the current object. This API is only to support  
		the old crypto API for BC reason. It is strongly recommended not to use this API.

		To make a copy of a message digest with its internal state intact,
		see CopyL().

		@return A pointer to the new reset CAsyncHash object.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		IMPORT_C CAsyncHash* ReplicateL();		

		/** 
		Creates a new CAsyncHash object with the exact same state as
		the current object. This API is only to support  
		the old crypto API for BC reason. It is strongly recommended not to use this API.

		This function copies all internal state of the message digest.
		To create a new CAsyncHash object without the state of
		the current object, see ReplicateL().

		@return A pointer to the new CAsyncHash object.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		IMPORT_C CAsyncHash* CopyL();
		
		/**
		 * @deprecated
		 * 
		 * Set the key used for HMAC mode operation.
		 * @param aKey the key for HMAC
		 * @leave KErrArgument if aKey is not of the expected type.
		 * @leave ...	Any of the crypto error codes defined in 
  						cryptospi_errs.h or any of the system-wide error codes.
		 */
		IMPORT_C void SetKeyL(const CKey& aKey);
			
		/**
		 * @deprecated
		 * 
		 * Set the operation mode, ie hash or hmac
		 * @param aOperationMode The UID to identifiy the operation mode
		 * @leave KErrNotSupported if the specified mode is not supported.
		 * @leave ...	Any of the crypto error codes defined in 
  						cryptospi_errs.h or any of the system-wide error codes.
		 */		
		IMPORT_C void SetOperationModeL(TUid aOperationMode);
		
	private:
		
		/**
		Constructor
		*/
		CAsyncHash(MAsyncHash* aAsyncHash, TInt aHandle);
		};

	/**
	the Factory to create synchronous and asynchronous hash instances
	*/
	class CHashFactory
		{
	public:

		/**
		 * @deprecated
		 * 
		 * Create a CHash instance
		 * 
		 * @param aHash 			The pointer to CHash
		 * @param aAlgorithmUid 	The specific hash algorithm e.g. MD2, SHA1, MD4
		 * @param aOperationMode 	The operation mode of the hash e.g. Hash mode, Hmac mode
		 * @param aKey 				The key for Hmac mode, which should be NULL in Hash mode
		 * @param aAlgorithmParams 	The parameters that are specific to a particular 
		 * 							algorithm. This is for extendibility and will normally be null.
		 * @return 					KErrNone if successful; otherwise, a system wide error code.
		 */
		IMPORT_C static void CreateHashL(CHash*& aHash,
										TUid aAlgorithmUid,
										TUid aOperationMode,
										const CKey* aKey,
										const CCryptoParams* aAlgorithmParams);

		/**
		 * @deprecated
		 * 
		 * Create a CAsyncHash instance
		 * 	
		 * @param aAsyncHash 		The pointer to CAsyncHash
		 * @param aAlgorithmUid 	The specific hash algorithm e.g. MD2, SHA1, MD4
		 * @param aOperationMode 	The operation mode of the hash e.g. Hash mode, Hmac mode
		 * @param aKey 				The key for Hmac mode, which should be NULL in Hash mode
		 * @param aAlgorithmParams 	The parameters that are specific to a particular 
		 * 							algorithm. This is for extendibility and will normally be null.
		 * @return 					KErrNone if successful; otherwise, a system wide error code.
		 */
		IMPORT_C static void CreateAsyncHashL(CAsyncHash*& aAsyncHash, 
										TUid aAlgorithmUid,
										TUid aOperationMode,
										const CKey* aKey,
										const CCryptoParams* aAlgorithmParams);

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT		
		/**
		 * Create a CHash instance
		 * 
		 * @param 	aHash The pointer to CHash
		 * @param 	aAlgorithmUid The specific hash algorithm e.g. MD2, SHA1, MD4
		 * @param 	aAlgorithmParams The parameters that are specific to a particular 
		 * 			algorithm. This is for extendibility and will normally be null.
		 * @return 	KErrNone if successful; otherwise, a system wide error code.
		 */
		IMPORT_C static void CreateHashL(CHash*& aHash,
										 TUid aAlgorithmUid,
										 const CCryptoParams* aAlgorithmParams);

		/**
		 * Create a CAsyncHash instance
		 * 	
		 * @param 	aAsyncHash The pointer to CAsyncHash
		 * @param 	aAlgorithmUid The specific hash algorithm e.g. MD2, SHA1, MD4
		 * @param 	aAlgorithmParams The parameters that are specific to a particular 
		 * 			algorithm. This is for extendibility and will normally be null.
		 * @return 	KErrNone if successful; otherwise, a system wide error code.
		 */
		IMPORT_C static void CreateAsyncHashL(CAsyncHash*& aAsyncHash, 
										      TUid aAlgorithmUid,
										      const CCryptoParams* aAlgorithmParams);
#endif	
		
		};
	}

#endif //__CRYPTOAPI_HASHAPI_H__
