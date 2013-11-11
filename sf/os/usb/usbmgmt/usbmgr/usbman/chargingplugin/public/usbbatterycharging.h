/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

/** @file
@internalComponent
*/

#ifndef USBBATTERYCHARGING_H
#define USBBATTERYCHARGING_H

#include <e32base.h>

// UID used for central respository
const TUid KUsbBatteryChargingCentralRepositoryUid = {0x10208DD7};	// UID3 for usbbatterychargingplugin

const TUint KUsbBatteryChargingKeyNumberOfCurrentValues = 1;

const TUint KUsbBatteryChargingCurrentValuesOffset = 0x1000;

enum TUsbIdPinState
    {
    EUsbBatteryChargingIdPinBRole = 0,
    EUsbBatteryChargingIdPinARole,
    };

#endif // USBBATTERYCHARGING_H
