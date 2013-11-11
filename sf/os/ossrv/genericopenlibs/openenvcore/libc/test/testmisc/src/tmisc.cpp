// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tmisc.cpp
// 
//


#include "tmisc.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <wchar.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <spawn.h>
#include <sched.h>
#include <sys/wait.h>
#include <sys/syslimits.h>
#include <time.h>

#define MAXVAL 3
int counter;

void* thread1(void* arg);
void* thread2(void* arg);
void* thread3(void* arg);

CTestMisc::~CTestMisc() 
	{ 
	}  

CTestMisc::CTestMisc(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestMisc::doTestStepL()
	{
	__UHEAP_MARK;
	int err;
	TVerdict err1;
   	if(TestStepName() == Ktest_random)
   		{
   		INFO_PRINTF1(_L("TestRandom():"));
   	   	err = TestRandom();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Ktest_nanosleep)
   		{
   		INFO_PRINTF1(_L("TestNanoSleep():"));
   	   	err = TestNanoSleep();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   	   	
   	else if(TestStepName() == Ktest_fileoper)
   		{
   			
   		INFO_PRINTF1(_L("TestFileOper():"));
   	   	err = TestFileOper();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Ktest_stdio)
   		{
   		INFO_PRINTF1(_L("TestStdio():"));
   	   	err = TestStdio();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   	   	
   	else if(TestStepName() == Ktest_realloc)
   		{
   		INFO_PRINTF1(_L("TestRealloc():"));
   	   	err = TestRealloc();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
    else if(TestStepName() == KTestSysConf)
   		{
   		INFO_PRINTF1(_L("TestSysConf():"));
   	   	err = TestSysConf();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestScandir)
   		{
   		INFO_PRINTF1(_L("TestScandir():"));
   	   	err = TestScandir();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestReaddir)
   		{
   		INFO_PRINTF1(_L("TestReaddir():"));
   	   	err = TestReaddir();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestDiropr)
   		{
   		INFO_PRINTF1(_L("TestDiropr():"));
   	   	err = TestDiropr();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestWOpendir)
   		{
   		INFO_PRINTF1(_L("TestWOpendir():"));
   	   	err = TestWOpendir();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestWDiropr)
   		{
   		INFO_PRINTF1(_L("TestWDiropr():"));
   	   	err = TestWDiropr();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestFMScalls)
   		{
   		INFO_PRINTF1(_L("TestFMScalls():"));
   	   	err = TestFMScalls();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestGetDirEntries)
   		{
   		INFO_PRINTF1(_L("TestGetDirEntries():"));
   	   	err = TestGetDirEntries();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestAlphasort)
   		{
   		INFO_PRINTF1(_L("TestAlphasort():"));
   	   	err = TestAlphasort();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestPosixSpawn)
   		{
   		INFO_PRINTF1(_L("TestPosixSpawn():"));
   	   	err = TestPosixSpawn();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   	else if(TestStepName() == KTestPosixSpawn1)
   		{
   		INFO_PRINTF1(_L("TestPosixSpawn1():"));
   	   	err = TestPosixSpawn1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   	   	   	   	 
 	   	   	   	 
	else if(TestStepName() == KTestSystem0)
   		{
   		INFO_PRINTF1(_L("TestSystem0():"));
   	   	err = TestSystem0();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestSystem1)
   		{
   		INFO_PRINTF1(_L("TestSystem1():"));
   	   	err = TestSystem1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestSystem2)
   		{
   		INFO_PRINTF1(_L("TestSystem2():"));
   	   	err = TestSystem2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestAbort1)
   		{
   		INFO_PRINTF1(_L("TestAbort1():"));
   	   	err = TestAbort1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestExit1)
   		{
   		INFO_PRINTF1(_L("TestExit1():"));
   	   	err = TestExit1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestExit2)
   		{
   		INFO_PRINTF1(_L("TestExit2():"));
   	   	err = TestExit2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestExitE1)
   		{
   		INFO_PRINTF1(_L("TestExitE1():"));
   	   	err = TestExitE1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
  	else if(TestStepName() == KTestExit3)
   		{
   		INFO_PRINTF1(_L("TestExit3():"));
   	   	err = TestExit3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
  	else if(TestStepName() == KTestExit4)
   		{
   		INFO_PRINTF1(_L("TestExit4():"));
   	   	err = TestExit4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestExit5)
   		{
   		INFO_PRINTF1(_L("TestExit5():"));
   	   	err = TestExit5();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestExit6)
   		{
   		INFO_PRINTF1(_L("TestExit6():"));
   	   	err = TestExit6();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}		
   	else if(TestStepName() == KTestWaitpid1)
   		{
   		INFO_PRINTF1(_L("TestWaitpid1():"));
   	   	err = TestWaitpid1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestWaitpid2)
   		{
   		INFO_PRINTF1(_L("TestWaitpid2():"));
   	   	err = TestWaitpid2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestWaitpid3)
   		{
   		INFO_PRINTF1(_L("TestWaitpid3():"));
   	   	err = TestWaitpid3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   	  
	else if(TestStepName() == Ksemoptest16 )
		{
		INFO_PRINTF1(_L("semoptest16():"));
		err = semoptest16();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
   	else if(TestStepName() == Ksemoptest17 )
		{
		INFO_PRINTF1(_L("semoptest17():"));
		err = semoptest17();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
   	else if(TestStepName() == Ksemoptest18 )
		{
		INFO_PRINTF1(_L("semoptest18():"));
		err = semoptest18();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
   	else if(TestStepName() == Ksemoptest19 )
		{
		INFO_PRINTF1(_L("semoptest19():"));
		err = semoptest19();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
   	else if(TestStepName() == Ksemoptest20 )
		{
		INFO_PRINTF1(_L("semoptest20():"));
		err = semoptest20();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		} 	
	else if(TestStepName() == KTestMaxFopen)
   		{
   		INFO_PRINTF1(_L("TestMaxFopen():"));
   	   	err = TestMaxFopen();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestPopen3FdTransfer)
	   	{
	   		INFO_PRINTF1(_L("TestPopen3FdTransfer():"));
	   	   	err = TestPopen3FdTransfer();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}
	else if(TestStepName() == KTestPopenFdTransfer)
	   	{
	   		INFO_PRINTF1(_L("TestPopenFdTransfer():"));
	   	   	err = TestPopenFdTransfer();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	}
	else if(TestStepName() == KTestSystemFdTransfer)
		{
			INFO_PRINTF1(_L("TestSystemFdTransfer():"));
		   	err = TestSystemFdTransfer();
		   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == KTestPosixSpawnFdTransfer)
		{
			INFO_PRINTF1(_L("TestPosixSpawnFdTransfer():"));
			err = TestPosixSpawnFdTransfer();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
   	
	else if(TestStepName() == KTestPopen3PipeReadTransfer)
		{
			INFO_PRINTF1(_L("TestPopen3PipeReadTransfer():"));
			err = TestPopen3PipeReadTransfer();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
   	
	else if(TestStepName() == KTestPopen3PipeWriteTransfer)
		{
			INFO_PRINTF1(_L("TestPopen3PipeWriteTransfer():"));
			err = TestPopen3PipeWriteTransfer();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
   	
	else if(TestStepName() == KTestPopenPipeReadTransfer)
		{
			INFO_PRINTF1(_L("TestPopenPipeReadTransfer():"));
			err = TestPopenPipeReadTransfer();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
   	
	else if(TestStepName() == KTestPopenPipeWriteTransfer)
		{
			INFO_PRINTF1(_L("TestPopenPipeWriteTransfer():"));
			err = TestPopenPipeWriteTransfer();
			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == KTestFileReadWrite)
   		{
   		INFO_PRINTF1(_L("TestFileReadWrite():"));
   	   	err = TestFileReadWrite();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	   	
	else if(TestStepName() == KTestPipeReadWrite)
   		{
   		INFO_PRINTF1(_L("TestPipeReadWrite():"));
   	   	err = TestPipeReadWrite();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestTzname)
   		{
   		INFO_PRINTF1(_L("TestTzname():"));
   	   	err = TestTzname();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
    else if(TestStepName() == KTestOpenWithExclusiveLock)
  			{
     		INFO_PRINTF1(_L("TestOpenWithExclusiveLock():"));
     		err1 = TestOpenWithExclusiveLock();
     		return err1;
     		}    
	__UHEAP_MARKEND;  	   	   	   	   	 
	return TestStepResult(); 
	}
	
// -----------------------------------------------------------------------------
// Tests to improve coverage
// -----------------------------------------------------------------------------
//                  
TVerdict CTestMisc::TestRandom()
    {    
    SetTestStepResult(EPass);
    
    unsigned seed=1;
    char state_array[64];
    size_t size=64;
    
    char* init_state=initstate(seed, &state_array[0], size); 
    if(init_state==NULL)
    	{
    	INFO_PRINTF1(_L("Error in initstate()..."));
		SetTestStepResult(EFail);    	    	
    	}    
    else
    	{
    	random();
    	unsigned change_seed=2;
    	srandom(change_seed);
    	char* set_state=setstate(&state_array[0]);
    	if(set_state==NULL)
    		{
    		INFO_PRINTF1(_L("Error in setstate()..."));
			SetTestStepResult(EFail);    	    	
			return TestStepResult();
    		}        	
    	char state[4];
    	char* init_state=initstate(seed, &state[0], 4); 
    	if(init_state!=NULL)
    		{
    		INFO_PRINTF1(_L("Error in initstate()..."));
			SetTestStepResult(EFail);    	    	
    		}       		
    	}
    	
	return TestStepResult();    	   
    }  
    
    
    
TVerdict CTestMisc::TestNanoSleep()
	{
    SetTestStepResult(EPass);
    
	struct timespec rqtp;
	struct timespec rmtp;	
	rqtp.tv_sec=1;
	rqtp.tv_nsec=10;


	int ret=nanosleep(&rqtp, &rmtp);
    if(ret!=0)
    	{
    	INFO_PRINTF1(_L("Error in nanosleep()..."));
		SetTestStepResult(EFail);    	    	
    	}     	
    	
	return TestStepResult();  	
	}

int getFirstMissingDrive(char& aChar)	
	{
	RFs fs;
	fs.Connect();

	//obtain list of available drives
	char ch = 'A';	
	TDriveList driveList8;
	fs.DriveList(driveList8);
	fs.Close();
	
	for (TInt i = 0; i < KMaxDrives; ++i)
		{
		if (driveList8[i]==0)
			{
			aChar=ch;
			return 0;
			}
		++ch;	
		}	
	return -1;		
	}

TVerdict CTestMisc::TestFileOper()
	{
    SetTestStepResult(EPass);	
	int index=0;
	FILE* fptr=NULL;
	
	char file[12+1]; //rqd max is 12
	char drive;
	if(getFirstMissingDrive(drive)==0)
		{
		sprintf(file, "%c:\\file.txt", drive);	
		fptr=fopen(file, "a");
		if(fptr!=NULL || errno!=ENOENT)
			{
			INFO_PRINTF2(_L("fopen() error- Error no. %d..."), errno);			
			SetTestStepResult(EFail);    	    
			return TestStepResult();	
			}
		}
		
	fptr=fopen("c:\\file.txt", "a");
    if(fptr==NULL)
    	{
    	INFO_PRINTF1(_L("Error in fopen()..."));
		SetTestStepResult(EFail);    	    
		return TestStepResult();	
    	}     

	while(index<5)
		{
		fputs("Testing stdio\n",fptr);
		putc(65, fptr);
		index++;		
		}
			
	char setbuf[15];
	setbuffer(fptr, setbuf, 15);
	fputs("writing into \n", fptr);	    
		    
    while(!feof(fptr) && !ferror(fptr))
    	{
		getc(fptr);	
    	}
    
    if( !(fileno(fptr)>0))		    	
    	{
		INFO_PRINTF1(_L("Error in fileno()..."));
		SetTestStepResult(EFail);  		   	
    	}
    
    int fid=open("CON:", O_RDWR);
	if(fid==-1)    
		{
		INFO_PRINTF1(_L("Error in open()..."));
		SetTestStepResult(EFail);  		
		fclose(fptr);
		return TestStepResult();			    			
		}
	
	char bufCon[] = "Writing to console\n"	;
	int written=write(fid, bufCon, 10);
	if(written!=10)		
		{
		INFO_PRINTF1(_L(" write failed()..."));
		SetTestStepResult(EFail);  							
		}	
		
	int fidcom=open("COM1:", O_WRONLY );
	if(fidcom==-1)    
		{
		INFO_PRINTF1(_L("Error in open()..."));
		SetTestStepResult(EFail);  		
		close(fid);
		fclose(fptr);
		return TestStepResult();			    			
		}

	char bufCom[] = "Writing to com1\n"	;
	written=write(fid, bufCom, 10);
	if(written!=10)		
		{
		INFO_PRINTF1(_L(" write failed()..."));
		SetTestStepResult(EFail);  		
		close(fidcom);
		close(fid);		
		fclose(fptr);
		return TestStepResult();			    					
		}	

	close(fidcom);
	close(fid);		
	fclose(fptr);			
	return TestStepResult();  	    		
	}
	
		    
TVerdict CTestMisc::TestRealloc()
	{
    SetTestStepResult(EPass);
    
    int* iptr=(int*)malloc(5*sizeof(int));
    if(iptr==NULL)
    	{
    	INFO_PRINTF1(_L("Error in malloc()..."));
		SetTestStepResult(EFail);    	    	
		return TestStepResult();
    	}         	
    	
    iptr=(int*)reallocf(iptr, 10*sizeof(int));
    if(iptr==NULL)
    	{
    	INFO_PRINTF1(_L("Error in reallocf()..."));
		SetTestStepResult(EFail);    	    	
    	}
    free(iptr);
   	return TestStepResult();  		
	}


TVerdict CTestMisc::TestSockAtMark()
	{
	SetTestStepResult(EPass);	
	
   	return TestStepResult();  		
	}

TVerdict CTestMisc::TestSysConf()
	{	
	long result;

  	INFO_PRINTF1(_L("Examining OPEN_MAX limit"));
  	
  	result = sysconf(_SC_OPEN_MAX);
  	if (result == -1) 
  		{
  		if (errno == 0)
  			{
  			INFO_PRINTF1(_L("OPEN_MAX is not supported."));
  			}
  		else
  			{
  			INFO_PRINTF1(_L("sysconf() error"));
  			}
  		SetTestStepResult(EFail);
  		}
    else 
    	{
    	INFO_PRINTF2(_L("OPEN_MAX is %ld\n"), result);
    	SetTestStepResult(EPass);
    	}
  	return TestStepResult();  	
	}

TVerdict CTestMisc::TestScandir()
	{
	struct dirent **namelist;
    int n;
    n = mkdir("c:\\private\\1028308C\\", S_IWUSR);
    if (n < 0)
    	{
    	INFO_PRINTF1(_L("Error in mkdir()"));
    	SetTestStepResult(EFail);
    	}
    n = mkdir("c:\\private\\1028308C\\test\\", S_IWUSR);
    if (n < 0)
    	{
    	INFO_PRINTF1(_L("Error in mkdir()"));
    	SetTestStepResult(EFail);
    	}
	FILE *fp = fopen("c:\\private\\1028308C\\test\\tmp.txt","w");
	if (fp == NULL)
    	{
    	INFO_PRINTF1(_L("Error in fopen()"));
    	SetTestStepResult(EFail);
    	}
    n = scandir("c:\\private\\1028308C\\test", &namelist, 0, NULL);
    if (n < 0)
    	{
    	INFO_PRINTF1(_L("Error in scandir()"));
    	SetTestStepResult(EFail);
    	}
    else 
    	{
    	INFO_PRINTF1(_L("Directory list"));
        while(n--) 
        	{
        	TBuf<MAX_LEN> buf;
        	TInt len = strlen(namelist[n]->d_name);
        	for (TInt i = 0 ; i<len ; i++)
        		{
        		buf.Append(namelist[n]->d_name[i]);
        		}
            INFO_PRINTF2(_L("%S"), &buf);
            free(namelist[n]);
            }
        free(namelist);
        SetTestStepResult(EPass);
       	}
    fclose(fp);
    n = unlink("c:\\private\\1028308C\\test\\tmp.txt");
    if (n < 0)
    	{
    	INFO_PRINTF1(_L("Error in unlink()"));
    	SetTestStepResult(EFail);
    	}
    n = rmdir("c:\\private\\1028308C\\test");
    if (n < 0)
    	{
    	INFO_PRINTF1(_L("Error in rmdir()"));
    	SetTestStepResult(EFail);
    	}
	return TestStepResult();  
	}

TVerdict CTestMisc::TestDiropr()
	{
	char* directory = "c:\\";
	struct dirent *dp;
		
	DIR* ret = opendir(directory);
	if (ret == NULL)
		{
		INFO_PRINTF1(_L("Can't open directory"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	while((dp = readdir(ret))!= NULL)
			{
			TBuf<MAX_LEN> buf;
        	TInt len = strlen(dp->d_name);
        	for (TInt i = 0 ; i<len ; i++)
        		{
        		buf.Append(dp->d_name[i]);
        		}
            INFO_PRINTF2(_L("%S"), &buf);
			}
	closedir(ret);
	ret = opendir(directory);
	long int pos = 2 , rdpos;
	//set the position of the directory stream 
	seekdir(ret , pos);
	rdpos = telldir(ret);
	if (pos == rdpos)
		{
		INFO_PRINTF1(_L("seekdir() and telldir() success"));
		}
	INFO_PRINTF2(_L("Dir or file at position %d"),pos+1);
	dp = readdir(ret);
	TBuf<MAX_LEN> buf;
    TInt len = strlen(dp->d_name);
    for (TInt i = 0 ; i<len ; i++)
     	{
     	buf.Append(dp->d_name[i]);
    	}
   	INFO_PRINTF2(_L("%S"), &buf);
   	//set position to beginning of directory stream
   	rewinddir(ret);
    pos = telldir(ret);
    if (pos == 0)
    	{
    	INFO_PRINTF1(_L("rewinddir() success!"));
    	}
    else
    	{
    	INFO_PRINTF1(_L("rewinddir() error!"));
    	}
    closedir(ret);
	return TestStepResult(); 
	}

TVerdict CTestMisc::TestReaddir()
	{
	char* directory = "c:\\";
	struct dirent *dp;
	
	DIR* ret = opendir(directory);
	if (ret == NULL)
		{
		INFO_PRINTF1(_L("Can't open directory"));
		SetTestStepResult(EFail);
		return TestStepResult(); 
		}
	else
		{
		while((dp = readdir(ret))!= NULL)
			{
			TBuf<MAX_LEN> buf;
        	TInt len = strlen(dp->d_name);
        	for (TInt i = 0 ; i<len ; i++)
        		{
        		buf.Append(dp->d_name[i]);
        		}
        	INFO_PRINTF2(_L("%S"), &buf);
			}
		if (dp == NULL)
			{
			if (errno == 0)
				{
				INFO_PRINTF1(_L("End of directory"));
				closedir(ret);
				SetTestStepResult(EPass);
				}
			else
				{
				INFO_PRINTF1(_L("readdir() error!"));
				SetTestStepResult(EFail);
				}
			}
				
		}
	return TestStepResult(); 	
	}
	
TVerdict CTestMisc::TestWOpendir()
	{
	wchar_t* directory = L"c:\\";
	struct wdirent *dp;
	
	WDIR* wret = wopendir(directory);
	if (wret == NULL)
		{
		INFO_PRINTF1(_L("Can't open directory"));
		SetTestStepResult(EFail);
		return TestStepResult(); 
		}
	INFO_PRINTF1(_L("wopendir success!"));
	//Now read directory and get first entry
	dp = wreaddir(wret);
	if (dp == NULL)
		{
		if (errno == 0)
			{
			INFO_PRINTF1(_L("End of directory"));
			wclosedir(wret);
			SetTestStepResult(EPass);
			}
		else
			{
			INFO_PRINTF1(_L("wreaddir() error!"));
			SetTestStepResult(EFail);
			}
		}
	wclosedir(wret);
	return TestStepResult(); 	
	}
	
TVerdict CTestMisc::TestWDiropr()
	{
	wchar_t* directory = L"c:\\";

	WDIR* wret = wopendir(directory);
	if (wret == NULL)
		{
		INFO_PRINTF1(_L("Can't open directory"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	long int pos = 2 , rdpos;
	//set the position of the directory stream 
	wseekdir(wret , pos);
	rdpos = wtelldir(wret);
	if (pos == rdpos)
		{
		INFO_PRINTF1(_L("wseekdir() and wtelldir() success"));
		}
	
   	//set position to beginning of directory stream
   	wrewinddir(wret);
    pos = wtelldir(wret);
    if (pos == 0)
    	{
    	INFO_PRINTF1(_L("wrewinddir() success!"));
    	}
    else
    	{
    	INFO_PRINTF1(_L("wrewinddir() error!"));
    	}
    wclosedir(wret);
	return TestStepResult(); 
	}

TVerdict CTestMisc::TestFMScalls()
	{
	SetTestStepResult(EPass);

	wrmdir(L"c:\\wfmstest1");
	wrmdir(L"c:\\wfmstest2");	
	
	wchar_t *dir1=L"c:\\wfmstest1";
	TInt ret=wmkdir(dir1, S_IWUSR);
	if (ret!=0)
		{
		INFO_PRINTF1(_L("wmkdir() failed..."));
		INFO_PRINTF2(_L("The errno value is %d\n"),errno);
		SetTestStepResult(EFail);
		return TestStepResult();		
		}	

	wchar_t *dir2=L"c:\\wfmstest2";
	ret=wmkdir(dir2, S_IWUSR);
	if (ret!=0)
		{
		INFO_PRINTF1(_L("wmkdir() failed..."));
		INFO_PRINTF2(_L("The errno value is %d\n"),errno);
		SetTestStepResult(EFail);
		wrmdir(L"c:\\wfmstest1");
		return TestStepResult();		
		}	

	ret=wchdir(dir1);			
	if (ret!=0)
		{
		INFO_PRINTF1(_L("wchdir() failed..."));
		INFO_PRINTF2(_L("The errno value is %d\n"),errno);
		SetTestStepResult(EFail);
		wrmdir(L"c:\\wfmstest1");
		wrmdir(L"c:\\wfmstest2");
		return TestStepResult();		
		}	
		
	wchar_t path[50];
	if(wgetcwd(path, 50)==NULL)
		{
		INFO_PRINTF1(_L("wrealpath() failed..."));
		INFO_PRINTF2(_L("The errno value is %d\n"),errno);
		SetTestStepResult(EFail);
		wrmdir(L"c:\\wfmstest1");
		wrmdir(L"c:\\wfmstest2");
		return TestStepResult();			
		}
	
	wchar_t *file1=L"c:\\wfmstest1\\temp.txt";
	ret=wopen(file1,O_CREAT, S_IWUSR);
	if (ret==-1)
		{
		INFO_PRINTF1(_L("wopen() failed..."));
		INFO_PRINTF2(_L("The errno value is %d\n"),errno);
		SetTestStepResult(EFail);
		wrmdir(L"c:\\wfmstest1");
		wrmdir(L"c:\\wfmstest2");
		return TestStepResult();		
		}	
	
	close(ret);
	
	wchar_t *file=L"c:\\wfmstest1\\temp1.txt";
	ret=wrename(file1, file);
	if (ret!=0)
		{
		INFO_PRINTF1(_L("wrename() failed..."));
		SetTestStepResult(EFail);
		return TestStepResult();		
		}
	
	ret=wchmod(file,S_IRUSR);
	if (ret!=0)
		{
		INFO_PRINTF1(_L("wchmod() failed..."));
		SetTestStepResult(EFail);
		return TestStepResult();		
		}
		
	struct stat statbuf ;
   	ret = wstat(file , &statbuf) ;
	if (ret!=0)
		{
		INFO_PRINTF1(_L("wstat() failed..."));
		SetTestStepResult(EFail);
		return TestStepResult();		
		}   
			
	wchar_t realpath[50+1];
	if(wrealpath(L"\\wfmstest1\\temp1.txt", realpath) ==NULL)
		{
		INFO_PRINTF1(_L("wrealpath() failed..."));
		SetTestStepResult(EFail);
		return TestStepResult();		
		}   	   						
	
	ret=wunlink(file);	
	if (ret!=0)
		{
		INFO_PRINTF1(_L("wunlink() failed..."));
		SetTestStepResult(EFail);
		return TestStepResult();		
		}   
		
	ret=wrmdir(dir1);
	if (ret!=0)
		{
		INFO_PRINTF1(_L("wrmdir() failed..."));
		SetTestStepResult(EFail);
		return TestStepResult();		
		}   
	
	ret=wrmdir(dir2);	
	if (ret!=0)
		{
		INFO_PRINTF1(_L("wrmdir() failed..."));
		SetTestStepResult(EFail);
		return TestStepResult();		
		}   

	if (!wsystem(NULL) || (wsystem(L"z:\\sys\\bin\\TPopenWriteChildPopen.exe") == -1))
		{
		INFO_PRINTF1(_L("wsystem() failed..."));
		SetTestStepResult(EFail);
		return TestStepResult();		
		}   

	return TestStepResult();	
	}

TVerdict CTestMisc::TestGetDirEntries()	
	{
	int fd;
	char buf[1024];
	long basep;
	
	fd = open("c:\\", O_RDONLY);
	if (fd == -1)
		{
		INFO_PRINTF1(_L("open failed..."));
		SetTestStepResult(EFail);
		return TestStepResult();
		}
		
	int ret = getdirentries(fd, buf, 1024, &basep);
	if (ret == -1)
		{
		INFO_PRINTF2(_L("getdirentries() error- Error no. %d..."), errno);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else if (ret == 0)
		{
		INFO_PRINTF1(_L("End of directory reached!"));
		SetTestStepResult(EPass);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("getdirentries() read %d bytes"), ret);
		SetTestStepResult(EPass);
		return TestStepResult();
		}
	}
	
TVerdict CTestMisc::TestAlphasort()	
	{
	SetTestStepResult(EPass);
	char* directory = "c:\\";
	struct dirent *dp1, *dp2;
	DIR* ret = opendir(directory);
	if (ret == NULL)
		{
		INFO_PRINTF1(_L("Can't open directory"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("Read first entry"));
	dp1 = readdir(ret);
	TBuf<MAX_LEN> buf;
	TInt len = strlen(dp1->d_name);
	for (TInt i = 0 ; i<len ; i++)
		{
		buf.Append(dp1->d_name[i]);
		}
	INFO_PRINTF2(_L("First File Entry:%S"), &buf);
	
	INFO_PRINTF1(_L("Read second entry"));
	char* directory1 = "c:\\logs";
	closedir(ret);
	ret = opendir(directory1);
	if (ret == NULL)
		{
		INFO_PRINTF1(_L("Can't open directory"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	dp2 = readdir(ret);
	TBuf<MAX_LEN> buf1;
	len = strlen(dp2->d_name);
	for (TInt i = 0 ; i<len ; i++)
		{
		buf1.Append(dp2->d_name[i]);
		}
	INFO_PRINTF2(_L("Second File Entry:%S"), &buf1);

	int n = alphasort(&dp1, &dp2);
	if (n < 0)
		{
		INFO_PRINTF1(_L("first entry less than second"));	
		}
	else if(n == 0) 
		{
		INFO_PRINTF1(_L("Both entries are same"));
		}
	else if(n > 0) 
		{
		INFO_PRINTF1(_L("first entry greater than second enttry"));
		}
	closedir(ret);
	
		
	return TestStepResult();
	}	
	

TVerdict CTestMisc::TestStdio()
	{
	SetTestStepResult(EPass);		

	int ret;
	char read_buf[10];

	wprintf(L"Using wprintf\n");
	putwchar(L'Y');

	wprintf(L"Read integer\n");		
    wscanf(L"%d",&ret);
    if(ret!=5) 
    	{
    	INFO_PRINTF1(_L("wscanf failed"));
		SetTestStepResult(EFail);    	
    	}
    	
    getwchar();			
	gets(&read_buf[0]);
	read_buf[5]='\0';
	
	if(strcmp("stdio", read_buf))
		{
		INFO_PRINTF1(_L("gets failed"));
		SetTestStepResult(EFail);  		
		}						
	if(puts(read_buf)<0)					
		{
		INFO_PRINTF1(_L("puts failed"));		
		SetTestStepResult(EFail);  		
		}
		
	int character;	
	if( (character=getchar())!=65)	
		{
		INFO_PRINTF1(_L("getchar failed"));				
		SetTestStepResult(EFail);  		
		}		
	if( !(putchar(character)>0) )
		{
		INFO_PRINTF1(_L("putchar failed"));						
		SetTestStepResult(EFail);  		
		}
		
	printf("Read integer again\n");
	scanf("%d",&ret);		
    if(ret!=5) 
    	{
    	INFO_PRINTF1(_L("scanf failed"));
		SetTestStepResult(EFail);      	
    	}

   	return TestStepResult();  			
	}
	
	
TVerdict CTestMisc::testposix()
	{	
	SetTestStepResult(EFail);
	
	short flag;
	pid_t pgroup;
	int policy;
	sigset_t sigmaskset;
	sigset_t sigdefaultset;
	sigset_t sigmaskget;
	sigset_t sigdefaultget;	
	struct sched_param schedset;
	struct sched_param schedget;
	

	//init
	posix_spawnattr_t attrp;
		
	int err=posix_spawnattr_init(&attrp);
	if(err!=0)
		{
		INFO_PRINTF1(_L("posix_spawnattr_init failed..."));
		return TestStepResult();
		}
	
			
	//set	
	err=posix_spawnattr_setflags(&attrp, (POSIX_SPAWN_SETSIGMASK|POSIX_SPAWN_SETPGROUP|POSIX_SPAWN_SETSCHEDPARAM|
								POSIX_SPAWN_SETSCHEDULER|POSIX_SPAWN_SETSIGDEF|POSIX_SPAWN_SETSIGMASK) );								

	if(err!=0)
		{
		INFO_PRINTF1(_L("posix_spawnattr_init failed..."));
		return TestStepResult();		
		}
								
	err=posix_spawnattr_setsigmask(&attrp, &sigmaskset);
	if(err!=0)
		{
		INFO_PRINTF1(_L("posix_spawnattr_init failed..."));
		return TestStepResult();
		}

	if(posix_spawnattr_getsigmask(&attrp, &sigmaskget) )//|| (*sigmaskget!=1))
		{
	  	INFO_PRINTF1(_L("posix_spawnattr_getsigdefault failed..."));
		return TestStepResult();
		}		
	if(posix_spawnattr_getflags(&attrp, &flag) || (flag!=(POSIX_SPAWN_SETSIGMASK|POSIX_SPAWN_SETPGROUP|POSIX_SPAWN_SETSCHEDPARAM|
								POSIX_SPAWN_SETSCHEDULER|POSIX_SPAWN_SETSIGDEF|POSIX_SPAWN_SETSIGMASK)))
		{
    	INFO_PRINTF1(_L("posix_spawnattr_getflags failed..."));
		return TestStepResult();
		}
	
	err=posix_spawnattr_setschedpolicy(&attrp, 1);
	if(err!=0)
		{
		INFO_PRINTF1(_L("posix_spawnattr_setschedpolicy failed..."));
		return TestStepResult();
		}
	
	if(posix_spawnattr_getschedpolicy(&attrp, &policy) || (policy!=1))
		{
    	INFO_PRINTF1(_L("posix_spawnattr_getschedpolicy failed..."));
		return TestStepResult();
		}		

	err=posix_spawnattr_setsigdefault(&attrp, &sigdefaultset);
	if(err!=0)
		{
		INFO_PRINTF1(_L("posix_spawnattr_setsigdefault failed..."));
		return TestStepResult();
		}
	
	if(posix_spawnattr_getsigdefault(&attrp, &sigdefaultget))// || (*sigdefaultget!=2))
		{
    	INFO_PRINTF1(_L("posix_spawnattr_getsigdefault failed..."));
		return TestStepResult();
		}			

	err=posix_spawnattr_setpgroup(&attrp, (pid_t)2);
	if(err!=0)
		{
		INFO_PRINTF1(_L("posix_spawnattr_setpgroup failed..."));
		return TestStepResult();
		}
	
	if(posix_spawnattr_getpgroup(&attrp, &pgroup) || (pgroup!=(pid_t)2))
		{
    	INFO_PRINTF1(_L("posix_spawnattr_getpgroup failed..."));
		return TestStepResult();
		}		

	err=posix_spawnattr_setschedparam(&attrp, &schedset);
	if(err!=0)
		{
		INFO_PRINTF1(_L("posix_spawnattr_setschedpolicy failed..."));
		return TestStepResult();
		}
	
	if(posix_spawnattr_getschedparam(&attrp, &schedget ))
		{
    	INFO_PRINTF1(_L("posix_spawnattr_getschedpolicy failed..."));
		return TestStepResult();
		}	

	posix_spawn_file_actions_t file_actions;

	err=posix_spawn_file_actions_init(&file_actions);

	
	if(err!=0)
		{
		INFO_PRINTF1(_L("posix_spawn_file_actions_init failed..."));
		return TestStepResult();
		}
	

	mkdir("C:\\testposix", S_IWUSR);	
	int fd1= open("c:\\testposix\\posixspawnfile1.txt", O_RDONLY|O_CREAT);
	int fd2= open("c:\\testposix\\posixspawnfile2.txt", O_RDWR|O_CREAT);		
	FILE* fp = NULL ;
	pid_t pid;
	char **argv=(char**)malloc(3*sizeof(char*));
	err=posix_spawn_file_actions_addopen(&file_actions,3, "c:\\testposix\\posixspawnfile3.txt", O_RDWR|O_CREAT, 0666);
	if(err!=0)
		{
		INFO_PRINTF1(_L("posix_spawn_file_actions_addopen failed..."));
		goto exit;		
		}

	err=posix_spawn_file_actions_adddup2(&file_actions, fd2, 1);
	if(err!=0)
		{
		INFO_PRINTF1(_L("posix_spawn_file_actions_adddup2 failed..."));
		goto exit;		
		}

	
	
	
	
	argv[0]=(char*)malloc(34*sizeof(char));
	argv[1]=(char*)malloc(4*sizeof(char));	
	argv[2]= 0;
	
	strcpy(argv[0], "z:\\sys\\bin\\posix_spawn.exe");
	sprintf(argv[1], "%d", fd1);


	if(posix_spawn(&pid, "z:\\sys\\bin\\posix_spawn.exe", &file_actions, &attrp, argv, (char**)NULL))
		{
		INFO_PRINTF1(_L("posix_spawn failed..."));
		goto exit;	
		}
	
	int status;
	if (waitpid(pid, &status, WUNTRACED)!=pid)
		{
		INFO_PRINTF1(_L("waitpid failed..."));	
		goto exit;	
		}
	//Get FILE pointer from open file pointer fd2 and read the string which is written by child process.
	fp = fdopen(fd2, "r");
	if (fp == NULL)
	{
		ERR_PRINTF1(_L("fdopen after child return failed."));
		goto exit;
	}
	rewind(fp);
	char str[5];
	if(fgets(str,5,fp) == NULL || strcmp(str,"pass") != 0)
		{
		ERR_PRINTF1(_L("Failed write to a file descriptor passed to the child process."));
		}
	else
		{
		SetTestStepResult(EPass);
		}
		
exit:
	close(fd2);
	close(fd1);
	unlink("c:\\testposix\\posixspawnfile3.txt");
	unlink("c:\\testposix\\posixspawnfile2.txt");
	unlink("c:\\testposix\\posixspawnfile1.txt");
	rmdir("C:\\testposix");
	free((void*)argv[0]);
	free((void*)argv[1]);
	free((void*)argv);
	posix_spawn_file_actions_destroy(&file_actions);
	return TestStepResult();				
	}


TVerdict CTestMisc::TestPosixSpawn()
{	
	SetTestStepResult(EPass);
	CActiveScheduler *pActiveScheduler=new CActiveScheduler();	
	CActiveScheduler::Install(pActiveScheduler);	
		
	if(testposix()==EFail)
		{
		INFO_PRINTF1(_L("posix_spawn test failed"));
		delete pActiveScheduler;
		SetTestStepResult(EFail);		
		}	  
	
	CActiveScheduler::Install(NULL);	
	delete pActiveScheduler;
	
	return TestStepResult();
}

TInt CTestMisc::TestPosixSpawn1()
{
		    
	int RetVal = 0;
	int ret = 0;
	pid_t Processpid;

	RetVal= posix_spawn(&Processpid,NULL,NULL,NULL,NULL,NULL);
	if(RetVal == 0)
	   {
	   		
	   	    (void)waitpid(Processpid,NULL,0);
	   }	

	if(ECHILD == RetVal)
		{
			ret = KErrNone;	
		}
	else
		{
			ret = KErrGeneral;	
		}
	return ret;
}

TVerdict CTestMisc::TestSystem0()
	{
	SetTestStepResult(EPass);
	
	char *command = "z:\\sys\\bin\\tstdlib_helloworld.exe";
	int i = system(command);
	
	if(i!=1)
		{
		SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}
	
TVerdict CTestMisc::TestSystem1()
	{
	SetTestStepResult(EPass);
	
	char *command = "z:\\sys\\bin\\tstdlib_helloworld.exe";
	char comname[64] = "   ";
	
	strcat(comname,command);
	int i = system(comname);
	
	if(i!=1)
		{
		SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}

TVerdict CTestMisc::TestSystem2()
	{
	SetTestStepResult(EPass);
	
	int i = system("z:\\sys\\bin\\tstdlib_testsystem.exe a b c");
	
	if(i!=4)
		{
		SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}

TVerdict CTestMisc::TestAbort1()
	{
	SetTestStepResult(EPass);
	
	int i = system("z:\\sys\\bin\\tstdlib_testabort.exe");
	
	if(i!=1)
		{
		SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}

TVerdict CTestMisc::TestExit1()
	{
	SetTestStepResult(EPass);
	
	#if defined(__WINSCW__) || defined(__WINS__)
		INFO_PRINTF1(_L("atexit() not supported on the emulator!"));

	#else
	int i = system("z:\\sys\\bin\\tstdlib_testexit.exe");
	
	char buf1[64];
	char buf2[64];

	FILE *fp = fopen("c:\\some.txt", "r");
	if(fp)
	{
		fscanf(fp, "%s", buf1);
		fgetc(fp); //skip the space
		fscanf(fp, "%s", buf2);
	}
	
	TBuf8<50> tempString1((TUint8 *)&buf1);
	TBuf8<50> tempString2((TUint8 *)&buf2);
	TBuf<50> displayString1;
	TBuf<50> displayString2;
	
	displayString1.Copy(tempString1);
	displayString2.Copy(tempString2);
	INFO_PRINTF3(_L("Executing exitfunc3.: %S %S\n"), &displayString1,&displayString2);

	strcat(buf1, " ");
	strcat(buf1, buf2);
	if(strcmp(buf1, "Executing exitfunc3.") != 0)
		{
		SetTestStepResult(EFail);
		}
	
	if(fp)
		{
		fgetc(fp); //skip the newline

		fscanf(fp, "%s", buf1);
		fgetc(fp); //skip the space
		fscanf(fp, "%s", buf2);
		}

	tempString1 = (TUint8 *)buf1;
	tempString2 = (TUint8 *)buf2;
	displayString1.Copy(tempString1);
	displayString2.Copy(tempString2);
	INFO_PRINTF3(_L("Executing exitfunc2.: %S %S\n"), &displayString1,&displayString2);
	
	strcat(buf1, " ");
	strcat(buf1, buf2);
	if(strcmp(buf1, "Executing exitfunc2.") != 0)
		SetTestStepResult(EFail);
	
	if(fp)
		{
		fgetc(fp); //skip the newline

		fscanf(fp, "%s", buf1);
		fgetc(fp); //skip the space
		fscanf(fp, "%s", buf2);
		}
	
	tempString1 = (TUint8 *)buf1;
	tempString2 = (TUint8 *)buf2;
	displayString1.Copy(tempString1);
	displayString2.Copy(tempString2);
	INFO_PRINTF3(_L("Executing exitfunc1.: %S %S\n"), &displayString1,&displayString2);

	strcat(buf1, " ");
	strcat(buf1, buf2);
	if(strcmp(buf1, "Executing exitfunc1.") != 0)
		{
		SetTestStepResult(EFail);
		}
	
	if(fp)
		{
		fclose(fp);	
		}
		
	if (i != 1)
		{
		INFO_PRINTF1(_L("The system() call has failed\n"));
		SetTestStepResult(EFail);
		}
		
	#endif
	
	return TestStepResult();
	}

TVerdict CTestMisc::TestExit2()
	{
	SetTestStepResult(EPass);
	
	int i = system("z:\\sys\\bin\\tstdlib_testexitec.exe");
	
	if(i!=3)
		{
		SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}

TVerdict CTestMisc::TestExitE1()
	{
	SetTestStepResult(EPass);
	
	int i = system("z:\\sys\\bin\\tstdlib_test_exitE.exe");
	
	if(i!=5)
		{
		SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}
	
TVerdict CTestMisc::TestExit3()
	{
	SetTestStepResult(EPass);
	
	int i = system("z:\\sys\\bin\\tstdlib_test_exit.exe");
	
	if(i!=6)
		{
		SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}
	
TVerdict CTestMisc::TestExit4()
	{
	SetTestStepResult(EPass);
	
	system("z:\\sys\\bin\\tstdlib_test_exit4.exe");
	system("z:\\sys\\bin\\tstdlib_test_exit4.exe");
	
	return TestStepResult();
	}	
	
TVerdict CTestMisc::TestExit5()
	{
	SetTestStepResult(EPass);
	
	system("z:\\sys\\bin\\tstdlib_test_exit5.exe");
	
	return TestStepResult();
	}	
	
TVerdict CTestMisc::TestExit6()
	{
	SetTestStepResult(EPass);
	
	if(system("z:\\sys\\bin\\tstdlib_test_exit6.exe") == -1)
		SetTestStepResult(EFail);
	
	return TestStepResult();
	}	
		
/* ********************************** */
/* Waitpid() test to check for status */
/* normal termination of the child    */
/* ********************************** */

TVerdict CTestMisc::TestWaitpid1()
	{
	SetTestStepResult(EPass);
	int status,ret1, ret;
	int fd[3];
	ret = popen3("z:\\sys\\bin\\helloworld_waitpid.exe", NULL, NULL, fd);
	INFO_PRINTF2(_L("popen3() returns %d\n"), ret);
	ret1 = waitpid(ret, &status, 0);
	INFO_PRINTF2(_L("waitpid() returns %d\n"), ret1);
	INFO_PRINTF2(_L("status %d\n"), status);	
	if (WIFEXITED(status))
		{
		INFO_PRINTF2(_L("exit status is %d\n"), WEXITSTATUS(status));
		if (WEXITSTATUS(status) == 4)
			{
			SetTestStepResult(EPass);
			}
		else
			{
			SetTestStepResult(EFail);
			}		
		}
	else
		{
		SetTestStepResult(EFail);		
		}	
	return TestStepResult();
	}
	
/* ************************************* */
/* Waitpid() test to check for status    */
/* when the child was killed	  		 */
/* ************************************* */

TVerdict CTestMisc::TestWaitpid2()
	{
	SetTestStepResult(EPass);
	int status,ret1, ret;
	int fd[3];
	RProcess pro;
	ret = popen3("z:\\sys\\bin\\helloworld_waitpid.exe", NULL, NULL, fd);
	INFO_PRINTF2(_L("popen3() returns %d\n"), ret);
	pro.Open(ret);
	pro.Kill(0);//exit reason is 0
	ret1 = waitpid(ret, &status, WNOHANG);
	INFO_PRINTF2(_L("waitpid() returns %d\n"), ret1);
	INFO_PRINTF2(_L("status %d\n"), status);	
	if (WIFEXITED(status))
		{
		INFO_PRINTF2(_L("exit status is %d\n"), WEXITSTATUS(status));
		if (WEXITSTATUS(status) == 0)
			{
			SetTestStepResult(EPass);
			}
		else
			{
			SetTestStepResult(EFail);
			}
		}
	else
		{
		SetTestStepResult(EFail);		
		}	
	return TestStepResult();
	}

/* ************************************* */
/* Waitpid() test to check for status    */
/* when the child panics	  		 */
/* ************************************* */

TVerdict CTestMisc::TestWaitpid3()
	{
	SetTestStepResult(EPass);
	int status,ret1, ret;
	int fd[3];
	RProcess pro;
	ret = popen3("z:\\sys\\bin\\helloworld_waitpid.exe", NULL, NULL, fd);
	INFO_PRINTF2(_L("popen3() returns %d\n"), ret);
	pro.Open(ret);
	pro.Panic(_L("Rprocess Panics here"), 3);//exit reason is 3
	ret1 = waitpid(ret, &status, WNOHANG);
	INFO_PRINTF2(_L("waitpid() returns %d\n"), ret1);
	INFO_PRINTF2(_L("status %d\n"), status);	
	if (!WIFEXITED(status))
		{
		INFO_PRINTF2(_L("exit status is %d\n"), WEXITSTATUS(status));
		if (WEXITSTATUS(status) == 3)
			{
			SetTestStepResult(EPass);
			}
		else
			{
			SetTestStepResult(EFail);
			}
		}
	else
		{
		SetTestStepResult(EFail);		
		}	
	return TestStepResult();
	}


						// SEM_UNDO

//If sem_op is a positive integer and the calling process has alter permission, the value of sem_op shall be added to semval and, if (sem_flg &SEM_UNDO) is non-zero, the value of sem_op shall be subtracted from the calling process' semadj value for the specified semaphore.

TInt CTestMisc::semoptest16()
	{
	int msq_id,ret=KErrNone;
	int ret1,i,x,semkey,get;
	union semun tmpo,arg;
	unsigned short tmpi[2];
	int fds[3];
	char *path;
	
	#if defined (__EPOC32__)
		path = "z:\\tstdapis\\tmisc.ini";
	#else
		path = "c:\\tstdapis\\tmisc.ini";
	#endif
	
	if ((semkey = ftok(path, 'r')) == (key_t) -1) 
		{
		_LIT(KFunc, "ftok failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    	
	if ((msq_id = semget(semkey,2, IPC_W | IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
	
	arg.val = 3;
	
	get = semctl(msq_id, 1, SETVAL,arg);
    if (get == -1) 
    	{
		_LIT(KFunc, "semaphore SETVAL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    x = popen3("z:\\sys\\bin\\Tsemundo1.exe",NULL,NULL,fds);
    if(x==-1)
    	{
		_LIT(KFunc, "popen3 failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
		}
	sleep(5);

    _LIT(KStmt,"After Semop operations ");
    INFO_PRINTF1(KStmt);
    tmpo.array = &tmpi[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    for(i=0;i<2;i++)
    	{
		_LIT(KFunc, "semaphore GETVAL for the %d semaphore is %d \n");
      	INFO_PRINTF3(KFunc, i,tmpi[i]);
    	}
    
    if( tmpi[0] != 0 && tmpi[1] != 2)
    	{
		_LIT(KFunc, "semaphore semop failed and didnt modified the semval\n");
      	INFO_PRINTF1(KFunc);
       	ret = KErrGeneral;
    	}
   	if (semctl(msq_id, 1, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
	return ret;
	}


TInt CTestMisc::semoptest17()
	{
	int msq_id,ret=KErrNone;
	int ret1,i,x,semkey,get;
	union semun tmpo,arg,arg1;
	unsigned short tmpi[2];
	int fds[3];
	char *path;
	
	#if defined (__EPOC32__)
		path = "z:\\tstdapis\\tmisc.ini";
	#else
		path = "c:\\tstdapis\\tmisc.ini";
	#endif
	
	if ((semkey = ftok(path, 'r')) == (key_t) -1) 
		{
		_LIT(KFunc, "ftok failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    	
	if ((msq_id = semget(semkey,2, IPC_W | IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
	
	arg.val = 5;
	
	get = semctl(msq_id, 0, SETVAL,arg);
    if (get == -1) 
    	{
		_LIT(KFunc, "semaphore SETVAL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    arg1.val = 10;
	
	get = semctl(msq_id, 1, SETVAL,arg1);
    if (get == -1) 
    	{
		_LIT(KFunc, "semaphore SETVAL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    x = popen3("z:\\sys\\bin\\Tsemundo2.exe",NULL,NULL,fds);
    if(x==-1)
    	{
		_LIT(KFunc, "popen3 failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
		}
	sleep(5);

    _LIT(KStmt,"After Semop operations ");
    INFO_PRINTF1(KStmt);
    tmpo.array = &tmpi[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    for(i=0;i<2;i++)
    	{
		_LIT(KFunc, "semaphore GETVAL for the %d semaphore is %d \n");
      	INFO_PRINTF3(KFunc, i,tmpi[i]);
    	}
    
    if( tmpi[0] != 20 && tmpi[1] != 30)
    	{
		_LIT(KFunc, "semaphore semop failed and didnt modified the semval\n");
      	INFO_PRINTF1(KFunc);
       	ret = KErrGeneral;
    	}
   	if (semctl(msq_id, 1, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
	return ret;
	}


TInt CTestMisc::semoptest18()
	{
	int msq_id,ret=KErrNone;
	int ret1,i,x,semkey,get;
	union semun tmpo,arg1;
	unsigned short tmpi[2];
	int fds[3];
	char *path;
	
	#if defined (__EPOC32__)
		path = "z:\\tstdapis\\tmisc.ini";
	#else
		path = "c:\\tstdapis\\tmisc.ini";
	#endif
	
	if ((semkey = ftok(path, 'r')) == (key_t) -1) 
		{
		_LIT(KFunc, "ftok failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    	
	if ((msq_id = semget(semkey,2, IPC_W | IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}

    x = popen3("z:\\sys\\bin\\Tsemundo3.exe",NULL,NULL,fds);
    if(x==-1)
    	{
		_LIT(KFunc, "popen3 failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
		}
	
	arg1.val = 4;
	
	get = semctl(msq_id, 1, SETVAL,arg1);
    if (get == -1) 
    	{
		_LIT(KFunc, "semaphore SETVAL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    sleep(5);
    
    _LIT(KStmt,"After Semop operations ");
    INFO_PRINTF1(KStmt);
    tmpo.array = &tmpi[0];
	ret1 = semctl(msq_id, 1,GETALL,tmpo);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore GETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    for(i=0;i<2;i++)
    	{
		_LIT(KFunc, "semaphore GETVAL for the %d semaphore is %d \n");
      	INFO_PRINTF3(KFunc, i,tmpi[i]);
    	}
    
    if( tmpi[0] != 1 && tmpi[1] != 1)
    	{
		_LIT(KFunc, "semaphore semop failed and didnt modified the semval\n");
      	INFO_PRINTF1(KFunc);
       	ret = KErrGeneral;
    	}
   	if (semctl(msq_id, 1, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
	return ret;
	}

				// THREADS & PROCESSES

// *** Thread1 *** 

void* StartFn1(void* arg)
	{
	int semnops=3,x;    
    int msgq_id = (int)arg;
    struct sembuf lSemBuf[3];
    union semun arg1,arg2,arg3;
    
	// wait for -5,-10,-15
	
	lSemBuf[0].sem_num = 0;
    lSemBuf[0].sem_op  = -5;
    lSemBuf[0].sem_flg = 0;
	
	lSemBuf[1].sem_num = 1;
	lSemBuf[1].sem_op  = -10;
	lSemBuf[1].sem_flg = 0;
        
    lSemBuf[2].sem_num = 2;
	lSemBuf[2].sem_op  = -15;
	lSemBuf[2].sem_flg = 0;
	
	x = semop(msgq_id,&lSemBuf[0],semnops);
	if(x==-1)
		{
		exit(0);
		}
	
	// wait
	sleep(5);
	
	// setval 6,11,16
	arg1.val = 6;
	int get = semctl(msgq_id, 0, SETVAL,arg1);
	
	arg2.val = 11;
	get = semctl(msgq_id, 1, SETVAL,arg2);
	
	arg3.val = 16;
	get = semctl(msgq_id, 2, SETVAL,arg3);
	
	
	// exit
    return NULL;
	}
	

// *** Thread2 ***

void* StartFn2(void* arg)
	{
	int semnops=3,x;    
    int msgq_id = (int)arg;
    struct sembuf lSemBuf[3];
    union semun arg1,arg2,arg3;
    
	// wait for wait for 6,11,16
	
	lSemBuf[0].sem_num = 0;
    lSemBuf[0].sem_op  = -6;
    lSemBuf[0].sem_flg = 0;
	
	lSemBuf[1].sem_num = 1;
	lSemBuf[1].sem_op  = -11;
	lSemBuf[1].sem_flg = 0;
        
    lSemBuf[2].sem_num = 2;
	lSemBuf[2].sem_op  = -16;
	lSemBuf[2].sem_flg = 0;
	
	x = semop(msgq_id,&lSemBuf[0],semnops);
	if(x==-1)
		{
		exit(0);
		}
	
	// wait
	sleep(5);
	
	// setval 7,12,17
	arg1.val = 7;
	int get = semctl(msgq_id, 0, SETVAL,arg1);
	
	arg2.val = 12;
	get = semctl(msgq_id, 1, SETVAL,arg2);
	
	arg3.val = 17;
	get = semctl(msgq_id, 2, SETVAL,arg3);
	
	// exit
    return NULL;
	}

TInt CTestMisc::semoptest19()
	{
	int msq_id,ret=KErrNone,semnops=2;
	int x,semkey;
	union semun arg1,arg2,arg3;
	int fds[3];
	struct sembuf lSemBuf[3];
	pthread_t testThread1;
	pthread_t testThread2;
	
	char *path;
	
	#if defined (__EPOC32__)
		path = "z:\\tstdapis\\tmisc.ini";
	#else
		path = "c:\\tstdapis\\tmisc.ini";
	#endif
	
	if ((semkey = ftok(path, 'r')) == (key_t) -1) 
		{
		_LIT(KFunc, "ftok failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
    	
	if ((msq_id = semget(semkey,3, IPC_W | IPC_R|IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}

	// setval to 5,10,15
	
	arg1.val = 5;
	int get = semctl(msq_id, 0, SETVAL,arg1);
	
	arg2.val = 10;
	get = semctl(msq_id, 1, SETVAL,arg2);
	
	arg3.val = 15;
	get = semctl(msq_id, 2, SETVAL,arg3);
	
	
	
	// create two threads
	
	pthread_create(&testThread1,NULL,StartFn1,(TAny*)msq_id);
	pthread_create(&testThread2,NULL,StartFn2,(TAny*)msq_id);
	
	// create a process 
	
	x = popen3("z:\\sys\\bin\\Tsemundo4.exe",NULL,NULL,fds);
    if(x==-1)
    	{
		_LIT(KFunc, "popen3 failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
		}
	
	
	// wait for -10,-15,-20
	lSemBuf[0].sem_num = 0;
    lSemBuf[0].sem_op  = -10;
    lSemBuf[0].sem_flg = 0;
	
	lSemBuf[1].sem_num = 1;
	lSemBuf[1].sem_op  = -15;
	lSemBuf[1].sem_flg = 0;
        
    lSemBuf[2].sem_num = 2;
	lSemBuf[2].sem_op  = -20;
	lSemBuf[2].sem_flg = 0;
	x = semop(msq_id,&lSemBuf[0],semnops);
	if(x==-1)
		{
		_LIT(KFunc, "semop failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
		}
	
	// exit
	if (semctl(msq_id, 2, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
	return ret;
	}

void* thread1(void* arg)
	{
	int semid = *(int*)(arg);
	int ret=0;
	
	struct sembuf LockSem[2];
	struct sembuf UnLockSem[2];
	
	LockSem[0].sem_num = 0;
	LockSem[0].sem_op  = 0;
	LockSem[0].sem_flg = SEM_UNDO;
	
	LockSem[1].sem_num = 1;
	LockSem[1].sem_op  = -1;
	LockSem[1].sem_flg = SEM_UNDO;
	
	
	UnLockSem[0].sem_num = 0;
	UnLockSem[0].sem_op  = 3;
	UnLockSem[0].sem_flg = SEM_UNDO;
		
	UnLockSem[1].sem_num = 1;
	UnLockSem[1].sem_op  = 2;
	UnLockSem[1].sem_flg = SEM_UNDO;

	printf("Calling the semop from thread1\n");
	
	while(1)
		{
		if(counter == MAXVAL)
			break;
		ret = semop(semid, &LockSem[0], 2);
		if(ret == -1 || counter == MAXVAL)
			{
			printf("Semop failed with errno %d\n",errno);
			break;
			}
		counter++;
		ret = semop(semid, &UnLockSem[0], 2);
		if(ret == -1)
			{
			printf("Semop failed with errno %d\n",errno);
			break;
			}

		}
		return 0;
	}

void* thread2(void* arg)
	{
	int semid = *(int*)(arg);
	int ret=0;
	
	struct sembuf LockSem[2];
	struct sembuf UnLockSem[2];
	
	LockSem[0].sem_num = 0;
	LockSem[0].sem_op  = 0;
	LockSem[0].sem_flg = SEM_UNDO;
	
	LockSem[1].sem_num = 1;
	LockSem[1].sem_op  = -2;
	LockSem[1].sem_flg = SEM_UNDO;
	
	
	UnLockSem[0].sem_num = 0;
	UnLockSem[0].sem_op  = 3;
	UnLockSem[0].sem_flg = SEM_UNDO;
		
	UnLockSem[1].sem_num = 1;
	UnLockSem[1].sem_op  = 1;
	UnLockSem[1].sem_flg = SEM_UNDO;

	
	printf("Calling the semop from thread2\n");
	
	while(1)
		{
		if(counter == MAXVAL)
			break;
		ret = semop(semid, &LockSem[0], 2);
		if(ret == -1 || counter == MAXVAL)
			{
			printf("Semop failed with errno %d\n",errno);
			break;
			}
		counter++;
		ret = semop(semid, &UnLockSem[0], 2);
		if(ret == -1)
			{
			printf("Semop failed with errno %d\n",errno);
			break;
			}

		}
		return 0;
	}



void* thread3(void* arg)
	{
	int semid = *(int*)(arg);
	int ret=0;
	
	struct sembuf LockSem[2];
	struct sembuf UnLockSem[2];
	struct sembuf SemBuf[2];
	
	LockSem[0].sem_num = 0;
	LockSem[0].sem_op  = -2;
	LockSem[0].sem_flg = SEM_UNDO;
				
	UnLockSem[0].sem_num = 0;
	UnLockSem[0].sem_op  = -1;
	UnLockSem[0].sem_flg = 0;	
	
	SemBuf[0].sem_num = 0;
	SemBuf[0].sem_op  = 5;
	SemBuf[0].sem_flg = SEM_UNDO;	

	
	printf("Calling the semop from thread2\n");
	
	while(1)
		{
		ret = semop(semid, &LockSem[0], 1);
		if(ret == -1)
			{
			printf("Semop failed with errno %d\n",errno);
			break;
			}
		printf("The value is %d\n",counter);
		
		if(counter == MAXVAL) //you check this by adding and comparing the total
			{
			semop(semid, &SemBuf[0], 1);	
			break;
			}

		ret = semop(semid, &UnLockSem[0], 1);
		if(ret == -1)
			{
			printf("Semop failed with errno %d\n",errno);
			break;
			}
		}
		return 0;
	}

TInt CTestMisc::semoptest20()
	{
	int semid,ret=KErrNone,ret1=0;
	struct sembuf sb[2];
	struct sembuf sb1[1];
	union semun setval;
	unsigned short int tmp[]={0,0};
	int result;
	
	pthread_t tid1;
	pthread_t tid2;
	pthread_t tid3;
	
	setval.array = &tmp[0];
	
	sb[0].sem_num = 1;
	sb[0].sem_op =  1;
	sb[0].sem_flg = SEM_UNDO;
	
	sb1[0].sem_num = 0;
	sb1[0].sem_op  = -5;
	sb1[0].sem_flg = 0;
	
	
	if ((semid = semget(IPC_PRIVATE,2,IPC_CREAT | IPC_EXCL | IPC_R | IPC_W)) == -1) 
		{
		_LIT(KFunc, "semaphore create failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	return KErrGeneral;
    	}
	
	semctl(semid,2,SETALL,setval);
	if(ret1 == -1)
		{
		_LIT(KFunc, "semaphore SETALL failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    	
	
	pthread_create(&tid1, NULL, thread1, &semid);
	pthread_create(&tid2, NULL, thread2, &semid);
	pthread_create(&tid3, NULL, thread3, &semid);
	

	result = semop(semid, &sb[0], 1);
	if(result == -1)
		{
		_LIT(KFunc, "semaphore semop failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
	
	result = semop(semid, &sb1[0], 1);
	if(result == -1)
		{
		_LIT(KFunc, "semaphore semop failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
    
    tmp[0] = 0;
	tmp[1] = 5;
	
	if(semctl(semid,2,SETALL,setval))
		{
		_LIT(KFunc, "semaphore setall failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}
	
    if (semctl(semid, 1, IPC_RMID) == -1) 
    	{
		_LIT(KFunc, "semaphore delete failed with errno %d\n");
      	INFO_PRINTF2(KFunc, errno);
       	ret = KErrGeneral;
    	}	
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_join(tid3,NULL);
    return ret;
	}

	
TVerdict CTestMisc::TestMaxFopen()
	{
    SetTestStepResult(EPass);
    
        FILE *fp[1220];
        char name[1220] =
        {
                0
        };
        int i=0;
      mkdir("c:\\Test0", 700);
  	  mkdir("c:\\Test1", 700);
  	  mkdir("c:\\Test2",700);
  	  mkdir("c:\\Test3",700);
  	  mkdir("c:\\Test4",700);
    
      for(i = 0; i < OPEN_MAX; i++)
      {
        if(i <= 50)
			sprintf(name, "%s%d%s", "c://Test0//Hello",i,".txt");
		else if (i <= 100)
			sprintf(name, "%s%d%s", "c://Test1//Hello",i,".txt");
		else if (i <= 150)
			sprintf(name, "%s%d%s", "c://Test2//Hello",i,".txt");
		else if (i <= 240)
			sprintf(name, "%s%d%s", "c://Test3//Hello",i,".txt");
		else 
			sprintf(name, "%s%d%s", "c://Test4//Hello",i,".txt");
      	errno = 0;

       
        if ((fp [i] = fopen (name, "a")) == NULL)
        {
            INFO_PRINTF2(_L("Failed to open file number : %d"), i);
            INFO_PRINTF2(_L("Error number : %d"), errno);

            if(i != (OPEN_MAX-3))
            {
            	SetTestStepResult(EFail);
            }
            
            break;
        }

      }
      for(--i; i >=0 ; i--)
      {
      	fclose(fp[i]);
      	if(i <= 50)
			sprintf(name, "%s%d%s", "c://Test0//Hello",i,".txt");
		else if (i <= 100)
			sprintf(name, "%s%d%s", "c://Test1//Hello",i,".txt");
		else if (i <= 150)
			sprintf(name, "%s%d%s", "c://Test2//Hello",i,".txt");
		else if (i <= 240)
			sprintf(name, "%s%d%s", "c://Test3//Hello",i,".txt");
		else 
			sprintf(name, "%s%d%s", "c://Test4//Hello",i,".txt");
		unlink(name);
   
      }
      rmdir("c:\\Test1");
      rmdir("c:\\Test2");
      rmdir("c:\\Test3");
      rmdir("c:\\Test4");
      return TestStepResult();
	}

TInt CTestMisc::TestPopen3FdTransfer()	
	{
	int fds[3];
	int fd1 = open("C:\\SharedFile.txt",O_CREAT|O_RDWR);
	
	if(fd1 <= 0)
	{
	INFO_PRINTF1(_L("Error opening SharedFile.txt file"));
	return errno;
	}
	int fd2 = open("C:\\popentest.txt",O_CREAT|O_RDWR);
	if(fd2 <= 0)
	{
	INFO_PRINTF1(_L("Error creating file popentest.txt"));
	close(fd1);
	return errno;
	}
	
	char buf[10];
	sprintf(buf,"%d",fd2);
	if(write(fd1,buf,strlen(buf)+1) == -1)
	{
	INFO_PRINTF1(_L("Error writing to SharedFile.txt"));
	close(fd1);
	close(fd2);
	return errno;
	}
	close(fd1);
	int pid,status;
	if ((pid=popen3("z:\\sys\\bin\\TPopen3FdTransfer.exe",NULL,environ,fds)) == -1) 
	{ 
	INFO_PRINTF1(_L("Cant create the process using popen3"));
	close(fd2);
	return errno;
	}	

	int ret = waitpid(pid, &status, 0);
	
	close(fd2);
	INFO_PRINTF2(_L("waitpid return result = %d"), status);
	return status;
	
	}

TInt CTestMisc::TestPopenFdTransfer()	
	{
	FILE *fp;
	char filename[15];
	int tempfd[10];
	int fd1 = open("C:\\SharedFile.txt",O_CREAT|O_RDWR);
	
	if(fd1 <= 0)
	{
	INFO_PRINTF1(_L("Error opening SharedFile.txt file"));
	return errno;
	}
	//open temporary files to check the fd transfer for more than 8 files
    for(int i = 1; i<= 10; i++)
    {
        sprintf(filename,"C:\\temp%d",i);
    	if((tempfd[i-1] = open(filename,O_CREAT|O_RDWR)) < 0)
    		{
    			INFO_PRINTF2(_L("Error opening %s file"),filename);
    			break;
    		}
    	
    }
	
    int fd2 = open("C:\\popentest.txt",O_CREAT|O_RDWR);
	if(fd2 <= 0)
	{
		INFO_PRINTF1(_L("Error creating file popentest.txt"));
		close(fd1);
		return errno;
	}
	
	char buf[10];
	sprintf(buf,"%d",fd2);
	if(write(fd1,buf,strlen(buf)+1) == -1)
	{
		INFO_PRINTF1(_L("Error writing to SharedFile.txt"));
		close(fd1);
		close(fd2);
		return errno;
	}
	close(fd1);
	char mode='r';
	if((fp = popen("z:\\sys\\bin\\TPopen3FdTransfer.exe", &mode)) == NULL)
	{ 
		INFO_PRINTF1(_L("Cant create the process using popen"));
		close(fd2);
		return errno;
	}

	int ret = pclose(fp);
	INFO_PRINTF2(_L("pclose return result = %d"), ret);
	close(fd2);
	
	for(int i = 1; i<= 10; i++)
	{
		close(tempfd[i-1]);	       
	}
	return ret;
	
	}

TInt CTestMisc::TestSystemFdTransfer()	
	{
	int fd1 = open("C:\\SharedFile.txt",O_CREAT|O_RDWR);
	
	if(fd1 <= 0)
	{
	INFO_PRINTF1(_L("Error opening SharedFile.txt file"));
	return errno;
	}
	int fd2 = open("C:\\popentest.txt",O_CREAT|O_RDWR);
	if(fd2 <= 0)
	{
	INFO_PRINTF1(_L("Error creating file popentest.txt"));
	close(fd1);
	return errno;
	}
	
	char buf[10];
	sprintf(buf,"%d",fd2);
	if(write(fd1,buf,strlen(buf)+1) == -1)
	{
	INFO_PRINTF1(_L("Error writing to SharedFile.txt"));
	close(fd1);
	close(fd2);
	return errno;
	}
	close(fd1);
	int ret;
	ret = system("z:\\sys\\bin\\TPopen3FdTransfer.exe");
	close(fd2);
	INFO_PRINTF2(_L("system return result = %d"), ret);
	return ret;
	
	}

TInt CTestMisc::TestPosixSpawnFdTransfer()	
	{
	int fd1 = open("C:\\SharedFile.txt",O_CREAT|O_RDWR);
	
	if(fd1 <= 0)
	{
	INFO_PRINTF1(_L("Error opening SharedFile.txt file"));
	return errno;
	}
	int fd2 = open("C:\\popentest.txt",O_CREAT|O_RDWR);
	if(fd2 <= 0)
	{
	INFO_PRINTF1(_L("Error creating file popentest.txt"));
	close(fd1);
	return errno;
	}
	
	char buf[10];
	sprintf(buf,"%d",fd2);
	if(write(fd1,buf,strlen(buf)+1) == -1)
	{
	INFO_PRINTF1(_L("Error writing to SharedFile.txt"));
	close(fd1);
	close(fd2);
	return errno;
	}
	close(fd1);
	
	
	char **argv=(char**)malloc(2*sizeof(char*));
	argv[0]=(char*)malloc(40*sizeof(char));
	argv[1]= 0;
	
	strcpy(argv[0], "z:\\sys\\bin\\TPopen3FdTransfer.exe");
	
	pid_t pid;
	if(posix_spawn(&pid, "z:\\sys\\bin\\TPopen3FdTransfer.exe", NULL, NULL, argv, (char**)NULL))
	{
	INFO_PRINTF1(_L("Cant create posix_spawn"));
	free((void*)argv);
	free((void*)argv[0]);
	close(fd2);
	return -1;
	}
	TInt status;
	waitpid(pid, &status, 0);
	
	close(fd2);
	free((void*)argv[0]);
	free((void*)argv);
	INFO_PRINTF2(_L("waitpid return result = %d"), status);
	return status;
	
	}

TInt CTestMisc::TestPopen3PipeReadTransfer()	
	{
	int fds[3];
	int fd1 = open("C:\\SharedFile.txt",O_CREAT|O_RDWR);
	
	if(fd1 <= 0)
	{
	INFO_PRINTF1(_L("Error opening SharedFile.txt file"));
	return errno;
	}
	
	char filename[15];
	int tempfd[10];
	
	for(int i = 1; i<= 10; i++)
	{
	sprintf(filename,"C:\\temp%d",i);
	if((tempfd[i-1] = open(filename,O_CREAT|O_RDWR)) < 0)
	{
	  	INFO_PRINTF2(_L("Error opening %s file"),filename);
	   	break;
	}
	    
	}
	int fd[2];
	if(pipe(fd) == -1)
	{
	INFO_PRINTF1(_L("Error creating pipes"));
	close(fd1);
	return errno;	
	} 
	
	char buf[10];
	sprintf(buf,"%d",fd[0]);
	if(write(fd1,buf,strlen(buf)+1) == -1)
	{
	INFO_PRINTF1(_L("Error writing to SharedFile.txt"));
	close(fd1);
	close(fd[0]),close(fd[1]);
	return errno;
	}
	close(fd1);
	int pid,status;
	if ((pid=popen3("z:\\sys\\bin\\PipeReadTransfer.exe",NULL,environ,fds)) == -1) 
	{ 
	INFO_PRINTF1(_L("Cant create the process popen3"));
	close(fd[0]),close(fd[1]);
	return errno;
	}	
    
	if(write(fd[1],"testing pipe",12) == -1)
	{
		INFO_PRINTF1(_L("Error writing to pipe"));
		close(fd[0]),close(fd[1]);
		return errno;
	}
	
	int ret = waitpid(pid, &status, 0);
	
	close(fd[0]),close(fd[1]);
	INFO_PRINTF2(_L("waitpid return result = %d"), status);
	
	for(int i = 1; i<= 10; i++)
	{
		close(tempfd[i-1]);	       
	}
	
	return status;
	
	}

TInt CTestMisc::TestPopenPipeReadTransfer()	
	{
	FILE *fp;
	int fd1 = open("C:\\SharedFile.txt",O_CREAT|O_RDWR);
	
	if(fd1 <= 0)
	{
	INFO_PRINTF1(_L("Error opening SharedFile.txt file"));
	return errno;
	}
	
	int fd[2];
	if(pipe(fd) == -1)
	{
	INFO_PRINTF1(_L("Error creating pipes"));
	close(fd1);
	return errno;	
	} 
	
	char buf[10];
	sprintf(buf,"%d",fd[0]);
	if(write(fd1,buf,strlen(buf)+1) == -1)
	{
	INFO_PRINTF1(_L("Error writing to SharedFile.txt"));
	close(fd1);
	close(fd[0]),close(fd[1]);
	return errno;
	}
	close(fd1);
	char mode='r';
	if((fp = popen("z:\\sys\\bin\\PipeReadTransfer.exe", &mode)) == NULL)
	{ 
	INFO_PRINTF1(_L("Cant create process using popen"));
	close(fd[0]),close(fd[1]);
	return errno;
	}
	if(write(fd[1],"testing pipe",12) == -1)
	{
		INFO_PRINTF1(_L("Error writing to pipe"));
		close(fd[0]),close(fd[1]);
		return errno;
	}
	
	int ret = pclose(fp);
	INFO_PRINTF2(_L("pclose return result = %d"), ret);
	close(fd[0]),close(fd[1]);
	return ret;
	
	}

TInt CTestMisc::TestPopen3PipeWriteTransfer()	
	{
	int fds[3];
	int fd1 = open("C:\\SharedFile.txt",O_CREAT|O_RDWR);
	
	if(fd1 <= 0)
	{
	INFO_PRINTF1(_L("Error opening SharedFile.txt file"));
	return errno;
	}
	
	int fd[2];
	if(pipe(fd) == -1)
	{
	INFO_PRINTF1(_L("Error creating pipes"));
	close(fd1);
	return errno;	
	} 
	
	char buf[10];
	sprintf(buf,"%d",fd[1]);
	if(write(fd1,buf,strlen(buf)+1) == -1)
	{
	INFO_PRINTF1(_L("Error writing to SharedFile.txt"));
	close(fd1);
	close(fd[0]),close(fd[1]);
	return errno;
	}
	close(fd1);
	int pid,status;
	if ((pid=popen3("z:\\sys\\bin\\PipeWriteTransfer.exe",NULL,environ,fds)) == -1) 
	{ 
	INFO_PRINTF1(_L("Can't create process using popen3"));
	close(fd[0]),close(fd[1]);
	return errno;
	}	
    
	if(read(fd[0],buf,5) == -1)
	{
		INFO_PRINTF1(_L("Error reading from pipe"));
		close(fd[0]),close(fd[1]);
		return errno;
	}
	
	int ret = waitpid(pid, &status, 0);
	
	close(fd[0]),close(fd[1]);
	INFO_PRINTF2(_L("waitpid return result = %d"), status);
	return status;
	
	}

TInt CTestMisc::TestPopenPipeWriteTransfer()	
	{
	FILE *fp;
	int fd1 = open("C:\\SharedFile.txt",O_CREAT|O_RDWR);
	
	if(fd1 <= 0)
	{
	INFO_PRINTF1(_L("Error opening SharedFile.txt file"));
	return errno;
	}
	
	int fd[2];
	if(pipe(fd) == -1)
	{
	INFO_PRINTF1(_L("Error creating pipes"));
	close(fd1);
	return errno;	
	} 
	
	char buf[10];
	sprintf(buf,"%d",fd[1]);
	if(write(fd1,buf,strlen(buf)+1) == -1)
	{
	INFO_PRINTF1(_L("Error writing to SharedFile.txt"));
	close(fd1);
	close(fd[0]),close(fd[1]);
	return errno;
	}
	close(fd1);
	char mode='r';
	if((fp = popen("z:\\sys\\bin\\PipeWriteTransfer.exe", &mode)) == NULL)
	{ 
	INFO_PRINTF1(_L("Cant create process using popen"));
	close(fd[0]),close(fd[1]);
	return errno;
	}
	
	if(read(fd[0],buf,5) == -1)
	{
		INFO_PRINTF1(_L("Error reading from pipe"));
		close(fd[0]),close(fd[1]);
		return errno;
	}
	
	int ret = pclose(fp);
	INFO_PRINTF2(_L("pclose return result = %d"), ret);
	close(fd[0]),close(fd[1]);
	return ret;
	
	}

TVerdict CTestMisc::TestPipeReadWrite()
	{
	 SetTestStepResult(EPass);
	 int  err, fd[2];
	 const char *bufW = "somesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesome"; 
	 char bufR[720];  
	 err = pipe (fd);
	  if (err < 0)
	    	{
	       INFO_PRINTF1 (_L("error in creating the pipe\n"));
	       }
	   fcntl(fd[1],F_SETFL, O_NONBLOCK);
	   err = write (fd[1], bufW, strlen (bufW));
	   if (err < 0)
		    {
		      INFO_PRINTF2(_L("error in writing the pipe\n"), err);
		    }
	  else
		    {
		     INFO_PRINTF1(_L("success in writing\n"));
		     INFO_PRINTF2 (_L("the returned value is %d\n"),  err);
		    }
		    
	  fcntl(fd[0],F_SETFL, O_NONBLOCK);
	  err = read (fd[0], bufR, err);
	  if (err < 0)
		    {
		    INFO_PRINTF2(_L("error in writing the pipe\n"), err);
		    }
	  else
		    {
		     INFO_PRINTF1(_L("success in reading\n"));
		     INFO_PRINTF2 (_L("the returned value is %d\n"),  err);
		    }
      return TestStepResult();
	}
	
TVerdict CTestMisc::TestFileReadWrite()
	{
	SetTestStepResult(EPass);
	int fd,ret,ret1,fd1;
	char *buf = (char*)malloc(688);
	char *buf1 = "somesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesomesome";
	unlink("c:\\readwrite.txt");
	fd = open("c:\\readwrite.txt",O_RDWR|O_CREAT,0666);
	fcntl(fd,F_SETFL, O_NONBLOCK);

	if (fd > 0)
		{	
		INFO_PRINTF1(_L("success in opening the file\n"));
		ret = write(fd,buf1,strlen(buf1)); //works fine in both mode.(acc. to the spec)
		if (ret == -1)
			{
			INFO_PRINTF1(_L("error in writing\n"));
			}
		else
			{
			INFO_PRINTF1(_L("success in writing\n"));
			INFO_PRINTF2(_L("value returned is %d"),ret);
			}
		close(fd);
		}
	fd1 = open("c:\\readwrite.txt",O_RDWR);
	if (fd1 >0 )
		{
		INFO_PRINTF1(_L("success in opening the file for the second time\n"));
		ret1 = read(fd1,buf,ret);
		if(ret1 == -1)
			{
			INFO_PRINTF1(_L("error in reading\n"));
			}
		else
			{
			INFO_PRINTF1(_L("success in reading\n"));
			INFO_PRINTF2(_L("value returned is %d"),ret1);
			}
		close(fd1);
		free(buf);
		}
	else
		{
		INFO_PRINTF1(_L("error in opening the file \n"));
		}
      return TestStepResult();
	}

TVerdict CTestMisc::TestTzname()
	{
	    SetTestStepResult(EPass);
		int i = -1;
		tzname[0] = "symbian";
		char * temp = "symbian";

		i = strcmp(temp,tzname[0]);
		if(i == 0)
			SetTestStepResult(EPass);
		else
			SetTestStepResult(EFail);
      
		return TestStepResult();
	}


TVerdict CTestMisc::TestOpenWithExclusiveLock()
	{
	SetTestStepResult(EPass);
	//creating the file for the first time
	int fd1 = open("C:\\openexclusivelockfile.txt",O_CREAT|O_RDWR, 0666);
	if(fd1 <= 0)
		{
		INFO_PRINTF1(_L("Error opening openexclusivelockfile.txt file"));
		SetTestStepResult(EFail);
		}
	//writing something into the file
	int writeval = write(fd1, "elephant on land", 18);
	if(writeval <= 0)
		{
		INFO_PRINTF1(_L("Error writing into openexclusivelockfile.txt file"));
		SetTestStepResult(EFail);
		}
	//closing the file descriptor
	close(fd1);
	//re-opening file with O_EXLOCK flag
	int fd2 = open("C:\\openexclusivelockfile.txt",O_EXLOCK, 0666);
	if(fd2 <= 0)
		{
		INFO_PRINTF1(_L("Error opening file openexclusivelockfile.txt"));
		SetTestStepResult(EFail);
		}
	//we will spawn an exe which will try to write to the child exe
	//and will fail if it tries to write to the locked region
	char **argv=(char**)malloc(2*sizeof(char*));
	argv[0]=(char*)malloc(50*sizeof(char));
	argv[1]= 0;
	
	strcpy(argv[0], "z:\\sys\\bin\\childopenexclusivelock.exe");
	
	pid_t pid;
	if(posix_spawn(&pid, "z:\\sys\\bin\\childopenexclusivelock.exe", NULL, NULL, argv, (char**)NULL))
		{
		INFO_PRINTF1(_L("Cant create posix_spawn"));
		free((void*)argv);
		free((void*)argv[0]);
		close(fd2);
		SetTestStepResult(EFail);
		}
	TInt status;
	waitpid(pid, &status, 0);
	INFO_PRINTF2(_L("waitpid returns = %d"), WEXITSTATUS(status));
	if (WEXITSTATUS(status) == 0)
		{
		INFO_PRINTF1(_L("child process failed to write into the locked region as expected"));
		INFO_PRINTF1(_L("test case passed"));		
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF1(_L("test case failed"));
		SetTestStepResult(EFail);
		}
	close(fd2);
	free((void*)argv[0]);
	free((void*)argv);
	unlink("C:\\openexclusivelockfile.txt");
	return TestStepResult();
	}
