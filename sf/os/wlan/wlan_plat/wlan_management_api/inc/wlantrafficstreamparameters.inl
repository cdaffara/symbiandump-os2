/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Parameter storage for a virtual traffic stream.
*
*/

/*
* %version: 1 %
*/

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline TWlanTrafficStreamParameters::TWlanTrafficStreamParameters(
    TUint8 aUserPriority ) :
    iParameterBitmap( EWlanParameterNone ),
    iType( EWlanTrafficStreamTrafficTypePeriodic),
    iId( 0 ),
    iDirection( EWlanTrafficStreamDirectionBidirectional ),
    iUserPriority( aUserPriority ),
    iNominalMsduSize( 0 ),
    iIsMsduSizeFixed( EFalse ),
    iMaximumMsduSize( 0 ),
    iMinimumServiceInterval( 0 ),
    iMaximumServiceInterval( 0 ),
    iInactivityInterval( 0 ),
    iSuspensionInterval( 0 ),
    iServiceStartTime( 0 ),
    iMinimumDataRate( 0 ),
    iMeanDataRate( 0 ),
    iPeakDataRate( 0 ),
    iMaximumBurstSize( 0 ),
    iDelayBound( 0 ),
    iMinimumPhyRate( TWlanRateNone ),
    iNominalPhyRate( TWlanRateNone ),
    iSba( 0 ),
    iIsAutomaticRetryAllowed( ETrue )
    {
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void TWlanTrafficStreamParameters::SetTrafficType(
    TWlanTrafficStreamTrafficType aType )
    {
    iParameterBitmap |= EWlanParameterType;
    iType = aType;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void TWlanTrafficStreamParameters::SetId(
    TUint8 aId )
    {
    iParameterBitmap |= EWlanParameterId;
    iId = aId;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void TWlanTrafficStreamParameters::SetDirection(
    TWlanTrafficStreamDirection aDirection )
    {
    iParameterBitmap |= EWlanParameterDirection;
    iDirection = aDirection;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void TWlanTrafficStreamParameters::SetNominalMsduSize(
    TUint16 aMsduSize,
    TBool aIsMsduSizeFixed )
    {
    iParameterBitmap |= EWlanParameterNominalMsduSize;
    iNominalMsduSize = aMsduSize;
    iIsMsduSizeFixed = aIsMsduSizeFixed;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void TWlanTrafficStreamParameters::SetMaximumMsduSize(
    TUint16 aMsduSize )
    {
    iParameterBitmap |= EWlanParameterMaximumMsduSize;
    iMaximumMsduSize = aMsduSize;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void TWlanTrafficStreamParameters::SetMinimumServiceInterval(
    TUint32 aServiceInterval )
    {
    iParameterBitmap |= EWlanParameterMinimumServiceInterval;
    iMinimumServiceInterval = aServiceInterval;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void TWlanTrafficStreamParameters::SetMaximumServiceInterval(
    TUint32 aServiceInterval )
    {
    iParameterBitmap |= EWlanParameterMaximumServiceInterval;
    iMaximumServiceInterval = aServiceInterval;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void TWlanTrafficStreamParameters::SetInactivityInterval(
    TUint32 aInactivityInterval )
    {
    iParameterBitmap |= EWlanParameterInactivityInterval;    
    iInactivityInterval = aInactivityInterval;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void TWlanTrafficStreamParameters::SetSuspensionInterval(
    TUint32 aSuspensionInterval )
    {
    iParameterBitmap |= EWlanParameterSuspensionInterval;
    iSuspensionInterval = aSuspensionInterval;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void TWlanTrafficStreamParameters::SetServiceStartTime(
    TUint32 aServiceStartTime )
    {
    iParameterBitmap |= EWlanParameterServiceStartTime;
    iServiceStartTime = aServiceStartTime;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void TWlanTrafficStreamParameters::SetMinimumDataRate(
    TUint32 aRate )
    {
    iParameterBitmap |= EWlanParameterMinimumDataRate;
    iMinimumDataRate = aRate;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void TWlanTrafficStreamParameters::SetMeanDataRate(
    TUint32 aRate )
    {
    iParameterBitmap |= EWlanParameterMeanDataRate;
    iMeanDataRate = aRate;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void TWlanTrafficStreamParameters::SetPeakDataRate(
    TUint32 aRate )
    {
    iParameterBitmap |= EWlanParameterPeakDataRate;
    iPeakDataRate = aRate;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void TWlanTrafficStreamParameters::SetMaximumBurstSize(
    TUint32 aBurstSize )
    {
    iParameterBitmap |= EWlanParameterMaximumBurstSize;
    iMaximumBurstSize = aBurstSize;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void TWlanTrafficStreamParameters::SetDelayBound(
    TUint32 aDelay )
    {
    iParameterBitmap |= EWlanParameterDelayBound;
    iDelayBound = aDelay;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void TWlanTrafficStreamParameters::SetMinimumPhyRate(
    TWlanRate aRate )
    {
    iParameterBitmap |= EWlanParameterMinimumPhyRate;
    iMinimumPhyRate = aRate;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void TWlanTrafficStreamParameters::SetNominalPhyRate(
    TWlanRate aRate )
    {
    iParameterBitmap |= EWlanParameterNominalPhyRate;
    iNominalPhyRate = aRate;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void TWlanTrafficStreamParameters::SetSba(
    TUint16 aRatio )
    {
    iParameterBitmap |= EWlanParameterSba;
    iSba = aRatio;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline void TWlanTrafficStreamParameters::SetAutomaticRetry(
    TBool aIsAllowed )
    {
    iParameterBitmap |= EWlanParameterIsRetryAllowed;    
    iIsAutomaticRetryAllowed = aIsAllowed;
    }
