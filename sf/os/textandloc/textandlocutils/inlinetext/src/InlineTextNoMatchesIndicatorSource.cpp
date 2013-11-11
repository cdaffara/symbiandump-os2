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
#include <tagma.h>
#include "InlineTextNoMatchesIndicatorSource.h"

// MODULE TEMPLATES

// MODULE DATA STRUCTURES

// LOCAL CONSTANTS
_LIT(KTextForUnknownPredictiveTextWestern, "?" );
_LIT(KTextForUnknownPredictiveTextArabic, "\x061f" );   // Arabic Question Mark

#define ARABIC_CODE_PAGE_BASE   0x0600

// ============================ MEMBER FUNCTIONS ===============================


////////////////////////////////////////////////////////////////////////////////
//
// CInlineTextNoMatchesIndicatorSource
//
////////////////////////////////////////////////////////////////////////////////

EXPORT_C CInlineTextNoMatchesIndicatorSource* CInlineTextNoMatchesIndicatorSource::NewL( const MTmSource& aTextSource )
    {
    CInlineTextNoMatchesIndicatorSource* self = 
        new (ELeave) CInlineTextNoMatchesIndicatorSource( aTextSource );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CInlineTextNoMatchesIndicatorSource::~CInlineTextNoMatchesIndicatorSource()
    {
    }

void CInlineTextNoMatchesIndicatorSource::CheckFormattingL(const TTmDocPos& aFrom, const TTmDocPos& aTo )
    {
    DoFormatL( aFrom, aTo );
    }

void CInlineTextNoMatchesIndicatorSource::DoFormatL( const TTmDocPos& aFrom, const TTmDocPos& aTo)
    {
    // Check if this contains no text. Can we adjust it to contain at least something?
    TTmDocPos from( aFrom );
    if ( aTo.iPos - from.iPos <= 0 && from.iPos > 0 )
        {
        from.iPos--;
        from.iLeadingEdge = ETrue;
        }
        
    InlineTextStore()->ClearRange( aFrom, aTo );

    TTmCharFormat format;
    TPtrC buffer;
    TBool endOfPara(EFalse);

    TInt charPos = from.iPos;
    // Process the buffer within the format 
    while ( charPos <= aTo.iPos && !endOfPara )
        {
        iTextSource.GetText( charPos, buffer, format );

        // Check for end of paragraph mark 
        TInt lengthInFormat = buffer.Length();
        // trim off the paragraph delimeter from the format chunk, or at least for our book-keeping of it
        if ( buffer[lengthInFormat-1] == KInlineTextParagraphDelimiter )
            {
            lengthInFormat--;
            endOfPara = ETrue;
            }

        if ( format.iEffects & TTmCharFormat::ENoMatchesIndicator )
            {
            // Have to ensure that this is at the logical end of the markup (eg, there could be more 
            // marked up text wrapped to the next line)
            if ( !FormatOfNextCharacterIsUnknownInlineFepTextStyle( charPos + lengthInFormat ) )
                // Store at end/trailing           
                StoreNoMatchesIndicatorInlineTextL( charPos + lengthInFormat, EFalse, buffer );
            }

        charPos += lengthInFormat;
        }

#ifdef INLINE_EDIT_DUMPING
    // Dumping code
    TBuf<80> buf;
    RDebug::Print(_L("\nDump of inline texts"));
    for ( TInt index = 0; index < InlineTextStore()->Count(); index++)
        {

        CInlineTextPositionedText* inlineText = InlineTextStore()->At( index );
        TPtrC ptr = inlineText->InlineText();
        buf.Format( _L("DocPos: %d  Trailing/Leading: %d   Text=<%S>"), 
            inlineText->DocPos().iPos,
            inlineText->DocPos().iLeadingEdge, &ptr );

        RDebug::Print( buf );
        }
#endif

    }

CInlineTextNoMatchesIndicatorSource::CInlineTextNoMatchesIndicatorSource( const MTmSource& aTextSource )
 : CInlineTextSource(), iTextSource( aTextSource )
    {}

void CInlineTextNoMatchesIndicatorSource::StoreNoMatchesIndicatorInlineTextL( 
    TInt aPos, 
    TBool aLeadingEdge,
    const TDesC& aSampleText )
    {
    TTmDocPos docPos( aPos, aLeadingEdge );

    CInlineTextPositionedText* inlineText = 
        CInlineTextPositionedText::NewL( 
            docPos, 
            TextForUnknownPredictiveTextIndication( aSampleText) );

    CleanupStack::PushL( inlineText );
    InlineTextStore()->InsertInlineTextL( inlineText );
    CleanupStack::Pop(); // inlineText
    }

TPtrC CInlineTextNoMatchesIndicatorSource::TextForUnknownPredictiveTextIndication( const TDesC& aSampleText ) const
    {
    TUint codebase = CodeBaseOfText( aSampleText);
    if ( codebase == ARABIC_CODE_PAGE_BASE )
        return KTextForUnknownPredictiveTextArabic(); //unknownTextBuffer;
    else
        return KTextForUnknownPredictiveTextWestern(); //unknownTextBuffer;
    }
/**
* Implementation is not safe for surrogate pairs
*/
TUint CInlineTextNoMatchesIndicatorSource::CodeBaseOfText( const TDesC& aSampleText ) const
    {
    TUint base = 0;

    // examine buffer; take first printable character
    // if none found, then 0 is returned
    TChar character;
    for ( TInt index = 0;index < aSampleText.Length(); index++)
        {
        character = aSampleText[index];
        if ( character.IsPrint() )
            {
            TUint ch(character); 
            ch &= 0xffffff00; // only interested in upper bits
            base = (TUint)ch;
            break;
            }
        }

    return base;
    }

TBool CInlineTextNoMatchesIndicatorSource::FormatOfNextCharacterIsUnknownInlineFepTextStyle( TInt aNextPos) const
    {
    TBool effectIsUnknownInlineFepText(EFalse);

    TPtrC textPtr;

    // Use separate, new format on the stack to hold the info on the next character
    TTmCharFormat format;

    // Get characters immediately following current chunk.
    iTextSource.GetText( aNextPos, textPtr, format );

    // Must be at least one character
    if ( textPtr.Length() >= 1 )
    // Check if it is the correct effect.
    if (format.iEffects & TTmCharFormat::ENoMatchesIndicator )
            effectIsUnknownInlineFepText = ETrue;

    return effectIsUnknownInlineFepText;
    }

//  End of File  
