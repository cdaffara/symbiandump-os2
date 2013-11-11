/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanElementLocator class.
*
*/

/*
* %version: 9 %
*/

#include "config.h"
#include "umacelementlocator.h"

// offset of the Length field from the beginning of element
const TUint KElemLengthOffset = 1;
// offset of the Type field from the beginning of element Information
const TUint KOuiTypeOffset = 3;

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanElementLocator::WlanElementLocator( const TUint8* aStart, TUint16 aLength )
    : iStart ( aStart ), iLength ( aLength ), iIterator( NULL )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanElementLocator::TWlanLocateStatus WlanElementLocator::InformationElement( 
    TUint8 aIeId, 
    TUint8& aIeLength, 
    const TUint8** aIeData,
    TBool aValidateLength )
    {
    // The general Information Element format is:
    // Element id (1 byte)
    // Length (1 byte)
    // Information (Length bytes)
    //

    TUint8 ie( 0 );
    TUint8 len( 0 );
    const TUint8* data( NULL );

    if ( FirstIE( ie, len, &data ) != EWlanLocateOk )
        {
        aIeLength = 0;
        *aIeData = NULL;
        return EWlanLocateElementNotFound;
        }

    TWlanLocateStatus ret( EWlanLocateOk );

    while ( ie != aIeId && ret == EWlanLocateOk )
        {
        ret = NextIE( ie, len, &data );
        }

    if ( ret == EWlanLocateElementNotFound )
        {
        aIeLength = 0;
        *aIeData = NULL;
        return ret;        
        }
    else
        {
        aIeLength = len;
        *aIeData = data;
        
        if ( aValidateLength )
            {
            return ValidIE( aIeId, aIeLength );
            }
        else
            {
            return EWlanLocateOk;
            }
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanElementLocator::TWlanLocateStatus WlanElementLocator::InformationElement( 
    TUint8 aIeId,
    const TIeOui& aIeOui,
    TUint8 aIeOuiType,
    TUint8& aIeLength, 
    const TUint8** aIeData )
    {
    // Element format is:
    // ID     = Element ID (1 byte)
    // Len    = Length (1 byte)
    // OUI    = OUI (3 bytes)
    // OUItyp = OUI Type (1 byte)
    // ...

    TUint8 ie( aIeId );
    TUint8 len( 0 );
    const TUint8* data( NULL );

    // Find the first element with matching IE id
    if ( InformationElement( aIeId, len, &data, EFalse ) != 
        EWlanLocateOk )
        { // there isn't one
        aIeLength = 0;
        *aIeData = NULL;
        return EWlanLocateElementNotFound;
        }

    // The ID is correct but also the OUI, OUI Type and OUI Subtype need to 
    // match
    TWlanLocateStatus ret( EWlanLocateOk );

    const TUint KMinAcceptableIeDataLen = 
        KIeOuiLength + 
        sizeof( KWmmElemOuiType );
    
    while ( ret == EWlanLocateOk && 
            ( len < KMinAcceptableIeDataLen ||
              ie != aIeId || 
              !( data[0] == aIeOui[0] && data[1] == aIeOui[1] &&
                 data[2] == aIeOui[2] 
               ) ||
              *( data + KOuiTypeOffset ) != aIeOuiType
            ) 
          )
        {
        ret = NextIE( ie, len, &data );
        }

    if ( ret == EWlanLocateElementNotFound  )
        {
        aIeLength = 0;
        *aIeData = NULL;
        return EWlanLocateElementNotFound;
        }
    else
        {
        aIeLength = len;
        *aIeData = data;
        return ValidIE( aIeId, aIeLength, data, aIeOuiType ); 
        }
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanElementLocator::TWlanLocateStatus WlanElementLocator::InformationElement( 
    TUint8 aIeId,
    const TIeOui& aIeOui,
    TUint8 aIeOuiType,
    TUint8 aIeOuiSubtype,    
    TUint8& aIeLength, 
    const TUint8** aIeData )
    {
    // Element format is:
    // ID     = Element ID (1 byte)
    // Len    = Length (1 byte)
    // OUI    = OUI (3 bytes)
    // OUItyp = OUI Type (1 byte)
    // OUIsub = OUI Subtype (1 byte)
    // ...

    TUint8 ie( aIeId );
    TUint8 len( 0 );
    const TUint8* data( NULL );

    // Find the first element with matching IE id
    if ( InformationElement( aIeId, len, &data, EFalse ) != 
        EWlanLocateOk )
        { // there isn't one
        aIeLength = 0;
        *aIeData = NULL;
        return EWlanLocateElementNotFound;
        }

    // The ID is correct but also the OUI, OUI Type and OUI Subtype need to 
    // match
    TWlanLocateStatus ret( EWlanLocateOk );

    const TUint KMinAcceptableIeDataLen = 
        KIeOuiLength + 
        sizeof( KWmmElemOuiType ) + 
        sizeof( KWmmInfoElemOuiSubType );
    
    // offset of the Subtype field from the beginning of element Information
    const TUint KOuiSubtypeOffset = 4;

    while ( ret == EWlanLocateOk && 
            ( len < KMinAcceptableIeDataLen ||
              ie != aIeId || 
              !( data[0] == aIeOui[0] && data[1] == aIeOui[1] &&
                 data[2] == aIeOui[2] 
               ) ||
              *( data + KOuiTypeOffset ) != aIeOuiType ||
              *( data + KOuiSubtypeOffset ) != aIeOuiSubtype
            ) 
          )
        {
        ret = NextIE( ie, len, &data );
        }

    if ( ret == EWlanLocateElementNotFound )
        {
        aIeLength = 0;
        *aIeData = NULL;
        return EWlanLocateElementNotFound;
        }
    else
        {
        aIeLength = len;
        *aIeData = data;
        return ValidIE( aIeId, aIeLength, data, aIeOuiType, aIeOuiSubtype );
        }
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanElementLocator::TWlanLocateStatus WlanElementLocator::FirstIE( 
    TUint8& aIeId, 
    TUint8& aIeLength, 
    const TUint8** aIeData)
    {
    iIterator = iStart;
    return CurrentIE( aIeId, aIeLength, aIeData );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanElementLocator::TWlanLocateStatus WlanElementLocator::NextIE(
    TUint8& aIeId, 
    TUint8& aIeLength, 
    const TUint8** aIeData)
    {
    iIterator += *( iIterator + KElemLengthOffset ) + KInfoElementHeaderLength;
    return CurrentIE( aIeId, aIeLength, aIeData );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanElementLocator::TWlanLocateStatus WlanElementLocator::CurrentIE(
    TUint8& aIeId, 
    TUint8& aIeLength, 
    const TUint8** aIeData) const
    {
    if ( iIterator >= iStart + iLength )
        {
        aIeId = 0;
        aIeLength = 0;
        *aIeData = NULL;
        return EWlanLocateElementNotFound;
        }
    aIeId = *iIterator;
    aIeLength = *( iIterator + KElemLengthOffset );
    *aIeData = iIterator + KInfoElementHeaderLength;
    return EWlanLocateOk;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanElementLocator::TWlanLocateStatus WlanElementLocator::ValidIE(
    TUint8 aIeId,
    TUint8 aIeLength,
    const TUint8* aIeData,
    TUint8 aIeOuiType,
    TUint8 aIeOuiSubtype ) const
    {
    OsTracePrint( KInfoLevel, (TUint8*)
        ("UMAC: WlanElementLocator::ValidIE: element id: %d"), aIeId );
    
    TWlanLocateStatus status ( EWlanLocateElementNotFound );
    
    switch ( aIeId )
        {
        case E802Dot11SupportedRatesIE:
            if ( aIeLength >= K802Dot11SupportedRatesIeDataMinLen &&
                 // Note! This is not according to 802.11 std, but some
                 // implementations do not follow the standard in this respect
                 aIeLength <= KMaxNumberOfDot11bAndgRates )
                {
                status = EWlanLocateOk;
                }
            break;
        case E802Dot11DsParameterSetIE:
            if ( aIeLength == K802Dot11DsParameterSetIeDataLen )
                {
                status = EWlanLocateOk;
                }
            break;
        case E802Dot11TimIE:
            if ( aIeLength >= K802Dot11TimIeDataMinLen && 
                 aIeLength <= K802Dot11TimIeDataMaxLen )
                {
                status = EWlanLocateOk;
                }
            break;
        case E802Dot11IbssParameterSetIE:
            if ( aIeLength == K802Dot11IbssParameterSetIeDataLen )
                {
                status = EWlanLocateOk;
                }
            break;
        case E802Dot11ErpInformationIE:
            if ( aIeLength == K802Dot11ErpInformationIeDataLen )
                {
                status = EWlanLocateOk;
                }
            break;
        case E802Dot11HtCapabilitiesIE:
            if ( aIeLength == K802Dot11HtCapabilitiesIeDataLen )
                {
                status = EWlanLocateOk;
                }
            break;
        case E802Dot11ExtendedRatesIE:
            if ( aIeLength >= K802Dot11ExtendedRatesIeDataMinLen )
                {
                status = EWlanLocateOk;
                }
            break;
        case E802Dot11HtOperationIE:
            if ( aIeLength == K802Dot11HtOperationIeDataLen )
                {
                status = EWlanLocateOk;
                }
            break;
        case E802Dot11VendorSpecificIE:
            if ( aIeData )
                {
                if ( !os_memcmp( 
                        aIeData, 
                        &KWmmElemOui, 
                        KIeOuiLength ) &&
                     aIeOuiType == KWmmElemOuiType &&
                     aIeOuiSubtype == KWmmInfoElemOuiSubType )
                    {
                    // WMM information element
                    
                    if ( aIeLength == KWmmInfoElemLen )
                        {
                        status = EWlanLocateOk;
                        }
                    }
                else if ( !os_memcmp( 
                            aIeData, 
                            &KWmmElemOui, 
                            KIeOuiLength ) &&
                        aIeOuiType == KWmmElemOuiType &&
                        aIeOuiSubtype == KWmmParamElemOuiSubtype )
                    {
                    // WMM Parameter Element
                    
                    if ( aIeLength == sizeof( SWmmParamElemData ) )
                        {
                        status = EWlanLocateOk;
                        }
                    }
                else
                    {
                    // implementation error
                    OsTracePrint( KErrorLevel, 
                        (TUint8*)("UMAC: aIeId: %d"), aIeId );
                    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );                    
                    }
                }
            else
                {
#ifndef NDEBUG                
                OsTracePrint( KErrorLevel, (TUint8*)
                    ("UMAC: WlanElementLocator::ValidIE: ERROR: aIeData is NULL") );
                OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );                            
#endif                
                status = EWlanLocateOk;
                }
            break;
        default:
            // implementation error
            OsTracePrint( KErrorLevel, 
                (TUint8*)("UMAC: aIeId: %d"), aIeId );
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );            
        }
    
    OsTracePrint( KInfoLevel, (TUint8*)
        ("UMAC: WlanElementLocator::ValidIE: status: %d"), status );
    
    return status;
    }
