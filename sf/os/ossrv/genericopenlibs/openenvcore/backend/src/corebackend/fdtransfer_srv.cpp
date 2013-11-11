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
// file descriptor transfer server - server implementation
// 
//

#include "fdtransfer_srv.h"
#include "sysif.h"
#include "fdesc.h"

// -------------------------------------------------------------------------------
// CShutdown::ConstructL
// -------------------------------------------------------------------------------
inline void CShutdown::ConstructL()
	{
	CTimer::ConstructL();
	}

// -------------------------------------------------------------------------------
// CShutdown::Start
// Starts the timer for the specified time given
// -------------------------------------------------------------------------------
inline void CShutdown::Start()
	{
	CActiveScheduler::Add(this);
	After(KFDTransferServerShutdownDelay);
	}

// -------------------------------------------------------------------------------
// CShutdown::RunL
// Initiate server exit when timer expires
// -------------------------------------------------------------------------------
inline void CShutdown::RunL()
    {
    CActiveScheduler::Stop();
    }
    


	
// -------------------------------------------------------------------------------
// CFileDesTransferSession::CreateL
// 2nd phase construction for sessions. Invoked by the CServer2 framework
// -------------------------------------------------------------------------------
inline void CFileDesTransferSession::CreateL()
    {
    Server().AddSession();
    }


// -------------------------------------------------------------------------------
// CFileDesTransferSession::ServiceL
// Handle a client request. Leaves are handled by the ServiceError method.
// -------------------------------------------------------------------------------
void CFileDesTransferSession::ServiceL(const RMessage2& aMsg)
    {
    switch (aMsg.Function())
        {
    case ETransferFile:
    	{
    	GetfilesFromClient(aMsg);
    	break;
    	}
    case ETransferPipe:
        {
        GetPipesFromClient(aMsg);
    	break;    	
        }
    default:
    	{
    	#ifdef _DEBUG
    	RDebug::Printf("CFileDesTransferSession: Illegal message received");
    	#endif
        break;	//_DEBUG
    	}
       	
        }
    }

// -------------------------------------------------------------------------------
// CFileDesTransferSession::GetPipesFromClient
// Inherits pipes from the client.  
// -------------------------------------------------------------------------------
void CFileDesTransferSession::GetPipesFromClient(const RMessage2& aMsg) const
{
	CFileTable& ftable = const_cast<CFileTable&>(Backend()->FileTable());
	TInt indx = aMsg.Int0();
	RPipe pipe;
	TInt err = pipe.Open(aMsg, 1);
	
	if (err == KErrNone)
	{
		TUint16 mode = 0;
		if(aMsg.Int2()& O_NONBLOCK)
			mode |= O_NONBLOCK;
	    
		if(indx > (ftable.iFids.Count()-1))
		{
			TRAP(err,ftable.ExpandFTableL(indx+1));
		}
		if(KErrNone == err)
			err = ftable.CreatePipeDesc(indx,pipe,mode);
		if (err != KErrNone)
		{
			pipe.Close();
		}
		
	}
	aMsg.Complete(KErrNone);
}

// -------------------------------------------------------------------------------
// CFileDesTransferSession::GetfilesFromClient
// Inherits normal files the client.  
// -------------------------------------------------------------------------------
void CFileDesTransferSession::GetfilesFromClient(const RMessage2& aMsg) const
{

	CFileTable& ftable = const_cast<CFileTable&>((Backend()->FileTable()));
	TInt indx = aMsg.Int0();
	CFileDesc* fdesc = (CFileDesc*)ftable.iPrivateHeap->AllocZ(sizeof(CFileDesc));
	if (!fdesc)
	{
	     aMsg.Complete(KErrNone);
	     return;				
	}
	fdesc = new (fdesc) CFileDesc;
	
#if defined(SYMBIAN_OE_LARGE_FILE_SUPPORT) && !defined(SYMBIAN_OE_NO_LFS)
	//The integer width is bigger for 64 bit sizes
	TBuf<140> params;
#else		
	TBuf<60> params;
#endif /* SYMBIAN_OE_LARGE_FILE_SUPPORT && !SYMBIAN_OE_NO_LFS */
	
	aMsg.Read(1,params);
	TInt err = (fdesc->FileHandle()).AdoptFromClient(aMsg,2,3);
	if(indx > (ftable.iFids.Count()-1))
	{
		TRAP(err,ftable.ExpandFTableL(indx+1));
	}
	if (err == KErrNone)
	{
		fdesc->SetState(params);
		err = fdesc->CreateLock();
	}
	if(err == KErrNone)
		{
		err = fdesc->Alloc();
		}
	if(err == KErrNone)
	{
	   ftable.Attach(indx, fdesc);
    
	}
	else
	{
		delete fdesc;
		(fdesc->FileHandle()).Close();
	}
	aMsg.Complete(KErrNone);  
	return ;
}

// -------------------------------------------------------------------------------
// CFileDesTransferSession::ServiceError
// Handle an error from CFileTransferSession::ServiceL().
// -------------------------------------------------------------------------------
void CFileDesTransferSession::ServiceError(const RMessage2& aMsg, TInt aError)
    {
    CSession2::ServiceError(aMsg, aError);
    }

// -------------------------------------------------------------------------------
// CFileDesTransferSession::Server
// Obtain a reference to the server
// -------------------------------------------------------------------------------
inline CFileDesTransferServer& CFileDesTransferSession::Server()
    {
	return *(static_cast<CFileDesTransferServer*>(const_cast<CServer2*>(CSession2::Server())));
    }
    
// -------------------------------------------------------------------------------
// CFileDesTransferSession::~CFileDesTransferSession
// This session is destroyed. Gone forever.
// -------------------------------------------------------------------------------
CFileDesTransferSession::~CFileDesTransferSession()
    {
    Server().DropSession();
    }


// -------------------------------------------------------------------------------
// CFileDesTransferServer::NewLC
// Server constructor
// -------------------------------------------------------------------------------
CFileDesTransferServer* CFileDesTransferServer::NewLC(RSemaphore& sem)
	{
	CFileDesTransferServer* pServer = new(ELeave) CFileDesTransferServer;
	CleanupStack::PushL(pServer);
	pServer->iSem = sem;
	pServer->ConstructL();
	// Leave pServer on the cleanup stack
	return pServer;
	}


// -------------------------------------------------------------------------------
// CFileDesTransferServer::ConstructL
// Second phase construction. Create the shutdown timer object and the semaphore
// -------------------------------------------------------------------------------
void CFileDesTransferServer::ConstructL()
    {
    CShutdown* shutdown = new(ELeave) CShutdown;
    CleanupStack::PushL(shutdown);
    shutdown->ConstructL();
	CleanupStack::Pop();
	iShutdown = shutdown;
	shutdown = 0;
    }

// -------------------------------------------------------------------------------
// CFileDesTransferServer::AddToScheduler
// Add both the server and the shutdown timer objects to the active scheduler
// -------------------------------------------------------------------------------
TInt CFileDesTransferServer::AddToScheduler(const TDesC& aName)
	{
	iShutdown->Start();
	return CServer2::Start(aName);
	}

// -------------------------------------------------------------------------------
// CFileDesTransferServer::RemoveFromScheduler
// Remove both the server and the shutdown timer objects from the active scheduler
// -------------------------------------------------------------------------------
void CFileDesTransferServer::RemoveFromScheduler()
	{
	if (iShutdown->IsAdded())
		{
		iShutdown->Deque();
		}
	Deque();
	}

// -------------------------------------------------------------------------------
// CFileDesTransferServer::NewSessionL
// Create a new client session.
// -------------------------------------------------------------------------------
CSession2* CFileDesTransferServer::NewSessionL(const TVersion&, const RMessage2&) const
    {
    return new(ELeave) CFileDesTransferSession;
    }

// -------------------------------------------------------------------------------
// CFileDesTransferServer::AddSession
// A new session is created. Cancel the shutdown timer if it was running.
// -------------------------------------------------------------------------------
inline void CFileDesTransferServer::AddSession()
    {
    iShutdown->Deque();
    ++iSessionCount;
    
    }

// -------------------------------------------------------------------------------
// CFileDesTransferServer::DropSession
// The session is destroyed. Stop the ActiveScheduler
// -------------------------------------------------------------------------------
inline void CFileDesTransferServer::DropSession()
    {
    if (--iSessionCount==0)
    	{
        CActiveScheduler::Stop();
    	}
    }

// -------------------------------------------------------------------------------
// CFileDesTransferServer::StartServer
// Start serving. Blocks until the active scheduler is stopped from CShutdown::RunL
// -------------------------------------------------------------------------------

void CFileDesTransferServer::StartServer()
	{
    iSem.Signal();
    // Start the scheduler
    CActiveScheduler::Start();
    // Block until 
    }

// -------------------------------------------------------------------------------
// CFileDesTransferServer::~CFileDesTransferServer
// -------------------------------------------------------------------------------
CFileDesTransferServer::~CFileDesTransferServer()
	{
	iSem.Close();
	delete iShutdown;
	}
