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
* crypto MAC application interface 
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef __CRYPTOAPI_MACAPI_H__
#define __CRYPTOAPI_MACAPI_H__

#include <e32base.h>
#include <cryptospi/cryptobaseapi.h>


namespace CryptoSpi
	{
	class MPlugin;
	class CCryptoParams;
	class CKey;
	class MMac;
	class MAsyncMac;
	
	
	/**
	 * Mac API, which wraps a synchronous Mac plugin implementation
	 * This Mac interface helps the client application to get the message 
	 * authentication code value of a given message which provides
	 * data integrity and data origin authentication. These two goals are 
	 * dependent upon the scope of the distribution of the secret key.
	 */
	
	NONSHARABLE_CLASS(CMac) : public CCryptoBase
		{
	public:
		/**
		 * @internalComponent
		 * Create a CMac instance from the given MMac instance
		 *
		 * @param aMac  	The mac plugin instance
		 * @param aHandle	The current plugin DLL loaded.
		 * @return      	pointer to a CMac instance
		 */
		static CMac* NewL(MMac* aMac, TInt aHandle);
		
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
		IMPORT_C TPtrC8 MacL(const TDesC8& aMessage);    
		
        /**
         * Adds data to the internal representation of messages for which the MAC value
		 * needs to be evaluated.
         * 
         * @param aMessage	The data to be included in the MAC evaluation.
		 * @leave ...		Any of the crypto error codes defined in 
  							cryptospi_errs.h or any of the system-wide error codes.
         */
		IMPORT_C void UpdateL(const TDesC8& aMessage);

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
		IMPORT_C TPtrC8 FinalL(const TDesC8& aMessage);

		/**
		 * This re-initialises the underlying MAC algorithm with a new symmetric key. 
         * It resets the MAC algorithm in a state similar to creating a new MAC instance
         * with the same underlying algorithm but a new symmetric key.
		 *
		 * @param aKey  Symmetric key for calculating message authentication code value. 
		 * @leave ...		Any of the crypto error codes defined in 
  							cryptospi_errs.h or any of the system-wide error codes.
		 */
		IMPORT_C void ReInitialiseAndSetKeyL(const CKey& aKey);    
		
		/**
		 * Creates a brand new reset CMac object containing no state
		 * information from the current object.  
		 * 
		 * @return 	A pointer to the new reset CMac object
		 * @leave ...		Any of the crypto error codes defined in 
  							cryptospi_errs.h or any of the system-wide error codes.
		 */
		IMPORT_C CMac* ReplicateL();		

		/** 
		 * Creates a new CMac object with the exact same state as
		 * the current object.  
 		 * This function copies all internal state of the message digest.
		 * 
		 * @return 	A pointer to the new CMac object
		 * @leave ...		Any of the crypto error codes defined in 
  							cryptospi_errs.h or any of the system-wide error codes.
		 */
		IMPORT_C CMac* CopyL();

		/**
		 * This destructor is exported so that the client application after using
		 * a specific plug-in implementation can destroy its instance. Both the framework 
		 * and the plug-in use/derived from the same interface 'MPlugin. 
		 * By exporting the destructor we are destroying the plug-in instance at
		 * client side via the CryptoSPI framework.
		 */
		IMPORT_C ~CMac();
		
	private:
		/**
		 * The constructor of this class is private. An user application will use
		 * CMacFactory::CreateMacL for the object's initialisation.
		 * 
		 * @param aMac 		The mac plug-in instance
		 * @param aHandle   The current plug-in DLL loaded
		 */
		CMac(MMac* aMac, TInt aHandle);
		};

	
	/**
	 * This is the asynchronous version of CMac class typically used by the 
	 * client applications if hardware plug-in implementation of 
	 * the MAC interface is present. 	
	 */
		
	NONSHARABLE_CLASS(CAsyncMac) : public CCryptoBase
		{
	public:
		/**
		 * @internalComponent
		 * Create a CAsyncMac instance from the given MAsyncMac instance
		 *
		 * @param aMac  The mac plugin instance
		 * @param aHandle	The current plugin DLL loaded.
		 * @return      pointer to a CMac instance
		 */
		static CAsyncMac* NewL(MAsyncMac* aMac, TInt aHandle);
			
		/**
		 * Adds message to the internal representation of data for which the MAC value
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
		IMPORT_C TPtrC8 MacL(const TDesC8& aMessage, TRequestStatus& aStatus);    
			
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
		IMPORT_C void UpdateL(const TDesC8& aMessage, TRequestStatus& aStatus);

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
		IMPORT_C TPtrC8 FinalL(const TDesC8& aMessage, TRequestStatus& aStatus);

		/**
		 * This re-initialises the underlying MAC algorithm with a new symmetric key. 
         * It resets the MAC algorithm in a state similar to creating a new MAC instance
         * with the same underlying algorithm but a new symmetric key.
		 *
		 * @param aKey  Symmetric key for calculating message authentication code value. 
		 * @leave ...		Any of the crypto error codes defined in 
  							cryptospi_errs.h or any of the system-wide error codes.
		 */
		IMPORT_C void ReInitialiseAndSetKeyL(const CKey& aKey);    
		
		/**
		 * Creates a brand new reset CAsyncMac object containing no state
		 * information from the current object.  
		 * 
		 * @return	A pointer to the new reset CAsyncMac object
		 * @leave ...		Any of the crypto error codes defined in 
  							cryptospi_errs.h or any of the system-wide error codes.
		 */
		IMPORT_C CAsyncMac* ReplicateL();		

		/** 
		 * Creates a new CAsyncMac object with the exact same state as
		 * the current object.  
		 * This function copies all internal state of the message digest.
		 * 
		 * @return	A pointer to the new CAsyncHash object
		 * @leave ...		Any of the crypto error codes defined in 
  							cryptospi_errs.h or any of the system-wide error codes.
		 */
		IMPORT_C CAsyncMac* CopyL();
		
		/**
		 * Cancels an outstanding request from the client.
		 */
		IMPORT_C void Cancel();

		/**
		 * This destructor is exported so that the client application after using
		 * a specific plug-in implementation can destroy its instance. Both the framework 
		 * and the plug-in use/derived from the same interface 'MPlugin. 
		 * By exporting the destructor we are destroying the plug-in instance at
		 * client side via the CryptoSPI framework.
		 */
		IMPORT_C ~CAsyncMac();
			
	private:
		/**
		 * The constructor of this class is private. An user application will use
		 * CMacFactory::CreateAsyncMacL for the object's initialisation.
		 * 
		 * @param aMac 		The mac plug-in instance
		 * @param aHandle   The current plug-in DLL loaded
		 */
		CAsyncMac(MAsyncMac* aMac, TInt aHandle);
		};

		
	/**
	 * The Factory to create synchronous and asynchronous Mac instances
	 */
		
	class CMacFactory
		{
	public:

		/**
		 * Create a CMac instance (for software based MAC plug-in dll implementation)
		 *
		 * @param aMac           	The pointer to CMac. This will be initialised with 
		 * 						  	the plug-in implementation of the desired MAC algorithm.
		 * @param aAlgorithmUid  	The specific MAC algorithm desired for evaluation of MAC value.
		 *                       	e.g. MD2, SHA1 or AES-XCBC-MAC-96, AES-XCBC-PRF-128
		 * @param aKey           	Symmetric key for calculating message authentication code value. 
		 * @param aAlgorithmParams  The parameters those are specific to a particular MAC algorithm.
		 * 							This is for extendibility and will normally be null.                     	
		 * @leave               	KErrNone if successful; otherwise, leaves with a system wide error code.
		 */
		IMPORT_C static void CreateMacL(CMac*& aMac,
										const TUid aAlgorithmUid,
										const CKey& aKey,
										const CCryptoParams* aAlgorithmParams);
		
		/**
		 * Create a CAsyncMac instance (for hardware based MAC plug-in dll implementation)
		 *
		 * @param aMac           	The pointer to CMac. This will be initialised with 
		 * 						  	the plug-in implementation of the desired MAC algorithm.
		 * @param aAlgorithmUid  	The specific MAC algorithm desired for evaluation of MAC value.
		 *                       	e.g. MD2, SHA1 or AES-XCBC-MAC-96, AES-XCBC-PRF-128
		 * @param aKey           	Symmetric key for calculating message authentication code value. 
		 * @param aAlgorithmParams  The parameters those are specific to a particular MAC algorithm.
		 * 							This is for extendibility and will normally be null.                     	
		 * @leave               	KErrNone if successful; otherwise, leaves with a system wide error code.
		 */
		IMPORT_C static void CreateAsyncMacL(CAsyncMac*& aMac,
										const TUid aAlgorithmUid,
										const CKey& aKey,
										const CCryptoParams* aAlgorithmParams);
	
	private:
		/**
		 * The class is used as a static class since there is no data 
		 * or behaviour in the class that depends on object identity.
		 * Therefore the default constructor of this class is made private. 
		 */
		CMacFactory();	
		};
		
	}

#endif //__CRYPTOAPI_MACAPI_H__
