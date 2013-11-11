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


#include "dummyeccsignerimpl.h"
#include "keys.h"
#include "pluginconfig.h"
#include "cryptospihai.h"

using namespace DummyEccHwCrypto;
using namespace CryptoSpiHai;

const TInt KMaxSignerOutputLength = 50;
const TInt KMaxSignerInputLength = 50;

// CDummyECCSignerImpl
CDummyECCSignerImpl* CDummyECCSignerImpl::NewL(const CKey& aKey,
        TUid aPaddingMode)
    {
    CDummyECCSignerImpl* self =
            CDummyECCSignerImpl::NewLC(aKey, aPaddingMode);
    CleanupStack::Pop(self);
    return self;
    }

CDummyECCSignerImpl* CDummyECCSignerImpl::NewLC(const CKey& aKey,
        TUid aPaddingMode)
    {
    CDummyECCSignerImpl* self =
            new (ELeave) CDummyECCSignerImpl(aPaddingMode);
    CleanupStack::PushL(self);
    self->ConstructL(aKey);
    return self;
    }

CDummyECCSignerImpl::CDummyECCSignerImpl(TUid aPaddingMode) :
    iPaddingMode(aPaddingMode)
    {
    }

CDummyECCSignerImpl::~CDummyECCSignerImpl()
    {
    delete iKey;
    }

void CDummyECCSignerImpl::ConstructL(const CKey& aKey)
    {
    SetKeyL(aKey);
    }

// MPlugin Interface
void CDummyECCSignerImpl::Close()
    {
    delete this;
    }
void CDummyECCSignerImpl::Reset()
    {
    }
void CDummyECCSignerImpl::GetCharacteristicsL(
        const TCharacteristics*& aPluginCharacteristics)
    {
    TInt numCiphers = sizeof(KSignerCharacteristics)
            / sizeof(TAsymmetricSignatureCharacteristics*);
    TInt32 implUid = ImplementationUid().iUid;
    for (TInt i = 0; i < numCiphers; ++i)
        {
        if (KSignerCharacteristics[i]->cmn.iImplementationUID == implUid)
            {
            aPluginCharacteristics = KSignerCharacteristics[i];
            break;
            }
        }
    }

const CExtendedCharacteristics* CDummyECCSignerImpl::GetExtendedCharacteristicsL()
    {
    // All Symbian software plug-ins have unlimited concurrency, cannot be reserved
    // for exclusive use and are not CERTIFIED to be standards compliant.
    return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
    }

TAny* CDummyECCSignerImpl::GetExtension(TUid /* aExtensionId */)
    {
    return 0;
    }
// End of MPlugin Interface

// MSignatureBase Interface
void CDummyECCSignerImpl::SetPaddingModeL(TUid /* aPaddingMode */)
    {
    User::Leave(KErrNotSupported);
    }

void CDummyECCSignerImpl::SetKeyL(const CKey& aKey)
    {
    // delete any previous key and recreate the key
    delete iKey;
    iKey = NULL;
    iKey = CKey::NewL(aKey);
    }

TInt CDummyECCSignerImpl::GetMaximumInputLengthL() const
    {
    return KMaxSignerInputLength;
    }

TInt CDummyECCSignerImpl::GetMaximumOutputLengthL() const
    {
    return KMaxSignerOutputLength;
    }

TUid CDummyECCSignerImpl::ImplementationUid() const
    {
    return KCryptoPluginEccSignerUid;
    }

void CDummyECCSignerImpl::SignL(const TDesC8& aInput,
        CCryptoParams& aSignature)
    {
    if (iKey->IsPresent(KPassedHandleToKeyUid))
        {
        const TInt keyHandle = iKey->GetTIntL(KPassedHandleToKeyUid);

        // Invoke the Spi HAI to perform the operation
        CCryptoSpiHai::SignL(keyHandle, aInput, aSignature);
        }
    else
        {
        User::Leave(KErrNotSupported);
        }
    }

// End of file
