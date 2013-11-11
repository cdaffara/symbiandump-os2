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
* Description:   Declaration of the TWlanStopScanningMode class.
*
*/

/*
* %version: 5 %
*/

#ifndef T_WLANSTOPSCANNINGMODE_H
#define T_WLANSTOPSCANNINGMODE_H

#include "umacscanfsm.h"

/**
 *  Stop scanning state.
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class TWlanStopScanningMode : public TWlanScanFsm 
    {

public:

    TWlanStopScanningMode () {};

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
     */
    virtual void OnScanningModeStopped( WlanScanFsmCntx& aCnxt );

    // Prohibit copy constructor
    TWlanStopScanningMode( const TWlanStopScanningMode& );
    // Prohibit assigment operator
    TWlanStopScanningMode& operator= ( const TWlanStopScanningMode& );    
    };

#endif // T_WLANSTOPSCANNINGMODE_H
