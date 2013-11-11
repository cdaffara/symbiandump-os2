// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SRVRES_H
#define SRVRES_H

#include <f32file.h>
#include "srvPerf.h"
#include "srvdefs.h"

class CRepositoryCacheManager;
class CObservable;

class TOwnerIdMapping
	{
public :
	inline TOwnerIdMapping();
	inline TOwnerIdMapping(TUint32 aRepUid, TUint32 aOwner);
	static TInt CompareUids (const TOwnerIdMapping& aOwnerIdMapping1, const TOwnerIdMapping& aOwnerIdMapping2) ;
	
public:
	TUint32 iRepUid;
	TUint32 iOwner;	
	};



using namespace NCentralRepositoryResources;

class TServerResources
	{
public:
	static void InitialiseL();
	static void Close();
	static TInt GetUid(TEntry& aEntry, TUid& aUid);
	inline static TInt AddOwnerIdLookupMapping(const TUint32 aRepositoryUid, const TUint32 aOwnerUid) ;
	inline static TInt FindOwnerIdLookupMapping(TUint32 aRepositoryId) ;
	static void CreateRepositoryFileNameLC(HBufC*& aFullFileName,
										   TUid aUid,   
										   TCentRepLocation aLocation,
                                           TCentRepFileType aFileType);
	static void CreateRepositoryFileNameL(HBufC*& aFullFileName,
										   TUid aUid,   
										   TCentRepLocation aLocation,
                                           TCentRepFileType aFileType);                                           

	static TBool CentrepFileExistsL(TUid aUid, TCentRepLocation aLocation, TCentRepFileType aType);
	static TBool CentrepFileExistsL(TUid aUid, TCentRepLocation aLocation);
	static TTime CentrepFileTimeStampL(TUid aUid, TCentRepLocation aLocation);
	static TBool RomFileExistsL(TUid aUid);
	static TBool InstallFileExistsL(TUid aUid);
	static TBool PersistsFileExistsL(TUid aUid);
#ifdef SYMBIAN_BAFL_SYSUTIL
	static TInt GetTextFromFile(const TDesC& aFilename,TDes8& aValue);
#endif
	static void  DeleteCentrepFileL(TUid aUid, TCentRepLocation aLocation, TCentRepFileType aType);
public:
	static RFs iFs;
	
	static HBufC* iRomDirectory;
	static HBufC* iDataDirectory;
	static HBufC* iInstallDirectory;
	static HBufC* iBURDirectory;
	static HBufC* iIniExt;
	static HBufC* iCreExt;
	static HBufC* iTrnsExt;
	static TUint8 iPersistsVersion;

	static RArray<TOwnerIdMapping> iOwnerIdLookUpTable;

	static CRepositoryCacheManager* iCacheManager;
	static CObservable* iObserver;
#ifdef __CENTREP_SERVER_PERFTEST__
	static TCentRepPerfTest iPerfTestMgr;
#endif
#ifdef __CENTREP_SERVER_MEMTEST__
	static void RecordTimerResult(TMemTestLocationIdentifier aLocation, TInt32 aIdentifier);
	static void StopRecordTimerResult();	
	static void StartRecordTimerResult();		
	
	static TInt32 iMemTestData[KMemBufMaxEntry];
	static TInt32 iMemTestDataCount;
#endif
	};


#include "srvres.inl"

#endif // SRVRES_H
