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
// 
//

#include <e32std.h>
#include <stdlib.h> //size_t
#include <errno.h>

// We use the EPOC32 memory allocation which is already
// thread-safe, so we don't need the _malloc_r variants

extern "C" {

EXPORT_C void* malloc (size_t nbytes)
	{
	if (nbytes >= (KMaxTInt/2))
		{
		errno = ENOMEM;
		return NULL;
		}

	TAny *Tptr = User::Alloc(nbytes);
	if(Tptr == NULL)
		errno = ENOMEM;
	
	return Tptr;
	}

EXPORT_C void* realloc (void *p, size_t nbytes)
	{
	if (nbytes >= (KMaxTInt/2))
		{
		errno = ENOMEM;
		return NULL;
		}
		
	TAny *Tptr = User::ReAlloc(p, nbytes);
	if(Tptr == NULL)
		errno = ENOMEM;
	
	return Tptr;
	}

EXPORT_C void free (void *p)
	{
	User::Free(p);
	}

} // extern "C"
