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
// Name        : stdioserver.cpp
// Part of     : stdio server
// To implement the server
// 
//


#include "StdioServer.h" //CStdioServer
#include "StdioSession.h" //CStdioSession
#include <e32debug.h>     //RDebug 

// Initializing the static variable which is the RFs and RCommServ
RFs CStdioServer::iFs; 
RCommServ CStdioServer::iCs;

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
 
	CStdioServer::NewL();
 
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
// Function Name : CStdioServer::NewL()
// Description   : Static class function to create the server. This function will
//	 			   be called Static function RunServerL(). This function leaves 
//				   the object on the stack and returns the handle to created 
//				   CstdioServer(). The server is created as an active object with 
//				   EPriorityStandard.  
//-------------------------------------------------------------------------------	

CServer2* CStdioServer::NewL()
	{
	CStdioServer *tptr = new (ELeave)	CStdioServer(EPriorityStandard);
	CleanupStack::PushL(tptr);
	tptr->ConstructL();
	CleanupStack::Pop();
	return tptr;
	}

	
RFs& CStdioServer::FsSession()
	{
	return iFs;
	}
	
RCommServ& CStdioServer::CsSession()
	{
	return iCs;
	}	
	
//-------------------------------------------------------------------------------
// Function Name : CStdioServer::CStdioServer()
// Description   : Default Constructor. Sets the proiority to EPriorityStandard of 
//				   the active object by passing it to the base class.Initialize the
//				   Session Count to 0.
//-------------------------------------------------------------------------------	


CStdioServer::CStdioServer(CActive::TPriority aPriority) : CServer2(aPriority, ESharableSessions)
 	{
	iSessionCount = 0;
	}
	
//-------------------------------------------------------------------------------
// Function Name : CStdioServer::ConstructL()
// Description   : This function starts the Server and is called from 
//			       CStdioServer::NewLC It calls the StartL function of Cserver2 
//			       to add the server to the active scheduler. Also constructs a 
//			       new asynchronous timer.then ShutDown::Start is called to request
//			       an event after the SHUTDOWNDELAY which is 2 secs, this is in 
//				   case the client session fails to connect.
//--------------------------------------------------------------------------------


void CStdioServer::ConstructL()
	{
    TInt rf = KErrNone; 
    TInt rc = KErrNone;
    StartL(KServerName);
    rf = iFs.Connect();
    if (rf != KErrNone)
        {
            User::Leave(rf);
        }
    rc = iCs.Connect();
    if (rc != KErrNone)
        {
            User::Leave(rc);
        }
	iShutDown = CShutDown::NewL();
	iShutDown->Start();
	}

	
//-------------------------------------------------------------------------------
// Function Name : 	CStdioServer::AddSession()
// Description   :	This function will be called by the CStdioSession::CreateL 
//					function when there is a request from the client to create a 
//					new session. The class will keep a count of the number of the 
//					session it has with the client.Anytime there is request for a
//					new the session the count is increased and CShutDown :: Cancel 
//					is called to cancels the shutdown timer. 
//--------------------------------------------------------------------------------
				

void CStdioServer::AddSession()
	{
	iSessionCount++;
	iShutDown->Cancel();
	}
	
	
//-------------------------------------------------------------------------------
// Function Name : 	CStdioServer::RemoveSession()
// Description   :	This function will be called from the destructor on the 
//					CStdioSession when there is request from the client to close 
//					the session or the client process dies suddenly. As the 
//					CStdioServer is designed to be transient,if the count of the 
//					Session reaches zero then ShutDown :: Start is called to 
//					request an event after the SHUTDOWNDELAY which is 2 secs.
//--------------------------------------------------------------------------------

void CStdioServer::RemoveSession()
	{
	iSessionCount--;
	if( iSessionCount == 0 )
		{
		iShutDown->Start();			
		}
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CStdioServer::~CStdioServer()
// Description   :	Desctructor. Closes the Session opened with File and Comm 
//					Server
//--------------------------------------------------------------------------------

CStdioServer::~CStdioServer()
	{
	iFs.Close();
	iCs.Close();
	}
	
	
//-------------------------------------------------------------------------------
// Function Name : 	CStdioServer::NewSessionL()
// Description   :	This function is called when ever there is a request from the 
//					client to create a session. This function will create a new 
//					object of CStdioSession type. 
//--------------------------------------------------------------------------------

	
CSession2* CStdioServer::NewSessionL(const TVersion& aVersion,const RMessage2& /*aMessage2*/) const
	{
	TVersion v(KStdioServMajorVersionNumber, KStdioServMinorVersionNumber, KStdioServBuildVersionNumber);
    if (!User::QueryVersionSupported(v, aVersion))
    	{
    	User::Leave(KErrNotSupported);  
    	}
    CSession2 *temp = NULL;
    TRAPD(ret , temp = CStdioSession::NewL());
    User::LeaveIfError(ret);
  	return temp;
	}

