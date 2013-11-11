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
* Description:  Active object to get notification of system time change.
*
*/

/*
* %version: 4 %
*/

// INCLUDE FILES
#include <e32std.h>
#include "wlansystemtimehandler.h"
#include "am_debug.h"

/** 
 * Region cache should expire after 5 hours and therefore
 * 30 min timer is set 10 times 
 */
const TInt KRegionCacheClearingTimer = 1800*1000000;  // 30 min
const TInt KRegionCacheClearingTimerCount = 10;
/** 
 * Two year timeout used to receive notification about system time change 
 */
const TInt KWlanSystemTimeHandlerTimeoutTwoYears = 2;

#ifdef _DEBUG
/**
 * Formatting of date time debug string.
 */
_LIT( KWlanSystemTimeHandlerDateTimeFormat, "%F %*E %*N %Y %H:%T:%S" );

/**
 * Maximun length for date time debug string.
 */
const TInt KWlanSystemTimeHandlerMaxDateTimeStrLen = 50;
#endif

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWlanSystemTimeHandler::CWlanSystemTimeHandler(
    MWlanSystemTimeCallback& aClient ) :
    CActive( CActive::EPriorityStandard ),
    iClient( aClient )
    {
    DEBUG( "CWlanSystemTimeHandler::CWlanSystemTimeHandler()" );
    }

// Static constructor.
CWlanSystemTimeHandler* CWlanSystemTimeHandler::NewL(
    MWlanSystemTimeCallback& aClient)
    {
    DEBUG( "CWlanSystemTimeHandler::NewL()" );
    CWlanSystemTimeHandler* self =
        new (ELeave) CWlanSystemTimeHandler( aClient );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Symbian 2nd phase constructor can leave.
void CWlanSystemTimeHandler::ConstructL()
    {
    DEBUG( "CWlanSystemTimeHandler::ConstructL()" );

    CActiveScheduler::Add( this );
    User::LeaveIfError( iTimer.CreateLocal() );
    }

// Destructor
CWlanSystemTimeHandler::~CWlanSystemTimeHandler()
    {
    DEBUG( "CWlanSystemTimeHandler::~CWlanSystemTimeHandler()" );

    Cancel();
    iTimer.Close();
    }

// ---------------------------------------------------------
// CWlanSystemTimeHandler::DoCancel
// Release synchronise call. Is called by CActive::Cancel()
// ---------------------------------------------------------
//
void CWlanSystemTimeHandler::DoCancel()
    {
    DEBUG( "CWlanSystemTimeHandler::DoCancel()" );
    iTimer.Cancel();
    }

// ---------------------------------------------------------
// CWlanSystemTimeHandler::StartTimer
// ---------------------------------------------------------
//
TInt CWlanSystemTimeHandler::StartTimer()
    {
    TTime timeout;
    timeout.HomeTime();
    timeout += TTimeIntervalYears( KWlanSystemTimeHandlerTimeoutTwoYears );
    
    /**
     * Set timeout to two years from now, just to receive notification when
     * system time changes.
     */ 
    iTimer.At(iStatus, timeout );
     
    SetActive();
    
#ifdef _DEBUG
    TBuf<KWlanSystemTimeHandlerMaxDateTimeStrLen> timeoutAt;
    TRAP_IGNORE( timeout.FormatL( timeoutAt, KWlanSystemTimeHandlerDateTimeFormat ) );
    DEBUG1( "CWlanSystemTimeHandler::Start() - system time change notification timer started, timeout at: %S", &timeoutAt );
#endif
    
    return KErrNone;
    }

// ---------------------------------------------------------
// CWlanSystemTimeHandler::StopTimer   
// ---------------------------------------------------------
//
void CWlanSystemTimeHandler::StopTimer()
    {
    DEBUG( "CWlanSystemTimeHandler::StopTimer()" );
    Cancel();
    }    

// ---------------------------------------------------------
// CWlanSystemTimeHandler::RunL
// ---------------------------------------------------------
//
void CWlanSystemTimeHandler::RunL()
    {
    DEBUG1( "CWlanSystemTimeHandler::RunL() - iStatus: %d", iStatus.Int() );

    switch( iStatus.Int() )
        {
        case KErrAbort:
            {
            DEBUG( "CWlanSystemTimeHandler::RunL() - system time changed, informing client" );
            /**
             * RTimer is aborted when system time changes.
             * 
             * Inform client that system time has now been changed. 
             */
            iClient.OnSystemTimeChange();
            // falls through on purpose
            }
        case KErrNone:
            {
            Cancel();

            TTime newTimeout;
            newTimeout.HomeTime();
            newTimeout +=  TTimeIntervalYears( KWlanSystemTimeHandlerTimeoutTwoYears );
            
            /**
             * Set new timeout to two years from now, just to receive notification when
             * system time changes next time.
             */ 
            iTimer.At( iStatus, newTimeout );
             
            SetActive();
            
#ifdef _DEBUG
            TBuf<KWlanSystemTimeHandlerMaxDateTimeStrLen> timeoutAt;
            TRAP_IGNORE( newTimeout.FormatL( timeoutAt, KWlanSystemTimeHandlerDateTimeFormat ) );
            DEBUG1( "CWlanSystemTimeHandler::RunL() - system time change notification timer restarted, timeout at: %S", &timeoutAt );
#endif
            
            break;
            }
        default:
            {
            DEBUG( "CWlanSystemTimeHandler::RunL() - ERROR: problem setting up system time change notifications, notifications are now disabled" );
            Cancel();
            }
        }
    }

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWlanPeriodicCacheClearingHandler::CWlanPeriodicCacheClearingHandler(
    MWlanSystemTimeCallback& aClient ) :
    CActive( CActive::EPriorityStandard ),
    iClearTimerCounter( 0 ),
    iClient( aClient )
    {
    DEBUG( "CWlanPeriodicCacheClearingHandler::CWlanPeriodicCacheClearingHandler()" );
    }

// Static constructor.
CWlanPeriodicCacheClearingHandler* CWlanPeriodicCacheClearingHandler::NewL(
    MWlanSystemTimeCallback& aClient)
    {
    DEBUG( "CWlanPeriodicCacheClearingHandler::NewL()" );
    CWlanPeriodicCacheClearingHandler* self =
        new (ELeave) CWlanPeriodicCacheClearingHandler( aClient );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Symbian 2nd phase constructor can leave.
void CWlanPeriodicCacheClearingHandler::ConstructL()
    {
    DEBUG( "CWlanPeriodicCacheClearingHandler::ConstructL()" );

    CActiveScheduler::Add( this );
    User::LeaveIfError( iClearTimer.CreateLocal() );
    }

// Destructor
CWlanPeriodicCacheClearingHandler::~CWlanPeriodicCacheClearingHandler()
    {
    DEBUG( "CWlanPeriodicCacheClearingHandler::~CWlanPeriodicCacheClearingHandler()" );

    Cancel();
    iClearTimer.Close();
    }

// ---------------------------------------------------------
// CWlanPeriodicCacheClearingHandler::DoCancel
// Release synchronise call. Is called by CActive::Cancel()
// ---------------------------------------------------------
//
void CWlanPeriodicCacheClearingHandler::DoCancel()
    {
    DEBUG( "CWlanPeriodicCacheClearingHandler::DoCancel()" );
    iClearTimer.Cancel();
    }

// ---------------------------------------------------------
// CWlanPeriodicCacheClearingHandler::StartTimer
// ---------------------------------------------------------
//
TInt CWlanPeriodicCacheClearingHandler::StartTimer()
    {
    DEBUG( "CWlanPeriodicCacheClearingHandler::StartTimer()" );
    
    // Issue an Asynchronous Request
    iClearTimer.After(iStatus, KRegionCacheClearingTimer);
     
    // set it to active once request is issued
    SetActive();
    
    return KErrNone;
    }

// ---------------------------------------------------------
// CWlanPeriodicCacheClearingHandler::StopTimer   
// ---------------------------------------------------------
//
void CWlanPeriodicCacheClearingHandler::StopTimer()
    {
    DEBUG( "CWlanPeriodicCacheClearingHandler::StopTimer()" );
    Cancel();
    iClearTimerCounter = 0;
    }    

// ---------------------------------------------------------
// CWlanPeriodicCacheClearingHandler::RunL
// Timer has expired. 
// ---------------------------------------------------------
//
void CWlanPeriodicCacheClearingHandler::RunL()
    {
    DEBUG( "CWlanPeriodicCacheClearingHandler::RunL()" );

    // This is the case when system time is changed and
    // hence will cause the timer to abort with KErrAbort.

	if( iStatus.Int() == KErrNone && 
	    iClearTimerCounter < KRegionCacheClearingTimerCount-1) 
        {
        iClearTimerCounter += 1;
        
        // Issue an Asynchronous Request
        iClearTimer.After(iStatus, KRegionCacheClearingTimer);
     
        // set it to active once request is issued
        SetActive();

        }
    else if ( iStatus.Int() == KErrNone && 
	          iClearTimerCounter >= KRegionCacheClearingTimerCount-1 ) 
        {
    	/* Inform client that system time has been changed so that
         * region information cache can be cleared. 
         */
        iClient.OnCacheClearTimerExpiration();
        iClearTimerCounter = 0;
        }
	else
	    {
	    DEBUG( "CWlanPeriodicCacheClearingHandler::RunL() - time expired" );
	    }	
    }

