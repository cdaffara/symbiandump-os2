/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#ifndef __EUSEREXT_H__
#define __EUSEREXT_H__


/** An RRArray subclass intended to hold R classes.  
 * 
 * When you call close on an RRArray it will do the sane thing and 
 * call close on all the R Class objects inside it. 
 *
 * @internalTechnology
 */
template<class T> class RRArray : public RArray<T>
	{
public:
	/** Constructor */
	inline RRArray();
	inline RRArray(TUint aGranularity);
	/** Free all resources, calling Close() on the each element in the array */
	inline void Close();
	};

template <class T>
inline RRArray<T>::RRArray()
	{
	}

template <class T>
inline RRArray<T>::RRArray(TUint aGranularity) : RArray<T>(aGranularity)
	{
	}

template <class T>
inline void RRArray<T>::Close()
	{
	TInt count = RArray<T>::Count();
	for(TInt ii = 0; ii < count; ii++)
		{
		(*this)[ii].Close();
		}
	RArray<T>::Close();
	}

#endif
