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

#if !defined(__DESCRIPTORS_H__)
#define __DESCRIPTORS_H__

#include <pcstore/pcstoredef.h>

namespace PCStore
{
class CMemoryBlock;
/**
@internalAll

Class to represent 8-bit descriptors.

This class implements a subset of 8-bit descriptor's behaviour, which are necessary 
for the externalization and internalization of 8-bit descriptors. It encapsulates 
the data members containing the data, its maximum length and actual length. It also 
provides member functions to append and reset the data, as well as get the pointer of 
the data. However, there is no method to partly modify the data.

The constructors, Set, Append, and assignment methods take a copy of the data they're 
given, which means it allocates memory to hold the data and releases it when needed.
Data represented by this class is treated as a contiguous set of 8-bit 
(i.e. single byte) values or data items.
*/
class CDes8
	{
public:
	CDes8();
	CDes8(const CDes8& aDes);
	explicit CDes8(TInt32 aMaxLength);
	CDes8(const TUint8* aPtr);
	CDes8(const TUint8* aPtr, TInt32 aLength);
	CDes8(const TUint8* aPtr, TInt32 aLength, TInt32 aMaxLength);
	~CDes8();
	
	const TUint8* Ptr() const;
	TInt32 Length() const;
	TInt32 MaxLength() const ;
	TInt32 Size() const;
	
	void SetLength(TInt32 aLength);
	void Set(const CDes8& aDes);
	void Set(const TUint8* aPtr, TInt32 aLength);
	void Set(const TUint8* aPtr, TInt32 aLength, TInt32 aMaxLength);
	void Append(const CDes8& aDes);
	void Append(const TUint8* aPtr, TInt32 aLength);

	CDes8& operator=(const CDes8& aDes);
	TBool operator==(const CDes8& aDes) const;
	TBool operator!=(const CDes8& aDes) const;	


private:
	CMemoryBlock* iMemoryBlock;
	TInt32 iLength;
	};


/**
@internalAll

Class to represent 16-bit descriptor.

This class implements a subset of 16-bit descriptor's behaviour, which are necessary 
for the externalization and internalization of 16-bit descriptors. It encapsulates 
the data members containing the data, its maximum length and actual length. It also 
provides member functions to access and reset, but not modify, the data.

The constructors, Set, Append, and assignment methods take a copy of the data they're 
given, which means it allocates memory to hold the data and releases it when needed.
Data represented by this class is treated as a contiguous set of 16-bit 
(i.e. 2-byte word) values or data items.
*/
class CDes16
	{

public:
	CDes16();
	CDes16(const CDes16& aDes);
	explicit CDes16(TInt32 aMaxLength);
	CDes16(const TUint16* aPtr);
	CDes16(const TUint16* aPtr, TInt32 aLength);
	CDes16(const TUint16* aPtr, TInt32 aLength, TInt32 aMaxLength);
	~CDes16();

	const TUint16* Ptr() const;
	TInt32 Length() const;
	TInt32 MaxLength() const ;
	TInt32 Size() const;
	
	void SetLength(TInt32 aLength);
	void Set(const CDes16& aDes);
	void Set(const TUint16* aPtr, TInt32 aLength);
	void Set(const TUint16* aPtr, TInt32 aLength, TInt32 aMaxLength);
	void Append(const CDes16& aDes);
	void Append(const TUint16* aPtr, TInt32 aLength);

	CDes16& operator=(const CDes16& aDes);
	TBool operator==(const CDes16& aDes) const;
	TBool operator!=(const CDes16& aDes) const;	

private:
	CMemoryBlock* iMemoryBlock;
	TInt32 iLength;
	};
}
#endif // !defined(__DESCRIPTORS_H__)
