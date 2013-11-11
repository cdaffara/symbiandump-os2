/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sysusrinclude.h
* Part of     : libc-sys specific include file
* Version     : 1.0
*
*/




#ifndef	_SYSUSRINCLUDE_H_
#define	_SYSUSRINCLUDE_H_
#include <sys/types.h>
#include <netdb.h>

/*
  Declaration of Handle functions for each Module 
 */
#ifdef __cplusplus
extern "C"  {
#endif
 
pid_t _getpid();
int _issetugid();
int _gethostname (char *name, size_t size);


// process group/user ID related API

IMPORT_C gid_t _getgid( void );
IMPORT_C int _setgid( gid_t gid );
IMPORT_C gid_t _getegid( void );
IMPORT_C int _setegid( gid_t gid );
IMPORT_C int _setregid( gid_t rgid, gid_t egid );
IMPORT_C uid_t _getuid( void );
IMPORT_C int _setuid( uid_t uid );
IMPORT_C uid_t _geteuid( void );
IMPORT_C int _seteuid( uid_t uid );
IMPORT_C int _setreuid( uid_t ruid, uid_t euid );
IMPORT_C pid_t _getpgid( pid_t pid );
IMPORT_C pid_t _setpgid( pid_t pid, pid_t pgid );
IMPORT_C pid_t _getpgrp( void );
IMPORT_C int _setpgrp( pid_t _pid, pid_t _pgrp );
IMPORT_C int _getgroups( int gidsetsize, gid_t grouplist[] );
IMPORT_C pid_t _setsid( void );
IMPORT_C pid_t _getppid( void );
IMPORT_C mode_t _umask(mode_t cmask);
IMPORT_C int _chown(const char *path, uid_t owner, gid_t group);
IMPORT_C int _lchown(const char *path, uid_t owner, gid_t group);
int _yield(void);

 #ifdef __cplusplus
 }
 #endif	//__cplusplus


#endif /* !_SYS_USR_INCLUDE_H_ */
 
