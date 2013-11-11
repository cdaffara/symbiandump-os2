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
* Implementation of the streaming reference agent server.
*
*/


#include "sraserver.h"


static const TUint sraRangeCount = 1;

static const TInt sraRanges[sraRangeCount] =
	{
	0	//All requests
	};

static const TUint8 sraElementsIndex[sraRangeCount] =
	{
	0	// Request must come from a client who has DRM capability.
	};							

static const CPolicyServer::TPolicyElement sraElements[] =
	{
	{_INIT_SECURITY_POLICY_C1(ECapabilityDRM), CPolicyServer::EFailClient},	
	};

static const CPolicyServer::TPolicy sraPolicy =
	{
	CPolicyServer::EAlwaysPass, // Allow all connects
	sraRangeCount,
	sraRanges,
	sraElementsIndex,
	sraElements,
	};

CSraServer::CSraServer()
//constructor
	: CPolicyServer(CActive::EPriorityStandard, sraPolicy, ESharableSessions),
	iVersion(SraVersion())
	{
	//empty
	}

CSraServer::~CSraServer()
//destructor
	{
	__ASSERT_DEBUG(iSessionCount == 0, PanicServer(EOutstandingSessions));
	
	delete iShutdownTimer;
	iFs.Close();
	SdpCodecStringPool::Close();
	}


CSraServer* CSraServer::NewLC()
	{
	CSraServer* self = new(ELeave)CSraServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


void CSraServer::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareProtected());
	User::LeaveIfError(iFs.CreatePrivatePath(iFs.GetSystemDrive()));
	SdpCodecStringPool::OpenL();
	StartL(KSraName);
	iShutdownTimer = CShutdownTimer::NewL(KSraShutdownPeriod);
	iShutdownTimer->Restart();
	}


CSession2* CSraServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
	{
	TBool supported = User::QueryVersionSupported(iVersion, aVersion); //(aCurrent, aRequested)
	if(!supported)
		{
		User::Leave(KErrNotSupported);
		}
	return CSraSession::NewL();
	}


void CSraServer::AddSession()
	{
	++iSessionCount;
	iShutdownTimer->Cancel();
	}


void CSraServer::RemoveSession()
	{
	if (--iSessionCount == 0)
		{
		iShutdownTimer->Restart();
		}
	}

