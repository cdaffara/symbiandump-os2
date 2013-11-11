/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Parser utility for 802.11 (re-)association response frames.
*
*/


#ifndef CORE_FRAME_ASSOC_RESP_H
#define CORE_FRAME_ASSOC_RESP_H

#include "am_platform_libraries.h"
#include "core_types.h"
#include "core_frame_dot11.h"

/**
 * Class for parsing 802.11 (re-)association response frames.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_frame_assoc_resp_c ) : public core_frame_dot11_c
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
    static core_frame_assoc_resp_c* instance(
        const core_frame_dot11_c& frame,
        bool_t is_copied = false_t );

    /**
     * Destructor.
     */
    virtual ~core_frame_assoc_resp_c();

    /**
     * Return the Capability field of the frame.
     * 
     * @since S60 v3.1
     * @return The Capability field of the frame.     
     */    
    u16_t capability() const;

    /**
     * Return the Status Code field of the frame.
     *
     * @since S60 v3.1
     * @return The Status Code field of the frame.
     */
    u16_t status_code() const;

    /**
     * Return the AID field of the frame.
     * 
     * @since S60 v3.1
     * @return The AID field of the frame.     
     */    
    u16_t aid() const;

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
    core_frame_assoc_resp_c(
        u16_t data_length,
        const u8_t* data,
        u16_t max_data_length );
    };

#endif // CORE_FRAME_ASSOC_RESP_H
