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

#include <centralrepository.h>
#include <e32test.h>
#include <f32file.h>
#include "../../test/t_cenrep_helper.h"

_LIT( KCentralRepositoryServerName, "Centralrepositorysrv");

enum TestMode{
	ETxt,
	ECre,
	EAutoCre,
	EAutoTxt
};

RTest TheTest(_L("Central Repository Tests"));
RFs TheFs;

//uid for txt testing purpose
const TUid uid={0x000001FF};
//uid for cre testing purpose
const TUid uidcre={0x00001FFF};
const TUid uidcretest = {0x000002FF};
const TUid uidmetatest = {0x00022222};

/** FILES FOR AUTO NEWL TESTING */
_LIT(KPrivateTemplateCreOutFile,"c:\\private\\10202be9\\persists\\00001fff.cre");
_LIT(KPrivateTemplateTxtOutFile,"c:\\private\\10202be9\\persists\\000001ff.cre");

_LIT(KPersistCreFileTxt,"c:\\private\\10202be9\\persists\\000001ff.cre");
_LIT(KPersistCreFileCre,"c:\\private\\10202be9\\persists\\000002ff.cre");
_LIT(KPersistMetaTestFile,"c:\\private\\10202be9\\persists\\00022222.cre");

_LIT(KTxtTestRefFile,"c:\\private\\00000000\\ref_winscwtxt.cre");
_LIT(KCreTestRefFile,"c:\\private\\00000000\\ref_winscwcre.cre");

_LIT(KMetaTestRefFile,"c:\\private\\00000000\\ref_00022222.cre");

_LIT(KCrcRecordFile,"c:\\crc.txt");

///////////////////////////////////////////////////////////////////////////////////////


//DEFINED IN THE COMMON CPP FILE
extern TInt CopyFile(const TDesC& aSource, const TDesC& aTarget);
extern void ObjectCreateDeleteOOM(TUid aUid,const TDesC& aInFilePath,const TDesC& aOutFilePath,TUint aTestMode);
extern void OomTest(void (*testFuncL)(CRepository* aRepository),TUid aUid,const TDesC& aInFilePath,const TDesC& aOutFilePath,TUint aTestMode);
extern void BasicFunctionL(TUid aUid,const TDesC& aInFilePath,const TDesC& aOutFilePath,TUint aTestMode);
extern void OomBasicFunction(TUid aUid,const TDesC& aInFilePath,const TDesC& aOutFilePath,TUint aTestMode);
extern void DoFileCompL(const TDesC& aGenerated, const TDesC& aReference, TUint32& aCrcValue);
extern void DEF130394L(TUid aUid);

void InitialiseLC(CRepository*& aRepository,TUid aUid,const TDesC& /**aInFilePath */,const TDesC& /**aOutFilePath*/,TUint /**aTestMode*/)
	{
	aRepository=CRepository::NewLC(aUid);
	}

void SetupEnv(const TDesC& /**aInFilePath*/,const TDesC& aOutFilePath,TUint aTestMode)
	{
	if (aTestMode==EAutoCre || aTestMode==EAutoTxt)
		TheFs.Delete(aOutFilePath);
	}

void CrcReferenceGenL()
	{
	KillProcess(KCentralRepositoryServerName);
	
	TheFs.Delete(KCrcRecordFile);
	
	// Auto Cre Test
	TUint32 crcCre;
	BasicFunctionL(uidcretest, KNullDesC, KPersistCreFileCre, EAutoCre);
	DoFileCompL(KPersistCreFileCre, KCreTestRefFile, crcCre);
	
	// Auto Txt Test
	TUint32 crcTxt = 0;
	BasicFunctionL(uid, KNullDesC(), KPersistCreFileTxt, EAutoTxt);
	DoFileCompL(KPersistCreFileTxt, KTxtTestRefFile, crcTxt);
	
	TBuf8<100> buf;
	buf.Format(_L8("%u-%u"), crcTxt, crcCre);
	
	RFile file;
	file.Create(TheFs, KCrcRecordFile, EFileWrite);
	file.Write(buf);
	}

/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4080
@SYMTestCaseDesc		Test for DEF130394
@SYMTestPriority		Normal
@SYMTestActions			This is Part 1 of the test - Part 2 is in t_cenrreppc.cpp
						Loading a repository multiple times and copy the created cre file
						to the test location of PC test.
@SYMTestExpectedResults A reference cre file in PC test test location
@SYMDEF					DEF130394
*/	
void TestForDEF130394L()
	{
	TheFs.Delete(KPersistMetaTestFile);
	DEF130394L(uidmetatest);
	CopyFile(KPersistMetaTestFile, KMetaTestRefFile);
	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-4043
@SYMTestCaseDesc		Client-Server-side CentralRepository functionality test
@SYMTestPriority		High
@SYMTestActions			Wrapper function calling up test functions. Test functions include test for basic
						functions of the component, OOm test and CRC comparison test.
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2111
*/						   
LOCAL_C void MainL()
	{
	//Note these files are equivalent, the cre being generated from txt
	TheTest.Start(_L("Functional TXT tests"));		

	User::LeaveIfError(TheFs.Connect());

	//CRE exists only
	TheTest.Next(_L("Functional NewLC Auto CRE tests"));	
	BasicFunctionL(uidcre,KNullDesC(),KPrivateTemplateCreOutFile(),EAutoCre);
	TheTest.Next(_L("OOM NewLC Auto CRE tests"));
	ObjectCreateDeleteOOM(uidcre,KNullDesC(),KPrivateTemplateCreOutFile(),EAutoCre);
	OomBasicFunction(uidcre,KNullDesC(),KPrivateTemplateCreOutFile(),EAutoCre);		

	//TXT exists only
	TheTest.Next(_L("Functional NewLC Auto TXT tests"));	
	BasicFunctionL(uid,KNullDesC(),KPrivateTemplateTxtOutFile(),EAutoTxt);
	TheTest.Next(_L("OOM NewLC Auto TXT tests"));					
	ObjectCreateDeleteOOM(uid,KNullDesC(),KPrivateTemplateTxtOutFile(),EAutoTxt);
	OomBasicFunction(uid,KNullDesC(),KPrivateTemplateTxtOutFile(),EAutoTxt);

#ifdef __WINSCW__ // no need to run on ARM
	TheTest.Next(_L("CRC comparison preparation for PC side test"));
	CrcReferenceGenL();
	
	TheTest.Next(_L("Test for DEF130394L"));
	TestForDEF130394L();
#endif
	
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
