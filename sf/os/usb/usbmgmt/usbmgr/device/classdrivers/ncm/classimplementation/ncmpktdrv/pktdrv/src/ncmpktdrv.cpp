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

#include "ncmpktdrv.h"

#include "ncmengine.h"
#include "ncmserver.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmpktdrvTraces.h"
#endif


/**
Constructor. Packet Driver object.
*/
CNcmPktDrv::CNcmPktDrv(CPktDrvFactory& aFactory) : CPktDrvBase(aFactory)
    {
    }

/**
Destructor.
*/
CNcmPktDrv::~CNcmPktDrv()
    {
    delete iNcmServer;
    delete iEngine;
    }

/**
Construction of the CNcmDrvBase object
@param aParent Pointer to the CLANLinkCommon class.
*/
void CNcmPktDrv::ConstructL(CLANLinkCommon* aParent)
    {
    OstTraceFunctionEntry0(CNCMPKTDRV_CONSTRUCTL);

    iParent = aParent;
    iEngine = CNcmEngine::NewL(*this);
    iNcmServer = CNcmServer::NewL(*iEngine);
    }

/**
Call to LDD or subordinate object to start/initialise the Physical device
*/
TInt CNcmPktDrv::StartInterface()
    {
    OstTraceFunctionEntry0(CNCMPKTDRV_STARTINTERFACE);

    TRAPD(err, iParent->FoundMACAddrL());
    if (KErrNone != err)
        {
        return err;
        }

    OstTrace0(TRACE_NORMAL, CNCMPKTDRV_STARTINTERFACE_DUP02, "CNcmPktDrv, calling LinkLayerUp");
    iParent->LinkLayerUp();
    OstTraceFunctionExit0(CNCMPKTDRV_STARTINTERFACE_DUP01);
    return err;
    }

/**
Call to LDD or subordinate object to stop/de-initialise the Physical device
*/
TInt CNcmPktDrv::StopInterface()
    {
    OstTraceFunctionEntry0(CNCMPKTDRV_STOPINTERFACE);

    iEngine->Stop();

    return KErrNone;
    }

/**
Call to LDD or subordinate object to reset/re-initialise the Physical device
*/
TInt CNcmPktDrv::ResetInterface()
    {
    return KErrNone;
    }

/**
EtherII MAC layer comments say we should free the packet buffer
RMBuf could contain a chain so get into a contiguous buffer
@param aPacket Reference to a chain of data buffers to be passed to the line.
@return 0 Tells the higher layer to send no more data.
        1 Tells higher layer that it can send more data.
*/
TInt CNcmPktDrv::Send(RMBufChain &aPacket)
    {
    TInt error = iEngine->Send(aPacket);
    aPacket.Free();

    return error;
    }

/**
Read the Available data.
@param aBuffer A Reference to a buffer holding data.
*/
void CNcmPktDrv::ReceiveEthFrame(RMBufPacket& aPacket)
    {
    iParent->Process(aPacket);
    }

/**
Resume Sending is a notification call into NIF from the lower layer telling the NIF that a
previous sending congestion situation has been cleared and it can accept more downstack data.
*/
void CNcmPktDrv::ResumeSending()
    {
    iParent->ResumeSending();
    }

/**
Call to LDD or subordinate object to set the receive mode of the LAN Device
@param aMode The mode to be set for the LAN Device.
@return KErrNotSupported LAN Device does not support.
*/
TInt CNcmPktDrv::SetRxMode(TRxMode /*aMode*/)
    {
    return KErrNotSupported;
    }

/**
Call to LDD or subordinate object to Get the receive mode of the LAN Device
@return KErrNotSupported LAN Device does not support.
*/
TInt CNcmPktDrv::GetRxMode() const
    {
    return KErrNotSupported;
    }

/**
Call to LDD or subordinate object to get the Hardware address of the LAN Device
@return NULL Failure.
        (NULL Terminated Binary String) The Hardware Address of the interface. LAN Device
        Specific
*/
TUint8* CNcmPktDrv::GetInterfaceAddress()const
    {
    return iEngine->InterfaceAddress();
    }

/**
Call to LDD or subordinate object to set the Hardware address of the LAN Device.
@param THWAddr Address of where the Multicast list should be written.
@return KErrNone         if Successful
        KErrNotSupported LAN Device does not support.
        Implementation specific Error Code  Failure
*/
TInt CNcmPktDrv::SetInterfaceAddress(const THWAddr&)
    {
    return KErrNotSupported;
    }

/**
Call to LDD or subordinate object to retrieve the Multicast List from the LAN Device
@param aAddr Address of where the Multicast list should be written.
@param n Output Parameter , number of Addresses written
@return KErrNone         if Successful
        KErrNotSupported LAN Device does not support.
        Implementation specific Error Code  Failure
*/
TInt CNcmPktDrv::GetMulticastList(const THWAddr* /*aAddr*/, TInt& /*n*/) const
    {
    return KErrNotSupported;
    }

/**
Call to LDD or subordinate object to set the Multicast List for the LAN Device.
@param aAddr Address of where the Multicast list should be written.
@param n Output Parameter , number of Addresses written
@return KErrNone         if Successful
        KErrNotSupported LAN Device does not support.
        Implementation specific Error Code  Failure
*/
TInt CNcmPktDrv::SetMulticastList(const THWAddr* /*aAddr*/, TInt /*n*/)
    {
    return KErrNotSupported;
    }

/**
Call to LDD or subordinate object to power up the LAN Device.
@return KErrNone         if Successful
        KErrNotSupported LAN Device does not support.
        Implementation specific Error Code  Failure
*/
TInt CNcmPktDrv::InterfacePowerUp()
    {
    return KErrNotSupported;
    }

/**
Call to LDD or subordinate object to power down the LAN Device
@return KErrNone         if Successful
        KErrNotSupported LAN Device does not support.
        Implementation specific Error Code  Failure
*/
TInt CNcmPktDrv::InterfacePowerDown()
    {
    return KErrNotSupported;
    }

/**
Call to LDD or subordinate object to suspend the LAN Device.
@return KErrNone         if Successful
        KErrNotSupported LAN Device does not support.
        Implementation specific Error Code  Failure
*/
TInt CNcmPktDrv::InterfaceSleep()
    {
    return KErrNotSupported;
    }

/**
Call to LDD or subordinate object to resume the LAN Device.
@return KErrNone         if Successful
        KErrNotSupported LAN Device does not support.
        Implementation specific Error Code  Failure
*/
TInt CNcmPktDrv::InterfaceResume()
    {
    return KErrNotSupported;
    }

/**
Receive notifications from agent
*/
TInt CNcmPktDrv::Notification(enum TAgentToNifEventType aEvent, void* aInfo)
    {
    TInt retval = KErrNotSupported;

    return retval;
    }

/**
Receive Control() calls from agent/nifman/connection
*/
TInt CNcmPktDrv::Control(TUint /*aLevel*/,TUint /*aName*/,TDes8& /*aOption*/, TAny* /*aSource*/)
    {
    return KErrNotSupported;
    }

/**
Report the progress of packet driver to NCM Class Controller for fatal error
*/
void CNcmPktDrv::FatalErrorNotification(TInt aError)
    {
    iParent->IfProgress(ENcmPktDrvFatalError, aError);
    }

#ifdef __OVER_DUMMYUSBSCLDD__
TInt CNcmPktDrv::State() const
    {
    return iEngine->State();
    }
#endif

