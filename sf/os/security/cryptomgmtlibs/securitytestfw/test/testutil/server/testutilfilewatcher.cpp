/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* TestUtil - filewatcher implementation
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#include "testutilfilewatcher.h"

// CFileWatcher

/*static*/ CFileWatcher* CFileWatcher::NewL(RFs& aFs, const RMessagePtr2& aMessage)
	{
	CFileWatcher* self=NewLC(aFs, aMessage);
	CleanupStack::Pop(self);
	return self;
	}
	
/*static*/ CFileWatcher* CFileWatcher::NewLC(RFs& aFs, const RMessagePtr2& aMessage)
	{
	CFileWatcher* self=new(ELeave) CFileWatcher(aFs, aMessage);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}
	
CFileWatcher::~CFileWatcher()
	{
	Cancel();
	}

CFileWatcher::CFileWatcher(RFs& aFs, const RMessagePtr2& aMessage)
	: CActive(EPriorityHigh), iFs(aFs), iMessage(aMessage)
	{
	CActiveScheduler::Add(this);
	}

void CFileWatcher::ConstructL()
	{
	TInt srcLen = iMessage.GetDesLength(0);
	HBufC* fileName=HBufC::NewLC(srcLen);
	TPtr ptr(fileName->Des());
	TRAPD(err, iMessage.ReadL(0, ptr));
	if (err!=KErrNone)
		{
		iMessage.Complete(KErrBadDescriptor);
		}
	else
		{
		iFs.NotifyChange(ENotifyFile, iStatus, *fileName);
		SetActive();	
		}
	CleanupStack::PopAndDestroy(fileName);
	}

void CFileWatcher::DoCancel()
	{
	iFs.NotifyChangeCancel(iStatus);
	iMessage.Complete(KErrCancel);
	}

void CFileWatcher::RunL()
	{
	iMessage.Complete(iStatus.Int());
	}

