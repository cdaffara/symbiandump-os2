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
* implementation of NCM communication interface class 
*
*/


#include <es_sock.h>
#include "ncmcommunicationinterface.h"
#include "ncmnotificationdescriptor.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmcommunicationinterfaceTraces.h"
#endif



const TUint KUsbRequestLengthIdx = 6;
const TUint KUsbRequestTypeIdx = 1;
const TInt  KIntEndpoint = 1;
const TInt  KNotificationRequestType = 0xA1;


#if defined(_DEBUG)
_LIT(KNcmCommInterfacePanic, "UsbNcmComm"); // must be <=16 chars
#endif


// Panic codes
enum TNcmCommPanicCode
    {
    ENcmCommWrongSetupLength = 1,
    ENcmCommWrongDataLength,
    ENcmCommWriteError,
    ENcmCMEndMark
    };


// ======== MEMBER FUNCTIONS ========
//

CNcmCommunicationInterface::CNcmCommunicationInterface(MNcmControlObserver& aEngine, RDevUsbcScClient& aLdd) : CActive(CActive::EPriorityHigh),
                            iEngine(aEngine), iPort(aLdd)
    {
    CActiveScheduler::Add(this);
    }

void CNcmCommunicationInterface::ConstructL()
    {
    iSenderAndReceiver = CNcmCommInterfaceSenderAndReceiver::NewL(iPort, *this);
    }

CNcmCommunicationInterface* CNcmCommunicationInterface::NewL(MNcmControlObserver& aEngine, RDevUsbcScClient& aLdd)
    {
    OstTraceFunctionEntry0( CNCMCOMMUNICATIONINTERFACE_NEWL_ENTRY );
    CNcmCommunicationInterface *self=new (ELeave) CNcmCommunicationInterface(aEngine, aLdd);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    OstTraceFunctionExit0( CNCMCOMMUNICATIONINTERFACE_NEWL_EXIT );
    return self;
    }

CNcmCommunicationInterface::~CNcmCommunicationInterface()
    {
    OstTraceFunctionEntry1( CNCMCOMMUNICATIONINTERFACE_CNCMCOMMUNICATIONINTERFACE_ENTRY, this );
    
    Cancel();
    delete iSenderAndReceiver;
    OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_CNCMCOMMUNICATIONINTERFACE_EXIT, this );
    }

//
//Start the control channel of NCM
//
TInt CNcmCommunicationInterface::Start()
    {
    OstTraceFunctionEntry1( CNCMCOMMUNICATIONINTERFACE_START_ENTRY, this );
    if (iStarted)
        {
        OstTrace0( TRACE_WARNING, CNCMCOMMUNICATIONINTERFACE_START, "CNcmCommunicationInterface, already started!" );        
        OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_START_EXIT, this );
        return KErrInUse;
        }

    TInt ret = GetInterfaceNumber();
    if (ret != KErrNone)
        {        
        OstTrace1( TRACE_FATAL, CNCMCOMMUNICATIONINTERFACE_START1, "GetInterfaceNumber failed ret=%d", ret);
        OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_START_EXIT_DUP1, this );
        return ret;
        }
    
    iStarted = ETrue;
    iRWState = EStateInitial;

    iSenderAndReceiver->Start();

    //force a call to RunL
    SetActive();
    TRequestStatus* status=&iStatus;
    User::RequestComplete(status, KErrNone);
    OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_START_EXIT_DUP2, this );
    return KErrNone;
    }

//
//Listen on the ep0 to receive the NCM control message from host
//
void CNcmCommunicationInterface::ReadSetup()
    {
    OstTraceFunctionEntry1( CNCMCOMMUNICATIONINTERFACE_READSETUP_ENTRY, this );
    iRWState = EStateReadSetup;
    iRequestType = 0;
    iDataStageLength = 0;
    TInt ret = iSenderAndReceiver->Read(iStatus, iSetupPacket, KSetupPacketLength);

    if (ret != KErrNone)
        {
        OstTrace1( TRACE_FATAL, CNCMCOMMUNICATIONINTERFACE_READSETUP, "read setup packet error %d", ret);        
        ControlMsgError(EInternalError);
        OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_READSETUP_EXIT, this );
        return;
        }
    SetActive();
    OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_READSETUP_EXIT_DUP1, this );
    }


//
//decode the setup packet to get command information.
//
void CNcmCommunicationInterface::DecodeSetup()
    {
    OstTraceFunctionEntry1( CNCMCOMMUNICATIONINTERFACE_DECODESETUP_ENTRY, this );
    
    __ASSERT_DEBUG(iSetupPacket.Length()==KSetupPacketLength, 
        User::Panic(KNcmCommInterfacePanic, ENcmCommWrongSetupLength));
		
    iRequestType = iSetupPacket[KUsbRequestTypeIdx];
    iDataStageLength = LittleEndian::Get16(&iSetupPacket[KUsbRequestLengthIdx]);
	
    switch (iRequestType)
        {
        case EGetNtbParameters:	
            if (iDataStageLength != KNtbParamStructLength)
                {
                ControlMsgError(EInvalidLengthToRead);
                break;
                }
            iEngine.HandleGetNtbParam(iDataBuffer);
            __ASSERT_DEBUG(iDataBuffer.Length()==KNtbParamStructLength, 
                User::Panic(KNcmCommInterfacePanic, ENcmCommWrongDataLength));			
            WriteDataIn();
            break;
          
        case EGetNtbInputSize:
            if (iDataStageLength != KNtbInputSizeStructLength)
                {
                ControlMsgError(EInvalidLengthToRead);
                break;
                }			
            iEngine.HandleGetNtbInputSize(iDataBuffer);
            __ASSERT_DEBUG(iDataBuffer.Length()==KNtbInputSizeStructLength, 
                User::Panic(KNcmCommInterfacePanic, ENcmCommWrongDataLength));						
            WriteDataIn();
            break;
			
        case ESetNtbInputSize:
            if (iDataStageLength != KNtbInputSizeStructLength)
                {
                ControlMsgError(EInvalidLengthToRead);
                break;
                }		
            ReadDataOut();
            break;
        default:
            TInt ret = iPort.EndpointZeroRequestError();
            OstTrace1( TRACE_ERROR, CNCMCOMMUNICATIONINTERFACE_DECODESETUP, "unsupport request, halt endpoint with EndpointZeroRequestError %d", ret);
            ReadSetup();
            break;
        }
    OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_DECODESETUP_EXIT, this );
    }



//
//Read the raw data of a control request message from host
//
void CNcmCommunicationInterface::ReadDataOut()
    {
    OstTraceFunctionEntry1( CNCMCOMMUNICATIONINTERFACE_READDATAOUT_ENTRY, this );
    iRWState = EStateReadDataout;
    iSenderAndReceiver->Read(iStatus, iDataBuffer, iDataStageLength);
    SetActive();
    OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_READDATAOUT_EXIT, this );
    }




//
//Parse the data out from host to specific NCM control message
//
void CNcmCommunicationInterface::ParseDataOut()
    {
    OstTraceFunctionEntry1( CNCMCOMMUNICATIONINTERFACE_PARSEDATAOUT_ENTRY, this );

    __ASSERT_DEBUG(iDataBuffer.Length()>0, 
        User::Panic(KNcmCommInterfacePanic, ENcmCommWrongDataLength));			

    TInt ret = KErrNone;
    switch (iRequestType)
        {
        case ESetNtbInputSize:
            ret = iEngine.HandleSetNtbInputSize(iDataBuffer);    
            break;
        default:
            ret = KErrNotSupported;
            break;
        }
    
    if (ret == KErrNone)
        {
        iPort.SendEp0StatusPacket();
        }
    else
        {
        OstTrace1( TRACE_WARNING, CNCMCOMMUNICATIONINTERFACE_PARSEDATAOUT, "handle request iRequestType error %d stall endpoint", ret);
        iPort.EndpointZeroRequestError();
        }
    ReadSetup();
    OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_PARSEDATAOUT_EXIT, this );
    }
    
//
// send connection status notification, aConnected = ETrue if connection up, otherwise EFalse if connection discnnected
//
TInt CNcmCommunicationInterface::SendConnectionNotification(TBool aConnected)
    {
    OstTraceFunctionEntry1( CNCMCOMMUNICATIONINTERFACE_SENDCONNECTIONNOTIFICATION_ENTRY, this );
    const TUint8 KConnectionNotificationCode = 0x00;
    const TUint16 KConnectedCode = 0x0001;
    const TUint16 KDisconnectCode = 0x0000;    

    TUSBNotificationNetworkConnection netNotify;
    netNotify.iRequestType = KNotificationRequestType;
    netNotify.iNotification = KConnectionNotificationCode;
    netNotify.iValue = (aConnected)?KConnectedCode:KDisconnectCode; 
    netNotify.iIndex = iInterfaceNumber;
    netNotify.iLength = 0;

    return WriteInterruptData(KIntEndpoint, netNotify.Des(), 
        netNotify.Des().Length());            
    }

//
// send speed notification
//
TInt CNcmCommunicationInterface::SendSpeedNotification(TInt aUSBitRate, TInt aDSBitRate)
    {
    OstTraceFunctionEntry1( CNCMCOMMUNICATIONINTERFACE_SENDSPEEDNOTIFICATION_ENTRY, this );
    const TUint8 KSpeedNotificationCode = 0x2A;
        
    TUSBNotificationConnectionSpeedChange speedNotify;
    speedNotify.iRequestType = KNotificationRequestType;
    speedNotify.iNotification = KSpeedNotificationCode;
    speedNotify.iValue = 0x00; 
    speedNotify.iIndex = iInterfaceNumber;
    speedNotify.iLength = 0x08;
    speedNotify.iUSBitRate = aUSBitRate;
    speedNotify.iDSBitRate = aDSBitRate;

    return WriteInterruptData(KIntEndpoint, speedNotify.Des(), 
        speedNotify.Des().Length());    
    }


//
//According to the receving request message, send back a response to the host
//
void CNcmCommunicationInterface::WriteDataIn()
    {
    OstTraceFunctionEntry1( CNCMCOMMUNICATIONINTERFACE_WRITEDATAIN_ENTRY, this );

    iRWState = EStateWriteDatain;
    TInt ret = iSenderAndReceiver->Write(iStatus, iDataBuffer, iDataBuffer.Length());
    __ASSERT_DEBUG(ret==KErrNone, User::Panic(KNcmCommInterfacePanic, ENcmCommWriteError));		
    SetActive();
    OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_WRITEDATAIN_EXIT, this );
    }

//
//Cancel the outgoing request
//
void CNcmCommunicationInterface::DoCancel()
    {
    OstTraceFunctionEntry1( CNCMCOMMUNICATIONINTERFACE_DOCANCEL_ENTRY, this );
    TRequestStatus* status = &iStatus;    
    User::RequestComplete(status, KErrCancel); 
    OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_DOCANCEL_EXIT, this );
    }

//
//Stop the control channel to stop the NCM
//
void CNcmCommunicationInterface::Stop()
    {
    OstTraceFunctionEntry1( CNCMCOMMUNICATIONINTERFACE_STOP_ENTRY, this );
    iStarted = EFalse;
    iSenderAndReceiver->Stop();
    Cancel();
    OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_STOP_EXIT, this );
    }

//
//AO RunL
//
void CNcmCommunicationInterface::RunL()
    {
    OstTraceFunctionEntry1( CNCMCOMMUNICATIONINTERFACE_RUNL_ENTRY, this );
    if(iStatus.Int() != KErrNone)
        {
        if (KErrCancel == iStatus.Int() )
            {
            }
        else
            {
            ControlMsgError(EInternalError);
            }
        OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_RUNL_EXIT, this );
        return;
        }

    switch(iRWState)
        {
        case EStateInitial:
            {
            ReadSetup();
            break;
            }
            
        case EStateReadSetup:
            {
            DecodeSetup();
            break;
            }
            
        case EStateReadDataout:
            {
            ParseDataOut();
            break;
            }

        case EStateWriteDatain:
            {
            ReadSetup();
            break;
            }    
        }
    OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_RUNL_EXIT_DUP1, this );
    }

  
//
//Any fatal error occurs when reading/sending a NCM control message via USB interface
//
void CNcmCommunicationInterface::ControlMsgError(TNcmCommErrorCode aCode)
    {
    OstTraceFunctionEntry1( CNCMCOMMUNICATIONINTERFACE_CONTROLMSGERROR_ENTRY, this );
    OstTrace1( TRACE_NORMAL, CNCMCOMMUNICATIONINTERFACE_CONTROLMSGERROR, "CNcmCommunicationInterface, Handle Ncm Control Message with err=%d", aCode);
    
    // Stall bus, there's nothing else we can do 
    iPort.EndpointZeroRequestError();  
    iEngine.ControlError(aCode);
    OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_CONTROLMSGERROR_EXIT, this );
    }


TInt CNcmCommunicationInterface::WriteInterruptData(TInt aEndPoint, 
                               TDesC8& aDes, 
                               TInt aLength)

    {
    OstTraceFunctionEntry1( CNCMCOMMUNICATIONINTERFACE_WRITEINTERRUPTDATA_ENTRY, this );

    TInt ret;
    RTimer timer;
    ret = timer.CreateLocal();
    if ( ret )
        {
        OstTrace1( TRACE_FATAL, CNCMCOMMUNICATIONINTERFACE_WRITEINTERRUPTDATA, "\ttimer.CreateLocal = %d- returning", ret);        
        OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_WRITEINTERRUPTDATA_EXIT, this );
        return ret;
        }
    TRequestStatus status;
    TRequestStatus timerStatus;

    TEndpointBuffer epBuffer;
    ret = iPort.OpenEndpoint(epBuffer, aEndPoint);
    if (ret != KErrNone)
        {
        timer.Close();
        OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_WRITEINTERRUPTDATA_EXIT_DUP1, this );
        return ret;
        }

    TAny *buf;
    TUint size;
    ret = epBuffer.GetInBufferRange(buf, size);
    if (ret != KErrNone)
        {        
        timer.Close();
        OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_WRITEINTERRUPTDATA_EXIT_DUP2, this );
        return ret;
        }
    else if (size < aLength)
        {
        timer.Close();
        OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_WRITEINTERRUPTDATA_EXIT_DUP3, this );
        return KErrArgument;
        }
    
    TPtr8 writeBuf((TUint8 *)buf, size);
    writeBuf.Copy(aDes.Ptr(), aLength);
    ret = epBuffer.WriteBuffer(buf, writeBuf.Size(), ETrue, status);
    if (ret != KErrNone)
        {
        timer.Close();
        OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_WRITEINTERRUPTDATA_EXIT_DUP4, this );
        return ret;
        }

    const TInt KWriteDataTimeout = 1000000;
    timer.After(timerStatus, KWriteDataTimeout);
    User::WaitForRequest(status, timerStatus);
    if ( timerStatus != KRequestPending )
        {
        // Timeout occurred, silently ignore error condition.
        // Assuming that the line has been disconnected
        OstTrace0( TRACE_ERROR, CNCMCOMMUNICATIONINTERFACE_WRITEINTERRUPTDATA1, "CNcmCommunicationInterface::WriteInterruptData() - Timeout occurred");
        iPort.WriteCancel(epBuffer.BufferNumber());
        User::WaitForRequest(status);
        ret = timerStatus.Int();
        }
    else
        {
        OstTrace0( TRACE_ERROR, CNCMCOMMUNICATIONINTERFACE_WRITEINTERRUPTDATA2, "CNcmCommunicationInterface::WriteInterruptData() - Write completed");
        timer.Cancel();
        User::WaitForRequest(timerStatus);
        ret = status.Int();
        }
    
    epBuffer.Close();
    timer.Close();
    OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_WRITEINTERRUPTDATA_EXIT_DUP5, this );
    return ret;
    }


//
// Get interface number
//
TInt CNcmCommunicationInterface::GetInterfaceNumber()
    {
    OstTraceFunctionEntry1( CNCMCOMMUNICATIONINTERFACE_GETINTERFACENUMBER_ENTRY, this );

    TInt interfaceSize = 0;
    // 2 is where the interface number is, according to the LDD API
    const TInt intNumOffsetInDes = 2;

    // 0 means the main interface in the LDD API
    TInt res = iPort.GetInterfaceDescriptorSize(0, interfaceSize);

    if ( res )
        {
        OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_GETINTERFACENUMBER_EXIT, this );
        return res;
        }

    HBufC8* interfaceBuf = HBufC8::New(interfaceSize);
    if ( !interfaceBuf )
        {
        OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_GETINTERFACENUMBER_EXIT_DUP1, this );
        return KErrNoMemory;
        }

    TPtr8 interfacePtr = interfaceBuf->Des();
    interfacePtr.SetLength(0);
    // 0 means the main interface in the LDD API
    res = iPort.GetInterfaceDescriptor(0, interfacePtr); 

    if ( res )
        {
        delete interfaceBuf;
        OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_GETINTERFACENUMBER_EXIT_DUP2, this );
        return res;
        }

    const TUint8* buffer = reinterpret_cast<const TUint8*>(interfacePtr.Ptr());
    iInterfaceNumber = buffer[intNumOffsetInDes]; 

    delete interfaceBuf;
    OstTraceFunctionExit1( CNCMCOMMUNICATIONINTERFACE_GETINTERFACENUMBER_EXIT_DUP3, this );
    return KErrNone;
    }



