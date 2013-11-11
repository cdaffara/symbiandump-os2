// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//  Name        : tlargefile.cpp


#include "tlargefile.h"


CTestLargefile::~CTestLargefile() 
	{ 
	}  

CTestLargefile::CTestLargefile(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestLargefile::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EFail);
	return TestStepResult();
	}



TVerdict CTestLargefile::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestLargefile::doTestStepL()
	{
	TVerdict err = EFail;
	if(TestStepName() == KTestLargeFileTruncate)
   		{
   	   	INFO_PRINTF1(_L("TestLargeFileTruncate():"));
   	   	err = TestLargeFileTruncate();
   	   	}
   	else if(TestStepName() == KTestLargeFileFtruncate)
   		{
   	   	INFO_PRINTF1(_L("TestLargeFileFtruncate():"));
   	   	err = TestLargeFileFtruncate();
   	   	}
   	else if(TestStepName() == KTestLargeFileReadWrite)
   		{
   	   	INFO_PRINTF1(_L("TestLargeFileReadWrite():"));
   	   	err = TestLargeFileReadWrite();
   	   	}
   	else if(TestStepName() == KTestLargeFileFseeknTell)
   		{
   	   	INFO_PRINTF1(_L("TestLargeFileFseeknTell():"));
   	   	err = TestLargeFileFseeknTell();
   	   	}
   	else if(TestStepName() == KTestLargeFileLseeknTell)
   		{
   	   	INFO_PRINTF1(_L("TestLargeFileLseeknTell():"));
   	   	err = TestLargeFileLseeknTell();
   	   	}
   	else if(TestStepName() == KTestCreatenFseeknPos)
   		{
   	   	INFO_PRINTF1(_L("TestCreatenFseeknPos():"));
   	   	err = TestCreatenFseeknPos();
   	   	}
   	else if(TestStepName() == KTestCreatenLseeknPos)
   		{
   	   	INFO_PRINTF1(_L("TestCreatenLseeknPos():"));
   	   	err = TestCreatenLseeknPos();
   	   	}
   	else if(TestStepName() == KTestChildFileTransfer)
   		{
   	   	INFO_PRINTF1(_L("TestChildFileTransfer():"));
   	   	err = TestChildFileTransfer();
   	   	}
   	else if(TestStepName() == KTestLargeFileFtw)
   		{
   	   	INFO_PRINTF1(_L("TestLargeFileFtw():"));
   	   	err = TestLargeFileFtw();
   	   	}
   	else if(TestStepName() == KTestLargeFileReaddir)
   		{
   	   	INFO_PRINTF1(_L("TestLargeFileReaddir():"));
   	   	err = TestLargeFileReaddir();
   	   	}
   	else if(TestStepName() == KTestLargeFileAlphasort)
   		{
   	   	INFO_PRINTF1(_L("TestLargeFileAlphasort():"));
   	   	err = TestLargeFileAlphasort();
   	   	}
   	else if(TestStepName() == KTestLargeFileWreaddir)
   		{
   	   	INFO_PRINTF1(_L("TestLargeFileWreaddir():"));
   	   	err = TestLargeFileWreaddir();
   	   	}
   	else if(TestStepName() == KTestNonsortScandir)
   		{
   	   	INFO_PRINTF1(_L("TestNonsortScandir():"));
   	   	err = TestNonsortScandir();
   	   	}
   	else if(TestStepName() == KTestSortScandir)
   		{
   	   	INFO_PRINTF1(_L("TestSortScandir():"));
   	   	err = TestSortScandir();
   	   	}
   	else if(TestStepName() == KTestFcntlwithSinglefd)
   		{
   	   	INFO_PRINTF1(_L("TestFcntlwithSinglefd():"));
   	   	err = TestFcntlwithSinglefd();
   	   	}
   	else if(TestStepName() == KTestFcntlwithDoublefds)
   		{
   	   	INFO_PRINTF1(_L("TestFcntlwithDoublefds():"));
   	   	err = TestFcntlwithDoublefds();
   	   	}
   	else if(TestStepName() == KTestLargeFileSeek)
   		{
   	   	INFO_PRINTF1(_L("TestLargeFileSeek():"));
   	   	err = TestLargeFileSeek();
   	   	}
   	else if(TestStepName() == KTestLargeFileGlob)
   		{
   	   	INFO_PRINTF1(_L("TestLargeFileGlob():"));
   	   	err = TestLargeFileGlob();
   	   	}
   	else if(TestStepName() == KTestLargeFileMmapWrite)
   		{
   	   	INFO_PRINTF1(_L("TestLargeFileMmapWrite():"));
   	   	err = TestLargeFileMmapWrite();
   	   	}
   	else if(TestStepName() == KTestLargeFileWriteMmap)
   		{
   	   	INFO_PRINTF1(_L("TestLargeFileWriteMmap():"));
   	   	err = TestLargeFileWriteMmap();
   	   	}
   	else if(TestStepName() == KTestNegSmallOffsetValues)
   		{
   	   	INFO_PRINTF1(_L("TestNegSmallOffsetValues():"));
   	   	err = TestNegSmallOffsetValues();
   	   	}
   	else if(TestStepName() == KTestNegLargeOffsetValues)
   		{
   	   	INFO_PRINTF1(_L("TestNegLargeOffsetValues():"));
   	   	err = TestNegLargeOffsetValues();
   	   	}
   	else if(TestStepName() == KTestLargeFileFreopen)
   		{
   	   	INFO_PRINTF1(_L("TestLargeFileFreopen():"));
   	   	err = TestLargeFileFreopen();
   	   	}
   	else if(TestStepName() == KTestLargeFileWfreopen)
   		{
   	   	INFO_PRINTF1(_L("TestLargeFileWfreopen():"));
   	   	err = TestLargeFileWfreopen();
   	   	}
   	else if( TestStepName() == KTestFcntlWriteLockwithMaxSize)
   	    {
   	    INFO_PRINTF1(_L("TestFcntlWriteLockwithMaxSize():"));
   	    err = TestFcntlWriteLockwithMaxSize();
   	    }
   	return err;
	}
	
 

