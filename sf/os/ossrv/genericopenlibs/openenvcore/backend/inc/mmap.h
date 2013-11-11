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
* Name        : mmap.h
* Part of     : backend
* Internal header for mmap implementation
*
*/


 
#ifndef	_MMAP_H_
#define	_MMAP_H_

#include <sys/types.h>
#include "reent.h"

struct MappedMemory
{
	int fd;
 	off_t offset;
 	size_t len;
 	int flags;
 	int memid;
 	void* addr;
 	void* fdescaddr; 
 	size_t maplen;	 	
};
 
// Function prototypes
__BEGIN_DECLS

IMPORT_C void* _mmap_r(struct _reent *r, size_t len, int prot, int flags, int fildes, off_t offset);

IMPORT_C int	_mprotect_r(struct _reent *,const void *, size_t, int);

IMPORT_C int	_msync_r(struct _reent *,void *, size_t, int);

IMPORT_C int	_munlock_r(struct _reent *,const void *, size_t);

IMPORT_C int	_munmap_r(struct _reent *,void *, size_t);

__END_DECLS

#endif /* !_MMAP_H_ */
