/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Policies for constant size arrays class template 
*                (see umaccarray.h). 
*
*/

/*
* %version: 10 %
*/

#ifndef _POLICYCARRAY_H
#define _POLICYCARRAY_H

/*
 *
 * Copy ctor and assignment operator policies 4 Carray
 *
 * @usage examples:
 *
 *       Carray<int* , 2, true, DeepCopy<int> > obj;
 *       Carray<int* , 2, true, DeepCopy<int> > obj2;
 *       obj[0] = new int(1);
 *       obj[1] = new int(2); 
 *       // test deep copy
 *       Carray<int* , 2, true, DeepCopy<int> > obj6(obj);
 *       // test deep assignment
 *       obj2 = obj6;
 *
 *       Carray<int , 2> obj3;
 *       Carray<int , 2> obj4;
 *       obj3[0] = 1;
 *       obj3[1] = 2;
 *       // test shallow copy
 *       Carray<int , 2> obj5(obj3);
 *       // test shallow assignment
 *      obj4 = obj3;
 *
 *       Carray<clonable* , 2, true, CloneCopy<clonable> > obj7;
 *       Carray<clonable* , 2, true, CloneCopy<clonable> > obj9;
 *       obj7[0] = new clonable(1);
 *       obj7[1] = new clonable(2);
 *       // test clone copy
 *       Carray<clonable* , 2, true, CloneCopy<clonable> > obj8(obj7);
 *       // test clone assignment
 *       obj9 = obj7;
 *
 *       Carray<int , 2, false, NoCopy<int> > obj10;
 *       Carray<int , 2, false, NoCopy<int> > obj11;
 *       // does not compile as it should not
 *       obj10 = obj11;
 */

#include "algorithm.h"

// normal shallow copy
template <class T>
struct ShallowCopy
{
typedef T* iterator;

        static void Copy(iterator srcbegin, iterator srcend, iterator trgbegin)
        {
                copy(srcbegin, srcend, trgbegin);
        }
};

// calls Clone() 4 copy
template <class T>
struct CloneCopy
{
typedef T** iterator;

        static void Copy(iterator srcbegin, iterator srcend, iterator trgbegin)
        {
                while(srcbegin != srcend)
                {
                        *trgbegin = (*srcbegin)->Clone();

                        ++srcbegin;
                        ++trgbegin;
                }
        }
};

// just what name states copying and assignment are not allowed
template <class T>
struct NoCopy {};

// global new and delete operators
struct OpNewAlloc
{
        static void* operator new(size_t size)
        {
                return ::operator new(size);
        }
        
        static void operator delete(void* p , size_t /*size*/)
        {
                ::operator delete(p);
        }
};

#endif //_POLICYCARRAY_H
