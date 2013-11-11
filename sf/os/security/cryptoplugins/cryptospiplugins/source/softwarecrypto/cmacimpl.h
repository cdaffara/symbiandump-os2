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
* Cipher MAC plugin implementation
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __CRYPTOAPI_SOFTWARECIPHERMACIMPL_H__
#define __CRYPTOAPI_SOFTWARECIPHERMACIMPL_H__

#include "cryptosymmetriccipherapi.h"
#include "keys.h"


namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
		
	/**
	 * This is the maximum block size in bytes currently supported by CMAC implementation.
	 * 
	 * The cipher based algorithms currently supported are
	 * AES-XCBC-MAC-96 and AES-XCBC-PRF-128.
	 */  
	const TInt KMacBlockSize = 16;		
	
	/** 
	 * The class is created from following substance classes
	 * 1. MMac
	 * 2. CSymmetricCipher
	 * Using the methods of the above classes we will transform/mould the Cipher methods in
	 * a way as to be inline with the MMac interface methods. The MMac methods will
	 * serve as the wrapper internal to which Cipher methods will work to provide the MAC value.
	 * 
	 * The class was created to provide consistency/similarity HMAC and CMAC works.
	 * Also for future extensibility of other Cipher algorithms.
	 */		
	NONSHARABLE_CLASS(CCMacImpl) : public CBase
		{		
	public:
		/**
		 * Cipher MAC implementation instance creation methods
		 *
		 *The owneship of 'aSymmetricCipher' is imparted to instance of this class.
		 */
		static CCMacImpl* NewL(const CKey& aKey,
			        		   CSymmetricCipher* aSymmetricCipher,	
				               TInt32 aAlgorithmUid);
		
		static CCMacImpl* NewLC(const CKey& aKey, 
							    CSymmetricCipher* aSymmetricCipher,	
							    TInt32 aAlgorithmUid);
		/**
		 * Simulating Methods from MPlugin
		 */
		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		
		// We will call CSymmetricBlockCipherImpl::Reset();
		void Reset();
		
		/**
		 *  Simulating MAC interfaces (Software based)
		 */
	    TPtrC8 MacL(const TDesC8& aMessage);
        void UpdateL(const TDesC8& aMessage);
        TPtrC8 FinalL(const TDesC8& aMessage);
		void ReInitialiseAndSetKeyL(const CKey& aKey);    
		void SetKeyL(const CKey& aKey);
		~CCMacImpl();
		CCMacImpl* CopyL();
		CCMacImpl* ReplicateL();
		
	private:
	    /**
		 * Constructors
		 */
    	CCMacImpl(CryptoSpi::CSymmetricCipher* aSymmetricCipher);
		CCMacImpl(const CCMacImpl&);
		
		/**
		 * Initialize the 'iCipherImpl' instances.
		 */
		void ConstructL(const CKey& aKey, TInt32 aAlgorithmUid);
		void DoUpdateL(const TDesC8& aMessage);
		TPtrC8 DoFinalL();
		void PadMessage();
		void ProcessBlockL();
		void XORKeyWithData(const TDesC8& aKey, TDes8& aOutput);
		CKey* Create128bitKeyL(const CKey& aKey);		
		
	private:			
		TInt32 iImplementationUid; 
		CKey* iKey;
		CryptoSpi::CSymmetricCipher* iCipherImpl;
		
		TBuf8<KMacBlockSize> iKey1;
		TBuf8<KMacBlockSize> iKey2;
		TBuf8<KMacBlockSize> iKey3;
		TBuf8<KMacBlockSize> iMacValue;
	
		TUint8 iE[KMacBlockSize];
		TUint8 iData[KMacBlockSize];
		TInt iCurrentTotalLength;
		// Resets the cipher with iE(128 zero bits) the next time MacL, 
		// UpdateL or FinalL are called. This was introduced as we cannot leave from the
		// non-leaving CCMacImpl::Reset() implementation of the MPlugin::Reset() pure 
		// virtual. To prevent behavioral break.
		TInt iDelayedReset;
		};
	}

#endif // __CRYPTOAPI_SOFTWARECIPHERMACIMPL_H__


