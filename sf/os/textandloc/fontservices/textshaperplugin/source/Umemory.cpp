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
* This maps Symbian OS memory management onto Icu Umemory functions
* Memory is allocated from the shaper internal heap, which has been saved in the TLS.
* If there is no memory available these memory functions leave, this is caught in the shaper wrapper.
* For debugging there is an alternative set of functions which log memory allocations
*
*/


#include "unicode/uobject.h"

#include <e32debug.h>

U_NAMESPACE_BEGIN

#if !defined  SHAPER_MEMORY_DEBUG
/* this is the production code versions */

void* UMemory::operator new(size_t size)
	{
	return User::Alloc(size);
	}

void UMemory::operator delete(void* p)
	{
	return User::Free(p);
	}

void* UMemory::operator new[](size_t size)
	{
	return User::Alloc(size);
	}

void UMemory::operator delete[](void* p)
	{
	return User::Free(p);
	}

	
#else 
/* test versions for debugging shaper heap memory problems */

// just for testing record some details on the memory use
TUint maxMemoryUsed = 0;

void* UMemory::operator new(size_t size)
	{
	// For debugging trap OOM
		
	// get a pointer to the heap in use from thread local storage	
	void* pointer = User::Alloc(size);
	
	// for development keep a count of the max heap used
	TInt totalAllocSize = 0;
	TInt used = User::AllocSize(totalAllocSize);
	if (totalAllocSize > maxMemoryUsed) 
		maxMemoryUsed = totalAllocSize;

	RDebug::Print(_L("UMemory::new %x size %d total heap %d cells %d "), pointer, size, totalAllocSize, used );
	
	return pointer;
	}

void UMemory::operator delete(void* p)
	{
	RDebug::Print(_L("delete %x "), p );
	User::Free(p);
	}

void* UMemory::operator new[](size_t size)
	{
	// for development keep a count of the max heap used
	TInt totalAllocSize = 0;
	TInt used = User::AllocSize(totalAllocSize);
	if (totalAllocSize > maxMemoryUsed) 
		maxMemoryUsed = totalAllocSize;
	
	RDebug::Print(_L("new[] %x size %d total heap %d cells %d"), pointer, size, totalAllocSize, used);

	return pointer;
	}

void UMemory::operator delete[](void* p)
	{
	RDebug::Print(_L("delete[] %x "), p );
	User::Free(p);
	}

void * UMemory::NewArray(int size, int count) 
	{
	void* pointer = User::Alloc(size*count);
	RDebug::Print(_L("UMemory::NewArray %d bytes %x"), size * count, pointer );
	return pointer;
	}

/*
For debugging code replacement for
#define LE_GROW_ARRAY(array, newSize) ( ((RHeap*)Dll::Tls())->ReAllocL(array, (newSize) * sizeof (array)[0]))
*/
void * UMemory::GrowArray(void * array, int newSize ) 
	{
	void* pointer = User::ReAlloc(array, newSize);
	RDebug::Print(_L("UMemory::GrowArray %d new %d bytes %x"), array, newSize, pointer);
	return pointer;
	}


void UMemory::FreeArray( void * array ) 
	{
	RDebug::Print(_L("delete array %x "), array);
	User::Free((void *)array);
	}

/* end of debug versions */
#endif

UObject::~UObject() {}



U_NAMESPACE_END
