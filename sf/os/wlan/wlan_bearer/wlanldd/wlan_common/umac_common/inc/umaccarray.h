/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class template for constant size arrays.
*
*/

/*
* %version: 8 %
*/

#ifndef _CARRAY_H
#define _CARRAY_H

/*
 *
 * The following code declares class array,
 * an STL container (as wrapper) for arrays of constant size.
 *
 * compile time modifiable polies:
 * - to call delete or not to 4 members of array
 * - copy ctor and assignment operator policies; see umaccarraypolicy.h         
 */

#include "algorithm.h"

typedef TUint size_t;

#include "umactypemanip.h"
#include "umaccarraypolicy.h"

template
<
        class T, 
        size_t the_size,
        bool deletepointees = EFalse,           // call delete or not 4 members of array
        class CopyingPolicy = ShallowCopy<T>,   // check out policyCarray.h 4 these
        class HeapAllocationPolicy = OpNewAlloc // our allocation policy 4 a heap allocated Carray         
> 
class Carray :  protected CopyingPolicy,
                public HeapAllocationPolicy 
{
	typedef Carray<T, the_size, deletepointees, CopyingPolicy, HeapAllocationPolicy> Self;

private:
	T v[the_size];

        // compile time function selectives
        void delete_pointees( Int2Type<ETrue> );
        void delete_pointees( Int2Type<EFalse> ) {};

public:

	// type definitions
	typedef T		    value_type;
	typedef T*		    iterator;
	typedef const T*	const_iterator;
	typedef T&		    reference;
	typedef const T&	const_reference;
	typedef size_t	    size_type;
	typedef size_t	    TUint;
	
	// default constructor
	Carray() {};
	// construct with inital data
	// do not allow to be used in explicit conversations
	explicit 
        Carray(T* p) { copy(p, p + the_size, begin()); }

    ~Carray() { delete_pointees( Int2Type<deletepointees>() ); }

	// iterator support
	iterator begin() {return v;}
	const_iterator begin() const {return v;}
	iterator end() {return v + the_size;}
	const_iterator end() const {return v + the_size;}

	// size is constant
	size_type size() const {return the_size;}
	size_type max_size() const {return the_size;}

	// direct element access
	reference operator[](size_t i) {return v[i];} 
	const_reference operator[](size_t i) const {return v[i];} 

	// front() and back()
        reference front() {return v[0]; }
        const_reference front() const {return v[0]; }
        reference back() {return v[the_size-1]; }
        const_reference back() const {return v[the_size-1];}

	//swap
    void swap(Self& r) { swap_ranges(begin(), end(), r.begin()); }

	// conversion to ordinary array
	const T* data() const {return v;}
	T* data() {return v;}

	// assignment
	Self& operator=(const Self&);

	// copy ctor
    Carray(const Self& o) 
        { Copy(const_cast<iterator>(o.begin()), const_cast<iterator>(o.end()), begin()); }
};

template<class T, size_t the_size, bool deletepointees, 
class CopyingPolicy, class HeapAllocationPolicy>
inline
void Carray<T, the_size, deletepointees, CopyingPolicy, 
HeapAllocationPolicy>::delete_pointees(Int2Type<ETrue>)
    {
    for( size_t i = 0 ; i < size() ; ++i )
        {
        delete v[i];
        }
    }

// asignment operator
template<class T, size_t the_size,  bool deletepointees, class CopyingPolicy, class HeapAllocationPolicy>
inline
Carray<T, the_size, deletepointees, CopyingPolicy, HeapAllocationPolicy>& 
Carray<T, the_size, deletepointees, CopyingPolicy, HeapAllocationPolicy>::operator=(const Self& o)
    {
    // use policy chosen at compile time
    Copy(const_cast<iterator>(o.begin()), const_cast<iterator>(o.end()), begin());      
	return (*this);
    }

// comparisons
template<class T, size_t the_size, bool deletepointees, class CopyingPolicy, class HeapAllocationPolicy>
inline
bool operator == (const Carray<T, the_size, deletepointees, CopyingPolicy, HeapAllocationPolicy>& x, 
const Carray<T, the_size, deletepointees, CopyingPolicy, HeapAllocationPolicy>& y)
    {
    return equal(x.begin(), x.end(), y.begin());
    }

// global swap()
template<class T, size_t the_size, bool deletepointees, class CopyingPolicy, class HeapAllocationPolicy>
inline void swap (Carray<T, the_size, deletepointees, CopyingPolicy, HeapAllocationPolicy>& x, 
Carray<T, the_size, deletepointees, CopyingPolicy, HeapAllocationPolicy>& y)
    {
    x.swap(y);
    }

#endif //_CARRAY_H
