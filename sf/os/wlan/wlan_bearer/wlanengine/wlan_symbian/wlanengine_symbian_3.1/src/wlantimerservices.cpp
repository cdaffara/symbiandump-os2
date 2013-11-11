/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class implements timer service with callback functionality.
*
*/

/*
* %version: 2 %
*/

#include <e32std.h>
#include <e32base.h>
#include "wlantimerservices.h"
#include "am_debug.h"

const TInt KMaxTimerTimeout            = 0x7FFFFFFF;
const TInt KFirstItemIndex             = 0;
const TInt KNoRequests                 = -1;
const TInt KEntriesAreEqual            = 0;
const TInt KFirstEntryIsBigger         = 1;
const TInt KFirstEntryIsSmaller        = -1;

#ifdef _DEBUG
/**
 * Formatting of date time debug string.
 */
_LIT( KWlanTimerServicesDateTimeFormat, "%F %*E %*N %D %H:%T:%S" );

/**
 * Maximun length for date time debug string.
 */
const TInt KWlanTimerServicesMaxDateTimeStrLen = 50;
#endif

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWlanTimerServices::CWlanTimerServices() :
    CActive( CActive::EPriorityStandard ),
    iRequestId( 0 )
    {
    DEBUG( "CWlanTimerServices::CWlanTimerServices()" );
    }

// Static constructor.
CWlanTimerServices* CWlanTimerServices::NewL()
    {
    DEBUG( "CWlanTimerServices::NewL()" );
    CWlanTimerServices* self =
        new (ELeave) CWlanTimerServices();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Symbian 2nd phase constructor can leave.
void CWlanTimerServices::ConstructL()
    {
    DEBUG( "CWlanTimerServices::ConstructL()" );

    CActiveScheduler::Add( this );
    User::LeaveIfError( iTimer.CreateLocal() );
    
    DEBUG( "CWlanTimerServices::ConstructL() - done" );
    }

// Destructor
CWlanTimerServices::~CWlanTimerServices()
    {
    DEBUG( "CWlanTimerServices::~CWlanTimerServices()" );

    Cancel();
    iTimer.Close();
    iRequests.Close();
    }

// ---------------------------------------------------------
// CWlanTimerServices::DoCancel
// Is called by CActive::Cancel()
// ---------------------------------------------------------
//
void CWlanTimerServices::DoCancel()
    {
    DEBUG( "CWlanTimerServices::DoCancel()" );
    iTimer.Cancel();
    }

// ---------------------------------------------------------
// CWlanTimerServices::CompareTimeouts
// ---------------------------------------------------------
//
TInt CWlanTimerServices::CompareTimeouts( const CTimeoutRequestEntry& aFirst,
                                          const CTimeoutRequestEntry& aSecond )
    {
    DEBUG( "CWlanTimerServices::CompareTimeouts()" );

    TInt ret( 0 );
    
    if( aFirst.At() == aSecond.At() )
        {
        ret = KEntriesAreEqual;
        }
    else if( aFirst.At() > aSecond.At() )
        {
        ret = KFirstEntryIsBigger;
        }
    else
        {
        ret = KFirstEntryIsSmaller;
        }
    
    DEBUG1( "CWlanTimerServices::CompareTimeouts() - returning %d", ret );
    return ret;
    }

// ---------------------------------------------------------
// CWlanTimerServices::CalculateInterval
// ---------------------------------------------------------
//
TInt CWlanTimerServices::CalculateInterval( TTimeIntervalMicroSeconds32& aInterval, TTime& aAt, TBool& aDoSeveralRounds ) const
    {
    TTime currentTime;
    currentTime.HomeTime();
    
#ifdef _DEBUG
    TBuf<KWlanTimerServicesMaxDateTimeStrLen> dbgString, timeoutDbgStr;
    TRAP_IGNORE( aAt.FormatL( timeoutDbgStr, KWlanTimerServicesDateTimeFormat ) );
    DEBUG1( "CWlanTimerServices::CalculateInterval() - timeout:  %S", &timeoutDbgStr );
    TRAP_IGNORE( currentTime.FormatL( dbgString, KWlanTimerServicesDateTimeFormat ) );
    DEBUG1( "CWlanTimerServices::CalculateInterval() - time now: %S", &dbgString );
#endif
    
    if( aAt < currentTime )
        {
        DEBUG( "CWlanTimerServices::CalculateInterval() - timeout happens in past, returning KErrArgument" );
        return KErrArgument;
        }
    
    TTimeIntervalDays days( aAt.DaysFrom( currentTime ) );
    DEBUG1( "CWlanTimerServices::CalculateInterval() - difference %d day(s)", days.Int() );
    if( days.Int() != 0 )
        {
        DEBUG( "CWlanTimerServices::CalculateInterval() - difference day or more, adjusting timeout to same day" );
        TDateTime timeout = aAt.DateTime();
        TDateTime now = currentTime.DateTime();
        timeout.SetYear( now.Year() );
        timeout.SetMonth( now.Month() );
        timeout.SetDay( now.Day() );
        aAt = TTime( timeout );
        if( aAt < currentTime )
            {
            DEBUG1( "CWlanTimerServices::CalculateInterval() - timeout would happen in past, adding %d day(s) to it", days.Int() );
            aAt += days;
            }
        
#ifdef _DEBUG
        TBuf<KWlanTimerServicesMaxDateTimeStrLen> newTimeoutDbgStr;
        TRAP_IGNORE( aAt.FormatL( newTimeoutDbgStr, KWlanTimerServicesDateTimeFormat ) );
        DEBUG1( "CWlanTimerServices::CalculateInterval() - timeout:  %S", &newTimeoutDbgStr );
#endif
        }
    
    TTimeIntervalMicroSeconds difference = aAt.MicroSecondsFrom( currentTime );
    
    TTimeIntervalMicroSeconds32 newInterval_32bit( KMaxTimerTimeout );
    
    TTimeIntervalMicroSeconds interval_64bit( newInterval_32bit.Int() );
    
    if ( difference < interval_64bit )
        {
        newInterval_32bit = static_cast<TTimeIntervalMicroSeconds32>( difference.Int64() );
        aDoSeveralRounds = EFalse;
        }
    else
        {
        DEBUG( "CWlanTimerServices::CalculateInterval() - requested timeout so big it requires several timer rounds" );
        aDoSeveralRounds = ETrue;
        }

    aInterval = newInterval_32bit;
    
    return KErrNone;
    }

// ---------------------------------------------------------
// CWlanTimerServices::StartTimer
// ---------------------------------------------------------
//
TInt CWlanTimerServices::StartTimer( TUint& aRequestId, TTime& aAt, MWlanTimerServiceCallback& aCb )
    {
    TTimeIntervalMicroSeconds32 newInterval( 0 );
    TBool doSeveralRounds( EFalse );
    
    TInt err = CalculateInterval( newInterval, aAt, doSeveralRounds );
    
    if( err != KErrNone ) 
        {
        DEBUG1( "CWlanTimerServices::StartTimer() - CalculateInterval() returned %d", err );
        return err;
        }
        
    CTimeoutRequestEntry entry( aAt, aCb, ++iRequestId, doSeveralRounds );
    
    TInt requestIdOfOriginallyFirstEntry( RequestIdOfFirstEntry() );
    
    TLinearOrder<CTimeoutRequestEntry> order( CWlanTimerServices::CompareTimeouts );
    err = iRequests.InsertInOrder( entry, order );
    
    if( err != KErrNone )
        {
        DEBUG1( "CWlanTimerServices::StartTimer() - InsertInOrder() returned %d", err );
        return err;
        }

    DEBUG1( "CWlanTimerServices::StartTimer() - new entry added to queue, request id %d", entry.RequestId() );
    
#ifdef _DEBUG
    for( TInt i = 0; i < iRequests.Count(); i++ )
        {
        DEBUG3( "CWlanTimerServices::StartTimer() - entry[%d]: request id %u, located @ 0x%08X",
                i,
                iRequests[i].RequestId(),
                &iRequests[i] );
        }
#endif
    
    if( requestIdOfOriginallyFirstEntry != iRequests[0].RequestId() )
        {
        // first request has changed, restart timer
        ActivateTimer( newInterval );
        }
    
    aRequestId = iRequestId;
    
    DEBUG( "CWlanTimerServices::StartTimer() - done" );
    return KErrNone;
    }

// ---------------------------------------------------------
// CWlanTimerServices::RequestIdOfFirstEntry   
// ---------------------------------------------------------
//
TInt CWlanTimerServices::RequestIdOfFirstEntry()
    {
    TInt idOfFirstEntry = KNoRequests;
    if( iRequests.Count() > 0 )
        {
        idOfFirstEntry = iRequests[KFirstItemIndex].RequestId();
        }
    DEBUG1( "CWlanTimerServices::RequestIdOfFirstEntry() - returning %d", idOfFirstEntry );
    return idOfFirstEntry;
    }    

// ---------------------------------------------------------
// CWlanTimerServices::RemoveRequest   
// ---------------------------------------------------------
//
void CWlanTimerServices::RemoveRequest( const TUint& aRequestId )
    {
    DEBUG1( "CWlanTimerServices::RemoveRequest( aRequestId: %u )", aRequestId );
    
    TInt numOfRequests( iRequests.Count() );
    DEBUG1( "CWlanTimerServices::RemoveRequest() - numOfRequests: %d", numOfRequests );
    
    for( TInt index( 0 ); index < numOfRequests; index++ )
        {
        DEBUG2( "CWlanTimerServices::RemoveRequest() - checking iRequests[%d].RequestId: %d", index, iRequests[index].RequestId() );
        if( iRequests[index].RequestId() == aRequestId )
            {
            DEBUG1( "CWlanTimerServices::RemoveRequest() - matching request id found, removing index: %d", index );
            iRequests.Remove( index );
            break;
            }
        }
    
    DEBUG( "CWlanTimerServices::RemoveRequest() - done" );
    }

// ---------------------------------------------------------
// CWlanTimerServices::StopTimer   
// ---------------------------------------------------------
//
void CWlanTimerServices::StopTimer( const TUint& aRequestId )
    {
    DEBUG1( "CWlanTimerServices::StopTimer( aRequestId: %u )", aRequestId );
    
    TInt numOfRequests( iRequests.Count() );
    DEBUG1( "CWlanTimerServices::StopTimer() - numOfRequests: %d", numOfRequests );

    TInt requestIdOfOriginallyFirstEntry( RequestIdOfFirstEntry() );
    
    DEBUG( "CWlanTimerServices::StopTimer() - calling RemoveRequest()" );
    RemoveRequest( aRequestId );
    DEBUG( "CWlanTimerServices::StopTimer() - RemoveRequest() returned" );

    if( iRequests.Count() > 0 )
        {
        if( requestIdOfOriginallyFirstEntry != iRequests[0].RequestId() )
            {
            DEBUG( "CWlanTimerServices::StopTimer() - first request changed, updating timer" );
            
            TTimeIntervalMicroSeconds32 newInterval( GetIntervalForNextRequest() );
                        
            ActivateTimer( newInterval );
            }
#ifdef _DEBUG
        else
            {
            DEBUG( "CWlanTimerServices::StopTimer() - first request hasn't changed" );
            }
#endif
        }
    else
        {
        DEBUG( "CWlanTimerServices::StopTimer() - request count is zero, cancelling timer" );
        Cancel();
        iTimer.Cancel();
        }
    DEBUG( "CWlanTimerServices::StopTimer() - done" );
    }    

// ---------------------------------------------------------
// CWlanTimerServices::GetIntervalForNextRequest
// ---------------------------------------------------------
//
TTimeIntervalMicroSeconds32 CWlanTimerServices::GetIntervalForNextRequest()
    {
    DEBUG( "CWlanTimerServices::GetIntervalForNextRequest()" );
    
    TTimeIntervalMicroSeconds32 interval( 0 );
    
    while( iRequests.Count() > 0  && 
            KErrArgument == CalculateInterval( interval,
                        iRequests[KFirstItemIndex].GetAt(),
                        iRequests[KFirstItemIndex].GetDoSeveralRounds() ))
        {
        DEBUG1( "CWlanTimerServices::GetIntervalForNextRequest() - request %u happens in past, time out and remove it", iRequests[KFirstItemIndex].RequestId() );
        TUint requestToBeRemoved = iRequests[KFirstItemIndex].RequestId();
        iRequests[KFirstItemIndex].Timeout();
        RemoveRequest( requestToBeRemoved );
        }
    
    DEBUG( "CWlanTimerServices::GetIntervalForNextRequest() - done" );
    return interval;
    }

// ---------------------------------------------------------
// CWlanTimerServices::RunL
// Timer has expired. 
// ---------------------------------------------------------
//
void CWlanTimerServices::RunL()
    {

    DEBUG1( "CWlanTimerServices::RunL() - iStatus: %d", iStatus.Int() );
    switch( iStatus.Int() )
        {
        case KErrCancel:
            {
            DEBUG( "CWlanTimerServices::RunL() - iStatus == KErrCancel -> timer cancelled" );
            break;
            }
        case KErrNone:
            {
            DEBUG( "CWlanTimerServices::RunL() - iStatus == KErrNone -> timeout occurred" );
            HandleTimeout();
            break;
            }
        default:
            {
            DEBUG( "CWlanTimerServices::RunL() - unexpected iStatus!" );
            ASSERT( 0 );
            }
        }
    DEBUG( "CWlanTimerServices::RunL() - done" );
    }

// ---------------------------------------------------------
// CWlanTimerServices::HandleTimeout
// ---------------------------------------------------------
//
void CWlanTimerServices::HandleTimeout()
    {
    DEBUG( "CWlanTimerServices::HandleTimeout()" );

    Cancel();
    iTimer.Cancel();
    
    // if requests exist...
    if( iRequests.Count() )
        {
        // if there's no need to do several rounds...
        if( !IsTimeInFuture( iRequests[KFirstItemIndex].At() ) )
        //if( iRequests[KFirstItemIndex].GetDoSeveralRounds() == EFalse )
            {
            // store id of the request to be timed out
            TUint requestIdToTimeout = iRequests[KFirstItemIndex].RequestId();
            DEBUG1( "CWlanTimerServices::HandleTimeout() - timeout request %u", requestIdToTimeout );
            iRequests[KFirstItemIndex].Timeout();
            DEBUG( "CWlanTimerServices::HandleTimeout() - make sure timed out request is removed" );
            RemoveRequest( requestIdToTimeout );
            }
        
        TTimeIntervalMicroSeconds32 newInterval( GetIntervalForNextRequest() );

        ActivateTimer( newInterval );

        }
#ifdef _DEBUG
    else
        {
        DEBUG( "CWlanTimerServices::HandleTimeout() - no requests" );
        }
#endif
    DEBUG( "CWlanTimerServices::HandleTimeout() - done" );
    }

// ---------------------------------------------------------
// CWlanTimerServices::HandleTimeout
// ---------------------------------------------------------
//
TBool CWlanTimerServices::IsTimeInFuture( const TTime& aAt ) const
    {
    DEBUG( "CWlanTimerServices::IsTimeInFuture()" );

    TBool ret( ETrue );
    
    TTime currentTime;
    currentTime.HomeTime();
    
#ifdef _DEBUG
    TBuf<KWlanTimerServicesMaxDateTimeStrLen> dbgString, timeoutDbgStr;
    TRAP_IGNORE( aAt.FormatL( timeoutDbgStr, KWlanTimerServicesDateTimeFormat ) );
    DEBUG1( "CWlanTimerServices::IsTimeInFuture() - time to check: %S", &timeoutDbgStr );
    TRAP_IGNORE( currentTime.FormatL( dbgString, KWlanTimerServicesDateTimeFormat ) );
    DEBUG1( "CWlanTimerServices::IsTimeInFuture() - time now:      %S", &dbgString );
#endif
    
    if( aAt <= currentTime )
        {
        DEBUG( "CWlanTimerServices::IsTimeInFuture() - time is not in the future" );
        ret = EFalse;
        }
    
    DEBUG1( "CWlanTimerServices::IsTimeInFuture() - returning %d", ret );
    return ret;
    }

// ---------------------------------------------------------
// CWlanTimerServices::ActivateTimer
// ---------------------------------------------------------
//
void CWlanTimerServices::ActivateTimer( const TTimeIntervalMicroSeconds32& aInterval )
    {
    DEBUG( "CWlanTimerServices::ActivateTimer()" );

    if( aInterval.Int() )
        {
        Cancel();
        iTimer.Cancel();
        iTimer.After( iStatus, aInterval );
        SetActive();
        DEBUG( "CWlanTimerServices::ActivateTimer() - timer active" );
        }   
    }

