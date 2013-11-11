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
* Description:
*
*/

#include <memspy/engine/memspyengineoutputlist.h>

// System includes
#include <e32debug.h>

// User includes
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include "MemSpyEngineOutputListItem.h"

// Constants
_LIT( KMemSpyOutputListPseudoBlankItem, " " );


CMemSpyEngineOutputList::CMemSpyEngineOutputList( CMemSpyEngineOutputSink* aSink )
:   iSink( aSink ), iFormatBufferPtr( NULL, 0 )
    {
    }


CMemSpyEngineOutputList::~CMemSpyEngineOutputList()
    {
    delete iFormatBuffer;
    iItems.ResetAndDestroy();
    iItems.Close();
    }


void CMemSpyEngineOutputList::ConstructL()
    {
    iFormatBuffer = HBufC::NewL( 1024 );
    iFormatBufferPtr.Set( iFormatBuffer->Des() );
    }


CMemSpyEngineOutputList* CMemSpyEngineOutputList::NewL()
    {
    CMemSpyEngineOutputList* self = new(ELeave) CMemSpyEngineOutputList();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


CMemSpyEngineOutputList* CMemSpyEngineOutputList::NewLC( CMemSpyEngineOutputSink& aSink )
    {
    CMemSpyEngineOutputList* self = new(ELeave) CMemSpyEngineOutputList( &aSink );
    CleanupStack::PushL( self );

    // No need to call ConstructL - it uses the sink's format buffer
    return self;
    }


TInt CMemSpyEngineOutputList::MdcaCount() const
    {
    return iItems.Count();
    }


TPtrC CMemSpyEngineOutputList::MdcaPoint( TInt aPos ) const
    {
    CMemSpyEngineOutputListItem* item = iItems[ aPos ];
    return TPtrC( item->Combined() );
    }


void CMemSpyEngineOutputList::PrintL()
    {
    ASSERT( iSink != NULL );
    PrintL( *iSink );
    }


void CMemSpyEngineOutputList::PrintL( CMemSpyEngineOutputSink& aSink )
    {
    const TInt count = iItems.Count();
    if  ( count > 0 )
        {
        // First pass to get max lengths
        TInt maxLengthCaption = 0;
        TInt maxLengthValue = 0;

        for( TInt j=0; j<count; j++ )
            {
            const CMemSpyEngineOutputListItem* item = iItems[ j ];
			if (item->Value().Length())
				{
	            maxLengthCaption = Max( maxLengthCaption, item->Caption().Length() );
	            maxLengthValue = Max( maxLengthValue, item->Value().Length() );
				}
			else
				{
				// If something doesn't have a value (ie it's a section header, represented as just a caption) then the caption
				// shouldn't be factored into the maxcaptionlength. But consider it in maxlengthValue to make sure we actually
				// make the overall buffers big enough
				maxLengthValue = Max( maxLengthValue, item->Caption().Length() );
				}
            }

        // Second pass - real this time - to print the values
        HBufC* line = HBufC::NewLC( ( maxLengthCaption + maxLengthValue ) + 20 );
        TPtr pLine( line->Des() );
        //
        for( TInt i=0; i<count; i++ )
            {
            const CMemSpyEngineOutputListItem* item = iItems[ i ];

            // Remove initial tabs in caption
            HBufC* caption = MemSpyEngineUtils::CleanupTextLC( item->Caption() );
        
            // Create value item & replace any further tabs
            HBufC* value = MemSpyEngineUtils::CleanupTextLC( item->Value() );

            // Now format the final line, with padding.
			if (value->Length()) 
				{
	            pLine.Justify( *caption, maxLengthCaption + 3, ELeft, TChar(' ') );
				}
			else
				{
				// items without value (ie just captions, ie section headers) aren't constrained by the maxLengthCaption restriction
				pLine.Copy(*caption);
				}
            pLine.Append( *value );
            CleanupStack::PopAndDestroy( 2, caption );

            // Sink output
            aSink.OutputLineL( pLine );
            }
        //
        CleanupStack::PopAndDestroy( line );
        }
    }


CMemSpyEngineOutputListItem& CMemSpyEngineOutputList::Item( TInt aPos )
    {
    CMemSpyEngineOutputListItem* ret = iItems[ aPos ];
    return *ret;
    }


const CMemSpyEngineOutputListItem& CMemSpyEngineOutputList::Item( TInt aPos ) const
    {
    const CMemSpyEngineOutputListItem* ret = iItems[ aPos ];
    return *ret;
    }


void CMemSpyEngineOutputList::AddItemL( const TDesC& aCaption )
    {
    AddItemL( aCaption, KNullDesC );
    }


void CMemSpyEngineOutputList::AddItemL( const TDesC& aCaption, const TDesC& aValue )
    {
    CMemSpyEngineOutputListItem* item = CMemSpyEngineOutputListItem::NewLC( aCaption, aValue );
    iItems.AppendL( item );
    CleanupStack::Pop( item );
    }


void CMemSpyEngineOutputList::AddItemL( const TDesC& aCaption, TInt aValue )
    {
    CMemSpyEngineOutputListItem* item = CMemSpyEngineOutputListItem::NewDecimalLC( aCaption, aValue );
    iItems.AppendL( item );
    CleanupStack::Pop( item );
    }


void CMemSpyEngineOutputList::AddItemL( const TDesC& aCaption, TUint aValue )
    {
    CMemSpyEngineOutputListItem* item = CMemSpyEngineOutputListItem::NewDecimalLC( aCaption, aValue );
    iItems.AppendL( item );
    CleanupStack::Pop( item );
    }


void CMemSpyEngineOutputList::AddItemL( const TDesC& aCaption, const TInt64& aValue )
    {
    CMemSpyEngineOutputListItem* item = CMemSpyEngineOutputListItem::NewLongLC( aCaption, aValue );
    iItems.AppendL( item );
    CleanupStack::Pop( item );
    }


void CMemSpyEngineOutputList::AddItemL( const TDesC& aCaption, TAny* aValue )
    {
    AddItemHexL( aCaption, (TUint) aValue );
    }


void CMemSpyEngineOutputList::AddItemL( const TDesC& aCaption, TUint* aValue )
    {
    AddItemHexL( aCaption, (TUint) aValue );
    }


void CMemSpyEngineOutputList::AddItemL( const TDesC& aCaption, TUint8* aValue )
    {
    AddItemHexL( aCaption, (TUint) aValue );
    }


void CMemSpyEngineOutputList::AddItemFormatL( TRefByValue<const TDesC> aFormat, ... )
    {
	VA_LIST list;
	VA_START(list,aFormat);

    TPtr formatBuffer( FormatBuffer() );
	formatBuffer.Zero();
    formatBuffer.FormatList( aFormat, list );
    //
    AddItemL( formatBuffer, KNullDesC );
    }


void CMemSpyEngineOutputList::AddItemFormatL( const TDesC& aCaption, TRefByValue<const TDesC> aValueFormat, ... )
    {
	VA_LIST list;
	VA_START(list,aValueFormat);

    TPtr formatBuffer( FormatBuffer() );
	formatBuffer.Zero();
    formatBuffer.FormatList( aValueFormat, list );
    //
    AddItemL( aCaption, formatBuffer );
    }


void CMemSpyEngineOutputList::AddItemFormatUCL( TRefByValue<const TDesC> aFormat, ... )
    {
	VA_LIST list;
	VA_START(list,aFormat);

    TPtr formatBuffer( FormatBuffer() );
	formatBuffer.Zero();
    formatBuffer.FormatList( aFormat, list );
    //
    AddItemUCL( formatBuffer, KNullDesC );
    }


void CMemSpyEngineOutputList::AddItemUCL( const TDesC& aCaption, const TDesC& aValue )
    {
    CMemSpyEngineOutputListItem* item = CMemSpyEngineOutputListItem::NewLC( aCaption, aValue );
    iItems.AppendL( item );
    CleanupStack::Pop( item );
    //
    item->iCaption->Des().UpperCase();
    item->UpdateCombinedL();
    }


void CMemSpyEngineOutputList::AddItemFormatUCL( const TDesC& aCaption, TRefByValue<const TDesC> aValueFormat, ... )
    {
	VA_LIST list;
	VA_START(list,aValueFormat);

    TPtr formatBuffer( FormatBuffer() );
	formatBuffer.Zero();
    formatBuffer.FormatList( aValueFormat, list );
    //
    AddItemUCL( aCaption, formatBuffer );
    }


void CMemSpyEngineOutputList::AddItemHexL( const TDesC& aCaption, TUint aValue )
    {
    CMemSpyEngineOutputListItem* item = CMemSpyEngineOutputListItem::NewHexLC( aCaption, aValue );
    iItems.AppendL( item );
    CleanupStack::Pop( item );
    }


void CMemSpyEngineOutputList::AddItemYesNoL( const TDesC& aCaption, TBool aYes )
    {
    CMemSpyEngineOutputListItem* item = CMemSpyEngineOutputListItem::NewYesNoLC( aCaption, aYes );
    iItems.AppendL( item );
    CleanupStack::Pop( item );
    }


void CMemSpyEngineOutputList::AddItemTrueFalseL( const TDesC& aCaption, TBool aTrue )
    {
    CMemSpyEngineOutputListItem* item = CMemSpyEngineOutputListItem::NewTrueFalseLC( aCaption, aTrue );
    iItems.AppendL( item );
    CleanupStack::Pop( item );
    }


void CMemSpyEngineOutputList::AddItemOnOffL( const TDesC& aCaption, TBool aOn )
    {
    CMemSpyEngineOutputListItem* item = CMemSpyEngineOutputListItem::NewOnOffLC( aCaption, aOn );
    iItems.AppendL( item );
    CleanupStack::Pop( item );
    }


void CMemSpyEngineOutputList::AddItemPercentageL( const TDesC& aCaption, TInt aOneHundredPercentValue, TInt aValue )
    {
    const TMemSpyPercentText val( MemSpyEngineUtils::FormatPercentage( TReal( aOneHundredPercentValue ), TReal( aValue ) ) );
    AddItemL( aCaption, val );
    }


void CMemSpyEngineOutputList::AddBlankItemL( TInt aRepetitions )
    {
    const TInt count = Count();
    InsertBlankItemL( count, aRepetitions );
    }


void CMemSpyEngineOutputList::AddUnderlineForPreviousItemL( TChar aUnderlineCharacter, TInt aBlankItemCount )
    {
    const TInt count = iItems.Count();
    if  ( count > 0 )
        {
        InsertUnderlineForItemAtL( count - 1, aUnderlineCharacter, aBlankItemCount );
        }
    }


void CMemSpyEngineOutputList::InsertItemL( TInt aPos, const TDesC& aCaption )
    {
    InsertItemL( aPos, aCaption, KNullDesC );
    }


void CMemSpyEngineOutputList::InsertItemL( TInt aPos, const TDesC& aCaption, const TDesC& aValue )
    {
    CMemSpyEngineOutputListItem* item = CMemSpyEngineOutputListItem::NewLC( aCaption, aValue );
    iItems.InsertL( item, aPos );
    CleanupStack::Pop( item );
    }


void CMemSpyEngineOutputList::InsertItemFormatUCL( TInt aPos, TRefByValue<const TDesC> aValueFormat, ... )
    {
	VA_LIST list;
	VA_START(list,aValueFormat);

    TPtr formatBuffer( FormatBuffer() );
	formatBuffer.Zero();
    formatBuffer.FormatList( aValueFormat, list );
    //
    InsertItemL( aPos, formatBuffer, KNullDesC );
    }


void CMemSpyEngineOutputList::InsertBlankItemL( TInt aPos, TInt aRepetitions )
    {
    while( aRepetitions-- > 0 )
        {
        InsertItemL( aPos, KMemSpyOutputListPseudoBlankItem, KMemSpyOutputListPseudoBlankItem );
        }
    }


void CMemSpyEngineOutputList::InsertUnderlineForItemAtL( TInt aPos, TChar aUnderlineCharacter, TInt aBlankItemCount )
    {
    const CMemSpyEngineOutputListItem& item = Item( aPos );
    
    // Clean text
    HBufC* caption = MemSpyEngineUtils::CleanupTextLC( item.Caption() );
    //RDebug::Print( _L("CMemSpyEngineOutputList::AddUnderlineForPreviousItemL() - [%3d] caption: %S"), caption->Length(), caption );
    HBufC* value = MemSpyEngineUtils::CleanupTextLC( item.Value() );
    //RDebug::Print( _L("CMemSpyEngineOutputList::AddUnderlineForPreviousItemL() - [%3d] value: %S"), value->Length(), value );
    
    // Make underline descriptor
    TBuf<1> underline;
    underline.Append( aUnderlineCharacter );

    // Make underline items
    const TInt lenCaption = caption->Length();
    if ( lenCaption > 0 )
        {
        TPtr pText( caption->Des() );
        pText.Repeat( underline );
        }

    const TInt lenValue = value->Length();
    if ( lenValue > 0 )
        {
        TPtr pText( value->Des() );
        pText.Repeat( underline );
        }

    // Create new item
    InsertItemL( aPos + 1, *caption, *value );

    // Clean up
    CleanupStack::PopAndDestroy( 2, caption );

    // Make blank row if needed
    InsertBlankItemL( aPos + 2, aBlankItemCount );
    }


TPtr& CMemSpyEngineOutputList::FormatBuffer()
    {
    if  ( iSink )
        {
        return iSink->FormatBuffer();
        }
    //
    return iFormatBufferPtr;
    }





