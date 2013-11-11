// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Methods, data structures and constants common to Feature Registry Setup EXE
// and Public API DLL, including private panic method.
// 
//

#ifndef FEATREGCMN_H
#define FEATREGCMN_H

#include <e32std.h>
#include <e32property.h>
#include <f32file.h>
#include <featregpan.h>

_LIT(KFeatureConfigFile,"_:\\private\\102744CA\\featreg.cfg");
typedef TBuf<32> TConfigFileName;

_LIT(KFeatRegSetupExe, "z:\\sys\\bin\\featregsetup.exe");

//match pattern when searching for the file
//note that for core image the file will be featreg.cfg
//and for other rofs section it will be in the format featreg.cfg[x-y]
//whwere x is the rom image id and y is currently set to 0(unused)
_LIT(KFeatregMatchPattern,"featreg.cfg*");
_LIT(KFeatregRomPrivatePath,"z:\\private\\102744CA\\");

// feature registry data is currently kept in an RProperty with category and key:
const TUid KFeaturePropCat = { 0x102744CA };
const TUint KFeaturePropKey = 0;
// if property with KFeaturePropCat and following key is defined, featregsetup.exe
// looks for config info in its c: private data cage instead of z: (for tests only):
const TUint KFeatRegConfigTestKey = 1;

/**
This function will try to find and open all the "featreg.cfg" files from the path specified
process them in the order of increasing rom image id before copying the aggregate content
into the buffer aBuf
*/
GLREF_C TInt ReadMultipleFeatureFileToBuf(RFs& aFs,const TDesC& aPath,RBuf8& aBuf);

/**
 * First block of data in feature config file and resulting binary property:
 * says how many individual feature entries and "default-supported" ranges follow.
 * Config data is invalid if header is not present, or it does not predict size
 * of feature config file/property.
 */
class TFeatureHeader
	{
public:
	TUint32 iTypePrefix;			// must be equal to validTypePrefix
	TUint32 iVersionNumber;			// zero is the only valid version number
	TUint32 iFeatureEntryCount;
	TUint32 iFeatureRangeCount;

	inline TUint32 PredictedPropertySize() const;
	inline void SetInvalid();
	inline TBool IsInvalid() const;
	inline TBool IsInvalidOrBadSize(TUint32 aActualPropertySize) const;
	};

// First 4 bytes of config file: ASCII f-e-a-t. Stored in TUint32 in little endian, i.e. reverse order
const TUint32 validTypePrefix = 0x74616566;
const TUint32 invalidTypePrefix = 0; // must not equal validTypePrefix

/**
 * Second block of data in feature config file and resulting binary property:
 * header.iFeatureEntryCount * TFeatureEntry
 * If a feature UID is listed, bits in its status word control the following:
 * - bit 0 (0x1): if set, feature is present, if not, feature is not present
 * - bit 1 (0x2): if set, feature is upgradable in this ROM configuration
 * - bits 2-31: reserved for future use & must be zero
 * Config data is invalid if these are not listed from lowest to highest UID with
 * no repeats.
 */
struct TFeatureEntry
	{
	TUint32 iUid;
	TUint32 iInfo;
	};

/**
 * Third/last block of data in feature config file and resulting binary property:
 * header.iFeatureRangeCount * TFeatureRange
 * Features with UIDs in these "default-supported" ranges are supported unless
 * they are individually listed in the "entry" block as not supported.
 * Config data is invalid if any ranges have iLowUid higher than iHighUid.
 */
struct TFeatureRange
	{
	TUint32 iLowUid;
	TUint32 iHighUid;
	};

GLREF_C void Panic(TFeatRegPanic aReason);


// ------------------------------------------------------------------------- //
// Methods, data structures and constants common to Feature Registry Setup EXE
// and Public API DLL, including private panic method.
// ------------------------------------------------------------------------- //


/**
Construct config file path for the system drive.
@param aFileName On completion will contain the full path and filename.
*/
inline void GetSystemDrivePath(TConfigFileName& aFileName)
	{
	aFileName.Copy(KFeatureConfigFile);
	aFileName[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive());
	}



/** returns the total size of the property that is predicted by the counts
	in the header. Confirm !IsInvalid() before using otherwise numerical
	overflow may occur */
inline TUint32 TFeatureHeader::PredictedPropertySize() const
	{
	return sizeof(TFeatureHeader) +
			+ iFeatureEntryCount * sizeof(TFeatureEntry)
			+ iFeatureRangeCount * sizeof(TFeatureRange);
	}

/** sets invalid values in the header that API will report as corrupt regardless
	of what data is provided with it */
inline void TFeatureHeader::SetInvalid()
	{
	iTypePrefix = invalidTypePrefix;
	iVersionNumber = 1;	// i.e. not zero
	// note total size of feature property may not exceed these constants,
	// so they are definitely invalid
	iFeatureEntryCount = RProperty::KMaxLargePropertySize + 1;
	iFeatureRangeCount = RProperty::KMaxLargePropertySize + 1;
	}

/** asks whether the header contains invalid values irrespective of data size 
	See also IsInvalidOrBadSize() below */
inline TBool TFeatureHeader::IsInvalid() const
	{
	// pretty safe to assume RProperty::KMaxLargePropertySize will not
	// ever be large enough to cause overflow problems
	return (iTypePrefix != validTypePrefix)
		|| (iVersionNumber != 0)
		|| (iFeatureEntryCount >  RProperty::KMaxLargePropertySize)
		|| (iFeatureRangeCount >  RProperty::KMaxLargePropertySize)
		|| (PredictedPropertySize() > RProperty::KMaxLargePropertySize);
	}

/** asks whether aActualPropertySize is in the valid range, header is invalid or
	mismatch between property size predicted by the header and that supplied */
inline TBool TFeatureHeader::IsInvalidOrBadSize(TUint32 aActualPropertySize) const
	{
	return (aActualPropertySize < sizeof(TFeatureHeader))
		|| (aActualPropertySize > RProperty::KMaxLargePropertySize)
		|| IsInvalid()
		|| (PredictedPropertySize() != aActualPropertySize);
	}


#endif
