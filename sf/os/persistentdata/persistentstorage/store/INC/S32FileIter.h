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

#ifndef __S32FILEITER_H__
#define __S32FILEITER_H__

#ifndef __S32STD_H__
#include <s32std.h>
#endif

//Forward declarations
class CPermanentFileStore;
class RPermanentStoreTocIter;

/**
This class describes an object that can serve as a stream ID forward iterator
for the controlled sequence of stream IDs in CPermanentFileStore instance.
RPermanentFileStoreIter instances will allow you to access only stream IDs which are
already commited to the store.

Usage pattern:
@code
CPermanentFileStore* store;
//create "store" ...
RPermanentFileStoreIter iter;
iter.ResetLC(*store);
TStreamId id;
while((id = iter.NextL()) != KNullStreamIdValue)
	{
	RStoreReadStream in;
	in.OpenLC(*store, id);
	//read from "in" stream
	CleanupStack::PopAndDestroy(&in);
	}
CleanupStack::PopAndDestroy(&iter);
@endcode

Any change (deleting/adding streams) of stream IDs sequence during the lifetime of 
RPermanentFileStoreIter instance will invalidate it.

@internalTechnology
@released
@see CPermanentFileStore
@see TStreamId
*/
class RPermanentFileStoreIter
	{

public:
	inline RPermanentFileStoreIter();
	IMPORT_C void Close();
	IMPORT_C void ResetLC(const CPermanentFileStore& aStore);
	inline void ResetL(const CPermanentFileStore& aStore);
	IMPORT_C TStreamId NextL();

private:
	RPermanentStoreTocIter* iImpl;

	};

#include <s32fileiter.inl>

#endif//__S32FILEITER_H__
