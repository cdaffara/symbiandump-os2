/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements the UPS database handle manager.	See class and function
* definitions for more information.
*
*/


/**
 @file
*/
#include <f32file.h>
#include "upscommon.h"
#include "policycache.h"
#include "upspolicycachehandle.h"

namespace UserPromptService
{

NONSHARABLE_CLASS(CPolicyCacheContainer) : public CBase
	{
public:
	static CPolicyCacheContainer *NewL(RFs &aFs);

	void IncrementReferenceCount();
	void DecrementReferenceCount();
	inline CPolicyCache *PolicyCache();

	TInt ReferenceCount() const;

	void NotifyOnRef1(TRequestStatus &aStatus);
	void CancelNotifyOnRef1();

private:
	void ConstructL(RFs &aFs);
	~CPolicyCacheContainer();
	
	TInt iReferenceCount;
	CPolicyCache *iPolicyCache;
	TRequestStatus *iClientRequest;
	};

inline CPolicyCache *CPolicyCacheContainer::PolicyCache()
	{
	ASSERT(iPolicyCache != 0);
	return iPolicyCache;
	}

inline TInt CPolicyCacheContainer::ReferenceCount() const
	{
	return iReferenceCount;
	}

RPolicyCacheCountedHandle::RPolicyCacheCountedHandle(RFs &aFs)
	: iFs(aFs), iContainer(0)
	{
	}

RPolicyCacheCountedHandle::RPolicyCacheCountedHandle(RPolicyCacheCountedHandle &aPolicyCacheManager)
	: iFs(aPolicyCacheManager.iFs), iContainer(0)
	{
	*this = aPolicyCacheManager;
	}

RPolicyCacheCountedHandle &RPolicyCacheCountedHandle::operator=(const RPolicyCacheCountedHandle &aRhs)
	{
	if(this == &aRhs) return *this;
	
	Release();

	if(aRhs.iContainer)
		{
		iContainer = aRhs.iContainer;
		iContainer->IncrementReferenceCount();
		}
	
	return *this;
	}

RPolicyCacheCountedHandle::~RPolicyCacheCountedHandle()
/**
	Destructor - make sure cache container is released.
*/
	{
	Release();
	}

void RPolicyCacheCountedHandle::OpenL()
/**
	Create/Open a new policy cache. If this manager is already opened then the existing cache is
	released first (see Release()).
*/
	{
	// First release any existing container/policy cache.
	Release();

	// Now create a new one
	iContainer = CPolicyCacheContainer::NewL(iFs);
	}

void RPolicyCacheCountedHandle::Release()
/**
	Release the current policy cache container. If this decreases its reference count to 0, it will be
	deleted.
*/
	{
	if(iContainer)
		{
		iContainer->DecrementReferenceCount();
		iContainer = 0;
		}
	}

TBool RPolicyCacheCountedHandle::IsOpen() const
	{
	return iContainer != 0;
	}

CPolicyCache *RPolicyCacheCountedHandle::operator->()
/**
	Returns the CPolicyCache ptr so -> can be used to call policy cache functions.

	If the class is not already open, then we will attempt to open ourself using OpenL().

	Note that this operator can leave...
*/
	{
	if(iContainer == 0)
		{
		OpenL();
		ASSERT(iContainer);
		}
	return iContainer->PolicyCache();
	}

void RPolicyCacheCountedHandle::NotifyOnRef1(TRequestStatus &aStatus)
/**
	Register for notifcation when the underlying CPolicyCacheContainer reference
	count reaches 1, presumably when the client holds the only reference.

	Only one client is allowed to register against a single CPolicyCacheContainer.
*/
	{
	if((iContainer == 0) || (iContainer->ReferenceCount() <= 1))
		{
		// No container or ref count is 1, so complete now.
		TRequestStatus *rs = &aStatus;
		*rs = KRequestPending;
		User::RequestComplete(rs, KErrNone);
		return;
		}
	iContainer->NotifyOnRef1(aStatus);
	}

void RPolicyCacheCountedHandle::CancelNotifyOnRef1()
/**
	Cancel the call to NotifyOnRef1. The request will be completed immediately.
*/
	{
	if(!iContainer)
		{
		return;
		}
	iContainer->CancelNotifyOnRef1();
	}

CPolicyCacheContainer *CPolicyCacheContainer::NewL(RFs &aFs)
/**
	Create a CPolicyCacheContainer containing a CPolicyCache with a reference count of 1.
*/
	{
	CPolicyCacheContainer *self = new(ELeave) CPolicyCacheContainer();
	CleanupStack::PushL(self);
	self->ConstructL(aFs);
	CleanupStack::Pop(self);
	return self;
	}

void CPolicyCacheContainer::IncrementReferenceCount()
/**
	Increment the reference count.
*/
	{
	ASSERT(iReferenceCount > 0);
	++iReferenceCount;
	}

void CPolicyCacheContainer::DecrementReferenceCount()
/**
	Decrement the reference count and delete self if it reaches 0.
*/
	{
	--iReferenceCount;
	if(iReferenceCount <= 1)
		{
		if(iClientRequest && iClientRequest->Int() == KRequestPending)
			{
			User::RequestComplete(iClientRequest, KErrNone);
			}
		}

	if(iReferenceCount <= 0)
		{
		delete this;
		}
	}

void CPolicyCacheContainer::NotifyOnRef1(TRequestStatus &aStatus)
/**
	Register for notifcation when the reference count reduces to 1, presumably 
	when the client holds the only reference.

	Only one client is allowed to register against a single CPolicyCacheContainer.
*/
	{
	BULLSEYE_OFF
	if(iClientRequest != 0)
		{
		TRequestStatus *rs = &aStatus;
		*rs = KRequestPending;
		User::RequestComplete(rs, KErrServerBusy);
		return;
		}
	BULLSEYE_RESTORE

	// Initialise client request
	iClientRequest = &aStatus;
	*iClientRequest = KRequestPending;

	// Are we already at ref 1?
	if(iReferenceCount <= 1)
		{
		User::RequestComplete(iClientRequest, KErrNone);
		}
	}

void CPolicyCacheContainer::CancelNotifyOnRef1()
/**
	Cancel the previous call to NotifyOnRef1.
*/
	{
	if((iClientRequest != 0) && (iClientRequest->Int() == KRequestPending))
		{
		User::RequestComplete(iClientRequest, KErrCancel);
		return;
		}
	}	

void CPolicyCacheContainer::ConstructL(RFs &aFs)
	{
	_LIT(KPolicyDir,"\\private\\10283558\\policies\\");
	iPolicyCache = CPolicyCache::NewL(aFs, KPolicyDir());
	iReferenceCount = 1;
	}

CPolicyCacheContainer::~CPolicyCacheContainer()
	{
	delete iPolicyCache;
	iPolicyCache = 0;

	if(iClientRequest && iClientRequest->Int() == KRequestPending)
		{
		User::RequestComplete(iClientRequest, KErrNone);
		iClientRequest = 0;
		}
	}

} // End of UserPromptService namespace

// End of file

