/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wrapper class for instantiating an implementation of
*                Generic WLAN Plugin via ECom framework.
*
*/

/*
* %version: 3 %
*/

#ifndef WLANPLUGINCLIENT_H
#define WLANPLUGINCLIENT_H

#include <ecom/ecom.h>

/**
* Class for instantiating an implementation via ECom.
*
*/
class CGenericWlanPluginClient : public CBase
    {
public:
    // Constructors and destructor

    /**
     * Static constructor.
     * @param aUid implementation UID to be instantiated.
     * @return Pointer to the constructed object.
     */
    inline static CGenericWlanPluginClient* NewL(const TUid aUid );

    /**
     * Destructor.
     */
    inline virtual ~CGenericWlanPluginClient();

    /**
     * List implementations for Generic WLAN Plugin interface.
     * @param aImplInfoArray Array of implementation informations.
     */
    inline static void ListImplementationsL(
            RImplInfoPtrArray& aImplInfoArray );

private: // Data

    /**
     * Identifies the instance of an implementation created by
     * the ECOM framework.
     */
    TUid iInstanceIdentifier;

    };

#include "wlanpluginclient.inl"

#endif // WLANPLUGINCLIENT_H
