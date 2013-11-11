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
* Description:  Class for parsing WMM QoS IEs.
*
*/


#ifndef CORE_FRAME_WMM_IE_H
#define CORE_FRAME_WMM_IE_H

#include "core_frame_dot11_ie.h"
#include "core_types.h"

/**
 * Class for parsing WMM Parameter Element IE.
 *
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( core_frame_wmm_ie_parameter_c ) : public core_frame_dot11_ie_c
    {

public:

    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v3.1
     * @param ie Pointer to the IE data.
     * @return A pointer to the created IE instance.
     */
    static core_frame_wmm_ie_parameter_c* instance(
        const core_frame_dot11_ie_c& ie );

    /** 
     * Destructor.
     */
    virtual ~core_frame_wmm_ie_parameter_c();

    /**
     * Return the Version field.
     * @return The Version field.
     */
    u8_t version() const;

    /**
     * Return whether U-APSD is supported.
     * @return Whether U-APSD is supported.
     */
    bool_t is_uapsd_supported() const;

    /**
     * Return whether Admission Control is mandatory for the given access class.
     * @return Whether Admission Control is mandatory for the given access class.
     */
    bool_t is_admission_control_required(
        core_access_class_e access_class ) const;

private:

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     */
    core_frame_wmm_ie_parameter_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

    };

#endif // CORE_FRAME_WMM_IE_H
