/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#ifndef MSMM_PUB_DEF_H
#define MSMM_PUB_DEF_H

/**
USB Mass Storage Device description structure
Used to identifier a particular USB MS device when the MSFDC adds a 
function or removes a device.
*/
NONSHARABLE_CLASS(TUSBMSDeviceDescription)
    {
public:
    /** Device Release Number */
    TUint16 iBcdDevice;
    /** Number of Possible Configurations */
    TUint8  iConfigurationNumber;
    /** Device identifier */
    TUint   iDeviceId;
    /** Reservered for Otg Host information */
    TUint32 iOtgInformation;
    /** Product ID (Assigned by Manufacturer) */
    TUint16 iProductId;
    /** Vendor ID (Assigned by USB Org) */
    TUint16 iVendorId;
    /** Serial Number String Descriptor */
    TName   iSerialNumber;
    /** Manufacturer String Descriptor */
    TName   iManufacturerString;
    /** Product String Descriptor */
    TName   iProductString;    
    /** Protocol to be used by the MSC */
    TUint8  iProtocolId;
    /** Transport to be used by the MSC */
    TUint8  iTransportId;
    /** Device's capabilitiy for RemoteWakeup */
    TUint8  iRemoteWakeup;
    /** OTG capability of the device */
    TUint8  iIsOtgClient; 
    };

#endif /*MSMM_PUB_DEF_H*/
