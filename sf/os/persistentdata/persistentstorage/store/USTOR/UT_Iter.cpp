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
//

#include "S32STOR.H"
#include "S32FILE.H"
#include "U32PERM.H"
#include "S32FileIter.h"

/**
After calling Close() RPermanentFileStoreIter instance could be used again
with the same or some other CPermanentFileStore instance.
*/
EXPORT_C void RPermanentFileStoreIter::Close()
	{
	if(iImpl)
		{
		iImpl->Close();
		}
	delete iImpl;
	iImpl = NULL;
	}

/**
Starts a CPermanentFileStore stream IDs iteration.
@param aStore CPermanentFileStore instance, which stream IDs sequence we want to 
explore.
@leave KErrNoMemory
*/
EXPORT_C void RPermanentFileStoreIter::ResetLC(const CPermanentFileStore& aStore)
	{
	__ASSERT_DEBUG(!iImpl, User::Invariant());
	::CleanupClosePushL(*this);
	CPermanentStoreCoord& coord = aStore.CoordL();
	CPermanentStoreToc& table = coord.TableL();
	iImpl = new (ELeave) RPermanentStoreTocIter(table);
	iImpl->ResetL();
	}

/**
Retrieves the next stream ID from stream IDs sequence, controlled by CPermanentFileStore 
instance. 
@return The next stream ID or KNullStreamIdValue if there are no more stream IDs.
@leave KErrNoMemory
*/
EXPORT_C TStreamId RPermanentFileStoreIter::NextL()
	{
	__ASSERT_DEBUG(iImpl, User::Invariant());
	TStreamId streamId = KNullStreamIdValue;
	CPermanentStoreToc::TEntry entry;
	while(iImpl->NextL(entry))
		{
		if((TUint32)entry.handle < KMaxStreamIdValue)
			{
			streamId = entry.handle;
			break;
			}
		}
	return streamId;
	}
