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
// The file-store database which provides the default format
// 
//

#include "US_STD.H"
#include <s32file.h>

EXPORT_C CDbFileStoreDatabase::CDbFileStoreDatabase(RFs& aFs)
	:iFs(aFs)
	{}

EXPORT_C CDbFileStoreDatabase::~CDbFileStoreDatabase()
	{
	if (iDelete)
		{
		delete iStore;		// must be destroyed before the file is deleted
		iStore=0;

		// If a debug build - record error
		TInt fileDeleteErr=iFs.Delete(*iName);
		#ifdef _DEBUG
			if (fileDeleteErr != KErrNone)
			{
				RDebug::Print(_L("CDbFileStoreDatabase::~CDbFileStoreDatabase - Failed to delete file. Error = %d"), fileDeleteErr);
			}
		#endif
		}
	delete iName;
	}

CDbFileStoreDatabase* CDbFileStoreDatabase::NewLC(RFs& aFs)
	{
	CDbFileStoreDatabase* self=new(ELeave) CDbFileStoreDatabase(aFs);
	CleanupStack::PushL(self);
	return self;
	}

//
// over-ride the store database and just mark the file for deletion
//
EXPORT_C void CDbFileStoreDatabase::DestroyL()
	{
	iDelete=ETrue;
	}

//
// Provide the "size" and "usage" properties
//
EXPORT_C TInt CDbFileStoreDatabase::Property(CDbDatabase::TProperty aProperty)
	{
	switch (aProperty)
		{
	case CDbDatabase::EUpdateStats:
		return 1;
	case CDbDatabase::ESize:
	case CDbDatabase::EUsage:
		{
		TInt size;
		TInt r=STATIC_CAST(CFileStore&,Store()).File().Size(size);
		if (r<0)
			return r;
		if (aProperty==CDbDatabase::ESize)
			return size;
		r=iReclaim;
		if (r<0)
			return r;
		return 100-(r*100)/size;	// usage in %
		}
	default:
		return CDbStoreDatabase::Property(aProperty);
		}
	}


//SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version of the method.
EXPORT_C void CDbFileStoreDatabase::CreateL(const TDesC& aName, TDbFormat::TCreate aMode,
                                            const TUidType& aType)
	{
	__ASSERT(!iName);	// check construction phase
//
	iName=aName.AllocL();
	CFileStore* store;
	switch (aMode)
		{
	default:
		__LEAVE(KErrNotSupported);
	case TDbFormat::ECreate:   
		store=CPermanentFileStore::CreateL(iFs,aName,EFileRead|EFileWrite);
		break;
	case TDbFormat::EReplace:
		store=CPermanentFileStore::ReplaceL(iFs,aName,EFileRead|EFileWrite);
		break;
		};
	iStore=store;
	iDelete=ETrue;		// cleanup fully in case of failure
	store->SetTypeL(aType);
	store->SetRootL(CreateRootL(CDbStoreDatabase::ConstructL()));
	store->CommitL();
	iDelete=EFalse;				// file is now good
	}

// SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version of the method.
// Create the standard file database. The database is the root stream
//
EXPORT_C CDbDatabase* CDbFileStoreDatabase::CreateL(RFs& aFs, const TDesC& aName,
                                                    TDbFormat::TCreate aMode, const TUidType& aType)
	{
	CDbFileStoreDatabase* self=NewLC(aFs);
	self->CreateL(aName,aMode,aType);
	CDbDatabase* db=self->InterfaceL();
	CleanupStack::Pop(self);
	return db;
	}


//
// default implementation. Database _is_ the root
//
EXPORT_C TStreamId CDbFileStoreDatabase::CreateRootL(TStreamId aDatabaseId)
	{
	return aDatabaseId;
	}

//
// Open, phase #1
// open the file-store and return the root stream id
//
EXPORT_C void CDbFileStoreDatabase::OpenL(const TDesC& aName,TDbFormat::TOpen aMode)
	{
	__ASSERT(!iName);	// check construction phase
//
	iName=aName.AllocL();
	TUint mode=aMode==TDbFormat::EReadOnly ? EFileShareReadersOnly : EFileWrite;
	CFileStore* store=CPermanentFileStore::OpenL(iFs,*iName,mode);
	iStore=store;
	CDbStoreDatabase::RestoreL(DatabaseIdL(store->Root()));
	}

//
// default implementation. Database _is_ the root
//
EXPORT_C TStreamId CDbFileStoreDatabase::DatabaseIdL(TStreamId aRootId)
	{
	return aRootId;
	}

//
// Create the standard file database. The database is the root stream
//
EXPORT_C CDbSource* CDbFileStoreDatabase::OpenL(RFs& aFs,const TDesC& aName,TDbFormat::TOpen aMode)
	{
	CDbFileStoreDatabase* self=NewLC(aFs);
	self->OpenL(aName,aMode);
	CDbSource* src=self->SourceL();
	CleanupStack::Pop();			// self
	return src;
	}

const TDbFormat KBuiltinFormat=
	{
	_S("epoc"),&CDbFileStoreDatabase::CreateL,&CDbFileStoreDatabase::OpenL,
	{KPermanentFileStoreLayoutUidValue,KDbmsFileDatabaseUidValue}
	};
	
GLDEF_D const TDbDriver KBuiltinDriver={1,&KBuiltinFormat};
