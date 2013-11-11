// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @internalComponent
// 
//

#include <e32std.h>
#include <e32base.h>
#include <e32test.h>
#include <f32file.h>
#include <bsul/inifile.h>		// CIniDocument8, CIniDocument16

RTest test(_L("Ini File Parser and Generator"));

RFs TheRFs;

using namespace BSUL;
#define UNUSED_VAR(a) a=a

_LIT(KIniFile8,"z:\\resource\\testconfig8.ini")	;

void LeaveIfNoMemory(TInt ret)
	{
	if (ret==KErrNoMemory)
		User::LeaveNoMemory();
	}

void CheckResources(RFs& aFs, TInt aOriginalHandleCount)
	{
	UNUSED_VAR(aOriginalHandleCount);
	//check we haven't leaked any handles
	TInt processHandleCount = 0;
	TInt threadHandleCount = 0;
	RThread().HandleCount(processHandleCount, threadHandleCount);
	test(threadHandleCount == aOriginalHandleCount);
	//check we haven't leaked any files
	aFs.ResourceCountMarkEnd();	
	}

TBool CompareFilesL(RFs& aFs,const TDesC& aFileNameA,const TDesC& aFileNameB)
	{
	TBool filesAreSame=FALSE;
	//open the file for reading
	RFile fileA;
	User::LeaveIfError(fileA.Open(aFs,aFileNameA,EFileShareReadersOrWriters ));
	CleanupClosePushL(fileA);
	RFile fileB;
	User::LeaveIfError(fileB.Open(aFs,aFileNameB,EFileShareReadersOrWriters ));
	CleanupClosePushL(fileB);
	
	//only process the file if it exists		
	TInt filesizeA=0;
	TInt filesizeB=0;
	fileA.Size(filesizeA);
	fileB.Size(filesizeB);
	if (filesizeA == filesizeB)
		{
		HBufC8* aBufferPtrA=HBufC8::NewLC(filesizeA);
		HBufC8* aBufferPtrB=HBufC8::NewLC(filesizeB);
		TPtr8 asWriteableBufferA(aBufferPtrA->Des());
		User::LeaveIfError(fileA.Read(0,asWriteableBufferA,filesizeA));
		TPtr8 asWriteableBufferB(aBufferPtrB->Des());
		User::LeaveIfError(fileB.Read(0,asWriteableBufferB,filesizeB));
		if (asWriteableBufferA.Compare(asWriteableBufferB) == 0)
			{
			filesAreSame=TRUE;
			}
		//pop the buffers
		CleanupStack::PopAndDestroy(2);
		}
	//close the files
	CleanupStack::PopAndDestroy(2);	
	return filesAreSame;
	}

class CIniParser8Test :public CBase
{
typedef void (CIniParser8Test::*ClassFuncPtr8L) (void);

public:
	static CIniParser8Test* NewL();
	~CIniParser8Test()
	{
	if (iIniDocument)
		{
		delete iIniDocument;
		iIniDocument=NULL;
		}
	}

	//simple create and delete
	//heavy
	static void CreateDeleteTest1L();
	static void CreateDeleteOOMTestL();
	//light
	static void CreateDeleteTest2L();
	static void CreateDeleteOOMTest2L();

	//List of tests
	//heavy weight
	void DoTest1L();
	void DoTest2L();
	void DoTest3L();
	void DoTest4L();
	void DoTest5L();
	void DoTest6L();
	void DoTest7L();
	void DoTest8L();
	void DoTest9L();
	//light weight
	void DoTest10L();
	
	//Consistency check
	void DoTest11L();
	
	//Defect 127618
	void DoTest12L();
			
	//class function utilities
	static void DoBasicTestL(ClassFuncPtr8L testFuncL, const TDesC& aTestDesc);
	static void DoOOMTestL(ClassFuncPtr8L testFuncL, const TDesC& aTestDesc);
	static void DoOOMWithConsistencyCheckTestL(ClassFuncPtr8L testFuncL, const TDesC& aTestDesc, const TBool aShouldBeDifferent);
	
private:
	CIniParser8Test():iIniDocument(NULL){}
	CIniDocument8* iIniDocument;
};

CIniParser8Test* CIniParser8Test::NewL()
	{
	CIniParser8Test* self=new (ELeave)CIniParser8Test();
	CleanupStack::PushL(self);
	self->iIniDocument=CIniDocument8::NewL(TheRFs,KIniFile8);
	CleanupStack::Pop();
	return self;
	}

void CIniParser8Test::DoBasicTestL(ClassFuncPtr8L testFuncL, const TDesC& aTestDesc)
	{
	test.Next(aTestDesc);

	__UHEAP_MARK;
  	// find out the number of open handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	CIniParser8Test* iniTest=CIniParser8Test::NewL();

	CleanupStack::PushL(iniTest);

	(iniTest->*testFuncL)();

	CleanupStack::PopAndDestroy();

	// check that no handles have leaked
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startProcessHandleCount == endProcessHandleCount);
	test(startThreadHandleCount  == endThreadHandleCount);

	__UHEAP_MARKEND;
	}

void CIniParser8Test::DoOOMTestL(ClassFuncPtr8L testFuncL, const TDesC& aTestDesc)
	{
	test.Next(aTestDesc);

	TInt err;
	TInt tryCount = 0;
	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		CIniParser8Test* iniTest=CIniParser8Test::NewL();
		CleanupStack::PushL(iniTest);

		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);
		TRAP(err, (iniTest->*testFuncL)());
		__UHEAP_SETFAIL(RHeap::ENone, 0);

		CleanupStack::PopAndDestroy(iniTest);
		iniTest=NULL;
		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		test(startProcessHandleCount == endProcessHandleCount);
		test(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

 	test(err==KErrNone);
	test.Printf(_L("- succeeded at heap failure rate of %i\n"), tryCount);
	}

void CIniParser8Test::DoOOMWithConsistencyCheckTestL(ClassFuncPtr8L testFuncL, const TDesC& aTestDesc, const TBool aShouldBeSame)
	{
	test.Next(aTestDesc);
	
	// find out the number of open handles
	TInt startProcessHandleCount = 0;
	TInt startThreadHandleCount = 0;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);
	
	//Compare the results against this instance.
	TInt err;
	CIniDocument8* referenceDoc=NULL;
	TRAP(err,referenceDoc=CIniDocument8::NewL(TheRFs,KIniFile8));
	User::LeaveIfError(err);
	CleanupStack::PushL(referenceDoc);
	
	//Open a file and Externalise the reference oom to it.
	User::LeaveIfError(referenceDoc->Externalise(_L("c:\\initest\\oom_ref.ini")));
	CIniParser8Test* iniTest=NULL;
	
	for (TInt i = 1;;i++)
		{
		__UHEAP_MARK;
		__UHEAP_FAILNEXT(i);
		
		TRAP(err, iniTest=CIniParser8Test::NewL());
		if (err != KErrNone)
			continue;
		
		CleanupStack::PushL(iniTest);
		
		TRAP(err, (iniTest->*testFuncL)());
		if (err != KErrNone)
			{
			test(iniTest->iIniDocument->CompareDocs(*referenceDoc));
			CleanupStack::PopAndDestroy(iniTest);
			__UHEAP_SETFAIL(RHeap::ENone, 0);
			__UHEAP_MARKEND;
			continue;
			}
		else
			{
			//Open a file and Externalise to it.
			TRAP(err, iniTest->iIniDocument->Externalise(_L("c:\\initest\\oom.ini")));
			if (err != KErrNone)
				{
				CleanupStack::PopAndDestroy(iniTest);
				__UHEAP_SETFAIL(RHeap::ENone, 0);
				__UHEAP_MARKEND;
				continue;
				}
			else
				{
				TBool result=EFalse;
				TRAP(err, result=CompareFilesL(TheRFs,_L("c:\\initest\\oom_ref.ini"), _L("c:\\initest\\oom.ini")));
				if (err != KErrNone)
					{
					CleanupStack::PopAndDestroy(iniTest);
					__UHEAP_SETFAIL(RHeap::ENone, 0);
					__UHEAP_MARKEND;
					continue;
					}
				test(result == aShouldBeSame);
				}
			}
		CleanupStack::PopAndDestroy(iniTest);
		//check we haven't leaked any heap memory
		__UHEAP_MARKEND;
		CheckResources(TheRFs, startThreadHandleCount);
		
		if (err != KErrNoMemory)
			{
			test(err == KErrNone);
			break;		// we reach here if we are unable to create the OOM condition.
			}
		 
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		} 
		
	__UHEAP_RESET;
	CleanupStack::PopAndDestroy(referenceDoc);
	
	test.Printf(_L("Completed consistency check."));
	}

void CIniParser8Test::CreateDeleteOOMTestL()
	{
	TInt err;
	TInt tryCount = 0;
	do
		{
		__UHEAP_MARK;

		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);

		CIniDocument8* ini=NULL;
		TRAP(err,ini=CIniDocument8::NewL(TheRFs,KIniFile8));

		delete ini;

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);
		test(startProcessHandleCount == endProcessHandleCount);
		test(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

	test(err==KErrNone);
	test.Printf(_L("- succeeded at heap failure rate of %i\n"), tryCount);
	}


void CIniParser8Test::CreateDeleteTest1L()
	{
	__UHEAP_MARK;

	CIniDocument8* ini=NULL;
	//note only support 16 bit Little Endian ini file
	ini=CIniDocument8::NewL(TheRFs,KIniFile8);

	delete ini;

	__UHEAP_MARKEND;
	}

void CIniParser8Test::CreateDeleteOOMTest2L()
	{
	TInt err;
	TInt tryCount = 0;
	do
		{
		__UHEAP_MARK;

		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);

		CIniFile8* ini=NULL;
		TRAP(err,ini=CIniFile8::NewL(TheRFs,KIniFile8));

		delete ini;

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);
		test(startProcessHandleCount == endProcessHandleCount);
		test(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

	test(err==KErrNone);
	test.Printf(_L("- succeeded at heap failure rate of %i\n"), tryCount);
	}


void CIniParser8Test::CreateDeleteTest2L()
	{
	__UHEAP_MARK;

	CIniFile8* ini=NULL;
	//note only support 16 bit Little Endian ini file
	ini=CIniFile8::NewL(TheRFs,KIniFile8);

	delete ini;

	__UHEAP_MARKEND;
	}

void CIniParser8Test::DoTest1L()
	{
	//Testing GetSectionList API
	RArray<TPtrC8> sectionNames;
	User::LeaveIfError(iIniDocument->GetSectionList(sectionNames));
	test(sectionNames.Count()==8);

	//Testing the sectionNames in name order
	test(sectionNames[0].Compare(_L8("1"))==0);
	test(sectionNames[1].Compare(_L8("MAPPINGS"))==0);
	test(sectionNames[2].Compare(_L8("MEDIA"))==0);
	test(sectionNames[3].Compare(_L8("OUTPUT_CHANNELS"))==0);
	test(sectionNames[4].Compare(_L8("SERVERS"))==0);
	test(sectionNames[5].Compare(_L8("SWTRACER"))==0);
	test(sectionNames[6].Compare(_L8("test_section"))==0);
	test(sectionNames[7].Compare(_L8("test_twosection"))==0);
	sectionNames.Reset();
	}

void CIniParser8Test::DoTest2L()
	{
	//Test GetKeyValue API
	TPtrC8 value;
	User::LeaveIfError(iIniDocument->GetKeyValue(_L8("MEDIA"),_L8("RDebug"),value));
	test(value.Compare(_L8("SwtRDebugPlugin.dll"))==0);
	User::LeaveIfError(iIniDocument->GetKeyValue(_L8("OUTPUT_CHANNELS"),_L8("1"),value));
	test(value.Compare(_L8("RDebug"))==0);
	User::LeaveIfError(iIniDocument->GetKeyValue(_L8("1"),_L8("new_setting"),value));
	test(value.Compare(_L8("value \\n value1\\t value2"))==0);

	//unknown section
	TInt ret=KErrNone;
	ret=iIniDocument->GetKeyValue(_L8("mySection"),_L8("mykey"),value);
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);
	//unknown key
	ret=iIniDocument->GetKeyValue(_L8("MEDIA"),_L8("mykey"),value);
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);
	//empty value
	ret=iIniDocument->GetKeyValue(_L8("SERVERS"),_L8("SWTRACER"),value);
	LeaveIfNoMemory(ret);
	test(value.Length()==0);
	}

void CIniParser8Test::DoTest3L()
	{
	//Test AddSection API
	RArray<TPtrC8> sectionNames;
	CleanupClosePushL(sectionNames);
	User::LeaveIfError(iIniDocument->AddSection(_L8("NEW-SECTION")));
	User::LeaveIfError(iIniDocument->GetSectionList(sectionNames));
	test(sectionNames.Count()==9);

	//case sensitive
	User::LeaveIfError(iIniDocument->AddSection(_L8("NeW-SECTION")));
	User::LeaveIfError(iIniDocument->GetSectionList(sectionNames));
	test(sectionNames.Count()==10);
	//adding existing section, no duplicate allowed
	TInt ret=iIniDocument->AddSection(_L8("NEW-SECTION"));
	LeaveIfNoMemory(ret);
	test(ret==KErrAlreadyExists);
	CleanupStack::PopAndDestroy();
	}

void CIniParser8Test::DoTest4L()
	{
	//Test RemoveSection API
	RArray<TPtrC8> sectionNames;
	CleanupClosePushL(sectionNames);

	//known section at start of file.
	User::LeaveIfError(iIniDocument->RemoveSection(_L8("SERVERS")));
	User::LeaveIfError(iIniDocument->GetSectionList(sectionNames));
	test(sectionNames.Count()==7);

	//check key inside section is also deleted
	TPtrC8 value;
	TInt ret=iIniDocument->GetKeyValue(_L8("SERVERS"),_L8("SWTRACER"),value);
	LeaveIfNoMemory(ret);
	//Any section	
	test(ret==KErrNotFound);

	//unknown section
	ret=iIniDocument->RemoveSection(_L8("AnySection"));
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);
	
	//nonexist section should be created at end
	User::LeaveIfError(iIniDocument->SetKey(_L8("unknown_section3"),_L8("unknown_key3"),_L8("unknown_value3")));
	User::LeaveIfError(iIniDocument->GetKeyValue(_L8("unknown_section3"),_L8("unknown_key3"),value));
	test(value.Compare(_L8("unknown_value3"))==0);
	
	//unknown section	
	ret=iIniDocument->RemoveSection(_L8("unknown_section3"));
	LeaveIfNoMemory(ret);
	test(ret==KErrNone);
	
	//Open a file and Externalise to it.
	User::LeaveIfError(iIniDocument->Externalise(_L("c:\\initest\\unknown8_3.ini")));
	CleanupStack::PopAndDestroy();		
	
	// Read  it back in and find the additions put in above.
	CIniDocument8* iniReRead=NULL;
	iniReRead=CIniDocument8::NewL(TheRFs,_L("c:\\initest\\unknown8_3.ini"));
	CleanupStack::PushL(iniReRead);	
	
	ret=iniReRead->GetKeyValue(_L8("unknown_section3"),_L8("unknown_key3"),value);
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);
	CleanupStack::PopAndDestroy(iniReRead);
	}

void CIniParser8Test::DoTest5L()
	{
	//Testing SetKey API
	TPtrC8 value;
	//Modifying existing value
	User::LeaveIfError(iIniDocument->SetKey(_L8("MEDIA"),_L8("RDebug"),_L8("NewPlugin.dll")));
	User::LeaveIfError(iIniDocument->GetKeyValue(_L8("MEDIA"),_L8("RDebug"),value));
	test(value.Compare(_L8("NewPlugin.dll"))==0);

	//nonexist key should be created
	User::LeaveIfError(iIniDocument->SetKey(_L8("MEDIA"),_L8("newplug"),_L8("")));
	User::LeaveIfError(iIniDocument->GetKeyValue(_L8("MEDIA"),_L8("newplug"),value));
	test(value.Compare(_L8(""))==0);

	//nonexist section should be created
	User::LeaveIfError(iIniDocument->SetKey(_L8("unknown_section"),_L8("unknown_key"),_L8("unknown_value")));
	User::LeaveIfError(iIniDocument->GetKeyValue(_L8("unknown_section"),_L8("unknown_key"),value));
	test(value.Compare(_L8("unknown_value"))==0);
	
	//nonexist key should be created
	User::LeaveIfError(iIniDocument->SetKey(_L8("SERVERS"),_L8("host"),_L8("newhost")));
	User::LeaveIfError(iIniDocument->GetKeyValue(_L8("SERVERS"),_L8("host"),value));		
	test(value.Compare(_L8("newhost"))==0);	
	
	//create a second key in first section
	User::LeaveIfError(iIniDocument->SetKey(_L8("SERVERS"),_L8("host2"),_L8("newhost2")));
	User::LeaveIfError(iIniDocument->GetKeyValue(_L8("SERVERS"),_L8("host2"),value));		
	test(value.Compare(_L8("newhost2"))==0);	
	
	//alter existing key value again
	User::LeaveIfError(iIniDocument->SetKey(_L8("unknown_section"),_L8("unknown_key"),_L8("unknown_value2")));
	User::LeaveIfError(iIniDocument->GetKeyValue(_L8("unknown_section"),_L8("unknown_key"),value));
	test(value.Compare(_L8("unknown_value2"))==0);
	
	//Open a file and Externalise to it.
	User::LeaveIfError(iIniDocument->Externalise(_L("c:\\initest\\unknown8_1.ini")));
	
	// Read  it back in and find the additions put in above.
	CIniDocument8* iniReRead=NULL;
	iniReRead=CIniDocument8::NewL(TheRFs,_L("c:\\initest\\unknown8_1.ini"));
	CleanupStack::PushL(iniReRead);	
	
	User::LeaveIfError(iniReRead->GetKeyValue(_L8("unknown_section"),_L8("unknown_key"),value));
	test(value.Compare(_L8("unknown_value2"))==0);
	User::LeaveIfError(iniReRead->GetKeyValue(_L8("SERVERS"),_L8("host2"),value));		
	test(value.Compare(_L8("newhost2"))==0);	
	User::LeaveIfError(iniReRead->GetKeyValue(_L8("MEDIA"),_L8("RDebug"),value));
	test(value.Compare(_L8("NewPlugin.dll"))==0);
	User::LeaveIfError(iniReRead->GetKeyValue(_L8("MEDIA"),_L8("newplug"),value));		
	test(value.Compare(_L8(""))==0);	
	CleanupStack::PopAndDestroy(iniReRead);
	
	}

void CIniParser8Test::DoTest6L()
	{
	//Testing RemoveKey API
	TPtrC8 value;
	//remove existing key in middle of file.
	User::LeaveIfError(iIniDocument->RemoveKey(_L8("OUTPUT_CHANNELS"),_L8("1")));
	TInt ret=iIniDocument->GetKeyValue(_L8("OUTPUT_CHANNELS"),_L8("1"),value);
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);

	//remove non-exist key 
	ret=iIniDocument->RemoveKey(_L8("OUTPUT_CHANNELS"),_L8("1"));
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);

	//remove non-exist section
	ret=iIniDocument->RemoveKey(_L8("Non-existSection"),_L8("1"));
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);
	
	//remove existing key at start of file
	User::LeaveIfError(iIniDocument->RemoveKey(_L8("SERVERS"),_L8("SWTRACER")));
	ret=iIniDocument->GetKeyValue(_L8("SERVERS"),_L8("SWTRACER"),value);
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);
	
	//nonexist section should be created at end
	User::LeaveIfError(iIniDocument->SetKey(_L8("unknown_section"),_L8("unknown_key"),_L8("unknown_value")));
	User::LeaveIfError(iIniDocument->GetKeyValue(_L8("unknown_section"),_L8("unknown_key"),value));
	test(value.Compare(_L8("unknown_value"))==0);
	
	//remove existing key at end of section
	User::LeaveIfError(iIniDocument->RemoveKey(_L8("unknown_section"),_L8("unknown_key")));
	ret=iIniDocument->GetKeyValue(_L8("unknown_section"),_L8("unknown_key"),value);
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);
	
	//Open a file and Externalise to it.
	User::LeaveIfError(iIniDocument->Externalise(_L("c:\\initest\\unknown8_2.ini")));
	
	// Read  it back in and find the additions put in above.
	CIniDocument8* iniReRead=NULL;
	iniReRead=CIniDocument8::NewL(TheRFs,_L("c:\\initest\\unknown8_2.ini"));
	CleanupStack::PushL(iniReRead);	
	
	ret=iniReRead->GetKeyValue(_L8("unknown_section"),_L8("unknown_key"),value);
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);
	
	ret=iniReRead->GetKeyValue(_L8("SERVERS"),_L8("SWTRACER"),value);
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);
	CleanupStack::PopAndDestroy(iniReRead);
	}

void CIniParser8Test::DoTest7L()
	{
	//Testing iterator class
	CIniSecIter8* iIniSecIter=NULL;
	TInt ret=KErrNone;

	//unknown section
	TRAP(ret,iIniSecIter=CIniSecIter8::NewL(_L8("Unknown"),iIniDocument));
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);

	//null document
	TRAP(ret,iIniSecIter=CIniSecIter8::NewL(_L8("Unknown"),NULL));
	LeaveIfNoMemory(ret);
	test(ret==KErrArgument);

	//known section
	iIniSecIter=CIniSecIter8::NewL(_L8("test_section"),iIniDocument);
	TPtrC8 key;
	TPtrC8 value;
	//test Next() and End();
	test(!iIniSecIter->End());
	test(iIniSecIter->Next(key,value));
	test(key.Compare(_L8("key1"))==0);
	test(value.Compare(_L8("value1"))==0);
	test(!iIniSecIter->End());
	test(iIniSecIter->Next(key,value));
	test(key.Compare(_L8("key2"))==0);
	test(value.Compare(_L8("value2"))==0);
	test(!iIniSecIter->End());
	test(iIniSecIter->Next(key,value));
	test(key.Compare(_L8("key3"))==0);
	test(value.Compare(_L8("value3"))==0);
	test(!iIniSecIter->End());
	test(iIniSecIter->Next(key,value));
	test(key.Compare(_L8("key4"))==0);
	test(value.Compare(_L8("value4"))==0);
	test(!iIniSecIter->End());
	test(iIniSecIter->Next(key,value));
	test(key.Compare(_L8("key5"))==0);
	test(value.Compare(_L8("value value value"))==0);
	test(iIniSecIter->End());
	test(iIniSecIter->Next(key,value)==EFalse);

	//test Reset()
	iIniSecIter->Reset();
	test(!iIniSecIter->End());
	test(iIniSecIter->Next(key,value));
	test(key.Compare(_L8("key1"))==0);
	test(value.Compare(_L8("value1"))==0);

	delete iIniSecIter;
	iIniSecIter=NULL;
	}

void CIniParser8Test::DoTest8L()
	{
	//Testing Externalise to ROM drive
	TInt ret=iIniDocument->Externalise(_L("z:\\output.ini"));
	LeaveIfNoMemory(ret);
	test(ret==KErrAccessDenied);

	//Testing Externalise to a New file
	User::LeaveIfError(iIniDocument->Externalise(_L("c:\\initest\\output8.ini")));

	test(CompareFilesL(TheRFs,_L("z:\\resource\\testconfig8.ini"), _L("c:\\initest\\output8.ini")));
	
	//Try opening the written ini file now to ensure no corruption in writing
	CIniDocument8* output=CIniDocument8::NewL(TheRFs,_L("c:\\initest\\output8.ini"));
	CleanupStack::PushL(output);
	User::LeaveIfError(output->SetKey(_L8("Test"),_L8("Test"),_L8("Test")));

	//Testing Externaliseing to the already exist file
	User::LeaveIfError(output->Externalise(_L("c:\\initest\\output8.ini")));
	CleanupStack::PopAndDestroy();

	//Opening an empty file and Externaliseing an empty file
	output=CIniDocument8::NewL(TheRFs,_L("c:\\initest\\unknown8.ini"));
	CleanupStack::PushL(output);
	User::LeaveIfError(output->Externalise(_L("c:\\initest\\unknown8.ini")));
	CleanupStack::PopAndDestroy();
	}

void CIniParser8Test::DoTest9L()
{
	//Test for no leakage when handling corrupt file
	CIniDocument8* ini=NULL;
	TRAPD(err,ini=CIniDocument8::NewL(TheRFs,_L("z:\\resource\\corruptconfig8.ini")));
	LeaveIfNoMemory(err);
	test(err==KErrCorrupt);
	delete ini;
}

void CIniParser8Test::DoTest10L()
{
	TPtrC8 value;
	//open existing ini file
	CIniFile8* ini=CIniFile8::NewL(TheRFs,_L("z:\\resource\\testconfig8.ini"));
	CleanupStack::PushL(ini);

	//mid section
	User::LeaveIfError(ini->FindVar(_L8("test_section"),_L8("key1"),value));
	test(value.Compare(_L8("value1"))==0);
	User::LeaveIfError(ini->FindVar(_L8("test_section"),_L8("key2"),value));
	test(value.Compare(_L8("value2"))==0);
	User::LeaveIfError(ini->FindVar(_L8("test_section"),_L8("key3"),value));
	test(value.Compare(_L8("value3"))==0);
	User::LeaveIfError(ini->FindVar(_L8("test_section"),_L8("key4"),value));
	test(value.Compare(_L8("value4"))==0);
	User::LeaveIfError(ini->FindVar(_L8("test_section"),_L8("key5"),value));
	test(value.Compare(_L8("value value value"))==0);

	//first section
	User::LeaveIfError(ini->FindVar(_L8("SERVERS"),_L8("SWTRACER"),value));
	test(value.Compare(_L8(""))==0);

	//last section
	User::LeaveIfError(ini->FindVar(_L8("1"),_L8("timestamps"),value));
	test(value.Compare(_L8("0"))==0);
	User::LeaveIfError(ini->FindVar(_L8("1"),_L8("setting"),value));
	test(value.Compare(_L8("value"))==0);

	CleanupStack::PopAndDestroy();

	//open a non existing file
	TInt ret=KErrNone;
	TRAP(ret,ini=CIniFile8::NewL(TheRFs,_L("z:\\resource\\nonexist.ini")));
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);

	//open an empty ini file
	ini=CIniFile8::NewL(TheRFs,_L("z:\\resource\\empty8.ini"));
	CleanupStack::PushL(ini);

	ret=ini->FindVar(_L8("empty"),_L8("empty"),value);
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);

	CleanupStack::PopAndDestroy();
}

void CIniParser8Test::DoTest11L()
	{
	TPtrC8 value;
	// We are trying to invoke an OOM condition for a single operation to test that the operation is atomic. 
	// Under that condition the object should be rolled back to the original state. The resulting document should be the same 
	// as before the condition was invoked. If the test succeeded, a new section should be created at end 
	// (which is nice but not the real focus of the test).
	User::LeaveIfError(iIniDocument->SetKey(_L8("unknown_section3"),_L8("unknown_key3"),_L8("unknown_value3")));
	}
	
void CIniParser8Test::DoTest12L()
{
	__UHEAP_MARK;
	
	CIniDocument8* Ori_ini = NULL;
	CIniDocument8* Com_ini = NULL;
	CIniDocument8* Cre_ini = NULL;
	
	
	RFs rfs;	
	rfs.Connect();
	
	TRAPD( err1, Ori_ini = CIniDocument8::NewL(rfs, _L("z:\\resource\\OriConfig8.ini")) );
	test(err1 == KErrNone);
	TRAPD( err2, Com_ini = CIniDocument8::NewL(rfs, _L("z:\\resource\\ComConfig8.ini")) );
	test(err2 == KErrNone);
	
	Ori_ini->SetKey(_L8("DEFECT"), _L8("Number"), _L8("127618"));
	Ori_ini->Externalise( _L("c:\\initest\\CreateConfig8.ini") );
	
	TRAPD( err3, Cre_ini = CIniDocument8::NewL(rfs, _L("c:\\initest\\CreateConfig8.ini")) );
	test(err3 == KErrNone);
	
	bool result = Cre_ini->CompareDocs(*Com_ini);
	test(result==true);
	
	rfs.Close();
	delete Ori_ini;
	delete Com_ini;
	delete Cre_ini;
	
	__UHEAP_MARKEND;	
}

static void DeleteFilesL()
	{
	CFileMan* fileman=CFileMan::NewL(TheRFs);

	fileman->Delete(_L("c:\\initest\\*"));

	delete fileman;
	}

/**
@SYMTestCaseID	SYSLIB-BAFL-CT-1558
@SYMTestCaseDesc 	Test CIniParser8Test
@SYMTestPriority 	High
@SYMTestActions  	Perform various component tests on CIniParser8Test (includes OOM)
			Testing all the exported APis
@SYMTestExpectedResults The test must not fail.
@SYMREQ PREQ505
*/
static void DoTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1558 "));
	DeleteFilesL();

	//8 bit basic testing

	CIniParser8Test::CreateDeleteTest1L();
	CIniParser8Test::DoBasicTestL(&CIniParser8Test::DoTest1L,_L("GetSectionList8"));
	CIniParser8Test::DoBasicTestL(&CIniParser8Test::DoTest2L,_L("GetKeyValue8"));
	CIniParser8Test::DoBasicTestL(&CIniParser8Test::DoTest3L,_L("AddSection8"));
	CIniParser8Test::DoBasicTestL(&CIniParser8Test::DoTest4L,_L("RemoveSection8"));
	CIniParser8Test::DoBasicTestL(&CIniParser8Test::DoTest5L,_L("SetKeyValue8"));
	CIniParser8Test::DoBasicTestL(&CIniParser8Test::DoTest6L,_L("RemoveKey8"));
	CIniParser8Test::DoBasicTestL(&CIniParser8Test::DoTest7L,_L("IniSecIter8"));
	CIniParser8Test::DoBasicTestL(&CIniParser8Test::DoTest8L,_L("Externalise"));
	CIniParser8Test::DoBasicTestL(&CIniParser8Test::DoTest9L,_L("Corrupt file"));
	
	//Defect 127618
	CIniParser8Test::DoBasicTestL(&CIniParser8Test::DoTest12L,_L("New Line Missing"));
								

	//8 bit OOM testing
	CIniParser8Test::CreateDeleteOOMTestL();
	CIniParser8Test::DoOOMTestL(&CIniParser8Test::DoTest1L,_L("GetSectionList8-OOM"));
	CIniParser8Test::DoOOMTestL(&CIniParser8Test::DoTest2L,_L("GetKeyValue8-OOM"));
	CIniParser8Test::DoOOMTestL(&CIniParser8Test::DoTest3L,_L("AddSection8-OOM"));
	CIniParser8Test::DoOOMTestL(&CIniParser8Test::DoTest4L,_L("RemoveSection8-OOM"));
	CIniParser8Test::DoOOMTestL(&CIniParser8Test::DoTest5L,_L("SetKeyValue8-OOM"));
	CIniParser8Test::DoOOMTestL(&CIniParser8Test::DoTest6L,_L("RemoveKey8-OOM"));
	CIniParser8Test::DoOOMTestL(&CIniParser8Test::DoTest7L,_L("IniSecIter8-OOM"));
	CIniParser8Test::DoOOMTestL(&CIniParser8Test::DoTest8L,_L("Externalise-OOM"));
	CIniParser8Test::DoOOMTestL(&CIniParser8Test::DoTest9L,_L("Corrupt file-OOM"));

	//8 bit light basic testing
	CIniParser8Test::CreateDeleteTest2L();
	CIniParser8Test::DoBasicTestL(&CIniParser8Test::DoTest10L,_L("Light FindVar"));
	//8 bit light OOM testing
	CIniParser8Test::CreateDeleteOOMTest2L();
	CIniParser8Test::DoOOMTestL(&CIniParser8Test::DoTest10L,_L("Light FindVar-OOM"));

	//Light weight temporary testing
	TInt ret=KErrNone;
	TTime startTime(0), stopTime(0);

	TPtrC8 value;
	startTime.UniversalTime();
for (TInt i=0;i<1000;i++)
	{
	CIniFile8* ini=CIniFile8::NewL(TheRFs,_L("z:\\resource\\testconfig8.ini"));
	ret=ini->FindVar(_L8("test_section"),_L8("key1"),value);
	test(value.Compare(_L8("value1"))==0);
	test(ret==KErrNone);
	ret=ini->FindVar(_L8("test_section"),_L8("key2"),value);
	test(value.Compare(_L8("value2"))==0);
	test(ret==KErrNone);
	ret=ini->FindVar(_L8("test_section"),_L8("key3"),value);
	test(value.Compare(_L8("value3"))==0);
	test(ret==KErrNone);
	ret=ini->FindVar(_L8("test_section"),_L8("key4"),value);
	test(value.Compare(_L8("value4"))==0);
	test(ret==KErrNone);
	ret=ini->FindVar(_L8("test_section"),_L8("key5"),value);
	test(value.Compare(_L8("value value value"))==0);
	test(ret==KErrNone);
	ret=ini->FindVar(_L8("SERVERS"),_L8("SWTRACER"),value);
	test(value.Compare(_L8(""))==0);
	test(ret==KErrNone);
	ret=ini->FindVar(_L8("1"),_L8("timestamps"),value);
	test(value.Compare(_L8("0"))==0);
	test(ret==KErrNone);
	ret=ini->FindVar(_L8("1"),_L8("setting"),value);
	test(value.Compare(_L8("value"))==0);
	test(ret==KErrNone);
	delete ini;
	}
	stopTime.UniversalTime();
	TTimeIntervalMicroSeconds timeTaken = stopTime.MicroSecondsFrom(startTime);
	test.Printf(_L("Time taken for Light= %d microseconds\n"), timeTaken.Int64() );

	//heavy weight

	startTime.UniversalTime();
for (TInt j=0;j<1000;j++)
	{
	CIniDocument8* dom=CIniDocument8::NewL(TheRFs,KIniFile8);
	ret=dom->GetKeyValue(_L8("test_section"),_L8("key1"),value);
	test(value.Compare(_L8("value1"))==0);
	test(ret==KErrNone);
	ret=dom->GetKeyValue(_L8("test_section"),_L8("key2"),value);
	test(value.Compare(_L8("value2"))==0);
	test(ret==KErrNone);
	ret=dom->GetKeyValue(_L8("test_section"),_L8("key3"),value);
	test(value.Compare(_L8("value3"))==0);
	test(ret==KErrNone);
	ret=dom->GetKeyValue(_L8("test_section"),_L8("key4"),value);
	test(value.Compare(_L8("value4"))==0);
	test(ret==KErrNone);
	ret=dom->GetKeyValue(_L8("test_section"),_L8("key5"),value);
	test(value.Compare(_L8("value value value"))==0);
	test(ret==KErrNone);
	ret=dom->GetKeyValue(_L8("SERVERS"),_L8("SWTRACER"),value);
	test(value.Compare(_L8(""))==0);
	test(ret==KErrNone);
	ret=dom->GetKeyValue(_L8("1"),_L8("timestamps"),value);
	test(value.Compare(_L8("0"))==0);
	test(ret==KErrNone);
	ret=dom->GetKeyValue(_L8("1"),_L8("setting"),value);
	test(value.Compare(_L8("value"))==0);
	test(ret==KErrNone);
	delete dom;
	}

	stopTime.UniversalTime();
	timeTaken = stopTime.MicroSecondsFrom(startTime);
	test.Printf(_L("Time taken for Heavy= %d microseconds\n"), timeTaken.Int64() );	
	
	startTime.UniversalTime();	
	// Consistency checks
	CIniParser8Test::DoOOMWithConsistencyCheckTestL(&CIniParser8Test::DoTest11L,_L("Consistency8-OOMC"), FALSE);

	stopTime.UniversalTime();
	timeTaken = stopTime.MicroSecondsFrom(startTime);
	test.Printf(_L("Time taken for consistency checks= %d microseconds\n"), timeTaken.Int64() );
	DeleteFilesL();	
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* trapCleanup=CTrapCleanup::New();
	test(TheRFs.Connect()==KErrNone);
	test.Start(_L("MyTest"));

	TRAPD(error, DoTestL());
	test(error == KErrNone);


	TheRFs.Close();
	test.End();
	test.Close();
	delete trapCleanup;
	__UHEAP_MARKEND;
	return error;
	}

