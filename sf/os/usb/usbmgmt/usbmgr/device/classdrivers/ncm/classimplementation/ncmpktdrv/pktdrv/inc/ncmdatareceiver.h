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
#ifndef NCMDATARECEIVER_H
#define NCMDATARECEIVER_H

#include <e32base.h>

#ifndef __OVER_DUMMYUSBSCLDD__
#include <d32usbcsc.h>
#else
#include <dummyusbsclddapi.h>
#endif

#include "ncmpktdrvcommon.h"
#include "ncmntbparser.h"


class CNcmDataInterface;

/**
Responsible for receiving NCM packet data
*/
NONSHARABLE_CLASS(CNcmDataReceiver) : public CActive, public MNcmNdpFrameObserver
    {
public:
    static CNcmDataReceiver* NewL(RDevUsbcScClient& aPort, CNcmDataInterface& aParent);
    ~CNcmDataReceiver();

    void Start();
    void Stop();

    //from MNcmNdpFrameObserver
    void ProcessEtherFrame(RMBufPacket&);
    void ExpireBuffer(TAny*);

    inline TUint NtbOutMaxSize() const;
private:
    void ConstructL();
    CNcmDataReceiver(RDevUsbcScClient& aPort, CNcmDataInterface& aParent);

    //From CActive
    void RunL();
    void DoCancel();

    void ReadData();

private:
    RDevUsbcScClient&       iPort;
    TEndpointBuffer         iEpIn;
    TUint8*                 iBuf;
    TUint                   iBufLen;
    TBool                   iZlp;
    CNcmNtbParser*          iNtbParser;
    CNcmDataInterface&      iParent;
    };

// inline functions
#include "ncmdatareceiver.inl"

#endif //NCMDATARECEIVER_H

