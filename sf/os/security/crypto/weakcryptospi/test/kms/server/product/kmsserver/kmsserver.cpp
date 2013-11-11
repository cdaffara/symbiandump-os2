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
* Key Management Server server class code.
*
*/


/**
 @file
*/


#include "kmslddcommon.h"

#include "kmsserver.h"


CKeyMgmtServer* CKeyMgmtServer::NewLC()
/**
	Factory function allocates new, initialized instance of CKeyMgmtServer.

	@return					New instance of CKeyMgmtServer which is owned
							by the caller.  On return, the server has been
							started.
 */
	{
	CKeyMgmtServer* self = new(ELeave) CKeyMgmtServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CKeyMgmtServer::CKeyMgmtServer()
/**
	Initialize this server as an AO with standard priority.
 */
:	CServer2(CActive::EPriorityStandard)
	{
	// empty.
	}

void CKeyMgmtServer::ConstructL()
/**
	Secondary initialization loads the KMS LDD and
	starts the server.
 */
	{
	TInt r = User::LoadLogicalDevice(KKmsLddFileName);
	if (r != KErrAlreadyExists)
		User::LeaveIfError(r);
	iLoadedLdd = ETrue;

	iShutdown = CKmsShutdown::NewL(KShutdownPeriodUs);

	StartL(KeyMgmtSvrImpl::KKeyMgmtSrvName);
	}

CKeyMgmtServer::~CKeyMgmtServer()
/**
	Unload the KMS LDD if it was successfully loaded.
 */
	{
	delete iShutdown;
	
	if (iLoadedLdd)
		User::FreeLogicalDevice(RKmsChannel::FactoryName());
	}

CSession2* CKeyMgmtServer::NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const
/**
	Implement CServer2 by allocating a new instance of CKeyMgmtSession.

	@param	aVersion		Version of server which client requires.
	@param	aMessage		Connect message.  Not used.
	@return					New instance of CKeyMgmtSession.
 */
	{
	(void) aMessage;

	const TVersion v = KeyMgmtSvrImpl::Version();
	TBool versionOk =  User::QueryVersionSupported(/* aCurrent */ v, /* aRequested */ aVersion);
	
	if (! versionOk)
		User::Leave(KErrNotSupported);
	
	CKeyMgmtSession* s = CKeyMgmtSession::NewL();
	const_cast<CKeyMgmtServer*>(this)->IncrementSessionCount();
	return s;
	}

void CKeyMgmtServer::IncrementSessionCount()
/**
	Record the fact that a session has been opened.  Cancels
	the shutdown timer.
 */
	{
	++iSessionCount;
	iShutdown->Cancel();
	}

void CKeyMgmtServer::DecrementSessionCount()
/**
	Record the fact that a session has been closed.  If no
	sessions are open, then starts the shutdown timer.
 */
	{
	--iSessionCount;
	if (iSessionCount == 0)
		iShutdown->Restart();
	}

