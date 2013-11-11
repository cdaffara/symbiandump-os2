/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Implements CSraProcessor processing receiving encrypted keys.
*
*/


#include "sraserver.h"
#include "sraprocessor.h"
#include "stkm.h"
#include <s32mem.h>
#include <caf/streaming/keyassociation.h>

using namespace StreamAccess;

CSraProcessor::CSraProcessor(CSraSession& aSession)
/**
	Initializes and adds the processor object to the active scheduler.
 */
	:CActive(CActive::EPriorityHigh),
	 iSession(aSession),
	 iState(EIdle)
		{
		CActiveScheduler::Add(this);
		}

		
CSraProcessor::~CSraProcessor()
/**
	Destructor.
 */
	{
	iClientSocket.Close();	
	iConnection.Close();	
	iSocketServ.Close();
	iBuffer.Close();
	Deque();
	}

CSraProcessor* CSraProcessor::NewL(CSraSession& aSession, const TDesC& aListenAddr, TUint aListenPort)
/**
	Factory function allocates a new, initialized instance of CSraProcessor.
 */
	{
	CSraProcessor *self = new(ELeave) CSraProcessor(aSession);
	CleanupStack::PushL(self);
	self->ConstructL(aListenAddr, aListenPort);
	CleanupStack::Pop(self);
	return self;
	}

void CSraProcessor::ConstructL(const TDesC& aListenAddr, TUint aListenPort)
/**
	Second phase constructor.
 */
	{
	// Create a session to esock server
	User::LeaveIfError(iSocketServ.Connect());	
	// Create a connection
	User::LeaveIfError(iConnection.Open(iSocketServ, KAfInet));		
	User::LeaveIfError(iConnection.Start());
	
	User::LeaveIfError(iClientSocket.Open(iSocketServ, KAfInet, KSockDatagram, KProtocolInetUdp, iConnection));
	
	// Create and bind the client socket
	TInetAddr listenAddr;
	User::LeaveIfError(listenAddr.Input(aListenAddr));
	listenAddr.SetPort(aListenPort);
	User::LeaveIfError(iClientSocket.Bind(listenAddr));
	
	iBuffer.CreateL(KMaxName);
	}

void CSraProcessor::StartProcessing()
	{
	iState = EListening;
	SelfComplete();
	}

void CSraProcessor::SelfComplete()
/**
	Generates an event on itself by completing on iStatus.
 */
	{
	TRequestStatus *status = &iStatus;
	User::RequestComplete(status,KErrNone);
	SetActive();
	}
	
void CSraProcessor::RunL()
//	Implements from CActive.
	{
	
	switch(iState)
		{			
		case EListening:
			 Listen();
			 break;
		
		case EProcessing:
			ProcessL();
			 break;
			 
		case EFinish:
			Finish();
			break;
			
		default://Unexpected state. Should never come here.
			User::Leave(KErrGeneral);
		}
	}

void CSraProcessor::DoCancel()
//	Implements from CActive.
	{
	switch(iState)
		{	
		case EProcessing:
			iClientSocket.CancelRecv();
			 break;
		default:
			break;
		}
	}

void CSraProcessor::Listen()
	{
	iState = EProcessing;
	TInt flags = 0;
	iClientSocket.Recv(iBuffer, flags, iStatus);
	SetActive();
	}

void CSraProcessor::ProcessL()
	{
	// Read a short term key message carrying a traffic encryption key
	RDesReadStream stream(iBuffer);
	stream.PushL();
	
	// Create an STKM object from the received STKM
	CStkm* stkm = CStkm::NewLC(stream);
	
	// Create an IPSec key association object by using the received STKM.
	// In the reference agent, it is assumed that all STKMs are protected by using IPSec protocol.
	// In a real-life agent, other protocols (e.g. SRTP and ISMACryp) should also be considered.
	CIpSecKeyAssociation* key = CIpSecKeyAssociation::NewLC(stkm->Spi(), stkm->EncryptionKey(), stkm->AuthenticationKey());
	
	// Process the new key association
	iSession.iKeyStreamSink->ProcessNewKeyAssociationL(*key);
	
	// Release the allocated memories
	CleanupStack::PopAndDestroy(3, &stream);
	
	// In the reference agent, we expect just one STKM to receive.
	// Therefore, the next state will be the final one.
	iState = EFinish;
	SelfComplete();
	}

void CSraProcessor::Finish()
	{
	iSession.iMutex.Signal();
	}
