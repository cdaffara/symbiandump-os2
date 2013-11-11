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
#include <bsul/inifile.h>	// CIniDocument8, CIniDocument16

RTest test(_L("Ini File Parser and Generator"));

RFs TheRFs;

using namespace BSUL;
#define UNUSED_VAR(a) a=a

_LIT(KIniFile16,"z:\\resource\\testconfig16.ini")	;

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
	RFile fileA;
	User::LeaveIfError(fileA.Open(aFs,aFileNameA,EFileShareReadersOrWriters ));
	CleanupClosePushL(fileA);
	RFile fileB;
	User::LeaveIfError(fileB.Open(aFs,aFileNameB,EFileShareReadersOrWriters ));
	CleanupClosePushL(fileB);
			
	TInt filesizeA=0;
	TInt filesizeB=0;
	fileA.Size(filesizeA);
	fileB.Size(filesizeB);
	if ( filesizeA == filesizeB)
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
		CleanupStack::PopAndDestroy(2);
		}
	CleanupStack::PopAndDestroy(2);	
	return filesAreSame;
	}

class CIniParser16Test :public CBase
{
typedef void (CIniParser16Test::*ClassFuncPtr8L) (void);

public:
	static CIniParser16Test* NewL();
	~CIniParser16Test()
	{
	if (iIniDocument)
		{
		delete iIniDocument;
		iIniDocument=NULL;
		}
	}
	
	//heavy create and delete
	static void CreateDeleteTest1L();
	static void CreateDeleteOOMTestL();	
	
	//light
	static void CreateDeleteTest2L();
	static void CreateDeleteOOMTest2L();	
	
	//List of tests
	//heavy
	void DoTest1L();
	void DoTest2L();
	void DoTest3L();
	void DoTest4L();
	void DoTest5L();
	void DoTest6L();
	void DoTest7L();
	void DoTest8L();
	void DoTest9L();
	//light
	void DoTest10L();
	void DoTest11L();
	
	//OOM consistency test
	void DoTest12L();
			
	//class function utilities
	static void DoBasicTestL(ClassFuncPtr8L testFuncL, const TDesC& aTestDesc);	
	static void DoOOMTestL(ClassFuncPtr8L testFuncL, const TDesC& aTestDesc);
	static void DoOOMWithConsistencyCheckTestL(ClassFuncPtr8L testFuncL, const TDesC& aTestDesc,const TBool aShouldBeSame);
private:
	CIniParser16Test():iIniDocument(NULL){}
	CIniDocument16* iIniDocument;
};

CIniParser16Test* CIniParser16Test::NewL()
	{
	CIniParser16Test* self=new (ELeave)CIniParser16Test();
	CleanupStack::PushL(self);
	self->iIniDocument=CIniDocument16::NewL(TheRFs,KIniFile16);		
	CleanupStack::Pop();
	return self;
	}

void CIniParser16Test::DoBasicTestL(ClassFuncPtr8L testFuncL, const TDesC& aTestDesc)
	{
	test.Next(aTestDesc);

	__UHEAP_MARK;
  	// find out the number of open handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);
	
	CIniParser16Test* iniTest=CIniParser16Test::NewL();

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

void CIniParser16Test::DoOOMTestL(ClassFuncPtr8L testFuncL, const TDesC& aTestDesc)
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

		CIniParser16Test* iniTest=CIniParser16Test::NewL();
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

void CIniParser16Test::CreateDeleteOOMTestL()
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

		CIniDocument16* ini=NULL;
		TRAP(err,ini=CIniDocument16::NewL(TheRFs,KIniFile16));
		
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

void CIniParser16Test::DoOOMWithConsistencyCheckTestL(ClassFuncPtr8L testFuncL, const TDesC& aTestDesc, const TBool aShouldBeSame)
	{
	test.Next(aTestDesc);
	
	// find out the number of open handles
	TInt startProcessHandleCount = 0;
	TInt startThreadHandleCount = 0;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);
	
	//Compare the results against this instance.
	TInt err;
	CIniDocument16* referenceDoc=NULL;
	TRAP(err,referenceDoc=CIniDocument16::NewL(TheRFs,KIniFile16));
	User::LeaveIfError(err);
	CleanupStack::PushL(referenceDoc);
	
	//Open a file and Externalise the reference oom to it.
	User::LeaveIfError(referenceDoc->Externalise(_L("c:\\initest\\oom_ref16.ini")));
	CIniParser16Test* iniTest=NULL;
	
	for (TInt i = 1;;i++)
		{
		__UHEAP_FAILNEXT(i);
		__UHEAP_MARK;
		
		TRAP(err, iniTest=CIniParser16Test::NewL());
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
			TRAP(err, iniTest->iIniDocument->Externalise(_L("c:\\initest\\oom16.ini")));
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
				TRAP(err, result=CompareFilesL(TheRFs,_L("c:\\initest\\oom_ref16.ini"), _L("c:\\initest\\oom16.ini")));
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

void CIniParser16Test::CreateDeleteTest1L()
	{
	__UHEAP_MARK;

	CIniDocument16* ini=NULL;
	//note only support 16 bit Little Endian ini file
	ini=CIniDocument16::NewL(TheRFs,KIniFile16);
	
	delete ini;
	
	__UHEAP_MARKEND;
	}

void CIniParser16Test::CreateDeleteOOMTest2L()
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

		CIniFile16* ini=NULL;
		TRAP(err,ini=CIniFile16::NewL(TheRFs,KIniFile16));
		
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


void CIniParser16Test::CreateDeleteTest2L()
	{
	__UHEAP_MARK;

	CIniFile16* ini=NULL;
	//note only support 16 bit Little Endian ini file
	ini=CIniFile16::NewL(TheRFs,KIniFile16);
	
	delete ini;
	
	__UHEAP_MARKEND;
	}	


void CIniParser16Test::DoTest1L()
	{
	//Testing GetSectionList API
	RArray<TPtrC16> sectionNames;
	User::LeaveIfError(iIniDocument->GetSectionList(sectionNames));
	test(sectionNames.Count()==8);	
	
	//Testing the sectionNames in name order
	test(sectionNames[0].Compare(_L("1"))==0);
	test(sectionNames[1].Compare(_L("MAPPINGS"))==0);
	test(sectionNames[2].Compare(_L("MEDIA"))==0);
	test(sectionNames[3].Compare(_L("OUTPUT_CHANNELS"))==0);
	test(sectionNames[4].Compare(_L("SERVERS"))==0);
	test(sectionNames[5].Compare(_L("SWTRACER"))==0);
	test(sectionNames[6].Compare(_L("test_section"))==0);
	test(sectionNames[7].Compare(_L("test_twosection"))==0);	
	sectionNames.Reset();
	}

void CIniParser16Test::DoTest2L()
	{
	//Test GetKeyValue API
	TPtrC16 value;
	User::LeaveIfError(iIniDocument->GetKeyValue(_L("MEDIA"),_L("RDebug"),value));
	test(value.Compare(_L("SwtRDebugPlugin.dll"))==0);
	User::LeaveIfError(iIniDocument->GetKeyValue(_L("OUTPUT_CHANNELS"),_L("1"),value));
	test(value.Compare(_L("RDebug"))==0);
	User::LeaveIfError(iIniDocument->GetKeyValue(_L("1"),_L("new_setting"),value));
	test(value.Compare(_L("value \\n value1\\t value2"))==0);
	
	//unknown section
	TInt ret=KErrNone;
	ret=iIniDocument->GetKeyValue(_L("mySection"),_L("mykey"),value);
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);
	//unknown key
	ret=iIniDocument->GetKeyValue(_L("MEDIA"),_L("mykey"),value);
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);	
	//empty value
	ret=iIniDocument->GetKeyValue(_L("SERVERS"),_L("SWTRACER"),value);
	LeaveIfNoMemory(ret);
	test(value.Length()==0);	
	}

void CIniParser16Test::DoTest3L()
	{
	//Test AddSection API
	RArray<TPtrC16> sectionNames;
	CleanupClosePushL(sectionNames);
	User::LeaveIfError(iIniDocument->AddSection(_L("NEW-SECTION")));
	User::LeaveIfError(iIniDocument->GetSectionList(sectionNames));
	test(sectionNames.Count()==9);

	//case sensitive
	User::LeaveIfError(iIniDocument->AddSection(_L("NeW-SECTION")));	
	User::LeaveIfError(iIniDocument->GetSectionList(sectionNames));
	test(sectionNames.Count()==10);	
	//adding existing section, no duplicate allowed
	TInt ret=iIniDocument->AddSection(_L("NEW-SECTION"));
	LeaveIfNoMemory(ret);
	test(ret==KErrAlreadyExists);
	CleanupStack::PopAndDestroy();	
	}

void CIniParser16Test::DoTest4L()
	{
	//Test RemoveSection API
	RArray<TPtrC16> sectionNames;
	CleanupClosePushL(sectionNames);	
	
	//known section
	User::LeaveIfError(iIniDocument->RemoveSection(_L("SERVERS")));
	User::LeaveIfError(iIniDocument->GetSectionList(sectionNames));
	test(sectionNames.Count()==7);
	
	//check key inside section is also deleted
	TPtrC16 value;
	TInt ret=iIniDocument->GetKeyValue(_L("SERVERS"),_L("SWTRACER"),value);
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);	
	
	//unknown section	
	ret=iIniDocument->RemoveSection(_L("AnySection"));
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);
	
	CleanupStack::PopAndDestroy();		
	}
	
void CIniParser16Test::DoTest5L()
	{
	//Testing SetKey API
	TPtrC16 value;
	//Modifying existing value
	User::LeaveIfError(iIniDocument->SetKey(_L("MEDIA"),_L("RDebug"),_L("NewPlugin.dll")));
	User::LeaveIfError(iIniDocument->GetKeyValue(_L("MEDIA"),_L("RDebug"),value));
	test(value.Compare(_L("NewPlugin.dll"))==0);
	
	//nonexist key should be created
	User::LeaveIfError(iIniDocument->SetKey(_L("MEDIA"),_L("newplug"),_L("")));
	User::LeaveIfError(iIniDocument->GetKeyValue(_L("MEDIA"),_L("newplug"),value));		
	test(value.Compare(_L(""))==0);	

	//nonexist section should be created
	User::LeaveIfError(iIniDocument->SetKey(_L("unknown_section"),_L("unknown_key"),_L("unknown_value")));
	User::LeaveIfError(iIniDocument->GetKeyValue(_L("unknown_section"),_L("unknown_key"),value));
	test(value.Compare(_L("unknown_value"))==0);	
	
	//Testing Externalise to a New file
	User::LeaveIfError(iIniDocument->Externalise(_L("c:\\initest\\output16_1.ini")));

	//Try opening the written ini file now to ensure no corruption in writing
	CIniDocument16* iniReRead=CIniDocument16::NewL(TheRFs,_L("c:\\initest\\output16_1.ini"));
	CleanupStack::PushL(iniReRead);
	User::LeaveIfError(iniReRead->GetKeyValue(_L("unknown_section"),_L("unknown_key"),value));
	test(value.Compare(_L("unknown_value"))==0);	
	User::LeaveIfError(iniReRead->GetKeyValue(_L("MEDIA"),_L("newplug"),value));		
	test(value.Compare(_L(""))==0);	
	
	CleanupStack::PopAndDestroy(iniReRead);
	}
	
void CIniParser16Test::DoTest6L()
	{
	//Testing RemoveKey API
	TPtrC16 value;
	//remove existing key
	User::LeaveIfError(iIniDocument->RemoveKey(_L("OUTPUT_CHANNELS"),_L("1")));
	TInt ret=iIniDocument->GetKeyValue(_L("OUTPUT_CHANNELS"),_L("1"),value);
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);	
	
	//remove non-exist key
	ret=iIniDocument->RemoveKey(_L("OUTPUT_CHANNELS"),_L("1"));
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);	
	
	//remove non-exist section
	ret=iIniDocument->RemoveKey(_L("Non-existSection"),_L("1"));
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);	
	}

void CIniParser16Test::DoTest7L()
	{
	//Testing iterator class
	CIniSecIter16* iIniSecIter=NULL;
	TInt ret=KErrNone;
	
	//unknown section
	TRAP(ret,iIniSecIter=CIniSecIter16::NewL(_L("Unknown"),iIniDocument));
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);
	
	//null document
	TRAP(ret,iIniSecIter=CIniSecIter16::NewL(_L("Unknown"),NULL));
	LeaveIfNoMemory(ret);
	test(ret==KErrArgument);				
	
	//known section
	iIniSecIter=CIniSecIter16::NewL(_L("test_section"),iIniDocument);
	TPtrC16 key;
	TPtrC16 value;
	//test Next() and End();
	test(!iIniSecIter->End());
	test(iIniSecIter->Next(key,value));
	test(key.Compare(_L("key1"))==0);
	test(value.Compare(_L("value1"))==0);	
	test(!iIniSecIter->End());	
	test(iIniSecIter->Next(key,value));
	test(key.Compare(_L("key2"))==0);
	test(value.Compare(_L("value2"))==0);
	test(!iIniSecIter->End());	
	test(iIniSecIter->Next(key,value));
	test(key.Compare(_L("key3"))==0);
	test(value.Compare(_L("value3"))==0);
	test(!iIniSecIter->End());	
	test(iIniSecIter->Next(key,value));
	test(key.Compare(_L("key4"))==0);
	test(value.Compare(_L("value4"))==0);
	test(!iIniSecIter->End());	
	test(iIniSecIter->Next(key,value));
	test(key.Compare(_L("key5"))==0);
	test(value.Compare(_L("value value value"))==0);			
	test(iIniSecIter->End());	
	test(iIniSecIter->Next(key,value)==EFalse);
		
	//test Reset()
	iIniSecIter->Reset();
	test(!iIniSecIter->End());	
	test(iIniSecIter->Next(key,value));
	test(key.Compare(_L("key1"))==0);
	test(value.Compare(_L("value1"))==0);

	delete iIniSecIter;
	iIniSecIter=NULL;	
	}
	
void CIniParser16Test::DoTest8L()
	{
	//Testing Externalise to ROM drive
	TInt ret=iIniDocument->Externalise(_L("z:\\output16.ini"));
	LeaveIfNoMemory(ret);
	test(ret==KErrAccessDenied);
	
	//Testing Externalise to a New file
	User::LeaveIfError(iIniDocument->Externalise(_L("c:\\initest\\output16.ini")));

	//Try opening the written ini file now to ensure no corruption in writing
	CIniDocument16* output=CIniDocument16::NewL(TheRFs,_L("c:\\initest\\output16.ini"));
	CleanupStack::PushL(output);
	User::LeaveIfError(output->SetKey(_L("Test"),_L("Test"),_L("Test")));

	//Testing Externaliseing to the already exist file
	User::LeaveIfError(output->Externalise(_L("c:\\initest\\output16_1.ini")));	
	CleanupStack::PopAndDestroy();	
	
	//Opening an empty file and Externaliseing an empty file
	output=CIniDocument16::NewL(TheRFs,_L("c:\\initest\\unknown16.ini"));
	CleanupStack::PushL(output);
	User::LeaveIfError(output->Externalise(_L("c:\\initest\\unknown16_1.ini")));
	CleanupStack::PopAndDestroy();	
	
	// Read  it back in and find the additions put in above.
	CIniDocument16* iniReRead=NULL;
	iniReRead=CIniDocument16::NewL(TheRFs,_L("c:\\initest\\output16_1.ini"));
	CleanupStack::PushL(iniReRead);	
	
	TPtrC16 value;
	User::LeaveIfError(iniReRead->GetKeyValue(_L("Test"),_L("Test"),value));
	test(value.Compare(_L("Test"))==0);
	
	CleanupStack::PopAndDestroy(iniReRead);
	}

void CIniParser16Test::DoTest9L()
{
	//Test for no leakage when handling corrupt file
	CIniDocument16* ini=NULL;
	TRAPD(err,ini=CIniDocument16::NewL(TheRFs,_L("z:\\resource\\corruptconfig16.ini")));
	LeaveIfNoMemory(err);
	test(err==KErrCorrupt);
	delete ini;
}

void CIniParser16Test::DoTest10L()
{
	TPtrC16 value;
	//open existing ini file
	CIniFile16* ini=CIniFile16::NewL(TheRFs, _L16("z:\\resource\\testconfig16.ini"));
	CleanupStack::PushL(ini);
	
	//mid section
	User::LeaveIfError(ini->FindVar(_L16("test_section"),_L16("key1"),value));
	test(value.Compare(_L16("value1"))==0);
	User::LeaveIfError(ini->FindVar(_L16("test_section"),_L16("key2"),value));
	test(value.Compare(_L16("value2"))==0);
	User::LeaveIfError(ini->FindVar(_L16("test_section"),_L16("key3"),value));
	test(value.Compare(_L16("value3"))==0);
	User::LeaveIfError(ini->FindVar(_L16("test_section"),_L16("key4"),value));
	test(value.Compare(_L16("value4"))==0);
	User::LeaveIfError(ini->FindVar(_L16("test_section"),_L16("key5"),value));
	test(value.Compare(_L16("value value value"))==0);

	//first section
	User::LeaveIfError(ini->FindVar(_L16("SERVERS"),_L16("SWTRACER"),value));
	test(value.Compare(_L16(""))==0);

	//last section
	User::LeaveIfError(ini->FindVar(_L16("1"),_L16("timestamps"),value));
	test(value.Compare(_L16("0"))==0);
	User::LeaveIfError(ini->FindVar(_L16("1"),_L16("setting"),value));
	test(value.Compare(_L16("value"))==0);

	CleanupStack::PopAndDestroy();
	
	//open a non existing file
	TInt ret=KErrNone;
	TRAP(ret,ini=CIniFile16::NewL(TheRFs,_L16("z:\\resource\\nonexist.ini")));	
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);
	
	//open an empty ini file
	ini=CIniFile16::NewL(TheRFs,_L16("z:\\resource\\empty16.ini"));
	CleanupStack::PushL(ini);	
	
	ret=ini->FindVar(_L16("empty"),_L16("empty"),value);
	LeaveIfNoMemory(ret);
	test(ret==KErrNotFound);	

	CleanupStack::PopAndDestroy();	
}

void CIniParser16Test::DoTest11L()
	{
	TPtrC16 value;
	//open existing 8 bit ini file with 16 bit reader
	CIniFile16* ini;
	TInt err;
	TRAP(err,ini=CIniFile16::NewL(TheRFs,_L16("z:\\resource\\testconfig8.ini")));
	test(err==KErrCorrupt || err==KErrNoMemory);
	
	//open existing 8 bit ini file, but allow conversion to 16 bits
	ini=CIniFile16::NewL(TheRFs,_L16("z:\\resource\\testconfig8.ini"),ETrue);
	CleanupStack::PushL(ini);
	
	//mid section
	User::LeaveIfError(ini->FindVar(_L16("test_section"),_L16("key1"),value));
	test(value.Compare(_L16("value1"))==0);
	User::LeaveIfError(ini->FindVar(_L16("test_section"),_L16("key2"),value));
	test(value.Compare(_L16("value2"))==0);
	User::LeaveIfError(ini->FindVar(_L16("test_section"),_L16("key3"),value));
	test(value.Compare(_L16("value3"))==0);
	User::LeaveIfError(ini->FindVar(_L16("test_section"),_L16("key4"),value));
	test(value.Compare(_L16("value4"))==0);
	User::LeaveIfError(ini->FindVar(_L16("test_section"),_L16("key5"),value));
	test(value.Compare(_L16("value value value"))==0);

	//first section
	User::LeaveIfError(ini->FindVar(_L16("SERVERS"),_L16("SWTRACER"),value));
	test(value.Compare(_L16(""))==0);

	//last section
	User::LeaveIfError(ini->FindVar(_L16("1"),_L16("timestamps"),value));
	test(value.Compare(_L16("0"))==0);
	User::LeaveIfError(ini->FindVar(_L16("1"),_L16("setting"),value));
	test(value.Compare(_L16("value"))==0);	

	CleanupStack::PopAndDestroy();	
	}

void CIniParser16Test::DoTest12L()
	{
	TPtrC16 value;
	// We are trying to invoke an OOM condition for a single operation to test that the operation is atomic. 
	// Under that condition the object should be rolled back to the original state. The resulting document should be the same 
	// as before the condition was invoked. If the test succeeded, a new section should be created at end 
	// (which is nice but not the real focus of the test).
	User::LeaveIfError(iIniDocument->SetKey(_L16("unknown_section3"),_L16("unknown_key3"),_L16("unknown_value3")));
	}

static void DeleteFilesL()
	{
	CFileMan* fileman=CFileMan::NewL(TheRFs);

	fileman->Delete(_L("c:\\initest\\*"));

	delete fileman;
	}

/**
@SYMTestCaseID		SYSLIB-BAFL-CT-1559
@SYMTestCaseDesc 	Test CIniParser16Test
@SYMTestPriority 	High
@SYMTestActions  	Perform component tests on CIniParser16Test (includes OOM)
			Testing all the exported apis.
@SYMTestExpectedResults The test must not fail.
@SYMREQ PREQ505
*/		
static void DoTestL()
	{
	TTime startTime(0), stopTime(0);
	TTimeIntervalMicroSeconds timeTaken;
	
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1559 "));
	
	DeleteFilesL();
	//16 bit basic testing
	CIniParser16Test::CreateDeleteTest1L();
	CIniParser16Test::DoBasicTestL(&CIniParser16Test::DoTest1L,_L("GetSectionList16"));
	CIniParser16Test::DoBasicTestL(&CIniParser16Test::DoTest2L,_L("GetKeyValue16"));
	CIniParser16Test::DoBasicTestL(&CIniParser16Test::DoTest3L,_L("AddSection16"));
	CIniParser16Test::DoBasicTestL(&CIniParser16Test::DoTest4L,_L("RemoveSection16"));
	CIniParser16Test::DoBasicTestL(&CIniParser16Test::DoTest5L,_L("SetKeyValue16"));
	CIniParser16Test::DoBasicTestL(&CIniParser16Test::DoTest6L,_L("RemoveKey16"));
	CIniParser16Test::DoBasicTestL(&CIniParser16Test::DoTest7L,_L("IniSecIter16"));
	CIniParser16Test::DoBasicTestL(&CIniParser16Test::DoTest8L,_L("Externalise16"));
	CIniParser16Test::DoBasicTestL(&CIniParser16Test::DoTest9L,_L("Corrupt file16"));							
	//16 bit OOM testing
	CIniParser16Test::CreateDeleteOOMTestL();	
	CIniParser16Test::DoOOMTestL(&CIniParser16Test::DoTest1L,_L("GetSectionList16-OOM"));
	CIniParser16Test::DoOOMTestL(&CIniParser16Test::DoTest2L,_L("GetKeyValue16-OOM"));
	CIniParser16Test::DoOOMTestL(&CIniParser16Test::DoTest3L,_L("AddSection16-OOM"));	
	CIniParser16Test::DoOOMTestL(&CIniParser16Test::DoTest4L,_L("RemoveSection16-OOM"));
	CIniParser16Test::DoOOMTestL(&CIniParser16Test::DoTest5L,_L("SetKeyValue16-OOM"));				
	CIniParser16Test::DoOOMTestL(&CIniParser16Test::DoTest6L,_L("RemoveKey16-OOM"));
	CIniParser16Test::DoOOMTestL(&CIniParser16Test::DoTest7L,_L("IniSecIter16-OOM"));	
	CIniParser16Test::DoOOMTestL(&CIniParser16Test::DoTest8L,_L("Externalise16-OOM"));	
	CIniParser16Test::DoOOMTestL(&CIniParser16Test::DoTest9L,_L("Corrupt file16-OOM"));
	
	//16 bit light basic testing
	CIniParser16Test::CreateDeleteTest2L();
	CIniParser16Test::DoBasicTestL(&CIniParser16Test::DoTest10L,_L("Light FindVar16"));
	CIniParser16Test::DoBasicTestL(&CIniParser16Test::DoTest11L,_L("Load 16"));
	//16 bit light OOM testing	
	CIniParser16Test::CreateDeleteOOMTest2L();
	CIniParser16Test::DoOOMTestL(&CIniParser16Test::DoTest10L,_L("Light FindVar-OOM16"));
	CIniParser16Test::DoOOMTestL(&CIniParser16Test::DoTest11L,_L("Load 16-OOM"));
	stopTime.UniversalTime();
	timeTaken = stopTime.MicroSecondsFrom(startTime);
	test.Printf(_L("Time taken for Heavy= %d microseconds\n"), timeTaken.Int64() );	
	startTime.UniversalTime();	
	// Consistency checks
	CIniParser16Test::DoOOMWithConsistencyCheckTestL(&CIniParser16Test::DoTest12L,_L("Consistency16-OOMC"), FALSE);

	stopTime.UniversalTime();
	timeTaken = stopTime.MicroSecondsFrom(startTime);
	test.Printf(_L("Time taken for consistency checks= %d microseconds\n"), timeTaken.Int64() );	
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* trapCleanup=CTrapCleanup::New();
	test(TheRFs.Connect()==KErrNone);
	test.Start(_L("Ini File Parser Test"));

	TRAPD(error, DoTestL());
	test(error == KErrNone);
	
	
	TheRFs.Close();
	test.End();
	test.Close();
	delete trapCleanup;
	__UHEAP_MARKEND;
	return error;
	}

