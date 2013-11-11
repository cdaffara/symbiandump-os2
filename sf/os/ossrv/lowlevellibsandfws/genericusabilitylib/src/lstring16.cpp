// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <estring.h>

const TUint KDefaultExpandSize = 16;


/**
Aligns the supplied capacity to the nearest growth factor

For performance reasons the growth factor, KDefaultExpandSizeShift,
is expressed as an exponent of 2 so shifting can be used to achieve the
alignment. 

a KDefaultExpandSizeShift value of 4 is equivalent to 16; 
giving newCapacity = ((newCapacity / 16) + 1) * 16

@param aNewCapacity The size to be aligned

@return The new, aligned capacity
*/
static inline TInt AlignCapacity(TInt aNewCapacity)
	{
	const TUint KDefaultExpandSizeShift = 4;

	return (TInt)((((TUint)aNewCapacity >> KDefaultExpandSizeShift) + 1) << KDefaultExpandSizeShift);
	}

/**
Guarantees that MaxLength() is greater than or equal to the supplied
capacity, reallocating the supplied capacity if necessary.

The actual value of MaxLength() after a call may differ from the exact
value requested, but if it does differ it will always be greater. This
flexibility allows the implementation to manage heap buffers more
efficiently.

The string descriptor's heap buffer may be reallocated in order to
accommodate the new size. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aMinRequiredCapacity The minimum value of MaxLength() required

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
void LString16::ReserveL(TInt aMinRequiredCapacity)
	{
	if (MaxLength() < aMinRequiredCapacity)
		{
		ReAllocL(AlignCapacity(aMinRequiredCapacity));
		}
	}


/**
Guarantees that MaxLength() is greater than or equal to the supplied
integer parameter, growing the underlying heap buffer if necessary.

The growth is exponential; maxLength *= 1.5
This is reported to give an amortised complexity of O(n) when adding
n characters. 
If the required capacity is larger than the expanded size then the
required capacity is used instead.

The actual value of MaxLength() after a call may differ from the exact
value requested, but if it does differ it will always be greater. This
flexibility allows the implementation to manage heap buffers more
efficiently.

@param aRequiredCapacity The minimum value of MaxLength() required

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
void LString16::ReserveCapacityGrowExponentialL(TInt aRequiredCapacity)
	{
	//work in unsigned int for the appropriate shift operation
	TUint max_length = MaxLength();
	TUint requiredCapacity = aRequiredCapacity; 

	if (max_length < requiredCapacity)
		{
		// max_length *= 3/2;
		max_length = (max_length + (max_length << 1)) >> 1;

		// take the bigger of the extended buffer or the required capactiy 
		ReAllocL(AlignCapacity((TInt)(max_length > requiredCapacity ? max_length : requiredCapacity)));
		}
	}

/**
Guarantees that free space in the buffer greater than or equal the 
supplied integer parameter, growing the underlying heap buffer 
if necessary.

@param aRequiredEmptySpace The minimum value of free space required

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
void LString16::ReserveFreeCapacityGrowExponentialL(TInt aRequiredEmptySpace)
	{
	ReserveCapacityGrowExponentialL(Length() + aRequiredEmptySpace);
	}

/**
Grows the underlying buffer using the exponential growth 
function. Guarantees that MaxLength() is greater than or 
equal to 1.5 * the current MaxLength.


@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
void LString16::ReserveCapacityGrowExponentialL()
	{
	ReserveCapacityGrowExponentialL(MaxLength() + 1);
	}


/**
Default constructor.

Constructs a zero-length 16-bit resizable string descriptor.

Note that the resulting object owns no allocated memory yet. This
default constructor never leaves.
*/
EXPORT_C LString16::LString16() 
	: iReserved(0)
	{
	}

/**
Destructor.

Frees any heap-allocated resources owned by this string descriptor. It
is safe to rely on this destructor to perform all necessary cleanup;
it is not necessary use the cleanup stack or to call Close() manually.

@see RBuf16::Close
*/
EXPORT_C LString16::~LString16()
	{
	RBuf16::Close();
	}

/**
Constructor to create a 16-bit resizable string descriptor with an
initial capacity.

The function allocates sufficient memory to contain descriptor data up to
the specified initial maximum length. 

The current length of the descriptor is set to zero. The maximum length of
the descriptor is set to the specified value.

@param aMaxLength  The maximum length of the descriptor.

@leave KErrNoMemory If there is insufficient memory.

@see RBuf16::CreateL
*/
EXPORT_C LString16::LString16(TInt aMaxLength)
	: iReserved(0)
	{
	RBuf16::CreateL(aMaxLength);
	}

/**
Constructor to create a 16-bit resizable string descriptor from a
pre-allocated heap descriptor.

Transfers ownership of the specified heap descriptor to this object.

@param aHBuf  The heap descriptor to be transferred to this object.
              This pointer can be NULL, which means that a zero length
              16-bit resizable string descriptor is created.

@see RBuf16::RBuf16(HBufC16*)
*/
EXPORT_C LString16::LString16(HBufC16* aHBuf)
	: iReserved(0)
	{
	if (aHBuf)
		RBuf16::Assign (aHBuf);
	}

/**
Constructor to create a 16-bit resizable string descriptor from a
pre-allocated raw heap buffer.

The allocated memory forms the buffer for this string descriptor. The
current length of the descriptor is set to zero.

@param aHeapCell  The allocated memory to be assigned to this object. This
                  pointer can be NULL, which means that a zero length 16-bit
                  resizable buffer descriptor is created.
@param aMaxLength The maximum length of the constructed string descriptor.

@panic USER 8 If the specified maximum length is greater then the size of
              the allocated heap cell, or the specified maximum length
              is NOT zero when the pointer to the heap cell is NULL.

@see RBuf16::Assign()
*/
EXPORT_C LString16::LString16(TUint16* aHeapCell,TInt aMaxLength)
	: iReserved(0)
	{
	RBuf16::Assign(aHeapCell, aMaxLength);
	}

/**
Constructor to create a 16-bit resizable string descriptor from a
pre-allocated raw heap buffer.

The allocated memory forms the buffer for this string descriptor. The
current length of the descriptor is set to the value of the second
parameter.

@param aHeapCell  The allocated memory to be assigned to this object.
@param aLength	  The length of the resulting string descriptor.
@param aMaxLength The maximum length of the resulting string descriptor.

@panic USER 8 If the specified maximum length is greater then the size of
              the allocated heap cell, or the specified length is greater then
              the specified	maximum length, or the specified maximum length
              is NOT zero when the pointer to the heap cell is NULL.

@see RBuf16::Assign()
*/
EXPORT_C LString16::LString16(TUint16* aHeapCell,TInt aLength,TInt aMaxLength)
	: iReserved(0)
	{
	RBuf16::Assign(aHeapCell, aLength, aMaxLength);
	}

/**
Constructor to create a 16-bit resizable string descriptor to contain
a copy of the specified (source) descriptor, or leave on failure.

The constructor allocates sufficient memory so that this string
descriptor's maximum length is the same as the length of the source
descriptor. Both the current length and the maximum length of this
string descriptor are set to the length of the source descriptor.

The data contained in the source descriptor is copied into this string
descriptor.

@param aDes Source descriptor to be copied into this object.

@leave KErrNoMemory If there is insufficient memory.

@see RBuf16::CreateL()
*/
EXPORT_C LString16::LString16(const TDesC16& aDes)
	: iReserved(0)
	{
	RBuf16::CreateL(aDes);
	}

/**
Copies data into this 16-bit string descriptor, replacing any existing
data, and expanding its heap buffer to accommodate if necessary.

The length of this descriptor is set to reflect the new data.

This operation may cause the target string descriptor's heap buffer to
be reallocated in order to accommodate the new data. As a result,
MaxLength() and Ptr() may return different values afterwards, and any
existing raw pointers to into the descriptor data may be invalidated.

Note that the automatic resizing performed is a change to the
functionality of this operation compared to other descriptor
classes. This change is only active on objects directly declared
LString16; when LString16 instances are instead manipulated via
references to TDes16 or TDesC16, the standard (non-resizing, panicing)
variant is invoked.

@param aDes A 16-bit non-modifiable descriptor.

@return A reference to this 16-bit string descriptor.

@leave KErrNoMemory If the heap buffer of the string descriptor being
              assigned to needs to be expanded, but there is
              insufficient memory to do so

@see LString16::CopyL
*/
EXPORT_C LString16& LString16::operator=(const TDesC16& aDes)
	{
	CopyL(aDes);
	return *this;
	}


/**
Transfers ownership of the specified 16-bit resizable descriptor's this object. 

@param aBuf The source 16-bit resizable buffer. The ownership of this
             object's buffer is to be transferred.
             
@return A reference to this 16-bit string descriptor.

@see Assign()
*/
EXPORT_C LString16& LString16::operator=(HBufC16* aBuf)
	{
	Assign(aBuf); 
	return *this;
	}


/**
Copies data into this 16-bit string descriptor, replacing any existing
data, and expanding its heap buffer to accommodate if necessary.

The length of this descriptor is set to reflect the new data.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aDes A 16-bit non-modifiable descriptor.

@leave KErrNoMemory If the heap buffer of the string descriptor being
              assigned to needs to be expanded, but there is
              insufficient memory to do so

@see LString16::operator=
@see TDes16::Copy
*/
EXPORT_C void LString16::CopyL(const TDesC16& aDes)
	{
	ReserveL(aDes.Length());
	RBuf16::Copy(aDes);
	}

/**
Copy constructor to create a 16-bit resizable string descriptor to
contain a copy of the specified (source) string descriptor's data, or
leave on failure.

The constructor allocates sufficient memory so that this string
descriptor's maximum length is the same as the length of the source
string descriptor. Both the current length and the maximum length of
this string descriptor are set to the length of the source descriptor.

The data contained in the source string descriptor is copied into this
string descriptor.

@param aDes Source string descriptor to be copied into this object.

@leave KErrNoMemory If there is insufficient memory.

@see RBuf16::CreateL()
*/
EXPORT_C LString16::LString16(const LString16& aDes)
	: iReserved(0)
	{
	RBuf16::CreateL(aDes);
	}


/**
Copies data into this 16-bit string descriptor, replacing any existing
data, and expanding its heap buffer to accommodate if necessary.

The length of this descriptor is set to reflect the new data.

This operation may cause the target string descriptor's heap buffer to
be reallocated in order to accommodate the new data. As a result,
MaxLength() and Ptr() may return different values afterwards, and any
existing raw pointers to into the descriptor data may be invalidated.

Note that the automatic resizing performed is a change to the
functionality of this operation compared to other descriptor
classes. This change is only active on objects directly declared
LString16; when LString16 instances are instead manipulated via
references to TDes16 or TDesC16, the standard (non-resizing, panicing)
variant is invoked.

@param aDes A 16-bit string descriptor.

@return A reference to this 16-bit string descriptor.

@leave KErrNoMemory If the heap buffer of the string descriptor being
              assigned to needs to be expanded, but there is
              insufficient memory to do so

@see LString16::CopyL
*/
EXPORT_C LString16& LString16::operator=(const LString16& aDes)
	{
	CopyL(aDes);
	return *this;
	}

/**
Constructor to create a 16-bit resizable string descriptor containing
a copy of the specified (source) zero-terminated string data, or leave
on failure.

The constructor allocates sufficient memory so that this string
descriptor's maximum length is the same as the length of the source
string. Both the current length and the maximum length of this string
descriptor are set to the length of the source string. 

The data contained in the source string is copied into this string
descriptor. The zero terminator is not copied.

@param aZeroTerminatedString A pointer to a zero-terminated string

@leave KErrNoMemory If there is insufficient memory.

@see LString16::CopyL
*/
EXPORT_C LString16::LString16(const TUint16* aZeroTerminatedString)
	: iReserved(0)
	{
	CopyL(aZeroTerminatedString);
	}

/**
Copies data into this 16-bit string descriptor, replacing any existing
data, and expanding its heap buffer to accommodate if necessary.

The length of this descriptor is set to reflect the new data.

This operation may cause the target string descriptor's heap buffer to
be reallocated in order to accommodate the new data. As a result,
MaxLength() and Ptr() may return different values afterwards, and any
existing raw pointers to into the descriptor data may be invalidated.

Note that the automatic resizing performed is a change to the
functionality of this operation compared to other descriptor
classes. This change is only active on objects directly declared
LString16; when LString16 instances are instead manipulated via
references to TDes16 or TDesC16, the standard (non-resizing, panicing)
variant is invoked.

@param aZeroTerminatedString A pointer to a zero-terminated string

@return A reference to this 16-bit string descriptor.

@leave KErrNoMemory If the heap buffer of the string descriptor being
              assigned to needs to be expanded, but there is
              insufficient memory to do so

@see LString16::CopyL
*/
EXPORT_C LString16& LString16::operator=(const TUint16* aZeroTerminatedString)
	{
	CopyL(aZeroTerminatedString);
	return *this;
	}

/**
Copies data into this 16-bit string descriptor, replacing any existing
data, and expanding its heap buffer to accommodate if necessary.

The length of this descriptor is set to reflect the new data.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aZeroTerminatedString A pointer to a zero-terminated string

@leave KErrNoMemory If the heap buffer of the string descriptor being
              assigned to needs to be expanded, but there is
              insufficient memory to do so

@see LString16::operator=
@see TDes16::Copy
*/
EXPORT_C void LString16::CopyL(const TUint16* aZeroTerminatedString)
	{
	ReserveL(User::StringLength(aZeroTerminatedString));
	RBuf16::Copy(aZeroTerminatedString);
	}

/**
Copies 8-bit descriptor data into this 16-bit string descriptor,
replacing any existing data, and expanding its heap buffer to
accommodate if necessary.

The length of this descriptor is set to reflect the new data.

Each 8-bit character value is widened to a 16-bit character value as
part of the copying process.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aDes An 8 bit non modifiable descriptor. 

@leave KErrNoMemory If the heap buffer of the string descriptor being
              assigned to needs to be expanded, but there is
              insufficient memory to do so

@see LString16::operator=
@see TDes16::Copy
*/
EXPORT_C void LString16::CopyL(const TDesC8& aDes)
	{
	ReserveL(aDes.Length());
	RBuf16::Copy(aDes);
	}

/**
Copies data into this 16-bit string descriptor, replacing any existing
data, and expanding its heap buffer to accommodate if necessary.

The length of this descriptor is set according to the second
parameter.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aBuf    The start address of data to be copied. 
@param aLength The length of data to be copied.

@leave KErrNoMemory If the heap buffer of the string descriptor being
              assigned to needs to be expanded, but there is
              insufficient memory to do so

@panic USER 11  if aLength is negative.

@see TDes16::Copy
*/
EXPORT_C void LString16::CopyL(const TUint16* aBuf,TInt aLength)
	{
	ReserveL(aLength);
	RBuf16::Copy(aBuf, aLength);
	}


/**
Sets the length of the data represented by the string descriptor to
the specified value.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aLength The new length of the descriptor.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 11 if aLength is negative 
*/
EXPORT_C void LString16::SetLengthL(TInt aLength)
	{
	ReserveL(aLength);
	RBuf16::SetLength(aLength);
	}


/**
Sets the storage space allocated to this descriptor to the specified 
value by growing or compressing its buffer size.

If the current length of the descriptor is greater than the specified
max length, length is truncated to max length.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aMaxLength The new maximum length of the descriptor.

@leave KErrNoMemory if the the buffer needs to be
reallocated and there are insufficient resources to do so 

@panic USER 11 if aLength is negative 
*/
EXPORT_C void LString16::SetMaxLengthL(TInt aMaxLength)
	{
	if (MaxLength() == aMaxLength) 
		{
		return;
		}

	if (Length() > aMaxLength) 
		{
		// truncate the current length
		RBuf16::SetLength(aMaxLength);
		}

	ReAllocL(aMaxLength);
	}


/**
Ensures that the remaining unused space is more than the supplied value. 

May reallocate a larger storage space to meet the requirement.
As a result MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

Typically, you use this method to reserve a known amount of required space
in one go instead of relying on the automatic growth pattern.

@param aExtraSpaceLength The extra space required.

@leave KErrNoMemory if the the buffer needs to be
reallocated and there are insufficient resources to do so.

@panic USER 11 if aLength is negative 
*/
EXPORT_C void LString16::ReserveFreeCapacityL(TInt aExtraSpaceLength)
	{
	ReserveL(Length() + aExtraSpaceLength);
	}


/**
Re-initialises the descriptor destroying its payload  

*/
EXPORT_C void LString16::Reset()
	{
	RBuf16::Close();
	}


/**
Re-allocates a smaller descriptor buffer space to the current 
descriptor length 
 
This may cause the string descriptor's heap buffer to be reallocated
in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

If there is insufficient memory to re-allocate the buffer then the
descriptor left unchanged
*/
EXPORT_C void LString16::Compress()
	{
	TInt length = Length();
	if (MaxLength() > length)
		{
		ReAlloc(length);
		}
	}


/**
Appends data onto the end of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aChar A single character to be appended. The length of the descriptor 
             is incremented by one.
             
@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@see LString16::operator+=
*/
EXPORT_C void LString16::AppendL(TChar aChar)
	{
	ReserveFreeCapacityGrowExponentialL(1);
	RBuf16::Append(aChar);
	}

/**
Appends data onto the end of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aChar A single character to be appended. The length of the descriptor 
             is incremented by one.
             
@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@see LString16::AppendL
*/
EXPORT_C LString16& LString16::operator+=(TChar aChar)
	{
	AppendL(aChar); 
	return *this;
	}

/**
Appends data onto the end of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aDes A 16-bit non modifiable descriptor whose data is to be appended.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16::AppendL(const TDesC16& aDes)
	{
	ReserveFreeCapacityGrowExponentialL(aDes.Length());
	RBuf16::Append(aDes);
	}

/**
Appends data onto the end of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aDes A 16-bit non modifiable descriptor whose data is to be appended.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@see LString16::AppendL
*/
EXPORT_C LString16& LString16::operator+=(const TDesC16& aDes)
	{
	AppendL(aDes); 
	return *this;
	}

/**
Appends data onto the end of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aBuf    A pointer to the data to be copied.
@param aLength The length of data to be copied.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 17  if aLength is negative.
*/
EXPORT_C void LString16::AppendL(const TUint16* aBuf,TInt aLength)
	{
	ReserveFreeCapacityGrowExponentialL(aLength);
	RBuf16::Append(aBuf, aLength);
	}

/** 
Fills the descriptor's data area with the specified character, replacing any 
existing data.

The descriptor is filled with the specified number of characters,
and its length is changed to reflect this.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aChar   The fill character.
@param aLength The new length of the descriptor and the number of fill characters 
               to be copied into it. 

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
               
@panic USER 11  if aLength is negative
*/
EXPORT_C void LString16::FillL(TChar aChar,TInt aLength)
	{
	ReserveL(aLength);
	RBuf16::Fill(aChar, aLength);
	}

/**
Fills the descriptor's data area with binary zeroes, i.e. 0x0000, replacing any 
existing data, and changes its length.

The descriptor is filled with the specified number of binary zeroes.
The descriptor's length is changed to reflect this.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aLength The new length of the descriptor and the number of binary zeroes
               to be copied into it. 
               
@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 11  if aLength is negative
*/
EXPORT_C void LString16::FillZL(TInt aLength)
	{
	ReserveL(aLength);
	RBuf16::FillZ(aLength);
	}

/**
Converts the specified unsigned integer into a fixed width character
representation based on the specified number system and copies the conversion
into this descriptor, replacing any existing data.

The length of this descriptor is set to reflect the new data.

The function generates the exact number of specified characters, either padding 
to the left with character zeroes or discarding low order characters as necessary.

When a hexadecimal conversion is specified, hexadecimal characters are in 
lower case.

This function is equivalent to using Format() with parameters which specify:

1. a fixed length target field

2. padding with zero characters, for example "%08x".

When this is the case, always use NumFixedWidth() in preference 
to Format() as it is more efficient.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aVal   The unsigned integer value. 
@param aRadix The number system representation for the unsigned integer. 
@param aWidth The number of characters: to be used to contain the conversion, 
              to be copied into this descriptor.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16::NumFixedWidthL(TUint aVal,TRadix aRadix,TInt aWidth)
	{
	Zero();
	AppendNumFixedWidthL(aVal, aRadix, aWidth);
	}

/**
Converts the specified unsigned integer into a fixed width character
representation based on the specified number system and appends the conversion
onto the end of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.

The function generates the exact number of specified characters, either padding 
to the left with character zeroes or discarding low order characters as
necessary.

When a hexadecimal conversion is specified, hexadecimal characters are in 
lower case.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aVal   The unsigned integer value. 
@param aRadix The number system representation for the unsigned integer. 
@param aWidth The number of characters to be used to contain the conversion,
              and to be appended to this descriptor.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16::AppendNumFixedWidthL(TUint aVal,TRadix aRadix,TInt aWidth)
	{
	ReserveFreeCapacityGrowExponentialL(aWidth);
	RBuf16::AppendNumFixedWidth(aVal, aRadix, aWidth);
	}

/**
Appends a zero terminator onto the end of this descriptor's data and returns 
a pointer to the data.

The length of the descriptor is not changed, but the capacity of the
descriptor may need to be grown to accommodate the zero terminator.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@return A pointer to the descriptor's zero terminated data.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C const TUint16 *LString16::PtrZL()
	{
	ReserveFreeCapacityL(1);
	return RBuf16::PtrZ();
	}

/**
Copies and folds data from the specified descriptor into this descriptor replacing 
any existing data.

The length of this descriptor is set to reflect the new 
data.

Note that folding is locale-independent behaviour. It is also important to 
note that there can be no guarantee that folding is in any way culturally 
appropriate, and should not be used when dealing with strings in natural
language.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aDes A 16-bit non-modifiable descriptor.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16::CopyFL(const TDesC16& aDes)
	{
	ReserveL(aDes.Length());
	RBuf16::CopyF(aDes);
	}

/**
Copies and collates data from the specified descriptor
into this descriptor replacing any existing data.

The length of this descriptor is set to reflect the new data.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aDes A 16-bit non-modifiable descriptor.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16::CopyCL(const TDesC16& aDes)
	{
	ReserveL(aDes.Length());
	RBuf16::CopyC(aDes);
	}

/**
Copies text from the specified descriptor and converts it to lower case before 
putting it into this descriptor, replacing any existing data.

The length of this descriptor is set to reflect the new data.

Conversion to lower case is implemented as appropriate to the current locale.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aDes A 16-bit non modifiable descriptor.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16::CopyLCL(const TDesC16& aDes)
	{
	ReserveL(aDes.Length());
	RBuf16::CopyLC(aDes);
	}

/**
Copies text from the specified descriptor and converts it to upper case before 
putting it into this descriptor, replacing any existing data.

The length of this descriptor is set to reflect the new data.

Conversion to upper case is implemented as appropriate to the current locale.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aDes A 16-bit non modifiable descriptor.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16::CopyUCL(const TDesC16& aDes)
	{
	ReserveL(aDes.Length());
	RBuf16::CopyUC(aDes);
	}

/**
Copies text from the specified descriptor and capitalises it before putting 
it into this descriptor, replacing any existing data.

The length of this descriptor is set to reflect the new data.

Capitalisation is implemented as appropriate to the current locale.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aDes A 16-bit non-modifiable descriptor.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16::CopyCPL(const TDesC16& aDes)
	{
	ReserveL(aDes.Length());
	RBuf16::CopyCP(aDes);
	}

/**
Appends and fills this descriptor with the specified character.

The descriptor is appended with the specified number of characters.
and its length is changed to reflect this.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aChar   The fill character. 
@param aLength The number of fill characters to be appended.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 11  if aLength is negative
*/
EXPORT_C void LString16::AppendFillL(TChar aChar,TInt aLength)
	{
	ReserveFreeCapacityGrowExponentialL(aLength);
	RBuf16::AppendFill(aChar, aLength);
	}

/**
Appends a zero terminator onto the end of this descriptor's data.

The length of the descriptor is not changed, but the capacity of the
descriptor may need to be grown to accommodate the zero terminator.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16::ZeroTerminateL()
	{
	ReserveFreeCapacityL(1);
	RBuf16::ZeroTerminate();
	}

/**
Swaps the data represented by this descriptor with the data represented by 
the specified descriptor.

The lengths of both descriptors are also swapped to reflect the change.

Note that each descriptor must be capable of accommodating the contents of
the other descriptor.

Each descriptor must be capable of accommodating the contents of the
other descriptor. If the maximum length of the descriptor parameter is
smaller than the length of the target LString16, then the function
raises a USER 11 panic. The target LString16 will be grown if
necessary to accommodate the descriptor parameter's data.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aDes The 16-bit modifiable descriptor whose data is to be swapped with 
            the data of this descriptor.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
            
@panic USER 11  if the maximum length of the descriptor parameter is smaller than the 
                length of the target LString16 
*/
EXPORT_C void LString16::SwapL(TDes16& aDes)
	{
	ReserveL(aDes.Length());
	TDes16::Swap(aDes);
	}

/**
Swaps the data represented by this string descriptor with the data
represented by the specified string descriptor.

The lengths of both string descriptors are also swapped to reflect the
change, and their buffers grown as necessary to accommodate the data
they receive.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aDes The 16-bit modifiable string descriptor whose data is to be swapped with 
            the data of this descriptor.

@leave KErrNoMemory if one of the underlying buffers needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16::SwapL(LString16& aDes)
	{
	this->ReserveL(aDes.Length());
	aDes.ReserveL(this->Length());
	TDes16::Swap(aDes);
	}


/**
Inserts data into this descriptor.

The length of this descriptor is changed to reflect the extra data.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aPos The position within the data where insertion is to start. This 
            is an offset value; a zero value refers to the leftmost data
            position.
            
@param aDes A 16-bit non modifiable descriptor whose data is to be inserted.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 10  if aPos is negative or is greater than the length of this
                descriptor.
*/
EXPORT_C void LString16::InsertL(TInt aPos,const TDesC16& aDes)
	{
	ReserveFreeCapacityGrowExponentialL(aDes.Length());
	RBuf16::Insert(aPos, aDes);
	}

/**
Replaces data in this descriptor.

The specified length can be different to the length of the replacement data.
The length of this descriptor changes to reflect the change of data.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aPos    The position within the data where replacement is to start. 
               This is an offset value; a zero value refers to the leftmost
               data position. 
            
@param aLength The length of data to be replaced.

@param aDes    The source 16-bit non modifiable descriptor whose data is to
               replace the target descriptor's data at aPos.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER  8  if aLength is negative 
               
@panic USER 10  if aPos is negative or is greater than the length of this
                descriptor.
                
@panic USER 16  if the length of the source descriptor aDes is negative 
*/
EXPORT_C void LString16::ReplaceL(TInt aPos,TInt aLength,const TDesC16& aDes)
	{
	TInt delta = aDes.Length() - aLength;
	if (delta > 0)
		{
		ReserveFreeCapacityGrowExponentialL(delta);
		}
	RBuf16::Replace(aPos, aLength, aDes);
	}

/**
Copies data into this descriptor and justifies it, replacing any existing data.

The length of this descriptor is set to reflect the new data.

The target area is considered to be an area of specified width positioned at
the beginning of this descriptor's data area. Source data is copied into, and
aligned within this target area according to the specified alignment
instruction.

If the length of the target area is larger than the length of the source, then
spare space within the target area is padded with the fill character.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aDes        A 16-bit non-modifiable descriptor containing the source data.
                   The length of the data to be copied is the smaller of:
                   the length of the source descriptor, and 
                   the width of the target area (only if this is not the
                   explicit negative value KDefaultJustifyWidth).

@param aWidth      The width of the target area. If this has the specific
                   negative value KDefaultJustifyWidth, then the width is
                   re-set to the length of the data source.

@param aAlignment The alignment of the data within the target area

@param aFill       The fill character used to pad the target area. 

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 11  if aWidth has a negative value other than KDefaultJustifyWidth.
*/
EXPORT_C void LString16::JustifyL(const TDesC16& aDes,TInt aWidth,TAlign aAlignment,TChar aFill)
	{
	TInt width = (aWidth == KDefaultJustifyWidth ? aDes.Length() : aWidth);
	ReserveL(width);
	RBuf16::Justify(aDes, aWidth, aAlignment, aFill);
	}

/** 
Converts the specified unsigned integer into a fixed width character
representation based on the specified number system and copies the conversion
into this descriptor, replacing any existing data.

The length of this descriptor is set to reflect the new data.

The function generates the exact number of specified characters, either padding 
to the left with character zeroes or discarding low order characters as
necessary.

When a hexadecimal conversion is specified, hexadecimal characters are in 
upper case.

This function is equivalent to using Format() with parameters which specify:

1. a fixed length target field

2. padding with zero characters, for example "%08x".

When this is the case, always use NumFixedWidthUC() in 
preference to Format() as it is more efficient.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aVal   The unsigned integer value. 
@param aRadix The number system representation for the unsigned integer. 
@param aWidth The number of characters: to be used to contain the conversion, 
              to be copied into this descriptor.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
              
@see TDes16::Format()
*/
EXPORT_C void LString16::NumFixedWidthUCL(TUint aVal,TRadix aRadix,TInt aWidth)
	{
	Zero();
	AppendNumFixedWidthUCL(aVal, aRadix, aWidth);
	}

/**
Converts the specified 64 bit unsigned integer into a character representation 
based on the specified number system and copies the conversion into this
descriptor, replacing any existing data.

The length of this descriptor is set to reflect the new data.

When a hexadecimal conversion is specified, hexadecimal characters are in 
upper case.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aVal   The 64 bit integer value. This is always treated as an unsigned
              value for all builds. 
@param aRadix The number system representation for the 64 bit integer. If no 
              explicit value is specified, then EDecimal is the default.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16::NumUCL(TUint64 aVal, TRadix aRadix)
	{
	Zero();
	AppendNumUCL(aVal, aRadix);
	}

/**
Converts the specified floating point number into a character representation 
and copies the conversion into this descriptor, replacing any existing data.

The length of this descriptor is set to reflect the new data.
	
The character representation of the real number is dictated by the specified 
format.

Note that the function leaves if the iType data member of the specified
TRealFormat object has both an invalid character representation format
(i.e. the format type) and invalid format flags.	        

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aVal    The floating point number to be converted. 
@param aFormat The format of the conversion. 

@return If the conversion is successful, the length of this descriptor. If 
        the conversion fails, a negative value indicating the cause of failure.
        In addition, extra information on the cause of the failure may be
        appended onto this descriptor. The possible values and their meaning
        are:
        
        1.KErrArgument - the supplied floating point number is not a valid
          number. The three characters NaN are appended to this descriptor.
          
        2.KErrOverflow - the number is too large to represent.
        2.1 For positive overflow, the three characters Inf are appended 
            to this descriptor.
        2.2 For negative overflow, the four characters -Inf are appended 
	        to this descriptor.
	        
	    3.KErrUnderflow - the number is too small to represent.
	    3.1 For positive underflow, the three characters Inf are appended
	        to this descriptor. 
        3.2	For negative underflow, the four characters -Inf are appended
            to this descriptor. 
	    
	    4.KErrGeneral - the conversion cannot be completed. There are a
	      number of possible reasons for this, but the most common is:
	    4.1 The character representation format (i.e. the format type), as
	        defined in the TRealFormat object is not recognised.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
	        
@see TRealFormat::iType
*/
EXPORT_C TInt LString16::NumL(TReal aVal,const TRealFormat &aFormat)
	{
	Zero();
	return AppendNumL(aVal, aFormat);
	}

/**
Converts the 64-bit signed integer into a decimal character representation 
and copies the conversion into this descriptor, replacing any existing data. 

The length of this descriptor is set to reflect the new data.

If the integer is negative, the character representation is prefixed by a 
minus sign.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aVal The 64-bit signed integer value.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16::NumL(TInt64 aVal)
	{
	Zero();
	AppendNumL(aVal);
	}

/**
Converts the specified 64 bit unsigned integer into a character representation 
based on the specified number system and copies the conversion into this
descriptor, replacing any existing data.

The length of this descriptor is set to reflect the new data.
	
When a hexadecimal conversion is specified, hexadecimal characters are in 
lower case.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.
	
@param aVal   The 64 bit integer value. This is treated as an unsigned
              value for all builds. 
@param aRadix The number system representation for the 64 bit integer.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16::NumL(TUint64 aVal, TRadix aRadix)
	{
	Zero();
	AppendNumL(aVal, aRadix);
	}

/**
Formats and copies text into this descriptor, replacing any existing data.

The length of this descriptor is set to reflect the new data.

The function takes a format string and a variable number of arguments.
The format string contains literal text embedded with directives for converting
the trailing list of arguments into text.

The embedded directives are character sequences prefixed with the '%' character.
The literal text is simply copied into this descriptor unaltered while
the '%' directives are used to convert successive arguments from the
trailing list.

The resulting stream of literal text and converted arguments is copied into
this descriptor.

The syntax of the embedded directives follows one of four general patterns.

Note that formatting of single numerical values can be achieved more
conveniently using the Num() and NumUC() member functions of this class.

The full description of the syntax of a format string cannot be	included here.
For full details, navigate to the Symbian OS guide, and follow the hierarchy of links:

@code
Symbian OS Guide
	Base
		Using  User Library (E32)
			Buffers and Strings
				Using Descriptors
					How to Use Descriptors
						Format string syntax
@endcode

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aFmt The descriptor containing the format string.
            The TRefByValue class provides a constructor which takes a
            TDesC8 type.

@param ...  A variable number of arguments to be converted to text as
            dictated by the format string. 

@panic USER 12  if the format string has incorrect syntax.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@see TDes16::Num()
@see TDes16::NumUC()
*/
EXPORT_C void LString16::FormatL(TRefByValue<const TDesC16> aFmt,...)
	{
    VA_LIST list;
    VA_START(list,aFmt);
    FormatListL(aFmt,list);
	}

/**
Formats and copies text into this descriptor, replacing any existing data.

The length of this descriptor is set to reflect the new data.

The behaviour of this function is the same as FormatL(). In practice, it is 
better and easier to use FormatL(), passing a variable number of arguments 
as required by the format string.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aFmt  The descriptor containing the format string.
@param aList A pointer to an argument list.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@see TDes16::Format()
@see VA_LIST
*/
EXPORT_C void LString16::FormatListL(const TDesC16& aFmt,VA_LIST aList)
	{
	Zero();
	AppendFormatListL(aFmt, aList);
	}

/**
Appends data onto the end of this descriptor's data and justifies it.
	
The source of the appended data is an existing descriptor.
	
The target area is considered to be an area of specified width, immediately 
following this descriptor's existing data. Source data is copied into, and 
aligned within this target area according to the specified alignment instruction.
	
If the length of the target area is larger than the length of the source, 
then spare space within the target area is padded with the fill character.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.
		
@param aDes        A 16-bit non-modifiable descriptor containing the source
                   data. The length of the data to be copied is the smaller of:
                   the length of the source descriptor, and
                   the width of the target area (only if this is not the
                   explicit negative value KDefaultJustifyWidth). 
	
@param aWidth      The width of the target area. If this has the specific
                   negative value KDefaultJustifyWidth, then the width is
	               re-set to the length of the data source.
	
@param aAlignment The alignment of the data within the target area. 
	
@param aFill       The fill character used to pad the target area.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 11  if aWidth has a negative value other than KDefaultJustifyWidth.
*/
EXPORT_C void LString16::AppendJustifyL(const TDesC16& Des,TInt aWidth,TAlign aAlignment,TChar aFill)
	{
	
	TInt width = (aWidth == KDefaultJustifyWidth ? Des.Length() : aWidth);
	ReserveFreeCapacityGrowExponentialL(width);
	RBuf16::AppendJustify(Des, aWidth, aAlignment, aFill);
	}

/**
Appends data onto the end of this descriptor's data and justifies it.
	
The source of the appended data is an existing descriptor.
	
The target area is considered to be an area of specified width, immediately 
following this descriptor's existing data. Source data is copied into, and 
aligned within this target area according to the specified alignment instruction.
	
If the length of the target area is larger than the length of the source, 
then spare space within the target area is padded with the fill character.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.
	
@param aDes        An 8-bit non-modifiable descriptor containing the source data. 

@param aLength     The length of data to be copied from the source descriptor. 
                   If this is greater than the width of the target area, then
                   the length of data copied is limited to the width.
                   The length of data to be copied must not be 	greater than
                   the length of the source descriptor. Note that this
                   condition is not automatically tested. 
                   
@param aWidth      The width of the target area. If this has the specific negative 
                   value KDefaultJustifyWidth, then the width is
                   re-set to the length of the data source.

@param aAlignment The alignment of the data within the target area. 

@param aFill       The fill character used to pad the target area.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 11  if aWidth has a negative value other than KDefaultJustifyWidth.
*/
EXPORT_C void LString16::AppendJustifyL(const TDesC16& Des,TInt aLength,TInt aWidth,TAlign aAlignment,TChar aFill)
	{
	
	TInt width = (aWidth == KDefaultJustifyWidth ? aLength : aWidth);
	ReserveFreeCapacityGrowExponentialL(width);
	
	RBuf16::AppendJustify(Des, aLength, aWidth, aAlignment, aFill);
	}

/**
Appends a zero terminated string onto the end of this descriptor's data and 
justifies it.

The zero terminator is not copied.

The target area is considered to be an area of specified width, immediately 
following this descriptor's existing data. Source data is copied into, and 
aligned within, this target area according to the specified alignment instruction.

If the length of the target area is larger than the length of the source, 
then spare space within the target area is padded with the fill character.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aZeroTerminatedString     A pointer to a zero terminated string The length of the data 
                   to be copied is the smaller of: the length of the string (excluding the zero 
                   terminator), the width of the target area (only if this is not the explicit 
                   negative value KDefaultJustifyWidth). 
                    
@param aWidth      The width of the target area. If this has the specific negative 
                   value KDefaultJustifyWidth, then the width is re-set to the length of the 
                   zero terminated string (excluding the zero terminator).
                    
@param aAlignment The alignment of the data within the target area. 

@param aFill       The fill character used to pad the target area.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 11  if aWidth has a negative value other than KDefaultJustifyWidth.
*/
EXPORT_C void LString16::AppendJustifyL(const TUint16* aZeroTerminatedString,TInt aWidth,TAlign aAlignment,TChar aFill)
	{
	
	TInt width = (aWidth == KDefaultJustifyWidth ? User::StringLength(aZeroTerminatedString) : aWidth);
	ReserveFreeCapacityGrowExponentialL(width);
	
	RBuf16::AppendJustify(aZeroTerminatedString, aWidth, aAlignment, aFill);

	}

/**
Appends data onto the end of this descriptor's data and justifies it.

The source of the appended data is a memory location.

The target area is considered to be an area of specified width, immediately 
following this descriptor's existing data. Source data is copied into, and 
aligned within, this target area according to the specified alignment instruction.

If the length of the target area is larger than the length of the source, 
then spare space within the target area is padded with the fill character.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aString     A pointer to a source memory location. 

@param aLength     The length of data to be copied. If this is greater than the 
                   width of the target area, then the length of data copied is
                   limited to the width.
               
@param aWidth      The width of the target area. If this has the specific negative 
                   value KDefaultJustifyWidth, then the width is
                   re-set to the length of the data source. 
               
@param aAlignment The alignment of the data within the target area. 

@param aFill       The fill character used to pad the target area.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 11  if aWidth has a negative value other than KDefaultJustifyWidth.
                
@panic USER 17  if aLength is negative.  
*/
EXPORT_C void LString16::AppendJustifyL(const TUint16* aString,TInt aLength,TInt aWidth,TAlign aAlignment,TChar aFill)
	{
	
	TInt width = (aWidth == KDefaultJustifyWidth ? aLength : aWidth);
	ReserveFreeCapacityGrowExponentialL(width);
	
	RBuf16::AppendJustify(aString, aLength, aWidth, aAlignment, aFill);
	}

/**
Converts the specified unsigned integer into a fixed width character
representation based on the specified number system and appends the conversion
onto the end of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.

The function generates the exact number of specified characters, either
padding to the left with character zeroes or discarding low order characters
as necessary.

When a hexadecimal conversion is specified, hexadecimal characters are in 
upper case.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aVal   The unsigned integer value. 
@param aRadix The number system representation for the unsigned integer. 
@param aWidth The number of characters: to be used to contain the conversion, 
              to be appended to this descriptor.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

*/
EXPORT_C void LString16::AppendNumFixedWidthUCL(TUint aVal,TRadix aRadix,TInt aWidth)
	{
	ReserveFreeCapacityGrowExponentialL(aWidth);
	RBuf16::AppendNumFixedWidthUC(aVal, aRadix, aWidth);
	}

/**
Converts the specified 64 bit integer into a character representation 
based on the specified number system and appends the conversion onto the end 
of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.
	
When a hexadecimal conversion is specified, hexadecimal characters are in 
upper case.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.
	
@param aVal   The 64 bit integer value. This is always treated as an unsigned
              value. 
@param aRadix The number system representation for the 64 bit integer. If no 
              explicit value is specified, then EDecimal is the default.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16::AppendNumUCL(TUint64 aVal, TRadix aRadix)
	{
	ReserveFreeCapacityGrowExponentialL(64 + 1 + KDefaultExpandSize);
	RBuf16::AppendNumUC(aVal, aRadix);
	}

/**
Converts the specified floating point number into a character representation 
and appends the conversion onto the end of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.
	
The character representation of the real number is dictated by the specified 
format.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.
	
@param aVal    The floating point number to be converted. 
@param aFormat The format of the conversion. 

@return If the conversion is successful, the length of this descriptor. If 
        the conversion fails, a negative value indicating the cause of failure.
        In addition, extra information on the cause of the failure may be
        appended onto this descriptor. The possible values and their meaning
        are:
        
        1.KErrArgument - the supplied floating point number is not a valid
          number. The three characters NaN are appended to this descriptor.
          
        2.KErrOverflow - the number is too large to represent.
        2.1 For positive overflow, the three characters Inf are appended 
            to this descriptor.
        2.2 For negative overflow, the four characters -Inf are appended 
	        to this descriptor.
	        
	    3.KErrUnderflow - the number is too small to represent.
	    3.1 For positive underflow, the three characters Inf are appended
	        to this descriptor. 
        3.2	For negative underflow, the four characters -Inf are appended
            to this descriptor. 
	    
	    4.KErrGeneral - the conversion cannot be completed. There are a
	      number of possible reasons for this, but the most common is:
	    4.1 The character representation format (i.e. the format type), as
	        defined in the TRealFormat object is not recognised.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C TInt LString16::AppendNumL(TReal aVal,const TRealFormat& aFormat) 
	{
	ReserveFreeCapacityGrowExponentialL(aFormat.iWidth + 1 + KDefaultExpandSize);
	return RBuf16::AppendNum(aVal, aFormat);
	}

/**
Converts the 64-bit signed integer into a decimal character representation 
and appends the conversion onto the end of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.

If the integer is negative, the character representation is prefixed by a 
minus sign.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aVal The 64-bit signed integer value.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16::AppendNumL(TInt64 aVal)
	{
	ReserveFreeCapacityGrowExponentialL(64 + 1 + KDefaultExpandSize); 
	RBuf16::AppendNum(aVal);
	}

/**
Converts the specified 64 bit integer into a character representation 
based on the specified number system and appends the conversion onto the end 
of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.
	
When a hexadecimal conversion is specified, hexadecimal characters are in 
lower case.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.
	
@param aVal   The 64 bit integer value. This is always treated as an unsigned
              value. 
@param aRadix The number system representation for the 64 bit integer.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16::AppendNumL(TUint64 aVal, TRadix aRadix)
	{
	ReserveFreeCapacityGrowExponentialL(64 + 1 + KDefaultExpandSize); 
	RBuf16::AppendNum(aVal, aRadix);
	}

/**
Formats and appends text onto the end of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.

The function takes a format string and a variable number of arguments.
The format string contains literal text, embedded with directives,
for converting the trailing list of arguments into text.

The embedded directives are character sequences prefixed with the '%' character.
The literal text is simply copied into this descriptor unaltered while
the '%' directives are used to convert successive arguments from the
trailing list. See the description of the Format() function.

Literal text is appended on a character by character basis, and the
underlying buffer is grown as necessary to accommodate it.

Text converted from a trailing argument is appended as a complete
string, and the underlying buffer is grown as necessary to accommodate
it.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.
  
@param aFmt             The 16-bit non-modifiable descriptor containing the
                        format string. The TRefByValue class provides a
                        constructor which takes a TDesC16 type. 

@param ...              A variable number of arguments to be converted to text
                        as dictated by the format string. 

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 12  if the format string has incorrect syntax.

@see TDes16::Format()
@see TDes16Overflow::Overflow()
*/
EXPORT_C void LString16::AppendFormatL(TRefByValue<const TDesC16> aFmt,...)
	{
    VA_LIST list;
    VA_START(list,aFmt);
    AppendFormatListL(aFmt,list);
	}

class TRetryOverflow16 : public TDes16Overflow
	{
public:
	TRetryOverflow16() : iOverflow(EFalse)
		{
		}

	virtual void Overflow(TDes16& /*aDes*/)
		{
		iOverflow = ETrue;
		}

	TBool iOverflow;
	};

/**
Formats and appends text onto the end of this descriptor's data.
	
The length of this descriptor is incremented to reflect the new content.
	
The behaviour of this function is the same as
AppendFormatL(TRefByValue<const TDesC16> aFmt,TDes16Overflow *aOverflowHandler,...).
In practice, it is better and easier to use AppendFormat(), passing a variable number of 
arguments as required by the format string.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.
	
@param aFmt          The descriptor containing the format string.
@param aList            A pointer to an argument list.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@see TDes16::AppendFormat
@see VA_LIST 
*/
EXPORT_C void LString16::AppendFormatListL(const TDesC16& aFmt,VA_LIST aList)
	{
	ReserveFreeCapacityGrowExponentialL(aFmt.Length() + KDefaultExpandSize); // We use aFmt as a hint
	for (;;)
		{
		TInt before = Length();
		TRetryOverflow16 overflow;
		RBuf16::AppendFormatList(aFmt, aList, &overflow);
		if (overflow.iOverflow)
			{
			SetLengthL(before); // Can't leave
			ReserveCapacityGrowExponentialL();
			}
		else
			{
			break;
			}
		}
	}


/**
Unlinks and transfers ownership of the specified 16-bit resizable descriptor's 
buffer to this object. The source descriptor is detached from the buffer. 

@param aString The source 16-bit resizable buffer. The ownership of this
             object's buffer is to be transferred.

@see RBuf16::Close()
*/
EXPORT_C void LString16::Assign(const LString16& aString)
	{
	// free any previously owned resource
	Reset();
	
	RBuf16::Assign(aString);
	// unlink buffer from original descriptor 
	new (const_cast<LString16*>(&aString)) LString16();
	}


/**
Transfers ownership of the specified 16-bit resizable descriptor's 
buffer to this object. The source descriptor is detached from the buffer. 

@param aRBuf The source 16-bit resizable buffer. The ownership of this
             object's buffer is to be transferred.

@see RBuf16::Assign()
*/

EXPORT_C void LString16::Assign(const RBuf16& aRBuf)
	{
	// free any previously owned resource
	Reset();
	
	RBuf16::Assign(aRBuf);

	// reset the RBuf;
	new (const_cast<RBuf16*>(&aRBuf)) RBuf16();
	}


/**
Transfers ownership of the specified 16-bit resizable descriptor's this object. 

@param aHBuf The heap descriptor to be transferred to this object. 
			 The ownership of this object's buffer is to be transferred.

@see RBuf16::Assign()
*/
EXPORT_C void LString16::Assign(HBufC16* aHBuf)
	{
	// free any previously owned resource
	Reset();
	
	RBuf16::Assign(aHBuf);
	}
	

/**
Assigns ownership of the specified allocated memory to this object.

@param aHeapCell The allocated memory to be assigned to this object. 
				This pointer can be NULL, which means that a zero length 
				16-bit resizable buffer descriptor is created.
				
@param aMaxLength The maximum length of the descriptor.
             
@panic USER 8 If the specified maximum length is greater then the size of the 
				allocated heap cell, or the specified maximum length is NOT 
				zero when the pointer to the heap cell is NULL.

@see RBuf16::Close()
@see RBuf16::Assign()
*/
EXPORT_C void LString16::Assign(TUint16* aHeapCell, TInt aMaxLength)
	{
	// free any previously owned resource
	Reset();
	
	RBuf16::Assign(aHeapCell, aMaxLength);
	}


/**
Transfers ownership of the specified 16-bit resizable descriptor's this object. 

@param aHeapCell The allocated memory to be assigned to this object.
                     
@param aLength The length of the descriptor.

@param aMaxLength The maximum length of the descriptor.
             
@panic USER 8 If the specified maximum length is greater then the size of the 
				allocated heap cell, or the specified length is greater then 
				the specified maximum length, or the specified maximum length 
				is NOT zero when the pointer to the heap cell is NULL.

@see RBuf16::Close()
@see RBuf16::Assign()
*/
EXPORT_C void LString16::Assign(TUint16* aHeapCell,TInt aLength,TInt aMaxLength)
	{
	// free any previously owned resource
	Reset();
	
	RBuf16::Assign(aHeapCell, aLength, aMaxLength);
	}

/**
Creates a 16-bit resizable buffer descriptor that has been initialised with
data from the specified read stream; leaves on failure.
			 
Data is assigned to the new descriptor from the specified stream.
This variant assumes that the stream contains the length of the data followed
by the data itself.

The function is implemented by calling the HBufC16::NewL(RReadStream&amp;,TInt)
variant and then assigning the resulting heap descriptor using
the RBuf16::Assign(HBufC16*) variant. The comments that describe
the HBufC16::NewL() variant	also apply to this RBuf16::CreateL() function.

The function may leave with one of the system-wide error codes,	specifically 
KErrOverflow, if the length of the data as read from the stream is greater than
the upper limit as specified by the aMaxLength parameter.

@param aStream    The stream from which the data length and the data to be
                  assigned to the new descriptor, are taken.
@param aMaxLength The upper limit on the length of data that the descriptor is
                  to represent. The value of this parameter must be non-negative
                  otherwise the	underlying function will panic.
*/
EXPORT_C void LString16::CreateL(RReadStream &aStream,TInt aMaxLength)
	{
	Reset();
	Assign(HBufC16::NewL(aStream,aMaxLength));
	}

/**
Appends data onto the end of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aZeroTerminatedString     A pointer to a zero terminated string .
@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@see LString16::AppendL
*/
EXPORT_C LString16& LString16::operator+=(const TUint16* aZeroTerminatedString)
	{
	AppendL(aZeroTerminatedString,User::StringLength(aZeroTerminatedString));
	return *this;
	}
/**
Appends data onto the end of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aZeroTerminatedString    A pointer to the data to be copied.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 17  if aLength is negative.
*/
EXPORT_C void LString16::AppendL(const TUint16* aZeroTerminatedString)
	{
	AppendL(aZeroTerminatedString,User::StringLength(aZeroTerminatedString));
	}

/**
Constructor to create a 16-bit resizable string descriptor containing
a copy of the specified (source) zero-terminated wide character string data, or leave
on failure.

The constructor allocates sufficient memory so that this string
descriptor's maximum length is the same as the length of the source
string. Both the current length and the maximum length of this string
descriptor are set to the length of the source string. 

The data contained in the source string is copied into this string
descriptor. The zero terminator is not copied.

@param aWideCharStr A pointer to a zero-terminated wide character string

@leave KErrNoMemory If there is insufficient memory.

@see LString16::CopyL
*/
EXPORT_C LString16::LString16(const wchar_t* aWideCharStr)
	: iReserved(0)
	{
	CopyL(reinterpret_cast<const TUint16*>(aWideCharStr));
	}

/**
Copies data into this 16-bit string descriptor, replacing any existing
data, and expanding its heap buffer to accommodate if necessary.

The length of this descriptor is set to reflect the new data.

This operation may cause the target string descriptor's heap buffer to
be reallocated in order to accommodate the new data. As a result,
MaxLength() and Ptr() may return different values afterwards, and any
existing raw pointers to into the descriptor data may be invalidated.

Note that the automatic resizing performed is a change to the
functionality of this operation compared to other descriptor
classes. This change is only active on objects directly declared
LString16; when LString16 instances are instead manipulated via
references to TDes16 or TDesC16, the standard (non-resizing, panicing)
variant is invoked.

@param aWideCharStr A pointer to a wide character zero-terminated string

@return A reference to this 16-bit string descriptor.

@leave KErrNoMemory If the heap buffer of the string descriptor being
              assigned to needs to be expanded, but there is
              insufficient memory to do so

@see LString16::CopyL
*/
EXPORT_C LString16& LString16::operator=(const wchar_t* aWideCharStr)
	{
	CopyL(reinterpret_cast<const TUint16*>(aWideCharStr));
	return *this;	
	}

/**
Appends data onto the end of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aWideCharStr     A pointer to a wide character zero terminated string .
@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@see LString16::AppendL
*/
EXPORT_C LString16& LString16::operator+=(const wchar_t*  aWideCharStr)
	{
	AppendL(reinterpret_cast<const TUint16*>(aWideCharStr),User::StringLength(reinterpret_cast<const TUint16*>(aWideCharStr)));
	return *this;
	}

/**
Copies data into this 16-bit string descriptor, replacing any existing
data, and expanding its heap buffer to accommodate if necessary.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aWideCharStr    A pointer to a wide character zero terminated string to be copied. 


@leave KErrNoMemory If the heap buffer of the string descriptor being
              assigned to needs to be expanded, but there is
              insufficient memory to do so

@panic USER 11  if aLength is negative.

@see TDes16::Copy
*/
EXPORT_C void LString16::CopyL(const wchar_t*  aWideCharStr)
	{
	CopyL(reinterpret_cast<const TUint16*>(aWideCharStr));	
	}

/**
Appends data onto the end of this descriptor's data and justifies it.

The source of the appended data is a memory location.

The target area is considered to be an area of specified width, immediately 
following this descriptor's existing data. Source data is copied into, and 
aligned within, this target area according to the specified alignment instruction.

If the length of the target area is larger than the length of the source, 
then spare space within the target area is padded with the fill character.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aWideCharStr     A pointer to a source memory location. 

@param aLength     The length of data to be copied. If this is greater than the 
                   width of the target area, then the length of data copied is
                   limited to the width.
               
@param aWidth      The width of the target area. If this has the specific negative 
                   value KDefaultJustifyWidth, then the width is
                   re-set to the length of the data source. 
               
@param aAlignment The alignment of the data within the target area. 

@param aFill       The fill character used to pad the target area.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 11  if aWidth has a negative value other than KDefaultJustifyWidth.
                
@panic USER 17  if aLength is negative.  
*/
EXPORT_C void LString16::AppendJustifyL(const wchar_t* aWideCharStr,TInt aWidth,TAlign anAlignment,TChar aFill)
	{
	AppendJustifyL(reinterpret_cast<const TUint16*>( aWideCharStr),aWidth,anAlignment,aFill);
	}

/**
Appends data onto the end of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aWideCharStr    A pointer to the data to be copied.
@param aLength The length of data to be copied.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 17  if aLength is negative.
*/
EXPORT_C void LString16::AppendL(const wchar_t* aWideCharStr,TInt aLength)
	{
	AppendL(reinterpret_cast<const TUint16*>(aWideCharStr),aLength);
	}
/**
Appends data onto the end of this descriptor's data.

The length of this descriptor is incremented to reflect the new content.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aWideCharStr    A pointer to the data to be copied.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 17  if aLength is negative.
*/
EXPORT_C void LString16::AppendL(const wchar_t* aWideCharStr)
	{
	AppendL(reinterpret_cast<const TUint16*>(aWideCharStr));
	}
/**
Determines whether this Descriptor's data is equal to the specified
string's data.

The comparison is implemented internally using the TDesC::Compare() function.

@param aWideCharStr The wide character string whose data is to be compared 
            with this Descriptor's data. 
            
@return True if equal, false otherwise. 

@see TDesC::Compare
*/
EXPORT_C TBool LString16::operator==( const wchar_t* aWideCharStr) const
	{
	return LString16::operator==(reinterpret_cast<const TUint16*>(aWideCharStr));
	}

/**
Determines whether this Descriptor's data is equal to the specified
string's data.

The comparison is implemented internally using the TDesC::Compare() function.

@param aZeroTerminatedString The wide Zero TerminatedString string whose data 
is to be compared with this Descriptor's data. 
            
@return True if equal, false otherwise. 

@see TDesC::Compare
*/
EXPORT_C TBool LString16::operator==( const TUint16* aZeroTerminatedString) const
	{
	return RBuf16::operator==(TPtrC16((aZeroTerminatedString), User::StringLength(aZeroTerminatedString)));
	}

/**
Determines whether this descriptor's data is less than the specified
strings's data.

The comparison is implemented internally using the TDesC::Compare() function.

@param aWideCharStr The wide character string whose data is to be compared 
            with this Descriptor's data. 
            
@return True if this descriptor's data is less than that of the specified string's data

@see TDesC::Compare
*/
EXPORT_C TBool LString16::operator<( const wchar_t* aWideCharStr) const
	{
	return LString16::operator<(reinterpret_cast<const TUint16*>(aWideCharStr));
	}

/**
Determines whether this descriptor's data is less than the specified
strings's data.

The comparison is implemented internally using the TDesC::Compare() function.

@param aZeroTerminatedString The wide Zero TerminatedString string whose data 
is to be compared with this Descriptor's data. 
            
@return True if this descriptor's data is less than that of the specified string's data

@see TDesC::Compare
*/
EXPORT_C TBool LString16::operator<(const TUint16* aZeroTerminatedString) const
	{
	return RBuf16::operator<(TPtrC16((aZeroTerminatedString), User::StringLength(aZeroTerminatedString)));
	}

/**
Determines whether this descriptor's data is less than the specified
strings's data.

The comparison is implemented internally using the TDesC::Compare() function.

@param aWideCharStr The wide character string whose data is to be compared 
            with this Descriptor's data. 
            
@return True if this descriptor's data is less than that of the specified string's data

@see TDesC::Compare
*/
EXPORT_C TBool LString16::operator<=( const wchar_t* aWideCharStr) const
	{
	return LString16::operator<=(reinterpret_cast<const TUint16*>(aWideCharStr));
	}

/**
Determines whether this descriptor's data is less than/equal to the specified
strings's data.

The comparison is implemented internally using the TDesC::Compare() function.

@param aZeroTerminatedString The wide Zero TerminatedString string whose data 
is to be compared with this Descriptor's data. 
            
@return True if this descriptor's data is less than/equal to that of the specified string's data

@see TDesC::Compare
*/
EXPORT_C TBool LString16::operator<=(const TUint16* aZeroTerminatedString) const
	{
	return RBuf16::operator<=(TPtrC16((aZeroTerminatedString), User::StringLength(aZeroTerminatedString)));
	}

/**
Determines whether this descriptor's data is greater than the specified
strings's data.

The comparison is implemented internally using the TDesC::Compare() function.

@param aWideCharStr The wide character string whose data is to be compared 
            with this Descriptor's data. 
            
@return True if this descriptor's data is greater than that of the specified string's data

@see TDesC::Compare
*/
EXPORT_C TBool LString16::operator>( const wchar_t* aWideCharStr) const
	{
	return LString16::operator>(reinterpret_cast<const TUint16*>(aWideCharStr));
	}

/**
Determines whether this descriptor's data is greater than the specified
strings's data.

The comparison is implemented internally using the TDesC::Compare() function.

@param aZeroTerminatedString The wide Zero TerminatedString string whose data 
is to be compared with this Descriptor's data. 
            
@return True if this descriptor's data is greater than that of the specified string's data

@see TDesC::Compare
*/
EXPORT_C TBool LString16::operator>(const TUint16* aZeroTerminatedString) const
	{
	return RBuf16::operator>(TPtrC16((aZeroTerminatedString), User::StringLength(aZeroTerminatedString)));
	}

/**
Determines whether this descriptor's data is greater than/equal to the specified
strings's data.

The comparison is implemented internally using the TDesC::Compare() function.

@param aWideCharStr The wide character string whose data is to be compared 
            with this Descriptor's data. 
              
@return True if this descriptor's data is greater than/equal to that of the specified string's data

@see TDesC::Compare
*/
EXPORT_C TBool LString16::operator>=( const wchar_t* aWideCharStr) const
	{
	return LString16::operator>=(reinterpret_cast<const TUint16*>(aWideCharStr));
	}

/**
Determines whether this descriptor's data is greater than the specified
strings's data.

The comparison is implemented internally using the TDesC::Compare() function.

@param aZeroTerminatedString The wide Zero TerminatedString string whose data 
is to be compared with this Descriptor's data. 
            
@return True if this descriptor's data is greater than that of the specified string's data

@see TDesC::Compare
*/
EXPORT_C TBool LString16::operator>=(const TUint16* aZeroTerminatedString) const
	{
	return RBuf16::operator>=(TPtrC16((aZeroTerminatedString), User::StringLength(aZeroTerminatedString)));
	}

/**
Determines whether this descriptor's data is not equal to the specified
strings's data.

The comparison is implemented internally using the TDesC::Compare() function.

@param aWideCharStr The wide character string whose data is to be compared 
            with this Descriptor's data.  
            
@return True if this descriptor's data is not equal to the specified string's data

@see TDesC::Compare
*/
EXPORT_C TBool LString16::operator!=( const wchar_t* aWideCharStr) const
	{
	return LString16::operator!=(reinterpret_cast<const TUint16*>(aWideCharStr));
	}

/**
Determines whether this descriptor's data is not equal to the specified
strings's data.

The comparison is implemented internally using the TDesC::Compare() function.

@param aZeroTerminatedString The wide Zero TerminatedString string whose data 
is to be compared with this Descriptor's data. 
            
@return True if this descriptor's data is not equal to the specified string's data

@see TDesC::Compare
*/
EXPORT_C TBool LString16::operator!=(const TUint16* aZeroTerminatedString) const
	{
	return RBuf16::operator!=(TPtrC16((aZeroTerminatedString), User::StringLength(aZeroTerminatedString)));
	}

/**
Searches this descriptor's data for a match with the match pattern supplied 
in the specified string.

The match pattern can contain the wildcard characters "*" and "?", where "*" 
matches zero or more consecutive occurrences of any character and "?" matches 
a single occurrence of any character.

Note that there is no 'escape character', which means that it is not possible
to match either the "*" character itself or the "?" character itself using
this function.

@param aWideCharStr The wide character string whose data is to be matched 
            with this Descriptor's data.

@return If a match is found, the offset within this descriptor's data where 
        the match first occurs. KErrNotFound, if there is no match.
*/
EXPORT_C TInt LString16::Match(const wchar_t* aWideCharStr) const
	{
	return LString16::Match(reinterpret_cast<const TUint16*>(aWideCharStr));
	}

/**
Searches this descriptor's data for a match with the match pattern supplied 
in the specified string.

The match pattern can contain the wildcard characters "*" and "?", where "*" 
matches zero or more consecutive occurrences of any character and "?" matches 
a single occurrence of any character.

Note that there is no 'escape character', which means that it is not possible
to match either the "*" character itself or the "?" character itself using
this function.

@param aZeroTerminatedString The wide Zero TerminatedString string whose data 
is to be matched with this Descriptor's data. 

@return If a match is found, the offset within this descriptor's data where 
        the match first occurs. KErrNotFound, if there is no match.
*/
EXPORT_C TInt LString16::Match(const TUint16* aZeroTerminatedString) const
	{
	return RBuf16::Match(TPtrC16((aZeroTerminatedString), User::StringLength(aZeroTerminatedString)));
	}
/**
Searches this descriptor's folded data for a match with the folded match 
pattern supplied in the specified string.

The match pattern can contain the wildcard characters "*" and "?", where "*" 
matches zero or more consecutive occurrences of any character and "?" matches 
a single occurrence of any character.

Note that folding is locale-independent behaviour. It is also important to 
note that there can be no guarantee that folding is in any way culturally 
appropriate, and should not be used for matching strings in natural language; 
use MatchC() for this.

Note that there is no 'escape character', which means that it is not possible
to match either the "*" character itself or the "?" character itself using
this function.

@param aWideCharStr The wide character string whose data is to be matched 
            with this Descriptor's data.

@return If a match is found, the offset within this descriptor's data where 
        the match first occurs. KErrNotFound, if there is no match. 

@see TDesC::MatchC()
*/
EXPORT_C TInt LString16::MatchF(const wchar_t* aWideCharStr) const
	{
	return LString16::MatchF(reinterpret_cast<const TUint16*>(aWideCharStr));
	}
/**
Searches this descriptor's folded data for a match with the folded match 
pattern supplied in the specified string.

The match pattern can contain the wildcard characters "*" and "?", where "*" 
matches zero or more consecutive occurrences of any character and "?" matches 
a single occurrence of any character.

Note that folding is locale-independent behaviour. It is also important to 
note that there can be no guarantee that folding is in any way culturally 
appropriate, and should not be used for matching strings in natural language; 
use MatchC() for this.

Note that there is no 'escape character', which means that it is not possible
to match either the "*" character itself or the "?" character itself using
this function.

@param aZeroTerminatedString The wide Zero TerminatedString string whose data 
is to be matched with this Descriptor's data. 

@return If a match is found, the offset within this descriptor's data where 
        the match first occurs. KErrNotFound, if there is no match. 

@see TDesC::MatchC()
*/
EXPORT_C TInt LString16::MatchF(const TUint16* aZeroTerminatedString) const
	{
	return RBuf16::MatchF(TPtrC16((aZeroTerminatedString), User::StringLength(aZeroTerminatedString)));
	}
/**
Compares this descriptor's data with the specified string's data.

The comparison proceeds on a byte for byte basis. The result of the comparison 
is based on the difference of the first bytes to disagree.

@param aWideCharStr The wide character string whose data is to be compared 
            with this Descriptor's data. 
             
@return Positive, if this descriptor is greater than the specified string. 
        Negative, if this descriptor is less than the specified string.
        Zero, if both the descriptor and the string have the same length 
        and the their contents are the same.
*/
EXPORT_C TInt LString16::Compare(const wchar_t* aWideCharStr) const
	{
	return LString16::Compare(reinterpret_cast<const TUint16*>(aWideCharStr));
	}

/**
Compares this descriptor's data with the specified string's data.

The comparison proceeds on a byte for byte basis. The result of the comparison 
is based on the difference of the first bytes to disagree.

@param aZeroTerminatedString The wide Zero TerminatedString string whose data 
is to be compared with this Descriptor's data. 
             
@return Positive, if this descriptor is greater than the specified string. 
        Negative, if this descriptor is less than the specified string.
        Zero, if both the descriptor and the string have the same length 
        and the their contents are the same.
*/
EXPORT_C TInt LString16::Compare(const TUint16* aZeroTerminatedString) const
	{
	return RBuf16::Compare(TPtrC16((aZeroTerminatedString), User::StringLength(aZeroTerminatedString)));
	}
/**
Compares this descriptor's folded data with the specified string's folded 
data. 

Note that folding is locale-independent behaviour. It is also important to 
note that there can be no guarantee that folding is in any way culturally 
appropriate, and should not be used for comparing strings in natural language; 

@param aWideCharStr The wide character string whose data is to be compared 
            with this Descriptor's data. 
            
@return Positive, if this descriptor is greater than the specified string. 
        Negative, if this descriptor is less than the specified string.
        Zero, if the descriptor and the specified string have the same length
        and the their contents are the same.
        
@see TDesC::Compare()
*/
EXPORT_C TInt LString16::CompareF(const wchar_t* aWideCharStr) const
	{
	return LString16::CompareF(reinterpret_cast<const TUint16*>(aWideCharStr));
	}

/**
Compares this descriptor's folded data with the specified string's folded 
data. 

Note that folding is locale-independent behaviour. It is also important to 
note that there can be no guarantee that folding is in any way culturally 
appropriate, and should not be used for comparing strings in natural language; 

@param aZeroTerminatedString The wide Zero Terminated String whose data 
is to be compared with this string's data. 
            
@return Positive, if this descriptor is greater than the specified string. 
        Negative, if this descriptor is less than the specified string.
        Zero, if the descriptor and the specified string have the same length
        and the their contents are the same.
        
@see TDesC::Compare()
*/
EXPORT_C TInt LString16::CompareF(const TUint16* aZeroTerminatedString) const
	{
	return RBuf16::CompareF(TPtrC16((aZeroTerminatedString), User::StringLength(aZeroTerminatedString)));
	}

/**
Searches for the first occurrence of the specified data sequence within this 
descriptor.

Searching always starts at the beginning of this descriptor's data.

@param aWideCharStr The wide character string whose data is to be searched for, 
            within this Descriptor's data. 
            
@return The offset of the data sequence from the beginning of this descriptor's 
        data. KErrNotFound, if the data sequence cannot be found.
*/
EXPORT_C TInt LString16::Find(const wchar_t* aWideCharStr) const
	{
	return LString16::Find(reinterpret_cast<const TUint16*>(aWideCharStr));
	}

/**
Searches for the first occurrence of the specified data sequence within this 
descriptor.

Searching always starts at the beginning of this descriptor's data.

@param aWideCharStr The wide character string whose data is to be searched for, 
            within this Descriptor's data. 
            
@return The offset of the data sequence from the beginning of this descriptor's 
        data. KErrNotFound, if the data sequence cannot be found.
*/
EXPORT_C TInt LString16::Find(const TUint16* aZeroTerminatedString) const
	{
	return RBuf16::Find(TPtrC16((aZeroTerminatedString), User::StringLength(aZeroTerminatedString)));
	}

/**
Searches for the first occurrence of the specified data sequence within this 
descriptor.

Searching always starts at the beginning of this descriptor's data.

@param aWideCharStr The wide character string whose data is to be searched for, 
            within this Descriptor's data. 
@param aLenS The length of the data sequence to be searched for. This value 
             must not be negative, otherwise the function raises a panic.
             
@return The offset of the data sequence from the beginning of this descriptor's 
        data. KErrNotFound, if the data sequence cannot be found.
       
@panic  USER 29 if aLenS is negative. 
*/
EXPORT_C TInt LString16::Find(const wchar_t* aWideCharStr,TInt aLenS) const
	{
	return LString16::Find(reinterpret_cast<const TUint16*>(aWideCharStr), aLenS);
	}

/**
Searches for the first occurrence of the specified folded data sequence within 
this descriptor's folded data. 

Searching always starts at the beginning of this descriptor's data.

Note that folding is locale-independent behaviour. It is also important to 
note that there can be no guarantee that folding is in any way culturally 
appropriate, and should not be used for finding strings in natural language; 

@param aWideCharStr The wide character string whose data is to be searched for, 
            within this Descriptor's data.
            
@return The offset of the data sequence from the beginning of this descriptor's 
        data. KErrNotFound, if the data sequence cannot be found. Zero, if the 
        length of the search data sequence is zero.

*/
EXPORT_C TInt LString16::FindF(const wchar_t* aWideCharStr) const
	{
	return LString16::FindF(reinterpret_cast<const TUint16*>(aWideCharStr));
	}

/**
Searches for the first occurrence of the specified folded data sequence within 
this descriptor's folded data. 

Searching always starts at the beginning of this descriptor's data.

Note that folding is locale-independent behaviour. It is also important to 
note that there can be no guarantee that folding is in any way culturally 
appropriate, and should not be used for finding strings in natural language; 

@param aWideCharStr The wide character string whose data is to be searched for, 
            within this Descriptor's data. 
            
@return The offset of the data sequence from the beginning of this descriptor's 
        data. KErrNotFound, if the data sequence cannot be found. Zero, if the 
        length of the search data sequence is zero.

*/
EXPORT_C TInt LString16::FindF(const TUint16* aZeroTerminatedString) const
	{
	return RBuf16::FindF(TPtrC16((aZeroTerminatedString), User::StringLength(aZeroTerminatedString)));
	}
/**
Searches for the first occurrence of the specified folded data sequence within 
this descriptor's folded data.

Searching always starts at the beginning of this descriptor's data.

Note that folding is locale-independent behaviour. It is also important to 
note that there can be no guarantee that folding is in any way culturally 
appropriate, and should not be used for finding strings in natural language; 

@param aWideCharStr The wide character string whose data is to be searched for, 
            within this Descriptor's data.
@param aLenS The length of the data sequence to be searched for. This value 
             must not be negative, otherwise the function raises a panic.
             
@return The offset of the data sequence from the beginning of this descriptor's 
        data. KErrNotFound, if the data sequence cannot be found. Zero, if the
        length of the search data sequence is zero.

@panic USER 29 if aLenS is negative

*/
EXPORT_C TInt LString16::FindF(const wchar_t* aWideCharStr, TInt aLen) const
	{
	return RBuf16::FindF(reinterpret_cast<const TUint16*>(aWideCharStr),aLen);
	}

/**
Copies and folds data from the specified string into this descriptor replacing 
any existing data.

The length of this descriptor is set to reflect the new 
data.

Note that folding is locale-independent behaviour. It is also important to 
note that there can be no guarantee that folding is in any way culturally 
appropriate, and should not be used when dealing with strings in natural
language.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aWideCharStr A wide character string

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16:: CopyFL(const wchar_t* aWideCharStr )
	{
	LString16::CopyFL(reinterpret_cast<const TUint16*>(aWideCharStr));
	}

/**
Copies and folds data from the specified string into this descriptor replacing 
any existing data.

The length of this descriptor is set to reflect the new 
data.

Note that folding is locale-independent behaviour. It is also important to 
note that there can be no guarantee that folding is in any way culturally 
appropriate, and should not be used when dealing with strings in natural
language.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aZeroTerminatedString A wide zero terminated string

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16:: CopyFL(const TUint16* aZeroTerminatedString )
	{
	LString16::CopyFL(TPtrC16((aZeroTerminatedString), User::StringLength(aZeroTerminatedString)));
	}

/**
Copies text from the specified string and converts it to lower case before 
putting it into this descriptor, replacing any existing data.

The length of this descriptor is set to reflect the new data.

Conversion to lower case is implemented as appropriate to the current locale.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aWideCharStr A wide character string.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16:: CopyLCL(const wchar_t* aWideCharStr)
	{
	LString16::CopyLCL(reinterpret_cast<const TUint16*>(aWideCharStr));
	}

/**
Copies text from the specified string and converts it to lower case before 
putting it into this descriptor, replacing any existing data.

The length of this descriptor is set to reflect the new data.

Conversion to lower case is implemented as appropriate to the current locale.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aZeroTerminatedString A wide zero terminated string.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16:: CopyLCL(const TUint16* aZeroTerminatedString)
	{
	LString16::CopyLCL(TPtrC16((aZeroTerminatedString), User::StringLength(aZeroTerminatedString)));
	}

/**
Copies text from the specified string and converts it to upper case before 
putting it into this descriptor, replacing any existing data.

The length of this descriptor is set to reflect the new data.

Conversion to upper case is implemented as appropriate to the current locale.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aWideCharStr A wide character string.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16:: CopyUCL(const wchar_t* aWideCharStr)
	{
	LString16::CopyUCL(reinterpret_cast<const TUint16*>(aWideCharStr));
	}

/**
Copies text from the specified string and converts it to upper case before 
putting it into this descriptor, replacing any existing data.

The length of this descriptor is set to reflect the new data.

Conversion to upper case is implemented as appropriate to the current locale.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aZeroTerminatedString A wide zero terminated string.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16:: CopyUCL(const TUint16* aZeroTerminatedString)
	{
	LString16::CopyUCL(TPtrC16((aZeroTerminatedString), User::StringLength(aZeroTerminatedString)));
	}

/**
Copies text from the specified string and capitalises it before putting 
it into this descriptor, replacing any existing data.

The length of this descriptor is set to reflect the new data.

Capitalisation is implemented as appropriate to the current locale.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aWideCharStr A wide character string.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16:: CopyCPL(const wchar_t* aWideCharStr)
	{
	LString16::CopyCPL(reinterpret_cast<const TUint16*>(aWideCharStr));
	}

/**
Copies text from the specified string and capitalises it before putting 
it into this descriptor, replacing any existing data.

The length of this descriptor is set to reflect the new data.

Capitalisation is implemented as appropriate to the current locale.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aZeroTerminatedString A wide zero terminated string.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so
*/
EXPORT_C void LString16:: CopyCPL(const TUint16* aZeroTerminatedString)
	{
	LString16::CopyCPL(TPtrC16((aZeroTerminatedString), User::StringLength(aZeroTerminatedString)));
	}
/**
Inserts data into this descriptor.

The length of this descriptor is changed to reflect the extra data.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aPos The position within the data where insertion is to start. This 
            is an offset value; a zero value refers to the leftmost data
            position.
            
@param aWideCharStr A wide character string.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 10  if aPos is negative or is greater than the length of this
                descriptor.
*/
EXPORT_C void LString16:: InsertL(TInt aPos,const wchar_t* aWideCharStr)
	{
	LString16::InsertL(aPos, reinterpret_cast<const TUint16*>(aWideCharStr));
	}

/**
Inserts data into this descriptor.

The length of this descriptor is changed to reflect the extra data.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aPos The position within the data where insertion is to start. This 
            is an offset value; a zero value refers to the leftmost data
            position.
            
@param aZeroTerminatedString A wide null terminated string.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 10  if aPos is negative or is greater than the length of this
                descriptor.
*/
EXPORT_C void LString16:: InsertL(TInt aPos,const TUint16* aZeroTerminatedString)
	{
	LString16::InsertL(aPos,TPtrC16((aZeroTerminatedString), User::StringLength(aZeroTerminatedString)));
	}

/**
Replaces data in this descriptor.

The specified length can be different to the length of the replacement data.
The length of this descriptor changes to reflect the change of data.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aPos    The position within the data where replacement is to start. 
               This is an offset value; a zero value refers to the leftmost
               data position. 
            
@param aLength The length of data to be replaced.

@param aWideCharStr The source wide character string

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER  8  if aLength is negative 
               
@panic USER 10  if aPos is negative or is greater than the length of this
                descriptor.
                
@panic USER 16  if the length of the source descriptor aDes is negative 
*/
EXPORT_C void LString16:: ReplaceL(TInt aPos,TInt aLength,const wchar_t* aWideCharStr)
	{
	LString16::ReplaceL(aPos,aLength,reinterpret_cast<const TUint16*>(aWideCharStr));
	}

/**
Replaces data in this descriptor.

The specified length can be different to the length of the replacement data.
The length of this descriptor changes to reflect the change of data.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aPos    The position within the data where replacement is to start. 
               This is an offset value; a zero value refers to the leftmost
               data position. 
            
@param aLength The length of data to be replaced.

@param aZeroTerminatedString The source wide null terminated character string

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER  8  if aLength is negative 
               
@panic USER 10  if aPos is negative or is greater than the length of this
                descriptor.
                
@panic USER 16  if the length of the source descriptor aDes is negative 
*/
EXPORT_C void LString16:: ReplaceL(TInt aPos,TInt aLength,const TUint16* aZeroTerminatedString)
	{
	LString16::ReplaceL(aPos,aLength,TPtrC16((aZeroTerminatedString), User::StringLength(aZeroTerminatedString)));
	}

/**
Copies data into this descriptor and justifies it, replacing any existing data.

The length of this descriptor is set to reflect the new data.

The target area is considered to be an area of specified width positioned at
the beginning of this descriptor's data area. Source data is copied into, and
aligned within this target area according to the specified alignment
instruction.

If the length of the target area is larger than the length of the source, then
spare space within the target area is padded with the fill character.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aWideCharStr    A wide character string containing the source data.
                   The length of the data to be copied is the smaller of:
                   the length of the source descriptor, and 
                   the width of the target area (only if this is not the
                   explicit negative value KDefaultJustifyWidth).

@param aWidth      The width of the target area. If this has the specific
                   negative value KDefaultJustifyWidth, then the width is
                   re-set to the length of the data source.

@param aAlignment The alignment of the data within the target area

@param aFill       The fill character used to pad the target area. 

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 11  if aWidth has a negative value other than KDefaultJustifyWidth.
*/
EXPORT_C void LString16:: JustifyL(const wchar_t* aWideCharStr,TInt aWidth,TAlign anAlignment,TChar aFill)
	{
	LString16::JustifyL(reinterpret_cast<const TUint16*>(aWideCharStr),aWidth,anAlignment,aFill);
	}

/**
Copies data into this descriptor and justifies it, replacing any existing data.

The length of this descriptor is set to reflect the new data.

The target area is considered to be an area of specified width positioned at
the beginning of this descriptor's data area. Source data is copied into, and
aligned within this target area according to the specified alignment
instruction.

If the length of the target area is larger than the length of the source, then
spare space within the target area is padded with the fill character.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aWideCharStr    A wide character string containing the source data.
                   The length of the data to be copied is the smaller of:
                   the length of the source descriptor, and 
                   the width of the target area (only if this is not the
                   explicit negative value KDefaultJustifyWidth).

@param aWidth      The width of the target area. If this has the specific
                   negative value KDefaultJustifyWidth, then the width is
                   re-set to the length of the data source.

@param aAlignment The alignment of the data within the target area

@param aFill       The fill character used to pad the target area. 

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 11  if aWidth has a negative value other than KDefaultJustifyWidth.
*/
EXPORT_C void LString16:: JustifyL(const TUint16* aZeroTerminatedString,TInt aWidth,TAlign anAlignment,TChar aFill)
	{
	LString16::JustifyL(TPtrC16((aZeroTerminatedString), User::StringLength(aZeroTerminatedString)),aWidth,anAlignment,aFill);
	}

/**
Appends data onto the end of this descriptor's data and justifies it.

The source of the appended data is a memory location.

The target area is considered to be an area of specified width, immediately 
following this descriptor's existing data. Source data is copied into, and 
aligned within, this target area according to the specified alignment instruction.

If the length of the target area is larger than the length of the source, 
then spare space within the target area is padded with the fill character.

This leaving variant of the standard, non-leaving descriptor method
differs in that this operation may cause the string descriptor's heap
buffer to be reallocated in order to accommodate the new data. As a
result, MaxLength() and Ptr() may return different values afterwards,
and any existing raw pointers to into the descriptor data may be
invalidated.

@param aString     A pointer to a source memory location. 

@param aLength     The length of data to be copied. If this is greater than the 
                   width of the target area, then the length of data copied is
                   limited to the width.
               
@param aWidth      The width of the target area. If this has the specific negative 
                   value KDefaultJustifyWidth, then the width is
                   re-set to the length of the data source. 
               
@param aAlignment The alignment of the data within the target area. 

@param aFill       The fill character used to pad the target area.

@leave KErrNoMemory if the underlying buffer needs to be
grown and there are insufficient resources to do so

@panic USER 11  if aWidth has a negative value other than KDefaultJustifyWidth.
                
@panic USER 17  if aLength is negative.  
*/
EXPORT_C void LString16:: AppendJustifyL(const wchar_t* aWideCharStr,TInt aLength,TInt aWidth,TAlign anAlignment,TChar aFill)
	{
	LString16::AppendJustifyL(reinterpret_cast<const TUint16*>(aWideCharStr),aLength, aWidth,anAlignment,aFill);
	}
//eof
