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

#if !defined(__MEMORYBLOCK_H__)
#define __MEMORYBLOCK_H__

#include <pcstore/pcstoredef.h>

namespace PCStore
{
/**
@internalComponent

Class to manage a block of allocated memory.

The class encapsulates the data members containing the data pointer and data length. 
It also provides member functions through which the data can be 
accessed, set or reset, but not modified.
*/
class CMemoryBlock
	{
public:
	CMemoryBlock();
	CMemoryBlock(TInt32 aSize);
	~CMemoryBlock();
	const TUint8* Ptr() const; 
	TInt32 Size() const; 
	void Set(const TUint8* aPtr, TInt32 aLength, TInt32 aSize);
	
	
private:
	CMemoryBlock(const CMemoryBlock& aVal); 
	CMemoryBlock& operator=(const CMemoryBlock& aVal);

	void Release();
	void Reallocate(TInt32 aSize);

private:
	TUint8* iPtr;
	/**
	 * Number of bytes allocated
	 */
	TInt32 iSize;
	};
}
#endif // !defined(__MEMORYBLOCK_H__)
