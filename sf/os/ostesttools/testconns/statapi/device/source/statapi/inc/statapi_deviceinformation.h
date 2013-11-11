/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#if !defined(__STATAPI_DEVICEINFORMATION_H__)
#define __STATAPI_DEVICEINFORMATION_H__

#include <hal_data.h>

//------------------------------------------------------------------------------

#define NO_MORE_DEVICE_INFORMATION			_S8("NO MORE DEVICE INFORMATION")	// end of array

const TText8* const ENUM_DEVICEINFORMATIONArray[5][20] = 
{
	{
		//UID Info
		_S8("5mx"),
		_S8("Brutus"),
		_S8("Cogent"),
		_S8("Win32 Emulator"),
		_S8("WinC"),
		_S8("CL7211 Evaluation"),
		_S8("LinkUp"),
		_S8("Assabet"),
		_S8("IQ80310"),
		_S8("Integrator"),
		_S8("Helen"),
		NO_MORE_DEVICE_INFORMATION
	},
	{
		//CPU Type
		_S8("ARM"),
		_S8("MCORE"),
		_S8("X86"),
		NO_MORE_DEVICE_INFORMATION
	},
	{
		//CPU ABI
		_S8("ARM4"),
		_S8("ARMI"),
		_S8("THUMB"),
		_S8("MCORE"),
		_S8("MSVC"),
		_S8("ARM5T"),
		NO_MORE_DEVICE_INFORMATION
	},
	{
		//DEVICE FAMILY
		_S8("Crystal"),
		_S8("Pearl"),
		_S8("Quartz"),
		NO_MORE_DEVICE_INFORMATION
	},
	{
		//Manufacturer
		_S8("Ericsson"),
		_S8("Motorola"),
		_S8("Nokia"),
		_S8("Panasonic"),
		_S8("Psion"),
		_S8("Intel"),
		_S8("Cogent"),
		_S8("Cirrus"),
		_S8("Linkup"),
		_S8("Texas Instruments"),
		NO_MORE_DEVICE_INFORMATION
	}
};

//------------------------------------------------------------------------------

const TUint ENUM_VALDEVICEINFORMATIONArray[5][20] =
{
	{
		//UID Info
		HALData::EMachineUid_Series5mx,
		HALData::EMachineUid_Brutus,
		HALData::EMachineUid_Cogent,
		HALData::EMachineUid_Win32Emulator,
		HALData::EMachineUid_WinC,
		HALData::EMachineUid_CL7211_Eval,
		HALData::EMachineUid_LinkUp,
#ifndef SYMBIAN_DIST_SERIES60
		HALData::EMachineUid_Assabet,
		HALData::EMachineUid_IQ80310,
		HALData::EMachineUid_Integrator,
		HALData::EMachineUid_Helen,
#endif
		NULL
	},
	{
		//CPU Type
		HALData::ECPU_ARM,
		HALData::ECPU_MCORE,
		HALData::ECPU_X86,
		NULL
	},
	{
		//CPU ABI
		HALData::ECPUABI_ARM4,
		HALData::ECPUABI_ARMI,
		HALData::ECPUABI_THUMB,
		HALData::ECPUABI_MCORE,
		HALData::ECPUABI_MSVC,
#ifndef SYMBIAN_DIST_SERIES60
		HALData::ECPUABI_ARM5T,
#endif
		NULL
	},
	{
		//DEVICE FAMILY
		HALData::EDeviceFamily_Crystal,
		HALData::EDeviceFamily_Pearl,
		HALData::EDeviceFamily_Quartz,
		NULL
	},
	{
		//Manufacturer
		HALData::EManufacturer_Ericsson,
		HALData::EManufacturer_Motorola,
		HALData::EManufacturer_Nokia,
		HALData::EManufacturer_Panasonic,
		HALData::EManufacturer_Psion,
		HALData::EManufacturer_Intel,
		HALData::EManufacturer_Cogent,
		HALData::EManufacturer_Cirrus,
		HALData::EManufacturer_Linkup,
#ifndef SYMBIAN_DIST_SERIES60
		HALData::EManufacturer_TexasInstruments,
#endif
		NULL
	}
};

#endif
