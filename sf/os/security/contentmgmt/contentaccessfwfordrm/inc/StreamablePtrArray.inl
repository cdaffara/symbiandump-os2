/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

/**
 @file
 @publishedAll
 @released
*/

#ifndef STREAMABLEPTRARRAY_INL
#define STREAMABLEPTRARRAY_INL

namespace ContentAccess
{
template <class T>
inline RStreamablePtrArray<T>::RStreamablePtrArray()
	{
	}

template <class T>
inline void RStreamablePtrArray<T>::Close()
	{
	iArray.ResetAndDestroy();
	}

template <class T>
inline void RStreamablePtrArray<T>::AppendL(T* aElement)
	{
	User::LeaveIfError(iArray.Append(aElement));
	}

template <class T>
inline TInt RStreamablePtrArray<T>::Count() const
	{
	return iArray.Count();
	}

template <class T>
inline T* RStreamablePtrArray<T>::operator [] (TInt aIndex) const
	{
	// case the value from the array back to its original type
	return iArray[aIndex];
	}

template <class T>
inline void RStreamablePtrArray<T>::ResetAndDestroy()
	{
	iArray.ResetAndDestroy();
	}

template <class T>		
inline void RStreamablePtrArray<T>::InternalizeL(RReadStream& aStream)
	{
	TInt i;

	// Read the number of CMetaData objects from the stream
	TInt count = aStream.ReadInt32L();

	// Read the objects from the stream and add them to the array
	for(i = 0; i < count; i++)
		{
		// create a new T object from the stream using the supplied factory function
		T* element = T::NewL(aStream);
		CleanupStack::PushL(element);

		AppendL(element);

		// Finished with cleanup stack, element now owned by the array so don't delete
		CleanupStack::Pop(element);
		}
	}

template <class T>
inline void RStreamablePtrArray<T>::ExternalizeL(RWriteStream& aStream) const
	{	
	TInt i;
	// Write the total number of elements to the stream
	aStream.WriteInt32L(iArray.Count());
	
	// Ask the array elements to write themselves to the stream one by one 
	for(i = 0; i < iArray.Count();i++)
		{
		iArray[i]->ExternalizeL(aStream);
		}
	}

template <class T>
inline void RStreamablePtrArray<T>::RemoveL(TInt aIndex)
	{
	if ( (aIndex >= iArray.Count()) || (aIndex < 0) )
		{
		User::Leave(KErrArgument);
		}
	iArray.Remove(aIndex);
	}
}

#endif // STREAMABLEPTRARRAY_INL

