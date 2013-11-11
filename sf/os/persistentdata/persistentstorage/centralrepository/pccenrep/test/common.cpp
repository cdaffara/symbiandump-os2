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

#include <f32file.h>
#include "../../test/t_cenrep_helper.h"
#include <e32test.h>

#ifdef __TOOLS2__
#define CENREP_PC_TEST
#endif

#ifdef CENREP_PC_TEST
	#include <x86tool/centralrepository.h>
#else
	#include <centralrepository.h>
#endif

_LIT( KCentralRepositoryServerName, "Centralrepositorysrv");


//DEFINED IN THE TEST MAIN CPP
extern void SetupEnv(const TDesC& aInFilePath,const TDesC& aOutFilePath,TUint aTestMode);
extern void InitialiseLC(CRepository*& aRepository,TUid aUid,const TDesC& aInFilePath,const TDesC& aOutFilePath,TUint aTestMode);
extern RFs TheFs;
extern RTest TheTest;

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

///////////////////////////////////////////////////////////////////////////////////////
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

void OomTest(void (*testFuncL)(CRepository* aRepository),TUid aUid,const TDesC& aInFilePath,const TDesC& aOutFilePath,TUint aTestMode)
	{	
	TInt error;
	TInt count = 0;

	do
		{
		SetupEnv(aInFilePath,aOutFilePath,aTestMode);
		//for CRE testing we need to ensure we have a fresh copy of CRE(sourced from the TXT template)

		__UHEAP_MARK;
		CRepository* repository=NULL;
		
#ifndef CENREP_PC_TEST
		//for CS testing, we want to kill server to start with fresh repos, otherwise might still use
		//cache version from previous test		
		KillProcess(KCentralRepositoryServerName);		
#endif		
		InitialiseLC(repository,aUid,aInFilePath,aOutFilePath,aTestMode);
		if (repository)
			CleanupStack::Pop();		
		// This is supported by symuser but still have problem, so skiped
//		User::__DbgSetAllocFail(RHeap::EUser, RHeap::EDeterministic, ++count);

		
  		// find out the number of open handles
  		// TOOLS2 somehow not supporting RThread().HandleCount()?
#ifndef __TOOLS2__  		
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);
#endif
		
		TRAP(error, (testFuncL)(repository));
		
		// check that no handles have leaked
#ifndef __TOOLS2__		
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);
		
		TEST2(endProcessHandleCount, startProcessHandleCount);
		TEST2(endThreadHandleCount, startThreadHandleCount);
#endif
//		User::__DbgSetAllocFail(RHeap::EUser, RHeap::ENone, 1);
		
		delete repository;
		
		__UHEAP_MARKEND;
		
		} while(error == KErrNoMemory);
	_LIT(KTestFailed, "Out of memory test failure on iteration %d\n");
	__ASSERT_ALWAYS(error==KErrNone, TheTest.Panic(error, KTestFailed, count));
 	}

void ObjectCreateDeleteOOM(TUid aUid,const TDesC& aInFilePath,const TDesC& aOutFilePath,TUint aTestMode)
	{
//	TInt count=0;
	TInt error=KErrNone;
	do
		{
		SetupEnv(aInFilePath,aOutFilePath,aTestMode);
		
		__UHEAP_MARK;
#ifndef __TOOLS2__		
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);
#endif		
		// This is supported by symuser but still has problems, so skipped.
//		User::__DbgSetAllocFail(RHeap::EUser, RHeap::EDeterministic, ++count);
		
		CRepository* repository=NULL;
		
		TRAP(error,InitialiseLC(repository,aUid,aInFilePath,aOutFilePath,aTestMode);CleanupStack::Pop());
		
		delete repository;

//		User::__DbgSetAllocFail(RHeap::EUser, RHeap::ENone, 1);

#ifndef __TOOLS2__		
		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);
		
		TEST2(endProcessHandleCount, startProcessHandleCount);
		TEST2(endThreadHandleCount, startThreadHandleCount);
#endif		
		
		__UHEAP_MARKEND;
		} while(error == KErrNoMemory);
	}

void GetFunctionL(CRepository* aRepository)
	{
	//[GET]
	TInt intVal;
	TReal realVal;
	User::LeaveIfError(aRepository->Get(6,intVal));
	TEST(intVal==12);
	User::LeaveIfError(aRepository->Get(8,realVal));
	TEST(realVal==1.5);
	TBuf<255> stringVal;
	User::LeaveIfError(aRepository->Get(0x300,stringVal));
	TEST(stringVal.Compare(_L("Hello World"))==0);	

	TBuf<5> shortBuffer;
	TInt actualLength=0;
	TInt ret=aRepository->Get(0x300,shortBuffer,actualLength);
	TEST(ret==KErrOverflow);
	TEST(actualLength==11);
	TEST(shortBuffer.Compare(_L("Hello"))==0);
		
	//[GETMETA]
	TUint32 metaValue;
	User::LeaveIfError(aRepository->GetMeta(2,metaValue));
	TEST(metaValue==0xa);
	//range based
	User::LeaveIfError(aRepository->GetMeta(0x204,metaValue));
	TEST(metaValue==0x20);
	//default based
	User::LeaveIfError(aRepository->GetMeta(0x10000,metaValue));
	TEST(metaValue==0x10);
	}

void FindFunctionL(CRepository* aRepository)
	{
	RArray<TUint32> keyList;
	CleanupClosePushL(keyList);
	//Find all settings
	TInt ret=aRepository->FindL(0xFFFFFFFF,0,keyList);
	User::LeaveIfError(ret);	
	TEST(ret==KErrNone);
	TEST(keyList.Count()==33);
	keyList.Reset();
	//Find match EQ specific value
	ret=aRepository->FindEqL(0xFFFFFFFF,0,10,keyList);
	User::LeaveIfError(ret);	
	TEST(ret==KErrNone);	
	TEST(keyList.Count()==3);
	keyList.Reset();
	//Find match NEQ specific value
	ret=aRepository->FindNeqL(0xFFFFFFFF,0,10,keyList);
	TEST(ret==KErrNone);
	TEST(keyList.Count()==30);
	keyList.Reset();		
	//Find using string matching instead
	_LIT(KString,"empty");
	ret=aRepository->FindEqL(0xFFFFFFFF,0,KString(),keyList);
	User::LeaveIfError(ret);
	TEST(ret==KErrNone);
	TEST(keyList.Count()==1);	
	CleanupStack::PopAndDestroy();
	}

void SetFunctionL(CRepository* aRepository)
	{
	//[SET]
	//int
	TUint32 metaValue;
	TInt value;
	User::LeaveIfError(aRepository->Get(1,value));
	TEST(value==1);
	User::LeaveIfError(aRepository->Set(1,100));
	User::LeaveIfError(aRepository->Get(1,value));
	TEST(value==100)	;
	//real
	TReal realValue;
	User::LeaveIfError(aRepository->Get(2,realValue));
	TEST(realValue==2.732);
	TReal newrealValue(5.464);
	User::LeaveIfError(aRepository->Set(2,newrealValue));
	User::LeaveIfError(aRepository->Get(2,realValue));
	TEST(realValue==5.464);
	//string
	TBuf<255> stringValue;
	User::LeaveIfError(aRepository->Get(0x10000,stringValue));
	TEST(stringValue.Compare(_L("empty"))==0);
	User::LeaveIfError(aRepository->Set(0x10000,_L("full")));
	stringValue.Zero();
	User::LeaveIfError(aRepository->Get(0x10000,stringValue));
	TEST(stringValue.Compare(_L("full"))==0);		
	
	//use set to create new setting and also check the meta
	TInt newIntegerKeyValue=205;
	User::LeaveIfError(aRepository->Set(0x205,newIntegerKeyValue));
	User::LeaveIfError(aRepository->Get(0x205,value));
	TEST(value==205);
	User::LeaveIfError(aRepository->GetMeta(0x205,metaValue));
	//as the key 0x205 is within range, expect use of range meta
	TEST(metaValue==0x20);	
	}

void CreateFunctionL(CRepository* aRepository)
	{
	TUint32 metaValue;
	TInt value=206;
	User::LeaveIfError(aRepository->Create(0x206,value));
	User::LeaveIfError(aRepository->Get(0x206,value));
	TEST(value==206);
	User::LeaveIfError(aRepository->GetMeta(0x206,metaValue));
	//within meta range
	TEST(metaValue==0x20);			
	
	TReal realValue=207.207;
	User::LeaveIfError(aRepository->Create(0x407,realValue));
	User::LeaveIfError(aRepository->Get(0x407,realValue));
	TEST(realValue==207.207);
	User::LeaveIfError(aRepository->GetMeta(0x407,metaValue));
	//default meta
	TEST(metaValue==0x10);
	
	//create already existing setting
	TInt ret=aRepository->Create(1,value);
	TEST(ret==KErrAlreadyExists);	
	}

void DeleteFunctionL(CRepository* aRepository)
	{
	//delete single
	TInt value;
	User::LeaveIfError(aRepository->Delete(6));
	TInt ret=aRepository->Get(6,value);
	TEST(ret==KErrNotFound);
	
	//delete range
	RArray<TUint32> keyList;
	CleanupClosePushL(keyList);
	//make sure that the list of keys are there first
	ret=aRepository->FindL(0x03010000,0xFFFFF0FF,keyList);
	User::LeaveIfError(ret);
	TEST(keyList.Count()==5);
	TUint32 error;
	User::LeaveIfError(aRepository->Delete(0x03010000,0xFFFFF0FF,error));
	//check that key no longer exist
	keyList.Reset();
	ret=aRepository->FindL(0x03010000,0xFFFFF0FF,keyList);
	if (ret!=KErrNotFound)
		User::LeaveIfError(ret);
	TEST(ret==KErrNotFound);
	TEST(keyList.Count()==0);
	CleanupStack::PopAndDestroy();	
	}

void MoveFunctionL(CRepository* aRepository)
	{
	RArray<TUint32> keyList;
	CleanupClosePushL(keyList);
	TUint32 errorKey;
	//check source key exists first
	keyList.Reset();
	TInt ret=aRepository->FindL(0x02010000,0xFFFFF0FF,keyList);
	User::LeaveIfError(ret);
	TEST(keyList.Count()==5);
	
	User::LeaveIfError(aRepository->Move(0x02010000,0x06010000,0xFFFFF0FF,errorKey));
	
	keyList.Reset();
	//check target now exists
	ret=aRepository->FindL(0x06010000,0xFFFFF0FF,keyList);
	User::LeaveIfError(ret);

	TEST(keyList.Count()==5);
	//check source now deleted
	keyList.Reset();
	ret=aRepository->FindL(0x02010000,0xFFFFF0FF,keyList);
	TEST(ret==KErrNotFound);
	TEST2(keyList.Count(),0);
	
	CleanupStack::PopAndDestroy();	
	}

/**
*/
void BasicFunctionL(TUid aUid,const TDesC& aInFilePath,const TDesC& aOutFilePath,TUint aTestMode)
	{
	//TEST SETUP
	SetupEnv(aInFilePath,aOutFilePath,aTestMode);
	
	__UHEAP_MARK;
	CRepository* repository=NULL;
	
	InitialiseLC(repository,aUid,aInFilePath,aOutFilePath,aTestMode);

#ifdef CENREP_PC_TEST
	//testing transaction
	TInt r= repository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);
	repository->CleanupCancelTransactionPushL();
#endif

	GetFunctionL(repository);
	FindFunctionL(repository);
	SetFunctionL(repository);
	CreateFunctionL(repository);
	DeleteFunctionL(repository);
	MoveFunctionL(repository);

#ifdef CENREP_PC_TEST
	CleanupStack::PopAndDestroy();
#endif

	CleanupStack::PopAndDestroy();	
	
	__UHEAP_MARKEND;
	}

void OomBasicFunction(TUid aUid,const TDesC& aInFilePath,const TDesC& aOutFilePath,TUint aTestMode)
	{
	__UHEAP_MARK;

	OomTest(GetFunctionL,aUid,aInFilePath,aOutFilePath,aTestMode);
	OomTest(FindFunctionL,aUid,aInFilePath,aOutFilePath,aTestMode);
	OomTest(SetFunctionL,aUid,aInFilePath,aOutFilePath,aTestMode);
	OomTest(CreateFunctionL,aUid,aInFilePath,aOutFilePath,aTestMode);
	OomTest(DeleteFunctionL,aUid,aInFilePath,aOutFilePath,aTestMode);
	OomTest(MoveFunctionL,aUid,aInFilePath,aOutFilePath,aTestMode);
	
	__UHEAP_MARKEND;
	}

void DEF130394L(TUid aUid)
	{
	CRepository* repos=NULL;
	
	repos = CRepository::NewL(aUid);
	
	TInt err = repos->Create(1,1);
	TEST(err==KErrNone);
	
	delete repos;
	
	repos = CRepository::NewL(aUid);
	
	err = repos->Create(1,1);
	TEST(err==KErrAlreadyExists);
	
	delete repos;
	}

void DoFileCompL(const TDesC& aGenerated, const TDesC& aReference, TUint32& aCrcValue)
	{
	RFile genFile;
	RFile refFile;
	TInt err = genFile.Open(TheFs,aGenerated,EFileRead);
	User::LeaveIfError(err);
	err = refFile.Open(TheFs,aReference,EFileRead);
	User::LeaveIfError(err);
	
	TInt sizeOfGen;
	genFile.Size(sizeOfGen);
	TInt sizeOfRef;
	refFile.Size(sizeOfRef);
	TEST(sizeOfGen == sizeOfRef);
	
	TUint32 crcGen = 0;
	TUint32 crcRef = 0;

	HBufC8* buf = HBufC8::New(sizeOfGen);
	if (!buf)
		{
		genFile.Close();
		refFile.Close();
		User::Leave(KErrNoMemory);
		}
	TPtr8 data(buf->Des());
	
	err = genFile.Read(data);
	User::LeaveIfError(err);
	Mem::Crc32(crcGen, buf, sizeOfGen);
	err = refFile.Read(data);
	User::LeaveIfError(err);
	Mem::Crc32(crcRef, buf, sizeOfRef);

	TEST(crcGen == crcRef);
	
	aCrcValue = crcGen;
	
	delete buf;
	genFile.Close();
	refFile.Close();
	
	return;
	}

void DoCrcCompL(const TUint32& aCrcValue, const TDesC& aCrcRecord, TBool aCreOrTxt)
	{
	RFile file;
	TInt err = file.Open(TheFs, aCrcRecord, EFileRead);
	User::LeaveIfError(err);
	
	TBuf8<100> buf;
	file.Read(buf);
	
	TBuf8<1> breaker(_L8("-"));
	TInt pos = buf.Find(breaker);
	
	TInt length = buf.Length();
	
	const TPtrC8 crePtr = buf.Right(length - pos -1);
	const TPtrC8 txtPtr = buf.Left(pos);
	
	TUint32 crc = 0;
	
	if (aCreOrTxt)
		{
		TLex8 lex (crePtr);
		lex.Val(crc, EDecimal);
		}
	else
		{
		TLex8 lex (txtPtr);
		lex.Val(crc, EDecimal);
		}
	TEST2(crc, aCrcValue);
	file.Close();
	}

