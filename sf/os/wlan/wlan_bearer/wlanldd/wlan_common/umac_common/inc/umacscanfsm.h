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
* Description:   Declaration of the TWlanScanFsm class.
*
*/

/*
* %version: 6 %
*/

#ifndef T_WLANSCANFSM_H
#define T_WLANSCANFSM_H

class WlanScanFsmCntx;

/**
 *  Scan state machine.
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class TWlanScanFsm 
    {

public:

    /**
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    virtual void StartScanningMode( WlanScanFsmCntx& aCnxt );

    /**
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    virtual void StopScanningMode( WlanScanFsmCntx& aCnxt );

    /**
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    virtual void OnScanningModeStarted( WlanScanFsmCntx& aCnxt );

    /**
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    virtual void OnScanningModeStopped( WlanScanFsmCntx& aCnxt );

protected:

    TWlanScanFsm() {};

    /**
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    void ChangeState( WlanScanFsmCntx& aCnxt, TWlanScanFsm& aNewState ) const;

    /**
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    virtual void Entry( WlanScanFsmCntx& aCnxt ) = 0;

private:

    };

#endif // T_WLANSCANFSM_H
