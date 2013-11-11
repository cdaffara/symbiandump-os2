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
* Description:   Declaration of the UmacContextImpl class
*
*/

/*
* %version: 107 %
*/

#ifndef WLANCONTEXTIMPL_H
#define WLANCONTEXTIMPL_H

#ifndef RD_WLAN_DDK
#include <wha_types.h>
#else
#include <wlanwha_types.h>
#endif

#include "Umac.h"
#include "FrameXferBlock.h"

#include "802dot11.h"
#include "umacinternaldefinitions.h"
#include "UmacMib.h"
#include "UmacAuthSeqNmbrExpected.h"
#include "umaceventdispatcher.h"
#include "umacpacketscheduler.h"
#include "umacpacketschedulerclient.h"
#include "umacnullsender.h"

#include "UmacDot11MacError.h"
#include "UmacDot11InitPhase1.h" 
#include "UmacDot11Idle.h"

#include "UmacDot11PrepareForBssMode.h"
#include "UmacDot11DisassociationPending.h"
#include "UmacDot11Synchronize.h"
#include "umacdot11idlescanningmode.h"

#include "UmacDot11ReassociationPending.h"
#include "UmacDot11SoftReset.h"
#include "UmacDot11OpenAuthPending.h"
#include "UmacDot11SharedAuthPending.h"
#include "UmacDot11AssociationPending.h"
#include "UmacDot11DeauthPending.h"
#include "UmacDot11InfrastructureModeInit.h"
#include "UmacDot11PrepareForIbssMode.h"
#include "umacdot11ibssnormalmode.h"
#include "umacdot11ibssscanningmode.h"
#include "umacdot11infrastructurenormalmode.h"
#include "umacdot11infrastructurescanningmode.h"
#include "umacdot11pwrmgmttransitionmode.h"
#include "UmacDot11MibDefaultConfigure.h"

#include "umacconnectcontext.h"
#include "umacprivacymodefilters.h"
#include "UmacTxRateAdaptation.h"
#include "UmacPRNGenerator.h"
#include "umacusertxdatacntx.h"
#include "umacdynamicpowermodemgmtcntx.h"
#include "umacnullsendcontroller.h"
#include "umacsignalpredictor.h"

class WlanMacState;
class WlanConnectContext;
class UmacManagementSideUmacCb;
class WlanAddBroadcastWepKey;
class WlanConfigureTxQueueParams;
class WlanConfigureTxAutoRatePolicy;
class WlanWsaInitiliaze;
class WlanWsaConfigure;
class WlanWsaJoin;
class WlanWsaScan;
class WlanWhaStopScan;
class WlanWsaReadMib;
class WlanWsaWriteMib;
class WlanWsaDisconnect;
class WlanWsaSetBssParameters;
class WlanWsaAddKey;
class WlanWsaRemoveKey;
class WlanWsaSetPsMode;
class WlanWhaConfigureQueue;
class WlanWhaConfigureAc;
class WlanWhaRelease;

struct SWsaCommands;


/**
*  WLAN context implementation
*/
class WlanContextImpl : 
    public WHA::MWhaCb,
    public MWlanPacketSchedulerClient,
    public MWlanNullSender      
    {

public:
    
    /**
    * Return value type for AddMulticastAddress() method
    */
    enum TGroupAddStatus
        {
        EOk,            
        EFull,          
        EAlreadyExists
        };                      

    /**
    * Lookup table to map 802.11 rate defintions 
    */
    class SupportedRateLookUp
        {
        public: // Methods

            /** Ctor */
            SupportedRateLookUp() : 
                // these initial values don't really matter as they are always
                // overwritten
                iSupportedRate( E802Dot11Rate1MBit ), iWsaRate( 0 ) {};

        private: // Methods

            // Prohibit copy constructor
            SupportedRateLookUp( const SupportedRateLookUp& );
            // Prohibit assigment operator
            SupportedRateLookUp& operator== ( const SupportedRateLookUp&);

        public: // Data
            
            /** 802.11 rate defintions */
            T802Dot11SupportedRate  iSupportedRate;
            /** corresponding WHA rate */
            TUint32                 iWsaRate;
        };

protected:
    
    /**
    * The states of the statemachine
    */
    struct States
        {
        public: // Methods

            /**
            * Ctor
            */
            States() {};

        private: // Methods

            // Prohibit assignment operator 
            States& operator= ( const States& );
            // Prohibit copy constructor 
            States( const States& );

        public: // Data
            WlanDot11InitPhase1 iInitPhase1State;
            WlanDot11MibDefaultConfigure iMibDefaultConfigure;
            WlanDot11Idle iIdleState;
            WlanDot11PrepareForBssMode iPrepareForBssMode;
            WlanDot11PrepareForIbssMode iPrepareForIbssMode;  
            WlanDot11OpenAuthPending iOpenAuthPendingState;
            WlanDot11SharedAuthPending iSharedAuthPending;
            WlanDot11AssociationPending iAssociationPendingState;
            WlanDot11InfrastructureModeInit iInfrastructureModeInit;
            WlanDot11IdleScanningMode iIdleScanningMode;
            WlanDot11IbssNormalMode iIbssNormalMode;
            WlanDot11IbssScanningMode iIbssScanningMode;
            WlanDot11InfrastructureScanningMode
                iInfrastructureScanningMode;
            WlanDot11InfrastructureNormalMode 
                iInfrastructureNormalMode;
            WlanDot11PwrMgmtTransitionMode iPwrMgmtTransitionMode;
            WlanDot11DisassociationPending iDisassociationPendingState;
            WlanDot11Synchronize iSynchronizeState;
            WlanDot11ReassociationPending iReassociationPendingState;
            WlanDot11DeauthPending iDeauthPendingState;
            WlanDot11SoftReset iSoftResetState;
            WlanDot11MacError iMacError;
        };

    /**
    * 802.11 management frame templates
    */
    class ManagementFrameTemplates
        {
        public: // Methods

            /** Ctor */
            ManagementFrameTemplates() :
                ialign1( 0 ),
                ialign2( 0 ),
                ialign3( 0 ),
                ialign4( 0 ),
                ialign5( 0 ),
                ialign6( 0 ),
                ialign7( 0 ),
                ialign8( 0 ),
                ialign9( 0 )
                {};

        private: // Methdods

            // Prohibit copy constructor
            ManagementFrameTemplates( const ManagementFrameTemplates& );
            // Prohibit assigment operator
            ManagementFrameTemplates& operator== ( 
                const ManagementFrameTemplates& );

        public: // Data

            /** authenticate frame template */
            SAuthenticationFrame        iAuthenticationFrame;
            /** 
            * 32-bit align 
            * only needed because these structs are packed
            */
            TUint32                     ialign1;
            /** authenticate frame template with HT Control field */
            SHtAuthenticationFrame      iHtAuthenticationFrame;
            /** 
            * 32-bit align 
            * only needed because these structs are packed
            */
            TUint32                     ialign2;
            /** association frame template */
            SAssociationRequestFrame    iAssociationRequestFrame;
            /** 
            * 32-bit align 
            * only needed because these structs are packed
            */
            TUint32                     ialign3;
            /** association frame template with HT Control field */
            SHtAssociationRequestFrame  iHtAssociationRequestFrame;
            /** 
            * 32-bit align 
            * only needed because these structs are packed
            */
            TUint32                     ialign4;
            /** deauthenticate frame template */
            SDeauthenticateFrame        iDeauthenticateFrame;                
            /** 
            * 32-bit align 
            * only needed because these structs are packed
            */
            TUint32                     ialign5;
            /** deauthenticate frame template with HT Control field */
            SHtDeauthenticateFrame      iHtDeauthenticateFrame;                
            /** 
            * 32-bit align 
            * only needed because these structs are packed
            */
            TUint32                     ialign6;
            /** disassociation frame template */
            SDisassociateFrame          iDisassociationFrame;
            /** 
            * 32-bit align 
            * only needed because these structs are packed
            */
            TUint32                     ialign7;
            /** disassociation frame template with HT Control field */
            SHtDisassociateFrame        iHtDisassociationFrame;
            /** 
            * 32-bit align 
            * only needed because these structs are packed
            */
            TUint32                     ialign8;
            /** resassociation frame template */
            SReassociationRequestFrame  iReassociationRequestFrame;
            /** 
            * 32-bit align 
            * only needed because these structs are packed
            */
            TUint32                     ialign9;
            /** resassociation frame template with HT Control field */
            SHtReassociationRequestFrame iHtReassociationRequestFrame;
        };
        
        /**
        * Container for multicast groups (specified by group mac addresses)
        * that we have joined. 
        *
        * When we have joined any multicast groups
        * we would like to receive only multicast packets that have been
        * sent to those groups.
        */
        class JoinedMulticastGroups
            {            
            public:
            
                /** Ctor */
                JoinedMulticastGroups();

                /**
                * Adds a group.
                * @param 
                * @return ETrue if aGroup was not fou
                */
                TGroupAddStatus AddGroup( const TMacAddress& aGroup );

                /**
                * Removes a group.
                * @param aGroup Group to be removed
                * @return ETrue if aGroup was found (and hence removed)
                *         EFalse otherwise
                */
                TBool RemoveGroup( const TMacAddress& aGroup );
                
                /**
                * Returns the number of groups that we are joined into
                * currently.
                * @return The number of groups joined currently.
                */
                TUint8 Count() const;

                /**
                * Gets all the groups by passing back a pointer to the 1st
                * group which is followed by all the other groups that 
                * we are joined into.
                * @return The number of groups (group addresses) returned
                */
                TUint8 GetGroups( const TMacAddress*& aGroups ) const;

                /**
                * Clears the whole container by removing all groups from it
                */
                void Reset();

            private:

                /**
                * Finds aGroup
                * @param aGroup Group to be located
                * @param aIndex Index pointing to aGroup if aGroup was found
                * @return ETrue if aGroup was found
                *         EFalse otherwise
                */
                TBool FindGroup( 
                    const TMacAddress& aGroup, 
                    TUint& aIndex ) const;
                    
                // Prohibit copy constructor
                JoinedMulticastGroups( const JoinedMulticastGroups& );
                // Prohibit assigment operator
                JoinedMulticastGroups& operator= 
                    ( const JoinedMulticastGroups& );
                    
            private: // Data
            
                /** 
                * The maximum number of multicast groups that we allow.
                * Note that the adaptation layer may support also
                * any smaller number of addresses, including zero
                */
                enum { KMaxNbrOfGroups = 32 };
            
                /** storage for group entries */
                TMacAddress iStorage[KMaxNbrOfGroups];
                
                /** is the corresponding storage entry free */
                TBool iFree[KMaxNbrOfGroups];
                
                /** 
                * 1st free index. As there are no empty storage entries in 
                * the middle, this is also the number of groups that we are
                * joined into currently */
                TUint8 iFirstFreeIndex;
            };

public:

    /**
    * C++ constructor.
    * @param WlanContext context that owns us
    */
    explicit WlanContextImpl( Umac& aUmac ); 

    /**
    * Dtor
    */
    virtual ~WlanContextImpl();

    /**
    * Constructs our supported rates lookup table
    */
    void MakeLookUpTable();

    void AttachWsa( WHA::Wha* aWha );
    void BootUp(
        const TUint8* aPda, 
        TUint32 aPdaLength,
        const TUint8* aFw, 
        TUint32 aFwLength );
    inline WlanManagementSideUmacCb& WlanManagementSideCb() { return iUmac; }
    inline WHA::Wha& Wha() { return *iWha; }
    inline WlanMacState& CurrentState();

    TBool OnDeferredWhaCommandComplete( 
        WHA::TCompleteCommandId aCompleteCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandCompletionParams& aCommandCompletionParams );
    
    TBool OnDeferredInternalEvent( TInternalEvent aInternalEvent );
    
    inline void RegisterEvent( const TOIDHeader& aOid );
    
    inline void RegisterEvent( TInternalEvent aInternalEvent );
    
    inline TBool ChannelEnabled( WlanEventDispatcher::TChannel aChannel ) const;

    inline void Enable( WlanEventDispatcher::TChannel aChannelMask );
    inline void Disable( WlanEventDispatcher::TChannel aChannelMask );

    inline TBool DispatchEvent();

    /**
     * Determines if a command completion event for the specified 
     * command is registered
     *
     * @param aCommandId Id of the command to check
     * @return ETrue if command completion event is registered
     *         EFalse otherwise
     */
    inline TBool CommandCompletionEventRegistered( 
        WHA::TCompleteCommandId aCommandId ) const;

    /**
     * Dispatches a single command completion event
     *
     * @since S60 3.1
     * @return ETrue If a global state change occurred as a result of the event
     *               dispatching
     *         EFalse otherwise
     */
    inline TBool DispatchCommandCompletionEvent();

    inline TBool ProtocolStackTxDataAllowed() const;

    /**
    * Second level initializer. Does statemachine initialization
    * @return ETrue success EFalse failure
    */
    TBool Initialize();

    WlanAddBroadcastWepKey&         AddBroadcastWepKey();
    WlanConfigureTxQueueParams&     ConfigureTxQueueParams();
    WlanConfigureTxAutoRatePolicy&  ConfigureTxAutoRatePolicy();    
    WlanWsaInitiliaze&              WsaInitiliaze();
    WlanWsaConfigure&               WsaConfigure();
    WlanWsaReadMib&                 WsaReadMib();
    WlanWsaWriteMib&                WsaWriteMib();
    WlanWsaJoin&                    WsaJoin();
    WlanWsaScan&                    WsaScan();
    WlanWhaStopScan&                WhaStopScan();
    WlanWsaDisconnect&              WsaDisconnect();
    WlanWsaSetBssParameters&        WsaSetBssParameters();
    WlanWsaAddKey&                  WsaAddKey();
    WlanWhaConfigureQueue&          WhaConfigureQueue();
    WlanWsaSetPsMode&               WsaSetPsMode();
    WlanWhaConfigureAc&             WhaConfigureAc();
    WlanWhaRelease&                 WhaRelease();

    inline void WHASettings( const WHA::SSettings& aSettings );
    inline WHA::SSettings& WHASettings();
    inline const WHA::SSettings& WHASettings() const;

    /**
    * Gets authentication frame template
    * @return authentication frame template
    */
    inline SAuthenticationFrame& GetAuthenticationFrame();

    /**
    * Gets authentication frame template with HT Control field
    * @return authentication frame template
    */
    inline SHtAuthenticationFrame& GetHtAuthenticationFrame();

    /**
    * Gets association request frame template
    * @return association request frame template
    */
    inline SAssociationRequestFrame& GetAssociationRequestFrame();

    /**
    * Gets association request frame template with HT Control field
    * @return association request frame template
    */
    inline SHtAssociationRequestFrame& GetHtAssociationRequestFrame();

    /**
    * Gets deauthenticate frame template
    * @return deauthenticate frame template
    */
    inline SDeauthenticateFrame& GetDeauthenticateFrame();

    /**
    * Gets deauthenticate frame template with HT Control field
    * @return deauthenticate frame template
    */
    inline SHtDeauthenticateFrame& GetHtDeauthenticateFrame();

    /**
    * Gets disassociation frame template
    * @return disassociation frame template
    */
    inline SDisassociateFrame& GetDisassociationFrame();

    /**
    * Gets disassociation frame template with HT Control field
    * @return disassociation frame template
    */
    inline SHtDisassociateFrame& GetHtDisassociationFrame();

    /**
    * Gets resassociation frame template
    * @return resassociation frame template
    */
    inline SReassociationRequestFrame& GetReassociationRequestFrame();

    /**
    * Gets resassociation frame template with HT Control field
    * @return resassociation frame template
    */
    inline SHtReassociationRequestFrame& GetHtReassociationRequestFrame();

    /**
    * Gets null data frame template
    * @return resassociation frame template
    */
    inline SNullDataFrame& NullDataFrame();

    /**
    * Gets QoS Null data frame template
    * @return QoS Null data frame template
    */
    inline SHtQosNullDataFrame& QosNullDataFrame();

    /**
    * Returns the length of the QoS Null data frame template when we
    * know our current/target network.
    * It's the length of a HT QoS Null data frame in case of a HT nw
    * and otherwise the length of a regular (non-HT) QoS Null data frame
    * @return Length of the currently relevant QoS Null data frame
    */
    inline TUint16 QosNullDataFrameLength() const;

    /**
    * Gets data frame template
    * @return data frame template
    */
    inline SDataFrameHeader& GetDataFrameHeader();

    /**
    * Gets protocol stack side ethernet tx context
    * @return protocol stack side ethernet write context
    */
    inline TWlanUserTxDataCntx& GetTxDataCntx();
    inline const TWlanUserTxDataCntx& GetTxDataCntx() const;

    /**
    * Gets EAPOL stack side ethernet tx context
    * @return EAPOL stack side ethernet write context
    */
    inline TWlanUserTxDataCntx& GetMgmtTxDataCntx();
    inline const TWlanUserTxDataCntx& GetMgmtTxDataCntx() const;

    /**
    * Resets authentication sequnece expected counter to initial state
    */
    inline void ResetAuthSeqNmbrExpected();

    /**
    * Gets the current Tx rate and related rate policy associated with the 
    * specified Tx queue
    *
    * @param aQueueId id of the Tx queue
    * @param aUseSpecialRatePolicy If ETrue, the use of the special Tx rate
    *        policy is requested
    *        If EFalse, the use of the special Tx rate policy is not requested
    * @param aRate initial max Tx rate to use; if relevant
    * @param aPolicyId ID of the rate class / policy to use
    */
    inline void TxRatePolicy(
        WHA::TQueueId aQueueId,
        TBool aUseSpecialRatePolicy,
        WHA::TRate& aRate, 
        TUint8& aPolicyId ) const;

    /**
    * Gets BSSID
    * @return BSSID
    */
    inline TMacAddress& GetBssId();

    /**
    * Regarding the features which we allow to be be configured on or off,
    * sets the allowed features per WLAN Mgmt client request
    *
    * @param aFeaturesAllowed Feature bit mask where 1 means that the use of 
    *        the corresponding feature is allowed and 0 means that its not
    *        allowed 
    */ 
    inline void FeaturesAllowed( TWlanFeatures aFeaturesAllowed );
        
    /**
    * Regarding the features which we allow to be be configured on or off,
    * returns the allowed features
    *
    * @return Feature bit mask where 1 means that the use of 
    *        the corresponding feature is allowed and 0 means that its not
    *        allowed 
    */ 
    inline TWlanFeatures FeaturesAllowed() const;
    
    inline void Aid( const TUint32 aAid );
    inline TUint32 Aid() const;

    inline void AtimWindow( const TUint32 aAtimWindow );
    inline TUint32 AtimWindow() const;

    /**
    * Gets SSID
    * @return SSID
    */
    inline TSSID& GetSsId();

    /**
    * Gets used privacy mode
    * @return used privacy mode
    */
    inline TEncryptionStatus& EncryptionStatus();

    inline WHA::TOperationMode NetworkOperationMode() const;
    inline void NetworkOperationMode( 
        WHA::TOperationMode aOperationMode );

    inline const TPairwiseKeyData* RoamingPairwiseKey() const;
    inline void  RoamingPairwiseKey( const TPairwiseKeyData* aKeyData );
    
    // setters and getters for key type
    inline void GroupKeyType( WHA::TKeyType aKeyType );
    inline WHA::TKeyType GroupKeyType() const;
    inline void PairWiseKeyType( WHA::TKeyType aKeyType );
    inline WHA::TKeyType PairWiseKeyType() const;

    inline void PairwiseCipher( TWlanCipherSuite aCipherSuite );
    inline TWlanCipherSuite PairwiseCipher() const;
    
    inline WHA::TChannelNumber NetworkChannelNumeber() const;
    inline void NetworkChannelNumeber( WHA::TChannelNumber aChannelNumber );

    inline TUint32 NetworkBeaconInterval() const;
    inline void NetworkBeaconInterval( TUint32 aBeaconInterval );   

    inline WHA::TPsMode DesiredDot11PwrMgmtMode() const;
    inline void DesiredDot11PwrMgmtMode( WHA::TPsMode aPsMode );

    inline WHA::TPsMode CurrentDot11PwrMgmtMode() const;
    inline void CurrentDot11PwrMgmtMode( WHA::TPsMode aPsMode );

    inline const TDot11PsModeWakeupSetting& DesiredPsModeConfig() const;
    inline void SetDesiredPsModeConfig(
        const TDot11PsModeWakeupSetting& aPsModeWakeupSetting );

    TPowerMgmtMode CurrentPwrMgmtMode() const;
    
    inline WHA::TPsMode ClientDot11PwrMgmtMode() const;
    inline void ClientDot11PwrMgmtMode( TPowerMode aPsMode );
    
    inline const TDot11PsModeWakeupSetting& ClientLightPsModeConfig() const;
    inline void SetClientLightPsModeConfig( 
        TWlanWakeUpInterval aWakeupMode, 
        TUint8 aListenInterval );

    inline const TDot11PsModeWakeupSetting& ClientDeepPsModeConfig() const;
    inline void SetClientDeepPsModeConfig( 
        TWlanWakeUpInterval aWakeupMode, 
        TUint8 aListenInterval );

    inline TBool DynamicPwrModeMgtDisabled() const;        
    void DynamicPwrModeMgtDisabled( TBool aValue );
    
    inline TBool UseShortPreamble() const;
    void UseShortPreamble( TBool aValue );

    inline TBool Reassociate() const;
    void Reassociate( TBool aValue );

    inline TBool UseShortSlotTime() const;
    void UseShortSlotTime( TBool aValue );

    inline TBool ProtectionBitSet() const;
    void ProtectionBitSet( TBool aValue );

    inline TBool QosEnabled() const;
    void QosEnabled( TBool aValue );

    inline TBool UapsdEnabled() const;
    void UapsdEnabled( TBool aValue );
    
    inline TBool MulticastFilteringDisAllowed() const;
    void MulticastFilteringDisAllowed( TBool aValue );

    inline TBool ErpIePresent() const;
    void ErpIePresent( TBool aValue );
            
    inline TBool DisassociatedByAp() const;
    void DisassociatedByAp( TBool aValue );
            
    inline TBool UapsdRequestedForVoice() const;
    void UapsdRequestedForVoice( TBool aValue );

    inline TBool UapsdRequestedForVideo() const;
    void UapsdRequestedForVideo( TBool aValue );

    inline TBool UapsdRequestedForBestEffort() const;
    void UapsdRequestedForBestEffort( TBool aValue );

    inline TBool UapsdRequestedForBackground() const;
    void UapsdRequestedForBackground( TBool aValue );

    inline TBool UapsdUsedForVoice() const;
    void UapsdUsedForVoice( TBool aValue );

    inline TBool UapsdUsedForVideo() const;
    void UapsdUsedForVideo( TBool aValue );

    inline TBool UapsdUsedForBestEffort() const;
    void UapsdUsedForBestEffort( TBool aValue );

    inline TBool UapsdUsedForBackground() const;
    void UapsdUsedForBackground( TBool aValue );

    inline TBool ApTestOpportunitySeekStarted() const;
    void ApTestOpportunitySeekStarted( TBool aValue );

    inline TBool ApTestOpportunityIndicated() const;
    void ApTestOpportunityIndicated( TBool aValue );

    inline TBool HtSupportedByNw() const;
    void HtSupportedByNw( TBool aValue );    
    
    inline TUint8 WmmParameterSetCount() const;
    inline void WmmParameterSetCount( TUint8 aValue );               

    inline TUint32 RateBitMask() const;
    inline void RateBitMask( TUint32 aValue );               

    inline TCwMinVector& CwMinVector();
    inline TCwMaxVector& CwMaxVector();
    inline TAifsVector& AifsVector();
    inline TTxOplimitVector& TxOplimitVector();
    inline TAcmVector& AcmVector();
    /**
    * Returns our Wmm IE
    * @return WMM IE
    */
    inline STxWmmIE& OurWmmIe();
        
    /**
    * Returns the WLAN Mgmt client provided Tx rate policy
    *
    * @since S60 3.1
    * @return Tx rate policy
    */
    inline TTxRatePolicy& RatePolicy();
    
    /**
    * Returns the WLAN Mgmt client provided Tx queue to Tx rate policy mapping
    *
    * @since S60 3.1
    * @return Tx queue to Tx rate policy mapping
    */
    inline TQueue2RateClass& Queue2RateClass();

    /**
    * Returns the WLAN Mgmt client provided initial max Tx rate for every
    * Mgmt client provided rate policy object
    *
    * @since S60 3.1
    * @return initial max Tx rate for every relevant Tx rate policy object
    */
    inline TInitialMaxTxRate4RateClass& InitialMaxTxRate4RateClass();

    /**
    * Returns the WLAN Mgmt client provided Tx auto rate policy
    *
    * @return Tx auto rate policy
    */
    inline TTxAutoRatePolicy& AutoRatePolicy();
    
    /**
    * Returns the ID of the special Tx auto rate policy
    * Returns zero if a special Tx auto rate policy is not available
    *
    * @return the ID of the special Tx auto rate policy, or zero
    */
    inline TUint8 SpecialTxAutoRatePolicy() const;

    /**
    * Sets the ID of the special Tx auto rate policy
    *
    * @param  aPolicyId the ID of the special Tx auto rate policy. Zero
    *         if a special Tx auto rate policy is not available
    */
    inline void SpecialTxAutoRatePolicy( TUint8 aPolicyId );

    /**
    * Returns the WLAN Mgmt client provided HT MCS policy
    *
    * @since S60 3.1
    * @return Tx rate policy
    */
    inline THtMcsPolicy& HtMcsPolicy();
    
    /**
    * Returns the WLAN Mgmt client provided Maximum U-APSD Service Period 
    * length
    *
    * @since S60 3.2
    * @return Maximum U-APSD Service Period length
    */
    inline TQosInfoUapsdMaxSpLen& UapsdMaxSpLen();

    inline TUint32 WhaCommandAct() const;
    inline void WhaCommandAct( TUint32 aAct );

    inline TUint32 Random();

    /**
    * Returns reference to the authentication algorithm number to be used
    * @return authentication algorithm number
    */
    inline TUint16& AuthenticationAlgorithmNumber();

    /**
    * To be called upon every Tx frame send completion
    * @param aRate actual rate transmitted
    * @param aSuccess was the frame transmitted successfully or not
    * @param aRequestedRate Tx rate that was originally requested
    */
    inline void OnTxCompleted( 
        const TUint32 aRate, 
        const TBool aSuccess, 
        WHA::TQueueId aQueueId, 
        WHA::TRate aRequestedRate );

    /**
    * Gets buffer for Rx data
    * 
    * @param aLengthinBytes Requested length of the buffer
    * @param aInternal ETrue if the buffer request was triggered inside UMAC
    *                  EFalse if it was triggered from WHA layer
    * @return Pointer to the beginning of the buffer on success
    *         NULL if a buffer couldn't be allocated
    */
    TUint8* GetRxBuffer( 
        const TUint16 aLengthinBytes, 
        TBool aInternal = EFalse );
        
    /**
    * Allocates Rx frame meta header
    * @return Rx frame meta header upon success
    *         NULL otherwise
    */
    TDataBuffer* GetRxFrameMetaHeader();

    /**
    * Deallocates Rx frame meta header
    * @param aMetaHeader Meta header to deallocate
    */
    void FreeRxFrameMetaHeader( TDataBuffer* aMetaHeader );
    
    TUint8* TxBuffer( TBool aWaitIfNotFree = EFalse );

    /**
    * Cancels the default timer
    */
    inline void CancelTimer();

    /**
    * Gets the authentication sequence number expected
    * @return authentication sequence number expected
    */
    inline TUint8 GetAuthSeqNmbrExpected() const;

    /**
    * Gets our supported rates IE
    * @return supported rates IE
    */
    inline SSupportedRatesIE& GetOurSupportedRatesIE();

    /**
    * Gets our extended supported rates IE
    * @return extended supported rates IE
    */
    inline SExtendedSupportedRatesIE& GetOurExtendedSupportedRatesIE();

    /**
    * Stores a Beacon or Probe Response frame body from the 
    * network we are going to connect to
    *
    * @since S60 3.2
    * @param aBody pointer to the frame body
    */
    inline void SetScanResponseFrameBody( const TUint8* aBody );

    /**
    * Returns a pointer to a Beacon or Probe Response frame body from the 
    * network we are going to connect to
    *
    * @since S60 3.2
    * @return pointer to the frame body
    */
    inline const TUint8* ScanResponseFrameBody() const;

    /**
    * Stores a Beacon or Probe Response frame body length
    *
    * @since S60 3.2
    * @param aLength frame body length
    */
    inline void SetScanResponseFrameBodyLength( TUint16 aLength );

    /**
    * Returns the length of a Beacon or Probe Response frame body from the 
    * network we are going to connect to
    *
    * @since S60 3.2
    * @return length of the frame body
    */
    inline TUint16 ScanResponseFrameBodyLength() const;

    /**
    * Stores the IE(s) to be included into the (re-)association request.
    *
    * @since S60 3.2
    * @param aIeData pointer to the IE(s). NULL, if none to be included
    */
    inline void IeData( const TUint8* aIeData );

    /**
    * Returns the IE(s) to be included into the (re-)association request.
    *
    * @since S60 3.2
    * @return pointer to the IE(s). NULL, if none to be included
    */
    inline const TUint8* IeData() const;

    /**
    * Stores the length of the IE(s) to be included into the (re-)association
    * request.
    *
    * @since S60 3.2
    * @param aIeDataLength length of the IE(s)
    */
    inline void IeDataLength( TUint16 aIeDataLength );

    /**
    * Returns the length of the IE(s) to be included into the (re-)association
    * request.
    *
    * @since S60 3.2
    * @return length of the IE(s)
    */
    inline TUint16 IeDataLength() const;

    /**
    * Stores the Radio Measurement value to be included into the (re-)association
    * request.
    *
    * @since S60 9.2
    * @param aRadioMeasurement True if Measurement on, otherwise false
    */
    inline void RadioMeasurement( TBool aRadioMeasurement );

    /**
    * Returns the Radio Measurement value to be included into the (re-)association
    * request.
    *
    * @since S60 9.2
    * @return Radio Measurement setting
    */
    inline TBool RadioMeasurement() const;

    /**
    * Sets desired privacy mode filter
    * @param aEncryptionStatus desired privacy mode
    */
    inline void SetActivePrivacyModeFilter( 
        WlanContextImpl& aCtxImpl, 
        TEncryptionStatus aEncryptionStatus );

    /**
    * Executes current privacy mode filter
    * @param aFrameheader Header of the received frame
    * @param aUserDataEnabled is protocol stack side datapath 
    *        enabled or not
    * @param aEthernetType Ethernet Type of the received frame
    * @param aUnicastKeyExists AP <-> client unicast key 
    *        is configured or not 
    * @param aAesOrTkipOrWapiEncrypted ETrue if the frame is encrypted with AES,
    *        TKIP or WAPI, EFalse otherwise
    * @return ETrue if frame can be accepted, EFalse otherwise
    */
    inline TBool ExecuteActivePrivacyModeFilter(
        const SDataFrameHeader& aFrameheader, 
        TBool aUserDataEnabled, 
        TUint16 aEthernetType,
        TBool aUnicastKeyExists, 
        TBool aAesOrTkipOrWapiEncrypted ) const;

    /**
    * Gets capability information field
    * @return capability information field
    */
    inline SCapabilityInformationField& GetCapabilityInformation();

    /**
    * Resets BSS loss indication flags.
    */
    inline void ResetBssLossIndications();

    /**
    * Gets AP advertised supported rates IE
    * @return supported rates IE
    */
    inline SSupportedRatesIE& GetApSupportedRatesIE();

    /**
    * Gets AP advertised extended supported rates IE
    * @return extended supported rates IE
    */
    inline SExtendedSupportedRatesIE& GetApExtendedSupportedRatesIE();

    /** Resets Tx rate adaptation object */
    inline void ResetTxRateAdaptation();

    /**
    * Gets min basic rate of BSS 
    * @return min basic rate of BSS
    */
    inline TUint32& GetMinBasicRate();

    inline TUint32& GetMaxBasicRate();

    inline void ClearBasicRateSet();
    inline void BasicRateSetBitSet( const TUint32 aRateBitToSet );
    inline TUint32 BasicRateSet() const;

    inline TBool WsaCmdActive() const;
    inline void ActivateWsaCmd();
    inline void DeActivateWsaCmd();

    /**
    * Increments authentication sequnece expected counter 
    * to next expected sequence number
    */
    inline void IncrementAuthSeqNmbrExpected();

    /**
    * Sets the rate adaptation object
    */
    inline TBool SetTxRateAdaptationRates( 
        TUint8 aPolicyId,
        WHA::TRate aRateBitmask );

    /**
    * Sets the Tx rate adaptation policy to be used with the specified Tx queue
    * @param aQueueId id of the Tx queue
    * @param aPolicyId id of the Tx rate policy
    */
    inline void SetTxRatePolicy( WHA::TQueueId aQueueId, TUint8 aPolicyId );

    /**
    * Sets the current max Tx rate which should be initially used when sending
    * frames using the specified Tx rate policy.
    * If this rate is not present in the specified rate policy, or if
    * it is not supported either by the nw or by WHA layer, the next possible
    * lower rate will be used instead.
    * However, if the specified rate is lower that any rate in the specified
    * rate policy, the lowest rate in the policy will be used.
    *
    * @since S60 3.1
    * @param aPolicyId id of the Tx rate policy
    * @param aRate initial max Tx rate
    */
    inline void SetCurrentMaxTxRate( TUint8 aPolicyId, WHA::TRate aRate );

    /**
    * Sets the Tx rate adaptation algorithm parameters
    *
    * @since S60 3.1
    * @param aMinStepUpCheckpoint minimum and initial rate increase 
    *        checkpoint in units of frames
    *        Range: [aStepDownCheckpoint,aMaxStepUpCheckpoint]
    * @param aMaxStepUpCheckpoint maximum rate increase checkpoint in units 
    *        of frames
    *        Range: [aStepDownCheckpoint,UCHAR_MAX]
    * @param aStepUpCheckpointFactor StepUpCheckpoint is multiplied with this
    *        value if sending a probe frame fails
    *        Range: [1,aMaxStepUpCheckpoint]
    * @param aStepDownCheckpoint after this many frames the need to decrease
    *        the rate is checked
    *        Range: [2,UCHAR_MAX]
    * @param aMinStepUpThreshold minimum and initial StepUpThreshold 
    *        percentage
    *        Range: [1,aMaxStepUpThreshold]
    * @param aMaxStepUpThreshold maximum StepUpThreshold percentage
    *        Range: [1,100]
    * @param aStepUpThresholdIncrement StepUpThreshold is incremented by this
    *        value if sending a probe frame fails
    *        Range: [0,aMaxStepUpThreshold]
    * @param aStepDownThreshold if the percentage of frames which failed to
    *        be transmitted at the originally requested rate is at least 
    *        aStepDownThreshold at the aStepDownCheckpoint, the rate will
    *        be decreased
    *        Range: [1,100]
    * @param aDisableProbeHandling if EFalse, the rate adaptation algorithm
    *        handles the first frame transmitted after a rate increase in a
    *        special way. Otherwise the special handling is disabled
    */
    inline void SetTxRateAdaptationAlgorithmParams( 
        TUint8 aMinStepUpCheckpoint,
        TUint8 aMaxStepUpCheckpoint,
        TUint8 aStepUpCheckpointFactor,
        TUint8 aStepDownCheckpoint,
        TUint8 aMinStepUpThreshold,
        TUint8 aMaxStepUpThreshold,
        TUint8 aStepUpThresholdIncrement,
        TUint8 aStepDownThreshold,
        TBool aDisableProbeHandling );

    /**
    * To be called when BSS Lost indication is received from WLAN PDD
    *
    * @since S60 5.0
    * @return ETrue if Consecutive Beacons Lost indication needs to be sent to
    *         WLAN Mgmt Client
    *         EFalse otherwise
    */
    inline TBool OnConsecutiveBeaconsLost();

    /**
    * To be called after more than threshold number of consecutive Tx failures
    *
    * @since S60 5.0
    * @return ETrue if Consecutive Tx Failures indication needs to be sent to
    *         WLAN Mgmt Client
    *         EFalse otherwise
    */
    inline TBool OnConsecutiveTxFailures();

    /**
    * To be called after more than threshold number of consecutive 802.11 Power
    * Mgmt Mode setting failures
    *
    * @since S60 5.0
    * @return ETrue if Consecutive Power Mode Set Failures indication needs to
    *         be sent to WLAN Mgmt Client
    *         EFalse otherwise
    */
    inline TBool OnConsecutivePwrModeSetFailures();

    /**
    * To be called when we have successfully communicated with the current AP
    *
    * @since S60 3.1
    * @return ETrue if BSS Regained indication needs to be sent to WLAN Mgmt 
    *         Client
    *         EFalse otherwise
    */
    TBool OnBssRegained();

    /**
     * Pushes a packet to the packet scheduler
     *
     * @since S60 3.1
     * @param aPacket the packet to transmit
     * @param aLength length of the packet
     * @param aQueueId id of the queue to use when sending the packet
     * @param aPacketId frame type
     * @param aMetaHeader frame meta header
     * @param aMore ETrue if another frame is also ready to be transmitted
     *              EFalse otherwise
     * @param aMulticastData ETrue if this is a multicast data frame
     * @param aUseSpecialRatePolicy ETrue if use of the special Tx rate
     *        policy is requested for this frame Tx
     * @return ETrue packet was accepted, EFalse otherwise
     */
    inline TBool PushPacketToPacketScheduler( 
        const TAny* aPacket, 
        TUint32 aLength, 
        WHA::TQueueId aQueueId,
        TUint32 aPacketId,
        const TDataBuffer* aMetaHeader,
        TBool aMore,
        TBool aMulticastData,
        TBool aUseSpecialRatePolicy = EFalse );

    /**
     * Removes all pending packet transmission entrys from
     * the packet scheduler
     *
     * @since S60 3.1
     */
    inline void FlushPacketScheduler();

    /**
     * Schedules a packet and sends it if possible.
     * If no packets to schedule exists does nothing
     * If packet was send it is removed from the storage
     *
     * @since S60 3.1
     * @param aMore ETrue if another frame is also ready to be transmitted
     *              EFalse otherwise
     */
    inline void SchedulePackets( TBool aMore );

    /**
     * Are there unsent Tx packets, i.e. packets not yet completed by WHA 
     * layer
     *
     * @since S60 3.2
     * @return ETrue if there are packets not yet completed by WHA layer
     *         EFalse otherwise
     */
    inline TBool UnsentTxPackets() const;
    
    /**
     * Increments the failed Tx packet counter
     *
     * @since S60 3.1
     */
    inline void IncrementFailedTxPacketCount();

    /**
     * Returns the value of failed Tx packet counter
     *
     * @since S60 3.1
     */
    inline TUint8 FailedTxPacketCount() const;

    /**
     * Resets the failed Tx packet counter
     *
     * @since S60 3.1
     */
    inline void ResetFailedTxPacketCount();

    /**
     * Initializes the set power mgmt mode counter
     *
     * @since S60 3.1
     */
    inline void InitializeSetPsModeCount();

    /**
     * Decrements the set power mgmt mode counter by one
     *
     * @since S60 3.1
     */
    inline void DecrementSetPsModeCount();

    /**
     * Returns the value of the set power mgmt mode counter
     *
     * @since S60 3.1
     * @return Set power mgmt mode counter value
     */
    inline TUint SetPsModeCount() const;
                    
    /**
    * Starts dynamic power mode management
    *
    * @since S60 3.1
    */
    inline void StartPowerModeManagement();

    /**
    * Stops dynamic power mode management
    *
    * @since S60 3.1
    */
    inline void StopPowerModeManagement();
                    
    /**
    * To be called when transmitting a user data or WLAN Management Client frame
    *
    * Informs Dynamic Power Mode Manager about frame Tx.
    * Determines the need to make a power mode transition.
    *
    * @since S60 3.1
    * @param aQueueId Id of the queue/AC via which the frame will be 
    *                 transmitted
    * @param aEtherType Ethernet type of the frame
    * @param aDot11FrameType 802.11 frame type
    * @return To which power management mode to change; if any at all
    */
    inline TPowerMgmtModeChange OnFrameTx( 
        WHA::TQueueId aQueueId,
        TUint16 aEtherType,
        T802Dot11FrameControlTypeMask aDot11FrameType = E802Dot11FrameTypeData );

    /** 
    * To be called when accepting an Rx frame
    *
    * @since S60 3.1
    * @param aAccessCategory AC/Queue via which the frame has beeen transmitted
    * @param aEtherType Ethernet type of the received frame
    * @param aPayloadLength length of the ethernet frame payload
    * @param aDaType DA type (Unicast, Multicast or Broadcast) of the frame
    * @return To which power management mode to change; if any at all
    */
    inline TPowerMgmtModeChange OnFrameRx( 
        WHA::TQueueId aAccessCategory,
        TUint16 aEtherType,
        TUint aPayloadLength,
        TDaType aDaType );
                        
    /**
    * To be called when receiving the PS Mode Error indication
    * 
    * Informs Dynamic Power Mode Manager about the indication.
    * Determines the need to make a power mode transition.
    *
    * @return To which power management mode to change; if any at all
    */
    inline TPowerMgmtModeChange OnPsModeErrorIndication();
    
    /**
    * Sets the dynamic power mode transition algorithm parameters
    *
    * @since S60 3.1
    * @param aCtxImpl statemachine context
    * @param aToLightPsTimeout time interval in microseconds after which 
    *        transition from Active mode to Light PS mode is considered
    * @param aToLightPsFrameThreshold frame count threshold used when 
    *        considering transition from Active to Light PS mode
    * @param aToActiveTimeout time interval in microseconds after which the 
    *        frame counter used when considering transition from Light PS 
    *        to Active mode is reset
    * @param aToActiveFrameThreshold frame count threshold used when 
    *        considering transition from Light PS to Active mode
    * @param aToDeepPsTimeout time interval in microseconds after which 
    *        transition from Light PS mode to Deep PS mode is considered
    * @param aToDeepPsFrameThreshold frame count threshold used when 
    *        considering transition from Light PS to Deep PS mode
    * @param aUapsdRxFrameLengthThreshold received frame 
    *        payload length (in bytes) threshold in U-APSD network for
    *        Best Effort Access Category
    * @return ETrue if a state change occurred
    *         EFalse otherwise
    */
    inline void SetPowerModeManagementParameters(
        TUint32 aToLightPsTimeout,
        TUint16 aToLightPsFrameThreshold,
        TUint32 aToActiveTimeout,
        TUint16 aToActiveFrameThreshold,
        TUint32 aToDeepPsTimeout,
        TUint16 aToDeepPsFrameThreshold,
        TUint16 aUapsdRxFrameLengthThreshold );

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
    inline void ConfigurePwrModeMgmtTrafficOverride( 
        TBool aStayInPsDespiteUapsdVoiceTraffic,
        TBool aStayInPsDespiteUapsdVideoTraffic,
        TBool aStayInPsDespiteUapsdBestEffortTraffic, 
        TBool aStayInPsDespiteUapsdBackgroundTraffic,
        TBool aStayInPsDespiteLegacyVoiceTraffic,
        TBool aStayInPsDespiteLegacyVideoTraffic,
        TBool aStayInPsDespiteLegacyBestEffortTraffic,
        TBool aStayInPsDespiteLegacyBackgroundTraffic );

    /**
    * Freezes the dynamic power mode management traffic override settings 
    * provided earlier with the ConfigurePwrModeMgmtTrafficOverride() method
    * and based on whether U-APSD is used for the different ACs/Tx queues
    *
    * @since S60 3.2
    */
    inline void FreezePwrModeMgmtTrafficOverride();

    /**
    * Restores the Active mode parameters of dynamic power mode management 
    * back to their WLAN Mgmt Client provided values
    */
    inline void RestorePwrModeMgmtParameters();

    /**
    * To be called upon Active to Light PS timer timeout
    *
    * @since S60 v5.1
    * @return ETrue if power mode transition should be done, 
    *         EFalse otherwise
    */
    inline TBool OnActiveToLightPsTimerTimeout();

    /**
    * To be called upon Light PS to Active timer timeout
    *
    * @since S60 v5.1
    * @return ETrue if power mode transition should be done, 
    *         EFalse otherwise
    */
    inline TBool OnLightPsToActiveTimerTimeout();

    /**
    * To be called upon Light PS to Deep PS timer timeout
    *
    * @since S60 v5.1
    * @return ETrue if power mode transition should be done, 
    *         EFalse otherwise
    */
    inline TBool OnLightPsToDeepPsTimerTimeout();

    /**
    * Adds a multicast (Rx) address to our internal bookkeeping.
    * @param aMacAddress Address to be added
    * @return See TGroupAddStatus definition.
    */
    inline TGroupAddStatus AddMulticastAddress( 
        const TMacAddress& aMacAddress );

    /**
    * Removes a multicast (Rx) address from our internal bookkeeping.
    * @param aMacAddress MAC address to be removed
    * @return ETrue if address was found (and hence removed)
    *         EFalse otherwise
    */
    inline TBool RemoveMulticastAddress( const TMacAddress& aMacAddress );
    
    /**
    * Returns the number of multicast (Rx) addresses that exist in our
    * internal bookkeeping currently
    * @return Address count
    */
    inline TUint8 MulticastAddressCount() const;

    /**
    * Gets all the multicast (Rx) addresses from our internal bookkeeping
    * by passing back a pointer to the 1st address which is followed by 
    * all the other addresses.
    * @return The number of addresses returned.
    */
    inline TUint8 GetMulticastAddresses( 
        const TMacAddress*& aMacAddresses ) const;

    /**
    * Clears our internal multicast (Rx) address bookkeeping by removing
    * all addresses.
    */
    inline void ResetMulticastAddresses();
                 
    inline void MarkInternalTxBufFree();
    
    inline TBool InternalTxBufBeingWaited() const;
    
    inline void ClearInternalTxBufBeingWaitedFlag();
    
    /**
     * Resets frame statistics
     *
     * @since S60 3.2
     */
    inline void ResetFrameStatistics();

    /**
     * Increments Rx unicast data frame count by one
     *
     * @since S60 3.2
     * @param aAccessCategory AC/Queue via which the frame was transmitted
     */
    inline void IncrementRxUnicastDataFrameCount( 
        WHA::TQueueId aAccessCategory );

    /**
     * Increments Tx unicast data frame count by one
     *
     * @since S60 3.2
     * @param aAccessCategory AC/Queue via which the frame was transmitted
     */
    inline void IncrementTxUnicastDataFrameCount(
        WHA::TQueueId aAccessCategory );

    /**
     * Increments Rx multicast data frame count by one
     *
     * @since S60 3.2
     * @param aAccessCategory AC/Queue via which the frame was transmitted
     */
    inline void IncrementRxMulticastDataFrameCount(
        WHA::TQueueId aAccessCategory );

    /**
     * Increments Tx multicast data frame count by one
     *
     * @since S60 3.2
     * @param aAccessCategory AC/Queue via which the frame was transmitted
     */
    inline void IncrementTxMulticastDataFrameCount(
        WHA::TQueueId aAccessCategory );

    /**
     * Increments Tx frame retry count by aCount
     *
     * @since S60 3.2
     * @param aAccessCategory AC/Queue via which the frame was transmitted
     * @param aCount number to add to the current count
     */
    inline void IncrementTxRetryCount(
        WHA::TQueueId aAccessCategory,
        TUint aCount );

    /**
     * Increments Tx error count by one
     *
     * @since S60 3.2
     * @param aAccessCategory AC/Queue via which the frame was transmitted
     */
    inline void IncrementTxErrorCount(WHA::TQueueId aAccessCategory );

    /**
     * Increments cumulative Tx Media Delay by aDelay
     *
     * @since S60 3.2
     * @param aAccessCategory AC/Queue via which the frame was transmitted
     * @param aDelay Amount of delay to add to the current cumulative delay
     */
    inline void IncrementTxMediaDelay( 
        WHA::TQueueId aAccessCategory,
        TUint aDelay );

    /**
     * Returns the average Tx Media Delay of the specified Access Category
     *
     * Note! When this method is called the txMediaDelay field shall contain 
     * the cumulative Tx Media Delay.
     *
     * @since S60 3.2
     * @param aAccessCategory AC/Queue for which the average is requested
     * @return Average Tx Media Delay
     */
    TUint AverageTxMediaDelay( WHA::TQueueId aAccessCategory ) const;

    /**
     * Calculates the average Tx Media Delay for all Access Categories
     * and stores the results into frame statistics results
     *
     * @since S60 3.2
     */
    void CalculateAverageTxMediaDelays();

    /**
     * Increments cumulative Total Tx Delay by aDelay
     *
     * @since S60 3.2
     * @param aAccessCategory AC/Queue via which the frame was transmitted
     * @param aDelay Amount of delay to add to the current cumulative delay
     */
    inline void IncrementTotalTxDelay( 
        WHA::TQueueId aAccessCategory,
        TUint aDelay );

    /**
     * Updates Total Tx Delay histogram
     *
     * @since S60 3.2
     * @param aAccessCategory AC/Queue via which the frame was transmitted
     * @param aDelay Total Tx Delay of the frame
     */
    void UpdateTotalTxDelayHistogram( 
        WHA::TQueueId aAccessCategory,
        TUint aDelay );

    /**
     * Returns the average Total Tx Delay of the specified Access Category
     *
     * Note! When this method is called the totalTxDelay field shall contain 
     * the cumulative Total Tx Delay.

     * @since S60 3.2
     * @param aAccessCategory AC/Queue for which the average is requested
     * @return Average Total Tx Delay
     */
    TUint AverageTotalTxDelay( WHA::TQueueId aAccessCategory ) const;

    /**
     * Calculates the average Total Tx Delay for all Access Categories
     * and stores the results into frame statistics results
     *
     * @since S60 3.2
     */
    void CalculateAverageTotalTxDelays();

    /**
     * Stores FCS error count recorded in received MPDUs
     *
     * @since S60 3.2
     * @param aCount the value to set
     */
    inline void StoreFcsErrorCount( TUint aCount );

    /**
     * Returns frame statistics
     *
     * @since S60 3.2
     * @return reference to frame statistics
     */
    inline const TStatisticsResponse& FrameStatistics() const;

    /**
    * Sets Null Frame Send Controller parameters
    *
    * @since S60 3.2
    * @param aVoiceCallEntryTimeout when we are not in Voice over WLAN Call
    *        state and we transmit at least aVoiceCallEntryTxThreshold
    *        Voice priority frames during the time period (microseconds)
    *        denoted by this parameter, we enter Voice over WLAN Call state
    * @param aVoiceCallEntryTxThreshold Threshold value for the number of 
    *        Voice priority Tx frames to enter Voice over WLAN Call state    
    * @param aNullTimeout NULL frame sending interval
    * @param aNoVoiceTimeout after this long time of no Voice priority data 
    *                        Tx, exit voice call state
    * @param aKeepAliveTimeout Keep Alive frame sending interval in 
    *                          infrastructure mode
    */
    inline void SetNullSendControllerParameters(
        TUint32 aVoiceCallEntryTimeout,
        TUint32 aVoiceCallEntryTxThreshold,
        TUint32 aNullTimeout,
        TUint32 aNoVoiceTimeout,
        TUint32 aKeepAliveTimeout );

    /**
    * Starts Voice over WLAN Call maintenance
    *
    * @since S60 3.2
    */
    inline void StartVoiceOverWlanCallMaintenance();

    /**
    * Stops Voice over WLAN Call maintenance
    *
    * @since S60 3.2
    */
    inline void StopVoiceOverWlanCallMaintenance();

    /**
    * Terminates Voice over WLAN Call maintenance
    */
    inline void TerminateVoiceOverWlanCallMaintenance();
    
    /**
    * Resumes QoS null frame sending, if relevant.
    * Doesn't change the Voice over WLAN Call state
    *
    * @since S60 3.2
    */
    inline void ResumeQosNullSending();

    /**
    * Starts Keep Alive frame sending
    *
    * @since S60 3.2
    */
    inline void StartKeepAlive();

    /**
    * Stops Keep Alive frame sending
    *
    * @since S60 3.2
    */
    inline void StopKeepAlive();

    /**
     * To be called upon every RX Data frame (other than Null and QoS Null Data) 
     *
     * @since S60 3.2
     * @param aQueueId Id of the queue/AC via which the frame will be transmitted
     * @param aPayloadLength length of the ethernet frame payload
     */
     inline void OnDataRxCompleted( 
         WHA::TQueueId aQueueId,
         TUint aPayloadLength );
     
    /**
    * To be called upon every Data frame (other than Null and QoS Null Data) 
    * send completion
    *
    * @since S60 3.2
    * @param aQueueId Id of the queue/AC via which the frame will be transmitted
    */
    inline void OnDataTxCompleted( WHA::TQueueId aQueueId );

    /**
    * To be called upon every QoS Null Data frame send completion
    *
    * @since S60 3.2
    */
    inline void OnQosNullDataTxCompleted();
    
    /**
    * To be called upon every Null Data frame send completion
    *
    * @since S60 3.2
    */
    inline void OnNullDataTxCompleted();

    /**
    * To be called upon Voice Call Entry Timer timeout
    *
    * @since S60 3.2
    */
    inline void OnVoiceCallEntryTimerTimeout();

    /**
    * To be called upon Null Timer timeout
    *
    * @since S60 3.2
    */
    inline void OnNullTimerTimeout();

    /**
    * To be called upon Voice Timer timeout
    *
    * @since S60 3.2
    */
    inline void OnNoVoiceTimerTimeout();
    
    /**
    * To be called upon Keep Alive Timer timeout
    *
    * @since S60 3.2
    */
    inline void OnKeepAliveTimerTimeout();
    
    /** 
    * Are we currently in Voice over WLAN Call state
    * 
    * @return ETrue if we are
    *         EFalse if we are not
    */
    inline TBool InVoiceCallState() const;
    
    /**
     * Insert new RCPI value into the Signal Predictor.
     *
     * @since S60 3.2
     * @param aTimestamp Current time (in microseconds)
     * @param aRcpi RCPI value from HW
     * @return ETrue if an indication about weakening signal should be created
     *         EFalse otherwise
     */
    inline TBool InsertNewRcpiIntoPredictor( TInt64 aTimestamp, WHA::TRcpi aRcpi );

    /**
     * Gets the latest median RCPI value from Signal Predictor.
     *
     * @since S60 v3.2
     * @param aTimestamp Current time (in microseconds).
     * @param aLatestMedian reference to the median.
     * @return ETrue if median RCPI value is available. 
     *         EFalse otherwise
     */
    inline TBool GetLatestMedianRcpiFromPredictor( 
        TInt64 aTimestamp,
        WHA::TRcpi& aLatestMedian ) const;

    /**
     * Configures the Signal Predictor.
     *
     * @since S60 v5.0
     * @param aTimeToWarnLevel Specifies the time (in microseconds)
     *        how far into the future signal prediction is done.
     * @param aTimeToNextInd The minimum time difference (in 
     *        microseconds) between two signal loss prediction indications.
     * @param aRcpiWarnLevel If this RCPI level is predicted to be
     *        reached within the time specified by aSpTimeToCountPrediction,
     *        a signal loss prediction indication is sent. 
     */
    inline void ConfigureWlanSignalPredictor(
        TUint32 aTimeToWarnLevel,
        TUint32 aTimeToNextInd,
        WHA::TRcpi aRcpiWarnLevel );

    /**
     * Adds the specified WLAN feature supported by us to the list of those 
     * supported features which are indicated in BSS membership selector
     *
     * @param aFeature Feature to add
     */
    void AddBssMembershipFeature( T802Dot11BssMembershipSelector aFeature );
    
    /**
     * Removes the specified WLAN feature from our list of those supported
     * features which are indicated in BSS membership selector
     *
     * @param aFeature Feature to remove
     */
    void RemoveBssMembershipFeature( TUint8 aItem );
    
    /**
     * Clears (i.e. makes empty) our list of features which are indicated 
     * in BSS membership selector
     */
    inline void ClearBssMembershipFeatureList();
    
    /**
     * Checks if the specified item is a WLAN feature indicated in BSS 
     * membership selector and if it is supported by us
     *
     * @param aItem Item to check
     * @return ETrue if supported
     *         EFalse otherwise
     */
    TBool BssMembershipFeatureSupported( TUint8 aItem ) const;

    /**
    * Gets our HT Capabilities element
    * @return HT Capabilities element
    */
    inline SHtCapabilitiesIE& GetOurHtCapabilitiesIe();

    /**
    * Gets target/current network's HT Capabilities element
    * @return HT Capabilities element
    */
    inline SHtCapabilitiesIE& GetNwHtCapabilitiesIe();

    /**
    * Gets target/current network's HT Operation element
    * @return HT Operation element
    */
    inline SHtOperationIE& GetNwHtOperationIe();

    /**
    * Gets HT Block Ack configuration provided by WLAN Mgmt client
    * @return HT Block Ack configuration
    */
    inline WHA::ShtBlockAckConfigure& GetHtBlockAckConfigure();
    
    /**
    * Gets proprietary SNAP header provided by WLAN Mgmt client
    * @return Proprieatary SNAP header
    */
    inline SSnapHeader& GetProprietarySnapHeader();

    /**
     * Gets WHA layer transmission status
     *
     * @param aTxQueueState State (full / not full) of every WHA transmit queue
     * @return ETrue if the Tx pipeline is active, i.e. Tx frame submission
     *         is allowed
     *         EFalse if the Tx pipeline is not active, i.e. Tx frame submission
     *         is not allowed
     */
    inline TBool GetWhaTxStatus( TWhaTxQueueState& aTxQueueState ) const;
    
protected: 

    /**
     * Method called when packet has been transferred to the WLAN device
     *
     * @since S60 3.1
     * @param aCtxImpl global state machine context
     * @param aPacketId packet whose transfer is complete
     * @param aMetaHeader frame meta header
     */
    virtual void OnPacketTransferComplete( 
        WlanContextImpl& aCtxImpl, 
        TUint32 aPacketId,
        TDataBuffer* aMetaHeader );
    
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
     * Method called when packet has been flushed (removed)
     * from packet scheduler
     *
     * @since S60 3.1
     * @param aCtxImpl global state machine context
     * @param aPacketId packet that was flushed
     * @param aMetaHeader frame meta header
     */
    virtual void OnPacketFlushEvent(
        WlanContextImpl& aCtxImpl, 
        TUint32 aPacketId,
        TDataBuffer* aMetaHeader );

    /**
    * Method called when Packet Scheduler's packet scheduling method 
    * should be called, as there exists a packet that is suitable for 
    * transmission.
    * NOTE: if any other Packet Scheduler method is called within this
    * context the result is undefined.
    * 
    * @param aCtxImpl global state machine context
    * @param aMore ETrue if another frame is also ready to be transmitted
    *              EFalse otherwise
    */
    virtual void CallPacketSchedule( 
        WlanContextImpl& aCtxImpl,
        TBool aMore );
        
        virtual void OnPacketPushPossible( WlanContextImpl& aCtxImpl );

    virtual void CommandResponse( 
        WHA::TCommandId aCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandResponseParams& aCommandResponseParams );

    virtual void CommandComplete( 
        WHA::TCompleteCommandId aCompleteCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandCompletionParams& aCommandCompletionParams );

    virtual void Indication( 
        WHA::TIndicationId aIndicationId,
        const WHA::UIndicationParams& aIndicationParams );

    // Frame sending callbacks

    virtual void SendPacketTransfer(
        WHA::TPacketId aPacketId );

    virtual void SendPacketComplete(
        WHA::TStatus aStatus,
        WHA::TPacketId aPacketId,
        WHA::TRate aRate,
        TUint32 aPacketQueueDelay,
        TUint32 aMediaDelay,        
        TUint8 aAckFailures,
        TUint16 aSequenceNumber );


    // Frame receive

    virtual TAny* RequestForBuffer( TUint16 aLength );

    virtual void ReceivePacket( 
        WHA::TStatus aStatus,
        const void* aFrame,
        TUint16 aLength,
        WHA::TRate aRate,
        WHA::TRcpi aRcpi,
        WHA::TChannelNumber aChannel,
        void* aBuffer,
        TUint32 aFlags );

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
    
    // Prohibit copy constructor
    WlanContextImpl( const WlanContextImpl& aCntx );
    // Prohibit assigment operator
    WlanContextImpl& operator= ( const WlanContextImpl& );

public:     // data
            
    WlanMacState*       iCurrentMacState;  
    Umac&               iUmac;


    /** The states of the statemachine */
    States              iStates;
    /** 802.11 MIB */
    SWlanMib            iWlanMib;
    /** supported rates lookup table */
    SupportedRateLookUp 
        iSupportedRatesLookUpTable[KMaxNumberOfDot11bAndgRates];
    /** is protocol stack side datapath enabled or not */
    TBool               iEnableUserData;

    TUint8* iCurrentRxBuffer;

private:  // definitions

    /** 
    * max number of features in our WLAN feature array (see below)
    */
    static const TUint KMaxNumOfWlanFeatures = 1;
    
    /** 
    * value used in WLAN feature array for unallocated elements
    */
    static const TUint8 KUnallocated = 0;
    
    /** 
    * container type for IDs of those WLAN features which are indicated in BSS
    * membership selector
    */
    typedef TUint8 TWlanFeatureArray[KMaxNumOfWlanFeatures];

private:  // data

    /**
    * If a bit in this mask is set, use of the corresponding WLAN 
    * feature is allowed; otherwise it is not allowed.
    * Note that this mask doesn't contain all supported WLAN features
    * but only the ones which we allow to be be configured on or off.
    */ 
    TWlanFeatures               iAllowedWlanFeatures;
    
    /** 
    * ETrue if the Tx data buffer for frames created internally in this 
    * component is free (applies both to the DMA and the non-DMA Tx buffer). 
    * EFalse if it is in use 
    */
    TBool                       iInternalTxBufFree;

    /** 
    * ETrue if someone if waiting for the internal Tx Buffer to become 
    * available. EFalse othwerwise 
    */
    TBool                       iInternalTxBufBeingWaited;

    TWlanUserTxDataCntx         iTxDataCntx;
    TWlanUserTxDataCntx         iMgmtTxDataCntx;

    /** data frame header template */
    SDataFrameHeader            iDataFrameHeader;     

    TUint32                     iAlignNullData;
    SNullDataFrame              iNullDataFrame;

    TUint32                     iAlignQosNullData;
    SHtQosNullDataFrame         iQosNullDataFrame;

    /** authentication sequence expected counter */
    WlanAuthSeqNmbrExpected     iAuthSeqNmbrExpected;
    /** connection state info */

    WlanConnectContext          iConnectContext;
    /** available privacy mode filters */
    WlanPrivacyModeFilters      iPrivacyModeFilters;
    /** 
    * container for joined multicast (Rx) groups. Contains the
    * multicast MAC addresses denoting the groups.
    */
    JoinedMulticastGroups       iJoinedMulticastGroups;
    /** 
    * has Consecutive Beacons Lost indication already been sent to 
    * WLAN Mgmt Client
    */
    TBool iConsecutiveBeaconsLostIndicated;
    /** 
    * has Consecutive Tx Failures indication already been sent to 
    * WLAN Mgmt Client
    */
    TBool iConsecutiveTxFailuresIndicated;
    /** 
    * has Consecutive 802.11 Power Mgmt Mode Set Failures indication already 
    * been sent to WLAN Mgmt Client
    */
    TBool iConsecutivePwrModeSetFailuresIndicated;
    /** 
    * failed Tx packet counter used to determine if we should indicate
    * Consecutive Tx Failures
    */
    TUint8                      iFailedTxPacketCount;    
    /** 
    * counter which states how many times we will still try to change the 
    * 802.11 power management mode before indicating Consecutive 802.11 
    * Power Mgmt Mode Set Failures, if we continue to be unsuccessful in 
    * the power management mode changing
    */
    TUint                       iSetPsModeCount;

    /** is a WHA cmd active */
    TBool                       iWsaCmdActive;

    WlanTxRateAdaptation        iTxRateAdaptation;

    WlanPRNGenerator            iPrnGenerator;

    // Asynchronous Completion Token for WHA command
    TUint32                     iWhaCommandAct;

    /** 802.11 management frame templates */
    ManagementFrameTemplates    iManagementFrameTemplates;

    WHA::Wha*           iWha;     
    WHA::SSettings      iWHASettings;        

    SWsaCommands*       iWsaCommands;

    WlanEventDispatcher iEventDispatcher;
    WlanPacketScheduler iPacketScheduler;
    /** dynamic power mode context */
    WlanDynamicPowerModeMgmtCntx iDynamicPowerModeCntx;

    /** frame statistics counters */
    TStatisticsResponse iFrameStatistics;
    
    /** Null Data Frame sending controller */
    WlanNullSendController iNullSendController;

    /** WLAN signal predictor */
    WlanSignalPredictor iWlanSignalPredictor;    

    /** 
    * includes IDs of those WLAN features which are indicated in BSS membership
    * selector and which we support 
    */
    TWlanFeatureArray iOurBssMembershipFeatureArray;
    
    /** 
    * HT Capabilities element which we use to inform about our static HT 
    * capabilities
    */
    SHtCapabilitiesIE iOurHtCapabilitiesIe;
    
    /** HT Block Ack configuration provided by WLAN Mgmt Client */
    WHA::ShtBlockAckConfigure iHtBlockAckConfigure;

    /**
    * Otherwise valid received 802.11 Data frames containing this SNAP header
    * are accepted and forwarded to the WLAN Management Client. 
    */
    SSnapHeader iProprietarySnapHeader;
    };

#include "UmacContextImpl.inl"

#endif      // WLANCONTEXTIMPL_H
