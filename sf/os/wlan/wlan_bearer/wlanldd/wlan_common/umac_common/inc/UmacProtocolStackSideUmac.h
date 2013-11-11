/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the MWlanProtocolStackSideUmac class.
*
*/

/*
* %version: 9 %
*/

#ifndef WLANPROTOCOLSTACKSIDEUMAC_H
#define WLANPROTOCOLSTACKSIDEUMAC_H

struct TMacAddress;
class TDataBuffer;

/**
*  
*/
class MWlanProtocolStackSideUmac
    {
public:

    virtual ~MWlanProtocolStackSideUmac() {};

    virtual const TMacAddress& StationId() const = 0;

    /**
    * Triggers the setting of the Tx offset for every frame type which can be
    * transmitted
    */
    virtual void SetTxOffset() = 0;

    /**
    * Transmit a protocol stack frame
    * 
    * The frame to be sent needs to be in 802.3 format
    * @param aDataBuffer meta header of the frame to be transmitted
    * @param aMore ETrue if another frame is also ready to be transmitted
    *              EFalse otherwise
    */
    virtual void TxProtocolStackData( 
        TDataBuffer& aDataBuffer,
        TBool aMore ) = 0;

    /**
    * Gets the WLAN vendor needs for extra space (bytes) in frame buffers
    *  
    * @param aRxOffset How much extra space needs to be reserved
    *        in the Rx buffer before every Rx frame that is received from the
    *        WHA layer.
    * @param aTxHeaderSpace How much extra space needs to be reserved
    *        in the Tx buffer before every Tx frame that is given to the 
    *        WHA layer.
    * @param aTxTrailerSpace How much extra space needs to be reserved
    *        in the Tx buffer after every Tx frame that is given to the 
    *        WHA layer.
    */
    virtual void GetFrameExtraSpaceForVendor( 
        TUint8& aRxOffset,
        TUint8& aTxHeaderSpace,
        TUint8& aTxTrailerSpace ) const = 0 ;

    /**
     * Is protocol stack side transmission permitted
     * UMAC adaptation needs to call this method every time before calling
     * TxProtocolStackData() to check if it is allowed to call that
     * method!
     *
     * @param aTxQueueState If the return value is ETrue, the state 
     *        (full / not full) of every WHA transmit queue. Otherwise, not
     *        valid.
     *        Note! A frame shall not be submitted to a full queue - even if
     *        the return value would be ETrue
     * @return ETrue if Tx frame submission is permitted
     *         EFalse if Tx frame submission is not permitted
     */
    virtual TBool TxPermitted( TWhaTxQueueState& aTxQueueState ) const = 0;    

    /**
     * Is user data Tx enabled
     *
     * @return ETrue if user data Tx is enabled
     *         EFalse if user data Tx is not enabled
     */
    virtual TBool UserDataTxEnabled() const = 0;
    };

#endif      // WLANPROTOCOLSTACKSIDEUMAC_H
