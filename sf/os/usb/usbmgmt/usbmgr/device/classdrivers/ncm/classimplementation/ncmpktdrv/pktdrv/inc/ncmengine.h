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
#ifndef NCMENGINE_H
#define NCMENGINE_H


#ifndef __OVER_DUMMYUSBSCLDD__
#include <es_mbuf.h>
#else
#include <usb/testncm/ethinter.h>
#endif

#include <random.h>

#include "ncmpktdrvcommon.h"
#include "ncmcommunicationinterface.h"
#include "ncmdatainterface.h"
#include "ncmsharedstatemanager.h"

class CNcmPktDrv;

/**
NCM Engine: coordinate with Data Interface, Comm Interface and Shared State Manager.
*/
NONSHARABLE_CLASS(CNcmEngine) : public CActive,
                                public MNcmDataObserver,
                                public MNcmControlObserver,
                                public MShareStateObserver
    {
public:
    static CNcmEngine* NewL(CNcmPktDrv& aPktDrv);
    ~CNcmEngine();

    //Start the engine, called by TransferHandle after PktDrvBase's StartInterface;
    TInt Start(RMessagePtr2& aMsg);
    //Stop the engine, called by PktDrvBase's StopInterface;
    void Stop();

    //Send a Packet to USB Host.
    TInt Send(RMBufChain &aPacket);
    //Get the Interface Mac Address;
    TUint8* InterfaceAddress();

    //From MNcmControlObserver
    TInt HandleSetNtbInputSize(TDes8&);
    TInt HandleGetNtbParam(TDes8&);
    TInt HandleGetNtbInputSize(TDes8&);
    void ControlError(TInt aError);

    //From MNcmDataObserver
    void ProcessReceivedDatagram(RMBufPacket&);
    void ResumeSending();
    void DataError(TInt aError);
    void HandleAltSetting(TInt aIfNum);
    TInt SetInEpBufferSize(TUint aSize);

    //From MShareStateObserver
    void NotifyDhcpStarted();

    inline TInt SetStateToWatcher(TInt aType, TInt aValue) const;
    inline TInt RegisterNotify(const RMessage2& aMsg) const;
    inline TInt DeRegisterNotify() const;

#ifdef __OVER_DUMMYUSBSCLDD__
public:
    TInt State() const {return iEngineState;}
#endif

private:
    //From CActive
    void RunL();
    void DoCancel();

    CNcmEngine();
    CNcmEngine(CNcmPktDrv& aPktDrv);
    void ConstructL();

    void InitLddL(const RMessagePtr2& aMsg);
    void RandomMacAddressL(TNcmMacAddress& aMacAddress);
    void DoNcmStarted();
    void StartDataLayer();
    void StopDataLayer();

private:
    enum TNcmEngineState
        {
        ENcmStateUninitialized = 0,
        ENcmStateStarting,
        ENcmStateStarted,
        ENcmStatePaused,
        ENcmStateStopped,
        ENcmStateMax
        };

private:
    CNcmPktDrv&             iPktDrv;

    CNcmDataInterface*      iDataInterface;
    CNcmCommunicationInterface* iCommInterface;
    CNcmSharedStateManager* iSharedStateManager;

    TNcmEngineState         iEngineState;
    TUint                   iDeviceState;

    TNcmMacAddress          iSymbianMacAddress;

    RDevUsbcScClient        iDataLdd;
    RDevUsbcScClient        iCommLdd;
    };

//inline functions
#include "ncmengine.inl"

#endif // NCMENGINE_H
