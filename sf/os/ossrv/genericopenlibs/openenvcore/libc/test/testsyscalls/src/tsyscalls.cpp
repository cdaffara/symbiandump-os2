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



#include "tsyscalls.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>  
#include <sys/utsname.h>


CTestSyscalls::~CTestSyscalls() 
	{ 
	}  

CTestSyscalls::CTestSyscalls(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestSyscalls::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestSyscalls::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestSyscalls::doTestStepL()
	{
	int err;
	if(TestStepName() == KCreat)
   		{
   		INFO_PRINTF1(_L("Creat():"));
   		err = Creat();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kopen1)
		{
		INFO_PRINTF1(_L("open1():"));
		err = open1();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
   	   	
	else if(TestStepName() == Kopen2)
		{
		INFO_PRINTF1(_L("open2():"));
		err = open2();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == Kopen3)
		{
		INFO_PRINTF1(_L("open3():"));
		err = open3();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == Kopen4)
		{
		INFO_PRINTF1(_L("open4():"));
		err = open4();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == Kopen5)
		{
		INFO_PRINTF1(_L("open5():"));
		err = open5();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == Kopen6)
		{
		INFO_PRINTF1(_L("open6():"));
		err = open6();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == KOpenTruncate1)
		{
		INFO_PRINTF1(_L("OpenTruncate1:"));
		err = OpenTruncate1();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == KOpenTruncate2)
		{
		INFO_PRINTF1(_L("OpenTruncate2:"));
		err = OpenTruncate2();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == Kopen7)
   		{
   		INFO_PRINTF1(_L("open7():"));
   		err = open7();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == KOpenInAppendMode)
   		{
   		INFO_PRINTF1(_L("OpenInAppendMode():"));
   		err = OpenInAppendMode();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == Kwrite1)
		{
   		INFO_PRINTF1(_L("write1():"));
		err = write1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kwrite2)
   		{
   		INFO_PRINTF1(_L("write2():"));
   		err = write2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == Kwrite3)
   		{
   		INFO_PRINTF1(_L("write3():"));
   		err = write3();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kwrite5)
		{
   		INFO_PRINTF1(_L("write5():"));
   		err = write5();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kread1)
   		{
   		INFO_PRINTF1(_L("read1():"));
   		err = read1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kread2)
   		{
		INFO_PRINTF1(_L("read2():"));
   		err = read2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kread3)
   		{
		INFO_PRINTF1(_L("read3():"));
   		err = read3();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kread4)
		{
		INFO_PRINTF1(_L("read4():"));
		err = read4();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == KOpendir)
   		{
   	   	INFO_PRINTF1(_L("Opendir():"));
   	   	err = Opendir();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KClosedir)
   		{
   	   	INFO_PRINTF1(_L("Closedir():"));
   	   	err = Closedir();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KReaddir)
   		{
   	   	INFO_PRINTF1(_L("Readdir():"));
   	   	err = Readdir();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KLseek)
   		{
   	   	INFO_PRINTF1(_L("Lseek():"));
   	   	err = Lseek();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KLseek1)
   		{
   	   	INFO_PRINTF1(_L("Lseek1():"));
   	   	err = Lseek1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
    else if(TestStepName() == KAccess)
   		{
   	   	INFO_PRINTF1(_L("Access():"));
   	   	err = Access();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KAccess1)
   		{
   	   	INFO_PRINTF1(_L("Access1():"));
   	   	err = Access1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KDup)
   		{
   	   	INFO_PRINTF1(_L("Dup():"));
   	   	err = Dup();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KDup2)
   		{
   	   	INFO_PRINTF1(_L("Dup2():"));
   	   	err = Dup2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KRename)
   		{
   	   	INFO_PRINTF1(_L("Rename():"));
   	   	err = Rename();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KRename1)
   		{
   	   	INFO_PRINTF1(_L("Rename1():"));
   	   	err = Rename1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KChmod)
   		{
   	   	INFO_PRINTF1(_L("Chmod():"));
   	   	err = Chmod();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KChmod1)
   		{
   	   	INFO_PRINTF1(_L("Chmod1():"));
   	   	err = Chmod1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
    else if(TestStepName() == KChmod_dir)
   		{
   	   	INFO_PRINTF1(_L("Chmod_dir():"));
   	   	err = Chmod_dir();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KFChmod)
   		{
   	   	INFO_PRINTF1(_L("FChmod():"));
   	   	err = FChmod();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KFChmod_dir)
   		{
   	   	INFO_PRINTF1(_L("FChmod_dir():"));
   	   	err = FChmod_dir();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KExit)
   		{
   	   	INFO_PRINTF1(_L("Exit():"));
   	   	err = Exit();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KClose)
   		{
   	   	INFO_PRINTF1(_L("Close():"));
   	   	err = Close();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KMkdir)
   		{
   	   	INFO_PRINTF1(_L("Mkdir():"));
   	   	err = Mkdir();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KMk_dir)
   		{
   	   	INFO_PRINTF1(_L("Mk_dir():"));
   	   	err = Mk_dir();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KRmdir)
   		{
   	   	INFO_PRINTF1(_L("Rmdir():"));
   	   	err = Rmdir();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KRm_dir)
   		{
   	   	INFO_PRINTF1(_L("Rm_dir():"));
   	   	err = Rm_dir();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
    else if(TestStepName() == KRmdir1)
   		{
   	   	INFO_PRINTF1(_L("Rmdir1():"));
   	   	err = Rmdir1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KRmdir_Chdir)
   		{
   	   	INFO_PRINTF1(_L("Rmdir_Chdir():"));
   	   	err = Rmdir_Chdir();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KFsync)
   		{
   	   	INFO_PRINTF1(_L("Fsync():"));
   	   	err = Fsync();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KUtimes)
   		{
   	   	INFO_PRINTF1(_L("Utimes():"));
   	   	err = Utimes();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
   	else if(TestStepName() == KUtime)
   		{
   	   	INFO_PRINTF1(_L("Utime():"));
   	   	err = Utime();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KChdir)
   		{
   	   	INFO_PRINTF1(_L("Chdir():"));
   	   	err = Chdir();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KFcntl)
   		{
   	   	INFO_PRINTF1(_L("Fcntl():"));
   	   	err = Fcntl();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KIoctl)
   		{
   	   	INFO_PRINTF1(_L("Ioctl():"));
   	   	err = Ioctl();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KFstat)
   		{
   	   	INFO_PRINTF1(_L("Fstat():"));
   	   	err = Fstat();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KStat)
   		{
   	   	INFO_PRINTF1(_L("Stat():"));
   	   	err = Stat();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KStat1)
   		{
   	   	INFO_PRINTF1(_L("Stat1():"));
   	   	err = Stat1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KStat2)
   		{
   	   	INFO_PRINTF1(_L("Stat2():"));
   	   	err = Stat2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KStat3)
   		{
   	   	INFO_PRINTF1(_L("Stat3():"));
   	   	err = Stat3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KGetpid)
   		{
   	   	INFO_PRINTF1(_L("Getpid():"));
   	   	err = Getpid();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KClock)
   		{
   	   	INFO_PRINTF1(_L("Clock():"));
   	   	err = Clock();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTime)
   		{
   	   	INFO_PRINTF1(_L("Time():"));
   	   	err = Time();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KWaitPid)
   		{
   	   	INFO_PRINTF1(_L("WaitPid():"));
   	   	err = WaitPid();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KReadV)
   		{
   	   	INFO_PRINTF1(_L("ReadV():"));
   	   	err = ReadV();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KWriteV)
   		{
   	   	INFO_PRINTF1(_L("WriteV():"));
   	   	err = WriteV();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KSleep)
   		{
   	   	INFO_PRINTF1(_L("Sleep():"));
   	   	err = Sleep();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KSeekDir)
   		{
   	   	INFO_PRINTF1(_L("SeekDir():"));
   	   	err = SeekDir();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KRewindDir)
   		{
   	   	INFO_PRINTF1(_L("RewindDir():"));
   	   	err = RewindDir();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTelldir)
   		{
   	   	INFO_PRINTF1(_L("Telldir():"));
   	   	err = Telldir();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestClock)
   		{
   	   	INFO_PRINTF1(_L("TestClock():"));
   	   	err = TestClock();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KCreat2)
   		{
   		INFO_PRINTF1(_L("Creat2():"));
   		err = Creat2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kopen8)
   		{
   		INFO_PRINTF1(_L("open8():"));
   		err = open8();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestStat)
   		{
   		INFO_PRINTF1(_L("KTestStat():"));
   		err = TestStat();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == KLseekttytest1)
   		{
   		INFO_PRINTF1(_L("Lseekttytest1():"));
   		err = Lseekttytest1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == KLseekttytest2)
   		{
   		INFO_PRINTF1(_L("Lseekttytest2():"));
   		err = Lseekttytest2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == KWaitPidtest)
   		{
   		INFO_PRINTF1(_L("WaitPidtest():"));
   		err = WaitPidtest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == KWaittest)
   		{
   		INFO_PRINTF1(_L("Waittest():"));
   		err = Waittest();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == KOpen_FileDes_Test)
   		{
   		INFO_PRINTF1(_L("Open_FileDes_Test():"));
   		err = Open_FileDes_Test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == Kopenuid)
   		{
   		INFO_PRINTF1(_L("openuid():"));
   		err = openuid();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == KMkdir1)
   		{
   	   	INFO_PRINTF1(_L("Mkdir1():"));
   	   	err = Mkdir1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KMkdir2)
   		{
   	   	INFO_PRINTF1(_L("Mkdir2():"));
   	   	err = Mkdir2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KRename2)
   		{
   	   	INFO_PRINTF1(_L("Rename2():"));
   	   	err = Rename2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Ktestfsync)
   		{
   		INFO_PRINTF1(_L("testfsync():"));
   		err = testfsync();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == Ktestrename)
   		{
   		INFO_PRINTF1(_L("testrename():"));
   		err = testrename();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == Ktestopenvalidate)
   		{
   		INFO_PRINTF1(_L("testopenvalidate():"));
   		err = testopenvalidate();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == Ksync_safe)
   		{
   		INFO_PRINTF1(_L("sync_safe():"));
   		err = sync_safe();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
   	else if(TestStepName() == KFstat1)
   		{
   	   	INFO_PRINTF1(_L("Fstat1():"));
   	   	err = Fstat1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KUtimes1)
   		{
   	   	INFO_PRINTF1(_L("Utimes1():"));
   	   	err = Utimes1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == KMkdir_test1)
   		{
   	   	INFO_PRINTF1(_L("Mkdir_test1():"));
   	   	err = Mkdir_test1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KChmod_test)
   		{
   	   	INFO_PRINTF1(_L("Chmod_test():"));
   	   	err = Chmod_test();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KChdir1)
   		{
   	   	INFO_PRINTF1(_L("Chdir1():"));
   	   	err = Chdir1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   
   	else if(TestStepName() == KRmdir2)
   		{
   	   	INFO_PRINTF1(_L("Rmdir2():"));
   	   	err = Rmdir2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KRename_test)
   		{
   	   	INFO_PRINTF1(_L("Rename_test():"));
   	   	err = Rename_test();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KRename3)
   		{
   	   	INFO_PRINTF1(_L("Rename3():"));
   	   	err = Rename3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KCreat1)
   		{
   		INFO_PRINTF1(_L("Creat1():"));
   		err = Creat1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KReadV1)
   		{
   	   	INFO_PRINTF1(_L("ReadV1():"));
   	   	err = ReadV1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
 	else if(TestStepName() == KUtimes2)
    		{
    	   	INFO_PRINTF1(_L("Utimes2():"));
    	   	err = Utimes2();
    	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
 		}
 	else if(TestStepName() == KStat_test)
    		{
    	   	INFO_PRINTF1(_L("Stat_test():"));
    	   	err = Stat_test();
    	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
    	   	}
    	else if(TestStepName() == KMkdir_test2)
    		{
    	   	INFO_PRINTF1(_L("Mkdir_test2():"));
    	   	err = Mkdir_test2();
    	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
    	   	}
    	else if(TestStepName() == KChmod2)
    		{
    	   	INFO_PRINTF1(_L("Chmod2():"));
    	   	err = Chmod2();
    	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
    	   	}
    	else if(TestStepName() == KChdir2)
    		{
    	   	INFO_PRINTF1(_L("Chdir2():"));
    	   	err = Chdir2();
    	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
    	   	} 
    	else if(TestStepName() == KRename4)
    		{
    	   	INFO_PRINTF1(_L("Rename4():"));
    	   	err = Rename4();
    	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
    	   	}
 	else if(TestStepName() == KRename5)
    		{
    	   	INFO_PRINTF1(_L("Rename5():"));
    	   	err = Rename5();
    	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
    	   	}  
 	else if(TestStepName() == KRmdir3)
    		{
    	   	INFO_PRINTF1(_L("Rmdir3():"));
    	   	err = Rmdir3();
    	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
    	   	}  
    	else if(TestStepName() == Kread5)
 		{
 		INFO_PRINTF1(_L("read5():"));
 		err = read5();
    		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
    		} 	   	 
	return TestStepResult(); 
	}


