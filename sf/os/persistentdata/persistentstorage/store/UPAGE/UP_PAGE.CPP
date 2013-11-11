// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "UP_STD.H"

#define UNUSED_VAR(a) a = a

const TInt KMemPoolGranularity=16;

EXPORT_C void MPagePool::PushL()
/** Pushes this object onto the cleanup stack. */
	{
	CleanupStack::PushL(TCleanupItem(REINTERPRET_CAST(TCleanupOperation,AcquireL()),this));
		// platform dependency
	}

EXPORT_C void MPagePool::Delete(TPageRef aRef)
/** Deletes a page, ignoring any errors.

@param aRef Reference to the page to delete */
	{
	TRAPD(ignore,DeleteL(aRef));
    UNUSED_VAR(ignore);
	}

EXPORT_C void MPagePool::DeleteL(TPageRef aRef)
/** Deletes a page, leaving if an error occurs.

@param aRef Reference to the page to delete */
	{
	if (aRef!=KNullPageRef)
		DoDeleteL(aRef);
	}

EXPORT_C CMemPagePool* CMemPagePool::NewL()
/** Allocates and constructs a new CMemPagePool object.

@return New CMemPagePool object */
	{
	CMemPagePool* pool=new(ELeave) CMemPagePool;
	return pool;
	}

EXPORT_C CMemPagePool *CMemPagePool::NewLC()
/** Allocates and constructs a new CMemPagePool object, and leaves it on the cleanup 
stack.

@return New CMemPagePool object */
	{
	CMemPagePool* pool=NewL();
	CleanupStack::PushL(pool);
	return pool;
	}

EXPORT_C CMemPagePool::CMemPagePool()
	: iPages(KMemPoolGranularity)
/** Default constructor. */
	{}

EXPORT_C CMemPagePool::~CMemPagePool()
/** Destructor.

On destruction, memory for all pages is freed. */
	{
	for (TInt ii=iPages.Count();--ii>=0;)
		User::Free(iPages[ii]);
	}

EXPORT_C TPageAbandonFunction CMemPagePool::AcquireL()
/** For memory-based pools, there is no need to abandon pages, so the function 
returned does nothing. 

@return Function that does nothing. */
	{
	return &DoAbandon;
	}

EXPORT_C TAny* CMemPagePool::AllocL()
/** Allocates a new unassigned page. 

@return Newly allocated page. */
	{
	TAny* page=User::AllocLC(KPoolPageSize);
	iPages.AppendL(page);
	CleanupStack::Pop();
	return page;
	}

EXPORT_C TAny* CMemPagePool::LockL(TPageRef aRef)
/** Returns a pointer to a specified page.

@param aRef Reference to the page to get
@return Page specified by aRef */
	{
	TAny* page=PageL(aRef);
	if (page==NULL)
		__LEAVE(KErrNotFound);
//
	return page;
	}

EXPORT_C TPageRef CMemPagePool::AssignL(const TAny* __DEBUG(aPage),TPageReclamation)
//
// Assign a reference to a newly allocated page. Supports only a single unassigned page at a time.
//
	{
	__ASSERT_DEBUG(iPages[iPages.Count()-1]==aPage,User::Invariant());
	return TPageRef(iPages.Count());
	}

EXPORT_C void CMemPagePool::UpdateL(const TAny*)
//
// Memory-based pages don't need to be updated.
//
	{}

EXPORT_C void CMemPagePool::Unlock(const TAny*,TPageChange)
//
// Memory-based pages don't need to be unlocked.
//
	{}

EXPORT_C void CMemPagePool::DoDeleteL(TPageRef aRef)
//
// Delete the page denoted by aRef.
//
	{
	TAny*& page=PageL(aRef);
	if (page==NULL)
		__LEAVE(KErrNotFound);
//
	User::Free(page);
	page=NULL;
	}

TAny*& CMemPagePool::PageL(TPageRef aRef)
//
// Return the page slot at aRef.
//
	{
	TInt i=aRef.Value()-1;
	if (i<0||i>=iPages.Count())
		__LEAVE(KErrNotFound);
//
	return iPages[i];
	}

void CMemPagePool::DoAbandon(MPagePool&)
//
// Abandoning memory-based pages is a no-op.
//
	{}

