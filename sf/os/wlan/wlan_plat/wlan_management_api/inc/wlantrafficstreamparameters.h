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

#ifndef WLANTRAFFICSTREAMPARAMETERS_H
#define WLANTRAFFICSTREAMPARAMETERS_H

#include <wlanmgmtcommon.h>

/**
 * Parameter storage for a virtual traffic stream.
 *
 * This class is used to store the parameters of a virtual traffic stream.
 * When instantiated, it sets all the parameters to the default values so
 * the client application only needs to set the parameters it specifically
 * wants to alter.
 *
 * Any parameter left to its default value means that the WLAN subsystem
 * is free to select a suitable value.
 */
NONSHARABLE_CLASS( TWlanTrafficStreamParameters )
    {

public:

    /**
     * Bit definitions for parameters.
     */
    enum TWlanParameter
        {
        EWlanParameterNone                      = 0x00000000,
        EWlanParameterType                      = 0x00000001,
        EWlanParameterId                        = 0x00000002,
        EWlanParameterDirection                 = 0x00000004,
        EWlanParameterNominalMsduSize           = 0x00000008,
        EWlanParameterMaximumMsduSize           = 0x00000010,
        EWlanParameterMinimumServiceInterval    = 0x00000020,
        EWlanParameterMaximumServiceInterval    = 0x00000040,        
        EWlanParameterInactivityInterval        = 0x00000080,
        EWlanParameterSuspensionInterval        = 0x00000100,
        EWlanParameterServiceStartTime          = 0x00000200,
        EWlanParameterMinimumDataRate           = 0x00000400,
        EWlanParameterMeanDataRate              = 0x00000800,
        EWlanParameterPeakDataRate              = 0x00001000,
        EWlanParameterMaximumBurstSize          = 0x00002000,
        EWlanParameterDelayBound                = 0x00004000,
        EWlanParameterMinimumPhyRate            = 0x00008000,
        EWlanParameterNominalPhyRate            = 0x00010000, 
        EWlanParameterSba                       = 0x00020000,
        EWlanParameterIsRetryAllowed            = 0x00040000
        };

    /**
     * Constructor.
     *
     * @param aUserPriority User Priority (0 - 7) admission is requested for.
     */
    inline TWlanTrafficStreamParameters(
        TUint8 aUserPriority );

    /**
     * Set the traffic pattern used.
     *
     * @param aType Traffic pattern used.
     */
    inline void SetTrafficType(
        TWlanTrafficStreamTrafficType aType );

    /**
     * Set the traffic stream ID used.
     *
     * @param aId Traffic Stream ID (0 - 7).
     */
    inline void SetId(
        TUint8 aId );

    /**
     * Set the traffic stream direction used.
     *
     * @param aDirection Traffic stream direction.
     */
    inline void SetDirection(
        TWlanTrafficStreamDirection aDirection );

    /**
     * Set the nominal MSDU size used.
     *
     * @param aMsduSize The nominal size of packets in bytes (0 - 0x7FFF).
     * @param aIsMsduSizeFixed Whether the size of packets is fixed.
     */
    inline void SetNominalMsduSize(
        TUint16 aMsduSize,
        TBool aIsMsduSizeFixed );

    /**
     * Set the maximum MSDU size used.
     *
     * @param aMsduSize The maximum size of packets in bytes (0 - 0xFFFF).
     */
    inline void SetMaximumMsduSize(
        TUint16 aMsduSize );

    /**
     * Set the minimum service interval used.
     *
     * @param aServiceInterval The minimum interval between packets in microseconds.
     */
    inline void SetMinimumServiceInterval(
        TUint32 aServiceInterval );

    /**
     * Set the maximum service interval used.
     *
     * @param aServiceInterval The maximum interval between packets in microseconds.
     */
    inline void SetMaximumServiceInterval(
        TUint32 aServiceInterval );

    /**
     * Set the inactivity interval used.
     *
     * @param aInactivityInterval The minimum amount of microseconds that may elapse
     *                            without arrival or transfer of packets belonging
     *                            to this traffic stream before the traffic stream
     *                            is deleted.
     */
    inline void SetInactivityInterval(
        TUint32 aInactivityInterval );

    /**
     * Set the suspension interval used.
     *
     * @param aSuspensionInterval The minimum amount of microseconds that may elapse
     *                            without arrival or transfer of packets belonging
     *                            to this traffic stream before polling for the
     *                            traffic stream is stopped.
     */
    inline void SetSuspensionInterval(
        TUint32 aSuspensionInterval );

    /**
     * Set the service start time used.
     *
     * @param aServiceStartTime The time in microseconds when the service period starts.
     */
    inline void SetServiceStartTime(
        TUint32 aServiceStartTime );

    /**
     * Set the minimum data rate used.
     *
     * @param aRate The lowest data rate in bits per second used for packets
     *              belonging to this traffic stream.
     */
    inline void SetMinimumDataRate(
        TUint32 aRate );

    /**
     * Set the average data rate used.
     *
     * @param aRate The average data rate in bits per second used for packets
     *              belonging to this traffic stream.
     */
    inline void SetMeanDataRate(
        TUint32 aRate );

    /**
     * Set the maximum data rate used.
     *
     * @param aRate The maximum data rate in bits per second used for packets
     *              belonging to this traffic stream.
     */
    inline void SetPeakDataRate(
        TUint32 aRate );

    /**
     * Set the maximum burst size used.
     *
     * @param aBurstSize The maximum burst in bytes that can arrive or be transmitted at
     *                   the peak data rate.
     */
    inline void SetMaximumBurstSize(
        TUint32 aBurstSize );

    /**
     * Set the maximum transport delay used.
     *
     * @param aDelay The maximum amount of microseconds allowed to transport
     *               a packet belonging to this traffic stream.
     */
    inline void SetDelayBound(
        TUint32 aDelay );

    /**
     * Set the minimum WLAN data rate used.
     *
     * @param aRate The minimum WLAN data rate in bits per second used for
     *              this traffic stream.
     */
    inline void SetMinimumPhyRate(
        TWlanRate aRate );

    /**
     * Set the nominal WLAN data rate used.
     *
     * @param aRate The nominal WLAN data rate in bits per second used for
     *              this traffic stream.
     */
    inline void SetNominalPhyRate(
        TWlanRate aRate );

    /**
     * Set the SBA ratio used.
     *
     * @param aRatio The excess ratio of actual over-the-air bandwidth to
     *               ideal bandwidth used for transporting packets belonging
     *               to this traffic stream.
     */
    inline void SetSba(
        TUint16 aRatio );

    /**
     * Set the whether re-creation of traffic streams is allowed.
     *
     * @param aIsAllowed Whether re-creation of traffic streams that have been
     *                   deleted or refused by the current WLAN access point
     *                   can be automatically retried.
     */
    inline void SetAutomaticRetry(
        TBool aIsAllowed );

public: // data

    /**
     * Bitmap of parameters that have been set.
     */
    TUint32 iParameterBitmap;

    /** 
     * Traffic pattern used.
     */
    TWlanTrafficStreamTrafficType iType;
    /**
     * Traffic stream ID used.
     */
    TUint8 iId;
    /**
     * Traffic stream direction used.
     */
    TWlanTrafficStreamDirection iDirection;
    /** 
     * User Priority used.
     */
    TUint8 iUserPriority;
    /**
     * Nominal MSDU size used.
     */
    TUint16 iNominalMsduSize;
    /**
     * Whether the size of packets is fixed.
     */
    TBool iIsMsduSizeFixed;
    /**
     * Maximum MSDU size used.
     */
    TUint16 iMaximumMsduSize;
    /**
     * Minimum service interval used.
     */
    TUint32 iMinimumServiceInterval;
    /**
     * Maximum service interval used.
     */
    TUint32 iMaximumServiceInterval;
    /**
     * Inactivity interval used.
     */
    TUint32 iInactivityInterval;
    /** 
     * Suspension interval used.
     */
    TUint32 iSuspensionInterval;
    /**
     * Service start time used.
     */
    TUint32 iServiceStartTime;
    /**
     * Minimum data rate used.
     */
    TUint32 iMinimumDataRate;
    /**
     * Average data rate used.
     */
    TUint32 iMeanDataRate;
    /**
     * Maximum data rate used.
     */
    TUint32 iPeakDataRate;
    /**
     * Maximum burst size used.
     */
    TUint32 iMaximumBurstSize;
    /**
     * Maximum transport delay used.
     */
    TUint32 iDelayBound;
    /**
     * Minimum WLAN data rate used.
     */
    TWlanRate iMinimumPhyRate;
    /**
     * Nominal WLAN data rate used.
     */
    TWlanRate iNominalPhyRate;
    /**
     * SBA ratio used.
     */
    TUint16 iSba;
    /**
     * Whether re-creation of traffic streams is allowed.
     */
    TBool iIsAutomaticRetryAllowed;
    };

#include "wlantrafficstreamparameters.inl"

#endif // WLANTRAFFICSTREAMPARAMETERS_H
