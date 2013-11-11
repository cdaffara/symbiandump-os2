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
* Description:  Class for manipulating scan channels masks.
*
*/


#ifndef CORE_SCAN_CHANNELS_H
#define CORE_SCAN_CHANNELS_H

#include "core_types.h"

/**
 * This class implements a storage for channel masks.
 *
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( core_scan_channels_c )
    {

public:

    /**
     * Constructor.
     */
    core_scan_channels_c();

    /**
     * Constructor.
     */
    core_scan_channels_c(
        const core_scan_channels_s& channels );

    /**
     * Constructor.
     */
    core_scan_channels_c(
        const core_scan_channels_c& channels );

    /**
     * Destructor.
     */
    virtual ~core_scan_channels_c();

    /**
     * Return the current channel mask as a struct.
     *
     * @since S60 v3.2
     * @return The current channel mask as a struct.
     */
    const core_scan_channels_s& channels();

    /**
     * Set the current channel mask to the given mask.
     *
     * @since S60 v3.2
     * @param channels The channel mask to set.     
     */
    void set(
        const core_scan_channels_s& channels );

    /**
     * Set the current channel mask to the given mask.
     *
     * @since S60 v3.2
     * @param channels The channel mask to set.     
     */
    void set(
        const core_scan_channels_c& channels );

    /**
     * Merge the current channel mask with the given mask.
     *
     * @since S60 v3.2
     * @param channels The channel mask to merge.
     */
    void merge(
        const core_scan_channels_c& channels );

    /**
     * Add the given channel to the channel mask.     
     *
     * @since S60 v3.2
     * @param band Band of the scan channel.
     * @param channel Channel to add.     
     */
    void add(
        u8_t band,
        u8_t channel );

    /**
     * Inverts the current channel mask.
     */
    void invert_channels();
    
    /**
     * Remove the given channel from the channel mask.
     * 
     * @since S60 v5.0
     * @param band Band of the scan channel.
     * @param channel Channel to remove.
     */
    void remove(
    	u8_t band,
    	u8_t channel );

    /**
     * Check whether the channel mask on the given band is empty.
     *
     * @since S60 v3.2
     * @param band Band of the scan channel.
     * @return true_t if the mask is empty, false_t otherwise.
     */
    bool_t is_empty(
        u8_t band );

private: // data

    /**
     * The current channels for 2.4GHz range.
     */
    u16_t channels2dot4ghz_m;

    /**
     * The current channel mask.
     */
    core_scan_channels_s channel_mask_m;

    };

#endif // CORE_SCAN_CHANNELS_H
