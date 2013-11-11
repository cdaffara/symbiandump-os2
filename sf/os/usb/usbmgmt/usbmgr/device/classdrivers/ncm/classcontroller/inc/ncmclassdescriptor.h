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
@internalComponent
*/


#ifndef NCMCLASSDESCRIPTOR_H
#define NCMCLASSDESCRIPTOR_H

#include <e32std.h>


const TUint   KUSBNcmClassSpecificBufferSize = 29;


/**
 * Defined NCM Class Descriptor
 */
NONSHARABLE_CLASS(TNcmClassDescriptor) 	///< Class-specific descriptor
	{
public:
	
	//Header Functional Descriptor
	TUint8	iHdrSize;			///< Header size
	TUint8	iHdrType;			///< Type
	TUint8	iHdrSubType;		///< Sub-type
	TUint16 iHdrBcdCDC; 		///< CDC version no in binary coded decimal

	//Union Functional Descriptor
	TUint8	iUnSize;			///< Union descriptor size
	TUint8	iUnType;			///< Union descriptor type
	TUint8	iUnSubType; 		///< Union descriptor sub-type
	TUint8	iUnMasterInterface; ///< Master interface number
	TUint8	iUnSlaveInterface;	///< Slave interface number

	//Ethernet Networking Functional Descriptor
	TUint8	iEthFunLength;	///<	Size of this functional descriptor
	TUint8	iEthFunType;	///<	CS_INTERFACE		
	TUint8	iEthFunSubtype;	///<	Ethernet Networking functional descriptor subtype as defined in [USBCDC1.2]
	TUint8	iMACAddress;		///<	Index of string descriptor. The string descriptor holds the 48bit Ethernet MAC address. 
	TUint	iEthernetStatistics;	///<	Indicates which Ethernet statistics functions the device collects. 
	TUint16	iMaxSegmentSize;	///<	The maximum segment size that the Ethernet device is capable of supporting. 
	TUint16	iNumberMCFilters;	///<	Contains the number of multicast filters that can be configured by the host.
	TUint8	iNumberPowerFilters;	///<	Contains the number of pattern filters that are available for causing wake-up of the host.

	
	//NCM Functional Descriptor
	TUint8	iNcmFunLength;	///<	Size of this functional descriptor
	TUint8	iNcmFunType;	///<	CS_INTERFACE
	TUint8	iNcmFunSubtype;	///<	NCM Functional Descriptor subtype
	TUint16	iNcmVersion;		///<	Release number of this specification in BCD
	TUint8	iNetworkCapabilities;	///<	Specifies the capabilities of this function
	
public:
	TDes8& Des();

private:

	TBuf8<KUSBNcmClassSpecificBufferSize> iBuffer;
	};

#endif // NCMCLASSDESCRIPTOR_H

