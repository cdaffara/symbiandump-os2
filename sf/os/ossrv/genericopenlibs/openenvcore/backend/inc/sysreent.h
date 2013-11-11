/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* The reentrant system calls here serve two purposes:
* 1) Provide reentrant versions of the system calls the ANSI C library
* requires.
* 2) Provide these system calls in a namespace clean way.
* It is intended that *all* system calls that the ANSI C library needs
* be declared here.  It documents them all in one place.  All library access
* to the system is via some form of these functions.
* There are three ways a target may provide the needed syscalls.
* 1) Define the reentrant versions of the syscalls directly.
* (eg: _open_r, _close_r, etc.).  Please keep the namespace clean.
* When you do this, set "syscall_dir" to "syscalls" in configure.in,
* and add -DREENTRANT_SYSCALLS_PROVIDED to target_cflags in configure.in.
* 2) Define namespace clean versions of the system calls by prefixing
* them with '_' (eg: _open, _close, etc.).  Technically, there won't be
* true reentrancy at the syscall level, but the library will be namespace
* clean.
* When you do this, set "syscall_dir" to "syscalls" in configure.in.
* 3) Define or otherwise provide the regular versions of the syscalls
* (eg: open, close, etc.).  The library won't be reentrant nor namespace
* clean, but at least it will work.
* When you do this, add -DMISSING_SYSCALL_NAMES to target_cflags in
* configure.in.
* Stubs of the reentrant versions of the syscalls exist in the libc/reent
* source directory and are used if REENTRANT_SYSCALLS_PROVIDED isn't defined.
* They use the native system calls: _open, _close, etc. if they're available
* (MISSING_SYSCALL_NAMES is *not* defined), otherwise open, close, etc.
* (MISSING_SYSCALL_NAMES *is* defined).
* WARNING: All identifiers here must begin with an underscore.  This file is
* included by stdio.h and others and we therefore must only use identifiers
* in the namespace allotted to us.  
* @file
* @internalComponent
* 
*
*/



#ifndef _SYSREENT_H_
#define _SYSREENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#define __need_size_t
#include <stddef.h>
#include <sys/_types.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/select.h>
#include <spawn.h>

/* FIXME: not namespace clean */
/*
@internalTechnology
*/
struct stat;
/*
@internalTechnology
*/
struct sockaddr;
struct TProtocolDesc;

/*
Reentrant versions of system calls.
Most of these are thread-safe in EPOC32 anyway
*/
IMPORT_C int	_chdir_r	(int *, const wchar_t *);
IMPORT_C int	_wchdir_r	(int *, const wchar_t *);
IMPORT_C int	_chmod_r	(int *, const wchar_t *, mode_t);
IMPORT_C int	_fchmod_r	(int *, int, mode_t);
IMPORT_C int	_wchmod_r	(int *, const wchar_t *, mode_t);
IMPORT_C int	_close_r	(int *, int);
IMPORT_C int	_dup_r		(int *, int);
IMPORT_C int	_dup2_r	(int *, int, int);
IMPORT_C int	_fcntl_r	(int *, int, int, long);
IMPORT_C int	_fstat_r	(int *, int, struct stat *);
IMPORT_C int	_fsync_r	(int *, int);
IMPORT_C wchar_t*	_wgetcwd_r	(int *, wchar_t *, size_t);
IMPORT_C int	_ioctl_r	(int *, int, int, void *);
IMPORT_C int	_link_r		(int *, const wchar_t *, const wchar_t *);
IMPORT_C off_t	_lseek_r	(int *, int, off_t, int);
IMPORT_C int	_mkdir_r	(int *, const wchar_t *, mode_t);
IMPORT_C int	_wmkdir_r	(int *, const wchar_t *, mode_t);
IMPORT_C int	_open_r	(int *, const wchar_t *, int, int);
IMPORT_C int	_wopen_r	(int *, const wchar_t *, int, int);
IMPORT_C int	_read_r	(int *, int, char *, size_t);
IMPORT_C wchar_t*	_wrealpath_r	(int *, const wchar_t *, wchar_t *);
IMPORT_C int	_rename_r	(int *, const wchar_t *_old, const wchar_t *_new);
IMPORT_C int	_wrename_r	(int *, const wchar_t *_old, const wchar_t *_new);
IMPORT_C int	_rmdir_r	(int *, const wchar_t *);
IMPORT_C int	_wrmdir_r	(int *, const wchar_t *);
IMPORT_C int	_stat_r		(int *, const wchar_t *, struct stat *);
IMPORT_C int    _lstat_r    (int *, const wchar_t *, struct stat *);
IMPORT_C int	_wstat_r	(int *, const wchar_t *, struct stat *);
IMPORT_C int	_unlink_r	(int *, const wchar_t *);
IMPORT_C int	_wunlink_r	(int *, const wchar_t *);
IMPORT_C int	_wait_r		(int *, int *);
IMPORT_C int	_waitpid_r	(int *, int, int *, int);
IMPORT_C int	_write_r	(int *, int, const char *, size_t);

IMPORT_C int	_accept_r	(int *, int, struct sockaddr *, size_t *);
IMPORT_C int	_bind_r	(int *, int, const struct sockaddr *, size_t);
IMPORT_C int	_connect_r	(int *, int, const struct sockaddr *, size_t);
IMPORT_C int	_getpeername_r	(int *, int, struct sockaddr *, size_t *);
IMPORT_C int	_getsockname_r	(int *, int, struct sockaddr *, size_t *);
IMPORT_C int	_getsockopt_r	(int *, int, int, int, void *, size_t *);
IMPORT_C int	_getprotobynumber_r (int*, int, struct TProtocolDesc*);
IMPORT_C int	_getprotobyname_r (int*, const char*, struct TProtocolDesc*);
IMPORT_C int	_gethostname_r (int*, char*, size_t);
IMPORT_C int	_listen_r	(int *, int, int);
IMPORT_C int	_recvfrom_r	(int *, int, char *, size_t, int, struct sockaddr *, size_t *);
IMPORT_C int	_sendto_r	(int *, int, const char *, size_t, int, struct sockaddr *, size_t);
IMPORT_C int	_setsockopt_r	(int *, int, int, int, void *, size_t);
IMPORT_C int	_socket_r	(int *, int, int, int);
IMPORT_C int	_shutdown_r	(int *, int, int);

#ifdef SYMBIAN_OE_POSIX_SIGNALS
IMPORT_C int _kill_r (int *, pid_t, int);
IMPORT_C int _raise_r (int *, int);
IMPORT_C int _sigqueue_r (int *, pid_t, int, const union sigval);

IMPORT_C int _sigfillset_r(int *, sigset_t *);
IMPORT_C int _sigaddset_r(int *, sigset_t *, int );
IMPORT_C int _sigdelset_r(int *, sigset_t *, int );
IMPORT_C int _sigismember_r(int *, const sigset_t *, int );
IMPORT_C int _sigandset_r(int *, sigset_t * , const sigset_t * , const sigset_t * );
IMPORT_C int _sigorset_r(int *, sigset_t * , const sigset_t * , const sigset_t * );
IMPORT_C int _sigisemptyset_r(int *, const sigset_t * );
IMPORT_C int _sigemptyset_r(int *, sigset_t* set);

IMPORT_C int _sigprocmask_r(int*, int , const sigset_t *, sigset_t *);
IMPORT_C int _sighold_r(int*, int);
IMPORT_C int _sigrelse_r(int*, int);
IMPORT_C int _sigpause_r(int*, int);

IMPORT_C int _sigwait_r(int*, const sigset_t*, int*);
IMPORT_C int _sigtimedwait_r(int*, const sigset_t*, siginfo_t*,
       const struct timespec*);
IMPORT_C int _sigwaitinfo_r(int*, const sigset_t*, siginfo_t*);

IMPORT_C int _sigaction_r(int *aErrno, int sig, const struct sigaction *act, struct sigaction *oact);
IMPORT_C void (*_sigset_r(int *aErrno, int sig, void (*disp)(int)))(int);
IMPORT_C void (*_bsd_signal_r(int *aErrno, int sig, void (*disp)(int)))(int);
IMPORT_C void (*_signal_r(int *aErrno, int sig, void (*disp)(int)))(int);
IMPORT_C int _sigignore_r(int *aErrno, int sig);
IMPORT_C unsigned int _alarm_r(unsigned int seconds);
IMPORT_C int _sigpending_r(int *aErrno,sigset_t *aSet);
IMPORT_C int _sigenable_r(int *aErrno);
#endif // SYMBIAN_OE_POSIX_SIGNALS
//forward declaration
struct utimbuf;
IMPORT_C int 	_utime_r (int *, const wchar_t*, const struct utimbuf*);

IMPORT_C DIR* _opendir_r (int*, const wchar_t* _path) ;
IMPORT_C int _wclosedir_r (int*, WDIR *dp) ;
IMPORT_C struct dirent* _readdir_r (int*, DIR *dp) ;
IMPORT_C void _wrewinddir_r(WDIR *dp);
IMPORT_C struct wdirent* _wreaddir_r(WDIR *dp);
IMPORT_C int _fchdir_r (int *aErrno, int filedesc);
IMPORT_C wchar_t* _getcwd_r (int *aErrno, wchar_t *_buf, size_t _size);
IMPORT_C int 	_mkfifo_r (int *aErrno, const wchar_t *pathname, mode_t mode);
IMPORT_C int 	_pipe_r (int *aErrno, int fildes[2]);
IMPORT_C int 	_wpopen3_r	(int *aErrno, const wchar_t *file, const wchar_t *cmd, wchar_t** envp, int fids[3]);

IMPORT_C int _posix_spawn_r (int* pid, const wchar_t* wpath,
							 const posix_spawn_file_actions_t* file_actions,
							 const posix_spawnattr_t* attrp,
							 const wchar_t* wargs,
							 wchar_t** wenvp);

IMPORT_C void _exit_r (int code);
/*
New function introduced for MRT2.0
Reentrant version of popen() function
*/
IMPORT_C int _wpopen_r (int *aErrno, const wchar_t* command, const char* mode);
IMPORT_C int _pclose_r (int *aErrno, int aFid);

IMPORT_C int _shmget_r	(int *aErrno, key_t key, int size, int shmflg);
IMPORT_C void* _shmat_r	(int *aErrno, int shmid, const void *shmaddr, int shmflg);
IMPORT_C int _shmdt_r	(int *aErrno, const void *shmaddr);

//forward declarations
struct shmid_ds;
struct sembuf;

IMPORT_C int _shmctl_r	(int *aErrno,int shmid, int cmd, struct shmid_ds *buf);

IMPORT_C int _semget_r	(int *aErrno, key_t key, int nsems, int semflg);
IMPORT_C int _semop_r	(int *aErrno, int semid, struct sembuf *sops, unsigned nsops);
IMPORT_C int _semctl_r	(int *aErrno, int semid, int semnum, int cmd, ...);

IMPORT_C int _select_r	(int *aErrno, int maxfd, fd_set *readfds,
						 fd_set *writefds, fd_set *exceptfds, struct timeval *tvptr);

IMPORT_C int _wsystem_r	(int *aErrno, const wchar_t *command);

typedef void* iconv_t;
IMPORT_C iconv_t _iconv_open_r(int* aErrno, const char* tocharset, const char* fromcharset);

IMPORT_C size_t _iconv_r(int* aErrno, iconv_t conversion_descriptor, const char** inbuf, size_t* inbytesleft, char** outbuf, size_t* outbytesleft);

IMPORT_C int _iconv_close_r(int* aErrno, iconv_t conversion_descriptor);
/*
Functions to allocate/deallocate on the backend's private heap
*/
IMPORT_C void* BackendAlloc(size_t aNBytes);
IMPORT_C void* BackendReAlloc(void* aPtr, size_t aNBytes);
IMPORT_C void BackendFree(void* aPtr);
IMPORT_C void* BackendAllocTLD(size_t aSize);

#define _remove_r(r,x)	_unlink_r(r,x)
#define _wremove_r(r,x)	_wunlink_r(r,x)


//forward declarations
struct msqid_ds;

IMPORT_C int _msgget_r(key_t aKey, int aMsgFlag, int* aErrno);
IMPORT_C int _msgsnd_r(int msqid, const void *msgp, size_t msgsz, int msgflg, int* aErrno);
IMPORT_C int _msgrcv_r(int msqid, void* msgp, size_t msgsz, long msgtyp, int msgflg, int* aErrno);
IMPORT_C int _msgctl_r(int msqid, int cmd, struct msqid_ds* buf, int* aErrno);

IMPORT_C int _setecho_r(int *aErrno, int aFd, uint8_t aEchoVal);

//forward declarations
struct ifreq;

IMPORT_C int _setdefaultif_r(int *aErrno, const struct ifreq* aIfReq);
IMPORT_C int _unsetdefaultif_r(int *aErrno);

IMPORT_C void *_memcpy_r(void *dst0, const void *src0, size_t length);
IMPORT_C void *_memmove_r(void *dst0, const void *src0, size_t length);

#ifdef __cplusplus
}	// extern "C"
#endif
#endif /* _SYSREENT_H_ */
