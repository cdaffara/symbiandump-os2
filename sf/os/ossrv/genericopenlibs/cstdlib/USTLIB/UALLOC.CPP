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

#include <e32std.h>
#include <stdlib_r.h>

// We use the EPOC32 memory allocation which is already
// thread-safe, so we don't need the _malloc_r variants

extern "C" {

EXPORT_C void* malloc (size_t nbytes)
	{
	return User::Alloc(nbytes);
	}

EXPORT_C void* realloc (void *p, size_t nbytes)
	{
	return User::ReAlloc(p, nbytes);
	}

EXPORT_C void free (void *p)
	{
	User::Free(p);
	}

#ifdef EKA2
EXPORT_C void* _placeholder_memmove (void* to, const void* from, size_t len)
#else
EXPORT_C void* memmove (void* to, const void* from, size_t len)
#endif
	{
	Mem::Copy(to, from, len);		// E32 unaligned careful copy
	return to;
	}

#ifdef EKA2
EXPORT_C void* _placeholder_memset (void* to, int c, size_t len)
#else
EXPORT_C void* memset (void* to, int c, size_t len)
#endif
	{
	Mem::Fill(to,len,c);
	return to;
	}

EXPORT_C void* calloc (size_t n, size_t size)
    {
    n *= size;
    void *ret = User::Alloc(n);
    if (ret == 0)
	return 0;
    Mem::FillZ(ret,n);
    return ret;
    }

EXPORT_C size_t strlen (const char *str)
    {
    return User::StringLength((TUint8*)str);
    }

EXPORT_C size_t wcslen (const wchar_t *str)
    {
    return User::StringLength((TUint16*)str);
    }

EXPORT_C unsigned int sleep (unsigned int secs)
	{
	// we don't allow the possibility of being woken by signals
	User::After(secs*1000000);
	return 0;
	}

} // extern "C"
