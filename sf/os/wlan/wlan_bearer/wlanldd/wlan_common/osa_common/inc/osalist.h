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

#ifndef WLANLIST_H
#define WLANLIST_H

#include "osalist_iterator.h"

/**
 *  double linked list with limited STL interface
 *
 *
 *  @lib wlanosa.lib
 *  @since S60 v3.2
 */
template<class T>
class list : public DBase
    {
    
    typedef list<T> Self;

public:

    struct Node : public DBase
        {

        /**
	     * Constructor.
	     *
	     * @since S60 v3.2
	     */
        explicit Node( const T& aElem ) 
            : iElem( aElem ), iNext( NULL ), iPrev( NULL ) {};

	    /**
	     * Destructor.
	     *
	     * @since S60 v3.2
	     */
        virtual ~Node() {};

        // Prohibit copy constructor.
        Node( const Node& );
        // Prohibit assigment operator.
        Node& operator= ( const Node& );

        T       iElem;

        Node*   iNext;
        Node*   iPrev;
        };
    
    friend class list_iterator<Node, T>;

	// type definitions
	typedef T		    value_type;
    typedef list_iterator<Node, T> iterator;
    typedef const list_iterator<Node, T> const_iterator;
	typedef T&		    reference;
	typedef const T&	const_reference;
	typedef TInt	    size_type;

    /**
     * creates an empty list without any elements
     *
     * @since S60 v3.2
     */
    inline list() : iFirst( NULL ), iLast( NULL ), iNumOfElems( 0 ) {};

    /**
     * destroys all elements and frees the memory
     *
     * @since S60 v3.2
     */
    inline virtual ~list();

    /**
     * returns a bidirectional iterator for the first element
     * for empty ranges equals to end()
     *
     * @since S60 v3.2
     * @return bidirectional iterator for the first element
     */
    inline iterator begin();

    /**
     * returns a const bidirectional iterator for the first element
     * for empty ranges equals to end()
     *
     * @since S60 v3.2
     * @return const bidirectional iterator for the first element
     */
    inline const_iterator begin() const;

    /**
     * returns a bidirectional iterator for the position after the last element
     *
     * @since S60 v3.2
     * @return bidirectional iterator for the position after the last element
     */
    inline iterator end();

    /**
     * returns a const bidirectional iterator for 
     * the position after the last element
     *
     * @since S60 v3.2
     * @return a const bidirectional iterator 
     * for the position after the last element
     */
    inline const_iterator end() const;

    /**
     * returns the actual number of elements
     *
     * @since S60 v3.2
     * @return the actual number of elements
     */
    inline size_type size() const;

    /**
     * returns whether the container is empty
     *
     * @since S60 v3.2
     * @return ETrue if the container is empty
     */
    inline TBool empty() const;

    /**
     * returns reference to the first element 
     * (no check wether the 1st element exists)
     * NOTE: if the container is empty the end result is undefined
     *
     * @since S60 v3.2
     * @return reference to the first element 
     */
    inline reference front();

    /**
     * returns const reference to the first element 
     * (no check wether the 1st element exists)
     * NOTE: if the container is empty the end result is undefined
     *
     * @since S60 v3.2
     * @return const reference to the first element 
     */
    inline const_reference front() const;

    /**
     * returns reference to the last element 
     * (no check wether the last element exists)
     * NOTE: if the container is empty the end result is undefined
     *
     * @since S60 v3.2
     * @return reference to the last element 
     */
    inline reference back();

    /**
     * returns const reference to the last element 
     * (no check wether the last element exists)
     * NOTE: if the container is empty the end result is undefined
     *
     * @since S60 v3.2
     * @return const reference to the last element 
     */
    inline const_reference back() const;

    /**
     * removes all elements (makes container empty)
     *
     * @since S60 v3.2
     */
    inline void clear();

    /**
     * appends a copy of aElem at the end
     * NOTE: either succeeds or has no effect, user must verify operation 
     * success by comparing size before and after call
     *
     * @since S60 v3.2
     * @param aElem element to be added
     */
    inline void push_back( const_reference aElem );

    /**
     * removes the last element (does not return it)
     * NOTE: if the container is empty the end result is undefined
     *
     * @since S60 v3.2
     */
    inline void pop_back();

    /**
     * inserts a copy of elem at the beginning
     * NOTE: either succeeds or has no effect, user must verify operation 
     * success by comparing size before and after call
     *
     * @since S60 v3.2
     * @param aElem element to be added
     */
    inline void push_front( const_reference aElem );

    /**
     * removes the first element (does not return it)
     * NOTE: if the container is empty the end result is undefined
     *
     * @since S60 v3.2
     */
    inline void pop_front();

    /**
     * inserts aElem before aPos
     * NOTE: aPos must be valid and != end() otherwise 
     * the end result is undefined
     * NOTE: either succeeds or has no effect, user must verify operation 
     * success by comparing size before and after call
     *
     * @since S60 v3.2
     * @param aPos positon after the insertion of aElem
     * @param aElem element to be inserted
     * @return the position of the inserted element
     */
    inline iterator insert( iterator aPos, const T& aElem );

    /**
     * removes the element at position aPos
     * NOTE: aPos must be valid and != end() otherwise 
     * the end result is undefined
     *
     * @since S60 v3.2
     * @param aPos positon where elemt to be removed
     * @return the position of the next element or the end() 
     * for empty sequence
     */
    inline iterator erase( iterator aPos );

private:

    // deny assignement
	Self& operator=( const Self& aElem );

    // deny copy
    list( const Self& aElem );

private: // data

    /**
     * first node
     * Own.  
     */
    Node*   iFirst;

    /**
     * last node
     * Own.  
     */
    Node*   iLast;

    /**
     * number of elements (nodes)
     */
    TInt    iNumOfElems;
   
    };

#include "osalist.inl"

#endif // WLANLIST_H
