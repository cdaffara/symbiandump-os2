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
* Description:  Routes data from peers to host and vice versa
*
*/



#ifndef CSOCKETROUTER_H
#define CSOCKETROUTER_H

//  INCLUDES
#include <e32base.h>
#include "Csocket.h"
#include "MSocketObserver.h"
#include "MProtocolObserverTCP.h"
#include "MProtocolObserverUDP.h"
#include "MUDPSenderObserver.h"
#include "CWriteEvent.h"

// DATA TYPES
enum TSocketRouterState
    {
    EStateWaitingFrameStart = 0,
    EStateFrameStartFound
    };

// FORWARD DECLARATIONS
class MBPProtocol;
class MSocketRouterObserver;
class CProtocolTCP;
class CProtocolUDP;
class CUDPSender;

// CLASS DECLARATION

/**
*  Routes data from peers to host and vice versa.
*/
NONSHARABLE_CLASS( CSocketRouter ) : public CActive,
    public MSocketObserver,
    public MProtocolObserverTCP,
    public MProtocolObserverUDP,
    public MUDPSenderObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aObserver Pointer to observer.
        */
        static CSocketRouter* NewL( MSocketRouterObserver* aObserver );

        /**
        * Two-phased constructor.
        * @param aObserver Pointer to observer.
        */
        static CSocketRouter* NewLC( MSocketRouterObserver* aObserver );

        /**
        * Destructor.
        */
        ~CSocketRouter();

    public: // New functions

        /**
        * Adds peer socket. The ownership transfers.
        * @param aSocket Opened RSocket object.
        */
        void AddPeerSocketL( RSocket* aSocket );

        /**
        * Adds UDP socket.
        * @param aPort Port which is "listened" for datagrams
        */
        void AddUDPSocketL( TUint aPort );

        /**
        * Removes peer socket from the array.
        * @param aIndex Index of array.
        */
        void RemovePeerSocket( TInt aIndex );

        /**
        * Removes peer socket from the array.
        * @param aSocket Socket to remove.
        */
        void RemovePeerSocket( RSocket* aSocket );

        /**
        * Finds peer socket from the array.
        * @param aPort A port to remove from the array
        * @return index of the peer socket, -1 if not found
        */
        TInt FindPeerSocket( TUint aPort );

        /**
        * Removes all peer sockets from the array.
        */
        void RemoveAllPeers();

        /**
        * Get socket array count
        * @return Number of CSocket instances in iPeerSocketArray
        */
        TInt SocketCount() const;

        /**
        * Sets the host socket. The ownership will NOT be transferred.
        */
        void SetHostSocketL( MSocket* aSocket );

        /**
        * Starts routing.
        */
        void StartRouting();

        /**
        * Stops routing.
        */
        void StopRouting();

        /**
        * @return ETrue if routing is activated and vice versa.
        */
        TBool IsRouting() const;

        /**
        * Resets message queue.
        */
        void ResetQueue();

        /**
        * Sends command via HTI to close the TCP connection
        * @param aPort Phone remote port (midlet) to close
        */
        void SendCloseTCPConnection( TUint aPort );

        /**
        * Writes the data to with the correct protocol
        * @param aProtocolDesc Which protocol was used to recieve this data
        * @param aPeerPort Peer port
        * @param aOriginalPort Port where the data was sent from
        * @param aData Data to be transmitted
        */
        void WriteCorrectFrameL( TProtocolDesc aProtocolDesc,
                                TUint aPeerPort,
                                TUint aOriginalPort,
                                const TDesC8& aData );

    protected: // Functions from base classes

        /**
        * From CActive. Pending request has been completed.
        */
        void RunL();

        /**
        * From CActive. Pending request has been cancelled.
        */
        void DoCancel();


    protected:  // From MSocketObserver

        void DataReceivedL( const MSocket* aSocket, const TDesC8& aData );
        void ErrorL( const MSocket* aSocket, TInt aErrorCode );
        void ObserverLeaved( const MSocket* aSocket, TInt aLeaveCode );
        void DisconnectedL( const MSocket* aSocket );

    protected:  // From MProtocolObserverTCP

        void TCPFrameParsedL( TUint aPort, const TDesC8& aData );
        void OpenLocalTCPConnectionL( TUint aPort );
        void OpenListeningTCPConnectionL( TUint aPort );
        void CloseTCPConnectionL( TUint aPort );
        void CloseAllTCPConnections();

    protected:  // From MProtocolObserverUDP

        void UDPFrameParsedL( TUint aPort, const TDesC8& aData );

    protected:  // From MProtocolObserver

        void FrameStarted();
        void ProtocolErrorL( TInt aErrorCode, const TDesC8& aReceivedData );

    protected:  // From MUDPSenderObserver

        void UDPSenderErrorL( TInt aErrorCode );
        void UDPSenderLeavedL( TInt aLeaveCode );

    private:

        void IssueHandleQueue();
        void WriteQueueL();

    protected:
        /**
        * Default constructor.
        * @param aObserver Pointer to observer.
        */
        CSocketRouter( MSocketRouterObserver* aObserver );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private:    // Owned data
        RSocketServ iSocketServ;
        TSocketRouterState iState;
        HBufC8* iReceiveBuffer;
        CArrayPtr<CSocket>* iPeerSocketArray;
        CArrayPtr<MBPProtocol>* iProtocolArray;
        MSocketRouterObserver* iObserver;
        MSocket* iHostSocket;
        TBool iRouting;
        CArrayPtr<CWriteEvent>* iWriteEventArray;
        CUDPSender* iUDPSender;

    private:    // Not owned
        CProtocolTCP* iProtocolTCP;
        CProtocolUDP* iProtocolUDP;
    };

#endif      // CSOCKETROUTER_H

// End of File
