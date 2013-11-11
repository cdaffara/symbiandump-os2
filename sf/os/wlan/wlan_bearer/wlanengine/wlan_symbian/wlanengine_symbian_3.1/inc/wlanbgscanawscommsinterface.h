/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This abstract class defines WLAN scan provider interface.
*
*/

/*
* %version: 3 %
*/


#ifndef WLANBGSCANAWSCOMMSINTERFACE_H
#define WLANBGSCANAWSCOMMSINTERFACE_H

#include <e32def.h>
#include <e32base.h>

/**
 * @brief Interface definition for WLAN Scan Result Provider API.
 *
 * This class defines the methods used for issuing and cancelling Scan requests.
 *
 * @since S60 v5.2
 */
class MWlanBgScanAwsComms
    {
    
public:
    
    virtual ~MWlanBgScanAwsComms(){};
    
    /**
     * AWS commands.
     */
    enum TAwsCommand
        {
        EStart = 0,
        EStop,
        ESetPowerSaveMode,
        EAwsStartupCompleted,
        EAwsCommandMax //not a real command
        };
    
    /**
     * AWS message.
     */
    struct TAwsMessage
        {
        TAwsCommand iCmd;
        TInt iParameter;
        };

    /**
     * Send or queue a message to be sent to AWS.
     *
     * @param aMessage Message to be sent
     * @since S60 v5.2
     */
    virtual void SendOrQueueAwsCommand( TAwsMessage& aMessage ) = 0;

    };


// forward declarations
class CWlanBgScan;

class CWlanBgScanAwsCommsFactory : public CBase
    {
public:
    static MWlanBgScanAwsComms* InstanceL( CWlanBgScan& aBgScan );
    };



#endif // WLANBGSCANAWSCOMMSINTERFACE_H
