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
* Description:   wrapper on top of C array with C++ STL interface
*
*/

/*
* %version: 3 %
*/

#ifndef WLANCARRAY_H
#define WLANCARRAY_H

#include "algorithm.h"

#include "osacarraypolicy.h"

/**
 *  wrapper on top of C array with C++ STL interface
 *
 *
 *  @lib wlanosa.lib
 *  @since S60 v3.2
 */
template
<
    class T, 
    TInt the_size,
    // check out osacarraypolicy.h 4 these
    class CopyingPolicy = NoCopy<T>
> 
class Carray : protected CopyingPolicy, public DBase
    {

	typedef Carray<T, the_size, CopyingPolicy> Self;

public:

	// type definitions
	typedef T		    value_type;
	typedef T*		    iterator;
	typedef const T*	const_iterator;
	typedef T&		    reference;
	typedef const T&	const_reference;
	typedef TInt	    size_type;
	
    /**
     * Constructor.
     *
     * @since S60 v3.2
     */
	Carray() {};

    /**
     * Constructor. Construct with inital data
     *
     * @since S60 v3.2
     * @param aP initial data
     */
	explicit inline Carray( T* aP ) { copy( aP, aP + the_size, begin()); }

    /**
     * returns iterator to begin of the sequence
     *
     * @since S60 v3.2
     * @return iterator to begin of the sequence
     */
    inline iterator begin() { return v; }

    /**
     * returns const iterator to begin of the sequence
     *
     * @since S60 v3.2
     * @return const iterator to begin of the sequence
     */
    inline const_iterator begin() const { return v; }

    /**
     * returns iterator to end of the sequence
     *
     * @since S60 v3.2
     * @return iterator to end of the sequence
     */
    inline iterator end() { return v + the_size; }

    /**
     * returns const iterator to end of the sequence
     *
     * @since S60 v3.2
     * @return const iterator to end of the sequence
     */
    inline const_iterator end() const { return v + the_size; }

    /**
     * returns size of the storage
     *
     * @since S60 v3.2
     * @return size of the storage
     */
    inline size_type size() const { return the_size; }

    /**
     * returns max size of the storage
     *
     * @since S60 v3.2
     * @return max size of the storage
     */
    inline size_type max_size() const { return the_size; }

    /**
     * non const [] operator access
     *
     * @since S60 v3.2
     * @return non const reference to element
     */
    inline reference operator[](TInt i) { return v[i]; } 

    /**
     * const [] operator access
     *
     * @since S60 v3.2
     * @return const reference to element
     */
    inline const_reference operator[](TInt i) const { return v[i]; } 

    /**
     * return non const reference to front of sequence
     *
     * @since S60 v3.2
     * @return non const reference to front of sequence
     */
    inline reference front() { return v[0]; }

    /**
     * return const reference to front of sequence
     *
     * @since S60 v3.2
     * @return const reference to front of sequence
     */
    inline const_reference front() const { return v[0]; }

    /**
     * return non const reference to back of sequence
     *
     * @since S60 v3.2
     * @return non const reference to back of sequence
     */
    inline reference back() { return v[the_size-1]; }

    /**
     * return const reference to back of sequence
     *
     * @since S60 v3.2
     * @return const reference to back of sequence
     */
    inline const_reference back() const { return v[the_size-1]; }

    /**
     * conversion to ordinary array
     *
     * @since S60 v3.2
     * @return const pointer to begin of the sequence
     */
    inline const T* data() const { return v; }

    /**
     * conversion to ordinary array
     *
     * @since S60 v3.2
     * @return non const pointer to begin of the sequence
     */
    inline T* data() { return v; }

    /**
     * assignment operator
     *
     * @since S60 v3.2
     */
	Self& operator=( const Self& );

    /**
     * copy constructor
     *
     * @since S60 v3.2
     */
    inline Carray( const Self& o ) 
        { 
        Copy( 
            const_cast<iterator>(o.begin()), 
            const_cast<iterator>(o.end()), 
            begin() ); 
        }

private:

	T v[the_size];
};

#include "osacarray.inl"

#endif // WLANCARRAY_H
