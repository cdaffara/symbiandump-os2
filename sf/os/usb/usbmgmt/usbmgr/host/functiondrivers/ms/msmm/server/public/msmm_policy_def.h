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
 @publishedPartner
*/

#ifndef MSMM_POLICY_DEF_H
#define MSMM_POLICY_DEF_H

/**
TPolicyRequestData is used to identify a particular logical unit 
to the Policy Plugin. Related device and interface information included.
*/
NONSHARABLE_CLASS(TPolicyRequestData)
    {
public:
    TUint16 iBcdDevice; // Device Release Number
    TUint8  iConfigurationNumber; // Number of Possible Configurations
    TUint   iDeviceId; // Device identifier
    TUint8  iInterfaceNumber; // Interface Number
    TUint8  iLogicUnitNumber; // Logical Unit Number
    TUint32 iOtgInformation; // Reserved for Otg Host information
    TUint16 iProductId; // Product ID (Assigned by Manufacturer)
    TUint16 iVendorId; // Vendor ID (Assigned by USB Org)
    TName   iSerialNumber; // Serial Number String Descriptor
    TName   iManufacturerString; // Manufacturer String Descriptor
    TName   iProductString; // Product String Descriptor
    };

/**
TPolicyMountRecord is used to record a particular mounting operation
*/
NONSHARABLE_CLASS(TPolicyMountRecord)
    {
public:
    TText8  iDriveName; // Drive letter
    TPolicyRequestData  iLogicUnit; // Mounted logical unit
    };

NONSHARABLE_CLASS(TSuspensionPolicy)
    {
public:
    /** Time internval to check media status and finalisation */
    TUint8  iStatusPollingInterval;
    /** Time interval to delay suspending the interface after finalisation */
    TUint8  iOtgSuspendTime; 
    };

#endif /*MSMM_POLICY_DEF_H*/
