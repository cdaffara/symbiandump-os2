/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Scan info
*
*/


// INCLUDE FILES
#include <e32std.h>
#include "gendebug.h"
#include "genscaninfo.h"
#include "wlanscaninfoimpl.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWlanScanInfoImpl::CWlanScanInfoImpl()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanScanInfoImpl::CWlanScanInfoImpl()" ) ) );
    }

// Symbian 2nd phase constructor can leave.
void CWlanScanInfoImpl::ConstructL()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanScanInfoImpl::ConstructL()" ) ) );

    iScanInfo = new(ELeave) ScanInfo( iScanList );
    }

// Static constructor.
CWlanScanInfoImpl* CWlanScanInfoImpl::NewL()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanScanInfoImpl::NewL()" ) ) );

    CWlanScanInfoImpl* self = new (ELeave) CWlanScanInfoImpl;
    CleanupStack::PushL( self );    
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CWlanScanInfoImpl::~CWlanScanInfoImpl()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanScanInfoImpl::~CWlanScanInfoImpl()" ) ) );
    delete iScanInfo;
    }

// ---------------------------------------------------------
// CWlanScanInfoImpl::Size
// ---------------------------------------------------------
//
TUint16 CWlanScanInfoImpl::Size() const
    {
    return iScanInfo->Size();
    }

// ---------------------------------------------------------
// CWlanScanInfoImpl::First
// ---------------------------------------------------------
//
const TWlanScanInfoFrame* CWlanScanInfoImpl::First()
    {
    return iScanInfo->First();
    }

// ---------------------------------------------------------
// CWlanScanInfoImpl::Next
// ---------------------------------------------------------
//
const TWlanScanInfoFrame* CWlanScanInfoImpl::Next()
    {
    return iScanInfo->Next();
    }

// ---------------------------------------------------------
// CWlanScanInfoImpl::Current
// ---------------------------------------------------------
//
const TWlanScanInfoFrame* CWlanScanInfoImpl::Current() const
    {
    return iScanInfo->Current();
    }

// ---------------------------------------------------------
// CWlanScanInfoImpl::IsDone
// ---------------------------------------------------------
//
TBool CWlanScanInfoImpl::IsDone() const
    {
    return iScanInfo->IsDone();
    }

// ---------------------------------------------------------
// CWlanScanInfoImpl::RXLevel
// ---------------------------------------------------------
//
TUint8 CWlanScanInfoImpl::RXLevel() const
    {
    TUint8 rssiValue = 110 - ( iScanInfo->RXLevel() / 2 );
    TraceDump( INFO_LEVEL, ( _L( "CWlanScanInfoImpl::RXLevel(): (RCP, RSS) == (%u, %u )" ), iScanInfo->RXLevel(), rssiValue ) );
    return rssiValue;
    }

// ---------------------------------------------------------
// CWlanScanInfoImpl::Bssid
// ---------------------------------------------------------
//
void CWlanScanInfoImpl::Bssid( TWlanBssid& aBssid ) const
    {
    TMacAddress bssid={0};
    iScanInfo->BSSID( bssid.iMacAddress );
    aBssid.Copy( &bssid.iMacAddress[0], KMacAddressLength );
    }

// ---------------------------------------------------------
// CWlanScanInfoImpl::BeaconInterval
// ---------------------------------------------------------
//
TUint16 CWlanScanInfoImpl::BeaconInterval() const
    {
    return iScanInfo->BeaconInterval();
    }

// ---------------------------------------------------------
// CWlanScanInfoImpl::Capability
// ---------------------------------------------------------
//
TUint16 CWlanScanInfoImpl::Capability() const
    {
    return iScanInfo->Capability();
    }

// ---------------------------------------------------------
// CWlanScanInfoImpl::SecurityMode
// Status : Draft
// ---------------------------------------------------------
//
TWlanConnectionSecurityMode CWlanScanInfoImpl::SecurityMode() const
    {
    TWlanConnectionSecurityMode securityMode;
    switch( iScanInfo->SecurityMode() )
        {
		case WlanSecurityModeWep:
			securityMode = EWlanConnectionSecurityWep;
			break;
		case WlanSecurityMode802_1x: // Fallthrough on purpose
		case WlanSecurityModeWapi:   // Fallthrough on purpose
		case WlanSecurityModeWapiPsk:
			securityMode = EWlanConnectionSecurity802d1x;
			break;
		case WlanSecurityModeWpaEap: // Fallthrough on purpose
		case WlanSecurityModeWpa2Eap:
			securityMode = EWlanConnectionSecurityWpa;
			break;
		case WlanSecurityModeWpaPsk: // Fallthrough on purpose
		case WlanSecurityModeWpa2Psk:
			securityMode = EWlanConnectionSecurityWpaPsk;
			break;
		default:
			securityMode = EWlanConnectionSecurityOpen;
			break;
        }
    return securityMode;
    }

// ---------------------------------------------------------
// CWlanScanInfoImpl::InformationElement
// ---------------------------------------------------------
//
TInt CWlanScanInfoImpl::InformationElement( TUint8 aIE, 
                                            TUint8& aLength, 
                                            const TUint8** aData )
    {
    return ConvertErrorCode( iScanInfo->InformationElement( aIE, aLength, aData ) );
    }

// ---------------------------------------------------------
// CWlanScanInfoImpl::WpaIE
// ---------------------------------------------------------
//
TInt CWlanScanInfoImpl::WpaIE( TUint8& aLength, 
                               const TUint8** aData )
    {
    return ConvertErrorCode( iScanInfo->WpaIE( aLength, aData ) );
    }

// ---------------------------------------------------------
// CWlanScanInfoImpl::FirstIE
// ---------------------------------------------------------
//
TInt CWlanScanInfoImpl::FirstIE( TUint8& aIE, 
                                 TUint8& aLength, 
                                 const TUint8** aData )
    {
    return ConvertErrorCode( iScanInfo->FirstIE( aIE, aLength, aData ) );
    }

// ---------------------------------------------------------
// CWlanScanInfoImpl::NextIE
// ---------------------------------------------------------
//
TInt CWlanScanInfoImpl::NextIE( TUint8& aIE, 
                                TUint8& aLength, 
                                const TUint8** aData )
    {
    return ConvertErrorCode( iScanInfo->NextIE( aIE, aLength, aData ) );
    }

// ---------------------------------------------------------
// CWlanScanInfoImpl::GetInternalList
// ---------------------------------------------------------
//
ScanList& CWlanScanInfoImpl::GetInternalList()
    {
    return iScanList;
    }

// ---------------------------------------------------------
// CWlanScanInfoImpl::UpdateResults
// ---------------------------------------------------------
//
TInt CWlanScanInfoImpl::UpdateResults()
    {
    ScanInfo* info = new ScanInfo( iScanList );
    if ( !info )
        {
        return KErrNoMemory;
        }

    delete iScanInfo;
    iScanInfo = info;

    return KErrNone;
    }

// ---------------------------------------------------------
// CWlanScanInfoImpl::IsProtectedSetupSupported
// ---------------------------------------------------------
//
TBool CWlanScanInfoImpl::IsProtectedSetupSupported()
    {
    return (iScanInfo->IsProtectedSetupSupported() ? ETrue : EFalse);
    }

// ---------------------------------------------------------
// CWlanScanInfoImpl::ExtendedSecurityMode
// ---------------------------------------------------------
//
TWlanConnectionExtentedSecurityMode CWlanScanInfoImpl::ExtendedSecurityMode() const
    {
    TWlanConnectionExtentedSecurityMode securityMode;
    switch( iScanInfo->SecurityMode() )
        {
        case WlanSecurityModeWep:
            /**
             * It's impossible to determine whether the AP requires open or shared
             * authentication based on scan results, assume open.
             */
            securityMode = EWlanConnectionExtentedSecurityModeWepOpen;
            break;
        case WlanSecurityMode802_1x:
            securityMode = EWlanConnectionExtentedSecurityMode802d1x;
            break;
        case WlanSecurityModeWpaEap:
            securityMode = EWlanConnectionExtentedSecurityModeWpa;
            break;
        case WlanSecurityModeWpa2Eap:
            securityMode = EWlanConnectionExtentedSecurityModeWpa2;
            break;
        case WlanSecurityModeWpaPsk:
            securityMode = EWlanConnectionExtentedSecurityModeWpaPsk;
            break;
        case WlanSecurityModeWpa2Psk:
            securityMode = EWlanConnectionExtentedSecurityModeWpa2Psk;
            break;
        case WlanSecurityModeWapi:
            securityMode = EWlanConnectionExtentedSecurityModeWapi;
            break;
        case WlanSecurityModeWapiPsk:
            securityMode = EWlanConnectionExtentedSecurityModeWapiPsk;
            break;
        default:
            securityMode = EWlanConnectionExtentedSecurityModeOpen;
            break;
        }
    return securityMode;
    }

// ---------------------------------------------------------
// CWlanScanInfoImpl::ConvertErrorCode
// ---------------------------------------------------------
//
TInt CWlanScanInfoImpl::ConvertErrorCode( TInt aCode ) const
    {
    TInt errorCode( KErrGeneral );
    
    switch( aCode )
        {
        case WlanScanError_Ok:
            {
            errorCode = KErrNone;
            break;
            }
        case WlanScanError_IeNotFound:
            {
            errorCode = KErrNotFound;
            break;
            }
        }
    return errorCode;
    }

//  End of File  
