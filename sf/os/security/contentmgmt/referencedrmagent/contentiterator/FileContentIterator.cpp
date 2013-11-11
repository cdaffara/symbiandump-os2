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
#include "FileContentIterator.h"
#include "FileContentIteratorBase.h"

using namespace ContentAccess;

CFileContentIterator* CFileContentIterator::NewL(const TDesC& aPath, TBool aRecursive, const TDesC8& aMimeType)
	{
	CFileContentIterator* self = new (ELeave) CFileContentIterator();
	CleanupStack::PushL(self);
	self->ConstructL(aPath, aRecursive, aMimeType);
	CleanupStack::Pop(self);
	return self;
	}

void CFileContentIterator::ConstructL(const TDesC& aPath, TBool aRecursive, const TDesC8& aMimeType)
	{
	if(aPath.Length() == 0)
		{
		User::Leave(KErrPathNotFound);
		}

	iPath = aPath.AllocL();
	iRecursive = aRecursive;
	iMimeType = aMimeType.AllocL();

	iManager = CManager::NewL();

	// Find the first content object
	iSubIterator = CFileContentIteratorBase::NewL(*iManager, *iPath, iRecursive, *iMimeType);
	}

CFileContentIterator::CFileContentIterator()
	{

	}
CFileContentIterator::~CFileContentIterator()
	{
	delete iSubIterator;
	delete iManager;
	delete iPath;
	delete iMimeType;
	}	


const TDesC& CFileContentIterator::FileName() const
	{
	return iSubIterator->FileName();
	}

const TDesC& CFileContentIterator::UniqueId() const
	{
	return iSubIterator->UniqueId();
	}

const TDesC& CFileContentIterator::Name() const
	{
	return iSubIterator->Name();
	}

const TDesC8& CFileContentIterator::MimeType() const
	{
	return iSubIterator->MimeType();
	}

TInt CFileContentIterator::Next()		
	{
	return iSubIterator->Next();
	}

