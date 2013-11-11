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
* Description:
*
*/

#ifndef MEMSPYENGINESERVER_H
#define MEMSPYENGINESERVER_H

// System includes
#include <e32base.h>

// User includes
#include <memspyengineclientinterface.h>
#include <memspy/engine/memspydevicewideoperations.h>

// Classes referenced
class CMemSpyEngine;
class CMemSpyDwOperationTracker;

NONSHARABLE_CLASS( CShutdown ) : public CTimer
    {
    enum {KMyShutdownDelay=10 * 1000000};       // 10s
public:
    inline CShutdown();
    inline void ConstructL();
    inline void Start();
private:
    void RunL();
    };

NONSHARABLE_CLASS( CMemSpyEngineServer ) : public CServer2
    {
public:
    static CMemSpyEngineServer* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineServer();
    
    CMemSpyDwOperationTracker* CurrentOperationTracker() const { return iCurrentOperationTracker; }
    void SetCurrentOperationTracker(CMemSpyDwOperationTracker* aTracker) { iCurrentOperationTracker = aTracker; }
    
    CMemSpyEngine& Engine() { return iEngine; } 
    
    void AddSession(TBool aCliRequest);
    void DropSession(TBool aCliRequest);

private:
    CMemSpyEngineServer( CMemSpyEngine& aEngine );
	void ConstructL();

protected: // From CServer2
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

private:
    CMemSpyEngine& iEngine;
    CMemSpyDwOperationTracker* iCurrentOperationTracker;
    
    TInt iSessionCount;
    TBool iCliConnected;
    
    CShutdown iShutdown;
    };



NONSHARABLE_CLASS( CMemSpyEngineSession ) : public CSession2
	{
public:
	static CMemSpyEngineSession* NewL( CMemSpyEngine& aEngine, const RMessage2& aMessage );
	~CMemSpyEngineSession();
	
	void CreateL();
	
private:
	CMemSpyEngineSession( CMemSpyEngine& aEngine );
	void ConstructL( const RMessage2& aMessage );

private: // From CSession2
	void ServiceL( const RMessage2& aMessage );

private: // Internal methods
    void DoServiceL( const RMessage2& aMessage );
    void DoAsyncServiceL( const RMessage2& aMessage );
    void DoUiServiceL( const RMessage2& aMessage );
    void DoCmdServiceL( const RMessage2& aMessage );
    static TInt ValidateFunction( TInt aFunction, TBool aIncludesThreadId, TBool aIncludesThreadName );
    void HandleThreadSpecificOpL( TInt aFunction, const TThreadId& aThreadId );
    void HandleThreadSpecificOpL( TInt aFunction, const TDesC& aThreadName );
    void HandleThreadAgnosticOpL( TInt aFunction, const RMessage2& aMessage );
    void StartDeviceWideOperationL(CMemSpyDeviceWideOperations::TOperation aOperation, const RMessage2& aMessage);
    
    inline CMemSpyEngineServer& Server() const { return *static_cast<CMemSpyEngineServer*>(const_cast<CServer2*>(CSession2::Server())); }

private:
    CMemSpyEngine& iEngine;
    HBufC* iClientThreadName;
    TUint32 iClientThreadId;
    TBool iIsCliRequest;
    };

/**
 * CMemSpyDwOperationTracker
 * Tracks device wide operation progress and calls iOperationMessage.Complete upon completion. 
 */
NONSHARABLE_CLASS( CMemSpyDwOperationTracker ) : public MMemSpyDeviceWideOperationsObserver
	{
public:
	static CMemSpyDwOperationTracker* NewL(CMemSpyDeviceWideOperations::TOperation aOperation, 
			const RMessage2& aOperationMessage,
			CMemSpyEngineServer& aServer);
	~CMemSpyDwOperationTracker();
	
	void AddNotificationL(const RMessage2& aMessage);
	
	void Cancel();

public: // From MMemSpyDeviceWideOperationsObserver
	void HandleDeviceWideOperationEvent(TEvent aEvent, TInt aParam1, const TDesC& aParam2);
	
private:
	CMemSpyDwOperationTracker(const RMessage2& aOperationMessage, CMemSpyEngineServer& aServer);
	void ConstructL(CMemSpyDeviceWideOperations::TOperation aOperation);
	
private:
	RMessage2 iOperationMessage;
	CMemSpyEngineServer& iServer;
	CArrayFixFlat<RMessage2>* iPendingNotifications;
	CMemSpyDeviceWideOperations* iOperation;
	TInt iProgress;
	};


#endif
