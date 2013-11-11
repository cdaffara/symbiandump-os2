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


#ifndef __STREAMABLEPTRARRAY_H__
#define __STREAMABLEPTRARRAY_H__

#include <e32base.h>
#include <s32strm.h>

namespace ContentAccess
	{
	/** Template class used to store an array of pointers that can also write itself to a stream

	The RStreamablePtrArray owns all the elements contained within it. The close 
	function must be called before the RStreamablePtrArray goes out of scope.

	The close function frees all resources and deletes all array elements
	
	The array can be streamed using the InternalizeL() and ExternalizeL() functions

  	*/
	template <class T>
		class RStreamablePtrArray
		{
	public:
		/** Constructor */
		inline RStreamablePtrArray();	
	
		/** Free all resources held by the array 
		
		Calls ResetAndDestroy();
		*/
		inline void Close();

		/** Append an element to the array
		
		Ownership of the pointer is transferred to the array
		@param aElement The item to add to the array
		*/
		inline void AppendL(T* aElement);

		/** Count the number of objects in the array */
		inline TInt Count() const;

		/** Return a pointer to an element at a given index in the array
		
		@param aIndex The zero based index in the array, must be less than Count()
		@return A pointer to the object stored in the array
		*/
		inline T* operator [] (TInt aIndex) const;
		
		/** Delete all elements in the array*/
		inline void ResetAndDestroy();
		
		/** Add array elements from a stream */
		inline void InternalizeL(RReadStream& aStream);

		/** Write the entire array to a stream */
		inline void ExternalizeL(RWriteStream& aStream) const;

		/** Remove an element from the array.
		    Note that the function does not delete the pointer.
		 */
		inline void RemoveL(TInt aIndex);

	private:
		RPointerArray <T> iArray;
		};
	}

// include inline function definitions
#include <caf/streamableptrarray.inl>

#endif
