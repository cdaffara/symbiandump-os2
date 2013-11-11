// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__DATASTOR_H__)
#define __DATASTOR_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#if !defined(__S32FILE_H__)
#include <s32file.h>
#endif
#if !defined(__APMSTD_H__)
#include <apmstd.h>
#endif

class RReadStream;
class RWriteStream;

/**
@internalComponent
*/
const TUid KUidDatastor = {0x1000071E};

/**
@internalComponent
*/
const TUid KUidDatastorSettings = {0x10000790};

/**
@internalComponent
*/
_LIT(KIniFileName,":\\private\\10003a3f\\Dtstor.ini");

/**
@internalTechnology
@released
*/
class TMappingDataTypeToApp
	{
public:
	IMPORT_C TMappingDataTypeToApp();
	IMPORT_C TMappingDataTypeToApp(const TDataType& aDataType, 
		TDataTypePriority aPriority, TUid aAppUid);
	IMPORT_C TMappingDataTypeToApp(const TDataType& aDataType, 
		TDataTypePriority aPriority, TUid aAppUid, TUid aServiceUid);
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
public:
	TDataType iDataType;
	TDataTypePriority iPriority;
	TUid iAppUid; // Uid [2] of the target application
	TUid iServiceUid;
	};

/** A store of mappings between data types, the UIDs of applications that 
can handle them and the priority at which the application handles them.

@internalTechnology
@released
*/					   
class CTypeStoreManager : public CBase
	{
public:
	IMPORT_C static CTypeStoreManager* NewL(RFs& aFs);
	IMPORT_C ~CTypeStoreManager();
	// Persistence
	IMPORT_C void StoreL();
	IMPORT_C void RestoreL();
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	// Enquiry
	IMPORT_C void GetAppByDataType(const TDataType& aDataType, TUid& aUid) const;
	IMPORT_C void GetAppByDataType(const TDataType& aDataType, 
		TUid aServiceUid, TUid& aUid) const;
	IMPORT_C void GetDataTypesByAppL(TUid aUid, CArrayFix<TDataType>* aTypeArray) const;
	IMPORT_C const CArrayFixFlat<TMappingDataTypeToApp>& MappingArray() const;
	// Management
	IMPORT_C void InsertDataMappingL(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid);
	IMPORT_C void InsertDataMappingL(const TDataType& aDataType, 
		TDataTypePriority aPriority, TUid aUid, TUid aServiceUid);
	IMPORT_C TBool InsertIfHigherL(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid);
	IMPORT_C TBool InsertIfHigherL(const TDataType& aDataType, 
		TDataTypePriority aPriority, TUid aUid, TUid aServiceUid);
	IMPORT_C void DeleteDataMapping(const TDataType& aDataType);
	IMPORT_C void DeleteDataMapping(const TDataType& aDataType, 
		TUid aServiceUid);
	inline TPtrC IniFileName() const;
	IMPORT_C void InsertAndStoreDataMappingL(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid, TUid aServiceUid);
	IMPORT_C void InsertAndStoreDataMappingL(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid);
	IMPORT_C TBool InsertAndStoreIfHigherL(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid);
	IMPORT_C TBool InsertAndStoreIfHigherL(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid, TUid aServiceUid);
	IMPORT_C void DeleteAndStoreDataMappingL(const TDataType& aDataType);
	IMPORT_C void DeleteAndStoreDataMappingL(const TDataType& aDataType, TUid aServiceUid);
    IMPORT_C TBool DeleteApplicationDataMappings(const TUid aAppUid);

private:
	CDictionaryStore* OpenIniFileLC() const;
	CTypeStoreManager(RFs& aFs);
	TInt FindDataMapping(const TDataType& aDataType, const TUid& aServiceUid) const;
    void ConstructL();	
private:
	CArrayFixFlat<TMappingDataTypeToApp> iAppMappings;
	RFs& iFs;
	RBuf iIniFileName;
	};

inline TPtrC CTypeStoreManager::IniFileName() const
	{
	return iIniFileName;
	}

#endif
