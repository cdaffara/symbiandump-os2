/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  spia client callback interface
*
*/

/*
* %version: 3 %
*/

#ifndef WLANSPIACLIENT_H
#define WLANSPIACLIENT_H

#include <wlanspia.h>

/**
 *  spia client callback interface
 *
 *
 *  @lib wlanpdd.pdd
 *  @since S60 v3.2
 */
class MWlanSpiaClient 
    {

public:

	/**
	 * Destructor.
	 *
	 * @since S60 v3.2
	 */
    virtual ~MWlanSpiaClient() {};

    /**
     * spia request complete callback, 
     * resources associated to the request can be freed in this method 
     *
     * NOTE: the callback client may call back in the same context
     *
     * @since S60 v3.2
     * @param aRequestId request ID
     * @param aStatus status of the operation
     *         ESuccess -       request completed successfully
     *         EFailure -       request failed due unspecified reason. 
     */
    virtual void OnRequestComplete( WlanSpia::TRequestId aRequestId, 
                                    SPIA::TStatus aStatus ) = 0;

    };


#endif // WLANSPIACLIENT_H
