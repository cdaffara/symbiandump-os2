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
* Description:  Inline method implementations of TStaticArrayC.
*
*/


// CONSTANTS
_LIT( KStaticArrayPanic,"StaticArray" );

template <class T>
inline const T& TStaticArrayC<T>::operator[]( TInt aIndex ) const
    {
    if ( ( aIndex >= iCount ) || ( aIndex < 0 ) )
        {
        Panic( EIndexOutOfBounds );
        }

    return  iArray[aIndex];
    }

template <class T>
inline void TStaticArrayC<T>::Panic( TPanicCode aPanicCode ) const
    {
    User::Panic( KStaticArrayPanic, aPanicCode );
    }

// End of File
