/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Active object to send and receive management packets (EAP packets).
*
*/


#ifndef WLANMGMTFRAMEHANDLER_H
#define WLANMGMTFRAMEHANDLER_H

#include <e32base.h>
#include "RWlanLogicalChannel.h"
#include "FrameXferBlock.h"
#include "am_platform_libraries.h"

class RWlanLogicalChannel;
class MWlanMgmtPacket;

/**
 * Callback interface
 */
class MWlanMgmtFrameCallback
    {
    public:
        virtual void OnFrameReceive(
            const TDataBuffer::TFrameType aFrameType,
            const TUint aLength,
            const TUint8* const aData,
            TUint8 aRcpi ) = 0;
    };

/**
 * Management packet handler.
 * Active object that waits for management packets from
 * driver side.
 * @since S60 v3.0
 * @lib wlmserversrv.lib
 */
NONSHARABLE_CLASS( CWlanMgmtFrameHandler ) : public CActive
    {
public: // Constructors and destructor

    /**
     * C++ default constructor.
     * @param aServiceProvider Interface to drivers.
     * @param aClient          Interfce to client.
     */
    CWlanMgmtFrameHandler(
        RWlanLogicalChannel& aServiceProvider,
        MWlanMgmtFrameCallback& aClient );

    /**
     * Static constructor.
     * @param aServiceProvider Interface to drivers.
     * @param aClient          Interfce to client.
     */
    static CWlanMgmtFrameHandler* NewL(
        RWlanLogicalChannel& aServiceProvider,
        MWlanMgmtFrameCallback& aClient);

    /**
     * Destructor.
     */
    virtual ~CWlanMgmtFrameHandler();

public: // New functions

    /**
     * Activate the handler
     * Handler starts to listen for received management frames
     * @since S60 v.3.1
     * @return error code
     */
    TInt Start();

    /**
     * Deactivate the handler
     * Handler stops to listen for received management frames
     * @since S60 v.3.1
     */
    void Stop();

    /**
    * Send management packet.
    * @param aFrameType         The type of the frame to send.
    * @param aLength            The length of sent data.
    * @param aData              The sent data.
    * @param aUserPriority      The user priority of the frame.
    * @param aDestination       Destination MAC address.
    * @param aSendUnencrypted   Whether the frame must be sent unencrypted.
    * @return error code
    */
    TInt SendFrame(
        const TDataBuffer::TFrameType aFrameType,
        const TUint aLength, 
        const TUint8* const aData,        
        TUint8 aUserPriority,
        const TMacAddress& aDestination,
        TBool aSendUnencrypted );

public: // Functions from base classes

    /**
    * From CActive Is executed when synchronous request is ready.
    */
    void RunL();

    /**
    * From CActive Cancel synchronous request.
    */
    void DoCancel();

private: // Functions

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
    /**
     * Request a synchronous call for management packets.
     */
    void IssueRequest();

private: // Data

    /**
     * Interface to receive packets.
     */
    RWlanLogicalChannel& iServiceProvider;
    /**
     * Interface to forward packets.
     */
    MWlanMgmtFrameCallback& iClient;
    /** 
     * Data transfer buffer. Not owned by this pointer.
     */
    RFrameXferBlock* iDataBlock;
    };

#endif // WLANMGMTFRAMEHANDLER_H
