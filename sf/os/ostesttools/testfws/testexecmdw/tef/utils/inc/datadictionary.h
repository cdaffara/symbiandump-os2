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
* This contains DataDictionary.h
*
*/



#ifndef __DATA_DICTIONARY_H__
#define __DATA_DICTIONARY_H__

#include <e32base.h>
#include <e32hashtab.h>

/**
 * Data dictionary maximum name size
 */
const TInt									KMaxDataDictionaryNameSize = 256;

/**
 * Defines a modifiable buffer descriptor to contain the name of an entry in the dictionary
 */
typedef TBuf<KMaxDataDictionaryNameSize>	TDataDictionaryName;

class CDataWrapper;

class CDataDictionary : public CBase
/**
 * @internalAll
 * @test
 *
 * @see		CBase
 *
 * This dictionary associates a name with a data wrapper object.
 * Maintenance of the dictionary is perfomed by the methods AddDataL and DeleteDataL.
 */
	{
private:
	/**
	 * @internalAll
	 * @test
	 *
	 * Data dictionary storage area
	 */
	typedef RHashMap<TDataDictionaryName, CDataWrapper*>		RDataStore;
	typedef THashMapIter<TDataDictionaryName, CDataWrapper*>	TDataIter;

public:
	CDataDictionary();
	virtual ~CDataDictionary();

	void			AddDataL(const TDataDictionaryName& aName, CDataWrapper* aData);
	void			DeleteDataL(const TDataDictionaryName& aName);
	void			SetCurrentDataL(const TDataDictionaryName& aName);
	CDataWrapper*	GetDataL(const TDataDictionaryName& aName);
	TAny*			CurrentObject();
	TAny*			GetObjectL(const TDesC& aName);
	void			SetObjectL(const TDataDictionaryName& aName, TAny* aObject);
	void			Empty();
	TInt			Outstanding(const TDesC& aName, TBool& aMoreToDo);

private:
	static TUint32	Hash(const TDataDictionaryName& aName);
	static TBool	Identity(const TDataDictionaryName& aName1, const TDataDictionaryName& aName2);

private:
	RDataStore		iStore;
	CDataWrapper*	iCurrentObject;
	};

#endif // __DATA_DICTIONARY_H__
