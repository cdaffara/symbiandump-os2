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

#include <e32cons.h>
#include <obex.h>
#include <obexserver.h>

#include "obexInitiator.h"

/**
 * Creates a new object of type CObexInitiator
 */
CObexInitiator* CObexInitiator::NewL()
	{
	CObexInitiator* obexInit = new(ELeave) CObexInitiator();
	CleanupStack::PushL(obexInit);
	obexInit->ConstructL();
	CleanupStack::Pop(obexInit);
	return obexInit;
	}
/**
 * Desctructor
 */	
CObexInitiator::~CObexInitiator()
	{
	delete iObexServer;
	iObexServer = NULL;
	__DEBUG_ONLY(delete iConsole);
	}

/**
 * Constructor
 */	
CObexInitiator::CObexInitiator() 
	{
	}
	
/**
 * 2nd phase constructor, this creates a new CObexServer
 * with protocol information for USB Obex.
 */
void CObexInitiator::ConstructL()
	{
	TObexUsbProtocolInfo info;
	info.iTransport = KObexUsbProtocol;
	info.iInterfaceStringDescriptor = KObexDescription;
	iObexServer = CObexServer::NewL(info);
	__DEBUG_ONLY(iConsole = Console::NewL(KConsoleName, TSize(KConsFullScreen,KConsFullScreen));
	iConsole->Printf(_L("A New Obex Server has been Created!\n")););
	}	


/**
 * Start the Obex Server by calling CObexServer::Start
 * Current instance that implements MObexServerNotify 
 * is passed to the CObexServer.
 */
void CObexInitiator::StartObexServerL()
	{
	TInt err = iObexServer->Start(this);
	User::LeaveIfError(err);
	__DEBUG_ONLY(iConsole->Printf(_L("Obex Server Started!\n\n")););
	}


/**
 * Stop the Obex Server and delete the CObexServer member.
 * Set to NULL to avoid de referencing freed memory
 */	
void CObexInitiator::StopObexServer()
	{
	iObexServer->Stop();
	}
	
	
//Start of the MServerNotify virtual functions

void CObexInitiator::ErrorIndication (TInt /*aError*/)
    {
    __DEBUG_ONLY(iConsole->Printf(_L("An OBEX Protocol error has occured!\n")););
    }

void CObexInitiator::TransportUpIndication ()
    {
    __DEBUG_ONLY(iConsole->Printf(_L("Transport Up\n")););
    }

void CObexInitiator::TransportDownIndication ()
    {
    __DEBUG_ONLY(iConsole->Printf(_L("Transport Down\n")););
    }

TInt CObexInitiator::ObexConnectIndication(const TObexConnectInfo& /*aRemoteInfo*/, const TDesC8& /*aInfo*/)
    {
    __DEBUG_ONLY(iConsole->Printf(_L("OBEX connection has been made with client\n")););
	return KErrNone;
    }

void CObexInitiator::ObexDisconnectIndication (const TDesC8& /*aInfo*/)
    {
    __DEBUG_ONLY(iConsole->Printf(_L("OBEX has been disconnected with client\n")););
    }

CObexBufObject* CObexInitiator::PutRequestIndication ()
    {
    __DEBUG_ONLY(iConsole->Printf(_L("A Put is being made\n")););
	return NULL;
	}

TInt CObexInitiator::PutPacketIndication ()
    {
	return KErrNone;
    }

TInt CObexInitiator::PutCompleteIndication ()
    {
    __DEBUG_ONLY(iConsole->Printf(_L("Put has completed\n")););
	return KErrNone;
    }


CObexBufObject* CObexInitiator::GetRequestIndication (CObexBaseObject* /*aRequiredObject*/)
    {
    __DEBUG_ONLY(iConsole->Printf(_L("Client has made a Get request\n")););
	return NULL;
	}

TInt CObexInitiator::GetPacketIndication ()
    {
    return KErrNone;
    }

TInt CObexInitiator::GetCompleteIndication ()
    {
    __DEBUG_ONLY(iConsole->Printf(_L("Get has completed\n")););
  	return KErrNone;
    }

TInt CObexInitiator::SetPathIndication (const CObex::TSetPathInfo& /*aPathInfo*/, const TDesC8& /*aInfo*/)
    {
	return KErrNone;
    }

void CObexInitiator::AbortIndication ()
    {
    __DEBUG_ONLY(iConsole->Printf(_L("Operation aborted\n")););
    }
    
