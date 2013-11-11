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
* %version: 4 %
*/

#ifndef WLANTIMERSERVICES_H
#define WLANTIMERSERVICES_H

#include <e32base.h>
#include <e32std.h>
#include <e32cmn.h>

#include "wlantimerserviceinterface.h"

class CTimeoutRequestEntry : public CBase
    {
public:
    CTimeoutRequestEntry::CTimeoutRequestEntry(
    	TTime& aAt,
    	MWlanTimerServiceCallback& aCb,
    	TUint aRequestId,
    	TBool aDoSeveralRounds ) :
        iCb( aCb ),
        iAt( aAt ),
        iRequestId( aRequestId ),
        iDoSeveralRounds( aDoSeveralRounds )
    {}
    void Timeout()
        {
        iCb.OnTimeout();
        }
    const TTime& At() const
        {
        return iAt;
        }
    TTime& GetAt()
        {
        return iAt;
        }
    const TUint RequestId() const
        {
        return iRequestId;
        }
    TBool& GetDoSeveralRounds()
        {
        return iDoSeveralRounds;
        }
    
private:
    MWlanTimerServiceCallback& iCb;
    TTime iAt;
    TUint iRequestId;
    TBool iDoSeveralRounds;
    };

/**
 * Class providing timer services.
 *
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( CWlanTimerServices ) : public CActive, public MWlanTimerServices
    {
public: // Constructors and destructor

    /**
     * Static constructor.
     */
    static CWlanTimerServices* NewL();

    /**
     * Destructor.
     */
    virtual ~CWlanTimerServices();

public: // New functions

    /**
     * Start the timer
     *
     * @since S60 v5.2
     * @param aRequestId reference to request id
     * @param aAt reference to time when timeout should occur
     * @param aCb reference to callback object
     * @return error code
     */
    TInt StartTimer( TUint& aRequestId, TTime& aAt, MWlanTimerServiceCallback& aCb );

    /**
     * Stop the timer
     *
     * @since S60 v5.2
     * @param aRequestId Id of timer to stop
     */
    void StopTimer( const TUint& aRequestId );
    
    /**
     * Handles timer timeout.
     *
     * @since S60 v5.2
     */
    void HandleTimeout();
    
public: // Functions from base classes

    /**
    * From CActive Is executed when synchronous request is ready.
    */
    void RunL();

    /**
    * From CActive Cancel synchronous request.
    */
    void DoCancel();

private: // Functions

    /**
     * C++ default constructor.
     */
    CWlanTimerServices();

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();
    
    /**
     * Compare timeouts of two timeout request entries.
     *
     * @since S60 v5.2
     *
     * @param aFirst first entry.
     * @param aSecond second entry.
     * @return  0 if entries have same timeout value,
     *          1 if first entry has bigger timeout value,
     *         -1 if second entry has bigger timeout value
     */
    static TInt CompareTimeouts( const CTimeoutRequestEntry& aFirst,
                                 const CTimeoutRequestEntry& aSecond );
    
    /**
     * Calculates a microsecond interval to be used in timer.
     *
     * @since S60 v5.2
     *
     * @param aInterval interval will be written to here.
     * @param aAt time when timeout should occur. If more than a day apart from current time,
     *            day will be set to current day.
     * @param aDoSeveralRounds here will be written whether several timer rounds are needed.
     * @return  KErrArgument if aAt happens in past,
     *          KErrNone otherwise
     */
    TInt CalculateInterval( TTimeIntervalMicroSeconds32& aInterval,
                            TTime& aAt,
                            TBool& aDoSeveralRounds ) const;
    
    /**
     * Returns request id of the first request.
     *
     * @since S60 v5.2
     *
     * @return id of the first request
     */
    TInt RequestIdOfFirstEntry();
    
    /**
     * Removes a request from the request map.
     *
     * @since S60 v5.2
     *
     * @param aRequestId request to be removed.
     */
    void RemoveRequest( const TUint& aRequestId );
    
    /**
     * Get interval for next request.
     * Times out requests which happen in past and only
     * returns interval for next request happening in future.
     *
     * @since S60 v5.2
     * 
     * @return interval for the next request
     */
    TTimeIntervalMicroSeconds32 GetIntervalForNextRequest();
    
    /**
     * Is time in future.
     *
     * @since S60 v5.2
     * 
     * @return ETrue if time is in the future,
     *         EFalse otherwise
     */
    TBool IsTimeInFuture( const TTime& aAt ) const;
    
    /**
     * Activates timer
     *
     * @since S60 v5.2
     * 
     * @param aInterval interval for timer
     */
    void ActivateTimer( const TTimeIntervalMicroSeconds32& aInterval );
    
private: // Data
   
    /**
     * The timer.
     */
    RTimer iTimer;
        
    /**
     * All timeout requests.
     */
    RArray<CTimeoutRequestEntry> iRequests;
    
    /**
     * Unique identifier for requests.
     */
    TUint iRequestId;
    
    };



#endif // WLANTIMERSERVICES_H
