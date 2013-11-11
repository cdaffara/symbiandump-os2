// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SCH_SVR.CPP
// 
//

// User includes
#include "SSCH_STD.H"
#include "SchLogger.h"
#include "SCHMAN.H"
#include "CSCHCODE.H"
#include "SCHEXE.H"
#include "SchSSAMan.h"

extern const BSUL::TClientMessageServerData KServerData;

CSchServer* CSchServer::NewLC()
	{
	CSchServer* self=new(ELeave) CSchServer(EPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSchServer::CSchServer(TInt aPriority)
	: CServer2(aPriority)
	{
	}

CSchServer::~CSchServer()
	{
	delete iTaskScheduler;
	delete iSSAMgr;

#ifdef __SCHLOGGING__
	delete iTheLog;
#endif
	}

void CSchServer::ConstructL()
	{
#ifdef __SCHLOGGING__
	iTheLog = CSheduleServerLog::NewL(_L("SchSvr"));
	Dll::SetTls(iTheLog);
#endif

 	// Create server storage path
 	RFs fs;
 	User::LeaveIfError(fs.Connect());

	TInt err = fs.CreatePrivatePath(RFs::GetSystemDrive());

	if(err != KErrNone && err != KErrAlreadyExists)
		User::Leave(err);
	
	fs.Close();

	iTaskScheduler = CTaskScheduler::NewL();

#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	iSSAMgr = new(ELeave) CSchStartupStateMgr(KDmHierarchyIdStartup, KSM2OSServicesDomain3);
#else
	iSSAMgr = new(ELeave) CSchStartupStateMgr(KDmHierarchyIdStartup, KBaseServicesDomain3);
#endif //SYMBIAN_SYSTEM_STATE_MANAGEMENT

	iSSAMgr->RegisterObserverL(iTaskScheduler);
	iSSAMgr->InitialiseL();
	
	//Initialise message framework
	BSUL::CClientMessage::InitialiseFrameworkL(KServerData);

	StartL(KSchSvrName);
	}

CSession2* CSchServer::NewSessionL(const TVersion &aVersion, const RMessage2& /*aMessage*/) const
//
// Create a new client for this server.
//
	{
	TVersion v(KESchSvrMajorVersionNumber,KESchSvrMinorVersionNumber,KESchSvrBuildVersionNumber);
	TBool r=User::QueryVersionSupported(v,aVersion);
	if (r==EFalse)
		User::Leave(KErrNotSupported);
	return new(ELeave) CSchSession(*iTaskScheduler);
	}

// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
static void RunSchedulerL()
	{
	// naming the server thread after the server helps to debug panics
	User::LeaveIfError(User::RenameThread(KSchSvrName));
	User::LeaveIfError(User::SetProcessCritical(User::ESystemCritical));
	RProcess().SetPriority(EPriorityHigh);

	// create and install the active scheduler we need
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	//
	// create the server (leave it on the cleanup stack)
	CSchServer::NewLC();
	//
	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);
	//
	// Ready to run
	CActiveScheduler::Start();
	//
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}

EXPORT_C TInt RunScheduler()
//
// Run the scheduler
//
	{
	__UHEAP_MARK;
	//
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunSchedulerL());
		delete cleanup;
		}
	//
	__UHEAP_MARKEND;
	return r;
	}


