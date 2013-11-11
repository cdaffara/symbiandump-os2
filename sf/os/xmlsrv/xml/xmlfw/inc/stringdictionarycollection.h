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

#ifndef __STRINGDICTIONARYCOLLECTION_H__
#define __STRINGDICTIONARYCOLLECTION_H__

#include <e32base.h>

class RStringPool;
class RString;
class TDesC8;

namespace Xml
{

class MStringDictionary;
class RStringDictionaryCollectionImpl;


class RStringDictionaryCollection
/**
The RStringDictionaryCollection class holds a collection of Dictionaries requested by the user.

@publishedAll
@released
*/
	{
public:
	IMPORT_C void OpenL();
	IMPORT_C void Close();

	IMPORT_C RStringDictionaryCollection();

	IMPORT_C void OpenDictionaryL(const TDesC8& aDictionaryDescription);

	IMPORT_C RStringPool& StringPool();

	IMPORT_C MStringDictionary& CurrentDictionaryL() const;
		
private:

	RStringDictionaryCollection(const RStringDictionaryCollection& aOriginal);
	RStringDictionaryCollection& operator=(const RStringDictionaryCollection& aRhs);
	void CreateImplL();

private:

/**
Hide the implementation details.
*/
	RStringDictionaryCollectionImpl*	iImpl;
	};

}

#endif //__STRINGDICTIONARYCOLLECTION_H__
