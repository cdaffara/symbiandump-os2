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
*
*/

#include <e32def.h>
#include <e32debug.h> 
#include <obexserver.h>
#include <obex.h>

#include "../../public/clientServerShared.h"
#include "classContServer.h"
#include "classContServerSession.h"
#include "obexInitiator.h"



/**
 * Creates a new CSession2 derived object
 */
CClassContServerSession* CClassContServerSession::NewL(CClassContServer& aServer)
	{
	CClassContServerSession* s = new(ELeave) CClassContServerSession(aServer);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

	
/**
 * Desctructor, delete the CObexInitiator object and set to NULL
 * to avaiod dereferencing freed memory
 */	
CClassContServerSession::~CClassContServerSession()
	{

	delete iObexInitiator;
	iObexInitiator = NULL;
	// tell server im gonna die
	iServer.SessionRemoved();
	}

	
/**
 * Constructor, CServer2 object is passed
 */
CClassContServerSession::CClassContServerSession(CClassContServer& aServer) :
	iServer(aServer)
	{	
	}


/**
 * 2nd phase constructor, new CObexIntiiator object is created under a Trap harness
 * this allows for RMessage2 to complete with the appropriate error code before leaving.
 */	
void CClassContServerSession::ConstructL()
	{
	iObexInitiator = CObexInitiator::NewL(); // Create new CObexServer object
	}


/**
 * This is the function that deals with an incoming request from 
 * the client and then routes to the necessary function call.
 */
void CClassContServerSession::ServiceL(const RMessage2& aMessage)
	{
	TRAPD(err, ForwardMessageL(aMessage));
	if (!iBadMessage)
		aMessage.Complete(err);
	}

/**
 * This is the function that maps the message from the client to
 * the appropriate call onto the OBEX server.
 */
void CClassContServerSession::ForwardMessageL(const RMessage2& aMessage)
	{
	switch(aMessage.Function())
		{
		case EStartClassContServer:
			StartServiceL();
			break;
		case EStopClassContServer:
			StopService();
			break;
		default:
			iBadMessage = ETrue;
			aMessage.Panic(KClientPanicCategory, EClientPanicBadMessage);
			break;
		}
	}

/**
 * This is the function that is called by the server when the client has
 * requested to start the Obex Server, CObexInitiator::StartObexServerL 
 * is responsible for calling CObexServer::Start, implementing an Obex Server.		
 */	
void CClassContServerSession::StartServiceL()
	{
	iObexInitiator->StartObexServerL(); // Call Start on the Obex Server
	}


/**
 * This is the function called by the server when the client has requested
 * to stop the Obex server, CObexInitiator::StopObexServer is esponsible
 * for calling CObexServer::Stop.
 *
 */
void CClassContServerSession::StopService()
	{
	iObexInitiator->StopObexServer(); //Stop the Obex server
	}




    
  
  
