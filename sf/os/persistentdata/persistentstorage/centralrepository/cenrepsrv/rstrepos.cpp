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
//

#include "rstrepos.h"

/**
Constructor.
Constructs the object of this class with the specified uid.

@param aUid The uid of the repository which is to be restored
*/
CRestoredRepository::CRestoredRepository(TUid aUid) :
	iReposUid(aUid), iChangedKeys()
	{
	}

/**
Destructor.
Closes the array of the changed keys.
*/
CRestoredRepository::~CRestoredRepository()
	{
	iChangedKeys.Close();
	}	
	
/**
Get the uid of the repository represented by this class.

@return the uid of the repository represented by this class.
*/	
TUid CRestoredRepository::Uid() const
	{
	return iReposUid;
	}	
	
	
/**
Gets the reference of the array of changed keys.

@return the reference of the array of changed keys.
*/
const RArray<TUint32>& CRestoredRepository::ChangedKeys() const
	{
	return const_cast<const RArray<TUint32>&>(iChangedKeys);
	}

		
/**
Adds a specified key to the key list of the repository.
which has been changed during the restoration.
If the key is already on the list, no repeated entry will be added.

@param aKey The key to be added.
@leave The function leaves with one of the system wide error codes except for
KErrAlreadyExists, if the operation fails.
*/
void CRestoredRepository::AddKeyL(TUint32 aKey)
	{
	TInt err = iChangedKeys.InsertInUnsignedKeyOrder(aKey);
	if((err != KErrNone) && (err != KErrAlreadyExists))
		{
		User::Leave(err);
		}
	}
	
/**
Compares 2 CRestoredRepository objects using their data members iUid.

@param aRstRepos1 The first CRestoredRepository object to be compared.
@param aRstRepos2 The second CRestoredRepository object to be compared.
@return: 1 if the first object is greater than the second. -1 if the 
first object is less than the second. 0 if they are equal.
*/
TInt CRestoredRepository::CompareUids(const CRestoredRepository& aRstRepos1,const CRestoredRepository& aRstRepos2)
	{
    if (aRstRepos1.iReposUid.iUid < aRstRepos2.iReposUid.iUid)
    	{
        return -1 ;
    	}
    else
    	{
	    if (aRstRepos1.iReposUid.iUid > aRstRepos2.iReposUid.iUid)
	    	{
		    return 1 ;
	    	}
    	else
    		{
	    	return 0 ;
    		}
		}
	}
