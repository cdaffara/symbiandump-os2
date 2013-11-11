// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Client interface to the CPosixServer
// 
//

#include "POSIXIF.H"
#include "LTIME.H"
#include "LPOSIX.H"
#include <fcntl.h>
#include <sys/errno.h>
#include <sys/serial.h>
#include <sys/wait.h>


#ifdef _DEBUG
#define DebugPrint	RDebug::Print
#else
inline void DebugPrint(const TDesC&, ...) {}
#endif

// RPosixSession
//
// The message protocol is to pass the errno pointer in p[0] and a pointer to a PosixParams
// in p[1]. The result is written back into the PosixParams.retval field.

int RPosixSession::Request(TInt aFunction, int& anErrno, PosixParams& aParams) const
	{
	return SendReceive(aFunction,TIpcArgs(&anErrno,&aParams));
	}

void RPosixSession::Request(TInt aFunction, int& anErrno, PosixParams& aParams, TRequestStatus& aStatus) const
	{
	SendReceive(aFunction,TIpcArgs(&anErrno,&aParams),aStatus);	// asynchronous request
	}

void RPosixSession::Request(TInt aFunction, const TIpcArgs& aArg,TRequestStatus &aStatus) const
	{
	RSessionBase::SendReceive(aFunction,aArg,aStatus);
	}
TInt RPosixSession::Request(TInt aFunction, const TIpcArgs& aArg) const
	{
	return RSessionBase::SendReceive(aFunction,aArg);
	}

TVersion RPosixSession::Version()
	{
	return TVersion(KCPosixMajorVersionNumber,KCPosixMinorVersionNumber,0);
	}

TInt RPosixSession::Connect (TDesC& aServerName)
	{
	TVersion version=Version();
	return CreateSession(aServerName,version,1+3);	// 3 extra message slots for pipes
	}

TInt RPosixSession::Connect ()
	{
	TBuf<80> serverName;
	PosixServerName(serverName);
	return CreateSession(serverName,Version(),1);
	}

// CPosixServer support functions exported from ESTLIB.DLL

#if defined(__WINS__)

// simple scheme to provide pretend processes on WINS

extern "C" void getcwd(char*,int);
typedef void (*FUNC)();

static int id=1;
EXPORT_C void NewProcessId () 
	{ 
	id += 10; 
	}

static FUNC procFn;
EXPORT_C void NextProcessFn (TAny* aFn)
	{
	procFn=(FUNC)aFn;
	}

TInt threadhelper (TAny* aFn)
	{
	CTrapCleanup::New();
	FUNC f=(FUNC)aFn;
	(*f)();
	return 0;
	}
TInt processhelper (TAny*)
	{
	// Do the MCRT0.OBJ things straight away
	SpawnPosixServerThread();
	char wd[80];
	getcwd(wd, sizeof(wd));		// connect to CPosixServer
	return threadhelper(procFn);
	}
#endif // __WINS__
_LIT(SERVER_FORMAT,"Posix-%d");
_LIT(SERVER_MATCH, "Posix-*");

EXPORT_C void PosixServerName(TDes& aBuffer)
//
// Construct the name of the CPosixServer for this process
//
	{
	TProcessId id=RProcess().Id();
	aBuffer.Format(SERVER_FORMAT,*REINTERPRET_CAST(int*,&id));
	}

struct rendezvous 
	{
	RThread iCaller;
	TRequestStatus* iStatus;
	};

EXPORT_C TInt SpawnPosixServerThread ()
//
// Try to start a PosixServer thread, assuming there isn't one already
//
	{
	RPosixSession probe;
	TInt err=probe.Connect();
	probe.Close();
	if (err==KErrNone)
		return KErrNone;	// server already exists
	TBuf<80> serverName;
	PosixServerName(serverName);
	TRequestStatus status(KRequestPending);
	struct rendezvous rv;
	rv.iCaller.Duplicate(RThread(),EOwnerProcess);
	rv.iStatus=&status;
	RThread server;
	err=server.Create(serverName,CPosixServer::ThreadFunction,0x2000,NULL,&rv);
	if (err==KErrNone) 
		{
		server.Resume();
		User::WaitForRequest(status);
		err=status.Int();
		server.Close();
		}
	rv.iCaller.Close();
	return err;
	}

EXPORT_C TInt InstallPosixServerActiveObject (TInt aPriority)
	{
	TRAPD(err, CPosixServer::InitL(aPriority));
	return err;
	}

void CPosixServer::InitL(TInt aPriority)
//
// Construct and install a CPosixServer active object
//
	{
	CPosixServer *pS=new(ELeave) CPosixServer(aPriority);
	CleanupStack::PushL(pS);
	TBuf<80> serverName;
	PosixServerName(serverName);

	User::LeaveIfError(pS->iFs.Connect());

	pS->iFids.InitL();

	// search for parent process
	pS->FindParentL();

	// set up default fids
	pS->DefaultConsoleL();

	DebugPrint(_L("Starting CPosixServer\n"));
	pS->StartL(serverName);
	CleanupStack::Pop(pS);
// Leave pS on the clean up stack for the calling routine to clean up normally on 
// Active Scheduler shutdown or in a failure case where  the scheduler does not 
// start due to an error.

	}

// CPosixServer

CPosixServer::CPosixServer(TInt aPriority)
	: CServer2(aPriority)
	{
	__DECLARE_NAME(_S("CPosixServer"));
	}

TInt CPosixServer::ThreadFunction(TAny* aPtr)
//
// Create and run an active scheduler containing a CPosixServer
//
	{
	CTrapCleanup* TheTrapCleanup=CTrapCleanup::New();

	RLibrary stdlib;
	stdlib.Load(_L("estlib"));	// workaround for RAM-loaded EXE calling RAM-loaded DLL

	struct rendezvous* rvp = (struct rendezvous *)aPtr;
	TInt ret=KErrNone;
	// start scheduler and server
	CActiveScheduler *pA=new CActiveScheduler;
	if (pA!=NULL) 
		{
		CActiveScheduler::Install(pA);
		ret=InstallPosixServerActiveObject();
		}
	// signal to the caller that we've started (or failed!)
	rvp->iCaller.RequestComplete(rvp->iStatus,ret);
	if (ret==KErrNone)
		{
		// start fielding requests from clients
		CActiveScheduler::Start();
		}
	// finished
	delete TheTrapCleanup;
	return(KErrNone);
	}

CSession2* CPosixServer::NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const
	{
	TBool r=User::QueryVersionSupported(RPosixSession::Version(),aVersion);
	if (!r)
		User::Leave(KErrNotSupported);

	RProcess clientProcess;
	RThread clientThread;
	User::LeaveIfError(aMessage.Client(clientThread));
	CleanupClosePushL(clientThread);
	User::LeaveIfError(clientThread.Process(clientProcess));
	TProcessId clientId = clientProcess.Id();
	clientProcess.Close();
	CleanupStack::PopAndDestroy(1); //close thread

	if (clientId!=RProcess().Id())
		{
		// A thread in a different process
		if (iChildren==0)
			User::Leave(KErrNotReady);	// quick rejection

		// We need an explicit CONST_CAST so that the CPosixRequest objects have a
		// mutable reference to the CPosixServer which holds the shared data structures
		return new(ELeave) CPosixIPCSession(CONST_CAST(CPosixServer&,*this));
		}
	// A thread in the same process
	return new(ELeave) CPosixSession(CONST_CAST(CPosixServer&,*this));
	}

void CPosixServer::ServerPanic (TPosixServerPanic aPanic)
	{
	_LIT(KPosixServerPanic, "Posix server");
	User::Panic(KPosixServerPanic,aPanic);
	}

void CPosixServer::FindParentL()
	{
	TFullName parent;
	TPosixIPCPid pid;
	TPosixIPCReply reply;

	TProcessId id=RProcess().Id();
	pid = *REINTERPRET_CAST(TUint*,&id);	// my process id

	TFindServer posixservers(SERVER_MATCH);
	while (posixservers.Next(parent)==KErrNone)
		{
		DebugPrint(_L("Are you my mother, %S?"), &parent);
		if (iParent.Connect(parent)!=KErrNone)
			continue;
		if (iParent.Request(PMAreYouMyMother,TIpcArgs(&pid, &reply))!=KErrNone)
			{
			iParent.Close();
			continue;
			}

		// found parent
		DebugPrint(_L("Found parent process %S"), &parent);

		// Create any pipes that might be required
		TUint mask=reply().iPipeMask;
		CPipeChildDesc* pipes[3];
		TInt i=0;
		for (i=0; i<3; i++, mask>>=1)
			{
			pipes[i]=0;
			if (mask&1)
				{
				CPipeChildDesc* pipe=new(ELeave) CPipeChildDesc(i,iParent);
				pipes[i]=pipe;
				pipe->PushLC();
				}
			}

		// organise the necessary descriptors
		TPtr env=HBufC::NewLC(reply().iEnvironmentSize)->Des();
		TPtr cwd=HBufC::NewLC(reply().iWorkingDirectorySize)->Des();

		// get the data from parent
		TInt err=iParent.Request(PMHelloMum, TIpcArgs(&pid, &env, &cwd));
		
		DebugPrint(_L("Environment string: %S"), &env);
		DebugPrint(_L("Working directory: %S"), &cwd);
		
		if(err!=KErrNone)
			{
			DebugPrint(_L("I've become an orphan"));
			// release stuff
			iParent.Close();
			User::Leave(err);
			break;
			}
		// apply to our process
		iEnv.ConstructL(reply().iVarCount,env);
		err=iFs.SetSessionPath(cwd);
		User::LeaveIfError(err);

		// free up the temporary descriptors
		CleanupStack::PopAndDestroy(2);

		// Attach the pipes!
		for (i=0; i<3; i++)
			{
			iFids.Attach(i, pipes[i]);
			if (pipes[i]!=0)
				CleanupStack::Pop();
			}
		return;
		}
	DebugPrint(_L("Posix-%d is a top-level process"), pid());
	User::LeaveIfError(PosixFilesystem::SetDefaultDir(iFs));
	}

int CPosixServer::POpen3(PosixParams* aParams, int& anErrno)
	{
	TInt err=KErrNoMemory;
	//coverity[alloc_fn]
	//coverity[assign]
	CPosixProcess* proc= new CPosixProcess(*this);
	if (proc!=0)
		{
		//coverity[leave_without_push]
		err=iFids.Reserve(aParams->pint);
		if (err==KErrNone)
			{
			TRAP(err,proc->POpen3L(aParams));
			}
		if (err==KErrNone)
			{
			DebugPrint(_L("POpen3 created process %d"), proc->iPid);
			proc->iNextProcess=iChildren;
			iChildren=proc;
			return (int)proc->iPid;	// success
			}
		delete proc;
		iFids.Detach(aParams->pint);
		}
	return MapError(err, anErrno);
	}

CPosixRequest* CPosixServer::Waiters() 
	{
	CPosixRequest* waiters=iWaitAnyQueue;
	iWaitAnyQueue=0;
	return waiters;
	}

// CPosixSession
//
// Each local thread gets one of these

CPosixSession::CPosixSession(CPosixServer& aServer)
	: iActive(aServer)
	{
	CActiveScheduler::Add(&iActive);
	__DECLARE_NAME(_S("CPosixSession"));
	}

void CPosixSession::ServiceL(const RMessage2& aMessage)
	{
	iActive.Service(aMessage);
	}

// CPosixRequest
//
// An active object contained within the Session that handles the deferred completion
// of asynchronous functions (e.g. read & write).

CPosixRequest::CPosixRequest(CPosixServer& aServer)
	: CActive(EPriorityStandard), iServer(aServer), iPtr(0,0)
	{
//	iFile=0;
//	iNewF=0;
//	iNewFid=0;
	}

void CPosixRequest::Service(const RMessage2& aMessage)
//
// The message protocol is to pass the errno pointer in p[0] and a pointer to a PosixParams
// in p[1]. The result is written back into the PosixParams.retval field.
//
	{
	if (aMessage.Function() == PMcancel)
		{
		Cancel();						// Cancel in the active scheduler
		if (iFile)
			EndAsynch(KErrCancel);		// Complete the cancelled request & clean up
		aMessage.Complete(KErrNone);
		return;
		}

	if (iFile!=0)
		{
		aMessage.Complete(KErrInUse);
		return;
		}
	int& anErrno=*REINTERPRET_CAST(int*,CONST_CAST(TAny*,aMessage.Ptr0()));
	PosixParams* params=REINTERPRET_CAST(PosixParams*,CONST_CAST(TAny*,aMessage.Ptr1()));
	switch (aMessage.Function())
		{

	// Asynchronous functions need queuing, active objects etc.

	case PMread:
	case PMwrite:
	case PMsendto:
	case PMrecvfrom:
		iPtr.Set((TText8*)params->ptr[0], params->len[0], params->len[0]);
		// and fall through...
	case PMfsync:
	case PMconnect:
	case PMshutdown:
		{
		TInt err=Fids().Asynch(params->fid,iFile);
		if (!err)
			{
			QueueAsynch(aMessage);	// start operation or queue if busy
			return;					// deferred completion through RunL
			}
		params->ret=MapError(err,anErrno);
		}
		break;

	case PMaccept:
		{
		TInt err=Fids().Asynch(params->fid,iFile);
		if (!err)
			{
			iNewF=0;
			iNewFid=Fids().Reserve();	// reserve a fid for the accepted socket
			err=iNewFid;
			if (iNewFid>=0)
				{
				QueueAsynch(aMessage);	// start operation or queue if busy
				return;					// deferred completion through RunL
				}
			}
		params->ret=MapError(err,anErrno);
		}
		break;

	case PMioctl:
	case PMioctlN:
		{
		TInt err=Fids().Asynch(params->fid,iFile);
		if (!err)
			{
			QueueAsynch(aMessage);	// start operation or queue if busy
			return;					// deferred completion through RunL
			}

		aMessage.Complete(err);	// Different calling convention
		return;
		}

	// complicated synchronous functions which might do their own completion

	case PMwaitpid:
		{
		// check for invalid options or if an invalid pid is specified. currently there is no 
		// support for process group id's so a pid less than -1 or equal to 0 is invalid
		if((params->pint[1] & ~(WNOHANG|WUNTRACED))|| (params->pint[0] < -1) || (params->pint[0] ==0))
			{
			anErrno=EINVAL;
			params->ret=-1;
			break;
			}
		if (params->pint[0]==-1 && params->pint[1]==0)	/* wait for any child */
			{
			iMessage=aMessage;
			iServer.WaitForAnyChild(this);
			return; // wait for the next child to die
			}
		CPosixProcess* child=iServer.Child(params->pint[0]);
		if (child!=0)
			{
			if (child->IsAlive())
				{
				if (params->pint[1]&1)	/* WNOHANG */
					{
					params->ret=0;
					break;
					}
				iMessage=aMessage;
				child->Queue(this);
				return;	// wait for the child to die
				}
			params->pint[0]=child->iExitReason;
			params->ret=child->iPid;
			iServer.Release(child);
			}
		else
			{
			anErrno=ECHILD;
			params->ret=-1;
			}
		}
		break;

	// simple synchronous functions

	case PMdup:
		params->ret=Fids().dup(params->fid,anErrno);
		break;
	case PMdup2:
		params->ret=Fids().dup2(params->fid,params->pint[0],anErrno);
		break;
	case PMopen: 
		{
		const wchar_t* name = params->cwptr[0];
		if	((L'C' == name[0]) && (L'O' == name[1]) && (L'M' == name[2]) && (L':' == name[4]) && ((name[3] >= L'1') && (name[3] <= L'9')) ||
			(L'I' == name[0]) && (L'R' == name[1]) && (L'C' == name[2]) && (L'O' == name[3]) && (L'M' == name[4]) && (L':' == name[6]) && ((name[5] >= L'1') && (name[5] <= L'9')))
			params->ret=Fids().open(params->cwptr[0],params->pint[0],params->pint[1],anErrno,Cs());
		else
			params->ret=Fids().open(params->cwptr[0],params->pint[0],params->pint[1],anErrno,Fs());

		}
		break;
	case PMclose: 
		params->ret=Fids().userclose(params->fid,anErrno);
		break;
	case PMlseek: 
		params->ret=Fids().lseek(params->fid,params->pint[0],params->pint[1],anErrno);
		break;
	case PMfstat: 
		params->ret=Fids().fstat(params->fid,(struct stat*)params->ptr[0],anErrno);
		break;
	case PMgetcwd: 
//		params->ret=(int)PosixFilesystem::getcwd(Fs(),params->ptr[0],params->len[0],anErrno);
		params->ret=(int)PosixFilesystem::getcwd(Fs(),params->wptr[0],params->len[0],anErrno);
		break;
	case PMchdir: 
		params->ret=PosixFilesystem::chdir(Fs(),params->cwptr[0],anErrno);
		break;
	case PMmkdir: 
//		params->ret=PosixFilesystem::mkdir(Fs(),params->cptr[0],params->pint[0],anErrno);
		params->ret=PosixFilesystem::mkdir(Fs(),params->cwptr[0],params->pint[0],anErrno);
		break;
	case PMrmdir: 
		params->ret=PosixFilesystem::rmdir(Fs(),params->cwptr[0],anErrno);
		break;
	case PMchmod: 
		params->ret=PosixFilesystem::chmod(Fs(),params->cwptr[0],params->pint[0],anErrno);
		break;
	case PMunlink: 
		params->ret=PosixFilesystem::unlink(Fs(),params->cwptr[0],anErrno);
		break;
	case PMstat: 
		params->ret=PosixFilesystem::stat(Fs(),params->cwptr[0],(struct stat*)params->ptr[0],anErrno);
		break;
	case PMrename: 
		params->ret=PosixFilesystem::rename(Fs(), params->cwptr[0],params->cwptr[1],anErrno);
		break;
	case PMResolvePath:
//		params->ret=PosixFilesystem::ResolvePath(Fs(),
//			*(TParse*)params->ptr[0],params->cptr[0],(TDes*)params->ptr[1]);
		params->ret=PosixFilesystem::ResolvePath(Fs(),
			*(TParse*)params->ptr[0],params->cwptr[0],(TDes*)params->ptr[1]);
		break;
	case PMsocket:
		params->ret=Fids().socket(params->pint[0],params->pint[1],params->pint[2],anErrno,Ss());
		break;
	case PMbind:
		params->ret=Fids().bind(params->fid,params->addr,anErrno);
		break;
	case PMlisten:
		params->ret=Fids().listen(params->fid,params->pint[0],anErrno);
		break;
	case PMsockname:
		params->ret=Fids().sockname(params->fid,params->addr,params->pint[0],anErrno);
		break;
	case PMgetsockopt:
		params->ret=Fids().getsockopt(params->fid,params->pint[0],params->pint[1],
			params->ptr[0],params->lenp[0],anErrno);
		break;
	case PMsetsockopt:
		params->ret=Fids().setsockopt(params->fid,params->pint[0],params->pint[1],
			params->ptr[0],params->len[0],anErrno);
		break;
	case PMgetenv:
		params->ret=(int)Env().getenv(params->cwptr[0]);
		break;
	case PMunsetenv:
		Env().unsetenv(params->cwptr[0]);	// no return value
		break;
	case PMsetenv:
		params->ret=Env().setenv(params->cwptr[0],params->cwptr[1],params->pint[0],anErrno);
		break;
	case PMioctlcomplete:
		params->ret=Fids().ioctlcomplete(params->fid,params->pint[0],params->ptr[0],*(REINTERPRET_CAST(TRequestStatus*, params->ptr[1])), anErrno);
		break;
	case PMTerminateProcess:
		{
		int status = params->fid;
		RProcess().Kill(status);
		}
		break;
	case PMpopen3:
		params->ret=iServer.POpen3(params,anErrno);
		break;
	default:
		aMessage.Complete(KErrNotSupported);
		return;
		}
	// deal with completion of a synchronous request
	aMessage.Complete(KErrNone);
	}

// Asynchronous requests
//
// 1. QueueAsynch() to get into the appropriate queue in the FileDesc
// 2. FileDesc calls StartAsynch() when it's our turn
// 3. StartAsynch() makes the relevant IO call and does SetActive()
// 4a.    RunL() handles the completion of the IO call and calls EndAsynch()
// 4b.    DoCancel() handles cancellation of the IO call, but doesn't call EndAsynch()
// 5. EndAsynch() removes us from the FileDesc queue and completes iMessage
//

void CPosixRequest::QueueAsynch(const RMessage2& aMessage)
//
// Add this to the appropriate queue in the associated file
//
	{
	iMessage=aMessage;	// Suggested by AndrewT to avoid code duplication
	iQueue=CFileDescBase::IOwriteQ;
	switch (aMessage.Function())
		{
	case PMread:
	case PMrecvfrom:
		iQueue=CFileDescBase::IOreadQ;
		break;
	case PMioctl:
		iQueue=CFileDescBase::IOioctlQ;
		break;
	case PMioctlN:
		iQueue=CFileDescBase::IOioctlNQ;
		break;
	default:
		// everything else uses the IOwriteQ, including Accept and Connect
		break;
		}

	__ASSERT_ALWAYS(iFile!=0,CPosixServer::ServerPanic(EPosix_NoPendingIO));
	iFile->AddLast(*this,iQueue);
	}

void CPosixRequest::StartAsynch()
//
// The request has reached the front of the queue and can now be actioned
//
	{
	PosixParams* params=REINTERPRET_CAST(PosixParams*,CONST_CAST(TAny*,iMessage.Ptr1()));
	switch (iMessage.Function())
		{
	case PMread:
		{
		
		//if we need to have a timer for this operation to cancel it later
		if (iFile->TimedRead())
			{
			iFile->ReadIsTimed = ETrue;
			TRAPD(tRes, {iFile->TimedMessage = CSerialTimer::NewL(iFile);});
			if (tRes != KErrNone)
				{
				//we have a problem here
				//basically, fake the async request completing with the returned error
				iStatus = KRequestPending;
				SetActive();
				TRequestStatus * ps = &iStatus;
				User::RequestComplete(ps, tRes);
				return;
				}

			iFile->TimedMessage->IssueRequest();
			}
		else
			iFile->ReadIsTimed = EFalse;

		iFile->ReadWasCancelled = EFalse;

		iFile->Read(iPtr,iStatus);
		
		}
		break;
	case PMrecvfrom:
		iFile->RecvFrom(iPtr,params->addr,params->pint[0],iStatus);
		break;
	case PMwrite:
		iFile->Write(iPtr,iStatus);
		break;
	case PMsendto:
		iFile->SendTo(iPtr,params->addr,params->pint[0],iStatus);
		break;
	case PMfsync:
		iFile->Sync(iStatus);
		break;
	case PMconnect:
		iFile->Connect(params->addr,iStatus);
		break;
	case PMshutdown:
		iFile->Shutdown(params->pint[0],iStatus);
		break;
	case PMaccept:
		iFile->Accept(iNewF,iStatus,Ss());
		break;
	case PMioctl:
		iFile->Ioctl(params->pint[0],params->ptr[0],iStatus);
		break;
	case PMioctlN:
		iFile->Ioctl(params->pint[0],params->ptr[0],iStatus);
		break;
	default:
		EndAsynch(KErrGeneral);
		return;
		}
	SetActive();	// for asynchronous completion via RunL
	return;
	}

void CPosixRequest::RunL()
//
// The pending IO has completed, so handle the result
//
	{
	__ASSERT_ALWAYS(iFile!=0,CPosixServer::ServerPanic(EPosix_NoPendingIO));
	TInt completion=KErrNone;
	int& anErrno=*REINTERPRET_CAST(int*,CONST_CAST(TAny*,iMessage.Ptr0()));
	PosixParams* params=REINTERPRET_CAST(PosixParams*,CONST_CAST(TAny*,iMessage.Ptr1()));
	switch (iMessage.Function())
		{
	case PMread:
		{
		if (iFile->ReadIsTimed)
			{
				//need to stop the timer
				delete iFile->TimedMessage;
				iFile->TimedMessage = NULL;
				iFile->ReadIsTimed = EFalse;
			}

		TInt err=iFile->ReadCompletion(iPtr, iStatus.Int());
		if (err==0)
			{
			params->ret=iPtr.Length();
			break;
			}
		//if the read was cancelled and we are to patch it due to me cancelling it

		if (iFile->ReadWasCancelled)
			{
			err = ETIMEDOUT;
			iFile->ReadWasCancelled = EFalse;
			}

		params->ret=MapError(err,anErrno);
		}
		break;
	case PMwrite:
		{
		TInt err=iFile->WriteCompletion(iPtr, iStatus.Int());
		if (err==0)
			{
			params->ret=iPtr.Length();
			break;
			}
		params->ret=MapError(err,anErrno);
		}
		break;
	case PMconnect:
	case PMshutdown:
	case PMfsync:
		params->ret=MapError(iStatus.Int(),anErrno);
		break;
	case PMsendto:
		{
		TInt err=iFile->SendToCompletion(iPtr, iStatus.Int());
		if (err==0)
			{
			params->ret=iPtr.Length();
			break;
			}
		params->ret=MapError(err,anErrno);
		}
		break;
	case PMrecvfrom:
		{
		TInt err=iFile->RecvFromCompletion(params->ret, iStatus.Int());
		if (err==0)
			{
			params->ret=iPtr.Length();
			break;
			}
		params->ret=MapError(err,anErrno);
		}
		break;
	case PMaccept:
		{
		TInt err=iStatus.Int();
		if (err)
			Fids().Attach(iNewFid,0);	// cancel the reservation
		else
			{
			err=Fids().Attach(iNewFid,iNewF);
			if (!err)
				{
				params->ret=iNewFid;
				break;	// so that we return the new fid
				}
			delete iNewF;
			iNewF=0;
			}
		params->ret=MapError(err,anErrno);
		}
		break;

	case PMioctlN:
		{
		completion=iStatus.Int();	// caller picks up completion explicitly 
		}
		break;

	case PMioctl:
		{
		completion=iStatus.Int();	// caller picks up completion explicitly 
//		TInt err=iFile->IoctlCompletion(params->pint[0], &params->ret, iStatus.Int());
//		params->ret=MapError(err,anErrno);
		}
		break;

	default:
		completion=KErrGeneral;	// arrgh - I imagine that it's going to die if we get here...
		break;
		}
	EndAsynch(completion);
	}

void CPosixRequest::EndAsynch(TInt aResult)
// 
// finish an asynchronous operation and complete iMessage
//
	{
	__ASSERT_ALWAYS(iFile!=0,CPosixServer::ServerPanic(EPosix_NoPendingIO));
	iFile->Remove(*this,iQueue);
	iFile->Close();	// balances the Dup() in CFileTable::Asynch(), may delete object!
	iFile=0;
	iMessage.Complete(aResult);	
	}


void CPosixRequest::DoCancel()
//
// The pending IO has been cancelled, so cancel the outstanding request
// Needs to deal with all of the cases in RunL, but doesn't call EndAsynch().
// This is called from CActive::Cancel() only when the object is active, but
// EndAsynch() needs to be called when the object is active or when it's just
// waiting in a FileDesc queue.
//
	{
	__ASSERT_ALWAYS(iFile!=0,CPosixServer::ServerPanic(EPosix_NoPendingIO));
	switch (iMessage.Function())
		{
	case PMread:
		iFile->ReadCancel();
		break;
	case PMrecvfrom:
		iFile->RecvFromCancel();
		break;
	case PMwrite:
		iFile->WriteCancel();
		break;
	case PMsendto:
		iFile->SendToCancel();
		break;
	case PMfsync:
		iFile->SyncCancel();
		break;
	case PMconnect:
		iFile->ConnectCancel();
		break;
	case PMshutdown:
		iFile->ShutdownCancel();
		break;
	case PMaccept:
		iFile->AcceptCancel();
		Fids().Attach(iNewFid,0);	// cancel the reservation
		break;
	case PMioctl:
		iFile->IoctlCancel();
		break;
	default:
		// it would be wrong to get here, so leave well alone
		break;
		}
	}

CPosixRequest::~CPosixRequest()
	{
	Cancel();
	if (iFile)
		EndAsynch(KErrCancel);
	}

// Handling waiting on other processes
//
void CPosixRequest::EnList(CPosixRequest*& aHead)
	{
	iNext=aHead;
	aHead=this;
	}

void CPosixRequest::WaitCompleted(TInt aPid, TInt aReason)
	{
	PosixParams* params=REINTERPRET_CAST(PosixParams*,CONST_CAST(TAny*,iMessage.Ptr1()));

	__ASSERT_DEBUG(iMessage.Function()==PMwaitpid, CPosixServer::ServerPanic(EPosix_BadWaitCompletion));
	params->pint[0]=aReason;
	params->ret=aPid;
	iMessage.Complete(KErrNone);

	CPosixRequest* next=iNext;
	iNext=0;
	if (next)
		next->WaitCompleted(aPid, aReason);
	}

static void ClosePipes(CPipeDesc* aPipes[3])
	{
	TInt i=0;
	for (i=0; i<3; i++)
		{
		CPipeDesc* pipe=aPipes[i];
		aPipes[i]=0;
		if (pipe)
			pipe->ClientClose();
		}
	}

TInt CPosixIPCSession::AreYouMyMotherL(const RMessage2& aMessage)
	{
	TPosixIPCPid pid;
	TPosixIPCReply reply;
	aMessage.ReadL(0,pid);
	DebugPrint(_L("Process %d asks am I its mother?"), pid());
	CPosixServer* pServ = const_cast<CPosixServer*>(static_cast<const CPosixServer*>(Server()));
	CPosixProcess* child=pServ->Child(pid());
	if (!child)
		return KErrNotFound;	// you are no child of mine
	DebugPrint(_L("Found child process"));
	child->Sizes(reply);
	aMessage.Write(1,reply);
	return KErrNone;
	}

TInt CPosixIPCSession::HelloMumL(const RMessage2& aMessage)
	{
	TPosixIPCPid pid;
	aMessage.ReadL(0,pid);

	DebugPrint(_L("Process %d is requesting its inheritance"),pid());

	CPosixServer* pServ = const_cast<CPosixServer*>(static_cast<const CPosixServer*>(Server()));
	CPosixProcess* child=pServ->Child(pid());
	if (!child)
		return KErrNotFound;	// you are no child of mine

	// CopyToChildL will pull out the second and third element out directly so as
	// to copy data to it. This is why data is passed in this way.
	child->CopyToChildL(aMessage);
	return KErrNone;
	}

void CPosixIPCSession::PipeRead(const RMessage2& aMessage)
	{
	TInt index=aMessage.Int0();
	if (iPipes[index]==0)
		aMessage.Complete(KErrEof);	// go away, incorrect thing!
	else
		iPipes[index]->ClientRead(aMessage);
	}

void CPosixIPCSession::PipeWrite(const RMessage2& aMessage)
	{
	TInt index=aMessage.Int0();
	if (iPipes[index]==0)
		aMessage.Complete(KErrEof);	// go away, incorrect thing!
	else
		iPipes[index]->ClientWrite(aMessage);
	}

void CPosixIPCSession::PipeIoctl(const RMessage2& aMessage)
	{
	TInt index=aMessage.Int0();
	if (iPipes[index]==0)
		aMessage.Complete(KErrEof);	// go away, incorrect thing!
	else
		iPipes[index]->ClientIoctl(aMessage);
	}

void CPosixIPCSession::PipeClose(const RMessage2& aMessage)
	{
	TInt index=aMessage.Int0();
	if (iPipes[index]!=0)
		iPipes[index]->ClientClose();
	aMessage.Complete(KErrNone);
	}

void CPosixIPCSession::PipeCancel(const RMessage2& aMessage)
	{
	TInt index=aMessage.Int0();
	if (iPipes[index]!=0)
		iPipes[index]->ClientCancel(aMessage);
	aMessage.Complete(KErrNone);
	}

void CPosixIPCSession::ServiceL(const RMessage2& aMessage)
//
// Handle the communication between CPosixServers
//
	{
	TInt response=KErrNone;
	switch (aMessage.Function())
		{
	case PMAreYouMyMother:
		response=AreYouMyMotherL(aMessage);
		break;
	case PMHelloMum:
		response=HelloMumL(aMessage);
		break;

	case PMPipeRead:
		PipeRead(aMessage);
		return;	// handles completion
	case PMPipeWrite:
		PipeWrite(aMessage);
		return; // handles completion
	case PMPipeIoctl:
		PipeIoctl(aMessage);
		return; // handles completion
	case PMPipeClose:
		PipeClose(aMessage);
		return; // handles completion

	case PMPipeCancel:
		PipeCancel(aMessage);
		return;

	default:
		response=KErrNotSupported;
		break;
		}
	aMessage.Complete(response);
	}

void CPosixIPCSession::SetPipes(CPipeDesc* aPipes[3])
//
// Accept ownership of the pipes between child and parent
//
	{
	TInt i=0;
	for (i=0; i<3; i++)
		{
		CPipeDesc* pipe=aPipes[i];
		iPipes[i]=pipe;
		aPipes[i]=0;
		if (pipe)
			pipe->SetClientSide(iPipes[i]);
		}
	}

CPosixIPCSession::~CPosixIPCSession()
	{
	ClosePipes(iPipes);
	}

// Active Object representing a POSIX process

CPosixProcess::CPosixProcess(CPosixServer& aServer)
	: CActive(EPriorityStandard), iServer(aServer)
	{
	// iPid=0;
	// iWaiters=0;
	// iNextProcess=0;
	// iEnvironment=0;
	// iWorkingDirectory=0;
	}

CPosixProcess* CPosixProcess::Find(CPosixProcess* proc, TInt pid)
	{
	while (proc!=0)
		{
		if (proc->iPid==pid)
			return proc;
		if (pid==-1 && !proc->IsAlive())	// for waitpid(WAIT_ANY,...)
			return proc;
		proc=proc->iNextProcess;
		}
	return 0;
	}

void CPosixProcess::Release(CPosixProcess** aHead, CPosixProcess* aChild)
	{
	while (*aHead!=0)
		{
		if ((*aHead)==aChild)
			{
			(*aHead)=aChild->iNextProcess;
			aChild->iNextProcess=0;
			delete aChild;
			return;
			}
		aHead=&(*aHead)->iNextProcess;
		}
	}



void CPosixProcess::POpen3L(PosixParams* aParams)
	{
	TInt i=0;
	CPipeDesc* pipes[3];
	for (i=0; i<3; i++)
		{
		if (aParams->pint[i]<0)
			pipes[i]=0;
		else
			{
			pipes[i]=new(ELeave) CPipeDesc(i);
			pipes[i]->PushLC();
			}
		}
	// truncate fileName to get the name of the executable
	TPtrC16 fileName((TText16*)aParams->wptr[0]);
	TPtrC16 commandLine((TText16*)aParams->cwptr[0]);

	HBufC16* env=Env().ExternalizeLC(iVarCount,aParams->eptr[0]);
	TFullName workingDirectory;
	TInt err=Fs().SessionPath(workingDirectory);
	User::LeaveIfError(err);
	HBufC* cwd=workingDirectory.AllocLC();

	// Use real processes
	err=iChild.Create(fileName,commandLine,EOwnerThread);
	User::LeaveIfError(err);
	TProcessId id=iChild.Id();
	iPid=*REINTERPRET_CAST(int*,&id);
	iChild.Logon(iStatus);
	CActiveScheduler::Add(this);
	SetActive();
	iChild.Resume();
	iEnvironment=env;
	iWorkingDirectory=cwd;
	CleanupStack::Pop(2);
	// Sort out the pipes
	for (i=0; i<3; i++)
		{
		CPipeDesc* pipe=pipes[i];
		iPipes[i]=pipe;
		if (pipe!=0)
			{
			CleanupStack::Pop();
			Fids().Attach(aParams->pint[i],pipe);
			pipe->SetClientSide(iPipes[i]);	// for FinalClose
			}
		}
	}

void CPosixProcess::Sizes(TPosixIPCReply& aReply) const
	{
	aReply().iWorkingDirectorySize=iWorkingDirectory->Length();
	aReply().iEnvironmentSize=iEnvironment->Length();
	aReply().iVarCount=iVarCount;
	TUint mask=0;
	TInt i=0;
	for (i=0; i<3; i++)
		{
		if (iPipes[i]!=0)
			mask |= 1<<i;
		}
	aReply().iPipeMask=mask;
	}

void CPosixProcess::CopyToChildL(const RMessage2& aMessage)
	{
	// copy iWorkingDirectory and iEnvironment to params
	aMessage.WriteL(1, *iEnvironment);
	aMessage.WriteL(2, *iWorkingDirectory);

	// don't need this data anymore
	delete iWorkingDirectory;
	iWorkingDirectory=0;
	delete iEnvironment;
	iEnvironment=0;

	(static_cast<CPosixIPCSession*>(aMessage.Session()))->SetPipes(iPipes);
	}

void CPosixProcess::RunL()
//
// Detects termination of the child process 
//
	{
	iExitReason=iStatus.Int();
	iChild.Close();
	DebugPrint(_L("Process %d appears to have terminated with status %d"), iPid, iExitReason);
	ClosePipes(iPipes);

	TInt reported=0;
	CPosixRequest* waiters=iWaiters;
	iWaiters=0;
	if (waiters) 
		{
		waiters->WaitCompleted(iPid,iExitReason);
		reported=1;
		}

	// And any of the outstanding "wait for any" requests held in the server
	waiters=iServer.Waiters();
	if (waiters)
		{
		waiters->WaitCompleted(iPid,iExitReason);
		reported=1;
		}
	if (reported)
		iServer.Release(this);
	}

void CPosixProcess::DoCancel()
	{
	// panic if iNextProcess or iWaiters is non-zero?
	iChild.LogonCancel(iStatus);
	iChild.Close();
	delete iEnvironment;
	iEnvironment=0;
	delete iWorkingDirectory;
	iWorkingDirectory=0;
	ClosePipes(iPipes);
	}

CPosixProcess::~CPosixProcess()
	{
	Cancel();
	}

// System Interface for process form of STDLIB

CProcessSystemInterface::CProcessSystemInterface()
	{}

CProcessSystemInterface::~CProcessSystemInterface()
	{
	iSession.Close();
	}

MSystemInterface& CProcessSystemInterface::Clone()
	{
	return *(new CProcessSystemInterface);
	}

void CProcessSystemInterface::Release()
	{
	delete this;
	}

TInt CProcessSystemInterface::Connect()
	{
	return iSession.Connect();	// is this the right thread though?
	}

// CProcessSystemInterface functions
//
// These functions just package up their arguments for transmission to the
// CPosixServer which will unpack them and call the corresponding function in
// its associated CLocalSystemInterface, except for the asynchronous functions 
// (currently read/write/fsync) which the server handles separately using an active 
// object to defer the RMessage::Complete until the asynchronous operation has completed.

static void doPanic(TInt aFunction, TInt aErr)
	{
	TBuf<100> detail;
	_LIT(KProcessSystemInterfacePanic, "POSIXIF (%d)");
	detail.Format(KProcessSystemInterfacePanic, aFunction);
	User::Panic(detail,aErr);
	}

int CProcessSystemInterface::Request (TInt aFunction, int& anErrno)
	{
	TInt err=iSession.Request(aFunction,anErrno,iParams);
	// KErrServerTerminated?
	if (err!=KErrNone)
		doPanic(aFunction,err);	// moved out of line to reduce stack requirement
	return iParams.ret;
	}

void CProcessSystemInterface::Request (TInt aFunction, int& anErrno, TRequestStatus& aStatus)
	{
	iSession.Request(aFunction,anErrno,iParams,aStatus);
	}

void CProcessSystemInterface::TerminateProcess (int status)
	{
	int anErrno;
	iParams.fid=status;
	Request(PMTerminateProcess,anErrno);
	RProcess().Terminate(status);	// just in case...
	}

int CProcessSystemInterface::dup (int fid, int& anErrno)
	{
	iParams.fid=fid;
	return Request(PMdup,anErrno);
	}

int CProcessSystemInterface::dup2 (int fid, int fid2, int& anErrno)
	{
	iParams.fid=fid;
	iParams.pint[0]=fid2;
	return Request(PMdup2,anErrno);
	}
	
int CProcessSystemInterface::open (const wchar_t* name, int mode, int perms, int& anErrno)
	{
	iParams.cwptr[0]=name;
	iParams.pint[0]=mode;
	iParams.pint[1]=perms;
	return Request(PMopen,anErrno);
	}

int CProcessSystemInterface::read (int fid, char* buf, unsigned long len, int& anErrno)
	{
	iParams.fid=fid;
	iParams.ptr[0]=buf;
	iParams.len[0]=len;
	return Request(PMread,anErrno);
	}

int CProcessSystemInterface::write (int fid, const char* buf, unsigned long len, int& anErrno)
	{
	iParams.fid=fid;
	iParams.ptr[0]=CONST_CAST(char*,buf);
	iParams.len[0]=len;
	return Request(PMwrite,anErrno);
	}

int CProcessSystemInterface::fsync (int fid, int& anErrno)
	{
	iParams.fid=fid;
	return Request(PMfsync,anErrno);
	}

int CProcessSystemInterface::close (int fid, int& anErrno)
	{
	iParams.fid=fid;
	return Request(PMclose,anErrno);
	}

int CProcessSystemInterface::lseek (int fid, int offset, int whence, int& anErrno)
	{
	iParams.fid=fid;
	iParams.pint[0]=offset;
	iParams.pint[1]=whence;
	return Request(PMlseek,anErrno);
	}

int CProcessSystemInterface::fstat (int fid, struct stat *st, int& anErrno)
	{
	iParams.fid=fid;
	iParams.ptr[0]=(char*)st;
	return Request(PMfstat,anErrno);
	}


wchar_t * CProcessSystemInterface::getcwd (wchar_t* buf, unsigned long len, int& anErrno)
	{
	iParams.wptr[0]=buf;

	iParams.len[0]=len;
	return (wchar_t *)Request(PMgetcwd,anErrno);
	}


int CProcessSystemInterface::chdir (const wchar_t* aPath, int& anErrno)
	{
	iParams.cwptr[0]=aPath;
	return Request(PMchdir,anErrno);
	}

int CProcessSystemInterface::mkdir (const wchar_t* aPath, int perms, int& anErrno)
	{
	iParams.cwptr[0]=aPath;
	iParams.pint[0]=perms;
	return Request(PMmkdir,anErrno);
	}

int CProcessSystemInterface::rmdir (const wchar_t* aPath, int& anErrno)
	{
	iParams.cwptr[0]=aPath;
	return Request(PMrmdir,anErrno);
	}

int CProcessSystemInterface::stat (const wchar_t* name, struct stat *st, int& anErrno)
	{
	iParams.cwptr[0]=name;
	iParams.ptr[0]=(char*)st;
	return Request(PMstat,anErrno);
	}

int CProcessSystemInterface::chmod (const wchar_t* name, int perms, int& anErrno)
	{
	iParams.cwptr[0]=name;
	iParams.pint[0]=perms;
	return Request(PMchmod,anErrno);
	}

int CProcessSystemInterface::unlink (const wchar_t* name, int& anErrno)
	{
	iParams.cwptr[0]=name;
	return Request(PMunlink,anErrno);
	}

int CProcessSystemInterface::rename (const wchar_t* oldname, const wchar_t* newname, int& anErrno)
	{
	iParams.cwptr[0]=oldname;
	iParams.cwptr[1]=newname;
	return Request(PMrename,anErrno);
	}

TInt CProcessSystemInterface::ResolvePath (TParse& aResult, const wchar_t* path, TDes* aFilename)
	{
	TInt ignored;
	iParams.ptr[0]=(char*)&aResult;
	iParams.cwptr[0]=path;
	iParams.ptr[1]=(char*)aFilename;
	return Request(PMResolvePath,ignored);
	}

TInt CProcessSystemInterface::socket (int family, int style, int protocol, int& anErrno)
	{
	iParams.pint[0]=family;
	iParams.pint[1]=style;
	iParams.pint[2]=protocol;
	return Request(PMsocket,anErrno);
	}

TInt CProcessSystemInterface::shutdown (int fid, int how, int& anErrno)
	{
	iParams.fid=fid;
	iParams.pint[0]=how;
	return Request(PMshutdown,anErrno);
	}

TInt CProcessSystemInterface::listen (int fid, int n, int& anErrno)
	{
	iParams.fid=fid;
	iParams.pint[0]=n;
	return Request(PMlisten,anErrno);
	}

TInt CProcessSystemInterface::accept (int fid, int& anErrno)
	{
	iParams.fid=fid;
	return Request(PMaccept,anErrno);
	}

TInt CProcessSystemInterface::bind (int fid, struct sockaddr* addr, unsigned long size, int& anErrno)
	{
	iParams.fid=fid;
	iParams.addr.Set(addr, size);
	return Request(PMbind,anErrno);
	}

TInt CProcessSystemInterface::connect (int fid, struct sockaddr* addr, unsigned long size, int& anErrno)
	{
	iParams.fid=fid;
	iParams.addr.Set(addr, size);
	return Request(PMconnect,anErrno);
	}

TInt CProcessSystemInterface::recvfrom (int fid, char* buf, unsigned long len, int flags, struct sockaddr* from, unsigned long* fromsize, int& anErrno)
	{
	iParams.fid=fid;
	iParams.ptr[0]=buf;
	iParams.len[0]=len;
	iParams.pint[0]=flags;
	iParams.addr.Prepare(from);
	TInt nbytes=Request(PMrecvfrom,anErrno);
	if (nbytes>=0) // i.e. no error
		iParams.addr.Get(from,fromsize);
	return nbytes;
	}

TInt CProcessSystemInterface::sendto (int fid, const char* buf, unsigned long len, int flags, struct sockaddr* to, unsigned long tosize, int& anErrno)
	{
	iParams.fid=fid;
	iParams.ptr[0]=CONST_CAST(char*,buf);
	iParams.len[0]=len;
	iParams.pint[0]=flags;
	iParams.addr.Set(to,tosize);
	return Request(PMsendto,anErrno);
	}

TInt CProcessSystemInterface::getsockopt (int fid, int level, int opt, void* buf, unsigned long* len, int& anErrno)
	{
	iParams.fid=fid;
	iParams.pint[0]=level;
	iParams.pint[1]=opt;
	iParams.ptr[0]=(char*)buf;
	iParams.lenp[0]=len;
	return Request(PMgetsockopt,anErrno);
	}

TInt CProcessSystemInterface::setsockopt (int fid, int level, int opt, void* buf, unsigned long len, int& anErrno)
	{
	iParams.fid=fid;
	iParams.pint[0]=level;
	iParams.pint[1]=opt;
	iParams.ptr[0]=(char*)buf;
	iParams.len[0]=len;
	return Request(PMsetsockopt,anErrno);
	}

TInt CProcessSystemInterface::sockname (int fid, struct sockaddr* addr, unsigned long* size, int anEnd, int& anErrno)
	{
	iParams.fid=fid;
	iParams.addr.Prepare(addr);
	iParams.pint[0]=anEnd;
	TInt err=Request(PMsockname,anErrno);
	if (err==0)
		iParams.addr.Get(addr,size);
	return err;
	}

TInt CProcessSystemInterface::ioctl (int fid, int cmd, void* param, int& anErrno)
	{
	TRequestStatus ioctlStatus;
	TInt err=ioctl(fid,cmd,param,ioctlStatus,anErrno);
	if (err==KErrNone)
		{
		User::WaitForRequest(ioctlStatus);
		err=ioctl_complete(fid,cmd,param,ioctlStatus,anErrno);
		}
	return err;
	}

wchar_t* CProcessSystemInterface::getenv (const wchar_t* name)
	{
	int dummy;
	iParams.cwptr[0]=name;
	return (wchar_t*)Request(PMgetenv,dummy);
	}

void CProcessSystemInterface::unsetenv (const wchar_t* name)
	{
	int dummy;
	iParams.cwptr[0]=name;
	Request(PMunsetenv,dummy);
	}

int CProcessSystemInterface::setenv (const wchar_t* name, const wchar_t* value, int rewrite, int& anErrno)
	{
	iParams.cwptr[0]=name;
	iParams.cwptr[1]=value;
	iParams.pint[0]=rewrite;
	return Request(PMsetenv,anErrno);
	}

int CProcessSystemInterface::popen3 (const wchar_t* file, const wchar_t* cmd, const wchar_t* mode, wchar_t** env, int fids[3], int& anErrno)
	{
	iParams.wptr[0]=(wchar_t*)file;
	iParams.cwptr[0]=cmd;
	iParams.cwptr[1]=mode;
	iParams.eptr[0]=env;
	iParams.pint[0]=fids[0];
	iParams.pint[1]=fids[1];
	iParams.pint[2]=fids[2];
	TInt child=Request(PMpopen3,anErrno);
	if (child>=0)
		{
		fids[0]=iParams.pint[0];
		fids[1]=iParams.pint[1];
		fids[2]=iParams.pint[2];
		};
	return child;
	}

int CProcessSystemInterface::waitpid (int pid, int* status, int options, int& anErrno)
	{
	iParams.pint[0]=pid;
	iParams.pint[1]=options;
	TInt ret=Request(PMwaitpid,anErrno);
	if (iParams.ret>=0 && status!=0)
		{
		*status=iParams.pint[0];
		return 	iParams.ret;
		}
		
	return ret;
	}

// C++ version of asynchronous ioctl
//
// WARNING - this stuff is fairly insecure. We give no guarantees about whether the ioctl or
// the completion will read the parameter information (sometimes it's both).
// 

int CProcessSystemInterface::ioctl (int fid, int cmd, void* param, TRequestStatus& aStatus, int& anErrno)
	{
	iParams.fid=fid;
	iParams.pint[0]=cmd;
	iParams.ptr[0]=(char*)param;

	if (cmd & 0x4000)
		Request(PMioctlN,anErrno,aStatus);
	else
		Request(PMioctl,anErrno,aStatus);
	
	return KErrNone;
	}

int CProcessSystemInterface::ioctl_complete (int fid, int cmd, void* param, TRequestStatus& aStatus, int& anErrno)
	{
	iParams.fid=fid;
	iParams.pint[0]=cmd;
	iParams.ptr[0]=(char*)param;
	iParams.ptr[1]=(char*)&aStatus;
	return Request(PMioctlcomplete,anErrno);
	}

int CProcessSystemInterface::ioctl_cancel (int /*fid*/, int& /*anErrno*/)
//
// Actually a generic Cancel function for any outstanding operation
//
	{
	TIpcArgs args;
	return iSession.Request(PMcancel,args);
	}
