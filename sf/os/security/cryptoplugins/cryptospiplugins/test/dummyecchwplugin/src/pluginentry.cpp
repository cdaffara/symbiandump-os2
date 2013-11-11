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


#include "pluginentry.h"
#include "pluginconfig.h"
#include "dummyeccimpl.h"
#include "dummyeccsignerimpl.h"
#include "keys.h"
#include <cryptospi/cryptospidef.h>

using namespace DummyEccHwCrypto;

EXPORT_C const TCharacteristics** CCryptoPluginEntry::Enumerate(
        TUid aInterface, TInt& aNumPlugins)
    {
    const TCharacteristics** ptr(0);

    switch (aInterface.iUid)
        {
        case KAsymmetricCipherInterface:
            {
            aNumPlugins = sizeof(KAsymmetricCipherCharacteristics)
                    / sizeof(TAsymmetricCipherCharacteristics*);
            ptr
                    = (const TCharacteristics**) &KAsymmetricCipherCharacteristics[0];
            }
            break;

        case KSignerInterface:
            {
            aNumPlugins = sizeof(KSignerCharacteristics)
                    / sizeof(TAsymmetricSignatureCharacteristics*);
            ptr = (const TCharacteristics**) &KSignerCharacteristics[0];
            }
            break;

        default:
            aNumPlugins = 0;
        }

    return ptr;
    }

EXPORT_C void CCryptoPluginEntry::GetExtendedCharacteristicsL(
        TUid /* aImplementationUid */, CExtendedCharacteristics*& /* aExt */)
    {
    User::Leave(KErrNotSupported);
    }   

EXPORT_C void CCryptoPluginEntry::CreateAsymmetricCipherL(
        MAsymmetricCipher*& aPlugin, TUid aImplementationId,
        const CKey& aKey, TUid aCryptoMode, TUid aPaddingMode,
        const CCryptoParams* /* aAlgorithmParams */)
    {
    switch (aImplementationId.iUid)
        {
        case KCryptoPluginEccCipher:
            {
            aPlugin = CDummyECCCipherImpl::NewL(aKey, aCryptoMode, aPaddingMode);
            }
            break;

        default:
            User::Leave(KErrNotFound);
        }
    }

EXPORT_C void CCryptoPluginEntry::CreateAsymmetricSignerL(
        MSigner*& aPlugin, TUid aImplementationId,
        const CKey& aKey, TUid aPaddingMode, const CCryptoParams* /* aAlgorithmParams */)
    {
    switch (aImplementationId.iUid)
        {
        case KCryptoPluginEccSigner:
            {
            aPlugin = CDummyECCSignerImpl::NewL(aKey, aPaddingMode);
            }
            break;

        default:
            User::Leave(KErrNotFound);
        }
    }

EXPORT_C void CCryptoPluginEntry::CreateAsymmetricVerifierL(
        MVerifier*& /*aPlugin*/, TUid /*aImplementationId*/,
        const CKey& /*aKey*/, TUid /*aPaddingMode*/, const CCryptoParams* /*aAlgorithmParams*/)
    {
    User::Leave(KErrNotFound);
    }

EXPORT_C void CCryptoPluginEntry::CreateHashL(MHash*& /*aPlugin*/,
        TUid /*aImplementationId*/, TUid /*aOperationMode*/,
        const CKey* /*aKey*/, const CCryptoParams* /*aAlgorithmParams*/)
    {
    User::Leave(KErrNotFound);
    }

EXPORT_C void CCryptoPluginEntry::CreateKeyAgreementL(
        MKeyAgreement*& /*aPlugin*/, TUid /*aImplementationId*/,
        const CKey& /*aPrivateKey*/, const CCryptoParams* /*aAlgorithmParams*/)
    {
    User::Leave(KErrNotFound);
    }

EXPORT_C void CCryptoPluginEntry::CreateKeyPairGeneratorL(
        MKeyPairGenerator*& /*aPlugin*/, TUid /*aImplementationId*/,
        const CCryptoParams* /*aAlgorithmParams*/)
    {
    User::Leave(KErrNotFound);
    }

EXPORT_C void CCryptoPluginEntry::CreateRandomL(MRandom*& /*aPlugin*/,
        TUid /*aImplementationId*/, const CCryptoParams* /*aAlgorithmParams*/)
    {
    User::Leave(KErrNotFound);
    }

EXPORT_C void CCryptoPluginEntry::CreateSymmetricCipherL(
        MSymmetricCipher*& /*aPlugin*/, TUid /*aImplementationId*/, const CKey& /*aKey*/,
        TUid /*aCryptoMode*/, TUid /*aOperationMode*/, TUid /*aPadding*/,
        const CCryptoParams* /*aAlgorithmParams*/)
    {
    User::Leave(KErrNotSupported);
    }
// End of file
