/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanConnectContext class.
*
*/

/*
* %version: 22 %
*/

#ifndef WLANCONNECTCONTEXT_H
#define WLANCONNECTCONTEXT_H

#include "umac_types.h"
#include "umacinternaldefinitions.h"

#ifndef RD_WLAN_DDK
#include <wha.h>
#else
#include <wlanwha.h>
#endif

#include "802dot11.h"
#include "umacoidmsg.h"
#include "wha_mibDefaultvalues.h"

/**
 *  WLAN connection context
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class WlanConnectContext
    {
    
public: 

    /**
    * C++ default constructor.
    */
    inline WlanConnectContext();
    
    /**
    * Destructor.
    */
    virtual inline ~WlanConnectContext();

private:

    // Prohibit copy constructor 
    WlanConnectContext( const WlanConnectContext& );
    // Prohibit assigment operator 
    WlanConnectContext& operator= ( const WlanConnectContext& );

public: // Data

    /** items from Connect mgmt command; begin */
    /** 
    * pointer to a Beacon or Probe Response frame body from the network
    * to connect to. This pointer is temporarily stored here in the roaming case
    * Not own.  
    */
    const TUint8*           iScanResponseFrameBody;
    /** length of the Beacon / Probe Response frame body (above) */
    TUint16                 iScanResponseFrameBodyLength;
    /** 
    * the IE(s) to be included into the (re-)association request.
    * NULL, if none to be included 
    * Not own.  
    */
    const TUint8*           iIeData;
    /** length of IE(s) to be included into the (re-)association request */
    TUint8                  iIeDataLength;
    /** BSSID where to connect/connectted to */
    ::TMacAddress           iBSSID;
    /** SSID where to connect/connectted to */
    TSSID                   iSSID;
    /** authentication algorithm number to be used */
    TUint16                 iAuthAlgorithmNbr;      
    /** used privacy mode */
    TEncryptionStatus       iEncryptionStatus;  
    /** Pairwise cipher to be used */
    TWlanCipherSuite        iPairwiseCipher;
    /** Radio Measurement settings */
    TBool                   iRadioMeasurement;
    /** adhoc or infrastructure mode */
    WHA::TOperationMode     iOperationMode;
    /** roaming pairwise cipher key data context. Not own */
    const TPairwiseKeyData* iRoamingPairwiseKey;
    /** items from Connect mgmt command; end */
    
    /** group key type inserted */
    WHA::TKeyType           iGroupKeyType;
    /** pairwise key type inserted */
    WHA::TKeyType           iPairWiseKeyType;

    /** items initially set from scaninfo - begin */
    /** minimum basic rate of the network */
    TUint32                 iNwsaMinBasicRate;
    /** maxium basic rate of the network*/
    TUint32                 iNwsaMaxBasicRate;
    /** basic rate set of the network*/
    TUint32                 iNwsaBasicRateSet;
    /** specifies to used channel of the network */
    WHA::TChannelNumber     iChannelNumber;
    /** items initially set from scaninfo - end */

    /** the desired dot11 power management mode in BSS mode */
    WHA::TPsMode            iDesiredDot11PwrMgmtMode;
    /** the current dot11 power management mode in BSS mode */
    WHA::TPsMode            iCurrentDot11PwrMgmtMode;
    
    /** the desired PS mode config */
    TDot11PsModeWakeupSetting iDesiredPsModeConfig;
    /** 
    * the desired dot11 power management mode in BSS mode 
    * from WLAN Mgmt Client perspective 
    */
    WHA::TPsMode            iClientDesiredDot11PwrMgtMode;
    
    /** the desired Light PS mode config from WLAN Mgmt Client perspective */ 
    TDot11PsModeWakeupSetting iClientLightPsModeConfig;
    
    /** the desired Deep PS mode config from WLAN Mgmt Client perspective */ 
    TDot11PsModeWakeupSetting iClientDeepPsModeConfig;    
    
    /** beacon interval */
    TUint32                 iBeaconInterval;         
    /** ATIM window */
    TUint32                 iAtim;

    /** holds the flags defined below it */
    TUint32                 iFlags;
    /** if network has short slottime bit set */
    static const TUint32 KUseShortSlotTime     = ( 1 << 0 );
    /** if network has protection bit set we must use ctstoself */
    static const TUint32 KProtectionBitSet     = ( 1 << 1 );
    /** QoS supported by the network */
    static const TUint32 KQosEnabled           = ( 1 << 2 );
    /** does AP demand to use short preamble or not */
    static const TUint32 KUseShortPreamble     = ( 1 << 3 );
    /** 
    * if we are roaming we will perform reassociation 
    * instead of association.
    */
    static const TUint32 KReassociate          = ( 1 << 4 );
    /** U-APSD supported by the network */
    static const TUint32 KUapsdEnabled         = ( 1 << 5 );
    /** multicast filtering is not allowed */
    static const TUint32 KMulticastFilteringDisAllowed = ( 1 << 6 );      
    /** network has sent ERP IE in beacon/probe response */
    static const TUint32 KErpIePresent         = ( 1 << 7 );      
    /** 
    * the AP has sent us either a disassociation or
    * a deauthentication frame. 
    */
    static const TUint32 KDisassociatedByAp    = ( 1 << 8 );      
    /** 
    * set if WLAN mgmt client has requested Voice AC to be made 
    * both trigger and delivery enabled 
    */
    static const TUint32 KUapsdRequestedForVoice = ( 1 << 9 );
    /** 
    * set if WLAN mgmt client has requested Video AC to be made 
    * both trigger and delivery enabled 
    */
    static const TUint32 KUapsdRequestedForVideo = ( 1 << 10 );
    /** 
    * set if WLAN mgmt client has requested Best Effort AC to be made
    * both trigger and delivery enabled
    */
    static const TUint32 KUapsdRequestedForBestEffort = ( 1 << 11 );
    /** 
    * set if WLAN mgmt client has requested Background AC to be made
    * both trigger and delivery enabled 
    */
    static const TUint32 KUapsdRequestedForBackground = ( 1 << 12 );
    /** 
    * set if Voice AC is both trigger and delivery enabled 
    */
    static const TUint32 KUapsdUsedForVoice = ( 1 << 13 );
    /** 
    * set if Video AC is both trigger and delivery enabled 
    */
    static const TUint32 KUapsdUsedForVideo = ( 1 << 14 );
    /** 
    * set if Best Effort AC is both trigger and delivery enabled
    */
    static const TUint32 KUapsdUsedForBestEffort = ( 1 << 15 );
    /** 
    * set if Background AC is both trigger and delivery enabled 
    */
    static const TUint32 KUapsdUsedForBackground = ( 1 << 16 );
    /** 
    * set if finding suitable time to perform AP tests has been started
    */
    static const TUint32 KApTestOpportunitySeekStarted = ( 1 << 17 );
    /** 
    * set if suitable time to perform AP tests has been indicated to WLAN 
    * Mgmt Client
    */
    static const TUint32 KApTestOpportunityIndicated = ( 1 << 18 );
    /** set if the target/current nw supports HT operation (802.11n) */
    static const TUint32 KHtSupportedByNw = ( 1 << 19 );
    /** 
    * set if WLAN Mgmt Client has disabled dynamic power mode management
    */
    static const TUint32 KDynamicPwrModeMgmtDisabled = ( 1 << 20 );

    
    /** AP advertised supported rates IE */
    SSupportedRatesIE   iApSupportedRates;
    /** AP advertised extended supported rates IE */
    SExtendedSupportedRatesIE iApExtendedSupportedRates;
    /** AP capability information fixed field */
    SCapabilityInformationField iCapabilityInformation;                
    /** association ID */
    TUint32             iAid;

    /** 
    * these are the rates we advertise 
    * to AP in assoc-request frame. Extended rates
    * used also if more than eight rates supported
    */
    SSupportedRatesIE   iOurSupportedRates;
    SExtendedSupportedRatesIE iOurExtendedSupportedRates;

    /** 
    * The following four arrays contain the Access Category
    * parameters for the current network.
    * Indexed with WHA::TQueueId values
    */
    TCwMinVector       iCwMin;
    TCwMaxVector       iCwMax;
    TAifsVector        iAIFS;
    TTxOplimitVector   iTxOplimit;    
    /** 
    * WMM Parater Set Count, which is a 4 bit value when coming
    * from the network
    * Has value KWmmParamSetNotDefined if WMM parameters have not 
    * been defined
    */
    TUint8 iWmmParamSetCount;
    /** 
    * WMM IE which we use to inform about our WMM (QoS) settings
    */
    STxWmmIE iOurWmmIe;
    /** 
    * Is admission control mandatory (i.e. required by the AP) for 
    * the ACs. Indexed with a WHA::TQueueId value
    */
    TAcmVector iAdmCtrlMandatory;    
    /** 
    * Bitmask of WHA rates which both us and the nw support
    */
    TUint32 iRateBitMask;
    /*
    * provided by WLAN mgmt client.
    * Defines the rate class(es) to use for frame Tx.
    * If WLAN PDD does not support as many rate classes as are specified
    * here, only the rate class specified for ELegacy 
    * queue, which shall be the first policy in this array, will be used.
    */
    TTxRatePolicy iRatePolicy;
    /*
    * provided by WLAN mgmt client.
    * Defines the rate class to be used for every Tx queue / QoS Access 
    * Category.
    * TQueueId is used to index this array.
    */
    TQueue2RateClass iQueue2RateClass;
    /*
    * provided by WLAN mgmt client.
    * Defines the Max Tx rate which will be initially used to transmit using 
    * the rate class in question. If the specified rate is not supported, 
    * the next lower supported rate from the rate policy will be used 
    * instead.
    * Only the first numOfPolicyObjects values (see TTxRatePolicy) from 
    * the beginning of the array are relevant.
    */
    TInitialMaxTxRate4RateClass iInitialMaxTxRate4RateClass;
    /** 
    * provided by WLAN mgmt client.
    * Defines the rate class(es) to use for frame Tx when the WLAN vendor
    * implementation handles Tx rate adaptation.
    * If WLAN PDD does not support as many rate classes as are specified
    * in this command message, only the rate class specified for ELegacy 
    * queue, which shall be the first class in this array, will be used.
    * The mapping defined in queue2RateClass applies also to these classes.
    * The shortRetryLimit and the longRetryLimit values defined as part of 
    * the ratePolicy are relevant also with the auto rate policy. 
    * Only the first numOfPolicyObjects values (see TTxRatePolicy) from 
    * the beginning of the array are relevant.
    */
    TTxAutoRatePolicy iAutoRatePolicy;
    /**
    * provided by WLAN mgmt client.
    * Defines the HT MCS sets to use for frame Tx when communicating
    * with a HT network.
    * If WLAN PDD does not support as many MCS sets as are specified
    * here, only the MCS set specified for ELegacy 
    * queue, which shall be the first MCS set in this array, will be used.
    * The mapping defined in queue2RateClass applies also to these MCS sets.
    * The shortRetryLimit and the longRetryLimit values defined as part of 
    * the iRatePolicy are relevant also with the MCS sets. 
    * Only the first numOfPolicyObjects values (see TTxRatePolicy) from 
    * the beginning of the array are relevant.
    */
    THtMcsPolicy iHtMcsPolicy;
    /** 
    * the ID of the special Tx rate policy which can be used to transmit
    * frames whose successful delivery is of special importance.
    * Value zero means that such a policy is not available. 
    */
    TUint8 iSpecialTxRatePolicyId;
    /**
    * Maximum U-APSD Service Period length. Indicates the max number of MSDUs and 
    * MMPDUs the WMM AP may deliver to a WMM STA during any service period 
    * triggered by the WMM STA.
    * This value is provided by WLAN mgmt client
    */
    TQosInfoUapsdMaxSpLen iUapsdMaxSpLen;
    /** 
    * HT Capabilities element of the target/current network informing its 
    * static HT capabilities
    */
    SHtCapabilitiesIE iNwHtCapabilitiesIe;
    /** 
    * HT Operation element of the target/current network informing its 
    * dynamic HT configuration
    */
    SHtOperationIE iNwHtOperationIe;
    };

#include "umacconnectcontext.inl"

#endif // WLANCONNECTCONTEXT_H
