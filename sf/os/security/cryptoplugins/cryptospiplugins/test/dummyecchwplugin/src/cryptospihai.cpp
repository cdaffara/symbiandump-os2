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


#include "cryptospihai.h"
#include "keys.h"
#include <e32def.h>
#include <cryptospi/cryptohashapi.h>
#include <pbedata.h>
#include "tcryptotokenhai.h"

using namespace CryptoSpiHai;

/**
 * Performs the signing operation.
 * 
 * A cryptoSPI plugin uses this, when it does not have access to the  
 * actual key.
 * 
 * @param aKeyHandle The key handle retrieved from hw crypto 
 * token
 * @param aInput The text which has to be signed. This is not being 
 * used due to signing logic used in this function.
 * @param aSignature Output param. The cryptoSPI signature.
 * 
 * @leave Can leave with all the leave codes present in HAI of 
 * reference crypto token implementation.
 * 
 * @note This function does not actually implement ECC signing. This 
 * function just shows how the private key can be extracted from 
 * crypto token hai. This function just returns the private key as 
 * output signature. The caller can verify the signature by ensuring 
 * that test case has same public and private keys and then comparing 
 * the signature with public key.
 */
EXPORT_C void CCryptoSpiHai::SignL(TInt aKeyHandle,
        const TDesC8& /*aInput*/, CryptoSpi::CCryptoParams& aSignature)
    {
    MCTToken* token = NULL;
    /**
     * We are dereferencing a NULL pointer below. We need to pass 
     * MCTToken here. It is not used currently.
     */
    CCryptoTokenHai *cryptoTokenHai = CCryptoTokenHai::NewLC(token);

    //Call Crypto Token HAI to get the actual key
    HBufC8* actualKey = NULL;
    cryptoTokenHai->ExportPrivateKeyL(aKeyHandle, actualKey);
    CleanupStack::PushL(actualKey);

    aSignature.AddL(*actualKey, CryptoSpi::KEccKeyTypeUid);
    CleanupStack::PopAndDestroy(actualKey);
    CleanupStack::PopAndDestroy(cryptoTokenHai);
    }

/**
 * Performs the decryption operation.
 * 
 * A cryptoSPI plugin uses this, when it does not have access to the 
 * actual key.
 * 
 * @param aKeyHandle The key handle retrieved from hw crypto token
 * @param aInput The cipher text. This is not being used due to signing  
 * logic used in this function.
 * @param aOutput Output param. The decrypted plain text
 * 
 * @leave Can leave with all the leave codes present in HAI of 
 * reference crypto token implementation.
 *
 * @note This function does not actually implement ECC decryption. This 
 * function just shows how the private key can be extracted from 
 * crypto token hai. This function just returns the private key as 
 * decrypted text. The caller can verify the decryption by ensuring 
 * that test case has same public and private keys and then comparing 
 * the decrypted text with public key. 
 */
EXPORT_C void CCryptoSpiHai::DecryptL(TInt aKeyHandle,
        const TDesC8& /*aInput*/, TDes8& aOutput)
    {
    MCTToken* token = NULL;
    CCryptoTokenHai *cryptoTokenHai = CCryptoTokenHai::NewLC(token);

    //Call Crypto Token HAI to get the actual key
    HBufC8* actualKey = NULL;
    cryptoTokenHai->ExportPrivateKeyL(aKeyHandle, actualKey);
    CleanupStack::PushL(actualKey);
    aOutput.Copy(*actualKey);

    CleanupStack::PopAndDestroy(actualKey);
    CleanupStack::PopAndDestroy(cryptoTokenHai);
    }

// End of file


