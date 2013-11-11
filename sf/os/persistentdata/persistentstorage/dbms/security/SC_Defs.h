// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Global definitions for DBMS security policy framework
// 
//

#ifndef __SC_DEFS_H__
#define __SC_DEFS_H__

#include <e32def.h>

/**
KMaxStmtLength constant represents max allowed text policy file statement length.
@internalComponent
*/
const TInt KMaxStmtLength = 256;

/**
The following three constants (KDBSCMajorVersion, KDBSCMinorVersion, KDBSCBuild) 
represent major, minor and build version numbers, currently 
supported by the DBMS security policy framework.
@internalComponent
*/

/**
An enum with consecutive DBSC major version numbers.
If a new major version number has to be added, add it at the end of the enum, giving
it the next available number m (KDBSCMajorVersion<m>) and change the value of 
KDBSCMajorVersion constant.
@internalComponent
*/
enum
	{
	//first security policy file major version.
	KDBSCMajorVersion1 = 1
	};

/**
An enum with consecutive DBSC minor version numbers.
If a new minor version number has to be added, add it at the end of the enum, giving
it the next available number n (KDBSCMinorVersion<m_n>, where m stands for major, n - for minor) 
and change the value of KDBSCMinorVersion constant.
@internalComponent
*/
enum
	{
	//first security policy file minor version.
	KDBSCMinorVersion1_0 = 0,
	//A new section type was added to the set of supported sections in the security
	//policy file - "[BACKUP]" with a "SID" statement, for the sid of the process having
	//rights to do databases backup and restore.
	KDBSCMinorVersion1_1 = 1
	};

//If a version v has to be compared agianst some other version, use:
//(v.maj * 128 + v.min). 128, because v.maj an v.min are 8 bit integers.
//The CVERSION macro does the calculation. "C" in the macro name stands for "COMPARABLE".
#define CVERSION(maj, min) (((maj) * 128) + (min))

//Current software version.
const TInt8 KDBSCMajorVersion	= KDBSCMajorVersion1;
const TInt8 KDBSCMinorVersion	= KDBSCMinorVersion1_1;
const TInt  KDBSCVersion		= CVERSION(KDBSCMajorVersion, KDBSCMinorVersion);

const TInt16 KDBSCBuild			= 1;//Stored in the file header, but not used

#endif//__SC_DEFS_H__
