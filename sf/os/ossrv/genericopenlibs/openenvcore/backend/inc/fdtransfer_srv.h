// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// File Descriptor Transfer server - class definitions.
// 
//

#ifndef __FDTRANSFER_SRV_H__
#define __FDTRANSFER_SRV_H__

#include <e32base.h>
#include "fdtransfermsg.h"

enum TPanic
    {
    EPanicBadDescriptor,
    EPanicIllegalFunction,
    EPanicAlreadyReceiving
    };

NONSHARABLE_CLASS(CShutdown) : public CTimer
    {
public:
    CShutdown() : CTimer(EPriorityStandard)
    	{
    	// nada
    	}
    void Start();
    void ConstructL();
private:
    static const TUint KFDTransferServerShutdownDelay=3000000;
private:
    void RunL();
    };

NONSHARABLE_CLASS(CFileDesTransferServer) : public CServer2
    {
public:
	// We want the transfers to be done on a priority. Hence the EPriorityHigh
    CFileDesTransferServer() : CServer2(EPriorityHigh)
    	{
    	// nada
    	}
    	
    ~CFileDesTransferServer();
    static CFileDesTransferServer* NewLC(RSemaphore& sem);
    
    TInt AddToScheduler(const TDesC& aName);
    void RemoveFromScheduler();
	void StartServer();
    void AddSession();
    void DropSession();
    
    CActiveScheduler* iOldScheduler;
    
private:
    void ConstructL();
    CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
private:
    TInt iSessionCount;
    CShutdown* iShutdown;
    RSemaphore iSem;
    };

NONSHARABLE_CLASS(CFileDesTransferSession) : public CSession2
    {
public:
    void CreateL();
private:
    ~CFileDesTransferSession();
    CFileDesTransferServer& Server();
    void ServiceL(const RMessage2& aMessage);
    void GetfilesFromClient(const RMessage2& aMsg) const;
    void GetPipesFromClient(const RMessage2& aMsg) const;
    void ServiceError(const RMessage2& aMessage, TInt aError);
	};
    
#endif // __FDTRANSFER_SRV_H__
