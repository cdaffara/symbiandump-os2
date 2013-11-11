/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* MAC(message authentication code) plugin interface
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __MAC_PLUGIN_H
#define __MAC_PLUGIN_H

#include <cryptospi/cryptoplugin.h>

namespace CryptoSpi
	{
	class CKey;
	
	/**
	 * The generic CryptoSPI MAC definition. This allow plug-ins
	 * to implement extensible MAC functionality and to work with all
	 * known existing hash based or symmetric cipher based MAC algorithms
	 * for e.g. MD2, MD4, MD5, SHA-1, SHA-256, SHA-512, RIPEMD-160, etc. or 
	 * AES-XCBC-MAC-96, AES-XCBC-PRF-128 etc. respectively. 
	 */
	class MMac : public MPlugin
	    {
	public:

		/**
		 * Adds message to the internal representation of data for which the MAC value
		 * needs to be evaluated and then returns a TPtrC8 of the finalised MAC value 
		 * of all the previously appended messages. 
		 * 
		 * @param aMessage  The data for which MAC value is to be evaluated.
		 * @return          A descriptor pointer to the buffer containing the
		 *                  resulting MAC value.
		 * @leave ...		Any of the crypto error codes defined in 
  							cryptospi_errs.h or any of the system-wide error codes.
		 */
		virtual TPtrC8 MacL(const TDesC8& aMessage) = 0;    
		
        /**
         * Adds data to the internal representation of messages for which the MAC value
		 * needs to be evaluated.
         * 
         * @param aMessage	The data to be included in the MAC evaluation.
		 * @leave ...		Any of the crypto error codes defined in 
  							cryptospi_errs.h or any of the system-wide error codes.
         */
        virtual void UpdateL(const TDesC8& aMessage) = 0;

        /**
         * Produces a final MAC value from all the previous updates of data to be MACed. 
         * It resets the MAC algorithm in a state similar to creating a new MAC instance
         * with the same underlying algorithm and supplied symmetric key.
         *  
         * @param aMessage	The data to be included in the MAC evaluation.
		 * @return          A descriptor pointer to the buffer containing the
		 *                  resulting MAC value.
		 * @leave ...		Any of the crypto error codes defined in 
  							cryptospi_errs.h or any of the system-wide error codes.
		 */
        virtual TPtrC8 FinalL(const TDesC8& aMessage) = 0;

		/**
		 * This re-initialises the underlying MAC algorithm with a new symmetric key. 
         * It resets the MAC algorithm in a state similar to creating a new MAC instance
         * with the same underlying algorithm but a new symmetric key.
		 *
		 * @param aKey  Symmetric key for calculating message authentication code value. 
		 * @leave ...	Any of the crypto error codes defined in 
  						cryptospi_errs.h or any of the system-wide error codes.
		 */
		virtual void ReInitialiseAndSetKeyL(const CKey& aKey) = 0;
		
		/**
		 * Creates a brand new reset MMac object containing no state
		 * information from the current object.  
		 * 
		 * @return 	A pointer to the new reset MMac object
		 * @leave ...	Any of the crypto error codes defined in 
  						cryptospi_errs.h or any of the system-wide error codes.
		 */
		virtual MMac* ReplicateL() = 0;		

		/** 
		 * Creates a new MMac object with the exact same state as
		 * the current object.  
 		 * This function copies all internal state of the message digest.
		 * 
		 * @return 	A pointer to the new MMac object
		 * @leave ...	Any of the crypto error codes defined in 
  						cryptospi_errs.h or any of the system-wide error codes.
		 */
		virtual MMac* CopyL() = 0;
	    };

	/**
	 * Asynchronous MAC interface typically used by the plug-in implementations 
	 * that are based for dedicated crypto hardware.
	 */    
	class MAsyncMac : public MPlugin
	    {
	public:

		/**
		 * Adds message to the internal representation of data for which the MAC value,
		 * needs to be evaluated and then returns a TPtrC8 of the finalised MAC value 
		 * of all the previously appended messages. 
		 * 
		 * @param aMessage  The data for which MAC value is to be evaluated.
		 * @param aStatus   Holds the completion status of an asynchronous 
		 * 					request for MAC evaluation.
		 * @return          A descriptor pointer to the buffer containing the
		 *                  resulting MAC value.
		 * @leave ...		Any of the crypto error codes defined in 
  							cryptospi_errs.h or any of the system-wide error codes.
		 */
		virtual TPtrC8 MacL(const TDesC8& aMessage, TRequestStatus& aStatus) = 0;    
			
        /**
         * Adds data to the internal representation of messages for which the MAC value
		 * needs to be evaluated.
         *
         * @param aMessage	The data to be included in the MAC evaluation.
		 * @param aStatus   Holds the completion status of an asynchronous 
		 * 					request for MAC evaluation.
		 * @leave ...		Any of the crypto error codes defined in 
  							cryptospi_errs.h or any of the system-wide error codes.
         */
        virtual void UpdateL(const TDesC8& aMessage, TRequestStatus& aStatus) = 0;

        /**
         * Produces a final MAC value from all the previous updates of data to be MACed. 
         * It resets the MAC algorithm in a state similar to creating a new MAC instance
         * with the same underlying algorithm and supplied symmetric key.
         *  
         * @param aMessage	The data to be included in the MAC evaluation.
		 * @param aStatus   Holds the completion status of an asynchronous 
		 * 					request for MAC evaluation.
		 * @return          A descriptor pointer to the buffer containing the
		 *                  resulting MAC value.
		 * @leave ...		Any of the crypto error codes defined in 
  							cryptospi_errs.h or any of the system-wide error codes.
         */
        virtual TPtrC8 FinalL(const TDesC8& aMessage, TRequestStatus& aStatus) = 0;

	    /**
		 * This re-initialises the underlying MAC algorithm with a new symmetric key. 
         * It resets the MAC algorithm in a state similar to creating a new MAC instance
         * with the same underlying algorithm but a new symmetric key.
		 *
		 * @param aKey     Symmetric key for calculating message authentication code value. 
		 * @param aStatus  Holds the completion status of an asynchronous 
		 * 				   request for MAC evaluation.
		 * @leave ...		Any of the crypto error codes defined in 
  							cryptospi_errs.h or any of the system-wide error codes.
		 */
		virtual void ReInitialiseAndSetKeyL(const CKey& aKey) = 0;    
		
		/**
		 * Cancels an outstanding request from the client.
		 */
		virtual void Cancel() = 0;
		
		/**
		 * Creates a brand new reset MAsyncMac object containing no state
		 * information from the current object.  
		 * 
		 * @return	A pointer to the new reset MAsyncHash object
		 * @leave ...	Any of the crypto error codes defined in 
  						cryptospi_errs.h or any of the system-wide error codes.
		 */
		virtual MAsyncMac* ReplicateL() = 0;		

		/** 
		 * Creates a new MAsyncMac object with the exact same state as
		 * the current object.  
		 * This function copies all internal state of the message digest.
		 * 
		 * @return	A pointer to the new MAsyncMac object
		 * @leave ...	Any of the crypto error codes defined in 
  						cryptospi_errs.h or any of the system-wide error codes.
		 */
		virtual MAsyncMac* CopyL() = 0;
	    };
	}

#endif  __MAC_PLUGIN_H
