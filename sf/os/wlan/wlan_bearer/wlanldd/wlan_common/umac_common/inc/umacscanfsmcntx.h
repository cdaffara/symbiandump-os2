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
* Description:   Declaration of the WlanScanFsmCntx class.
*
*/

/*
* %version: 6 %
*/

#ifndef C_WLANSCANFSMCNTX_H
#define C_WLANSCANFSMCNTX_H

#include "umacscanmoderunning.h"
#include "umacscanmodestopped.h"
#include "umacstartscanningmode.h"
#include "umacstopscanningmode.h"

class TWlanScanFsm;
class MWlanScanFsmCb;

/**
 *  Scan statemachine context.
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class WlanScanFsmCntx 
    {

    friend class TWlanScanFsm;
    friend class TWlanScanModeRunning;
    friend class TWlanScanModeStopped;
    friend class TWlanStartScanningMode;
    friend class TWlanStopScanningMode;

public:
    
    
    inline explicit WlanScanFsmCntx( MWlanScanFsmCb& aCb );

    /**
     * ?description
     *
     * @since S60 3.1
     * @return ?description
     */
    inline TWlanScanFsm& Current();

private:

    /**
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    inline void Current( TWlanScanFsm& aNewCurrent );

    /**
     * ?description
     *
     * @since S60 3.1
     * @return ?description
     */
    inline MWlanScanFsmCb& Cb();

    /**
     * ?description
     *
     * @since S60 3.1
     * @return ?description
     */
    inline TWlanScanFsm& ScanModeRunning();

    /**
     * ?description
     *
     * @since S60 3.1
     * @return ?description
     */
    inline TWlanScanFsm& ScanModeStopped();

    /**
     * ?description
     *
     * @since S60 3.1
     * @return ?description
     */
    inline TWlanScanFsm& StartScanningMode();

    /**
     * ?description
     *
     * @since S60 3.1
     * @return ?description
     */
    inline TWlanScanFsm& StopScanningMode();

    // Prohibit copy constructor
    WlanScanFsmCntx( const WlanScanFsmCntx& );
    // Prohibit assigment operator
    WlanScanFsmCntx& operator= ( const WlanScanFsmCntx& );    

private: // data

    /**
     * ?description_of_member
     */
    MWlanScanFsmCb&         iCb;

    /**
     * ?description_of_member
     */
    TWlanScanModeRunning    iWlanScanModeRunning;

    /**
     * ?description_of_member
     */
    TWlanScanModeStopped    iWlanScanModeStopped;

    /**
     * ?description_of_member
     */
    TWlanStartScanningMode  iWlanStartScanningMode;

    /**
     * ?description_of_member
     */
    TWlanStopScanningMode   iWlanStopScanningMode;  

    /**
     * ?description_of_pointer_member
     * Own.  
     */
    TWlanScanFsm*           iCurrentState;

    };

#include "umacscanfsmcntx.inl"

#endif // C_WLANSCANFSMCNTX_H
