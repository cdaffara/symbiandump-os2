/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CenRep keys
*
*/


#ifndef WLANDEVICESETTINGSPRIVATECRKEYS_H
#define WLANDEVICESETTINGSPRIVATECRKEYS_H

#include "wlandevicesettingsinternalcrkeys.h"
/**
* Base for user defined values.
* Keys defined below are for default values. If using user defined settings,
* this base value has to be added to the key.
* (e.g. KWlanBeacon => default value of beacon setting,
* KWlanBeacon+KWlanUserSettingsbase => user defined beacon value is read)
*/
const TUint32 KWlanUserSettingsbase =                    0x00000100;

/**
* KWlanDeviceSettingsVersion 
* Defines the settings version.
*/
const TUint32 KWlanDeviceSettingsVersion =               0x00000001;

#endif      // WLANDEVICESETTINGSPRIVATECRKEYS_H
     
//  End of File
