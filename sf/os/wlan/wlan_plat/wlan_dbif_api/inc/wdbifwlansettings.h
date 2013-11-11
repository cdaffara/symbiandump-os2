/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles the custom WLANServiceTable in CommsDat
*
*/

/*
* %version: 13 %
*/

#ifndef WLANSETTINGS_H
#define WLANSETTINGS_H

#include <commsdattypesv1_1.h>
#include <commsdat.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif


using namespace CommsDat;

// Should be same as KMaxWepKeyLength, but defined here to
// avoid dependency to 802dot11.h
const TUint32 KWLMMaxWEPKeyLength = 29;

// The maximum WPA Pre-Shared Key length.
const TUint KWLMMaxWpaPskLength = 64;

// Maximum length of a SSID in BYTES
const TUint8 KMaxSSIDLen = 32;

// Size of Expanded EAP Type
const TUint8 KExpandedEapTypeSize = 8; //8

/**
* Data structure for storing a WEP key.
*/
struct TWep
    {
    /** Specifies which key to add or remove. Range: 0-3 */
    TUint32 KeyIndex;
    /** The length of KeyMaterial in bytes. Range: 0-29 */
    TUint32 KeyLength;
    /** Array that stores the WEP key. */
    TUint8 KeyMaterial[KWLMMaxWEPKeyLength];
    };

/**
* Data structure for storing a WPA Pre-Shared Key.
*/
struct TWpaPsk
	{
	/** The length of KeyMaterial in bytes. Range: 8-64 */
	TUint KeyLength;
	/** Array of that stores the WPA Pre-Shared Key. */
	TUint8 KeyMaterial[KWLMMaxWpaPskLength];
	};

const TInt KWlanWpaPskLength =     64;

enum EWepAuthenticationMode
    {
    EWepAuthModeOpen = 0,
    EWepAuthModeShared = 1    
    };

enum EConnectionMode
    {
    Adhoc =0,
    Infrastructure=1
    };

enum EWlanSecurityMode
    {
    AllowUnsecure = 1,
    Wep = 2,
    Wlan8021x = 4,
    Wpa = 8,
    Wpa2Only = 16,
    Wapi = 32
    };

/**
 * Format of the preshared key (hex/ASCII)
 */
enum TWlanPresharedKeyFormat
    {
    EWlanPresharedKeyFormatAscii,
    EWlanPresharedKeyFormatHex
    };

/**
* struct for wlan settings data
*/
struct SWLANSettings
    {
    TUint32                         Id;
    TBuf<KMaxMedTextLength>         Name;
    
    TUint32                         ServiceID;
    TUint32                         ConnectionMode;        // Infrastructure / adhoc
    TBuf<KMaxSSIDLen>            SSID;                // network name
    
    TBuf<KMaxSSIDLen>            UsedSSID;
    TBool                           ScanSSID;
    TUint32                         ChannelID;

    TUint32                         SecurityMode;         // SecurityMode (Allow insecure, wep, 802.1xs, wpa, wpa2only) enum EWlanSecurityMode
    
    TWep                            WepKey1; 
    TWep                            WepKey2;
    TWep                            WepKey3;
    TWep                            WepKey4;
    TUint32                         WepIndex;  
    TUint32                         AuthenticationMode; // open / Shared
    
    TBuf8<KWlanWpaPskLength>        WPAPreSharedKey;
    TUint32                         WPAKeyLength;
    TBool                           EnableWpaPsk;
    TUint32                         PresharedKeyFormat; // format of the preshared key
    
    // there is also WLAN_EAPS, but it is a long text field, handled separately
    
    };

struct SWlanIAPId
    {
    TUint iWLANRecordId;
    TUint iIAPId;
    };

/**
* Struct for containing eap identifiers
*/
typedef struct _TEap
    {
    TBool    Enabled;
    TBuf8<3>    UID; // Unique ID
    } TEap;

typedef RPointerArray<TEap> TEapArray;
/**
* Struct for containing expanded eap identifiers
*/
struct SEapExpandedType
    {
    TBuf8<KExpandedEapTypeSize>    EapExpandedType; // Unique ID
    };

typedef RPointerArray<SEapExpandedType> RExpandedEapTypeArray;

/**
* Structure for storing information about a secondary SSID.
*/
struct TWlanSecondarySsid
	{
	TUint32 id;
	TBuf<KMaxSSIDLen> ssid;
	TBuf<KMaxSSIDLen> usedSsid;
	};

//  CLASS DECLARATION

/**
* Class for handling wlan table in commsdat
*
* @lib wlandbif.lib
*/
NONSHARABLE_CLASS( CWLanSettings ) : public CBase
    {
public:  // Constructors and destructor

    /**
    * Constructor
    */
    IMPORT_C CWLanSettings();
    
    /**
    * Destructor
    * Handles disconnection if needed
    */
    IMPORT_C virtual ~CWLanSettings();
    
public:  // New functions

    /**
    * Makes a connection to CommsDat
    * @param 
    * @return KErrNone or other symbian error code
    * @remarks
    * @see
    */
    IMPORT_C TInt Connect();
    
    /**
    * Disconnect function that disconnects the connection to commsdat
    * @param 
    * @return
    * @remarks
    * @see
    */
    IMPORT_C void Disconnect();
    
    /**
    * Gets WLAN settings with specified ID
    * @param aId is the id of wlan settings record that is fetched, aSettings contains the wlan settings
    * @return KErrNone or other symbian error code
    * @remarks
    * @see
    */
    IMPORT_C TInt GetWlanSettings(TUint32 aId,SWLANSettings &aSettings);
    
    /**
    * Gets all the wlan settings for specified LAN service
    * @param aId is the id of lan settings that uses wanted wlan settings. aSettings contains the wlan settings
    * @return KErrNone or other symbian error code
    * @remarks
    * @see
    */
    IMPORT_C TInt GetWlanSettingsForService(TUint32 aId,SWLANSettings &aSettings);

    /**
    * Gets WLAN settings with specified IAP ID
    * @param aId is the IAP id that is fetched, aSettings contains the wlan settings
    * @return KErrNone or other symbian error code
    * @remarks
    * @see
    */
    IMPORT_C TInt GetWlanSettingsForIap(TUint32 aId,SWLANSettings &aSettings);

    /**
    *
    * Not supported
    *
    */
    IMPORT_C TInt GetFirstSettings(SWLANSettings &aSettings);

    /**
    *
    * Not supported
    *
    */
    IMPORT_C TInt GetNextSettings(SWLANSettings &aSettings);
    
    /**
    * writes all the wlan settings to CommsDat, if settings.id is 0, new record is created
    * returns KErrNone on success, other error code if failed
    */
    IMPORT_C TInt WriteWlanSettings(SWLANSettings &aSettings);

    /**
    * Deletes wlan settings record with specified ID
    * returns KErrNone on success, other error code if failed
    */
    IMPORT_C TInt DeleteWlanSettings(TUint32 aId);

    /**
    * Deletes wlan settings record for specified LAN service
    * returns KErrNone on success, other error code if failed
    */
    IMPORT_C TInt DeleteWlanSettingsForService(TUint32 aId);

    /**
    *
    * Not supported
    *
    */
    IMPORT_C void AddWlanTableL();

    /**
    * Sets the EAP information to current wlan settings (first do a GetWlanSettings{ForService})
    * returns
    */
    IMPORT_C void SetEapDataL(const TEapArray &aEaps);

    /**
    * Gets the current wlan settings EAP information (first do a GetWlanSettings{ForService})
    * returns
    */
    IMPORT_C void GetEapDataL(TEapArray &aEaps);

    /**
    * Sets aEasyWlan to ETrue if the aServiceID service is an easy wlan
    * returns
    */
    IMPORT_C TInt IsEasyWlan(TUint aServiceId, TBool &aEasyWlan);
    
    /** 
    * Gets WLAN Service Id's which correspond to IAP's using WLAN 
    * returns found WLANServiceTable and IAP record ID's in aServices
    */
    IMPORT_C void GetIAPWlanServicesL( RArray<SWlanIAPId>& aServices);
    
    /**
    * Get all secondary SSIDs for the specified LAN service.
    * @since Series 60 3.0
    * @param aÍd ID of the LAN service.
    * @param aSsids Secondary SSIDs for the service.
    * @return KErrNone if successful, a Symbian error code otherwise.
    */
    IMPORT_C TInt GetSecondarySsidsForService(
    	TUint32 aId,
    	RArray<TWlanSecondarySsid>& aSsids );
    
    /**
    * Gets the current wlan settings EAP information (first do a GetWlanSettings{ForService})
    * returns
    */
	IMPORT_C void GetEapDataL( RExpandedEapTypeArray &aEapsEnabled, RExpandedEapTypeArray &aEapsDisabled );
	
    /**
    * Sets the EAP information to current wlan settings (first do a GetWlanSettings{ForService})
    * returns
    */
	IMPORT_C void SetEapDataL( RExpandedEapTypeArray &aEapsEnabled, RExpandedEapTypeArray &aEapsDisabled );
	
private:

    enum TWlanRecIdType
         {
         EWlanRecordId,
         EWlanServiceId,
         EWlanIapId,
         };
         
    // prepare iWLANServiceRecord (leaves if oom)
    void CreateWLANServiceRecordL( TUint32 aId, TWlanRecIdType aType);
    
    /**
    * Gets current settings
    */
    void GetDataL(SWLANSettings& aSettings);

    /**
    * Writes aSettings to current record
    */
    void WriteDataL(SWLANSettings& aSettings);
    
    /**
    * Reads current Wep Keys
    */
    void ReadWepKeysL(SWLANSettings &aSettings);

    /**
    * If secmode is other than WEP, clear wep keys
    */
    void ClearWepKeys( SWLANSettings &aSettings);

    /**
    * Returns the hex value of aValue, which originally is the ascii value of a hex digit
    */
    void AsciiToHexValue( TUint8& aValue);

    /**
    * Writes Wep Keys to current settings
    */
    void WriteWepKeysL(SWLANSettings &aSettings);

    /**
    * Unpacks wep keys from CommsDat format to TWep struct
    */
    void UnPackWepKey( TDesC8& aPackedKey,TWep& aInputKey, TInt aKeyIndex);

    /**
    * Packs wep keys from TWep to CommsDat format
    */
    void PackWepKeyL(const TWep& aInputKey, CMDBField<TDesC8>* aPackedKey);

    void DoSetEapDataL( TDesC& aData);

	void DoSetEapDataL( TDesC8& aEnabledData, TDesC8& aDisabledData );
    HBufC* DoGetEapDataLC();
    
    void DoGetEapDataLC( HBufC8*& enabledEapsData, HBufC8*& disabledEapsData );
    void DoConnectL();

private:  // Data

    // the current "row"
    TInt iCurrentRecord;

    CMDBGenericRecord* iWLANRecord;
    CMDBSession* iDBSession;
    };

#endif // WLANSETTINGS_H
