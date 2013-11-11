/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanEventDispatcher class.
*
*/

/*
* %version: 17 %
*/

#ifndef WLANEVENTDISPATCHER_H
#define WLANEVENTDISPATCHER_H

#ifndef RD_WLAN_DDK
#include <wha.h>
#else
#include <wlanwha.h>
#endif

#include "umacoidmsg.h"

#include "umac_types.h"

class WlanContextImpl;
class MWlanEventDispatcherClient;

/**
 *  Event dispatcher which is used to serialize MAC prototocl
 *  statemachine access.
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class WlanEventDispatcher
    {

public:

    /**  channel identifier type */
    typedef TUint32 TChannel;

    /**  oid channel identifier */
    static const TChannel KOidChannel = ( 1 << 0 );

    explicit WlanEventDispatcher( 
        MWlanEventDispatcherClient& aEventDispatcherClient );
        
    inline ~WlanEventDispatcher();

    /**
     * Registers a WHA command completion event
     *
     * @since S60 3.1
     * @param aCompleteCommandId ID of the WHA command
     * @param aStatus Command completion status
     * @param aCommandCompletionParams Command completion output parameters
     */
    void Register( 
        WHA::TCompleteCommandId aCompleteCommandId,
        WHA::TStatus aStatus,
        const WHA::UCommandCompletionParams& aCommandCompletionParams );

    /**
     * Registers a WLAN Mgmt Client command
     *
     * @since S60 3.1
     * @param aOid Command parameters
     */
    inline void Register( const TOIDHeader& aOid );

    /**
     * Registers an internal UMAC event (triggered by an external event)
     *
     * @since S60 3.1
     * @param aInternalEvent Event to register
     */
    inline void Register( TInternalEvent aInternalEvent );

    /**
     * Enables the specified event dispatching channels
     *
     * @since S60 3.1
     * @param aChannelMask Mask of the channels to enable
     */
    inline void Enable( TChannel aChannelMask );

    /**
     * Disables the specified event dispatching channels
     *
     * @since S60 3.1
     * @param aChannelMask Mask of the channels to disable
     */
    inline void Disable( TChannel aChannelMask );

    /**
     * Determines if specified even dispatching channel(s) is (are) enabled
     *
     * @since S60 3.1
     * @param aChannel The channel(s) to check
     * @return ETrue if the channel(s) is (are) enabled
     *         EFalse otherwise
     */
    inline TBool ChannelEnabled( TChannel aChannel ) const;

    /**
     * Dispatches a single event, if there are any registered
     *
     * @since S60 3.1
     * @return ETrue If a global state change occurred because of the event
     *               dispatching
     *         EFalse otherwise
     */
    TBool Dispatch();

    /**
     * Dispatches a single command completion event
     *
     * @since S60 3.1
     * @return ETrue If a global state change occurred as a result of the event
     *               dispatching
     *         EFalse otherwise
     */
    TBool DispatchCommandCompletionEvent();

    /**
     * Determines if a command completion event for the specified 
     * command is registered
     *
     * @param aCommandId Id of the command to check
     * @return ETrue if command completion event is registered
     *         EFalse otherwise
     */
    inline TBool CommandCompletionRegistered( 
        WHA::TCompleteCommandId aCommandId ) const;
    
private:

    /**
     * Dispatches a single internal event
     *
     * @since S60 3.1
     * @return ETrue If a global state change occurred because of the event
     *               dispatching
     *         EFalse otherwise
     */
    TBool DispatchInternalEvent();
    
    /**
     * Dispatches a single WLAN Mgmt Client command event
     *
     * @since S60 3.1
     * @return ETrue If a global state change occurred because of the event
     *               dispatching
     *         EFalse otherwise
     */
    TBool DispatchOidEvent();

    // Prohibit copy constructor.
    WlanEventDispatcher( const WlanEventDispatcher& );
    // Prohibit assigment operator.
    WlanEventDispatcher& operator= ( const WlanEventDispatcher& );

private: // data

    MWlanEventDispatcherClient& iEventDispatcherClient;

    struct TCommandResp
        {
        WHA::TCompleteCommandId iId;
        WHA::TStatus iStatus;
        WHA::UCommandCompletionParams iParams;
        
        inline TCommandResp();            
        };

    /**
     * stores WHA command completion output parameters
     */
    TCommandResp                iWhaCommandCompletionParams;

    /**
     * True, if a command completion event has been registed.
     * At most one command completion event can exist at a time
     */
    TBool                       iCommandCompletionRegistered;

    /**
     * bit mask specifying the event dispatching channels which are currently
     * disabled (if any)
     */
    TChannel                    iDisableChannelMask;
    
    /**
     * pointer to the structure holding the WLAN Mgmt Client command 
     * parameters. NULL if a command event is not registered
     * Not own.
     */
    const TOIDHeader*           iOid;

    /**
     * bitmask for registered internal events
     */
    TInternalEvent              iInternalEvents;

    };

#include "umaceventdispatcher.inl"

#endif // WLANEVENTDISPATCHER_H
