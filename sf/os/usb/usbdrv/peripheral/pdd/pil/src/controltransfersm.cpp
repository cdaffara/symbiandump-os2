/*
  Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
  All rights reserved.

  This program and the accompanying materials are made available 
  under the terms of the Eclipse Public License v1.0 which accompanies 
  this distribution, and is available at 
  http://www.eclipse.org/legal/epl-v10.html

  Initial Contributors:
  Nokia Corporation - initial contribution.

  Contributors:
*/

#include "controltransfersm.h"

// Bitmap of setup packet
/*
Offset 0, bmRequestType, 1 bytes
    1 Bit-Map
        D7 Data Phase Transfer Direction
        0 = Host to Device
        1 = Device to Host
    
        D6..5 Type
        0 = Standard
        1 = Class
        2 = Vendor
        3 = Reserved
    
        D4..0 Recipient
        0 = Device
        1 = Interface
        2 = Endpoint
        3 = Other
        4..31 = Reserved

Offset 1, bRequest

Offset 6, Count, 2 bytes
        Number of bytes to transfer if there is a data phase
*/

#if defined(_DEBUG)

#define CTSM_ID "ControlTransferSM "

char* DebugName[] = 
    {
    "Setup",
    "Data Out",
    "Status In",
    "Data In",
    "Status Out"
    };
    
#endif

// Static data instance
TUsbcSetup TSetupPkgParser::iSetupPkt;

TSetupPkgParser::TSetupPkgParser()
    {
    iSetupPkt.iRequestType = 0;
    iSetupPkt.iRequest = 0;
    
    iSetupPkt.iValue  = 0;
    iSetupPkt.iIndex  = 0;
    iSetupPkt.iLength = 0;
    }

// Code for TSetupPkgParser
// we do a bitwise copy here.
void TSetupPkgParser::Set(const TUint8* aSetupBuf)
    {
    // TUint8 index
    iSetupPkt.iRequestType = static_cast<const TUint8*>(aSetupBuf)[0];
    iSetupPkt.iRequest = static_cast<const TUint8*>(aSetupBuf)[1];
    // TUint16 index from here!
    iSetupPkt.iValue = SWAP_BYTES_16((reinterpret_cast<const TUint16*>(aSetupBuf))[1]);
    iSetupPkt.iIndex = SWAP_BYTES_16((reinterpret_cast<const TUint16*>(aSetupBuf))[2]);
    iSetupPkt.iLength = SWAP_BYTES_16((reinterpret_cast<const TUint16*>(aSetupBuf))[3]);
    
    __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "TSetupPkgParser::Set"));
    }

// return the next stage by decoding the setup packet 
// the possible stage followed by a setup packet are:
//      StatusIn (no data stage)
//      DataOut  (host sent to peripheral)
//      DataIn   (peripheral to host)
UsbShai::TControlStage TSetupPkgParser::NextStage()
    {
    UsbShai::TControlStage ret = UsbShai::EControlTransferStageMax;
    
    // Take the data length out, 0 length means no data stage
    if (iSetupPkt.iLength == 0)
        {
        ret = UsbShai::EControlTransferStageStatusIn;
        }
    else if ((iSetupPkt.iRequestType & KUsbRequestType_DirMask) == KUsbRequestType_DirToDev)
        {
        // Dir to device means host will send data out
        ret = UsbShai::EControlTransferStageDataOut;
        }
    else
        {
        // Otherwise, there must be a datain stage follows
        ret = UsbShai::EControlTransferStageDataIn;
        } 
    
    __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "TSetupPkgParser::NextStage %d", ret));
    
    return ret;
    }

// Base class of stage sm
TControlStageSm::TControlStageSm(DControlTransferManager& aTransferMgr):
    iTransferMgr(aTransferMgr)
    {    
    }

void TControlStageSm::ChangeToStage(UsbShai::TControlStage aToStage)
    {
    __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "<> TControlStageSm::ChangeToStage: %s",DebugName[aToStage]));
    iTransferMgr.iCurrentStage = aToStage;
    }

void TControlStageSm::ClearPendingRead()
    {
    iTransferMgr.iReadPending = EFalse;
    }
    
// Code for DControlTransferManager
// 

DControlTransferManager::DControlTransferManager(MControlTransferIf& aCtrTransIf):
    iCtrTransferIf(aCtrTransIf)
    {    
    for(int i=0; i<UsbShai::EControlTransferStageMax; i++)
        {
        iState[i] = NULL;
        }
    
    Reset();
    }

TInt DControlTransferManager::SetupEndpointZeroRead()
    {
    if(iState[iCurrentStage]->IsRequstAllowed(TControlTransferRequestRead))
        {
        if(!iReadPending)
            {
            iReadPending = ETrue;
            return CtrTransferIf().ProcessSetupEndpointZeroRead();
            }
        else
            {
            // A read operation already on going, ignore this request
            return KErrNone;
            }
        }
    else
        {
        __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID " !! SetupEndpointZeroRead discard"));
        return KErrNotReady;
        }
    }
    
TInt DControlTransferManager::SetupEndpointZeroWrite(const TUint8* aBuffer, TInt aLength, TBool aZlpReqd)
    {
    if(iState[iCurrentStage]->IsRequstAllowed(TControlTransferRequestWrite))
        {
        return CtrTransferIf().ProcessSetupEndpointZeroWrite(aBuffer,aLength,aZlpReqd);
        }
    else
        {
        __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID " !! SetupEndpointZeroWrite discard"));
        return KErrNotReady;
        }    
    }
    
TInt DControlTransferManager::SendEp0ZeroByteStatusPacket()
    {
     if(iState[iCurrentStage]->IsRequstAllowed(TControlTransferRequestSendStatus))
        {
        iCurrentStage = UsbShai::EControlTransferStageSetup;
        return CtrTransferIf().ProcessSendEp0ZeroByteStatusPacket();
        }
    else
        {
        __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID " !! SendEp0ZeroByteStatusPacket discard"));
        return KErrNotReady;
        }
    }
    
TInt DControlTransferManager::StallEndpoint(TInt aRealEndpoint)
    {
    // Endpoint is stalled, we need to reset our state machine.
    Reset();
    return CtrTransferIf().ProcessStallEndpoint(aRealEndpoint);
    }

void DControlTransferManager::Ep0SetupPacketProceed()
    {
    __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID " !! Missed setup packet processed"));
    CtrTransferIf().ProcessEp0SetupPacketProceed();
    }
        
void DControlTransferManager::Ep0DataPacketProceed()
    {
    __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID " !! Missed data packet processed")); 
    CtrTransferIf().ProcessEp0DataPacketProceed();
    }
    
void DControlTransferManager::Reset()
    {
    iCurrentStage = UsbShai::EControlTransferStageSetup;
    iReadPending = EFalse;
    }
    
void DControlTransferManager::Ep0RequestComplete(TUint8* aBuf, 
                                             TInt aCount, 
                                             TInt aError, 
                                             UsbShai::TControlPacketType aPktType)
    {   
    __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "> DControlTransferManager::Ep0RequestComplete, packet type: %s", DebugName[aPktType])); 
    // If a setup packet comes, update our local setup packet buffer first
    if(aPktType == UsbShai::EControlPacketTypeSetup)
        {
        iPacketParser.Set(aBuf);
        // This is the only place this variable to be reset.
        iDataTransfered = 0;
        }
    
    // RequestComplete will return ETrue if it can not handle a packet
    // And it knows that some other sm can handle it.
    // It will update the state to the one which can hanlde that packet first.        
    TBool furtherProcessNeeded = ETrue;
    while(furtherProcessNeeded)
        {        
        __KTRACE_OPT(KUSB, Kern::Printf("   We're at Stage:         %s", DebugName[iCurrentStage]));
        furtherProcessNeeded = iState[iCurrentStage]->RequestComplete(aCount,aError,aPktType);
        __KTRACE_OPT(KUSB, Kern::Printf("   We're moved to stage:   %s", DebugName[iCurrentStage]));
        }    
    
    __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "< DControlTransferManager::Ep0RequestComplete")); 
    }

// setup the state machine for a state
void DControlTransferManager::AddState(UsbShai::TControlStage aStage,TControlStageSm& aStageSm)
    {
    if( (aStage >= UsbShai::EControlTransferStageSetup) && (aStage < UsbShai::EControlTransferStageMax))
        {
        __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID " AddState(), Stage: %s", DebugName[aStage]));
        iState[aStage] = &aStageSm;
        }
    }
        
// *************** Code for SETUP state machines **************************************
//
DSetupStageSm::DSetupStageSm(DControlTransferManager& aTransferMgr):
    TControlStageSm(aTransferMgr)
    {
    }

// WE are waiting a SETUP packet
TBool DSetupStageSm::RequestComplete(TInt aPktSize, TInt aError, UsbShai::TControlPacketType aPktType)
    {
    TBool ret = EFalse;
    
    __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "DSetupStageSm::RequestComplete"));
    
    if(aPktType != UsbShai::EControlPacketTypeSetup)
        {
        // we just discard any non-setup packet
        __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "ALERT: DSetupStageSm - Non-Setup recieved"));
        return ret;
        }
    
    // change state to whatever returned from this call.
    ChangeToStage(iTransferMgr.PktParser().NextStage());
    
    // We're going to complete User's read request, consume the previous
    // read operation
    ClearPendingRead();
    
    // Setup packet are always need to be processed
    iTransferMgr.CtrTransferIf().ProcessSetupPacket(aPktSize,aError);
    
    return EFalse;
    }

TBool DSetupStageSm::IsRequstAllowed(TControlTransferRequest aRequest)
    {
    // Allow user to read, No other operation is allowed
    TBool ret = (aRequest == TControlTransferRequestRead)?ETrue:EFalse;
    
    if( ! ret)
        {
        __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "Warning: request %d was blocked at DSetupStageSm",aRequest));
        }
        
    return ret;
    }

    
// *************** Code for DATA IN state machines **************************************

DDataInStageSm::DDataInStageSm(DControlTransferManager& aTransferMgr):
    TControlStageSm(aTransferMgr)
    {
    }

// We are waiting for a competion of DATA IN packet
TBool DDataInStageSm::RequestComplete(TInt aPktSize, TInt aError, UsbShai::TControlPacketType aPktType)
    {
    TInt completionCode = aError;
    TBool furtherRequest = EFalse;
    
    switch(aPktType)
        {
        case UsbShai::EControlPacketTypeSetup:
            {
            __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "ALERT: DDataInStageSm - Setup recieved"));
            // Something goes wrong, host is abandoning the unfinished control transfer
            completionCode = KErrGeneral;
            
            // Force SM restart at setup stage
            ChangeToStage(UsbShai::EControlTransferStageSetup);
            
            // this packet is partially processed here
            // need another SM to continue
            furtherRequest = ETrue;
            }
            break;
            
        case UsbShai::EControlPacketTypeDataIn:
            {
            // PSL notifing us that the data had been sent to host
            // next step is to wait for the status from Host
            ChangeToStage(UsbShai::EControlTransferStageStatusOut);
             
            // In USB spec, a compete control transfer must inclue a status stage
            // which is not case in reality,some PSL/Hardware will swallow the
            // Status out report, so, we just complete client normally.
            }
            break;
            
        default:
            {
            __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "ALERT: DDataInStageSm -  %s recieved",DebugName[aPktType]));
            
            // Unexpected packet will be discard, and lead us reset state machine
            // so that we can wait for next SETUP packet.
            // Of course error will be report to any client if any there.
            ChangeToStage(UsbShai::EControlTransferStageSetup);
            completionCode = KErrGeneral;
            }
            break;
        }
    
    iTransferMgr.CtrTransferIf().ProcessDataInPacket(aPktSize,completionCode);

    return furtherRequest;
    }

TBool DDataInStageSm::IsRequstAllowed(TControlTransferRequest aRequest)
    {
    // Only write is possible because host is waiting for data from us
    TBool ret = (aRequest == TControlTransferRequestWrite)?ETrue:EFalse;
    
    if( ! ret)
        {
        __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "Warning: request %d was blocked at DDataInStageSm",aRequest));
        }
        
    return ret;
    };
    
// *************** Code for STATUS OUT state machines **************************************
DStatusOutStageSm::DStatusOutStageSm(DControlTransferManager& aTransferMgr):
    TControlStageSm(aTransferMgr)
    {
    }

// We are waiting for a competion of STATUS OUT or a SETUP packet if PSL or hardware don't
// complete a status in packet
TBool DStatusOutStageSm::RequestComplete(TInt aPktSize, TInt aError, UsbShai::TControlPacketType aPktType)
    {
    TBool furtherRequest = EFalse;
    TInt completionCode = aError;
    
    switch(aPktType)
        {
        case UsbShai::EControlPacketTypeSetup:
            {
            __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "ALERT: DStatusOutStageSm - Setup recieved"));
            // hw or PSL may not send back the status packet for a DATA OUT
            // and we're ok for this, just back to EControlTransferStageSetup stage           
            
            // Force SM restart at setup stage
            ChangeToStage(UsbShai::EControlTransferStageSetup);
            
            // this packet is partially processed here
            // need another SM to continue
            furtherRequest = ETrue;
            }
            break;

        case UsbShai::EControlPacketTypeStatusOut:
            {
            // Force SM restart at setup stage
            ChangeToStage(UsbShai::EControlTransferStageSetup);
            }
            break;
        
        default:
            {
            __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "ALERT: DStatusOutStageSm -  %s recieved",DebugName[aPktType]));

            // Unexpected packet will be discard, and lead us reset state machine
            // so that we can wait for next SETUP packet.
            // Of course error will be report to any client if any there.            
            ChangeToStage(UsbShai::EControlTransferStageSetup);
            completionCode = KErrGeneral;
            }
            break;
        }

    iTransferMgr.CtrTransferIf().ProcessStatusOutPacket(completionCode);
        
    return furtherRequest;
    
    }

TBool DStatusOutStageSm::IsRequstAllowed(TControlTransferRequest aRequest)
    {
    // Read is ok since client don't care the status out stage.
    // and this lead no hurt to anybody.
    TBool ret = (aRequest == TControlTransferRequestRead)?ETrue:EFalse;
    
    if( ! ret)
        {
        __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "Warning: request %d was blocked at DStatusOutStageSm",aRequest));
        }
        
    return ret;
    };

// *************** Code for DATA OUT state machines **************************************
//
DDataOutStageSm::DDataOutStageSm(DControlTransferManager& aTransferMgr):
    TControlStageSm(aTransferMgr)
    {
    }

TBool DDataOutStageSm::RequestComplete(TInt aPktSize, TInt aError, UsbShai::TControlPacketType aPktType)
    {
    TBool furtherRequest = EFalse;
    TInt completionCode = aError;
    
    switch(aPktType)
        {
        case UsbShai::EControlPacketTypeSetup:
            {
            __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "ALERT: DDataOutStageSm - Setup recieved"));
            // Host is abandon the previous Transfer 
            completionCode = KErrGeneral;
            
            // Force SM restart at setup stage
            ChangeToStage(UsbShai::EControlTransferStageSetup);

            // this packet is partially processed here
            // need another SM to continue
            furtherRequest = ETrue;
            }
            break;
            
        case UsbShai::EControlPacketTypeDataOut:
            {
            iTransferMgr.DataReceived(aPktSize);
            
            if(!iTransferMgr.IsMoreBytesNeeded())
                {
                // We had recieved enough bytes as indicated by the setup
                // packet, Data stage is finished. enter STATUS IN state
                ChangeToStage(UsbShai::EControlTransferStageStatusIn);       
                }            
            }
            break;
        
        case UsbShai::EControlPacketTypeStatusIn:
            {
            // Status in had been sent to host
            // return and waiting for new SETUP
            ChangeToStage(UsbShai::EControlTransferStageSetup);
            }
            break;
            
        default:
            {
            __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "ALERT: DDataOutStageSm -  %s recieved",DebugName[aPktType]));
            
            // Unexpected packet will be discard, and lead us reset state machine
            // so that we can wait for next SETUP packet.
            // Of course error will be report to any client if any there.
            ChangeToStage(UsbShai::EControlTransferStageSetup);
            completionCode = KErrGeneral;
            }
            break;
        }
    
    ClearPendingRead();
    iTransferMgr.CtrTransferIf().ProcessDataOutPacket(aPktSize,completionCode);
    
    return furtherRequest;
    }

TBool DDataOutStageSm::IsRequstAllowed(TControlTransferRequest aRequest)
    {
    // only read operation is allowed in data out stage.
    TBool ret = (aRequest == TControlTransferRequestRead)?ETrue:EFalse;
    
    if( ! ret)
        {
        __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "Warning: request %d was blocked at DDataOutStageSm",aRequest));
        }
        
    return ret;
    };

// *************** Code for DATA OUT state machines **************************************
//
DStatusInStageSm::DStatusInStageSm(DControlTransferManager& aTransferMgr):
    TControlStageSm(aTransferMgr)
    {
    }

TBool DStatusInStageSm::RequestComplete(TInt aPktSize, TInt aError, UsbShai::TControlPacketType aPktType)
    {
    TBool furtherRequest = EFalse;
    TInt completionCode = KErrNone;
    
    switch(aPktType)
        {
        case UsbShai::EControlPacketTypeSetup:
            {
            __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "ALERT: DStatusInStageSm - Setup recieved"));
            // Status in is an optional for PSL
            // If we received a setup packet here, we assume the previous
            // status in packet had been successfully sent to host.
            
            // Force SM restart at setup stage
            ChangeToStage(UsbShai::EControlTransferStageSetup);
            
            // this packet is partially processed here
            // need another SM to continue
            furtherRequest = ETrue;
            }
            break;
            
        case UsbShai::EControlPacketTypeStatusIn:
            {
            // Status in had been recieved, monitor setup packet then.
            ChangeToStage(UsbShai::EControlTransferStageSetup);
            }
            break;
            
        default:
            {
            __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "ALERT: DStatusInStageSm -  %s recieved",DebugName[aPktType]));

            // Unexpected packet will be discard, and lead us reset state machine
            // so that we can wait for next SETUP packet.
            // Of course error will be report to any client if any there.
            ChangeToStage(UsbShai::EControlTransferStageSetup);
            completionCode = KErrGeneral;
            }
            break;
        }
    
    iTransferMgr.CtrTransferIf().ProcessStatusInPacket(completionCode);
    
    return furtherRequest;
    }

TBool DStatusInStageSm::IsRequstAllowed(TControlTransferRequest aRequest)
    {
    // Read is ok even we are wait for the client to send a zero status packet
    TBool ret = ((aRequest == TControlTransferRequestSendStatus) || 
                (aRequest == TControlTransferRequestRead))?ETrue:EFalse;
    
    if( ! ret)
        {
        __KTRACE_OPT(KUSB, Kern::Printf(CTSM_ID "Warning: request %d was blocked at DStatusInStageSm",aRequest));
        }
        
    return ret;
    }
    
// End of file

