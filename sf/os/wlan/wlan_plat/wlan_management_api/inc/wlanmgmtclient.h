/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
*                MWlanMgmtInterface via ECom framework.
*
*/

/*
* %version: 7 %
*/

#ifndef WLANMGMTCLIENT_H
#define WLANMGMTCLIENT_H

// INCLUDES
#include <ecom/ecom.h>
#include "wlanmgmtinterface.h"

// CLASS DECLARATION
/**
 * @brief Class for instantiating an implementation of MWlanMgmtInterface via ECom.
 * @since Series 60 3.0
 */
class CWlanMgmtClient : public CBase, public MWlanMgmtInterface
    {
    public:  // Methods

       // Constructors and destructor

        /**
         * Static constructor.
         * @return Pointer to the constructed object.
         */
        inline static CWlanMgmtClient* NewL();

        /**
         * Destructor.
         */
        inline virtual ~CWlanMgmtClient();

    private: // Data

        /**
         * Identifies the instance of an implementation created by
         * the ECOM framework.
         */
        TUid iInstanceIdentifier;
    };

#include "wlanmgmtclient.inl"

#endif // WLANMGMTCLIENT_H
            
// End of File
