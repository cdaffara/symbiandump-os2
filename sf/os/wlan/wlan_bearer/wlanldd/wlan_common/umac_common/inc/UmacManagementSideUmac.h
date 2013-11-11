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
* Description:   Declaration of the MWlanManagementSideUmac class.
*
*/

/*
* %version: 15 %
*/

#ifndef WLANMANAGEMENTSIDEUMAC_H
#define WLANMANAGEMENTSIDEUMAC_H

#include "umacoidmsg.h"

#ifndef RD_WLAN_DDK
#include <wha.h>
#else
#include <wlanwha.h>
#endif

struct SOidMsgStorage;

/**
*  
*/
class MWlanManagementSideUmac
    {
public:

    virtual ~MWlanManagementSideUmac() {};

    /**
    * OID request handler routine
    * @param aOutputBuffer Pointer to the output buffer (query-operations)
    * @param aOutputBufferSize Size of the buffer pointed to by aOutputBuffer
    * @return Status of the operation see #TStatus
    */
    virtual void HandleOid(
        const TOIDHeader* aOid,             
        TAny* aOutputBuffer, 
        TUint aOutputBufferSize) = 0;

    virtual void AttachWsa( WHA::Wha* aWha ) = 0;

    virtual TBool Init() = 0;

    virtual void BootUp( 
        const TUint8* aPda, 
        TUint32 aPdaLength,
        const TUint8* aFw, 
        TUint32 aFwLength ) = 0;

    virtual void FinitSystem() = 0;
    
    /**
    * Write management frame.
    * @param aDataBuffer meta header of the frame to be transmitted
    * @return KErrNone
    */
    virtual void WriteMgmtFrame(TDataBuffer& aDataBuffer) = 0;

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
    };

#endif      // WLANMANAGEMENTSIDEUMAC_H
