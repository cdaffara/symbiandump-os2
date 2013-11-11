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

/**
 @internalTechnology
 @released
*/
inline RPermanentFileStoreIter::RPermanentFileStoreIter() :
	iImpl(NULL)
	{
	}

/**
Starts a CPermanentFileStore stream IDs iteration.
@param aStore CPermanentFileStore instance, which stream IDs sequence we want to 
explore.
@leave KErrNoMemory
@internalTechnology
@released
*/
inline void RPermanentFileStoreIter::ResetL(const CPermanentFileStore& aStore)
	{
	ResetLC(aStore);
	CleanupStack::Pop(this);
	}
