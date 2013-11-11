/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

#include "CSortKeyArray.h"

namespace {

#ifdef _DEBUG
    
enum TPanicReason
    {
    EPanicPreCond_SetText
    };
    
void Panic( TPanicReason aReason )
    {
    _LIT( KPanicText, "CSortKeyArray" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG

} // namespace

// -----------------------------------------------------------------------------
// default construcotr
// -----------------------------------------------------------------------------
//
inline CSortKeyArray::CSortKeyArray()
    {
    }

// -----------------------------------------------------------------------------
// 2-nd construcotr
// -----------------------------------------------------------------------------
//
CSortKeyArray* CSortKeyArray::NewL()
    {
    return new(ELeave) CSortKeyArray;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CSortKeyArray::~CSortKeyArray()
    {
    iKeyTypes.Close();
    iTexts.Close();
    }

// -----------------------------------------------------------------------------
// CSortKeyArray::AppendL
// -----------------------------------------------------------------------------
//
void CSortKeyArray::AppendL( const TSortKey& aKey )
    {
    User::LeaveIfError( iKeyTypes.Append( aKey.Type() ) );
    TInt err = iTexts.Append( aKey.Text() );
    if ( err != KErrNone )
        {
        iKeyTypes.Remove( iKeyTypes.Count() - 1 );
        User::Leave( err );
        }
    }

// -----------------------------------------------------------------------------
// CSortKeyArray::SetText
// -----------------------------------------------------------------------------
//
void CSortKeyArray::SetText( const TDesC& aText, TInt aIndex )
    {
    __ASSERT_DEBUG( aIndex < iTexts.Count(), Panic( EPanicPreCond_SetText ) );
    iTexts[aIndex].Set( aText );
    }

// -----------------------------------------------------------------------------
// CSortKeyArray::Reset
// -----------------------------------------------------------------------------
//
void CSortKeyArray::Reset()
    {
    iKeyTypes.Reset();
    iTexts.Reset();
    }

// -----------------------------------------------------------------------------
// CSortKeyArray::SortKeyCount
// -----------------------------------------------------------------------------
//
TInt CSortKeyArray::SortKeyCount() const
    {
    return iKeyTypes.Count();
    }

// -----------------------------------------------------------------------------
// CSortKeyArray::SortKeyAt
// -----------------------------------------------------------------------------
//
TSortKey CSortKeyArray::SortKeyAt( TInt aIndex ) const
    {
    return TSortKey( iTexts[aIndex], iKeyTypes[aIndex] );
    }

// End of File
