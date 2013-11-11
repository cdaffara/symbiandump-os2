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
#include <stringpool.h>

#include <xml/stringdictionarycollection.h>
#include <xml/xmlframeworkconstants.h>
#include <xml/xmlframeworkerrors.h>
#include "XmlFrameworkPanics.h"
#include <xml/plugins/stringdictionary.h>

#include "stringdictionarycollectionimpl.h"

using namespace Xml;

void RStringDictionaryCollectionImpl::DestroyMapping(TAny* aPtr)
/**
Tidy memory associated with this object.

@param				aPtr The StringDictionary pointer.

*/
	{
	TDtorMapping* ptr = static_cast<TDtorMapping*>(aPtr);
	 
	if (ptr->iDtorKey != KNullUid && ptr->iStringDictionary)
		{
		// Tell ECom to destroy its plugin details
		REComSession::DestroyedImplementation(ptr->iDtorKey);

		// Free memory
		ptr->iStringDictionary->Release();
		}

	delete (ptr);
	}



RStringDictionaryCollectionImpl::RStringDictionaryCollectionImpl()
/**
Default constructor

@post				This object is properly constructed.

*/
:	iRefCount(0),
	iStringDictionary(NULL)
	{
	// do nothing;
	}



void RStringDictionaryCollectionImpl::OpenL()
/**
This method opens this resource incrementing the reference count.
It must be the first method called after construction.

@post				The object is ready to be used.

*/
	{
	++iRefCount;

	if (iRefCount == 1)
		{
		// Need to inc before just in case the Open leaves.
		// In this case a subsequent Close will panic if the count is dec below zero.
		iStringPool.OpenL();
		}
	}


TInt RStringDictionaryCollectionImpl::Close()
/**
This method cleans up the object before destruction. It releases all resources in
accordance to the R Class pattern.

@post				This object may be allowed to go out of scope.

*/
	{
	if (iRefCount==1)
		{
		TInt count = iDtorKeyAndDictionaryList.Count();

		while (count)
			{
			DestroyMapping(iDtorKeyAndDictionaryList[--count]);
			}

		iDtorKeyAndDictionaryList.Reset();

		iStringPool.Close();

		REComSession::FinalClose();
		}

	--iRefCount;
	__ASSERT_ALWAYS(iRefCount >= 0, Panic(EXmlFrameworkPanicReferenceCountNegative));
	
	return iRefCount;
	}


void RStringDictionaryCollectionImpl::OpenDictionaryL(const TDesC8& aDictionaryDescription)
/**
This method loads the Dictionary.

@pre				Connect has been called.
@post				The Dictionary has been loaded.

@param				aDictionaryDescription The Namepspace MIME type.
*/
	{
	if (aDictionaryDescription.Size() == 0)
		{
		// No Mime type provided.
		// Dictionary not required.
		iStringDictionary = NULL;
		return;
		}

	// Implies a Dictionary exists

	RString nsUri = iStringPool.OpenStringL(aDictionaryDescription);
	CleanupClosePushL(nsUri);

	if (LocateDictionary(nsUri, iStringDictionary) == EFalse)
		{
		// Locate the plugin
		iStringDictionary = ConstructDictionaryL(aDictionaryDescription, iStringPool);
		}

	CleanupStack::PopAndDestroy(&nsUri);
	}



TBool RStringDictionaryCollectionImpl::LocateDictionary(const RString& aNsUri, MStringDictionary*& aStringDictionary)
/**
This method locates an existing loaded Dictionary by it uri name.

@return				EFalse if the Dictionary cannot be located, ETrue if it has.

@param				aNsUri The Dictionary uri.
@param				aStringDictionary On return, points to the requested Dictionary.
*/
	{
	TBool found = EFalse;
	TUint count = iDtorKeyAndDictionaryList.Count();

	while (!found && count>0)
		{
		// Check if its been loaded already
		if (iDtorKeyAndDictionaryList[--count]->iStringDictionary->CompareThisDictionary(aNsUri))
			{
			aStringDictionary = iDtorKeyAndDictionaryList[count]->iStringDictionary;
			found = ETrue;
			}
		}
	return found;
	}



RStringPool& RStringDictionaryCollectionImpl::StringPool()
/**
This method obtains a handle to the RStringPool

@return				the RStringPool

*/
	{
	return iStringPool;
	}


MStringDictionary& RStringDictionaryCollectionImpl::CurrentDictionaryL() const
/**
This method obtains the current string dictionary in use.
Also, serves as a way to test if any dictionaries have been loaded as none needs be.

@return				the current Dictionary in use, if any.
@leave 				KErrXmlMissingStringDictionary if there is no dictionary

*/
	{
	if (!iStringDictionary)
		{
		User::Leave(KErrXmlMissingStringDictionary);
		}

	return *iStringDictionary;
	}



MStringDictionary* RStringDictionaryCollectionImpl::ConstructDictionaryL(const TDesC8& aDictionaryUri, 
															RStringPool& aStringPool)
/**
This method constructs a StringDictionary.

@return				A pointer to the String Dictionary plugin found.
@leave				KErrXmlDictionaryPluginNotFound If ECom fails to find 
					the object a leave occurs.

@param				aDictionaryUri The uri of this Dictionary.
@param				aStringPool Contains the RTable for this Dictionary
*/
	{
	TDtorMapping* mapping = new(ELeave) TDtorMapping;
	mapping->iDtorKey = KNullUid;
	mapping->iStringDictionary = NULL;
	
	CleanupStack::PushL(TCleanupItem(DestroyMapping, mapping));

	// Set resolving parameters to find a plug-in with a matching Dictionary URI
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(aDictionaryUri);
	resolverParams.SetWildcardMatch(ETrue);

	TAny* any = NULL;
	TRAPD(err, any = REComSession::CreateImplementationL(KStringDictionaryInterfaceUid, 
														 mapping->iDtorKey, 
														 &aStringPool, 
														 resolverParams));
													
	if (err != KErrNone)
		{
		if (err == KErrNotFound)
			{
			User::Leave(KErrXmlStringDictionaryPluginNotFound);
			}

		User::Leave(err);
		}

	mapping->iStringDictionary = static_cast<MStringDictionary*>(any);
	
	User::LeaveIfError(iDtorKeyAndDictionaryList.Append(mapping));

	CleanupStack::Pop(mapping);
	return (mapping->iStringDictionary); 
	}
