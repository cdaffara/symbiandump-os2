/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Empty implementation of the default HtiSecurityManager
*
*/


#include "HtiSecurityManager.h"

CHtiSecurityManager* CHtiSecurityManager::NewL()
    {
    CHtiSecurityManager* obj = NewLC();
    CleanupStack::Pop();
    return obj;
    }

CHtiSecurityManager* CHtiSecurityManager::NewLC()
    {
    CHtiSecurityManager* obj = new (ELeave) CHtiSecurityManager;
    CleanupStack::PushL(obj);
    obj->ConstructL();
    return obj;
    }

CHtiSecurityManager::CHtiSecurityManager()
    {
    iSecurityContext = EFalse;
    }

void CHtiSecurityManager::ConstructL()
    {
    }

CHtiSecurityManager::~CHtiSecurityManager()
    {
    }

TDesC8* CHtiSecurityManager::SetSecurityContext(const TDesC8& /*aToken*/)
    {
    iSecurityContext = ETrue;
    HBufC8* replyToken = HBufC8::NewL(0);
    return replyToken; //null token
    }

void CHtiSecurityManager::ResetSecurityContext()
    {
    iSecurityContext = EFalse;
    }

TBool CHtiSecurityManager::IsContextEstablashed() const
    {
    return iSecurityContext;
    }

TDesC8* CHtiSecurityManager::WrapL(const TDesC8& aMessage, TBool /*aEncrypt*/)
    {
    //just copy message
    HBufC8* wrapped = HBufC8::NewLC( aMessage.Size() );
    (*wrapped) = aMessage;
    CleanupStack::Pop();
    return wrapped;
    }

TDesC8* CHtiSecurityManager::UnwrapL(const TDesC8& aMessage)
    {
    //just copy message
    HBufC8* unwrapped = HBufC8::NewLC( aMessage.Size() );
    (*unwrapped) = aMessage;
    CleanupStack::Pop();
    return unwrapped;
    }

TPtrC8 CHtiSecurityManager::MIC(const TDesC8& /*aMessage*/) const
    {
    return TPtrC8(KNullDesC8);
    }

