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
* Description:  ECOM plugin to communicate over IP port
*
*/


#ifndef CHTICONNECTIONMANAGER_H
#define CHTICONNECTIONMANAGER_H


// INCLUDES
#include <HtiCommPluginInterface.h> // defined in HtiFramework project
#include <es_sock.h>
#include <in_sock.h>
#include <commdbconnpref.h>

#include "HtiIPCommServerCommon.h"
#include "HtiSocketMonitor.h"
#include "HtiIPCommServer.h"


// FORWARD DECLARATIONS
class CHtiIPCommServer;
class CHtiCfg;

// CLASS DECLARATION

class MHtiTimerObserver
{
public:
    virtual void TimerExpiredL() = 0;
};


class CHtiTimer : public CTimer
{
public:
    static CHtiTimer* NewL( MHtiTimerObserver& aObserver );
    virtual ~CHtiTimer();

protected: // From CActive
    void RunL();
    TInt RunError(TInt aError);

private:
    CHtiTimer( MHtiTimerObserver& aObserver );
    void ConstructL();

private:
    MHtiTimerObserver& iObserver;
};


class CHtiConnectionManager : public CActive,
                              public MHtiSocketObserver,
                              public MHtiTimerObserver
{
public:
    enum TConnectionstate {
        EStartingIAP,
        EWaitingConnection,
        EConnecting,
        EConnected,
        EDisconnecting,
        EDisconnected
    };

    static CHtiConnectionManager* NewL( CHtiIPCommServer* aServer );
    ~CHtiConnectionManager();

    void Receive( const RMessage2& aMessage );
    void Send( const RMessage2& aMessage );
    void CancelReceive();
    void CancelSend();

private:
    CHtiConnectionManager( CHtiIPCommServer* aServer );
    void ConstructL();

    // Config file reading separated to 2 parts because
    // ReadConnectionConfigL might need dns resolver and
    // that should be done after the IAP is started...
    void ReadIAPConfigL();
    void ReadConnectionConfigL();

    void ReadSocket();
    void WriteSocket();

    void StartConnectingL();
    void StartListeningL();

    /*
     *  Cancels all outstanding server session requests
     */
    void CancelAllRequests();

    void ShowErrorNotifierL( const TDesC& aText, TInt aErr = KErrGeneral );

protected: // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

public: // MHtiTimerObserver
    void TimerExpiredL();

public: // MHtiSocketObserver
    void ReportComplete( MHtiSocketObserver::TRequestType aType, TInt aError );



private:
    CHtiIPCommServer*   iServer;

    CHtiCfg*            iCfg;
    TUint32             iIAPId;

    TInt                iListenPort;
    TInetAddr           iRemoteHost;

    RSocketServ         iSocketServ;
    RConnection         iConnection;
    RSocket             iDataSocket;
    RSocket             iListenSocket;
    TSockXfrLength      iRecvLen;
    TConnectionstate    iState;

    RMessage2           iReceiveRequest;
    RMessage2           iSendRequest;

    TBool               iReceiveRequestComplete;
    TBool               iSendRequestComplete;

    CHtiSocketMonitor*  iSendMonitor;
    CHtiSocketMonitor*  iReceiveMonitor;

    CHtiTimer*          iConnectTimer;
    TInt                iConnectTimeout;

    TBuf8<KIPCommServerReceiveBufferMaxSize> iReceiveBuffer;
    TBuf8<KIPCommServerSendBufferMaxSize>    iSendBuffer;
    
    TCommDbConnPref     iConnPref;
};

#endif
