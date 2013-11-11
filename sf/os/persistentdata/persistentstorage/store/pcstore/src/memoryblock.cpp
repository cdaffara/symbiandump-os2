// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include <pcstore/storeexception.h>
#include "memoryblock.h"


namespace PCStore
{
/**
Default constructor.

Constructs an empty object which has no memory allocated.
*/
CMemoryBlock::CMemoryBlock()
	:iPtr(NULL), iSize(0)
	{}

/**
Constructs a memory block in the specified size.

It allocates the memory of the specified size and fills it with zeros.

Assert aSize must not be negative.

@param aSize the size of memory to allocate.
*/
CMemoryBlock::CMemoryBlock(TInt32 aSize)
	:iPtr(NULL), iSize(0)
	{
	assert(aSize >= 0);
	Reallocate(aSize);
	}


/**
Destructor.

Releases the allocated memory.
*/
CMemoryBlock::~CMemoryBlock()
	{
	Release();
	}

/**
Gets the pointer to the memory it allocated.

The data cannot be changed through the returned pointer.

@return the pointer to the memory it allocated.
*/
const TUint8* CMemoryBlock::Ptr() const
	{
	return iPtr; 
	}

/**
Sets the memory block with the specified data, length and size.

Reallocates the memory and copies the data from the given pointer of 
the given length.

Assert aLength must not be negative, aLength is less than aSize, aPtr must not 
be NULL.

If it fails to allocate memory, it throws exception. The new content 
will not be set and the old content will be remained.

@param aPtr the pointer of the data to be set.
@param aLength the length of the data.
@param aSize the size of the memory to reallocate.
*/
void CMemoryBlock::Set(const TUint8* aPtr, TInt32 aLength, TInt32 aSize)
	{
	assert(aLength >= 0 && aSize >= aLength && aPtr != NULL);
	Reallocate(aSize);
	::memcpy(iPtr, aPtr,aLength);
	}

/**
Releases the memory block.

Releases the memory if it has been allocated and sets the size to zero.
*/
void CMemoryBlock::Release()
	{
	delete[] iPtr;
	iPtr = NULL;
	iSize = 0;
	}

/**
Gets the size of the memory block, which is the number of bytes of memory it has
allocated.

@return the number of bytes of the allocated memory.
*/
TInt32 CMemoryBlock::Size() const
	{
	return iSize; 
	}

/**
Reallocates the memory with the specified size and set the member data iPtr and 
iSize to reflect the new allocation.

Calls Release() to free the previously allocated memory on the success of the 
allocation. If the allocation fails, throws an exception but does not affect 
the old content.

Assert aSize must not be negative.

@param aSize the number of bytes to be allocated.
*/
void CMemoryBlock::Reallocate(TInt32 aSize)
	{
	assert(aSize >= 0);
	if(aSize > 0) 
		{
		TUint8* ptr;
		ptr = new TUint8 [aSize];
		Release();
		iPtr = ptr;
		iSize = aSize;
		}
	}
}
