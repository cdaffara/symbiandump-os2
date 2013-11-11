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
* 
*
*/



#ifndef	_SPAWN_H_
#define	_SPAWN_H_

#include <signal.h>
#include <sys/types.h>
#include <sched.h>

typedef struct {
	short _flags;
	pid_t _pgrp;
	sigset_t _sd;
	sigset_t _sm;
	struct sched_param _sp;
	int _policy;
} posix_spawnattr_t;

/* Flags that can be set in posix_spawnattr_t */
#define POSIX_SPAWN_RESETIDS		0x01
#define POSIX_SPAWN_SETPGROUP		0x02
#define POSIX_SPAWN_SETSIGDEF		0x04
#define POSIX_SPAWN_SETSIGMASK		0x08
#define POSIX_SPAWN_SETSCHEDPARAM	0x10
#define POSIX_SPAWN_SETSCHEDULER	0x20

// forward declaration
struct file_actions_t;
typedef struct {
	int _cnt;
	struct file_actions_t* _fa;
} posix_spawn_file_actions_t;

__BEGIN_DECLS

IMPORT_C int posix_spawn(pid_t* pid, const char* path,
				const posix_spawn_file_actions_t* file_actions,
				const posix_spawnattr_t* attrp, char *const argv[],
				char *const envp[]);
				
IMPORT_C int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t* file_actions,
				int fid);
				
IMPORT_C int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t* file_actions,
				int fid1, int fid2);
				
IMPORT_C int posix_spawn_file_actions_addopen(posix_spawn_file_actions_t* file_actions,
          		int fid, const char* path, int oflag, mode_t mode);
          		
IMPORT_C int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t* file_actions);

IMPORT_C int posix_spawn_file_actions_init(posix_spawn_file_actions_t* file_actions);

IMPORT_C int posix_spawnattr_destroy(posix_spawnattr_t* attrp);

IMPORT_C int posix_spawnattr_getsigdefault(const posix_spawnattr_t* attrp, 
				sigset_t* sigdefault);
									
IMPORT_C int posix_spawnattr_getflags(const posix_spawnattr_t* attrp, short* flags);

IMPORT_C int posix_spawnattr_getpgroup(const posix_spawnattr_t* attrp, pid_t* pgroup);

IMPORT_C int posix_spawnattr_getschedparam(const posix_spawnattr_t* attrp,
          		struct sched_param* schedparam);
          		
IMPORT_C int posix_spawnattr_getschedpolicy(const posix_spawnattr_t* attrp, int* policy);

IMPORT_C int posix_spawnattr_getsigmask(const posix_spawnattr_t* attrp, sigset_t* sigmask);

IMPORT_C int posix_spawnattr_init(posix_spawnattr_t* attrp);

IMPORT_C int posix_spawnattr_setsigdefault(posix_spawnattr_t* attrp,
				const sigset_t* sigdefault);

IMPORT_C int posix_spawnattr_setflags(posix_spawnattr_t* attrp, short flags);

IMPORT_C int posix_spawnattr_setpgroup(posix_spawnattr_t* attrp, pid_t pgroup);

IMPORT_C int posix_spawnattr_setschedparam(posix_spawnattr_t* attrp,
				const struct sched_param* schedparam);
				
IMPORT_C int posix_spawnattr_setschedpolicy(posix_spawnattr_t* attrp, int policy);

IMPORT_C int posix_spawnattr_setsigmask(posix_spawnattr_t* attrp, const sigset_t* sigmask);
          
__END_DECLS

#endif /* _SPAWN_H_ */
