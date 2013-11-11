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
#include "manager.h"
#include "EmbeddedObject.h"
#include "FileContentIteratorBase.h"
#include "EmbeddedcontentIterator.h"

using namespace ContentAccess;

_LIT(KCAFBackSlashCharacter, "\\");


CFileContentIteratorBase* CFileContentIteratorBase::NewL(CManager &aManager, const TDesC& aPath, TBool aRecursive, const TDesC8& aMimeType)
	{
	CFileContentIteratorBase* self = new (ELeave) CFileContentIteratorBase(aManager, aPath, aRecursive, aMimeType);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CFileContentIteratorBase::CFileContentIteratorBase(CManager& aManager, const TDesC& aPath, TBool aRecursive, const TDesC8& aMimeType) :
		iManager(aManager), iPath(aPath), iRecursive(aRecursive), iMimeType(aMimeType)
	{
	iDirIndex = 0;
	iFileIndex = 0;
	}

void CFileContentIteratorBase::ConstructL()
	{
	// Get our list of files and directories
	User::LeaveIfError(iManager.GetDir(iPath, ESortByName, KEntryAttNormal, iCurrentFileList, iCurrentDirectoryList));

	if(!iRecursive)
		{
		// Won't be needing the list of directories
		delete iCurrentDirectoryList;
		iCurrentDirectoryList = NULL;
		}

	// Find the Next,  (ie. first content object) inside this directory
	// This will leave if nothing was found
	User::LeaveIfError(Next());
	}

CFileContentIteratorBase::~CFileContentIteratorBase()
	{
	delete iSubIterator;
	delete iCurrentDirectoryList;
	delete iCurrentFileList;
	delete iNewPath;
	}

const TDesC& CFileContentIteratorBase::FileName() const
	{
	return iSubIterator->FileName();
	}

const TDesC& CFileContentIteratorBase::UniqueId() const
	{
	return iSubIterator->UniqueId();
	}

const TDesC& CFileContentIteratorBase::Name() const
	{
	return iSubIterator->Name();
	}
	
const TDesC8& CFileContentIteratorBase::MimeType() const
	{
	return iSubIterator->MimeType();
	}
		
TInt CFileContentIteratorBase::Next()
	{	
	_LIT(KSysDirEntry, "sys");
	
	TInt ret = KErrNotFound;

	// If we are already looking into some sub container, try finding the next in there
	if(iSubIterator)
		{
		ret = iSubIterator->Next();
		if(ret == KErrNone)
			{
			return ret;
			}
		else 
			{
			// Delete sub-iterator
			delete iSubIterator;
			iSubIterator = NULL;
			delete iNewPath;
			iNewPath = NULL;
			}
		}


	// Search files in our directory first
	// this allows us to free memory used by iCurrentFileList
	// before recursing into sub-directories
	if(iCurrentFileList)
		{
		for ( ; iFileIndex < iCurrentFileList->Count(); iFileIndex++)
			{
			// Get a reference to the file we are interested in
			const TEntry &entry = (*iCurrentFileList)[iFileIndex];
		
			// create a new path string for the sub directory
			TRAP(ret, iNewPath = HBufC::NewL(iPath.Length() + entry.iName.Length()));
			if(ret != KErrNone)
				{
				return ret;
				}
			iNewPath->Des().Append(iPath);
			iNewPath->Des().Append(entry.iName);
		
			// Look inside the file
			TRAP(ret, iSubIterator = CEmbeddedContentIterator::NewL(TVirtualPathPtr(*iNewPath, KNullDesC()), iRecursive, iMimeType));
		
			if(ret == KErrNone)
				{	
				// must have found something in the file
				// make sure next time we look at the next item in our list
				iFileIndex++;
				return KErrNone;
				}
			// next iteration
			delete iNewPath;
			iNewPath = NULL;
			}
		delete iCurrentFileList;
		iCurrentFileList = NULL;
		}


	if(iRecursive)
		{
		// Search sub directories
		for ( ; iDirIndex < iCurrentDirectoryList->Count(); iDirIndex++)
			{
			// Get a reference to the directory we are interested in
			const TEntry &entry = (*iCurrentDirectoryList)[iDirIndex];

			// make sure it's not the sys directory
			if(KSysDirEntry().CompareF(entry.iName) == 0 && iPath.Length() == 3)
				{
				// go to the next iteration, skip the sys directory
				continue;
				}

			// create a new path string for the sub directory
			TRAP(ret, iNewPath = HBufC::NewL(iPath.Length() + 1 + entry.iName.Length()));
			if(ret != KErrNone)
				{
				return ret;
				}
			iNewPath->Des().Append(iPath);
			iNewPath->Des().Append(entry.iName);
			iNewPath->Des().Append(KCAFBackSlashCharacter);
		

			// lets look inside the directory
			TRAP(ret, iSubIterator = CFileContentIteratorBase::NewL(iManager, *iNewPath, iRecursive, iMimeType));

			if(ret == KErrNone)
				{	
				// must have found something in the sub-iterator
				// make sure next time we look at the next item in our list
				iDirIndex++;
				return KErrNone;
				}
			// next iteration
			delete iNewPath;
			iNewPath = NULL;
			}
		}
	
	// reached the end of our list of directories and list of files
	return KErrNotFound;	
	}
