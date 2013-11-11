/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Portions Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 *	@(#)signal.h	8.3 (Berkeley) 3/30/94
 * $FreeBSD: src/include/signal.h,v 1.24 2003/03/31 23:30:41 jeff Exp $
 */

#ifndef _SIGNAL_H_
#define	_SIGNAL_H_

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/signal.h>

__BEGIN_DECLS

struct timespec;
/* The following definitions have been taken from siglist.c file
 * Once signals are implemented then these definitions must be moved to that
 * file. (sys_signame,sys_siglist,sys_nsig)  */

static const char *const sys_signame[NSIG] = {
	"Signal 0",
	"hup",				/* SIGHUP */
	"int",				/* SIGINT */
	"quit",				/* SIGQUIT */
	"ill",				/* SIGILL */
	"trap",				/* SIGTRAP */
	"abrt",				/* SIGABRT */
	"emt",				/* SIGEMT */
	"fpe",				/* SIGFPE */
	"kill",				/* SIGKILL */
	"bus",				/* SIGBUS */
	"segv",				/* SIGSEGV */
	"sys",				/* SIGSYS */
	"pipe",				/* SIGPIPE */
	"alrm",				/* SIGALRM */
	"term",				/* SIGTERM */
	"urg",				/* SIGURG */
	"stop",				/* SIGSTOP */
	"tstp",				/* SIGTSTP */
	"cont",				/* SIGCONT */
	"chld",				/* SIGCHLD */
	"ttin",				/* SIGTTIN */
	"ttou",				/* SIGTTOU */
	"io",				/* SIGIO */
	"xcpu",				/* SIGXCPU */
	"xfsz",				/* SIGXFSZ */
	"vtalrm",			/* SIGVTALRM */
	"prof",				/* SIGPROF */
	"winch",			/* SIGWINCH */
	"info",				/* SIGINFO */
	"usr1",				/* SIGUSR1 */
	"usr2"				/* SIGUSR2 */
};

static const char *const sys_siglist[NSIG] = {
	"Signal 0",
	"Hangup",			/* SIGHUP */
	"Interrupt",			/* SIGINT */
	"Quit",				/* SIGQUIT */
	"Illegal instruction",		/* SIGILL */
	"Trace/BPT trap",		/* SIGTRAP */
	"Abort trap",			/* SIGABRT */
	"EMT trap",			/* SIGEMT */
	"Floating point exception",	/* SIGFPE */
	"Killed",			/* SIGKILL */
	"Bus error",			/* SIGBUS */
	"Segmentation fault",		/* SIGSEGV */
	"Bad system call",		/* SIGSYS */
	"Broken pipe",			/* SIGPIPE */
	"Alarm clock",			/* SIGALRM */
	"Terminated",			/* SIGTERM */
	"Urgent I/O condition",		/* SIGURG */
	"Suspended (signal)",		/* SIGSTOP */
	"Suspended",			/* SIGTSTP */
	"Continued",			/* SIGCONT */
	"Child exited",			/* SIGCHLD */
	"Stopped (tty input)",		/* SIGTTIN */
	"Stopped (tty output)",		/* SIGTTOU */
	"I/O possible",			/* SIGIO */
	"Cputime limit exceeded",	/* SIGXCPU */
	"Filesize limit exceeded",	/* SIGXFSZ */
	"Virtual timer expired",	/* SIGVTALRM */
	"Profiling timer expired",	/* SIGPROF */
	"Window size changes",		/* SIGWINCH */
	"Information request",		/* SIGINFO */
	"User defined signal 1",	/* SIGUSR1 */
	"User defined signal 2"		/* SIGUSR2 */
};

#if !defined (SIG_BLOCK)
	#define SIG_UNBLOCK 1
	#define SIG_BLOCK   2
	#define SIG_SETMASK 3
#endif

/* Type for data associated with a signal.  */
union sigval
{
	int    sival_int;
	void  *sival_ptr;
};

typedef struct __siginfo_t
{
	int           si_signo;
	int           si_code;
	int           si_errno;
	pid_t         si_pid;
	uid_t         si_uid;
	void         *si_addr;
	int           si_status;
	long          si_band;
	union sigval  si_value;
} siginfo_t;


typedef struct sigevent
{
	union sigval           sigev_value;  
    int sigev_signo;
    int sigev_notify;

    void (*sigev_notify_function) (union sigval);       /* Function to start.  */
    void *sigev_notify_attributes;                  /* Really pthread_attr_t.  */
} sigevent_t;

/* `sigev_notify' values.  */
enum
{
  SIGEV_SIGNAL = 0,             /* Notify via signal.  */
# define SIGEV_SIGNAL   SIGEV_SIGNAL
  SIGEV_NONE,                   /* Other notification: meaningless.  */
# define SIGEV_NONE     SIGEV_NONE
  SIGEV_THREAD,                 /* Deliver via thread creation.  */
# define SIGEV_THREAD   SIGEV_THREAD

  SIGEV_THREAD_ID = 4           /* Send signal to specific thread.  */
#define SIGEV_THREAD_ID SIGEV_THREAD_ID
};

static const int sys_nsig = sizeof(sys_siglist) / sizeof(sys_siglist[0]);

IMPORT_C int sigaction(int sig, const struct sigaction *act, struct sigaction *oact);

IMPORT_C int sigemptyset(sigset_t* set);

#ifdef SYMBIAN_OE_POSIX_SIGNALS
IMPORT_C int kill(pid_t pid, int sig);
IMPORT_C int raise(int sig);
IMPORT_C int sigqueue(pid_t pid, int sig, const union sigval value);

IMPORT_C int sigfillset(sigset_t *set);
IMPORT_C int sigaddset(sigset_t *set, int signo);
IMPORT_C int sigdelset(sigset_t *set, int signo);
IMPORT_C int sigismember(const sigset_t *set, int signo);
IMPORT_C int sigandset(sigset_t * set, const sigset_t * left, const sigset_t * right);
IMPORT_C int sigorset(sigset_t * set, const sigset_t * left, const sigset_t * right);
IMPORT_C int sigisemptyset(const sigset_t * set);

IMPORT_C int sigprocmask(int how, const sigset_t* set,sigset_t* oset);
IMPORT_C int sighold(int signo);
IMPORT_C int sigrelse(int signo);
IMPORT_C int sigpause(int signo);

IMPORT_C int sigwait(const sigset_t *set, int *sig);
IMPORT_C int sigtimedwait(const sigset_t *set, siginfo_t *info,
       const struct timespec *timeout);
IMPORT_C int sigwaitinfo(const sigset_t *set, siginfo_t *info);

IMPORT_C void (*bsd_signal(int, void (*)(int)))(int);
IMPORT_C void (*sigset(int, void (*)(int)))(int);
IMPORT_C void (*signal(int, void (*)(int)))(int);
IMPORT_C int sigpending(sigset_t *set);
IMPORT_C int sigignore(int sig);
IMPORT_C void psignal(int sig, const char *s);

IMPORT_C int sigenable();

#endif
__END_DECLS

#endif /* !_SIGNAL_H_ */
