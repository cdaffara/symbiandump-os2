/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of TWlanUserTxDataCntx class.
*
*/

/*
* %version: 8 %
*/

#ifndef T_WLANUSERTXDATACNTX_H
#define T_WLANUSERTXDATACNTX_H

#include "802dot11.h"

class TDataBuffer;

/**
 *  Encapsulates a frame transmit buffer
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class TWlanUserTxDataCntx 
    {

public:

    TWlanUserTxDataCntx () : 
        iStartOfFrame( 0 ), 
        iLengthOfData( 0 ), 
        iEthernetBuffer( NULL ) {}

    inline TUint8* StartOfFrame();

    inline void LengthOfFrame( TUint aFrameLength );

    inline TUint LengthOfFrame() const;

    inline void Dot11FrameReady( TUint8* aStartOfFrame, TUint aLengthOfFrame );

private: // data

    /** pointer to the beginning of the ready to be sent 802.11 frame */ 
    TUint8*             iStartOfFrame;
    /**
     * length of the data in the buffer
     */
    TUint               iLengthOfData;
    /**
     * data buffer that holds the ethernet II critter to send next
     * its waiting for dot11 encapulation here
     */
    TDataBuffer*        iEthernetBuffer;
    };

#include "umacusertxdatacntx.inl"

#endif // T_WLANUSERTXDATACNTX_H
