/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Custom CommsDat id's for WLANServiceTable
*
*/

/*
* %version: 7 %
*/

#ifndef WDBIFDEFS_H
#define WDBIFDEFS_H

//  CONSTANTS

#define WLAN_SECONDARY_SSID                     _S("SecondarySSID")

#define WLAN_SEC_SSID_SERVICE_ID                _S("ServiceId")
#define WLAN_SEC_SSID_NAME                      _S("SecSSIDName")
#define WLAN_SEC_SSID_SCANNED_SSID              _S("ScannedSSID")
#define WLAN_SEC_SSID_USED_SSID                 _S("UsedSSID")
#define WLAN_SEC_SSID_ID                        _S("SecSSIDID")
#define KWlanUserSettings                       1
#define WLAN_DEVICE_SETTINGS                    _S("WLANDeviceTable")
#define WLAN_DEVICE_SETTINGS_TYPE               _S("WlanDeviceSettingsType")
#define WLAN_BG_SCAN_INTERVAL                   _S("WlanBgScanInterval")

const TMDBElementId KCDTIdWlanServiceId  = 0x00030000;          //EUint32
const TMDBElementId KCDTIdWlanConnMode  = 0x00040000;           //EUint32
    
const TMDBElementId KCDTIdWlanSSID          = 0x00050000;       //EText
const TMDBElementId KCDTIdWlanWepKey1       = 0x00060000;       //EDesC8
const TMDBElementId KCDTIdWlanWepKey2       = 0x00070000;       //EDesC8
const TMDBElementId KCDTIdWlanWepKey3       = 0x00080000;       //EDesC8
const TMDBElementId KCDTIdWlanWepKey4       = 0x00090000;       //EDesC8
const TMDBElementId KCDTIdWlanWepIndex      = 0x000A0000;       //EUint32
const TMDBElementId KCDTIdWlanSecMode       = 0x000B0000;       //EUint32
const TMDBElementId KCDTIdWlanAuthMode      = 0x000C0000;       //EUint32
const TMDBElementId KCDTIdWlanEnableWpaPsk  = 0x000D0000;       //EUint32
const TMDBElementId KCDTIdWlanWpaPreSharedKey  = 0x000E0000;    //EDesC8
const TMDBElementId KCDTIdWlanFormatKey1    = 0x00140000;       //EUint32

const TMDBElementId KCDTIdWlanWpaKeyLength  = 0x000F0000;       //EUint32
const TMDBElementId KCDTIdWlanEaps          = 0x00100000;       //ELongText

const TMDBElementId KCDTIdWlanScanSSID         = 0x00110000;    //EBool
const TMDBElementId KCDTIdWlanChannelID        = 0x00120000;    //EUint32
const TMDBElementId KCDTIdWlanUsedSSID         = 0x00130000;    //EText

const TMDBElementId KCDTIdWlanEapsExtEnabled           = 0x00190000;       //EDesC8
const TMDBElementId KCDTIdWlanEapsExtDisabled          = 0x001A0000;       //EDesC8

#endif  // WDBIFDEFS_H

// end of file
