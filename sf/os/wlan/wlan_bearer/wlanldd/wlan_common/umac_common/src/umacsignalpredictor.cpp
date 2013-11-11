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
* Description:   Implementation of the WlanSignalPredictor class.
*
*/

/*
* %version: 7 %
*/

#include "config.h"
#include "umacsignalpredictor.h"


static const TUint KThousand = 1000;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanSignalPredictor::WlanSignalPredictor()
    :
    iTimeToWarnLevel( KTimeToIndicationLevel ),
    iWindowSize( KWindowSizeDefault ),
    iTimeToNextInd( KTimeToNextIndication ),
    iWarningLevel( KRoamLevel ),
    iValueCount( 0 ),
    iMedianCount( 0 ),
    iLatestMedian( 0 ),
    iOldestValue( 0 ),
    iLatestValue( 0 ),
    iLeastRcpi( 0 ),
    iMaxRcpi( 0 ),
    iValueCountMax( KValueCountMax ),
    iMinValueCountForPrediction( KMinValueCountForPrediction ),
    iRunning( ETrue )
    {
    os_memset( iLevelTable, KDummy, sizeof( iLevelTable ) );
    os_memset( iMedianTable,KDummy, sizeof( iMedianTable ) );
    iRoamed.iDone = EFalse;

    OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanSignalPredictor::WlanSignalPredictor") );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanSignalPredictor::ConfigureSignalPredictor( 
    TUint32     aTimeToWarnLevel,
    TUint32     aTimeToNextInd,
    WHA::TRcpi  aRcpiWarnLevel )
    {
    OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanSignalPredictor::ConfigureSignalPredictor") );

    iTimeToWarnLevel = ( aTimeToWarnLevel ) ? aTimeToWarnLevel / KThousand : 
                                              iTimeToWarnLevel;

    iTimeToNextInd = ( aTimeToNextInd ) ? aTimeToNextInd / KThousand : 
                                          iTimeToNextInd;

    iWarningLevel = ( aRcpiWarnLevel ) ? aRcpiWarnLevel : iWarningLevel;
    
    OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanSignalPredictor::iTimeToWarnLevel: %d"), 
        iTimeToWarnLevel );
    OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanSignalPredictor::iTimeToNextInd: %d"), 
        iTimeToNextInd );
    OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanSignalPredictor::iWarningLevel: %d"), 
        iWarningLevel );
    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanSignalPredictor::InsertNewRcpi( 
    TUint32    aTimestamp, 
    WHA::TRcpi aRcpi )
    {
    // Check all preconditions for prediction

    // Check state of module SPC Stopped
    if ( EFalse == iRunning ) 
        {
        return EFalse;
        }
    
    // Convert the timestamp from microseconds to milliseconds
    aTimestamp = aTimestamp / KThousand;

    // If new timestamp is old enough from the latest one.
    if ( iValueCount && 
         (aTimestamp < (iLevelTable[iLatestValue].iTimestamp + KValueDelay) ) )
        {
        return EFalse;
        }

    // Remove too old values
    if ( iValueCount )
        {
        RemoveOldValues( aTimestamp );
        }

    // Insert the new value
    return InsertValue( aTimestamp, aRcpi );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanSignalPredictor::GetLatestMedian( 
    TUint32 aTimestamp,
    TUint8& aLastMedian ) const
    {
    // Convert the timestamp into milliseconds
    aTimestamp = aTimestamp / KThousand;

    if ( iMedianCount )
        {
        // Check first if the latest median is too old already.
        if ( iWindowSize >= 
             Delay( iMedianTable[iLatestMedian].iTimestamp, aTimestamp ) )
            {
            aLastMedian = iMedianTable[iLatestMedian].iRcpi;
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanSignalPredictor::GetLatestRcpi( 
    TUint32 aTimestamp,
    TUint8& aLatestRcpi )
    {
    // Convert the timestamp into milliseconds
    aTimestamp = aTimestamp / KThousand;

    RemoveOldValues( aTimestamp );
    
    if ( iValueCount )
        {
        aLatestRcpi = iLevelTable[iLatestValue].iRcpi;
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanSignalPredictor::RemoveOldValues( 
    TUint32 aTimestamp )
    {
    TBool oldFound( ETrue );
    
    while ( oldFound && iValueCount )
        {
        TUint32 deltaTime = 
            Delay( iLevelTable[iOldestValue].iTimestamp, aTimestamp );
        if ( deltaTime > iWindowSize )
            {
            RemoveValue( iOldestValue );
            }
        else
            {
            oldFound = EFalse;
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanSignalPredictor::RemoveValue( 
    TUint8 aIndex )
    {
    // The only one is removed.
    if ( KOneValue == iValueCount )
        {
        os_memset( iLevelTable, KDummy, sizeof( iLevelTable ) );
        iValueCount  = 0;
        iOldestValue = 0;
        iLatestValue = 0;
        iLeastRcpi   = 0;
        iMaxRcpi     = 0;
        return;
        }
    // The biggest rcpi is removed.
    else if ( aIndex == iMaxRcpi )
        {
        iLevelTable[iLevelTable[aIndex].iPrev].iNext = iValueCountMax;
        iMaxRcpi = iLevelTable[aIndex].iPrev;
        }
    // The least rcpi is removed.
    else if ( aIndex == iLeastRcpi )
        {
        iLevelTable[iLevelTable[aIndex].iNext].iPrev = iValueCountMax;
        iLeastRcpi = iLevelTable[aIndex].iNext;
        }
    // The middle rcpi is removed.
    else
        {
        iLevelTable[iLevelTable[aIndex].iPrev].iNext = iLevelTable[aIndex].iNext;
        iLevelTable[iLevelTable[aIndex].iNext].iPrev = iLevelTable[aIndex].iPrev;
        }
       
    os_memset( &iLevelTable[aIndex], KDummy, sizeof( TValue ) );
    
    iOldestValue++;
    if ( iOldestValue == iValueCountMax )
        {
        iOldestValue = 0;
        }
    iValueCount--;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanSignalPredictor::InsertValue( 
    TUint32    aTimestamp, 
    WHA::TRcpi aRcpi )
    {
    // If rcpi storage is full remove the oldest one to get space for new one
    if ( iValueCount == iValueCountMax )
        {
        RemoveValue( iOldestValue );
        }
    // Insert the new value into rcpi storage
    if ( !iValueCount )
        {
        // First item in rcpi storage.
        iOldestValue = 0;
        iLatestValue = 0;
        iLeastRcpi   = 0;
        iMaxRcpi     = 0;
        iLevelTable[iOldestValue].iTimestamp = aTimestamp;
        iLevelTable[iOldestValue].iRcpi      = aRcpi;
        iLevelTable[iOldestValue].iPrev      = iValueCountMax;
        iLevelTable[iOldestValue].iNext      = iValueCountMax;
        }
    else
        {
        // Already items in storage.
        TBool  biggerFound( EFalse );
        TUint8 tmpLeastIndex( iLeastRcpi );

        iLatestValue++;
        if ( iLatestValue == iValueCountMax )
            {
            iLatestValue = 0;
            }
        iLevelTable[iLatestValue].iTimestamp = aTimestamp;
        iLevelTable[iLatestValue].iRcpi      = aRcpi;

        // Insert this right place in Rcpi order.
        while ( !biggerFound )
            {
            if ( iLevelTable[tmpLeastIndex].iRcpi > aRcpi )
                {
                if ( tmpLeastIndex == iLeastRcpi )
                    {
                    iLevelTable[iLatestValue ].iPrev = iValueCountMax;
                    iLevelTable[iLatestValue ].iNext = tmpLeastIndex;
                    iLevelTable[tmpLeastIndex].iPrev = iLatestValue; 
                    iLeastRcpi = iLatestValue;
                    }
                else 
                    {
                    iLevelTable[iLatestValue ].iPrev = iLevelTable[tmpLeastIndex].iPrev;
                    iLevelTable[iLatestValue ].iNext = tmpLeastIndex;
                    iLevelTable[tmpLeastIndex].iPrev = iLatestValue; 
                    iLevelTable[iLevelTable[iLatestValue].iPrev].iNext = iLatestValue;
                    }
                biggerFound = ETrue;
                }
            else
                {
                if ( tmpLeastIndex == iMaxRcpi )
                    {
                    // Biggest rcpi value.
                    iLevelTable[iMaxRcpi].iNext     = iLatestValue;
                    iLevelTable[iLatestValue].iPrev = iMaxRcpi;
                    iLevelTable[iLatestValue].iNext = iValueCountMax;
                    iMaxRcpi    = iLatestValue;
                    biggerFound = ETrue;
                    }
                }
            tmpLeastIndex = iLevelTable[tmpLeastIndex].iNext;
            }
        }
    iValueCount++;

    OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanSignalPredictor::InsertValue Rcpi: %d"), 
        aRcpi );

    OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanSignalPredictor::InsertValue Timestamp: %d"), 
        aTimestamp );

    OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanSignalPredictor::InsertValue Rcpi count: %d"), 
        iValueCount );


	// Check first if the latest filtered value is old enough so that new
	// value can be added. If not, no new value is added.
	TUint32 FilterDelay = Delay( iMedianTable[iLatestMedian].iTimestamp, 
								 iLevelTable[iLatestValue].iTimestamp );

	if ( FilterDelay < KMedianDelay )
		{
		OsTracePrint( KRxFrame, (TUint8*) 
			("UMAC: WlanSignalPredictor::InsertValue: Too small delay between medians %d, No prediction. "), 
			FilterDelay );

		return EFalse;
		}

	// Calculate weight to be used in mean filter.
	// Median weight factor ratio 4/7 is experimental value. ratio 10/1 would 
    // mean in practise that NewMean = GetCurrentMedian()
	// and Ratio 1/1000 NewMean =  latest iLevelTable[iLatestValue].iRcpi 	
	TUint8 WeightFactor = ( iValueCount * 4 ) / 7;

	// Get new filtered value using floating mean filter.
	// Previous value (=LatestMedian at this point) is weighted using
	// value above.
	TUint8 NewMean = ( iMedianTable[iLatestMedian].iRcpi * WeightFactor + 
                       GetCurrentMedian() ) / ( WeightFactor + 1 );

	OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanSignalPredictor::InsertValue WeightFactor: %d"), 
        WeightFactor );

	OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanSignalPredictor::InsertValue NewMean: %d"), 
        NewMean );

    return InsertMedian( aTimestamp, NewMean );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint8 WlanSignalPredictor::GetCurrentMedian() const
    {
    
    TUint8 tempIdx( iLeastRcpi );

    if ( KOneValue == iValueCount )
        {
        return iLevelTable[tempIdx].iRcpi;
        }
    else if ( iValueCount % 2 )
        {
        for ( TUint8 i = 1; i <= iValueCount/2; i++ )
            {
            tempIdx = iLevelTable[tempIdx].iNext;
            }
        return iLevelTable[tempIdx].iRcpi;
        }
    else
        {
        for ( TUint8 i = 1; i < iValueCount/2; i++ )
            {
            tempIdx = iLevelTable[tempIdx].iNext;
            }
        return ( (iLevelTable[tempIdx].iRcpi + 
                 iLevelTable[iLevelTable[tempIdx].iNext].iRcpi)/2 );
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanSignalPredictor::InsertMedian( 
    TUint32 aTimestamp, 
    TUint8  aMedian )
    {
    TUint32 deltaTime( 0 );
    
    if ( !iMedianCount )
        {
        iMedianCount  = KOneValue;
        iLatestMedian = 0;
        iMedianTable[iLatestMedian].iTimestamp = aTimestamp;
        iMedianTable[iLatestMedian].iRcpi      = aMedian;
        }
    else
        {
        deltaTime = Delay( iMedianTable[iLatestMedian].iTimestamp, aTimestamp );
        if ( deltaTime > iWindowSize )
            {
            os_memset( iMedianTable,KDummy, sizeof( iMedianTable ) );
            iLatestMedian = 0;
            iMedianCount  = KOneValue;
            iMedianTable[iLatestMedian].iTimestamp = aTimestamp;
            iMedianTable[iLatestMedian].iRcpi      = aMedian;
            }
        else
            {
            iLatestMedian = iLatestMedian ? 0 : 1;
            iMedianCount  = 2;
            iMedianTable[iLatestMedian].iTimestamp = aTimestamp;
            iMedianTable[iLatestMedian].iRcpi      = aMedian;
            }
        }
    OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanSignalPredictor::InsertMedian Median: %d"), 
        aMedian );

    OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanSignalPredictor::InsertMedian MedianCount: %d"), 
        iMedianCount );

    return GetPrediction();
    }



// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanSignalPredictor::GetPrediction()
    {
    OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanSignalPredictor::GetPrediction") );

    // Roaming indication delay check.
    if ( iRoamed.iDone )
        {
        if ( iTimeToNextInd >= 
             Delay(iRoamed.iTimestamp, iMedianTable[iLatestMedian].iTimestamp) )
            {
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: WlanSignalPredictor::Latest roaming indication delay not past: No prediction") );

            // Old indication still not enough old.
            return EFalse;
            }
        else
            {
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: WlanSignalPredictor::Latest roaming indication delay past: Get prediction") );
            // Old indication old enough to send new one, clean the flag.
            iRoamed.iDone = EFalse;
            }
        }

    // One median value, is it under warning level.
    if ( iMinValueCountForPrediction > iValueCount )
        {
        OsTracePrint( KRxFrame, (TUint8*)
            ("UMAC: WlanSignalPredictor:: Count of rcpi values not enough:") );

		// If latest filtered value is below warning level
		if ( iMedianTable[iLatestMedian].iRcpi < iWarningLevel )
			{
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: WlanSignalPredictor:: Last median under Warning level: Send roam indication.") );
			}
		else
			{
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: WlanSignalPredictor:: No prediction.") );
			return EFalse;
			}
        }

    // Is the latest median bigger or equal than old one.
    else if ( iMedianTable[iLatestMedian].iRcpi >= 
              iMedianTable[iLatestMedian?0:1].iRcpi )
        {
        OsTracePrint( KRxFrame, (TUint8*)
            ("UMAC: WlanSignalPredictor:: Last median above old one: No roam indication.") );
        return EFalse;
        }
        // Otherwise count the prediction for roaming indication.
        else
            {
            TUint32 Drop( 0 );
            TUint32 DropPeriod( 0 );
            TUint32 DropToRoam( 0 );
            TUint32 TimeToRoam( 0 );

            Drop = iMedianTable[iLatestMedian ? 0 : 1].iRcpi - 
                   iMedianTable[iLatestMedian].iRcpi;

            DropToRoam = iMedianTable[iLatestMedian].iRcpi - iWarningLevel;

            DropPeriod = Delay( iMedianTable[iLatestMedian ? 0 : 1].iTimestamp, 
                                iMedianTable[iLatestMedian].iTimestamp);

            TimeToRoam = ( DropPeriod * DropToRoam ) / Drop;

            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: WlanSignalPredictor::GetPrediction TimeToRoam: %d ms."), 
                TimeToRoam );

		    // Level comparison changed to use
		    // filtered signal value. This significantly
		    // reduces amount of indications.
            if ( TimeToRoam	< iTimeToWarnLevel && 
                 KDiffRoamLevel	> iMedianTable[iLatestMedian].iRcpi )
                {
                OsTracePrint( KRxFrame, (TUint8*) 
                    ("UMAC: WlanSignalPredictor::All conditions ok. Send roam indication.")  );
                }
             else
                {
                 OsTracePrint( KRxFrame, (TUint8*) 
                    ("UMAC: WlanSignalPredictor::Some conditions nok. No roam indication.")  );
                return EFalse;
                }
            }
    
    // If we get this point, roaming indication is sent. Save the indication info.
    iRoamed.iDone = ETrue;
    iRoamed.iTimestamp = iMedianTable[iLatestMedian].iTimestamp;
    
    // Start roaming
    return ETrue;
    }




// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint32 WlanSignalPredictor::Delay( 
    TUint32 aOldTime, 
    TUint32 aNewTime ) const
    {
    TUint32 delay;
    if ( aOldTime > aNewTime )
        {
        delay = ~aOldTime + 1 + aNewTime;
        }
    else
        {
        delay = aNewTime - aOldTime;    
        }
    return delay;
    }





