// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "featregcmn.h"
#include <featreg.h>

LOCAL_D RFs	TheFs;

LOCAL_D RTest TheTest(_L("featregsetup OOM Test"));

//Test macros and functions

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

/**
@SYMTestCaseID			SYSLIB-FEATREG-CT-3373
@SYMTestCaseDesc		OOM Test for the function ReadMultipleFeatureFileToBuf
@SYMTestPriority		High
@SYMTestActions			OOM Testing the function ReadMultipleFeatureFileToBuf
						when processing multiple featreg files from different rom section
						Test relies on the additional ROFS2 and ROFS3 section
						during buildrom time
@SYMTestExpectedResults	The test must not fail.
@SYMDEF				 	DEF101195
*/	
static void CompositeOOMTesting()
	{
		TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-FEATREG-CT-3373 Composite OOM Testing "));
	TInt failAt=0;
	TInt err=KErrNone;
	do 
		{
		__UHEAP_MARK;
  
		// find out the number of open handles
		TInt pHCountStart;
		TInt tHCountStart;
		RThread().HandleCount(pHCountStart,tHCountStart);

		RBuf8 buffer;

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);

		err=ReadMultipleFeatureFileToBuf(TheFs,KFeatregRomPrivatePath,buffer);

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		buffer.Close();

		// check that no handles have leaked
		TInt pHCountEnd;
		TInt tHCountEnd;
		RThread().HandleCount(pHCountEnd,tHCountEnd);

		TEST2(pHCountStart,pHCountEnd);
		TEST2(tHCountStart,tHCountEnd);
		__UHEAP_MARKEND;
		} 
	while (err == KErrNoMemory);
	RDebug::Print(_L("Allocation successful at count=%d"),failAt);
 	TEST2(err,KErrNone);	
	}

/**
@SYMTestCaseID		SYSLIB-FEATREG-CT-3374
@SYMTestCaseDesc		Tests the correct list of features are published by featregsetup
					in a composite rom with multiple featreg files
@SYMTestPriority		High
@SYMTestActions		This will be completely testing the functionality from the client side point of view
					whether the correct feature content is published.by featregsetup when
					reading and processing multiple files.
@SYMTestExpectedResults	The test must not fail.
@SYMDEF				 	DEF101195
                        PDEF113655
*/	
static void CompositeROMTestingL()
	{
	/**	
	------------------------------------------------------------------
	COMPOSITE ROM CONFIGURATION
	MOUNTING CORE-ROFS1-ROFS2-ROFS3
	F=FEATURE; XF=EXCLUDE_FEATURE DSR=DEFAULT SUPPORTED RANGE
	------------------------------------------------------------------
	CORE	 F: Fax(0x10279806) Usb(0x1027980B)(already included in DSR),OmaDsHostServers(0x10282663)
			XF: -
		   DSR: 0x10279806 - 0x10281805 (Default techview)
	------------------------------------------------------------------	   							
	ROFS2	 F: TestAA  
			XF: TestBB, Fax, TestCC
		   DSR: 0x10279806 - 0x10281805 (Default techview)
		   		0x10288888 - 0x10299999
		   		0x102AAAAA - 0x102BBBBB
	------------------------------------------------------------------
	ROFS3	 F: TestBB, Usb, TestCC (TestCC is in DSR range)
			XF: TestAA, Usb    
		   DSR: 0x10279806 - 0x10281805 (Default techview)
		   		0x102CCCCC - 0x102DDDDD
		   		0x1CCCCCCA - 0x1CCCCCCF
	------------------------------------------------------------------	
	*/
		TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-FEATREG-CT-3374 Composite ROM Testing "));		
	__UHEAP_MARK;
	RFeatureRegistry featreg;
	User::LeaveIfError(featreg.Open());
	CleanupClosePushL(featreg);

	//SINGLE ENTRY TEST	
	//first test that the overriding feature are behaving as expected
	TUid TestAA={0x1AAAAAAA};
	TUid TestBB={0x1BBBBBBB};
	TUid Usb={0x1027980B};
	TUid Fax={0x10279806};
	TUid OmaDHS={0x10282663};
	TUid TestCC={0x1CCCCCCC};
	
	//as a result of the overriding ROFS3 will disable ROFS2 TestAA
	//and ROFS3 will enable ROFS2 TestBB
	//and ROFS2 will disable Fax from CORE
	//
	//PDEF113655: TestCC is in DSR. It is explicitly excluded in ROFS2 and is explicitly
	//included in ROFS3. This tests that TestCC has not been optimised out of the config 
	//because it is in the DSR  
	
	TUint32 info=0;
	TEST2(featreg.QuerySupport(TestAA),0);
	TEST2(featreg.QuerySupport(TestAA,info),0);	
	TEST2(info,0);
	TEST(featreg.QuerySupport(TestBB)>0);
	TEST(featreg.QuerySupport(TestBB,info)>0);	
	TEST2(info,1);
	TEST2(featreg.QuerySupport(Fax),0);
	TEST2(featreg.QuerySupport(Fax,info),0);
	TEST2(info,0);
	TEST(featreg.QuerySupport(Usb)>0);
	TEST(featreg.QuerySupport(Usb,info)>0);	
	TEST2(info,1);
	TEST(featreg.QuerySupport(OmaDHS)>0);
	TEST(featreg.QuerySupport(OmaDHS,info)>0);	
	TEST2(info,1);
	TEST(featreg.QuerySupport(TestCC)>0);
	TEST(featreg.QuerySupport(TestCC,info)>0);	
	TEST2(info,1);

	//DEFAULT SUPPORTED RANGE TEST	
	//now check the default CORE range(Exclude the fax as this is disabled)
	for (TInt i=0x10279807;i<=0x10281805;i++)
		{
		TUid featUid={i};
		TEST(featreg.QuerySupport(featUid)>0);
		TEST(featreg.QuerySupport(featUid,info)>0);		
		TEST2(info,1);		
		}
	//now check the range in ROFS2 and ROFS3
	for (TInt j=0x10288888;j<=0x10299999;j++)
		{
		TUid featUid={j};
		TEST(featreg.QuerySupport(featUid)>0);
		TEST(featreg.QuerySupport(featUid,info)>0);					
		TEST2(info,1);
		}
	for (TInt k=0x102AAAAA;k<=0x102BBBBB;k++)
		{
		TUid featUid={k};
		TEST(featreg.QuerySupport(featUid)>0);
		TEST(featreg.QuerySupport(featUid,info)>0);					
		TEST2(info,1);		
		}
	for (TInt l=0x102CCCCC;l<=0x102DDDDD;l++)
		{
		TUid featUid={l};
		TEST(featreg.QuerySupport(featUid)>0);
		TEST(featreg.QuerySupport(featUid,info)>0);
		TEST2(info,1);							
		}
	for (TInt m=0x1CCCCCCA;m<=0x1CCCCCCF;m++)
		{
		TUid featUid={m};
		TEST(featreg.QuerySupport(featUid)>0);
		TEST(featreg.QuerySupport(featUid,info)>0);
		TEST2(info,1);							
		}				
	CleanupStack::PopAndDestroy();
	__UHEAP_MARKEND;
	}
	
/**
@SYMTestCaseID		SYSLIB-FEATREG-CT-3375
@SYMTestCaseDesc		Tests Corrupt file handling of ReadMultipleFeatureFileToBuf
@SYMTestPriority		High
@SYMTestActions		This test will test that if one of the configuration files is corrupt, the
					ReadMultipleFeatureFileToBuf function will always return KErrCorrupt, also
					ensure there is no memory leak when it returned KErrCorrupt.
@SYMTestExpectedResults	The test must not fail.
@SYMDEF				 DEF101195
*/
static void CompositeCorruptFileTesting()	
	{
		TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-FEATREG-CT-3375 Composite Corrupt Testing "));
	__UHEAP_MARK;
	
	//both test files will be stored in z:\featregcomposite
	//the featreg.cfg is ok but featreg.cfg[1-0] is a corrupt file
	_LIT(KTestDirectory,"z:\\featregcomposite\\");
	RBuf8 buffer;
	TInt err=ReadMultipleFeatureFileToBuf(TheFs,KTestDirectory,buffer);	
	TEST2(err,KErrCorrupt);
	
	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID		SYSLIB-FEATREG-CT-3471
@SYMTestCaseDesc		List Config Files Names Under Featreg Data cage
@SYMTestPriority		High
@SYMTestActions		This test returns in a list format the names of all the featreg config files in its data cage.
					In order to validate EC114 Changes for FeatReg
@SYMTestExpectedResults	Names of featreg config files is returned.
@SYMDEF				 DEF104374
*/
static void OutputFileNameL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-FEATREG-CT-3471 "));
	_LIT(KFeatRegDir,"z:\\private\\102744CA\\*");
	
	CDir* dirList;
	
	User::LeaveIfError(TheFs.GetDir(KFeatRegDir, KEntryAttMaskSupported,ESortByName,dirList));
	
	RDebug::Print(_L("These are the files under z:\\private\\102744CA\\ "));
	
	for (TInt i=0;i<dirList->Count();i++)
		{
		RDebug::Print(_L(" %S "), &(*dirList)[i].iName);
		}
	    
	delete dirList;	
	}


//Note that tests can be run in both emulator(automatic) and hardware(manual) however with some difference:
//Emulator: Test relies on pre-generated cfg files which is copied and removed using a batch file
//Hardware: Test will rely on the buildrom to generate the cfg files for each rom section, as this involes
//			flashing additional rom sections to Techviwe NAND2 rom, this test is manual
LOCAL_C void RunTestL()
	{
	User::LeaveIfError(TheFs.Connect());
	
	CompositeCorruptFileTesting();
	CompositeOOMTesting();	
	CompositeROMTestingL();
	
	OutputFileNameL();
	
	TheFs.Close();
	}

LOCAL_C TInt MainL()
{
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	TRAPD(err,RunTestL());
    TEST2(err, KErrNone);
    
	delete scheduler;
}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	TheTest.Title();
	TheTest.Start(_L("Featregsetup Composite Testing"));

	CTrapCleanup* cleanup = CTrapCleanup::New();

    TRAPD(err,MainL());
	TEST2(err, KErrNone);

	delete cleanup;
	
	TheTest.End();
	TheTest.Close();
	
	__UHEAP_MARKEND;
	return(0);
	}
