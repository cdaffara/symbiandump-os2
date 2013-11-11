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
#include <s32file.h>

EXPORT_C RFilePagePool::RFilePagePool()
/** Default constructor. */
	{}

EXPORT_C RFilePagePool::RFilePagePool(CPageCache& aCache)
	: TCachePagePool(aCache)
/** Constructor with a page cache for the pool.

@param aCache Page cache for the pool */
	{}

EXPORT_C void RFilePagePool::Close()
/** Flushes cached pages to the file, and closes the file. */
	{
	TCachePagePool::Flush();
	Release();
	}

EXPORT_C void RFilePagePool::Release()
/** Closes the file without flushing the cache. */
	{
	Purge();
	iFile.Close();
	}

EXPORT_C TInt RFilePagePool::Flush()
/** Flushes the page cache and the file.

@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	TInt r=TCachePagePool::Flush();
	if (r==KErrNone)
		{
		r=iFile.Flush();
		}
	return r;
	}

EXPORT_C void RFilePagePool::FlushL()
/** Flushes the page cache and the file, leaving with a system-wide error code 
if an error occurs. */
	{
	TCachePagePool::FlushL();
	__LEAVE_IF_ERROR(iFile.Flush());
	}

EXPORT_C TPageRef RFilePagePool::ExtendL(const TAny* aPage,TPageReclamation)
	{
	TInt pos=0;
	__LEAVE_IF_ERROR(iFile.Seek(ESeekEnd,pos));
	__ASSERT_DEBUG(pos>=0,User::Invariant());
	if (pos%KPoolPageSize!=0)
		__LEAVE(KErrCorrupt);
//
	__LEAVE_IF_ERROR(iFile.Write(pos,TPtrC8((const TUint8*)aPage,KPoolPageSize)));
	return TPageRef(pos/KPoolPageSize+1);
	}

EXPORT_C void RFilePagePool::WriteL(TPageRef aRef,const TAny* aPage,TPageChange)
	{
	TInt pos=(aRef.Value()-1)*KPoolPageSize;
	__LEAVE_IF_ERROR(iFile.Write(pos,TPtrC8((const TUint8*)aPage,KPoolPageSize)));
	}

EXPORT_C void RFilePagePool::ReadL(TPageRef aRef,TAny* aPage)
	{
	TInt pos=(aRef.Value()-1)*KPoolPageSize;
	TPtr8 ptr((TUint8*)aPage,KPoolPageSize);
	__LEAVE_IF_ERROR(iFile.Read(pos,ptr));
	if (ptr.Length()<KPoolPageSize)
		__LEAVE(KErrEof);
	}

