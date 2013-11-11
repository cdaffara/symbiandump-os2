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
* implementation for read and write data from share chunk LDD 
*
*/


/**
@internalComponent
*/

#include "ncmcommunicationinterface.h"
#include "ncmengine.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmcomminterfacesenderandreceiverTraces.h"
#endif



// ======== MEMBER FUNCTIONS ========
//

CNcmCommInterfaceSenderAndReceiver::CNcmCommInterfaceSenderAndReceiver(RDevUsbcScClient& aPort, CNcmCommunicationInterface& aComm) 
    : CActive(CActive::EPriorityStandard),
    iPort(aPort),
    iCommInterface(aComm),
    iState(EUnInit)
    {
    CActiveScheduler::Add(this);
    }

CNcmCommInterfaceSenderAndReceiver* CNcmCommInterfaceSenderAndReceiver::NewL(RDevUsbcScClient& aPort, CNcmCommunicationInterface& aComm)
    {
    return new(ELeave) CNcmCommInterfaceSenderAndReceiver(aPort, aComm);
    }

CNcmCommInterfaceSenderAndReceiver::~CNcmCommInterfaceSenderAndReceiver()
    {
    Stop();
    }

void CNcmCommInterfaceSenderAndReceiver::Start()
    {
    OstTraceFunctionEntry1( CNCMCOMMINTERFACESENDERANDRECEIVER_START_ENTRY, this );
    
    TInt ret = iPort.OpenEndpoint(iEp0Buffer, KEp0Number);
    if (ret != KErrNone)
        {
        OstTrace1( TRACE_FATAL, CNCMCOMMINTERFACESENDERANDRECEIVER_START1, "OpenEndpoint error %d", ret);
        iCommInterface.ControlMsgError(EInternalError);
        OstTraceFunctionExit1( CNCMCOMMINTERFACESENDERANDRECEIVER_START_EXIT, this );
        return;
        }
    iState = EIdle;
    OstTraceFunctionExit1( CNCMCOMMINTERFACESENDERANDRECEIVER_START_EXIT_DUP1, this );
    }

void CNcmCommInterfaceSenderAndReceiver::Stop()
    {
    OstTraceFunctionEntry0( CNCMCOMMINTERFACESENDERANDRECEIVER_STOP_ENTRY );
    
    iState = EIdle;
    Cancel();    
    iEp0Buffer.Close();
    OstTraceFunctionExit0( CNCMCOMMINTERFACESENDERANDRECEIVER_STOP_EXIT );
    }


//
//Read data of aLength from LDD
//

TInt CNcmCommInterfaceSenderAndReceiver::Read(TRequestStatus& aStatus, TDes8& aBuf, TInt aLength)
    {
    OstTraceFunctionEntry1( CNCMCOMMINTERFACESENDERANDRECEIVER_READ_ENTRY, this );   
    OstTrace1( TRACE_NORMAL, CNCMCOMMINTERFACESENDERANDRECEIVER_READ, "CNcmCommInterfaceSenderAndReceiver::Read length %d", aLength );
        
    iCompleteStatus = &aStatus;
    *iCompleteStatus = KRequestPending;
    if (iState != EIdle)
        {
        OstTrace0( TRACE_FATAL, CNCMCOMMINTERFACESENDERANDRECEIVER_READ_DUP1, "CNcmCommInterfaceSenderAndReceiver Read In Use" );
        OstTraceFunctionExit1( CNCMCOMMINTERFACESENDERANDRECEIVER_READ_EXIT, this );
        return KErrInUse;
        }
    iReceiveBuf = &aBuf;
    iReceiveBuf->Zero();
    iToReceiveLength = aLength;
    iState = EReceiving;
    ReadData();
    OstTraceFunctionExit1( CNCMCOMMINTERFACESENDERANDRECEIVER_READ_EXIT_DUP1, this );
    return KErrNone;
    }

//
//write aLength data to ldd
//
TInt CNcmCommInterfaceSenderAndReceiver::Write(TRequestStatus& aStatus, TDesC8& aBuf, TInt aLength)
    {
    OstTraceFunctionEntry1( CNCMCOMMINTERFACESENDERANDRECEIVER_WRITE_ENTRY, this );
    OstTrace1( TRACE_NORMAL, CNCMCOMMINTERFACESENDERANDRECEIVER_WRITE, "CNcmCommInterfaceSenderAndReceiver::Write length %d", aLength );

    iCompleteStatus = &aStatus;
    *iCompleteStatus = KRequestPending;

    if (iState != EIdle)
        {
        OstTrace0( TRACE_FATAL, CNCMCOMMINTERFACESENDERANDRECEIVER_WRITE1, "CNcmCommInterfaceSenderAndReceiver::Write in Use" );        
        OstTraceFunctionExit1( CNCMCOMMINTERFACESENDERANDRECEIVER_WRITE_EXIT, this );
        return KErrInUse;
        }

    TAny *buf;
    TUint size;
    TInt ret = iEp0Buffer.GetInBufferRange(buf, size);
    if (ret != KErrNone)
        {
        OstTrace0( TRACE_FATAL, CNCMCOMMINTERFACESENDERANDRECEIVER_WRITE2, "CNcmCommInterfaceSenderAndReceiver::Write in Use" );        
        OstTraceFunctionExit1( CNCMCOMMINTERFACESENDERANDRECEIVER_WRITE_EXIT_DUP1, this );
        return ret;
        }
    else if (size < aLength)
        {
        OstTrace0( TRACE_FATAL, CNCMCOMMINTERFACESENDERANDRECEIVER_WRITE3, "write data is bigger than ldd buffer size" );        
        OstTraceFunctionExit1( CNCMCOMMINTERFACESENDERANDRECEIVER_WRITE_EXIT_DUP2, this );
        return KErrArgument;
        }

    TPtr8 writeBuf((TUint8 *)buf, size);
    writeBuf.Copy(aBuf.Ptr(), aLength);    
    ret = iEp0Buffer.WriteBuffer(buf, writeBuf.Size(), ETrue, iStatus);
    if (ret != KErrNone)
        {
        OstTrace1( TRACE_FATAL, CNCMCOMMINTERFACESENDERANDRECEIVER_WRITE4, "WriteBuffer error %d", ret );
        OstTraceFunctionExit1( CNCMCOMMINTERFACESENDERANDRECEIVER_WRITE_EXIT_DUP3, this );
        return ret;
        }
    iState = ESending;
    SetActive();
    OstTraceFunctionExit1( CNCMCOMMINTERFACESENDERANDRECEIVER_WRITE_EXIT_DUP4, this );
    return KErrNone;
    }



//
//Read data from USB share chunk LDD
//
void CNcmCommInterfaceSenderAndReceiver::ReadData()
    {
    OstTraceFunctionEntry1( CNCMCOMMINTERFACESENDERANDRECEIVER_READDATA_ENTRY, this );
    TInt ret;
    TAny* buf;
    TUint8* receivebuf; 
    TUint receiveLen;
    TBool zlp;

    FOREVER
        {
        ret = iEp0Buffer.GetBuffer(buf, receiveLen, zlp, iStatus, iToReceiveLength);
        receivebuf = (TUint8*)buf; 
        if (ret == TEndpointBuffer::KStateChange)
            {
            OstTrace0( TRACE_NORMAL, CNCMCOMMINTERFACESENDERANDRECEIVER_READDATA, "receive state change, discard it" );
            continue;
            }

        else if (ret == KErrCompletion)
            {
            OstTrace1( TRACE_NORMAL, CNCMCOMMINTERFACESENDERANDRECEIVER_READDATA1, "read data length %d from ldd", receiveLen );
            if (receiveLen > iToReceiveLength)
                {
                User::RequestComplete(iCompleteStatus, KErrOverflow);
                iState = EIdle;
                OstTrace0( TRACE_NORMAL, CNCMCOMMINTERFACESENDERANDRECEIVER_READDATA2,"data from ldd is bigger than receive buffer" );
                OstTraceFunctionExit1( CNCMCOMMINTERFACESENDERANDRECEIVER_READDATA_EXIT, this );
                return;
                }
            
            iReceiveBuf->Append(receivebuf, receiveLen);
            iToReceiveLength -= receiveLen;
            if (!iToReceiveLength)
                {        
                User::RequestComplete(iCompleteStatus, KErrNone);
                iState = EIdle;
                OstTrace0( TRACE_NORMAL, CNCMCOMMINTERFACESENDERANDRECEIVER_READDATA3,"Complete read request" );
                OstTraceFunctionExit1( CNCMCOMMINTERFACESENDERANDRECEIVER_READDATA_EXIT_DUP1, this );
                return;
                }
            continue;
            }
        else if (ret == KErrNone)
            {
            break;
            }
        else
            {
            OstTrace1( TRACE_NORMAL, CNCMCOMMINTERFACESENDERANDRECEIVER_READDATA4, "GetBuffer error %d", ret);
            User::RequestComplete(iCompleteStatus, KErrNone);
            iState = EIdle;
            OstTraceFunctionExit1( CNCMCOMMINTERFACESENDERANDRECEIVER_READDATA_EXIT_DUP2, this );
            return;
            }
        }
    SetActive();
    OstTraceFunctionExit1( CNCMCOMMINTERFACESENDERANDRECEIVER_READDATA_EXIT_DUP3, this );
    }

//
//RunL, looply receive data from share chunk LDD until receive all required data
//
void CNcmCommInterfaceSenderAndReceiver::RunL()
    {
    OstTraceFunctionEntry1( CNCMCOMMINTERFACESENDERANDRECEIVER_RUNL_ENTRY, this );
    if(iStatus.Int() != KErrNone)
        {
        if (KErrCancel == iStatus.Int() )
            {
            OstTrace0( TRACE_NORMAL, CNCMCOMMINTERFACESENDERANDRECEIVER_RUNL,"CNcmCommInterfaceSenderAndReceiver, control channel is cancelled" );
            }
        else
            {
            OstTrace1( TRACE_NORMAL, CNCMCOMMINTERFACESENDERANDRECEIVER_RUNL1, "CNcmCommInterfaceSenderAndReceiver::RunL error %d", iStatus.Int());
            }
        User::RequestComplete(iCompleteStatus, iStatus.Int());
        }
    else
        {
        if (iState == EReceiving)
            {
            ReadData();
            }
        else if (iState == ESending)
            {
            User::RequestComplete(iCompleteStatus, KErrNone);
            iState = EIdle;
            OstTrace0( TRACE_NORMAL, CNCMCOMMINTERFACESENDERANDRECEIVER_RUNL2,"Complete write request" );
            }
        }
    OstTraceFunctionExit1( CNCMCOMMINTERFACESENDERANDRECEIVER_RUNL_EXIT, this );
    }

//
//Cancel the outgoing read request
//
void CNcmCommInterfaceSenderAndReceiver::DoCancel()
    {    
    OstTraceFunctionEntry1( CNCMCOMMINTERFACESENDERANDRECEIVER_DOCANCEL_ENTRY, this );
    iPort.ReadCancel(KUsbcScEndpointZero);
    iPort.WriteCancel(KUsbcScEndpointZero);
    OstTraceFunctionExit1( CNCMCOMMINTERFACESENDERANDRECEIVER_DOCANCEL_EXIT, this );
    }
