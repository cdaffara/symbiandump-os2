/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of WlanConnectContext inline methods
*
*/

/*
* %version: 16 %
*/

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline WlanConnectContext::WlanConnectContext() :
    iScanResponseFrameBody( NULL ),
    iScanResponseFrameBodyLength( 0 ),
    iIeData( NULL ),
    iIeDataLength( 0 ),
    iBSSID( KZeroMacAddr ),
    iAuthAlgorithmNbr( 0 ),
    iEncryptionStatus( EEncryptionDisabled ),
    iPairwiseCipher( EWlanCipherSuiteNone ),
    iRadioMeasurement( EFalse ),
    iOperationMode( WHA::EBSS ),
    iRoamingPairwiseKey( NULL ),
    iGroupKeyType( WHA::EKeyNone ), 
    iPairWiseKeyType( WHA::EKeyNone ),
    iNwsaMinBasicRate( 0 ), 
    iNwsaMaxBasicRate( 0 ),
    iNwsaBasicRateSet( 0 ),
    iChannelNumber( 0 ),
    iDesiredDot11PwrMgmtMode( static_cast<WHA::TPsMode>( 0 ) ),
    iCurrentDot11PwrMgmtMode( static_cast<WHA::TPsMode>( 0 ) ),
    iClientDesiredDot11PwrMgtMode( static_cast<WHA::TPsMode>( 0 ) ),
    iBeaconInterval( 0 ),
    iAtim( 0 ), 
    iFlags( 0 | 
            KUapsdRequestedForVoice | 
            KUapsdRequestedForVideo  | 
            KUapsdRequestedForBestEffort |
            KUapsdRequestedForBackground ),
    iAid( 0 ),
    iWmmParamSetCount( KWmmParamSetNotDefined ),
    iRateBitMask( 0 ),
    iSpecialTxRatePolicyId( 0 ),
    iUapsdMaxSpLen( EMaxSpLenAllFrames )
    {
    os_memset( &iSSID, 0, sizeof( iSSID ) );
    iDesiredPsModeConfig.iWakeupMode = EWakeUpIntervalAllBeacons;
    iDesiredPsModeConfig.iListenInterval = 0 ;
    iClientLightPsModeConfig.iWakeupMode = EWakeUpIntervalAllBeacons;
    iClientLightPsModeConfig.iListenInterval = 0 ;
    iClientDeepPsModeConfig.iWakeupMode = EWakeUpIntervalAllDtims;
    iClientDeepPsModeConfig.iListenInterval = 0;
    os_memset( iCwMin, 0, sizeof( iCwMin ) );
    os_memset( iCwMax, 0, sizeof( iCwMax ) );
    os_memset( iAIFS, 0, sizeof( iAIFS ) );
    os_memset( iTxOplimit, 0, sizeof( iTxOplimit ) );
    os_memset( iAdmCtrlMandatory, 0, sizeof( iAdmCtrlMandatory ) );
    
    // first clear ...
    os_memset( &iRatePolicy, 0, sizeof( iRatePolicy ) );    
    // .. then initialize Tx rate policy with the default value, which 
    // specifies only a single rate class
    // Note that the types WHA::StxRatePolicy and TTxRatePolicy are
    // effectively equivalent upto - and including - the definition of the
    // 1st rate class, so this memcpy is ok
    os_memcpy(
        &iRatePolicy, 
        &WHA::KTxRatePolicyMibDefault, 
        sizeof( WHA::KTxRatePolicyMibDefault ) );

    // initialize every Tx queue to use the 1st rate class, at index 0
    os_memset( 
        &iQueue2RateClass, 
        0, 
        sizeof( iQueue2RateClass ) );

    os_memset( 
        &iInitialMaxTxRate4RateClass, 
        0, 
        sizeof( iInitialMaxTxRate4RateClass ) );

    // initialize initial max Tx rate for 1st rate class
    iInitialMaxTxRate4RateClass[0] = WHA::KRate1Mbits;
    
    os_memset( 
        &iAutoRatePolicy,
        0,
        sizeof( iAutoRatePolicy ) );
    
    os_memset( 
        &iHtMcsPolicy,
        0,
        sizeof( iHtMcsPolicy ) );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanConnectContext::~WlanConnectContext()
    {
    iScanResponseFrameBody = NULL;
    iIeData = NULL;
    iRoamingPairwiseKey = NULL;
    }
