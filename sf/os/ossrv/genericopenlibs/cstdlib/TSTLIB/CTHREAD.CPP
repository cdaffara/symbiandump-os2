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
// C routines for creating EPOC32 threads
// 
//

#include <e32std.h>
#include <estlib.h>		/* for SpawnPosixServerThread */
#include <unistd.h>
#include <stdio.h>		/* for popen3 */
#include <stdlib.h>		/* for mbstowcs */
#include "CTEST.H"
#include <sys/errno.h>		/* for errno */
#include <sys/wait.h>

typedef void (*FUNC)();

struct cthread 
{
	RThread iThread;
	RProcess iProcess;
	TRequestStatus iStatus;
	int pid;
};

TInt threadhelper (TAny* aFn)
	{
	CTrapCleanup::New();
	FUNC f=(FUNC)aFn;
	(*f)();
	return 0;
	}

#if defined(__WINS__)

IMPORT_C void NewProcessId();		// WINS bodges for multiple "processes"
IMPORT_C void NextProcessFn(TAny*);	

TInt processhelper (TAny* aFn)
	{
	// Do the MCRT0.OBJ things straight away
	NewProcessId();
	SpawnPosixServerThread();
	char wd[80];
	getcwd(wd, sizeof(wd));		// connect to CPosixServer
	return threadhelper(aFn);
	}

#endif //__WINS__


extern "C" {

EXPORT_C int start_posix_server()
	{
	start_redirection_server();
	return SpawnPosixServerThread();
	}

EXPORT_C void* create_thread(void (*aFn)(), char* aName)
	{
#ifdef _UNICODE
	TPtrC8 ptr((TText8*)aName);
	TBuf<80> name;
	name.Copy(ptr);
#else
	TPtrC8 name((TText8*)aName);
#endif /* _UNICODE */
	struct cthread* t = new cthread;
	// 16k stack, share parent's heap
	TInt err=t->iThread.Create(name, threadhelper, 0x4000, NULL, (TAny*)aFn);
	t->iThread.Logon(t->iStatus);
	if (err!= KErrNone)
		return 0;
	return (void*)t;
	}

EXPORT_C void start_thread(void* aThread)
	{
	struct cthread* t=REINTERPRET_CAST(struct cthread*,aThread);
	t->iThread.Resume();
	}

EXPORT_C int wait_for_thread(void* aThread)
	{
	struct cthread* t=REINTERPRET_CAST(struct cthread*,aThread);
	User::WaitForRequest(t->iStatus);
	int ret=t->iThread.ExitReason();
	t->iThread.Close();
	delete t;
	return ret;
	}

EXPORT_C void* create_process(void (*aFn)(), char* aName, char* mode, int fids[3])
	{
#ifdef _UNICODE
	TPtrC8 ptr((TText8*)aName);
	TBuf<80> name;
	name.Copy(ptr);
#else
	TPtrC8 name((TText8*)aName);
#endif /* _UNICODE */
	struct cthread* t = new cthread;
	TFileName this_exe = t->iProcess.FileName();
	TBuf<256> cmd;
	cmd.Format(_L("%S %S"),&this_exe,&name);
	cmd.ZeroTerminate();

#ifdef _UNICODE
	wchar_t wmode[MAXPATHLEN+1];
	mbstowcs(wmode, mode, MAXPATHLEN);
	t->pid=wpopen3((const wchar_t*)cmd.Ptr(), wmode, 0, fids);
#else
	t->pid=popen3((const char*)cmd.Ptr(), mode, 0, fids);
#endif
	if (t->pid < 0)
		return 0;
	User::After(1000000); // 1 Second
	return (void*)t;
	}

EXPORT_C void start_process(void* /*aProcess*/)
	{
	// too late, it's already running!
	}

EXPORT_C int wait_for_process(void* aProcess)
	{
	struct cthread* t=REINTERPRET_CAST(struct cthread*,aProcess);
	int exit=-1;
	int pid=waitpid(t->pid, &exit, 0);
	if (pid<0)
		return errno;
	return exit;
	}

EXPORT_C int wait_for_process_id(void* aProcess, int procid, int opt, int* status)
	{
	// added function to enable calling of waitpid with specific parameters
	int pid=waitpid(procid, status, opt);
	return pid;
	}
	
EXPORT_C int get_proc_id(void* aProcess)
	{
	// return the pid of a process
	struct cthread* t=REINTERPRET_CAST(struct cthread*,aProcess);
	return t->pid;
	}

// Testing the dodgy asynchronous form of select

EXPORT_C int async_ioctl(int aFid, int aCmd, void* aParam, int* status)
	{
	TRequestStatus& theStatus = *(TRequestStatus*)status;
	return ioctl(aFid,aCmd,aParam,theStatus);
	}

EXPORT_C int async_ioctl_completion(int aFid, int aCmd, void* aParam, int* status)
	{
	TRequestStatus& theStatus = *(TRequestStatus*)status;
	User::WaitForRequest(theStatus);
	return ioctl_complete(aFid,aCmd,aParam,theStatus);
	}

} // extern "C"
