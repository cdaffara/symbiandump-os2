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



#ifndef __OVER_DUMMYUSBSCLDD__
#include <e32base.h>
#include <d32usbc.h>//EEndPoint2
#else
#include <dummyusbsclddapi.h>
#endif

#include "ncmdatareceiver.h"
#include "ncmdatainterface.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmdatareceiverTraces.h"
#endif



#ifdef _DEBUG
_LIT(KDataReceiverPanic, "DataRcvrPanic");
#endif

/**
@file
@internalComponent
*/

/**
 * Constructor
 */
CNcmDataReceiver::CNcmDataReceiver(RDevUsbcScClient& aPort, CNcmDataInterface& aParent) :
                                            CActive(CActive::EPriorityStandard),
                                            iPort(aPort),
                                            iParent(aParent)
    {
    CActiveScheduler::Add(this);
    }

/**
 * NewL to create object.
 */
CNcmDataReceiver* CNcmDataReceiver::NewL(RDevUsbcScClient& aPort, CNcmDataInterface& aParent)
    {
    OstTraceFunctionEntry0(CNCMDATARECEIVER_NEWL);

    CNcmDataReceiver* self = new(ELeave) CNcmDataReceiver(aPort, aParent);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    OstTraceFunctionExit1(CNCMDATARECEIVER_NEWL_DUP01, self);
    return self;
    }

void CNcmDataReceiver::ConstructL()
    {
    iNtbParser = CNcmNtb16Parser::NewL(*this);
    }

/**
 * Destructor
 */
CNcmDataReceiver::~CNcmDataReceiver()
    {
    OstTraceFunctionEntry0(CNCMDATARECEIVER_CNCMDATARECEIVER_DUP01);

    Cancel();
    delete iNtbParser;
    }

/**
 * Start to receive and parse the USB data.
 */
void CNcmDataReceiver::Start()
    {
    OstTraceFunctionEntry0(CNCMDATARECEIVER_START);

    TInt ret = iPort.OpenEndpoint(iEpIn, EEndpoint2);
    if (KErrNone != ret)
        {
        OstTrace1(TRACE_ERROR, CNCMDATARECEIVER_START_DUP01, "OpenEndpoint error=%d", ret);
        iParent.DataError(ret);
        OstTraceFunctionExit0(CNCMDATARECEIVER_START_DUP02);
        return;
        }
    ReadData();
    OstTraceFunctionExit0(CNCMDATARECEIVER_START_DUP03);
    }

/**
 * Read the data packet from NCM host
 */
void CNcmDataReceiver::ReadData()
    {
    OstTraceFunctionEntry0(CNCMDATARECEIVER_READDATA);

    const TInt KRetryCount = 3;
    TInt ret = KErrNone;
    TInt errCnt = KRetryCount;

    while(errCnt)
        {
        ret = iEpIn.TakeBuffer(reinterpret_cast<TAny*&>(iBuf), iBufLen, iZlp, iStatus);
        if (KErrCompletion == ret)
            {
            errCnt = KRetryCount;
            if (iBufLen > 0)
                {
                OstTraceExt3(TRACE_NORMAL, CNCMDATARECEIVER_READDATA_DUP02, "iBuf=%x, iBufLen=%d, iZlp=%d", (TInt)iBuf, iBufLen, iZlp);
                ret = iNtbParser->Parse(iBuf, iBufLen, iZlp);
                }
            }
        else if (KErrNone == ret || KErrEof == ret)
            {
            break;
            }
        else
            {
            OstTrace1(TRACE_ERROR, CNCMDATARECEIVER_READDATA_DUP03, "TakeBuffer error=%d", ret);
            errCnt --;
            }
        }

    if (KErrNone == ret)
        {
        SetActive();
        }
    else if (KErrEof != ret)
        {
        iParent.DataError(ret);
        }

    OstTraceFunctionExit0(CNCMDATARECEIVER_READDATA_DUP01);
    }

/**
 * Expire the Share Chunk LDD's buffer (see above TakeBuffer)
 */
void CNcmDataReceiver::ExpireBuffer(TAny* aBuf)
    {
    TInt ret = iEpIn.Expire(aBuf);
    __ASSERT_DEBUG(KErrNone==ret, User::Panic(KDataReceiverPanic, __LINE__));
    }

/**
 * Deliver the received packet data to upper link
 */
void CNcmDataReceiver::ProcessEtherFrame(RMBufPacket& aPacket)
    {
    iParent.ProcessDatagram(aPacket);
    }

/**
 * RunL, a state machine to read the NCM packet data from NCM host
 */
void CNcmDataReceiver::RunL()
    {
    if(KErrNone == iStatus.Int())
        {
        ReadData();
        }
    else
        {
        OstTrace1(TRACE_ERROR, CNCMDATARECEIVER_RUNL, "iStatus.Int()=%d", iStatus.Int());
        iParent.DataError(iStatus.Int());
        }
    }

/**
 * Cancel the outgoing read request
 */
void CNcmDataReceiver::DoCancel()
    {
    OstTraceFunctionEntry0(CNCMDATARECEIVER_DOCANCEL);
    iPort.ReadCancel(iEpIn.BufferNumber());
    }

void CNcmDataReceiver::Stop()
    {
    OstTraceFunctionEntry0(CNCMDATARECEIVER_STOP);
    Cancel();
    iNtbParser->Reset();
    TInt ret = iEpIn.Close();

    OstTraceFunctionExit0(CNCMDATARECEIVER_STOP_DUP01);
    }
