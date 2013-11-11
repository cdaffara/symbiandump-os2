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
* %version: 5 %
*/

#include <e32base.h>
#include <e32const.h> 

#include "wlanscanproviderinterface.h"
#include "awsinterface.h"
#include "awsenginebase.h"
#include "wlanbgscanawscommsinterface.h"
#include "wlanbgscan.h"
#include "wlanbgscanstates.h"
#include "wlandevicesettings.h" // default values in case invalid data is passed in NotifyChangedSettings
#include "wlantimer.h"
#include "am_debug.h"

/** Constant indicating that scanning is disabled */
const TUint32 KWlanNoScanning( 0xFFFFFFFF );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWlanBgScanStates::CWlanBgScanStates
// ---------------------------------------------------------------------------
//
CWlanBgScanStates::CWlanBgScanStates( CWlanBgScan& aBgScan, MWlanScanResultProvider& aProvider, MWlanTimerServices& aTimerServices ) :
    iBgScan( aBgScan ),
    iUsedBgScanInterval( KWlanNoScanning ),
    iBgScanInterval( KWlanNoScanning ),
    iAggressiveBgScanInterval( KWlanNoScanning ),
    iAggressiveBgScanTimer( NULL ),
    iNextState( EBgScanStateMax ),
    iAwsStartupStatus( KErrNotFound ),
    iAwsCmd( MWlanBgScanAwsComms::EAwsCommandMax ),
    iAwsCmdStatus( KErrNone ),
    iAutoInterval( KWlanBgScanIntervalNever ),
    iProvider( aProvider ),
    iTimerServices( aTimerServices ),
    iBgScanState( EBgScanStateMax ),
    iIntervalChangeRequestId( 0 ),
    iCompletedAwsCommand( MWlanBgScanAwsComms::EAwsCommandMax ),
    iAwsCommandCompletionCode( KErrNone ),
    iWlanBgScanState( MWlanBgScanProvider::EWlanBgScanOff )
    {
    DEBUG( "CWlanBgScanStates::CWlanBgScanStates()" );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::ConstructL
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::ConstructL()
    {
    DEBUG( "CWlanBgScanStates::ConstructL()" );
    
    if( iBgScan.AwsPresence() == CWlanBgScan::EAwsStarting )
        {
        DEBUG( "CWlanBgScanStates::ConstructL() - AWS starting up, goto Init state" );
        iBgScanState = EBgScanInit;
        }
    else
        {
        DEBUG( "CWlanBgScanStates::ConstructL() - AWS not present, goto Off state" );
        iBgScanState = EBgScanOff;
        }
        
    // Create a timer for guarding when to drop out of aggressive BG scan mode
    iAggressiveBgScanTimer = CWlanTimer::NewL( TCallBack( StopAggressiveBgScan, this ),
                                               TCallBack( StopAggressiveBgScan, this ),
                                               TCallBack( StopAggressiveBgScan, this ) );
                
    DEBUG( "CWlanBgScanStates::ConstructL() - done" );    
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::~CWlanBgScanStates
// ---------------------------------------------------------------------------
//
CWlanBgScanStates::~CWlanBgScanStates()
    {
    DEBUG( "CWlanBgScanStates::~CWlanBgScanStates()" );
    delete iAggressiveBgScanTimer;
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::StartAggressiveBgScan
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::StartAggressiveBgScan( TUint32& aInterval, TUint32& aDuration )
    {
    DEBUG3( "CWlanBgScanStates::StartAggressiveBgScan() - new interval: %u, old interval: %u, duration: %u",
        aInterval, iAggressiveBgScanInterval, aDuration );
    // Serve the request only if the requested interval is smaller or equal than
    // what is already being served
    if( aInterval <= iAggressiveBgScanInterval )
        {
        iAggressiveBgScanInterval = aInterval;
    
        // Start the timer guarding when to drop out of aggressive mode
        iAggressiveBgScanTimer->Cancel();
        iAggressiveBgScanTimer->After( aDuration );    
    
        // Refresh interval used in BG scanning
        RefreshUsedInterval();
        }
    }
    
// ---------------------------------------------------------------------------
// CWlanBgScanStates::StopAggressiveBgScan
// ---------------------------------------------------------------------------
//
TInt CWlanBgScanStates::StopAggressiveBgScan( TAny* aPtr )
    {
    DEBUG( "CWlanBgScanStates::StopAggressiveBgScan()" );
    
    CWlanBgScanStates* self = static_cast<CWlanBgScanStates*>( aPtr );
    
    // Disable aggressive mode
    self->iAggressiveBgScanInterval = KWlanNoScanning;
    
    // Refresh interval used in BG scanning
    self->RefreshUsedInterval();
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::GetBgInterval
// ---------------------------------------------------------------------------
//
TUint32 CWlanBgScanStates::GetBgInterval()
    {
    TUint32 scanInterval( iUsedBgScanInterval );
    
    if( scanInterval == KWlanNoScanning )
        {
        scanInterval = KWlanBgScanIntervalNever;
        }
        
    return scanInterval;
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::IntervalChanged
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::IntervalChanged()
    {
    DEBUG( "CWlanBgScanStates::IntervalChanged()" );

    if( iBgScan.AwsPresence() == CWlanBgScan::EAwsStarting )
        {
        DEBUG( "CWlanBgScanStates::IntervalChanged() - AWS is still starting up, returning" );
        return;
        }
    
    NextState( CWlanBgScanStates::EBgScanEventIntervalChanged );
    
    DEBUG2( "CWlanBgScanStates::IntervalChanged() - current interval %u, current state %u",
            GetBgInterval(),
            iBgScanState );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::NextState
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::NextState( const CWlanBgScanStates::TWlanBgScanSmEvent& aEvent )
    {
    DEBUG1( "CWlanBgScanStates::NextState() - aEvent %u", aEvent );
    
    switch ( iBgScanState )
        {
        case EBgScanInit:
            {
            InStateInit( iBgScanState, aEvent );
            break;
            }
        case EBgScanOff:
            {
            InStateOff( iBgScanState, aEvent );
            break;
            }
        case EBgScanOn:
            {
            InStateOn( iBgScanState, aEvent );
            break;
            }
        case EBgScanAuto:
            {
            InStateAuto( iBgScanState, aEvent );
            break;
            }
        case EBgScanAutoAws:
            {
            InStateAutoAws( iBgScanState, aEvent );
            break;
            }
        case EBgScanSendingAwsCmd:
            {
            InStateSendingAwsCmd( iBgScanState, aEvent );
            break;
            }
        default:
            {
            DEBUG1( "CWlanBgScanStates::NextState() - error: invalid state %d", iBgScanState );
            ASSERT( 0 );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::InStateInit
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::InStateInit( TWlanBgScanState& aState, const CWlanBgScanStates::TWlanBgScanSmEvent& aEvent )
    {
    switch( aEvent )
        {
        case CWlanBgScanStates::EBgScanEventAwsStartupComplete:
            {
            DEBUG( "CWlanBgScanStates::InStateInit() - event: EBgScanEventAwsStartupComplete" );
            switch( AwsStartupStatus() )
                {
                case KErrNone:
                    {
                    DEBUG( "CWlanBgScanStates::InStateInit() - AWS startup successful" );
                    switch ( iBgScanSettings.backgroundScanInterval )
                        {
                        case KWlanBgScanIntervalNever:
                            {
                            DEBUG( "CWlanBgScanStates::InStateInit() - state change Init to Off" );
                            StateChangeInitToOff( aState );
                            break;
                            }
                        case KWlanBgScanIntervalAutomatic:
                            {
                            DEBUG( "CWlanBgScanStates::InStateInit() - state change Init to AutoAws" );
                            StateChangeInitToAutoAws( aState );
                            break;
                            }
                        default:
                            {
                            DEBUG( "CWlanBgScanStates::InStateInit() - state change Init to On" );
                            StateChangeInitToOn( aState );
                            }
                        }
                    break;
                    }
                default:
                    {
                    DEBUG1( "CWlanBgScanStates::InStateInit() - AWS startup failed! Status %d", AwsStartupStatus() );
                    switch ( iBgScanSettings.backgroundScanInterval )
                        {
                        case KWlanBgScanIntervalNever:
                            {
                            DEBUG( "CWlanBgScanStates::InStateInit() - state change Init to Off" );
                            StateChangeInitToOff( aState );
                            break;
                            }
                        case KWlanBgScanIntervalAutomatic:
                            {
                            DEBUG( "CWlanBgScanStates::InStateInit() - state change Init to Auto" );
                            StateChangeInitToAuto( aState );
                            break;
                            }
                        default:
                            {
                            DEBUG( "CWlanBgScanStates::InStateInit() - state change Init to On" );
                            StateChangeInitToOn( aState );
                            break;
                            }
                        }
                    break;
                    }
                }
            break;
            }
        default:
            {
            DEBUG1( "CWlanBgScanStates::InStateInit() - event %d ignored", aEvent );
            }
        }
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::StateChangeInitToOn
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::StateChangeInitToOn( TWlanBgScanState& aState )
    {
    DEBUG( "CWlanBgScanStates::StateChangeInitToOn()" );
    aState = EBgScanOn;
    SetInterval( iBgScanSettings.backgroundScanInterval );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::StateChangeInitToOff
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::StateChangeInitToOff( TWlanBgScanState& aState )
    {
    DEBUG( "CWlanBgScanStates::StateChangeInitToOff()" );
    aState = EBgScanOff;
    SetInterval( KWlanBgScanIntervalNever );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::StateChangeInitToAuto
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::StateChangeInitToAuto( TWlanBgScanState& aState )
    {
    aState = EBgScanAuto;
    DEBUG( "CWlanBgScanStates::StateChangeInitToAuto() - determine next interval change time and request callback" );
    iBgScan.ScheduleAutoIntervalChange();
    SetInterval( iBgScan.CurrentAutoInterval() );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::InStateOff
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::InStateOff( TWlanBgScanState& aState, const CWlanBgScanStates::TWlanBgScanSmEvent& aEvent )
    {
    switch( aEvent )
        {
        case CWlanBgScanStates::EBgScanEventIntervalChanged:
            {
            DEBUG( "CWlanBgScanStates::InStateOff() - event: EBgScanEventIntervalChanged" );
            switch( iBgScanSettings.backgroundScanInterval )
                {
                case KWlanBgScanIntervalNever:
                    {
                    DEBUG( "CWlanBgScanStates::InStateOff() - no change in the interval" );
                    aState = EBgScanOff;
                    break;
                    }
                case KWlanBgScanIntervalAutomatic:
                    {
                    DEBUG( "CWlanBgScanStates::InStateOff() - state change Off to Automatic" );
                    StateChangeOffToAutomatic( aState );
                    break;
                    }
                default:
                    {
                    DEBUG1( "CWlanBgScanStates::InStateOff() - state change Off to On (interval: %u)", iBgScanSettings.backgroundScanInterval );
                    StateChangeOffToOn( aState, iBgScanSettings.backgroundScanInterval );
                    }
                }
            break;
            }
        default:
            {
            DEBUG1( "CWlanBgScanStates::InStateOff() - event %d ignored", aEvent );
            }
        }
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::StateChangeOffToOn
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::StateChangeOffToOn( TWlanBgScanState& aState, TUint32 aNewBgScanInterval )
    {
    aState = EBgScanOn;
    SetInterval( aNewBgScanInterval );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::StateChangeOffToAutomatic
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::StateChangeOffToAutomatic( TWlanBgScanState& aState )
    {
    if ( iBgScan.AwsPresence() == CWlanBgScan::EAwsPresent )
        {
        DEBUG( "CWlanBgScanStates::StateChangeOffToAutomatic() - state change Off to SendingAwsCmd" );
        aState = EBgScanSendingAwsCmd;
        DEBUG( "CWlanBgScanStates::InStaStateChangeOffToAutomaticteOff() - if command sending succeeds, next state is AutoAws" );
        iNextState = EBgScanAutoAws;
        MWlanBgScanAwsComms::TAwsMessage msg = { MWlanBgScanAwsComms::EStart, 0 };
        iBgScan.AwsCommand( msg );
        DEBUG( "CWlanBgScanStates::StateChangeOffToAutomatic() - SendOrQueueAwsCommand() returned" );
        }
    else
        {
        DEBUG( "CWlanBgScanStates::StateChangeOffToAutomatic() - state change Off to Auto" );
        DEBUG( "CWlanBgScanStates::StateChangeOffToAutomatic() - determine next interval change time and request callback" );
        aState = EBgScanAuto;
        iBgScan.ScheduleAutoIntervalChange();
        SetInterval( iBgScan.CurrentAutoInterval() );
        }
    }
// ---------------------------------------------------------------------------
// CWlanBgScanStates::InStateOn
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::InStateOn( TWlanBgScanState& aState, const CWlanBgScanStates::TWlanBgScanSmEvent& aEvent )
    {
    switch( aEvent )
        {
        case CWlanBgScanStates::EBgScanEventIntervalChanged:
            {
            DEBUG( "CWlanBgScanStates::InStateOn() - event: EBgScanEventIntervalChanged" );
            switch( iBgScanSettings.backgroundScanInterval )
                {
                case KWlanBgScanIntervalNever:
                    {
                    DEBUG( "CWlanBgScanStates::InStateOn() - state change On to Off" );
                    StateChangeOnToOff( aState );
                    break;
                    }
                case KWlanBgScanIntervalAutomatic:
                    {
                    DEBUG( "CWlanBgScanStates::InStateOn() - state change On to Auto" );
                    StateChangeOnToAutomatic( aState );
                    break;
                    }
                default:
                    {
                    DEBUG( "CWlanBgScanStates::InStateOn() - state change On to On" );
                    StateChangeOnToOn( aState, iBgScanSettings.backgroundScanInterval );
                    }
                }
            break;
            }
        default:
            {
            DEBUG1( "CWlanBgScanStates::InStateOn() - event %d ignored", aEvent );
            }
        }
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::StateChangeOnToOn
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::StateChangeOnToOn( TWlanBgScanState& aState, TUint32 aNewBgScanSetting  )
    {
    aState = EBgScanOn;
    SetInterval( aNewBgScanSetting );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::StateChangeOnToAutomatic
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::StateChangeOnToAutomatic( TWlanBgScanState& aState )
    {
    if ( iBgScan.AwsPresence() == CWlanBgScan::EAwsPresent )
        {
        DEBUG( "CWlanBgScanStates::StateChangeOnToAutomatic() - state change Init to SendingAwsCmd" );
        aState = EBgScanSendingAwsCmd;
        DEBUG( "CWlanBgScanStates::StateChangeOnToAutomatic() - if command sending succeeds, next state is AutoAws" );
        iNextState = EBgScanAutoAws;
        MWlanBgScanAwsComms::TAwsMessage msg = { MWlanBgScanAwsComms::EStart, 0 };
        iBgScan.AwsCommand( msg );
        DEBUG( "CWlanBgScanStates::StateChangeOnToAutomatic() - SendOrQueueAwsCommand() returned" );
        }
    else
        {
        aState = EBgScanAuto;
        iBgScan.ScheduleAutoIntervalChange();
        SetInterval( iBgScan.CurrentAutoInterval() );        
        }
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::StateChangeOnToOff
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::StateChangeOnToOff( TWlanBgScanState& aState )
    {
    aState = EBgScanOff;
    SetInterval( KWlanBgScanIntervalNever );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::InStateAuto
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::InStateAuto( TWlanBgScanState& aState, const CWlanBgScanStates::TWlanBgScanSmEvent& aEvent )
    {
    switch( aEvent )
        {
        case CWlanBgScanStates::EBgScanEventIntervalChanged:
            {
            DEBUG( "CWlanBgScanStates::InStateAuto() - event: EBgScanEventIntervalChanged" );
            switch( iBgScanSettings.backgroundScanInterval )
                {
                case KWlanBgScanIntervalNever:
                    {
                    DEBUG( "CWlanBgScanStates::InStateAuto() - state change Auto to Off" );
                    StateChangeAutoToOff( aState );
                    break;
                    }
                case KWlanBgScanIntervalAutomatic:
                    {
                    DEBUG( "CWlanBgScanStates::InStateAuto() - state still Auto" );
                    StateChangeAutoToAuto( aState );
                    break;
                    }
                default:
                    {
                    DEBUG( "CWlanBgScanStates::InStateAuto() - state change Auto to On" );
                    StateChangeAutoToOn( aState, iBgScanSettings.backgroundScanInterval );
                    }
                }
            break;
            }
        default:
            {
            DEBUG1( "CWlanBgScanStates::InStateAuto() - event %d ignored", aEvent );
            }
        }
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::StateChangeAutoToOff
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::StateChangeAutoToOff( TWlanBgScanState& aState )
    {
    aState = EBgScanOff;
    iTimerServices.StopTimer( iIntervalChangeRequestId );
    iIntervalChangeRequestId = 0;
    SetInterval( KWlanBgScanIntervalNever );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::StateChangeAutoToAuto
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::StateChangeAutoToAuto( TWlanBgScanState& aState )
    {
    aState = EBgScanAuto;
    iBgScan.ScheduleAutoIntervalChange();
    SetInterval( iBgScan.CurrentAutoInterval() );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::StateChangeAutoToOn
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::StateChangeAutoToOn( TWlanBgScanState& aState, TUint32 aNewBgScanSetting  )
    {
    aState = EBgScanOn;
    iTimerServices.StopTimer( iIntervalChangeRequestId );
    iIntervalChangeRequestId = 0;
    SetInterval( aNewBgScanSetting );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::InStateAutoAws
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::InStateAutoAws( TWlanBgScanState& aState, const CWlanBgScanStates::TWlanBgScanSmEvent& aEvent )
    {
    switch( aEvent )
        {
        case CWlanBgScanStates::EBgScanEventIntervalChanged:
            {
            DEBUG( "CWlanBgScanStates::InStateAutoAws() - event: EBgScanEventIntervalChanged" );
            switch( iBgScanSettings.backgroundScanInterval )
                {
                case KWlanBgScanIntervalNever:
                    {
                    DEBUG( "CWlanBgScanStates::InStateAutoAws() - state change AutoAws to Off" );
                    StateChangeAutoAwsToOff( aState );
                    break;
                    }
                case KWlanBgScanIntervalAutomatic:
                    {
                    DEBUG( "CWlanBgScanStates::InStateAutoAws() - no change in the interval" );
                    aState = EBgScanAutoAws;
                    break;
                    }
                default:
                    {
                    DEBUG( "CWlanBgScanStates::InStateAutoAws() - state change AutoAws to On" );
                    StateChangeAutoAwsToOn( aState );
                    }
                }
            break;
            }
        case CWlanBgScanStates::EBgScanEventAwsIntervalChanged:
            {
            DEBUG( "CWlanBgScanStates::InStateAutoAws() - event: EBgScanEventAwsIntervalChanged" );
            SetInterval( AutoInterval() );
            break;
            }
        default:
            {
            DEBUG1( "CWlanBgScanStates::InStateAutoAws() - event %d ignored", aEvent );
            }
        }
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::StateChangeAutoAwsToOff
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::StateChangeAutoAwsToOff( TWlanBgScanState& aState )
    {
    aState = EBgScanSendingAwsCmd;
    iNextState = EBgScanOff;
    DEBUG( "CWlanBgScanStates::StateChangeAutoAwsToOff() - calling SendOrQueueAwsCommand()" );
    MWlanBgScanAwsComms::TAwsMessage msg = { MWlanBgScanAwsComms::EStop, 0 };
    iBgScan.AwsCommand( msg );
    DEBUG( "CWlanBgScanStates::StateChangeAutoAwsToOff() - SendOrQueueAwsCommand() returned" );
    iProvider.CancelScan();
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::StateChangeAutoAwsToOn
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::StateChangeAutoAwsToOn( TWlanBgScanState& aState )
    {
    aState = EBgScanSendingAwsCmd;
    iNextState = EBgScanOn;
    DEBUG( "CWlanBgScanStates::StateChangeAutoAwsToOn() - calling SendAwsCommand()" );
    MWlanBgScanAwsComms::TAwsMessage msg = { MWlanBgScanAwsComms::EStop, 0 };
    iBgScan.AwsCommand( msg );
    DEBUG( "CWlanBgScanStates::StateChangeAutoAwsToOn() - SendAwsCommand() returned" );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::InStateSendingAwsCmd
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::InStateSendingAwsCmd( TWlanBgScanState& aState, const CWlanBgScanStates::TWlanBgScanSmEvent& aEvent )
    {
    DEBUG1( "CWlanBgScanStates::InStateSendingAwsCmd() - event: %d", aEvent );

    switch( aEvent )
        {
        case CWlanBgScanStates::EBgScanEventAwsCmdComplete:
            {
            DEBUG( "CWlanBgScanStates::InStateSendingAwsCmd() - event: EBgScanEventAwsCmdComplete" );
            switch( AwsCmdStatus() )
                {
                case KErrNone:
                    {
                    DEBUG( "CWlanBgScanStates::InStateSendingAwsCmd() - AWS command sending successful" );
                    switch ( iNextState )
                        {
                        case EBgScanOff:
                            {
                            ASSERT( AwsCmd() == MWlanBgScanAwsComms::EStop );
                            DEBUG( "CWlanBgScanStates::InStateSendingAwsCmd() - state change SendingAwsCmd to Off" );
                            StateChangeSendingAwsCmdToOff( aState );
                            break;
                            }
                        case EBgScanOn:
                            {
                            ASSERT( AwsCmd() == MWlanBgScanAwsComms::EStop );
                            DEBUG( "CWlanBgScanStates::InStateSendingAwsCmd() - state change SendingAwsCmd to On" );
                            StateChangeSendingAwsCmdToOn( aState );
                            break;
                            }
                        case EBgScanAutoAws:
                            {
                            ASSERT( AwsCmd() == MWlanBgScanAwsComms::EStart );
                            DEBUG( "CWlanBgScanStates::InStateSendingAwsCmd() - state change SendingAwsCmd to AutoAws" );
                            StateChangeSendingAwsCmdToAutoAws( aState );
                            break;
                            }
                        default:
                            {
                            DEBUG1( "CWlanBgScanStates::InStateSendingAwsCmd() - illegal state change from SendingAwsCmd to %u", iNextState );
                            ASSERT( 0 );
                            }
                        }
                    break;
                    }
                default:
                    {
                    DEBUG2( "CWlanBgScanStates::InStateSendingAwsCmd() - AWS command %d sending failed! Status %d", AwsCmd(), AwsCmdStatus() );
                    switch ( iNextState )
                        {
                        case EBgScanOff:
                            {
                            DEBUG( "CWlanBgScanStates::InStateSendingAwsCmd() - state change SendingAwsCmd to Off" );
                            StateChangeSendingAwsCmdToOff( aState );
                            break;
                            }
                        case EBgScanOn:
                            {
                            DEBUG( "CWlanBgScanStates::InStateSendingAwsCmd() - state change SendingAwsCmd to On" );
                            StateChangeSendingAwsCmdToOn( aState );
                            break;
                            }
                        case EBgScanAutoAws:
                            {
                            DEBUG( "CWlanBgScanStates::InStateSendingAwsCmd() - state change SendingAwsCmd to Auto" );
                            StateChangeSendingAwsCmdToAuto( aState );
                            break;
                            }
                        default:
                            {
                            DEBUG1( "CWlanBgScanStates::InStateSendingAwsCmd() - illegal state change from SendingAwsCmd to %u", iNextState );
                            ASSERT( 0 );
                            }
                        }
                    break;
                    }
                }
            break;
            }
        default:
            {
            DEBUG1( "CWlanBgScanStates::InStateSendingAwsCmd() - event %d ignored", aEvent );
            }
        }
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::StateChangeInitToAutoAws
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::StateChangeInitToAutoAws( TWlanBgScanState& aState )
    {
    if ( iBgScan.AwsPresence() == CWlanBgScan::EAwsPresent )
        {
        DEBUG( "CWlanBgScanStates::StateChangeInitToAutoAws() - state change Init to SendingAwsCmd" );
        aState = EBgScanSendingAwsCmd;
        DEBUG( "CWlanBgScanStates::StateChangeInitToAutoAws() - if command sending succeeds, next state is AutoAws" );
        iNextState = EBgScanAutoAws;
        MWlanBgScanAwsComms::TAwsMessage msg = { MWlanBgScanAwsComms::EStart, 0 };
        iBgScan.AwsCommand( msg );
        DEBUG( "CWlanBgScanStates::StateChangeInitToAutoAws() - SendOrQueueAwsCommand() returned" );
        }
    else
        {
        DEBUG( "CWlanBgScanStates::StateChangeInitToAutoAws() - error: AWS not present!" );
        ASSERT( 0 );
        DEBUG( "CWlanBgScanStates::StateChangeInitToAutoAws() - state change Init to Auto" );
        aState = EBgScanAuto;
        DEBUG( "CWlanBgScanStates::StateChangeInitToAutoAws() - * determine next interval change time and request callback" );
        iBgScan.ScheduleAutoIntervalChange();
        SetInterval( iBgScan.CurrentAutoInterval() );
        }
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::StateChangeSendingAwsCmdToOff
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::StateChangeSendingAwsCmdToOff( TWlanBgScanState& aState )
    {
    DEBUG( "CWlanBgScanStates::StateChangeSendingAwsCmdToOff()" );
    aState = EBgScanOff;
    SetInterval( KWlanBgScanIntervalNever );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::StateChangeSendingAwsCmdToOn
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::StateChangeSendingAwsCmdToOn( TWlanBgScanState& aState )
    {
    DEBUG( "CWlanBgScanStates::StateChangeSendingAwsCmdToOn()" );
    aState = EBgScanOn;
    SetInterval( iBgScanSettings.backgroundScanInterval );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::StateChangeSendingAwsCmdToAuto
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::StateChangeSendingAwsCmdToAuto( TWlanBgScanState& aState )
    {
    DEBUG( "CWlanBgScanStates::StateChangeSendingAwsCmdToAuto()" );
    aState = EBgScanAuto;
    DEBUG( "CWlanBgScanStates::StateChangeSendingAwsCmdToAuto() - * determine next interval change time and request callback" );
    iBgScan.ScheduleAutoIntervalChange();
    SetInterval( iBgScan.CurrentAutoInterval() );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::StateChangeSendingAwsCmdToAutoAws
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::StateChangeSendingAwsCmdToAutoAws( TWlanBgScanState& aState )
    {
    DEBUG( "CWlanBgScanStates::StateChangeSendingAwsCmdToAutoAws()" );
    aState = EBgScanAutoAws;
    SetInterval( AutoInterval() );
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::SetInterval
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::SetInterval( TUint32 aInterval )
    {
    DEBUG2( "CWlanBgScanStates::SetInterval() - current: %u, new: %u", iBgScanInterval, aInterval );

    if( aInterval == KWlanBgScanIntervalNever )
        {
        iBgScanInterval = KWlanNoScanning;
        }
    else
        {
        iBgScanInterval = aInterval;
        }
    
    RefreshUsedInterval();
    }

// ---------------------------------------------------------------------------
// CWlanBgScanStates::RefreshUsedInterval
// ---------------------------------------------------------------------------
//
void CWlanBgScanStates::RefreshUsedInterval()
    {
    TUint oldInterval = iUsedBgScanInterval;
    
    DEBUG4( "CWlanBgScanStates::RefreshUsedInterval() - agg: %u, normal: %u, used: %u, bg scan state: %u",
        iAggressiveBgScanInterval, iBgScanInterval, iUsedBgScanInterval, iWlanBgScanState );
    
    // If      ( WLAN BG scanning is OFF )               -> use interval: KWlanNoScanning 
    // Else If ( aggressive interval < normal interval ) -> use interval: aggressive interval
    // Otherwise                                         -> use interval: background scan interval
    if( iWlanBgScanState == MWlanBgScanProvider::EWlanBgScanOff )
        {
        iUsedBgScanInterval = KWlanNoScanning;
        }
    else if( iAggressiveBgScanInterval < iBgScanInterval )
        {
        iUsedBgScanInterval = iAggressiveBgScanInterval;
        }
    else
        {
        iUsedBgScanInterval = iBgScanInterval;
        }
    
    // If the interval changed, update it to P&S
    if( iUsedBgScanInterval != oldInterval )
        {
        DEBUG2( "CWlanBgScanStates::RefreshUsedInterval() - used interval changed (new: %u, old: %u), publish it via P&S",
            iUsedBgScanInterval, oldInterval );
        TUint32 interval = GetBgInterval();
        iProvider.PublishBgScanInterval( interval );
        }
   
    // Check if scanning has to be cancelled or carried out immediately
    if( KWlanNoScanning == iUsedBgScanInterval )
        {
        DEBUG( "CWlanBgScanStates::RefreshUsedInterval() - scanning disabled, cancel pending scan" );
        // In case scanning was disabled, cancel the pending scan request
        iProvider.CancelScan();
        }
    else if( iUsedBgScanInterval < oldInterval )
        {
        DEBUG( "CWlanBgScanStates::RefreshUsedInterval() - new interval smaller than the old one, scan immediately" );
        // In case the new interval is smaller than the old one, scan
        // immediately
        iProvider.CancelScan();
        iProvider.Scan( KWlanBgScanMaxDelayExpireImmediately );
        }
    // else
        // In case the new interval is bigger than the old one, it
        // is taken into use after the pending scan request completes.
    DEBUG1( "CWlanBgScanStates::RefreshUsedInterval() - using interval: %u", iUsedBgScanInterval );
    
    }

    
    
