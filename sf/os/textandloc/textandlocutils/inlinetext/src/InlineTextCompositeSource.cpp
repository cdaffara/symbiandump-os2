/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies).
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

#include "InlineTextCompositeSource.h"

// MODULE TEMPLATES


// MODULE DATA STRUCTURES

_LIT( KBadCharacter, " " );

// ============================ MEMBER FUNCTIONS ===============================


EXPORT_C CInlineTextCompositeSource* CInlineTextCompositeSource::NewL()
    { 
    CInlineTextCompositeSource* self  = new (ELeave) CInlineTextCompositeSource();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CInlineTextCompositeSource::~CInlineTextCompositeSource()
    {
    // delete the pointed-to objects because this object owns them
    if (iInlineTextSourceArray)
        iInlineTextSourceArray->ResetAndDestroy();
    delete iInlineTextSourceArray;
    }

EXPORT_C void CInlineTextCompositeSource::InstallInlineTextSourceL( CInlineTextSource* aInlineTextSource )
    {
    if ( aInlineTextSource ) 
        iInlineTextSourceArray->AppendL( aInlineTextSource );
    }


TInt CInlineTextCompositeSource::GetNextInlineTextPosition(
    const TTmDocPos& aFrom, 
    TInt aMaxLength, 
    TTmDocPos& aNext)
    {
    TInt retValue = KErrNotFound;

    // For now, call CheckFormattingL() at the top of each 
    TTmDocPos to( aFrom.iPos + aMaxLength - 1, EFalse );
    if ( to < aFrom )
        {
        to = aFrom; // Makes at least a legal, if zero containing, range
        }
    TRAP_IGNORE( CheckFormattingL( aFrom, to ) );

    // The individual formatters are requested and the minimum position is returned
    TTmDocPos minDocPos(aFrom);
    minDocPos.iPos += ( aMaxLength + 2); // Ensure it is bigger than legal 

    TInt count = iInlineTextSourceArray->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        TTmDocPos thisNextPos;
        TInt err = iInlineTextSourceArray->At(i)
            ->GetNextInlineTextPosition( aFrom, aMaxLength, thisNextPos );
        if ( err == KErrNone ) // one has been found
            {
            if ( thisNextPos < minDocPos )
                {
                minDocPos = thisNextPos;
                retValue = KErrNone;
                }
            }
        }
    if ( retValue == KErrNone )
        aNext = minDocPos;
    return retValue;
    }

//
//
TPtrC CInlineTextCompositeSource::GetInlineText(const TTmDocPos& aAt)
    {
    TInt count = iInlineTextSourceArray->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        TPtrC retPtr;
        if ( iInlineTextSourceArray->At(i)->HasInlineTextAt( aAt, retPtr ) )
            return retPtr;
        }
  
#ifdef _DEBUG    
    TBuf<80> buf;
    buf.Format( _L("Bad fetch of inline text at Pos: %d  Trailing/Leading: %d"), 
        aAt.iPos,
        aAt.iLeadingEdge );
    RDebug::Print( buf );    
    
    __ASSERT_DEBUG( EFalse, Panic( EInlineTextBadInlineTextFetch ) );
#endif

    return KBadCharacter();

    }

//
// Formatting notification is just passed to all inline text formatters
//
void CInlineTextCompositeSource::CheckFormattingL(
    const TTmDocPos& aFrom, const TTmDocPos& aTo )
    {
    TInt count = iInlineTextSourceArray->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iInlineTextSourceArray->At(i)->CheckFormattingL( aFrom, aTo );
        }
    }

CInlineTextCompositeSource::CInlineTextCompositeSource()
    {}

void CInlineTextCompositeSource::ConstructL()
    {
    iInlineTextSourceArray = new (ELeave) CInlineTextSourceArray(1); // Unit granularity
    }

void CInlineTextCompositeSource::EditObserver( TInt aStart, TInt aExtent )
    {
    TInt count = iInlineTextSourceArray->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iInlineTextSourceArray->At(i)->EditObserver( aStart, aExtent );
        }
    }

//  End of File  
