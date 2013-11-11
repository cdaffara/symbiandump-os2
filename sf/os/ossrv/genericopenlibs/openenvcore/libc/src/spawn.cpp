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
//

#include <e32std.h>
#include "spawn_r.h"
#include <spawn.h>
#include <sys/syslimits.h>
#include <errno.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include "sysreent.h"

extern "C" {

// -----------------------------------------------------------------------------
// posix_spawn_file_actions_init
// Initialize the file actions structure
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawn_file_actions_init(posix_spawn_file_actions_t* file_actions)
	{
	file_actions->_fa = new file_actions_t;
	if (!file_actions->_fa)
		{
		return ENOMEM;
		}
	return 0;
	}

// -----------------------------------------------------------------------------
// posix_spawn_file_actions_addopen
// Add an open action to the file actions structure
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawn_file_actions_addopen(
				posix_spawn_file_actions_t* file_actions, 
				int fid,
				const char* path, 
				int oflag, 
				mode_t mode)
	{
	if (fid < 0 || fid > OPEN_MAX)
		{
		return EBADF;
		}
		
	TFileAction* fa = new TFileAction;
	if (!fa)
		{
		return ENOMEM;
		}
	
	memset(fa, 0, sizeof(TFileAction));	
	fa->iOp = EOpen;
	fa->iFid1 = fid;
	fa->iOFlag = oflag;
	fa->iMode = mode;
	
	int len = strlen(path)+1;
	wchar_t* wpath = new wchar_t[len];
	if (!wpath)
		{
		delete fa;
		return ENOMEM;
		}
		
	if (mbstowcs(wpath, path, len) == (size_t)-1)
		{
		delete fa;
		delete[] wpath;
		return EILSEQ;
		}
	
	fa->iPath = new TFileName;
	fa->iPath->Copy((TText16*)wpath, len);
	delete[] wpath;
	
	(file_actions->_fa->iActions).AddLast(*fa);
	if (file_actions->_fa->iIter == NULL)
		{
		(file_actions->_fa->iIter).SetToFirst();
		}
	
	return 0;
	}
	
// -----------------------------------------------------------------------------
// posix_spawn_file_actions_adddup2
// Add a dup2 action to the file actions structure
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawn_file_actions_adddup2(
				posix_spawn_file_actions_t* file_actions, 
				int fid1, 
				int fid2)
	{
	if (fid1 < 0 || fid2 < 0 || fid1 > OPEN_MAX || fid2 > OPEN_MAX)
		{
		return EBADF;
		}
		
	TFileAction* fa = new TFileAction;
	if (!fa)
		{
		return ENOMEM;
		}

	memset(fa, 0, sizeof(TFileAction));		
	fa->iOp = EDup;
	fa->iFid1 = fid1;
	fa->iFid2 = fid2;
	
	(file_actions->_fa->iActions).AddLast(*fa);
	if (file_actions->_fa->iIter == NULL)
		{
		(file_actions->_fa->iIter).SetToFirst();
		}
		
	return 0;
	}

// -----------------------------------------------------------------------------
// posix_spawn_file_actions_addclose
// Add a close action to the file actions structure
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawn_file_actions_addclose(
				posix_spawn_file_actions_t* file_actions, 
				int fid)
	{
	if (fid < 0 || fid > OPEN_MAX)
		{
		return EBADF;
		}
		
	TFileAction* fa = new TFileAction;
	if (!fa)
		{
		return ENOMEM;
		}
	
	memset(fa, 0, sizeof(TFileAction));	
	fa->iOp = EClose;
	fa->iFid1 = fid;
	(file_actions->_fa->iActions).AddLast(*fa);
	if (file_actions->_fa->iIter == NULL)
		{
		(file_actions->_fa->iIter).SetToFirst();
		}
		
	return 0;
	}

// -----------------------------------------------------------------------------
// posix_spawn_file_actions_destroy
// Empty and destroy the file actions structure
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t* file_actions)
	{
	if (!file_actions || !file_actions->_fa)
		{
		return EINVAL;
		}
		
	if (file_actions->_fa->iActions.IsEmpty())
		{
		delete file_actions->_fa;
		return 0;
		}
	
	TFileAction* fa = (file_actions->_fa->iIter)++;
	while (fa)
		{
		if (fa->iPath)
			{
			delete fa->iPath;
			}
			
		delete fa;
		fa = (file_actions->_fa->iIter)++;
		}
		
	file_actions->_fa->iActions.Reset();
	delete file_actions->_fa;
	return 0;
	}

// -----------------------------------------------------------------------------
// posix_spawnattr_init
// Initialize the spawn attributes structure
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawnattr_init(posix_spawnattr_t* attrp)
	{
	// we don't support these flags. simply set all attribs to 0.
	if (!attrp)
		{
		return EINVAL;
		}
	memset(attrp, 0, sizeof(posix_spawnattr_t));
	return 0;
	}

// -----------------------------------------------------------------------------
// posix_spawnattr_getsigdefault
// Returns the sigdefault attribute
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawnattr_getsigdefault(
				const posix_spawnattr_t* attrp, 
				sigset_t* sigdefault)
	{
	if (!attrp || !sigdefault)
		{
		return EINVAL;
		}
	
	*sigdefault = attrp->_sd;
	return 0;
	}

// -----------------------------------------------------------------------------
// posix_spawnattr_getflags
// Return the flags attribute
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawnattr_getflags(
				const posix_spawnattr_t* attrp,
				short* flags)
	{
	if (!attrp || !flags)
		{
		return EINVAL;
		}
	
	*flags = attrp->_flags;
	return 0;
	}

// -----------------------------------------------------------------------------
// posix_spawnattr_getpgroup
// Return the process group attribute
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawnattr_getpgroup(
				const posix_spawnattr_t* attrp, 
				pid_t* pgroup)
	{
	if (!attrp || !pgroup)
		{
		return EINVAL;
		}
	
	*pgroup = attrp->_pgrp;
	return 0;
	}

// -----------------------------------------------------------------------------
// posix_spawnattr_getschedparam
// Return scheduling parameters attribute
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawnattr_getschedparam(
				const posix_spawnattr_t* attrp,
          		struct sched_param* schedparam)
	{
	if (!attrp || !schedparam)
		{
		return EINVAL;
		}
	
	*schedparam = attrp->_sp;
	return 0;
	}

// -----------------------------------------------------------------------------
// posix_spawnattr_getschedpolicy
// Return the scheduling policy attribute
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawnattr_getschedpolicy(
				const posix_spawnattr_t* attrp, 
				int* policy)
	{
	if (!attrp || !policy)
		{
		return EINVAL;
		}
	
	*policy = attrp->_policy;
	return 0;
	}

// -----------------------------------------------------------------------------
// posix_spawnattr_getsigmask
// Return the signal mask attribute
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawnattr_getsigmask(
				const posix_spawnattr_t* attrp, 
				sigset_t* sigmask)
	{
	if (!attrp || !sigmask)
		{
		return EINVAL;
		}
	
	*sigmask = attrp->_sm;
	return 0;
	}

// -----------------------------------------------------------------------------
// posix_spawnattr_setsigdefault
// Sets the sigdefault attribute
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawnattr_setsigdefault(
				posix_spawnattr_t* attrp,
				const sigset_t* sigdefault)
	{
	if (!attrp || !sigdefault)
		{
		return EINVAL;
		}
	
	attrp->_sd = *sigdefault;
	return 0;
	}

// -----------------------------------------------------------------------------
// posix_spawnattr_setflags
// Sets the flags attribute
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawnattr_setflags(posix_spawnattr_t* attrp, short flags)
	{
	if (!attrp)
		{
		return EINVAL;
		}
	
	attrp->_flags = flags;
	return 0;
	}

// -----------------------------------------------------------------------------
// posix_spawnattr_setpgroup
// Sets the process group attribute
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawnattr_setpgroup(posix_spawnattr_t* attrp, pid_t pgroup)
	{
	if (!attrp)
		{
		return EINVAL;
		}
	
	attrp->_pgrp = pgroup;
	return 0;
	}

// -----------------------------------------------------------------------------
// posix_spawnattr_setschedparam
// Sets the scheduling parameters attribute
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawnattr_setschedparam(
				posix_spawnattr_t* attrp,
				const struct sched_param* schedparam)
	{
	if (!attrp || !schedparam)
		{
		return EINVAL;
		}
	
	attrp->_sp = *schedparam;
	return 0;
	}

// -----------------------------------------------------------------------------
// posix_spawnattr_setschedpolicy
// Sets the scheduling policy attribute
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawnattr_setschedpolicy(posix_spawnattr_t* attrp, int policy)
	{
	if (!attrp)
		{
		return EINVAL;
		}
	
	attrp->_policy = policy;
	return 0;
	}

// -----------------------------------------------------------------------------
// posix_spawnattr_setsigdefault
// Sets the sigmask attribute
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawnattr_setsigmask(
				posix_spawnattr_t* attrp, 
				const sigset_t* sigmask)
	{
	if (!attrp || !sigmask)
		{
		return EINVAL;
		}
	
	attrp->_sm = *sigmask;
	return 0;
	}

// -----------------------------------------------------------------------------
// posix_spawnattr_setsigdefault
// Empty and cleanup the spawn attributes structure
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawnattr_destroy(posix_spawnattr_t* /*attrp*/)
	{
	// nothing to do
	return 0;
	}

// -----------------------------------------------------------------------------
// posix_spawn
// Launch a child process specified by path and obtain its pid
// This API allows the caller to specify command line arguments and envp for the child
// In addition, one can also specify a set of file operations that will be performed
// and a set of attributes that will be applied in the child before it enters its main.
// -----------------------------------------------------------------------------
EXPORT_C int posix_spawn(
				pid_t* pid, 
				const char* path,
				const posix_spawn_file_actions_t* file_actions,
				const posix_spawnattr_t* attrp, 
				char *const argv[],
				char *const envp[])
	{
	if(path == NULL || *path == '\0')
		{
		return ECHILD;
		}
		
	int len = strlen(path) + 1;
	wchar_t* wpath = new wchar_t[len];
	if (mbstowcs(wpath, path, len) == (size_t)-1)
		{
		delete[] wpath;
		return EILSEQ;
		}
	
	int ret = 0;
	
	wchar_t* wargs = NULL;
	wchar_t** wenvp = NULL;
	
	if (argv && argv[1])
		{
		TInt totlen = 0;
		// argv[0] is (or atleast should be) the exe name
		for (int i = 1; argv[i]; ++i)
			{
			totlen += strlen(argv[i]) + 1;
			}
			
		wargs = new wchar_t[totlen+1];
		
		if (!wargs)
			{
			ret = ENOMEM;
			goto bailout;
			}

		wchar_t* wp = wargs;
		// argv[0] is (or atleast should be) the exe name
		for (int i = 1; argv[i]; ++i)
			{
			int len = strlen(argv[i]);
			if (mbstowcs(wp, argv[i], len) == (size_t)-1)
				{
				ret = EILSEQ;
				goto bailout;
				}
			wp[len++] = L' ';
			wp += len;
			}
		
		// replace the last blank with a null character
		*(--wp) = 0;
		}
	
	if (envp)
		{
		TInt count = 0;
		for (; envp[count]; ++count) {}

		//coverity[alloc_fn]

		//coverity[assign]
	
		wenvp = new wchar_t*[count+1];
		if (!wenvp)
			{
			ret = ENOMEM;
			goto bailout;
			}
			
		for (int i = 0; i < count; ++i)
			{
			int len = strlen(envp[i]) + 1;
			wenvp[i] = new wchar_t[len];
			if (wenvp[i] == NULL)
				{
				ret = ENOMEM;
				goto bailout;
				}
			
			if (mbstowcs(wenvp[i], envp[i], len) == (size_t)-1)
				{
				ret = EILSEQ;
				wenvp[i+1] = NULL;
				goto bailout;
				}
			}
				
		wenvp[count] = 0;
		}

	//coverity[leave_without_push]

	ret = _posix_spawn_r(pid, wpath, file_actions, attrp, wargs, wenvp);

bailout:
	delete[] wpath;
	if (wargs)
		{
		delete[] wargs;
		}
		
	if (wenvp)
		{
		for (int i = 0; wenvp[i]; ++i)
			{
			delete wenvp[i];
			}
		delete wenvp;
		}

	return ret;
	}

}  //extern "C"
