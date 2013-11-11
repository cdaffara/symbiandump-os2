/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines attributes for provisioning structure passed to CFP
*
*/

/*
* %version: 5 %
*/

#include "wlanmcpr.h"
#include "WlanProvision.h"

using namespace ESock;

//
// Attribute tables for provisioning structures passed to CFProtocol
//

START_ATTRIBUTE_TABLE(TLanIp4Provision, TLanIp4Provision::EUid, TLanIp4Provision::ETypeId)
// No attributes, even though there are data members, because the structure is
// not serialised - just passed to data side.
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE(TLanIp6Provision, TLanIp6Provision::EUid, TLanIp6Provision::ETypeId)
// No attributes, even though there are data members, because the structure is
// not serialised - just passed to data side.
END_ATTRIBUTE_TABLE()
