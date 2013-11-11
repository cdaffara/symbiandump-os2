/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Main log server engine.
* Process log requests from multiple clients simultaneously.
*
*/



/**
 @file server.cpp
*/
#include "server.h"
#include <unistd.h>
#include <fcntl.h>

CLogServer* CLogServer::NewL()
/**
 * @return - Instance of the log server
 */
	{
	CLogServer * server = new (ELeave) CLogServer();
	CleanupStack::PushL(server);
	server->ConstructL();
	// CServer base class call
	server->StartL(KTestExecutePIPSLogServerName);
	CleanupStack::Pop(server);
	return server;
	}

void CLogServer::ConstructL()
/**
 * Second phase construction
 */
	{
	}


CLogServer::CLogServer() : CServer2(EPriorityStandard,ESharableSessions)
/**
 * Constructor
 */
	{
	}

CLogServer::~CLogServer()
/**
 * Destructor
 */
	{
	// Close the array of control structures
	LogControl().Close();
	//Fs().Close();
	}


CSession2* CLogServer::NewSessionL(const TVersion& /*aVersion*/,const RMessage2& /*aMessage*/) const
/**
 * @param RMessage - RMessage for the session open
 */
	{
	// Just create the session
	CLogSession* session = new (ELeave) CLogSession();
	return session;
	}

void CLogServer::ControlComplete(CLogSession& aControl)
/**
 * @param aControl - Logfile control class reference
 *
 * Checks to see if this control session can be removed 
 */
	{
	if(aControl.SessionCount())
		return;

	// There are no subsessions mapped to the logfile control class and
	// no data buffers on its write queue
	// Loop through the server's control array and remove it then delete it
	TInt i;
	for(i=0;i<LogControl().Count();i++)
		{
		if(&aControl == LogControl()[i])
			{
			// Done
			LogControl().Remove(i);
			delete &aControl;
			break;
			}
		}
	// If it's the last one then exit the server
	if(!LogControl().Count())
		CActiveScheduler::Stop();
	}


///////

CLogSession::CLogSession()
:iFileDes(-1)
/**
 * Constructor
 */
	{
	}

CLogSession::~CLogSession()
/**
 * Destructor
 */
	{
	// Check for null
	// Session close without a createlog call leaves iControl null
	// A logfile control structure can have multiple server sessions
	// decrement its server session count
	RemoveSession();
	CLogServer* p=(CLogServer*) Server();
	// Shuts Down the server if this is the last open session
	p->ControlComplete(*this);
	}

void CLogSession::ServiceL(const RMessage2& aMessage)
/**
 * @param aMessage - Function and data for the session
 */
	{
	switch(aMessage.Function())
		{
//		case RTestExecutePIPSLogServ::ECreateLog :
//			{}
		// One of the API write calls
		case RTestExecutePIPSLogServ::EWriteLog :
			{
			// Data can be any size
			// Get the length from second argument
			TInt bufferLength = aMessage.Int1();
			// Get a heap buffer of the right size
			HBufC8* buffer = HBufC8::NewLC(bufferLength);
			TPtr8 ptr(buffer->Des());
			// read the data
			aMessage.ReadL(0,ptr);			
			// Get a buffer control class contructed with the heap data
			// takes ownership
			//CLogBuffer* logBuffer = new (ELeave) CLogBuffer(*buffer);
			CleanupStack::Pop(buffer);
			//get the name of the pipe we want to dump the logs to
			TInt pipeNameLen = aMessage.Int3();
			// Get a heap buffer of the right size
			HBufC8* pipebuffer = HBufC8::NewLC(pipeNameLen);
			TPtr8 pipePtr(pipebuffer->Des());
			// read the data
			aMessage.ReadL(2,pipePtr);			
			// Get a buffer control class contructed with the heap data
			// takes ownership
			CleanupStack::Pop(pipebuffer);
			
			if((iFileDes == -1)  || (!iPipeName ) || (iPipeName->CompareF(pipePtr))  )
				{
				iPipeName = new TPtr8(pipePtr) ; 
				//iPipeName.Set(pipePtr) ;  
				
				char arr[KMaxPath];
				int var;
				for (var = 0; var < pipePtr.Length(); ++var) {
					arr[var]=pipePtr[var];
				}
				arr[var]='\0';
				
				iFileDes = open( arr, O_WRONLY);
				}
			
			const char *printsomething = (const char *)ptr.Ptr() ;
			// incases when the pipe name given was empty / incorrect
			// or unavailable for some reason, the open is expected
			// to return -1, and then we do nothing
			if(iFileDes != -1 ) 
				{
				write(iFileDes , printsomething, bufferLength) ; 
				aMessage.Complete(KErrNone);
				}
			else
				{
				aMessage.Complete(KErrNotFound);
				}
			// apparently closing the pipe is an issue with named pipes
			// so let be for now
			//close(aFileDes); 
			}
			break;

		default:
			break;
		}
	}

