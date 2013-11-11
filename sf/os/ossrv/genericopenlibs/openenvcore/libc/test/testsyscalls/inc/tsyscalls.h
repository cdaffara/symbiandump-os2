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


#ifndef __TESTSYSCALLS_H__
#define __TESTSYSCALLS_H__

#include <test/testexecutestepbase.h>

_LIT(KCreat, "Creat"); 
_LIT(Kopen1, "open1"); 
_LIT(Kopen2, "open2"); 
_LIT(Kopen3, "open3"); 
_LIT(Kopen4, "open4");
_LIT(Kopen5, "open5");
_LIT(Kopen6, "open6");
_LIT(KOpenTruncate1, "OpenTruncate1");
_LIT(KOpenTruncate2, "OpenTruncate2");
_LIT(Kopen7, "open7");
_LIT(KOpenInAppendMode, "OpenInAppendMode");

_LIT(Kwrite1, "write1");
_LIT(Kwrite2, "write2");
_LIT(Kwrite3, "write3");
_LIT(Kwrite5, "write5");

_LIT(Kread1, "read1");
_LIT(Kread2, "read2");
_LIT(Kread3, "read3");
_LIT(Kread4, "read4");

_LIT(KOpendir, "Opendir");
_LIT(KClosedir, "Closedir");

_LIT(KReaddir, "Readdir");
_LIT(KLseek, "Lseek");
_LIT(KLseek1, "Lseek1");
_LIT(KAccess, "Access");
_LIT(KAccess1, "Access1");
_LIT(KDup, "Dup");
_LIT(KDup2, "Dup2");


_LIT(KRename, "Rename");
_LIT(KRename1, "Rename1");
_LIT(KRename2, "Rename2");

_LIT(KChmod, "Chmod");
_LIT(KChmod1, "Chmod1");
_LIT(KChmod_dir, "Chmod_dir");
_LIT(KFChmod, "FChmod");
_LIT(KFChmod_dir, "FChmod_dir");

_LIT(KExit, "Exit");
_LIT(KClose, "Close");

_LIT(KMkdir, "Mkdir");
_LIT(KMk_dir, "Mk_dir");

_LIT(KRmdir, "Rmdir");
_LIT(KRm_dir, "Rm_dir");
_LIT(KRmdir1, "Rmdir1");
_LIT(KRmdir_Chdir, "Rmdir_Chdir");

_LIT(KFsync, "Fsync");

_LIT(KUtimes, "Utimes");
_LIT(KUtime, "Utime");

_LIT(KChdir, "Chdir");

_LIT(KFcntl, "Fcntl"); 
_LIT(KIoctl, "Ioctl");
_LIT(KFstat, "Fstat");
_LIT(KStat, "Stat");
_LIT(KStat1, "Stat1");
_LIT(KStat2, "Stat2");
_LIT(KStat3, "Stat3");
_LIT(KGetpid, "Getpid");
_LIT(KClock, "Clock");
_LIT(KTime, "Time");
_LIT(KWaitPid, "WaitPid");
_LIT(KReadV, "ReadV");
_LIT(KWriteV, "WriteV");
_LIT(KSleep, "Sleep");
_LIT(KSeekDir, "SeekDir");
_LIT(KRewindDir, "RewindDir");
_LIT(KTelldir, "Telldir");
_LIT(KTestClock, "TestClock");

_LIT(KCreat2, "Creat2");
_LIT(Kopen8, "open8");
_LIT(KTestStat, "TestStat");
_LIT(KLseekttytest1, "Lseekttytest1");
_LIT(KLseekttytest2, "Lseekttytest2");
_LIT(KWaitPidtest, "WaitPidtest");
_LIT(KWaittest, "Waittest");
_LIT(KOpen_FileDes_Test,"Open_FileDes_Test");
_LIT(Kopenuid, "openuid");
_LIT(KMkdir1, "Mkdir1");
_LIT(KMkdir2, "Mkdir2");

_LIT(Ktesttmpopen, "testtmpopen");
_LIT(Ktestfsync, "testfsync");
_LIT(Ktestrename, "testrename");
_LIT(Ktestopenvalidate, "testopenvalidate");
_LIT(Ksync_safe, "sync_safe");
_LIT(KFstat1, "Fstat1");
_LIT(KUtimes1, "Utimes1");
_LIT(KMkdir_test1, "Mkdir_test1");
_LIT(KChmod_test, "Chmod_test");
_LIT(KChdir1, "Chdir1");
_LIT(KRmdir2, "Rmdir2");
_LIT(KRename_test, "Rename_test");
_LIT(KRename3, "Rename3");
_LIT(KCreat1, "Creat1");
_LIT(KReadV1, "ReadV1");
_LIT(KUtimes2, "Utimes2");
_LIT(KStat_test, "Stat_test");
_LIT(KMkdir_test2, "Mkdir_test2");
_LIT(KChmod2, "Chmod2");
_LIT(KChdir2, "Chdir2");
_LIT(KRename4, "Rename4");
_LIT(KRename5, "Rename5");
_LIT(KRmdir3, "Rmdir3");
_LIT(Kread5, "read5");

class CTestSyscalls : public CTestStep
	{
public:
	~CTestSyscalls(); 
	CTestSyscalls(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	TInt Creat(); 
   	TInt open1(); 
   	TInt open2(); 
   	TInt open3(); 
   	TInt open4();
   	TInt open5();
   	TInt open6();
   	TInt OpenTruncate1();
   	TInt OpenTruncate2();
   	TInt open7();
   	TInt OpenInAppendMode();
 
   	TInt write1();
   	TInt write2();
   	TInt write3();
   	TInt write5();
 
   	TInt read1();
   	TInt read2();
   	TInt read3();
   	TInt read4();
 
   	TInt Opendir();
   	TInt Closedir();

   	TInt Readdir();
   	TInt Lseek();
   	TInt Lseek1();
   	TInt Access();
   	TInt Access1();
   	TInt Dup();
   	TInt Dup2();

   
   	TInt Rename();
   	TInt Rename1();
   
   	TInt Chmod();
   	TInt Chmod1();
   	TInt Chmod_dir();
   	TInt FChmod();
   	TInt FChmod_dir();

   	TInt Exit();
   	TInt Close();

   	TInt Mkdir();
	TInt Mk_dir();
	
   	TInt Rmdir();
   	TInt Rm_dir();
   	TInt Rmdir1();
   	TInt Rmdir_Chdir();

   	TInt Fsync();

   	TInt Utimes();
   	TInt Utime();

   	TInt Chdir();

   	TInt Fcntl(); 
   	TInt Ioctl();
   	TInt Fstat();
   	TInt Stat();
   	TInt Stat1();
   	TInt Stat2();
	TInt Stat3();
   	TInt Getpid();
   	TInt Clock();
   	TInt Time();
   	TInt WaitPid();
   	TInt ReadV();
   	TInt WriteV();
   	TInt Sleep();
   	TInt SeekDir();
   	TInt RewindDir();
   	TInt Telldir();
   	TInt TestClock();
   	
   	TInt Creat2();
   	TInt open8();
   	TInt TestStat();
   	TInt Lseekttytest1();
   	TInt Lseekttytest2();
   	TInt WaitPidtest();
   	TInt Waittest();
   	TInt openuid();
   	TInt Rename2();
   	TInt CTestSyscalls :: Writev_Dir_Test(char*) ;
   	TInt CTestSyscalls :: Writev_Invalid_Fd(void);
   	TInt CTestSyscalls :: Writev_Readfd_Test(char*);
   	TInt CTestSyscalls :: Writev_Invalidcount_Test(char*, TInt);
   	TInt CTestSyscalls :: Writev_AddInvalid_Test(char*,TInt);
   	TInt CTestSyscalls :: Writev_MaxIov_Test(char*);
   	TInt CTestSyscalls :: Writev_Negativecount_Test(char*,TInt);
   	TInt CTestSyscalls :: Writev_Normal(char*);
   	TInt CTestSyscalls :: Readv_Normal(char*);
   	TInt CTestSyscalls :: Readv_Dir_Test(char*);
   	TInt CTestSyscalls :: Readv_Invalid_Fd(void);
   	TInt CTestSyscalls :: Readv_Writefd_Test(char*);
   	TInt CTestSyscalls :: Readv_AddInvalid_Test(char*,TInt);
   	TInt CTestSyscalls :: Readv_MaxIov_Test(char*);
   	TInt CTestSyscalls :: Readv_Invalidcount_Test(char*,TInt);
   	TInt CTestSyscalls :: Readv_Negativecount_Test(char*,TInt);
   	TInt CTestSyscalls :: Open_FileDes_Test();
	TInt Mkdir1();
   	TInt Mkdir2();
   	TInt testfsync();
   	TInt testrename();
   	TInt testopenvalidate();
	TInt sync_safe();
	TInt Fstat1();
   	TInt Utimes1();
   	TInt Mkdir_test1();
   	TInt Chmod_test();
   	TInt Chdir1();
   	TInt Rmdir2();
   	TInt Rename_test();
   	TInt Rename3();
   	TInt Creat1();
   	TInt ReadV1();
    	TInt Utimes2();
    	TInt Stat_test();
    	TInt Mkdir_test2();
    	TInt Chmod2();
    	TInt Chdir2();
    	TInt Rename4();
    	TInt Rename5();
    	TInt Rmdir3();
    	TInt read5();

	};
#endif 
