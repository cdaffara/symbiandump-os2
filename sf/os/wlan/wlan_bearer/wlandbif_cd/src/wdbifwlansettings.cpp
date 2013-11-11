/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles the custom WLANServiceTable  in CommsDat
*
*/

/*
* %version: 14 %
*/

//  INCLUDE FILES

#include <e32svr.h>
#include <commdb.h>
#include <commsdattypeinfov1_1.h>
#include "wdbifwlansettings.h"
#include "wdbifwlandefs.h"
#include "gendebug.h"

// LOCAL CONSTANTS

_LIT( KGenericTable, "WLANServiceTable");

const int KMaxEapDataBufLength = 128;
const int KOldStyleMaxEapTypeValue = 100;

// ---------------------------------------------------------
// Class CWlanSettings
// ---------------------------------------------------------

// MEMBER FUNCTIONS

// -----------------------------------------------------------------------------
// CWLanSettings::CWLanSettings()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CWLanSettings::CWLanSettings() : iCurrentRecord( KErrNotFound),
                                           iWLANRecord( 0),
                                           iDBSession( 0)
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::CWlanSettings()")));    
    }

// Destructor
//
EXPORT_C CWLanSettings::~CWLanSettings() 
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::~CWlanSettings()")));    
    delete iDBSession;
    delete iWLANRecord;
    }

// ---------------------------------------------------------
// TInt CWLanSettings::Connect()
// Connects to the CommsDat storage server
// Wraps around leavable function DoConnectL
// returns KErrNone on success
// ---------------------------------------------------------
//
EXPORT_C TInt CWLanSettings::Connect()
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::Connect")));

    TRAPD(err,DoConnectL());
    return err;
    }

// ---------------------------------------------------------
// void CWLanSettings::Disconnect()
// Disconnects from the CommsDat storage server
// ---------------------------------------------------------
//
EXPORT_C void CWLanSettings::Disconnect()
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::Disconnect")));
    
    delete iDBSession;
    iDBSession = NULL;
    
    delete iWLANRecord;
    iWLANRecord = NULL;
    
    iCurrentRecord = KErrNotFound;
    }

// ---------------------------------------------------------
// void CWLanSettings::GetDataL( SWLANSettings& aSettings)
// Reads data from a WLANServiceTable record which has previously
// been fetched (= current record) from the CommsDat.
// returns settings in aSettings
// ---------------------------------------------------------
//
void CWLanSettings::GetDataL( SWLANSettings& aSettings)
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetDataL start")));
    
    ClearWepKeys( aSettings);
    
    aSettings.Id = iWLANRecord->RecordId();
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetDataL() - aSettings.Id: %d"),
        aSettings.Id));

    aSettings.Name = *((CMDBField<TDesC>*)iWLANRecord->GetFieldByIdL(KCDTIdRecordName));
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetDataL() - aSettings.Name: %S [%d]"),
        &aSettings.Name, aSettings.Name.Length()));

    aSettings.ServiceID = *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanServiceId));
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetDataL() - aSettings.ServiceID: %d"),
        aSettings.ServiceID));
    
    aSettings.ConnectionMode = *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanConnMode));
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetDataL() - aSettings.ConnectionMode: %d"),
        aSettings.ConnectionMode));
    
    aSettings.SSID = *((CMDBField<TDesC>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanSSID));
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetDataL() - aSettings.SSID: %S [%d]"),
        &aSettings.SSID, aSettings.SSID.Length()));
    
    // new
    aSettings.UsedSSID = *((CMDBField<TDesC>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanUsedSSID));
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetDataL() - aSettings.UsedSSID: %S [%d]"),
        &aSettings.UsedSSID, aSettings.UsedSSID.Length()));
    
    aSettings.ScanSSID = *((CMDBField<TBool>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanScanSSID));
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetDataL() - aSettings.ScanSSID: %d"),
        aSettings.ScanSSID));
    
    aSettings.ChannelID = *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanChannelID));
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetDataL() - aSettings.ChannelID: %d"),
        aSettings.ChannelID));

    // end new

    aSettings.AuthenticationMode = EWepAuthModeOpen; // defaults to open (in case of wep, the value will be read from db later)
    aSettings.EnableWpaPsk = EFalse;
    aSettings.WPAKeyLength = 0;
    aSettings.WPAPreSharedKey.Zero();
    aSettings.PresharedKeyFormat = EWlanPresharedKeyFormatAscii;

    aSettings.SecurityMode = *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanSecMode));
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetDataL() - aSettings.SecurityMode: %d"),
        aSettings.SecurityMode));
    
    if( aSettings.SecurityMode == Wep)
        {    
        // authentication mode can be != open only when WEP in use
        aSettings.AuthenticationMode = *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanAuthMode));
        TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetDataL() - aSettings.AuthenticationMode: %d"),
            aSettings.AuthenticationMode));

        aSettings.WepIndex = *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanWepIndex));
        TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetDataL() - aSettings.WepIndex: %d"),
            aSettings.WepIndex));
        ReadWepKeysL(aSettings);           
        TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetDataL, Wep keys succesfully read")));
        }
    else if( aSettings.SecurityMode == Wlan8021x )
        {
        aSettings.WPAKeyLength = *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanWpaKeyLength));
        TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetDataL() - aSettings.WPAKeyLength: %d"),
            aSettings.WPAKeyLength));
        }
    else if( aSettings.SecurityMode > Wlan8021x )
        {
        aSettings.WPAKeyLength = *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanWpaKeyLength));
        TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetDataL() - aSettings.WPAKeyLength: %d"),
            aSettings.WPAKeyLength));       
        aSettings.EnableWpaPsk = *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanEnableWpaPsk));
        TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetDataL() - aSettings.EnableWpaPsk: %d"),
            aSettings.EnableWpaPsk));       
        CMDBField<TDesC8>* wpaKey = static_cast<CMDBField<TDesC8>*>(iWLANRecord->GetFieldByIdL(KCDTIdWlanWpaPreSharedKey));        
        aSettings.WPAPreSharedKey.Append( *wpaKey);
        
        // In WPA-PSK case the preshared key can be either 8 - 63 characters in ASCII
        // or 64 characters in hex.
        //
        // In WAPI case the preshared key format is stored in WEP key #1 format field.
        if( aSettings.EnableWpaPsk )
            {
            if( aSettings.SecurityMode == Wapi )
                {
                TRAP_IGNORE(
                    aSettings.PresharedKeyFormat = *(CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL( KCDTIdWlanFormatKey1 ) ); 
                }
            else if( aSettings.WPAKeyLength == KWLMMaxWpaPskLength )
                {
                aSettings.PresharedKeyFormat = EWlanPresharedKeyFormatHex;
                }
            }            
        
        TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetDataL() - aSettings.PresharedKeyFormat: %d"),
            aSettings.PresharedKeyFormat));       
        }
    
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetDataL end")));
    }
    


void CWLanSettings::ClearWepKeys( SWLANSettings &aSettings)
    {
    // it is enough to set the keylengths to zero, the key is not attempted to read if length == 0
    aSettings.WepKey1.KeyLength = 0;    
    aSettings.WepKey2.KeyLength = 0;
    aSettings.WepKey3.KeyLength = 0;
    aSettings.WepKey4.KeyLength = 0;
    }

// ---------------------------------------------------------
// void CWLanSettings::WriteDataL( SWLANSettings& aSettings)
// Writes data from aSettings to a WLANServiceTable record which has 
// previously been fetched (or newly created) from the CommsDat.
// ---------------------------------------------------------
//
void CWLanSettings::WriteDataL(SWLANSettings& aSettings)
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::WriteDataL start")));

    CMDBField<TDesC>* textField = (CMDBField<TDesC>*)iWLANRecord->GetFieldByIdL(KCDTIdRecordName);
    textField->SetMaxLengthL(aSettings.Name.Length());
    textField->SetL( aSettings.Name);
    
    *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanServiceId)) = aSettings.ServiceID;
    
    *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanConnMode)) = aSettings.ConnectionMode;

    textField = (CMDBField<TDesC>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanSSID);
    textField->SetMaxLengthL( aSettings.SSID.Length());
    textField->SetL( aSettings.SSID);
        
    *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanAuthMode)) = aSettings.AuthenticationMode;
       
    *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanSecMode)) = aSettings.SecurityMode;

    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::WriteDataL, SecurityMode = %d"),aSettings.SecurityMode));

    if( aSettings.SecurityMode == Wep)
        {
        *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanWepIndex)) = aSettings.WepIndex;
        
        //
        // never write wep keys
        //
        //WriteWepKeysL( aSettings);
        }
    else if( aSettings.SecurityMode > Wep)
        {
        //
        // never write wpa psk key
        //
        /*
        TBuf8<KWlanWpaPskLength> buf;
        
        TInt len = aSettings.WPAPreSharedKey.Length();
        
        for( TInt i=0; i<len; i++)
            {
            buf.Append( aSettings.WPAPreSharedKey[i]);
            }
        
        CMDBField<TDesC8>* binField = (CMDBField<TDesC8>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanWpaPreSharedKey);
        binField->SetMaxLengthL( len);
        binField->SetL( buf);
        */
        *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanWpaKeyLength)) = aSettings.WPAKeyLength;
        *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanEnableWpaPsk)) = aSettings.EnableWpaPsk;
        }
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::WriteDataL end")));
    }


// ---------------------------------------------------------
// TInt CWLanSettings::GetWlanSettings(TUint32 aId,SWLANSettings &aSettings)
// Gets a WLANServiceTable record from CommsDat with record ID aId
// writes record data to aSettings; the record ID of the fetched record is set as the 
// current record
// returns KErrNone on success
// ---------------------------------------------------------
//
EXPORT_C TInt CWLanSettings::GetWlanSettings(TUint32 aId,SWLANSettings &aSettings)
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetWlanSettings %d"),aId));
    
    TRAPD( err, CreateWLANServiceRecordL( aId, EWlanRecordId));
    
    if( !err)
        {
        TRAP( err, GetDataL( aSettings));
        if( !err)
            {
            iCurrentRecord = aId;    
            }
        }
        
    return err;
    }

// ---------------------------------------------------------
// TInt CWLanSettings::GetWlanSettingsForService(TUint32 aId,SWLANSettings &aSettings)
// Gets a WLANServiceTable record from CommsDat with service ID aId
// writes record data to aSettings; the record ID of the fetched record is set as the 
// current record
// returns KErrNone on success
// ---------------------------------------------------------
//
EXPORT_C TInt CWLanSettings::GetWlanSettingsForService(TUint32 aId,SWLANSettings &aSettings)
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetWlanSettingsForService %d"),aId));
    
    TRAPD( err, CreateWLANServiceRecordL( aId, EWlanServiceId));
    
    if( !err)
        {
        TRAP( err, GetDataL( aSettings));
        if( !err)
            {
            iCurrentRecord = aSettings.Id;        
            }
        }
        
    return err;
    }

// ---------------------------------------------------------
// TInt CWLanSettings::GetWlanSettingsForIap( const TUint32 aId, SWLANSettings &aSettings )
// Gets a WLANServiceTable record from CommsDat with IAP ID aId
// writes record data to aSettings; the record ID of the fetched record is set as the 
// current record
// returns KErrNone on success
// ---------------------------------------------------------
//
EXPORT_C TInt CWLanSettings::GetWlanSettingsForIap(TUint32 aId,SWLANSettings &aSettings)
    {
    TraceDump( WARNING_LEVEL, ( _L("CWlanSettings::GetWlanSettingsForIap %d"), aId ) );

    TRAPD( err, CreateWLANServiceRecordL( aId, EWlanIapId ) );
    if ( !err )
        {
        TRAP( err, GetDataL( aSettings ) );
        if ( !err )
            {
            iCurrentRecord = aSettings.Id;
            }
        }

    return err;
    }

// ---------------------------------------------------------
// TInt CWLanSettings::GetFirstSettings( SWLANSettings &aSettings )
// 
// Not implemented (never used)
//
// returns KErrNotSupported 
// ---------------------------------------------------------
//
EXPORT_C TInt CWLanSettings::GetFirstSettings(SWLANSettings& /*aSettings*/) // implemented if needed
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetFirstSettings")));

    return KErrNotSupported;
    }

// ---------------------------------------------------------
// TInt CWLanSettings::GetNextSettings( SWLANSettings &aSettings)
// 
// Not implemented (never used)
//
// returns KErrNotSupported 
// ---------------------------------------------------------
//
EXPORT_C TInt CWLanSettings::GetNextSettings(SWLANSettings& /*aSettings*/) // implemented if needed
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetNextSettings")));

    return KErrNotSupported;
    }

// ---------------------------------------------------------
// TInt CWLanSettings::AddWlanTable( )
// 
// Not implemented (WLAN table is created by UI)
//
// leaves with KErrNotSupported 
// ---------------------------------------------------------
//
EXPORT_C void CWLanSettings::AddWlanTableL()
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::AddWlanTableL")));
    
    User::Leave( KErrNotSupported);    
    }


// ---------------------------------------------------------
// TInt CWLanSettings::WriteWlanSettings( SWLANSettings &aSettings)
// Writes settings from aSettings to a WLANServiceTable record in CommsDat
// If aSettings.Id == 0, creates a new record, given new record ID 
// is returned in aSettings.Id; otherwise modifies an existing record
// returns KErrNone on success
// ---------------------------------------------------------
//
EXPORT_C TInt CWLanSettings::WriteWlanSettings( SWLANSettings& aSettings)
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::WriteWlanSettings start")));

    TInt id = ( aSettings.Id == 0 ? KCDNewRecordRequest : aSettings.Id);
                                            
    TRAPD( err, CreateWLANServiceRecordL( id, EWlanRecordId));
    
    if( !err)
        {
        if( aSettings.Id == 0) // create new
            {  // store first (placeholder for a new generic wlan record)
            TraceDump(WARNING_LEVEL,(_L("CWlanSettings::WriteWlanSettings() creating new record")));
            TRAP( err, iWLANRecord->StoreL( *iDBSession)); 
            if( err)
                {
                return err;
                }
            aSettings.Id = iWLANRecord->RecordId(); // return new id to caller    
            TraceDump(WARNING_LEVEL,(_L("CWlanSettings::WriteWlanSettings() new record id %d"),aSettings.Id));
            }
            
        TRAP( err, WriteDataL( aSettings)); // write from SWLANSettings to iWLANRecord
        if( err)
            {
            TRAP_IGNORE( iWLANRecord->DeleteL( *iDBSession)); // get rid of the placeholder
            TraceDump(WARNING_LEVEL,(_L("CWlanSettings::WriteWlanSettings(), error writing to record; nothing stored")));
            return err;
            }
                // modify the placeholder or an existing record
        TRAP( err, iWLANRecord->ModifyL( *iDBSession));
        }
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::WriteWlanSettings end")));
    return err;
    }


// ---------------------------------------------------------
// void CWLanSettings::ReadWepKeysL( SWLANSettings &aSettings)
// Reads WEP keys from current WLANServiceTable record; the
// record from which data is read must be fetched from CommsDat
// before calling ReadWepKeysL()
// ---------------------------------------------------------
//


void CWLanSettings::ReadWepKeysL( SWLANSettings &aSettings)
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::ReadWepKeysL start")));

    // 1    
    CMDBField<TDesC8>* wepKey = static_cast<CMDBField<TDesC8>*>(iWLANRecord->GetFieldByIdL(KCDTIdWlanWepKey1));
    UnPackWepKey( wepKey->GetL(), aSettings.WepKey1, 0);

    // 2
    wepKey = static_cast<CMDBField<TDesC8>*>(iWLANRecord->GetFieldByIdL(KCDTIdWlanWepKey2));
    UnPackWepKey( wepKey->GetL(), aSettings.WepKey2, 1);

    // 3
    wepKey = static_cast<CMDBField<TDesC8>*>(iWLANRecord->GetFieldByIdL(KCDTIdWlanWepKey3));
    UnPackWepKey( wepKey->GetL(), aSettings.WepKey3, 2);

    // 4
    wepKey = static_cast<CMDBField<TDesC8>*>(iWLANRecord->GetFieldByIdL(KCDTIdWlanWepKey4));
    UnPackWepKey( wepKey->GetL(), aSettings.WepKey4, 3);
    
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::ReadWepKeysL end")));
    }


// ---------------------------------------------------------
// void CWLanSettings::WriteWepKeysL( SWLANSettings &aSettings)
// Writes WEP keys from aSettings to current WLANServiceTable record
// The record which the data is written to must be fetched from CommsDat
// before calling WriteWepKeysL()
// ---------------------------------------------------------
//
void CWLanSettings::WriteWepKeysL( SWLANSettings &aSettings)
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::WriteWepKeys start")));

    // 1
    CMDBField<TDesC8>* wepKey = static_cast<CMDBField<TDesC8>*>(iWLANRecord->GetFieldByIdL(KCDTIdWlanWepKey1));

    ASSERT( aSettings.WepKey1.KeyLength <= KWLMMaxWEPKeyLength);
    PackWepKeyL( aSettings.WepKey1, wepKey);
    
    // 2
    wepKey = static_cast<CMDBField<TDesC8>*>(iWLANRecord->GetFieldByIdL(KCDTIdWlanWepKey2));

    ASSERT( aSettings.WepKey2.KeyLength <= KWLMMaxWEPKeyLength);
    PackWepKeyL( aSettings.WepKey2, wepKey);
    
    // 3
    wepKey = static_cast<CMDBField<TDesC8>*>(iWLANRecord->GetFieldByIdL(KCDTIdWlanWepKey3));

    ASSERT( aSettings.WepKey3.KeyLength <= KWLMMaxWEPKeyLength);
    PackWepKeyL( aSettings.WepKey3, wepKey);

    // 4
    wepKey = static_cast<CMDBField<TDesC8>*>(iWLANRecord->GetFieldByIdL(KCDTIdWlanWepKey4));

    ASSERT( aSettings.WepKey4.KeyLength <= KWLMMaxWEPKeyLength);
    PackWepKeyL( aSettings.WepKey4, wepKey);
    
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::WriteWepKeys end")));
    }


// ---------------------------------------------------------
// void CWLanSettings::PackWepKeyL( const TWep& aInputKey, CMDBField<TDesC>& aPackedKey)
// Copies wepkey string from wepkey structs TUint8 buffer to a WLANServiceTable
// record's WEP key field descriptor
// ---------------------------------------------------------
//
void CWLanSettings::PackWepKeyL( const TWep& aInputKey, CMDBField<TDesC8>* aPackedKey)
    {
    TraceDump(ERROR_LEVEL,(_L("CWlanSettings::PackWepKeyL %d"),aInputKey.KeyIndex));

    if( aInputKey.KeyLength > KWLMMaxWEPKeyLength)
        {
        User::Leave( KErrArgument);
        }
    
    TBuf8<KWLMMaxWEPKeyLength> wepBuf;
    
    for (TUint i = 0 ; i<aInputKey.KeyLength; i++)
        {
        wepBuf.Append(aInputKey.KeyMaterial[i]);
        }
    
    aPackedKey->SetL( wepBuf);
    }


void CWLanSettings::AsciiToHexValue( TUint8& aValue)
    {
    // 0-9 = [48(0x30) - 57(0x39)], A-F = [65(0x41) - 70(0x46)], a-f = [97(0x61)-102(0x66)]
    
    if( aValue >= '0' && aValue <= '9') // 0..9
        {
        aValue -= '0';
        }
    else if( aValue >= 'A' && aValue <= 'F')  // A..F
        {
        aValue -= ('A' - 10);
        }
    else if( aValue >= 'a' && aValue <= 'f') // a..f
        {
        aValue -= ('a' - 10);    
        }
    }

// ---------------------------------------------------------
// void CWLanSettings::UnPackWepKeyL( const TDesC& aInputKey, TWep& aUnpackedKey, TInt aKeyIndex )
// Copies 8 bit wepkey string to TWep structs TUint8 buffer from a WLANServiceTable
// record's WEP key field descriptor
// ---------------------------------------------------------
//
void CWLanSettings::UnPackWepKey( TDesC8& aInputKey, TWep& aUnpackedKey, TInt aKeyIndex )
    {
    TraceDump(ERROR_LEVEL,(_L("CWlanSettings::UnPackWepKey %d"),aKeyIndex));
    
    TUint length = aInputKey.Length() / 2; // in aInputKey, 0xXY numbers are represented as ascii(X)ascii(Y)
                                           // -> actual length is half of the length of aInputKey
    
    if( length == 0 || length > KWLMMaxWEPKeyLength)
        {
        aUnpackedKey.KeyLength = 0;
        return;
        }
    
    for( TUint i=0, j=0; i<length ; i++)
        {
        TUint8 hexMSB = aInputKey[j++];
        TUint8 hexLSB = aInputKey[j++];
    
        AsciiToHexValue( hexMSB);
        AsciiToHexValue( hexLSB);
        
        TUint8 keyMaterial = hexMSB * 0x10 + hexLSB;
        
        aUnpackedKey.KeyMaterial[i] = keyMaterial;
        }
    aUnpackedKey.KeyLength = length;
    aUnpackedKey.KeyIndex = aKeyIndex;
    }


// ---------------------------------------------------------
// TInt CWLanSettings::DeleteWlanSettings( TUint32 aId)
// Deletes a WLANServiceTable record with record ID aId
// returns KErrNone on success
// ---------------------------------------------------------
//
EXPORT_C TInt CWLanSettings::DeleteWlanSettings( TUint32 aId)
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::DeleteWlanSettings")));
    
    TRAPD( err, CreateWLANServiceRecordL( aId, EWlanRecordId));
    
    if( !err)
        {
        TRAP( err, iWLANRecord->DeleteL( *iDBSession));
        }
    return err;    
    }


// ---------------------------------------------------------
// TInt CWLanSettings::DeleteWlanSettingsForService( TUint32 aId)
// Deletes a WLANServiceTable record with service ID aId
// returns KErrNone on success
// ---------------------------------------------------------
//
EXPORT_C TInt CWLanSettings::DeleteWlanSettingsForService( TUint32 aId)
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::DeleteWlanSettingsForService")));
    
    TRAPD( err, CreateWLANServiceRecordL( aId, EWlanServiceId));
    
    if( !err)
        {
        TRAP( err, iWLANRecord->DeleteL( *iDBSession));
        }

    return err;    
    }


// ---------------------------------------------------------
// void CWLanSettings::SetEapDataL( const TEapArray &aEaps)
// Sets EAP data to current WLANServiceTable record
// Format "+123,- 34", + means enabled, - disabled, then id, 
// id is always 3 characters for easy parsing
//
// GetWlanSettings() or GetWlanSettingsForService() must be called prior to
// calling SetEapDataL() so that current record is set
// ---------------------------------------------------------
//
EXPORT_C void CWLanSettings::SetEapDataL( const TEapArray &aEaps)
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::SetEapDataL start")));

    // the behaviour has been that first user calls GetWlanSettingsForService()
    // and then writes EAP data..
 
    if( iCurrentRecord == KErrNotFound)
        {
        User::Leave( KErrNotReady);
        }

    TInt num( aEaps.Count());
    
    if( num == 0)
        {
        return;
        }
   
    HBufC16* DbText = HBufC16::NewLC( 5*num); // 5 chars per eap entry + ','
    TPtr16 ptr( DbText->Des());
    TBuf16<3+1> UidTmp;   // 4 chars per eap entry

    ptr = _L("");
    for( TInt i=0 ; i<num; i++)
        {
        UidTmp.Copy( aEaps[i]->UID);
        if( i > 0)
            {
            ptr.Append(',');
            }
        ptr.AppendFormat( _L("%c%3s"), aEaps[i]->Enabled ? '+' : '-', UidTmp.PtrZ());
        }
    
    DoSetEapDataL( ptr);
    
    CleanupStack::PopAndDestroy(DbText);
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::SetEapDataL end")));
    }


EXPORT_C void CWLanSettings::SetEapDataL( RExpandedEapTypeArray &aEapsEnabled, RExpandedEapTypeArray &aEapsDisabled )
    {
    TraceDump( WARNING_LEVEL, ( _L("CWlanSettings::SetEapDataL start") ) );

    // the behaviour has been that first user calls GetWlanSettingsForService()
    // and then writes EAP data..
 
    if ( iCurrentRecord == KErrNotFound )
        {
        User::Leave( KErrNotReady );
        }
    
    HBufC8* const DbDataEnabled = HBufC8::NewLC( aEapsEnabled.Count() * KExpandedEapTypeSize ); // 8 bytes per eap entry
    TPtr8 enabledPtr( DbDataEnabled->Des() );
    for ( TInt i = 0 ; i < aEapsEnabled.Count(); i++ )
        {
		enabledPtr.Append( aEapsEnabled[i]->EapExpandedType );
        }
        
    HBufC8* const DbDataDisabled = HBufC8::NewLC( aEapsDisabled.Count() * KExpandedEapTypeSize ); // 8 bytes per eap entry
    TPtr8 disabledPtr( DbDataDisabled->Des() );
    for ( TInt i = 0 ; i < aEapsDisabled.Count(); i++ )
        {
		disabledPtr.Append( aEapsDisabled[i]->EapExpandedType );
        }

    DoSetEapDataL( enabledPtr, disabledPtr );
    
    CleanupStack::PopAndDestroy( DbDataDisabled );
    CleanupStack::PopAndDestroy( DbDataEnabled );
    
    TraceDump( WARNING_LEVEL, ( _L("CWlanSettings::SetEapDataL end") ) );
    }

// ---------------------------------------------------------
// void CWLanSettings::DoSetEapDataL( TDesC& aData)
// Sets EAP data to current WLANServiceTable record
// ---------------------------------------------------------
//
void CWLanSettings::DoSetEapDataL( TDesC& aData)
    {
    
    ASSERT( iCurrentRecord > 0);
    
    CMDBField<TDesC>* eapList = (CMDBField<TDesC>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanEaps);
    eapList->SetMaxLengthL( aData.Length());
    eapList->SetL( aData);
    iWLANRecord->ModifyL( *iDBSession);

    }

// ---------------------------------------------------------
// void CWLanSettings::DoSetEapDataL( TDesC8& aEnabledData, TDesC8& aDisabledData )
// Sets EAP data to current WLANServiceTable record
// ---------------------------------------------------------
//
void CWLanSettings::DoSetEapDataL( TDesC8& aEnabledData, TDesC8& aDisabledData )
    {
    ASSERT( iCurrentRecord > 0 );
    
    CMDBField<TDesC8>* const eapEnabledList = (CMDBField<TDesC8>*) iWLANRecord->GetFieldByIdL( KCDTIdWlanEapsExtEnabled );
    eapEnabledList->SetMaxLengthL( aEnabledData.Length() );
    eapEnabledList->SetL( aEnabledData );
    
    CMDBField<TDesC8>* const eapDisabledList = (CMDBField<TDesC8>*) iWLANRecord->GetFieldByIdL( KCDTIdWlanEapsExtDisabled );
    eapDisabledList->SetMaxLengthL( aDisabledData.Length() );
    eapDisabledList->SetL( aDisabledData );
    
    iWLANRecord->ModifyL( *iDBSession );

    }

// ---------------------------------------------------------
// HBufC* CWLanSettings::DoGetEapDataLC()
// Gets eap data from current WLANServiceTable record
// returns buffer which contains the data
// ---------------------------------------------------------
//
HBufC* CWLanSettings::DoGetEapDataLC()
    {
    
    ASSERT( iCurrentRecord > 0);
    
    HBufC* temp = HBufC::NewLC( KMaxEapDataBufLength);
    
    TPtr ptr( temp->Des());
        
    ptr = *((CMDBField<TDesC>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanEaps));
    
    return temp;
    }

// ---------------------------------------------------------
// HBufC* CWLanSettings::DoGetEapDataLC( HBufC8*& enabledEapsData, HBufC8*& disabledEapsData )
// Gets eap data from current WLANServiceTable record
// returns buffer which contains the data
// ---------------------------------------------------------
//
void CWLanSettings::DoGetEapDataLC( HBufC8*& enabledEapsData, HBufC8*& disabledEapsData )
	{
    ASSERT( iCurrentRecord > 0 );
    
    enabledEapsData = HBufC8::NewLC( KMaxEapDataBufLength );
    disabledEapsData = HBufC8::NewLC( KMaxEapDataBufLength );

    TPtr8 ptr( enabledEapsData->Des() );
    TPtr8 ptr2( disabledEapsData->Des() );
    ptr = *( (CMDBField<TDesC8>*) iWLANRecord->GetFieldByIdL( KCDTIdWlanEapsExtEnabled) );
   // ptr2 = disabledEapsData->Des();
    ptr2 = *( (CMDBField<TDesC8>*) iWLANRecord->GetFieldByIdL( KCDTIdWlanEapsExtDisabled) );
	}

// ---------------------------------------------------------
// void CWLanSettings::GetEapDataL( TEapArray &aEaps)
// Gets EAP data from current WLANServiceTable record
// see format in SetEapDataL()
//
// GetWlanSettings() or GetWlanSettingsForService() must be called prior to
// calling GetEapDataL() so that current record is set
// ---------------------------------------------------------
//
EXPORT_C void CWLanSettings::GetEapDataL( TEapArray &aEaps)
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetEapDataL start")));
 
    if( iCurrentRecord == KErrNotFound)
        {
        User::Leave( KErrNotReady);
        }
    
    HBufC* eapData = 0;

    eapData = DoGetEapDataLC();
   
    aEaps.Reset();
    
    TUint Length = eapData->Length();
   
    if( Length == 0)
        {
        CleanupStack::PopAndDestroy( eapData);
        return;
        }
        
    TPtr16 ptr( eapData->Des());
    TEap* eapTmp;
    TUint x = 0;
    
    while( x < Length)
        {
        eapTmp = new (ELeave) TEap; 
        if( (ptr[x++]) == '+')
            {
            eapTmp->Enabled = ETrue;
            }
            
        else 
            {
            eapTmp->Enabled=EFalse;
            }
           
        eapTmp->UID.Zero();
        eapTmp->UID.Append( ptr[x++]);
        eapTmp->UID.Append( ptr[x++]);
        eapTmp->UID.Append( ptr[x++]);
        eapTmp->UID.Trim();
        x++; // skip ',' if there is one
        aEaps.Append( eapTmp); 
        }
    CleanupStack::PopAndDestroy( eapData);
    }
// ---------------------------------------------------------
// void CWLanSettings::GetEapDataL( RExpandedEapTypeArray &aEapsEnabled, RExpandedEapTypeArray &aEapsDisabled )
// Gets EAP data from current WLANServiceTable record
// see format in SetEapDataL()
//
// GetWlanSettings() or GetWlanSettingsForService() must be called prior to
// calling GetEapDataL() so that current record is set
// ---------------------------------------------------------
//
EXPORT_C void CWLanSettings::GetEapDataL( RExpandedEapTypeArray &aEapsEnabled, RExpandedEapTypeArray &aEapsDisabled )
    {
    TraceDump( WARNING_LEVEL, ( _L("CWlanSettings::GetEapDataL start") ) );
 
    if ( iCurrentRecord == KErrNotFound )
        {
        User::Leave( KErrNotReady );
        }
   
    aEapsEnabled.Reset();
    aEapsDisabled.Reset();
   
    HBufC* const eapData = DoGetEapDataLC();

    const TUint Length = eapData->Length();
   
    if ( Length == 0 )
        {
    	TraceDump(WARNING_LEVEL,(_L("CWlanSettings::start getting data")));

        // No old-style EAP data
		CleanupStack::PopAndDestroy( eapData );
		        
        // Get EAP data as in new format
		HBufC8* enabledEapsData;
		HBufC8* disabledEapsData;
		DoGetEapDataLC( enabledEapsData, disabledEapsData );
    	TraceDump(WARNING_LEVEL,(_L("CWlanSettings::data taken")));
		
	    // Array pointer
	    TPtr8 ptr( enabledEapsData->Des() );
    
		TInt i = 0;
		for ( i = 0; i<(enabledEapsData->Length() / KExpandedEapTypeSize); i++ )
			{
	        SEapExpandedType* const expandedEap = new (ELeave) SEapExpandedType();
	        CleanupStack::PushL(expandedEap);
	        expandedEap->EapExpandedType = ptr.Mid( i * KExpandedEapTypeSize, KExpandedEapTypeSize );
	        aEapsEnabled.AppendL( expandedEap );
			CleanupStack::Pop( expandedEap );
    		TraceDump(WARNING_LEVEL,(_L("CWlanSettings::inloop1")));
 			}
        TraceDump(WARNING_LEVEL,(_L("CWlanSettings::first loop ok")));
			
		ptr = disabledEapsData->Des();
		for ( i = 0; i<(disabledEapsData->Length() / KExpandedEapTypeSize); i++ )
			{
	        SEapExpandedType* const expandedEap = new (ELeave) SEapExpandedType();
	        CleanupStack::PushL(expandedEap);
	        expandedEap->EapExpandedType = ptr.Mid( i * KExpandedEapTypeSize, KExpandedEapTypeSize );
	        aEapsDisabled.AppendL( expandedEap );
			CleanupStack::Pop( expandedEap );
    		TraceDump(WARNING_LEVEL,(_L("CWlanSettings::inloop2")));
			}
        TraceDump(WARNING_LEVEL,(_L("CWlanSettings::second loop ok")));
			    
		CleanupStack::PopAndDestroy( disabledEapsData );
		CleanupStack::PopAndDestroy( enabledEapsData );
		
	    TraceDump( WARNING_LEVEL,( _L("CWlanSettings::GetEapDataL end") ) );
		return;
        }
    
    else
    	{
    	// Convert to new format. This part should be removed after old-atyle EAP list support is not needed anymore
    	
    	TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetEapDataL - old style data")));

	    TUint16 x = 0;
	    TUint16 newEapValue;
	    TBool enableEapType;

	    // Array pointer
	    const TPtrC16 ptr( eapData->Des() );
	    
	    while ( (x+3) < Length )
	        {
	        TBool addToList = ETrue;
	        if ( ptr[x] == '+' )
	            {
	            // Append into enabled list
	            enableEapType = ETrue;
	            }
	        else if ( ptr[x] == '-' )
	            {
	            // Append into disabled list
	        	enableEapType = EFalse;
	            }
	        else 
	            {
	            // Unknown value
	            TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetEapDataL - unknown value")));
	            enableEapType = EFalse;
	            addToList = EFalse;
	            }

            ++x;
            TChar tens( ptr[x+1] );
            TChar ones( ptr[x+2] );

            newEapValue = KOldStyleMaxEapTypeValue;
	        
            if( tens.IsDigit() && ones.IsDigit() )
                {
                newEapValue = tens.GetNumericValue() * 10 + ones.GetNumericValue();	
                }

            if( addToList && ptr[x] == '0' && newEapValue < KOldStyleMaxEapTypeValue )
                {
                SEapExpandedType* const expandedEap = new (ELeave) SEapExpandedType();
                CleanupStack::PushL( expandedEap );
	        
                expandedEap->EapExpandedType.Zero();

                expandedEap->EapExpandedType.Append( 0xFE );

                if ( newEapValue == 99 )
                    {
                    // Plain MSCHAPv2
                    expandedEap->EapExpandedType.Append( 0xFF );
                    expandedEap->EapExpandedType.Append( 0xFF );
                    expandedEap->EapExpandedType.Append( 0xFF );
                    expandedEap->EapExpandedType.Append( 0x00 );
                    expandedEap->EapExpandedType.Append( 0x00 );
                    expandedEap->EapExpandedType.Append( 0x00 );
                    expandedEap->EapExpandedType.Append( 0x1A );
                    }
                else
                    {
                    expandedEap->EapExpandedType.Append( 0x00 );
                    expandedEap->EapExpandedType.Append( 0x00 );
                    expandedEap->EapExpandedType.Append( 0x00 );
                    expandedEap->EapExpandedType.Append( 0x00 );
                    expandedEap->EapExpandedType.Append( 0x00 );
                    expandedEap->EapExpandedType.Append( 0x00 );
                    expandedEap->EapExpandedType.Append( newEapValue );
                    }
	        
                if ( enableEapType )
                    {
                    aEapsEnabled.AppendL( expandedEap );
                    }
                else
                    {
                    aEapsDisabled.AppendL( expandedEap );
                    }
	        
                CleanupStack::Pop( expandedEap );
                }
                
            x += 4;
            }	        
	    
	    // Store new-style EAP list
	    HBufC8* const DbDataEnabled = HBufC8::NewLC( aEapsEnabled.Count() * KExpandedEapTypeSize ); // 8 bytes per eap entry
	    TPtr8 enabledPtr( DbDataEnabled->Des() );
	    for ( TInt i = 0 ; i < aEapsEnabled.Count(); i++ )
	        {
			enabledPtr.Append( aEapsEnabled[i]->EapExpandedType );
	        }
	        
	    HBufC8* const DbDataDisabled = HBufC8::NewLC( aEapsDisabled.Count() * KExpandedEapTypeSize ); // 8 bytes per eap entry
	    TPtr8 disabledPtr( DbDataDisabled->Des() );
	    for ( TInt i = 0 ; i < aEapsDisabled.Count(); i++ )
	        {
			disabledPtr.Append( aEapsDisabled[i]->EapExpandedType );
	        }
	    
	    CMDBField<TDesC8>* const eapEnabledList = (CMDBField<TDesC8>*) iWLANRecord->GetFieldByIdL( KCDTIdWlanEapsExtEnabled );
	    eapEnabledList->SetMaxLengthL( enabledPtr.Length() );
	    eapEnabledList->SetL( enabledPtr );
	    
	    CMDBField<TDesC8>* const eapDisabledList = (CMDBField<TDesC8>*) iWLANRecord->GetFieldByIdL( KCDTIdWlanEapsExtDisabled );
	    eapDisabledList->SetMaxLengthL( disabledPtr.Length() );
	    eapDisabledList->SetL( disabledPtr );
	    
	    // Empty old-style EAP list
	    CMDBField<TDesC>* const eapList = (CMDBField<TDesC>*) iWLANRecord->GetFieldByIdL( KCDTIdWlanEaps );
	    eapList->SetMaxLengthL( 0 );
	    
	    iWLANRecord->ModifyL( *iDBSession );
	    
		CleanupStack::PopAndDestroy( DbDataDisabled );
		CleanupStack::PopAndDestroy( DbDataEnabled );
	    
		CleanupStack::PopAndDestroy( eapData );
    	}
	
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetEapDataL end")));
    }


// ---------------------------------------------------------
// TInt CWLanSettings::IsEasyWlan( TUint aServiceId, TBool &aEasyWlan)
// Checks if wlan settings with given ServiceId is easywlan iap
// returns KErrNone on success
// ---------------------------------------------------------
//
EXPORT_C TInt CWLanSettings::IsEasyWlan( TUint aServiceId, TBool &aEasyWlan)
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::IsEasyWlan %d"),aServiceId));
    
    aEasyWlan = EFalse;

    TRAPD( err, CreateWLANServiceRecordL( aServiceId, EWlanServiceId));

    if( !err)
        {
            CMDBField<TDesC>* ssid = NULL;
            TRAP(err, ssid = static_cast<CMDBField<TDesC>*>(iWLANRecord->GetFieldByIdL(KCDTIdWlanSSID)));
            if (err)
                {
            	return err;
                }
            
            TPtrC pSsid;
            TRAP(err, pSsid.Set(ssid->GetL()));
            if (err)
                {
            	return err;
                }
            
            if( pSsid.Length() == 0)
                {
                aEasyWlan = ETrue;
                }
                
                
        }
    return err;
    }

// ----------------------------------------------------------
// void CWLanSettings::GetIAPWlanServicesL( RArray<SWlanIAPId>& aServices)
// Find all WLANServiceTable records which correspond to wlan IAPs 
// returns ID's to both WLANServiceTable records and IAP records in aServices
// ----------------------------------------------------------
//

EXPORT_C void CWLanSettings::GetIAPWlanServicesL( RArray<SWlanIAPId>& aServices)
    {
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetIAPWlanServicesL start")));
    CleanupClosePushL( aServices );
    
    CMDBRecordSet<CCDIAPRecord>* iapRecSet = new (ELeave) CMDBRecordSet<CCDIAPRecord>(KCDTIdIAPRecord);
    CleanupStack::PushL( iapRecSet);
    
    CCDIAPRecord* iapPrimer = static_cast<CCDIAPRecord*>
                ( CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord));
    CleanupStack::PushL( iapPrimer);

    // prime record set for service type "LANService"
    TPtrC servType( KCDTypeNameLANService);
    iapPrimer->iServiceType.SetL( servType);
    iapRecSet->iRecords.AppendL( iapPrimer);
    
    CleanupStack::Pop( iapPrimer );
    iapPrimer = 0;
    
    TBool found( EFalse);

    // get all iap records with service type "LANService"    
    found = iapRecSet->FindL( *iDBSession);
    
    // find all such WLANServiceTable records that have a ServiceID that match some lanId
    if( found)
        {
        TraceDump(WARNING_LEVEL, (_L("\tfound %d IAPs with LANService type"),iapRecSet->iRecords.Count()));
        for( TInt i=0; i<iapRecSet->iRecords.Count(); i++)
            {
            TRAPD( error, CreateWLANServiceRecordL( (*iapRecSet)[i]->iService, EWlanServiceId));
            
            if( error == KErrNone)
                {    // found a WLANServiceTable record with ServiceID that matches a LANService record id
                SWlanIAPId id;
                id.iWLANRecordId = iWLANRecord->RecordId();  // WLANServiceTable record ID (not the value of ServiceID of the found record)
                id.iIAPId = (*iapRecSet)[i]->RecordId();    // IAP record id
                User::LeaveIfError( aServices.Append( id)); 
                }
            }
        }
    CleanupStack::PopAndDestroy( iapRecSet );
    CleanupStack::Pop( &aServices );
    TraceDump(WARNING_LEVEL,(_L("CWlanSettings::GetIAPWlanServicesL end")));
    }

// -----------------------------------------------------------------------------
// CWLanSettings::GetSecondarySsidsForService
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CWLanSettings::GetSecondarySsidsForService(
	TUint32 aId,
	RArray<TWlanSecondarySsid>& aSsids )
	{
	TraceDump( INFO_LEVEL, ( _L( "CWlanSettings::GetSecondarySsidsForService()" ) ) );

	aSsids.Reset();

    /**
    * This is done using the old commdb interface because Symbian
    * doesn't have a class definition for WLAN Secondary SSID table.
    */
    CCommsDatabase* db = NULL;
    TRAPD( ret, db = CCommsDatabase::NewL() );
    if ( ret != KErrNone )
        {
        TraceDump( ERROR_LEVEL,
            ( _L( "CWlanSettings::GetSecondarySsidsForService() - unable to create CCommsDatabase" ) ) );

        return ret;
        }
    
    TraceDump(INFO_LEVEL,
        ( _L( "CWlanSettings::GetSecondarySsidsForService() - searching for WLAN ID %u" ), aId ) );

    CCommsDbTableView* table = NULL;
    TRAP( ret, table = db->OpenViewMatchingUintLC(
        TPtrC( WLAN_SECONDARY_SSID ),
        TPtrC( WLAN_SEC_SSID_SERVICE_ID ),
        aId ); CleanupStack::Pop( table ) );
    if ( ret != KErrNone )
        {
        TraceDump( ERROR_LEVEL,
            ( _L( "CWlanSettings::GetSecondarySsidsForService() - unable to create CCommsDbTableView" ) ) );
        delete db;
        
        return ret;
        }

    ret = table->GotoFirstRecord();
    while ( ret == KErrNone )
        {
        TWlanSecondarySsid entry;
        TRAPD( err, table->ReadUintL( TPtrC( WLAN_SEC_SSID_ID ), entry.id ) );

        if ( err == KErrNone )
            {
            TraceDump( INFO_LEVEL,
                ( _L( "CWlanSettings::GetSecondarySsidsForService() - reading ID %u" ), entry.id ) );
            
            TRAP( err, table->ReadTextL( TPtrC( WLAN_SEC_SSID_SCANNED_SSID ), entry.ssid ) );
            }

        if ( err == KErrNone )
            {
            TRAP( err, table->ReadTextL( TPtrC( WLAN_SEC_SSID_USED_SSID ), entry.usedSsid ) );
            }

        if ( err == KErrNone )
            {
            TraceDump( ERROR_LEVEL,
                ( _L( "CWlanSettings::GetSecondarySsidsForService() - appending entry" ) ) );
            aSsids.Append( entry );
            }
        else
            {
            TraceDump( ERROR_LEVEL,
                ( _L( "CWlanSettings::GetSecondarySsidsForService() - unable to read, discarding" ) ) );            
            }
        ret = table->GotoNextRecord();    
        }

    delete table;
    delete db;
	
	return KErrNone;
	}
    
// ---------------------------------------------------------
// void CWLanSettings::CreateWLANServiceRecordL( TUint32 aId, TWlanRecIdType aType)
// Fetches a WLANServiceTable record from CommsDat or creates a new record if
// aId == KCDNewRecordRequest
// if aType == EWlanRecordId, fetches record using record ID as criteria (can be LoadL'd)
// if aType == EWlanServiceId, fetches record using service ID as criteria (must be FindL'd)
// on success, iWLANRecord points to the fetched or created record
// on failure, iWLANRecord remains NULL
// ---------------------------------------------------------
//
void CWLanSettings::CreateWLANServiceRecordL( TUint32 aId, TWlanRecIdType aType)
    {
    if( iWLANRecord)
        {
        delete iWLANRecord;
        iWLANRecord = NULL;
        }

    if ( aType == EWlanIapId )
        {
        /**
         * Load the IAP record and get the service ID that should point
         * to the WLAN service table.
         */        
        CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord *>
            ( CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
        CleanupStack::PushL( iapRecord );

        TraceDump( INFO_LEVEL,
            ( _L( "CWlanSettings::CreateWLANServiceRecordL() - searching for IAP ID %u"  ), aId ) );

        iapRecord->SetRecordId( aId );
        TRAPD( ret, iapRecord->LoadL( *iDBSession ) );
        if ( ret !=  KErrNone )
            {
            TraceDump( ERROR_LEVEL,
                ( _L( "CWlanSettings::CreateWLANServiceRecordL() - LoadL leaved with %d"  ), ret ) );

            User::Leave( ret );
            }

        if ( TPtrC( iapRecord->iServiceType ) != TPtrC( KCDTypeNameLANService ) ||
             TPtrC( iapRecord->iBearerType ) != TPtrC( KCDTypeNameLANBearer ) )
            {
            TraceDump( ERROR_LEVEL,
                ( _L( "CWlanSettings::CreateWLANServiceRecordL() - IAP doesn't point to LANService table" ) ) );

            User::Leave( KErrNotFound );
            }
        aId = iapRecord->iService;

        TraceDump( INFO_LEVEL,
            ( _L( "CWlanSettings::CreateWLANServiceRecordL() - WLAN ServiceID is %u"  ), aId ) );

        CleanupStack::PopAndDestroy( iapRecord );
        }

    CMDBGenericRecord* const genRec = static_cast<CMDBGenericRecord*> ( CCDRecordBase::RecordFactoryL( 0 ) );
    CleanupStack::PushL( genRec );

    genRec->InitializeL( KGenericTable(),NULL);
    
    if( aType == EWlanRecordId)
        {
        genRec->SetRecordId( aId);
        genRec->LoadL( *iDBSession);
        }
    else // EWlanServiceId
        {
        genRec->LoadL( *iDBSession);
        
        CMDBField<TUint32>* sidField = static_cast<CMDBField<TUint32>*>
                                        (genRec->GetFieldByIdL( KCDTIdWlanServiceId));
            // prime with service id                
        *sidField = aId;
        
        TBool found = genRec->FindL( *iDBSession);
        
        if( !found)
            {
            User::Leave( KErrNotFound);
            }

        }

    CleanupStack::Pop(); //genRec
    iWLANRecord = genRec;
    }

//
// L-methods called and trapped by the public non-leaving methods
//

// ---------------------------------------------------------
// void CWLanSettings::DoConnectL()
// Connects to the CommsDat storage server
// ---------------------------------------------------------
//
void CWLanSettings::DoConnectL()
    {
    TraceDump(ERROR_LEVEL,(_L("CWlanSettings::DoConnectL")));    
    
    if( !iDBSession)
        {
        iDBSession = CMDBSession::NewL( KCDCurrentVersion);
        }
    
    }

