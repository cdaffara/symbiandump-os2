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
#include <sched.h>
#include "sysusrinclude.h"


// MACROS
#define MASK_RWUSR	0066



extern "C" {

EXPORT_C gid_t _getgid( void )
{	
	return 0;
}

EXPORT_C int _setgid( gid_t /*gid*/ )
{	
	return 0;
}


EXPORT_C gid_t _getegid( void )
{	
	return 0;
}

EXPORT_C int _setegid( gid_t /*gid*/ )
{		
	return 0;
}

EXPORT_C int _setregid( gid_t /*rgid*/, gid_t /*egid*/ )
{		
	return 0;
}

EXPORT_C uid_t _getuid( void )
{	
	return 0;
}

EXPORT_C int _setuid( uid_t /*uid*/ )
{	
	return 0;
}

EXPORT_C uid_t _geteuid( void )
{	
	return 0;
}

EXPORT_C int _seteuid( uid_t /*uid*/ )
{		
	return 0;
}

EXPORT_C int _setreuid( uid_t /*ruid*/, uid_t /*euid*/ )
{		
	return 0;
}

EXPORT_C pid_t _getpgid( pid_t /*pid*/ )
{		
	return 0;
}

EXPORT_C pid_t _setpgid( pid_t /*pid*/, pid_t /*pgid*/ )
{		
	return 0;
}

EXPORT_C pid_t _getpgrp( void )
{		
	return 0;
}

EXPORT_C int _setpgrp( pid_t /*_pid*/, pid_t /*_pgrp*/ )
{		
	return 0;
}

EXPORT_C int _getgroups( int /*gidsetsize*/, gid_t* /*grouplist[]*/ )
{				
	return 0;
}

EXPORT_C pid_t _setsid( void )
{				
	return 0;
} 

EXPORT_C pid_t _getppid( void )
{				
	return 0;
} 

EXPORT_C mode_t _umask(mode_t /*cmask*/)
{				
	/* return Read+write for user permission */
	return (mode_t) ( MASK_RWUSR );
} 

EXPORT_C int _chown(const char */*path*/, uid_t /*owner*/, gid_t /*group*/)
{				
	return 0;
} 

EXPORT_C int _lchown(const char */*path*/, uid_t /*owner*/, gid_t /*group*/)
{
	return 0;
}

} //extern C end
