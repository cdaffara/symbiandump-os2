// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : ipcserver.cpp
// Part of     : ipc server
// To implement the ipc server
// 
//


#include "ipcserver.h" //CIpcServer
#include "ipcsession.h" //CIpcSession
#include <e32debug.h>     //RDebug 
#include "factory.h"			// PosixIpcFactory
#include "messagequeue.h"
#include "semaphorequeue.h"
#include "ipcutils.h"
#include "filesocketaddrmanager.h"

//-------------------------------------------------------------------------------
// Function Name : RunTheServerL()
// Description   : Static Function called from the TRAP. It Creates the scheduler
//				   installs and signals the starting client process once the 
//				   server is up.
//-------------------------------------------------------------------------------	

static void RunTheServerL()
	{
	User::LeaveIfError(RThread::RenameMe(KServerName));
	 
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
 
	CIpcServer::NewL();
 
	RProcess::Rendezvous(KErrNone);
	
	CActiveScheduler::Start();
 
	CleanupStack::PopAndDestroy();
	}
	
//-------------------------------------------------------------------------------
// Function Name : RunServer()
// Description   : Static function called from the E32Main function.This function
//			       starts the TRAP and calls the static function RunTheServerL()
//				   from the TRAP. 
//-------------------------------------------------------------------------------	

static TInt RunServer()
	{
	TRAPD(ret, RunTheServerL());
	return ret;
	}
	

//-------------------------------------------------------------------------------
// Function Name : E32Main()
// Description   : Entry point for the server process.
//				   
//-------------------------------------------------------------------------------	
TInt E32Main()
	{
	TInt ret = KErrNoMemory;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup)
		{
		ret = RunServer();
		delete cleanup;
		}
	return ret;
	}
	
//-------------------------------------------------------------------------------
// Function Name : CIpcServer::NewL()
// Description   : Static class function to create the server. This function will
//	 			   be called Static function RunServerL(). This function leaves 
//				   the object on the stack and returns the handle to created 
//				   CIpcServer(). The server is created as an active object with 
//				   EPriorityStandard.  
//-------------------------------------------------------------------------------	

CServer2* CIpcServer::NewL()
	{
	CIpcServer *tptr = new (ELeave)	CIpcServer(EPriorityStandard);
	CleanupStack::PushL(tptr);
	tptr->ConstructL();
	CleanupStack::Pop();
	return tptr;
	}

	

	
//-------------------------------------------------------------------------------
// Function Name : CIpcServer::CIpcServer()
// Description   : Default Constructor. Sets the proiority to EPriorityStandard of 
//				   the active object by passing it to the base class.Initialize the
//				   Session Count to 0.
//-------------------------------------------------------------------------------	


CIpcServer::CIpcServer(CActive::TPriority aPriority) : CServer2(aPriority, ESharableSessions)
 	{
	iSessionCount = 0;
	}
	
//-------------------------------------------------------------------------------
// Function Name : CIpcServer::ConstructL()
// Description   : This function starts the Server and is called from 
//			       CIpcServer::NewLC It calls the StartL function of Cserver2 
//			       to add the server to the active scheduler. Also constructs a 
//			       new asynchronous timer.then ShutDown::Start is called to request
//			       an event after the SHUTDOWNDELAY which is 2 secs, this is in 
//				   case the client session fails to connect.
//--------------------------------------------------------------------------------


void CIpcServer::ConstructL()
	{
	StartL(KServerName);
	iShutDown = CShutDown::NewL();
	iMsgQHandler = PosixIpcFactory::CreateMsgQueueHandlerL();
	iSharedMemHandler = PosixIpcFactory::CreateSharedMemoryHandlerL();
	iSemQHandler = PosixIpcFactory::CreateSemQueueHandlerL();
	iFileSockAddrManager = PosixIpcFactory::CreateLocSocketAddrManagerL();
	iShutDown->Start();
	}

	
//-------------------------------------------------------------------------------
// Function Name : 	CIpcServer::AddSession()
// Description   :	This function will be called by the CStdioSession::CreateL 
//					function when there is a request from the client to create a 
//					new session. The class will keep a count of the number of the 
//					session it has with the client.Anytime there is request for a
//					new the session the count is increased and CShutDown :: Cancel 
//					is called to cancels the shutdown timer. 
//--------------------------------------------------------------------------------
				

void CIpcServer::AddSession()
	{
	iSessionCount++;
	iShutDown->Cancel();
	}
	
	
//-------------------------------------------------------------------------------
// Function Name : 	CIpcServer::RemoveSession()
// Description   :	This function will be called from the destructor on the 
//					CStdioSession when there is request from the client to close 
//					the session or the client process dies suddenly. As the 
//					CIpcServer is designed to be transient,if the count of the 
//					Session reaches zero then ShutDown :: Start is called to 
//					request an event after the SHUTDOWNDELAY which is 2 secs.
//--------------------------------------------------------------------------------

void CIpcServer::RemoveSession()
	{
	iSessionCount--;
	if( iSessionCount == 0 )
		{
		iShutDown->Start();			
		}
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CIpcServer::~CIpcServer()
// Description   :	Desctructor. Closes the Session opened with File and Comm 
//					Server
//--------------------------------------------------------------------------------

CIpcServer::~CIpcServer()
	{
	if ( iMsgQHandler )
		{
		delete iMsgQHandler;
		}
	if ( iSharedMemHandler )
		{
		delete iSharedMemHandler;
		}
	if ( iSemQHandler )
		{
		delete iSemQHandler;
		}
	if( iFileSockAddrManager)
		{
		delete iFileSockAddrManager;
	}
	}
	
	
//-------------------------------------------------------------------------------
// Function Name : 	CIpcServer::NewSessionL()
// Description   :	This function is called when ever there is a request from the 
//					client to create a session. This function will create a new 
//					object of CStdioSession type. 
//--------------------------------------------------------------------------------

	
CSession2* CIpcServer::NewSessionL(const TVersion& aVersion,const RMessage2& /*aMessage2*/) const
	{
	TVersion v(KStdioServMajorVersionNumber, KStdioServMinorVersionNumber, KStdioServBuildVersionNumber);
  
	if (!User::QueryVersionSupported(v, aVersion))
    	{
    	User::Leave(KErrNotSupported);  
    	}
    
	return CIpcSession::NewL();
	}


//-------------------------------------------------------------------------------
// Function Name : 	CIpcServer::ServiceL()
// Description   : 	For any request from the client process, the this method
//					checks the request and issues the corresponding request.
//					This we are handling at server because ipcs objects are system 
//					system wide.
//-------------------------------------------------------------------------------	
void CIpcServer::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function ())
		{
		//if this query is related to message Q, call
		//ServiceL of message Q handler
		case EMsgGet:
		case EMsgCtl:
		case EMsgRcv:
		case EMsgSnd:
			iMsgQHandler->ServiceL(aMessage);
			break;
		case EShmGet:
		case EShmAt:
		case EShmDt:
		case EShmCtl:
#ifdef SYMBIAN_OE_LIBRT
		case ESetShmSize:
		case EGetShmSize:
#endif //SYMBIAN_OE_LIBRT
			iSharedMemHandler->ServiceL(aMessage);
			break;

		case ESemGet:
		case ESemCtl:
		case ESemOp:
			iSemQHandler->ServiceL(aMessage);
			break;
		case EFileSocketGetPathByPort:
		case EFileSocketGetPortByPath:	
		case EFileSocketAddPath:
		case EFileSocketRemovePath:
			iFileSockAddrManager->ServiceL(aMessage);
			break;
		default:
			aMessage.Complete (KErrNotSupported);
			break;
		}
		return;
	}

void CIpcServer::RemoveFileSockPathBySession(CSession2 *aSession)
	{
	((CLocSocketAddrManager*)iFileSockAddrManager)->RemovePathBySession(aSession);
	}
