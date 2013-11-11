/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the Umac class.
*
*/

/*
* %version: 32 %
*/

#ifndef UMAC_H
#define UMAC_H

#include "umaceventdispatcherclient.h"
#include "UmacProtocolStackSideUmac.h"
#include "UmacProtocolStackSideUmacCb.h"
#include "UmacManagementSideUmac.h"
#include "UmacManagementSideUmacCb.h"
#include "UmacTimerClient.h"
#include "umacdfcclient.h"

#ifndef RD_WLAN_DDK
#include <wha.h>
#else
#include <wlanwha.h>
#endif


class WlanContextImpl;
class WlanConnectContext;
class WlanPrivacyModeFilters;

class Umac : 
    public MWlanProtocolStackSideUmac, 
    public WlanProtocolStackSideUmacCb,
    public WlanManagementSideUmacCb,
    public MWlanManagementSideUmac,
    public MWlanTimerClient,
    public MWlanEventDispatcherClient,
    public MWlanDfcClient
    {

public:

    /**
    * C++ default constructor.
    */
    Umac();

    virtual ~Umac();

    virtual void OnTimeout( TWlanTimer aTimer );

    virtual void OnDfc( TAny* aCtx );
    
    void CompleteManagementCommand( 
        TInt aReason, 
        const TAny* aData = NULL, 
        TUint32 aLengthInBytes = 0 );
    
    /**
    * From MManagementSideUmac
    * OID request handler routine
    * @param aInputBuffer Pointer to the input buffer (set-operations)
    * @param aInputBufferSize Size of the buffer pointed to by aInputBuffer
    * @param aOutputBuffer Pointer to the output buffer (query-operations)
    * @param aOutputBufferSize Size of the buffer pointed to by aOutputBuffer
    * @return Status of the operation see #TStatus
    */
    virtual void HandleOid( 
        const TOIDHeader* aOid,             
        TAny* aOutputBuffer, 
        TUint aOutputBufferSize );

    /**
    * From MManagementSideUmac
    */
    virtual void AttachWsa( WHA::Wha* aWha );
    virtual TBool Init();
    virtual void BootUp(
        const TUint8* aPda, 
        TUint32 aPdaLength,
        const TUint8* aFw, 
        TUint32 aFwLength );

    virtual void FinitSystem();

    // From MWlanProtocolStackSideUmac
    
    virtual const TMacAddress& StationId() const;

    /**
    * Triggers the setting of the Tx offset on the protocol stack side for 
    * every frame type which can be transmitted
    */
    virtual void SetTxOffset();

    /**
    * Transmit a protocol stack frame
    * 
    * The frame to be sent needs to be in 802.3 format
    * @param aDataBuffer meta header of the frame to be transmitted
    * @param aMore ETrue if another frame is also ready to be transmitted
    *              EFalse otherwise
    */
    virtual void TxProtocolStackData( 
        TDataBuffer& aDataBuffer,
        TBool aMore );

    /**
    * Gets the WLAN vendor needs for extra space (bytes) in frame buffers
    *  
    * @param aRxOffset How much extra space needs to be reserved
    *        in the Rx buffer before every Rx frame that is received from the
    *        WHA layer.
    * @param aTxHeaderSpace How much extra space needs to be reserved
    *        in the Tx buffer before every Tx frame that is given to the 
    *        WHA layer.
    * @param aTxTrailerSpace How much extra space needs to be reserved
    *        in the Tx buffer after every Tx frame that is given to the 
    *        WHA layer.
    */
    virtual void GetFrameExtraSpaceForVendor( 
        TUint8& aRxOffset,
        TUint8& aTxHeaderSpace,
        TUint8& aTxTrailerSpace ) const;    

    /**
     * From MWlanProtocolStackSideUmac
     * Is protocol stack side transmission permitted
     *
     * @param aTxQueueState If the return value is ETrue, the state 
     *        (full / not full) of every WHA transmit queue. Otherwise, not
     *        valid.
     *        Note! A frame shall not be submitted to a full queue - even if
     *        the return value would be ETrue
     * @return ETrue if Tx frame submission is permitted
     *         EFalse if Tx frame submission is not permitted
     */
    virtual TBool TxPermitted( TWhaTxQueueState& aTxQueueState ) const;
    
    /**
     * From MWlanProtocolStackSideUmac
     * Is user data Tx enabled
     *
     * @return ETrue if user data Tx is enabled
     *         EFalse if user data Tx is not enabled
     */
    virtual TBool UserDataTxEnabled() const;
    
    virtual void WriteMgmtFrame(TDataBuffer& aDataBuffer);       

private:

    /**
    * Connect OID request handler
    * @param aInputBuffer Pointer to the input buffer
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    virtual TBool HandleConnect(const TAny *aInputBuffer);

    /**
    * Start IBSS OID request handler
    * @param aInputBuffer Pointer to the input buffer
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    virtual TBool HandleStartIBSS(const TAny *aInputBuffer);

    /**
    * Disconnect OID request handler
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    virtual TBool HandleDisconnect();

    /**
    * Set power mode OID request handler
    * @param aInputBuffer Pointer to the input buffer
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    virtual TBool HandleSetPowerMode(const TAny *aInputBuffer);

    /**
    * Set RCPI trigger level OID request handler
    * @param aInputBuffer Pointer to the input buffer
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    virtual TBool HandleSetRcpiTriggerLevel(const TAny *aInputBuffer);

    /**
    * Set Tx power level OID request handler
    * @param aInputBuffer Pointer to the input buffer
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    virtual TBool HandleSetTxPowerLevel(const TAny *aInputBuffer);

    /**
    * Configure OID request handler
    * @param aInputBuffer Pointer to the input buffer
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    virtual TBool HandleConfigure(const TAny *aInputBuffer);

    /**
    * Scan OID request handler
    * @param aInputBuffer Pointer to the input buffer
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    virtual TBool HandleScan(const TAny *aInputBuffer);

    /**
    * Stop scan OID request handler
    *
    * @since S60 3.2
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    TBool HandleStopScan();

    /**
    * Get last RCPI OID request handler
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    virtual TBool HandleGetLastRcpi();     

    /**
    * Disable user data OID request handler
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    virtual TBool HandleDisableUserData();   

    /**
    * Enable user data OID request handler
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    virtual TBool HandleEnableUserData();   

    /**
    * Add cipher key OID request handler
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */        
    virtual TBool HandleAddCipherKey(const TAny *aInputBuffer);
    
    /**
    * Add multicast address OID request handler
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */        
    virtual TBool HandleAddMulticastAddr(const TAny *aInputBuffer);

    /**
    * Remove multicast address OID request handler
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */        
    virtual TBool HandleRemoveMulticastAddr(const TAny *aInputBuffer);
    
    /**
    * BSS lost configure OID request handler
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */        
    virtual TBool HandleConfigureBssLost(const TAny *aInputBuffer);

    /**
    * Set Tx rate adaptation parameters OID request handler
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */        
    virtual TBool HandleSetTxRateAdaptParams(const TAny *aInputBuffer);

    /**
    * Configure Tx rate policies OID request handler
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */        
    virtual TBool HandleConfigureTxRatePolicies(const TAny *aInputBuffer);

    /**
    * Set power mode management parameters OID request handler
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */        
    virtual TBool HandleSetPowerModeMgmtParams(const TAny *aInputBuffer);

    /**
    * Configure PS Mode Traffic Override OID request handler
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */        
    virtual TBool HandleConfigurePwrModeMgmtTrafficOverride( 
        const TAny *aInputBuffer );

    /**
    * Get Frame Statistics OID request handler
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */        
    virtual TBool HandleGetFrameStatistics();

    /**
    * Configure U-APSD OID request handler
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */        
    virtual TBool HandleConfigureUapsd( const TAny *aInputBuffer );

    /**
    * Configure Tx Queue OID request handler
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */        
    virtual TBool HandleConfigureTxQueue( const TAny *aInputBuffer );

    /**
    * Get MAC Address OID request handler
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */        
    virtual TBool HandleGetMacAddress();

    /**
    * Configure ARP IP Address Filtering OID request handler
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */        
    virtual TBool HandleConfigureArpIpAddressFiltering( 
        const TAny *aInputBuffer );

    /**
    * Configure HT Block Ack OID request handler
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */        
    virtual TBool HandleConfigureHtBlockAck( const TAny *aInputBuffer );
    
    /**
    * Configure Proprietary SNAP Header OID request handler
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */        
    virtual TBool HandleConfigureProprietarySnapHdr( 
        const TAny *aInputBuffer );
    
    /**
    * 
    */        
    virtual TBool OnWhaCommandComplete( 
        WHA::TCompleteCommandId aCompleteCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandCompletionParams& aCommandCompletionParams );
                     
    /**
    * 
    */        
    virtual TBool OnInternalEvent( TInternalEvent aInternalEvent );

    // Prohibit copy constructor
    Umac( const Umac& );
    // Prohibit assigment operator
    Umac& operator= ( const Umac& );
    
private:   // Data

    /** pointer to our implementation details (pimpl-idiom) */
    WlanContextImpl*                            iPimpl;

    TBool                                       iManagementRequestPending;
    WlanManagementSideUmacCb::SOidOutputData    iOidOutputData;
    };

#endif // UMAC_H

