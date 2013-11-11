// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SAPRIVATE_H__
#define __SAPRIVATE_H__

/**
@internalComponent
*/
_LIT(KSystemAgentServerName, "!SystemAgent2");
_LIT(KSystemAgentServerImageName, "SysAgt2Svr");

const TUint KSysAgentServMajorVersionNumber=1;
const TUint KSysAgentServMinorVersionNumber=0;
const TUint KSysAgentServBuildVersionNumber=0;

/** 
This is a duplicate of KPosLastKnownLocationCategory from lbs's epos_lastknownlocationpsykeys.h
It should NOT be used directly.
Defines value UID of Location P&S keys category.
The publish and subscribe key is defined by SysAgent2 server.
@internalTechnology
 */
const TUid KSAPosLastKnownLocationCategory = { 0x10009BFA };

/**
This is a duplicate of KPosLastKnownLocation from lbs's epos_lastknownlocationpsykeys.h
ID of the Last Known Location key.
It should NOT be used directly.
Contains binary values, a buffer with packaged TPositionInfo.

If it contains a buffer, which length does not match TPositionInfo,
such value shall be ignored.

Client must have ReadDeviceData capability in order to read this key.
@internalTechnology
*/
const TInt KSAPosLastKnownLocation = 0x00000001;

/** 
This is a duplicate of KPosIndicatorCategoryUid from lbs's locationfwdomainpskeys.h
It should NOT be used directly.
Defines UID of Positioning Indicator P&S keys category. 
@internalTechnology
*/
const TUid KSAPosIndicatorCategory = { 0x101F7A79 };

/**
This is a duplicate of KPosIntGpsHwStatus from lbs's locationfwdomainpskeys.h
ID of the Integrated GPS HW Status key.
It should NOT be used directly.
May contain values defined by @ref TPosIntGpsHwIndicatorState.

Default value (i.e. when this key is not found) is EPosIntGpsHwIndicatorOff.

If it contains any other value than defined in @ref TPosIntGpsHwState,
such value shall be ignored and TPosIndicatorState::EPosIntGpsHwIndicatorOff
shall be assumed.
@internalTechnology
*/
const TInt KSAPosIntGpsHwStatus = 0x00000001;

#endif //__SAPRIVATE_H__
