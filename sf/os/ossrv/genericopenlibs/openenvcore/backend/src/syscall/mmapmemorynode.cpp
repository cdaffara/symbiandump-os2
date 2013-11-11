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
* Description:  Implementation of CMmapMemoryNode
*
*/



//  INCLUDES
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <liblogger.h>
#include "mmapmemorynode.h"
#include "sysreent.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmapMemoryNode::CMmapMemoryNode
// C++ default constructor.
// -----------------------------------------------------------------------------
// 
CMmapMemoryNode::CMmapMemoryNode() 		
	{   
	iMapMem.fd = -1;
	iMapMem.offset = 0;
    iMapMem.len = 0;
    iMapMem.flags = 0;
    iMapMem.memid = 0;
    iMapMem.addr = (NULL); 
    iMapMem.fdescaddr = (NULL) ; 
    iMapMem.maplen = 0; 
    }
    
// -----------------------------------------------------------------------------
// CMmapMemoryNode::~CMmapMemoryNode
// C++ default destructor.
// -----------------------------------------------------------------------------
// 
CMmapMemoryNode::~CMmapMemoryNode()
	{
    shmid_ds buf = 
    	{
    	{ 0,0,0,0,0,0,0	},
    	0,0,0,0,0,0,0,0	
    	};
    // call shmdt and shmctl  
	int aErrno = 0;
	if(!_shmdt_r(&aErrno, iMapMem.addr))
		{
		if(!_shmctl_r(&aErrno,iMapMem.memid, IPC_RMID, &buf))
			{
			/*done*/
			}
		}
	}
    
// -----------------------------------------------------------------------------
// CMmapMemoryNode::NewL
// 1st phase Constructor
// -----------------------------------------------------------------------------
// 
CMmapMemoryNode* CMmapMemoryNode::NewL(size_t len, int flag, int shmflg, \
									   int fildes, off_t offset, int& anErrno, size_t maplen)
	{
    CMmapMemoryNode* self = new (ELeave) CMmapMemoryNode;
    CleanupStack::PushL(self);
    if(-1 == self->ConstructL(len, flag, shmflg, fildes, offset, anErrno, maplen))
    	{
    	CleanupStack::Pop(self);
    	self->iMapMem.addr = ((void *)-1);
    	return self;
    	}
    CleanupStack::Pop(self);      
    return self;
	}
    
// -----------------------------------------------------------------------------
// CMmapMemoryNode::NewL
// 2nd phase constructor
// -----------------------------------------------------------------------------
// 
int CMmapMemoryNode::ConstructL(size_t len, int flag, int shmflg, int fildes, \
								 off_t offset, int& anErrno, size_t maplen)
{	
	int lshmid;	
	void* lmapaddr;	
	int readlen;
	int retval = -1;
	off_t cache;
	
    TInt aErrno = 0;
    /* Using shared memory to allocate memory for required length */
	if((lshmid = _shmget_r(&aErrno,IPC_PRIVATE, len, shmflg)) > 0)	
		{	
		lmapaddr = _shmat_r(&aErrno,lshmid, (void*)NULL, shmflg);
		if((KErrNone == aErrno) && (-1 != (int)lmapaddr))
			{
			/*cache the current file position before seeking to new position*/
			cache = _lseek_r(&aErrno, fildes, 0, SEEK_CUR);
			/*Seek in the file to required offset */
			if(offset == _lseek_r(&aErrno,fildes, offset, SEEK_SET))
				{
				//Zero fill the memory
				memset(lmapaddr, 0, len);
				/*Read the contents of the file to memory */
			readlen = _read_r(&aErrno,fildes, (char*)lmapaddr, maplen);
				//TODO: To take care if EOF is hit while reading, i.e readlen == 0
				if(readlen <= len)
					{
					iMapMem.fd = fildes;
					iMapMem.offset = offset;
					iMapMem.len = len;
					iMapMem.flags = flag;
					iMapMem.memid = lshmid;
					iMapMem.addr = lmapaddr;
					iMapMem.maplen = maplen;
					retval = 0;
					}
				else if(-1 == readlen)
					{
					anErrno = ENXIO;
					retval = -1;
					}
				/*Now seek back to the original position*/
				_lseek_r(&aErrno, fildes, cache, SEEK_SET);
				}
			else
				{
				anErrno = ENXIO;
				retval = -1;
				}
			}
		else
			{
			anErrno = EMFILE;
			retval = -1;
			}		
		if(-1 == retval)
  			{
			/*Remove the newly created shared memory*/
			Backend()->iIpcS.shmctl(lshmid, IPC_RMID, NULL,aErrno);
			}
		}
	else
		{
		anErrno = EMFILE;
		retval = -1;
		}            
		                
    return retval;            
	}

#ifdef __SYMBIAN_COMPILE_UNUSED__
// -----------------------------------------------------------------------------
// CMmapMemoryNode::MapLength
// Observer for length
// -----------------------------------------------------------------------------
// 
size_t CMmapMemoryNode::MapLength()
	{
    return iMapMem.len;
	}
    
// -----------------------------------------------------------------------------
// CMmapMemoryNode::MapFileOffset
// Observer for file offset
// -----------------------------------------------------------------------------
// 
off_t CMmapMemoryNode::MapFileOffset()
	{
    return iMapMem.offset;
	}
#endif    
    
// -----------------------------------------------------------------------------
// CMmapMemoryNode::LinkOffset
// Determine message data member link offset for single queue
// -----------------------------------------------------------------------------
// 
TInt CMmapMemoryNode::LinkOffset()
	{
    return _FOFF(CMmapMemoryNode, iLink);
	}

// -----------------------------------------------------------------------------
// CMmapMemoryNode::MappedAddress
// return pointer to mapped memory.
// -----------------------------------------------------------------------------
// 
TAny* CMmapMemoryNode::MappedAddress()
	{
    return iMapMem.addr;
	}

// -----------------------------------------------------------------------------
// CMmapMemoryNode::GetMemInfo
// return mapped memory info i.e struct MappedMemory.
// -----------------------------------------------------------------------------
// 
struct MappedMemory CMmapMemoryNode::GetMemInfo()
	{
    return iMapMem;
	}

// -----------------------------------------------------------------------------
// CMmapMemoryNode::SetFdescAddress
// store fdescbase address.
// -----------------------------------------------------------------------------
// 
void CMmapMemoryNode::SetFdescAddress(void* aAddr)
	{
    iMapMem.fdescaddr = aAddr;
	}

// -----------------------------------------------------------------------------
// CMmapMemoryNode::SyncMemory
// Sync the mapped memory into the underlying file.
// -----------------------------------------------------------------------------
//

int CMmapMemoryNode::SyncMemory(int len, int syncflag, int& anErrno)
	{
	int retval = -1;
	int flags;
	off_t cache;
	int aErrno = 0;
	long anArg = 0L;
    flags = _fcntl_r(&aErrno, iMapMem.fd, F_GETFL, anArg);   
    if(MS_INVALIDATE == syncflag)
    	{
    	/* Make sure the file is open atleast in read-only mode */
    	if(-1 != flags && (O_RDWR == (flags & O_ACCMODE) || O_RDONLY == (flags & O_ACCMODE)))
    		{
    		/*cache the current location before we seek to a new position*/
    		cache = _lseek_r(&aErrno, iMapMem.fd, 0, SEEK_CUR);
    		if(iMapMem.offset == _lseek_r(&aErrno,iMapMem.fd, iMapMem.offset, SEEK_SET))
				{
				int readlen = _read_r(&aErrno,iMapMem.fd, (char*)iMapMem.addr, len);
				if(-1 == readlen)
					{
					anErrno = aErrno;
					}
				else if(len >= readlen)
					{
					retval = 0;
					}			
				/*Now seek back to the original position*/
				_lseek_r(&aErrno, iMapMem.fd, cache, SEEK_SET);
				}
			else
				{
				anErrno = ENOMEM;
				}
    		
    		}
    	}
	else if(-1 != flags && (O_RDWR == (flags & O_ACCMODE) || O_WRONLY == (flags & O_ACCMODE)))
		{
		/*cache the current location before we seek to a new position*/
    	cache = _lseek_r(&aErrno, iMapMem.fd, 0, SEEK_CUR);
    		if(iMapMem.offset == _lseek_r(&aErrno,iMapMem.fd, iMapMem.offset, SEEK_SET))	
			{
			//sync/async capability, both are treated the same way
			if((MS_ASYNC == syncflag) || (MS_SYNC == syncflag))
				{							
				int writelen = _write_r(&aErrno,iMapMem.fd, (char*)iMapMem.addr, len);
				if(-1 == writelen)
					{
					anErrno = aErrno;
					}
				else if(len >= writelen)
					{
					retval = 0;
					}
				}
			else
				{
				anErrno = EINVAL;
				}
			/*Now seek back to the original position*/
			_lseek_r(&aErrno, iMapMem.fd, cache, SEEK_SET);
			}
		else
			{
			anErrno = ENOMEM;
			}
		}
	else
		{
		anErrno = ENOMEM;
		}
            
	return retval;            
	}
    
// End of File
