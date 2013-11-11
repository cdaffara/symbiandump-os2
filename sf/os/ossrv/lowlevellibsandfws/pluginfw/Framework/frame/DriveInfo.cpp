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
// Implementation for CEComCachedDriveInfo class
// 
//

/**
 @file
 @internalComponent
*/

#include <e32const.h>
#include <f32file.h>
#include <bsul/bsul.h>
#include <ecom/ecomerrorcodes.h>
#include "DriveInfo.h"
#include "EComPatchDataConstantv2.h"
#include "EComInternalErrorCodes.h"
const TInt KInvalidIndex = -1;
const TInt32 KInvalidDrvNumber = -1;

/** static member of CEComCachedDriveInfo */
TFixedArray<TEComDrvFlags, KMaxDrives> CEComCachedDriveInfo::iDriveAttr;

/** static member of CEComCachedDriveInfo */
TInt CEComCachedDriveInfo::iLastIndex = KInvalidIndex;

/** static member of CEComCachedDriveInfo */
TBool CEComCachedDriveInfo::iInitialized = EFalse;


/** Standard factory method to instantiate CEComCachedDriveInfo instances.
*/
CEComCachedDriveInfo* CEComCachedDriveInfo::NewL(RFs& aFs)
	{
	CEComCachedDriveInfo* self = new (ELeave) CEComCachedDriveInfo;
	CleanupStack::PushL(self);
	self->ConstructL(aFs, KDiscoveryDisabledDriveList);
	CleanupStack::Pop(self);
	return self;
	}

/** Constructor of CEComCachedDriveInfo. Nothing to do. */
CEComCachedDriveInfo::CEComCachedDriveInfo()
	{
	}

/** Standard two-phase construction to complete construction
of CEComCachedDriveInfo.

@param aFs reference to a connected RFs session.
@param aDiscoveryDisabledMask bits set indicate the drives contain no plug-ins
	and need not be scanned. There is a mechanism to allow licensees to
	specify this mask at ROM build time.
@leave KErrNoMemory if out of memory, or any of the other system wide errors.
*/
void CEComCachedDriveInfo::ConstructL(RFs& aFs, TUint32 aDiscoveryDisabledMask)
	{
	if (iInitialized)
		{
		return;
		}

	iLastIndex = KInvalidIndex;

	// Get list of installed drives. This info is not available from
	// BSUL::CCachedDriveInfo API's.
	TDriveList drvList;
	User::LeaveIfError( aFs.DriveList(drvList) );

	BSUL::CCachedDriveInfo* cachedDrvInfo = BSUL::CCachedDriveInfo::NewL(aFs);
	CleanupStack::PushL(cachedDrvInfo);

	// determine attributes of valid drives
	for (TInt i = EDriveA; i <= EDriveZ; i++)
		{
		if (drvList[i])
			{
			// drive exist
			TInt j = ++iLastIndex;

			iDriveAttr[j].iDrvNumber = i; 

			TDriveUnit drvUnit(i);
			if (cachedDrvInfo->IsReadOnlyInternalL(drvUnit))
				{
				iDriveAttr[j].iFlags = EEComDrvAttrReadOnlyInternal;
				// RO internal drive does not have any other attributes
				continue;
				}

			iDriveAttr[j].iFlags = 0; 

			// Check if drive is removable
			if (cachedDrvInfo->IsFlagSetOnDriveL(drvUnit, KDriveAttRemovable))
				{
				iDriveAttr[j].iFlags |= EEComDrvAttrRemovable;
				}

			// Check if drive is on ReadWrite drive
			if (! cachedDrvInfo->IsFlagSetOnDriveL(drvUnit, KDriveAttRom))
				{
				iDriveAttr[j].iFlags |= EEComDrvAttrWritable;
				}

			TUint32 drvBitMask = 1;
			drvBitMask <<= i;
			// Three conditions for discovery disable: disabled by licensees,
			// remote drives and substituted drives
			if ( (drvBitMask & aDiscoveryDisabledMask) ||
				 cachedDrvInfo->IsFlagSetOnDriveL(drvUnit, KDriveAttSubsted) ||
				 cachedDrvInfo->IsFlagSetOnDriveL(drvUnit, KDriveAttRemote) )
				{
				iDriveAttr[j].iFlags |= EEComDrvAttrNoDiscovery;
				}
			} // if drvList[i]
		} // for

	CleanupStack::PopAndDestroy(cachedDrvInfo);
	__ASSERT_DEBUG(iLastIndex >= 0, User::Invariant());

	for (TInt i = iLastIndex + 1; i < KMaxDrives; i++)
		{
		iDriveAttr[i].iDrvNumber = KInvalidDrvNumber; 
		}

	iInitialized = ETrue;
	}

/** Is EEComDrvAttrReadOnlyInternal attribute set on the given drive?
@param aDrive the drive number
@return ETrue if drive is RO internal. EFalse means either no such drive or
	not RO internal.
@leave KEComErrDriveNotFound if aDrive is not supposed to be scanned,
	i.e. drive absent or disabled by licensees or subst or remote.
    Use TEComCachedDriveInfoIterator::SetPos instead if want to test drive
	attribute.
*/
TBool CEComCachedDriveInfo::DriveIsReadOnlyInternalL(const TInt aDrive) const
	{
	TEComCachedDriveInfoIterator iter(*this);

	if (! iter.SetPos(aDrive))
		{
		// This method is intended to be used on drives that are known
		// to be valid, e.g. drive extracted from the path of a discovered
		// DLL.
		User::Leave(KEComErrDriveNotFound);
		}
	return iter.DriveIsReadOnlyInternal();
	}

/** Test if the EEComDrvAttrRemovable attribute for the given drive is set.
@param aDrive the drive number
@return ETrue if drive is removable. EFalse means either no such drive or
	not removable.
@leave KEComErrDriveNotFound if aDrive is not supposed to be scanned,
	i.e. drive absent or disabled by licensees or subst or remote.
    Use TEComCachedDriveInfoIterator::SetPos instead if want to test drive
	is removable.
*/
TBool CEComCachedDriveInfo::DriveIsRemovableL(const TInt aDrive) const
	{
	TEComCachedDriveInfoIterator iter(*this);

	if (! iter.SetPos(aDrive))
		{
		// This method is intended to be used on drives that are known
		// to be valid, e.g. drive extracted from the path of a discovered
		// DLL.
		User::Leave(KEComErrDriveNotFound);
		}
	return iter.DriveIsRemovable();
	}

/** Test if the EEComDrvAttrWritable attribute for the given drive is set.
@param aDrive the drive number
@return ETrue if drive is writable. EFalse means either no such drive or
	drive not writable.
@leave KEComErrDriveNotFound if aDrive is not supposed to be scanned,
	i.e. drive absent or disabled by licensees or subst or remote.
    Use TEComCachedDriveInfoIterator::SetPos instead if want to test drive
	is removable.
*/
TBool CEComCachedDriveInfo::DriveIsWritableL(const TInt aDrive) const
	{
	TEComCachedDriveInfoIterator iter(*this);

	if (! iter.SetPos(aDrive))
		{
		// This method is intended to be used on drives that are known
		// to be valid, e.g. drive extracted from the path of a discovered
		// DLL.
		User::Leave(KEComErrDriveNotFound);
		}
	return iter.DriveIsWritable();
	}

// Implementation for TEComCachedDriveInfoIterator class

/** TEComCachedDriveInfoIterator Constructor
Note that the object is not yet in valid state. User must first call
First() or Last().
*/
TEComCachedDriveInfoIterator::TEComCachedDriveInfoIterator(const CEComCachedDriveInfo& aCachedDriveInfo)
	: iDriveAttr(aCachedDriveInfo.iDriveAttr)
	{
	iIndex = KInvalidIndex;
	}

/** Check if the drive is enabled for scanning and move index there if true.
@param aDrive the drive number of the drive to check.
@return ETrue the drive is enabled for scanning. Index is moved to that location.
	If drive not to be scanned, index is moved out of bound.
*/
TBool TEComCachedDriveInfoIterator::SetPos(const TInt aDrive)
	{
	for (iIndex = CEComCachedDriveInfo::iLastIndex; iIndex >= 0; iIndex--)
		{
		if (iDriveAttr[iIndex].iDrvNumber == aDrive)
			{
		    if (0 == (iDriveAttr[iIndex].iFlags & EEComDrvAttrNoDiscovery))
				{
				return ETrue;
				}
			iIndex = KInvalidIndex;
			break;
			}
		}
	return EFalse;
	}

/** Position iterator at first valid drive so as to
iterate the drive list in increasing drive number order. */
void TEComCachedDriveInfoIterator::First(void)
	{
	for (iIndex = 0; iIndex <= CEComCachedDriveInfo::iLastIndex; iIndex++)
		{
		if (0 == (iDriveAttr[iIndex].iFlags & EEComDrvAttrNoDiscovery))
			{
			return;
			}
		}
	}

/** Position iterator at last valid drive so as to
iterate the drive list in decreasing drive number order. */
void TEComCachedDriveInfoIterator::Last(void)
	{
	for (iIndex = CEComCachedDriveInfo::iLastIndex; iIndex >=0; iIndex--)
		{
		if (0 == (iDriveAttr[iIndex].iFlags & EEComDrvAttrNoDiscovery))
			{
			return;
			}
		}
	}

/** To be used in for and while loops to check if iterator
has stepped through all posible drives. */
TBool TEComCachedDriveInfoIterator::InRange(void) const
	{
	return (iIndex >= 0 && iIndex <= CEComCachedDriveInfo::iLastIndex);
	}

/** Increment iterator to next valid drive. */
void TEComCachedDriveInfoIterator::Next(void)
	{
	if (InRange())
		{
		for (++iIndex; iIndex <= CEComCachedDriveInfo::iLastIndex; iIndex++)
			{
			if (0 == (iDriveAttr[iIndex].iFlags & EEComDrvAttrNoDiscovery))
				{
				return;
				}
			}
		}
	}

/** Decrement iterator to next valid drive. */
void TEComCachedDriveInfoIterator::Prev(void)
	{
	if (InRange())
		{
		for (--iIndex; iIndex >= 0; iIndex--)
			{
			if (0 == (iDriveAttr[iIndex].iFlags & EEComDrvAttrNoDiscovery))
				{
				return;
				}
			}
		}
	}

/**
@panic USER:133 from TFixedArray if iterator is out of bound.
*/
TDriveNumber TEComCachedDriveInfoIterator::DriveNumber(void) const
	{
	return static_cast<TDriveNumber>( iDriveAttr.At(iIndex).iDrvNumber );
	}

/**
@panic USER:133 from TFixedArray if iterator is out of bound.
*/
TDriveUnit TEComCachedDriveInfoIterator::DriveUnit(void) const
	{
	return TDriveUnit( DriveNumber() );
	}

/** Test if the EEComDrvAttrReadOnlyInternal attribute for the current
drive is set.
@panic USER:133 from TFixedArray if iterator is out of bound.
*/
TBool TEComCachedDriveInfoIterator::DriveIsReadOnlyInternal(void) const
	{
	if (EEComDrvAttrReadOnlyInternal & iDriveAttr.At(iIndex).iFlags)
		{
		return ETrue;
		}
	return EFalse;
	}

/** Test if the EEComDrvAttrRemovable attribute for the current
drive is set.
@panic USER:133 from TFixedArray if iterator is out of bound.
*/
TBool TEComCachedDriveInfoIterator::DriveIsRemovable(void) const
	{
	if (EEComDrvAttrRemovable & iDriveAttr.At(iIndex).iFlags)
		{
		return ETrue;
		}
	return EFalse;
	}

/** Test if the EEComDrvAttrWritable attribute for the current drive is set.
@return ETrue if drive is writable. EFalse means drive is not writable.
@panic USER:133 from TFixedArray if iterator is out of bound.
*/
TBool TEComCachedDriveInfoIterator::DriveIsWritable(void) const
	{
	if (EEComDrvAttrWritable & iDriveAttr.At(iIndex).iFlags)
		{
		return ETrue;
		}
	return EFalse;
	}
