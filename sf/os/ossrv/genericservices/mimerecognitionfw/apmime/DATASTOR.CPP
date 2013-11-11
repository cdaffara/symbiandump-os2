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

#include "DATASTOR.H"
#include "APMPAN.H"

const TUid KOpenServiceUid = { 0x10208DCA };

const TInt KAppMappingGranularity=1;
//
//	class TMappingDataTypeToApp
//

EXPORT_C TMappingDataTypeToApp::TMappingDataTypeToApp()
	:iDataType(TDataType(_L8(""))), iServiceUid(KOpenServiceUid)
	{
	iAppUid.iUid=0;
	}

EXPORT_C TMappingDataTypeToApp::TMappingDataTypeToApp(const TDataType& aDataType,
	TDataTypePriority aPriority, TUid aAppUid)
	:iDataType(aDataType),
	iPriority(aPriority),
	iAppUid(aAppUid),
	iServiceUid(KOpenServiceUid)
	{
	}
	
EXPORT_C TMappingDataTypeToApp::TMappingDataTypeToApp(const TDataType& aDataType,
	TDataTypePriority aPriority, TUid aAppUid, TUid aServiceUid)
	:iDataType(aDataType),
	iPriority(aPriority),
	iAppUid(aAppUid),
	iServiceUid(aServiceUid)
	{
	}

EXPORT_C void TMappingDataTypeToApp::InternalizeL(RReadStream& aStream)
	{
	aStream >> iDataType;
	iPriority=aStream.ReadInt32L();
	iAppUid.iUid=aStream.ReadInt32L();
	iServiceUid.iUid=aStream.ReadInt32L();
	}

EXPORT_C void TMappingDataTypeToApp::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iDataType;
	aStream.WriteInt32L(iPriority);
	aStream.WriteInt32L(iAppUid.iUid);
	aStream.WriteInt32L(iServiceUid.iUid);
	}


//
// class CTypeStoreManager
//

EXPORT_C void CTypeStoreManager::InternalizeL(RReadStream& aStream)
/** Internalises the array of data type mappings from a read stream.

@param aStream Stream from which the data type mappings are internalised. */
	{
	aStream >> iAppMappings;
	}

EXPORT_C void CTypeStoreManager::ExternalizeL(RWriteStream& aStream) const
/** Externalises the data type mappings to a write stream.

@param aStream Stream to which the data type mappings should be externalised. */
	{
	aStream << iAppMappings;
	}

EXPORT_C CTypeStoreManager* CTypeStoreManager::NewL(RFs& aFs)
/** Constructs a CTypeStoreManager object.

@param aFs A session with the file server.
@return The newly created CTypeStoreManager object. */
	{
	CTypeStoreManager* self = new(ELeave) CTypeStoreManager(aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return (self);
	}
	
void CTypeStoreManager::ConstructL()
	{
	TChar sysDrive = RFs::GetSystemDriveChar();
	TInt maxSizeOfFileName = KIniFileName().Length() + 1;
	iIniFileName.CreateL(maxSizeOfFileName);
	iIniFileName.Append(sysDrive);
	iIniFileName.Append(KIniFileName());
	}
CTypeStoreManager::CTypeStoreManager(RFs& aFs)
	: iAppMappings(KAppMappingGranularity),
	iFs(aFs)
	{
	__ASSERT_ALWAYS(RProcess().SecureId()==0x10003a3f, Panic(EPanicNotBeingUsedFromWithinApparcServerProcess));
	}

EXPORT_C CTypeStoreManager::~CTypeStoreManager()
/** Destructor. */
	{
	iIniFileName.Close();
	}

EXPORT_C void CTypeStoreManager::StoreL()
/** Stores the data type mappings to the data store ini file 
(c:\\System\\Data\\Dtstor.ini), creating a new ini file if it does 
not already exist. */
	{
	CDictionaryStore* iniFile=OpenIniFileLC();
	iniFile->RemoveL(KUidDatastorSettings);
	RDictionaryWriteStream outStream;
	outStream.AssignLC(*iniFile,KUidDatastorSettings);
	outStream << *this;
	outStream.CommitL();
    CleanupStack::PopAndDestroy();
    iniFile->CommitL();
    CleanupStack::PopAndDestroy();	// inifile
	}
	 
EXPORT_C void CTypeStoreManager::RestoreL()
/** Restores the data type mappings from the data store ini file. */
	{
	CDictionaryStore* iniFile=OpenIniFileLC();
	RDictionaryReadStream inStream;
	inStream.OpenLC(*iniFile,KUidDatastorSettings);
	inStream >> *this;
    CleanupStack::PopAndDestroy(2); // inStream * iniFile
	}

/** Changes an existing data type mapping, or adds a new one.

If the data type does not exist in the store, the new mapping is appended. 
If the data type does exist, its mapping is replaced.

The service is considered to be the KOpenServiceUid service.

@param aDataType A new or existing data type.
@param aPriority The priority with which the application handles the data type.
@param aUid The UID of the application to associate with the data type. 
*/
EXPORT_C void CTypeStoreManager::InsertDataMappingL(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid)
	{
	InsertDataMappingL(aDataType, aPriority, aUid, KOpenServiceUid);
	}
	
/** Changes an existing data type mapping, or adds a new one.

If the data type does not exist in the store, the new mapping is appended. 
If the data type does exist, its mapping is replaced.

@param aDataType A new or existing data type.
@param aPriority The priority with which the application handles the data type.
@param aUid The UID of the application to associate with the data type. 
@param aServiceUid The UID of the service. 
*/
EXPORT_C void CTypeStoreManager::InsertDataMappingL(const TDataType& aDataType, 
	TDataTypePriority aPriority, TUid aUid, TUid aServiceUid)
	{
	TInt i = FindDataMapping(aDataType, aServiceUid);
	if ( i == KErrNotFound )
		iAppMappings.AppendL( TMappingDataTypeToApp( aDataType, aPriority, aUid, aServiceUid ) );
	else
		{
		TMappingDataTypeToApp& mapping = iAppMappings[i];
		mapping.iDataType=aDataType;
		mapping.iPriority=aPriority;
		mapping.iAppUid=aUid;
		mapping.iServiceUid=aServiceUid;
		}
	}

/** Changes an existing data type mapping, or adds a new one.
If the data type does not exist in the store, or if it does and its existing priority 
is less than aPriority, the new mapping is added to the store, or replaces the existing one. 
Otherwise, no change is made.

The service is considered to be the KOpenServiceUid service.

@param aDataType A new or existing data type.
@param aPriority The priority with which the application handles the data type.
@param aUid The UID of the application to associate with the data type.
@return ETrue if the new mapping was added or an existing mapping replaced, EFalse if no 
change was made.
*/
EXPORT_C TBool CTypeStoreManager::InsertIfHigherL(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid)
	{
	return InsertIfHigherL(aDataType, aPriority, aUid, KOpenServiceUid);
	}
	
/** Changes an existing data type mapping, or adds a new one.
If the data type does not exist in the store, or if it does and its existing priority 
is less than aPriority, the new mapping is added to the store, or replaces the existing one. 
Otherwise, no change is made.

@param aDataType A new or existing data type.
@param aPriority The priority with which the application handles the data type.
@param aUid The UID of the application to associate with the data type.
@param aServiceUid The UID of the service. 
@return ETrue if the new mapping was added or an existing mapping replaced, EFalse if no 
change was made.
*/
EXPORT_C TBool CTypeStoreManager::InsertIfHigherL(const TDataType& aDataType, 
	TDataTypePriority aPriority, TUid aUid, TUid aServiceUid)
	{
	TInt i = FindDataMapping( aDataType, aServiceUid );
	if ( i == KErrNotFound || iAppMappings[i].iPriority < aPriority )
		{
		InsertDataMappingL( aDataType, aPriority, aUid, aServiceUid );
		return ETrue;
		}
	else
		return EFalse;
	}

/** Removes an existing data type mapping from the store.

The service is considered to be the KOpenServiceUid service.

@param aDataType Data type whose mapping should be removed.
@panic USER 0 The specified data type cannot be found. Debug builds only.
*/
EXPORT_C void CTypeStoreManager::DeleteDataMapping(const TDataType& aDataType)
	{
	DeleteDataMapping(aDataType, KOpenServiceUid);
	}

/** Removes an existing data type mapping from the store.

@param aDataType Data type whose mapping should be removed.
@param aServiceUid The UID of the service. 
@panic USER 0 The specified data type cannot be found. Debug builds only.
*/	
EXPORT_C void CTypeStoreManager::DeleteDataMapping(const TDataType& aDataType, 
	TUid aServiceUid)
	{
	TInt i=FindDataMapping(aDataType, aServiceUid);
	__ASSERT_DEBUG(i!=KErrNotFound,User::Invariant());
	iAppMappings.Delete(i);
	}

/** Gets the UID of the application mapped to the specified data type.

The service is considered to be the KOpenServiceUid service.

@param aDataType The data type.
@param aUid On return, the UID of the application associated with the 
data type, or KNullUid if the data type is not found. 
*/
EXPORT_C void CTypeStoreManager::GetAppByDataType(const TDataType& aDataType, TUid& aUid) const
	{
	GetAppByDataType(aDataType, KOpenServiceUid, aUid);
	}
	
/** Gets the UID of the application mapped to the specified data type.

The service is considered to be the KOpenServiceUid service.

@param aDataType The data type.
@param aServiceUid The UID of the service.
@param aUid On return, the UID of the application associated with the 
data type, or KNullUid if the data type is not found. 
*/
EXPORT_C void CTypeStoreManager::GetAppByDataType(const TDataType& aDataType, 
	TUid aServiceUid, TUid& aUid) const
	{
	TInt i=FindDataMapping(aDataType, aServiceUid);
	aUid=(i==KErrNotFound? KNullUid : iAppMappings[i].iAppUid);
	}

EXPORT_C void CTypeStoreManager::GetDataTypesByAppL(TUid aUid, CArrayFix<TDataType>* aTypeArray) const
/** Populates an array with all the data types supported by the specified application.

If the specified UID is zero, the array is populated with all the data types found in 
the store.

@param aUid An application UID.
@param aTypeArray An empty array. On return, contains all data types supported by the 
application.
@panic APMIME 5 The array is NULL.
@panic APMIME 6 The array passed to the function is not empty. Debug builds only. */
	{
	__ASSERT_ALWAYS(aTypeArray,Panic(EInvalidArgument));
	__ASSERT_DEBUG(!aTypeArray->Count(),Panic(EArrayNotEmpty));
	TInt count=iAppMappings.Count();
	for (TInt i=0; i<count; i++)
		{
		if ((iAppMappings[i].iAppUid==aUid || aUid.iUid==0) &&
			(iAppMappings[i].iServiceUid == KOpenServiceUid))
			{
			aTypeArray->AppendL(iAppMappings[i].iDataType);
			}
		}
	}

EXPORT_C const CArrayFixFlat<TMappingDataTypeToApp>& CTypeStoreManager::MappingArray() const
/** Returns the array of data type mappings.

@return The array of data type mappings. */
	{
	return iAppMappings;
	}

TInt CTypeStoreManager::FindDataMapping(const TDataType& aDataType, 
	const TUid& aServiceUid) const
	{
	TInt count=iAppMappings.Count();
	for (TInt i=0; i<count; i++)
		{
		// Match the pattern at the start of the mime type
		if ((iAppMappings[i].iDataType.Des8().Match(aDataType.Des8())==0) &&
			(iAppMappings[i].iServiceUid == aServiceUid))
			{
			return i;
			}
		}
	return KErrNotFound;
	}

CDictionaryStore* CTypeStoreManager::OpenIniFileLC() const
//	Open dtstor's ini file -  will create a new one if it doesn't exist or is corrupted
	{
	const TPtrC iniFileName(IniFileName());
	TInt err=iFs.MkDirAll(iniFileName);
	if(err!=KErrAlreadyExists)
		{
		User::LeaveIfError(err);
		}
	CDictionaryStore* iniFile=NULL;
	TRAP(err,iniFile=CDictionaryFileStore::OpenL(iFs,iniFileName,KUidDatastor));
	if (err==KErrNone)
		{
		CleanupStack::PushL(iniFile);
		}
	else if (err==KErrEof || err==KErrCorrupt)
		{
		User::LeaveIfError(iFs.Delete(iniFileName));
		iniFile=CDictionaryFileStore::OpenLC(iFs,iniFileName,KUidDatastor);
		err=KErrNone;
		}
	User::LeaveIfError(err);
	return iniFile;
	}

EXPORT_C void CTypeStoreManager::InsertAndStoreDataMappingL(const TDataType& aDataType, 
	TDataTypePriority aPriority, TUid aUid)
	{
	InsertAndStoreDataMappingL(aDataType, aPriority, aUid, KOpenServiceUid);
	}

EXPORT_C void CTypeStoreManager::InsertAndStoreDataMappingL(const TDataType& aDataType, 
	TDataTypePriority aPriority, TUid aUid, TUid aServiceUid)
	{
	InsertDataMappingL(aDataType, aPriority, aUid, aServiceUid);
	TRAPD(ret,StoreL());
	if(ret!=KErrNone)
		{
		DeleteDataMapping(aDataType, aServiceUid);	
		}
	}

EXPORT_C TBool CTypeStoreManager::InsertAndStoreIfHigherL(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid)
	{
	return InsertAndStoreIfHigherL(aDataType, aPriority, aUid, KOpenServiceUid);
	}

EXPORT_C TBool CTypeStoreManager::InsertAndStoreIfHigherL(const TDataType& aDataType, 
	TDataTypePriority aPriority, TUid aUid, TUid aServiceUid)
	{
	TInt i = FindDataMapping( aDataType, aServiceUid );
	if ( i == KErrNotFound || iAppMappings[i].iPriority < aPriority )
		{
		InsertAndStoreDataMappingL( aDataType, aPriority, aUid, aServiceUid );
		return ETrue;
		}
	else
		return EFalse;
	}	

EXPORT_C void CTypeStoreManager::DeleteAndStoreDataMappingL(const TDataType& aDataType)
	{
	DeleteAndStoreDataMappingL(aDataType, KOpenServiceUid);
	}	

EXPORT_C void CTypeStoreManager::DeleteAndStoreDataMappingL(const TDataType& aDataType, 
	TUid aServiceUid)
	{
	TInt i=FindDataMapping(aDataType, aServiceUid);
	__ASSERT_DEBUG(i!=KErrNotFound,User::Invariant());
	TMappingDataTypeToApp mapping (iAppMappings[i].iDataType,iAppMappings[i].iPriority,iAppMappings[i].iAppUid,iAppMappings[i].iServiceUid);
	iAppMappings.Delete(i);
	TRAPD(ret,StoreL());
	if(ret!=KErrNone)
		{
		iAppMappings.InsertL(i,mapping);	
		}
	}

//Removes data mappings related to aAppUid
//Returns modification status of service registry
EXPORT_C TBool CTypeStoreManager::DeleteApplicationDataMappings(const TUid aAppUid)
    {
    TInt count=iAppMappings.Count();
    TInt index=0;
    TBool modified=EFalse;

    //goes through service registry to find data mappings related to aAppUid
    while(index<count)
        {
        if (iAppMappings[index].iAppUid==aAppUid) 
            {
             iAppMappings.Delete(index);
             //As data mapping is removed from service registry,  reduce the count
             count--;
             modified=ETrue;
             }
        else
            index++;
        }
    return(modified);
    }
