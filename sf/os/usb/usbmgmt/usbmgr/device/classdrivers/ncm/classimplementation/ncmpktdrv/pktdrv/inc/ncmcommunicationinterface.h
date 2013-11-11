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
* header file for NCM communication interface class 
*
*/

/**
@file
@internalComponent
*/

#ifndef NCMCOMMUNICATIONINTERFACE_H
#define NCMCOMMUNICATIONINTERFACE_H

#include <e32base.h>

#ifndef __OVER_DUMMYUSBSCLDD__
#include <d32usbcsc.h>
#else //!__OVER_DUMMYUSBSCLDD__
#include <dummyusbsclddapi.h>
#endif// __OVER_DUMMYUSBSCLDD__

#include "ncmcomminterfacesenderandreceiver.h"
#include "ncmpktdrvcommon.h"

const TUint KSetupPacketLength = 8;
const TUint KNtbParamStructLength = 28;
const TUint KNtbInputSizeStructLength = 4;

_LIT(KNcmCommInterface, "NcmCommunicationInterface");

enum TNcmCommErrorCode
    {
    EInternalError,
    EInvalidSetupPacketLength,
    EInvalidOutSetupPacket,
    EInvalidInSetupPacket,
    EInvalidLengthToRead,
    EInvalidDataPacketLength,
    EInvalidFilterOffset
    };

/**
* The interface to Ncm engine
*/

NONSHARABLE_CLASS(MNcmControlObserver)
{
public:
    virtual TInt HandleSetNtbInputSize(TDes8& aBuf) = 0;
    virtual TInt HandleGetNtbParam(TDes8& aBuf) = 0;
    virtual TInt HandleGetNtbInputSize(TDes8& aBuf) = 0;
    virtual void ControlError(TInt aError) = 0;
};

/**
*  implement NCM control interface
*/

NONSHARABLE_CLASS(CNcmCommunicationInterface) : public CActive
    {
public:
    static CNcmCommunicationInterface* NewL(MNcmControlObserver& aEngine, RDevUsbcScClient& aLdd);
    ~CNcmCommunicationInterface();
    TInt Start();
    void Stop();
    void ControlMsgError(TNcmCommErrorCode aCode);
    TInt SendConnectionNotification(TBool aConnected);
    TInt SendSpeedNotification(TInt aUSBitRate, TInt aDSBitRate);

public:
    inline RDevUsbcScClient& Ldd();
    inline TBool IsStarted();
    
private:
    enum TNcmCommRWState
        {
        EStateInitial = 0,
        EStateReadSetup, 
        EStateReadDataout, 
        EStateWriteDatain
        };

    enum TUsbRequestType
        {
        EGetNtbParameters = 0x80,
        EGetNtbInputSize  = 0x85,
        ESetNtbInputSize  = 0x86
        };

private:
    CNcmCommunicationInterface(MNcmControlObserver& aEngine, RDevUsbcScClient& aLdd);
    void ConstructL();
    void RunL();
    void DoCancel();
    void ReadSetup();
    void DecodeSetup();
    void ReadDataOut();
    void WriteDataIn();
    void ParseDataOut();
    TInt WriteInterruptData(TInt aEndPoint, TDesC8& aDes, TInt aLength);
    TInt GetInterfaceNumber();
    

private:
    
    MNcmControlObserver&                   iEngine;
    /**
    * USB request type
    */        
    TUint8                                 iRequestType;
    /**
    * data stage data length
    */    
    TUint16                                iDataStageLength;
    /**
    * usb setup packet
    */    
    TBuf8<KSetupPacketLength>              iSetupPacket;
    /**
    * control transfer data
    */    
    TBuf8<KNtbParamStructLength>           iDataBuffer;
    TNcmCommRWState                        iRWState;
    TBool                                  iStarted;
    RDevUsbcScClient&                      iPort;
    /**
    * instance to CNcmCommInterfaceSenderAndReceiver
    */    
    CNcmCommInterfaceSenderAndReceiver*    iSenderAndReceiver;
    /**
    * communication interface number
    */    
    TUint8 iInterfaceNumber;

    };

// Inline functions
#include "ncmcommunicationinterface.inl"

#endif //NCMCOMMUNICATIONINTERFACE_H
