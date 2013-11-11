/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanDot11Associated class
*
*/

/*
* %version: 47 %
*/

#ifndef WLANDOT11ASSOCIATED_H
#define WLANDOT11ASSOCIATED_H

#include "UmacDot11State.h"
#include "FrameXferBlock.h"
#include "umacinternaldefinitions.h"

class TWlanUserTxDataCntx;

/**
* This is a state where STA has entered to state where 
* it is able send MPDU's to a network 
*/
class WlanDot11Associated : public WlanDot11State
    {
public:

    /**
    * Transmit a protocol stack frame
    * 
    * The frame to be sent needs to be in 802.3 format
    * @param aCtxImpl global state machine context
    * @param aDataBuffer meta header of the frame to be transmitted
    * @param aMore ETrue if another frame is also ready to be transmitted
    *              EFalse otherwise
    */
    virtual TBool TxData( 
        WlanContextImpl& aCtxImpl,
        TDataBuffer& aDataBuffer,
        TBool aMore );

    virtual TAny* RequestForBuffer( 
        WlanContextImpl& aCtxImpl,
        TUint16 aLength );

    virtual void ReceivePacket( 
        WlanContextImpl& aCtxImpl, 
        WHA::TStatus aStatus,
        const void* aFrame,
        TUint16 aLength,
        WHA::TRate aRate,
        WHA::TRcpi aRcpi,
        WHA::TChannelNumber aChannel,
        TUint8* aBuffer,
        TUint32 aFlags );
           
protected:

    /**
    * C++ default constructor.
    */
        WlanDot11Associated() {};

    /**
    * Destructor.
    */
    virtual ~WlanDot11Associated() {};

    virtual void DoSetTxMpduDaAddress( 
        SDataFrameHeader& aDataFrameHeader, 
        const TMacAddress& aMac ) const = 0;
    
    virtual TBool DoIsRxFrameSAourAddress( 
        WlanContextImpl& aCtxImpl,
        const SDataFrameHeader& aFrameHeader,
        const SAmsduSubframeHeader* aSubFrameHeader = NULL ) const = 0;

    /**
    * Called to check is To/From DS bit combination valid

    * @return ETrue valid combination, EFalse else
    */
    virtual TBool DoIsValidAddressBitCombination(
        const SDataFrameHeader& aFrameHeader ) const = 0;

    virtual void DoBuildEthernetFrame(
        TDataBuffer& aBuffer,
        const SDataMpduHeader& aDot11DataMpdu,
        const TUint8* aStartOfEtherPayload,
        TUint aEtherPayloadLength,
        TBool aAmsdu,
        TUint8* aCopyBuffer ) = 0;

    /**
    * Handles a received frame having a proprietary SNAP header
    *
    * @since S60 3.1
    * @param aBuffer meta header for the Rx frame
    * @param aQosData ETrue if this is a QoS data frame
    * @param aFrame pointer to the start of the frame
    * @param aSubFrameHeader Pointer to the beginning of the subframe header.
    *        NULL if the MSDU is not a subframe within an A-MSDU
    * @param aLength frame length
    * @param aDecryptHeaderLen length of the decrypt header of the 
    *        frame (e.g. IV etc.)
    * @param aDecryptTrailerLen length of the decrypt trailer of the 
    *        frame (e.g. MIC etc.)
    * @param aHtControlLen length of the HT Control field of the 
    *        frame's MAC header; or zero if field is not present
    * @param aCopyBuffer If not NULL, the frame is part of an A-MSDU
    *        and it needs to be copied to the buffer denoted by this
    *        pointer
    */
    void HandleProprietarySnapRxFrame( 
        TDataBuffer& aBuffer,
        TBool aQosData, 
        const TAny* const aFrame, 
        const SAmsduSubframeHeader* aSubFrameHeader,
        TUint aLength,
        TUint aDecryptHeaderLen,
        TUint aDecryptTrailerLen,
        TUint aHtControlLen,
        TUint8* aCopyBuffer ) const;
    
    /**
    * Called upon receiving a beacon frame
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
    * Called upon receiving a probe response frame
    * @param aCtxImpl statemachine context
    * @param aRcpi RCPI of the frame
    * @param aBuffer pointer to the beginning of the Rx buffer allocated
    *        for the frame
    */
    virtual void OnProbeResponseFrameRx( 
        WlanContextImpl& aCtxImpl,
        const TAny* aFrame,
        const TUint32 aLength,
        WHA::TRcpi aRcpi,
        TUint8* aBuffer );

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
    * Determines if the received 802.11 Data MPDU - excluding the contained
    * MSDU or A-MSDU - is valid
    * @param aLength Length of the MPDU
    * @param aFrameHeader MAC header of the MPDU
    * @param aQosData ETrue if the MPDU is a QoS Data frame
    *        EFalse othwerwise
    * @param aAmsdu ETrue if the MPDU contains an A-MSDU
    *        EFalse othwerwise
    * @param aHtControlLen Length of the HT Control field in the MPDU's MAC
    *        header. Zero if the field is not present
    * @param aSecurityHeaderLen Length of the security header of the MPDU.
    *        Zero if the payload is not encrypted
    * @param aSecurityTrailerLen Length of the security trailer of the MPDU.
    *        Zero if the payload is not encrypted
    * @return ETrue if the MPDU is valid, EFalse otherwise 
    */
    inline TBool RxDataMpduValid(
        const TUint32 aLength,
        const SDataFrameHeader& aFrameHeader,
        TBool aQosData,
        TBool aAmsdu,
        TUint aHtControlLen,
        TUint aSecurityHeaderLen,
        TUint aSecurityTrailerLen ) const;

    /**
    * Determines the minimum length that the received 802.11 Data MPDU must
    * have so that we are able to process is
    * @param aQosData ETrue if the frame is a QoS Data frame
    *        EFalse othwerwise
    * @param aAmsdu ETrue if the MPDU contains an A-MSDU
    *        EFalse othwerwise
    * @param aHtControlLen Length of the HT Control field in the MPDU's MAC
    *        header. Zero if the field is not present
    * @param aSecurityHeaderLen Length of the security header of the MPDU.
    *        Zero if the payload is not encrypted
    * @param aSecurityTrailerLen Length of the security trailer of the MPDU.
    *        Zero if the payload is not encrypted
    * @return   
    */
    inline TUint MinAcceptableRxDataMpduLen(
        TBool aQosData,
        TBool aAmsdu,
        TUint aHtControlLen,
        TUint aSecurityHeaderLen,
        TUint aSecurityTrailerLen ) const;

    /**
    * Determines whether the Destination Address (DA) of the received MPDU is
    * a unicast, a brodcast or a multicast (but not a broadcast) address  
    * @param aFrameHeader MAC header of the MPDU
    * @return See above  
    */
    inline TDaType RxFrameDaType(
        const SDataFrameHeader& aFrameHeader ) const;
            
    /**
    * Determines if the received MSDU is valid to be forwarded to the
    * relevant client 
    * @param aCtxImpl statemachine context
    * @param aFrameHeader MAC header of the MPDU encapsulating the MSDU
    * @param aSubFrameHeader Pointer to the beginning of the subframe header.
    *                        NULL if the MSDU is not a subframe within an A-MSDU
    * @param aStartOfSnap Pointer to the beginning of the SNAP header
    * @param aEtherType Ethernet type of the received MSDU
    * @param aMulticast ETrue if the frame is a multicast, EFalse otherwise
    * @param aFlags Flags from WHA frame receive method
    * @param aSnapstatus Result of the SNAP header check of the received MSDU
    * @return ETrue if the MSDU is valid, EFalse otherwise 
    */
    inline TBool RxMsduValid(
        WlanContextImpl& aCtxImpl,
        const SDataFrameHeader& aFrameHeader,
        const SAmsduSubframeHeader* aSubFrameHeader,
        const TUint8* aStartOfSnap,
        TUint16 aEtherType,
        TBool aMulticast,
        TUint32 aFlags,
        TSnapStatus& aSnapStatus ) const;

    /**
    * Determines the Ethernet payload length of an MSDU, which may or may not
    * be a part of an A-MSDU
    * @param aMpduLength Length of the received MPDU
    * @param aSubframeLength Length of the A-MSDU subframe. Zero if the MPDU
    *        doesn't contain an A-MSDU
    * @param aQosData ETrue if the frame is a QoS Data frame
    *        EFalse othwerwise
    * @param aHtControlLen Length of the HT Control field in the MPDU's MAC
    *        header. Zero if the field is not present
    * @param aSecurityHeaderLen Length of the security header of the MPDU.
    *        Zero if the payload is not encrypted
    * @param aSecurityTrailerLen Length of the security trailer of the MPDU.
    *        Zero if the payload is not encrypted
    * @return Ethernet payload length of the MSDU
    */
    inline TUint RxMsduEthernetPayloadLength(
        const TUint32 aMpduLength, 
        TUint aSubframeLength, 
        TBool aQosData,
        TUint aHtControlLen,
        TUint aSecurityHeaderLen,
        TUint aSecurityTrailerLen ) const;

    /**
    * Allocates a new Rx buffer for a frame to be forwarded to WLAN Mgmt client
    * @param aCtxImpl statemachine context
    * @param aProprieatarySnapFrame ETrue if the frame contains a proprietary
    *        SNAP header, EFalse othwerwise
    * @param aQosFrame ETrue if the frame is a QoS Data frame
    *        EFalse othwerwise
    * @param aHtControlLen Length of the HT Control field in the frame's MAC
    *        header. Zero if the field is not present
    * @param aEtherPayloadLength Length of the frame's Ethernet payload
    * @return Pointer to the beginning of the allocated buffer
    *         NULL if allocation is unsuccessful
    */
    inline TUint8* NewBufForMgmtClientRxFrame(
        WlanContextImpl& aCtxImpl,
        TBool aProprieatarySnapFrame,
        TBool aQosFrame,
        TUint aHtControlLen,
        TUint aEtherPayloadLength ) const;
    
    /**
    * Determines if the received MSDU is for the protocol stack client
    * @param aEtherType Ethernet type of the received MSDU
    * @param aSnapstatus Result of the SNAP header check of the received MSDU
    * @return ETrue if the MSDU is for the protocol stack client
    *         EFalse if the MSDU is for the WLAN Mgmt client
    */
    inline TBool RxMsduForUser(
        TUint16 aEtherType,
        TSnapStatus aSnapstatus ) const;

    /**
    * Updates 802.11 Data frame Rx statistics
    * @param aCtxImpl statemachine context
    * @param aEtherType Ethernet type of the received frame
    * @param aMulticast ETrue if the frame is a multicast, EFalse otherwise
    * @param aAccessCategory Access Category of the received frame
    */
    inline void UpdateDataFrameRxStatistics(
        WlanContextImpl& aCtxImpl,
        TUint16 aEtherType,
        TBool aMulticast,
        WHA::TQueueId aAccessCategory ) const;

    /**
    * Called upon receiving a dot11 data MPDU
    * @param aCtxImpl statemachine context
    * @param aFlags Flags from WHA frame receive method
    * @param aRcpi RCPI of the frame
    * @param aBuffer pointer to the beginning of the Rx buffer allocated
    *        for the frame
    */
    virtual void OnDataFrameRx(
        WlanContextImpl& aCtxImpl,
        const TAny* aFrame,
        const TUint32 aLength,
        TUint32 aFlags,
        WHA::TRcpi aRcpi,
        TUint8* aBuffer );

    /**
    * Called upon receiving a 802.11 Management Action frame.
    *
    * @since S60 3.2
    * @param aCtxImpl statemachine context
    * @param aFrame Pointer to the beginning of the received frame
    * @param aLength Length of the frame. Measured from the first byte of 
    *                the MAC header to the last byte of the frame body
    * @param aRcpi Received channel power indicator
    * @param aBuffer pointer to the beginning of the Rx buffer allocated
    *        for the frame
    */
    void OnManagementActionFrameRx(
        WlanContextImpl& aCtxImpl,
        const TAny* aFrame,
        const TUint32 aLength,
        WHA::TRcpi aRcpi,
        TUint8* aBuffer ) const;

    void EncapsulateFrame(
        WlanContextImpl& aCtxImpl,
        TWlanUserTxDataCntx& aDataCntx,
        TDataBuffer& aDataBuffer,
        TUint16& aEtherType );

    /**
    * Determines if Tx frames need to be encrypted
    *
    * @since S60 3.2
    * @param aCtxImpl statemachine context
    * @param aDataBuffer meta header of the frame to be transmitted
    * @return ETrue if Tx frames need to be encrypted
    *         EFalse otherwise
    */
    TBool EncryptTxFrames( 
        WlanContextImpl& aCtxImpl, 
        const TDataBuffer& aDataBuffer ) const;
    
    /**
    * Determines the space in bytes to be reserved for the security header
    * in the MPDU to be transmitted 
    *
    * @param aCtxImpl statemachine context
    * @param aDataBuffer meta header of the frame to be transmitted
    * @return See above
    */
    TUint ComputeEncryptionOffsetAmount(
        const WlanContextImpl& aCtxImpl,
        const TDataBuffer& aDataBuffer ) const;

    /**
    * Determines the space in bytes to be reserved for the security trailer
    * in the MPDU to be transmitted 
    *
    * @param aCtxImpl statemachine context
    * @param aDataBuffer meta header of the frame to be transmitted
    * @return See above
    */
    TUint EncryptTrailerLength( 
        WlanContextImpl& aCtxImpl,
        const TDataBuffer& aDataBuffer ) const;

    TUint ComputeQosOffsetAmount( WlanContextImpl& aCtxImpl ) const;

    TUint DecryptHdrOffset( 
        WlanContextImpl& aCtxImpl, 
        TUint32 aFlags ) const;
        
    TUint DecryptTrailerOffset( 
        WlanContextImpl& aCtxImpl, 
        TUint32 aFlags ) const;

    /**
    * Checks the validity of SNAP header
    *
    * @since S60 3.1
    * @param aCtxImpl statemachine context
    * @param aStartOfSnap is a pointer to the start of the SNAP header
    */
    TSnapStatus ValiDateSnapHeader(
        WlanContextImpl& aCtxImpl,
        const TUint8* aStartOfSnap ) const;
    
    void EncapsulateEthernetFrame( 
        WlanContextImpl& aCtxImpl, 
        TWlanUserTxDataCntx& aDataCntx, 
        TDataBuffer& aDataBuffer,
        TUint16& aEtherType ) const;
    
    /**
    * Encapsulates a frame starting with a proprietary SNAP header inside an 
    * MPDU
    *
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    * @param aDataCntx user Tx frame context
    * @param aDataBuffer meta header of the frame to be transmitted
    * @param aEncrypt ETrue if the frame needs to be encrypted
    *        EFalse otherwise
    * @param aEncryptionOffset Length (bytes) to be reserved for the 
    *        encryption header
    * @param aEncryptTrailerLength Length (bytes) to be reserved for the 
    *        encryption trailer
    * @param aQosOffset Length (bytes) to be reserved for the 
    *        QoS Control field
    * @param aHtControlOffset Length (bytes) to be reserved for the 
    *        HT Control field
    */
    void EncapsulateSnapFrame(
        WlanContextImpl& aCtxImpl,
        TWlanUserTxDataCntx& aDataCntx,
        TDataBuffer& aDataBuffer,
        TBool aEncrypt,
        TUint aEncryptionOffset,
        TUint aEncryptTrailerLength,
        TUint aQosOffset,
        TUint aHtControlOffset ) const;

    /**
    * Sets a ready made 802.11 frame to Tx buffer
    *
    * @since S60 3.2
    * @param aCtxImpl global statemachine context
    * @param aDataCntx user Tx frame context
    * @param aDataBuffer meta header of the frame to be transmitted
    */
    void SetDot11FrameToTxBuffer(
        const WlanContextImpl& aCtxImpl,
        TWlanUserTxDataCntx& aDataCntx,
        TDataBuffer& aDataBuffer ) const;

    /**
    * Indicates Consecutive Beacons Lost 
    * to WLAN Mgmt Client; if necessary
    * 
    * @since S60 5.0
    * @param aCtxImpl global statemachine context
    */
    virtual void DoConsecutiveBeaconsLostIndication( WlanContextImpl& aCtxImpl );

    /**
    * Indicates Consecutive Tx Failures
    * to WLAN Mgmt Client; if necessary
    * 
    * @since S60 5.0
    * @param aCtxImpl global statemachine context
    */
    virtual void DoConsecutiveTxFailuresIndication( WlanContextImpl& aCtxImpl );

    virtual void DoRegainedBSSIndication( WlanContextImpl& aCtxImpl );

    virtual void OnPacketSendComplete(
        WlanContextImpl& aCtxImpl, 
        WHA::TStatus aStatus,
        TUint32 aPacketId,
        WHA::TRate aRate,
        TUint32 aPacketQueueDelay,
        TUint32 aMediaDelay,
        TUint aTotalTxDelay,
        TUint8 aAckFailures,
        WHA::TQueueId aQueueId,
        WHA::TRate aRequestedRate,
        TBool aMulticastData );            

    /**
     * From MWlanUserEvent
     * Add/set (or replace) a broadcast WEP key
     *
     * @since S60 3.1
     * @param aCtxImpl statemachine context
     * @param aKeyIndex Index of the key in the default key table 
     * @param aKeyLength Length of the key in BYTES
     * @param aKey The WEP key
     * @param aMac MAC address associated with the key
     * @return ETrue if state transition occurred, EFalse otherwise
     */
    virtual TBool AddBroadcastWepKey(
        WlanContextImpl& aCtxImpl,
        TUint32 aKeyIndex,             
        TBool aUseAsDefaulKey,                
        TUint32 aKeyLength,                      
        const TUint8 aKey[KMaxWEPKeyLength],
        const TMacAddress& aMac );   

    /**
     * From MWlanUserEvent
     * Reconfigures the specified Tx queue if necessary
     *
     * @since S60 v3.2
     * @param aCtxImpl statemachine context
     * @param aQueueId ID of the queue to reconfigure
     * @param aMediumTime The amount of time the queue is allowed to access 
     *                    the WLAN air interface.
     * @param aMaxTxMSDULifetime Maximum Transmit MSDU Lifetime to be used 
     *                           for the specified queue.
     * @return ETrue if a state change occurred
     *         EFalse otherwise
     */
    virtual TBool ConfigureTxQueueIfNecessary( 
            WlanContextImpl& aCtxImpl,
            TQueueId aQueueId,
            TUint16 aMediumTime,
            TUint32 aMaxTxMSDULifetime );
        
    /**
    * If the power mgmt mode needs to be changed - based on 
    * aPowerMgmtModeChange - proceeds with the necessary actions
    * 
    * @param aCtxImpl statemachine context
    * @param aPowerMgmtModeChange 
    * @return ETrue if a state change occurred in the state machine 
    *         EFalse otherwise
    */
    TBool PowerMgmtModeChange(
        WlanContextImpl& aCtxImpl,
        TPowerMgmtModeChange aPowerMgmtModeChange );
    
private:           

    /**
     * Configures Tx rate policy objects, sets the policy object to use for 
     * every Tx Queue / QoS AC, and also sets the Initial Max Tx Rate to be 
     * used for the configured policy objects
     *
     * @since S60 v3.1
     * @param aCtxImpl statemachine context
     * @param aRatePolicy rate policy (policy objects)
     * @param aQueue2RateClass Tx queue (AC) to rate policy object mapping
     * @param aInitialMaxTxRate4RateClass initial max Tx rate for the
     *        policy objects
     * @param aAutoRatePolicy auto rate policy
     * @param aHtMcsPolicy HT MCS policy
     * @return ETrue if a state change occurred
     *         EFalse otherwise
     */
    virtual TBool ConfigureTxRatePolicies( 
        WlanContextImpl& aCtxImpl,
        const TTxRatePolicy& aRatePolicy,
        const TQueue2RateClass& aQueue2RateClass,
        const TInitialMaxTxRate4RateClass& aInitialMaxTxRate4RateClass,
        const TTxAutoRatePolicy& aAutoRatePolicy,
        const THtMcsPolicy& aHtMcsPolicy );

    /**
    * Configures dynamic power mode management traffic override
    *
    * The settings here become effective once using the PS mode has been 
    * allowed by WLAN Mgmt Client.
    * When a setting below is ETrue, any amount of Rx or Tx traffic via
    * the AC in question won't cause a change from PS to CAM mode once PS
    * mode has been entered, and traffic via that AC won't make us to 
    * stay in CAM either.
    * Every AC has a separate setting for U-APSD and legacy PS.
    * The U-APSD setting is used if U-APSD is used for the AC in question.
    * Otherwise the corresponding legacy setting is used.
    *    
    * @since S60 3.2
    * @param aCtxImpl statemachine context
    * @param aStayInPsDespiteUapsdVoiceTraffic U-APSD Voice AC setting
    * @param aStayInPsDespiteUapsdVideoTraffic U-APSD Video AC setting
    * @param aStayInPsDespiteUapsdBestEffortTraffic U-APSD Best Effort AC 
    *                                               setting
    * @param aStayInPsDespiteUapsdBackgroundTraffic U-APSD Background AC 
    *                                               setting
    * @param aStayInPsDespiteLegacyVoiceTraffic legacy Voice AC setting
    * @param aStayInPsDespiteLegacyVideoTraffic legacy Video AC setting
    * @param aStayInPsDespiteLegacyBestEffortTraffic legacy Best Effort AC 
    *                                                setting
    * @param aStayInPsDespiteLegacyBackgroundTraffic legacy Background AC 
    *                                                setting
    * @return ETrue if a state change occurred
    *         EFalse otherwise
    */
    virtual TBool ConfigurePwrModeMgmtTrafficOverride( 
        WlanContextImpl& aCtxImpl,
        TBool aStayInPsDespiteUapsdVoiceTraffic,
        TBool aStayInPsDespiteUapsdVideoTraffic,
        TBool aStayInPsDespiteUapsdBestEffortTraffic, 
        TBool aStayInPsDespiteUapsdBackgroundTraffic,
        TBool aStayInPsDespiteLegacyVoiceTraffic,
        TBool aStayInPsDespiteLegacyVideoTraffic,
        TBool aStayInPsDespiteLegacyBestEffortTraffic,
        TBool aStayInPsDespiteLegacyBackgroundTraffic );

    // Prohibit copy constructor
    WlanDot11Associated( const WlanDot11Associated& );
    // Prohibit assigment operator
    WlanDot11Associated& operator= ( const WlanDot11Associated& );  
        
    };

#endif      // WLANDOT11ASSOCIATED_H
