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

#include <e32test.h>
#include <f32file.h>
#include <e32std.h>
#include "heaprepos.h"

#ifdef __TOOLS2__
#define CENREP_PC_TEST
#endif

using namespace NCentralRepositoryConstants;

RTest TheTest(_L("Central Repository Tests"));

RFs TheFs;

const TUid uid = {0x000001ff};
const TUid testUid = {0x000003ff};
const TUid owner = {0x12345};

enum {EIniFromTxt, EIniFromCre};

#ifndef __TOOLS2__
	_LIT(KCopyTxtFile,"c:\\private\\00000000\\copy000001ff.txt");
	_LIT(KCreFile,"c:\\private\\00000000\\000001ff.cre");
	_LIT(KTxtFile,"c:\\private\\00000000\\000001ff.txt");
#else
	//in TOOLS2 this is relative
	_LIT(KCopyTxtFile,"copy000001ff.txt");
	_LIT(KCreFile,"000001ff.cre");
	_LIT(KTxtFile,"000001ff.txt");
#endif
	
	
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macroses and functions

LOCAL_C void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest(EFalse, aLine);
		}
	}
LOCAL_C void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

TInt CopyFile(const TDesC& aSource, const TDesC& aTarget)
	{
	RFile file;
	TInt ret=file.Open(TheFs,aSource,EFileRead);
	if (ret!=KErrNone)
		return ret;
	TInt fileSize;
	file.Size(fileSize);
	HBufC8* buf=HBufC8::New(fileSize);
	if (!buf)
		{
		file.Close();
		return KErrNoMemory;
		}
	TPtr8 mod(buf->Des());
	file.Read(mod);
	file.Close();
	ret=file.Replace(TheFs,aTarget,EFileWrite);
	if (ret==KErrNone)
		{
		file.Write(*buf);
		}
	file.Close();
	delete buf;
	return ret;
	}

void CreateFreshCreL()
	{
	TInt err = CopyFile(KCopyTxtFile, KTxtFile);
	User::LeaveIfError(err);
	
	CHeapRepository* repository = CHeapRepository::NewL(uid);
	CleanupStack::PushL(repository);
	
	CIniFileIn* iniFile = NULL;
	err = CIniFileIn::NewLC(TheFs,iniFile,KTxtFile);
	User::LeaveIfError(err);
	repository->ReloadContentL(*iniFile);
	CleanupStack::PopAndDestroy(); // iniFile
	
	err = repository->CommitChanges(TheFs, KPersistFormatVersion, KCreFile);
	User::LeaveIfError(err);
	
	CleanupStack::PopAndDestroy(); // repository
	}

void InitializeL(TInt aIniMode, CHeapRepository* aRep)
	{
	if (aIniMode == EIniFromTxt)
		{
		TInt err = CopyFile(KCopyTxtFile, KTxtFile);
		User::LeaveIfError(err);
		
		CIniFileIn* iniFile = NULL;
		err = CIniFileIn::NewLC(TheFs,iniFile,KTxtFile);
		User::LeaveIfError(err);
		aRep->ReloadContentL(*iniFile);
		CleanupStack::PopAndDestroy(); // iniFile
		}
	else
		{
		CreateFreshCreL();
		TBufC<50> filename(KCreFile);
		aRep->CreateRepositoryFromCreFileL(TheFs, filename);
		}
	}

TBool CheckPolicy(const TSecurityPolicy& aPolicy, TCapability aCapibility)
	{
	TSecurityPolicy policy (aCapibility, ECapability_None, ECapability_None, ECapability_None);
	TPtrC8 pkg(policy.Package());
	TInt ret = pkg.Compare(aPolicy.Package());
	return (ret == 0);
	}

TBool CheckPolicy(const TSecurityPolicy& aPolicy, TSecurityPolicy::TSecPolicyType aSecPolicyType)
	{
	TSecurityPolicy policy (aSecPolicyType);
	TPtrC8 pkg(policy.Package());
	TInt ret = pkg.Compare(aPolicy.Package());
	return (ret == 0);
	}

void GetSetFunctions(CHeapRepository* aRep)
	{
	TBool gotSomeSettings = !(aRep->IsEmpty());
	TEST(gotSomeSettings);
	
	TUid repUid = aRep->Uid();
	TEST(repUid == uid);
	aRep->SetUid(testUid);
	repUid = aRep->Uid();
	TEST(repUid == testUid);
	aRep->SetUid(uid); // set back
	
	TUid repOwner = aRep->Owner();
	TEST(repOwner == owner);
	aRep->SetOwner(testUid);
	repOwner = aRep->Owner();
	TEST(repOwner == testUid);
	aRep->SetOwner(owner); // set back
	
	TUint32 defaultMeta = aRep->DefaultMeta();
	TEST(defaultMeta == 0x10);
	TUint32 meta = 0x100;
	aRep->SetDefaultMeta(meta);
	defaultMeta = aRep->DefaultMeta();
	TEST(defaultMeta == 0x100);
	aRep->SetDefaultMeta(0x10); //set back
	
	TTime timeStamp = aRep->TimeStamp();
	TEST(timeStamp == 0);
	aRep->SetTimeStamp(100);
	timeStamp = aRep->TimeStamp();
	TEST(timeStamp == 100);
	aRep->SetTimeStamp(0); //set back
	
	RSettingsArray& settings = aRep->SettingsArray();
	TEST2(settings.Count(), 33);
	
	RArray<TUint32>& deletedSettings = aRep->DeletedSettingsArray();
	TEST2(deletedSettings.Count(), 0);

	RPointerArray<TSettingsAccessPolicy>& singlePolicy = aRep->SinglePolicyArray();
	TEST2(singlePolicy.Count(), 4);
	
	RRangePolicyArray& rangePolicy = aRep->RangePolicyArray();
	TEST2(rangePolicy.Count(), 4);
	
	RDefaultMetaArray& rangeMeta = aRep->RangeMetaArray();
	TEST2(rangeMeta.Count(), 2);
	}

void SecurityFunctions(CHeapRepository* aRep)
	{
	// 1. Get read/write Policy of a setting
	TUint32 key = 1;
	TServerSetting* setting = aRep->SettingsArray().Find(key);
	// Seems CentRep is using TypeC7 policy.
	TSecurityPolicy readPolicy (ECapabilityLocalServices, ECapability_None, ECapability_None, ECapability_None); 
	TSecurityPolicy writePolicy (ECapabilityLocalServices, ECapability_None, ECapability_None, ECapability_None);
	TSettingsAccessPolicy accessPolicy (readPolicy, writePolicy, key);
	setting->SetAccessPolicy(&accessPolicy);
	
	// Set policy for single setting, will not fall back to default.
	TSecurityPolicy policy = aRep->GetReadAccessPolicy(key);
	TEST(CheckPolicy(policy, ECapabilityLocalServices));
	policy = aRep->GetReadAccessPolicy(*setting);
	TEST(CheckPolicy(policy, ECapabilityLocalServices));
	policy = aRep->GetWriteAccessPolicy(key);
	TEST(CheckPolicy(policy, ECapabilityLocalServices));
	policy = aRep->GetWriteAccessPolicy(*setting);
	TEST(CheckPolicy(policy, ECapabilityLocalServices));
	
	// Policy for single setting not set, will fall back to default.
	policy = aRep->GetReadAccessPolicy(2);
	TEST(CheckPolicy(policy, ECapabilityReadDeviceData));
	policy = aRep->GetWriteAccessPolicy(2);
	TEST(CheckPolicy(policy, ECapabilityWriteDeviceData));
	
	// 2. Get read/write default policy
	policy = aRep->GetDefaultReadAccessPolicy();
	TEST(CheckPolicy(policy, ECapabilityReadDeviceData));
	policy = aRep->GetDefaultWriteAccessPolicy();
	TEST(CheckPolicy(policy, ECapabilityWriteDeviceData));
	
	// 3. Get fallback read/write default policy
	// Get Read policy:
	// have single policy, fallback to single
	policy = aRep->GetFallbackReadAccessPolicy(6); 	
	TEST(CheckPolicy(policy, ECapabilityNetworkServices)); 
	
	// have single and range policy, fallback to single
	policy = aRep->GetFallbackReadAccessPolicy(0x202); 
	TEST(CheckPolicy(policy, ECapabilityDRM)); 
	
	// only have range policy, fallback to range
	policy = aRep->GetFallbackReadAccessPolicy(0x201); 
	TEST(CheckPolicy(policy, TSecurityPolicy::EAlwaysPass));
	
	// have no policy set, fallback to default
	policy = aRep->GetReadAccessPolicy(2); 
	TEST(CheckPolicy(policy, ECapabilityReadDeviceData));
	
	// Get Read policy:
	// have single policy, fallback to single
	policy = aRep->GetFallbackWriteAccessPolicy(0x11);
	TEST(CheckPolicy(policy, ECapabilityLocalServices));
	
	// have range policy, fallback to range
	policy = aRep->GetFallbackWriteAccessPolicy(0x101);
	TEST(CheckPolicy(policy, ECapabilityNetworkServices));
	
	// have no policy set, fallback to default
	policy = aRep->GetFallbackWriteAccessPolicy(2);
	TEST(CheckPolicy(policy, ECapabilityWriteDeviceData));
	
	// 4. Get Access policy
	TSettingsAccessPolicy* accPlc = aRep->GetFallbackAccessPolicy(0x202);
	const TSecurityPolicy* readSecPlc = accPlc->GetReadAccessPolicy();
	TEST(CheckPolicy(*readSecPlc, ECapabilityDRM)); // Single
//	const TSecurityPolicy* writeSecPlc = accPlc->GetWriteAccessPolicy();
// 	Defect found here, need to be fixed later.	
//	TEST(CheckPolicy(*writeSecPlc, TSecurityPolicy::EAlwaysPass)); // range
	
	aRep->SetDefaultPolicy(accessPolicy);
	policy = aRep->GetDefaultReadAccessPolicy();
	TEST(CheckPolicy(policy, ECapabilityLocalServices));
	policy = aRep->GetDefaultWriteAccessPolicy();
	TEST(CheckPolicy(policy, ECapabilityLocalServices));
	}

void OtherOperationsL(CHeapRepository* aRep)
	{
	// Three functions has been tested in InitializeL() and CreateFreshCreL():
	// TInt CommitChanges(RFs&, TUint8, const TDesC&);
	// void CreateRepositoryFromCreFileL(RFs&, TDesC&);
	// TInt ReloadContentL(CIniFileIn&);
	
	// Three functions has been indirectly tested.
	// void ExternalizeCre(TUint8, RWriteStream&) const;
	// void InternalizeCreL(RReadStream&);
	
	// Rest of functions
	// 1. ReloadContentExceptSettingsL
	TInt err = CopyFile(KCopyTxtFile, KTxtFile);
	User::LeaveIfError(err);
	
	CIniFileIn* iniFile = NULL;
	err = CIniFileIn::NewLC(TheFs,iniFile,KTxtFile);
	User::LeaveIfError(err);
	err = aRep->ReloadContentExceptSettingsL(*iniFile);
	User::LeaveIfError(err);
	
	TUid repUid = aRep->Uid();
	TEST(repUid == uid);
	
	TUid repOwner = aRep->Owner();
	TEST(repOwner == owner);
	
	TUint32 defaultMeta = aRep->DefaultMeta();
	TEST(defaultMeta == 0x10);
	
	TTime timeStamp = aRep->TimeStamp();
	TEST(timeStamp == 0);
	
	RSettingsArray& settings = aRep->SettingsArray();
	TEST2(settings.Count(), 0);
	RPointerArray<TSettingsAccessPolicy>& singlePolicy = aRep->SinglePolicyArray();
	TEST2(singlePolicy.Count(), 0);
	RRangePolicyArray& rangePolicy = aRep->RangePolicyArray();
	TEST2(rangePolicy.Count(), 4);
	RDefaultMetaArray& rangeMeta = aRep->RangeMetaArray();
	TEST2(rangeMeta.Count(), 2);
	
	// 2. ReadSettingSavePolicyL

	TServerSetting setting;
	TSettingsAccessPolicy* policy = NULL;
	TBool singleMetaFound;
	
	// read 5 settings
	for (TInt i = 0; i < 5 ; i++)
		{
		TInt err = aRep->ReadSettingSavePolicyL(*iniFile, setting, policy, singleMetaFound);
		setting.Reset();
		User::LeaveIfError(err);
		}
	
	CleanupStack::PopAndDestroy(); // iniFile
	TEST2(singlePolicy.Count(), 2);
	
	// 3. Test for Create() and SetMetaDataOnRead tested indirectly
	TSecurityPolicy readPolicy (ECapabilityLocalServices, ECapability_None, ECapability_None, ECapability_None); 
	TSecurityPolicy writePolicy (ECapabilityLocalServices, ECapability_None, ECapability_None, ECapability_None);
	
	TServerSetting setting1 (0x205);
	setting1.SetType(TServerSetting::EInt);
	setting1.SetIntValue(99);
	TSettingsAccessPolicy* policy1 = new(ELeave) TSettingsAccessPolicy(readPolicy, writePolicy, 0x205);
	aRep->Create(setting1, policy1);
	
	TEST2(settings.Count(), 1);
	TEST2(setting1.Meta(), 0x20);
	
	TServerSetting setting2 (0x206);
	setting2.SetType(TServerSetting::EReal);
	TReal* temp = new(ELeave) TReal(20.0);
	setting2.SetRealValue(temp);
	temp = NULL;
	setting2.SetMeta(0x30);
	TSettingsAccessPolicy* policy2 = new(ELeave) TSettingsAccessPolicy(readPolicy, writePolicy, 0x206);
	aRep->Create(setting2, policy2, ETrue);
	
	TEST2(settings.Count(), 2);
	TEST2(setting2.Meta(), 0x30);
	
	delete policy1;
	delete policy2;
	
	}

void DoTestL(TInt aIniMode)
	{
	__UHEAP_MARK;
	
	CHeapRepository* rep = CHeapRepository::NewL(uid);
	CleanupStack::PushL(rep);
	
	InitializeL(aIniMode, rep);
	
	GetSetFunctions(rep);
	SecurityFunctions(rep);
	
	CleanupStack::PopAndDestroy();
	
	rep = CHeapRepository::NewL(uid);
	CleanupStack::PushL(rep);
	
	OtherOperationsL(rep);
	
	CleanupStack::PopAndDestroy();

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-UT-4045
@SYMTestCaseDesc		Unit Test for CHeapRepository
@SYMTestPriority		High
@SYMTestActions			Test input and output of every single functions of CHeapRepository directly or indirectly.
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2111
*/	
				   
LOCAL_C void MainL()
	{
	//Note these files are equivalent, the cre being generated from txt
	TheTest.Start(_L("Central Repository Unit Testing"));		

	User::LeaveIfError(TheFs.Connect());
	DoTestL(EIniFromTxt);
	DoTestL(EIniFromCre);

	TheFs.Close();
	TheTest.End();
	TheTest.Close();
	}

TInt E32Main()
	{	
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	TRAPD(err, MainL());
	if (err != KErrNone)
		User::Panic(_L("Testing failed: "), err);
	
	delete cleanup;
	__UHEAP_MARKEND;

	return 0;
	}
