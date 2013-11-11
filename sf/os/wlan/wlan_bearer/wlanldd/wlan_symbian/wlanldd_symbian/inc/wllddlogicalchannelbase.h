/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the DWlanLogicalChannelBase class.
*
*/

/*
* %version: 3 %
*/

#ifndef D_WLANLOGICALCHANNELBASE_H
#define D_WLANLOGICALCHANNELBASE_H

#include <kernel.h>

/**
* An abstract base class for a logical channel that provides a framework in 
* which user side client requests are executed in the context of a single 
* kernel side thread or in the context of the client thread in supervisor 
* mode
*
*  @since S60 v5.1
*/
class DWlanLogicalChannelBase : public DLogicalChannel
    {
    
public:
    
    /**
     * Base class constructor for DWlanLogicalChannelBase objects.
     * @pre Calling thread must be in a critical section.
     * @pre No fast mutex can be held.
     * @pre Kernel must be unlocked
     * @pre Interrupts enabled
     * @pre Call in a thread context
     */
    DWlanLogicalChannelBase();
    
    /**
     * Base class destructor for DWlanLogicalChannelBase objects.
     * @pre Calling thread must be in a critical section.
     * @pre No fast mutex can be held.
     * @pre Kernel must be unlocked
     * @pre Interrupts enabled
     * @pre Call in a thread context
     */
    virtual ~DWlanLogicalChannelBase();
    
    /**
     * Handles a client request in the client context.
     * It overrides DLogicalChannel::Request() to handle Fast Control
     * requests in the context of the client's thread and passes all other 
     * requests to the driver thread as kernel side messages.
     * @param  aReqNo  The number of the client request
     * @param  a1      Arbitrary argument
     * @param  a2      Arbitrary argument
     * @return Return value from the device driver call
     * @pre    Called in context of client thread.
     * @pre    Calling thread must not be in a critical section.
     * @pre    No fast mutex can be held.
     */
    virtual TInt Request( TInt aReqNo, TAny* a1, TAny* a2 );
  
    /**
     * Process a function for this logical channel.
     * This method is executed in the context of the user mode client 
     * thread, but in supervisor mode. Code executed in this mode 
     * MUST NOT take a lot of time.
     * @param aFunction Defines the operation/function to be performed.
     * @param param     Function specific parameter
     * @return function specific return value.
     */
    virtual TAny* DoControlFast( TInt aFunction, TAny* param ) = 0;

private: // Methods

    // Prohibit copy constructor.
    DWlanLogicalChannelBase ( const DWlanLogicalChannelBase& );
    // Prohibit assignment operator.
    DWlanLogicalChannelBase& operator= ( const DWlanLogicalChannelBase& );
    
private: // definitions
    
    enum 
        {
        /**
         * The smallest Fast Control Function ID that DoControlFast()
         * accepts.
         */
        EMinControlFast = static_cast<TInt>(0x0E000000),
        
        /**
         * The largest Fast Control Function ID that DoControlFast()
         * accepts.
         */
        EMaxControlFast = static_cast<TInt>(0x10000000)
        };
    };

#endif // D_WLANLOGICALCHANNELBASE_H
