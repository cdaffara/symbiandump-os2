// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//


/**
 @file
 @internalComponent
*/

#ifndef OMXILSPECVERSION_H
#define OMXILSPECVERSION_H

#include <e32def.h>
#include <e32const.h>

#include <openmax/il/khronos/v1_x/OMX_Types.h>

class TOmxILVersion
	{

public:

	IMPORT_C TOmxILVersion(TUint8 aMajor,
							TUint8	aMinor,
							TUint8	aRev,
							TUint8	aStep);


	IMPORT_C operator OMX_VERSIONTYPE&();
	
#ifdef _OMXIL_COMMON_SPEC_VERSION_CHECKS_ON
	TBool operator!=(const OMX_VERSIONTYPE& aVer) const;

	TBool operator==(const OMX_VERSIONTYPE& aVer) const;
#endif

private:

	OMX_VERSIONTYPE iSpecVersion;

	};

class TOmxILSpecVersion : public TOmxILVersion
	{

public:

	static const TUint8 KSpecVersionMajor	  = OMX_VERSION_MAJOR;
	static const TUint8 KSpecVersionMinor	  = OMX_VERSION_MINOR;
	static const TUint8 KSpecVersionRevision  = OMX_VERSION_REVISION;
	static const TUint8 KSpecVersionStep	  = OMX_VERSION_STEP;

public:

	IMPORT_C TOmxILSpecVersion();

	};

#endif // OMXILSPECVERSION_H

