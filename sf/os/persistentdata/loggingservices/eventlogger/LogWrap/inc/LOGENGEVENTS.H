// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef LOGENGEVENTS_H
#define LOGENGEVENTS_H

#include <e32std.h>
#include <logfilterandeventconstants.hrh>
#include <logwrapconst.h>

/**
Location Requested by On Board Application Event Uid.
@publishedPartner
@released
*/
const TUid KLogLbsSelfLocateEventTypeUid = {KLogLbsSelfLocateEventType};

/**
Location Requested by Remote Party Event Uid.
@publishedPartner
@released
*/
const TUid KLogLbsExternalLocateEventTypeUid = {KLogLbsExternalLocateEventType};

/**
Location Sent to Remote Party Event Uid.
@publishedPartner
@released
*/
const TUid KLogLbsTransmitLocationEventTypeUid = {KLogLbsTransmitLocationEventType};

/**
Location Determined by Network Event Uid.
@publishedPartner
@released
*/
const TUid KLogLbsNetworkLocateEventTypeUid = {KLogLbsNetworkLocateEventType};

/**
Location Data Obtained from Network Event Uid.
@publishedPartner
@released
*/
const TUid KLogLbsAssistanceDataEventTypeUid = {KLogLbsAssistanceDataEventType};

/**
Uid for Packet data event type
@publishedPartner
@released
*/
const TUid KLogPacketDataEventTypeUid = {KLogPacketDataEventType};
/**
Event flag
@publishedPartner
@released
*/
const TLogFlags KLogEventContactSearched = 0x2;

#endif
