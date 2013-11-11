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
* Description:   Declaration of the DWlanLogicalChannel class.
*
*/

/*
* %version: 33 %
*/

#ifndef DWLANLOGICALCHANNEL_H
#define DWLANLOGICALCHANNEL_H

#include "UmacManagementSideUmacCb.h"
#include "UmacProtocolStackSideUmacCb.h"
#include "wllddlogicalchannelbase.h"
#include "RWlanLogicalChannel.h"

#include "wllddoidmsgstorage.h"
#include "wlldddmausablememory.h"
#include "wllddpowerindicator.h"
#include "wllddpowerhandler.h"

#include "EtherCardApi.h"
#include "FrameXferBlock.h"

class DWlanLogicalDevice;
class Umac;

#ifdef RD_WLAN_DDK
class WlanOsa;
#endif

class DEthernetFrameMemMngr;
class RFrameXferBlock;
class WlanChunk;

class DWlanLogicalChannel : 
    public DWlanLogicalChannelBase,
    public WlanManagementSideUmacCb,
    public WlanProtocolStackSideUmacCb,
    public MWlanPowerIndicator
    {
    // type definitions
    // linked list entry for storing indications
    struct TIndicationListEntry
        {
        TIndication indication;        
        struct TIndicationListEntry *next;
        };

    /** max number of indication list entries */
    enum { KMaxIndicationListEntries = 6 };

public:

#ifndef RD_WLAN_DDK
    DWlanLogicalChannel( 
        DWlanLogicalDevice& aParent, 
        Umac& aUmac,
        TDynamicDfcQue& aDfcQ,
        DMutex& aMutex, 
        DChunk*& aSharedMemoryChunk,
        WlanChunk*& aRxFrameMemoryPool );
#else        
    DWlanLogicalChannel( 
        DWlanLogicalDevice& aParent,
        Umac& aUmac,
        TDynamicDfcQue& aDfcQ,
        WlanOsa*& aOsa,
        DChunk*& aSharedMemoryChunk,
        WlanChunk*& aRxFrameMemoryPool );
#endif        

    virtual ~DWlanLogicalChannel();

    /**
    * Returns a reference to a pointer pointing to the only shared memory
    * chunk instance
    *
    * @return See above
    */
	DChunk*& SharedMemoryChunk();

    /**
    * Sets the number of extra bytes required to align Rx buffer start
    * address, to be returned to WHA layer, to allocation unit boundary
    */
    void SetRxBufAlignmentPadding( TInt aRxBufAlignmentPadding );
    
    /**
    * Returns the number of extra bytes required to align Rx buffer start
    * address, to be returned to WHA layer, to allocation unit boundary
    * @return See above
    */
    TInt RxBufAlignmentPadding() const;
	
    /**	
    * Second phase constructor for DLogicalChannelBase objects.
    *
    * It is called in creating thread context in a critical section with
    * no fast mutexes	held.
    * The default implementation does nothing.
    *
    * @since S60 3.1
    * @param	aUnit	Requested unit number
    * @param	aInfo	Additional info supplied by client
    * @param	aVer	Requested interface version
    *
    * @return	KErrNone if construction was successful,
    *           otherwise one of the other system-wide error codes.
    */
	virtual TInt DoCreate(TInt aUnit, const TDesC8* aInfo, const TVersion& aVer);

	/**
	* Processes a message for this logical channel.
	* This function is called in the context of a DFC thread.
    *
    * @since S60 3.1
	* @param aMsg     The message to process.
	*                 The iValue member of this distinguishes the message type:
	*                 iValue==ECloseMsg, channel close message
	*                 iValue==KMaxTInt, a 'DoCancel' message
	*                 iValue>=0, a 'DoControl' message with function number equal to iValue
	*                 iValue<0, a 'DoRequest' message with function number equal to ~iValue
	*/
	virtual void HandleMsg(TMessageBase* aMsg);
	
    /**
     * Process a function for this logical channel.
     * @param aFunction Defines the operation/function to be performed.
     * @param param     Function specific parameter
     * @return function specific return value.
     */
	virtual TAny* DoControlFast( TInt aFunction, TAny* aParam );
	
private:

    /**
     * Process a function for this logical channel.
	 *
     * @param aFunction Defines the operation/function to be performed.
     * @param aParam Function specific parameter
     * @return Function specific return value.
     */
    TAny* OnMgmtSideControlFast( TInt aFunction, TAny* aParam );
    
    /**
     * Process a function for this logical channel.
	 *
     * @param aFunction Defines the operation/function to be performed.
     * @param aParam Function specific parameter
     * @param aTriggerTx ETrue if Tx operation should be triggered
     *                   EFalse otherwise
     * @return Function specific return value.
     */
    TAny* OnEthernetSideControlFast( 
        TInt aFunction, 
        TAny* aParam,
        TBool& aTriggerTx );    
    
    /**
    * Called from statemachine when oid has been completed. 
    * Triggers handling of a new oid
    * @param aOid OID handled
    * @param aReason completion code
    */
    virtual void OnOidCompleted( 
        TInt aReason, 
        SOidOutputData& OidOutputData );

    /**
    * Sets the Tx offset for every frame type which can be transmitted
    *
    * @since S60 5.0
    * @param aEthernetFrameTxOffset Tx offset for Ethernet frames and Ethernet
    *        Test frames
    * @param aDot11FrameTxOffset Tx offset for 802.11 frames
    * @param aSnapFrameTxOffset Tx offset for SNAP frames
    */
    virtual void SetMgmtSideTxOffsets( 
        TUint32 aEthernetFrameTxOffset,
        TUint32 aDot11FrameTxOffset,
        TUint32 aSnapFrameTxOffset );

    /**
    * Gets buffer for Rx data
    * @param aLengthinBytes Requested buffer length
    * @return buffer for Rx data upon success
    *         NULL otherwise
    */
    virtual TUint8* GetBufferForRxData( TUint aLengthinBytes );
    
    /**
    * Completes a data read operation from management side
    *
    * @param aBufferStart first element of the array that holds pointers to
    *        Rx frame meta headers
    * @param aNumOfBuffers number of meta header pointers in the array
    */
    virtual void MgmtDataReceiveComplete( 
        TDataBuffer*& aBufferStart, 
        TUint32 aNumOfBuffers );

    /**
    * Completes a data write operation from management side
    * @param aErr completion code 
    */
    virtual void MgmtPathWriteComplete ( TInt aErr );

    /**
    * Sends a indication to the management side
    * @param aIndication indication code 
    */
    virtual void OnInDicationEvent( TIndication aIndication );

    /**
    * Frees the specified Rx frame buffer
    *
    * @param aBufferToFree The buffer to free
    */
    virtual void MarkRxBufFree( TUint8* aBufferToFree );

    /**
    * Sets the Tx offset for every frame type which can be transmitted
    *
    * @since S60 5.0
    * @param aEthernetFrameTxOffset Tx offset for Ethernet frames
    */
    virtual void SetProtocolStackTxOffset( 
        TUint32 aEthernetFrameTxOffset );

    /**
    * Called when the Tx packet in question has been trasferred to the WLAN
    * device.
    * 
    * @param aCompletionCode Status of the operation.
    * @param aMetaHeader Meta header associated with the related Tx packet
    */
    virtual void OnTxProtocolStackDataComplete( 
        TInt aCompletionCode,
        TDataBuffer* aMetaHeader );

    /**
    * Called when a Tx packet - submitted by someone else than the Protocol
    * Stack Side Client - has been trasferred to the WLAN device.
    */
    virtual void OnOtherTxDataComplete();
    
    /**
    * Called when the Tx of a frame has completed (either successfully or
    * unsuccessfully). 
    */
    virtual void OnTxDataSent();
    
    virtual TUint8* DmaPrivateTxMemory();

    /**
    * Completes a data read operation from protocol stack side
    *
    * @param aBufferStart first element of the array that holds pointers to
    *        Rx frame meta headers
    * @param aNumOfBuffers number of meta header pointers in the array
    * @return ETrue if this event was processed successfully
    *         EFalse otherwise
    */
    virtual TBool ProtocolStackDataReceiveComplete( 
        TDataBuffer*& aBufferStart, 
        TUint32 aNumOfBuffers );

    /**
    * Determines if the Protocol Stack Side client is ready to handle any
    * callbacks from UMAC
    *
    * @return ETrue if the client is ready
    *         EFalse if the client is not ready
    */
    virtual TBool ProtocolStackSideClientReady() const;
    
    /**
    * From WlanProtocolStackSideUmacCb.
    * Indicates that WLAN Mgmt Client has (re-)enabled protocol stack side
    * user data Tx.
    */
    virtual void UserDataReEnabled();
    
    virtual void RegisterTimeout( 
        TUint32 aTimeoutInMicroSeconds, 
        TWlanTimer aTimer );
    virtual void CancelTimeout( TWlanTimer aTimer );

    virtual void RegisterDfc( TAny* aCntx );
    virtual void CancelDfc();

    /**
    * Allocates Rx frame meta header
    * @return Rx frame meta header upon success
    *         NULL otherwise
    */
    virtual TDataBuffer* GetRxFrameMetaHeader();

    /**
    * Deallocates Rx frame meta header
    * @param aMetaHeader Meta header to deallocate
    */
    virtual void FreeRxFrameMetaHeader( TDataBuffer* aMetaHeader );
    
    void DoCancel( TInt aMask );

    TInt DoRequest( 
        TInt aReqNo, 
        TRequestStatus* aStatus, 
        TAny* a1, 
        TAny* a2 );
    TInt OnMgmtSideDoRequest( 
        TInt aReqNo, 
        TRequestStatus* aStatus, 
        TAny* a1, 
        TAny* a2 );
    TInt OnEthernetSideRequest( 
        TInt aReqNo, 
        TRequestStatus* aStatus, 
        TAny* a1, 
        TAny* a2 );

    TInt DoControl( TInt aFunction, TAny* a1, TAny* a2);
    TInt OnMgmtSideControl( TInt aFunction, TAny* a1, TAny* a2 );
    TInt OnEthernetSideControl( TInt aFunction, TAny* a1, TAny* a2 );

    TInt OnInitialiseEthernetFrameBuffers( 
        TSharedChunkInfo* aSharedChunkInfo );

    void OnReleaseEthernetFrameBuffers();

    void TxProtocolStackData();
    void TxManagementData();

    TBool OnReadEthernetFrameRequest();

    /**
    * Stores a indication to indication list
    * @param aIndication indication to be stored
    */
    void StoreIndication( TIndication aIndication );

    /**
    * Gets first stored indication and assigns it to indication buffer
    * @return ETrue a stored indication existed, otherwise EFalse
    */
    TBool GetStoredIndication();

    /**
    * Calls GetStoredIndication() and after that 
    * completes the outstanding request
    */
    void TryToCompleteIndicationRequest();

    /**
    * Relases a indication list entry
    * @param entry to be relased
    */
    void ReleaseIndicationListEntry( TIndicationListEntry* aEntry );

    /**
    * Gets a next free indication list entry
    * @return a next free indication list entry
    */
    TIndicationListEntry* GetFreeIndicationListEntry();

    /**
    * Deallocates the indication list and marks all pointers NULL
    */
    void FreeIndicationList();

    /**
    * Clears all indication list entrys
    */
    void PurgeIndicationList();

    /**
    * From CWlanPhy
    * Registers the notification to the PDD.
    * This request is used for registering indication buffer to PDD
    * @param aBuffer pointer to TIndication 
    */
    void IndicationRequest( TIndication* aBuffer );

    /**
    * From CWlanPhy
    * Releases indication buffer from PDD
    */
	void CancelIndicationRequest();
    
    /**
    * Called by the PDD when a notification is received.
    */
    void IndicationComplete();

    TBool InitIndicationListEntries();

    /**
    * Default Timer timeout callback function
    *
    * @since S60 3.1
    * @param aPtr A pointer passed to the callback function when called.
    *             This pointer was given to the timer in timer construction.
    */
    static void OnTimeOut( TAny *aPtr );

    /**
    * Voice Call Entry Timer timeout DFC callback method
    *
    * @since S60 3.2
    */
    void RealVoiceCallEntryTimerDfc();

    /**
    * Voice Call Entry Timer timeout DFC callback function
    *
    * @since S60 3.2
    * @param aPtr this pointer (pointer to this object instance)
    */
    static void VoiceCallEntryTimerDfcDoToggle( TAny* aPtr );

    /**
    * Voice Call Entry Timer timeout callback function
    *
    * @since S60 3.2
    * @param aPtr A pointer passed to the callback function when called.
    *             This pointer was given to the timer in timer construction.
    */
    static void OnVoiceCallEntryTimerTimeOut( TAny *aPtr );

    /**
    * Null Timer timeout DFC callback method
    *
    * @since S60 3.2
    */
    void RealNullTimerDfc();

    /**
    * Null Timer timeout DFC callback function
    *
    * @since S60 3.2
    * @param aPtr this pointer (pointer to this object instance)
    */
    static void NullTimerDfcDoToggle( TAny* aPtr );

    /**
    * Null Timer timeout callback function
    *
    * @since S60 3.2
    * @param aPtr A pointer passed to the callback function when called.
    *             This pointer was given to the timer in timer construction.
    */
    static void OnNullTimerTimeOut( TAny *aPtr );

    /**
    * No Voice Timer timeout DFC callback method
    *
    * @since S60 3.2
    */
    void RealNoVoiceTimerDfc();

    /**
    * No Voice Timer timeout DFC callback function
    *
    * @since S60 3.2
    * @param aPtr this pointer (pointer to this object instance)
    */
    static void NoVoiceTimerDfcDoToggle( TAny* aPtr );

    /**
    * No Voice Timer timeout callback function
    *
    * @since S60 3.2
    * @param aPtr A pointer passed to the callback function when called.
    *             This pointer was given to the timer in timer construction.
    */
    static void OnNoVoiceTimerTimeOut( TAny *aPtr );

    /**
    * Keep Alive Timer timeout DFC callback method
    *
    * @since S60 3.2
    */
    void RealKeepAliveTimerDfc();

    /**
    * Keep Alive Timer timeout DFC callback function
    *
    * @since S60 3.2
    * @param aPtr this pointer (pointer to this object instance)
    */
    static void KeepAliveTimerDfcDoToggle( TAny* aPtr );

    /**
    * Keep Alive Timer timeout callback function
    *
    * @since S60 3.2
    * @param aPtr A pointer passed to the callback function when called.
    *             This pointer was given to the timer in timer construction.
    */
    static void OnKeepAliveTimerTimeOut( TAny *aPtr );

    /**
    * Active to Light PS Timer timeout DFC callback method
    *
    * @since S60 5.1
    */
    void RealActiveToLightPsTimerDfc();

    /**
    * Active to Light PS Timer timeout DFC callback function
    *
    * @since S60 5.1
    * @param aPtr this pointer (pointer to this object instance)
    */
    static void ActiveToLightPsTimerDfcDoToggle( TAny* aPtr );

    /**
    * Active to Light PS Timer timeout callback function
    *
    * @since S60 5.1
    * @param aPtr A pointer passed to the callback function when called.
    *             This pointer was given to the timer in timer construction.
    */
    static void OnActiveToLightPsTimerTimeOut( TAny *aPtr );
    
    /**
    * Light PS to Active Timer timeout DFC callback method
    *
    * @since S60 5.1
    */
    void RealLightPsToActiveTimerDfc();

    /**
    * Light PS to Active Timer timeout DFC callback function
    *
    * @since S60 5.1
    * @param aPtr this pointer (pointer to this object instance)
    */
    static void LightPsToActiveTimerDfcDoToggle( TAny* aPtr );

    /**
    * Light PS to Active Timer timeout callback function
    *
    * @since S60 5.1
    * @param aPtr A pointer passed to the callback function when called.
    *             This pointer was given to the timer in timer construction.
    */
    static void OnLightPsToActiveTimerTimeOut( TAny *aPtr );
    
    /**
    * Light PS to Deep PS Timer timeout DFC callback method
    *
    * @since S60 5.1
    */
    void RealLightPsToDeepPsTimerDfc();

    /**
    * Light PS to Deep PS Timer timeout DFC callback function
    *
    * @since S60 5.1
    * @param aPtr this pointer (pointer to this object instance)
    */
    static void LightPsToDeepPsTimerDfcDoToggle( TAny* aPtr );

    /**
    * Light PS to Deep PS Timer timeout callback function
    *
    * @since S60 5.1
    * @param aPtr A pointer passed to the callback function when called.
    *             This pointer was given to the timer in timer construction.
    */
    static void OnLightPsToDeepPsTimerTimeOut( TAny *aPtr );
    
    /**
    * Default Timer timeout DFC callback method
    *
    * @since S60 3.1
    */
    void RealTimerDfc();

    void RealDfc();

    /**
    * Default Timer timeout DFC callback function
    *
    * @since S60 3.1
    * @param aPtr this pointer (pointer to this object instance)
    */
    static void TimerDfcDoToggle( TAny* aPtr );

    static void DfcDoToggle( TAny* aPtr );
    
    /**
    * Protocol Stack Side Tx Trigger DFC callback method
    */
    void RealTxTriggerDfc();
    
    /**
    * Protocol Stack Side Tx Trigger DFC callback function
    *
    * @param aPtr this pointer (pointer to this object instance)
    */
    static void TxTriggerDfcDoToggle( TAny* aPtr );

    void FinitSystem();
    void InitSystem( TAny* aInputBuffer, TUint aInputLength );

    /**
    * Allocates kernel mode storage for actual parameters pointed to by
    * pointers in an OID command
    *
    * @since S60 3.1
    * @return ETrue if the operation succeeds;
    *         EFalse otherwise
    */
    TBool HandlePointerParameters();

    /**
    * Allocates kernel mode storage for scan response frame body and copies 
    * it from user side to kernel side from the connect command parameters
    *
    * @since S60 3.2
    * @return ETrue if the operation succeeds;
    *         EFalse otherwise
    */
    TBool HandleScanResponseFrameBodyCase();
    
    /**
    * Allocates kernel mode storage for IE(s) and copies it (them) from user 
    * side to kernel side when there is IE(s) present in the connect command
    *
    * @since S60 3.2
    * @return ETrue if the operation succeeds;
    *         EFalse otherwise
    */
    TBool CheckAndHandleIeDataCase();
    
    /**
    * Frees kernel mode storage allocated for scan response frame body
    *
    * @since S60 3.2
    */
    void FreeScanResponseFramebody();
    
    /**
    * Frees kernel mode storage allocated for IE(s)
    *
    * @since S60 3.2
    */
    void FreeIeData();
    
    /**
    * From MWlanPowerIndicator
    * Indicates that power up notification has been received from kernel 
    * side Power Manager.
    */
    virtual void OnPowerUp();

    /**
    * From MWlanPowerIndicator
    * Indicates that power down request has been received from kernel 
    * side Power Manager.
    *
    * @since S60 3.1
    */
    virtual void OnPowerDown();
        
    // Prohibit copy constructor.
    DWlanLogicalChannel ( const DWlanLogicalChannel & );
    // Prohibit assigment operator.
    DWlanLogicalChannel & operator= ( const DWlanLogicalChannel & );

private:    // data

    static const TUint32   KFreeOpenParamsMask            = ( 1 << 0 );
    static const TUint32   KFreeScanResponseFramebodyMask = ( 1 << 1 );    
    static const TUint32   KDfcCancelledMask              = ( 1 << 2 );
    static const TUint32   KFreeIeDataMask                = ( 1 << 3 );
    static const TUint32   KPowerHandlerRegistered        = ( 1 << 4 );
    static const TUint32   KTxTriggerArmed                = ( 1 << 5 );

    TOpenParam      iOpenParam;

    DThread*        iClient;   
    TInt            iUnit;
    Umac&           iUmac;
    /** pointer to our DFC queue. Not own */
    TDynamicDfcQue* iDfcQ;
    DWlanLogicalDevice& iParent;

    /** management side requests */
    TRequestStatus* iWlanGeneralRequestStatus;
    TRequestStatus* iWlanRequestNotifyStatus;
    TRequestStatus* iWlanReceiveFrameStatus;
    TRequestStatus* iWlanSendFrameStatus;    

    /** indication request buffer to USER mode */
	TIndication*	        iIndicationBuffer;
    /** pointer to first stored indication */
	TIndicationListEntry*   iIndicationListHead;
    /** pointer to the first free indication list entry */
	TIndicationListEntry*	iFreeIndicationListHead;

    /** protocol stack side requests */
    TRequestStatus* iResumeTxStatus;
    TRequestStatus* iEthernetReceiveFrameStatus;

    /** Pointer to ethernet memory manager */
    DEthernetFrameMemMngr*  iEthernetFrameMemMngr;

    /** Default Timer DFC object */
    TDfc                    iTimerDfc;
    /** Default Timer object */
    NTimer                  iTimer;
    /** Voice Call Entry Timer object */
    NTimer                  iVoiceCallEntryTimer;
    /** Voice Call Entry Timer DFC object */
    TDfc                    iVoiceCallEntryTimerDfc;
    /** NULL Timer object */
    NTimer                  iNullTimer;
    /** NULL Timer DFC object */
    TDfc                    iNullTimerDfc;
    /** No Voice Timer object */
    NTimer                  iNoVoiceTimer;
    /** No Voice Timer DFC object */
    TDfc                    iNoVoiceTimerDfc;
    /** Keep Alive Timer object */
    NTimer                  iKeepAliveTimer;
    /** Keep Alive Timer DFC object */
    TDfc                    iKeepAliveTimerDfc;
    /** Active to Light PS Timer object */
    NTimer                  iActiveToLightPsTimer;
    /** Active to Light PS Timer DFC object */
    TDfc                    iActiveToLightPsTimerDfc;
    /** Light PS to Active Timer object */
    NTimer                  iLightPsToActiveTimer;
    /** Light PS to Active Timer DFC object */
    TDfc                    iLightPsToActiveTimerDfc;
    /** Light PS to Deep PS Timer object */
    NTimer                  iLightPsToDeepPsTimer;
    /** Light PS to Deep PS DFC object */
    TDfc                    iLightPsToDeepPsTimerDfc;
    
    TDfc                    iDfc;
    /** Protocol Stack Side Tx Trigger DFC object */
    TDfc                    iTxTriggerDfc;

    SOidMsgStorage          iOidMsgStorage;
    TUint32                 iFlags;
    
    /** our power handler */
    DWlanPowerHandler       iPowerHandler;
    /** 
    * ETrue if we are powering down because power manager has requested us
    * to do so
    */
    TBool                   iPoweringDown;

    TAny*                   iDfcCtx;

    WlanDmaUsableMemory     iDmaTxMemory;

#ifndef RD_WLAN_DDK
    /** 
    * reference to mutex used to protect LDD from simultaneous execution 
    * by several different threads. 
    */
    DMutex&                 iMutex;
#else
    /** 
    * OS abstraction object reference
    */
    WlanOsa*&               iOsa;
#endif    

    /** 
    * reference to shared memory chunk for frame transfer between user and
    * kernel address spaces
    */
    DChunk*&                iSharedMemoryChunk;

    /** 
    * reference to Rx frame memory pool manager
    */
    WlanChunk*&             iRxFrameMemoryPool;
    
    /** 
     * ETrue if the Tx packet submission loop is active. 
     * Is used to deny calling UMAC TxProtocolStackData() if the loop is
     * already active. This prevents also recursive call stack growth.
     */
    TBool                   iTxActive;
    
    /** 
    * ETrue if the user mode client is allowed to add new frames to Tx
    * queues 
    */
    TBool                   iAddTxFrameAllowed;
    };


#endif // DWLANLOGICALCHANNEL_H

