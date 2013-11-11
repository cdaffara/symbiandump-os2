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


#ifndef __RCPOINTERARRAY_H
#define __RCPOINTERARRAY_H
#include <e32cmn.h>

/** 
 * An array of pointers to objects, that adds close type behaviour.
 *
 * For use with C class pointers that are owned by the array. 
 * @see RCPointerArray
 * @since v7.0 
 */
template<class T> class RCPointerArray : public RPointerArray<T>
	{
 public:
	/** Constructor */
	inline RCPointerArray();
	/** Closes the array and frees all resources (including deleting objects in the array). */
	inline void Close();
	};

/** Default constructor. */
template <class T>
inline RCPointerArray<T>::RCPointerArray()
	{
	}

/**
 * Closes the array and frees all resources; this includes deleting the objects 
 * whose pointers are held by the array (i.e. it's the same as ResetAndDestroy()).
 */
template <class T>
inline void RCPointerArray<T>::Close()
	{
	this->ResetAndDestroy();
	}

#endif
