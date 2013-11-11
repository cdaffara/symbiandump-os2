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
* Description:  Declaration of CMmapMemoryNode
*
*/



#ifndef MMAP_MEMORY_NODE_H
#define MMAP_MEMORY_NODE_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include "mmap.h"
#include "base.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CMmapMemoryNode;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/*
*  This class is responsible to managing mapped memory nodes.  
*/
NONSHARABLE_CLASS ( CMmapMemoryNode ): public BBase
    {
public:
    /*
    * constructor.
 	*/
    static CMmapMemoryNode * NewL(size_t len, int flag, int shmflg, \
    							  int fildes, off_t offset, int& anErrno, size_t maplen);
    
    /*
    * Default destructor.
    */
    ~CMmapMemoryNode();
    
    /*
    * Description:Give message length
    * return:Message length
    */
    size_t MapLength();
    
    /*
    * Description:Give message type
    * return: message type
    */
    off_t MapFileOffset();
    
    /*
    * Determine memory data member link offset for single queue
    */
    static TInt LinkOffset();
    
    /*
    * Get the file mapped memory address
    */
    TAny* MappedAddress();
    
    /*
    * Get the file mapped memory info
    */
    struct MappedMemory GetMemInfo();
    
    /*
    * Set the file descriptor base address
    */
    void SetFdescAddress(void* aAddr);
    
    /*
    * Sync mapped memory to file
    */
    int SyncMemory(int len, int flags, int& anErrno);
    
private:
    /*
    * Default constructor.
    */
    CMmapMemoryNode();        
    
    /*
    * 2nd phase constructor.
    */
    int ConstructL(size_t len, int flag, int shmflg, int fildes, \
    				off_t offset, int& anErrno, size_t maplen);
        
private: // data
    struct MappedMemory iMapMem;
    TSglQueLink iLink;
    };

#endif // MMAP_MEMORY_NODE_H

// End of File
