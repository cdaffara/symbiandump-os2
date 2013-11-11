// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : SdpFmtAttributeField.h
// Part of       : Local SDP Codec
// Version       : 1.0
//



#include <s32mem.h>
#include "sdpfmtattributefield.h"
#include "sdpattributefield.h"
#include "sdprtpmapvalue.h"
#include "sdputil.h"
#include "sdpcodecconstants.h"
#include "sdpcodecstringpool.h"
#include "sdpcodecerr.h"
#include "sdpcodec.pan"
#include "_sdpdefs.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::CSdpFmtAttributeField
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSdpFmtAttributeField::CSdpFmtAttributeField()
: iFormat ( KNullDesC8() ), iValuePart( KNullDesC8() )
    {
	}

// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdpFmtAttributeField::ConstructL(   
    const TDesC8& aText )
    {
    iPool = SdpCodecStringPool::StringPoolL();

    iAttributeField = CSdpAttributeField::DecodeL( aText );
    FormatValueParamsL( iAttributeField );   
    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdpFmtAttributeField::ConstructL( RStringF aAttribute,
                                        const TDesC8& aFormat,
                                        const TDesC8& aValue )
    {
    iPool = SdpCodecStringPool::StringPoolL();

    TInt length = aFormat.Length() + aValue.Length() + KSPStr().Length();        
    HBufC8* text = HBufC8::NewLC( length );
    text->Des().Copy( aFormat );
    if ( aValue.Length() > 0 )
        {
        text->Des().Append( KSPStr );
        text->Des().Append( aValue );
        }

    iAttributeField = CSdpAttributeField::NewL( aAttribute, *text );
    CleanupStack::PopAndDestroy(); //text
    
    FormatValueParamsL( iAttributeField );
    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::DecodeL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpFmtAttributeField* CSdpFmtAttributeField::DecodeL( 
    const TDesC8& aText )
    {
    CSdpFmtAttributeField* field = 
        CSdpFmtAttributeField::DecodeLC( aText );
    CleanupStack::Pop();    // field
    return field;    
    }

// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::DecodeLC
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpFmtAttributeField* CSdpFmtAttributeField::DecodeLC(    
    const TDesC8& aText )
    {
    CSdpFmtAttributeField* field = new ( ELeave ) CSdpFmtAttributeField;
    CleanupStack::PushL( field );
    field->ConstructL( aText );
    return field;
    }

// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpFmtAttributeField* CSdpFmtAttributeField::NewL(
    RStringF aAttribute,
    const TDesC8& aFormat,
    const TDesC8& aValue )
    {
    CSdpFmtAttributeField* field =
        CSdpFmtAttributeField::NewLC( aAttribute, aFormat, aValue );
    CleanupStack::Pop();    // field
    return field;
    }

// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::NewLC
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpFmtAttributeField* CSdpFmtAttributeField::NewLC(
    RStringF aAttribute,
    const TDesC8& aFormat,
    const TDesC8& aValue )
    {
    CSdpFmtAttributeField* field = new ( ELeave ) CSdpFmtAttributeField;
    CleanupStack::PushL( field );
    field->ConstructL( aAttribute, aFormat, aValue );
    return field;
    }

// Destructor
EXPORT_C CSdpFmtAttributeField::~CSdpFmtAttributeField()
    {
    delete iAttributeField;
    }

// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::EncodeL
// Encodes the string to a proper message format
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpFmtAttributeField::EncodeL( RWriteStream& aStream ) const
    {
    __TEST_INVARIANT;
    iAttributeField->EncodeL( aStream );
    }

// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::CloneL
// Creates an exact copy of the object
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpFmtAttributeField* CSdpFmtAttributeField::CloneL() const
    {
    __TEST_INVARIANT;
    
    CSdpFmtAttributeField* obj =
        CSdpFmtAttributeField::NewL( iAttributeField->Attribute(), 
                                     Format(), Value() );                               

    __ASSERT_DEBUG( *obj == *this, 
                    User::Panic( KSdpCodecPanicCat, KSdpCodecPanicInternal ) );

    return obj;
    }

// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::operator==
// Creates an exact copy of the object
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSdpFmtAttributeField::operator==(
    const CSdpFmtAttributeField& aObj ) const
    {
    __TEST_INVARIANT;
    
    return ( *iAttributeField == *(aObj.AttributeField()) 
        && iFormat.Compare(aObj.Format()) == 0
        && iValuePart.Compare(aObj.Value()) == 0);
    }

// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::Attribute
// Returns attribute field
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSdpFmtAttributeField::Attribute() const
    {
    __TEST_INVARIANT;
    return iAttributeField->Attribute();
    }

// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::Format
// Returns Format
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSdpFmtAttributeField::Format() const
    {   
    __TEST_INVARIANT;

    return iFormat;
    }

// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::Value
// Returns Format value
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSdpFmtAttributeField::Value() const
    {
    __TEST_INVARIANT;
    return iValuePart;
    }

// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::SetL
// Sets new format attribute
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpFmtAttributeField::SetL( RStringF aAttribute, 
                                        const TDesC8& aFormat, 
                                        const TDesC8& aValue )
    {
    __TEST_INVARIANT;

    TInt length = aFormat.Length() + aValue.Length() + KSPStr().Length();        
    HBufC8* txt = HBufC8::NewLC( length );
    txt->Des().Copy( aFormat );
    if ( aValue.Length() > 0 )
        {
        txt->Des().Append( KSPStr );
        txt->Des().Append( aValue );
        }

    CSdpAttributeField* attrFld = CSdpAttributeField::NewL(aAttribute, *txt);
    CleanupStack::PopAndDestroy(); //txt
    
    delete iAttributeField;
    iAttributeField = attrFld;    

    FormatValueParamsL( iAttributeField );

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::ExternalizeL
// Externalizes object to stream
// -----------------------------------------------------------------------------
//
void CSdpFmtAttributeField::ExternalizeL( RWriteStream& aStream ) const
    {
    __TEST_INVARIANT;

    iAttributeField->ExternalizeL( aStream );
    }

// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::InternalizeL
// Internalizes object from stream
// -----------------------------------------------------------------------------
//
CSdpFmtAttributeField* CSdpFmtAttributeField::InternalizeL( 
    RReadStream& aStream )
    {
    CSdpFmtAttributeField* obj = new ( ELeave ) CSdpFmtAttributeField();
    CleanupStack::PushL( obj );
    obj->DoInternalizeL( aStream );
    CleanupStack::Pop();

    return obj;
    }

// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::DoInternalizeL
// Does the "2nd phase construction" of internalization
// -----------------------------------------------------------------------------
//
void CSdpFmtAttributeField::DoInternalizeL( RReadStream& aStream )
    {
    iPool = SdpCodecStringPool::StringPoolL();
    iAttributeField = CSdpAttributeField::InternalizeL( aStream );
    FormatValueParamsL( iAttributeField );
    }

// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::AttributeField
// Returns attribute field
// -----------------------------------------------------------------------------
//
const CSdpAttributeField* CSdpFmtAttributeField::AttributeField() const
    {
    __TEST_INVARIANT;
    return iAttributeField;
    }

// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::FormatValueParamsL
// Formats value parameters
// -----------------------------------------------------------------------------
//
void CSdpFmtAttributeField::FormatValueParamsL( CSdpAttributeField* aField )
    {
    __ASSERT_ALWAYS (aField->Attribute() == 
                     iPool.StringF( SdpCodecStringConstants::EAttributeFmtp, 
                                SdpCodecStringConstants::Table ) 
                  || aField->Attribute() ==
                     iPool.StringF( SdpCodecStringConstants::EAttributeRtpmap,
                                SdpCodecStringConstants::Table ),
                User::Leave( KErrSdpCodecMediaAttributeField ));
    DecodeFormatL( aField->Value() );
    if ( iValuePart.Length() == 0 &&
         aField->Attribute() ==
         iPool.StringF( SdpCodecStringConstants::EAttributeRtpmap,
                        SdpCodecStringConstants::Table ))
        {
        User::Leave( KErrSdpCodecMediaAttributeField );
        }
    }


// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::DecodeFormatL
// Decodes format parameter from string
// -----------------------------------------------------------------------------
//
void CSdpFmtAttributeField::DecodeFormatL( const TDesC8& aText )
    {
    __ASSERT_ALWAYS( aText.Length() > 0, 
                     User::Leave( KErrSdpCodecAttributeField ) );

    TInt pos = aText.Locate( KSPChar );

    if ( pos == 0 )
        {
        User::Leave( KErrSdpCodecAttributeField );
        }
    
    if ( pos > 0 )
        {
        iFormat.Set( aText.Left( pos ) );

        TPtrC8 formatParameters( aText.Right( aText.Length() - pos - 1 ) );
        
        if ( formatParameters.Length() > 0 )
            {
            iValuePart.Set( formatParameters );
            }
        else
            {
            iValuePart.Set( KNullDesC8 );
            }
        }
    else
        {
        // No format parameters
        iFormat.Set( aText );
        iValuePart.Set( KNullDesC8 );
        }
    }

// For DEBUG builds
// -----------------------------------------------------------------------------
// CSdpFmtAttributeField::__DbgTestInvariant
// Test invariant
// -----------------------------------------------------------------------------
//	
void CSdpFmtAttributeField::__DbgTestInvariant() const
	{
    TBool invariant = iAttributeField != NULL;

    if ( !invariant )
        {
        User::Invariant();
        }
	}
