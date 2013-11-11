/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class to receive asynchronous notifications from WLAN drivers.
*
*/


#ifndef WLANNOTIFICATIONHANDLER_H
#define WLANNOTIFICATIONHANDLER_H

#include <e32base.h>

#include "umac_types.h"
#include "am_platform_libraries.h"

class RWlanLogicalChannel;

/**
 * Callback interface
 */
class MWlanNotificationCallback
    {
    public:
        virtual void OnNotify(const TIndication& aIndication) = 0;
    };

/**
 * Active Object that receives notifications from driver side
 * @since S60 v.3.0
 * @lib wlmserversrv.lib
 */
NONSHARABLE_CLASS( CWlanNotificationHandler ) : public CActive
    {
public: // Constructors and destructor

    static CWlanNotificationHandler* NewL(
        RWlanLogicalChannel& aChannel,
        MWlanNotificationCallback& aClient );

    /**
     * C++ default constructor.
     * @param aService Interface to request notifirations.
     * @param aClient  Interface to forward notifications.
     */
    CWlanNotificationHandler( 
        RWlanLogicalChannel& aChannel,
        MWlanNotificationCallback& aClient );

    /**
     * Destructor.
     */
    virtual ~CWlanNotificationHandler();

public: // New methods

    /**
     * Activate the handler
     * Handler starts to listen for notifications
     * @since S60 v.3.1
     * @return error code
     */
    TInt Start();
    
    /**
     * Deactivate the handler
     * Handler stops to listen for received management frames
     * @since S60 v.3.1
     */
    void Stop();

public: // Methods from base classes

    /**
     * From CActive
     * Cancel waiting.
     */
    void DoCancel();

    /**
     * From CActive 
     * Receive an event..
     */
    void RunL();

protected:

    /**
     * ConstructL
     */
    void ConstructL();

    /**
     * Wait for an event
     */
    void IssueRequest();
    
private: // data

    /**
     * Provider of the asynchronous service
     */
    RWlanLogicalChannel& iChannel;

    /**
     * Reference to the client
     */
    MWlanNotificationCallback& iClient;

    /**
     * The indication message to be sent to the client
     */
    TIndication iIndication;

    };

#endif // WLANNOTIFICATIONHANDLER_H
