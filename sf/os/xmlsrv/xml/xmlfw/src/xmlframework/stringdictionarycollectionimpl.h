// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __STRINGDICTIONARYCOLLECTIONIMPL_H__
#define __STRINGDICTIONARYCOLLECTIONIMPL_H__

#include <stringpool.h>

namespace Xml
{

class MStringDictionary;

class RStringDictionaryCollectionImpl
/**
The RStringDictionaryCollectionImpl class hides the implementation of the 
RStringDictionaryCollection.

It is responsible for maintaining the list of string dictionary plugins
required by the framework.

@internalTechnology
@released
*/
	{
public:
	void OpenL();
	TInt Close();

	RStringDictionaryCollectionImpl();

	void OpenDictionaryL(const TDesC8& aDictionaryDescription);

	RStringPool& StringPool();

	MStringDictionary& CurrentDictionaryL() const;
		
private:

	RStringDictionaryCollectionImpl(const RStringDictionaryCollectionImpl& aOriginal);
	RStringDictionaryCollectionImpl& operator=(const RStringDictionaryCollectionImpl& aRhs);

	TBool LocateDictionary(const RString& aNsUri, MStringDictionary*& aStringDictionary);
	MStringDictionary* ConstructDictionaryL(const TDesC8& aDictionaryUri, 
										    RStringPool& aStringPool);

private:

	struct TDtorMapping
	{
	TUid				iDtorKey;
	MStringDictionary*	iStringDictionary;
	};

	RPointerArray<TDtorMapping>	iDtorKeyAndDictionaryList;

	static void DestroyMapping(TAny* aPtr);


/**
The reference count for this collection.
*/
	TInt											iRefCount;

/**
The StringPool for the Collection.
*/
	RStringPool										iStringPool;

/**
The current Dictionary in use.
We do not own this; for quick access only.
*/
	MStringDictionary*								iStringDictionary;

	};

}

#endif //__STRINGDICTIONARYCOLLECTIONIMPL_H__
