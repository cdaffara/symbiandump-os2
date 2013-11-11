// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// DBMS server implementation
// 
//

#include "SD_STD.H"
#include "Sd_PlatDep.h"

using namespace DBSC;

//CDbmsActiveScheduler class just exposes the access to 
//CActiveScheduler::Level() method, which is needed to 
//make a decision when to call CDbmsActiveScheduler::Stop().
class CDbmsActiveScheduler : public CActiveScheduler
	{
public:
	inline TInt Level() const
		{
		return CActiveScheduler::Level();
		}
	};

///////////////////////
// Class CDbsServer
inline CDbsServer::CDbsServer() :
	CServer2(0, ESharableSessions), 
	iSources(iCache),
	iDbPropsFactory(iFs),
	iDriveSpaceCol(iFs)
	{
	DbgPrint1(_L("###CDbsServer::CDbsServer(), Server ProcID=%d\n"), RDbProcess().Id());
	}

//"iCache.Hold(this,KDbsExitDelay)" statement will put CDbsServer instance in the cache, 
//which ensures that CDbsServer instance will be automatically destroyed if nobody
//uses it next KDbsExitDelay microseconds.
CDbsServer* CDbsServer::NewL()
	{
	DbgPrint1(_L("###CDbsServer::NewL(), Server ProcId=%d\n"), RDbProcess().Id());
	CDbsServer* self= new (ELeave) CDbsServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	self->iCache.Hold(self, KDbsExitDelay);		// the last thing we do here
	//Intentional behaviour, resources freed after timeout only
	//coverity[use_after_free]
	return self;
	}

//
// 2nd phase construction - ensure the timer and server objects are running
//
void CDbsServer::ConstructL()
	{
	DbgPrint1(_L("###CDbsServer::ConstructL(), Server ProcId=%d\n"), RDbProcess().Id());
	__LEAVE_IF_ERROR(Dll::SetTls(this));
	StartL(KDbsServerName);
	__LEAVE_IF_ERROR(iFs.Connect());
	iDbPropsFactory.OpenL();
	//EDriveZ - Symbian OS ROM drive !?!?
	iDbPropsFactory.GetPrivatePathL(EDriveZ, iFileName);
	iPolicyProxy = CPolicyProxy::NewL(iFs,iFileName);
	User::LeaveIfError(iCache.Open(ECacheSize, ETrue));	// create a timed cache
	}

//
// Invoked via cache entry expiry (or cleanup)
//
CDbsServer::~CDbsServer()
	{
	DbgPrint1(_L("###CDbsServer::~CDbsServer(), Server ProcId=%d\n"), RDbProcess().Id());
	iDriveSpaceCol.Close();
	delete iPolicyProxy;
	iDbPropsFactory.Close();
	iCache.Close();
	iSources.Close();
	iFs.Close();
	//Stop the scheduler if the nesting level > 0.
	//Sometime it happens that more than one thread tries to run the server and you will have
	//CDbmsActiveScheduler::Install() called multiple times. But you have to stop the 
	//scheduler only once!
	CDbmsActiveScheduler* scheduler = static_cast <CDbmsActiveScheduler*> (CActiveScheduler::Current());
	if(scheduler->Level() > 0)
		{
		DbgPrint2(_L("###CDbsServer::~CDbsServer(), stop the scheduler, Server ProcId=%d, scheduler=%x\n"), RDbProcess().Id(), (TInt)scheduler);
		CDbmsActiveScheduler::Stop();
		}
	}

//
// Creates a new client session. This should really check the version number.
//
CSession2* CDbsServer::NewSessionL(const TVersion& aVersion,const RMessage2&) const
	{
	if(!User::QueryVersionSupported(RDbs::Version(), aVersion))
		{
		__LEAVE(KErrNotSupported);
		}
	CSession2* session = new (ELeave) CDbsSession;
	iCache.Release(*this);
	return session;
	}

//
// Returns the instance of the server
//
CDbsServer* CDbsServer::Instance()
	{
	return (CDbsServer*)Dll::Tls();
	}

//
// initiates exit if the last client is closing
//
void CDbsServer::RemoveSession()
	{
	iSessionIter.SetToFirst();
	iSessionIter++;
	if(iSessionIter++ == 0)
		{
		iCache.Hold(this, KDbsExitDelay);
		}
	}

//
// Performs all server initialisation, in particular creation of the
// scheduler and server.
// Noting has to be leaved in the cleanup stack!
// Both: DBMS server and the active scheduler will be destroyed
// later. The active scheduler - in CDbsServer destructor.
// CDbsServer instance - by iCache instance (RDbCache).
static void CreateServerL()
	{
	DbgPrint1(_L("###DBMS-CreateServerL(), Server ProcId=%d\n"), RDbProcess().Id());
	// naming the server thread after the server helps to debug panics
	User::LeaveIfError(User::RenameThread(KDbsServerName));
	// ensure the server thread has a handle on EDBMS.DLL
	LoadDbmsLibraryL();

	// create and install the active scheduler we need
	CDbmsActiveScheduler* scheduler = new (ELeave) CDbmsActiveScheduler;
	CleanupStack::PushL(scheduler);
	CDbmsActiveScheduler::Install(scheduler);

	// create the server
	(void)CDbsServer::NewL();

	CleanupStack::Pop(scheduler);

	// Initialisation complete, now signal the client
	RDbProcess::Rendezvous(KErrNone);
	}

//
// Performs all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
//
static void RunServerL()
	{
	DbgPrint1(_L("###DBMS-RunServerL(), Server ProcId=%d\n"), RDbProcess().Id());
	::CreateServerL();
	//After successfull creation CreateServerL() call leaves the active scheduler instance
	//and the DBMS server instance in the heap - they will be destroyed later.
	__UHEAP_MARK;
	//Start the scheduler. The execution control is transferred to the curent
	//active scheduler. The statement after "CDbmsActiveScheduler::Start();" will
	//be reached after the stop of active scheduler.
	CDbmsActiveScheduler::Start();
	__UHEAP_MARKEND;
	}

//
// Server process entry-point
//
EXPORT_C TInt Dbs::Run()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt err = KErrNoMemory;
	if(cleanup)
		{
		TRAP(err, ::RunServerL());
		delete cleanup;
		}
	DbgPrint2(_L("###Dbs::Run(), end, Server ProcId=%d, err=%d\n"), RDbProcess().Id(), err);
	return err;
	}
	
///////////////////////
// Client called code

//
// Starts the DBMS server
//
TInt Dbs::Start()
	{
	DbgPrint1(_L("#-#Dbs::Start(), Client ProcId=%d\n"), RDbProcess().Id());
	RDbProcess server;
	TInt err = ::CreateDbmsProcess(server);
	if(err != KErrNone)
		{
		DbgPrint2(_L("#-#Dbs::Start(), CreateDbmsProcess, Client ProcId=%d, err=%d\n"), RDbProcess().Id(), err);
		return err;
		}

	TRequestStatus stat;
	server.Rendezvous(stat);
	if(stat != KRequestPending)
		{
		server.Kill(0);		// abort startup
		}
	else
		{
		server.Resume();	// logon OK - start the server
		}
	User::WaitForRequest(stat);		// wait for start or death

	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	err = (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	DbgPrint2(_L("#-#Dbs::Start(), end, Client ProcId=%d, err=%d\n"), RDbProcess().Id(), err);
	return err;
	}

