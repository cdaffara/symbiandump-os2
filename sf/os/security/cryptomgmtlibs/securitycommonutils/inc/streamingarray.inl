/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Implementation of utility functions for copying and streaming RArray/RpointerArray. 
*
*/


/**
 @file 
 @internalTechnology
 @released
*/

#ifndef STREAMINGARRAY_INL
#define STREAMINGARRAY_INL

#include <s32mem.h>
#include <scs/streamingarray.h>
#include <scs/nullstream.h>
#include <scs/streamingarray.h>

// Traits classes - internally required by RArray/RPointerArray functions

template <class T>
class TTraits
	{
public:
	static T* CopyL(const T& aRight) { return T::NewL(aRight);}
	static T* ReadFromStreamL(RReadStream& aStream) { return T::NewL(aStream); }
	static T* ReadFromStreamLC(RReadStream& aStream) { return T::NewLC(aStream); }
	static void WriteToStreamL(const T& aItem, RWriteStream& aStream) { aStream << aItem; }
	};

// Specialisation for HBufC
template <>
class TTraits<HBufC16>
	{
public:
	static HBufC16* CopyL(const HBufC16& aOther) { return aOther.AllocL();}
	static HBufC16* ReadFromStreamL(RReadStream& aStream) { return HBufC16::NewL(aStream, KMaxTInt); }
	static HBufC16* ReadFromStreamLC(RReadStream& aStream) { return HBufC16::NewLC(aStream, KMaxTInt); }
	static void WriteToStreamL(const HBufC16& aItem, RWriteStream& aStream) { aStream << aItem; }
	};
		
template <>
class TTraits<HBufC8>
	{
public:
	static HBufC8* CopyL(const HBufC8& aOther) { return aOther.AllocL();}
	static HBufC8* ReadFromStreamL(RReadStream& aStream) { return HBufC8::NewL(aStream, KMaxTInt); }
	static HBufC8* ReadFromStreamLC(RReadStream& aStream) { return HBufC8::NewLC(aStream, KMaxTInt); }
	static void WriteToStreamL(const HBufC8& aItem, RWriteStream& aStream) { aStream << aItem; }
	};

// Size calculator - internally required by RPointerArray functions

template <typename T>
TInt GetObjectBufferSizeL(const T& aObject)
	{
	TInt size(0);
	RNullWriteStream nullstream;
	CleanupClosePushL(nullstream);
	nullstream << aObject;
	nullstream.CommitL();
	size = nullstream.BytesWritten();
	CleanupStack::PopAndDestroy(&nullstream);
	return size;
	}

template <class T>
TInt ExternalizedBufferSizeL(const RPointerArray<T>& aArray)
/**
	Calculates how much buffer is required for a pointers array to be externalized.
	@param aArray The pointer array whose buffer size will be calculated.
	@return Returns the number of bytes occupied by the elements of the pointer array.
 */
	{
	TInt totalSize(1);	// 1-byte is reserved for the array count
	TInt count = aArray.Count();
	
	for (TInt i = 0; i < count; ++i)
			{
			totalSize += GetObjectBufferSizeL(*aArray[i]);
			}
	return totalSize;
	}


// RPointerArray utilities

template <class T>
void CopyPointerArrayL(RPointerArray<T>& aTarget, const RPointerArray<T>& aSource)
	{
	TInt arrayCount =  aSource.Count();
	for (TInt i = 0; i < arrayCount; ++i)
		{
		T* item = TTraits<T>::CopyL(*aSource[i]);
		CleanupStack::PushL(item);
		User::LeaveIfError(aTarget.Append(item));
		CleanupStack::Pop(item);
		}
	}
	
template <class T>
HBufC8* ExternalizePointersArrayLC(const RPointerArray<T>& aArray, TBool aAddLength)
	{
	TInt arraySize = ExternalizedBufferSizeL(aArray);
	HBufC8 *arrayBuf = HBufC8::NewLC(arraySize + sizeof(TInt32));
	TPtr8 arrayPtr(arrayBuf->Des());
	
	RDesWriteStream stream(arrayPtr);
	stream.PushL();
	ExternalizePointersArrayL(aArray, stream, aAddLength);
	stream.Pop();
	stream.Release();
	return arrayBuf;
	}

template <class T>
void ExternalizePointersArrayL(const RPointerArray<T>& aArray, RWriteStream& aStream, TBool aAddLength)
	{
	TInt arrayCount = aArray.Count();
	if(aAddLength)
		aStream << TCardinality(arrayCount);
	for (TInt i = 0; i < arrayCount; ++i)
		{
		TTraits<T>::WriteToStreamL(*aArray[i], aStream);
		}
	aStream.CommitL();
	}

template <class T>
void InternalizePointersArrayL(RPointerArray<T>& aArray, RReadStream& aStream, TBool aAddLength)
	{
	TInt count (0);
	if(aAddLength)
		{
		TCardinality c;
		aStream >> c;
		count = c;
		}
	for (TInt i = 0; i < count; ++i)
		{
		T* item = TTraits<T>::ReadFromStreamL(aStream);
		CleanupStack::PushL(item);
		User::LeaveIfError(aArray.Append(item));
		CleanupStack::Pop(item);
		}
	}


// Size calculator - internally required by RArray/RPointerArray functions

template <class T>
TInt ExternalizedBufferSizeL(const RArray<T>& aArray)
/**
	Calculates how much buffer is required for a fixed length array to be externalized.
	@param aArray The array whose buffer size will be calculated.
	@return Returns the number of bytes of the array's dynamic memory.
 */
	{
	TInt totalSize(1); // 1-byte is reserved for the array count
	TInt count = aArray.Count();

	for (TInt i = 0; i < count; ++i)
			{
			totalSize += GetObjectBufferSizeL(TPckgC<T>(aArray[i]));
			}
	return totalSize;
	}


// RArray utilities

template <class T>
void CopyFixedLengthArrayL(RArray<T>& aTarget, const RArray<T>& aSource)
	{
	TInt arrayCount = aSource.Count();
	for (TInt i = 0; i < arrayCount; ++i)
		{
		User::LeaveIfError(aTarget.Append(aSource[i]));
		}
	}

template <class T>
HBufC8* ExternalizeFixedLengthArrayL(const RArray<T>& aArray)
	{
	TInt arraySize = ExternalizedBufferSizeL(aArray, arraySize);
	HBufC8 *arrayBuf = HBufC8::NewLC(arraySize + sizeof(TInt32));
	TPtr8 arrayPtr(arrayBuf->Des());
	
	RDesWriteStream stream(arrayPtr);
	stream.PushL();
	ExternalizeFixedLengthArrayL(aArray, stream);
	stream.Pop();
	stream.Release();
	return arrayBuf;
	}

template <class T>
void ExternalizeFixedLengthArrayL(const RArray<T>& aArray, RWriteStream& aStream)
	{
	TInt arrayCount = aArray.Count();
	aStream << TCardinality(arrayCount);
	for (TInt i = 0; i < arrayCount; ++i)
		{
		aStream.WriteL(TPckgC<T>(aArray[i]));
		}
	aStream.CommitL();
	}


template <class T>
void InternalizeFixedLengthArrayL(RArray<T>& aArray, RReadStream& aStream)
	{
	TCardinality c;
	aStream >> c;
	TInt count = c;
	for (TInt i = 0; i < count; ++i)
		{
		T item;
		TPckg<T> itemPckg(item);
		aStream.ReadL(itemPckg);
		User::LeaveIfError(aArray.Append(item));
		}
	}

#endif /* STREAMINGARRAY_INL*/
