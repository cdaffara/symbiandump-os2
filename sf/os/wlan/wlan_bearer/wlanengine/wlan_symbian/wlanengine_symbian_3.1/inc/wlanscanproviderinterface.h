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
* Description:  This abstract class defines WLAN scan provider interface.
*
*/

/*
* %version: 10 %
*/


#ifndef WLANSCANPROVIDERINTERFACE_H
#define WLANSCANPROVIDERINTERFACE_H

#include <e32def.h>

/**
 * @brief Interface definition for WLAN Scan Result Provider API.
 *
 * This class defines the methods used for issuing and cancelling Scan requests.
 *
 * @since S60 v5.2
 */
class MWlanScanResultProvider
    {

public:

    /**
     * Issue scan request.
     *
     * @since S60 v5.2
     * @param aMaxDelay Maximum amount of seconds the client is willing to wait for
     *                  the availability results. The valid range is from 0 to 1200
     *                  seconds.
     */
    virtual void Scan( const TUint& aMaxDelay ) = 0;

    /**
     * Cancel pending scan request.
     *
     * @since S60 v5.2
     */        
    virtual void CancelScan() = 0;

    /**
     * Publish WLAN background scan interval via P&S.
     *
     * @since S60 v5.2
     * @param aInterval Background scan interval in seconds.
     */
    virtual void PublishBgScanInterval( TUint32& aInterval ) = 0;
    };



/**
 * @brief Interface definition for WLAN Background Scan Provider API.
 *
 * This class defines the methods WLAN engine uses to communicate with 
 * WLAN Background Scan provider.
 *
 * @since S60 v5.2
 */
class MWlanBgScanProvider
    {

public:
    
    enum TWlanBgScanState
        {
        EWlanBgScanOff,
        EWlanBgScanOn
        };
    
    struct TWlanBgScanSettings
        {
        TUint32 backgroundScanInterval;
        TUint psmServerMode;
        TUint bgScanPeakStartTime;
        TUint bgScanPeakEndTime;
        TUint32 bgScanIntervalPeak;
        TUint32 bgScanIntervalOffPeak;
        
        TWlanBgScanSettings() :
            backgroundScanInterval( 0 ),
            psmServerMode( 0 ),
            bgScanPeakStartTime( 0),
            bgScanPeakEndTime( 0 ),
            bgScanIntervalPeak( 0 ),
            bgScanIntervalOffPeak( 0 )
            { }
        
        TWlanBgScanSettings( TUint32 aBackgroundScanInterval,
                            TUint aPsmServerMode,
                            TUint aBgScanPeakStartTime,
                            TUint aBgScanPeakEndTime,
                            TUint32 aBgScanIntervalPeak,
                            TUint32 aBgScanIntervalOffPeak ) :
            backgroundScanInterval( aBackgroundScanInterval ),
            psmServerMode( aPsmServerMode ),
            bgScanPeakStartTime( aBgScanPeakStartTime),
            bgScanPeakEndTime( aBgScanPeakEndTime ),
            bgScanIntervalPeak( aBgScanIntervalPeak ),
            bgScanIntervalOffPeak( aBgScanIntervalOffPeak )
            { }
        
        };

    /**
     * Destructor.
     */
    virtual ~MWlanBgScanProvider() {};
    
    /**
     * Called when Scan is complete.
     *
     * @since S60 v5.2
     */
    virtual void ScanComplete() = 0;
        
    /**
     * Called to set BG scan state (on/off).
     *
     * @since S60 v5.2
     * 
     * @param aState New WLAN state
     */
    virtual void SetBgScanState( const TWlanBgScanState &aState ) = 0;
    
    /**
     * Whether background scan is enabled.
     *
     * @since S60 v5.2
     * @return True if background scan is enabled,
     *         False otherwise.
     */
    virtual TBool IsBgScanEnabled() = 0;
    
    /**
     * Notification about changed settings.
     *
     * @since S60 v5.2
     * 
     * @param aSettings new settings to be taken into use
     */
    virtual void NotifyChangedSettings( TWlanBgScanSettings& aSettings ) = 0;

    /**
     * Start aggressive background scanning.
     *
     * @since S60 v5.2
     * @param aInterval interval for aggressive scan, in seconds
     * @param aTimeout how long aggressive scanning is carried out
     */
    virtual void StartAggressiveBgScan(
        TUint32& aInterval,
        TUint32& aTimeout ) = 0;
    };


#endif // WLANSCANPROVIDERINTERFACE_H
