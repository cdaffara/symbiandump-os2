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
* Description:   Carray inline implementations
*
*/

/*
* %version: 3 %
*/

// -----------------------------------------------------------------------------
// assignment operator 
// -----------------------------------------------------------------------------
//
template<class T, TInt the_size,  class CopyingPolicy>
inline Carray<T, the_size, CopyingPolicy>& 
Carray<T, the_size, CopyingPolicy>::operator=( const Self& o )
    {
    // use policy chosen at compile time
    Copy( 
        const_cast<iterator>(o.begin()), 
        const_cast<iterator>(o.end()), 
        begin() );      

	return (*this);
    }

// -----------------------------------------------------------------------------
// operator == 
// -----------------------------------------------------------------------------
//
template<class T, TInt the_size, class CopyingPolicy>
inline TBool operator == (
    const Carray<T, the_size, CopyingPolicy>& x, 
    const Carray<T, the_size, CopyingPolicy >& y )
    {
    return equal( x.begin(), x.end(), y.begin() );
    }

