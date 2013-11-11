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
#ifndef CCRYPTOAPI_DUMMYECCHWPLUGINCONFIG_H
#define CCRYPTOAPI_DUMMYECCHWPLUGINCONFIG_H

#include <cryptospi/cryptoparams.h>
#include <e32cmn.h>
#include <cryptospi/romlit.h>
#include "cryptospi/cryptospidef.h"

namespace DummyEccHwCrypto
    {
    using namespace CryptoSpi;
    /**
     Creator Name
     */
    _ROMLIT16(KNokia, "Nokia");
    /**
     implementation Name
     */
    _ROMLIT16(KEccCipherDescription, "Ecc");
    _ROMLIT16(KEccSignerDescription, "EccSigner");

    /**
     ECC Cipher Characteristics
     */
    static const TInt32 KCryptoPluginEccCipher = 0xA000D697;
    static const TUid KCryptoPluginEccCipherUid =
        {
        KCryptoPluginEccCipher
        };
    static const TInt32 KECCCipherPaddingModes[] =
        {
        KPaddingModeNone
        };

    static const TAsymmetricCipherCharacteristics KECC_1 =
        {
        KAsymmetricCipherInterface, /* iInterfaceUID */
        KAlgorithmCipherEcc, /* iAlgorithmUID */
        KCryptoPluginEccCipher, /* iImplementationUID */
        &KNokia, /* iCreatorName */
        EFalse, /* iIsFIPSApproved */
        ETrue, /* iIsHardwareSupported */
        6, /* iMaxConcurrencySupported */
        &KEccCipherDescription, /* iAlgorithmName */
        100, /* iLatency */
        90, /* iThroughput */
        360, /* iMaximumKeyLength */
        KECCCipherPaddingModes, /* iSupportedPaddingModes */
        sizeof(KECCCipherPaddingModes) / sizeof(KECCCipherPaddingModes[0]), /* iPaddingModeNum */
        3
        /* iKeySupportMode */
        };

    /**
     ECC Signer Characteristics
     */
    static const TInt32 KCryptoPluginEccSigner = 0xA000D698;
    static const TUid KCryptoPluginEccSignerUid =
        {
        KCryptoPluginEccSigner
        };
    static const TInt32 KECCSignerPaddingModes[] =
        {
        KPaddingModeNone
        };

    static const TAsymmetricSignatureCharacteristics KECCSigner_1 =
        {
        KSignerInterface, /* iInterfaceUID */
        KAlgorithmSignerEcc, /* iAlgorithmUID */
        KCryptoPluginEccSigner, /* iImplementationUID */
        &KNokia, /* iCreatorName */
        EFalse, /* iIsFIPSApproved */
        ETrue, /* iIsHardwareSupported */
        6, /* iMaxConcurrencySupported */
        &KEccSignerDescription, /* iAlgorithmName */
        100, /* iLatency */
        90, /* iThroughput */
        360, /* iMaximumKeyLength */
        KECCCipherPaddingModes, /* iSupportedPaddingModes */
        sizeof(KECCCipherPaddingModes) / sizeof(KECCCipherPaddingModes[0]), /* iPaddingModeNum */
        3
        /* iKeySupportMode */
        };

    /**
     For each crypto algorithm type implemented in this plugin list the characteristics
     */

    /**
     Asymmetric Cipher implementation characteristic table
     */
    static const TAsymmetricCipherCharacteristics
            * const KAsymmetricCipherCharacteristics[] =
                {
                &KECC_1
                };

    /**
     Signer implementation characteristic table
     */
    static const TAsymmetricSignatureCharacteristics
            * const KSignerCharacteristics[] =
                {
                &KECCSigner_1
                };

    }

#endif //CCRYPTOAPI_DUMMYECCHWPLUGINCONFIG_H
