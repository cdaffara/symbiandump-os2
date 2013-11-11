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
* hash plugin interface
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __HashPlugin_h__
#define __HashPlugin_h__

#include <cryptospi/cryptoplugin.h>

namespace CryptoSpi
	{
	class CKey;
	/**
	The generic CryptoSPI hash definition. Intended to allow plug-ins
	to implement extensible hash functionality, and to work with all
	known existing hashing algorithms, e.g. MD2, MD4, MD5, SHA-1,
	SHA-256, SHA-512, RIPEMD-160, etc.
	*/
	class MHash : public MPlugin
	    {
	public:
		/**
		Adds aMessage to the internal representation of data to be hashed,
		then returns a TPtrC8 of the finalised hash of all the previously
		appended messages.

		@param aMessage  The data to be included in the hash.
		@return A descriptor pointer to the buffer containing the resulting hash.
		*/
		virtual TPtrC8 Hash(const TDesC8& aMessage) = 0;    // Combination of Update and Final

		/**
		Adds data to the internal representation of messages to be hashed.
		@param aMessage	The data to be included in the hash.
		*/
		virtual void Update(const TDesC8& aMessage) = 0;

		/**
		Produces a final hash value from all the previous updates of data
		to be hashed.
		@param aMessage	The data to be included in the hash.
		*/
		virtual TPtrC8 Final(const TDesC8& aMessage) = 0;

		/**
		Creates a brand new reset MHash object containing no state
		information from the current object.  

		To make a copy of a message digest with its internal state intact,
		see CopyL().
		@return A pointer to the new reset MHash object
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual MHash* ReplicateL() = 0;		

		/** 
		Creates a new MHash object with the exact same state as
		the current object.  

		This function copies all internal state of the message digest.  
		To create a new MHash object without the state of
		the current object, see ReplicateL().
		@return A pointer to the new MHash object
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual MHash* CopyL() = 0;
		
		/**
		 * @deprecated
		 * 
		 * Set the key used for HMAC mode operation.
		 * @param aKey	The key for HMAC
		 * @leave KErrArgument if aKey is not of the expected type.
		 * @leave KErrNotSupported if the key is not of valid length.
		 * @leave ...	Any of the crypto error codes defined in 
  						cryptospi_errs.h or any of the system-wide error codes.
		 */
		virtual void SetKeyL(const CKey& aKey) = 0;
			
		/**
		 * @deprecated
		 * 
		 * Set the operation mode, ie hash or hmac
		 * @param aOperationMode	The UID to identifiy the operation mode

		@leave KErrNotSupported if the operation mode is not supported.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		 */		
		virtual void SetOperationModeL(TUid aOperationMode) = 0;
	    };


	class MAsyncHash: public MPlugin
	    {
    public:
		/**
		 * Adds aMessage to the internal representation of data to be hashed,
		 * then returns a TPtrC8 of the finalised hash of all the previously
		 * appended messages.
		 *
		 * @param aMessage	The data to be included in the hash.
		 * @param aHash A descriptor pointer to the buffer containing the resulting hash.
		 * @param aStatus
		 */
		virtual void Hash(const TDesC8& aMessage, TPtrC8 aHash, TRequestStatus& aStatus) = 0;    // Combination of Update and Final

		/**
		 * Adds data to the internal representation of messages to be hashed.
		 * @param aMessage	The data to be included in the hash.
		 * @param aStatus
		 */
		virtual void Update(const TDesC8& aMessage, TRequestStatus& aStatus) = 0;

		/**
		 * Produces a final hash value from all the previous updates of data
		 * to be hashed.
		 * @param aMessage  Data to be included in the hash.
		 * @param aFinal A descriptor pointer to the buffer containing the resulting hash.
		 * @param aStatus
		 */
		virtual void Final(const TDesC8& aMessage, TPtrC8 aFinal, TRequestStatus& aStatus) = 0;

		/**
		 * Cancel an outstanding request
		 */
		virtual void Cancel() = 0;

		/**
		Creates a brand new reset MAsyncHash object containing no state
		information from the current object.  

		To make a copy of a message digest with its internal state intact,
		see CopyL().

		@return A pointer to the new reset MAsyncHash object

		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual MAsyncHash* ReplicateL() = 0;		

		/** 
		Creates a new MAsyncHash object with the exact same state as
		the current object.  

		This function copies all internal state of the message digest.
		To create a new MAsyncHash object without the state of
		the current object, see ReplicateL().

		@return A pointer to the new MAsyncHash object

		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual MAsyncHash* CopyL() = 0;
		
		/**
		 * @deprecated
		 * 
		 * Set the key used for HMAC mode operation.
		 * @param aKey	The key for HMAC
		 * @leave KErrArgument if aKey is not of the expected type.
		 * @leave KErrNotSupported if the key is not of valid length.
		 * @leave ...	Any of the crypto error codes defined in 
  						cryptospi_errs.h or any of the system-wide error codes.
		 */
		virtual void SetKeyL(const CKey& aKey) = 0;
			
		/**
		 * @deprecated
		 * 
		 * Set the operation mode, ie hash or hmac
		 * @param aOperationMode	The UID to identifiy the operation mode
	 	 * @leave KErrArgument if aKey is not of the expected type.
		 * @leave KErrNotSupported if the key is not of valid length.
		 * @leave ...	Any of the crypto error codes defined in 
  						cryptospi_errs.h or any of the system-wide error codes.
		 */		
		virtual void SetOperationModeL(TUid aOperationMode) = 0;
	    };	
	}

#endif  // __HashPlugin_h__
