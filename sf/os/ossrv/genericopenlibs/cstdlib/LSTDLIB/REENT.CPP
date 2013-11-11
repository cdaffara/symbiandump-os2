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
// FUNCTION
// <<reent>>---definition of impure data.
// INDEX
// reent
// This module defines the impure data area used by the
// non-rentrant functions, such as strtok.
// 
//

#include "SYSIF.H"
#include <sys/reent.h>
#include <stdlib.h>

/* Interim cleanup code */

static void cleanup_glue (struct _glue *glue)
	{
	/* Have to reclaim these in reverse order: */
	if (glue->_next)
		cleanup_glue (glue->_next);
	free (glue);
	}

extern "C" {

/**
The struct _reent is managed on a per-thread basis by EPOC32, so there is no
global variable _impure_pointer and everyone has to use _REENT (panics in the
event of an error) or REENT2 (which returns a NULL pointer in the event of an
error).

@return
@param ptr
*/
EXPORT_C void
_reclaim_reent (struct _reent *ptr)
	{
	/* atexit stuff */
	if ((ptr->_atexit) && (ptr->_atexit != &ptr->_atexit0))
		{
		struct _atexit *p, *q;
		for (p = ptr->_atexit; p != &ptr->_atexit0;)
			{
			q = p;
			p = p->_next;
			free(q);
			}
		}

	if (ptr->environ)
		free(ptr->environ);

	if (ptr->_netdb)
		free(ptr->_netdb);

	if (ptr->__sdidinit)
		{
		/* cleanup won't reclaim memory 'coz usually it's run
		before the program exits, and who wants to wait for that? */
		ptr->__cleanup (ptr);
		if (ptr->__sglue._next)
			cleanup_glue (ptr->__sglue._next);
		}

	// narrow environment buffer
	if (ptr->_pNarrowEnvBuffer)
		{
		free(ptr->_pNarrowEnvBuffer);
		ptr->_pNarrowEnvBuffer = 0;
		}

	MSystemInterface& sysIf=Interface(ptr);
	sysIf.Release();

	ptr->_system=0;
	}

EXPORT_C void _REENT_INIT(struct _reent *ptr)
	{
	MSystemInterface& sysIf=Interface(ImpurePtr());
	_init_reent(ptr,&sysIf.Clone());
	}

extern "C" void __sinit(struct _reent*);	// LSTDIO/LOCAL.H

EXPORT_C void _init_reent(struct _reent *ptr, void* _system)
	{
	ptr->_system=_system;
	__sinit(ptr);
	ptr->_next[0]=1;
	ptr->_next[1]=1;

	ptr->_pNarrowEnvBuffer = 0;
	}

} // extern "C"
