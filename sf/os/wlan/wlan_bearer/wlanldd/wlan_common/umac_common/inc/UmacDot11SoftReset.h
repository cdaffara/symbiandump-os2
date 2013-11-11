/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanDot11SoftReset class
*
*/

/*
* %version: 14 %
*/

#ifndef WLANDOT11SOFTRESET_H
#define WLANDOT11SOFTRESET_H

#include "UmacDot11State.h"

/**
 *  Resets the WHA layer back to the same initial state which it was in after
 *  Initialize and Configure WHA commands. However, does not reset the MIB
 *  elements.
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class WlanDot11SoftReset : public WlanDot11State
    {    
public:
   
    /**
    * C++ default constructor.
    */
    WlanDot11SoftReset() : iState( EINIT ) {};

    /**
    * Destructor.
    */
    virtual ~WlanDot11SoftReset() {};

protected:

    // Types for the FSM
    //
    
    // events for the FSM
    enum TEvent
        {
        // state entry action to be executed
        ESTATEENTRY,
        // underlying sw layer tx delivery complete event 
        ETXCOMPLETE,        
        // abort execution event
        EABORT,
        // defined as an upper bound
        EEVENTMAX           
        };

    // states of the FSM
    enum TState
        {
        // start state of the state machine
        EINIT,   
        EISSUEDISCONNECT,
        ECONFTXQUEUE,
        ECONTINUEDOT11TRAVERSE,
        // defined as an upper bound        
        ESTATEMAX                       
        }; 

    /**
     * ?member_description.
     *
     * @since Series 60 3.1
     * @param aCtxImpl global statemachine context
     */
    void Fsm( 
        WlanContextImpl& aCtxImpl, 
        TEvent aEvent );

    /**
     * ?member_description.
     *
     * @since Series 60 3.1
     * @param aCtxImpl global statemachine context
     */
    void OnStateEntryEvent( 
        WlanContextImpl& aCtxImpl );

    /**
     * Performs state initialization actions.
     *
     * @since Series 60 3.1
     * @param aCtxImpl global statemachine context
     */
    virtual void InitActions( WlanContextImpl& aCtxImpl );

    /**
     * ?member_description.
     *
     * @since Series 60 3.1
     * @param aCtxImpl global statemachine context
     */
    void OnTxCompleteEvent( 
        WlanContextImpl& aCtxImpl );

    /**
     * ?member_description.
     *
     * @since Series 60 3.1
     * @param aCtxImpl global statemachine context
     */
    void OnAbortEvent( WlanContextImpl& aCtxImpl );

    /**
     * ?member_description.
     *
     * @since Series 60 3.1
     * @param aCtxImpl global statemachine context
     */
    void ContinueDot11StateTraversal( 
        WlanContextImpl& aCtxImpl );

    /**
     * ?member_description.
     *
     * @since Series 60 3.1
     * @param aCtxImpl global statemachine context
     */
    void ChangeInternalState( 
        WlanContextImpl& aCtxImpl, 
        TState aNewState );

    /**
     * ?member_description.
     *
     * @since Series 60 3.1
     * @param aCtxImpl global statemachine context
     */
    void IssueDisconnect( WlanContextImpl& aCtxImpl );

    /**
     * ?member_description.
     *
     * @since Series 60 3.1
     * @param aCtxImpl global statemachine context
     */
    void ConfigureQueue( WlanContextImpl& aCtxImpl );
                
    /**
     * From ?base_class1.
     * Returns the states name
     *
     * @since Series 60 3.1
     * @param aLength (OUT) length of the name of the state
     * @return name of the state
     */
#ifndef NDEBUG 
        virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    /**
     * From ?base_class1.
     * ?description
     *
     * @since Series 60 3.1
     * @param aCtxImpl global statemachine context
     */
    virtual void Entry( WlanContextImpl& aCtxImpl);

    /**
     * From ?base_class1.
     * ?description
     *
     * @since Series 60 3.1
     * @param aCtxImpl global statemachine context
     */
    virtual void Exit( WlanContextImpl& aCtxImpl);

private:

    // Prohibit copy constructor 
    WlanDot11SoftReset( const WlanDot11SoftReset& );
    // Prohibit assigment operator
    WlanDot11SoftReset& operator= ( const WlanDot11SoftReset& );   

protected: // Data

    TState              iState;       

#ifndef NDEBUG
    // max length of state name for tracing
    enum { KMaxStateStringLength = 50 };
    // max length of event name for tracing
    enum { KMaxEventStringLength = KMaxStateStringLength };

    // state names for tracing
    static const TUint8 iStateName[ESTATEMAX][KMaxStateStringLength];
    // event names for tracing
    static const TUint8 iEventName[EEVENTMAX][KMaxEventStringLength];             
    /** name of the state */
    static const TInt8 iName[];        
#endif
    };

#endif      // WLANDOT11SOFTRESET_H
