/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanConfigureTxQueueParams class
*
*/

/*
* %version: 10 %
*/

#ifndef WLAN_CONFIGURE_TX_QUEUE_PARAMS_H
#define WLAN_CONFIGURE_TX_QUEUE_PARAMS_H

#include "UmacWsaComplexCommand.h"

/**
 *  configures tx queue parameters of the device
 *
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class WlanConfigureTxQueueParams : public WlanWsaComplexCommand
    {

public: 

    /**
    * C++ default constructor.
    */
    WlanConfigureTxQueueParams() : 
         iState( EINIT ) {};

    /**
    * Destructor.
    */
    virtual ~WlanConfigureTxQueueParams() {};

private:

    // Types for the FSM

    // events for the FSM
    enum TEvent
        {
        ESTATEENTRY,    // state entry action to be executed                        
        ETXCOMPLETE,    // underlying sw layer tx delivery complete event   
        EEVENTMAX       // defined as an upper bound
        };

    // states of the FSM
    enum TState
        {
        EINIT,          // start state of the state machine
        ECONFIGUREBESTEFFORTQUEUE,
        ECONFIGUREVOICEQUEUE,
        ECONFIGUREVIDEOQUEUE,
        ECONFIGUREBACKGROUNDQUEUE,
        ECONFIGUREAC,
        EFINIT,
        ESTATEMAX       // defined as an upper bound        
        }; 

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void Fsm( 
        WlanContextImpl& aCtxImpl, 
        TEvent aEvent ); 

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnStateEntryEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnTxCompleteEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
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
    void ConfigureQueue( 
        WlanContextImpl& aCtxImpl,
        WHA::TQueueId aQueueId );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void ConfigureAc( WlanContextImpl& aCtxImpl );

    /**
     * From ?base_class1.
     * ?description
     *
     * @since Series 60 3.1
     * @param aCtxImpl global statemachine context
     */
    virtual void Entry( WlanContextImpl& aCtxImpl );

    /**
     * From ?base_class1.
     * ?description
     *
     * @since Series 60 3.1
     * @param aCtxImpl global statemachine context
     */
    virtual void Exit( WlanContextImpl& aCtxImpl);

    /**
     * From ?base_class1.
     * ?description
     *
     * @since Series 60 3.1
     * @param 
     */
#ifndef NDEBUG 
        virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    // Prohibit copy constructor.
    WlanConfigureTxQueueParams( const WlanConfigureTxQueueParams& );
    // Prohibit assignment operator.
    WlanConfigureTxQueueParams& operator= 
        ( const WlanConfigureTxQueueParams& );

private:    // Data

    TState              iState;

#ifndef NDEBUG
    /** max length of state name for tracing */
    enum { KMaxStateStringLength = 50 };
    /** max length of event name for tracing */
    enum { KMaxEventStringLength = KMaxStateStringLength };

    /** state names for tracing */
    static const TUint8 iStateName[ESTATEMAX][KMaxStateStringLength];
    /** event names for tracing */
    static const TUint8 iEventName[EEVENTMAX][KMaxEventStringLength];
    static const TInt8  iName[];
#endif // !NDEBUG

    };

#endif      // WLAN_CONFIGURE_TX_QUEUE_PARAMS_H
            
// End of File
