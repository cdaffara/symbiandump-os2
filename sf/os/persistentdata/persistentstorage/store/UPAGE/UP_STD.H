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

#include <s32crypt.h>
#include "U32STD.H"

class TCachePage
	{
public:
	inline TDblQueLink& Link();
	inline void Deque();
	inline TBool IsQued() const;
	inline void Reque(TDblQue<TCachePage>& aQue);
private:
	inline const TDblQueLinkBase& Link() const;
public:
	TDblQueLink iLink;
	TCachePagePool* iOwner;
	TPageRef iRef;
	TPageChange iChange;
	};
//
class StorePagePool
	{
public:
	static TPageRef ExtendL(RStorePagePool& aPool,const TAny* aPage,TPageReclamation aReclamation,const CPBEncryptionBase* aKey=NULL);
	static void WriteL(RStorePagePool& aPool,TPageRef aRef,const TAny* aPage,TPageChange aChange,const CPBEncryptionBase* aKey=NULL);
	static void ReadL(RStorePagePool& aPool,TPageRef aRef,TAny* aPage,const CPBEncryptionBase* aKey=NULL);
	static void DeleteL(RStorePagePool& aPool,TPageRef aRef,const CPBEncryptionBase* aKey=NULL);
private:
	static void PadL(RWriteStream& aStream,TInt aLength);
	static void EncryptL(RWriteStream& aStream,const TAny* aPage,const CPBEncryptionBase& aKey);
	static void EncryptNewL(RWriteStream& aStream,const TAny* aPage,const CPBEncryptionBase& aKey);
	static void DecryptL(RReadStream& aStream,TAny* aPage,const CPBEncryptionBase& aKey);
	static void SeekL(MStreamBuf* aBuf,TInt aMark,TPageRef aRef,const CPBEncryptionBase* aKey);
	};
//
enum TPagePanic
	{
	EPageNotOpen,
	EPageChangeInvalid,
	EPageDoubleLock,
	EPageNotLocked,
	EPageCacheInUse,
	EPageCacheFull,
	EPageReclaimAvailable,
	EPageCipherTextOverrun,
	ECryptKeyUninitialised,
	EPageBadKeyType
	};

GLREF_C void Panic(TPagePanic aPanic);

#include "UP_STD.INL"

