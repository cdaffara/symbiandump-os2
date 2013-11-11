/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the TWlanSignalPredictor class.
*
*/

/*
* %version: 7 %
*/

#ifndef WLANSIGNALPREDICTOR_H
#define WLANSIGNALPREDICTOR_H

#ifndef RD_WLAN_DDK
#include <wha_types.h>
#else
#include <wlanwha_types.h>
#endif

/**
 *  Signal Level Predictor machine.
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.2
 */
class WlanSignalPredictor
    {

public:

    /**
     * Constructor.
     */
    WlanSignalPredictor();

    /**
     * Destructor.
     */
    inline ~WlanSignalPredictor();

    /**
     * Starts the Signal Predictor.
     * If signal predictor is stopped, Start command will start it.
     * By default it is started.
     * @since S60 v3.2
     */    
    inline void Start();

    /**
     * Stops the Signal Predictor.
     *
     * @since S60 v3.2
     */    
    inline void Stop();

    /**
     * Configure the Signal Predictor operation.
     *
     * Note! If given param is 0, then old value remains.
     * @since S60 v3.2
     * @param aTimeToWarnLevel Specifies the time (in microseconds)
     *        how far into the future signal prediction is done.
     * @param aTimeToNextInd The minimum time difference (in 
     *        microseconds) between two signal loss prediction indications.
     * @param aRcpiWarnLevel If this RCPI level is predicted to be
     *        reached within the time specified by aSpTimeToCountPrediction,
     *        a signal loss prediction indication is sent. 
     */
    void ConfigureSignalPredictor( 
        TUint32    aTimeToWarnLevel,
        TUint32    aTimeToNextInd,
        WHA::TRcpi aRcpiWarnLevel );

    /**
     * Insert new rcpi information into the Signal Predictor.
     *
     * @since S60 v3.2
     * @param aTimestamp 32-bit timestamp in microseconds.  
     * @param aRcpi rcpi value from HW
     * @return ETrue if signal loss prediction indication should be sent
     */
    TBool InsertNewRcpi( 
        TUint32    aTimestamp, 
        WHA::TRcpi aRcpi ); 

    /**
     * Get the latest median value from Signal Predictor.
     *
     * @since S60 v3.2
     * @param aTimestamp 32-bit timestamp in milliseconds.  
     * @param aLatestMedian reference to the median.
     * @return Boolean about if median value is available. 
     */
    TBool GetLatestMedian( 
        TUint32 aTimestamp,
        TUint8& aLatestMedian ) const;

    /**
     * Get the latest rcpi from Signal Predictor.
     *
     * @since S60 v3.2
     * @param aTimestamp 32-bit timestamp in milliseconds.  
     * @param aLatestRcpi reference to the rcpi.
     * @return Boolean about if rcpi value is available. 
     */    
    TBool GetLatestRcpi( 
        TUint32 aTimestamp,
        TUint8& aLatestRcpi );

protected:

    /**
     * Checks if there are too old values in rcpi table.
     *
     * @since S60 v3.2
     * @param aTimestamp 32-bit timestamp in milliseconds.  
     */    
    void RemoveOldValues( 
        TUint32 aTimestamp );
    
    /**
     * removes an old value from rcpi table.
     *
     * @since S60 v3.2
     * @param aIndex Index of rcpi to be removed from rcpi table.
     */    
    void RemoveValue( 
        TUint8 aIndex );
    
    /**
     * Inserts new median value into median table.
     *
     * @since S60 v3.2
     * @param aTimestamp 32-bit timestamp in milliseconds.  
     * @param aMedian a new median value.
     * @return ETrue if signal loss prediction indication should be sent
     */    
    TBool InsertMedian( 
        TUint32 aTimestamp, 
        TUint8  aMedian );
    
    /**
     * Inserts new rcpi value into rcpi table.
     *
     * @since S60 v3.2
     * @param aTimestamp 32-bit timestamp in milliseconds.  
     * @param aRcpi a new rcpi value.
     * @return ETrue if signal loss prediction indication should be sent
     */    
    TBool InsertValue( 
        TUint32    aTimestamp, 
        WHA::TRcpi aRcpi );
    
    /**
     * Count the current median from the rcpi table.
     *
     * @since S60 v3.2
     * @return Current median value. 
     */    
    TUint8 GetCurrentMedian() const;
    
    /**
     * Counts the estimation about the signal behaviour.
     *
     * @since S60 v3.2
     * @return ETrue if signal loss prediction indication should be sent
     */    
    TBool GetPrediction();
    
    /**
     * Get the time interval between old and new times.
     *
     * @since S60 v3.2.
     * @param aOldTime 32-bit timestamp in milliseconds.
     * @param aNewTime 32-bit timestamp in milliseconds.
     * @return Time interval  in milliseconds.
     */    
    TUint32 Delay(
        TUint32 aOldTime, 
        TUint32 aNewTime ) const;

private:
   
    struct TValue  /* value info element */
        {
        TUint32    iTimestamp; /* timestamp */
        TUint8     iPrev;      /* next smaller rcpi value */
        TUint8     iNext;      /* next bigger  rcpi value */
        WHA::TRcpi iRcpi;      /* rcpi value */
        TUint8     iPad;       /* padding byte */
        };

    struct TLastRoam /* roaming info element */
        {
        TUint32 iTimestamp;/* timestamp */
        TBool   iDone;     /* valid or not */
        };

    /**
    * Only one iten
    */
    static const TUint KOneValue = 1;

    /**
    * Just dummy data pattern
    */
    static const TUint8 KDummy = 0x55;

    /**
    * Rcpi table size
    */
    static const TUint32 KValueCountMax = 25;

    /**
    * Median table size
    */
    static const TUint32 KMedianCountMax = 2;

    /**
    * Minimum time delay which is needed to accept next rcpi value
    */
    static const TUint32 KValueDelay = 10;

    /**
    * Minimum time delay which is needed to accept next median value
    */
    static const TUint32 KMedianDelay = 35;

    /**
    * Window size in milliseconds
    */
    static const TUint32 KWindowSizeDefault = 1000;

    /**
    * Time to indication sending in milliseconds
    */
    static const TUint32 KTimeToIndicationLevel = 1000;

    /**
    * Minimum time in milliseconds to next signal loss prediction indication sending
    */
    static const TUint32 KTimeToNextIndication = 1000;

    /**
    * Rcpi level when signal loss prediction indication generated
    */
    static const TUint8 KRoamLevel = 80;

    /**
    * Rcpi level above which, the indication is not sent
    */
    static const TUint8 KDiffRoamLevel = 120;

    /**
    * Minimum count of rcpi values to get signal loss prediction
    */
    static const TUint8 KMinValueCountForPrediction = 10;

    /**
    *  rcpi table 
    */
    TValue iLevelTable[ KValueCountMax ];

    /**
    *  median table 
    */
    TValue iMedianTable[ KMedianCountMax ];
    
    /**
    *  time interval to the indication level in milliseconds 
    */
    TUint32 iTimeToWarnLevel;

    /**
    *  observation time interval in milliseconds 
    */
    TUint32 iWindowSize;

    /**
    *  minimum time interval between two signal loss prediction 
    *  indications, in milliseconds 
    */
    TUint32 iTimeToNextInd;
    
    /**
    *  level when roam indication in generated 
    */
    TUint8 iWarningLevel;
    
    /**
    *  rcpi count in the rcpi table 
    */
    TUint8 iValueCount;
    
    /**
    *  median count in the median table 
    */
    TUint8 iMedianCount;
    
    /**
    *  index of the latest median item in the median table 
    */
    TUint8 iLatestMedian;
    
    /**
    *  index of the latest rcpi item in the rcpi table 
    */
    TUint8 iOldestValue;
    
    /**
    *  index of the latest rcpi item in the rcpi table 
    */
    TUint8 iLatestValue;
    
    /**
    *  index of the least rcpi item in the rcpi table 
    */
    TUint8 iLeastRcpi;
    
    /**
    *  index of the biggest rcpi item in the rcpi table 
    */
    TUint8 iMaxRcpi;

    /**
    *  Used value count, max value is KValueCountMax 
    */
    TUint8 iValueCountMax;

    /**
    *  Used value count, max value is KValueCountMax 
    */
    TUint8 iMinValueCountForPrediction;

    /**
    *  State information */
    TBool iRunning;

    /**
    *  Last signal loss prediction indication info 
    */
    TLastRoam iRoamed;
    };

#include "umacsignalpredictor.inl"

#endif // WLANSIGNALPREDICTOR_H
