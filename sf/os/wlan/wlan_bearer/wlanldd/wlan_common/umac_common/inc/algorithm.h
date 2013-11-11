/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Generic operation definitions.
*
*/

/*
* %version: 11 %
*/

#ifndef ALGORITHM_H
#define ALGORITHM_H

/**
* global operator!= for type T
* @param aLhs left hand side
* @param aRhs right hand side
* @return ETrue equal, EFalse not equal
*/
template< class T >
inline TBool operator!= (
    const T& aLhs,
    const T& aRhs)
    {
    return !( aLhs == aRhs );
    }

/**
* global operator > for type T
* @param aLhs left hand side
* @param aRhs right hand side
* @return ETrue greater, EFalse else
*/
template< class T >
inline TBool operator > (
    const T& aLhs,
    const T& aRhs)
    {
    return ( aRhs < aLhs );
    }

/**
* global operator <= for type T
* @param aLhs left hand side
* @param aRhs right hand side
* @return ETrue smaller or equal, EFalse else
*/
template< class T >
inline TBool operator <= (
    const T& aLhs,
    const T& aRhs)
    {
    return !( aRhs < aLhs );
    }

/**
* global operator >= for type T
* @param aLhs left hand side
* @param aRhs right hand side
* @return ETrue greater or equal, EFalse else
*/
template< class T >
inline TBool operator >= (
    const T& aLhs,
    const T& aRhs)
    {
    return !( aLhs < aRhs );
    }

/**
* Just like equal in C++ STL for testing equality for T type
* Checks weather elements in the range [aBeg, aEnd) are equal 
* to the elements in the range starting with aCmpBeg
* Complexity: linear
* @param aBeg begin of the search range
* @param aEnd end ( one past last element ) of the search range
* @param aCmpBeg begin of the range to be compared with
* @return ETrue equal, EFalse not equal
*/
template< class T >
inline TBool equal( 
    const T* aBeg, 
    const T*const aEnd, 
    const T* aCmpBeg)
    {
    while ( aBeg != aEnd )
        {
        if ( *aBeg != *aCmpBeg )
            {
            return EFalse;
            }
        ++aBeg;
        ++aCmpBeg;
        }

    return ETrue;
    }

/**
* Just like find in C++ STL
* Returns the position of the first element in the range [aBeg, aEnd)
* that has a value equal to aValue
* Complexity: linear
* @param aBeg begin of range
* @param aEnd  end ( one past last element ) of the range
* @param aValue value to be searched
* @return the position of the first element in the range (aBeg, aEnd]
* that has a value equal to aValue. aEnd is returned if no matching 
* element is found
*/
template< class T, class J >
inline T* find(
    const T* aBeg,
    const T* const aEnd,
    const J& aValue)
    {
    while ( aBeg != aEnd )
        {
        if ( *aBeg == aValue )
            {
            break;
            }

        ++aBeg;
        }

    return const_cast<T*>(aBeg);
    }

/**
* Just like find_if in C++ STL
* Returns the position of the first element in the range [aBeg, aEnd)
* for which the unary predicate op(elem) yields true
* Complexity: linear
* @param aBeg begin of range
* @param aEnd  end ( one past last element ) of the range
* @param aUnaryPredicate a unary predicate
* @return the position of the first element in the range (aBeg, aEnd]
* for which the unary predicate op(elem) yields true. 
* aEnd is returned if no matching element is found
* NOTE: aUnaryPredicate should not change its state during a function call
*/
template< class T, class J >
inline T* find_if(
    const T* aBeg,
    const T* const aEnd,
    const J& aUnaryPredicate)
    {
    while ( aBeg != aEnd )
        {
        if ( aUnaryPredicate( *aBeg ) )
            {
            break;
            }

        ++aBeg;
        }

    return const_cast<T*>(aBeg);
    }

/**
* Just like fill in C++ STL for T type
* Assigns aValue to each element in the range [aBeg, aEnd)
* The caller must ensure that the destination range is big enough 
* Complexity: linear
* @param aBeg begin of range
* @param aEnd  end ( one past last element ) of the range
* @param aValue value to be assigned
*/
template< class T, class J>
inline void fill(
    T* aBeg,
    const T* const aEnd,
    const J& aValue)
    {
    while ( aBeg != aEnd )
        {
        *aBeg = aValue;
        ++aBeg;
        }
    }

/**
* Just like fill_n in C++ STL for T type
* Assigns aValue to the first aNum elements in the range starting with aBeg
* The caller must ensure that the destination range is big enough 
* Complexity: linear
* @param aBeg begin of range
* @param aNum number of elements to be processed
* @param aValue value to be assigned
*/
template< class T, class J >
inline void fill_n(
    T* aBeg,
    TUint32 aNum,
    const J& aValue)
    {
    while ( aNum )
        {
        *aBeg = aValue;
        ++aBeg;
        --aNum;
        }
    }

/**
* Just like copy in C++ STL 
* @param aSrc begin of copy
* @param aSrcEnd end of copy
* @param aDest target of copy
* @return aDest
*/
template<class T>
inline T* copy(
    const T* aSrc,
    const T* aSrcEnd,
    T* aDest)
    {
    while ( aSrc != aSrcEnd )
        {
        *aDest = *aSrc;
        ++aSrc;
        ++aDest;
        }

    return aDest;
    }

template<class T>
inline T* reverse_copy(
    const T* aSrcBeg,
    const T* aSrcEnd,
    T* aDest)
    {
    --aSrcEnd;
    while ( aSrcEnd >= aSrcBeg )
        {        
        *aDest = *aSrcEnd;
        --aSrcEnd;
        ++aDest;
        }

    return aDest;
    }

/**
* Does a looped copy for T type
* @param aSource source of the copy
* @param aDest destination where to copy
* @param aCount number of iterations
* @return aDest
*/
template<class T>
inline TAny* assign(
    const T* aSource,
    T* aDest,
    const TInt aCount)
    {
    TAny* origdest = static_cast<TAny*>(aDest);
    for ( TInt idx = 0 ; idx != aCount ; ++idx, ++aSource, ++aDest )
        {
        *aDest = *aSource;
        }

    return origdest;
    }

#endif      // ALGORITHM_H
