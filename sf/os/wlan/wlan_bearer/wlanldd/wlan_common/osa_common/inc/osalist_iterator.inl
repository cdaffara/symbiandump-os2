/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   inline implementation of list_iterator
*
*/

/*
* %version: 3 %
*/

#include "algorithm.h"  // for operator !=

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T, class J>
inline list_iterator<T, J>::list_iterator( const Self& aElem ) 
    : iNode( aElem.iNode )
    {

    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T, class J>
inline list_iterator<T, J>& list_iterator<T, J>::operator=( const Self& aElem )
    {
    iNode = aElem.iNode;
    return *this;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T, class J>
inline T* list_iterator<T, J>::operator->() 
    {
    return iNode;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T, class J> 
inline J& list_iterator<T, J>::operator*()
    {
    return iNode->iElem;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T, class J> 
inline list_iterator<T, J>& list_iterator<T, J>::operator++()
    {
    iNode = iNode->iNext; 
    return *this;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T, class J> 
inline list_iterator<T, J>& list_iterator<T, J>::operator--()
    {
    iNode = iNode->iPrev; 
    return *this;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T, class J> 
inline T* list_iterator<T, J>::operator()()
    {
    return iNode;
    }
