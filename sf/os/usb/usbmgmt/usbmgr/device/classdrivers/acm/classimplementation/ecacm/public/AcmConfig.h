/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @released
*/

#ifndef __ACMCONFIG_H__
#define __ACMCONFIG_H__

#include <e32base.h>

/**
Constant to use as the key when subscribing to the ACM function
configuration data.
*/
static const TInt32 KAcmKey = 0x1028577C;

/**
Structure describing the ACM function configuration
*/
NONSHARABLE_STRUCT(TAcmConfig)
	{
	/**
	iProtocol
	iProtocol is the only field used at this time. Stores the protocol 
	that this AcmFunction was created to use.
	*/
	TUint8 iProtocol;	
	/**
	Reserved for future use
	*/
	TInt8 iReserved8Bit1; 
	/**
	Reserved for future use
	*/
	TInt8 iReserved8Bit2;
	/**
	Reserved for future use
	*/
	TInt8 iReserved8Bit3;
	/**
	Reserved for future use
	*/
	TInt32 iReserved1;
	/**
	Reserved for future use
	*/
	TInt32 iReserved2;
	/**
	Reserved for future use
	*/
	TInt32 iReserved3;
	/**
	Reserved for future use
	*/
	TInt32 iReserved4;
	};

/**
Structure to hold TAcmConfig structs and publish all available 
configurations in a single wrapper.
*/
NONSHARABLE_STRUCT(TPublishedAcmConfigs)
	{	
	/**
	Constant to define the maximum number of ACM functions it should
	be possible to create. Used to calculate the size of the byte
	array to allocate when publishing the data.
	*/
	static const TInt16 KAcmMaxFunctions = 8;
				   			
	/**
	iAcmConfigVersion
	Version 1 is the only valid version at this time.
	*/
	TUint		iAcmConfigVersion;
	/**
	iAcmCount
	iAcmCount defines number of valid AcmConfigs in sequence. Only AcmConfigs
	with an array index < AcmCount are valid. If iAcmCount == 0 then there  
	are no valid AcmConfigs at this time.
	*/
	TUint		iAcmCount; 
	
	/**
	iAcmConfig
	Array of TAcmConfig structs to hold the available Acm configurations
	*/
	TAcmConfig	iAcmConfig[KAcmMaxFunctions];
	
	/**
	Reserved for future use
	*/
	TInt32 iReserved1;
	/**
	Reserved for future use
	*/
	TInt32 iReserved2;
		/**
	Reserved for future use
	*/
	TInt32 iReserved3;
	/**
	Reserved for future use
	*/
	TInt32 iReserved4;	
	};

#endif //__ACMCONFIG_H__