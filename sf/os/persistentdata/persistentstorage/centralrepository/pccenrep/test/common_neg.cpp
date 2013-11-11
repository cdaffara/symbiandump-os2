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

#ifdef CENREP_PC_TEST
	#include <x86tool/centralrepository.h>
#else
	#include <centralrepository.h>
#endif

#include <e32test.h>
#include <f32file.h>

enum TestMode{
	ETxt,
	ECre,
	EAutoCre,
	EAutoTxt
};

//DEFINED IN THE TEST MAIN CPP
extern void SetupEnv(const TDesC& aInFilePath,const TDesC& aOutFilePath,TUint aTestMode);
extern RFs TheFs;
extern RTest TheTest;

using namespace NCentralRepositoryConstants;

_LIT(KLongBuf16,"sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss\
	ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss\
	ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss\
	ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss\
	ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss\
	ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss\
	ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss\
	ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss\
	ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss\
	ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss\
	ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss\
	ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss");


////////////////////////////////////////////////////////////////////////////////
// Test macro and function.

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

////////////////////////////////////////////////////////////////////////////////

/**
NEWL Negative Testing
*/
void NewL(TUint aTestMode)
	{
	CRepository* repos=NULL;
	//Negative testing to do with initialising the repository
	
	//-repository file not found
	TUid nonexistentRepository={0x88888888};
	TRAPD(err,repos=CRepository::NewL(nonexistentRepository));
	TEST2(err, KErrNotFound);
	
	//-repository file corrupt
	TUid corruptRepositoryUid;
	if (aTestMode==EAutoCre)
		corruptRepositoryUid=TUid::Uid(0x88888881);
	else
		corruptRepositoryUid=TUid::Uid(0x88888880);
	TRAP(err,repos=CRepository::NewL(corruptRepositoryUid));
	TEST2(err, KErrCorrupt);
	
	delete repos;
	}


void GetFunction(CRepository* aRepository)
	{
	TInt error;
	TInt intVal;
	TReal realVal;
	TBuf<50> longStr;
	
	//GENERAL
	//-Setting does not exist
	error = aRepository->Get(100, intVal);
	TEST2(error, KErrNotFound);
	error = aRepository->Get(100, realVal);
	TEST2(error, KErrNotFound);
	error = aRepository->Get(100, longStr);
	TEST2(error, KErrNotFound);
	
	//-Setting exist but not requested type(mismatch)
	//INT
	error = aRepository->Get(1, realVal);
	TEST2(error, KErrArgument);
	error = aRepository->Get(1, longStr);
	TEST2(error, KErrArgument);
	
	//REAL
	error = aRepository->Get(2, intVal);
	TEST2(error, KErrArgument);
	error = aRepository->Get(2, longStr);
	TEST2(error, KErrArgument);
	
	//DESC8,DESC16
	error = aRepository->Get(0x300, intVal);
	TEST2(error, KErrArgument);
	error = aRepository->Get(0x300, realVal);
	TEST2(error, KErrArgument);
	
	//-The buffer allocated too small
	TBuf8<4> short8BitStr;
	error = aRepository->Get(11, short8BitStr);
	TEST2(error, KErrOverflow);
	//-The bufer allocated is ok
	TBuf8<12> good8BitStr;
	error = aRepository->Get(11, good8BitStr);
	TEST2(error, KErrNone);
	//-The buffer allocated is too big(not a problem)
	TBuf8<50> long8BitStr;
	error = aRepository->Get(11, long8BitStr);
	TEST2(error, KErrNone);
	//DESC8,DESC16(with actual length)
	//-The buffer allocated too small
	TBuf<4> shortStr;
	error = aRepository->Get(0x300, shortStr);
	TEST2(error, KErrOverflow);
	//-The bufer allocated is ok
	TBuf<11> goodStr;
	error = aRepository->Get(0x300, goodStr);
	TEST2(error, KErrNone);
	//-The buffer allocated is too big(not a problem)
	error = aRepository->Get(0x300, longStr);
	TEST2(error, KErrNone);
	}
	
void GetMeta(CRepository* aRepository)
	{
	//-Setting does not exist	
	TUint32 meta;
	TInt error;
	error = aRepository->GetMeta(100, meta);
	TEST2(error, KErrNotFound);
	}

void FindFunctionL(CRepository* aRepository)
	{
	RArray<TUint32> keyList;
	RArray<TUint32> shortKeyList(1);
	CleanupClosePushL(keyList);
	CleanupClosePushL(shortKeyList);
	
	TInt error;
	
	//FindL
	//-Partial setting list requested not found(based on partialid and mask)
	error = aRepository->FindL(0x0000FFFF, 0x0000F000, keyList);
	TEST2(error, KErrNotFound);
	
	//-purposely initially allocated a array with small size(force it to grow)
	error = aRepository->FindL(0xFFFFFFFF, 0x0, shortKeyList);
	TEST2(error, KErrNone);
	TEST2(shortKeyList.Count(), 33);
	
	//FindEqL/FindNeqL
	keyList.Reset();
	error = aRepository->FindEqL(0xFFFFFFFF, 0, 199, keyList);
	TEST2(error, KErrNotFound);

	//GENERAL
	//-Partial setting list requested not found(based on partialid and mask)
	keyList.Reset();
	error = aRepository->FindEqL(0x0000FFFF, 0x0000F000, 100, keyList);
	TEST2(error, KErrNotFound);
	keyList.Reset();
	error = aRepository->FindNeqL(0x0000FFFF, 0x0000F000, 100, keyList);
	TEST2(error, KErrNotFound);
	
	//-Partial setting list found but some setting not the TYPE as requested(ensure not returned)
	//INT
	keyList.Reset();
	error = aRepository->FindEqL(0x00100000, 0xFFFFF0FF, 88, keyList);
	TEST2(error, KErrNone);
	TEST2(keyList.Count(), 2);
	
	keyList.Reset();
	error = aRepository->FindNeqL(0x00100000, 0xFFFFF0FF, 88, keyList);
	TEST2(error, KErrNone);
	TEST2(keyList.Count(), 3);
	
	//REAL
	keyList.Reset();
	error = aRepository->FindEqL(0x00100000, 0xFFFFF0FF, 88.0, keyList);
	TEST2(error, KErrNone);
	TEST2(keyList.Count(), 2);
	
	keyList.Reset();
	error = aRepository->FindNeqL(0x00100000, 0xFFFFF0FF, 88.0, keyList);
	TEST2(error, KErrNone);
	TEST2(keyList.Count(), 3);

	_LIT(KString16, "88");
	
	keyList.Reset();
	error = aRepository->FindEqL(0x00100000, 0xFFFFF0FF, KString16, keyList);
	TEST2(error, KErrNone);
	TEST2(keyList.Count(), 1);
	
	keyList.Reset();
	error = aRepository->FindNeqL(0x00100000, 0xFFFFF0FF, KString16, keyList);
	TEST2(error, KErrNone);
	TEST2(keyList.Count(), 4);
	
	_LIT(KString8, "88");
	
	keyList.Reset();
	error = aRepository->FindEqL(0x00100000, 0xFFFFF0FF, KString8, keyList);
	TEST2(error, KErrNone);
	TEST2(keyList.Count(), 1);
	
	keyList.Reset();
	error = aRepository->FindNeqL(0x00100000, 0xFFFFF0FF, KString8, keyList);
	TEST2(error, KErrNone);
	TEST2(keyList.Count(), 4);
	
	//-A value specified greater than the max limit allowed KMaxBinaryLength(8 bit) and KMaxUnicodeStringLength(16 bit)
	TBuf8<KMaxUnicodeStringLength*3> KLongBuf8;
	KLongBuf8.Copy(KLongBuf16);
	KLongBuf8.Append(KLongBuf16);

	keyList.Reset();
	error = aRepository->FindEqL(0xFFFFFFFF, 0, KLongBuf8, keyList);
	TEST2(error, KErrNotFound);
	keyList.Reset();
	error = aRepository->FindNeqL(0xFFFFFFFF, 0, KLongBuf8, keyList);
	TEST2(error, KErrNone);
	TEST2(shortKeyList.Count(), 33);
	
	keyList.Reset();
	error = aRepository->FindEqL(0xFFFFFFFF, 0, KLongBuf16, keyList);
	TEST2(error, KErrNotFound);
	keyList.Reset();
	error = aRepository->FindNeqL(0xFFFFFFFF, 0, KLongBuf16, keyList);
	TEST2(error, KErrNone);
	TEST2(shortKeyList.Count(), 33);
	
	//-A value specified empty
	_LIT8(KEmptyStr8, "");
	keyList.Reset();
	error = aRepository->FindEqL(0xFFFFFFFF, 0, KEmptyStr8, keyList);
	TEST2(error, KErrNotFound);
	keyList.Reset();
	error = aRepository->FindNeqL(0xFFFFFFFF, 0, KEmptyStr8, keyList);
	TEST2(error, KErrNone);
	TEST2(shortKeyList.Count(), 33);
	
	_LIT16(KEmptyStr16, "");
	keyList.Reset();
	error = aRepository->FindEqL(0xFFFFFFFF, 0, KEmptyStr16, keyList);
	TEST2(error, KErrNotFound);
	keyList.Reset();
	error = aRepository->FindNeqL(0xFFFFFFFF, 0, KEmptyStr16, keyList);
	TEST2(error, KErrNone);
	TEST2(shortKeyList.Count(), 33);
	
	CleanupStack::PopAndDestroy(2);
	}

void SetFunction(CRepository* aRepository)
	{
	//----------SET
	//GENERAL:
	//-Setting exist but not requested type(mismatch)
	//-If setting does not exist, it creates one(also check the expected meta)
	//INT
	//REAL
	//DESC8,DESC16
	//-Try to set a setting that is greater than KMaxBinaryLength(8 bit) and KMaxUnicodeStringLength(16 bit)
	//-Try to set a setting with empty string content(is this a problem)
		
	TInt intVal=0;
	TReal realVal=0;
	TUint32 metaVal=0;
	
	//INT
	TInt err=aRepository->Set(0x2,intVal);
	TEST2(err, KErrArgument);
	err=aRepository->Set(0x888,888);
	TEST2(err, KErrNone);
	err=aRepository->Get(0x888,intVal);
	TEST2(intVal, 888);
	//will use default one
	err=aRepository->GetMeta(0x888,metaVal);
	TEST2(metaVal, 0x10);
	
	//REAL
	err=aRepository->Set(0x1,realVal);
	TEST2(err, KErrArgument);
	err=aRepository->Set(0x333,333.33);
	TEST2(err, KErrNone);
	err=aRepository->Get(0x333,realVal);
	TEST(realVal == 333.33);
	//will use range one
	err=aRepository->GetMeta(0x333,metaVal);
	TEST2(metaVal, 0x20);
	
	//DESC8,DESC16
	_LIT8(KBuf8,"full");
	err=aRepository->Set(0x1,KBuf8());
	TEST2(err, KErrArgument);
	_LIT(KBuf16,"full");
	err=aRepository->Set(0x1,KBuf16());
	TEST2(err, KErrArgument);
	err=aRepository->Set(0x111,KBuf16());
	TEST2(err, KErrNone);
	TBuf16<255> buffer16;
	err=aRepository->Get(0x111,buffer16);
	TEST2(err, KErrNone);
	TEST2(buffer16.Compare(KBuf16()), 0);
	err=aRepository->GetMeta(0x111,metaVal);
	//will use range one
	err=aRepository->GetMeta(0x111,metaVal);
	TEST2(metaVal, 0x20);	
	
	//Set an existing string setting with string longer than limit(KMaxUnicodeStringLength)
	TBuf8<KMaxUnicodeStringLength*3> KLongBuf8;
	KLongBuf8.Copy(KLongBuf16());
	KLongBuf8.Append(KLongBuf16());

	//try it on existing string setting
	err=aRepository->Set(0x222,KLongBuf16());
	TEST2(err, KErrArgument);
	err=aRepository->Set(0x222,KLongBuf8);
	TEST2(err, KErrArgument);
	
	//set an empty string content
	_LIT8(KEmptyString8,"");
	err=aRepository->Set(0x444,KEmptyString8());
	TEST2(err, KErrNone);
	_LIT16(KEmptyString16,"");
	err=aRepository->Set(0x555,KEmptyString16());
	TEST2(err, KErrNone);
	}

void CreateFunction(CRepository* aRepository)
	{
	//----------CREATE
	//GENERAL:
	//-Setting already exist
	//INT
	//REAL
	//DESC8,DESC16
	//-Try to create a setting that is greater than KMaxBinaryLength(8 bit) and KMaxUnicodeStringLength(16 bit)
	//-Try to set a setting with empty string content(is this a problem)
	TInt intVal = 0;
	TReal realVal = 0.0;

	//INT
	TInt err=aRepository->Create(1,intVal);
	TEST2(err, KErrAlreadyExists);
	//REAL
	err=aRepository->Create(2,realVal);
	TEST2(err, KErrAlreadyExists);
	//DESC8,DESC16
	_LIT8(KString8,"string");
	_LIT16(KString16,"string");	
	err=aRepository->Create(5,KString8);
	TEST2(err, KErrAlreadyExists);
	err=aRepository->Create(5,KString16);
	TEST2(err, KErrAlreadyExists);		
	
	//empty string
	_LIT8(KEmptyString8,"");
	_LIT8(KEmptyString16,"");
	err=aRepository->Create(0x1111,KEmptyString8());
	TEST2(err, KErrNone); 
	err=aRepository->Create(0x2222,KEmptyString16());
	TEST2(err, KErrNone);
	
	//Create an existing string setting with string longer than limit(KMaxUnicodeStringLength)
	TBuf8<KMaxUnicodeStringLength*3> KLongBuf8;
	KLongBuf8.Copy(KLongBuf16());
	KLongBuf8.Append(KLongBuf16());

	err=aRepository->Create(0x3333,KLongBuf16());
	TEST2(err, KErrArgument);
	err=aRepository->Create(0x4444,KLongBuf8);
	TEST2(err, KErrArgument);
	}

void DeleteFunction(CRepository* aRepository)
	{
	//----------DELETE SINGLE
	//-Setting does not exist
	//-Delete a setting and then recreate,see original meta data preserved(vary with default,range,single)
	//-When deleted always verify setting does not exist
	TInt err=aRepository->Delete(0x757);
	TEST2(err, KErrNotFound);
	
	//delete and existing setting with single meta, then recreate it, expect same meta before deletion
	TReal realVal;
	TUint32 metaVal;
	//this is the original setting meta
	err=aRepository->GetMeta(2,metaVal);
	TEST2(metaVal, 0xa);
	err=aRepository->Delete(2);
	TEST2(err, KErrNone);
	err=aRepository->Get(2,realVal);
	TEST2(err, KErrNotFound);
	err=aRepository->Create(2,realVal);
	TEST2(err, KErrNone);
	TUint32 newMetaVal;
	err=aRepository->GetMeta(2,newMetaVal);
	TEST2(err, KErrNone);
	TEST2(newMetaVal, metaVal);
	
	
	//----------DELETE RANGE
	//-Partial setting list requested not found(based on partialid and mask)
	//-Check the aErrorKey returned with each of the negative testing.
	TUint32 error;
	err=aRepository->Delete(0x0F010000,0xFFFFF0FF,error);
	TEST2(err, KErrNotFound);
	//if not found, the error key here will be set to the partial ID
	TEST2(error, 0x0F010000);

	}

void MoveFunction(CRepository* aRepository)
	{
	//-Partial setting list requested not found(based on partialid and mask)
	//-Intended target setting already exist(say 1 out of X)
	TUint32 errorId;	
	TInt err=aRepository->Move(0x0F010000,0x0E010000,0xFFFFF0FF,errorId);
	TEST2(err, KErrNotFound);
	TEST2(errorId, 0x0F010000);

	
	err=aRepository->Move(0x03010000,0x02010000,0xFFFFF0FF,errorId);
	TEST2(err, KErrAlreadyExists);
	//errorId will be set to the first id of the target when error is encountered
	TEST2(errorId, 0x02010100);
	}

/**
*/
void NegativeTestingL(TUid aUid,const TDesC& aInFile,const TDesC& aOutFile,TUint aTestMode)
	{
	SetupEnv(aInFile,aOutFile,aTestMode);
	
	__UHEAP_MARK;
	
	//repository initialising negative testing
	NewL(aTestMode);
	
	//ok repository but operation negative testing
	CRepository* repository=CRepository::NewLC(aUid);

	//these are all negative testing specifically each of the error code
	GetFunction(repository);
	GetMeta(repository);
	FindFunctionL(repository);
	SetFunction(repository);
	CreateFunction(repository);
	DeleteFunction(repository);
	MoveFunction(repository);

	CleanupStack::PopAndDestroy();	
	
	__UHEAP_MARKEND;
	}

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
