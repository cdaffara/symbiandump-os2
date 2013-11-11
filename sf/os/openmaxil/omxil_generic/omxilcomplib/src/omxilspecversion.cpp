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

#include <openmax/il/common/omxilspecversion.h>

const TUint8 TOmxILSpecVersion::KSpecVersionMajor;
const TUint8 TOmxILSpecVersion::KSpecVersionMinor;
const TUint8 TOmxILSpecVersion::KSpecVersionRevision;
const TUint8 TOmxILSpecVersion::KSpecVersionStep;

EXPORT_C
TOmxILVersion::TOmxILVersion(TUint8	aMajor,
							 TUint8	aMinor,
							 TUint8	aRev,
							 TUint8	aStep)
	{
	iSpecVersion.s.nVersionMajor = aMajor;
	iSpecVersion.s.nVersionMinor = aMinor;
	iSpecVersion.s.nRevision	 = aRev;
	iSpecVersion.s.nStep		 = aStep;
	}

EXPORT_C
TOmxILVersion::operator OMX_VERSIONTYPE&()
	{
	return iSpecVersion;
	}

#ifdef _OMXIL_COMMON_SPEC_VERSION_CHECKS_ON
TBool
TOmxILVersion::operator!=(
	const OMX_VERSIONTYPE& aVer) const
	{
	return !operator==(aVer);
	}

TBool
TOmxILVersion::operator==(
	const OMX_VERSIONTYPE& aVer) const
	{
	if (iSpecVersion.s.nVersionMajor == aVer.s.nVersionMajor		&&
		iSpecVersion.s.nVersionMinor == aVer.s.nVersionMinor		&&
		iSpecVersion.s.nRevision	 == aVer.s.nRevision			&&
		iSpecVersion.s.nStep		 == aVer.s.nStep)
		{
		return ETrue;
		}
	return EFalse;
	}
#endif	

EXPORT_C
TOmxILSpecVersion::TOmxILSpecVersion()
	:
	TOmxILVersion(KSpecVersionMajor,
				  KSpecVersionMinor,
				  KSpecVersionRevision,
				  KSpecVersionStep)
	{
	}



