/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains OID msg storage definition
*
*/

/*
* %version: 17 %
*/

#ifndef WLANOIDMSGSTORAGE_H
#define WLANOIDMSGSTORAGE_H

#include "umacoidmsg.h"

union WalOidMessages
    {
    TConnectMsg                     iConnectMsg;
    TStartIBSSMsg                   iStartIBSSMsg;
    TScanMsg                        iScanMsg;
    TStopScanMsg                    iStopscanMsg;
    TDisconnectMsg                  iDisconnectMsg;
    TSetPowerModeMsg                iSetPowerModeMsg;
    TSetRcpiTriggerLevelMsg         iSetRcpiTriggerLevelMsg;
    TSetTxPowerLevelMsg             iSetTxPowerLevelMsg;
    TConfigureMsg                   iConfigureMsg;
    TGetLastRcpiMsg                 iGetLastRcpiMsg;
    TDisableUserDataMsg             iDisableUserDataMsg;
    TEnableUserDataMsg              iEnableUserDataMsg;
    TAddCipherKeyMsg                iAddCipherKeyMsg;
    TAddMulticastAddrMsg            iAddMulticastAddrMsg;
    TRemoveMulticastAddrMsg         iRemoveMulticastAddrMsg;
    TConfigureBssLostMsg            iConfigureBssLostMsg;
    TSetTxRateAdaptationParamsMsg   iSetTxRateAdaptationParamsMsg;
    TConfigureTxRatePoliciesMsg     iConfigureTxRatePoliciesMsg;
    TSetPowerModeMgmtParamsMsg      iSetPowerModeMgmtParamsMsg;
    TConfigurePwrModeMgmtTrafficOverrideMsg iConfigurePwrModeMgmtTrafficOverrideMsg;
    TGetFrameStatisticsMsg          iGetFrameStatisticsMsg;
    TConfigureUapsdMsg              iConfigureUapsdMsg;
    TConfigureTxQueueMsg            iConfigureTxQueueMsg;
    TGetMacAddressMsg               iGetMacAddressMsg;
    TConfigureArpIpAddressFilteringMsg iConfigureArpIpAddressFilteringMsg;
    TConfigureHtBlockAckMsg         iConfigureHtBlockAckMsg;
    TConfigureProprietarySnapHdrMsg iConfigureProprietarySnapHdrMsg;
    };

struct SOidMsgStorage
    {
    WalOidMessages  iOidMsgs;
    };

#endif      // WLANOIDMSGSTORAGE_H
