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

#include <ecom/ecom.h>

#include <xml/stringdictionarycollection.h>

#include "stringdictionarycollectionimpl.h"

using namespace Xml;

EXPORT_C RStringDictionaryCollection::RStringDictionaryCollection()
/**
Default constructor

@post				This object is properly constructed.

*/
:	iImpl(NULL)
	{
	// do nothing;
	}



void RStringDictionaryCollection::CreateImplL()
/**
Create the implementation object.

@pre				The object has just been constructed but not opened.
@post				The object is fully constructed and ready to be used.

*/
	{
	if (!iImpl)
		{
		iImpl = new (ELeave) RStringDictionaryCollectionImpl;
		}
	}



EXPORT_C void RStringDictionaryCollection::OpenL()
/**
This method opens this resource incrementing the reference count.
It must be the first method called after construction.

@post				The object is ready to be used.

*/
	{
	CreateImplL();
	iImpl->OpenL();
	}


EXPORT_C void RStringDictionaryCollection::Close()
/**
This method cleans up the object before destruction. It releases all resources in
accordance to the R Class pattern.

@post				This object may be allowed to go out of scope.

*/
	{
	if (iImpl)
		{
		if (iImpl->Close() == 0)
			{
			delete iImpl;
			iImpl = NULL;
			}
		}
	}


EXPORT_C void RStringDictionaryCollection::OpenDictionaryL(const TDesC8& aDictionaryDescription)
/**
This method loads the Dictionary.

@pre				Connect has been called.
@post				The Dictionary has been loaded.

@param				aDictionaryDescription The Namepspace MIME type.
*/
	{
	iImpl->OpenDictionaryL(aDictionaryDescription);
	}



EXPORT_C RStringPool& RStringDictionaryCollection::StringPool()
/**
This method obtains a handle to the RStringPool

@return				the RStringPool

*/
	{
	return iImpl->StringPool();
	}


EXPORT_C MStringDictionary& RStringDictionaryCollection::CurrentDictionaryL() const
/**
This method obtains the current string dictionary in use.
Also, serves as a way to test if any dictionaries have been loaded as none needs be.

@return				the current Dictionary in use, if any.
@leave 				KErrXmlMissingStringDictionary if there is no dictionary

*/
	{
	return iImpl->CurrentDictionaryL();
	}
