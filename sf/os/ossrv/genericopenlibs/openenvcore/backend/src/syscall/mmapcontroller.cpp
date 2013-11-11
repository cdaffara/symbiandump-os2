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
* Description:  Implementation of CMmapControllerNode
*
*/


//  INCLUDES
#include <sys/errno.h>
#include <sys/mman.h>
#include <liblogger.h>
#include "mmapcontrollernode.h"
#include "fdesc.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmapControllerNode::CMmapControllerNode
// C++ default constructor.
// -----------------------------------------------------------------------------
// 
CMmapControllerNode::CMmapControllerNode() :                       
            iMmapQueue(CMmapMemoryNode::LinkOffset()),
            iMmapQueueIter(iMmapQueue)
	{
	}
    
// -----------------------------------------------------------------------------
// CMmapControllerNode::~CMmapControllerNode
// C++ default destructor.
// -----------------------------------------------------------------------------
// 
CMmapControllerNode::~CMmapControllerNode()
	{     
	CMmapMemoryNode* lmemnode;
	iMmapQueueIter.SetToFirst();
	while ((lmemnode = iMmapQueueIter++) != NULL)
    	{    	
        iMmapQueue.Remove(*lmemnode);
        delete lmemnode;
    	};
    
	}

// -----------------------------------------------------------------------------
// CMmapControllerNode::NewL
// 1st phase constructor.
// -----------------------------------------------------------------------------
// 
CMmapControllerNode * CMmapControllerNode::NewL()
	{
    CMmapControllerNode* self = new (ELeave) CMmapControllerNode();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CMmapControllerNode::Construct
// 2nd phase constructor.
// -----------------------------------------------------------------------------
// 
void CMmapControllerNode::ConstructL()
	{
	}
    
// -----------------------------------------------------------------------------
// CMmapControllerNode::AddMmapNodeL
// Add a mapped memory node into controller list
// -----------------------------------------------------------------------------
// 

TAny* CMmapControllerNode::AddMmapNodeL(struct _reent *r, size_t len, int flag,	\
										int shmflg, int fildes, off_t offset, size_t maplen)
	{
	CMmapControllerNode *lmemcontroller;
	
     
    /*Check TLS for presence of a node controller if any, if not create one*/    
	if (!(r->_memaddr))
		{
		lmemcontroller = CMmapControllerNode::NewL();
		r->_memaddr = (void*)lmemcontroller;
		}
	else
		{
		lmemcontroller = (CMmapControllerNode*)r->_memaddr;
		}
	
	/* create a new memory mapped node */
    CMmapMemoryNode* lmemnode = CMmapMemoryNode::NewL(len, flag, shmflg, \
    												  fildes, offset, r->_errno, maplen);        
	if(((void *)-1) != lmemnode->MappedAddress())
		{
		lmemcontroller->iMmapQueue.AddLast(*lmemnode);
		//To mark file table entry to indicate mmap exists
		CFileDescBase *f = 0;		
		struct MappedMemory meminfo;
		        
        meminfo = lmemnode->GetMemInfo();		        
		f = Backend()->GetDesc(meminfo.fd);
		if(f)        
			{								
			f->SetAttributes((TUint32)KMMapedFd, (TBool)ETrue);
			lmemnode->SetFdescAddress((void*)f);
			}
		else
			{
			//Should we respond?
			}
		}    
             
	return lmemnode->MappedAddress();
	}

// -----------------------------------------------------------------------------
// CMmapControllerNode::DeleteMmapNode
// Delete a mapped memory node from controller list
// -----------------------------------------------------------------------------
// 
int CMmapControllerNode::DeleteMmapNode(TAny* aAddr, int& anErrno)
	{
	int retval = -1;
		
	if (!(iMmapQueue.IsEmpty()))
		{	
		CMmapMemoryNode* lmemnode;
		iMmapQueueIter.SetToFirst();
		/* Iterate through to find the matching memory node*/
		while ((lmemnode = iMmapQueueIter++) != NULL)
	    	{
			if(aAddr == lmemnode->MappedAddress())
				{		        
		        //To actually close the fd object if user has already called for a close fd
		        CFileDescBase *f=0;
		        struct MappedMemory lmeminfo;
		        
		        lmeminfo = lmemnode->GetMemInfo();
		        f = (CFileDescBase *)lmeminfo.fdescaddr;
				if(f)
					{									
		        	TUint32 fileatt = f->Attributes();
		        	f->SetAttributes((TUint32)KMMapedFd, (TBool)EFalse);
		        	if(fileatt & KInvalidFd)
		        		{
		        		//We flush the changes back to file, before closing it
		        		if(MAP_SHARED == lmeminfo.flags)
							{						
							retval = lmemnode->SyncMemory(lmeminfo.maplen, MS_SYNC, anErrno);
							}
		        		f->Close();
		        		}
		        	else
		        		{
		        		//We flush the changes back to file
		        		if(MAP_SHARED == lmeminfo.flags)
							{						
							retval = lmemnode->SyncMemory(lmeminfo.maplen, MS_SYNC, anErrno);
							}
		        		}
					}
				else
				{
					//Should we respond?
				}
				iMmapQueue.Remove(*lmemnode);
		        delete lmemnode;
		        retval = 0;
		        break;
				}	    	
	    	}
	    if(-1 == retval)
	    	{
	    	anErrno = EINVAL;
	    	}
		}
	else
		{
		anErrno = EINVAL;		
		}
	
	return retval;	
	}        

// -----------------------------------------------------------------------------
// CMmapControllerNode::SyncMmapNode
// Sync a mapped memory node to its associated resource file
// -----------------------------------------------------------------------------
// 

int CMmapControllerNode::SyncMmapNode(TAny* aAddr, size_t len, int flags, int& anerror)
	{
	int retval = -1;
	struct MappedMemory lmeminfo;
	bool lnodeexist = FALSE;
	
	
	if (!(iMmapQueue.IsEmpty()))
		{	
		CMmapMemoryNode* lmemnode;
		iMmapQueueIter.SetToFirst();
		while ((lmemnode = iMmapQueueIter++) != NULL)
	    	{
			if(aAddr == lmemnode->MappedAddress())
				{
				//Only if the memory is shared do we want it to be
				//made visible across processes
				lnodeexist = TRUE;
				lmeminfo = lmemnode->GetMemInfo();
				if(MAP_SHARED == lmeminfo.flags)
					{
					if(lmeminfo.maplen >= len)
						{
						retval = lmemnode->SyncMemory(len, flags, anerror);
						}
					else
						{
						anerror = ENOMEM;						
						}
					}
				else
					{
					if((MS_ASYNC == flags) || (MS_SYNC == flags) || \
					   (MS_INVALIDATE == flags))
						{
						/*we just don't respond if memory was mapped to anthing 
					  	other than MAP_SHARED */
						retval = 0;
						}
					else
						{
						anerror = EINVAL;
						retval = -1;
						}
					}
				break;
				}						
	    	}
	    /*We are flagging a EINVAL error saying the aAddr could be mapped but not page aligned
	      For all reasons it could be invalid address, but we are looking for page alignment basically*/
	    if(!lnodeexist)
			{
			anerror = EINVAL;
			}  
	      
		}
	if(!lnodeexist)
		{
		anerror = ENOMEM;
		}
	return retval;
	}

// -----------------------------------------------------------------------------
// CMmapControllerNode::ChprotMmapNode
// Change the protection of a mapped memory. This is currently only build
// supported and not functionally supported
// -----------------------------------------------------------------------------
// 
int CMmapControllerNode::ChprotMmapNode(const TAny* aAddr, size_t /*len*/, int /*prot*/, int& anErrno)
	{
	int retval = -1;
	
	
	if (!(iMmapQueue.IsEmpty()))
		{	
		CMmapMemoryNode* lmemnode;
		iMmapQueueIter.SetToFirst();
		while ((lmemnode = iMmapQueueIter++) != NULL)
	    	{
			if(aAddr == lmemnode->MappedAddress())
				{		      
				/*Currently we do not support protection modification*/
				anErrno = ENOSYS;		        
		        break;
				}	    	
	    	}
	    if(-1 == retval)
	    	{
	    	anErrno = EINVAL;
	    	}
		}
	else
		{
		anErrno = EINVAL;		
		}
		
	return retval;	
	}    
// End of File
