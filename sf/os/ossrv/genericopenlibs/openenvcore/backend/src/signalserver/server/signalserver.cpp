// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : signalserver.cpp
// Part of     : Signal Server
//



#include <e32base.h>
#include "signalserver.h"
#include "signalsession.h"
#include "signalcommon.h"
#include "tsignalmessage.h"
#include <signal.h>



static void RunTheServerL()
	{
	CSignalServer* lServer = 0;

	User::LeaveIfError(RThread::RenameMe(KServerName));

	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	lServer = static_cast<CSignalServer*>(CSignalServer::NewL());
	RProcess::Rendezvous(KErrNone);
	CActiveScheduler::Start();
	if(lServer)
		{
		delete lServer;
		lServer = 0;
		}
	CleanupStack::PopAndDestroy();
	}

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

CSignalServer::CSignalServer(CActive::TPriority aPriority) : CServer2(aPriority, ESharableSessions)
	{
	iSessionCount = 0;
	}

CSignalServer::~CSignalServer()
	{
	if(iShutDown)
		{
		delete iShutDown;
		iShutDown = 0;
		}
	}

CServer2* CSignalServer::NewL()
	{
	CSignalServer* self = new (ELeave)CSignalServer(EPriorityStandard);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CSignalServer::ConstructL()
	{
	StartL(KServerName);
	iShutDown = CSigShutDown::NewL();
	RPipe::Init();
	iShutDown->Start();
	}

TInt CSignalServer::CheckForCapability(const RMessage2& aMessage)
	{
	RThread lThread;
	RProcess lProcess;
	TInt lRetVal = KErrGeneral;

	TInt lPid = aMessage.Int0();
	TInt lSigNo = aMessage.Int1();

	if(lSigNo != SIGKILL && lSigNo != SIGSTOP)
		return KErrNone;

	lRetVal = aMessage.Client(lThread);
	if(lRetVal!= KErrNone)
		return lRetVal;
	lRetVal = lThread.Process(lProcess);
	if(lRetVal!= KErrNone)
		return lRetVal;

	if(!lProcess.HasCapability(ECapabilityPowerMgmt) && lPid!=lProcess.Id())
		lRetVal = KErrGeneral;
	else
		lRetVal = KErrNone;

	lThread.Close();
	lProcess.Close();
	return lRetVal;
	}

CSession2* CSignalServer::NewSessionL(const TVersion& aVersion,const RMessage2& /*aMessage2*/) const
	{
	TVersion v(KSignalServMajorVersionNumber, KSignalServMinorVersionNumber, KSignalServBuildVersionNumber);

	if (!User::QueryVersionSupported(v, aVersion))
    	{
    	User::Leave(KErrNotSupported);
    	}

	return CSignalSession::NewL(); // TBD
	}

void CSignalServer::AddSession()
	{
	iSessionCount++;
	iShutDown->Cancel();
	}

void CSignalServer::RemoveSession()
	{
	iSessionCount--;

	TInt lIdx = 0;
	for(lIdx = 0; lIdx < iProcessHdlList.Count();)
		{
		TProcessId lPid(iProcessHdlList[lIdx].mPid);
		RProcess lProcHdl;
		if(lProcHdl.Open(lPid) != KErrNone)
			{
            iProcessHdlList[lIdx].mPipe.Close();
			iProcessHdlList.Remove(lIdx);
			}
		else
			{
			lProcHdl.Close();
			lIdx++;
			}
		}

	if( iSessionCount == 0 )
		{
		iShutDown->Start();
		}
	}

void CSignalServer::ServiceL(const RMessage2& aMessage)
	{
	TInt lPid = 0;
	TOeProcessHdl lTmpHdl;
	TInt lIndex = 0;
	TUint8 lSignal = 0;
	RThread lClientThread;
	RProcess lProcess;

	switch (aMessage.Function ())
		{
		//if this query is related to message Q, call
		//ServiceL of message Q handler
		case ESendSignal:
			if(CheckForCapability(aMessage) != KErrNone)
				aMessage.Complete(KErrPermissionDenied);
			else
				{
				// serialize data
				lPid = aMessage.Int0();
				lSignal = aMessage.Int1();

				lTmpHdl.mPid = lPid;
				lIndex = iProcessHdlList.Find(lTmpHdl,TIdentityRelation<TOeProcessHdl>(TOeProcessHdl::MatchByPid));

				if(lIndex == KErrNotFound)
					aMessage.Complete(KErrGeneral);
				else
					{
					TSignalMessage lMsg;
					lMsg.mType = TSignalMessage::ESignal;
					lMsg.mSignal = lSignal;
					TBuf8<KSigMsgLength> lMsgBuf;
					lMsg.Marshall(lMsgBuf);

					iProcessHdlList[lIndex].mPipe.WriteBlocking(lMsgBuf,KSigMsgLength);
					aMessage.Complete (KErrNone);
					}
				}
			break;
		case ESendSignalValue:
			if(CheckForCapability(aMessage) != KErrNone)
				aMessage.Complete(KErrPermissionDenied);
			else
				{
				// serialize data
				lPid = aMessage.Int0();
				lSignal = aMessage.Int1();
				TUint32 lValue = (TUint32)aMessage.Int2();
				lTmpHdl.mPid = lPid;

				lIndex = iProcessHdlList.Find(lTmpHdl,TIdentityRelation<TOeProcessHdl>(TOeProcessHdl::MatchByPid));
				if(lIndex == KErrNotFound)
					aMessage.Complete(KErrGeneral);
				else
					{
					TSignalMessage lMsg;
					lMsg.mType = TSignalMessage::ESignalValuePair;
					lMsg.mSigVal.mSignal = lSignal;
					lMsg.mSigVal.mValue = lValue;
					TBuf8<KSigMsgLength> lMsgBuf;
					lMsg.Marshall(lMsgBuf);

					iProcessHdlList[lIndex].mPipe.WriteBlocking(lMsgBuf,KSigMsgLength);
					aMessage.Complete (KErrNone);
					}
				}
			break;
		case ESendPipe:
			aMessage.Client(lClientThread);
			lClientThread.Process(lProcess);
			lPid = lProcess.Id();

			lTmpHdl.mPid = lPid;
			lIndex = iProcessHdlList.Find(lTmpHdl,TIdentityRelation<TOeProcessHdl>(TOeProcessHdl::MatchByPid));
			if(lIndex == KErrNotFound)
				{
				TOeProcessHdl lHdl;
				lHdl.mPid = lPid;
				lHdl.mPipe.Open(aMessage,0);
				iProcessHdlList.Append(lHdl);
				aMessage.Complete(KErrNone);
				}
			else
				{
				if(iProcessHdlList[lIndex].mPipe.Open(aMessage,0)!=KErrNone)
					aMessage.Complete(KErrGeneral);
				else
					aMessage.Complete(KErrNone);
				}

			lProcess.Close();
			lClientThread.Close();
			break;
		default:
			aMessage.Complete(KErrNotSupported);
			break;
		}
		return;
	}
