/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/

#ifndef WLANSDIOACLIENT_H
#define WLANSDIOACLIENT_H

#include <wlansdioa.h>

/**
 *  SDIOA client callback interface
 *
 *
 */
class MWlanSdioaClient
    {

public:

	/**
	 * Destructor
	 *
	 */
    virtual ~MWlanSdioaClient() {};

    /**
     * SDIOA request complete callback
     *
     * @param aRequestId SDIOA request ID
     * @param aStatus Status of the operation
     *          ESuccess    -   The request completed successfully
     *          EFailure    -   The request failed
     */
    virtual void OnRequestComplete( WlanSdioa::TRequestId aRequestId,
                                    SDIOA::TStatus aStatus ) = 0;
                                        
    /**
     * SDIO interrupt callback
     *
     * This method will be called after a device interrupt only if SDIO 
     * (DAT1 multiplexed) interrupts have been enabled with the 
     * WlanSdioa::EnableSDIOInterrupt() method.
     */
    virtual void OnSDIOInterrupt() = 0;                                        
    };


#endif // WLANSDIOACLIENT_H
