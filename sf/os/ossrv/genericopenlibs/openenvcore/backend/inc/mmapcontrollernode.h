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
* Description:  Declaration of CMmapControllerNode
*
*/



#ifndef MMAP_CONTROLLER_NODE_H
#define MMAP_CONTROLLER_NODE_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include "mmapmemorynode.h"
#include "base.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CMmapMemoryNode;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/*
*  This class is responsible to managing the memory mapped nodes.
*  This will maintain the mapped memory in a list format.
*  @since MRT 2.0
*/
NONSHARABLE_CLASS ( CMmapControllerNode ): public BBase
    {
public:
    /*
    * 1st phase constructor. 
    */
    static CMmapControllerNode * NewL();
    
    /*
    * Default destructor.
    */
    ~CMmapControllerNode();
    
    /*
    * To create a mapped memory and add in mmap node list
    */
    
    static TAny* AddMmapNodeL(struct _reent *r, size_t len, int flag, \
					   int shmflg, int fildes, off_t offset, size_t maplen);
	
	/*
    * To unmap a mapped memory and delete from mmap node list
    */
					   
    int DeleteMmapNode(TAny* aAddr, int& anerror);
    
    /*
    * To sync a mapped memory into the file below
    */
					   
    int SyncMmapNode(TAny* aAddr, size_t len, int flags, int& anerror);
    
    /*
    * To modify protection mode of a mapped memory.
    */
    
    int ChprotMmapNode(const TAny* addr, size_t len, int prot, int& anerrno);
    
    /*
    * To delete a mapped memory from node list.
    */
    
    static void RemoveControllerL(void *p);
    
private:
    /*
    * Default constructor.
    */
    CMmapControllerNode();
    
    /*
    * 2nd phase constructor.    
    */
    void ConstructL();

private: // data    
    TSglQue<CMmapMemoryNode> iMmapQueue;
    TSglQueIter<CMmapMemoryNode> iMmapQueueIter;
    };

#endif // MMAP_CONTROLLER_NODE_H

// End of File
