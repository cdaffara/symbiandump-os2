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


#include <assert.h>
#include <string.h>
#include <wchar.h>
#include <pcstore/descriptors.h>
#include <pcstore/storeexception.h>
#include "memoryblock.h"

namespace PCStore
{
/**
Default constructor.

Constructs an empty 8-bit descriptor.
It represents no data and its maximum length and length are zeros.

The descriptor can, subsequently, be set to represent data.
*/
CDes8::CDes8()
	:iLength(0) 
	{
	iMemoryBlock = new CMemoryBlock();
	}

/**
Copy constructor.

Constructs an 8-bit descriptor from an existing one.

The descriptor can, subsequently, be set to represent data.

@param aDes The reference to the existing descriptor to copy the contents from.
*/
CDes8::CDes8(const CDes8& aDes)
	{
	iMemoryBlock = new CMemoryBlock();
	Set(aDes);
	}

/**
Constructs an empty 8-bit descriptor with the maximum length.

It represents no data and length is zero. But its maximum length 
is set to the specified number.

aMaxLength cannot be negative.

The descriptor can, subsequently, be set to represent data.

@param aMaxLength The maximum length of the descriptor.
*/
CDes8::CDes8(TInt32 aMaxLength)
	:iLength(0)
	{
	assert(aMaxLength >= 0);
	iMemoryBlock = new CMemoryBlock(aMaxLength);
	}


/**
Constructs an 8-bit descriptor with the data of a zero-terminated string.

The maximum length and actual length of the descriptor are set to the length 
of the zero-terminated string, excluding the zero terminator.

aPtr must not be NULL.

@param aPtr The pointer to the zero-terminated string.
*/
CDes8::CDes8(const TUint8* aPtr)
	{
	assert(aPtr != NULL);
	iMemoryBlock = new CMemoryBlock();
	Set(aPtr,strlen(reinterpret_cast<const TInt8*>(aPtr)));
	}

/**
Constructs an 8-bit descriptor with the specified data and length.

The maximum length and length of the descriptor are set to the specified number.

aLength cannot be negative. aPtr must not be NULL.

@param aPtr The pointer to the data to be represented.
@param aLength The length of the data to be represented.
*/
CDes8::CDes8(const TUint8* aPtr, TInt32 aLength)
	{
	assert(aPtr != NULL && aLength >= 0);
	iMemoryBlock = new CMemoryBlock();
	Set(aPtr, aLength);
	}

/**
Constructs an 8-bit descriptor with the specified data, length and maximum length.

The maximum length and actual length of the descriptor are set to the specified numbers.

aPtr must not be NULL. The aLength cannot be negative or greater than the aMaxLength. 

@param aPtr The pointer to the data to be represented.
@param aLength The length of the data to be represented.
@param aMaxLength The maximum length of the descriptor.
*/
CDes8::CDes8(const TUint8* aPtr, TInt32 aLength, TInt32 aMaxLength)
	{
	assert(aPtr != NULL && aLength >= 0 && aMaxLength >= aLength);
	iMemoryBlock = new CMemoryBlock();
	Set(aPtr, aLength, aMaxLength);
	}


/**
Destructor.

Release iMemoryBlock;
*/
CDes8::~CDes8()
	{
	delete iMemoryBlock;
	}


/**
Sets the 8-bit descriptor with the specified 8-bit descriptor.

The maximum length and actual length of the descriptor are set to the same as the
specified object's.

@param aDes The reference to the specified 8-bit descriptor.
*/
void CDes8::Set(const CDes8& aDes)
	{
	iMemoryBlock->Set(aDes.Ptr(), aDes.Length(), aDes.Size());
	iLength = aDes.iLength;
	}

/**
Sets the 8-bit descriptor with the specified data and length.

The maximum length and actual length of the descriptor are set to the 
specified number.

aPtr must not be NULL. The aLength cannot be negative.

If it fails to allocate memory, it throws exception. The new content 
will not be set and the old content will be remained.

@param aPtr The pointer to the data to be represented.
@param aLength The length of the data to be represented.
*/
void CDes8::Set(const TUint8* aPtr, TInt32 aLength)
	{
	assert(aPtr != NULL && aLength >= 0);
	Set(aPtr, aLength, aLength);
	}

/**
Sets the 8-bit descriptor with the specified data, length and maximum length.

The maximum length and actual length of the descriptor are set to the 
specified numbers.

The aLength cannot be negative or greater than the aMaxLength. aPtr must 
not be NULL.

If it fails to allocate memory, it throws exception. The new content 
will not be set and the old content will be remained.

@param aPtr The pointer to the data to be represented.
@param aLength The length of the data to be represented.
@param aMaxLength The maximum length of the descriptor.
*/
void CDes8::Set(const TUint8* aPtr, TInt32 aLength, TInt32 aMaxLength) 
	{
	assert(aPtr != NULL && aLength >= 0 && aMaxLength >= aLength);
	iMemoryBlock->Set(aPtr,aLength, aMaxLength);
	iLength = aLength;
	}

/**
Appends data onto the end of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.

No additional memory will be allocated for the data to be appended.

Assert the resulting new length of this descriptor is less than its 
maximum length.

@param aDes The reference to the descriptor whose data is to be appended.
*/	
void CDes8::Append(const CDes8& aDes)
	{
	TInt len = Length();
    TInt n = aDes.Length();
	assert(len + n <= MaxLength());
	SetLength(len+n);
	TUint8* startPtr = const_cast<TUint8*> (Ptr()+ len);
	// Memory block does not know about the actual length,
	// so we copy the memory to the pointer directly
    ::memcpy(startPtr, aDes.Ptr(),n);
	}

/**
Appends data onto the end of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.

No additional memory will be allocated for the data to be appended.

Assert aLength is positive then the resulting new length of this descriptor 
is less than its maximum length. aPtr must not be NULL.

@param aPtr The pointer to the data to be copied.
@param aLength The length of data to be copied.
*/
void CDes8::Append(const TUint8* aPtr, TInt32 aLength)
	{
	assert(aPtr != NULL && aLength >= 0);
	TInt len = Length();
	assert(len + aLength <= MaxLength());
	SetLength(len + aLength); 
	TUint8* startPtr = const_cast<TUint8*> (Ptr()+ len);
	// Memory block does not know about the actual length,
	// so we copy the memory to the pointer directly
    ::memcpy(startPtr, aPtr, aLength);
	}

/**
Gets the pointer to the data represented by the 8-bit descriptor.

The data cannot be changed through the returned pointer.

@return The pointer to the data represented by the 8-bit descriptor.
*/
const TUint8* CDes8::Ptr() const
	{
	return  iMemoryBlock->Ptr();
	}

/**
Gets the length of the data.

This is the number of 8-bit values or data items represented by the descriptor.

@return The length of the data represented by the descriptor. 
*/
TInt32 CDes8::Length() const 
	{
	return  iLength ;
	}

/**
Gets the maximum length of the 8-bit descriptor.

This is the maximum number of 8-bit values or data items that can be represented by 
the descriptor.

@return The maximum length of the data that can be represented by the descriptor.
*/
TInt32 CDes8::MaxLength() const 
	{
	return iMemoryBlock->Size() ;
	}

/**
Sets the length of this 8-bit descriptor.

The length of the descriptor is set to the specified number, with the data and maximum length 
remained the same.

The aLength cannot be negative or greater than the maximum length of the descriptor.

@param aLength The length to set.
*/
void CDes8::SetLength(TInt32 aLength)
	{
	assert(aLength >= 0 && aLength <= MaxLength());
	iLength = aLength;
	}

/**
Gets the size of the 8-bit descriptor, which is the number of bytes occupied by it.

@return The number of bytes occupied by the descriptor.
*/
TInt32 CDes8::Size() const
	{
	return iMemoryBlock->Size();
	}


/**
Assignment operator.

Set the contents of this 8-bit descriptor from an existing one.

@param aDes The reference to the existing descriptor to obtain the contents.
@return The reference to the object itself.
*/
CDes8& CDes8::operator=(const CDes8& aDes)
	{
	Set(aDes);
	return (*this);
	}

/**
Equality comparison operator. 

Determines whether this 8-bit descriptor is equal to the specified 8-bit descriptor.
It compares the length and the data content of both parties. But the maximum lengths are 
ignored.

@param aDes The reference to the descriptor to compare to.
@return true if equal, FALSE otherwise.
*/
TBool CDes8::operator==(const CDes8& aDes) const
	{
	if(iLength != aDes.Length())
		{
		return false;
		}

	const TUint8* ptr1 = aDes.Ptr();
	const TUint8* ptr2 = iMemoryBlock->Ptr();
	return ::memcmp(ptr1,ptr2,iLength) == 0;
	}

/**
Inequality comparison operator. 

Determines whether this 8-bit descriptor is unequal to the specified 8-bit descriptor.
It compares the length and the data content of both parties. The maximum lengths are 
ignored.

@param aDes The reference to the descriptor to compare to.
@return true if unequal, FALSE otherwise.
*/
TBool CDes8::operator!=(const CDes8& aDes) const
	{
	return (! (*this == aDes));
	}
	
/**
Default constructor.

Constructs an empty 16-bit descriptor.
It represents no data and its maximum length and length are zeros.

The descriptor can, subsequently, be set to represent data.
*/
CDes16::CDes16()
	:iLength(0) 
	{
	iMemoryBlock = new CMemoryBlock();
	}


/**
Copy constructor.

Constructs an 16-bit descriptor from an existing one.

The descriptor can, subsequently, be set to represent data.

@param aDes The reference to the existing descriptor to copy the contents from.
*/
CDes16::CDes16(const CDes16& aDes)
	{
	iMemoryBlock = new CMemoryBlock();
	Set(aDes);
	}


/**
Constructs an empty 16-bit descriptor with the maximum length.

It represents no data and length is zero. But its maximum length 
is set to the specified number.

aMaxLength cannot be negative.

The descriptor can, subsequently, be set to represent data.

@param aMaxLength The maximum length of the descriptor.
*/
CDes16::CDes16(TInt32 aMaxLength)
	:iLength(0)
	{
	assert(aMaxLength >= 0);
	iMemoryBlock = new CMemoryBlock(aMaxLength * 2);
	}

/**
Constructs a 16-bit descriptor with the data of a zero-terminated wide-character 
string.

The maximum length and actual length of the descriptor are set to the length 
of the zero-terminated string, excluding the zero terminator.

aPtr must not be NULL.

@param aPtr The pointer to the zero-terminated wide-character string.
*/
CDes16::CDes16(const TUint16* aPtr)
	{
	assert(aPtr != NULL);
	iMemoryBlock = new CMemoryBlock();
	Set(aPtr,wcslen(reinterpret_cast<const wchar_t*>(aPtr)));
	}


/**
Constructs a 16-bit descriptor with the specified data and length.

The maximum length and length of the descriptor are set to the specified number.

aPtr must not be NULL. aLength cannot be negative.

@param aPtr The pointer to the data to be represented.
@param aLength The length of the data to be represented.
*/
CDes16::CDes16(const TUint16* aPtr, TInt32 aLength)
	{
	assert(aPtr != NULL && aLength >= 0);
	iMemoryBlock = new CMemoryBlock();
	Set(aPtr, aLength);
	}


/**
Constructs an 16-bit descriptor with the specified data, length and maximum length.

The maximum length and actual length of the descriptor are set to the specified numbers.

aPtr must not be NULL. The aLength cannot be negative or greater than the aMaxLength.

@param aPtr The pointer to the data to be represented.
@param aLength The length of the data to be represented.
@param aMaxLength The maximum length of the descriptor.
*/
CDes16::CDes16(const TUint16* aPtr, TInt32 aLength, TInt32 aMaxLength)
	{
	assert(aPtr != NULL && aLength >= 0 && aMaxLength >= aLength);
	iMemoryBlock = new CMemoryBlock();
	Set(aPtr, aLength, aMaxLength);
	}


/**
Destructor.

Release iMemoryBlock;
*/
CDes16::~CDes16()
	{
	delete iMemoryBlock;
	}

/**
Sets the 16-bit descriptor with the specified 16-bit descriptor.

The maximum length and actual length of the descriptor are set to the same as the
specified object's.

If it fails to allocate memory, it throws exception. The new content 
will not be set and the old content will be remained.

@param aDes The reference to the specified 16-bit descriptor.
*/
void CDes16::Set(const CDes16& aDes)
	{
	iMemoryBlock->Set(reinterpret_cast<const TUint8*>(aDes.Ptr()), aDes.iLength * 2, aDes.Size());
	iLength = aDes.iLength;
	}

/**
Sets the 16-bit descriptor with the specified data and length.

The maximum length and actual length of the descriptor are set to the specified number.

aPtr must not be NULL. The aLength cannot be negative.

If it fails to allocate memory, it throws exception. The new content 
will not be set and the old content will be remained.

@param aPtr The pointer to the data to be represented.
@param aLength The length of the data to be represented.
*/
void CDes16::Set(const TUint16* aPtr, TInt32 aLength)
	{
	assert(aPtr != NULL && aLength >= 0);
	Set(aPtr, aLength, aLength);
	}


/**
Sets the 16-bit descriptor with the specified data, length and maximum length.

The maximum length and actual length of the descriptor are set to the specified numbers.

aPtr must not be NULL. The aLength cannot be negative or greater than the aMaxLength.

If it fails to allocate memory, it throws exception. The new content 
will not be set and the old content will be remained.

@param aPtr The pointer to the data to be represented.
@param aLength The length of the data to be represented.
@param aMaxLength The maximum length of the descriptor.
*/
void CDes16::Set(const TUint16* aPtr, TInt32 aLength, TInt32 aMaxLength) 
	{
	assert(aPtr != NULL && aLength >= 0 && aMaxLength >= aLength);
	iMemoryBlock->Set(reinterpret_cast<const TUint8*>(aPtr),aLength * 2, aMaxLength * 2);
	iLength = aLength;
	}

/**
Appends data onto the end of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.

No additional memory will be allocated for the data to be appended.

Assert the resulting new length of this descriptor is less than its 
maximum length.

@param aDes A 16-bit descriptor whose data is to be appended.
*/
void CDes16::Append(const CDes16& aDes)
	{
	TInt len = Length();
    TInt n = aDes.Length();
	assert(len + n <= MaxLength());
	SetLength(len+n);
	const wchar_t* startPtr = reinterpret_cast<const wchar_t*>(Ptr()+ len);
	// Memory block does not know about the actual length,
	// so we copy the memory to the pointer directly
    ::wmemcpy(const_cast<wchar_t*>(startPtr), reinterpret_cast<const wchar_t*>(aDes.Ptr()),n);
	}

/**
Appends data onto the end of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.

No additional memory will be allocated for the data to be appended.

Assert aLength is positive and the resulting new length of this descriptor 
is less than its maximum length. aPtr must not be NULL.

@param aPtr The pointer to the data to be copied.
@param aLength The length of data to be copied.
*/
void CDes16::Append(const TUint16* aPtr, TInt32 aLength)
	{
	assert(aPtr != NULL && aLength >=0);
	TInt len = Length();
	assert(len + aLength <= MaxLength());
	SetLength(len + aLength);
	const wchar_t* startPtr = reinterpret_cast<const wchar_t*>(Ptr()+ len);
	// Memory block does not know about the actual length,
	// so we copy the memory to the pointer directly
	::wmemcpy(const_cast<wchar_t*>(startPtr), reinterpret_cast<const wchar_t*>(aPtr),aLength);
    //::wmemcpy(startPtr, aPtr, aLength);
	}


/**
Gets the pointer to the data represented by the 16-bit descriptor.

The data cannot be changed through the returned pointer.

@return The pointer to the data represented by the 16-bit descriptor.
*/
const TUint16* CDes16::Ptr() const
	{
	return  reinterpret_cast<const TUint16*>(iMemoryBlock->Ptr());
	}


/**
Gets the length of the data.

This is the number of 16-bit values or data items represented by the descriptor.

@return The length of the data represented by the descriptor. 
*/
TInt32 CDes16::Length() const
	{
	return  iLength;
	}

/**
Gets the maximum length of the 16-bit descriptor.

This is the maximum number of 16-bit values or data items that can be represented by 
the descriptor.

@return The maximum length of the data that can be represented by the descriptor.
*/
TInt32 CDes16::MaxLength() const 
	{
	return  iMemoryBlock->Size()/2;
	}

/**
Sets the length of this 16-bit descriptor.

The length of the descriptor is set to the specified number, with the data and maximum length 
remained the same.

The aLength cannot be negative or greater than maximum length of the descriptor.

@param aLength The length to set.
*/
void CDes16::SetLength(TInt32 aLength)
	{
	assert(aLength >= 0 && aLength <= MaxLength());
	iLength = aLength;
	}


/**
Gets the size of the 16-bit descriptor, which is the number of bytes occupied by it.

@return The number of bytes occupied by the descriptor.
*/
TInt32 CDes16::Size() const
	{
	return iMemoryBlock->Size();
	}

/**
Assignment operator.

Set the contents of this 16-bit descriptor from an existing one.

@param aDes The reference to the existing descriptor to obtain the contents.
@return The reference to the object itself.
*/
CDes16& CDes16::operator=(const CDes16& aDes)
	{
	Set(aDes);
	return (*this);
	}

/**
Equality comparison operator. 

Determines whether this 16-bit descriptor is equal to the specified 16-bit descriptor.
It compares the length and the data content of both parties. The maximum lengths are 
ignored.

@param aDes The reference to the descriptor to compare to.
@return true if equal, FALSE otherwise.
*/
TBool CDes16::operator==(const CDes16& aDes) const
	{
	if(iLength != aDes.Length()) 
		{
		return false;
		}

	const TUint16* ptr1 = aDes.Ptr();
	const TUint16* ptr2 = reinterpret_cast<const TUint16*>(iMemoryBlock->Ptr());
	return ::wmemcmp(reinterpret_cast<const wchar_t *>(ptr1),reinterpret_cast<const wchar_t *>(ptr2),iLength) == 0;
	}

/**
Inequality comparison operator. 

Determines whether this 16-bit descriptor is unequal to the specified 16-bit descriptor.
It compares the length and the data content of both parties. The maximum lengths are 
ignored.

@param aDes The reference to the descriptor to compare to.
@return true if unequal, FALSE otherwise.
*/
TBool CDes16::operator!=(const CDes16& aDes) const
	{
	return (! (*this == aDes));
	}
}
