/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef __TMISC_H__
#define __TMISC_H__

#include <test/testexecutestepbase.h>
#define MAX_LEN 50
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/stat.h> 
#include <pthread.h>

_LIT(Ktest_random, "test_random");
_LIT(Ktest_realloc, "test_realloc");
_LIT(Ktest_nanosleep, "test_nanosleep");
_LIT(Ktest_fileoper, "test_fileoper");
_LIT(Ktest_stdio, "test_stdio");
_LIT(KTestSysConf, "TestSysConf");
_LIT(KTestScandir, "TestScandir");
_LIT(KTestReaddir, "TestReaddir");
_LIT(KTestDiropr, "TestDiropr");
_LIT(KTestWOpendir, "TestWOpendir");
_LIT(KTestWDiropr, "TestWDiropr");
_LIT(KTestFMScalls, "TestFMScalls");
_LIT(KTestGetDirEntries, "TestGetDirEntries");
_LIT(KTestAlphasort, "TestAlphasort");
_LIT(KTestPosixSpawn, "TestPosixSpawn");
_LIT(KTestDirLseek, "TestDirLseek");
_LIT(KTestPosixSpawn1, "TestPosixSpawn1");
_LIT(KTestSystem0, "TestSystem0");
_LIT(KTestSystem1, "TestSystem1");
_LIT(KTestSystem2, "TestSystem2");
_LIT(KTestAbort1, "TestAbort1");
_LIT(KTestExit1, "TestExit1");
_LIT(KTestExit2, "TestExit2");
_LIT(KTestExitE1, "TestExitE1");
_LIT(KTestExit3, "TestExit3");
_LIT(KTestExit4, "TestExit4");
_LIT(KTestExit5, "TestExit5");
_LIT(KTestExit6, "TestExit6");
_LIT(KTestWaitpid1, "TestWaitpid1");
_LIT(KTestWaitpid2, "TestWaitpid2");
_LIT(KTestWaitpid3, "TestWaitpid3");
_LIT(Ksemoptest16, "semoptest16");
_LIT(Ksemoptest17, "semoptest17");
_LIT(Ksemoptest18, "semoptest18");
_LIT(Ksemoptest19, "semoptest19");
_LIT(Ksemoptest20, "semoptest20");
_LIT(KTestMaxFopen, "TestMaxFopen");
_LIT(KTestPipeReadWrite, "TestPipeReadWrite");
_LIT(KTestFileReadWrite, "TestFileReadWrite");
_LIT(KTestPopen3FdTransfer, "TestPopen3FdTransfer");
_LIT(KTestPopenFdTransfer, "TestPopenFdTransfer");
_LIT(KTestSystemFdTransfer, "TestSystemFdTransfer");
_LIT(KTestPosixSpawnFdTransfer, "TestPosixSpawnFdTransfer");
_LIT(KTestPopen3PipeReadTransfer, "TestPopen3PipeReadTransfer");
_LIT(KTestPopen3PipeWriteTransfer, "TestPopen3PipeWriteTransfer");
_LIT(KTestPopenPipeReadTransfer, "TestPopenPipeReadTransfer");
_LIT(KTestPopenPipeWriteTransfer, "TestPopenPipeWriteTransfer");
_LIT(KTestTzname, "TestTzname");

_LIT(KSysConfTest, "SysConfTest");
_LIT(KTestSysConf_Negative, "TestSysConf_Negative");
_LIT(Ktestclock, "testclock");
_LIT(KTestOpenWithExclusiveLock, "TestOpenWithExclusiveLock");

class CTestMisc : public CTestStep
	{
public:
	CTestMisc(const TDesC& aStepName);
	virtual ~CTestMisc(); 
	virtual TVerdict doTestStepL();

	TVerdict TestRandom();
	TVerdict TestNanoSleep();
	TVerdict TestFileOper();
	TVerdict TestRealloc();
	TVerdict TestSockAtMark();
	TVerdict TestStdio();
	TVerdict TestSysConf();	
	TVerdict TestScandir();
	TVerdict TestReaddir();
	TVerdict TestDiropr();
	TVerdict TestWOpendir();
	TVerdict TestWDiropr();
	TVerdict TestFMScalls();
	TVerdict TestGetDirEntries();
	TVerdict TestAlphasort();
	TVerdict TestPosixSpawn();	
	TVerdict testposix()	;
	TVerdict TestDirLseek()	;
	TInt TestPosixSpawn1();

	TVerdict TestSystem0()	;
	TVerdict TestSystem1()	;
	TVerdict TestSystem2()	;
	TVerdict TestAbort1()	;
	TVerdict TestExit1()	;
	TVerdict TestExit2()	;
	TVerdict TestExitE1()	;
	TVerdict TestExit3()	;
	TVerdict TestExit4()	;
	TVerdict TestExit5()	;
	TVerdict TestExit6()	;
	TVerdict TestWaitpid1();
	TVerdict TestWaitpid2();
	TVerdict TestWaitpid3();
	TInt semoptest16();
	TInt semoptest17();
	TInt semoptest18();
	TInt semoptest19();
	TInt semoptest20();
	TVerdict TestMaxFopen();
	TVerdict TestPipeReadWrite();
	TVerdict TestFileReadWrite();
	TInt TestPopen3FdTransfer()	;
	TInt TestPopenFdTransfer()	;
	TInt TestSystemFdTransfer()	;
	TInt TestPosixSpawnFdTransfer();
	TInt TestPopen3PipeReadTransfer();
	TInt TestPopen3PipeWriteTransfer();
	TInt TestPopenPipeReadTransfer();
	TInt TestPopenPipeWriteTransfer();
	TVerdict TestTzname();
	TVerdict SysConfTest();
  	TVerdict testclock();
  	TVerdict TestSysConf_Negative();
  	TVerdict TestOpenWithExclusiveLock();	
	};
#endif //__TMISC_H__
