/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for parsing and generating echo test frames.
*
*/


#ifndef CORE_FRAME_ECHO_TEST_H
#define CORE_FRAME_ECHO_TEST_H

#include "am_platform_libraries.h"
#include "core_types.h"
#include "core_frame_ethernet.h"

/**
 * Class for parsing and generating echo test frames.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_frame_echo_test_c ) : public core_frame_ethernet_c
    {

public:

    /**
     * Factory for creating a parser for the given frame.
     *
     * @since S60 v3.2
     * @param frame Frame to be parsed.
     * @return Pointer to the created parser instance.
     */
    static core_frame_echo_test_c* instance(
        const core_frame_ethernet_c& frame );

    /**
     * Factory for generating an echo test frame.
     *
     * @since S60 v3.2
     * @param destination The destination MAC field of the frame.
     * @param source The source MAC field of the frame.
     * @param token Token field of the frame.
     * @return Pointer to the created frame instance.
     */
    static core_frame_echo_test_c* instance(
        const core_mac_address_s& destination,
        const core_mac_address_s& source,
        u16_t token );

    /**
     * Destructor.
     */
    virtual ~core_frame_echo_test_c();

    /**
     * Return the Token field.
     */
    u16_t token() const;

protected:

    /**
     * Generate an echo test frame.
     *
     * @param destination The destination MAC field of the frame.
     * @param source The source MAC field of the frame.
     * @param bssid The BSSID field of the frame.
     * @param token Token field of the frame.
     */
    void generate(
        const core_mac_address_s& destination,
        const core_mac_address_s& source,
        u16_t token );

    /**
     * Constructor.
     *
     * @param data_length Length of the frame.
     * @param data Pointer to the frame data.
     * @param max_data_length The maximum size of the frame buffer.    
     */
    core_frame_echo_test_c(
        u16_t data_length,
        const u8_t* data,
        u16_t max_data_length );

    };

#endif // CORE_FRAME_ECHO_TEST_H
