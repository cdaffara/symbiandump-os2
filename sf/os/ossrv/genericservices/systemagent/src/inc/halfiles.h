// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// HALSettings header file 
// 
//

/**
 @file
 @internalComponent
*/

#ifndef HALFILES_H
#define HALFILES_H

#include <hal.h>
#include <f32file.h>
#include <bautils.h>
#include <bacline.h> 

/** HAL attributes data folder */
_LIT(KHalFilePath,"_:\\private\\102825B1\\");
/** HAL attributes data file name */
_LIT(KHalFileName,"HAL.DAT");
/** Buffer descriptor for holding complete HAL data file path and name. */
typedef TBuf<28> THalFileName; 



/**First 4 bytes in the HAL.DAT ('h' 'a' 'l' and version '0')
*/
const TUint32 typePrefix = 0x006C6168; 

/**
HALSettings HAL.DAT header class
This class is used to validate HAL.DAT file header
*/
class THalFileHeader
	{
	TUint32 iMachineUid;	//Machine UID	
	TUint32 iTypePrefix;	//HAL.DAT first 4 bytes 'h' 'a' 'l' and version '0'
	public:
	THalFileHeader(TUint32 aMachineUid, TUint32 aTypePrefix);
	TInt ValidateHeader();
	};
	
/** Function to manage command line
*/	
TInt HALSettingsManager();

/** Function to Initialise HAL attribute
*/
TInt InitialiseHAL();
/** Function to Persist HAL attribute
*/
TInt PersistHAL();


#endif //HALFILES_H
