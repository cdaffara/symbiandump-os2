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
* Description:  Parser utility for 802.11 beacon/probe response frames.
*
*/


#ifndef CORE_FRAME_BEACON_H
#define CORE_FRAME_BEACON_H

#include "am_platform_libraries.h"
#include "core_types.h"
#include "core_frame_dot11.h"

/**
 * Class for 802.11 beacon/probe response frames.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_frame_beacon_c ) : public core_frame_dot11_c
    {

public:

    /**
     * Factory for creating a parser for the given frame.
     *
     * @since S60 v3.1
     * @param frame Frame to be parsed.
     * @param is_copied Whether the object should take a copy of the frame data.    
     * @return Pointer to the created parser instance.
     */
    static core_frame_beacon_c* instance(
        const core_frame_dot11_c& frame,
        bool_t is_copied = false_t );
   
    /**
     * Destructor.
     */
    virtual ~core_frame_beacon_c();

    /**
     * Return the timestamp field of the frame.
     * 
     * @since S60 v3.1
     * @return The timestamp field of the frame.
     */
    core_tsf_value_s timestamp() const;

    /**
     * Return the beacon interval field of the frame.
     * 
     * @since S60 v3.1
     * @return The beacon interval field of the frame.
     */
    u16_t beacon_interval() const;
    
    /**
     * Return the capability field of the frame.
     * 
     * @since S60 v3.1
     * @return The capability field of the frame.     
     */    
    u16_t capability() const;

private:

    /**
     * Return the offset of the first IE.
     * @return The offset of the first IE. 0 if the frame doesn't support IEs.
     */
    u16_t first_ie_offset() const;

    /**
     * Constructor.
     *
     * @param data_length Length of the frame.
     * @param data Pointer to the frame data.
     * @param max_data_length The maximum size of the frame buffer.    
     */
    core_frame_beacon_c(
        u16_t data_length,
        const u8_t* data,
        u16_t max_data_length );
    };

#endif // CORE_FRAME_BEACON_H
