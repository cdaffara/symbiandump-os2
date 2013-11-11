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
*                MWlanEapolInterface via ECom framework.
*
*/

/*
* %version: 4 %
*/

#ifndef WLANEAPOLCLIENT_H
#define WLANEAPOLCLIENT_H

#include <ecom/ecom.h>
#include "wlaneapolinterface.h"

const TInt KCWlanEapolClientUid = 0x2000B05C;
const TInt KCWlanWapiClientUid = 0x200195A1;

/**
 * @brief Class for instantiating an implementation of MWlanEapolInterface via ECom.
 *
 * @since S60 v3.2
 */
class CWlanEapolClient : public CBase, public MWlanEapolInterface
    {

public:

    /**
     * Static constructor.
     * @param aUid UID of implementation to instantiate.
     * @param aPartner Pointer to callback instance.
     * @return Pointer to the constructed instance.
     */
    inline static CWlanEapolClient* NewL(
        TInt aUid,
        MWlanEapolCallbackInterface* aPartner );

    /**
     * Destructor.
     */
    inline virtual ~CWlanEapolClient();

private: // data

    /**
     * Identifies the instance of an implementation created by
     * the ECOM framework.
     */
    TUid iInstanceIdentifier;

    };

#include "wlaneapolclient.inl"

#endif // WLANEAPOLCLIENT_H
