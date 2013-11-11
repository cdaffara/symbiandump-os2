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
// Contains implementation of BSUL class to cache drive information
// 
//

/**
 @file
*/

#include <bsul/ccacheddriveinfo.h>

using namespace BSUL;

/**
 Constructs CCachedDriveInfo object by retrieving the drive status using aFs
 @param aFs Reference to connected filesystem
 */
EXPORT_C CCachedDriveInfo* CCachedDriveInfo::NewL(RFs& aFs)
	{
	CCachedDriveInfo *self = new(ELeave) CCachedDriveInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aFs);
	CleanupStack::Pop(self);
	return self;
	}

/**
 Constructs CCachedDriveInfo object by retrieving the drive status using aFs
 @param aFs Reference to connected filesystem
 */
EXPORT_C CCachedDriveInfo* CCachedDriveInfo::NewLC(RFs& aFs)
	{
	CCachedDriveInfo *self = new(ELeave) CCachedDriveInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aFs);
	return self;
	}
/**
 @internalComponent
 */
CCachedDriveInfo::CCachedDriveInfo()
	{}

/**
 @internalComponent
 */
void CCachedDriveInfo::ConstructL(RFs& aFs)
	{
	// Goes through each drive, and stores whether or not it is available, 
	// the drive's attributes, and the drive's media attributes
	for (TDriveUnit drive(EDriveZ); drive >= EDriveA; drive = TInt(drive) - 1)
		{
		ASSERT(aFs.IsValidDrive(drive));
		
		TDriveInfo driveInfo;
		User::LeaveIfError(aFs.Drive(driveInfo, drive));
		iDriveAndMediaAttributes[drive].iDriveAttributes = driveInfo.iDriveAtt;
		iDriveAndMediaAttributes[drive].iMediaAttributes = driveInfo.iMediaAtt;
		iDriveAndMediaAttributes[drive].iMediaType = driveInfo.iType;
		}
	}
	
/**
 Frees all allocated resources
 */
EXPORT_C CCachedDriveInfo::~CCachedDriveInfo()
	{
	}

/**
 @internalComponent
 Returns the TDriveUnit associated with the given path
 @param aFullName File name that includes a drive
 @return The drive unit associated with aFullName
 @leave Leaves with a system-wide error code if the aFullName cannot be parsed,
 		or with KErrBadName if the supplied path does not contain a drive letter
 */
TDriveUnit CCachedDriveInfo::PathToDriveUnitL(const TDesC& aFullName)
	{
	// check if the filename can be parsed
	TParse checkParse;
	TInt retcode = checkParse.Set(aFullName, NULL, NULL);
	User::LeaveIfError(retcode);
	
	TParsePtrC parse(aFullName);
	if (!parse.DrivePresent())
		{
		User::Leave(KErrBadName);
		}
	
	TDriveUnit driveId(aFullName);
	
	return driveId;
	}

/**
 Checks if the drive associated with aFullName is both read-only and internal by 
 checking that the KMediaAttWriteProtected and KDriveAttInternal flags are both set.

 @param aFullName File name that includes a drive
 @return Returns ETrue if the drive is read-only and internal, EFalse otherwise (including when the drive is not mounted)
 @leave Leaves with a system-wide error code if the aFullName cannot be parsed,
 		or with KErrBadName if the supplied path does not contain a drive letter
 */
EXPORT_C TBool CCachedDriveInfo::IsReadOnlyInternalL(const TDesC& aFullName) const
	{
	TDriveUnit driveId = PathToDriveUnitL(aFullName);
	
	return IsReadOnlyInternalL(driveId);
	}

/**
 Checks if the specified drive is both read-only and internal by checking that the 
 KMediaAttWriteProtected and KDriveAttInternal flags are both set.

 @param aDrive The drive whose read-only status is being retrieved
 @return Returns ETrue if the drive is read-only and internal, EFalse otherwise (including when the drive is not mounted)
 @leave Leaves with KErrBadName if aDrive is not a valid drive between EDriveA and EDriveZ
 */
EXPORT_C TBool CCachedDriveInfo::IsReadOnlyInternalL(TDriveUnit aDrive) const
	{
	if (aDrive < EDriveA || aDrive > EDriveZ)
		{
		User::Leave(KErrBadName);
		}

	if ( (iDriveAndMediaAttributes[aDrive].iDriveAttributes & KDriveAttInternal) &&
		 (iDriveAndMediaAttributes[aDrive].iMediaAttributes & KMediaAttWriteProtected) )
		{
		return ETrue;
		}
	
	return EFalse;
	}

/**
 This method allows the caller to test the attributes of a drive to see if 
 they are set. For example, a drive can be check whether it is remote by 
 checking the flag KDriveAttRemote is set.
 Valid flags are those defined in e32const.h starting KDriveAtt*

 @see TDriveInfo
 @param aDrive The drive whose attribute information is being tested
 @param aFlags The attributes to be checked for the drive
 @return ETrue if attributes in the drive are set, EFalse otherwise 
 @leave KErrBadName if aDrive is not a valid drive between EDriveA and EDriveZ
 */
EXPORT_C TBool CCachedDriveInfo::IsFlagSetOnDriveL(TDriveUnit aDrive, TUint aFlags) const
	{
	if (aDrive < EDriveA || aDrive > EDriveZ)
		{
		User::Leave(KErrBadName);
		}

	return ((iDriveAndMediaAttributes[aDrive].iDriveAttributes & aFlags) == aFlags) ? ETrue : EFalse;
	}
		
/**
 This method allows the caller to check the media type of a drive, to 
 see if a drive is of a particular media type. It also optionally
 returns the media type of the drive.
  
 @see TDriveInfo
 @param aDrive The drive whose media type information is being checked.
 @param aTestValue The media type to compare against the drive provided.
 @param aActual return parameter - if the caller provides a pointer
 for this parameter type, then it will be populated with the actual media type
 of the drive. This is useful to avoid repeated queries if the media type
 is needed.
 @return ETrue if the drive media type matches the media type provided
 @leave KErrBadName if aDrive is not a valid drive between EDriveA and EDriveZ
 */
EXPORT_C TBool CCachedDriveInfo::MediaTypeL(TDriveUnit aDrive, TMediaType aTestValue, TMediaType* aActual) const
	{
	if (aDrive < EDriveA || aDrive > EDriveZ)
		{
		User::Leave(KErrBadName);
		}
	
	if (aActual)
		{
		*aActual = iDriveAndMediaAttributes[aDrive].iMediaType;
		}
	
	return (iDriveAndMediaAttributes[aDrive].iMediaType == aTestValue) ? ETrue : EFalse;
	}

/**
 This method fetches the default removable memory card (MMC card). To determine
 the MMC drive a default algorithm will be used.

 The algorithm to be used will be to search through the drive list provided
 by RFs for the first drive that has the following properties 
 1) Drive type is EMediaHardDisk
 2) Drive attributes are KDriveAttRemovable and KDriveAttLocal
  
 @see TDriveInfo
 @return TDriveUnit The drive found by the algorithm
 @leave KErrNotFound if drive not found
 */
EXPORT_C TDriveUnit CCachedDriveInfo::GetDefaultRemovableMemoryCardDriveL() const
	{
	for (TInt driveNum=EDriveA; driveNum<=EDriveZ; driveNum++)
		{
	    // Confirmed on H4 board that the properties below were present for an MMC card. Also
		// E32 has a test PBASE-T_MMCDRV-0164 that also confirms this.
		if (IsFlagSetOnDriveL(driveNum, KDriveAttRemovable|KDriveAttLocal) &&
	        MediaTypeL(driveNum, EMediaHardDisk))
			{
			// Found MMC drive.
			return driveNum;
			}
	    }

	// No valid MMC drive available.
	User::Leave(KErrNotFound);
	return -1; // to avoid warning
	}

