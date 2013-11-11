/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __RMPOINTERARRAY_H__
#define __RMPOINTERARRAY_H__


/**
 * An array of pointers to objects, that adds close type behaviour.
 *
 * For use with M class pointers, which have a release method and are owned by 
 * the array. When the array is closed, the objects will be released.
 *
 * @see RMPointerArray
 * @since v7.0 
 */
template<class T> class RMPointerArray : public RPointerArray<T>
	{
 public:
	/** Constructor */
	inline RMPointerArray();
	/** Frees all resources, calling Release() on the contents of the array */
	inline void Close();
	};

/** Default constructor. */
template <class T>
inline RMPointerArray<T>::RMPointerArray()
	{
	}

/** 
 * Closes the array and frees all resources; this includes deleting the objects 
 * whose pointers are held by the array. 
 */
template <class T>
inline void RMPointerArray<T>::Close()
	{
	TInt count = this->Count();
	for (TInt ii = 0; ii < count; ii++)
		(*this)[ii]->Release();
	this->Reset();
	}

#endif
