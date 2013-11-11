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

#include <e32base.h>
#include <e32cons.h>

#ifndef __OVER_DUMMYUSBSCLDD__
#include <d32usbc.h>//EEndPoint2
#else
#include <dummyusbsclddapi.h>
#endif
#include "ncmdatainterface.h"
#include "ncmdatareceiver.h"
#include "ncmdatasender.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmdatainterfaceTraces.h"
#endif




const TInt KAltSetting1 = 1;
const TInt KAltSetting0 = 0;


CNcmDataInterface* CNcmDataInterface::NewL(MNcmDataObserver& aEngine,
        RDevUsbcScClient& aLdd)
    {
    CNcmDataInterface *self = new (ELeave) CNcmDataInterface(aEngine, aLdd);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CNcmDataInterface::CNcmDataInterface(MNcmDataObserver& aEngine,
        RDevUsbcScClient& aLdd) :
    CActive(CActive::EPriorityStandard), iEngine(aEngine), iLdd(aLdd)
    {
    CActiveScheduler::Add(this);
    }

void CNcmDataInterface::ConstructL()
    {
    OstTraceFunctionEntry0(CNCMDATAINTERFACE_CONSTRUCTL);

    iReceiver = CNcmDataReceiver::NewL(iLdd, *this);
    iSender = CNcmDataSender::NewL(iLdd, *this);
    OstTraceFunctionExit0(CNCMDATAINTERFACE_CONSTRUCTL_DUP01);
    }

CNcmDataInterface::~CNcmDataInterface()
    {
    OstTraceFunctionEntry0(CNCMDATAINTERFACE_CNCMDATAINTERFACE_DUP01);
    Cancel();

    delete iReceiver;
    delete iSender;
    OstTraceFunctionExit0(CNCMDATAINTERFACE_CNCMDATAINTERFACE_DUP02);
    }

/**
 Start the NCM data channel
 */
void CNcmDataInterface::Start()
    {
    OstTraceFunctionEntry0(CNCMDATAINTERFACE_START);
    if (!iStarted)
        {
        iReceiver->Start();
        iSender->Start();
        iStarted = ETrue;
        }
    OstTraceFunctionExit0(CNCMDATAINTERFACE_START_DUP01);
    }

/**
 Stop the NCM data channel
 */
void CNcmDataInterface::Stop()
    {
    OstTraceFunctionEntry0(CNCMDATAINTERFACE_STOP);
    if (iStarted)
        {
        iReceiver->Stop();
        iSender->Stop();
        iStarted = EFalse;

        //after stop().
        if (KAltSetting1 == iAltSetting)
            {

            iAltSetting = KAltSetting0;
            }
        }
    OstTraceFunctionExit0(CNCMDATAINTERFACE_STOP_DUP01);
    }

/**
 Send packet data from upper link to NCM host
 */
TInt CNcmDataInterface::Send(RMBufChain &aPacket)
    {
    const TInt KContinue = 1;
    const TInt KStop = 0;

    TInt ret = iSender->Send(aPacket);
    if (ret == KErrNone)
        {
        return KContinue;
        }
    else if (ret == KErrNotReady || ret == KErrCongestion)
        {
        return KStop;
        }
    else
        {
        iEngine.DataError(ret);
        return KStop;
        }
    }

/**
 * Acitve this AO to monitor the LDD's alt-setting changes.
 */
void CNcmDataInterface::ActivateLdd()
    {
    TInt alt = 0;
    TInt ret = KErrNone;

    ret = iLdd.GetAlternateSetting(alt);
    OstTraceExt2(TRACE_NORMAL, CNCMDATAINTERFACE_ACTIVATELDD, "ActivateLDD:alt=%d, ret=%d", alt, ret);

    if (KErrNone == ret)
        {
        if (iAltSetting == alt)
            {
            iLdd.AlternateDeviceStatusNotify(iStatus, iIfState);
            SetActive();
            }
        else
            {
            TRequestStatus* status = &iStatus;
            iStatus = KRequestPending;
            iIfState = KUsbAlternateSetting | alt;
            SetActive();

            User::RequestComplete(status, KErrNone);
            }
        }
    else
        {
        //It's possible when Usb Device Status is not configed.
        iLdd.AlternateDeviceStatusNotify(iStatus, iIfState);
        SetActive();
        }
    }

void CNcmDataInterface::RunL()
    {
    OstTraceExt2(TRACE_NORMAL, CNCMDATAINTERFACE_RUNL, "State=%x, status=%d", iIfState, iStatus.Int());

    if (KErrNone == iStatus.Int())
        {
        iLdd.AlternateDeviceStatusNotify(iStatus, iIfState);
        SetActive();

        if (iIfState & KUsbAlternateSetting)
            {
            TInt ret = KErrNone;
            iAltSetting = iIfState & ~KUsbAlternateSetting;

            if (KAltSetting1 == iAltSetting)
                {
                ret = iLdd.StartNextOutAlternateSetting(ETrue);
                if (KAltSetting1 == ret)
                    {
                    iEngine.HandleAltSetting(iAltSetting);
                    }
                else
                    {
                    iEngine.DataError(ret);
                    }
                }
            else
                {
                iEngine.HandleAltSetting(iAltSetting);
                ret = iLdd.StartNextOutAlternateSetting(ETrue);
                if (KAltSetting0 != ret)
                    {
                    iEngine.DataError(ret);
                    }
                }
            }
        }
    else
        {
        iEngine.DataError(iStatus.Int());
        }
    }

/**
 * AO cancel.
 */
void CNcmDataInterface::DoCancel()
    {
    iLdd.AlternateDeviceStatusNotifyCancel();
    }


/**
 * Get Connection Speed. (NB. Upstream's is the same as downstream's).
 */
TInt CNcmDataInterface::GetSpeed(TInt& aSpeed)
    {
    const TInt KHighSpeed = 480 * 1024 * 1024;
    const TInt KFullSpeed = 12 * 1024 * 1024;

    if (iLdd.CurrentlyUsingHighSpeed())
        {
        aSpeed = KHighSpeed;
        }
    else
        {
        aSpeed = KFullSpeed;
        }

    return KErrNone;

    }
/**
 * Get Ntb Input Size.
 */
TInt CNcmDataInterface::GetNtbInputSize(TDes8& aSize)
    {
    const TUint KNtbIntputSizeLength = 4;
    aSize.SetLength(KNtbIntputSizeLength);
    LittleEndian::Put32(&aSize[0], iSender->NtbInMaxSize());
    return KErrNone;
    }

/**
 * Set Ntb Input Size.
 */
TInt CNcmDataInterface::SetNtbInputSize(TDesC8& aSize)
    {
    TInt size = LittleEndian::Get32(aSize.Ptr());
    return iSender->SetNtbInMaxSize(size, KAltSetting0==iAltSetting);
    }

/**
 * Get Ntb Parameter.
 */
TInt CNcmDataInterface::GetNtbParam(TDes8& aParamBuf)
    {
    TNcmNtbInParam  param;

    iSender->GetNtbParam(param);

    const TUint32 KNtbParamLength = 0x001C;
    const TUint32 KNtbFormat = 0x0001;
    const TUint   KNtbParamLengthOffset = 0;                               //0
    const TUint   KNtbFormatOffset = KNtbParamLengthOffset + 2;            //2
    const TUint   KNtbInMaxSizeOffset = KNtbFormatOffset + 2;              //4
    const TUint   KNtbInDivisorOffset = KNtbInMaxSizeOffset + 4;           //8
    const TUint   KNtbInPayloadRemainderOffset = KNtbInDivisorOffset + 2;  //10
    const TUint   KNtbInAlignmentOffset = KNtbInPayloadRemainderOffset + 2;//12
    const TUint   KNtbOutMaxSizeOffset = KNtbInAlignmentOffset + 4;        //16
    const TUint   KNtbOutDivisorOffset = KNtbOutMaxSizeOffset + 4;         //20
    const TUint   KNtbOutPayloadRemainderOffset = KNtbOutDivisorOffset + 2;//22
    const TUint   KNtbOutAlignmentOffset = KNtbOutPayloadRemainderOffset + 2;//24

    aParamBuf.FillZ(KNtbParamLength);
    LittleEndian::Put16(&aParamBuf[KNtbParamLengthOffset], KNtbParamLength);
    LittleEndian::Put16(&aParamBuf[KNtbFormatOffset], KNtbFormat);
    LittleEndian::Put32(&aParamBuf[KNtbInMaxSizeOffset], param.iNtbInMaxSize);
    LittleEndian::Put16(&aParamBuf[KNtbInDivisorOffset], param.iNdpInDivisor);
    LittleEndian::Put16(&aParamBuf[KNtbInPayloadRemainderOffset], param.iNdpInPayloadRemainder);
    LittleEndian::Put16(&aParamBuf[KNtbInAlignmentOffset], param.iNdpInAlignment);

    //Tricky way: copy the Sender's parameter as the Receiver's.
    LittleEndian::Put32(&aParamBuf[KNtbOutMaxSizeOffset], iReceiver->NtbOutMaxSize());
    LittleEndian::Put16(&aParamBuf[KNtbOutDivisorOffset], param.iNdpInDivisor);
    LittleEndian::Put16(&aParamBuf[KNtbOutPayloadRemainderOffset], param.iNdpInPayloadRemainder);
    LittleEndian::Put16(&aParamBuf[KNtbOutAlignmentOffset], param.iNdpInAlignment);

    return KErrNone;
    }

TInt CNcmDataInterface::SetInEpBufferSize(TUint aSize)
    {
    return iSender->SetInEpBufferSize(aSize);
    }

