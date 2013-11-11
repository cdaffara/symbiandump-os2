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

#ifndef NCMDATAINTERFACE_H
#define NCMDATAINTERFACE_H

#include <e32base.h>
#ifndef __OVER_DUMMYUSBSCLDD__
#include <nifmbuf.h>
#include <networking/ethinter.h>
#include <d32usbcsc.h>
#else
#include <dummyusbsclddapi.h>
#include <usb/testncm/ethinter.h>
#endif

#include "ncmpktdrvcommon.h"

class    CNcmDataSender;
class    CNcmDataReceiver;

/**
The interface to NCM data channel.
*/
NONSHARABLE_CLASS(MNcmDataObserver)
{
public:
    /**
    Process received datagram parsed from NCM.
    @param[in]   aPacket, contains the datagram(ethernet frame) to be dealed with.
    */
    virtual void ProcessReceivedDatagram(RMBufPacket& aPacket) = 0;
    /**
    Notify the upper layer(TCP/IP stack) to continue to send data.
    */
    virtual void ResumeSending() = 0;
    /**
    Notify unrecoverable error in data channel.
    */
    virtual void DataError(TInt aError) = 0;
    /**
    Notify the Data Interface's alt-setting changes, which commonly from 0 to 1, or vice versa.
    @param[in]   aIfAltSet, the alt-setting number.
    */
    virtual void HandleAltSetting(TInt aIfAltSet) = 0;
};

/**
Maintain the logic of Ncm data channel
*/
NONSHARABLE_CLASS(CNcmDataInterface) : public CActive
    {
public:
    static CNcmDataInterface* NewL(MNcmDataObserver& aEngine, RDevUsbcScClient& aLdd);
    ~CNcmDataInterface();

    inline TBool IsStarted() const;
    void Start();
    void Stop();
    TInt Send(RMBufChain& aPacket);

    TInt GetSpeed(TInt& aSpeed);
    TInt GetNtbInputSize(TDes8& aSize);
    TInt SetNtbInputSize(TDesC8& aSize);
    TInt GetNtbParam(TDes8& aParam);
    TInt SetInEpBufferSize(TUint aSize);

    void ActivateLdd();
    inline void DataError(TInt aErroCode) const;
    inline void ResumeSending() const;
    inline void ProcessDatagram(RMBufPacket& aPacket) const;
    inline TInt AltSetting() const;

private:
    CNcmDataInterface(MNcmDataObserver& aEngine, RDevUsbcScClient& aLdd);
    void ConstructL();

    //From CActive
    void RunL();
    void DoCancel();

private:
    TBool               iStarted;
    CNcmDataSender*     iSender;
    CNcmDataReceiver*   iReceiver;
    MNcmDataObserver&   iEngine;

    RDevUsbcScClient&   iLdd;
    TInt                iAltSetting;
    TUint               iIfState;
    };

// inline functions
#include "ncmdatainterface.inl"

#endif // NCMDATAINTERFACE_H
