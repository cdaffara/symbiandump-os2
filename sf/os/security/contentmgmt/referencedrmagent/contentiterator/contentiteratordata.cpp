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


#include "contentiteratordata.h"

using namespace ContentAccess;

CContentIteratorData* CContentIteratorData::NewL(const TDesC& aSearchPath, TBool aRecursive, const TDesC8& aMimeType)
	{
	CContentIteratorData* self = new (ELeave) CContentIteratorData();
	CleanupStack::PushL(self);
	self->ConstructL(aSearchPath, aRecursive, aMimeType);
	CleanupStack::Pop(self);
	return self;
	}
	
CContentIteratorData::CContentIteratorData()
	{
	}
	
CContentIteratorData::~CContentIteratorData()
	{
	// release semaphore 
	iThreadSemaphore.Signal();
	
	iDataLockSemaphore.Close();
	iThreadSemaphore.Close();
	}

void CContentIteratorData::ConstructL(const TDesC& aSearchPath, TBool aRecursive, const TDesC8& aMimeType)
	{
	// Remember search parameters, the CContentIterator::ThreadEntry() 
	// will ask for these later
	iPath.Copy(aSearchPath);
	iRecursive = aRecursive;
	iMimeType.Copy(aMimeType);
	
	
	// Create semaphores
	User::LeaveIfError(iDataLockSemaphore.CreateLocal(1, EOwnerProcess));	
	User::LeaveIfError(iThreadSemaphore.CreateLocal(1, EOwnerProcess));
	
	// Grab semaphore so CContentIterator::ThreadEntry function can't enter the loop yet
	iThreadSemaphore.Wait();
	}

				
void CContentIteratorData::Lock()
	{
	iDataLockSemaphore.Wait();
	}

void CContentIteratorData::Unlock()
	{
	iDataLockSemaphore.Signal();
	}
		
void CContentIteratorData::CompleteClientRequest(TInt aError)
	{
	RThread clientThread;
	clientThread.Open(iClientThreadId);
	clientThread.RequestComplete(iClientRequest, aError);
	clientThread.Close();
	}
	
	
void CContentIteratorData::SetClientRequest(TThreadId& aClientThreadId, TRequestStatus& aStatus)
	{
	// Remember the thread and TRequestStatus that must be completed later
	iClientThreadId = aClientThreadId;
	iClientRequest = &aStatus;
	}
	
void CContentIteratorData::RunThreadFunction(TFileIteratorOperation aFunction)
	{
	// Unblock the CContentIterator::ThreadEntry() function and allow it to run
	iFunction = aFunction;
	iThreadSemaphore.Signal();
	}

TFileIteratorOperation CContentIteratorData::ThreadWait()
	{
	// Called by CContentIterator::ThreadEntry() to block execution until RunThreadFunction() is called
	iThreadSemaphore.Wait();
	return iFunction;
	}

		
void CContentIteratorData::SetData(const TDesC& aPath, const TDesC& aUniqueId, const TDesC& aName, const TDesC8& aMimeType)
	{
	// Set the results found in the search
	iPath.Copy(aPath);
	iMimeType.Copy(aMimeType);
	iUniqueId.Copy(aUniqueId);
	iName.Copy(aName);
	}

const TDesC& CContentIteratorData::Path() const
	{
	return iPath; 
	}

const TDesC8& CContentIteratorData::MimeType() const
	{
	return iMimeType; 
	}
	
const TDesC& CContentIteratorData::UniqueId() const
	{
	return iUniqueId;
	}

const TDesC& CContentIteratorData::Name() const
	{
	return iName;
	}
	
TBool CContentIteratorData::IsRecursive() const
	{
	return iRecursive;	
	}
