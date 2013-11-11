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


#include <e32base.h>

#ifndef __OVER_DUMMYUSBSCLDD__
#include <d32usbc.h>
#else
#include <dummyusbsclddapi.h>
#endif

#include "ncmdatasender.h"
#include "ncmntb16builder.h"
#include "ncmdatainterface.h"
#include "ncmbuffermanager.h"
#include "ncmntbbuildsimplepolicy.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmdatasenderTraces.h"
#endif


//
//write buffer alignment value should get from share chunk LDD, current LDD has no API for this
//use a pagesize here instead
//
const TInt KAlignSize = 1024;


// ======== MEMBER FUNCTIONS ========
//

CNcmDataSender::CNcmDataSender(RDevUsbcScClient& aPort, CNcmDataInterface& aParent) : CActive(EPriorityStandard),
                                            iPort(aPort),
                                            iParent(aParent)
    {
    CActiveScheduler::Add(this);
    }

CNcmDataSender* CNcmDataSender::NewL(RDevUsbcScClient& aPort, CNcmDataInterface& aParent)
    {
    OstTraceFunctionEntry0( CNCMDATASENDER_NEWL_ENTRY );
    CNcmDataSender *self = new (ELeave) CNcmDataSender(aPort, aParent);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    OstTraceFunctionExit0( CNCMDATASENDER_NEWL_EXIT );
    return self;
    }

void CNcmDataSender::ConstructL()
    {

    iNtbBuilder = CNcmNtb16Builder::NewL(*this);
    iBufferManager = CNcmBufferManager::NewL();
    iBufferManager->SetAlignSize(KAlignSize);
    iBuildPolicy = CNcmNtbBuildSimplePolicy::NewL(*iNtbBuilder);
    iBuildPolicy->NtbBuilder().SetBuildPolicy(*iBuildPolicy);
    iStarted = EFalse;
    iStopSending = EFalse;
    iIsSending = EFalse;
    }

CNcmDataSender::~CNcmDataSender()
    {
    Cancel();
    delete iNtbBuilder;
    delete iBufferManager;
    delete iBuildPolicy;
    }

//
// Start the sender
//
void CNcmDataSender::Start()
    {
    OstTraceFunctionEntry1( CNCMDATASENDER_START_ENTRY, this );
    if (iStarted)
        {
        OstTrace0( TRACE_ERROR, CNCMDATASENDER_START, "Alrealy start, return directly." );
        OstTraceFunctionExit1( CNCMDATASENDER_START_EXIT, this );
        return;
        }

    if (iStopSending)
        {
        iParent.ResumeSending();
        iStopSending = EFalse;
        }

    TInt ret = iPort.OpenEndpoint(iEpOut, EEndpoint1);
    if (ret != KErrNone)
        {
        OstTrace1( TRACE_FATAL, CNCMDATASENDER_START1, "OpenEndpoint error %d", ret );
        iParent.DataError(ret);
        OstTraceFunctionExit1( CNCMDATASENDER_START_EXIT_DUP1, this );
        return;
        }

    TAny* buf = NULL;
    TUint size;
    ret = iEpOut.GetInBufferRange(buf, size);
    if (ret != KErrNone)
        {
        OstTrace1( TRACE_FATAL, CNCMDATASENDER_START2, "iEpOut.GetInBufferRange failed ret=%d", ret );
        iParent.DataError(ret);
        OstTraceFunctionExit1( CNCMDATASENDER_START_EXIT_DUP2, this );
        return;
        }
    else if (size < NtbInMaxSize())
        {
        OstTrace0( TRACE_FATAL, CNCMDATASENDER_START3, "LDD buffer size is small than NTB size" );
        iParent.DataError(KErrArgument);
        OstTraceFunctionExit1( CNCMDATASENDER_START_EXIT_DUP3, this );
        return;
        }
    OstTraceExt2( TRACE_NORMAL, CNCMDATASENDER_START4, "GetInBufferRange at 0x%x size %d", (TUint)buf, size);


    iBufferManager->InitBufferArea(buf, size);
    ret = iBufferManager->SetBufferCellSize(NtbInMaxSize());
    if (ret != KErrNone)
        {
        OstTrace1( TRACE_FATAL, CNCMDATASENDER_START5, "iBufferManager->SetBufferCellSize failed ret=%d", ret );
        iParent.DataError(KErrArgument);
        OstTraceFunctionExit1( CNCMDATASENDER_START_EXIT_DUP4, this );
        return;
        }
    iBuildPolicy->UpdateBufferSize(NtbInMaxSize());
    iBuildPolicy->UpdateTotalBufferCount(iBufferManager->FreeBufferCount());
    iStarted = ETrue;
    OstTraceFunctionExit1( CNCMDATASENDER_START_EXIT_DUP5, this );
    }


void CNcmDataSender::RunL()
    {
    OstTraceFunctionEntry1( CNCMDATASENDER_RUNL_ENTRY, this );
    OstTrace1( TRACE_NORMAL, CNCMDATASENDER_RUNL, "CNcmDataSender::RunL:Status=%d", iStatus.Int());

    if(iStatus.Int() != KErrNone)
        {
        if (KErrCancel == iStatus.Int() )
            {
            }
        else
            {
            iParent.DataError(iStatus.Int());
            }
        OstTraceFunctionExit1( CNCMDATASENDER_RUNL_EXIT, this );
        return;
        }
    SendNtbComplete();
    OstTraceFunctionExit1( CNCMDATASENDER_RUNL_EXIT_DUP1, this );
    }

//
//Start a New Ntb in Builder
//

TInt CNcmDataSender::StartNewNtb()
    {
    OstTraceFunctionEntry1( CNCMDATASENDER_STARTNEWNTB_ENTRY, this );
    TNcmBuffer ncmBuffer;
    TInt ret = iBufferManager->GetBuffer(ncmBuffer);
	if (ret == KErrNone || ret == KErrCongestion)
        {
        iNtbBuilder->StartNewNtb(ncmBuffer);
        }
    else
        {
        OstTrace1( TRACE_FATAL, CNCMDATASENDER_STARTNEWNTB, "iBufferManager->GetBuffer failed ret=%d", ret);
        }
    OstTraceFunctionExit1( CNCMDATASENDER_STARTNEWNTB_EXIT, this );
    return ret;
    }


//add a Ethernet packet to current NTB
//

TInt CNcmDataSender::Send(RMBufChain& aPacket)
    {
    OstTraceFunctionEntry1( CNCMDATASENDER_SEND_ENTRY, this );

    if (!iStarted)
        {
        OstTrace0( TRACE_WARNING, CNCMDATASENDER_SEND1, "Sender is not started" );
        iStopSending = ETrue;
        OstTraceFunctionExit1( CNCMDATASENDER_SEND_EXIT, this );
        return KErrNotReady;
        }

    TInt ret = KErrNone;
    if (!iNtbBuilder->IsNtbStarted())
        {
        ret = StartNewNtb();
        if (ret != KErrNone && ret != KErrCongestion)
            {
            OstTraceFunctionExit1( CNCMDATASENDER_SEND_EXIT_DUP1, this );
            return ret;
            }
        }

    TBool isCongestion = (ret == KErrCongestion)?ETrue:EFalse;

    ret = iNtbBuilder->AppendPacket(aPacket);

    if (ret == KErrBufferFull)
        {
    //current NTB is full and can't add new packet, start a new NTB and insert packet to it
        ret = StartNewNtb();
        if (ret == KErrNone)
            {
            ret = iNtbBuilder->AppendPacket(aPacket);
            }
        else if (ret == KErrCongestion)
            {
            isCongestion = ETrue;
            ret = iNtbBuilder->AppendPacket(aPacket);
            }
        }

    if (isCongestion && ret == KErrNone)
        {
        OstTrace0( TRACE_NORMAL, CNCMDATASENDER_SEND2, "CNcmDataSender::Send congestion" );
        iStopSending = ETrue;
        OstTraceFunctionExit1( CNCMDATASENDER_SEND_EXIT_DUP2, this );
        return KErrCongestion;
        }
    OstTraceFunctionExit1( CNCMDATASENDER_SEND_EXIT_DUP3, this );
    return ret;
    }

//
//Cancel the outgoing request of sending
//

void CNcmDataSender::DoCancel()
    {
    OstTraceFunctionEntry1( CNCMDATASENDER_DOCANCEL_ENTRY, this );
    iPort.WriteCancel(iEpOut.BufferNumber());
    OstTraceFunctionExit1( CNCMDATASENDER_DOCANCEL_EXIT, this );
    }

void CNcmDataSender::SendNtbPayload(TNcmBuffer& aBuf)
    {
    OstTraceFunctionEntry1( CNCMDATASENDER_SENDNTBPAYLOAD_ENTRY, this );
    OstTraceExt2( TRACE_NORMAL, CNCMDATASENDER_SENDNTBPAYLOAD, "SendNtbPayload:%08x, len=%d", (TUint)aBuf.iPtr, aBuf.iLen);

    if (iIsSending)
        {
        OstTrace0( TRACE_NORMAL, CNCMDATASENDER_SENDNTBPAYLOAD1, "SendNtbPayload: there is NTB on sending, store the buffer" );
        iTxArray.Append(aBuf);
        OstTraceFunctionExit1( CNCMDATASENDER_SENDNTBPAYLOAD_EXIT, this );
        return;
        }

    iSendingBuffer = aBuf;
    iIsSending = ETrue;

    iStatus = KRequestPending;
    TInt ret;
    //
    //   compliant to NCM spec with Zlp case
    //
    if (aBuf.iLen == NtbInMaxSize())
        {
        OstTrace0( TRACE_NORMAL, CNCMDATASENDER_SENDNTBPAYLOAD2, "iEpOut.WriteBuffer without zlp" );
        ret = iEpOut.WriteBuffer((TAny*)aBuf.iPtr, aBuf.iLen, EFalse, iStatus);
        }
    else
        {
        OstTrace0( TRACE_NORMAL, CNCMDATASENDER_SENDNTBPAYLOAD3, "iEpOut.WriteBuffer with zlp" );
        ret = iEpOut.WriteBuffer((TAny*)aBuf.iPtr, aBuf.iLen, ETrue, iStatus);
        }
    if (ret == KErrNone && !IsActive())
        {
        SetActive();
        }
    else
        {
        OstTrace1( TRACE_FATAL, CNCMDATASENDER_SENDNTBPAYLOAD4, "iEpOut.WriteBuffer failed ret = %d", ret);
        iParent.DataError(ret);
        }
    OstTraceFunctionExit1( CNCMDATASENDER_SENDNTBPAYLOAD_EXIT_DUP1, this );
    return;
    }

//
// called when a sender request is completed by LDD
//

void CNcmDataSender::SendNtbComplete()
    {
    OstTraceFunctionEntry1( CNCMDATASENDER_SENDNTBCOMPLETE_ENTRY, this );

    iIsSending = EFalse;
    iBufferManager->FreeBuffer(iSendingBuffer);
    iBuildPolicy->UpdateFreeBufferCount(iBufferManager->FreeBufferCount());

    // when buffer manage has enough buffers, notify networking to stop flow control
    if (!iBufferManager->IsCongestion() && iStopSending)
        {
        iStopSending = EFalse;
        iParent.ResumeSending();
        }
    if (iTxArray.Count())
        {
        OstTrace0( TRACE_NORMAL, CNCMDATASENDER_SENDNTBCOMPLETE3, "send NTB in sending queue" );
        SendNtbPayload(iTxArray[0]);
        iTxArray.Remove(0);
        }
    OstTraceFunctionExit1( CNCMDATASENDER_SENDNTBCOMPLETE_EXIT, this );
    }

void CNcmDataSender::GetNtbParam(TNcmNtbInParam& aParam)
    {
    iNtbBuilder->GetNtbParam(aParam);
    }

TInt CNcmDataSender::SetNtbInMaxSize(TInt aSize, TBool aIsAltZero)
    {
    OstTraceFunctionEntry1( CNCMDATASENDER_SETNTBINMAXSIZE_ENTRY, this );
    OstTrace1( TRACE_NORMAL, CNCMDATASENDER_SETNTBINMAXSIZE, "SetNtbInMaxSize %d", aSize);

    TInt ret = iNtbBuilder->SetNtbInMaxSize(aSize);
    if (ret != KErrNone)
        {
        OstTrace1( TRACE_FATAL, CNCMDATASENDER_SETNTBINMAXSIZE1, "iNtbBuilder->SetNtbInMaxSize failed %d", ret);
        OstTraceFunctionExit1( CNCMDATASENDER_SETNTBINMAXSIZE_EXIT, this );
        return ret;
        }

//
//if sender is not started, endpoint buffer is not setup, so does not call SetBufferCellSize
//
    if (!iStarted)
        {
        OstTraceFunctionExit1( CNCMDATASENDER_SETNTBINMAXSIZE_EXIT_DUP1, this );
        return KErrNone;
        }

//    if not in alternate setting 0, there may be data in buffer, so doesn't reset buffers.
    if (!aIsAltZero)
        {
        OstTraceFunctionExit1( CNCMDATASENDER_SETNTBINMAXSIZE_EXIT_DUP2, this );
        return KErrNone;
        }

    OstTraceFunctionExit1( CNCMDATASENDER_SETNTBINMAXSIZE_EXIT_DUP4, this );
    return KErrNone;
    }


void CNcmDataSender::Stop()
    {
    OstTraceFunctionEntry1( CNCMDATASENDER_STOP_ENTRY, this );
    Cancel();
    iBuildPolicy->Cancel();
    iNtbBuilder->Reset();
    iTxArray.Reset();
    iEpOut.Close();
    iStopSending = EFalse;
    iStarted = EFalse;
    iIsSending = EFalse;
    OstTraceFunctionExit1( CNCMDATASENDER_STOP_EXIT, this );
    }

// call this function to notify the buffer size of In endpoint buffer size of LDD and adjust the MaxInNtbSize supported
// report to host

TInt CNcmDataSender::SetInEpBufferSize(TUint aSize)
    {
    OstTraceFunctionEntry1( CNCMDATASENDER_SETINEPBUFFERSIZE_ENTRY, this );
    TInt ret = KErrNone;
    TUint size = aSize;

	OstTrace1( TRACE_NORMAL, CNCMDATASENDER_SETINEPBUFFERSIZE, "SetInEpBufferSize aSize=%d", aSize);


// the buffer may not aligned, so decrease max possible offset due to aligment
    size -= KAlignSize-1;
    TInt ntbSize = iNtbBuilder->NtbInMaxSize();
    TInt cellSize = KAlignSize;
    TInt minNtbInMaxSize = iNtbBuilder->MinNtbInMaxSize();
    TInt count;
    TBool find = EFalse;
    while (ntbSize >= minNtbInMaxSize)
        {
        cellSize = (ntbSize+KAlignSize-1)&~(KAlignSize-1);
        count = size / cellSize;
        if (count < iBufferManager->RequiredBufferCount())
            {
            ntbSize /= 2;
            continue;
            }
        find = ETrue;
        break;
        }

    if (ntbSize == iNtbBuilder->NtbInMaxSize())
        {
        ret = KErrNone;
        }
    else if (find)
        {
        ret = iNtbBuilder->SetNtbInMaxSize(cellSize);
        }
    else
        {
        ret = KErrGeneral;
        }
    OstTraceFunctionExit1( CNCMDATASENDER_SETINEPBUFFERSIZE_EXIT, this );
    return ret;
    }

