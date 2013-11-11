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
* Description:  ?Description
*
*/


// INCLUDE FILES

#include "InlineTextBase.h"

// LOCAL FUNCTIONS

_LIT( KInlineTextPanicText ,"Inline Text Formatting" );

GLDEF_C void Panic( TInlineTextPanic aPanic )
    {
    User::Panic( KInlineTextPanicText, aPanic );
    }

// MODULE TEMPLATES

// MODULE DATA STRUCTURES

// ============================ MEMBER FUNCTIONS ===============================

////////////////////////////////////////////////////////////////////////////////
//
// CInlineTextPositionedText
//
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// 2 stage constructor
// -----------------------------------------------------------------------------

CInlineTextPositionedText* CInlineTextPositionedText::NewL( const TTmDocPos& aPosition, const TDesC& aInlineText )
    {
    CInlineTextPositionedText* self = 
        new (ELeave) CInlineTextPositionedText( aPosition ); // Only partially constructed at this point
    CleanupStack::PushL( self );
    self->ConstructL( aInlineText );
    CleanupStack::Pop();
    return self;
    }

CInlineTextPositionedText::~CInlineTextPositionedText()
    {
    delete iText;
    }

const TTmDocPos& CInlineTextPositionedText::DocPos() const
    {
    return iDocPos;
    }

const TDesC& CInlineTextPositionedText::InlineText() const
    {
    return *iText;
    }

CInlineTextPositionedText::CInlineTextPositionedText( const TTmDocPos& aPosition )
    : iDocPos(aPosition)
    {
    }

void CInlineTextPositionedText::ConstructL( const TDesC& aInlineText )
    {
    iText = aInlineText.AllocL();
    }
////////////////////////////////////////////////////////////////////////////////
//
// CInlineTextStore
//
////////////////////////////////////////////////////////////////////////////////

const TInt KInlineTextStoreGranularity(2);

CInlineTextStore* CInlineTextStore::NewL()
    {
    return new (ELeave) CInlineTextStore();
    }
       
CInlineTextStore::~CInlineTextStore()
    {
    ResetAndDestroy();
    }

void CInlineTextStore::Clear()
    {
    ResetAndDestroy();
    }

void CInlineTextStore::ClearRange( const TTmDocPos& aStart, const TTmDocPos& aEnd )
    {
    TInt nToDelete(0);
    TInt firstToDelete(0);
    for (TInt index = Count() - 1; index >=0 ; --index)
	    {
        CInlineTextPositionedText* inlineText = At(index);
        const TTmDocPos& docPos = inlineText->DocPos();
		if ( aStart <= docPos && docPos <= aEnd )
            {
            delete ( inlineText );
            firstToDelete = index; // running lowest index to delete
            nToDelete++;
            }
		}
    // Perform deletions from array in one go as this is an expensive operation
	if ( nToDelete > 0 )
        Delete( firstToDelete, nToDelete );
    }

void CInlineTextStore::InsertInlineTextL( CInlineTextPositionedText* aInlineText )
    {
    TInt insertPosition(0);
    for (TInt ii = Count()-1; ii >= 0; --ii)
		{
		if ( At(ii)->DocPos() <= aInlineText->DocPos() )
            {
            insertPosition = ii+1;
            break;
            }
		}

    InsertL( insertPosition, aInlineText );
    }

const TTmDocPos* CInlineTextStore::NextInlineTextDocPos( const TTmDocPos& aDocPos ) const
    {
    const TTmDocPos* retPtr = NULL;
    TInt index = NextIndexStartingAtDocPos( aDocPos );
    if ( index >=0 )
        retPtr = &(At( index )->DocPos());
    return retPtr;
    }

TInt CInlineTextStore::NextIndexStartingAtDocPos( const TTmDocPos& aDocPos ) const
    {

    TInt count = Count();
    if ( count == 0 )
        return -1;
      
    if ( At(count-1)->DocPos() < aDocPos  ) // even biggest is at lower docpos
        return -1;

    TInt index;
    for (index = 0; index < count ; ++index)
	    {
		if ( At(index)->DocPos() >= aDocPos  )
            {
            break;
            }
		}

    __ASSERT_DEBUG( index < count , Panic( EInlineTextStoreCorrupted) );

    return index;
    }

TInt CInlineTextStore::IndexFromDocPos( const TTmDocPos& aDocPos ) const
    {
    TInt matchedIndex = -1;
    TInt indexFound = NextIndexStartingAtDocPos( aDocPos );
    if (indexFound != -1 &&  At(indexFound)->DocPos() == aDocPos )
        matchedIndex = indexFound;
    return matchedIndex;
    }

CInlineTextStore::CInlineTextStore() : CArrayPtrFlat<CInlineTextPositionedText>( KInlineTextStoreGranularity )
    {}
 

////////////////////////////////////////////////////////////////////////////////
//
// CInlineTextSource
//
////////////////////////////////////////////////////////////////////////////////

CInlineTextSource::CInlineTextSource()
    {}

void CInlineTextSource::ConstructL()
    {
    iInlineTextStore = CInlineTextStore::NewL();
    }

CInlineTextSource::~CInlineTextSource()
    {
    delete iInlineTextStore;
    }

TInt CInlineTextSource::GetNextInlineTextPosition(
    const TTmDocPos& aFrom, 
    TInt aMaxLength, 
    TTmDocPos& aNext)
    {

    TInt errReturn = KErrNotFound; // Default is that there is nothing found

    const TTmDocPos* nextPos = iInlineTextStore->NextInlineTextDocPos( aFrom );
    // This returns NULL if nothing found   
    if ( nextPos )
        {

        if ( // pos must be strictly less than aFrom + aMaxLength, but may be 1 further if trailing
            (nextPos->iPos < (aFrom.iPos + aMaxLength) )
            || 
            (!nextPos->iLeadingEdge && (nextPos->iPos <= (aFrom.iPos + aMaxLength)) ) 
           )
            {
            aNext = *nextPos; // Copies the structure
            errReturn = KErrNone; // Report valid data
            }
        }
        
    return errReturn;
    }

TPtrC CInlineTextSource::GetInlineText(const TTmDocPos& aPos)
    {
    TInt index = iInlineTextStore->IndexFromDocPos( aPos );
    if ( index >= 0 )
        return iInlineTextStore->At( index )->InlineText();
    else
        {
        __ASSERT_DEBUG( EFalse, Panic(EInlineTextBadInlineTextFetch) );
        return KNullDesC();
        }
    }

TBool CInlineTextSource::HasInlineTextAt(const TTmDocPos& aPos, TPtrC& aPtrFound) const
    {
    TBool retVal( EFalse );
    TInt index = iInlineTextStore->IndexFromDocPos( aPos );
    if ( index != -1 )
        {
        retVal = ETrue;
        aPtrFound.Set(iInlineTextStore->At( index )->InlineText()) ;
        }
    return retVal;
    }

CInlineTextStore* CInlineTextSource::InlineTextStore() const
    {
    return iInlineTextStore;
    }

/**
* Empty implementation as all "when to format policy" is determined by sub-classes.  However, 
* this class implements so that sub-classes only have to implement what they need.
*/
void CInlineTextSource::CheckFormattingL(const TTmDocPos& /*aFrom*/, const TTmDocPos& /*aTo*/ )
    {}

/**
* Empty implementation as all upon-edit policy is determined by sub-classes.  However, 
* this class implements so that sub-classes only have to implement what they need.
*/
void CInlineTextSource::EditObserver( TInt /*aStart*/, TInt /*aExtent*/ ) 
    {}
//  End of File  
