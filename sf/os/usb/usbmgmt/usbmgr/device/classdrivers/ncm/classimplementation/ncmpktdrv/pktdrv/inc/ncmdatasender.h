/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


/**
@file
@internalComponent
*/


#ifndef NCMDATASENDER_H
#define NCMDATASENDER_H

#include <e32base.h>

#ifndef __OVER_DUMMYUSBSCLDD__
#include <d32usbcsc.h>
#else
#include <dummyusbsclddapi.h>
#include <usb/testncm/ethinter.h>
#endif

#include "ncmntbbuilder.h"
#include "ncmpktdrvcommon.h"


class CNcmDataInterface;
class RDevUsbcScClient;
class CNcmBufferManager;
class CNcmNtbBuildPolicy;


NONSHARABLE_CLASS(MNcmNtbBuildObserver)
    {
public:    
    /**
    * send a NTB out, this is interface with CNtbBuilder
    *
    * @param aBuffer contain a whole NTB.
    */
    virtual void SendNtbPayload(TNcmBuffer& aBuffer) = 0;
    };



/**
Responsible for sending Ncm packet data
*/

NONSHARABLE_CLASS(CNcmDataSender) : public CActive, 
                                    public MNcmNtbBuildObserver

    {
public:
    static CNcmDataSender* NewL(RDevUsbcScClient& aPort, CNcmDataInterface& aParent);
    ~CNcmDataSender();

public:
    /** derived from MNcmNtbBuildObserver */
    void SendNtbPayload(TNcmBuffer& aBuffer);

public:    
    /**
     * change NtbInMax size
     *
     * @param aSize, new NtbInMax size
     * @param aIsAltZero, is alternate setting 0
     * @return KErrNone           if success, 
     *         KErrArgument       if the aSize is bigger than 65535, 
     *           KErrUnknown        if aSize is too bigger and can't create enough buffer block
     */
    TInt SetNtbInMaxSize(TInt aSize, TBool aIsAltZero=ETrue);
    void RunL();
    void Start();
    /**
     * send a ethernet frame
     *
     * @param aPacket, the ethernet frame
     * @return KErrNone            if success, 
     *         KErrNotReady        if sender is not started.
     *         KErrCongestion      if there is less buffers, need to start flow control.  
     */    
    TInt Send(RMBufChain& aPacket);
    void GetNtbParam(TNcmNtbInParam& aParam);
    void Stop();
    TInt SetInEpBufferSize(TUint aSize);		
    inline TInt NtbInMaxSize(); 
  
private:
    CNcmDataSender(RDevUsbcScClient& aPort, CNcmDataInterface& aParent);
    void ConstructL();
    void SendNtbComplete();
    TInt StartNewNtb();
    void DoCancel();
        
private:
    RDevUsbcScClient&           iPort;
    CNcmDataInterface&          iParent;
    /**
    * LDD Endpoint buffer for in NCM in endpoint
    */    
    TEndpointBuffer             iEpOut;
    /**
    * store the buffers to be sent to LDD
    */    
    RArray<TNcmBuffer>          iTxArray;
    /**
    * store the buffer is on sending
    */
    TNcmBuffer                  iSendingBuffer;
    /**
    * there is a buffer currenly on sending
    */
    TBool                       iIsSending;
    TBool                       iStarted;
    CNcmNtbBuilder*             iNtbBuilder;
    CNcmBufferManager*          iBufferManager;
    CNcmNtbBuildPolicy*         iBuildPolicy;
    /**
    * networking may send packet when data sender is not started, use this to determin if do send resuming when start
    */
    TBool                       iStopSending;
    
    };

// Inline functions
#include "ncmdatasender.inl"

#endif //NCMDATASENDER_H
