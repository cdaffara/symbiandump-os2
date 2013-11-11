/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of CCommAdapter that uses media plugins to read
*        HTI messages and send new ones back. Because comm plug-in
*        interface define async functions, this class is AO.
*
*/


#ifndef MCOMMADAPTER_H__
#define MCOMMADAPTER_H__

#include <e32base.h>

//forward declaration
class CHtiDispatcher;
class CHTICommPluginInterface;
class CHtiMessage;

class CHtiCommAdapter: public CActive
    {
private:
    /**
    * Enum of different states depending on the current operation
    */
    enum TCommState
        {
        EIdle,
        EReceiving,
        EReceivingCont,
        ESending
        };

public:
    /**
    * Construct the object and adds it to CActiveScheduler.
    *
    * @param aCommPlugin communication plug-in
    * @param aDispatcher message dispatcher
    * @param aMaxMsgSize max size of an incoming message that will be
    *           accepted
    */
    static CHtiCommAdapter* NewL(CHTICommPluginInterface* aCommPlugin,
        CHtiDispatcher* aDispatcher,
        TInt aMaxMsgSize);

    /**
    * Construct the object and adds it to CActiveScheduler
    *
    * @param aCommPlugin communication plug-in
    * @param aDispatcher message dispatcher
    * @param aMaxMsgSize max size of an incoming message that will be
    *           accepted
    */
    static CHtiCommAdapter* NewLC(CHTICommPluginInterface* aCommPlugin,
        CHtiDispatcher* aDispatcher,
        TInt aMaxMsgSize);

    /**
    * Destructor
    */
    ~CHtiCommAdapter();

    /**
    * Issue an async request to receive whole HTI message and
    * pass it to the framework on complition.
    */
    void ReceiveMessage();

    /**
    * Send a HTI message via comm plug-in.
    * Ownership of aMessage is tranfered to CCommAdapter.
    * When message is sent, aMessage will be deleted.
    *
    * @param aMessage a HTI message to send
    */
    void SendMessage(CHtiMessage* aMessage);

    /**
    * Stop all ongoing operations and clean all internal buffers.
    */
    void Reset();

protected:

    CHtiCommAdapter(CHtiDispatcher* aDispatcher,
                 CHTICommPluginInterface* aCommPlugin,
                TInt aMaxMsgSize);

    void ConstructL();


    /**
    * Issue request to receive the next packet.
    * If aContinue = ETrue than iState is set to EReceivingCont
    * Otherwise it set to EReceiving.
    * iState set to EReceivingCont after message beginning was received
    * and its continuation is expected
    *
    * @param aContinue continuation flag
    */
    void ReceiveMessage(TBool aContinue);

    /**
    * Handle received data from buffer and reissue request
    */
    void HandleReceiveL();
    void HandleReceiveContL();

    /**
    * Reissue request to send message
    */
    void HandleSend();


protected: //from CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private: //data members

    /**
    * Current communication state
    */
    TCommState iState;

    /**
    * Pointer to message dispatcher.
    */
    CHtiDispatcher* iDispatcher;

    /**
    * Communication ECom plug-in
    */
    CHTICommPluginInterface* iCommPlugin;

    /**
    * Buffer for receiving data. It'is allocated once and used for
    * all subsequent receiving operation. Data from this buffer copied
    * to iMsgToReceive buffer that than passed to the framework for
    * processing
    */
    HBufC8* iBuffer;

    /**
    * Pointer to the buffer for receiveing data
    */
    TPtr8 iBufferPtr;

    /**
    * HTI incoming message. It's passed to the framework
    * when whole message is received
    */
    CHtiMessage* iMsgToReceive;

    /**
    * Small buffer used when there is some data left in iBuffer that
    * cannot be processed (less than min header size)
    */
    HBufC8* iLeftovers;

    /**
    * Keep message passed in SendMessage() between calls to comm. plugin
    * and deleted when sending is complete
    */
    CHtiMessage* iMsgToSend;

    /**
    * Pointer to the part of iMsgToSend started from iMsgToSendOffset
    * that is being sent via current comm. plug-in
    */
    TPtrC8 iMsgToSendPtr; //ptr to msg part passed to plug-ins

    /**
    * Offset in iMsgToSend of a part that is being sent
    */
    TInt iMsgToSendOffset;

    /**
    * Amount of receving bytes to skip.
    * It's ussed when there is no memory for incoming message or
    * its size too big.
    */
    TInt iSkipLength;

    /**
    * Max message size to accept
    */
    const TInt iMaxMessageSize;
    };

#endif
