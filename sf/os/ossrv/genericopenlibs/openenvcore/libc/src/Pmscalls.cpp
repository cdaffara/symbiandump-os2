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
*
*/


#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sched.h>

#include "sysif.h"
#include "sysusrinclude.h"


#define MASK_RWUSR    0066   // Read and write permission for user alone

extern "C" {
// Calling Process GID observer and modifier

EXPORT_C int setgid( gid_t gid )
	{
	return _setgid(gid );
	}



EXPORT_C gid_t getegid( void )
	{
	return _getegid();
	}



EXPORT_C int setegid( gid_t gid )
	{		
	return _setegid(gid);
	}


EXPORT_C int setregid( gid_t rgid, gid_t egid )
	{		
	return _setregid(rgid, egid);
	}


// Calling Process UID observer and modifier 

EXPORT_C gid_t getgid( void )
	{	
	return _getgid();
	}

EXPORT_C uid_t getuid( void )
	{	
	return _getuid();
	}

EXPORT_C int setuid( uid_t uid )
	{	
	return _setuid(uid);
	}


EXPORT_C uid_t geteuid( void )
	{	
	return _geteuid();
	}

EXPORT_C int seteuid( uid_t uid )
	{		
	return _seteuid(uid);
	}


EXPORT_C int setreuid( uid_t ruid, uid_t euid )
	{		
	return _setreuid(ruid, euid);
	}


// Process group ID observer and modifier 

EXPORT_C pid_t getpgid( pid_t pid )
	{		
	return _getpgid(pid);
	}

EXPORT_C pid_t setpgid( pid_t pid, pid_t pgid )
	{		
	return _setpgid(pid, pgid);
	}


EXPORT_C pid_t getpgrp( void )
	{		
	return _getpgrp();
	}



EXPORT_C int setpgrp( pid_t _pid, pid_t  _pgrp )
	{		
	return _setpgrp(_pid,_pgrp);
	}


// Supplementary Group ID observer

EXPORT_C int getgroups( int size, gid_t grouplist[] )
	{				
	return _getgroups(size, grouplist);
	}


// Create New Session(process group)

EXPORT_C pid_t setsid( void )
	{				
	return _setsid();
	} 


// Parent process ID

EXPORT_C pid_t getppid( void )
	{				
	return _getppid();
	} 


// Set file mode creation mask

EXPORT_C mode_t umask(mode_t cmask)
	{				
	return _umask(cmask);
	} 


// change owner and group of a file

EXPORT_C int chown(const char *path, uid_t owner, gid_t group)
	{				
	return _chown(path, owner, group);
	} 


//change the owner and group of a symbolic link

EXPORT_C int lchown(const char *path, uid_t owner, gid_t group)
	{
	return _lchown(path, owner, group);
	}


// Initialize supplementry group list

EXPORT_C int initgroups(const char */*name*/, gid_t /*basegid*/)
	{
	return 0;
	}

// yield control to a similar or higher priority thread

EXPORT_C int sched_yield( void )
	{
	return usleep(0);
	}


//Return the maximum priority as 0. 


EXPORT_C int sched_get_priority_max(int /*policy*/)
	{
	/* returning absolute max priorit, needs to be reviewed*/
	return (255);
	}


// Return the minimum priority as 0.

EXPORT_C int sched_get_priority_min(int /*policy*/)
	{
	return 0;
	}

// Set end of the data segment to specified value.
EXPORT_C int brk(const void* /*end_data_seg*/)
	{
	return 0;
	}

} // extern "C"
