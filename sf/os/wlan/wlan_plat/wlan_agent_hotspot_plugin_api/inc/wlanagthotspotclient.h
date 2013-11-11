/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ECOM plugin for hotspot logic
*
*/

/*
* %version: 3 %
*/

#ifndef WLANAGTHOTSPOTCLIENT_H
#define WLANAGTHOTSPOTCLIENT_H

#include <ecom/ecom.h>

/**
* Class for instantiating an implementation of CWlanHotSpotInterface via ECom.
*
* @lib wlanagt.agt
* @since Series 60 3.0
*/
class CWlanAgtHotSpotClient : public CBase
    {
    public:

        /**
        * Static constructor.
        * @return Pointer to the constructed object.
        */
        inline static CWlanAgtHotSpotClient* NewL( );        
        
        /**
        * Destructor.
        */
        virtual ~CWlanAgtHotSpotClient();

	// Dummy functions to interact with Hot spot frame work
	// Plugin implementation will override these functions to actually interact with the HS framework

	virtual void HotSpotStart( const TUint aIapId, TRequestStatus& aStatus ) = 0;
	
	virtual void HotSpotStartAgain( const TUint aIapId, TRequestStatus& aStatus ) = 0;

	virtual void HotSpotCloseConnection( const TUint aIapId, TRequestStatus& aStatus ) = 0;

	virtual void HotSpotCancel( const TUint aIapId ) = 0;

    private:    // Data

        /**
        * Identifies the instance of an implementation created by
        * the ECOM framework.
        */
        TUid iInstanceIdentifier;
    };

#include "wlanagthotspotclient.inl"

#endif // WLANAGTHOTSPOTCLIENT_H
