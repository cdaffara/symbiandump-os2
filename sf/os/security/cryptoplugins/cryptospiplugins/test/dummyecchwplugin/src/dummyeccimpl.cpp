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


#include "dummyeccimpl.h"

#include <e32def.h>
#include <cryptospi/cryptospidef.h>
#include "keys.h"
#include <cryptospi/plugincharacteristics.h>
#include "pluginconfig.h"
#include "cryptospihai.h"

using namespace DummyEccHwCrypto;
using namespace CryptoSpiHai;

/**
 * These are just randomly selected numbers. There is no logic behind 
 * their values.
 */ 
const TInt KMaxOutputLength = 50;
const TInt KMaxInputLength = 50;

CDummyECCCipherImpl* CDummyECCCipherImpl::NewL(const CKey& aKey,
        TUid aCryptoMode, TUid aPaddingMode)
    {
    CDummyECCCipherImpl* self = CDummyECCCipherImpl::NewLC(aKey, aCryptoMode,
            aPaddingMode);
    CleanupStack::Pop(self);
    return self;
    }

CDummyECCCipherImpl* CDummyECCCipherImpl::NewLC(const CKey& aKey,
        TUid aCryptoMode, TUid aPaddingMode)
    {
    CDummyECCCipherImpl* self = new (ELeave) CDummyECCCipherImpl(aCryptoMode,
            aPaddingMode);
    CleanupStack::PushL(self);
    self->ConstructL(aKey);
    return self;
    }

CDummyECCCipherImpl::CDummyECCCipherImpl(TUid aCryptoMode, TUid aPaddingMode) :
    iCryptoMode(aCryptoMode), iPaddingMode(aPaddingMode)
    {
    }

void CDummyECCCipherImpl::ConstructL(const CKey& aKey)
    {
    SetKeyL(aKey);
    }

// MPlugin Interface Start
void CDummyECCCipherImpl::Close()
    {
    delete this;
    }

void CDummyECCCipherImpl::Reset()
    {
    }

void CDummyECCCipherImpl::GetCharacteristicsL(
        const TCharacteristics*& aPluginCharacteristics)
    {
    TInt numCiphers = sizeof(KAsymmetricCipherCharacteristics)
            / sizeof(TAsymmetricCipherCharacteristics*);
    TInt32 implUid = ImplementationUid().iUid;
    for (TInt i = 0; i < numCiphers; ++i)
        {
        if (KAsymmetricCipherCharacteristics[i]->cmn.iImplementationUID
                == implUid)
            {
            aPluginCharacteristics = KAsymmetricCipherCharacteristics[i];
            break;
            }
        }
    }

const CExtendedCharacteristics* CDummyECCCipherImpl::GetExtendedCharacteristicsL()
    {
    // All Symbian software plug-ins have unlimited concurrency, cannot be reserved
    // for exclusive use and are not CERTIFIED to be standards compliant.
    return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
    }

TAny* CDummyECCCipherImpl::GetExtension(TUid /* aExtensionId */)
    {
    return 0;
    }
// End of MPlugin Interface

// MAsymmetricCipherBase Interface
void CDummyECCCipherImpl::SetKeyL(const CKey& aKey)
    {
    // delete any previous key and recreate the key
    delete iKey;
    iKey = NULL;
    iKey = CKey::NewL(aKey);
    }

void CDummyECCCipherImpl::SetCryptoModeL(TUid aCryptoMode)
    {
    switch (aCryptoMode.iUid)
        {
        case KCryptoModeEncrypt:
        case KCryptoModeDecrypt:
            break;
        default:
            User::Leave(KErrNotSupported);
        }
    iCryptoMode = aCryptoMode;
    }

void CDummyECCCipherImpl::SetPaddingModeL(TUid /* aPaddingMode */)
    {
    User::Leave(KErrNotSupported);
    }

TInt CDummyECCCipherImpl::GetMaximumInputLengthL() const
    {
    return KMaxInputLength;
    }

TInt CDummyECCCipherImpl::GetMaximumOutputLengthL() const
    {
    return KMaxOutputLength;
    }
// End of MAsymmetricCipherBase Interface

// MAsymmetricCipher Interface
void CDummyECCCipherImpl::ProcessL(const TDesC8& aInput, TDes8& aOutput)
    {
    if (iCryptoMode.iUid == KCryptoModeEncrypt)
        {
        EncryptL(aInput, aOutput);
        }
    else
        {
        DecryptL(aInput, aOutput);
        }
    }

TUid CDummyECCCipherImpl::ImplementationUid() const
    {
    return KCryptoPluginEccCipherUid;
    }

CDummyECCCipherImpl::~CDummyECCCipherImpl()
    {
    delete iKey;
    }

void CDummyECCCipherImpl::EncryptL(const TDesC8& /* aInput */, TDes8& /* aOuput */)
    {
    User::Leave(KErrNotSupported);
    }

void CDummyECCCipherImpl::DecryptL(const TDesC8& aInput, TDes8& aOutput)
    {
    if (iKey->IsPresent(KPassedHandleToKeyUid))
        {
        const TInt& keyHandle = iKey->GetTIntL(KPassedHandleToKeyUid);

        // Invoke the Spi HAI to perform the operation
        CCryptoSpiHai::DecryptL(keyHandle, aInput, aOutput);
        }
    else
        {
        User::Leave(KErrNotSupported);
        }
    }
// End of file


