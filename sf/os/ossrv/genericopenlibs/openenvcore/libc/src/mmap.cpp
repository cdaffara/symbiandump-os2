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

#include <sys/mman.h>
#include "mmap.h"

extern "C" 
{

// -----------------------------------------------------------------------------
// Funcation name: mmap
// Description: Provides mmap functionality.
// Returns:  valid address : On success
//           -1    		   : On error
// In case of error, errno value set
//      
// -----------------------------------------------------------------------------

EXPORT_C void* mmap(void */*addr*/, size_t len, int prot, int flags, int fildes, off_t offset)
	{
	//TODO;To validate addr for page boundry and sharedmemory
	return _mmap_r(_REENT, len, prot, flags, fildes, offset);
	}

// -----------------------------------------------------------------------------
// Funcation name: munmap
// Description: Provides munmap functionality.
// Returns:   0 : On success
//           -1 : On error
// In case of error, errno value set
// Remark: support for unmapping part of the mapped pages is not supported
//         Unmap will remove all mappings those were mapped starting from the offset
//         to offset + len (refer to mmap signature)  
// -----------------------------------------------------------------------------

EXPORT_C int munmap(void *addr, size_t len)
	{
	//TODO;To validate addr for page boundry and sharedmemory		
	return _munmap_r(_REENT, addr, len);				
	}

// -----------------------------------------------------------------------------
// Funcation name: msync
// Description: Provides msync functionality.
// Returns:   0 : On success
//           -1 : On error
// In case of error, errno value set
//      
// -----------------------------------------------------------------------------

EXPORT_C int msync(void *addr, size_t len, int flags)
	{	
	return _msync_r(_REENT, addr, len, flags);
	}

// -----------------------------------------------------------------------------
// Funcation name: mprotect
// Description: Provides msync functionality.
// Returns:   0 : On success
//           -1 : On error
// In case of error, errno value set
// Remark: mprotect is only build enabled and not functional     
// -----------------------------------------------------------------------------

EXPORT_C int mprotect(const void *addr, size_t len, int prot)
	{	
	return _mprotect_r(_REENT, addr, len, prot);
	}

    
} // extern "C"

//  End of File
