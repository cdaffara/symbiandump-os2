/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanDot11InfrastructureMode class
*
*/

/*
* %version: 35 %
*/

#ifndef WLANDOT11INFRASTRUCTUREMODE_H
#define WLANDOT11INFRASTRUCTUREMODE_H

#include "UmacDot11Associated.h"

class WlanElementLocator;

/**
* This is a state where STA has succesfully 
* authenticated and associated to a infrastructuremode network
*/
class WlanDot11InfrastructureMode : public WlanDot11Associated
    {         
public:

    /**
    * Connect (authenticate and associate) to a BSS.
    * As we are already connected, this means actually roaming.
    *
    * @since S60 3.1
    * @param aCtxImpl statemachine context
    * @param aSSID Name of the network
    * @param aBSSID BSSID of the access point
    * @param aAuthAlgorithmNbr Authentication algorithm number to be used
    * @param aEncryptionStatus Used encryption level
    * @param aIsInfra ETrue when connecting to infrastructure network
    * @param aScanResponseFrameBodyLength length of the scan response frame body
    * @param aScanResponseFrameBody scan response frame body
    * @param aIeData The IE(s) to be included into the (re-)association request.
    *              NULL, if none to be included
    * @param aIeDataLength Length of the IE(s) to be included into the 
    *                    (re-)association request
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    virtual TBool Connect(
        WlanContextImpl& aCtxImpl,
        const TSSID& aSSID,                 
        const TMacAddress& aBSSID,          
        TUint16 aAuthAlgorithmNbr,      
        TEncryptionStatus aEncryptionStatus,
        TBool aIsInfra,
        TUint16 aScanResponseFrameBodyLength,
        const TUint8* aScanResponseFrameBody,
        const TUint8* aIeData,
        TUint16 aIeDataLength );

    /**
    * Connect (authenticate and associate) to a BSS.
    * As we are already connected, this means actually roaming.
    *
    * @since S60 3.1
    * @param aCtxImpl statemachine context
    * @param aSSID              Name of the network.
    * @param aBSSID             BSSID of the access point.
    * @param aAuthAlgorithmNbr  Authentication algorithm number to be used
    * @param aEncryptionStatus  Used encryption level.
    * @return KErrNone = command completed successfully, 
    *         any other = failure
    */
    TBool Connect(
        WlanContextImpl& aCtxImpl,
        const TSSID& aSSID,                 
        const TMacAddress& aBSSID,          
        TUint16 aAuthAlgorithmNbr,      
        TEncryptionStatus aEncryptionStatus );
                      
protected:

    /**
    * C++ default constructor.
    */
    WlanDot11InfrastructureMode() {};

    /**
    * Destructor.
    */
    virtual ~WlanDot11InfrastructureMode() {};

    virtual void DoSetTxMpduDaAddress( 
        SDataFrameHeader& aDataFrameHeader, 
        const TMacAddress& aMac ) const;

    virtual TBool DoIsRxFrameSAourAddress( 
        WlanContextImpl& aCtxImpl,
        const SDataFrameHeader& aFrameHeader,
        const SAmsduSubframeHeader* aSubFrameHeader ) const;

    virtual TBool DoIsValidAddressBitCombination(
        const SDataFrameHeader& aFrameHeader ) const;

    virtual void DoBuildEthernetFrame(
        TDataBuffer& aBuffer,
        const SDataMpduHeader& aDot11DataMpdu,
        const TUint8* aStartOfEtherPayload,
        TUint aEtherPayloadLength,
        TBool aAmsdu,
        TUint8* aCopyBuffer );

    /**
    * Indicates Consecutive 802.11 Power Management Mode Set Failures
    * to WLAN Mgmt Client; if necessary
    * 
    * @since S60 5.0
    * @param aCtxImpl global statemachine context
    */
    virtual void DoConsecutivePwrModeSetFailuresIndication( 
        WlanContextImpl& aCtxImpl );
    
private:

    /**
    * Set trigger level for RCPI trigger. 
    * @param aCtxImpl statemachine context
    * @param aRcpiTrigger RCPI trigger level
    * @return ETrue if a state change occurred in the state machine 
    *         EFalse otherwise
    */
    virtual TBool SetRcpiTriggerLevel(
        WlanContextImpl& aCtxImpl,
        TUint32 aRcpiTrigger);               

    /**
    * Updates the RCPI Trigger Level MIB
    *
    * @since Series 60 3.1
    * @param aCtxImpl Global statemachine context
    * @param aRcpiTrigger the value to assign to the MIB
    * @param aCompleteManagementRequest ETrue if this request came from the
    *                                   WLAN Mgmt client and that request
    *                                   needs to be completed.
    *                                   EFalse otherwise.
    * @return ETrue if state transition occurred, EFalse otherwise
    */
    virtual TBool SetRcpiTriggerLevelMib(
        WlanContextImpl& aCtxImpl,
        TUint32 aRcpiTrigger,
        TBool aCompleteManagementRequest );

    /**
    * Validates our current ERP parameters
    *
    * @param aCtxImpl statemachine context
    * @param aElementLocator
    */
    void ValidateErpParams( 
        WlanContextImpl& aCtxImpl,
        WlanElementLocator& aElementLocator );

    /**
    * Validates our current QoS AC parameters
    *
    * @param aCtxImpl statemachine context
    * @param aElementLocator
    */
    void ValidateAcParams( 
        WlanContextImpl& aCtxImpl,
        WlanElementLocator& aElementLocator );

    /**
    * Validates our current HT operation parameters
    *
    * @param aCtxImpl statemachine context
    * @param aElementLocator
    */
    void ValidateHtBssOperationParams( 
        WlanContextImpl& aCtxImpl,
        WlanElementLocator& aElementLocator );

    /**
    * Called upon receiving a deauthentication frame
    * @param aCtxImpl statemachine context
    * @param aBuffer pointer to the beginning of the Rx buffer allocated
    *        for the frame
    */
    virtual void OnDeauthenticateFrameRx( 
        WlanContextImpl& aCtxImpl, 
        TUint8* aBuffer );

    /**
    * Called upon receiving a disassociation frame
    * @param aCtxImpl statemachine context
    * @param aBuffer pointer to the beginning of the Rx buffer allocated
    *        for the frame
    */
    virtual void OnDisassociateFrameRx( 
        WlanContextImpl& aCtxImpl,
        TUint8* aBuffer );
    
    /**
    * Called upon receiving a beacon type frame
    *
    * @param aCtxImpl statemachine context
    * @param aRcpi RCPI of the frame
    * @param aBuffer pointer to the beginning of the Rx buffer allocated
    *        for the frame
    */
    virtual void OnBeaconFrameRx( 
        WlanContextImpl& aCtxImpl,
        const TAny* aFrame,
        const TUint32 aLength,
        WHA::TRcpi aRcpi,
        TUint8* aBuffer );

    /**
    * Disconnect STA from current network.
    * @param aCtxImpl statemachine context
    * @return KErrNone = command completed successfully, 
    * any other = failure
    */
    virtual TBool Disconnect( WlanContextImpl& aCtxImpl );

    /**
    * Write for management data
    * The frame to be sent needs to be in 802.3 format
    * Data is copied to NIC's Tx queue. That normally succeeds, but in 
    * the event the Tx queue is full copying is deferred to a 
    * more appropriate time -> TxCompleted function
    * @param aCtxImpl statemachine context
    * @param aDataBuffer meta header of the frame to be transmitted
    */
    virtual void TxMgmtData( 
        WlanContextImpl& aCtxImpl,
        TDataBuffer& aDataBuffer );

    /**
    * Scan all available networks. 
    * This SW module decides is background scan or foreground scan used
    * @param aCtxImpl statemachine context
    * @param aMode passive or active scan see #TScanMode
    * @param aSSID Network that APs are scanned. Can be broadcast SSID.
    * @param aScanRate Rate that is used in active scanning 
    * @param aMinChannelTime min. time to listen beacons/probe responses on 
    *                        a channel
    * @param aMaxChannelTime max. time to listen beacons/probe responses on 
    *                        a channel
    * @param aSplitScan if ETrue, use split scan
    * @return KErrNone = command completed successfully, 
    * any other = failure
    */
    virtual TBool RealScan(
        WlanContextImpl& aCtxImpl,
        TScanMode aMode,                    
        const TSSID& aSSID,                 
        TUint32 aScanRate,                    
        SChannels& aChannels,
        TUint32 aMinChannelTime,            
        TUint32 aMaxChannelTime,
        TBool aSplitScan );                          
    
    virtual void DoRcpiIndication( 
        WlanContextImpl& aCtxImpl,
        WHA::TRcpi aRcpi );

    /**
    * Voice Call Entry timer timeout function
    *
    * @since S60 v3.2
    * @param aCtxImpl statemachine context
    * @return ETrue if a state change occurred in the state machine
    *         EFalse otherwise
    */
    virtual TBool OnVoiceCallEntryTimerTimeout( WlanContextImpl& aCtxImpl );

    /**
    * Null timer timeout function
    *
    * @since S60 v3.2
    * @param aCtxImpl statemachine context
    * @return ETrue if a state change occurred in the state machine
    *         EFalse otherwise
    */
    virtual TBool OnNullTimerTimeout( WlanContextImpl& aCtxImpl );

    /**
    * No Voice timer timeout function
    *
    * @since S60 v3.2
    * @param aCtxImpl statemachine context
    * @return ETrue if a state change occurred in the state machine
    *         EFalse otherwise
    */
    virtual TBool OnNoVoiceTimerTimeout( WlanContextImpl& aCtxImpl );

    /**
    * To be called upon Keep Alive Timer timeout
    *
    * @since S60 3.2
    * @param aCtxImpl statemachine context
    * @return ETrue if a state change occurred in the state machine
    *         EFalse otherwise
    */
    virtual TBool OnKeepAliveTimerTimeout( WlanContextImpl& aCtxImpl );

    virtual TBool OnWlanWakeUpIntervalChange( WlanContextImpl& aCtxImpl );

    /**
     * Request to send a Null Data Frame
     *
     * @since S60 v3.2
     * @param aCtxImpl statemachine context
     * @param aQosNull ETrue if a QoS Null Data frame should be transmitted
     *                 EFalse if a regular Null Data frame should be transmitted
     * @return ETrue if the send request was successfully submitted
     *         EFalse otherwise
     */
    virtual TBool TxNullDataFrame( 
        WlanContextImpl& aCtxImpl, 
        TBool aQosNull );


    // Prohibit copy constructor.
    WlanDot11InfrastructureMode( 
        const WlanDot11InfrastructureMode& );
    // Prohibit assigment operator.
    WlanDot11InfrastructureMode& operator= ( 
        const WlanDot11InfrastructureMode& );              
    };

#endif      // WLANDOT11INFRASTRUCTUREMODE_H
