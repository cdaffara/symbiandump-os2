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
* Description:   Declaration of the TWlanScanModeRunning class.
*
*/

/*
* %version: 5 %
*/

#ifndef T_WLANSCANMODERUNNING_H
#define T_WLANSCANMODERUNNING_H

#include "umacscanfsm.h"

/**
 *  Scan executing state.
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class TWlanScanModeRunning : public TWlanScanFsm
    {

public:

    TWlanScanModeRunning() {};

    /**
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */

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
     * @param ?arg2 ?description
     * @return ?description
     */
    virtual void StopScanningMode( WlanScanFsmCntx& aCnxt );

    /**
     * From TWlanScanFsm.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    virtual void OnScanningModeStopped( WlanScanFsmCntx& aCnxt );

    // Prohibit copy constructor
    TWlanScanModeRunning( const TWlanScanModeRunning& );
    // Prohibit assigment operator
    TWlanScanModeRunning& operator= ( const TWlanScanModeRunning& );    

    };


#endif // T_WLANSCANMODERUNNING_H
