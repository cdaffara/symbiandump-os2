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
* mac plugin implementation
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __CRYPTOAPI_SOFTWAREMACIMPL_H__
#define __CRYPTOAPI_SOFTWAREMACIMPL_H__

#include <cryptospi/macplugin.h>
#include "cryptosymmetriccipherapi.h"
#include "softwarehashbase.h"
#include "keys.h"
#include "cmacimpl.h"
#include "hmacimpl.h"


namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
    	
	NONSHARABLE_CLASS(CMacImpl) : public CBase, public MMac  
		{		
	public:
		/**
		 * MAC implementation instance creation methods
		 */
		static CMacImpl* NewL(const CKey& aKey,
				              const TUid aImplementationId,
				              const CCryptoParams* aAlgorithmParams);

		/**
		 * Methods from MPlugin: Base class for all the plugins
		 */
		void GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics);	
		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		TAny* GetExtension(TUid aExtensionId);
		void Reset();
		// this deletes the instance of this class. The method is called at the user side
		// indirectly through the MPlugin pointer instance.
		void Close();		
		
		/**
		 *  From MMac: MAC interface (Software based)
		 */
	    TPtrC8 MacL(const TDesC8& aMessage);
        void UpdateL(const TDesC8& aMessage);
        TPtrC8 FinalL(const TDesC8& aMessage);
		void ReInitialiseAndSetKeyL(const CKey& aKey);    
		MMac* ReplicateL();
		MMac* CopyL();
	
	private:
	
		/**
		 *  Enumerators to recognize the type of algorithm used for MAC
		 */
		enum TMacBase {EHashBased, ECipherBased};


		/**
		 * Constructors and Destructors
		 */
		~CMacImpl();
    	CMacImpl();
		CMacImpl(const CMacImpl& aMacImpl);
		
		/**
		 * Initialize the 'iHmacImpl' and 'iCipherImpl' instances.
		 */
		void ConstructL(const CKey& aKey, const TUid aImplementationId,const CCryptoParams* aAlgorithmParams);
		
		/**
		 * This will return the Uid of the specific implementation
		 * used for the algorithm.
		 */
		TUid ImplementationUid() const;
	
	private:			
		TUid iImplementationUid;
		TMacBase iBase;
		CKey* iKey;
		
		/**
		 * The hash based MAC implementation. This is software based implementation.
		 */
		CHMacImpl* iHmacImpl;
	
		/**
		 * The symmetric cipher based MAC implementation.
		 */
		CCMacImpl* iCmacImpl;
		};
	}

#endif // __CRYPTOAPI_SOFTWAREMACIMPL_H__
