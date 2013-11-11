/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

/**
@file
@internalComponent
*/
#ifndef NCMENGINE_INL
#define NCMENGINE_INL

//Assumption: NcmEngine & NcmServer are existed simultaneously!!
#ifdef _DEBUG
_LIT(KEnginePanic, "CNcmEnginePanic");
#endif
/**
 * Set IapId and Dhcp Provision Result to SharedState manager.
 */
inline TInt CNcmEngine::SetStateToWatcher(TInt aType, TInt aValue) const
    {
    __ASSERT_DEBUG(iSharedStateManager, User::Panic(KEnginePanic, __LINE__));
    return iSharedStateManager->SetStateValue(aType, aValue);
    }

/**
 * Register the callback of Class Controller to do Dhcp Provision.
 */
inline TInt CNcmEngine::RegisterNotify(const RMessage2& aMsg) const
    {
    __ASSERT_DEBUG(iSharedStateManager, User::Panic(KEnginePanic, __LINE__));
    return iSharedStateManager->RegisterNotify(aMsg);
    }

/*
 * Deregister the callback.
 */
inline TInt CNcmEngine::DeRegisterNotify() const
    {
    __ASSERT_DEBUG(iSharedStateManager, User::Panic(KEnginePanic, __LINE__));
    return iSharedStateManager->DeRegisterNotify();
    }


#endif //NCMENGINE_INL
