/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface to read and write WLAN device specific settings.
*
*/

/*
* %version: 21 %
*/

// INCLUDE FILES
#include <commdb.h>
#include <centralrepository.h>
#include <commsdattypesv1_1.h>
#include <e32std.h>

#include "am_debug.h"
#include "wlandevicesettings.h"
#include "wlandevicesettingsprivatecrkeys.h"

// CONSTANTS
// Ids for Wlan settings type.
const TUint32 KWlanDefaultSettings            = 0;
const TUint32 KWlanUserSettings               = 1;
const TUint32 KMaxCommsDbWriteAttempts        = 10;
const TUint32 KRetryIntervalUsIfCommsDbLocked = 100000;

// Table name
_LIT( KWlanDeviceSettings,          "WLANDeviceTable" );
_LIT( KTableVersion,                "Version");
_LIT( KWlanDeviceSettingsType,      "WlanDeviceSettingsType" );
_LIT( KBgScanInterval,              "WlanBgScanInterval" );
_LIT( KUseDefaultSettings,          "WlanUseDefaultSettings" );
_LIT( KWlanLongRetry,               "WlanLongRetry" );
_LIT( KWlanRTSThreshold,            "WlanRTS" );
_LIT( KWlanShortRetry,              "WlanShortRetry" );
_LIT( KWlanTxPowerLevel,            "WlanTxPowerLevel" );
_LIT( KWlanAllowRadioMeasurements,  "AllowRadioMeasurements" );
_LIT( KWlanPowerMode,               "WlanPowerMode" );

// Increase version every time the content of the table changes!
const TUint32 KWlanDeviceSettingsTableVersion = 9;

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CWlanDeviceSettings::CWlanDeviceSettings
// NOTE! This class is derived from CBase, so, it is
//       initialised with zero. The initialisation of its
//       attributes is unnecessary.
// ---------------------------------------------------------
//
CWlanDeviceSettings::CWlanDeviceSettings()
    {
    } 

// ---------------------------------------------------------
// CWlanDeviceSettings::ConstructL
// ---------------------------------------------------------
//
void CWlanDeviceSettings::ConstructL()
    {
    DEBUG( "CWlanDeviceSettings::ConstructL()" );
    // Open the CommsDB.
    iDb = CCommsDatabase::NewL();

    // Check that the version of WlanDeviceSettings is correct.
    TRAPD( err, CheckWlanDeviceSettingsTableL() );

    if ( err != KErrNone )
        {
        // Okay, the version is not correct or the table is not found.
        DEBUG( "ERROR: SETTINGS NOT OK!!" );
        delete iDb;
        iDb = NULL;
        User::Leave( err );
        }

    // Open the WLAN device settings table.
    OpenTableL();
    DEBUG( "CWlanDeviceSettings::ConstructL() Tables opened ok." );
    }

// ---------------------------------------------------------
// CWlanDeviceSettings::NewL
// ---------------------------------------------------------
//
EXPORT_C CWlanDeviceSettings* CWlanDeviceSettings::NewL()
    {
    DEBUG( "CWlanDeviceSettings::NewL()" );

    CWlanDeviceSettings* self = new (ELeave) CWlanDeviceSettings;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CWlanDeviceSettings::~CWlanDeviceSettings
// ---------------------------------------------------------
//
EXPORT_C CWlanDeviceSettings::~CWlanDeviceSettings()
    {
    DEBUG( "CWlanDeviceSettings::~CWlanDeviceSettings()" );
    delete iDefTable;
    delete iUsrTable;
    delete iDb;
    }

// ---------------------------------------------------------
// CWlanDeviceSettings::GetDefaultSettings
// ---------------------------------------------------------
//
EXPORT_C void CWlanDeviceSettings::GetDefaultSettings( 
    SWlanDeviceSettings& aSettings )
    {
    DEBUG( "CWlanDeviceSettings::GetDefaultSettings()" );

    aSettings.beacon = KWlanDefaultBeacon;
    aSettings.longRetry = KWlanDefaultLongRetryLimit;
    aSettings.rts = KWlanDefaultRTSThreshold;
    aSettings.shortRetry = KWlanDefaultShortRetryLimit;
    aSettings.backgroundScanInterval = KWlanDefaultBackgroundScanInterval;
    aSettings.txPowerLevel = KWlanDefaultTxPowerLevel;
    aSettings.scanRate = E1Mbps;
    aSettings.rcpiTrigger = KWlanDefaultRcpiTrigger;
    aSettings.minActiveChannelTime = KWlanDefaultMinChanneltime;
    aSettings.maxActiveChannelTime = KWlanDefaultMaxChanneltime;
    aSettings.maxTxMSDULifeTime = KWlanDefaultMaxTransmitMSDULifetime;
    aSettings.useDefaultSettings = ETrue;		
    aSettings.scanExpirationTimer = KWlanDefaultScanExpirationTimer;
    aSettings.unloadDriverTimer = KWlanDefaultUnloadDriverTimer;
    aSettings.roamTimer = KWlanDefaultRoamTimer;
    aSettings.rcpiDifference = KWlanDefaultRcpiDifference;
    aSettings.connRegainTimer = KWlanDefaultConnRegainTimer;
    aSettings.maxTriesToFindNw = KWlanDefaultMaxTriesToFindNw;
    aSettings.delayBetweenFindNw = KWlanDefaultDelayBetweenFindNw;
    aSettings.powerMode = KWlanDefaultowerMode;
    aSettings.allowRadioMeasurements = ETrue;
    aSettings.minPassiveChannelTime = KWlanDefaultMinPassiveChannelTime;
    aSettings.maxPassiveChannelTime = KWlanDefaultMaxPassiveChannelTime;
    aSettings.maxApFailureCount = KWlanDefaultMaxApFailureCount;
    aSettings.maxApAuthFailureCount = KWlanDefaultMaxApAuthFailureCount;
    aSettings.longBeaconFindCount = KWlanDefaultLongBeaconFindCount;
    aSettings.qosNullFrameInterval = KWlanDefaultQosNullFrameInterval;
    aSettings.qosNullFrameTimeout = KWlanDefaultQosNullFrameTimeout;
    aSettings.keepAliveInterval = KWlanDefaultKeepAliveInterval;          
    aSettings.scanStopRcpiThreshold = KWlanDefaultScanStopRcpiThreshold;      
    aSettings.minRcpiForIapAvailability = KWlanDefaultMinRcpiForIapAvailability;  
    aSettings.qosNullFrameEntryTimeout = KWlanDefaultQoSNullFrameEntryTimeout;   
    aSettings.maxApDeauthenticationCount = KWlanDefaultMaxApDeauthenticationCount; 
    aSettings.apDeauthenticationTimeout = KWlanDefaultApDeauthenticationTimeout;  
    aSettings.maxDtimSkipInterval = KWlanDefaultMaxDtimSkipInterval;
    aSettings.psActiveToLightTimeout = KWlanDefaultPsActiveToLightTimeout;
    aSettings.psActiveToLightThreshold = KWlanDefaultPsActiveToLightThreshold;
    aSettings.psLightToActiveTimeout = KWlanDefaultPsLightToActiveTimeout;
    aSettings.psLightToActiveThreshold = KWlanDefaultPsLightToActiveThreshold;
    aSettings.psLightToDeepTimeout = KWlanDefaultPsLightToDeepTimeout;
    aSettings.psLightToDeepThreshold = KWlanDefaultPsLightToDeepThreshold;
    aSettings.psUapsdRxThreshold = KWlanDefaultPsUapsdRxFrameLengthThreshold;    
    aSettings.rcpiRoamMinInterval = KWlanDefaultRcpiRoamMinInterval;
    aSettings.rcpiRoamMaxInterval = KWlanDefaultRcpiRoamMaxInterval;
    aSettings.rcpiRoamAttemptsPerInterval = KWlanDefaultRcpiRoamAttemptsPerInterval;
    aSettings.rcpiRoamNextIntervalFactor = KWlanDefaultRcpiRoamNextIntervalFactor;
    aSettings.rcpiRoamNextIntervalAddition = KWlanDefaultRcpiRoamNextIntervalAddition;
    aSettings.scanListExpirationTime = KWlanDefaultScanListExpirationTime;
    aSettings.qosNullFrameEntryTxCount = KWlanDefaultQoSNullFrameEntryTxCount;
    aSettings.spRcpiTarget = KWlanDefaultSpRcpiTarget;
    aSettings.spTimeTarget = KWlanDefaultSpTimeTarget;
    aSettings.spMinIndicationInterval = KWlanDefaultSpMinIndicationInterval;
    aSettings.bssLostRoamMinInterval = KWlanDefaultBssLostRoamMinInterval;
    aSettings.bssLostRoamMaxInterval = KWlanDefaultBssLostRoamMaxInterval;
    aSettings.bssLostRoamAttemptsPerInterval = KWlanDefaultBssLostRoamAttemptsPerInterval;
    aSettings.bssLostRoamNextIntervalFactor = KWlanDefaultBssLostRoamNextIntervalFactor;
    aSettings.bssLostRoamNextIntervalAddition = KWlanDefaultBssLostRoamNextIntervalAddition;
    aSettings.bssLostRoamMaxTriesToFindNw = KWlanDefaultBssLostRoamMaxTriesToFindNw;
    aSettings.trafficStreamCreationTimeout = KWlanDefaultTrafficStreamCreationTimeout;
    aSettings.beaconLostThreshold = KWlanDefaultBeaconLostThreshold;
    aSettings.btBeaconLostThreshold = KWlanDefaultBtBeaconLostThreshold;
    aSettings.txFailThreshold = KWlanDefaultTxFailThreshold;
    aSettings.btTxFailThreshold = KWlanDefaultBtTxFailThreshold;
    aSettings.powerSaveDelay = KWlanDefaultPowerSaveDelay;
    aSettings.regionExpirationTime = KWlanDefaultRegionExpirationTime;
    aSettings.rrmMinMeasurementInterval = KWlanDefaultRrmMinMeasurementInterval;
    aSettings.psmServerMode = KWlanDefaultPsmServerMode;
    aSettings.bgScanPeakPeriodStart = KWlanDefaultBgScanPeakPeriodStart;
    aSettings.bgScanPeakPeriodEnd = KWlanDefaultBgScanPeakPeriodEnd;
    aSettings.bgScanIntervalPeak = KWlanDefaultBgScanIntervalPeakPeriod;
    aSettings.bgScanIntervalOffPeak = KWlanDefaultBgScanIntervalOffPeakPeriod;
    aSettings.automaticTrafficStreamMgmt = ETrue;
    aSettings.minActiveBroadcastChannelTime = KWlanDefaultMinActiveBroadcastChannelTime;
    aSettings.maxActiveBroadcastChannelTime = KWlanDefaultMaxActiveBroadcastChannelTime;
    aSettings.enabledFeatures = KWlanDefaultEnabledFeatures;
    aSettings.region = KWlanDefaultRegion;
    aSettings.regionTimestamp = KWlanDefaultRegionTimestamp;
    }

// ---------------------------------------------------------
// CWlanDeviceSettings::ReadL
// Read data from CommDB and copies it to the parameter.
// ---------------------------------------------------------
//
EXPORT_C void CWlanDeviceSettings::ReadL( SWlanDeviceSettings& aSettings )
    {
    DEBUG( "CWlanDeviceSettings::ReadL()" );

    ReadL( EFalse, aSettings );
    if ( aSettings.useDefaultSettings )
        {
        ReadL( ETrue, aSettings );
        }
    }

// ---------------------------------------------------------
// CWlanDeviceSettings::ReadL
// Read data from CommDB and copies it to the parameter.
// ---------------------------------------------------------
//
EXPORT_C void CWlanDeviceSettings::ReadL(
    TBool aGetDefaultSettings,
    SWlanDeviceSettings& aSettings )
    {
    DEBUG1( "CWlanDeviceSettings::ReadL() - GetDefaultSettings == %u",
            aGetDefaultSettings );
            
    // Initialize to hard coded default values
    GetDefaultSettings( aSettings );

    // Read private settings from CenRep
    ReadPrivateData( aSettings );

    DEBUG( "CWlanDeviceSettings::ReadL() - private data read!" );

    // Start reading CommsDat
    // Both tables has to been found
    User::LeaveIfNull( iDefTable );
    User::LeaveIfNull( iUsrTable );
    
    CCommsDbTableView* table;
    if ( aGetDefaultSettings )
        {
        table = iDefTable;
        }
    else
        {
        table = iUsrTable;
        }

    // Read (and convert enumerated) data.
    // NOTE! backgroundScanInterval is
    // always read from user table because they are not part of
    // advanced settings.
    //
    iUsrTable->ReadUintL( KBgScanInterval, aSettings.backgroundScanInterval );
    table->ReadBoolL( KUseDefaultSettings, aSettings.useDefaultSettings );
    table->ReadUintL( KWlanLongRetry, aSettings.longRetry );
    table->ReadUintL( KWlanShortRetry, aSettings.shortRetry );
    table->ReadUintL( KWlanRTSThreshold, aSettings.rts );
    table->ReadUintL( KWlanTxPowerLevel, aSettings.txPowerLevel );
    table->ReadBoolL( KWlanAllowRadioMeasurements, aSettings.allowRadioMeasurements );
    table->ReadUintL( KWlanPowerMode, aSettings.powerMode );
    DEBUG( "CWlanDeviceSettings::ReadL() - done" );

#ifdef _DEBUG
//    LogSettings( aSettings );
#endif // _DEBUG
    }

// ---------------------------------------------------------
// CWlanDeviceSettings::WriteL
// Save data to CommDB.
// ---------------------------------------------------------
//
EXPORT_C void CWlanDeviceSettings::WriteL(
    const SWlanDeviceSettings& aSettings)
    {
    DEBUG( "CWlanDeviceSettings::WriteL()" );
    
    // Write private settings to CenRep
    WritePrivateData( aSettings );
    
    // Write CommsDat settings
    User::LeaveIfNull( iUsrTable );
    User::LeaveIfError( iUsrTable->UpdateRecord() ); // Begin changes.

    iUsrTable->WriteUintL( KBgScanInterval, aSettings.backgroundScanInterval );
    iUsrTable->WriteBoolL( KUseDefaultSettings, aSettings.useDefaultSettings );
    iUsrTable->WriteUintL( KWlanLongRetry, aSettings.longRetry );
    iUsrTable->WriteUintL( KWlanShortRetry, aSettings.shortRetry );
    iUsrTable->WriteUintL( KWlanRTSThreshold, aSettings.rts );
    iUsrTable->WriteUintL( KWlanTxPowerLevel, aSettings.txPowerLevel );
    iUsrTable->WriteBoolL( KWlanAllowRadioMeasurements, aSettings.allowRadioMeasurements );
    iUsrTable->WriteUintL( KWlanPowerMode, aSettings.powerMode );

    TInt err = iUsrTable->PutRecordChanges(); // End and save changes.
    
    if( err == KErrLocked )
        {
        for( TInt retryCount = 1; retryCount <= KMaxCommsDbWriteAttempts; retryCount++ )
            {
            DEBUG2( "CWlanDeviceSettings::WriteL() - CommsDb locked, waiting for %u us before retrying, retryCount: %u",
                    KRetryIntervalUsIfCommsDbLocked,
                    retryCount );
            User::After( TTimeIntervalMicroSeconds32( KRetryIntervalUsIfCommsDbLocked ) );
            err = iUsrTable->PutRecordChanges();
            if( err != KErrLocked )
                {
                break;
                }
            }
        }
    
    User::LeaveIfError( err );

    DEBUG( "CWlanDeviceSettings::WriteL() - done" );
    }

// ---------------------------------------------------------
// CWlanDeviceSettings::ReadPrivateData
// ---------------------------------------------------------
//
TInt CWlanDeviceSettings::ReadPrivateData( SWlanDeviceSettings& aSettings )
    {
    DEBUG( "CWlanDeviceSettings::ReadPrivateData()" );
    
    TInt err = KErrNone;
    CRepository* repository = NULL;
    TRAP( err, repository = CRepository::NewL( KCRUidWlanDeviceSettingsRegistryId ) );
    if( err != KErrNone )
        {
        DEBUG1( "CWlanDeviceSettings::ReadPrivateData() - Could not access repository (%d), using hardcoded values", err );
        return err;
        }
    // No need to use CleanupStack because no possibility to leave

    // Read KWlanBeacon
    TInt temp = 0;
    err = repository->Get( KWlanBeacon, temp );
    if( err == KErrNone ) 
        {
        aSettings.beacon = temp;
        }

    // Read KWlanScanRate
    err = repository->Get( KWlanScanRate, temp );
    if( err == KErrNone ) 
        {
        aSettings.scanRate = static_cast<TRate>( temp );
        }

    // Read KWlanRcpiTrigger
    err = repository->Get( KWlanRcpiTrigger, temp );
    if( err == KErrNone )
        {
        aSettings.rcpiTrigger = temp;
        }

    // Read KWlanMinActiveChannelTime
    err = repository->Get( KWlanMinActiveChannelTime, temp );
    if( err == KErrNone )
        {
        aSettings.minActiveChannelTime = temp;
        }
    
    // Read KWlanMaxActiveChannelTime
    err = repository->Get( KWlanMaxActiveChannelTime, temp );
    if( err == KErrNone )
        {
        aSettings.maxActiveChannelTime = temp;
        }
    
    // Read KWlanMaxTxMSDULifeTime
    err = repository->Get( KWlanMaxTxMSDULifeTime, temp );
    if( err == KErrNone )
        {
        aSettings.maxTxMSDULifeTime = temp;
        }
    
    // Read KWlanScanExpirationTimer
    err = repository->Get( KWlanScanExpirationTimer, temp );
    if( err == KErrNone )
        {
        aSettings.scanExpirationTimer = temp;
        }
    
    // Read KWlanUnloadDriverTimer
    err = repository->Get( KWlanUnloadDriverTimer, temp );
    if( err == KErrNone )
        {
        aSettings.unloadDriverTimer = temp;
        }

    // Read KWlanRoamTimer
    err = repository->Get( KWlanRoamTimer, temp );
    if( err == KErrNone )
        {
        aSettings.roamTimer = temp;
        }
        
    // Read KWlanRcpiDifference
    err = repository->Get( KWlanRcpiDifference, temp );
    if( err == KErrNone ) 
        {
        aSettings.rcpiDifference = temp;
        }

    // Read KWlanConnRegainTimer
    err = repository->Get( KWlanConnRegainTimer, temp );
    if( err == KErrNone ) 
        {
        aSettings.connRegainTimer = temp;
        }

    // Read KWlanMaxTriesToFindNw
    err = repository->Get( KWlanMaxTriesToFindNw, temp );
    if( err == KErrNone ) 
        {
        aSettings.maxTriesToFindNw = temp;
        }

    // Read KWlanDelayBetweenFindNw
    err = repository->Get( KWlanDelayBetweenFindNw, temp );
    if( err == KErrNone ) 
        {
        aSettings.delayBetweenFindNw = temp;
        }

    // Read KWlanMinPassiveChannelTime
    err = repository->Get( KWlanMinPassiveChannelTime, temp );
    if( err == KErrNone ) 
        {
        aSettings.minPassiveChannelTime = temp;
        }

    // Read KWlanMaxPassiveChannelTime
    err = repository->Get( KWlanMaxPassiveChannelTime, temp );
    if( err == KErrNone ) 
        {
        aSettings.maxPassiveChannelTime = temp;
        }
        
    // Read KWlanMaxApFailureCount
    err = repository->Get( KWlanMaxApFailureCount, temp );
    if( err == KErrNone ) 
        {
        aSettings.maxApFailureCount = temp;
        }
        
    // Read KWlanLongBeaconFindCount
    err = repository->Get( KWlanLongBeaconFindCount, temp );
    if( err == KErrNone ) 
        {
        aSettings.longBeaconFindCount = temp;
        }
        
    // Read KWlanQosNullFrameInterval
    err = repository->Get( KWlanQosNullFrameInterval, temp );
    if( err == KErrNone ) 
        {
        aSettings.qosNullFrameInterval = temp;
        }
        
    // Read KWlanQosNullFrameTimeout
    err = repository->Get( KWlanQosNullFrameTimeout, temp );
    if( err == KErrNone ) 
        {
        aSettings.qosNullFrameTimeout = temp;
        }                                

    // Read KWlanKeepAliveInterval
    err = repository->Get( KWlanKeepAliveInterval, temp );
    if( err == KErrNone ) 
        {
        aSettings.keepAliveInterval = temp;
        }                                

    // Read KWlanScanStopRcpiThreshold
    err = repository->Get( KWlanScanStopRcpiThreshold, temp );
    if( err == KErrNone ) 
        {
        aSettings.scanStopRcpiThreshold = temp;
        }                                

    // Read KWlanMinRcpiForIapAvailability
    err = repository->Get( KWlanMinRcpiForIapAvailability, temp );
    if( err == KErrNone ) 
        {
        aSettings.minRcpiForIapAvailability = temp;
        }                                

    // Read KWlanQoSNullFrameEntryTimeout
    err = repository->Get( KWlanQoSNullFrameEntryTimeout, temp );
    if( err == KErrNone ) 
        {
        aSettings.qosNullFrameEntryTimeout = temp;
        }                                

    // Read KWlanMaxApDeauthenticationCount
    err = repository->Get( KWlanMaxApDeauthenticationCount, temp );
    if( err == KErrNone ) 
        {
        aSettings.maxApDeauthenticationCount = temp;
        }                                

    // Read KWlanApDeauthenticationTimeout
    err = repository->Get( KWlanApDeauthenticationTimeout, temp );
    if( err == KErrNone ) 
        {
        aSettings.apDeauthenticationTimeout = temp;
        }                                

    // Read KWlanMaxDtimSkipInterval
    err = repository->Get( KWlanMaxDtimSkipInterval, temp );
    if( err == KErrNone ) 
        {
        aSettings.maxDtimSkipInterval = temp;
        }                                
    
    // Read KWlanPsActiveToLightTimeout
    err = repository->Get( KWlanPsActiveToLightTimeout, temp );
    if( err == KErrNone ) 
        {
        aSettings.psActiveToLightTimeout = temp;
        }       

    // Read KWlanPsActiveToLightThreshold
    err = repository->Get( KWlanPsActiveToLightThreshold, temp );
    if( err == KErrNone ) 
        {
        aSettings.psActiveToLightThreshold = temp;
        }       

    // Read KWlanPsLightToActiveTimeout
    err = repository->Get( KWlanPsLightToActiveTimeout, temp );
    if( err == KErrNone ) 
        {
        aSettings.psLightToActiveTimeout = temp;
        }       

    // Read KWlanPsLightToActiveThreshold
    err = repository->Get( KWlanPsLightToActiveThreshold, temp );
    if( err == KErrNone ) 
        {
        aSettings.psLightToActiveThreshold = temp;
        }       

    // Read KWlanPsLightToDeepTimeout
    err = repository->Get( KWlanPsLightToDeepTimeout, temp );
    if( err == KErrNone ) 
        {
        aSettings.psLightToDeepTimeout = temp;
        }       

    // Read KWlanPsLightToDeepThreshold
    err = repository->Get( KWlanPsLightToDeepThreshold, temp );
    if( err == KErrNone ) 
        {
        aSettings.psLightToDeepThreshold = temp;
        }       

    // Read KWlanPsUapsdRxFrameLengthThreshold
    err = repository->Get( KWlanPsUapsdRxFrameLengthThreshold, temp );
    if( err == KErrNone ) 
        {
        aSettings.psUapsdRxThreshold = temp;
        }       

    // Read KWlanRcpiRoamMinInterval
    err = repository->Get( KWlanRcpiRoamMinInterval, temp );
    if( err == KErrNone ) 
        {
        aSettings.rcpiRoamMinInterval = temp;
        }       
    
    // Read KWlanRcpiRoamMaxInterval
    err = repository->Get( KWlanRcpiRoamMaxInterval, temp );
    if( err == KErrNone ) 
        {
        aSettings.rcpiRoamMaxInterval = temp;
        }       
    
    // Read KWlanRcpiRoamAttemptsPerInterval
    err = repository->Get( KWlanRcpiRoamAttemptsPerInterval, temp );
    if( err == KErrNone ) 
        {
        aSettings.rcpiRoamAttemptsPerInterval = temp;
        }       
    
    // Read KWlanRcpiRoamNextIntervalFactor
    err = repository->Get( KWlanRcpiRoamNextIntervalFactor, temp );
    if( err == KErrNone ) 
        {
        aSettings.rcpiRoamNextIntervalFactor = temp;
        }       
    
    // Read KWlanRcpiRoamNextIntervalAddition
    err = repository->Get( KWlanRcpiRoamNextIntervalAddition, temp );
    if( err == KErrNone ) 
        {
        aSettings.rcpiRoamNextIntervalAddition = temp;
        }       
    
    // Read KWlanScanListExpirationTime
    err = repository->Get( KWlanScanListExpirationTime, temp );
    if( err == KErrNone ) 
        {
        aSettings.scanListExpirationTime = temp;
        }       
    
    // Read KWlanQoSNullFrameEntryTxCount
    err = repository->Get( KWlanQoSNullFrameEntryTxCount, temp );
    if( err == KErrNone ) 
        {
        aSettings.qosNullFrameEntryTxCount = temp;
        }       
    
    // Read KWlanSpRcpiTarget
    err = repository->Get( KWlanSpRcpiTarget, temp );
    if( err == KErrNone ) 
        {
        aSettings.spRcpiTarget = temp;
        }       

    // Read KWlanSpTimeTarget
    err = repository->Get( KWlanSpTimeTarget, temp );
    if( err == KErrNone ) 
        {
        aSettings.spTimeTarget = temp;
        }       
    
    // Read KWlanSpMinIndicationInterval
    err = repository->Get( KWlanSpMinIndicationInterval, temp );
    if( err == KErrNone ) 
        {
        aSettings.spMinIndicationInterval = temp;
        }       
    
    // Read KWlanBssLostRoamMinInterval
    err = repository->Get( KWlanBssLostRoamMinInterval, temp );
    if( err == KErrNone ) 
        {
        aSettings.bssLostRoamMinInterval = temp;
        }       

    // Read KWlanBssLostRoamMaxInterval
    err = repository->Get( KWlanBssLostRoamMaxInterval, temp );
    if( err == KErrNone ) 
        {
        aSettings.bssLostRoamMaxInterval = temp;
        }       
    
    // Read KWlanBssLostRoamAttemptsPerInterval
    err = repository->Get( KWlanBssLostRoamAttemptsPerInterval, temp );
    if( err == KErrNone ) 
        {
        aSettings.bssLostRoamAttemptsPerInterval = temp;
        }       

    // Read KWlanBssLostRoamNextIntervalFactor
    err = repository->Get( KWlanBssLostRoamNextIntervalFactor, temp );
    if( err == KErrNone ) 
        {
        aSettings.bssLostRoamNextIntervalFactor = temp;
        }       
    
    // Read KWlanBssLostRoamNextIntervalAddition
    err = repository->Get( KWlanBssLostRoamNextIntervalAddition, temp );
    if( err == KErrNone ) 
        {
        aSettings.bssLostRoamNextIntervalAddition = temp;
        }       
    
    // Read KWlanBssLostRoamMaxTriesToFindNw
    err = repository->Get( KWlanBssLostRoamMaxTriesToFindNw, temp );
    if( err == KErrNone ) 
        {
        aSettings.bssLostRoamMaxTriesToFindNw = temp;
        }       
    
    // Read KWlanTrafficStreamCreationTimeout
    err = repository->Get( KWlanTrafficStreamCreationTimeout, temp );
    if( err == KErrNone ) 
        {
        aSettings.trafficStreamCreationTimeout = temp;
        }       
    
    // Read KWlanBeaconLostThreshold
    err = repository->Get( KWlanBeaconLostThreshold, temp );
    if( err == KErrNone ) 
        {
        aSettings.beaconLostThreshold = temp;
        }       
    
    // Read KWlanBtBeaconLostThreshold
    err = repository->Get( KWlanBtBeaconLostThreshold, temp );
    if( err == KErrNone ) 
        {
        aSettings.btBeaconLostThreshold = temp;
        }       

    // Read KWlanTxFailThreshold
    err = repository->Get( KWlanTxFailThreshold, temp );
    if( err == KErrNone ) 
        {
        aSettings.txFailThreshold = temp;
        }       

    // Read KWlanBtTxFailThreshold
    err = repository->Get( KWlanBtTxFailThreshold, temp );
    if( err == KErrNone ) 
        {
        aSettings.btTxFailThreshold = temp;
        }       

    // Read KWlanPowerSaveDelay
    err = repository->Get( KWlanPowerSaveDelay, temp );
    if( err == KErrNone ) 
        {
        aSettings.powerSaveDelay = temp;
        }

    // Read KWlanRegionExpirationTime
    err = repository->Get( KWlanRegionExpirationTime, temp );
    if( err == KErrNone ) 
        {
        aSettings.regionExpirationTime = temp;
        }

    // Read KWlanRrmMinMeasurementInterval
    err = repository->Get( KWlanRrmMinMeasurementInterval, temp );
    if( err == KErrNone ) 
        {
        aSettings.rrmMinMeasurementInterval = temp;
        }  

    // Read KWlanPsmSrvMode
    err = repository->Get( KWlanPsmSrvMode, temp );
    if( err == KErrNone ) 
        {
        aSettings.psmServerMode = temp;
        }
    
    // Read KWlanBgScanPeakPeriodStart
    err = repository->Get( KWlanBgScanPeakPeriodStart, temp );
    if( err == KErrNone ) 
        {
        aSettings.bgScanPeakPeriodStart = temp;
        }
    
    // Read KWlanBgScanPeakPeriodEnd
    err = repository->Get( KWlanBgScanPeakPeriodEnd, temp );
    if( err == KErrNone ) 
        {
        aSettings.bgScanPeakPeriodEnd = temp;
        }
    
    // Read KWlanBgScanIntervalPeakPeriod
    err = repository->Get( KWlanBgScanIntervalPeakPeriod, temp );
    if( err == KErrNone ) 
        {
        aSettings.bgScanIntervalPeak = temp;
        }

    // Read KWlanBgScanIntervalOffPeakPeriod
    err = repository->Get( KWlanBgScanIntervalOffPeakPeriod, temp );
    if( err == KErrNone ) 
        {
        aSettings.bgScanIntervalOffPeak = temp;
        }
		
    // Read KWlanAutomaticTrafficStreamMgmt
    err = repository->Get( KWlanAutomaticTrafficStreamMgmt, temp );
    if( err == KErrNone ) 
        {
        aSettings.automaticTrafficStreamMgmt = static_cast<TBool>( temp );
        }  

    // Read KWlanMinActiveBroadcastChannelTime
    err = repository->Get( KWlanMinActiveBroadcastChannelTime, temp );
    if( err == KErrNone ) 
        {
        aSettings.minActiveBroadcastChannelTime = temp;
        }  

    // Read KWlanMaxActiveBroadcastChannelTime
    err = repository->Get( KWlanMaxActiveBroadcastChannelTime, temp );
    if( err == KErrNone ) 
        {
        aSettings.maxActiveBroadcastChannelTime = temp;
        }  

    // Read KWlanEnabledFeatures
    err = repository->Get( KWlanEnabledFeatures, temp );
    if( err == KErrNone ) 
        {
        aSettings.enabledFeatures = temp;
        }  

    // Read KWlanRegion
    err = repository->Get( KWlanRegion, temp );
    if( err == KErrNone ) 
        {
        aSettings.region = temp;
        }                                

    // Read KWlanRegionTimestamp
    err = repository->Get( KWlanRegionTimestamp, temp );
    if( err == KErrNone ) 
        {
        aSettings.regionTimestamp = temp;
        } 

    // Cleanup
    delete repository;
    DEBUG( "CWlanDeviceSettings::ReadPrivateData() - done" );
    return KErrNone;
    }

// ---------------------------------------------------------
// CWlanDeviceSettings::WriteCenRepKeyL
// ---------------------------------------------------------
//
EXPORT_C void CWlanDeviceSettings::WriteCenRepKeyL( const TUint32 aKey, const TInt aValue ) const
    {
    DEBUG2( "CWlanDeviceSettings::WriteCenRepKeyL( aKey: 0x%X, aValue: %d )", aKey, aValue );
    
    CRepository* repository = CRepository::NewL( KCRUidWlanDeviceSettingsRegistryId );

    TInt err = repository->Set( aKey, aValue );
    if( err != KErrNone ) 
        {
        DEBUG2( "CWlanDeviceSettings::WriteCenRepKeyL() - Could not set value %d to key 0x%X", aValue, aKey );
        }

    // Cleanup
    delete repository;
    
    User::LeaveIfError( err );
    }

// ---------------------------------------------------------
// CWlanDeviceSettings::WritePrivateData
// ---------------------------------------------------------
//
TInt CWlanDeviceSettings::WritePrivateData( const SWlanDeviceSettings& aSettings )
    {
    DEBUG( "CWlanDeviceSettings::WritePrivateData()" );
    
    TInt err = KErrNone;
    CRepository* repository = NULL;
    TRAP( err, repository = CRepository::NewL( KCRUidWlanDeviceSettingsRegistryId ) );
    if( err != KErrNone )
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - Could not access repository (%d), not saving", err );
        return err;
        }
    // No need to use CleanupStack because no possibility to leave
    
    // Write KWlanBeacon
    err = repository->Set(
        KWlanBeacon, 
        static_cast<TInt>( aSettings.beacon ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanBeacon );
        }

    // Write KWlanScanRate
    err = repository->Set( 
        KWlanScanRate, 
        static_cast<TInt>( aSettings.scanRate ) );
    if( err ) 
        { 
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanScanRate ); 
        }

    // Write KWlanRcpiTrigger
    err = repository->Set( 
        KWlanRcpiTrigger, 
        static_cast<TInt>( aSettings.rcpiTrigger ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanRcpiTrigger );
    	}

    // Write KWlanMinActiveChannelTime
    err = repository->Set( 
        KWlanMinActiveChannelTime, 
        static_cast<TInt>( aSettings.minActiveChannelTime ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanMinActiveChannelTime );
        }

    // Write KWlanMaxActiveChannelTime
    err = repository->Set( 
        KWlanMaxActiveChannelTime, 
        static_cast<TInt>( aSettings.maxActiveChannelTime ) );
    if( err ) 
        { 
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanMaxActiveChannelTime );
        }

    // Write KWlanMaxTxMSDULifeTime
    err = repository->Set( 
        KWlanMaxTxMSDULifeTime, 
        static_cast<TInt>( aSettings.maxTxMSDULifeTime ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanMaxTxMSDULifeTime );
        }

    // Write KWlanScanExpirationTimer
    err = repository->Set( 
        KWlanScanExpirationTimer, 
        static_cast<TInt>( aSettings.scanExpirationTimer ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanScanExpirationTimer );
        }

    // Write KWlanUnloadDriverTimer
    err = repository->Set( 
        KWlanUnloadDriverTimer, 
        static_cast<TInt>( aSettings.unloadDriverTimer ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanUnloadDriverTimer );
        }

    // Write KWlanRoamTimer
    err = repository->Set( 
        KWlanRoamTimer, 
        static_cast<TInt>( aSettings.roamTimer ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanRoamTimer );
        }

    // Write KWlanRcpiDifference
    err = repository->Set( 
        KWlanRcpiDifference, 
        static_cast<TInt>( aSettings.rcpiDifference ) );
    if( err ) 
        { 
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanRcpiDifference );
        }

    // Write KWlanConnRegainTimer
    err = repository->Set( 
        KWlanConnRegainTimer, 
        static_cast<TInt>( aSettings.connRegainTimer ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanConnRegainTimer );
        }

    // Write KWlanMaxTriesToFindNw
    err = repository->Set( 
        KWlanMaxTriesToFindNw, 
        static_cast<TInt>( aSettings.maxTriesToFindNw ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanMaxTriesToFindNw );
        }

    // Write KWlanDelayBetweenFindNw
    err = repository->Set( 
        KWlanDelayBetweenFindNw, 
        static_cast<TInt>( aSettings.delayBetweenFindNw ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanDelayBetweenFindNw );
        }

    // Write KWlanMinPassiveChannelTime
    err = repository->Set( 
        KWlanMinPassiveChannelTime, 
        static_cast<TInt>( aSettings.minPassiveChannelTime ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanMinPassiveChannelTime );
        }

    // Write KWlanMaxPassiveChannelTime
    err = repository->Set( 
        KWlanMaxPassiveChannelTime, 
        static_cast<TInt>( aSettings.maxPassiveChannelTime ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanMaxPassiveChannelTime );
        }

    // Write KWlanMaxApFailureCount
    err = repository->Set( 
        KWlanMaxApFailureCount, 
        static_cast<TInt>( aSettings.maxApFailureCount ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanMaxApFailureCount );
        }

    // Write KWlanLongBeaconFindCount
    err = repository->Set( 
        KWlanLongBeaconFindCount, 
        static_cast<TInt>( aSettings.longBeaconFindCount ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanLongBeaconFindCount );
        }

    // Write KWlanQosNullFrameInterval
    err = repository->Set( 
        KWlanQosNullFrameInterval, 
        static_cast<TInt>( aSettings.qosNullFrameInterval ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanQosNullFrameInterval );
        }

    // Write KWlanQosNullFrameTimeout
    err = repository->Set( 
        KWlanQosNullFrameTimeout, 
        static_cast<TInt>( aSettings.qosNullFrameTimeout ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanQosNullFrameTimeout );
        }

    // Write KWlanKeepAliveInterval
    err = repository->Set( 
    	KWlanKeepAliveInterval, 
        static_cast<TInt>( aSettings.keepAliveInterval ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanKeepAliveInterval );
        }

    // Write KWlanScanStopRcpiThreshold
    err = repository->Set( 
    	KWlanScanStopRcpiThreshold, 
        static_cast<TInt>( aSettings.scanStopRcpiThreshold ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanScanStopRcpiThreshold );
        }

    // Write KWlanMinRcpiForIapAvailability
    err = repository->Set( 
    	KWlanMinRcpiForIapAvailability, 
        static_cast<TInt>( aSettings.minRcpiForIapAvailability ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanMinRcpiForIapAvailability );
        }

    // Write KWlanQoSNullFrameEntryTimeout
    err = repository->Set( 
    	KWlanQoSNullFrameEntryTimeout, 
        static_cast<TInt>( aSettings.qosNullFrameEntryTimeout ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanQoSNullFrameEntryTimeout );
        }

    // Write KWlanMaxApDeauthenticationCount
    err = repository->Set( 
    	KWlanMaxApDeauthenticationCount, 
        static_cast<TInt>( aSettings.maxApDeauthenticationCount ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanMaxApDeauthenticationCount );
        }

    // Write KWlanApDeauthenticationTimeout
    err = repository->Set( 
    	KWlanApDeauthenticationTimeout, 
        static_cast<TInt>( aSettings.apDeauthenticationTimeout ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanApDeauthenticationTimeout );
        }

    // Write KWlanMaxDtimSkipInterval
    err = repository->Set( 
        KWlanMaxDtimSkipInterval, 
        static_cast<TInt>( aSettings.maxDtimSkipInterval ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanMaxDtimSkipInterval );
        }    

    // Write KWlanPsActiveToLightTimeout
    err = repository->Set( 
        KWlanPsActiveToLightTimeout, 
        static_cast<TInt>( aSettings.psActiveToLightTimeout ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanPsActiveToLightTimeout );
        }   

    // Write KWlanPsActiveToLightThreshold
    err = repository->Set( 
        KWlanPsActiveToLightThreshold, 
        static_cast<TInt>( aSettings.psActiveToLightThreshold ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanPsActiveToLightThreshold );
        }   

    // Write KWlanPsLightToActiveTimeout
    err = repository->Set( 
        KWlanPsLightToActiveTimeout, 
        static_cast<TInt>( aSettings.psLightToActiveTimeout ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanPsLightToActiveTimeout );
        }   

    // Write KWlanPsLightToActiveThreshold
    err = repository->Set( 
        KWlanPsLightToActiveThreshold, 
        static_cast<TInt>( aSettings.psLightToActiveThreshold ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanPsLightToActiveThreshold );
        }   

    // Write KWlanPsLightToDeepTimeout
    err = repository->Set( 
        KWlanPsLightToDeepTimeout, 
        static_cast<TInt>( aSettings.psLightToDeepTimeout ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanPsLightToDeepTimeout );
        }   

    // Write KWlanPsLightToDeepThreshold
    err = repository->Set( 
        KWlanPsLightToDeepThreshold, 
        static_cast<TInt>( aSettings.psLightToDeepThreshold ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanPsLightToDeepThreshold );
        }       

    // Write KWlanPsUapsdRxFrameLengthThreshold
    err = repository->Set( 
        KWlanPsUapsdRxFrameLengthThreshold, 
        static_cast<TInt>( aSettings.psUapsdRxThreshold ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanPsUapsdRxFrameLengthThreshold );
        }

    // Write KWlanRcpiRoamMinInterval
    err = repository->Set( 
        KWlanRcpiRoamMinInterval, 
        static_cast<TInt>( aSettings.rcpiRoamMinInterval ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanRcpiRoamMinInterval );
        }

    // Write KWlanRcpiRoamMaxInterval
    err = repository->Set( 
        KWlanRcpiRoamMaxInterval, 
        static_cast<TInt>( aSettings.rcpiRoamMaxInterval ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanRcpiRoamMaxInterval );
        }
    
    // Write KWlanRcpiRoamAttemptsPerInterval
    err = repository->Set( 
        KWlanRcpiRoamAttemptsPerInterval, 
        static_cast<TInt>( aSettings.rcpiRoamAttemptsPerInterval ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanRcpiRoamAttemptsPerInterval );
        }
    
    // Write KWlanRcpiRoamNextIntervalFactor
    err = repository->Set( 
        KWlanRcpiRoamNextIntervalFactor, 
        static_cast<TInt>( aSettings.rcpiRoamNextIntervalFactor ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanRcpiRoamNextIntervalFactor );
        }
    
    // Write KWlanRcpiRoamNextIntervalAddition
    err = repository->Set( 
        KWlanRcpiRoamNextIntervalAddition, 
        static_cast<TInt>( aSettings.rcpiRoamNextIntervalAddition ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanRcpiRoamNextIntervalAddition );
        }
    
    // Write KWlanScanListExpirationTime
    err = repository->Set( 
        KWlanScanListExpirationTime, 
        static_cast<TInt>( aSettings.scanListExpirationTime ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanScanListExpirationTime );
        }
    
    // Write KWlanQoSNullFrameEntryTxCount
    err = repository->Set( 
        KWlanQoSNullFrameEntryTxCount, 
        static_cast<TInt>( aSettings.qosNullFrameEntryTxCount ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanQoSNullFrameEntryTxCount );
        }
    
    // Write KWlanSpRcpiTarget
    err = repository->Set( 
        KWlanSpRcpiTarget, 
        static_cast<TInt>( aSettings.spRcpiTarget ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanSpRcpiTarget );
        }
    
    // Write KWlanSpTimeTarget
    err = repository->Set( 
        KWlanSpTimeTarget, 
        static_cast<TInt>( aSettings.spTimeTarget ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanSpTimeTarget );
        }

    // Write KWlanSpMinIndicationInterval
    err = repository->Set( 
        KWlanSpMinIndicationInterval, 
        static_cast<TInt>( aSettings.spMinIndicationInterval ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanSpMinIndicationInterval );
        }
    
    // Write KWlanBssLostRoamMinInterval
    err = repository->Set( 
        KWlanBssLostRoamMinInterval, 
        static_cast<TInt>( aSettings.bssLostRoamMinInterval ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanBssLostRoamMinInterval );
        }
    
    // Write KWlanBssLostRoamMaxInterval
    err = repository->Set( 
        KWlanBssLostRoamMaxInterval, 
        static_cast<TInt>( aSettings.bssLostRoamMaxInterval ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanBssLostRoamMaxInterval );
        }
    
    // Write KWlanBssLostRoamAttemptsPerInterval
    err = repository->Set( 
        KWlanBssLostRoamAttemptsPerInterval, 
        static_cast<TInt>( aSettings.bssLostRoamAttemptsPerInterval ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanBssLostRoamAttemptsPerInterval );
        }
    
    // Write KWlanBssLostRoamNextIntervalFactor
    err = repository->Set( 
        KWlanBssLostRoamNextIntervalFactor, 
        static_cast<TInt>( aSettings.bssLostRoamNextIntervalFactor ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanBssLostRoamNextIntervalFactor );
        }
    
    // Write KWlanBssLostRoamNextIntervalAddition
    err = repository->Set( 
        KWlanBssLostRoamNextIntervalAddition, 
        static_cast<TInt>( aSettings.bssLostRoamNextIntervalAddition ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanBssLostRoamNextIntervalAddition );
        }
    
    // Write KWlanBssLostRoamMaxTriesToFindNw
    err = repository->Set( 
        KWlanBssLostRoamMaxTriesToFindNw, 
        static_cast<TInt>( aSettings.bssLostRoamMaxTriesToFindNw ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanBssLostRoamMaxTriesToFindNw );
        }
    
    // Write KWlanTrafficStreamCreationTimeout
    err = repository->Set( 
        KWlanTrafficStreamCreationTimeout, 
        static_cast<TInt>( aSettings.trafficStreamCreationTimeout ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanTrafficStreamCreationTimeout );
        }
    
    // Write KWlanBeaconLostThreshold
    err = repository->Set( 
        KWlanBeaconLostThreshold, 
        static_cast<TInt>( aSettings.beaconLostThreshold ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanBeaconLostThreshold );
        }

    // Write KWlanBtBeaconLostThreshold
    err = repository->Set( 
        KWlanBtBeaconLostThreshold, 
        static_cast<TInt>( aSettings.btBeaconLostThreshold ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanBtBeaconLostThreshold );
        }
    
    // Write KWlanTxFailThreshold
    err = repository->Set( 
        KWlanTxFailThreshold, 
        static_cast<TInt>( aSettings.txFailThreshold ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanTxFailThreshold );
        }
    
    // Write KWlanBtTxFailThreshold
    err = repository->Set( 
        KWlanBtTxFailThreshold, 
        static_cast<TInt>( aSettings.btTxFailThreshold ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanBtTxFailThreshold );
        }

    // Write KWlanPowerSaveDelay
    err = repository->Set( 
        KWlanPowerSaveDelay, 
        static_cast<TInt>( aSettings.powerSaveDelay ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanPowerSaveDelay );
        }

    // Write KWlanRrmMinMeasurementInterval
    err = repository->Set( 
        KWlanRrmMinMeasurementInterval, 
        static_cast<TInt>( aSettings.rrmMinMeasurementInterval ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanRrmMinMeasurementInterval );
        }

    // Write KWlanPsmSrvMode
    err = repository->Set( 
        KWlanPsmSrvMode, 
        static_cast<TInt>( aSettings.psmServerMode ) );
    if( err )
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanPsmSrvMode );
        }
    
    // Write KWlanBgScanPeakPeriodStart
    err = repository->Set( 
        KWlanBgScanPeakPeriodStart, 
        static_cast<TInt>( aSettings.bgScanPeakPeriodStart ) );
    if( err )
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanBgScanPeakPeriodStart );
        }
    
    // Write KWlanBgScanPeakPeriodEnd
    err = repository->Set( 
        KWlanBgScanPeakPeriodEnd, 
        static_cast<TInt>( aSettings.bgScanPeakPeriodEnd ) );
    if( err )
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanBgScanPeakPeriodEnd );
        }
    
    // Write KWlanBgScanIntervalPeakPeriod
    err = repository->Set( 
            KWlanBgScanIntervalPeakPeriod, 
        static_cast<TInt>( aSettings.bgScanIntervalPeak ) );
    if( err )
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanBgScanIntervalPeakPeriod );
        }
    
    // Write KWlanBgScanIntervalOffPeakPeriod
    err = repository->Set( 
            KWlanBgScanIntervalOffPeakPeriod, 
        static_cast<TInt>( aSettings.bgScanIntervalOffPeak ) );
    if( err )
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanBgScanIntervalOffPeakPeriod );
        }

    // Write KWlanAutomaticTrafficStreamMgmt
    err = repository->Set( 
        KWlanAutomaticTrafficStreamMgmt, 
        static_cast<TInt>( aSettings.automaticTrafficStreamMgmt ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanAutomaticTrafficStreamMgmt );
        }

    // Write KWlanMinActiveBroadcastChannelTime
    err = repository->Set( 
        KWlanMinActiveBroadcastChannelTime, 
        static_cast<TInt>( aSettings.minActiveBroadcastChannelTime ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanMinActiveBroadcastChannelTime );
        }

    // Write KWlanMaxActiveBroadcastChannelTime
    err = repository->Set( 
        KWlanMaxActiveBroadcastChannelTime, 
        static_cast<TInt>( aSettings.maxActiveBroadcastChannelTime ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanMaxActiveBroadcastChannelTime );
        }

    // Write KWlanEnabledFeatures
    err = repository->Set( 
        KWlanEnabledFeatures, 
        static_cast<TInt>( aSettings.enabledFeatures ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanEnabledFeatures );
        }

    // Write KWlanRegion
    err = repository->Set( 
    	KWlanRegion, 
        static_cast<TInt>( aSettings.region ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanRegion );
        }

    // Write KWlanRegionTimestamp
    err = repository->Set( 
    	KWlanRegionTimestamp, 
        static_cast<TInt>( aSettings.regionTimestamp ) );
    if( err ) 
        {
        DEBUG1( "CWlanDeviceSettings::WritePrivateData() - could not set key 0x%X", KWlanRegionTimestamp );
        }
		
    // Cleanup
    delete repository;
    
    DEBUG( "CWlanDeviceSettings::WritePrivateData() - done" );
    return KErrNone;        
    }

// ---------------------------------------------------------
// CWlanDeviceSettings::CheckWlanDeviceSettingsTableL
// ---------------------------------------------------------
//
void CWlanDeviceSettings::CheckWlanDeviceSettingsTableL()
    {
    DEBUG( "CWlanDeviceSettings::CheckWlanDeviceSettingsTableL()" );

    User::LeaveIfNull( iDb );

    CCommsDbTableView* view;

    // Open view to table where version field matches the current value   
    view = iDb->OpenViewMatchingUintLC(
        KWlanDeviceSettings(),
        KTableVersion,
        KWlanDeviceSettingsTableVersion );

    // Check if there's at least one row in the view
    User::LeaveIfError( view->GotoFirstRecord() );

    CleanupStack::PopAndDestroy(view);
    }
    
// ---------------------------------------------------------
// CWlanDeviceSettings::OpenTableL
// ---------------------------------------------------------
//
void CWlanDeviceSettings::OpenTableL()
    {
    DEBUG( "CWlanDeviceSettings::OpenTableL()" );
    
    User::LeaveIfNull( iDb );

    // Open default settings.
    iDefTable = iDb->OpenViewMatchingUintLC( KWlanDeviceSettings,
                                             KWlanDeviceSettingsType,
                                             KWlanDefaultSettings );

    CleanupStack::Pop(iDefTable);

    if ( iDefTable->GotoFirstRecord() != KErrNone )
        {
        NewRecordL( KWlanDefaultSettings );
        User::LeaveIfError( iDefTable->GotoFirstRecord() );
        }

    // Open user settings.
    iUsrTable = iDb->OpenViewMatchingUintLC( KWlanDeviceSettings,
                                             KWlanDeviceSettingsType,
                                             KWlanUserSettings );

    CleanupStack::Pop(iUsrTable);

    if ( iUsrTable->GotoFirstRecord() != KErrNone )
        {
        NewRecordL( KWlanUserSettings );
        User::LeaveIfError( iUsrTable->GotoFirstRecord() );
        }
    }

// ---------------------------------------------------------
// CWlanDeviceSettings::NewRecordL
// ---------------------------------------------------------
//
void CWlanDeviceSettings::NewRecordL( TUint32 aTableType )
    {
    DEBUG1( "CWlanDeviceSettings::NewRecordL() Type=%d", aTableType );

    CCommsDbTableView* table;
    if ( aTableType == KWlanDefaultSettings )
        {
        table = iDefTable;
        }
    else
        {
        table = iUsrTable;
        }

    TUint32 id;
    User::LeaveIfError( table->InsertRecord( id ) );
    
    SWlanDeviceSettings settings;
    GetDefaultSettings( settings );
    
    table->WriteUintL( KWlanDeviceSettingsType, aTableType );    
    table->WriteUintL( KTableVersion, KWlanDeviceSettingsTableVersion );
    table->WriteUintL( KBgScanInterval, settings.backgroundScanInterval );
    table->WriteBoolL( KUseDefaultSettings, settings.useDefaultSettings );
    table->WriteUintL( KWlanLongRetry, settings.longRetry );
    table->WriteUintL( KWlanShortRetry, settings.shortRetry );
    table->WriteUintL( KWlanRTSThreshold, settings.rts );
    table->WriteUintL( KWlanTxPowerLevel, settings.txPowerLevel );
    table->WriteBoolL( KWlanAllowRadioMeasurements, settings.allowRadioMeasurements );
    table->WriteUintL( KWlanPowerMode, settings.powerMode );
    
    User::LeaveIfError( table->PutRecordChanges() );
    }

#ifdef _DEBUG
// ---------------------------------------------------------
// CWlanDeviceSettings::LogSettings
// ---------------------------------------------------------
//
void CWlanDeviceSettings::LogSettings( const SWlanDeviceSettings& aSettings ) const
    {
    DEBUG1( "CWlanDeviceSettings::LogSettings() - beacon == %u",
        aSettings.beacon );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - longRetry == %u",
        aSettings.longRetry );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - rts == %u",
        aSettings.rts );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - shortRetry == %u",
        aSettings.shortRetry );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - backgroundScanInterval == %u",
        aSettings.backgroundScanInterval );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - txPowerLevel == %u",
        aSettings.txPowerLevel );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - scanRate == %u",
        static_cast<TUint32>( aSettings.scanRate ) );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - rpciTrigger == %u",
        aSettings.rcpiTrigger );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - minActiveChannelTime == %u",
        aSettings.minActiveChannelTime );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - maxActiveChannelTime == %u",
        aSettings.maxActiveChannelTime );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - maxTxMSDULifeTime == %u",
        aSettings.maxTxMSDULifeTime );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - useDefaultSettings == %u",
        aSettings.useDefaultSettings );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - scanExpirationTimer == %u",
        aSettings.scanExpirationTimer );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - unloadDriverTimer == %u",
        aSettings.unloadDriverTimer );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - roamTimer == %u",
        aSettings.roamTimer );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - rcpiDifference == %u",
        aSettings.rcpiDifference );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - connRegainTimer == %u",
        aSettings.connRegainTimer );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - maxTriesToFindNw == %u",
        aSettings.maxTriesToFindNw );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - delayBetweenFindNw == %u",
        aSettings.delayBetweenFindNw );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - wlanPowerMode == %u",
        aSettings.powerMode );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - allowRadioMeasurements == %u",
        aSettings.allowRadioMeasurements );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - minPassiveChannelTime == %u",
        aSettings.minPassiveChannelTime );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - maxPassiveChannelTime == %u",
        aSettings.maxPassiveChannelTime );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - maxApFailureCount == %u",
        aSettings.maxApFailureCount );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - maxApAuthFailureCount == %u",
        aSettings.maxApAuthFailureCount );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - longBeaconFindCount == %u",
        aSettings.longBeaconFindCount );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - qosNullFrameInterval == %u",
        aSettings.qosNullFrameInterval );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - qosNullFrameTimeout == %u",
        aSettings.qosNullFrameTimeout );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - keepAliveInterval == %u",
        aSettings.keepAliveInterval );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - scanStopRcpiThreshold == %u",
        aSettings.scanStopRcpiThreshold );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - minRcpiForIapAvailability == %u",
        aSettings.minRcpiForIapAvailability );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - qosNullFrameEntryTimeout == %u",
        aSettings.qosNullFrameEntryTimeout );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - maxApDeauthenticationCount == %u",
        aSettings.maxApDeauthenticationCount );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - apDeauthenticationTimeout == %u",
        aSettings.apDeauthenticationTimeout );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - maxDtimSkipInterval == %u",
        aSettings.maxDtimSkipInterval );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - psActiveToLightTimeout == %u",
        aSettings.psActiveToLightTimeout );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - psActiveToLightThreshold == %u",
        aSettings.psActiveToLightThreshold );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - psLightToActiveTimeout == %u",
        aSettings.psLightToActiveTimeout );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - psLightToActiveThreshold == %u",
        aSettings.psLightToActiveThreshold );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - psLightToDeepTimeout == %u",
        aSettings.psLightToDeepTimeout );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - psLightToDeepThreshold == %u",
        aSettings.psLightToDeepThreshold );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - psUapsdRxThreshold == %u",
        aSettings.psUapsdRxThreshold );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - rcpiRoamMinInterval == %u",
        aSettings.rcpiRoamMinInterval );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - rcpiRoamMaxInterval == %u",
        aSettings.rcpiRoamMaxInterval );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - rcpiRoamAttemptsPerInterval == %u",
        aSettings.rcpiRoamAttemptsPerInterval );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - rcpiRoamNextIntervalFactor == %u",
        aSettings.rcpiRoamNextIntervalFactor );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - rcpiRoamNextIntervalAddition == %u",
        aSettings.rcpiRoamNextIntervalAddition );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - scanListExpirationTime == %u",
        aSettings.scanListExpirationTime );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - qosNullFrameEntryTxCount == %u",
        aSettings.qosNullFrameEntryTxCount );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - spRcpiTarget == %u",
        aSettings.spRcpiTarget );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - spTimeTarget == %u",
        aSettings.spTimeTarget );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - spMinIndicationInterval == %u",
        aSettings.spMinIndicationInterval );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - bssLostRoamMinInterval == %u",
        aSettings.bssLostRoamMinInterval );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - bssLostRoamMaxInterval == %u",
        aSettings.bssLostRoamMaxInterval );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - bssLostRoamAttemptsPerInterval == %u",
        aSettings.bssLostRoamAttemptsPerInterval );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - bssLostRoamNextIntervalFactor == %u",
        aSettings.bssLostRoamNextIntervalFactor );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - bssLostRoamNextIntervalAddition == %u",
        aSettings.bssLostRoamNextIntervalAddition );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - bssLostRoamMaxTriesToFindNw == %u",
        aSettings.bssLostRoamMaxTriesToFindNw );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - trafficStreamCreationTimeout == %u",
        aSettings.trafficStreamCreationTimeout );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - beaconLostThreshold == %u",
        aSettings.beaconLostThreshold );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - btBeaconLostThreshold == %u",
        aSettings.btBeaconLostThreshold );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - txFailThreshold == %u",
        aSettings.txFailThreshold );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - btTxFailThreshold == %u",
        aSettings.btTxFailThreshold );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - powerSaveDelay == %u",
        aSettings.powerSaveDelay );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - rrmMinMeasurementInterval == %u",
        aSettings.rrmMinMeasurementInterval );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - psmServerMode == %d",
        aSettings.psmServerMode );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - bgScanPeakPeriodStart == %d",
        aSettings.bgScanPeakPeriodStart );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - bgScanPeakPeriodEnd == %d",
        aSettings.bgScanPeakPeriodEnd );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - bgScanIntervalPeak == %d",
        aSettings.bgScanIntervalPeak );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - bgScanIntervalOffPeak == %d",
        aSettings.bgScanIntervalOffPeak );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - automaticTrafficStreamMgmt == %u",
        static_cast<TUint32>( aSettings.automaticTrafficStreamMgmt ) );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - minActiveBroadcastChannelTime == %d",
        aSettings.minActiveBroadcastChannelTime );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - maxActiveBroadcastChannelTime == %d",
        aSettings.maxActiveBroadcastChannelTime );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - enabledFeatures == %d",
        aSettings.enabledFeatures );    
    DEBUG1( "CWlanDeviceSettings::LogSettings() - region == %u",
        aSettings.region );
    DEBUG1( "CWlanDeviceSettings::LogSettings() - regionTimestamp == %d",
        aSettings.regionTimestamp );
    }
#endif // _DEBUG

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File
