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
* header file of class for read and write data from share chunk LDD 
*
*/


/**
@file
@internalComponent
*/

#ifndef NCMCOMMINTERFACESENDERANDRECEIVER_H
#define NCMCOMMINTERFACESENDERANDRECEIVER_H

#include <e32base.h>

#ifndef __OVER_DUMMYUSBSCLDD__
#include <d32usbcsc.h>
#else
#include <dummyusbsclddapi.h>
#endif



class CNcmCommunicationInterface;

/**
Responsible for sending and receiving data on Control Endpoint
*/

NONSHARABLE_CLASS(CNcmCommInterfaceSenderAndReceiver) : public CActive
    {
public:
    static CNcmCommInterfaceSenderAndReceiver* NewL(RDevUsbcScClient& aPort, CNcmCommunicationInterface& aCommInterface);
    ~CNcmCommInterfaceSenderAndReceiver();
    void Start();
    void Stop();
    /**
    * read aLength data from LDD
    *
    * @param aStatus complete status for this request.
    * @param aBuf buffer to store data.    
    * @param aLength length to read.    
    */
    TInt Read(TRequestStatus& aStatus, TDes8& aBuf, TInt aLength);
    /**
    * write aLength data to LDD
    *
    * @param aStatus complete status for this request.
    * @param aBuf buffer of data.    
    * @param aLength length to write.    
    */
    TInt Write(TRequestStatus& aStatus, TDesC8& aBuf, TInt aLength);
    
private:
    enum TNcmControlEndpointState
        {
        EUnInit = 1,
        EIdle,
        EReceiving,
        ESending        
        };
    
    CNcmCommInterfaceSenderAndReceiver(RDevUsbcScClient& aPort, CNcmCommunicationInterface& aCommInterface);
    void ReadData();

private:
    /**
    *  derived from CActive
    */
    void RunL();
    void DoCancel();
    
private:
    RDevUsbcScClient&               iPort;
    /**
    *  LDD control endpoint buffer
    */
    TEndpointBuffer                 iEp0Buffer;
    CNcmCommunicationInterface&     iCommInterface;
    /**
    *  point to buffer to store data of Read function, this is not ownership
    */
    TDes8*                          iReceiveBuf;
    /**
    *  length of data left to be read from LDD for a Read request
    */
    TInt                            iToReceiveLength;
    /**
    *  complete status for Read and Write function, this is not ownership
    */
    TRequestStatus*                 iCompleteStatus;
    TNcmControlEndpointState        iState;

    };


#endif //NCMCOMMINTERFACESENDERANDRECEIVER_H

