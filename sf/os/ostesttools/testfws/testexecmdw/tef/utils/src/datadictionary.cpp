/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
* @file
* This contains CDataDictionary
*
*/



/**
 @prototype
 @test
*/

#include <e32std.h>

#include "datadictionary.h"
#include "datawrapper.h"

TUint32 CDataDictionary::Hash(const TDataDictionaryName& aName)
	{
	return DefaultHash::Des16(aName);
	}

TBool CDataDictionary::Identity(const TDataDictionaryName& aName1, const TDataDictionaryName& aName2)
	{
	return aName1.Compare(aName2)==0;
	}

CDataDictionary::CDataDictionary()
/**
 * Constructor
 */
:	iStore(Hash, Identity)
,	iCurrentObject(NULL)
	{
	}


CDataDictionary::~CDataDictionary()
/**
 * Destructor
 */
	{
	Empty();
	}

void CDataDictionary::AddDataL(const TDataDictionaryName& aName, CDataWrapper* aData)
/**
 * Add a data entry to the dictionary
 *
 * @param	aName - name of the dictionary entry
 * @param	aData - the data to add. The dictionary becomes owner of the object
 *			and is responsble for it's deletion
 *
 * @see		TDataDictionaryName
 * @see		CDataWrapper
 *
 * @leave	KErrAlreadyExists if the dictionary entry aName, or aData already exists.
 * @leave	KErrNoMemory if memory could not be allocated to store the copies of aName and aData.
 * @leave	system wide errors
 */
	{
	//	Ensure name does not already exist
	if ( iStore.Find(aName) != NULL )
		{
		User::Leave(KErrAlreadyExists);
		}

	//	Ensure data does not already exist
	TDataIter	iter(iStore);
	iter.Reset();
	for ( CDataWrapper*const * data=iter.NextValue(); data!=NULL; data=iter.NextValue() )
		{
		if ( aData == *data )
			{
			User::Leave(KErrAlreadyExists);
			}
		}

	iStore.InsertL(aName, aData);
	if ( iCurrentObject == NULL )
		{
		iCurrentObject=aData;
		}
	}

void CDataDictionary::DeleteDataL(const TDataDictionaryName& aName)
/**
 * Delete a data entry from the dictionary
 *
 * @param	aName - name of the dictionary entry
 *
 * @see		TDataDictionaryName
 *
 * @leave	KErrNotFound if the dictionary entry aName is not found
 */
	{
	//	Ensure name already exist
	CDataWrapper**	wrapper=iStore.Find(aName);

	if ( wrapper != NULL )
		{
		delete* wrapper;
		iStore.Remove(aName);
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}

void CDataDictionary::SetCurrentDataL(const TDataDictionaryName& aName)
/**
 * Set the current active dictionary entry
 *
 * @param	aName - name of the dictionary entry
 *
 * @see		TDataDictionaryName
 *
 * @leave	KErrNotFound if the dictionary entry aName is not found
 */
	{
	//	Ensure name already exist
	CDataWrapper**	wrapper=iStore.Find(aName);

	if ( wrapper != NULL )
		{
		iCurrentObject=*wrapper;
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}

CDataWrapper* CDataDictionary::GetDataL(const TDataDictionaryName& aName)
/**
 * Get an object in the dictionary
 *
 * @param	aName - name of the dictionary entry
 *
 * @see		TDataDictionaryName
 *
 * @leave	KErrNotFound if the dictionary entry aName is not found
 *
 * @return	The data object found
 */
	{
	//	Ensure name already exist
	CDataWrapper**	ret=iStore.Find(aName);

	if ( ret == NULL )
		{
		User::Leave(KErrNotFound);
		}

	return *ret;
	}

TAny* CDataDictionary::GetObjectL(const TDesC& aName)
/**
 * Get an object in the dictionary
 *
 * @param	aName - name of the dictionary entry
 *
 * @return	The object found
 *
 * @leave	KErrNotFound if the dictionary entry aName is not found
 */
	{
	//	Ensure name already exist
	CDataWrapper**	wrapper=iStore.Find(aName);
	TAny*			ret=NULL;

	if ( wrapper != NULL )
		{
		ret=(*wrapper)->GetObject();
		}
	else
		{
		User::Leave(KErrNotFound);
		}

	return ret;
	}

void CDataDictionary::Empty()
/**
 * Empty the object dictionary
 */
	{
	TDataIter	iter(iStore);
	iter.Reset();
	for ( CDataWrapper*const * data=iter.NextValue(); data!=NULL; data=iter.NextValue() )
		{
		delete *data;
		}
	iStore.Close();

	iCurrentObject=NULL;
	}

TInt CDataDictionary::Outstanding(const TDesC& aName, TBool& aMoreToDo)
/**
 * Delegates to a data object to see if
 *  it has any outstanding requests
 *
 * @param	aName - Name of the dictionary entry
 *
 * @leave	KErrNotFound if the dictionary entry aName is not found
 *
 * @return	ETrue if there are any outstanding requests
 */
	{
	TInt	retCode=KErrNone;

	aMoreToDo=EFalse;
	if ( aName.Compare(KTEFNull)==0 )
		{
		TDataIter	iter(iStore);
		iter.Reset();
		for ( CDataWrapper*const * data=iter.NextValue(); (data!=NULL) && (!aMoreToDo); data=iter.NextValue() )
			{
			aMoreToDo = (*data)->Outstanding();
			}
		}
	else
		{
		//	Ensure name already exist
		CDataWrapper**	wrapper=iStore.Find(aName);

		if ( wrapper != NULL )
			{
			aMoreToDo = (*wrapper)->Outstanding();
			}
		else
			{
			retCode=KErrNotFound;
			}
		}

	return retCode;
	}

TAny* CDataDictionary::CurrentObject()
/**
 * Get the object of the current active dictionary entry
 *
 * @return	The object that the current data wraps
 */
	{
	return iCurrentObject ? iCurrentObject->GetObject() : NULL;
	}

/**
 * Set an object in the dictionary
 *
 * @param	aName - name of the dictionary entry
 * @param	aObject - object that the wrapper is testing
 *
 * @see		TDataDictionaryName
 *
 * @leave	KErrNotFound if the dictionary entry aName is not found
 * @leave	KErrNotSupported if the the function is not supported
 */
void CDataDictionary::SetObjectL(const TDataDictionaryName& aName, TAny* aObject)
	{
	GetDataL(aName)->SetObjectL(aObject);
	}
