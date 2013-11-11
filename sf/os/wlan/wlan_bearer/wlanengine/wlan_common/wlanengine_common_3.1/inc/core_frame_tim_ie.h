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
* Description:  Class for parsing 802.11 TIM element IEs.
*
*/


#ifndef CORE_FRAME_TIM_IE_H
#define CORE_FRAME_TIM_IE_H

#include "core_frame_dot11_ie.h"
#include "core_types.h"

/**
 * Class for parsing 802.11 TIM element IEs.
 *
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_frame_tim_ie_c ) : public core_frame_dot11_ie_c
    {

public:

    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v3.1
     * @param ie Pointer to the IE data.
     * @return A pointer to the created IE instance.
     */
    static core_frame_tim_ie_c* instance(
        const core_frame_dot11_ie_c& ie );

    /** 
     * Destructor.
     */
    virtual ~core_frame_tim_ie_c();

    /**
     * Return the DTIM Count field.
     *
     * @return The DTIM Count field.
     */
    u8_t dtim_count() const;

    /**
     * Return the DTIM Period field.
     *
     * @return The DTIM Period field.
     */
    u8_t dtim_period() const;

private:

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     */
    core_frame_tim_ie_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

    };

#endif // CORE_FRAME_TIM_IE_H
