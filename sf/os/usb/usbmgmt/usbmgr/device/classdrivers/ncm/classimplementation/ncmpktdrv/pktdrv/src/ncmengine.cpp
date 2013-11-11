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

#include "ncmengine.h"
#include "ncmpktdrv.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmengineTraces.h"
#endif



const TUint8 KDataIFAltSet1 = 1;

/**
Create a new CNcmControlEngine object.
*/
CNcmEngine *CNcmEngine::NewL(CNcmPktDrv& aPktDrv)
    {
    OstTraceFunctionEntry0( CNCMENGINE_NEWL );

    CNcmEngine *self=new (ELeave) CNcmEngine(aPktDrv);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    OstTraceFunctionExit1( CNCMENGINE_NEWL_DUP01, self );
    return self;
    }

/**
Create the CNcmEngine object.
*/
void CNcmEngine::ConstructL()
    {
    OstTraceFunctionEntry0( CNCMENGINE_CONSTRUCTL );

    RandomMacAddressL(iSymbianMacAddress);

    iSharedStateManager = CNcmSharedStateManager::NewL(*this);
    iDataInterface = CNcmDataInterface::NewL(*this, iDataLdd);
    iCommInterface = CNcmCommunicationInterface::NewL(*this, iCommLdd);

    OstTraceFunctionExit0( CNCMENGINE_CONSTRUCTL_DUP01 );
    }

/**
Constructor.
@param aPktDrv Pointer to NCM Packet Driver.
*/
CNcmEngine::CNcmEngine(CNcmPktDrv& aPktDrv) : CActive(CActive::EPriorityStandard),
                                    iPktDrv(aPktDrv)
    {
    CActiveScheduler::Add(this);
    }

/**
Destructor.
*/
CNcmEngine::~CNcmEngine()
    {
    OstTraceFunctionEntry0( CNCMENGINE_CNCMENGINE_DUP10 );

    //double check for possibly missed by PktDrvBase's StopInterface;
    Stop();

    delete iCommInterface;
    delete iDataInterface;
    delete iSharedStateManager;

    OstTraceFunctionExit0( CNCMENGINE_CNCMENGINE_DUP11 );
    }

/**
 * Initialize the DataInterface's and CommInterface's LDD instances with transferred handle.
 */
void CNcmEngine::InitLddL(const RMessagePtr2& aMsg)
    {
    OstTraceFunctionEntry0( CNCMENGINE_INITLDDL );

    const TUint KCommLddOffset = 0;                     //0
    const TUint KCommChunkOffset = KCommLddOffset + 1;  //1
    const TUint KDataLddOffset = KCommChunkOffset + 1;  //2
    const TUint KDataChunkOffset = KDataLddOffset + 1;  //3
    const TUint KHandlesInStackCount = 3;

    User::LeaveIfError(iCommLdd.Open(aMsg, KCommLddOffset, EOwnerProcess));
    CleanupClosePushL(iCommLdd);

    RChunk* chunk;
    //Get the Ldd's RChunk, but don't own it.
    User::LeaveIfError(iCommLdd.GetDataTransferChunk(chunk));

    User::LeaveIfError(chunk->Open(aMsg, KCommChunkOffset, FALSE, EOwnerProcess));
    CleanupClosePushL(*chunk);

    User::LeaveIfError(iDataLdd.Open(aMsg, KDataLddOffset, EOwnerProcess));
    CleanupClosePushL(iDataLdd);

    User::LeaveIfError(iDataLdd.GetDataTransferChunk(chunk));
    User::LeaveIfError(chunk->Open(aMsg, KDataChunkOffset, FALSE, EOwnerProcess));

    CleanupStack::Pop(KHandlesInStackCount);

    OstTraceFunctionExit0( CNCMENGINE_INITLDDL_DUP01 );
    }

/**
 * Start Engine to initialize LDD, and start monitor LDD device status changes.
 */
TInt CNcmEngine::Start(RMessagePtr2& aMsg)
    {
    OstTraceFunctionEntry0( CNCMENGINE_START );

    __ASSERT_DEBUG(iEngineState == ENcmStateUninitialized, User::Panic(KEnginePanic, __LINE__));
    if (ENcmStateUninitialized != iEngineState)
        {
        OstTraceFunctionExitExt( CNCMENGINE_START_DUP10, this, KErrNotSupported );
        return KErrNotSupported;
        }

    TRAPD(err, InitLddL(aMsg));
    if (KErrNone != err)
        {
        OstTrace1( TRACE_ERROR, CNCMENGINE_START_DUP01, "InitLdd return error=%d;", err );
        return err;
        }

    if (KErrNone != iCommLdd.DeviceStatus(reinterpret_cast<TUsbcDeviceState&>(iDeviceState)))
        {
        RChunk* commChunk = NULL;
        RChunk* dataChunk = NULL;
        iCommLdd.GetDataTransferChunk(commChunk);
        iDataLdd.GetDataTransferChunk(dataChunk);
        commChunk->Close();
        dataChunk->Close();
        iDataLdd.Close();
        iCommLdd.Close();
        OstTraceFunctionExitExt( CNCMENGINE_START_DUP11, this, KErrNotReady );
        return KErrNotReady;
        }
    OstTrace1( TRACE_NORMAL, CNCMENGINE_START_DUP02, "CommLdd DeviceStatus=%d", iDeviceState);

    if (EUsbcDeviceStateConfigured == iDeviceState)
        {
        iStatus = KRequestPending;
        TRequestStatus* reportStatus = &iStatus;
        SetActive();

        User::RequestComplete(reportStatus, KErrNone);
        }
    else
        {
        iCommLdd.AlternateDeviceStatusNotify(iStatus, iDeviceState);
        SetActive();
        }

    iDataInterface->ActivateLdd();

    iEngineState = ENcmStateStarting;

    OstTraceFunctionExitExt( CNCMENGINE_START_DUP12, this, KErrNone );
    return KErrNone;
    }

/**
 * Monitor the USB device State, Start/Stop Communication/Data Interface
 */
void CNcmEngine::RunL()
    {
    OstTraceExt2( TRACE_NORMAL, CNCMENGINE_RUNL, "iDeviceState=%d;iStatus.Int()=%d", iDeviceState, iStatus.Int() );

    if (KErrNone == iStatus.Int())
        {
        iCommLdd.AlternateDeviceStatusNotify(iStatus, iDeviceState);
        SetActive();
        }
    else
        {
        iPktDrv.FatalErrorNotification(iStatus.Int());
        Stop();
        return;
        }

    switch(iDeviceState)
        {
        case EUsbcDeviceStateConfigured:
            if (!iCommInterface->IsStarted())
                {
                iCommInterface->Start();

                StartDataLayer();
                }
            break;

        case EUsbcDeviceStateSuspended:
            StopDataLayer();
            break;

        case EUsbcDeviceStateUndefined:

			iDataLdd.ResetAltSetting();
            StopDataLayer();
            if (iCommInterface->IsStarted())
                {
                iCommInterface->Stop();
                }
            break;

        default:
            //do nothing here.
            break;
        }
    }

/**
AO cancel
*/
void CNcmEngine::DoCancel()
    {
    OstTraceFunctionEntry0( CNCMENGINE_DOCANCEL );
    iCommLdd.AlternateDeviceStatusNotifyCancel();
    }

/**
 * Fatal error report from Control Channel
 */
void CNcmEngine::ControlError(TInt aError)
    {
    OstTrace1( TRACE_NORMAL, CNCMENGINE_CONTROLERROR, "aError=%d", aError );

    iPktDrv.FatalErrorNotification(aError);
    Stop();
    }

/**
 *Enter the started state.
 */
void CNcmEngine::DoNcmStarted()
    {
    OstTraceFunctionEntry0( CNCMENGINE_DONCMSTARTED );

    iDataInterface->Start();
    TInt speed = 0;
    iDataInterface->GetSpeed(speed);

    iCommInterface->SendSpeedNotification(speed, speed);
    iCommInterface->SendConnectionNotification(ETrue);

    __ASSERT_DEBUG(((iEngineState==ENcmStateStarting) || (iEngineState==ENcmStatePaused)), User::Panic(KEnginePanic, __LINE__));
    iEngineState = ENcmStateStarted;
    iSharedStateManager->NotifyNcmConnected();
    }

/**
 *Start the data channel
 */
void CNcmEngine::StartDataLayer()
    {
    OstTraceFunctionEntry0( CNCMENGINE_STARTDATALAYER );

    if (iDataInterface->IsStarted())
        {
        OstTrace0( TRACE_WARNING, CNCMENGINE_STARTDATALAYER_DUP01, "The DataIf has already been started!" );

        __ASSERT_DEBUG(0, User::Panic(KEnginePanic, __LINE__));
        return;
        }

    if (KDataIFAltSet1 != iDataInterface->AltSetting())
        {
        OstTrace0( TRACE_NORMAL, CNCMENGINE_STARTDATALAYER_DUP02, "Data Interface is not ready to start; it's fine!!" );
        return;
        }

    TInt ret = iSharedStateManager->NotifyDhcpProvisionRequested();
    if (KErrAlreadyExists == ret)
        {
        DoNcmStarted();
        }
    else
        {
        if (KErrNone != ret)
            {
            //minus error when try to set IPBearerCoexistence P&S key. just continue...
            OstTrace1( TRACE_WARNING, CNCMENGINE_STARTDATALAYER_DUP03, "SharedStateManager issued dhcp request return error:;ret=%d", ret );
            }

        iCommInterface->SendConnectionNotification(EFalse);

        if (ENcmStatePaused == iEngineState)
            {
            iEngineState = ENcmStateStarting;
            }
        }
    }

/**
 *Stop the data channel
 */
void CNcmEngine::StopDataLayer()
    {
    OstTraceFunctionEntry0( CNCMENGINE_STOPDATALAYER );

    if (iDataInterface->IsStarted())
        {
        __ASSERT_DEBUG((iEngineState==ENcmStateStarted), User::Panic(KEnginePanic, __LINE__));
        iEngineState = ENcmStatePaused;
        iSharedStateManager->NotifyNcmDisconnected(KErrNone);

        iDataInterface->Stop();
        }
    }

/**
 *Stop NCM
 */
void CNcmEngine::Stop()
    {
    OstTraceFunctionEntry0( CNCMENGINE_STOP );

    if (ENcmStateStarted == iEngineState ||
        ENcmStatePaused == iEngineState ||
        ENcmStateStarting == iEngineState)
        {
        Cancel();
        iDataInterface->Cancel();

        if (iDataInterface->IsStarted())
            {
            iDataInterface->Stop();
            }

        if (iCommInterface->IsStarted())
            {
            iCommInterface->Stop();
            }

        RChunk* commChunk = NULL;
        RChunk* dataChunk = NULL;

        iCommLdd.GetDataTransferChunk(commChunk);
        iDataLdd.GetDataTransferChunk(dataChunk);
        commChunk->Close();
        dataChunk->Close();
        iDataLdd.Close();
        iCommLdd.Close();
        }

    if (ENcmStateStarted == iEngineState ||
        ENcmStateStarting == iEngineState)
        {
        iSharedStateManager->NotifyNcmDisconnected(KErrNone);
        }
    iEngineState = ENcmStateStopped;
    }

/**
 * Send the ethernet frame to USB Host.
*/
TInt CNcmEngine::Send(RMBufChain &aPacket)
    {
    OstTraceExt2( TRACE_NORMAL, CNCMENGINE_SEND, "aPacket=%x;aPacket.Length()=%d", ( TUint )&( aPacket ), aPacket.Length() );

    return iDataInterface->Send(aPacket);
    }

/**
Get the Hardware address of the LAN Device
@return MAC address of the Symbian device
*/
TUint8* CNcmEngine::InterfaceAddress()
    {
    OstTraceExt3( TRACE_NORMAL, CNCMENGINE_INTERFACEADDRESS, "Local MAC is 02:00:00:%02X:%02X:%02X.", iSymbianMacAddress[3], iSymbianMacAddress[4], iSymbianMacAddress[5]);
    return static_cast<TUint8*>(&iSymbianMacAddress[0]);
    }

void CNcmEngine::RandomMacAddressL(TNcmMacAddress& aMacAddress)
    {
    OstTrace1( TRACE_NORMAL, CNCMENGINE_RANDOMMACADDRESSL, "aMacAddress=%x", ( TUint )&( aMacAddress ) );
    //random the MAC address
    aMacAddress.SetLength(KEthernetAddressLength);
    const TUint KOUILength = 3;
    const TInt len = KEthernetAddressLength - KOUILength;

    TPtr8 ptr(&aMacAddress[0] + KOUILength,  len, len);
    TRandom::RandomL(ptr);

    //mark it as locally managed address
    aMacAddress[0] = 0x02;
    aMacAddress[1] = 0x00;
    aMacAddress[2] = 0x00;
    }

/** MControlInterfaceObserver
 * Set Ntb Input Size. See NCM1.0 spec.
 */
TInt CNcmEngine::HandleSetNtbInputSize(TDes8& aSize)
    {
    OstTraceFunctionEntry1( CNCMENGINE_HANDLESETNTBINPUTSIZE, ( TUint )&( aSize ) );

    return iDataInterface->SetNtbInputSize(aSize);
    }

/**
 * Set Ntb Parameters. See NCM1.0 spec.
 */
TInt CNcmEngine::HandleGetNtbParam(TDes8& aReturnBuf)
    {
    OstTraceFunctionEntry1( CNCMENGINE_HANDLEGETNTBPARAM, ( TUint )&( aReturnBuf ) );

    return iDataInterface->GetNtbParam(aReturnBuf);
    }

/**
 * Get Ntb Input Size. See NCM1.0 spec.
 */
TInt CNcmEngine::HandleGetNtbInputSize(TDes8& aSize)
    {
    OstTraceFunctionEntry1( CNCMENGINE_HANDLEGETNTBINPUTSIZE, ( TUint )&( aSize ) );

    return iDataInterface->GetNtbInputSize(aSize);
    }

/**
 * Enable NCM connection by setting NCM speed & connection, when notified that DHCP Provison done successfully.
 */
void CNcmEngine::NotifyDhcpStarted()
    {
    OstTraceFunctionEntry0( CNCMENGINE_NOTIFYDHCPSTARTED );
    __ASSERT_DEBUG(!iDataInterface->IsStarted(), User::Panic(KEnginePanic, __LINE__));
    if (iDeviceState != EUsbcDeviceStateConfigured || KDataIFAltSet1 != iDataInterface->AltSetting())
        {
        OstTraceExt2( TRACE_WARNING, CNCMENGINE_NOTIFYDHCPSTARTED_DUP01, "NotifyDhcpStarted comes so later;iDeviceState=%d;iDataInterface->AltSetting()=%d", iDeviceState, iDataInterface->AltSetting() );
        return;
        }

    DoNcmStarted();
    }

/**
 *Upwards notify
 * @param aPacket A Reference to a buffer holding data.
 */
void CNcmEngine::ProcessReceivedDatagram(RMBufPacket& aPacket)
    {
    OstTraceExt2( TRACE_NORMAL, CNCMENGINE_PROCESSRECEIVEDDATAGRAM, "aPacket=%x;aPacket.Length()=%d", ( TUint )&( aPacket ), aPacket.Length() );
    iPktDrv.ReceiveEthFrame(aPacket);
    }

/**
 * Resume Sending is a notification call into NIF from the lower layer telling the NIF that a
 * previous sending congestion situation has been cleared and it can accept more downstack data.
 */
void CNcmEngine::ResumeSending()
    {
    OstTraceFunctionEntry0( CNCMENGINE_RESUMESENDING );
    iPktDrv.ResumeSending();
    }

/**
 * Error report from Data Channel
 */
void CNcmEngine::DataError(TInt aError)
    {
    OstTrace1( TRACE_ERROR, CNCMENGINE_DATAERROR, "CNcmEngine::DataError;aError=%d", aError );

    iPktDrv.FatalErrorNotification(aError);
    }
/**
 * Handle the data interface alt setting changes.
 */
void CNcmEngine::HandleAltSetting(TInt aIfAltSet)
    {
    OstTraceExt3( TRACE_NORMAL, CNCMENGINE_HANDLEALTSETTING, "CNcmEngine::HandleAltSetting;aIfAltSet=%d;iDataInterface->AltSetting()=%d;iDeviceState=%d", aIfAltSet, iDataInterface->AltSetting(), (TInt) iDeviceState );

    if (KDataIFAltSet1 == aIfAltSet)
        {
        StartDataLayer();
        }
    else
        {
        StopDataLayer();
        }
    }

TInt CNcmEngine::SetInEpBufferSize(TUint aSize)
    {
    OstTrace1( TRACE_NORMAL, CNCMENGINE_SETINEPBUFFERSIZE, "aSize=%u", aSize );
    return iDataInterface->SetInEpBufferSize(aSize);
    }

