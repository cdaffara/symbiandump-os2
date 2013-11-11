/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <caf/caftypes.h>
#include <caf/embeddedobject.h>
#include "drmfilecontainer.h"
#include "drmfilecontent.h"

using namespace ReferenceTestAgent;
using namespace ContentAccess;

CDrmFileContainer* CDrmFileContainer::NewL(CDrmFileContainer* aParent)
	{
	CDrmFileContainer* self = new (ELeave) CDrmFileContainer(aParent);
	return self;
	}

CDrmFileContainer::CDrmFileContainer(CDrmFileContainer* aParent) : iParentContainer(aParent)
	{
	}
	
CDrmFileContainer::~CDrmFileContainer()
	{
	delete iName;
	delete iUniqueId;
	iContainers.ResetAndDestroy();
	iContainers.Close();
	iContents.ResetAndDestroy();
	iContents.Close();
	}
		

EXPORT_C void CDrmFileContainer::GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& aArray)
	{
	GetEmbeddedObjectsL(aArray, EContainerObject);
	GetEmbeddedObjectsL(aArray, EContentObject);
	}
	
EXPORT_C void CDrmFileContainer::GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& aArray, TEmbeddedType aType)
	{
	if(aType == EContentObject)
		{
		for(TInt i = 0; i < iContents.Count(); i++)
			{
			CDrmFileContent* content = iContents[i];
			CEmbeddedObject* object = CEmbeddedObject::NewL(content->UniqueId(), content->Name(), content->MimeType(), EContentObject);
			CleanupStack::PushL(object);
			aArray.AppendL(object);
			CleanupStack::Pop(object);
			}
		
		}
	else if(aType == EContainerObject)
		{
		for(TInt i = 0; i < iContainers.Count(); i++)
			{
			CDrmFileContainer* container = iContainers[i];
			CEmbeddedObject* object = CEmbeddedObject::NewL(container->UniqueId(), container->Name(), KNullDesC8(), EContainerObject);
			CleanupStack::PushL(object);
			aArray.AppendL(object);
			CleanupStack::Pop(object);
			}
		}
	}
	
	
EXPORT_C TInt CDrmFileContainer::Search(RStreamablePtrArray<CEmbeddedObject>& aArray, const TDesC8& aMimeType, TBool aRecursive)
	{
	TRAPD( err, SearchL(aArray, aMimeType, aRecursive) );
	return err;
	}

void CDrmFileContainer::SearchL(RStreamablePtrArray<CEmbeddedObject>& aArray, const TDesC8& aMimeType, TBool aRecursive)
	{
	TInt i = 0;
	TInt searchResult = KErrNotFound;

	for(i = 0; i < iContents.Count(); i++)
		{
		CDrmFileContent* content = iContents[i];
		
		// Only add contents if the mime type matches the type we are searching for
		if(content->MimeType().CompareF(aMimeType) == 0)
			{
			CEmbeddedObject* object = CEmbeddedObject::NewL(content->UniqueId(), content->Name(), content->MimeType(), EContentObject);
			CleanupStack::PushL(object);
			aArray.AppendL(object);
			CleanupStack::Pop(object);

			// At least one content object has been found
			searchResult = KErrNone;
			}
		}

	if(aRecursive)
		{
		// Search within embedded containers
		TInt err;
		for(i = 0; i < iContainers.Count(); i++)
			{
			err = iContainers[i]->Search(aArray, aMimeType, aRecursive);
			if(err != KErrNotFound)
				{
				if(err == KErrNone)
					{
					// at least one object has been found
					searchResult = KErrNone;
					}
				else
					{
					// some other unexpected error occurred
					User::Leave(err);
					}
				}
			}
		}
	User::LeaveIfError(searchResult);
	}
	

CDrmFileContainer* CDrmFileContainer::ParentContainer()
	{
	return iParentContainer;
	}
	
RPointerArray <CDrmFileContainer>& CDrmFileContainer::Containers()
	{
	return iContainers;
	}

RPointerArray <CDrmFileContent>& CDrmFileContainer::Contents()
	{
	return iContents;
	}

void CDrmFileContainer::ListAllContentL(RPointerArray <CDrmFileContent>& aContentArray)
	{
	TInt i;
	// add content from this container
	for(i = 0; i < iContents.Count(); i++)
		{
		User::LeaveIfError(aContentArray.Append(iContents[i]));		
		}
		
	// traverse embedded containers
	for(i = 0; i < iContainers.Count(); i++)
		{
		iContainers[i]->ListAllContentL(aContentArray);
		}
	}

const TDesC& CDrmFileContainer::UniqueId()
	{
	return *iUniqueId;
	}

const TDesC& CDrmFileContainer::Name()
	{
	return *iName;
	}
		
CDrmFileContainer& CDrmFileContainer::AddContainerL()
	{
	CDrmFileContainer *newContainer = CDrmFileContainer::NewL(this);
	CleanupStack::PushL(newContainer);
	User::LeaveIfError(iContainers.Append(newContainer));
	CleanupStack::Pop(newContainer);
	return *(iContainers[iContainers.Count() - 1]);
	}
	

void CDrmFileContainer::SetNameL(const TDesC& aName)
	{
	if(iName)
		{
		User::Leave(KErrCorrupt);
		}
	iName = aName.AllocL();
	}
	
void CDrmFileContainer::SetUniqueIdL(const TDesC& aUniqueId )
	{
	if(iUniqueId)
		{
		User::Leave(KErrCorrupt);
		}
	iUniqueId= aUniqueId.AllocL();
	}
	
void CDrmFileContainer::AddContentL(const TDesC8& aCid, const TDesC& aName, const TDesC& aUniqueId, const TDesC& aFileName, RAttributeSet& aAttributeSet, RStringAttributeSet& aStringAttributeSet)
	{
	CDrmFileContent *newContent = CDrmFileContent::NewL(aCid, aName, aUniqueId, aFileName, aAttributeSet, aStringAttributeSet);
	CleanupStack::PushL(newContent);
	User::LeaveIfError(iContents.Append(newContent));
	CleanupStack::Pop(newContent);
	}
	
