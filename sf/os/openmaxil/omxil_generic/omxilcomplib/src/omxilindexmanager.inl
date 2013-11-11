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

inline COmxILIndexManager::COmxILIndexManager()
	{
	}

inline COmxILIndexManager::~COmxILIndexManager()
	{
	iManagedOmxParamIndexes.Close();
	iManagedOmxConfigIndexes.Close();
	}

inline void
COmxILIndexManager::InsertParamIndexL(TUint aParamIndex)
	{
	TInt err = iManagedOmxParamIndexes.InsertInOrder(aParamIndex);

	// Note that index duplication is OK.
	if (KErrNone != err && KErrAlreadyExists != err)
		{
		User::Leave(err);
		}

	}

inline TInt
COmxILIndexManager::FindParamIndex(TUint aParamIndex) const
	{
	return iManagedOmxParamIndexes.SpecificFindInOrder(
		aParamIndex,
		EArrayFindMode_First);
	}

inline void
COmxILIndexManager::InsertConfigIndexL(TUint aConfigIndex)
	{
	TInt err = iManagedOmxConfigIndexes.InsertInOrder(aConfigIndex);

	// Note that index duplication is OK.
	if (KErrNone != err && KErrAlreadyExists != err)
		{
		User::Leave(err);
		}

	}

inline TInt
COmxILIndexManager::FindConfigIndex(TUint aConfigIndex) const
	{
	return iManagedOmxConfigIndexes.SpecificFindInOrder(
		aConfigIndex,
		EArrayFindMode_First);
	}

inline RArray<TUint>&
COmxILIndexManager::ManagedParamIndexes()
	{
	return iManagedOmxParamIndexes;
	}

inline RArray<TUint>&
COmxILIndexManager::ManagedConfigIndexes()
	{
	return iManagedOmxConfigIndexes;
	}


