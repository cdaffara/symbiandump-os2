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
* Description:   Declaration of the TWlanScanModeStopped class.
*
*/

/*
* %version: 5 %
*/

#ifndef T_WLANSCANMODESTOPPED_H
#define T_WLANSCANMODESTOPPED_H

#include "umacscanfsm.h"

/**
 *  Scan stopped state.
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class TWlanScanModeStopped : public TWlanScanFsm
    {

public:

    TWlanScanModeStopped() {};

private:

    // from base class TWlanScanFsm

    /**
     * From TWlanScanFsm.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
    virtual void Entry( WlanScanFsmCntx& aCnxt );

    /**
     * From TWlanScanFsm.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
    virtual void StartScanningMode( WlanScanFsmCntx& aCnxt );

    /**
     * From TWlanScanFsm.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
    virtual void StopScanningMode( WlanScanFsmCntx& aCnxt );

    // Prohibit copy constructor
    TWlanScanModeStopped( const TWlanScanModeStopped& );
    // Prohibit assigment operator
    TWlanScanModeStopped& operator= ( const TWlanScanModeStopped& );    
    };

#endif // T_WLANSCANMODESTOPPED_H
