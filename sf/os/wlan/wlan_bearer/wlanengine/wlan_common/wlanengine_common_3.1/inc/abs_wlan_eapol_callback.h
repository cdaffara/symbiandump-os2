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
* Description:  Callback interface from WLM server to eapol_handler
*
*/


#ifndef ABS_WLAN_EAPOL_CALLBACK_H
#define ABS_WLAN_EAPOL_CALLBACK_H

#include "am_platform_libraries.h"

/**
 * Abstract adaptation layer interface for eapol callbacks.
 *
 * @since S60 v3.2
 */
class abs_wlan_eapol_callback_c
    {
public:

    /**
     * Destructor.
     */
    virtual ~abs_wlan_eapol_callback_c() {};

    /**
     * Callback interface to partner.
     *
     * @since S60 v3.2
     * @param data Pointer to the data to be sent.
     * @param length Length of the data to be sent.
     * @return Return value is specified in interface specification.
     */
    virtual u32_t send_data(
        const void * const data, 
        const u32_t length ) = 0;

    };

#endif // ABS_WLAN_EAPOL_CALLBACK_H
