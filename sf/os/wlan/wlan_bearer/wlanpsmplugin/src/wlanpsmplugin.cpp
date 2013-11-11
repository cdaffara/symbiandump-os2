/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ECOM plugin implementation
*
*/

/*
* %version: 7 %
*/

// INCLUDE FILES
#include <e32std.h>
#include <ecom/implementationproxy.h>
#include <commdb.h>
#include <psmsettingsprovider.h>
#include "wdbifwlandefs.h"
#include <wlanmgmtclient.h>
#include "wlanpsmplugin.h"
#include "am_debug.h"

// CONSTANT DEFINITIONS
const TUint32 KWlanPsmConfigId = 0x200100C0;

enum TPsmPluginFirstKeys
    {
    EWlanPsmKey1 = 1    
    };

/**
* Pairs ECom implementation UIDs with a pointer to the instantiation 
* method for that implementation. Required for all ECom implementation
* collections.
*/
const TImplementationProxy ImplementationTable[] =
    {
        {{0x200100C2}, reinterpret_cast<TProxyNewLPtr>(CWlanPsmPlugin::NewL)}
    };

// ========================== EXPORTED FUNCTIONS =========================

// ---------------------------------------------------------
// Returns an instance of the proxy table.
// Returns: KErrNone
// ---------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

// ============================ MEMBER FUNCTIONS ===============================

//
// ----------------------------------------------------------------------------------
// CWlanPsmPlugin::CWlanPsmPlugin()
// ----------------------------------------------------------------------------------
//  
CWlanPsmPlugin::CWlanPsmPlugin( TPsmPluginCTorParams& aInitParams ) :
    CPsmPluginBase( aInitParams )
	{	
	DEBUG ( "CWlanPsmPlugin::CWlanPsmPlugin()" );
	}

// -----------------------------------------------------------------------------
// CWlanPsmPlugin::ConstructL(
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWlanPsmPlugin::ConstructL()
    {
    DEBUG ( "CWlanPsmPlugin::ConstructL()" );
    User::LeaveIfError( iWlmServer.Connect() );
    }
    
//
// ----------------------------------------------------------------------------------
// CWlanPsmPlugin::NewL() 
// ----------------------------------------------------------------------------------
//  
// Two-phased constructor.
CWlanPsmPlugin* CWlanPsmPlugin::NewL( TPsmPluginCTorParams& aInitParams )
	{
	DEBUG ( "CWlanPsmPlugin::NewL()" );
	
	CWlanPsmPlugin* self = new ( ELeave ) CWlanPsmPlugin( aInitParams );

	CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
	}
	
//
// ----------------------------------------------------------------------------------
// CWlanPsmPlugin::~CWlanPsmPlugin()
// ----------------------------------------------------------------------------------
//  
// Destructor.
CWlanPsmPlugin::~CWlanPsmPlugin()
	{
    DEBUG ( "CWlanPsmPlugin::~CWlanPsmPlugin()" );
    iWlmServer.Close();
	}

//
// ----------------------------------------------------------------------------------
// CWlanPsmPlugin::NotifyModeChange()
// ----------------------------------------------------------------------------------
//  
void CWlanPsmPlugin::NotifyModeChange( const TInt aMode )
    {
    DEBUG1( "CWlanPsmPlugin::NotifyModeChange(), Mode:%d", aMode );

    //call the function that can Leave
    TRAPD( err, NotifyModeChangeL(aMode) );
    if (err)
    	{
    	DEBUG("CWlanPsmPlugin::NotifyModeChange(), NotifyModeChangeL Leave'd");
    	}
    }


//
// ----------------------------------------------------------------------------------
// CWlanPsmPlugin::NotifyModeChangeL()
// ----------------------------------------------------------------------------------
//  
void CWlanPsmPlugin::NotifyModeChangeL( const TInt aMode )
    {
    
#ifdef _DEBUG
    if( aMode == EPsmsrvModePowerSave )
        {
        DEBUG1( "CWlanPsmPlugin::NotifyModeChangeL() - aMode: EPsmsrvModePowerSave (%d)", aMode );
        }
    else if ( aMode == EPsmsrvPartialMode )
        {
        DEBUG1( "CWlanPsmPlugin::NotifyModeChangeL() - aMode: EPsmsrvPartialMode (%d)", aMode );
        }
    else if ( aMode == EPsmsrvModeNormal )
        {
        DEBUG1( "CWlanPsmPlugin::NotifyModeChangeL() - aMode: EPsmsrvModeNormal (%d)", aMode );
        }
    else
        {
        DEBUG1( "CWlanPsmPlugin::NotifyModeChangeL() - unrecognised aMode: %d", aMode );
        }
#endif
    
    //NOTE: The plugin changes only the current BG scan interval in CommsDat.
    //Updating of Default BG scan interval in CenRep is taken care by PSM Engine
    //using Passive configuration file.
    
    RConfigInfoArray infoArray;
    TPsmsrvConfigInfo info1;
    TUint32 bgScanIntervalCurrentlyInCommsDat, bgScanIntervalFromPsmFramework;
    TBool updateBgScanIntervalToCommsDat = EFalse;       
    CCommsDatabase*    commDB = NULL;
    CCommsDbTableView* table  = NULL;

    //Read current BG interval from CommsDat
    commDB = CCommsDatabase::NewL();
    CleanupStack::PushL( commDB );

    table = commDB->OpenViewMatchingUintLC( TPtrC(WLAN_DEVICE_SETTINGS),
    										TPtrC(WLAN_DEVICE_SETTINGS_TYPE),
    										KWlanUserSettings );

    User::LeaveIfError( table->GotoFirstRecord() );
	
	table->ReadUintL( TPtrC( WLAN_BG_SCAN_INTERVAL ), bgScanIntervalCurrentlyInCommsDat ); 

    // Backup and Get settings...
    info1.iConfigId = EWlanPsmKey1;
    info1.iConfigType = EConfigTypeInt;
    info1.iIntValue = bgScanIntervalCurrentlyInCommsDat;
    infoArray.Append( info1 );

    iSettingsProvider.BackupAndGetSettingsL( infoArray, KWlanPsmConfigId );
    
    bgScanIntervalFromPsmFramework = infoArray[0].iIntValue;
    
    DEBUG2( "CWlanPsmPlugin::NotifyModeChangeL() - current value in CommsDat: %u, PSM Framework provided value: %u",
    					bgScanIntervalCurrentlyInCommsDat, bgScanIntervalFromPsmFramework);
    
    if ( EPsmsrvModePowerSave == aMode || EPsmsrvPartialMode == aMode )
    	{
        DEBUG( "CWlanPsmPlugin::NotifyModeChangeL() - PSM server mode is changing to EPsmsrvModePowerSave/EPsmsrvPartialMode" );
        
        // if bg scan interval is neither non-zero nor automatic...
        if ( bgScanIntervalCurrentlyInCommsDat != 0 && bgScanIntervalCurrentlyInCommsDat != 0xFFFFFFFF )
            {
            DEBUG1( "CWlanPsmPlugin::NotifyModeChangeL() - storing Automatic value to CommsDat as current CommsDat value (%u) is non-zero and non-Automatic",
                    bgScanIntervalCurrentlyInCommsDat );
            // set bg scan interval to automatic
            updateBgScanIntervalToCommsDat = ETrue;
            }
#ifdef _DEBUG
        else
            {
            DEBUG1( "CWlanPsmPlugin::NotifyModeChangeL() - CommsDat update not needed as current value (%u) is zero or Automatic", bgScanIntervalCurrentlyInCommsDat );
            }
#endif        
    	}
    else 
    	{
        DEBUG( "CWlanPsmPlugin::NotifyModeChangeL() - PSM server mode is changing to EPsmsrvModeNormal" );
        // If backed up value is...
        // * zero -> if current value in commsdat is automatic, user has set bg scan to automatic during power save, no need to change commsdat.
        //        -> if current value in commsdat is zero -> commsdat had setting zero before entering power save -> no need to change commsdat.
        //        -> UI shall prevent setting other values in commsdat during power save.
        // * automatic -> if current value in commsdat is automatic, no need to change commsdat.
        //             -> if current value in commsdat is zero, no need to change commsdat.
        //             -> UI shall prevent setting other values in commsdat during power save.
        // * non-zero -> if current value in commsdat is automatic, plugin set value to automatic during entering power save mode -> restore backed up setting to commsdat.
        //            -> if current value in commsdat is zero, user has disabled background scan -> no need to change commsdat.
        //            -> UI shall prevent setting other values in commsdat during power save.
        if( bgScanIntervalFromPsmFramework != 0 && bgScanIntervalFromPsmFramework != 0xFFFFFFFF )
            {
            DEBUG1( "CWlanPsmPlugin::NotifyModeChangeL() - backed up value (%u) is non-zero or non-Automatic", bgScanIntervalFromPsmFramework );
            if( bgScanIntervalCurrentlyInCommsDat == 0xFFFFFFFF )
                {
                DEBUG( "CWlanPsmPlugin::NotifyModeChangeL() - current value in CommsDat is Automatic -> restoring backed up value to CommsDat" );
                // restore background scan interval from backup
                updateBgScanIntervalToCommsDat = ETrue;
                }
#ifdef _DEBUG
            else
                {
                DEBUG( "CWlanPsmPlugin::NotifyModeChangeL() - no CommsDat update needed as current value in commsdat is zero" );
                if( bgScanIntervalCurrentlyInCommsDat != 0 )
                    {
                    DEBUG1( "CWlanPsmPlugin::NotifyModeChangeL() - value in CommsDat is non-zero (%u) -> do assert, UI shouldn't allow setting other than 0 or Automatic",
                            bgScanIntervalCurrentlyInCommsDat );
                    ASSERT( 0 );
                    }
                }
#endif        
            }
#ifdef _DEBUG
        else
            {
            DEBUG1( "CWlanPsmPlugin::NotifyModeChangeL() - no CommsDat update needed as backed up value (%u) is zero or Automatic", bgScanIntervalFromPsmFramework );
            }
#endif      
        }
           
    if ( updateBgScanIntervalToCommsDat )
    	{
        DEBUG1( "CWlanPsmPlugin::NotifyModeChangeL() - updating CommsDat with value: %d",
							bgScanIntervalFromPsmFramework);

		//update CommsDb to reflect the changed value
        User::LeaveIfError( table->UpdateRecord() );
		table->WriteUintL( TPtrC(WLAN_BG_SCAN_INTERVAL), bgScanIntervalFromPsmFramework );
		User::LeaveIfError( table->PutRecordChanges() );
		
		DEBUG( "CWlanPsmPlugin::NotifyModeChangeL() - informing WLAN Engine about changed value" );
		iWlmServer.NotifyChangedSettings();
    	}
    
    DEBUG( "CWlanPsmPlugin::NotifyModeChangeL() - notifying WLAN Engine about new power save mode" );
    iWlmServer.NotifyChangedPsmSrvMode( aMode );

    // cleanup
    CleanupStack::PopAndDestroy( table );
    CleanupStack::PopAndDestroy( commDB );
    
    infoArray.Reset();
    }

