/*
  Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
  All rights reserved.

  This program and the accompanying materials are made available 
  under the terms of the Eclipse Public License v1.0 which accompanies 
  this distribution, and is available at 
  http://www.eclipse.org/legal/epl-v10.html

  Initial Contributors:
  Nokia Corporation - initial contribution.

  Contributors:
*/

#ifndef USBCONTROLXFERIF_H
#define USBCONTROLXFERIF_H

#include <e32def.h>                 // General types definition

/** 
 * Callback interfaces that used to process packet received from Host
 * and interfaces through which that PIL or app can send request to PSL
 */
NONSHARABLE_CLASS(MControlTransferIf)
    {
    public:
        // Interface for RX
        // Transfer direction from Device to Host
        // This two interface is saying PSL had finished
        // sending those data to host.
        virtual void ProcessDataInPacket(TInt aCount,TInt aErrCode) = 0;
        virtual void ProcessStatusInPacket(TInt aErrCode) = 0;
        
        // Transfer direction from Host to Device
        // they are saying that some data had been recieved from host
        virtual void ProcessDataOutPacket(TInt aCount,TInt aErrCode) = 0;
        virtual void ProcessStatusOutPacket(TInt aErrCode) = 0;
        virtual void ProcessSetupPacket(TInt aCount,TInt aErrCode) = 0;
        
        // Interface for TX and Control
        // Data/Status transfer function
        virtual TInt ProcessSetupEndpointZeroRead() = 0;
        virtual TInt ProcessSetupEndpointZeroWrite(const TUint8* aBuffer, TInt aLength, TBool aZlpReqd=EFalse) = 0;
        virtual TInt ProcessSendEp0ZeroByteStatusPacket() = 0;
        virtual TInt ProcessStallEndpoint(TInt aRealEndpoint) = 0;
        
        // Flow control interface
        // In case of we can not deliver received packet(setup or data) to a registered client
        // (because the request callback is not ready), we need PSL stop reporting more packet
        // to PIL until the pending packet had been process.
        // this 2 functions is used to notify PSL: you can continue, the pending packet is proceed.
        virtual void ProcessEp0SetupPacketProceed() = 0;
        virtual void ProcessEp0DataPacketProceed() = 0;
    };

#endif //USBCONTROLXFERIF_H

// End of file

