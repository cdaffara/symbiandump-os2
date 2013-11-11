// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Declaration for CEcomCachedDriveInfo class
// 
//

/**
 @file
 @internalComponent
*/

#ifndef DRIVEINFO_H
#define DRIVEINFO_H

#include <e32std.h>
#include <f32file.h>

/** A set of bit flags representing drive attributes of interest
 to ECOM */
enum TEComDrvAttr
	{
	// No scanning performed on 3 kinds of drives
	// 1. discovery disabled by licensees, 2. remote (network) drive,
	//    and 3. substituted drive. 
	EEComDrvAttrNoDiscovery		= 0x1,

	// plug-ins on removable media needs extra validation
	EEComDrvAttrRemovable		= 0x2,

	EEComDrvAttrReadOnlyInternal= 0x4,

	// Custom resolver cache cares about this
	EEComDrvAttrWritable		= 0x8
	};

/** A struct to store the driveNumber and TEComDrvAttr of a drive. */
struct TEComDrvFlags
	{
	TUint32 iFlags;
	TInt32	iDrvNumber;
	};

/** class to keep track of drive attributes of interest to ECOM.
 */
class CEComCachedDriveInfo : public CBase
	{
public:
	static CEComCachedDriveInfo* NewL(RFs& aFs);
	TBool DriveIsReadOnlyInternalL(const TInt aDrive) const;
	TBool DriveIsRemovableL(const TInt aDrive) const;
	TBool DriveIsWritableL(const TInt aDrive) const;

private:
	CEComCachedDriveInfo();
	void ConstructL(RFs& aFs, TUint32 aDiscoveryDisabledMask);

private:
	static TFixedArray<TEComDrvFlags, KMaxDrives> iDriveAttr;
	static TInt  iLastIndex;
	static TBool iInitialized;

	friend class TEComCachedDriveInfoIterator;	// to access private data
	friend class TDriveInfo_StateAccessor; // for testing
	};


/** Iterator which return only valid drives.

After instantiation, user must call either First() or Last()
to set the initial position and check InRange() before calling
the methods to access the drive number and attributes.
*/
class TEComCachedDriveInfoIterator
	{
public:
	TEComCachedDriveInfoIterator(const CEComCachedDriveInfo& aCachedDriveInfo);

	void First(void);
	void Next(void);
	void Last(void);
	void Prev(void);
	TBool SetPos(const TInt aDrive);
	TBool InRange(void) const;

	TDriveNumber DriveNumber(void) const;
	TDriveUnit	DriveUnit(void) const;
	TBool DriveIsReadOnlyInternal(void) const;
	TBool DriveIsRemovable(void) const;
	TBool DriveIsWritable(void) const;

private:
	const TFixedArray<TEComDrvFlags, KMaxDrives>& iDriveAttr;
	TInt  iIndex;
	};

#endif // DRIVEINFO_H
