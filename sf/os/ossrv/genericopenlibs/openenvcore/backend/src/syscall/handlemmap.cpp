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
* Description:  Implementation of mmap/munmap/msync.
*
*/


// INCLUDE FILES

#include <e32hal.h>
#include <sys/errno.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include "mmapcontrollernode.h"
#include "sysreent.h"
#include "lposix.h"
#include "mmap.h"

__BEGIN_DECLS

static int ComputeMemFlag(int prot);

// -----------------------------------------------------------------------------
// Funcation name: _mmap_r
// Description: Provides mmap functionality.
// Returns:  valid address : On success
//           -1    		   : On error
// In case of error, errno value set
// -----------------------------------------------------------------------------

EXPORT_C TAny* _mmap_r(struct _reent *r, size_t len, int prot, int flags, int fildes, off_t offset)
{	
	TInt pageSize = -1;	
	UserHal::PageSizeInBytes(pageSize);	
	size_t lpagesize = pageSize;/*getpagesize();*/
	int lshmflg;
	void* lmapaddr=NULL;
	int lfileflg;
	size_t maplen;
	
	if((0 == len) || (!(flags & MAP_PRIVATE) && !(flags & MAP_SHARED)))
	{
		r->_errno = EINVAL;
		return MAP_FAILED;
	}	
	else
	{
		/*Align len to page boundry*/
		size_t ltemp;
		maplen = len;
		ltemp = (len / lpagesize);
		if(len % lpagesize)
		{			
			len = (ltemp * lpagesize) + lpagesize;
		}		 	
	}
	/* validate fd */
	long anArg = 0L;
	int aErrno = 0;
	lfileflg = _fcntl_r(&aErrno, fildes, F_GETFL, anArg); 
	if((-1 ==  lfileflg) || (-1 == fildes))	
	{
		r->_errno = EBADF;
		return MAP_FAILED;
	}
	/* offset should be page aligned */	
	if(offset % lpagesize)
	{			
		r->_errno = EINVAL;
		return MAP_FAILED;
	}
	/* MAP_FIXED is not supported*/		
	if(!(MAP_FIXED & flags))	
	{	
		if(((O_RDONLY == lfileflg) && (PROT_WRITE & prot) && (MAP_SHARED & flags)) \
		 || (O_WRONLY == lfileflg))
		{
			r->_errno = EACCES;
			return MAP_FAILED;	
		}
		lshmflg = ComputeMemFlag(prot);		
		if(lshmflg)
		{
			/* check for any leave */	
			TRAPD(err, lmapaddr = CMmapControllerNode::AddMmapNodeL(r, len, flags, lshmflg, fildes, offset, maplen));
	            	if(err != KErrNone)
	            	{
	                	MapError(err, r->_errno);
	                	return MAP_FAILED;
	            	}
			if(((void *)-1) == lmapaddr)
			{					
				return MAP_FAILED;	
			}
			else
			{				
				return lmapaddr;
			}
		}
		else
		{				
			r->_errno = ENOTSUP;
			return MAP_FAILED;
		}
	}
	else
	{
		r->_errno = ENOTSUP;
		return MAP_FAILED;
	}
		        
}


// -----------------------------------------------------------------------------
// Funcation name: _munmap_r
// Description: Provides munmap functionality.
// Returns:   0 : On success
//           -1 : On error
// In case of error, errno value set
// Remark: support for unmapping part of the mapped pages is not supported
//         Unmap will remove all mappings those were mapped starting from the offset
//         to offset + len (refer to mmap signature)  
// -----------------------------------------------------------------------------

EXPORT_C int _munmap_r(struct _reent *r, void *addr, size_t len)
{
	int retval = -1;
	if(0 != len)
	{
		CMmapControllerNode* lmemcontrol = (CMmapControllerNode*)r->_memaddr;
		if(lmemcontrol)
		{
			retval = lmemcontrol->DeleteMmapNode(addr, r->_errno);
		}		
	}	
	else
	{
		r->_errno = EINVAL;
		retval = -1;
	}	
	return retval;
}



// -----------------------------------------------------------------------------
// Funcation name: _msync_r
// Description: Provides msync functionality.
// Returns:   0 : On success
//           -1 : On error
// In case of error, errno value set
// -----------------------------------------------------------------------------

EXPORT_C int _msync_r(struct _reent *r, void *addr, size_t len, int flags)
{
	int retval = -1;
	CMmapControllerNode* lmemcontrol = (CMmapControllerNode*)r->_memaddr;	
	
	if(lmemcontrol)
	{
		retval = lmemcontrol->SyncMmapNode(addr, len, flags, r->_errno);
	}
	else
	{
		r->_errno = ENOMEM;
		retval = -1;
	}
	return retval;
	
}


// -----------------------------------------------------------------------------
// Funcation name: _mprotect_r
// Description: Provides msync functionality.
// Returns:   0 : On success
//           -1 : On error
// In case of error, errno value set
// Remark: mprotect is only build enabled and not functional     
// -----------------------------------------------------------------------------

EXPORT_C int _mprotect_r(struct _reent *r, const void *addr, size_t len, int prot)
{
	int retval = -1;
	CMmapControllerNode* lmemcontrol = (CMmapControllerNode*)r->_memaddr;	
	
	if(lmemcontrol)
	{
		retval = lmemcontrol->ChprotMmapNode(addr, len, prot, r->_errno);
	}	
	else
	{
		r->_errno = EINVAL;
		retval = -1;
	}
	return retval;
}


/*------------------------------------------------------------------------------
 *                 LOCAL FUNCTIONS
 *------------------------------------------------------------------------------
 */
 
static int ComputeMemFlag(int prot)
{
	int lshmflg = 0;
	switch(prot)
	{
	/* A read/write/exec here would mean read/write/exec for owner alone, 
	 * as "group and others" concept are not supported by the platform
	 * exec-only will not supported since memory execution is not 
	 * supported by the platform. Everything else is translated to read/write
	 * since shared memory (used internally by mmap) supports only read/write 
	 * mode.
	 */ 
	case PROT_READ:
	case PROT_WRITE:
	case PROT_WRITE | PROT_READ:
	case PROT_READ  | PROT_EXEC:
	case PROT_WRITE | PROT_EXEC:
	case PROT_WRITE | PROT_READ | PROT_EXEC:
		lshmflg = lshmflg | S_IRWXU | IPC_CREAT | IPC_EXCL;
		break;				
	case PROT_NONE:
	case PROT_EXEC:
	default:		
		break;
	}
	return lshmflg;
}

    
__END_DECLS
//  End of File

