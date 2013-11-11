/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the MWlanEventDispatcherClient class.
*
*/

/*
* %version: 21 %
*/

#ifndef M_WLANEVENTDISPATCHERCLIENT_H
#define M_WLANEVENTDISPATCHERCLIENT_H

#ifndef RD_WLAN_DDK
#include <wha.h>
#else
#include <wlanwha.h>
#endif

class WlanContextImpl;

/**
 *  Event dispatcher callback interface
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class MWlanEventDispatcherClient
    {

public:

    /**    
     *
     * @since S60 3.1
     * @param aCtxImpl the one and only global state machine context
     * @param 
     * @param 
     * @param 
     * @return
     */
    
    virtual TBool HandleConnect(const TAny *aInputBuffer) = 0;
    
    virtual TBool HandleStartIBSS(const TAny *aInputBuffer) = 0;

    virtual TBool HandleDisconnect() = 0;

    virtual TBool HandleSetPowerMode(const TAny *aInputBuffer) = 0;

    virtual TBool HandleSetRcpiTriggerLevel(const TAny *aInputBuffer) = 0;

    virtual TBool HandleSetTxPowerLevel(const TAny *aInputBuffer) = 0;

    virtual TBool HandleConfigure(const TAny *aInputBuffer) = 0;

    virtual TBool HandleScan(const TAny *aInputBuffer ) = 0;

    virtual TBool HandleStopScan() = 0;

    virtual TBool HandleGetLastRcpi() = 0;     

    virtual TBool HandleDisableUserData() = 0;   

    virtual TBool HandleEnableUserData() = 0;   

    virtual TBool HandleAddCipherKey(const TAny *aInputBuffer) = 0;
    
    virtual TBool HandleAddMulticastAddr(const TAny *aInputBuffer) = 0;

    virtual TBool HandleRemoveMulticastAddr(const TAny *aInputBuffer) = 0;    

    virtual TBool HandleConfigureBssLost(const TAny *aInputBuffer) = 0;

    virtual TBool HandleSetTxRateAdaptParams(const TAny *aInputBuffer) = 0;

    virtual TBool HandleConfigureTxRatePolicies(const TAny *aInputBuffer) = 0;    

    virtual TBool HandleSetPowerModeMgmtParams(const TAny *aInputBuffer) = 0;

    virtual TBool HandleConfigurePwrModeMgmtTrafficOverride( 
        const TAny *aInputBuffer ) = 0;
        
    virtual TBool HandleGetFrameStatistics() = 0;
    
    virtual TBool HandleConfigureUapsd( const TAny *aInputBuffer ) = 0;
    
    virtual TBool HandleConfigureTxQueue( const TAny *aInputBuffer ) = 0;

    virtual TBool HandleGetMacAddress() = 0;

    virtual TBool HandleConfigureArpIpAddressFiltering( 
        const TAny *aInputBuffer ) = 0;

    virtual TBool HandleConfigureHtBlockAck( const TAny *aInputBuffer ) = 0;

    virtual TBool HandleConfigureProprietarySnapHdr( 
        const TAny *aInputBuffer ) = 0;
    
    virtual TBool OnWhaCommandComplete( 
        WHA::TCompleteCommandId aCompleteCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandCompletionParams& aCommandCompletionParams ) = 0;

    virtual TBool OnInternalEvent( TInternalEvent aInternalEvent ) = 0;
    };

#endif // M_WLANEVENTDISPATCHERCLIENT_H
