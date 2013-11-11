/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
* 
* Description: This file contains implementation of TestEngineClient's 
* inline functions.
*
*/

#ifndef TEST_ENGINE_CLIENT_INL
#define TEST_ENGINE_CLIENT_INL

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION

/*
-------------------------------------------------------------------------------

    Class:CFixedFlatArray

    Method: CFixedFlatArray

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
template <class T>
CFixedFlatArray<T>::CFixedFlatArray() :
    iArray( NULL ),
    iBufferPtr(0,0)
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CFixedFlatArray

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: const TInt aCount: in: Count of contents

    Return Values: None

    Errors/Exceptions: Leaves if memory allocation for iArray of iBuffer fails

    Status: Approved

-------------------------------------------------------------------------------
*/
template <class T>
void CFixedFlatArray<T>::ConstructL( const TInt aCount )
    {

    iCount = aCount;
    iArray = new (ELeave) T[iCount];

    const TInt size = iCount * sizeof (T);
    iBuffer = HBufC8::NewMaxL ( size );

    iBufferPtr.Set( iBuffer->Des() );
    iBufferPtr.SetLength( size );
    iBufferPtr.Set( (TUint8*) iArray, size,size );

    }

/*
-------------------------------------------------------------------------------

    Class: CFixedFlatArray

    Method: NewL

    Description: Two-phased constructor.

    Parameters: None

    Return Values: CFixedFlatArray<T>: Array of CFixedFlatArray<T> objects

    Errors/Exceptions: Leaves if ConstructL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
template <class T>
CFixedFlatArray<T>* CFixedFlatArray<T>::NewL(TInt aSize) 
    {
    CFixedFlatArray<T>* self = new ( ELeave ) CFixedFlatArray<T>();
    CleanupStack::PushL( self );
    self->ConstructL(aSize);
    CleanupStack::Pop();
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CFixedFlatArray

    Method: ~CFixedFlatArray

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/    
template <class T>
CFixedFlatArray<T>::~CFixedFlatArray()
    {    
    delete[] iArray;
    delete iBuffer;

    }

/*
-------------------------------------------------------------------------------

    Class: CFixedFlatArray

    Method: Des

    Description: Returns descriptor for array

    Parameters: None

    Return Values: TPtr8&: Descriptor for array

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/ 
template <class T>
TPtr8& CFixedFlatArray<T>::Des()
    {   
    return iBufferPtr;

    }

/*
-------------------------------------------------------------------------------

    Class: CFixedFlatArray

    Method: operator[]

    Description: Returns index operator

    Parameters: TInt aIndex: in: Index of object to be returned

    Return Values: T&: Reference to requested object

    Errors/Exceptions: Panics if aIndex is out of range

    Status: Approved

-------------------------------------------------------------------------------
*/ 
template <class T>
T& CFixedFlatArray<T>::operator[] ( TInt aIndex ) const
    {
    CheckIndex( aIndex );

    return iArray[aIndex];

    }

/*
-------------------------------------------------------------------------------

    Class: CFixedFlatArray

    Method: Count

    Description: Returns count

    Parameters: None

    Return Values: TInt: iCount

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
template <class T>
TInt CFixedFlatArray<T>::Count() const
    {
    return iCount;

    }

/*
-------------------------------------------------------------------------------

    Class: CFixedFlatArray

    Method: Set

    Description: Set buffer to array.

    Parameters: TInt aIndex: in: Index
                T& aBuf: in: Buffer

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
template <class T>
void CFixedFlatArray<T>::Set( TInt aIndex, T& aBuf )
    {
    CheckIndex( aIndex );

    iArray[aIndex] = aBuf;

    }

/*
-------------------------------------------------------------------------------

    Class: CFixedFlatArray

    Method: CheckIndex

    Description: Set buffer to array.

    Parameters: TInt aIndex: in: Index to be checked

    Return Values: None

    Errors/Exceptions: Panics if index is incorrect

    Status: Approved

-------------------------------------------------------------------------------
*/
template <class T>
void CFixedFlatArray<T>::CheckIndex( TInt aIndex ) const
    {
    if ( aIndex < 0 || aIndex >= iCount )
        {
        User::Panic( _L( "CFixedFlatArray: Array index out of range" ), KErrArgument );
        }

    }

#endif      // TEST_ENGINE_CLIENT_INL

// End of File
