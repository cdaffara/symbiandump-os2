/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the MWlanNullSender class.
*
*/

/*
* %version: 5 %
*/

#ifndef WLAN_NULL_SENDER_H
#define WLAN_NULL_SENDER_H

/**
 *  Null Data Frame Sender interface
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.2
 */
class MWlanNullSender 
    {

public:

    /**
     * Request to send a Null Data Frame
     *
     * @since S60 v3.2
     * @param aCtxImpl statemachine context
     * @param aQosNull ETrue if a QoS Null Data frame should be transmitted
     *                 EFalse if a regular Null Data frame should be transmitted
     * @return ETrue if the send request was successfully submitted
     *         EFalse otherwise
     */
    virtual TBool TxNullDataFrame( 
        WlanContextImpl& aCtxImpl,
        TBool aQosNull ) = 0;

    };

#endif // WLAN_NULL_SENDER_H
