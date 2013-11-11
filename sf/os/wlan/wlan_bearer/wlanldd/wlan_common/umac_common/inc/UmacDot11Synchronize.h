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
* Description:   Declaration of the UmacDot11Synchronize class
*
*/

/*
* %version: 20 %
*/

#ifndef WLANDOT11SYNCHRONIZE_H
#define WLANDOT11SYNCHRONIZE_H

#include "UmacDot11Roam.h"


/**
*  Joins to a new BSS network so that reassociation to that network 
*  can be performed.
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanDot11Synchronize : public WlanDot11Roam
    {
public:
    
    /**
    * C++ default constructor.
    */
    WlanDot11Synchronize();

    /**
    * Destructor.
    */
    virtual ~WlanDot11Synchronize();

protected:

    // Types for the FSMs
    //
    
    // events for the FSM
    enum TEvent
        {
        // state entry action to be executed
        ESTATEENTRY,
        // underlying sw layer tx delivery complete event 
        ETXCOMPLETE,        
        // abort FSM execution 
        EABORT,
        // defined as an upper bound
        EEVENTMAX           
        };

    // states of the FSM
    enum TState
        {
        // start state of the state machine
        EINIT,            
        ESETDOT11SLOTTIME,          // configure slottime mib if 
                                    // supported by the WLAN vendor solution
        ESETCTSTOSELF,              // configure ctstoself mib 
        ECONFTXQUEUE,               // configure just the legacy tx queue
        ECONFTXQUEUEPARAMS,         // configure tx queue parameters
        ESETTXRATEPOLICY,           // set Tx rate policy
        ESETHTCAPABILITIES,         // configure HT capabilities mib 
        ESETHTBSSOPERATION,         // configure HT BSS operation mib
        ERESETHTCAPABILITIES,       // reset HT capabilities mib
        EISSUEJOIN,                 // issue join command
        ESETHTBLOCKACKCONF,         // configure HT Block Ack configure mib
        ERESETHTBLOCKACKCONF,       // reset HT Block Ack configure mib
        ESETPAIRWISEKEY,            // set pairwise key
        ECONTINUEDOT11TRAVERSE,     // continue dot11 fsm traversal
        ESTATEMAX                   // defined as an upper bound                       
        }; 

    void Fsm( 
        WlanContextImpl& aCtxImpl, 
        TEvent aEvent );

    void OnStateEntryEvent( 
        WlanContextImpl& aCtxImpl );

    void OnTxCompleteEvent( 
        WlanContextImpl& aCtxImpl );

    void OnAbortEvent( WlanContextImpl& aCtxImpl );

    void ContinueDot11StateTraversal( 
        WlanContextImpl& aCtxImpl );

    void ChangeInternalState( 
        WlanContextImpl& aCtxImpl, 
        TState aNewState );

    TInt InitActions( WlanContextImpl& aCtxImpl ) const;
    
    void SetDot11SlotTime( 
        WlanContextImpl& aCtxImpl );

    void SetCtsToSelf( WlanContextImpl& aCtxImpl );
    
    void ConfigureQueue( WlanContextImpl& aCtxImpl );
        
    void ConfigureTxQueueParams( WlanContextImpl& aCtxImpl );
    
    void SetTxRatePolicy( WlanContextImpl& aCtxImpl );    

    /**
    * Informs lower layer about the HT capabilities
    * of the target network
    * @param aCtxImpl global statemachine context
    */
    void SetHtCapabilities( WlanContextImpl& aCtxImpl );    

    /**
    * Informs lower layer about the dynamic HT configuration
    * of the target network
    * @param aCtxImpl statemachine context
    */
    void SetHtBssOperation( WlanContextImpl& aCtxImpl );    

    /**
    * Resets the HT Capabilities MIB to its default value
    * @param aCtxImpl statemachine context
    */
    void ResetHtCapabilities( WlanContextImpl& aCtxImpl );
    
    /**
    * Resets the HT Block Ack Configure MIB to its default value
    * @param aCtxImpl statemachine context
    */
    void ResetHtBlockAckConfiguration( WlanContextImpl& aCtxImpl );

    void IssueJoin( 
        WlanContextImpl& aCtxImpl );

    /**
    * Informs lower layer about the HT Block Ack configuration
    * of the target network
    * @param aCtxImpl statemachine context
    */
    void SetHtBlockAckConfiguration( WlanContextImpl& aCtxImpl );

    /**
     * Sets pairwise key according to the used cipher suite
     *
     * @param aCtxImpl global statemachine context
     */
    void SetPtk( WlanContextImpl& aCtxImpl );

    /**
     * Sets CCMP key
     *
     * @param aCtxImpl global statemachine context
     */
    TBool SetCcmpPtk( WlanContextImpl& aCtxImpl );

    /**
     * Sets TKIP key
     *
     * @param aCtxImpl global statemachine context
     */
    TBool SetTkipPtk( WlanContextImpl& aCtxImpl );

    /**
     * Sets WEP key
     *
     * @param aCtxImpl global statemachine context
     */
    TBool SetWepKey( WlanContextImpl& aCtxImpl );
    
#ifndef NDEBUG 
    virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    void Entry( WlanContextImpl& aCtxImpl );
    
    void Exit( WlanContextImpl& aCtxImpl );
    
    virtual void OnWhaCommandResponse( 
        WlanContextImpl& aCtxImpl, 
        WHA::TCommandId aCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandResponseParams& aCommandResponseParams,
        TUint32 aAct );

private:

    // Prohibit copy constructor.
    WlanDot11Synchronize( const WlanDot11Synchronize& );
    // Prohibit assigment operator.
    WlanDot11Synchronize& operator=( const WlanDot11Synchronize& );

protected:    // Data
    
    TState              iState;

    /** status of the Join operation */
    TInt iJoinStatus;

#ifndef NDEBUG
    // max length of state name for tracing
    enum { KMaxStateStringLength = 50 };
    // max length of event name for tracing
    enum { KMaxEventStringLength = KMaxStateStringLength };

    // state names for tracing
    static const TUint8 iStateName[ESTATEMAX][KMaxStateStringLength];
    // event names for tracing
    static const TUint8 iEventName[EEVENTMAX][KMaxEventStringLength];         
    // name of the state 
    static const TInt8 iName[];        
#endif
    };

#endif      // WLANDOT11SYNCHRONIZE_H
