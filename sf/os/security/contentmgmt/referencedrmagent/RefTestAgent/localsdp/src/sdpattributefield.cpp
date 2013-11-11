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
// Name          : SdpAttributeField.h
// Part of       : Local SDP Codec
// Version       : 1.0
//



#include <s32strm.h>
#include <uri8.h>
#include "sdpattributefield.h"
#include "sdpattributefieldptrs.h"
#include "sdpfmtattributefield.h"
#include "sdputil.h"
#include "sdpcodec.pan"
#include "sdpcodecstringconstants.h"
#include "sdpcodecconstants.h"
#include "sdpcodecstringpool.h"
#include "sdprtpmapvalue.h"
#include "sdpcodecerr.h"
#include "_sdpdefs.h"

// CONSTANTS
const TInt KHeaderIndex = 0;
const TInt KAttributeIndex = 1;
const TInt KMinAttributeNum = 2;
const TInt KMaxSubtagLength = 8;
const TInt KMaxPrimarySubtagLength = 8;

// Valid property attribute strings
const TInt KPropertyStringNum = 4;
const SdpCodecStringConstants::TStrings 
    KValidPropertyStrings[KPropertyStringNum] =
    {
    SdpCodecStringConstants::EAttributeRecvonly,
    SdpCodecStringConstants::EAttributeSendrecv,
    SdpCodecStringConstants::EAttributeSendonly,
    SdpCodecStringConstants::EAttributeInactive
    };

// Valid value attribute strings
const TInt KValueStringNum = 22;
const SdpCodecStringConstants::TStrings KValidValueStrings[KValueStringNum] =
    {
    SdpCodecStringConstants::EAttributeCat,
    SdpCodecStringConstants::EAttributeKeywds,
    SdpCodecStringConstants::EAttributeTool,
    SdpCodecStringConstants::EAttributePtime,
    SdpCodecStringConstants::EAttributeMaxptime,
    SdpCodecStringConstants::EAttributeRtpmap,
    SdpCodecStringConstants::EAttributeOrient,
    SdpCodecStringConstants::EAttributeType,
    SdpCodecStringConstants::EAttributeCharset,
    SdpCodecStringConstants::EAttributeSdplang,
    SdpCodecStringConstants::EAttributeLang,
    SdpCodecStringConstants::EAttributeFramerate,
    SdpCodecStringConstants::EAttributeQuality,    
    SdpCodecStringConstants::EAttributeFmtp,
    SdpCodecStringConstants::EAttributeControl,
    SdpCodecStringConstants::EAttributeEtag,
    SdpCodecStringConstants::EAttributeMid,
    SdpCodecStringConstants::EAttributeGroup,
    SdpCodecStringConstants::EAttributeCurr,
    SdpCodecStringConstants::EAttributeDes,
    SdpCodecStringConstants::EAttributeConf,
    SdpCodecStringConstants::EAttributeRange
    };

// Valid orient value strings
const TInt KValidOrientStringNum = 3;
static const TText8* const KOrientStrings[KValidOrientStringNum] = 
    {
    _S8("portrait\0"),
    _S8("landscape\0"),
    _S8("seascape\0")
    };

const TInt KValidGroupSemanticsNum = 3;
static const TText8* const KGroupSemantics[KValidGroupSemanticsNum] =
    {
    _S8("LS"),
    _S8("FID"),
    _S8("SRF")
    };

const TInt KValidStatusTypeNum = 3;
static const TText8* const KStatusTypes[KValidStatusTypeNum] =
    {
    _S8("e2e"),
    _S8("local"),
    _S8("remote")
    };

const TInt KValidDirectionTagsNum = 4;
static const TText8* const KDirectionTags[KValidDirectionTagsNum] =
    {
    _S8("none"),
    _S8("send"),
    _S8("recv"),
    _S8("sendrecv")
    };

const TInt KValidStrengthTagsNum = 5;
static const TText8* const KStrengthTags[KValidStrengthTagsNum] =
    {
    _S8("mandatory"),
    _S8("optional"),
    _S8("none"),
    _S8("failure"),
    _S8("unknown")
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSdpAttributeField::CSdpAttributeField
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSdpAttributeField::CSdpAttributeField()
	{
	}

// -----------------------------------------------------------------------------
// CSdpAttributeField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdpAttributeField::ConstructL( const TDesC8& aText )
	{
    // aText should be in format
    // a=<attribute> or a=<attribute>:<value> or 
    // a=<attribute>:<value> <value specific attributes>

    __ASSERT_ALWAYS(aText.Length() > 0, User::Leave(KErrSdpCodecAttributeField));

    iPool = SdpCodecStringPool::StringPoolL();
    CreateAttributeFieldPtrsL();
    
    RArray<TPtrC8> array;
    
    TInt length = aText.Length();
    if ( aText[aText.Length() - 1] == KLFChar )
        {
        if ( length > 1 && aText[aText.Length() - 2] == KCRChar )
            {
            length--;
            }
        length--;
        }  
    TPtrC8 restValue( aText.Left( length ));
    
    __ASSERT_ALWAYS(SdpUtil::IsByteString(restValue) 
            || SdpUtil::IsToken(restValue)
            || SdpUtil::IsValidChars(KEqualStr, restValue),
            User::Leave(KErrSdpCodecAttributeField));
    
    array = SdpUtil::GetElementsFromLineL( aText, KErrSdpCodecAttributeField );
    CleanupClosePushL( array );

    RStringF origHeader =  iPool.StringF( SdpCodecStringConstants::EAttribute, 
                                          SdpCodecStringConstants::Table );
    
    // We need to have at least two tokens (and this should be always the case,
    // unless TSdpUtil::GetElementsFromLineL() gets modified)
    __ASSERT_ALWAYS( origHeader.DesC().Compare( array[KHeaderIndex] ) == 0
                    && array.Count() >= KMinAttributeNum,
        User::Leave( KErrSdpCodecAttributeField ) );
        
    if ( array[KAttributeIndex].Locate( KColonChar ) == KErrNotFound )
        {
        // For property attributes exactly two attributes are needed
        __ASSERT_ALWAYS( array.Count() == KMinAttributeNum,
            User::Leave( KErrSdpCodecAttributeField ));

        FormatProperyAttributeL( array[KAttributeIndex] );  
        }
    else
        {    
        HBufC8* attribute = NULL;
        HBufC8* value = NULL;             
        CreateAttributeAndValueStringsLC( array, attribute, value );
        FormatValueAttributeL( *attribute, *value );
        CleanupStack::PopAndDestroy( 2 );   // attribute, value
        }

    CleanupStack::PopAndDestroy();  // array

    __TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CSdpAttributeField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdpAttributeField::ConstructL( RStringF aAttribute, 
                                    const TDesC8& aValue )
	{	
    iPool = SdpCodecStringPool::StringPoolL();
    CreateAttributeFieldPtrsL();

    TBool property( EFalse );
    for ( TInt i( 0 ); i < KPropertyStringNum && !property; i++ )
        {
        if ( aAttribute == 
             iPool.StringF( KValidPropertyStrings[i], 
                            SdpCodecStringConstants::Table ) )
            {
            property = ETrue;
            }
        }

    if ( property || aValue.Length() == 0)
        {
        // For property attributes, no value is valid value
        __ASSERT_ALWAYS( aValue.Length() == 0, 
            User::Leave( KErrSdpCodecAttributeField ));
                   
        FormatProperyAttributeL( aAttribute.DesC() );  
        }
    else
        {   
        FormatValueAttributeL( aAttribute.DesC(), aValue );        
        }        

    __TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CSdpAttributeField::DecodeL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpAttributeField* CSdpAttributeField::DecodeL( const TDesC8& aText )
	{
	CSdpAttributeField* obj = DecodeLC( aText );
	CleanupStack::Pop();
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpAttributeField::DecodeLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpAttributeField* CSdpAttributeField::DecodeLC( 
    const TDesC8& aText )
	{
	CSdpAttributeField* obj = new ( ELeave ) CSdpAttributeField;
	CleanupStack::PushL( obj );
	obj->ConstructL( aText );
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpAttributeField::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpAttributeField* CSdpAttributeField::NewL( RStringF aAttribute, 
                                                    const TDesC8& aValue )
	{
	CSdpAttributeField* obj = NewLC( aAttribute, aValue );
	CleanupStack::Pop();
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpAttributeField::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpAttributeField* CSdpAttributeField::NewLC( RStringF aAttribute,
                                                        const TDesC8& aValue )
	{
	CSdpAttributeField* obj = new ( ELeave ) CSdpAttributeField;
	CleanupStack::PushL(obj);
	obj->ConstructL( aAttribute, aValue );
	return obj;
	}

// Destructor
EXPORT_C CSdpAttributeField::~CSdpAttributeField()
	{    
    iAttribute.Close();
    CSdpAttributeFieldPtrs* tmp = 
        reinterpret_cast< CSdpAttributeFieldPtrs* >( iValue ); 
	delete tmp;
	}

// -----------------------------------------------------------------------------
// CSdpAttributeField::EncodeL
// Encodes a string into valid output string format
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpAttributeField::EncodeL( RWriteStream& aStream ) const
	{
	__TEST_INVARIANT;
    
    RStringF header = iPool.StringF( SdpCodecStringConstants::EAttribute, 
                                     SdpCodecStringConstants::Table );
    aStream.WriteL( header.DesC() );
    aStream.WriteL( iAttribute.DesC() );
    TPtrC8 value( AttributeFieldPtrs().Value() );
    if ( value.Length() > 0 )
        {
        aStream.WriteL( KColonStr );
        aStream.WriteL( value );
        }
    aStream.WriteL( KCRLFStr );
	}

// -----------------------------------------------------------------------------
// CSdpAttributeField::CloneL
// Clones an object and returns the new attribute field
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpAttributeField * CSdpAttributeField::CloneL() const
	{
	__TEST_INVARIANT;

    CSdpAttributeField* obj = 
        CSdpAttributeField::NewL( iAttribute, AttributeFieldPtrs().Value() );

	__ASSERT_DEBUG( *this == *obj, 
                    User::Panic(KSdpCodecPanicCat, KSdpCodecPanicInternal) );
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpAttributeField::operator==
// Checks if the two attribute fields are equal
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSdpAttributeField::operator== (
    const CSdpAttributeField& aObj ) const
	{
	__TEST_INVARIANT;

    return ( iAttribute == aObj.Attribute() && 
             AttributeFieldPtrs().Value().CompareF( aObj.Value() ) == 0 );
	}

// -----------------------------------------------------------------------------
// CSdpAttributeField::Attribute
// Returns attribute part of attribute field
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSdpAttributeField::Attribute() const
	{
    __TEST_INVARIANT;
    return iAttribute;
	}

// -----------------------------------------------------------------------------
// CSdpAttributeField::Value
// Returns value part of attribute field
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSdpAttributeField::Value() const
	{
    __TEST_INVARIANT;
    
    return AttributeFieldPtrs().Value();
	}

// -----------------------------------------------------------------------------
// CSdpAttributeField::SetL
// Sets new attribute field values
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpAttributeField::SetL( RStringF aAttribute, 
                                        const TDesC8& aValue )
	{
    __TEST_INVARIANT;

    ConstructL( aAttribute, aValue ); 

    __TEST_INVARIANT;  
	}

// -----------------------------------------------------------------------------
// CSdpAttributeField::AssignTo
// Assigns attribute to a format attribute
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpAttributeField::AssignTo(
    const CSdpFmtAttributeField& aFmtAttribute)
	{
	AttributeFieldPtrs().SetFmtAttribute(&aFmtAttribute);
	}

// -----------------------------------------------------------------------------
// CSdpAttributeField::BelongsTo
// Check if the attribute field belongs to the format attribute
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSdpAttributeField::BelongsTo(
    const CSdpFmtAttributeField& aFmtAttribute) const
	{
	return ( AttributeFieldPtrs().FmtAttribute() == &aFmtAttribute );
	}

// -----------------------------------------------------------------------------
// CSdpAttributeField::ExternalizeL
// Externalizes the object to stream
// -----------------------------------------------------------------------------
//
void CSdpAttributeField::ExternalizeL( RWriteStream& aStream ) const
	{
    // Attribute
    aStream.WriteUint32L( iAttribute.DesC().Length() );
    aStream.WriteL( iAttribute.DesC() );
    // Value
    TPtrC8 value( AttributeFieldPtrs().Value() );
    aStream.WriteUint32L( value.Length() );
    if ( value.Length() > 0 )
        {
        aStream.WriteL( value, value.Length() );
        }
	}

// -----------------------------------------------------------------------------
// CSdpAttributeField::InternalizeL
// Internalizes the object from stream
// -----------------------------------------------------------------------------
//
CSdpAttributeField* CSdpAttributeField::InternalizeL( RReadStream& aStream )
	{
    RStringPool pool = SdpCodecStringPool::StringPoolL();

	// Attribute
    HBufC8* tmpBuf2;
    TUint32 length = aStream.ReadUint32L();
    HBufC8* tmpBuf = HBufC8::NewLC( length );
    TPtr8 ptr( tmpBuf->Des() );
    aStream.ReadL( ptr, length ); 
    RStringF attribute = pool.OpenFStringL( ptr );
    CleanupClosePushL( attribute );

    length = aStream.ReadUint32L();
    if ( length > 0 )
        {
        tmpBuf2 = HBufC8::NewLC( length );
        TPtr8 ptr2( tmpBuf2->Des() );
        aStream.ReadL( ptr2, length );
        }
    else
        {
        tmpBuf2 = KNullDesC8().AllocLC();
        }

    CSdpAttributeField* obj = CSdpAttributeField::NewL( attribute, *tmpBuf2 );
    CleanupStack::PopAndDestroy( 3 );  // attribute, tmpBuf, tmpBuf2

    return obj;
	}

// -----------------------------------------------------------------------------
// CSdpAttributeField::FormatPropertyAttributeL
// Formats and initializes class to be a property attribute
// -----------------------------------------------------------------------------
//
void CSdpAttributeField::FormatProperyAttributeL( const TDesC8& aAttribute )
    {
    __ASSERT_ALWAYS( SdpUtil::IsToken( aAttribute ), 
        User::Leave( KErrSdpCodecAttributeField ));

    TBool found( EFalse );
    for ( TInt i( 0 ); i < KPropertyStringNum && !found; i++ )
        {
        RStringF attrString = 
            iPool.StringF( KValidPropertyStrings[i], 
                        SdpCodecStringConstants::Table );

        if ( attrString.DesC().CompareF( aAttribute ) == 0 )
            {
            AttributeFieldPtrs().SetValueL( KNullDesC8 );
            
            iAttribute.Close();           
            iAttribute = attrString.Copy();            
            iPropertyAttribute = ETrue;            

            found = ETrue;
            }                                       
        }

    if ( !found )
        {
        HBufC8* tempValue = KNullDesC8().AllocLC();
        SetNotPredefinedAttributeL(aAttribute, *tempValue);
        CleanupStack::PopAndDestroy(); // tempValue
        }
    }

// -----------------------------------------------------------------------------
// CSdpAttributeField::FormatValueAttributeL
// Formats and initializes class to be a value attribute
// -----------------------------------------------------------------------------
//
void CSdpAttributeField::FormatValueAttributeL( const TDesC8& aAttribute,
                                                const TDesC8& aValue )
    {
    TBool found( EFalse ); 

    __ASSERT_ALWAYS( SdpUtil::IsToken( aAttribute ) 
                && SdpUtil::IsByteString( aValue ),
            User::Leave( KErrSdpCodecAttributeField ) );

    TInt i = 0;
    for( i = 0; i < KPropertyStringNum && !found; i++)
        {
        RStringF attrString = 
            iPool.StringF( KValidPropertyStrings[i], 
                        SdpCodecStringConstants::Table );

        if ( attrString.DesC().CompareF( aAttribute ) == 0 )
            {
            //found = ETrue;
            User::Leave(KErrSdpCodecAttributeField);
            }
        }
    
    for ( i = 0; i < KValueStringNum && !found; i++ )
        {
        RStringF attrString = 
            iPool.StringF( KValidValueStrings[i], 
                        SdpCodecStringConstants::Table );

        if ( attrString.DesC().CompareF( aAttribute ) == 0 )
            {
            VerifyValueAttributeL( KValidValueStrings[i], aValue );
            iAttribute.Close();                          
            iAttribute = attrString.Copy();            
            found = ETrue;
            }
        }

    if ( !found )
        {
        SetNotPredefinedAttributeL(aAttribute, aValue);
        }
    }

// -----------------------------------------------------------------------------
// CSdpAttributeField::VerifyValueAttributeL
// Verifies that value given to the attribute is valid
// -----------------------------------------------------------------------------
//
void CSdpAttributeField::VerifyValueAttributeL( 
    SdpCodecStringConstants::TStrings aString,
    const TDesC8& aValue )
    {
    TUint str( aString );
    switch ( str )
        {
        case SdpCodecStringConstants::EAttributeCat:
        case SdpCodecStringConstants::EAttributeType:    
        case SdpCodecStringConstants::EAttributeCharset: 
        case SdpCodecStringConstants::EAttributeEtag:
            __ASSERT_ALWAYS( aValue.Length() > 0 
                && aValue.Locate( KSPChar ) == KErrNotFound,
            User::Leave( KErrSdpCodecAttributeField ) );
            break;

        case SdpCodecStringConstants::EAttributeKeywds:
        case SdpCodecStringConstants::EAttributeTool:      
            __ASSERT_ALWAYS( aValue.Length() > 0, 
                 User::Leave( KErrSdpCodecAttributeField ) );
            break;

        case SdpCodecStringConstants::EAttributePtime:
        case SdpCodecStringConstants::EAttributeMaxptime:
        case SdpCodecStringConstants::EAttributeQuality:
            __ASSERT_ALWAYS( SdpUtil::IsDigit( aValue ),
                User::Leave( KErrSdpCodecAttributeField ) );
            break;

        case SdpCodecStringConstants::EAttributeOrient:
            CheckValidOrientL( aValue );
            break;
      
        case SdpCodecStringConstants::EAttributeSdplang:
        case SdpCodecStringConstants::EAttributeLang:
            CheckValidLangStrL( aValue );
            break;

        case SdpCodecStringConstants::EAttributeFramerate:
            CheckValidFrameRateL( aValue );
            break;

        case SdpCodecStringConstants::EAttributeControl:
            CheckValidControlL( aValue );
            break;

        case SdpCodecStringConstants::EAttributeMid:
            if (SdpUtil::IsToken( aValue ) == EFalse)
                {
                User::Leave( KErrSdpCodecAttributeField );
                }
            break;

        case SdpCodecStringConstants::EAttributeGroup:
            CheckValidGroupL( aValue );
            break;

        case SdpCodecStringConstants::EAttributeCurr:
            CheckValidCurrAndConfL( aValue );
            break;
        
        case SdpCodecStringConstants::EAttributeDes:
            CheckValidDesL( aValue );
            break;

        case SdpCodecStringConstants::EAttributeConf:
            CheckValidCurrAndConfL( aValue );
            break;

        case SdpCodecStringConstants::EAttributeRange:
            CheckValidRangeL( aValue );
            break;

        case SdpCodecStringConstants::EAttributeRtpmap:
            CheckValidRtpmapL( aValue );
            break;

        case SdpCodecStringConstants::EAttributeFmtp:
            CheckValidFmtpL( aValue );
            break;

        default:
            User::Leave( KErrSdpCodecAttributeField );
            break;
        }

    AttributeFieldPtrs().SetValueL( aValue );
    iPropertyAttribute = EFalse;
    }

// -----------------------------------------------------------------------------
// CSdpAttributeField::CreateAttributeAndValueStringsLC
// Forms attribute and value strings
// -----------------------------------------------------------------------------
//
void CSdpAttributeField::CreateAttributeAndValueStringsLC( 
    RArray<TPtrC8>& aArray,
    HBufC8*& aAttribute,
    HBufC8*& aValue )
    {
    TInt pos = aArray[KAttributeIndex].Locate( KColonChar );
    __ASSERT_ALWAYS( pos > 0, User::Leave( KErrSdpCodecAttributeField ) );

    aAttribute = HBufC8::NewLC( pos );
    aAttribute->Des().Copy( aArray[KAttributeIndex].Left( pos ) );

    // Find out the total length of value string
    TPtrC8 initialValue( aArray[KAttributeIndex].Right( 
                            aArray[KAttributeIndex].Length() - pos - 1 ) );

    TInt valueLength( initialValue.Length() );    
    for ( TInt i( KAttributeIndex + 1 ); i < aArray.Count(); i++ )
        {
        valueLength += KSPStr().Length();
        valueLength += aArray[i].Length();
        }

    // Copy the value string to the value buffer
    aValue = HBufC8::NewLC( valueLength );
    aValue->Des().Append( initialValue );
    for ( TInt j( KAttributeIndex + 1 ); j < aArray.Count(); j++ )
        {
        aValue->Des().Append( KSPStr );
        aValue->Des().Append( aArray[j] ) ;
        }
    }

// -----------------------------------------------------------------------------
// CSdpAttributeField::CheckValidOrientL
// Checks if value is valid orient value
// -----------------------------------------------------------------------------
//
void CSdpAttributeField::CheckValidOrientL( const TDesC8& aValue )
    {    
    TBool found( EFalse );
    for ( TInt i( 0 ); i < KValidOrientStringNum && !found; i++ )
        {
        TPtrC8 strPtr( KOrientStrings[i] );
        if ( strPtr.CompareF( aValue ) == 0 )
            {
            found = ETrue;
            } 
        }

    __ASSERT_ALWAYS(found, User::Leave( KErrSdpCodecAttributeField ) );
    }

// -----------------------------------------------------------------------------
// CSdpAttributeField::CheckValidLangStrL
// Checks if value is valid language tag
// -----------------------------------------------------------------------------
//
void CSdpAttributeField::CheckValidLangStrL( const TDesC8& aValue )
    {
    // Language-Tag = Primary-subtag *( "-" Subtag )
    // Primary-subtag = 1*8ALPHA
    // Subtag = 1*8(ALPHA / DIGIT)
    TInt primarySubtagPos = aValue.Locate( KHyphenChar );

    if ( primarySubtagPos > KMaxPrimarySubtagLength )
        {
        User::Leave( KErrSdpCodecAttributeField );
        }    

    TInt subtagPos( primarySubtagPos );    
    TPtrC8 ptr( aValue.Right( aValue.Length() - subtagPos - 1 ) );
    // Check all subtags
    while ( subtagPos != ptr.Length() )
        {
        subtagPos = ptr.Locate( KHyphenChar );
            
        if ( subtagPos == KErrNotFound )
            {
            subtagPos = ptr.Length();
            }
      
        // Check length
        __ASSERT_ALWAYS( subtagPos > 0 && subtagPos < KMaxSubtagLength,
            User::Leave( KErrSdpCodecAttributeField ) );
   
        if ( subtagPos != ptr.Length() )
            {
            ptr.Set( aValue.Right( ptr.Length() - subtagPos - 1 ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// CSdpAttributeField::CheckValidFrameRateL
// Checks if value is valid framerate value
// -----------------------------------------------------------------------------
//
void CSdpAttributeField::CheckValidFrameRateL( const TDesC8& aValue )
    {
    TInt dotFound = 0;
    // There can be one dot and digits
    for ( TInt i( 0 ); i < aValue.Length(); i++ )
        {
        __ASSERT_ALWAYS((aValue[i] == KDotChar || TChar( aValue[i] ).IsDigit())
            && dotFound <= 1,
            User::Leave( KErrSdpCodecAttributeField ));
                        
        if ( aValue[i] == KDotChar )
            {
            dotFound++;
            }
        }
    }

// -----------------------------------------------------------------------------
// CSdpAttributeField::CheckValidControlL
// Checks if value is valid uri for group attribute
// -----------------------------------------------------------------------------
//      
void CSdpAttributeField::CheckValidControlL( const TDesC8& aValue )
    {
    TUriParser8 parser;
	User::LeaveIfError(parser.Parse(aValue));
	}

// -----------------------------------------------------------------------------
// CSdpAttributeField::CheckValidGroupL
// Checks if value is valid value for group attribute
// -----------------------------------------------------------------------------
//      
void CSdpAttributeField::CheckValidGroupL( const TDesC8& aValue )
    {
   ValidValueInListL(KValidGroupSemanticsNum, KGroupSemantics, aValue);
    TInt pos = aValue.Locate( KSPChar );
    TPtrC8 ptr(aValue);

    while ( pos != KErrNotFound)
        {
        ptr.Set( ptr.Right( ptr.Length() - pos - 1 ) );
        pos = ptr.Locate( KSPChar );           
        }    
    }

// -----------------------------------------------------------------------------
// CSdpAttributeField::CheckValidDesL
// Checks if value is valid value for des attribute
// -----------------------------------------------------------------------------
//      
void CSdpAttributeField::CheckValidDesL( const TDesC8& aValue )
    {
    TInt pos = aValue.Locate( KSPChar );
    __ASSERT_ALWAYS(pos > 0, User::Leave( KErrSdpCodecAttributeField ));
    TPtrC8 ptr(aValue.Left(pos));
    __ASSERT_ALWAYS( SdpUtil::IsToken(ptr), 
        User::Leave( KErrSdpCodecAttributeField ) );
        
    ptr.Set(aValue.Right( aValue.Length() - pos - 1 ));
    pos = ptr.Locate( KSPChar );
    __ASSERT_ALWAYS(pos > 0, User::Leave( KErrSdpCodecAttributeField ));
    ValidValueInListL(KValidStrengthTagsNum, KStrengthTags, ptr.Left( pos ));

    ptr.Set(ptr.Right( ptr.Length() - pos - 1 ));
    pos = ptr.Locate( KSPChar );
    __ASSERT_ALWAYS(pos > 0, User::Leave( KErrSdpCodecAttributeField ));
    ValidValueInListL(KValidStatusTypeNum, KStatusTypes, ptr.Left( pos ));
    
    ptr.Set(ptr.Right( ptr.Length() - pos - 1 ));
    __ASSERT_ALWAYS(ptr.Locate( KSPChar ) == KErrNotFound, 
                    User::Leave( KErrSdpCodecAttributeField ));
    ValidValueInListL(KValidDirectionTagsNum, KDirectionTags, ptr);
    }

// -----------------------------------------------------------------------------
// CSdpAttributeField::CheckValidCurrAndConfL
// Checks if value is valid value for curr and conf attribute
// -----------------------------------------------------------------------------
//      
void CSdpAttributeField::CheckValidCurrAndConfL( const TDesC8& aValue )
    {
    TInt pos = aValue.Locate( KSPChar );
    __ASSERT_ALWAYS(pos > 0, User::Leave( KErrSdpCodecAttributeField ));
    TPtrC8 ptr(aValue.Left(pos));
    __ASSERT_ALWAYS( SdpUtil::IsToken(ptr), 
        User::Leave( KErrSdpCodecAttributeField ) );
    
    ptr.Set(aValue.Right( aValue.Length() - pos - 1 ));
    pos = ptr.Locate( KSPChar );
    __ASSERT_ALWAYS(pos > 0, User::Leave( KErrSdpCodecAttributeField ));
    ValidValueInListL(KValidStatusTypeNum, KStatusTypes, ptr.Left( pos ));

    ptr.Set(ptr.Right( ptr.Length() - pos - 1 ));
    __ASSERT_ALWAYS(ptr.Locate( KSPChar ) == KErrNotFound, 
                    User::Leave( KErrSdpCodecAttributeField ));
    ValidValueInListL(KValidDirectionTagsNum, KDirectionTags, ptr);
    }

// -----------------------------------------------------------------------------
// CSdpAttributeField::ValidValueInListL
// Checks if value is included in predefined values list
// -----------------------------------------------------------------------------
//      
void CSdpAttributeField::ValidValueInListL( TInt aItemsCount, 
                                            const TText8* const aValuesList[],
                                            const TDesC8& aValue )
    {
    TBool found( EFalse );
    TInt pos = 0;
    for ( TInt i( 0 ); i < aItemsCount && !found; i++ )
        {
        TPtrC8 strPtr( aValuesList[i] );
        pos = aValue.Find(strPtr); 
        if ( pos != KErrNotFound && pos >= 0 )
            {
            found = ETrue;
            } 
        }

    __ASSERT_ALWAYS(found, User::Leave( KErrSdpCodecAttributeField ));
    }

// -----------------------------------------------------------------------------
// CSdpAttributeField::CheckValidCRangefL
// Checks if precondition type value is valid value
// -----------------------------------------------------------------------------
//      
void CSdpAttributeField::CheckValidRangeL( const TDesC8& aValue )
    {
    TInt found = aValue.Find(KEqualStr);
    __ASSERT_ALWAYS(found != KErrNotFound && found > 0, User::Leave( KErrSdpCodecAttributeField ));
    }

// -----------------------------------------------------------------------------
// CSdpAttributeField::CheckValidRtpmapL
// Checks if value is valid rtpmap value
// -----------------------------------------------------------------------------
//
void CSdpAttributeField::CheckValidRtpmapL( const TDesC8& aValue )
    {
    // If this doesn't leave, then the value is OK
    TSdpRtpmapValue::DecodeL( aValue );
    }

// -----------------------------------------------------------------------------
// CSdpAttributeField::CheckValidFmtpL
// Checks if value is valid format value
// -----------------------------------------------------------------------------
//
void CSdpAttributeField::CheckValidFmtpL( const TDesC8& aValue )
    {
    __ASSERT_ALWAYS(SdpUtil::IsByteString(aValue) && aValue.Length() > 0, 
                User::Leave( KErrSdpCodecMediaAttributeField ));
    }

// -----------------------------------------------------------------------------
// CSdpAttributeField::SetNotPredefinedAttributeL
// Sets attribute and value. Attribute is not one of predefined ones.
// -----------------------------------------------------------------------------
//
void CSdpAttributeField::SetNotPredefinedAttributeL(const TDesC8& aAttribute, 
                                                    const TDesC8& aValue)
    {
    iAttribute.Close();
    iAttribute = iPool.OpenFStringL(aAttribute);
    AttributeFieldPtrs().SetValueL( aValue );
    }

// -----------------------------------------------------------------------------
// CSdpAttributeField::IsFmtAttribute
// Check if the attribute field belongs to any format attribute
// -----------------------------------------------------------------------------
//
TBool CSdpAttributeField::IsFmtAttribute()
	{
	if ( AttributeFieldPtrs().FmtAttribute() )
		{
		return ETrue;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSdpAttributeField::CreateAttributeFieldPtrsL
// -----------------------------------------------------------------------------
//	
void CSdpAttributeField::CreateAttributeFieldPtrsL()
    {
    CSdpAttributeFieldPtrs* newObj = CSdpAttributeFieldPtrs::NewL();
    CSdpAttributeFieldPtrs* oldObj = 
        reinterpret_cast< CSdpAttributeFieldPtrs* >( iValue );
    delete oldObj;
    iValue = reinterpret_cast< HBufC8* >( newObj );
    }

// -----------------------------------------------------------------------------
// CSdpAttributeField::AttributeFieldPtrs
// -----------------------------------------------------------------------------
//  
inline CSdpAttributeFieldPtrs& CSdpAttributeField::AttributeFieldPtrs()
    {
    return *( reinterpret_cast< CSdpAttributeFieldPtrs* >( iValue ) );
    }

// -----------------------------------------------------------------------------
// CSdpAttributeField::AttributeFieldPtrs
// -----------------------------------------------------------------------------
//  
inline const CSdpAttributeFieldPtrs& 
CSdpAttributeField::AttributeFieldPtrs() const
    {
    return *( reinterpret_cast< CSdpAttributeFieldPtrs* >( iValue ) );
    }

// For DEBUG builds

// -----------------------------------------------------------------------------
// CSdpOriginField::__DbgTestInvariant
// Test invariant
// -----------------------------------------------------------------------------
//	
void CSdpAttributeField::__DbgTestInvariant() const
	{    
	TBool invariant = ( 
        SdpUtil::IsToken( iAttribute.DesC() ) 
		&& iValue != 0 );

	if (!invariant)
		User::Invariant();
	}
