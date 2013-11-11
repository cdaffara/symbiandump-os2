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


#include "drmfiles.h"
#include "virtualFile.h"
#include "drmfilecontainer.h"
#include "drmfilecontent.h"
#include "reftestagentconstants.h"
#include <caf/caftypes.h>

using namespace ReferenceTestAgent;
using namespace ContentAccess;

CDrmFiles* CDrmFiles::NewL()
	{
	CDrmFiles* self = new (ELeave) CDrmFiles;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CDrmFiles::CDrmFiles()
	{
	}

void CDrmFiles::ConstructL()
	{
	// create the root container which holds all other containers and content
	iRootContainer = CDrmFileContainer::NewL(NULL);
	iCurrentContainer = iRootContainer;

	// set up "entire file" content object
	RAttributeSet attributeSet;
	CleanupClosePushL(attributeSet);
	
	RStringAttributeSet stringAttributeSet;
	CleanupClosePushL(stringAttributeSet);
	
	attributeSet.AddL(EIsForwardable);
	attributeSet.AddL(EIsProtected);
	
	// set some entire file default attributes 
	// TO DO - the other attributes
	User::LeaveIfError(attributeSet.SetValue(EIsForwardable, ETrue, KErrNone));
	User::LeaveIfError(attributeSet.SetValue(EIsProtected, EFalse, KErrNone));
	
	TBuf <KMaxDataTypeLength> mimeType;
	mimeType.Copy(KRtaMimeContent());
	stringAttributeSet.AddL(EMimeType);
	User::LeaveIfError(stringAttributeSet.SetValue(EMimeType, mimeType, KErrNone));
	
	iEntireFile = CDrmFileContent::NewL(KNullDesC8(),KNullDesC16(), KNullDesC16(), KNullDesC16(), attributeSet, stringAttributeSet);;
	CleanupStack::PopAndDestroy(2, &attributeSet); // stringAttributeSet
	}
	
void CDrmFiles::UpdateContentL()
	{
	// build a quick access index list of all content within the file
	iRootContainer->ListAllContentL(iContentArray);
	}

CDrmFiles::~CDrmFiles()
	{
	// Do not ResetAndDestroy()
	// CDrmFilesContent objects are owned by CDrmFilesContainer objects
	iContentArray.Reset();
	
	delete iRootContainer;
	
	delete iEntireFile;
	}

CDrmFileContainer& CDrmFiles::RootContainer()
	{
	return *iRootContainer;
	}

EXPORT_C CDrmFileContainer& CDrmFiles::CurrentContainer()
	{
	return *iCurrentContainer;
	}

EXPORT_C TInt CDrmFiles::OpenContainer(const TDesC& aUniqueId)
	{
	TInt err = KErrNotFound;
	// look for the container within our current container
	for(TInt i = 0; i < iCurrentContainer->Containers().Count(); i++)
		{
		if(iCurrentContainer->Containers()[i]->UniqueId() == aUniqueId)
			{
			iCurrentContainer = iCurrentContainer->Containers()[i];
			err = KErrNone;
			break;
			}
		}
	return err;
	}

EXPORT_C TInt CDrmFiles::CloseContainer()
	{
	if(iCurrentContainer->ParentContainer() != NULL)
		{
		// Go up a level
		iCurrentContainer = iCurrentContainer->ParentContainer();
		return KErrNone;
		}
	else
		{
		// Must already be at the root container
		return KErrNotFound;
		}
	}

EXPORT_C CDrmFileContent& CDrmFiles::FindL(const TDesC& aUniqueId)
	{
	TBool found = EFalse;
	TInt i = 0;
	
	if(aUniqueId == KDefaultContentObject() && iContentArray.Count() == 1)
		{
		// There is only one content object present in this file
		// return that as the default content object
		found = ETrue;
		}
	else if(aUniqueId.Length() == 0)
		{
		// entire file object, uniqueId is ""
		return *iEntireFile;
		}
	else
		{
		for(i = 0; i < iContentArray.Count(); i++)
			{
			if(iContentArray[i]->UniqueId() == aUniqueId)
				{
				found = ETrue;
				break;
				}
			}
		}
	
	if(!found)
		{
		User::Leave(KErrNotFound);	
		}
	return *iContentArray[i];
	}

EXPORT_C CDrmFileContent& CDrmFiles::FindL(const TDesC8& aCid)
	{
	TBool found = EFalse;
	TInt i = 0;
	for(i= 0; i < iContentArray.Count(); i++)
		{
		if(iContentArray[i]->Cid() == aCid)
			{
			found = ETrue;
			break;
			}
		}

	
	if(!found)
		{
		if(aCid.Length() == 0)
			{
			// entire file object
			return *iEntireFile;
			}
		else
			{
			User::Leave(KErrNotFound);		
			}
		}
	return *iContentArray[i];
	}


EXPORT_C CDrmFileContent& CDrmFiles::Content(TInt aIndex)
	{
	return *iContentArray[aIndex];	
	}
	
EXPORT_C TInt CDrmFiles::ContentCount()
	{
	return iContentArray.Count();
	}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
EXPORT_C void CDrmFiles::SetEntireFileL(RFile64& aFile, RSemaphore& aFileLock, TInt64 aOffset, TInt64 aSize)
#else
EXPORT_C void CDrmFiles::SetEntireFileL(RFile& aFile, RSemaphore& aFileLock, TInt aOffset, TInt aSize)
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	{
	iEntireFile->SetCommonSourceFileL(aFile, aFileLock, aOffset, aSize);	
	}


