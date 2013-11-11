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
* Utility functions for copying and streaming RArray/RpointerArray. 
*
*/


/**
 @file 
 @internalTechnology
 @released
*/

#ifndef STREAMINGARRAY_H
#define STREAMINGARRAY_H

#include <e32std.h>
#include <s32strm.h>
	

// RPointerArray utilities

/**
	Copy a pointer array of template objects to another one.
	Note that only objects are copied into the target array, NOT pointers.
	T class must provide a static T::NewL(T&) construction function.
	@param aTarget	The pointer array to which a given array will be copied.
	@param aSource  The pointer array which will be copied.
 */
template <class T>
void CopyPointerArrayL(RPointerArray<T>& aTarget, const RPointerArray<T>& aSource);

/**
	Externalize an array of pointers of template class.
	@param aArray The pointer array which will be externalized
	@param aAddLength Whether the length of the array is added to the beginning of the externalized data.
	@return Pointer to the memory of the externalized array which is left on 
			the cleanup stack. Ownership is transferred.
			
 */
template <class T>
HBufC8* ExternalizePointersArrayLC(const RPointerArray<T>& aArray, TBool aAddLength=ETrue);

/**
	Externalize an array of pointers of template class.
	@param aArray The pointer array which will be externalized.
	@param aStream A stream to the buffer where the pointer array will be externalized.
	@param aAddLength Whether the length of the array is added to the beginning of the externalized data.
 */
template <class T>
void ExternalizePointersArrayL(const RPointerArray<T>& aArray, RWriteStream& aStream, TBool aAddLength=ETrue);

/**
	Internalize a given externalized pointers array stream.
	Template class must provide a static T::NewL(RReadStream&) function.
	@param aArray The pointer array which the stream will be internalized into.
	@param aStream A stream to the externalized pointer array
	@param aAddLength Whether the length of the array exists at the beginning of the data being internalized.
 */
template <class T>
void InternalizePointersArrayL(RPointerArray<T>& aArray, RReadStream& aStream, TBool aAddLength=ETrue);


// RArray utilities

/**
	Copy an array of fixed length template objects to another one.
	Note that T must have a valid copy c'tor.
	@param aTarget	The array to which a given array will be copied.
	@param aSource  The array which will be copied.
 */
template <class T>
void CopyFixedLengthArrayL(RArray<T>& aTarget, const RArray<T>& aSource);


/**
	Externalize an array of fixed length template objects.
	@param aArray The array which will be externalized.
	@return Pointer to the memory of the externalized array which is left on 
			the cleanup stack. Ownership is transferred.
 */
template <class T>
HBufC8* ExternalizeFixedLengthArrayLC(const RArray<T>& aArray);

/**
	Externalize an array of fixed length template objects.
	@param aArray The array which will be externalized.
	@param aStream A stream to the buffer where the fixed length array will be externalized.
 */
template <class T>
void ExternalizeFixedLengthArrayL(const RArray<T>& aArray, RWriteStream& aStream);

/**
	Internalize a given externalized fixed length array stream.
	@param aArray The pointer array which the stream will be internalized into.
	@param aStream A stream to the externalized fixed length array
 */
template <class T>
void InternalizeFixedLengthArrayL(RArray<T>& aArray, RReadStream& aStream);

#include <scs/streamingarray.inl>

#endif  // STREAMINGARRAY_H
