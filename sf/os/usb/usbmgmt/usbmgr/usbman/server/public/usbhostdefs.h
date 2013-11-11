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
* Shared HOST definitions
*
*/

/**
 @file
 @publishedPartner
*/

#ifndef __USBHOSTDEFS_H__
#define __USBHOSTDEFS_H__

#include <e32std.h>

/** An event in the attachment-detachment lifecycle of a USB device. */
enum TUsbHostStackEventType
	{
	/** A device attachment has occurred. */
	EDeviceAttachment	= 0,
	
	/** An attempt to load Function Driver(s) has occurred. */
	EDriverLoad			= 1,
	
	/** A device detachment has occurred. */
	EDeviceDetachment	= 2,
	};

/** For a 'driver loading' event, whether the attempt to load drivers was 
successful, partially successful, or unsuccessful. */
const TUint KMultipleDriversFound = 0x00000004;

enum TDriverLoadStatus
	{
	/** Driver loading was successful for all the device's interfaces. */
	EDriverLoadSuccess			= 0,
	
	/** Driver loading was successful for one or more of the device's 
	interfaces, but not for all of them. */
	EDriverLoadPartialSuccess	= 1,
	
	/** Driver loading was successful for none of the device's interfaces. */
	EDriverLoadFailure			= 2,
	
	/** Driver loading was successful for all the device's interfaces, with multiple drivers found */
	EDriverLoadSuccessMultipleDriversFound			= EDriverLoadSuccess|KMultipleDriversFound,
	
	/** Driver loading was successful for one or more of the device's 
	interfaces, but not for all of them, with multiple drivers found. */
	EDriverLoadPartialSuccessMultipleDriversFound	= EDriverLoadPartialSuccess|KMultipleDriversFound,
	
	/** Driver loading was successful for none of the device's interfaces, with multiple drivers found. */
	EDriverLoadFailureMultipleDriversFound           = EDriverLoadFailure|KMultipleDriversFound
	};

NONSHARABLE_CLASS(TDeviceEventInformation)
	{
public:
	inline TDeviceEventInformation()
		// These initialisations are arbitrary
		:	iDeviceId(0),
			iEventType(EDeviceAttachment),
			iError(KErrNone),
			iDriverLoadStatus(EDriverLoadSuccess),
			iVid(0),
			iPid(0)
		{}

	// Always relevant- the ID of the device the event relates to.
	TUint iDeviceId;

	// Always relevant- the type of event that has occurred.
	TUsbHostStackEventType iEventType;

	// Relevant to attachment and driver load events.
	TInt iError;

	// Relevant to driver load events only.
	TDriverLoadStatus iDriverLoadStatus;

	// Relevant to attachments with iError KErrNone.
	TUint16 iVid;

	// Relevant to attachments with iError KErrNone.
	TUint16 iPid;
	};
	
const TInt KUsbMaxSupportedLanguageIds = 127;	// Max number of supported USB Language Ids


enum TUsbOtgAttributes
	{	
	/** Device supported SRP protocol */
	EUsbOtgSRPSupported = 0x01,
	
	/** Device supported HNP protocol */
	EUsbOtgHNPSupported = 0x02,	
	
	EUsbOtgAttributeCount,
	};

NONSHARABLE_CLASS(TOtgDescriptor)
	{
public:
	inline TOtgDescriptor()
		:	iDeviceId(0),
		    iAttributes(0),
		    iReserved1(0),
		    iReserved2(0),
		    iReserved3(0)
		{}

	// The ID of the device the OTG descriptor relates to.
	TUint iDeviceId;
    
	// The OTG attributes - currently only D0 and D1 are used for SRP and HNP.
	TUint8 iAttributes;
	
	// Reserved
	TInt iReserved1;
	
	// Reserved
	TInt iReserved2;

	// Reserved
	TInt iReserved3;
	};


NONSHARABLE_CLASS(TConfigurationDescriptor)
	{
public:
	inline TConfigurationDescriptor()
		:	iDeviceId(0),
		    iNumInterfaces(0),
		    iConfigration(0),
		    iAttributes(0),
		    iMaxPower(0),
		    iReserved(0)
		{}	

	// The ID of the device the configration descriptor relates to.
	TUint iDeviceId;
    
	// the number of interfaces
	TUint8 iNumInterfaces;
	
	// the configration value
	TUint8 iConfigration;

	// Attributes field of configration
	TUint8 iAttributes;

	// MaxPower field of configration
	TUint8 iMaxPower;

	//reserved
	TInt iReserved;
	};


#endif //__USBHOSTDEFS_H__
