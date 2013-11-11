/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
    @brief USB charging information header
    @version 0.2.0

    This header specifies the published information of USB charging plug-in.
    @publishedPartner
*/

#ifndef USBCHARGINGPUBLISHEDINFO_H
#define USBCHARGINGPUBLISHEDINFO_H

#include <e32base.h>


/** this is only applicable when charging port is a Standard Downstream Port 
*/
enum TUsbConnectionStatus
    {
    /** this is the initial value.  */
    EUsbConnectionStatusNone,
    
    /** This status means that this USB device is in suspend state. */
    EUsbConnectionStatusSuspend,
    
    /** This status means that this USB device is in configured state. */
    EUsbConnectionStatusConfigured
    };

enum TUsbChargingPortType
    {
    /** 
    * This device is no longer connected to a powered port.
    * iMinAvailableVbusCurrent, iMaxVbusCurrent and iMinVbusVoltage are 0. 
    * iUsbConnStatus is EUsbConnectionStatusNone.
    */
    EUsbChargingPortTypeNone = 0,
    
    /**
    * This device is connected to a port that can't be recognized.
    * iMinAvailableVbusCurrent, iMaxVbusCurrent and iMinVbusVoltage are 0. 
    * iUsbConnStatus is EUsbConnectionStatusNone.
    */
    EUsbChargingPortTypeUnsupported,
    
    /**
    * This device is connected to a charging port.
    * This is not an exact port type and the port may be a dedicated charging port or a charging downstream port. 
    * For there is more constraint on charging downstream port output, current and voltage are computed according 
    * to charging downstream port. 
    * See below for detail.
    */
    EUsbChargingPortTypeChargingPort,
    
    /**
    * This device is connected to a dedicated charging port.
    * iMinAvailableVbusCurrent is 500mA (refer to min IDCHG in table 5-2 of BC1.1).
    * iMaxVbusCurrent is 1800mA (refer to IDEV_DCHG in table 5-2 of BC1.1). 
    * iMinVbusVoltage is 2000mV ( refer to VCHG_SHT_DWN in table 5-1 of BC1.1). 
    * iUsbConnStatus is EUsbConnectionStatusNone. 
    */
    EUsbChargingPortTypeDedicatedChargingPort,
    
    /**
    * This device is connected to a charging downstream port.
    * - If this device works at low speed or full speed mode.
    *     - iMinAvailableVbusCurrent is 500mA (refer to min IDCHG in table 5-2 of BC1.1).
    *     - iMaxVbusCurrent is 1500mA (refer to IDEV_HCHG_LFS in table 5-2 of BC1.1). 
    *     - iMinVbusVoltage is 2000mV (refer to VCHG_SHT_DWN in table 5-1 of BC1.1). 
    *     - iUsbConnStatus is EUsbConnectionStatusNone. 
    *
    * - If this device works at high speed.
    *     - if USB hardware supports current limit when chirping.
    *         - iMinAvailableVbusCurrent is 500mA (refer to min IDCHG in table 5-2 of BC1.1).
    *         - iMaxVbusCurrent is 900mA (refer to IDEV_HCHG_HS in table 5-2 of BC1.1). 
    *         - iMinVbusVoltage is 2000mV (refer to VCHG_SHT_DWN in table 5-1 of BC1.1). 
    *         - iUsbConnStatus is EUsbConnectionStatusNone 
    *     - if USB hardware does not support current limit when chirping
    *         - iMinAvailableVbusCurrent is 500mA (refer to min IDCHG in table 5-2 of BC1.1).
    *         - iMaxVbusCurrent is 560mA (refer to IDEV_HCHG_CHRP in table 5-2 of BC1.1).
    *         - iMinVbusVoltage is 2000mV (refer to VCHG_SHT_DWN in table 5-1 of BC1.1). 
    *         - iUsbConnStatus is EUsbConnectionStatusNone. 	
    */
    EUsbChargingPortTypeChargingDownstreamPort,
    
    /** This device is connected to a standard downstream port.
    * This device negoiates the charging current with standard host by configuration descriptor. There are three numbers of 
    * milliamps that can be negoiated, 500mA, 100mA and 8mA.  
    * - if any of the three numbers is acceptted by host.
    *     - if this device is in configured state.
    *         - iMinAvailableVbusCurrent and iMaxVbusCurrent are 500mA or 100mA or 8mA. 
    *         - iUsbConnStatus is EUsbConnectionStatusConfigured.
    *     - if this device is in suspend state.
    *         - iMinAvailableVbusCurrent and iMaxVbusCurrent are 2mA (refer to ISUSP in table 5-2 of BC1.1).
    *         - iUsbConnStatus is EUsbConnectionStatusSuspend.	
    *     - iMinVbusVoltage is 4500mV in both cases.  4.5 V comes from the port supply requirement 4.75 V with the worst case
    *     voltage drops for 500 mA current taken into account. That is: 4.75 V - VGNDD - VBUSD = 4.75 V - 0.125 V - 0.125 V = 4.5 V. 
    *     See Section 7.2.2 Voltage Drop Budget in USB 2.0 spec.
    *    
    * - if none of the three numbers is acceptted by host.
    *     - iMinAvailableVbusCurrent, iMaxVbusCurrent and iMinVbusVoltage are 0 and iUsbConnStatus is EUsbConnectionStatusNone.
    */
    EUsbChargingPortTypeStandardDownstreamPort,
    
    /** This device is connected to an ACA and ID pin status is RidA. 
    * When ID pin status is RidA, this device works as A-device and is connected to an ACA. The ACA is connected with a charger 
    * or a charging downstream port on ACA's charging port and may be connected to a B-device on ACA's accessory port.  
    * The B-device can draw current from the charger. 
    * iMinAvailableVbusCurrent is not more than 500mA. This value comes from 500mA (min IDCHG refer to in table 5-2 of BC1.1) minus the current that B-device
    * can draw. This value is maxpower field of B-device configuration descriptor. If there is no connected B-device, minus 0 from 500mA. 
    * iMaxVbusCurrent is 1500mA (refer to max IDCHG in table 5-2 of BC1.1). For ACA can't distinguish a charger and a charging downstream
    * port so we select more contraint output.
    * iMinVbusVoltage is 2000mV (refer to VCHG_SHT_DWN in table 5-1 of BC1.1). 
    * iUsbConnStatus is EUsbConnectionStatusNone.
    */
    EUsbChargingPortTypeAcaRidA,
    
    /** This device is connected to an ACA and ID pin status is RidB
    * When ID pin status is RidB, this device works as a B-device and is connected to an ACA. The ACA is connected with a charger or a  
    * charging downstream port on ACA's charging port. ACA may also be connected to A-device on accessory port but A-device VBUS 
    * powers off. A-device shall not draw current from the accessory port. 
    * iMinAvailableVbusCurrent is 500mA (refer to min IDCHG in table 5-2 of BC1.1).
    * iMaxVbusCurrent is 1500mA (refer to max IDCHG in table 5-2 of BC1.1). 
    * iMinVbusVoltage is 2000mV (refer to VCHG_SHT_DWN in table 5-1 of BC1.1). 
    * iUsbConnStatus is EUsbConnectionStatusNone.
    */
    EUsbChargingPortTypeAcaRidB,
    
    /** This device is connected to an ACA and ID pin status is RidC
    * When ID pin status is RidC, this device work as a B-device and is connected to an ACA. The ACA is connected with a charger or  
    * a charging downstream port on ACA's charging port and a A-device on accessory port. The A-device VBus powers on. A-device
    * shall not draw current from accessory port. 
    * iMinAvailableVbusCurrent is 500mA (refer to min IDCHG in table 5-2 of BC1.1).
    * iMaxVbusCurrent is 1500mA (refer to max IDCHG in table 5-2 of BC1.1). 
    * iMinVbusVoltage is 2000mV (refer to VCHG_SHT_DWN in table 5-1 of BC1.1). 
    * iUsbConnStatus is EUsbConnectionStatusNone.
    */
    EUsbChargingPortTypeAcaRidC
    };

/**    
* the P&S key category of published charing information.
*/
const TUid KPropertyUidUsbBatteryChargingCategory = {0x101fe1db};

/**    
* the P&S key ID of published charing information.
*/
const TUint KPropertyUidUsbBatteryChargingInfo = 1;

/** 
* the structure of published charing information, data in P&S is a TPckgBuf wrappering this structure  
*/
NONSHARABLE_STRUCT(TPublishedUsbChargingInfo)
    {
    /** the port type this device connects to. */
    TUsbChargingPortType  iChargingPortType;

    /** this field is only appliable when this device connects to a standard downstream port. */
    TUsbConnectionStatus  iUsbConnStatus;

    /**
    *	This is the number of milliamps in unit 1 mA that can safely be drawn from VBUS 
    * without needing to observe the VBUS level.
    */	
    TUint16 iMinAvailableVbusCurrent;

    /**
    *	This is the number of milliamps that can be drawn from VBus in unit 1 mA. VBUS Level may drop down and must be monitored
    * to prevent VBUS Level drop below iMaxVbusCurrent.
    */
    TUint16 iMaxVbusCurrent;

    /** VBUS Level must be above this number when charging and this number is in unit 1 mV. */
    TUint16 iMinVbusVoltage;
    };


#endif // USBCHARGINGPUBLISHEDINFO_H
