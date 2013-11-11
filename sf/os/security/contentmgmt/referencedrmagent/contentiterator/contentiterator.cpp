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



#include "cafpanic.h"
#include "contentIterator.h"
#include "FileContentIterator.h"
#include "contentiteratordata.h"

using ContentAccess::CContentIterator;
using ContentAccess::TVirtualPathPtr;

_LIT(KIteratorThread,"ContentIterator");
EXPORT_C CContentIterator* CContentIterator::NewL(const TDesC& aPath, TBool aRecursive, const TDesC8& aMimeType) 
	{  
	CContentIterator* self = new (ELeave) CContentIterator();
	CleanupStack::PushL(self);
	self->ConstructL(aPath, aRecursive, aMimeType);
	CleanupStack::Pop();
	return self;
	}

CContentIterator::CContentIterator() : CActive(EPriorityStandard)
	{
	}

CContentIterator::~CContentIterator()
	{
	// tell thread to cancel and shutdown
	Cancel();
	
	// close thread handle
	iWorkerThread.Close();
	delete info; 
	}

void CContentIterator::ConstructL(const TDesC& aPath, TBool aRecursive, const TDesC8& aMimeType)
	{	
	// This data buffer will be shared between the client and the worker thread
	info = CContentIteratorData::NewL(aPath,aRecursive,aMimeType);
	
	// create the thread, need a big heap and stack for recursively searching through directories
	User::LeaveIfError(iWorkerThread.Create(KIteratorThread(),CContentIterator::ThreadEntry,32768, KMinHeapSize, 131072, (void *) info , EOwnerProcess));

	// add ourselves to active scheduler
	CActiveScheduler::Add(this);

	// Set up notification in case the thread panics etc
	iStatus = KRequestPending;
	iWorkerThread.Logon(iStatus);
	SetActive();
	
	// start the thread
	iWorkerThread.Resume();
	}


void CContentIterator::DoCancel()
	{
	// Wait until thread finishes whatever it's doing
	info->Lock();
	 
	// Signal for the thread to close
	info->RunThreadFunction(EIteratorShutdownThread);
	}

void CContentIterator::RunL()
	{
	// Thread must have completed
	if(iWorkerThread.ExitType() == EExitPanic)
		{
		// Thread panicd, better panic our thread
		User::Panic(KCafPanicString, ECafPanicContentIteratorThreadPanic);
		}
	}

EXPORT_C TVirtualPathPtr CContentIterator::VirtualPath()
	{
	// Wait until thread finishes whatever it's doing
	info->Lock(); 
	iFileName.Copy(info->Path());	
	iUniqueId.Copy(info->UniqueId());	
	info->Unlock(); 
	return TVirtualPathPtr(iFileName, iUniqueId);
	}

EXPORT_C const TDesC&  CContentIterator::Name()
	{
	// Wait until thread finishes whatever it's doing
	info->Lock(); 
	iName.Copy(info->Name());	
	info->Unlock(); 
	return iName;
	}

EXPORT_C const TDesC8& CContentIterator::MimeType()
	{
	// Wait until thread finishes whatever it's doing
	info->Lock(); 
	iMimeType.Copy(info->MimeType());
	info->Unlock(); 
	return iMimeType;
	}

EXPORT_C void CContentIterator::Next(TRequestStatus &aStatus)
	{
	// Wait until thread finishes whatever it's doing
	info->Lock(); 

	// Remember which thread and TRequestStatus to notify
	TThreadId id = RThread().Id();
	info->SetClientRequest(id, aStatus);
	
	// Tell it to find the next iteration
	info->RunThreadFunction(EIteratorFindNextContentObject); 
	}
	

TInt CContentIterator::ThreadEntry(TAny* aAny) 
	{
	TBool exitNow = EFalse;
	TInt err;
	CFileContentIterator* iterator = NULL;
	RThread clientThread;
	CContentIteratorData* info = reinterpret_cast <CContentIteratorData *> (aAny);
	
	// create a trap handler
	CTrapCleanup* cleanup = CTrapCleanup::New();
	
	while(!exitNow)
		{
		// Thread will wait here until signaled by other CContentIterator functions
		switch(info->ThreadWait())
			{
		// Client thread has asked us to shutdown, exit loop
		case EIteratorShutdownThread:
			exitNow = ETrue;
			break;
		// Client thread is asking us to find the next object	
		case EIteratorFindNextContentObject:
			if(!iterator)
				{
				TRAP(err, iterator = CFileContentIterator::NewL(info->Path(), info->IsRecursive(), info->MimeType()));
				info->SetData(KNullDesC(), KNullDesC(), KNullDesC(), KNullDesC8());
				}
			else
				{
				err = iterator->Next();
				}
	
			if(err == KErrNone)
				{
				// If a content object was found, write the value back 
				// into the other thread while the info is still locked
				info->SetData(iterator->FileName(),iterator->UniqueId(), iterator->Name(), iterator->MimeType());
				}

			// Complete the clients asynchronous request
			info->CompleteClientRequest(err);
			break;
		default:
			User::Panic(KCafPanicString, ECafPanicContentIteratorUnknownRequest);
			break;
			};

		// Allow the client to post a new request
		info->Unlock(); 
		}
	delete iterator;
	delete cleanup;
	return KErrNone;
	}

