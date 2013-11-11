/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanDot11PrepareForBssMode class
*
*/

/*
* %version: 18 %
*/

#ifndef WLANDOT11PREPAREFORBSSMODE_H
#define WLANDOT11PREPAREFORBSSMODE_H

#include "UmacDot11State.h"

/**
*  State to join to a BSS network
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanDot11PrepareForBssMode : public WlanDot11State
    {

public:
    
    /**
    * C++ default constructor.
    */
    WlanDot11PrepareForBssMode() : iState( EINIT ), iJoinFailed ( EFalse ) {};
    
    /**
    * Destructor.
    */
    virtual ~WlanDot11PrepareForBssMode() {};

            
private:  // definitions

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
        EINIT,                      // start state of the state machine
        ESETSLEEPMODE,              // configure sleep mode mib
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
        ECONTINUEDOT11TRAVERSE,     // continue dot11 fsm traversal
        ESTATEMAX                   // defined as an upper bound        
        }; 

private:  

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void ChangeInternalState( 
        WlanContextImpl& aCtxImpl, 
        TState aNewState );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void Fsm( 
        WlanContextImpl& aCtxImpl, 
        TEvent aEvent ); 

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnStateEntryEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnTxCompleteEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnAbortEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void SetSleepMode( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void SetDot11SlotTime( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void SetCtsToSelf( WlanContextImpl& aCtxImpl );

    /**
     * ?member_description.
     *
     * @since Series 60 3.1
     * @param aCtxImpl global statemachine context
     */
    void ConfigureQueue( WlanContextImpl& aCtxImpl );

    /**
     * ?member_description.
     *
     * @since Series 60 3.1
     * @param aCtxImpl global statemachine context
     */
    void ConfigureTxQueueParams( WlanContextImpl& aCtxImpl );

    /**
     * ?member_description.
     *
     * @since Series 60 3.1
     * @param aCtxImpl global statemachine context
     */
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

    /**
    * Informs lower layer about the HT Block Ack configuration
    * of the target network
    * @param aCtxImpl statemachine context
    */
    void SetHtBlockAckConfiguration( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void IssueJoin( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void ContinueDot11StateTraversal( WlanContextImpl& aCtxImpl );

    // Functions from base classes

    /**
    * From ?base_class ?member_description
    */
    virtual void Entry( WlanContextImpl& aCtxImpl );

    /**
    * From ?base_class ?member_description
    */
    virtual void Exit( WlanContextImpl& aCtxImpl );

    /**
    * From ?base_class ?member_description
    */
    virtual void OnWhaCommandResponse( 
        WlanContextImpl& aCtxImpl, 
        WHA::TCommandId aCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandResponseParams& aCommandResponseParams,
        TUint32 aAct );
        
    /**
    * From ?base_class ?member_description
    * Returns the states name
    */
#ifndef NDEBUG 
        virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    // Prohibit copy constructor 
    WlanDot11PrepareForBssMode( const WlanDot11PrepareForBssMode& );
    // Prohibit assigment operator 
    WlanDot11PrepareForBssMode& operator=( 
        const WlanDot11PrepareForBssMode& );

private:    // Data

    // state of the object
    TState iState;
    
    // true, if the Join operation failed
    TBool iJoinFailed;

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

#endif      // WLANDOT11PREPAREFORBSSMODE_H
