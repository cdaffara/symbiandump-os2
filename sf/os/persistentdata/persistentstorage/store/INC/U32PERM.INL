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

// Class TPermanentStoreHeader
inline TUint8* TPermanentStoreHeader::Ptr()
	{return REINTERPRET_CAST(TUint8*,&iBackup);}
inline const TUint8* TPermanentStoreHeader::Ptr() const
	{return REINTERPRET_CAST(const TUint8*,&iBackup);}
inline TPermanentStoreHeader::TPermanentStoreHeader(TInt aToc)
	{Set(aToc,0,aToc);}
inline TPermanentStoreHeader::TPermanentStoreHeader(TInt aBackupToc,TInt aHandle,TInt aReference)
	{
	__ASSERT_DEBUG(aHandle!=0,User::Invariant());
	Set(aBackupToc,aHandle,aReference);
	}
inline TBool TPermanentStoreHeader::IsDirty() const
	{return iBackup&0x1;}
inline void TPermanentStoreHeader::MarkDirty()
	{iBackup|=0x1;}
inline void TPermanentStoreHeader::SetBackupToc(TInt aBackupToc)
	{
	__ASSERT_DEBUG(aBackupToc>=0,User::Invariant());
	iBackup=TUint32(aBackupToc)<<1;
	}
inline TInt TPermanentStoreHeader::BackupToc() const
	{return iBackup>>1;}
inline TInt TPermanentStoreHeader::Handle() const
	{return iHandle;}
inline TInt TPermanentStoreHeader::Reference() const
	{return iRef;}

// Class CPermanentStoreToc
inline TInt CPermanentStoreToc::Extent() const
	{return iExt;}
inline TBool CPermanentStoreToc::IsVirtual() const
	{return iPrimary<0;}
inline TInt CPermanentStoreToc::Primary() const
	{return iPrimary&KMaskStreamIdValue;}
inline void CPermanentStoreToc::Changed()
	{iPrimary|=KHandleInvalid;}
inline TStreamPos CPermanentStoreToc::Base() const
	{return iBase;}
inline TStreamExchange& CPermanentStoreToc::Host() const
	{
	__ASSERT_DEBUG(iHost!=NULL,User::Invariant());
	return *iHost;
	}
inline const CPermanentStoreToc::TEntry* CPermanentStoreToc::Entry(TInt aHandle) const
	{return CONST_CAST(CPermanentStoreToc*,this)->Entry(aHandle);}
inline TBool CPermanentStoreToc::HasDelta() const
	{return iOff!=iTocOff;}

// Class RPermanentStoreTocIter
inline void RPermanentStoreTocIter::Close()
	{Release();}

// Class TPermanentStoreCache
inline TPermanentStoreCache::TPermanentStoreCache()
	{Invalidate();}

// Class CPermanentStoreCoord
inline TBool CPermanentStoreCoord::IsTrim() const
	{return !(iState&EClip);}
inline void CPermanentStoreCoord::Clipped()
	{iState&=~EClip;}
inline TStreamPos CPermanentStoreCoord::Base() const
	{return iBase;}
inline TStreamExchange& CPermanentStoreCoord::Host() const
	{
	__ASSERT_DEBUG(iHost!=NULL,User::Invariant());
	return *iHost;
	}
inline TInt CPermanentStoreCoord::Toc() const
	{return iToc;}
inline CPermanentStoreToc& CPermanentStoreCoord::Table() const
	{
	__ASSERT_DEBUG(iTable!=NULL,User::Invariant());
	return *iTable;
	}
inline TUint CPermanentStoreCoord::Generation() const
	{return iGen;}
inline void CPermanentStoreCoord::Inc()
	{++iRefs;}
inline void CPermanentStoreCoord::Dec()
	{--iRefs;}
inline TBool CPermanentStoreCoord::Accessed() const
	{return iAccess;}

// Class HPermanentStoreBuf
inline HPermanentStoreBuf::HPermanentStoreBuf(CPermanentStoreCoord& aCoord)
	: RFrame16Buf(aCoord.Base()),iCoord(&aCoord),iHandle(0)
	{}
inline CPermanentStoreCoord& HPermanentStoreBuf::Coord() const
	{
	__ASSERT_DEBUG(iCoord!=NULL,User::Invariant());
	return *iCoord;
	}

// Class CPermanentStoreCollector
inline CPermanentStoreCoord& CPermanentStoreCollector::Coord() const
	{
	__ASSERT_DEBUG(iCoord!=NULL,User::Invariant());
	return *iCoord;
	}
inline TStreamExchange& CPermanentStoreCollector::Host() const
	{
	__ASSERT_DEBUG(iHost!=NULL,User::Invariant());
	return *iHost;
	}
inline TBool CPermanentStoreCollector::Compacting() const
	{return iReloc!=NULL;}

