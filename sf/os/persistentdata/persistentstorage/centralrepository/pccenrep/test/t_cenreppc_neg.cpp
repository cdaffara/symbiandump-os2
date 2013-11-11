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

#include <x86tool/centralrepository.h>
#include <e32test.h>
#include <f32file.h>

#ifdef __TOOLS2__
#define CENREP_PC_TEST
#endif

using namespace NCentralRepositoryConstants;

enum TestMode{
	ETxt,
	ECre,
	EAutoCre,
	EAutoTxt
};

RTest TheTest(_L("Central Repository Tests"));
RFs TheFs;

const TUid uid={0x000001ff};
const TUid uidcre={0x00001FFF};

#ifndef __TOOLS2__
/** FILES FOR AUTO NEWL TESTING */
	_LIT(KPrivateTemplateCreFile,"c:\\private\\00000000\\00001fff.cre");
	_LIT(KCopyPrivateTemplateCreFile,"c:\\private\\00000000\\copy00001fff.cre");
	_LIT(KPrivateTemplateTxtFile,"c:\\private\\0000000\\000001ff.txt");
	_LIT(KPrivateTemplateTxtOutFile,"c:\\private\\00000000\\000001ff.cre");
	_LIT(KCorruptCreFile,"c:\\private\\00000000\\88888881.cre");
	_LIT(KCorruptTxtFile,"c:\\private\\00000000\\88888880.txt");	
	
#else
/** FILES FOR AUTO NEWL TESTING */	
	_LIT(KPrivateTemplateCreFile,"00001fff.cre");
	_LIT(KCopyPrivateTemplateCreFile,"copy00001fff.cre");	
	_LIT(KPrivateTemplateTxtFile,"000001ff.txt");
	_LIT(KPrivateTemplateTxtOutFile,"000001ff.cre");
	_LIT(KCorruptCreFile,"88888881.cre");
	_LIT(KCorruptTxtFile,"88888880.txt");		
#endif

//DEFINED IN THE COMMON CPP FILE
extern TInt CopyFile(const TDesC& aSource, const TDesC& aTarget);
extern void NegativeTestingL(TUid aUid,const TDesC& aInFile,const TDesC& aOutFile,TUint aTestMode);

void SetupEnv(const TDesC& aInFilePath,const TDesC& aOutFilePath,TUint aTestMode)
	{
	if (aTestMode==EAutoTxt)
		TheFs.Delete(aOutFilePath);
	if (aTestMode==EAutoCre)
		{
		TheFs.Delete(aOutFilePath);
		CopyFile(KCopyPrivateTemplateCreFile,aInFilePath);			
		}
	}

///////////////////////////////////////////////////////////////////////////////////////
// Test macro and function

LOCAL_C void Check(TInt aValue, TInt aExpected, TInt aLine)
    {
    if(aValue != aExpected)
        {
        RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
        TheTest(EFalse, aLine);
        }
    }
    
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

//Negative testing for NewL(TDesC,TDesC)
/**
Creates a CRepository object for accessing a repository specified in the input file and push it to cleanup stack
@param aInputFileName the location of the input file it should be in the format <file_path><XXXXXXXX><.txt/.cre> where XXXXXXXX is a 32 bit hex number
@param aOutputFileName the location fo the output it should be in the format <file_path><XXXXXXXX><.cre> where XXXXXXX is a 32 bit hex number
@leave KErrArgument if the file specified in the input and output do not follow the specification above
	 KErrCorrupt if the input file is corrupted
   	 KErrNoMemory if run out of memory
*/
void AdditionalPCNegativeTesting()
	{
	CRepository* repos=NULL;
	
	//not following file specification file name
	//input wrong
	TRAPD(err,repos=CRepository::NewL(_L("abc.txt"),_L("12345678.cre")));
	TEST2(err, KErrArgument);
	//output wrong
	TRAP(err,repos=CRepository::NewL(_L("12345678.txt"),_L("abc.cre")));
	TEST2(err, KErrArgument);
	
	//output specified as txt
	TRAP(err,repos=CRepository::NewL(_L("12345678.txt"),_L("87654321.txt")));
	TEST2(err, KErrArgument);
	
	//non-existent input
	TRAP(err,repos=CRepository::NewL(_L("12345678.txt"),_L("12345678.cre")));
	TEST2(err, KErrNotFound);
	
	//corrupt input
	TRAP(err,repos=CRepository::NewL(KCorruptCreFile(),_L("12345678.cre")));
	TEST2(err, KErrCorrupt);
	TRAP(err,repos=CRepository::NewL(KCorruptTxtFile(),_L("12345678.cre")));
	TEST2(err, KErrCorrupt);
	
	delete repos;
	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-4042
@SYMTestCaseDesc		PC-side CentralRepository negtive test
@SYMTestPriority		High
@SYMTestActions			Giving invalid input, checking error code returned.
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2111
*/						   
LOCAL_C void MainL()
	{
	//Note these files are equivalent, the cre being generated from txt
	TheTest.Start(_L("Repository Negative testing"));		

	User::LeaveIfError(TheFs.Connect());

	//CRE exists only
	TheTest.Next(_L("CRE Negative Testing"));
	NegativeTestingL(uidcre,KPrivateTemplateCreFile(),KPrivateTemplateCreFile(),EAutoCre);

	//TXT exists only
	TheTest.Next(_L("TXT Negative Testing"));	
	NegativeTestingL(uid,KPrivateTemplateTxtFile(),KPrivateTemplateTxtOutFile(),EAutoTxt);			

	TheTest.Next(_L("Additional PC side Negative Testing"));	
	AdditionalPCNegativeTesting();

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
