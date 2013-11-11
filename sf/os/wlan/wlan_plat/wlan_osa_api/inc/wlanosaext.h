/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  osaext interface declaration
*
*/

/*
* %version: 4 %
*/

#ifndef WLANOSAEXT_H
#define WLANOSAEXT_H

#include <wlanosaplatform.h>

/**
 *  wlanosa extension interface declaration
 *
 *
 *  @lib wlanosa.lib
 *  @since S60 v3.2
 */
class MWlanOsaExt 
    {

public:

	/**
	 * Destructor.
	 *
	 * @since S60 v3.2
	 */
    virtual ~MWlanOsaExt() {};
    
    /**
     * Acquires the mutual exclusion lock of the WLAN subsystem
     *
     * @since S60 v3.2
     */
    virtual void MutexAcquire() = 0;

    /**
     * Releases the mutual exclusion lock of the WLAN subsystem
     *
     * @since S60 v3.2
     */
    virtual void MutexRelease() = 0;

    };


#endif // WLANOSAEXT_H
