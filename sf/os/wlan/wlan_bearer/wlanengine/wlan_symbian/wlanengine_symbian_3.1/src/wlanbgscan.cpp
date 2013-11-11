/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class implements WLAN background scan logic.
*
*/

/*
* %version: 18 %
*/

#include <e32base.h>
#include <e32const.h> 

#include "wlanscanproviderinterface.h"
#include "awsinterface.h"
#include "awsenginebase.h"
#include "wlanbgscanawscommsinterface.h"
#include "wlanbgscanstates.h"
#include "wlanbgscan.h"
#include "wlandevicesettings.h" // default values in case invalid data is passed in NotifyChangedSettings
#include "am_debug.h"

/**
 * One day to be added to TTime.
 */
const TInt KAddOneDay = 1;

/**
 * Get hours from CenRep variable.
 */
const TInt KGetHours = 100;

/**
 * Zero seconds.
 */
const TInt KZeroSeconds = 0;

/**
 * Last minute of a day.
 */
const TInt KWlanBgScanTwentyThreeFiftyNineOclock = 2359;

/**
 * Maximun interval for background scan.
 */
const TInt KWlanBgScanMaxInterval = 1800;

#ifdef _DEBUG
/**
 * Formatting of date time debug string.
 */
_LIT( KWlanBgScanDateTimeFormat, "%F %*E %*N %Y %H:%T:%S" );
_LIT( KWlanBgScanDateTimeFormat2, "%H%T" );

/**
 * Maximun length for date time debug string.
 */
const TInt KWlanBgScanMaxDateTimeStrLen = 50;
#endif


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWlanBgScan::CWlanBgScan
// ---------------------------------------------------------------------------
//
CWlanBgScan::CWlanBgScan( MWlanScanResultProvider& aProvider, MWlanTimerServices& aTimerServices ) :
    CWlanBgScanStates( *this, aProvider, aTimerServices ),
    iProvider ( aProvider ),
    iAwsComms( NULL ),
    iAwsOk( EFalse ),
    iCurrentPsmServerMode( 0 )
    {
    DEBUG( "CWlanBgScan::CWlanBgScan()" );
    }

// ---------------------------------------------------------------------------
// CWlanBgScan::ConstructL
// ---------------------------------------------------------------------------
//
void CWlanBgScan::ConstructL()
    {
    DEBUG( "CWlanBgScan::ConstructL()" );
    
    // create AWS comms interface
    TRAPD( err, iAwsComms = CWlanBgScanAwsCommsFactory::InstanceL( *this ) );
    if( err != KErrNone )
        {
        DEBUG1( "CWlanBgScan::ConstructL() - AWS comms creation failed with code %i", err );

        delete iAwsComms;
        iAwsComms = NULL;
        }
    else
        {
        DEBUG( "CWlanBgScan::ConstructL() - AWS comms creation successful" );
        }
    
    CWlanBgScanStates::ConstructL();
            
    DEBUG( "CWlanBgScan::ConstructL() - done" );    
    }

// ---------------------------------------------------------------------------
// CWlanBgScan::NewL
// ---------------------------------------------------------------------------
//
CWlanBgScan* CWlanBgScan::NewL( MWlanScanResultProvider& aProvider, MWlanTimerServices& aTimerServices )
    {
    DEBUG( "CWlanBgScan::NewL()" );
    CWlanBgScan* self = new ( ELeave ) CWlanBgScan( aProvider, aTimerServices );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }

// ---------------------------------------------------------------------------
// CWlanBgScan::~CWlanBgScan
// ---------------------------------------------------------------------------
//
CWlanBgScan::~CWlanBgScan()
    {
    DEBUG1( "CWlanBgScan::~CWlanBgScan() - deallocating iAwsComms @ 0x%08X", iAwsComms );
    delete iAwsComms;
    iAwsComms = NULL;
    }

// ---------------------------------------------------------------------------
// From class MWlanBgScanProvider.
// CWlanBgScan::ScanComplete
// ---------------------------------------------------------------------------
//
void CWlanBgScan::ScanComplete()
    {
    DEBUG1( "CWlanBgScan::ScanComplete() - current interval %us", GetBgInterval() );

    if ( GetBgInterval() != KWlanBgScanIntervalNever )
        {
        DEBUG1( "CWlanBgScan::ScanComplete() - issue a new request with %us as expiry", GetBgInterval() );
        iProvider.Scan( GetBgInterval() );
        }
    }

// ---------------------------------------------------------------------------
// From class MWlanBgScanProvider.
// CWlanBgScan::WlanSetBgScanState
// ---------------------------------------------------------------------------
//
void CWlanBgScan::SetBgScanState( const MWlanBgScanProvider::TWlanBgScanState &aState )
    {
    DEBUG2( "CWlanBgScan::SetBgScanState() - old state: %u, new state: %u", iWlanBgScanState, aState );
    
    // store state
    iWlanBgScanState = aState;

    RefreshUsedInterval();

    }

// ---------------------------------------------------------------------------
// From class MWlanBgScanProvider.
// CWlanBgScan::IsBgScanEnabled
// ---------------------------------------------------------------------------
//
TBool CWlanBgScan::IsBgScanEnabled()
    {
    // If ( interval != never )                              -> Return True
    // Otherwise                                             -> return False
    DEBUG1( "CWlanBgScan::IsBgScanEnabled() - returning %u", 
           ( GetBgInterval() != KWlanBgScanIntervalNever ) ? 1 : 0 );
        
    return ( GetBgInterval() != KWlanBgScanIntervalNever );
    }

// ---------------------------------------------------------------------------
// From class MWlanBgScanProvider.
// CWlanBgScan::NotifyChangedSettings
// ---------------------------------------------------------------------------
//
void CWlanBgScan::NotifyChangedSettings( MWlanBgScanProvider::TWlanBgScanSettings& aSettings )
    {
    DEBUG6( "CWlanBgScan::NotifyChangedSettings( interval: %u, psm srv mode: %u, peak start: %04u, peak end: %04u, peak: %u, off-peak: %u )",
            aSettings.backgroundScanInterval,
            aSettings.psmServerMode,
            aSettings.bgScanPeakStartTime,
            aSettings.bgScanPeakEndTime,
            aSettings.bgScanIntervalPeak,
            aSettings.bgScanIntervalOffPeak);
    
    // validate settings and use default values if needed
    MWlanBgScanProvider::TWlanBgScanSettings settingsToUse;
    CheckSettings( settingsToUse, aSettings ); // Note: Any PSM server mode value is allowed.
    
    // store settings for later use
    iBgScanSettings = settingsToUse;
    
    IntervalChanged();
    
    if( iCurrentPsmServerMode != iBgScanSettings.psmServerMode )
        {
        if( AwsPresence() == CWlanBgScan::EAwsPresent )
            {
            DEBUG( "CWlanBgScan::NotifyChangedSettings() - sending power mode command to AWS" );
            MWlanBgScanAwsComms::TAwsMessage msg = { MWlanBgScanAwsComms::ESetPowerSaveMode, iBgScanSettings.psmServerMode };
            iAwsComms->SendOrQueueAwsCommand( msg );
            }
        
        // remember current psm server mode
        iCurrentPsmServerMode = iBgScanSettings.psmServerMode;
        }
    
    }

// ---------------------------------------------------------------------------
// CWlanBgScan::CheckSettings
// ---------------------------------------------------------------------------
//
void CWlanBgScan::CheckSettings(
        MWlanBgScanProvider::TWlanBgScanSettings& aSettingsToUse,                   // [OUT]
        const MWlanBgScanProvider::TWlanBgScanSettings& aProposedSettings ) const   // [IN]
    {
    aSettingsToUse = aProposedSettings;

    if( aSettingsToUse.bgScanPeakStartTime > KWlanBgScanTwentyThreeFiftyNineOclock ||
        aSettingsToUse.bgScanPeakEndTime > KWlanBgScanTwentyThreeFiftyNineOclock )
        {
        DEBUG2( "CWlanBgScan::CheckSettings() - peak start or end invalid, using default values (start %u, end %u)",
                KWlanDefaultBgScanPeakPeriodStart,
                KWlanDefaultBgScanPeakPeriodEnd );
        aSettingsToUse.bgScanPeakStartTime = KWlanDefaultBgScanPeakPeriodStart;
        aSettingsToUse.bgScanPeakEndTime = KWlanDefaultBgScanPeakPeriodEnd;
        }
    if( aSettingsToUse.bgScanIntervalPeak > KWlanBgScanMaxInterval )
        {
        DEBUG1( "CWlanBgScan::CheckSettings() - peak interval invalid, using default value %u",
                KWlanDefaultBgScanIntervalPeakPeriod );
        aSettingsToUse.bgScanIntervalPeak = KWlanDefaultBgScanIntervalPeakPeriod;
        }
    if( aSettingsToUse.bgScanIntervalOffPeak > KWlanBgScanMaxInterval )
        {
        DEBUG1( "CWlanBgScan::CheckSettings() - off-peak interval invalid, using default value %u",
                KWlanDefaultBgScanIntervalOffPeakPeriod );
        aSettingsToUse.bgScanIntervalOffPeak = KWlanDefaultBgScanIntervalOffPeakPeriod;
        }    
    }

// ---------------------------------------------------------------------------
// CWlanBgScan::IsIntervalChangeNeeded
// ---------------------------------------------------------------------------
//
TBool CWlanBgScan::IsIntervalChangeNeeded()
    {
    TBool ret( ETrue );
    
    // no need to change interval if both peak and off-peak intervals are the same
    if( iBgScanSettings.bgScanPeakStartTime == iBgScanSettings.bgScanPeakEndTime )
        {
        ret = EFalse;
        }
    
    DEBUG1( "CWlanBgScan::IsIntervalChangeNeeded() - returning %d", ret );
    
    return ret;    
    }

// ---------------------------------------------------------------------------
// CWlanBgScan::ScheduleAutoIntervalChange
// ---------------------------------------------------------------------------
//
void CWlanBgScan::ScheduleAutoIntervalChange()
    {
    DEBUG( "CWlanBgScan::ScheduleAutoIntervalChange()" );
    
    iTimerServices.StopTimer( iIntervalChangeRequestId );

    if( IsIntervalChangeNeeded() )
        {
        TTime intervalChangeAt = AutoIntervalChangeAt();
        
        if( KErrNone != iTimerServices.StartTimer( iIntervalChangeRequestId, intervalChangeAt, *this ) )
            {
            DEBUG( "CWlanBgScan::ScheduleAutoIntervalChange() - error: requesting timeout failed, peak <-> off-peak interval will not be changed" );
            }
        }
    else
        {
        DEBUG( "CWlanBgScan::ScheduleAutoIntervalChange() - peak <-> off-peak interval change not needed" );
        }
    
    DEBUG( "CWlanBgScan::ScheduleAutoIntervalChange() - returning" );
    
    }

// ---------------------------------------------------------------------------
// CWlanBgScan::AutoIntervalChangeAt
// ---------------------------------------------------------------------------
//
TTime CWlanBgScan::AutoIntervalChangeAt()
    {
    TTime currentTime;
    currentTime.HomeTime();
    TDateTime change_time( currentTime.DateTime() );
    
#ifdef _DEBUG
    change_time = currentTime.DateTime();
    TBuf<KWlanBgScanMaxDateTimeStrLen> timeNow;
    TRAP_IGNORE( currentTime.FormatL( timeNow, KWlanBgScanDateTimeFormat ) );
    DEBUG1( "CWlanBgScan::AutoIntervalChangeAt() - time now: %S", &timeNow );
#endif
    
    switch( TimeRelationToRange( currentTime, iBgScanSettings.bgScanPeakStartTime, iBgScanSettings.bgScanPeakEndTime ) )
        {
        case ESmaller:
            {
            change_time.SetHour( iBgScanSettings.bgScanPeakStartTime / KGetHours );
            change_time.SetMinute( iBgScanSettings.bgScanPeakStartTime % KGetHours );
            change_time.SetSecond( KZeroSeconds );
            currentTime = change_time;
            break;
            }
        case EInsideRange:
            {
            change_time.SetHour( iBgScanSettings.bgScanPeakEndTime / KGetHours );
            change_time.SetMinute( iBgScanSettings.bgScanPeakEndTime % KGetHours );
            change_time.SetSecond( KZeroSeconds );
            currentTime = change_time;
            if( iBgScanSettings.bgScanPeakStartTime > iBgScanSettings.bgScanPeakEndTime )
                {
                DEBUG( "CWlanBgScan::AutoIntervalChangeAt() - peak end happens tomorrow" );
                currentTime += TTimeIntervalDays( KAddOneDay );
                }
            else
                {
                DEBUG( "CWlanBgScan::AutoIntervalChangeAt() - peak end happens today" );
                }
            break;
            }
        case EGreater:
            {
            change_time.SetHour( iBgScanSettings.bgScanPeakStartTime / KGetHours );
            change_time.SetMinute( iBgScanSettings.bgScanPeakEndTime % KGetHours );
            change_time.SetSecond( KZeroSeconds );
            currentTime = change_time;
            currentTime += TTimeIntervalDays( KAddOneDay );
            break;
            }
        }
    
#ifdef _DEBUG
    change_time = currentTime.DateTime();
    TBuf<KWlanBgScanMaxDateTimeStrLen> dbgString;
    TRAP_IGNORE( currentTime.FormatL( dbgString, KWlanBgScanDateTimeFormat ) );
    DEBUG1( "CWlanBgScan::AutoIntervalChangeAt() - interval change to occur: %S", &dbgString );
#endif
    
    return currentTime;
    }

// ---------------------------------------------------------------------------
// CWlanBgScan::TimeRelationToRange
// ---------------------------------------------------------------------------
//
CWlanBgScan::TRelation CWlanBgScan::TimeRelationToRange( const TTime& aTime, TUint aRangeStart, TUint aRangeEnd ) const
    {
#ifdef _DEBUG
    TBuf<KWlanBgScanMaxDateTimeStrLen> dbgString;
    TRAP_IGNORE( aTime.FormatL( dbgString, KWlanBgScanDateTimeFormat2 ) );
    DEBUG1( "CWlanBgScan::TimeRelationToRange() - time:  %S", &dbgString );
#endif
        
    TDateTime dateTime( aTime.DateTime() );
    
    TUint timeToCheck = ( dateTime.Hour() * KGetHours ) + dateTime.Minute();

    DEBUG2( "CWlanBgScan::TimeRelationToRange() - range: %04u - %04u", aRangeStart, aRangeEnd );

    CWlanBgScan::TRelation relation( ESmaller );
    
    if( aRangeStart == aRangeEnd )
        {
        DEBUG( "CWlanBgScan::TimeRelationToRange() - returning: EGreater" );
        relation = EGreater;
        }
    else if( aRangeStart > aRangeEnd )
        {
        DEBUG( "CWlanBgScan::TimeRelationToRange() - range crosses the midnight" );
        /**
         * As range crosses midnight, there is no way for the relation to be ESmaller.
         */
        if( timeToCheck < aRangeEnd || timeToCheck >= aRangeStart )
            {
            DEBUG( "CWlanBgScan::TimeRelationToRange() - returning: EInsideRange" );
            relation = EInsideRange;
            }
        else
            {
            DEBUG( "CWlanBgScan::TimeRelationToRange() - returning: EGreater" );
            relation = EGreater;
            }
        }
    else
        {
        if( timeToCheck < aRangeStart )
            {
            DEBUG( "CWlanBgScan::TimeRelationToRange() - returning: ESmaller" );
            relation = ESmaller;
            }
        else if( timeToCheck >= aRangeStart && timeToCheck < aRangeEnd )
            {
            DEBUG( "CWlanBgScan::TimeRelationToRange() - returning: EInsideRange" );
            relation = EInsideRange;
            }
        else
            {
            DEBUG( "CWlanBgScan::TimeRelationToRange() - returning: EGreater" );
            relation = EGreater;
            }
        }
    
    return relation;
    }

// ---------------------------------------------------------------------------
// CWlanBgScan::CurrentAutoInterval
// ---------------------------------------------------------------------------
//
TUint32 CWlanBgScan::CurrentAutoInterval()
    {
    TUint32 interval( 0 );
    TTime currentTime;
    currentTime.HomeTime();

    if( TimeRelationToRange( currentTime, iBgScanSettings.bgScanPeakStartTime, iBgScanSettings.bgScanPeakEndTime ) == CWlanBgScan::EInsideRange )
        {
        interval = iBgScanSettings.bgScanIntervalPeak;
        }
    else
        {       
        interval = iBgScanSettings.bgScanIntervalOffPeak;
        }
    
    DEBUG1( "CWlanBgScan::CurrentAutoInterval() - current interval: %u", interval );

    return interval;
    }

// ---------------------------------------------------------------------------
// CWlanBgScan::StartAggressiveBgScan
// ---------------------------------------------------------------------------
//
void CWlanBgScan::StartAggressiveBgScan( TUint32& aInterval, TUint32& aTimeout )
    {
    CWlanBgScanStates::StartAggressiveBgScan( aInterval, aTimeout );
    }
    
// ---------------------------------------------------------------------------
// CWlanBgScan::OnTimeout
// ---------------------------------------------------------------------------
//
void CWlanBgScan::OnTimeout()
    {
    DEBUG( "CWlanBgScan::OnTimeout()" );
    
    NextState( CWlanBgScanStates::EBgScanEventIntervalChanged );
    
    }

// ---------------------------------------------------------------------------
// CWlanBgScan::DoSetInterval
// ---------------------------------------------------------------------------
//
void CWlanBgScan::DoSetInterval( TUint32 aNewInterval )
    {
    DEBUG1( "CWlanBgScan::DoSetInterval( aNewInterval: %u )", aNewInterval );
        
    SetAutoInterval( aNewInterval );
    DEBUG( "CWlanBgScan::DoSetInterval() - interval stored, giving event to state machine" );
    
    NextState( CWlanBgScanStates::EBgScanEventAwsIntervalChanged );

    }

// ---------------------------------------------------------------------------
// CWlanBgScan::AwsPresence
// --------------------------------------------------------------------------
//
CWlanBgScan::TAwsPresence CWlanBgScan::AwsPresence()
    {
    CWlanBgScan::TAwsPresence ret( CWlanBgScan::EAwsPresent );
    
    // If ( ( iAwsOk == EFalse ) And ( iAwsComms == NULL ) )  -> Return EAwsNotPresent
    // If ( ( iAwsOk == EFalse ) And ( iAwsComms != NULL ) )  -> Return EAwsStarting
    // Otherwise                                              -> return EAwsPresent
    
    if( iAwsOk == EFalse && iAwsComms == NULL )
        {
        ret = CWlanBgScan::EAwsNotPresent;
        }
    if( iAwsOk == EFalse && iAwsComms != NULL )
        {
        ret = CWlanBgScan::EAwsStarting;
        }
    
    DEBUG1( "CWlanBgScan::AwsStatus() - returning %i", ret );
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CWlanBgScan::AwsStartupComplete
// --------------------------------------------------------------------------
//
void CWlanBgScan::AwsStartupComplete( TInt aStatus )
    {
    DEBUG1( "CWlanBgScan::AwsStartupComplete( aStatus: %d )", aStatus );
    
    SetAwsStartupStatus( aStatus );

    if( aStatus == KErrNone )
        {
        // AWS is ok
        iAwsOk = ETrue;
        }
#ifdef _DEBUG
    else
        {
        DEBUG( "CWlanBgScan::AwsStartupComplete() - error: AWS startup completed with error status, AWS not in use" );
        }
#endif
    
    NextState( CWlanBgScanStates::EBgScanEventAwsStartupComplete );
    
    }

// ---------------------------------------------------------------------------
// CWlanBgScan::AwsCommandComplete
// --------------------------------------------------------------------------
//
void CWlanBgScan::AwsCommandComplete( MWlanBgScanAwsComms::TAwsCommand& aCommand, TInt aStatus )
    {
    DEBUG2( "CWlanBgScan::AwsCommandComplete( aCommand: %d, aStatus: %d )", aCommand, aStatus );
    
    SetAwsCmdStatus( aCommand, aStatus );

    NextState( CWlanBgScanStates::EBgScanEventAwsCmdComplete );
    
    }

// ---------------------------------------------------------------------------
// CWlanBgScan::AwsCommand
// --------------------------------------------------------------------------
//
void CWlanBgScan::AwsCommand( MWlanBgScanAwsComms::TAwsMessage& aCommand )
    {
    DEBUG( "CWlanBgScan::AwsCommand() - calling SendOrQueueAwsCommand()" );
    iAwsComms->SendOrQueueAwsCommand( aCommand );
    
    }

