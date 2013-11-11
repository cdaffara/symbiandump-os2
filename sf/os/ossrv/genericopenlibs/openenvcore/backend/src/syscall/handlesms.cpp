// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <sys/errno.h>
#include <unistd.h>

#include "sysreent.h"
#include "sysif.h"

extern "C" {

#ifdef SYMBIAN_OE_POSIX_SIGNALS

/* A reentrant version of kill()*/
EXPORT_C int _kill_r (int *aErrno, pid_t pid, int sig)
	{
	return Backend()->kill(pid, sig, *aErrno);
	}

/* A reentrant version of raise()*/
EXPORT_C int _raise_r (int *aErrno, int sig)
	{
	return Backend()->raise(sig, *aErrno);
	}

EXPORT_C int _sigqueue_r (int *aErrno, pid_t pid, int sig, const union sigval val)
	{
	return Backend()->sigqueue(pid,sig,val,*aErrno);
	}

EXPORT_C int _sigfillset_r(int* aErrno, sigset_t *set)
	{
	return Backend()->sigfillset(set, *aErrno);
	}

EXPORT_C int _sigaddset_r(int* aErrno, sigset_t *set, int signo)
	{
	return Backend()->sigaddset(set, signo, *aErrno);
	}

EXPORT_C int _sigdelset_r(int* aErrno, sigset_t *set, int signo)
	{
	return Backend()->sigdelset(set, signo, *aErrno);
	}

EXPORT_C int _sigismember_r(int *aErrno, const sigset_t *set, int signo)
	{
	return Backend()->sigismember(set, signo, *aErrno);
	}

EXPORT_C int _sigandset_r(int *aErrno, sigset_t * set, const sigset_t *left, const sigset_t *right)
	{
	return Backend()->sigandset(set, left, right, *aErrno);
	}

EXPORT_C int _sigorset_r(int *aErrno, sigset_t * set, const sigset_t *left, const sigset_t *right)
	{
	return Backend()->sigorset(set, left, right, *aErrno);
	}

EXPORT_C int _sigisemptyset_r(int *aErrno, const sigset_t *set)
	{
	return Backend()->sigisemptyset(set, *aErrno);
	}

EXPORT_C int _sigemptyset_r(int *aErrno, sigset_t *set)
	{
	return Backend()->sigemptyset(set, *aErrno);
	}

EXPORT_C int _sigprocmask_r (int *aErrno, int how, const sigset_t *set, sigset_t *oset)
	{
	return Backend()->sigprocmask(how,set,oset,*aErrno);
	}

EXPORT_C int _sighold_r (int *aErrno, int signo)
	{
	return Backend()->sighold(signo,*aErrno);
	}

EXPORT_C int _sigrelse_r (int *aErrno, int signo)
	{
	return Backend()->sigrelse(signo,*aErrno);
	}

EXPORT_C int _sigpause_r (int *aErrno, int signo)
	{
	return Backend()->sigpause(signo,*aErrno);
	}

EXPORT_C int _sigwait_r(int *aErrno, const sigset_t *set, int *signo)
	{
	return Backend()->sigwait(set,signo,*aErrno);
	}

EXPORT_C int _sigtimedwait_r(int *aErrno, const sigset_t *set, siginfo_t *info,
       const struct timespec *timeout)
	{
	return Backend()->sigtimedwait(set,info,timeout,*aErrno);
	}

EXPORT_C int _sigwaitinfo_r(int *aErrno, const sigset_t *set, siginfo_t *info)
	{
	return Backend()->sigwaitinfo(set,info,*aErrno);
	}
EXPORT_C int _sigaction_r(int *aErrno, int sig, const struct sigaction *act, struct sigaction *oact)
	{
	return Backend()->sigaction(sig, act, oact, *aErrno);
	}

EXPORT_C void (*_bsd_signal_r(int *aErrno, int sig, void (*disp)(int)))(int)
	{
	return Backend()->bsd_signal(sig, disp, *aErrno);
	}

EXPORT_C void (*_sigset_r(int *aErrno, int sig, void (*disp)(int)))(int)
	{
	return Backend()->sigset(sig, disp, *aErrno);
	}

EXPORT_C void (*_signal_r(int *aErrno, int sig, void (*disp)(int)))(int)
	{
	return Backend()->signal(sig, disp, *aErrno);
	}

EXPORT_C int _sigignore_r(int *aErrno, int sig)
	{
	return Backend()->sigignore(sig, *aErrno);
	}

EXPORT_C unsigned int _alarm_r(unsigned int aSecs)
	{
	return Backend()->alarm(aSecs);
	}

EXPORT_C int _sigpending_r(int *aErrno, sigset_t *aSet)
	{
	return Backend()->sigpending(aSet, *aErrno);
	}

EXPORT_C int _sigenable_r(int *aErrno)
	{
	aErrno = aErrno;
	Backend()->InitSignalHandler();
	return 0;
	}	
#endif // SYMBIAN_OE_POSIX_SIGNALS

} //extern "C"
