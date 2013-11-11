/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*                MWlanControlInterface via ECom framework.
*
*/

/*
* %version: 3 %
*/

#ifndef WLANCONTROLCLIENT_H
#define WLANCONTROLCLIENT_H

#include <ecom/ecom.h>
#include "wlancontrolinterface.h"

/**
 * @brief Class for instantiating an implementation of MWlanControlInterface via ECom.
 *
 * @since S60 v3.2
 */
class CWlanControlClient : public CBase, public MWlanControlInterface
    {

public:

    /**
     * Static constructor.
     * @return Pointer to the constructed instance.
     */
    inline static CWlanControlClient* NewL();

    /**
     * Destructor.
     */
    inline virtual ~CWlanControlClient();

private: // data

    /**
     * Identifies the instance of an implementation created by
     * the ECOM framework.
     */
    TUid iInstanceIdentifier;

    };

#include "wlancontrolclient.inl"

#endif // WLANCONTROLCLIENT_H
