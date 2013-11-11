/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "tsyssim.h"

// MACROS
#define MASK_RWUSR	0066


// -----------------------------------------------------------------------------
// CTestSyssim::getgrpid : To get group ID
// description : This is a test function for simulated "getgid" call. Concept of
// 				 user and groups are not supported by the platform unlike unix like
//               systems.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::getgrpid(  )
	{
    getgid();
    _LIT(Kerr , "getgrpid is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::setgrpid : To set group ID
// description : This is a test function for simulated "setgid" call. Concept of
// 				 user and groups are not supported by the platform unlike unix like
//               systems.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::setgrpid(  )
	{
    setgid((gid_t)0);
    _LIT(Kerr , "setgrpid is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::geteffgrpid : To get effective group ID
// description : This is a test function for simulated "getegid" call. Concept of
// 				 user and groups are not supported by the platform unlike unix like
//               systems.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::geteffgrpid(  )
	{
    getegid();
    _LIT(Kerr , "geteffgrpid is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::seteffgrpid : To set effective group ID
// description : This is a test function for simulated "setegid" call. Concept of
// 				 user and groups are not supported by the platform unlike unix like
//               systems.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::seteffgrpid(  )
	{
    setegid((gid_t)0);
    _LIT(Kerr , "seteffgrpid is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::setregrpid : To set real and effective group ID
// description : This is a test function for simulated "setregid" call. Concept of
// 				 user and groups are not supported by the platform unlike unix like
//               systems.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::setregrpid(  )
	{
    setregid((gid_t)0, (gid_t)0);
    _LIT(Kerr , "setregrpid is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::getusrid : To get user ID
// description : This is a test function for simulated "getuid" call. Concept of
// 				 user and groups are not supported by the platform unlike unix like
//               systems.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::getusrid(  )
	{
    getuid();
    _LIT(Kerr , "getusrid is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::setusrid : To set user ID
// description : This is a test function for simulated "setuid" call. Concept of
// 				 user and groups are not supported by the platform unlike unix like
//               systems.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::setusrid(  )
	{
    setuid((uid_t)0);
    _LIT(Kerr , "setusrid is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::geteffusrid : To get effective user ID
// description : This is a test function for simulated "geteuid" call. Concept of
// 				 user and groups are not supported by the platform unlike unix like
//               systems.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::geteffusrid(  )
	{
    geteuid();
    _LIT(Kerr , "geteffusrid is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::seteffusrid : To set effective user ID
// description : This is a test function for simulated "seteuid" call. Concept of
// 				 user and groups are not supported by the platform unlike unix like
//               systems.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::seteffusrid(  )
	{
    seteuid((uid_t)0);
    _LIT(Kerr , "seteffusrid is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::setreusrid : To set real and effective user ID
// description : This is a test function for simulated "setreuid" call. Concept of
// 				 user and groups are not supported by the platform unlike unix like
//               systems.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::setreusrid(  )
	{
    setreuid((uid_t)0, (uid_t)0);
    _LIT(Kerr , "setreusrid is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::getpgrpid : To get process group ID
// description : This is a test function for simulated "getpgid" call. Concept of
// 				 user and groups are not supported by the platform unlike unix like
//               systems.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::getpgrpid(  )
	{
    getpgid((pid_t)0);
    _LIT(Kerr , "getpgrpid is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::setpgrpid : To set process group ID
// description : This is a test function for simulated "setpgid" call. Concept of
// 				 user and groups are not supported by the platform unlike unix like
//               systems.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::setpgrpid(  )
	{
    setpgid((pid_t)0, (pid_t)0);
    _LIT(Kerr , "setpgrpid is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::getprocgrp : This is equivalent to getpgid(0)
// description : This is a test function for simulated "getpgrp" call. Concept of
// 				 user and groups are not supported by the platform unlike unix like
//               systems.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::getprocgrp(  )
	{
    getpgrp();
    _LIT(Kerr , "getprocgrp is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::setprocgrp : This is equivalent to setpgid(0,0)
// description : This is a test function for simulated "setpgrp" call. Concept of
// 				 user and groups are not supported by the platform unlike unix like
//               systems.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::setprocgrp(  )
	{
    setpgrp((pid_t)0, (pid_t)0);
    _LIT(Kerr , "setprocgrp is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::getgrps : To get supplementary group list.
// description : This is a test function for simulated "getgroups" call. Concept of
// 				 user and groups are not supported by the platform unlike unix like
//               systems.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::getgrps(  )
	{
	int size = 0;
	gid_t list[2];
	
    getgroups(size, list);
    _LIT(Kerr , "getgrps is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::setsessid : To create new session or process group.
// description : This is a test function for simulated "setsid" call. Concept of
// 				 process group or session are not supported by the platform unlike
//               unix like systems. 
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::setsessid(  )
	{		
    setsid();
    _LIT(Kerr , "setsessid is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::getpprocid : To get parent process ID.
// description : This is a test function for simulated "getppid" call. Ability to
// 				 obtain parent process ID are not supported by the platform unlike
//               unix like systems. 
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::getpprocid(  )
	{		
    getppid();
    _LIT(Kerr , "getpprocid is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::filemask : To set file's mode creation mask.
// description : This is a test function for simulated "umask" call. Ability to
// 				 obtain parent process ID are not supported by the platform unlike
//               unix like systems. 
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::filemask(  )
	{		
    umask((mode_t)MASK_RWUSR);
    _LIT(Kerr , "filemask is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::chowner : To change the file's owner and group.
// description : This is a test function for simulated "chown" call. Concept of
// 				 user and groups are not supported by the platform unlike unix like
//               systems.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::chowner(  )
	{		
	char *file = "C:\\MyFile.txt";
    chown(file, 0, 0);
    _LIT(Kerr , "chowner is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::lchowner : To change a symbolic link file's owner and group.
// description : This is a test function for simulated "lchown" call. Concept of
// 				 user and groups are not supported by the platform unlike unix like
//               systems.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::lchowner(  )
	{		
	char *file = "C:\\MyFile.txt";
    lchown(file, 0, 0);
    _LIT(Kerr , "chowner is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::initgrps : Initialize supplementry group list.
// description : This is a test function for simulated "initgroups" call. Concept of
// 				 user and groups are not supported by the platform unlike unix like
//               systems.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::initgrps(  )
	{		
	char list[2];
    initgroups(list, 0);
    _LIT(Kerr , "initgrps is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::schedyield : yield control to a similar or higher priority thread.
// description : This is a test function for simulated "sched_yield" call. Concept of
// 				 thread/process yield are not supported by the platform unlike unix like
//               systems.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::schedyield(  )
	{			
    sched_yield();
    _LIT(Kerr , "schedyield is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::getproioritymax: yield control to a similar or higher priority thread.
// description : This is a test function for simulated "sched_get_priority_max" call.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::getproioritymax(  )
	{			
    sched_get_priority_max(1);
    _LIT(Kerr , "getproioritymax is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestSyssim::getproioritymin: yield control to a similar or higher priority thread.
// description : This is a test function for simulated "sched_get_priority_min" call.
// Returns: KErrNone: Always
// -----------------------------------------------------------------------------
//
TInt CTestSyssim::getproioritymin(  )
	{			
    sched_get_priority_min(1);
    _LIT(Kerr , "getproioritymin is simulated and not supported by plf");
    WARN_PRINTF1(Kerr);
    return KErrNone;
	}

//  End of File
