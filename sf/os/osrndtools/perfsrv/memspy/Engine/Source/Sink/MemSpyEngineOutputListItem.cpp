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

#include "MemSpyEngineOutputListItem.h"

// System includes
#include <e32debug.h>

// User includes
#include <memspy/engine/memspyengineutils.h>

// Constants
const TInt KMemSpyNumericFormatBufferSize = 20;

// Literal constants
_LIT(KMemSpyNumericHexFormat, "0x%08x");
_LIT(KMemSpyNumericUnsignedFormat, "%u");
_LIT(KMemSpyNumericDecFormat, "%d");
_LIT(KMemSpyNumericLongFormat, "%Ld");
_LIT(KMemSpyCaptionYes, "Yes");
_LIT(KMemSpyCaptionNo, "No");
_LIT(KMemSpyCaptionOn, "On");
_LIT(KMemSpyCaptionOff, "Off");
_LIT(KMemSpyCaptionTrue, "True");
_LIT(KMemSpyCaptionFalse, "False");



CMemSpyEngineOutputListItem::CMemSpyEngineOutputListItem()
    {
    }


CMemSpyEngineOutputListItem::~CMemSpyEngineOutputListItem()
    {
    delete iCaption;
    delete iValue;
    delete iCombined;
    }


void CMemSpyEngineOutputListItem::ConstructL( const TDesC& aCaption, const TDesC& aValue )
    {
    iCaption = aCaption.AllocL();
    iValue = aValue.AllocL();
    //
    UpdateCombinedL();
    }


CMemSpyEngineOutputListItem* CMemSpyEngineOutputListItem::NewLC( const CMemSpyEngineOutputListItem& aCopyMe )
    {
    CMemSpyEngineOutputListItem* self = new(ELeave) CMemSpyEngineOutputListItem();
    CleanupStack::PushL( self );
    self->ConstructL( aCopyMe.Caption(), aCopyMe.Value() );
    return self;
    }


CMemSpyEngineOutputListItem* CMemSpyEngineOutputListItem::NewLC( const TDesC& aCaption )
    {
    CMemSpyEngineOutputListItem* self = new(ELeave) CMemSpyEngineOutputListItem();
    CleanupStack::PushL( self );
    self->ConstructL( aCaption, KNullDesC );
    return self;
    }


CMemSpyEngineOutputListItem* CMemSpyEngineOutputListItem::NewLC( const TDesC& aCaption, const TDesC& aValue )
    {
    CMemSpyEngineOutputListItem* self = new(ELeave) CMemSpyEngineOutputListItem();
    CleanupStack::PushL( self );
    self->ConstructL( aCaption, aValue );
    return self;
    }


CMemSpyEngineOutputListItem* CMemSpyEngineOutputListItem::NewLC( const TDesC& aCaption, TUint aValue )
    {
    CMemSpyEngineOutputListItem* ret = CMemSpyEngineOutputListItem::NewLC( aCaption );
    ret->SetUnsignedL( aValue );
    return ret;
    }


CMemSpyEngineOutputListItem* CMemSpyEngineOutputListItem::NewHexLC( const TDesC& aCaption, TUint aValue )
    {
    CMemSpyEngineOutputListItem* ret = CMemSpyEngineOutputListItem::NewLC( aCaption );
    ret->SetHexL( aValue );
    return ret;
    }


CMemSpyEngineOutputListItem* CMemSpyEngineOutputListItem::NewDecimalLC( const TDesC& aCaption, TInt aValue )
    {
    CMemSpyEngineOutputListItem* ret = CMemSpyEngineOutputListItem::NewLC( aCaption );
    ret->SetDecimalL( aValue );
    return ret;
    }


CMemSpyEngineOutputListItem* CMemSpyEngineOutputListItem::NewLongLC( const TDesC& aCaption, const TInt64& aValue )
    {
    CMemSpyEngineOutputListItem* ret = CMemSpyEngineOutputListItem::NewLC( aCaption );
    ret->SetLongL( aValue );
    return ret;
    }


CMemSpyEngineOutputListItem* CMemSpyEngineOutputListItem::NewYesNoLC( const TDesC& aCaption, TBool aYes )
    {
    CMemSpyEngineOutputListItem* ret = CMemSpyEngineOutputListItem::NewLC( aCaption );
    ret->SetYesNoL( aYes );
    return ret;
    }


CMemSpyEngineOutputListItem* CMemSpyEngineOutputListItem::NewTrueFalseLC( const TDesC& aCaption, TBool aTrue )
    {
    CMemSpyEngineOutputListItem* ret = CMemSpyEngineOutputListItem::NewLC( aCaption );
    ret->SetTrueFalseL( aTrue );
    return ret;
    }


CMemSpyEngineOutputListItem* CMemSpyEngineOutputListItem::NewOnOffLC( const TDesC& aCaption, TBool aOn )
    {
    CMemSpyEngineOutputListItem* ret = CMemSpyEngineOutputListItem::NewLC( aCaption );
    ret->SetOnOffL( aOn );
    return ret;
    }


CMemSpyEngineOutputListItem* CMemSpyEngineOutputListItem::NewPercentageLC( const TDesC& aCaption, TInt aOneHundredPercentValue, TInt aValue )
    {
    CMemSpyEngineOutputListItem* ret = CMemSpyEngineOutputListItem::NewLC( aCaption );
    ret->SetPercentageL( aOneHundredPercentValue, aValue );
    return ret;
    }


void CMemSpyEngineOutputListItem::SetValueL( const TDesC& aValue )
    {
    if  ( iValue == NULL )
        {
        iValue = aValue.AllocL();
        }
    else
        {
        if  ( iValue->Des().MaxLength() < aValue.Length() )
            {
            iValue = iValue->ReAllocL( aValue.Length() );
            }
            
        // Now its safe to assign new content
        *iValue = aValue;
        }
        
    UpdateCombinedL();
    }


void CMemSpyEngineOutputListItem::SetUnsignedL( TUint aValue )
    {
    TBuf<KMemSpyNumericFormatBufferSize> val;
    val.Format( KMemSpyNumericUnsignedFormat, aValue );
    SetValueL( val );
    }


void CMemSpyEngineOutputListItem::SetHexL( TUint aValue )
    {
    TBuf<KMemSpyNumericFormatBufferSize> val;
    val.Format( KMemSpyNumericHexFormat, aValue );
    SetValueL( val );
    }


void CMemSpyEngineOutputListItem::SetDecimalL( TInt aValue )
    {
    TBuf<KMemSpyNumericFormatBufferSize> val;
    val.Format( KMemSpyNumericDecFormat, aValue );
    SetValueL( val );
    }


void CMemSpyEngineOutputListItem::SetLongL( const TInt64& aValue )
    {
    TBuf<KMemSpyNumericFormatBufferSize> val;
    val.Format( KMemSpyNumericLongFormat, aValue );
    SetValueL( val );
    }


void CMemSpyEngineOutputListItem::SetYesNoL( TBool aYes )
    {
    if  ( aYes )
        {
        SetValueL( KMemSpyCaptionYes );
        }
    else
        {
        SetValueL( KMemSpyCaptionNo );
        }
    }


void CMemSpyEngineOutputListItem::SetTrueFalseL( TBool aTrue )
    {
    if  ( aTrue )
        {
        SetValueL( KMemSpyCaptionTrue );
        }
    else
        {
        SetValueL( KMemSpyCaptionFalse );
        }
    }


void CMemSpyEngineOutputListItem::SetOnOffL( TBool aOn )
    {
    if  ( aOn )
        {
        SetValueL( KMemSpyCaptionOn );
        }
    else
        {
        SetValueL( KMemSpyCaptionOff );
        }
    }


void CMemSpyEngineOutputListItem::SetPercentageL( TInt aOneHundredPercentValue, TInt aValue )
    {
    const TMemSpyPercentText val( MemSpyEngineUtils::FormatPercentage( TReal( aOneHundredPercentValue ), TReal( aValue ) ) );
    SetValueL( val );
    }


void CMemSpyEngineOutputListItem::UpdateCombinedL()
    {
    const TInt requiredLength = Caption().Length() + Value().Length() + 10;
    //
    if  ( iCombined == NULL )
        {
        iCombined = HBufC::NewL( requiredLength );
        }
    else if ( iCombined->Des().MaxLength() < requiredLength )
        {
        iCombined = iCombined->ReAllocL( requiredLength );
        }
        
    TPtr pCombined( iCombined->Des() );
    pCombined.Zero();
    pCombined.Append( _L("\t") );
    pCombined.Append( Caption() );
    pCombined.Append( _L("\t\t") );
    pCombined.Append( Value() );
    }






