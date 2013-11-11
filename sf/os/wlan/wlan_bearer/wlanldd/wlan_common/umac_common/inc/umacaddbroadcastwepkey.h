/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanAddBroadcastWepKey class.
*
*/

/*
* %version: 7 %
*/

#ifndef WLANADDBROADCASTWEPKEY_H
#define WLANADDBROADCASTWEPKEY_H

#include "UmacWsaComplexCommand.h"


/**
*  FSM for inserting a single default WEP key 
*  Inserts it as group and pairwise key 
*  and also marks it as the default group key
*
*  @lib wlanumac.lib
*  @since S60 3.1
*/
class WlanAddBroadcastWepKey : public WlanWsaComplexCommand
    {

    // Types for the FSM
    
    // events for the FSM
    enum TEvent
        {
        ESTATEENTRY,    // state entry action to be executed                        
        ETXCOMPLETE,    // underlying sw layer tx delivery complete event   
        EABORT,         // abort execution event
        EEVENTMAX       // defined as an upper bound
        };

    // states of the FSM
    enum TState
        {
        EINIT,          // start state of the state machine
        EADDGROUPKEY,   // add group key
        EWRITEMIB,      // write mib (conditional exec)
        EADDPAIRWISEKEY,// add pairwise key (conditional exec)
        EFINIT,         // end state
        ESTATEMAX       // defined as an upper bound        
        }; 

public:

    /**
    * C++ default constructor.
    */
    WlanAddBroadcastWepKey() : 
        iState( EINIT ), iFlags( 0 ), iKeyIndex( 0 ), iKeyLength( 0 ), 
        iKey( NULL ), iMacAddr( KZeroMacAddr ), iMemory( NULL ) {};

    /**
    * Destructor.
    */
    virtual inline ~WlanAddBroadcastWepKey();

    /**
    * ?member_description.
    * @since S60 3.1
    * @param 
    */
    void Set( 
        const TMacAddress& aMac,
        TUint32 aKeyIndex,             
        TBool aUseAsDefaulKey,     
        TBool aUseAsPairwiseKey,
        TUint32 aKeyLength,                      
        const TUint8 aKey[KMaxWEPKeyLength] );

private:

    // Prohibit copy constructor.
    WlanAddBroadcastWepKey( const WlanAddBroadcastWepKey& );
    // Prohibit assigment operator.
    WlanAddBroadcastWepKey& operator= 
        ( const WlanAddBroadcastWepKey& );

    virtual void Entry( WlanContextImpl& aCtxImpl );
    virtual void Exit( WlanContextImpl& aCtxImpl);

#ifndef NDEBUG 
    virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

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
    void OnAbortEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void AddGroupKey( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void AddPairwiseKey( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void WriteMib( WlanContextImpl& aCtxImpl );

    /**
    * evaluates if key is to be marked as the default key 
    * @since S60 3.1
    * @return ETrue when evaluates true
    */
    inline TBool UseAsDefaultKey() const;

    /**
    * evaluates if key is inserted as a pairwise key
    * @since S60 3.1
    * @return ETrue when evaluates true
    */
    inline TBool UseAsPairwiseKey() const;

    static const TUint32 KUseAsDefaultKey      = ( 1 << 0 );
    static const TUint32 KUseAsPairwiseKey     = ( 1 << 1 );

private:    // Data

    TState          iState;

    TUint32         iFlags;
    TUint32         iKeyIndex;             
    TUint32         iKeyLength;                      
    /**
    * cipher key to add
    * Not own
    */
    const TUint8*   iKey;
    TMacAddress     iMacAddr;

    // general purpose memory handle for the object
    TAny*            iMemory;

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

#include "umacaddbroadcastwepkey.inl"

#endif      // WLANADDBROADCASTWEPKEY_H
