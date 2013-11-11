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
#ifndef NCMPKTDRV_H
#define NCMPKTDRV_H


#include <e32base.h>
#ifndef __OVER_DUMMYUSBSCLDD__
#include <nifmbuf.h>
#include <es_mbuf.h>
#include <networking/pktdrv.h>
#include <networking/ethinter.h>
#else
#include <usb/testncm/ethinter.h>
#endif

class CNcmEngine;
class CNcmServer;

/**
The Ncm packet driver
*/
NONSHARABLE_CLASS(CNcmPktDrv) : public CPktDrvBase
    {
public:
    CNcmPktDrv(CPktDrvFactory& aFactory);
    virtual ~CNcmPktDrv();
    virtual void ConstructL(CLANLinkCommon* aParent);

    //From CPktDrvBase
    virtual TInt StartInterface();
    virtual TInt StopInterface();
    virtual TInt ResetInterface();
    virtual TInt SetRxMode(TRxMode AMode);
    virtual TInt GetRxMode() const;
    virtual TInt SetInterfaceAddress(const THWAddr&);
    virtual TUint8* GetInterfaceAddress()const;
    virtual TInt GetMulticastList(const THWAddr* aAddr, TInt& n) const;
    virtual TInt SetMulticastList(const THWAddr* aAddr, TInt n);
    virtual TInt InterfacePowerUp();
    virtual TInt InterfacePowerDown();
    virtual TInt InterfaceSleep();
    virtual TInt InterfaceResume();
    virtual TInt Notification(enum TAgentToNifEventType aEvent, void* aInfo);
    virtual TInt Control(TUint aLevel,TUint aName,TDes8& aOption, TAny* aSource=0);
    virtual TInt Send(RMBufChain& aPkt);

    //Upcall from Control Object
    void ReceiveEthFrame(RMBufPacket&);
    void ResumeSending();
    void FatalErrorNotification(TInt aError);

#ifdef __OVER_DUMMYUSBSCLDD__
public:
    TInt State() const;
#endif

private:
    CNcmEngine*        iEngine;
    CNcmServer*        iNcmServer;
    };

#endif // NCMPKTDRV_H
