/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include "tsignatureinput.h"

/* CHashingSignatureInput */

CHashingSignatureInput* CHashingSignatureInput::NewL(
    CMessageDigest::THashId aHashId)
    {
    CHashingSignatureInput* self = NewLC(aHashId);
    CleanupStack::Pop(self);
    return self;
    }

CHashingSignatureInput* CHashingSignatureInput::NewLC(
    CMessageDigest::THashId aHashId)
    {
    CHashingSignatureInput* self = new(ELeave)CHashingSignatureInput;
    CleanupStack::PushL(self);
    self->ConstructL(aHashId);
    return self;
    }

void CHashingSignatureInput::Update(const TDesC8& aInput)
    {
    iHash->Update(aInput);
    }

TPtrC8 CHashingSignatureInput::Final(void)
    {
    return iHash->Final();
    }

CHashingSignatureInput::CHashingSignatureInput(void)
    {
    }

CHashingSignatureInput::~CHashingSignatureInput(void)
    {
    delete iHash;
    }

void CHashingSignatureInput::ConstructL(CMessageDigest::THashId aHashId)
    {
    iHash = CMessageDigestFactory::NewDigestL(aHashId);
    }

