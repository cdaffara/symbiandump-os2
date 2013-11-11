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

#include "UF_STD.H"

const TInt KBaseOffset=KFileStoreStartOffset-KPermanentStoreHeaderOffset;
#if defined(__NO_CLASS_CONSTS__)
#define KBase TStreamPos(KBaseOffset)
#else
const TStreamPos KBase=TStreamPos(KBaseOffset);
#endif

EXPORT_C TUid CPermanentFileStore::Layout() const
/** Gets the UID that uniquely identifies this file store as a permanent file store.

@return KPermanentFileStoreLayoutUid. */
	{
	return KPermanentFileStoreLayoutUid;
	}

EXPORT_C CPermanentFileStore::CPermanentFileStore(RFile& aFile)
//
// Constructor creating a new file store.
//
	: CFileStore(aFile)/*,iCoord(NULL)*/
	{}

EXPORT_C CPermanentFileStore::CPermanentFileStore(RFileBuf& aBuf,const TUidType& aType)
//
// Constructor opening an existing file store.
//
	: CFileStore(aBuf,aType)/*,iCoord(NULL)*/
	{}

EXPORT_C void CPermanentFileStore::MarshalL()
	{
	__ASSERT_DEBUG(iCoord==NULL,User::Invariant());
	iCoord=new(ELeave) CPermanentStoreCoord(KBase,Host());
	CFileStore::MarshalL();
	iRoot=Coord().PrimaryL();
	}

EXPORT_C CPermanentFileStore::~CPermanentFileStore()
/** Frees resources owned by the object, prior to its destruction. */
	{
	Destruct();
	delete iCoord;
	}

EXPORT_C MStreamBuf* CPermanentFileStore::DoReadL(TStreamId anId) const
	{
	if (iCoord==NULL)
		__LEAVE(KErrNotFound);
//
	return HPermanentStoreBuf::OpenL(CoordL(),anId,HPermanentStoreBuf::ERead);
	}

EXPORT_C MStreamBuf* CPermanentFileStore::DoCreateL(TStreamId& anId)
	{
	return HPermanentStoreBuf::CreateL(TrimL(),anId);
	}

EXPORT_C MStreamBuf* CPermanentFileStore::DoWriteL(TStreamId anId)
	{
	if (iCoord==NULL)
		__LEAVE(KErrNotFound);
//
	return HPermanentStoreBuf::OpenL(CoordL(),anId);
	}

EXPORT_C MStreamBuf* CPermanentFileStore::DoReplaceL(TStreamId anId)
	{
	if (iCoord==NULL)
		__LEAVE(KErrNotFound);
//
	return HPermanentStoreBuf::ReplaceL(TrimL(),anId);
	}

EXPORT_C void CPermanentFileStore::ExternalizeL(RWriteStream& aStream) const
//
// Externalize this store's structure, during construction only.
//
	{
	__ASSERT_DEBUG(!Host().IsActive()&&iCoord==NULL,User::Invariant());
	TUint8 buf[-KPermanentStoreHeaderOffset];
	Mem::FillZ(buf,sizeof(buf));
	aStream.WriteL(buf,sizeof(buf));
	}

EXPORT_C void CPermanentFileStore::InternalizeL(RReadStream& aStream)
//
// Internalize this store's structure, during marshalling or refreshment.
//
	{
	__ASSERT_DEBUG(!Host().IsActive(),User::Invariant());
	Coord().InternalizeL(aStream);
	}

EXPORT_C void CPermanentFileStore::DoSetRootL(TStreamId anId)
//
// Set the root stream id.
//
	{
	CoordL().ChangedL();
	iRoot=anId;
	}

EXPORT_C TStreamId CPermanentFileStore::DoExtendL()
	{
	return CoordL().ExtendL();
	}

EXPORT_C void CPermanentFileStore::DoDeleteL(TStreamId anId)
	{
	if (iCoord==NULL)
		__LEAVE(KErrNotFound);
//
	CoordL().DeleteL(anId);
	}

EXPORT_C void CPermanentFileStore::DoCommitL()
//
// Establish a new commit point, committing changes made since the last one.
//
	{
	if (iCoord!=NULL&&TrimL().CommitL(iRoot))
		{
		return;
		}
//
	SynchL();
	}

EXPORT_C void CPermanentFileStore::DoRevertL()
//
// Roll back to the state at the last commit point.
//
	{
	CPermanentStoreCoord* coord=iCoord;
	if (coord!=NULL&&coord->RevertL(iRoot))
		return;
//
	SynchL();
	}

EXPORT_C MIncrementalCollector* CPermanentFileStore::DoReclaimL()
	{
	return CPermanentStoreCollector::ReclaimerL(CoordL());
	}

EXPORT_C MIncrementalCollector* CPermanentFileStore::DoCompactL()
	{
	return CPermanentStoreCollector::CompactorL(TrimL());
	}

CPermanentStoreCoord& CPermanentFileStore::CoordL() const
	{
	CPermanentStoreCoord* coord=iCoord;
	if (coord==NULL)
		{
		coord=new(ELeave) CPermanentStoreCoord(KBase,Host());
		MUTABLE_CAST(CPermanentStoreCoord*&,iCoord)=coord;
		}
	else if (!Host().IsActive())
		CONST_CAST(CPermanentFileStore*,this)->RefreshL();
	return *coord;
	}

CPermanentStoreCoord& CPermanentFileStore::TrimL()
	{
	CPermanentStoreCoord& coord=CoordL();
	if (!coord.IsTrim())
		{
		TInt lim=coord.LimitL().Offset(); // it can be slightly smaller than this
		TInt size=Host().HostL()->SizeL();
		if (size>lim)
			SetSizeL(lim);
		coord.Clipped();
		}
	return coord;
	}

EXPORT_C CFileStore* CPermanentFileStore::DoNewL(RFile& aFile)
//
// Factory function creating a new file.
//
	{
	return new(ELeave) CPermanentFileStore(aFile);
	}

