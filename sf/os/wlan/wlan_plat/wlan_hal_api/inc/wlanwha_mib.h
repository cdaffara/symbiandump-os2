/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Part of WLAN HAL API
*
*/

/*
* %version: 8 %
*/

#ifndef WHA_MIB_H
#define WHA_MIB_H

#include <wlanwhanamespace.h>
#include <wlanwha_types.h>

NAMESPACE_BEGIN_WHA

struct Sdot11StationId
    {
    TMacAddress iAddr;
    TUint8      iReserved[2];
    };

struct Sdot11MaxReceiveLifeTime
    {
    TUint32 iDot11MaxReceiveLifeTime;
    };

struct Sdot11SlotTime 
    {
    TUint32 iDot11SlotTime;
    };

struct Sdot11GroupAddressesTable 
    {    
    enum { KHeaderSize = sizeof( TBool ) + sizeof( TUint32 ) };

    TBool           iEnable;       
    TUint32         iNumOfAddrs;
    TMacAddress     iAddrData[1]; // addresses start here
    };

struct Sdot11WepDefaultKeyId 
    {
    TPrivacyKeyId   iDot11WepDefaultKeyId;
    TUint8          iReserved[3];
    };

struct Sdot11CurrentTxPowerLevel 
    {
    TPowerLevel iDot11CurrentTxPowerLevel;
    };

struct Sdot11RTSThreshold 
    {
    TUint32 iDot11RTSThreshold;
    };

struct SctsToSelf 
    {
    TBool iCtsToSelf;
    };

struct SarpIpAddressTable
    {
    TBool          iEnable;
    TIpv4Address   iIpV4Addr; 
    };

struct StemplateFrame
    {    
    enum { KHeaderSize = sizeof( TTemplateType ) + sizeof( TRate ) + sizeof( TUint32 ) };

    TTemplateType   iFrameType;
    TRate           iInitialTransmitRate;
    TUint32         iLength;
    TUint8          iTemplateData[1]; // the template data starts here
    };

struct SrxFilter 
    {
    enum { KPromiscous          = (1 << 0) };
    enum { KBssId               = (1 << 1) }; /** deprecated */

    TUint32 iRxFilter;
    };

struct SDefaultIeFilterTable
    {
    enum { KTrackChange     = (1 << 0) };
    enum { KTrackPresence   = (1 << 1) };

    TUint8      iIe;
    TUint8      iTreatMeant;
    };

struct SIe221FilterTable
    {
    enum { KTrackChange     = (1 << 0) };
    enum { KTrackPresence   = (1 << 1) };

    enum { KOuiLen          = 3 };

    TUint8          iIe;
    TUint8          iTreatMeant;
    TUint8          iOui[KOuiLen];     
    TUint8          iType;
    TUint16         iVersion;
    };

struct SbeaconFilterIeTable 
    {    
    enum { KHeaderSize = sizeof( TUint32 ) };

    TUint32     iNumofElems;
    TUint8      iIeTable[1]; // IE table details start here
    };

struct SbeaconFilterEnable 
    {
    TBool   iEnable;
    TUint32 iCount; /** deprecated */
    };

struct SsleepMode 
    {
    TSleepMode iMode;
    };

struct SwlanWakeUpInterval 
    {
    TWlanWakeUpInterval iMode;
    TUint8              iListenInterval;
    TUint8              iReserved[2];
    };

struct SbeaconLostCount 
    {
    TUint32 iLostCount;
    };

struct SrcpiThreshold 
    {
    TRcpi   iThreshold;
    TUint8  iReserved[3];
    };

struct SstatisticsTable 
    {
    TUint8  iSnr;
    TRcpi   iRcpi;
    TUint8  iReserved[2];
    };

struct SibssPsConfig /** deprecated */
    {
    TIbssPsMode iMode;
    };

struct StxRateClass
    {
    TUint8  iTxPolicy54;
    TUint8  iTxPolicy48;
    TUint8  iTxPolicy36;
    TUint8  iTxPolicy33;
    TUint8  iTxPolicy24;
    TUint8  iTxPolicy22;
    TUint8  iTxPolicy18;
    TUint8  iTxPolicy12;
    TUint8  iTxPolicy11;
    TUint8  iTxPolicy9;
    TUint8  iTxPolicy6;
    TUint8  iTxPolicy5_5;
    TUint8  iTxPolicy2;
    TUint8  iTxPolicy1;

    TUint8  iShortRetryLimit;
    TUint8  iLongRetryLimit;

    TUint32 iFlags;
    };

struct StxRatePolicy 
    {    
    enum { KHeaderSize = sizeof( TUint32 ) };

    TUint32         iNumOfPolicyObjects;
    StxRateClass    iTxRateClass[1]; // rate classes start here
    };

struct ScountersTable                    
    {
    TUint32 iPlcpError;
    TUint32 iFcsError;
    };

struct Sdot11Preamble
    {
    TPreambleType iType;
    TUint8        iReserved[3];
    };

struct ShtCapabilities
    {
    TBool           iHtSupport;
    TMacAddress     iPeerMac;
    THtRxStbc       iRxStbc;
    THtMaxAmpdu     iMaxAmpduLength;
    THtCapabilities iPeerFeatures;
    THtMcsSet       iMcsSet;
    THtAmpduSpacing iAmpduSpacing;
    THtMcsFeedback  iMcsFeedback;
    TUint32	        iTxBeamFormingCapab;
    TUint8          iAntennaSelCapab;
    TUint8          iReserved[3];
    };

struct ShtBssOperation
    {
    enum { KNonGreenfieldPresent    = ( 1 << 0 ) };
    enum { KPcoActive               = ( 1 << 2 ) };
    enum { KRifsPermitted           = ( 1 << 3 ) };
    enum { KDualCtsProtReq          = ( 1 << 4 ) };
    enum { KSecondaryBeaconTx       = ( 1 << 5 ) };
    enum { KLsigTxopProtection      = ( 1 << 6 ) };

    TUint32	            iInfo;
    THtMcsSet           iMcsSet;
    THtProtection       iOpMode;
    THtSecChannelOffset	iSecChOffset;
    THtChannelWidth	    iApChWidth;
    TUint8              iReserved[3];
    };

struct ShtSecondaryBeacon
    {
    THtSecBeacon iSecBeacon;
    TUint8       iReserved[3];
    };

struct ShtBlockAckConfigure
    {
    static const TUint8 KTid0 = ( 1 << 0 );
    static const TUint8 KTid1 = ( 1 << 1 );
    static const TUint8 KTid2 = ( 1 << 2 );
    static const TUint8 KTid3 = ( 1 << 3 );
    static const TUint8 KTid4 = ( 1 << 4 );
    static const TUint8 KTid5 = ( 1 << 5 );
    static const TUint8 KTid6 = ( 1 << 6 );
    static const TUint8 KTid7 = ( 1 << 7 );

    TUint8 iTxBlockAckUsage;
    TUint8 iRxBlockAckUsage;
    TUint8 iReserved[2];
    };

struct StxAutoRatePolicy
    {
    TRate     iBAndGRates;
    THtMcsSet iMcsSet;
    TUint8    iTxRateClassId;
    TUint8    iShortRetryLimit;
    TUint8    iLongRetryLimit;
    TUint8    iReserved[3];
    };

struct SetherTypeFilter
    {    
    enum { KHeaderSize = sizeof( TFilterMode ) + sizeof( TUint8 ) };

    TFilterMode     iFilterMode;       
    TUint8          iNumOfTypes;
    TUint16         iTypeData[1]; // Ethernet type codes start here
    };

struct SbroadcastUdpFilter
    {    
    enum { KHeaderSize = sizeof( TFilterMode ) + sizeof( TUint8 ) };

    TFilterMode     iFilterMode;       
    TUint8          iNumOfPorts;
    TUint16         iPortData[1]; // UDP port numbers start here
    };

NAMESPACE_END_WHA

#endif // WHA_MIB_H
