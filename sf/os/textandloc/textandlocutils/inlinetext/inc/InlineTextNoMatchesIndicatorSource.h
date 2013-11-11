/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*     Classes used for holding inline text for the Form MTmInlineTextSource interface.
*
*/


#ifndef INLINETEXTNOMATCHESINDICATORSOURCE_H
#define INLINETEXTNOMATCHESINDICATORSOURCE_H

// INCLUDES

#include "InlineTextBase.h"
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <tagma.h>
#else
#include <tagma.h>
#include <tagmalayoutandsource.h>
#endif

// CONSTANTS

// CLASS DECLARATION

/**
* Concrete implementation of MTmInlineTextSource for marking text has no predictive
* text entry matches.
* 
*  @lib InlineText
*  @since 3.2
*/
NONSHARABLE_CLASS(CInlineTextNoMatchesIndicatorSource) : public CInlineTextSource
    {
    public:
    // 2 stage constructor
    IMPORT_C static CInlineTextNoMatchesIndicatorSource* NewL( const MTmSource& aTextSource );
    // destructor
    ~CInlineTextNoMatchesIndicatorSource();

    public: // From CInlineTextSource
    /**
    * Implementation of framework method. Documented in header
    */
    virtual void CheckFormattingL(const TTmDocPos& aFrom, const TTmDocPos& aTo );

    private:
    /**
    * C++ constructor. 
    */
    CInlineTextNoMatchesIndicatorSource( const MTmSource& aTextSource );

    /**
    * non-virtual implementation method for CheckFormattingL
    */
    void DoFormatL( const TTmDocPos& aFrom, const TTmDocPos& aTo );

    /**
    * Framework method implemented to store the inline text at the position and leading-ness
    *
    * @param aPos           Position of gap ( 0 = before charaacter 0; 1 = after character 0/before char 1 ...)
    * @param aLeadingEdge   use ETrue if the inline text is associated with the next character
    * @param aTextSample    text upon which to base decision as to what No Matches text
    *                       to use
    */
    void StoreNoMatchesIndicatorInlineTextL( 
        TInt aPos,
        TBool aLeadingEdge,
        const TDesC& aTextSample );

    /**
    * This returns a text sensitive no matches indication
    *
    * @param aRightToLeft   ETrue if chunk is right to left directional text.
    * @param aBuffer        const TDesC& buffer containing text to determine language dependent, 
    * @returns a TPtr to the No Matches indication text to use for this buffer
    */
    TPtrC TextForUnknownPredictiveTextIndication( const TDesC& aTextSample ) const;

    /**
    * Returns a value that is the base value of a unicode code page. The significant content of
    * the buffer ( indices [1,aBufferLength]) and is examined from low to high index.  
    * Code page is the here defined as the unicode value excluding its lowest byte - Masked 
    * by 0xFF00 
    *
    * If a character with non-zero code page is found, that value is returned.  Otherwise, 0x0000
    * is returned
    * 
    * To totally dependable, this routine depends upon the chunk being of uniform codepage, 
    * or mixed codepage 0 + one other codepage.  For no-matches indicator text, this is the case
    *
    * @param aTextSample    buffer containing text, 
    * @returns              Codepage of first
    */

    TUint CodeBaseOfText( const TDesC& aTextSample ) const;

    /**
    * Utility method to test whether the next character (in the next chunk) is also in the 
    * No matches indicator markup.
    *
    * @param aNexPos        position in the text source of the next character (if there is one)
    * @returns              ETrue only if there is another character and that is has the
    *                       no matches indicator markup.
    */
    TBool FormatOfNextCharacterIsUnknownInlineFepTextStyle( TInt aNextPos) const;

    private:
    const MTmSource& iTextSource;
    };

#endif

// End of File
