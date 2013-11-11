/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include <f32file.h>
#include <apmstd.h>

#include "contentIterator.h"
#include "virtualpath.h"
#include "content.h"
#include "EmbeddedcontentIteratorBase.h"
#include "EmbeddedObject.h"

using namespace ContentAccess;


CEmbeddedContentIteratorBase* CEmbeddedContentIteratorBase ::NewL(CContent& aContent, TBool aRecursive, const TDesC8& aMimeType)
	{
	CEmbeddedContentIteratorBase* self = new (ELeave) CEmbeddedContentIteratorBase(aContent, aRecursive, aMimeType);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CEmbeddedContentIteratorBase::ConstructL()
	{
	// populate the array with the items at this level of the content file
	iContent.GetEmbeddedObjectsL(iEmbeddedContentObjects, EContentObject);
	iContent.GetEmbeddedObjectsL(iEmbeddedContainerObjects, EContainerObject);

	// Find the Next,  (ie. first content object)
	// This will leave if there is no object
	User::LeaveIfError(Next());
	}

CEmbeddedContentIteratorBase::CEmbeddedContentIteratorBase(CContent& aContent, TBool aRecursive, const TDesC8& aMimeType) 
	: iContent(aContent), iRecursive(aRecursive), iMimeType(aMimeType)
	{
	iContentIndex = 0;
	iContainerIndex = 0;
	}

CEmbeddedContentIteratorBase::~CEmbeddedContentIteratorBase()
	{
	delete iSubIterator;

	// Clear array 
	iEmbeddedContainerObjects.Close();
	iEmbeddedContentObjects.Close();
	}	

CEmbeddedObject& CEmbeddedContentIteratorBase::EmbeddedObject()
	{
	// The next pointer is currently on an object inside the subiterator
	if(iSubIterator)
		{
		return iSubIterator->EmbeddedObject();
		}
	else
		{	
		return *iEmbeddedContentObjects[iContentIndex - 1];
		}
	}
		
TInt CEmbeddedContentIteratorBase::Next()
	{
	// Must figure out if there's a constant for this
	TBuf8 <KMaxDataTypeLength> mimeType;
	TInt ret = KErrNotFound;

	// If we are already looking into a container, try finding the next in there
	if(iSubIterator)
		{
		ret = iSubIterator->Next();
		if(ret == KErrNone)
			{
			return ret;
			}
		else 
			{
			// come back to our level
			iContent.CloseContainer();

			// Delete sub-iterator
			delete iSubIterator;
			iSubIterator = NULL;
			}
		}
	

	// Find content objects inside this container
	for( ; iContentIndex < iEmbeddedContentObjects.Count(); iContentIndex++)
		{
		// if we don't care about mime type, this content object will do
		if(iMimeType.Length() == 0)
			{
			// make sure next time we look at the next item in our array
			iContentIndex++;
			return KErrNone;
			}

		// See if the content object has the right mime type that we are looking for
		mimeType.Copy(iEmbeddedContentObjects[iContentIndex]->MimeType());
		if(iMimeType == mimeType)
			{
			// make sure next time we look at the next item in our array
			iContentIndex++;
			return KErrNone;
			}
		// otherwise continue to next iteration, for loop incrementes iContentIndex		
		}		
	
	// Free memory allocated for content objects
	if(iContentIndex)
		{
		iEmbeddedContentObjects.ResetAndDestroy();
		iContentIndex = 0;
		}

	// Find content objects within nested containers
	for( ; iContainerIndex < iEmbeddedContainerObjects.Count(); iContainerIndex++)
		{
		// If it's a container look inside 
		iContent.OpenContainer(iEmbeddedContainerObjects[iContainerIndex]->UniqueId());
		TRAPD(err, iSubIterator = CEmbeddedContentIteratorBase::NewL(iContent, iRecursive, iMimeType));
		if(err == KErrNone)
			{
			// must have found something inside
			// make sure next time we search at the next item in our array
			iContainerIndex++;
			return KErrNone;
			}
		// otherwise continue to next iteration
		}

	// must be at the end of the array, ie. can't find any more content objects
	return KErrNotFound;
	}

	
