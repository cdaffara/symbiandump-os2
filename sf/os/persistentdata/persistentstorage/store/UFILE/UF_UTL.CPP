// Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#pragma BullseyeCoverage off

//
// Panic the process with STORE-File as the category.
//
GLDEF_C void Panic(TFilePanic aPanic)
	{
	_LIT(KCategory,"STORE-File");
	User::Panic(KCategory,aPanic);
	}

#pragma BullseyeCoverage on

EXPORT_C CFileStore* FileStoreFactory::DirectLC(RFileBuf& aBuf,const TUidType& aType)
/** Opens an existing direct file store and creates a file store object to represent 
it.

@param aBuf The stream buffer to be used.
@param aType The Uidtype for the store.
@return A pointer to the new direct file store object. This is Null, if the 
specified Uidtype is KNullUid or the first Uid component is not KDirectFileStoreLayoutUid. */
	{
	if ((! aType.IsValid()) || aType[0]!=KDirectFileStoreLayoutUid)
		return NULL;
//
	CDirectFileStore* store=new(ELeave) CDirectFileStore(aBuf,aType);
	CleanupStack::PushL(store);
	store->MarshalL();
	return store;
	}

EXPORT_C CFileStore* FileStoreFactory::PermanentLC(RFileBuf& aBuf,const TUidType& aType)
/** Opens an existing permanent file store and creates a file store object to represent 
it.

@param aBuf The stream buffer to be used.
@param aType The Uidtype for the store.
@return A pointer to the new permanent file store object. This is Null, if 
the specified Uidtype is KNullUid or the first Uid component is not KPermanentFileStoreLayoutUid. */
	{
	if ((! aType.IsValid()) || aType[0]!=KPermanentFileStoreLayoutUid)
		return NULL;
//
	CPermanentFileStore* store=new(ELeave) CPermanentFileStore(aBuf,aType);
	CleanupStack::PushL(store);
	store->MarshalL();
	return store;
	}

