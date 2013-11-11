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
// The Store-Database
// 
//

#ifndef __D32STOR_H__
#define __D32STOR_H__
#ifndef __D32TABLE_H__
#include "D32TABLE.H"
#endif
#ifndef __S32STOR_H__
#include <s32stor.h>
#endif

// other classes referenced
class MDbStreamFilter;
class CDbStoreCompression;
class CClusterCache;

// classes defined
class CDbStoreDatabase;
class CDbFileStoreDatabase;

/**
@internalComponent
*/
enum TDbStoreVersion { EDbStoreVersion2=3,EDbStoreSecure,EDbStoreCompressed };

/**
@internalComponent
*/
class CDbStoreDatabase : public CDbTableDatabase
	{
public:
	enum TZipType {EDeflate,EInflate};
	
private:
	#ifndef PAGE_CACHE_SIZE
		#define PAGE_CACHE_SIZE 16
	#else
		#if PAGE_CACHE_SIZE < 8
			#error "PAGE_CACHE_SIZE macro value can't be less than 8"
		#endif
	#endif
	enum {EPageCachePages=PAGE_CACHE_SIZE};
	enum {EModified=0x1,EDamaged=0x2,ESharedStore=0x80000000};
	class CCompactor;
	
public:
// factory functions for pure StoreDatabase
	static CDbDatabase* CreateL(CStreamStore* aStore,TStreamId& aStreamId);
	static CDbSource* OpenL(CStreamStore* aStore,TStreamId aStreamId);
	static void CompressL(CStreamStore* aStore,TStreamId aStreamId,TZipType aZip);
// accessors
	inline CStreamStore& Store();
	inline MDbStreamFilter* Filter();
	CClusterCache& ClusterCacheL();
	MPagePool& PagePoolL();
// user-class's transaction protocol
	void MarkL();
	void IndexRecoveredL();
	
protected:
	IMPORT_C CDbStoreDatabase();
	IMPORT_C ~CDbStoreDatabase();
// phase 2 c'tors
	IMPORT_C TStreamId ConstructL();
// framework: authentication
	IMPORT_C void AuthenticateL();
	IMPORT_C void RestoreL(TStreamId aStreamId);
// framework: pure delegated functions
	IMPORT_C TInt Property(CDbDatabase::TProperty aProperty);
	IMPORT_C void DestroyL();
// framework: transaction protocol
	IMPORT_C void LoadSchemaL();
	IMPORT_C void SynchL(TDbLockType aLock);
	IMPORT_C void Revert(TDbLockType aLock);
	IMPORT_C void Idle();
// framework: object factory
	IMPORT_C CDbTableDef* CreateTableL(const TDesC& aName,const CDbColSet& aColSet,const CDbKey* aPrimaryKey);
	IMPORT_C CDbTableIndexDef* CreateIndexL(const CDbTableDef& aTable,const TDesC& aName,const CDbKey& aKey);
	IMPORT_C CDbTable* TableL(const CDbTableDef& aDef);
// framework: incremental factory
	IMPORT_C CStepper* TableAlterL(CDbTableDef& aTable,const HDbColumnSet& aNewSet,TInt& aStep);
	IMPORT_C CStepper* RecordDiscarderL(const CDbTableDef& aTable,TInt& aStep);
	IMPORT_C CStepper* IndexDiscarderL(const CDbTableDef& aTable,const CDbTableIndexDef& anIndex,TInt& aStep);
	IMPORT_C CStepper* UtilityL(CDbDatabase::TUtility aType,TInt& aStep);
	
private:
// framework: reserved
	IMPORT_C void Reserved_1();
	IMPORT_C void Reserved_2();
// implementationp functions
	static CDbStoreDatabase* NewLC(CStreamStore* aStore);
//
	void CompressTablesL();
	void CompressL(TStreamId aStreamId,TZipType aZip);
//
	void ReadHeaderL(RReadStream& aStream);
	void InitPagePoolL();
	void ReplaceSchemaL();
	void ReplaceTokenL(TUint aFlags);
	void SynchStoreL(TDbLockType aLock);
	CDbStoreCompression& CompressionL();
	CStepper* RecoverL(TInt& aStep);
	
protected:
	CStreamStore* iStore;
	TInt iReclaim;
	
private:
	TUint8 iVersion;
	TUint8 iFlags;
	TUint8 iSharedStore;
	TStreamId iSchemaId;
	TStreamId iTokenId;
	TStorePagePoolToken iPoolToken;
	RStorePagePool* iPagePool;
	CPageCache* iPageCache;
	CClusterCache* iClusterCache;
	MDbStreamFilter* iFilter;
	CDbStoreCompression* iCompression;
	
	};

/**
The general file-store database class
@internalComponent
*/
const TInt32 KDbmsFileDatabaseUidValue=0x10000EBE;
const TUid KDbmsFileDatabaseUid={KDbmsFileDatabaseUidValue};

/**
@internalComponent
*/
class CDbFileStoreDatabase : public CDbStoreDatabase
	{
public:
	IMPORT_C static CDbDatabase* CreateL(RFs& aFs, const TDesC& aName, TDbFormat::TCreate aMode,
                                         const TUidType& aType);
	IMPORT_C static CDbSource* OpenL(RFs& aFs,const TDesC& aName,TDbFormat::TOpen aMode);
	
protected:
	IMPORT_C CDbFileStoreDatabase(RFs& aFs);
	IMPORT_C ~CDbFileStoreDatabase();
//
	IMPORT_C void CreateL(const TDesC& aName, TDbFormat::TCreate aMode, const TUidType& aType);
	IMPORT_C virtual TStreamId CreateRootL(TStreamId aDatabaseId);
//
	IMPORT_C void OpenL(const TDesC& aName,TDbFormat::TOpen aMode);
	IMPORT_C virtual TStreamId DatabaseIdL(TStreamId aRootId);
// from CDbStoreDatabase
	IMPORT_C TInt Property(CDbDatabase::TProperty aProperty);
	IMPORT_C void DestroyL();
	
private:
	static CDbFileStoreDatabase* NewLC(RFs& aFs);
	
private:
	RFs& iFs;
	HBufC* iName;
	TBool iDelete;
	
	};

#endif
