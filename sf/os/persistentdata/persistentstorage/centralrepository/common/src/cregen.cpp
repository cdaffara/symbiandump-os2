// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32file.h>
#include "cregen.h"
#include "heaprepos.h"
#include "srvparams.h"

_LIT(KTmpExtension,"tmp");

void CCreGenerator::CommitChangesToCreL(RFs& aFs,TUint8 aPersistVersion,CHeapRepository& aRep,const TDesC& aTargetFilePath)
	{
	HBufC* tmpFilePath=aTargetFilePath.AllocLC();
	TPtr tmpFilePathPtr(tmpFilePath->Des());
	tmpFilePathPtr.Replace(tmpFilePath->Length()-3,3,KTmpExtension());

	CDirectFileStore* store = CDirectFileStore::ReplaceLC(aFs, *tmpFilePath,(EFileWrite | EFileShareExclusive));
	
	const TUid uid2	 = KNullUid ;														 
	store->SetTypeL(TUidType(KDirectFileStoreLayoutUid, uid2, KServerUid3)) ; 

	// Write the stream index/dictionary as root stream within the store
	// so we can access it when we do a restore later on
	RStoreWriteStream rootStream ;
	TStreamId rootStreamId = rootStream.CreateLC(*store) ;
	ExternalizeCre(aPersistVersion,aRep, rootStream) ;
	rootStream.CommitL() ;
		
	CleanupStack::PopAndDestroy(&rootStream) ;
	store->SetRootL(rootStreamId);
	store->CommitL();
	CleanupStack::PopAndDestroy(store) ;	
	User::LeaveIfError(aFs.Replace(*tmpFilePath,aTargetFilePath));
	CleanupStack::PopAndDestroy();
	}

void CCreGenerator::CreateReposFromCreL(RFs& aFs,CHeapRepository& aRep, const TDesC& aFilePath
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	,TUint8& aCreVersion
#endif	
	)
	{
	RFile file;
	TInt err = file.Open(aFs,aFilePath,EFileRead|EFileShareReadersOnly);	
	
	if (err != KErrNone)
		{
		if(err ==KErrNotFound || err==KErrPathNotFound)
			User::Leave(KErrNotFound);
		else
			User::Leave(err);
		}
	
	CleanupClosePushL(file);
	
	CDirectFileStore* store = CDirectFileStore::FromLC (file);
	if(store->Type()[0] != KDirectFileStoreLayoutUid)
		{
		User::Leave(KErrCorrupt);
		}

	// Get the root stream and attempt to read the index from it
	TStreamId rootStreamId = store->Root() ;
	RStoreReadStream rootStream ;
	rootStream.OpenLC(*store, rootStreamId);
	// Internalize the repository
	InternalizeCreL(aRep, rootStream
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	,aCreVersion
#endif	
	);
	CleanupStack::PopAndDestroy(3, &file);
	}

void CCreGenerator::ExternalizeCre(TUint8 aPersistVersion,const CHeapRepository& aRep, RWriteStream& aStream)
	{
	aStream << aPersistVersion;
	aStream << aRep.iUid ;
	aStream << aRep.iOwner ;
	
	TUint32 count=aRep.iSinglePolicies.Count();
	aStream << count;
	for(TUint32 i=0; i<count;i++)
		{
		aStream << *(aRep.iSinglePolicies[i]);
		}
	
	aStream << aRep.iRangePolicies ;
	aStream << aRep.iDefaultPolicy.GetReadAccessPolicy()->Package() ;
	aStream << aRep.iDefaultPolicy.GetWriteAccessPolicy()->Package() ;
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	if (aPersistVersion>=KPersistFormatSupportsIndMetaIndicator)
		{
		aStream << aRep.iDefaultPolicy.HighKey();
		aStream << aRep.iDefaultPolicy.KeyMask();		
		}
#endif		
		
	aStream << aRep.iDefaultMeta ;
	aStream << aRep.iRangeMeta ;
	aStream << aRep.iTimeStamp.Int64() ;

	aStream << aRep.iSettings ;
	
	// Deleted settings
	count = aRep.iDeletedSettings.Count() ;
	aStream << count ;
	for (TUint32 i=0; i<count; i++)
		{
		aStream << aRep.iDeletedSettings[i];
		}
	}

void CCreGenerator::InternalizeCreL(CHeapRepository& aRep, RReadStream& aStream
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	,TUint8& aCreVersion
#endif	
	)
	{
	TUint8 version;
	aStream >> version;
	 
 	// Check the UID is the same as that expected.
 	// UID should always be correctly initialised by the time we reach
 	// this point.
 	TUid tempUid;
 	aStream >> tempUid;
 
 	if (tempUid != aRep.iUid)
 		{
 		#ifdef _DEBUG
 		RDebug::Print(_L("CSharedRepository::InternalizeCreL - expected UID (from filename) - %08X,  UID extracted from binary file - %08X "), aRep.iUid.iUid, tempUid.iUid);
 		#endif
 		User::Leave(KErrCorrupt);
 		}
  
	aStream >> aRep.iOwner ;
 
	TUint32 count;
	aStream >> count;
	for(TUint32 i=0; i<count;i++)
		{
		TSettingsAccessPolicy* singlePolicy = new(ELeave) TSettingsAccessPolicy;
		CleanupStack::PushL(singlePolicy);
		aStream >> *singlePolicy;
		aRep.iSinglePolicies.AppendL(singlePolicy);
		CleanupStack::Pop(singlePolicy);
		}
 	
	aRep.iRangePolicies.Reset();		
	aStream >> aRep.iRangePolicies ;
	
	HBufC8* securityPolicyPackage ;
	securityPolicyPackage = HBufC8::NewLC(aStream, 10000) ;
	TSecurityPolicy defaultReadPolicy;
	defaultReadPolicy.Set(securityPolicyPackage->Des()) ;
	CleanupStack::PopAndDestroy(securityPolicyPackage) ;
	securityPolicyPackage = HBufC8::NewLC(aStream, 10000) ;
	TSecurityPolicy defaultWritePolicy;
	defaultWritePolicy.Set(securityPolicyPackage->Des()) ;
	CleanupStack::PopAndDestroy(securityPolicyPackage) ;
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	aCreVersion=version;
	TUint32 highKey=0;
	TUint32 keyMask=0;
	if (aCreVersion>=KPersistFormatSupportsIndMetaIndicator)
		{
		aStream >> highKey;
		aStream >> keyMask;		
		}
	aRep.iDefaultPolicy=TSettingsAccessPolicy(defaultReadPolicy,defaultWritePolicy, KUnspecifiedKey,highKey,keyMask);		
#else				
	aRep.iDefaultPolicy=TSettingsAccessPolicy(defaultReadPolicy,defaultWritePolicy, KUnspecifiedKey);
#endif	
	aStream >> aRep.iDefaultMeta ;
	
	aRep.iRangeMeta.Reset();
	aStream >> aRep.iRangeMeta ;
	
	TInt64 timeStampInt ;
	aStream >> timeStampInt ;
	aRep.iTimeStamp = timeStampInt ;

	aRep.iSettings.Reset() ;
	aStream >> aRep.iSettings ;
 	
	if (version >= KPersistFormatSupportsDeletedSettings)
		{
		// Deleted Settings 
		aStream >> count  ;
		for (TUint32 i=0; i<count; i++)
			{
			TUint32 keyValue ;
			aStream >> keyValue ;
			aRep.iDeletedSettings.InsertInUnsignedKeyOrderL(keyValue);
			}
		}
 
	// Set up access policies
	TInt numElements = aRep.iSettings.Count();
	for (TInt count = 0; count < numElements; count++)
		{
		TServerSetting* setting= &(aRep.iSettings[count]);
		TUint32 key=setting->Key();
		setting->SetAccessPolicy(aRep.GetFallbackAccessPolicy(key));
		}
	}
