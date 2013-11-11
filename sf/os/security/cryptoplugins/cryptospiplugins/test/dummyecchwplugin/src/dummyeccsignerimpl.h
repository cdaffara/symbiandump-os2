/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


/**
 @file
 @internalComponent
 @released
*/
#ifndef	CDUMMYECCSIGNERIMPL_H
#define	CDUMMYECCSIGNERIMPL_H
#include <e32base.h>
#include <e32cmn.h>
#include "keys.h"
#include "signerplugin.h"

namespace DummyEccHwCrypto
	{
	using namespace CryptoSpi;

    /**
     * Implements the MSigner interface.
     * 
     * This should be used to create the signer object to perform 
     * signing using ECC.
     */
	NONSHARABLE_CLASS(CDummyECCSignerImpl) : public CBase, public MSigner
		{
	public:
		static CDummyECCSignerImpl* NewL(const CryptoSpi::CKey& aKey, 
			TUid aPaddingMode);
		static CDummyECCSignerImpl* NewLC(const CryptoSpi::CKey& aKey, 
			TUid aPaddingMode);

		// MPlugin Interface
		void Close();
        void Reset();
        void GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics);
        const CExtendedCharacteristics* GetExtendedCharacteristicsL();
        TAny* GetExtension(TUid aExtensionId);
        // End of MPlugin Interface    
        
        // MSignatureBase inteface
		void SetPaddingModeL(TUid aPaddingMode);
		void SetKeyL(const CKey& aPrivateKey);
		TInt GetMaximumInputLengthL() const;
		TInt GetMaximumOutputLengthL() const;
		// End of MSignatureBase inteface
		
		// MSigner interface
		void SignL(const TDesC8& aInput, CCryptoParams& aSignature);

		TUid ImplementationUid() const;
		/// Destructor
		~CDummyECCSignerImpl();

	private:
		CDummyECCSignerImpl(TUid aPaddingMode);
		void ConstructL(const CryptoSpi::CKey& aKey);

	private:
	    /* The key extracted from a CKey object. This would just have 
         * a handle to the key stored in hardware. Hence the actual key 
         * would not be available to the user of this cryptoSPI plugin.
         */
        CKey* iKey;
	    TUid iPaddingMode;
		};
	}

#endif //CDUMMYECCSIGNERIMPL_H
