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
* Description:   list declaration
*
*/

/*
* %version: 3 %
*/

#ifndef WLANLIST_ITERATOR_H
#define WLANLIST_ITERATOR_H

#include "algorithm.h"  // for operator !=

/**
 *  list iterator with limited interface
 *
 *
 *  @lib wlanosa.lib
 *  @since S60 v3.2
 */
template<class T, class J> 
class list_iterator : public DBase
    {

	typedef list_iterator<T, J> Self;

public:

    /**
     * ctor
     *
     * @since S60 v3.2
     * @param aElem data to be iterated
     */
    explicit list_iterator( T* aElem = NULL ) : iNode( aElem ) {};

    /**
     * dtor
     *
     * @since S60 v3.2
     */
	virtual ~list_iterator() {};

    /**
     * copy ctor 
     *
     * @since S60 v3.2
     * @param aElem me, myself and I
     */
    inline list_iterator( const Self& aElem );

    /**
     * assignment 
     *
     * @since S60 v3.2
     * @param aElem me, myself and I
     * @return me, myself and I
     */
	inline Self& operator=( const Self& aElem );

    /**
     * operator ->
     *
     * @since S60 v3.2
     * @return node of the actual element
     */
	inline T* operator->();

    /**
     * operator*
     *
     * @since S60 v3.2
     * @return actual element
     */
	inline J& operator*();

    /**
     * operator++ (prefix++) step forward
     *
     * @since S60 v3.2
     * @return iterator with the new position
     */
    inline Self& operator++();

    /**
     * operator-- (prefix--) step backward
     *
     * @since S60 v3.2
     * @return iterator with the new position
     */
    inline Self& operator--();

    /**
     * operator()
     *
     * @since S60 v3.2
     * @return the one and only node
     */
    inline T* operator()();

    /**
     * operator==
     *
     * @since S60 v3.2
     * @param aObj object to compare against
     * @return ETrue upon equal, any other for non equal
     */
    inline TBool operator== ( const Self& aObj ) const 
        { 
        return ( iNode == (aObj.iNode) );
        }

private:

    /**
     * list node
     * Not own.  
     */
    T*   iNode;

    };

#include "osalist_iterator.inl"

#endif // WLANLIST_ITERATOR_H


