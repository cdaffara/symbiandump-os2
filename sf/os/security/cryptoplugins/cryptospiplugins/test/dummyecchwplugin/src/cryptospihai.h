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
#ifndef	CCRYPTOSPIHAI_H
#define	CCRYPTOSPIHAI_H

#include <e32base.h>
#include <e32cmn.h>
#include <cryptospi/cryptospidef.h>
#include <cryptospi/cryptoparams.h>

namespace CryptoSpiHai
    {
    /**
     * This class forms the hardware abstraction interface part of the 
     * cryptoSPI. In the production code, this should be replaced by a 
     * device driver. This layer abstracts the hardware from a cryptoSPI
     * plugin. It interacts with the HAI of crypto token to get the actual 
     * key and performs the operations. This layer should be implemented in
     * kernel space.
     */
    NONSHARABLE_CLASS(CCryptoSpiHai) : public CBase
        {
    public:
        /**
         * Performs the signing operation.
         * 
         * A cryptoSPI plugin uses this, when it does not have access 
         * to the actual key.
         * 
         * @param aKeyHandle The key handle retrieved from hw crypto 
         * token
         * @param aInput The text which has to be signed.
         * @param aSignature Output param. The cryptoSPI signature.
         * 
         * @leave Can leave with all the leave codes present in HAI of 
         * reference crypto token implementation. 
         */
        IMPORT_C static void SignL(TInt aKeyHandle,
                const TDesC8& aInput, CryptoSpi::CCryptoParams& aSignature);
        
        /**
         * Performs the decryption operation.
         * 
         * A cryptoSPI plugin uses this, when it does not have access 
         * to the actual key.
         * 
         * @param aKeyHandle The key handle retrieved from hw crypto 
         * token
         * @param aInput The cipher text.
         * @param aOutput Output param. The decrypted plain text
         * 
         * @leave Can leave with all the leave codes present in HAI of 
         * reference crypto token implementation. 
         */        
        IMPORT_C static void DecryptL(TInt aKeyHandle,
                const TDesC8& aInput, TDes8& aOuput);
        };
    }

#endif	//	CCRYPTOSPIHAI_H
