// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implements CStaServer which provides streaming test agent services.
// 
//

#include "staserver.h"

using namespace StreamAccess;

static const TUint staRangeCount = 2;

static const TInt staRanges[staRangeCount] =
	{
	0,							//Range 0 - 0 to EBaseSession-1. Not used.
	CScsServer::EBaseSession,	//Range 1 - EBaseSession to KMaxTInt inclusive.
	};

static const TUint8 staElementsIndex[staRangeCount] =
	{
	CPolicyServer::ENotSupported, // Range 0 is not supported.
	0,							  // Range 1 must come from a client who has DRM capability.
	};							

static const CPolicyServer::TPolicyElement staElements[] =
	{
	{_INIT_SECURITY_POLICY_C1(ECapabilityDRM), CPolicyServer::EFailClient},	
	};

static const CPolicyServer::TPolicy staPolicy =
	{
	CPolicyServer::EAlwaysPass, // Allow all connects
	staRangeCount,
	staRanges,
	staElementsIndex,
	staElements,
	};

//
//CStaServer
//

CStaServer::CStaServer()
/**
	Intializes the Streaming Test Agent object with its version and policy.
 */
	:	CScsServer(StaVersion(),staPolicy)
		{
		//empty
		}

CStaServer::~CStaServer()
/**
	Destructor. Cleanup the Streaming Test Agent object.
 */
	{
	iFs.Close();
	SdpCodecStringPool::Close();
	}
	
	
CStaServer* CStaServer::NewLC()
/**
	Factory function allocates new, initialized instance of CStaServer.

	@return		New, initialized instance of CStaServer which is left on the cleanup stack.
 */
	{
	CStaServer *sta = new (ELeave) CStaServer();
	CleanupStack::PushL(sta);
	sta->ConstructL();
	return sta;
	}


void CStaServer::ConstructL()
/**
	Second phase constructor starts the Streaming Test Agent Server.
 */
	{
	CScsServer::ConstructL(KStaShutdownPeriod);
	
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareProtected());
	User::LeaveIfError(iFs.CreatePrivatePath(iFs.GetSystemDrive()));
	SdpCodecStringPool::OpenL();
	StartL(KStaName);
	}


CScsSession* CStaServer::DoNewSessionL(const RMessage2& aMessage)
/**
	Implement CScsServer by allocating a new instance of CStaSession.

	@param	aMessage	Standard server-side handle to message.
	@return				New instance of CStaSession which is owned by the caller.
 */
	{
	(void)aMessage;
	return CStaSession::NewL(*this);
	}


