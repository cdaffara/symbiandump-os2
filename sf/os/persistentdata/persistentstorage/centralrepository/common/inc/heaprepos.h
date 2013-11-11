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

#ifndef HEAPREPOS_H_
#define HEAPREPOS_H_

#include <e32base.h>
#include "setting.h"
#include "datatype.h"
#include "cregen.h"
#include "inifile.h"

/**
@internalTechnology
*/
class CHeapRepository : public CBase
	{
	friend class CCreGenerator;
	friend class TConvToolTester;
	
public:	
	static CHeapRepository* NewL(TUid aUid);
	~CHeapRepository();
	void ResetContent();	

	//-------------general get/set function-------------------------------------
	TBool IsEmpty();
	inline TUid Uid() const {return iUid;};
	inline void SetUid(TUid aUid){iUid=aUid;};
	inline TUid Owner() {return iOwner ;} ;
	inline void SetOwner(TUid aOwner){iOwner=aOwner;};
	
	inline RSettingsArray& SettingsArray();
	inline RArray<TUint32>& DeletedSettingsArray();
	inline RPointerArray<TSettingsAccessPolicy>& SinglePolicyArray();
	inline RRangePolicyArray& RangePolicyArray();
	inline RDefaultMetaArray& RangeMetaArray();
	inline TUint32 DefaultMeta() {return iDefaultMeta;};
	inline void SetDefaultMeta(TUint32 aMeta){iDefaultMeta=aMeta;};
	
	inline TTime TimeStamp() const {return iTimeStamp;};
	inline void SetTimeStamp(const TTime& aTime){iTimeStamp=aTime;};
	

	//-------------security retrieval function-----------------------------
	static TBool SinglePolicyMatchOnKey(const TSettingsAccessPolicy& aSinglePolicy, const TSettingsAccessPolicy& aSinglePolicyIndexItem);
	static TInt CompareKeyIds(const TSettingsAccessPolicy& aSinglePolicy, const TSettingsAccessPolicy& aSinglePolicyIndexItem);	
	TSettingsAccessPolicy* GetFallbackAccessPolicy(TUint32 aId
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	,TBool aSkipSingle=EFalse
#endif	
	);
	const TSecurityPolicy& GetFallbackReadAccessPolicy(TUint32 aId);
	inline const TSecurityPolicy& GetReadAccessPolicy(const TServerSetting& aSetting);
	inline const TSecurityPolicy& GetReadAccessPolicy(TUint32 aId);
	inline const TSecurityPolicy& GetDefaultReadAccessPolicy();
	const TSecurityPolicy& GetFallbackWriteAccessPolicy(TUint32 aId);
	inline const TSecurityPolicy& GetWriteAccessPolicy(const TServerSetting& aSetting);
	inline const TSecurityPolicy& GetWriteAccessPolicy(TUint32 aId);
	inline const TSecurityPolicy& GetDefaultWriteAccessPolicy();
	inline void SetDefaultPolicy(const TSettingsAccessPolicy& aDefaultPolicy)
		{
		iDefaultPolicy.iReadAccessPolicy=aDefaultPolicy.iReadAccessPolicy;
		iDefaultPolicy.iWriteAccessPolicy=aDefaultPolicy.iWriteAccessPolicy;		
		}
	inline TSettingsAccessPolicy& GetDefaultAccessPolicy()
		{	
		return iDefaultPolicy;
		}

	//-------------file i/o cre read/write and ini read write---------------
	void ExternalizeCre(TUint8 aPersistVersion,RWriteStream& aStream) const;
	void InternalizeCreL(RReadStream& aStream) ;
	TInt CommitChanges(RFs& aFs,TUint8 aPersistVersion,const TDesC& aTargetFilePath);
	void CreateRepositoryFromCreFileL(RFs& aFs,const TDesC& aFilePath);
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	void CreateRepositoryFromCreFileL(RFs& aFs,const TDesC& aFilePath,TUint8& aCreVersion);	
	void InternalizeCreL(RReadStream& aStream,TUint8& aCreVersion)
		{
		CCreGenerator::InternalizeCreL(*this, aStream,aCreVersion);
		}
#endif			
	void ReloadContentL(CIniFileIn& aIniFile);
	TInt ReloadContentExceptSettingsL(CIniFileIn& aIniFile);

	void SetMetaDataOnRead(TServerSetting& aSetting, TBool aSingleMetaFound);
	static void SinglePoliciesCleanup(TAny *aPtr);	 

	TInt ReadSettingSavePolicyL(CIniFileIn& aFile,TServerSetting& aSetting, TSettingsAccessPolicy* &aPolicy, TBool& aSingleMetaFound);	
	TInt Create(TServerSetting& s, TSettingsAccessPolicy* &aPolicy, TBool aSingleMetaFound=EFalse);

	//only used for the conv tool so configured out here using macro	
#ifdef CENTREP_CONV_TOOL	
	void DoCommitChangesToIniFileL(RFs& aFs,const TDesC& aOutFileName
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	,TUint32 aCreVersion
#endif	
	);
#endif
	
private:
	void DoCommitChangesL(RFs& aFs,TUint8 aPersistVersion,const TDesC& aFilePath);
	CHeapRepository(TUid aUid);
		
private:
	enum {KGranularity=8};
	RSettingsArray iSettings;
	TUid iUid;
	TUid iOwner;
		
	//security policy
	TSettingsAccessPolicy iDefaultPolicy;	
	RPointerArray<TSettingsAccessPolicy> iSinglePolicies;
	RRangePolicyArray iRangePolicies;
	
	//meta data
	TUint32 iDefaultMeta ;
	RDefaultMetaArray iRangeMeta ;
	
	// Keep track of deleted keys so that deletions can be propagated across
	// a Backup/Restore cycle when backup data is merged with initialisation
	// file during restore process.
	RArray<TUint32> iDeletedSettings ;
	TTime iTimeStamp;
	};

#include "heaprepos.inl"

#endif /*HEAPREPOS_H_*/
