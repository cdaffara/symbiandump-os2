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

#include "D32DBX.H"
#include "D32STOR.H"
#include <s32file.h>

const TInt32 KTestUidValue=0x01234567u;

NONSHARABLE_CLASS(COplDatabase) : public CDbFileStoreDatabase
	{
public:
	static CDbDatabase* CreateL(RFs& aFs,const TDesC& aName,TDbFormat::TCreate aMode,const TUidType& aType);
	static CDbSource* OpenL(RFs& aFs,const TDesC& aName,TDbFormat::TOpen aMode);
private:
	inline COplDatabase(RFs& aFs) :CDbFileStoreDatabase(aFs) {}
	static COplDatabase* NewLC(RFs& aFs);
	TStreamId CreateRootL(TStreamId aDatabaseId);
	TStreamId DatabaseIdL(TStreamId aRootId);	// override
	};

const TInt32 KUidOplDocValue=268435573;
const TInt32 KUidExternalOplFileValue=268435594;
const TInt32 KUidOplInterpreterValue=268435816;
const TUid KUidOplInterpreter={KUidOplInterpreterValue};

// Class OplDatabase
// this supports the "opl" and "opl.doc" formats

COplDatabase* COplDatabase::NewLC(RFs& aFs)
	{
	COplDatabase* self=new(ELeave) COplDatabase(aFs);
	CleanupStack::PushL(self);
	return self;
	}

CDbDatabase* COplDatabase::CreateL(RFs& aFs,const TDesC& aName,TDbFormat::TCreate aMode,const TUidType& aType)
	{
	COplDatabase* self=NewLC(aFs);
	self->CDbFileStoreDatabase::CreateL(aName,aMode,aType);
	CDbDatabase* db=self->InterfaceL();
	CleanupStack::Pop();			// self
	return db;
	}

TStreamId COplDatabase::CreateRootL(TStreamId aDatabaseId)
	{
	RStoreWriteStream root;
	TStreamId id=root.CreateLC(*iStore);
	root<<KUidOplInterpreter<<aDatabaseId;
	root.CommitL();
	CleanupStack::PopAndDestroy();	// root
	return id;
	}

CDbSource* COplDatabase::OpenL(RFs& aFs,const TDesC& aName,TDbFormat::TOpen aMode)
	{
	COplDatabase* self=NewLC(aFs);
	self->CDbFileStoreDatabase::OpenL(aName,aMode);
	CDbSource* src=self->SourceL();
	CleanupStack::Pop();			// self
	return src;
	}

TStreamId COplDatabase::DatabaseIdL(TStreamId aRootId)
	{
    RStoreReadStream read;
    read.OpenLC(*iStore,aRootId);
	TUid ignore;
    read>>ignore>>aRootId;
    CleanupStack::PopAndDestroy(); // read
    return aRootId;
	}

// data

NONSHARABLE_CLASS(CDataDocument) : public CDbFileStoreDatabase
	{
public:
	static CDbDatabase* CreateL(RFs& aFs,const TDesC& aName,TDbFormat::TCreate aMode,const TUidType& aType);
	static CDbSource* OpenL(RFs& aFs,const TDesC& aName,TDbFormat::TOpen aMode);
private:
	inline CDataDocument(RFs& aFs) :CDbFileStoreDatabase(aFs) {}
	TStreamId DatabaseIdL(TStreamId aRootId);	// override
	};

const TInt32 KUidAppDllDocValue=0x1000006d;
const TInt32 KUidDataAppValue=0x10000086;
const TUid KUidDataHeadStream={0x1000012e};

// Class DataDocument

//
// Cannot do this
//
CDbDatabase* CDataDocument::CreateL(RFs&,const TDesC&,TDbFormat::TCreate,const TUidType&)
	{
	User::Leave(KErrNotSupported);
	return 0;
	}

//
// read-only!
//
CDbSource* CDataDocument::OpenL(RFs& aFs,const TDesC& aName,TDbFormat::TOpen aMode)
	{
	if (aMode!=TDbFormat::EReadOnly)
		User::Leave(KErrNotSupported);
	CDataDocument* self=new(ELeave) CDataDocument(aFs);
	CleanupStack::PushL(self);
	self->CDbFileStoreDatabase::OpenL(aName,aMode);
	CDbSource* src=self->SourceL();
	CleanupStack::Pop();			// self
	return src;
	}

TStreamId CDataDocument::DatabaseIdL(TStreamId aRootId)
	{
    CStreamDictionary* dict=CStreamDictionary::NewLC();
    RStoreReadStream read;
    read.OpenLC(*iStore,aRootId);
    read>>*dict;
    CleanupStack::PopAndDestroy(); // read
    TStreamId id=dict->At(KUidDataHeadStream);
    CleanupStack::PopAndDestroy();  // dict
    read.OpenLC(*iStore,id); 
    read>>id;
    CleanupStack::PopAndDestroy(); // read
	return id;
	}

const TDbFormat KFormats[]=
	{
	{
	_S("test"),&CDbFileStoreDatabase::CreateL,&CDbFileStoreDatabase::OpenL,
	{KPermanentFileStoreLayoutUidValue,KTestUidValue}
	},
	{
	_S("data"),&CDataDocument::CreateL,&CDataDocument::OpenL,
	{KPermanentFileStoreLayoutUidValue,KUidAppDllDocValue,KUidDataAppValue}
	},
	{
	_S("opl"),&COplDatabase::CreateL,&COplDatabase::OpenL,
	{KPermanentFileStoreLayoutUidValue,KUidExternalOplFileValue}
	},
	{
	_S("opl.doc"),&COplDatabase::CreateL,&COplDatabase::OpenL,
	{KPermanentFileStoreLayoutUidValue,KUidOplDocValue}
	}
	};

const TDbDriver KDriver={4,KFormats};

EXPORT_C const TDbDriver& DbmsDriver()
	{
	return KDriver;
	}

