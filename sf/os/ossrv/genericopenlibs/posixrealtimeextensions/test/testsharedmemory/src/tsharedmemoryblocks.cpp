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
// Name        : tsharedmemoryblocks.cpp
// Test cases for blocking signal api's
//



#include "tsharedmemory.h"

static int closenunlink(int fd,char *name )
	{
	int ret, ret1 = KErrGeneral;
	ret = close(fd);
	if(ret != 0)
		{
		goto close;		
		}
	ret = shm_unlink(name);
	if(ret != 0)
		{
		goto close;		
		}	
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory1
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: Test case added to open the shared memory object using shm_open() and unlinking the same using shm_unlink()
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory1 (  )
	{
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd;
	TBuf8<50> shmname ;
	char filename[50];
	ret = GetStringFromConfig(ConfigSection(), _L("Shmname"), Shmname );
	shmname.Copy(Shmname) ; 
	char *file = (char *) shmname.Ptr() ;
	if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read input file name") );
     	goto close;
		}
    file[shmname.Length()] = '\0' ;
	strcpy(filename , file) ;
	fd = shm_open(filename, O_CREAT, 0777);
	if (fd <= 0)
		{
		ERR_PRINTF2(_L("Error in opening the file and errno is %d"),errno);
		goto close;
		}
	ret = closenunlink(fd,filename);
	if(ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Error in deleting the file"));
		goto close;		
		}
	INFO_PRINTF1(_L("Successfully able to create a shared memory"));	
	ret1 = KErrNone;

	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory2
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: Trying to open a file which is closed and unlinked
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory2 (  )
	{
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd;
	TBuf8<50> shmname ;
	char filename[50];
    ret = GetStringFromConfig(ConfigSection(), _L("Shmname"), Shmname );
    shmname.Copy(Shmname) ; 
	char *file = (char *) shmname.Ptr() ;
    if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read input file name") );
     	goto close;
		}
	file[shmname.Length()] = '\0' ;
	strcpy(filename , file) ;
	fd = shm_open(filename,O_CREAT|O_RDWR,0777);
	if (fd <= 0)
		{
		ERR_PRINTF2(_L("Error in opening the file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to create a shared memory"));
	ret = closenunlink(fd,filename);
	if(ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Error in deleting the file"));
		goto close;		
		}
	INFO_PRINTF1(_L("Successfully able to delete a shared memory"));
	fd = shm_open(filename,O_RDWR,0777);
	if((fd != -1) || (errno != ENOENT))
		{
		ERR_PRINTF2(_L("shm_open() failed on negative test and errno is %d"),errno);
		goto close;	
		}
	INFO_PRINTF1(_L("shm_open() successfully returned ENOENT on negative test"));
	ret1 = KErrNone;

	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory3
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: Trying to open a file which is already present using O_EXCL
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory3 (  )
	{
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd;
	TBuf8<50> shmname ;
	char filename[50];
    ret = GetStringFromConfig(ConfigSection(), _L("Shmname"), Shmname );
    shmname.Copy(Shmname) ; 
	char *file = (char *) shmname.Ptr() ;
    if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read input file name") );
     	goto close;
		}
	file[shmname.Length()] = '\0' ;
	strcpy(filename , file) ;
	fd = shm_open(filename,O_CREAT,0777);
	if (fd <= 0)
		{
		ERR_PRINTF2(_L("Error in opening the file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to create a shared memory"));
	ret =  close(fd);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in closing the file and errno is %d"),errno);
		goto close;		
		}
	fd = shm_open(filename,O_CREAT|O_EXCL,0777);
	if((fd != -1) || (errno != EEXIST))
		{
		ERR_PRINTF1(_L("shm_open() failed on negative test"));
		goto close;	
		}
	INFO_PRINTF1(_L("shm_open() successfully returned EEXIST on negative test"));
	ret = shm_unlink(filename);
	if(ret != 0)
		{
		ERR_PRINTF1(_L("Error in deleting the file"));
		goto close;		
		}
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory4
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: Trying to create a shared memory using O_CREAT|O_EXCL|O_RDWR flag
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory4 (  )
	{
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd;
	TBuf8<50> shmname ;
	char filename[50];
    ret = GetStringFromConfig(ConfigSection(), _L("Shmname"), Shmname );
    shmname.Copy(Shmname) ; 
	char *file = (char *) shmname.Ptr() ;
    if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read input file name") );
     	goto close;
		}
	file[shmname.Length()] = '\0' ;
	strcpy(filename , file) ;
	fd = shm_open(filename,O_CREAT|O_EXCL|O_RDWR,0777);
	if (fd <= 0)
		{
		ERR_PRINTF2(_L("Error in opening the file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to create a shared memory"));
	ret =  closenunlink(fd,filename);
	if(ret == KErrGeneral)
		{
		ERR_PRINTF2(_L("Error in closing the file and errno is %d"),errno);
		goto close;		
		}
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory5
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: To validate that the lowest numbered file descriptor not currently opened for that process is returned by shm_open()
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory5 (  )
	{
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd, fd1, fd2, fd3;
	TBuf8<50> shmname ;
	char filename[50];
    ret = GetStringFromConfig(ConfigSection(), _L("Shmname"), Shmname );
    shmname.Copy(Shmname) ; 
	char *file = (char *) shmname.Ptr() ;
    if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read input file name") );
     	goto close;
		}
	file[shmname.Length()] = '\0' ;
	strcpy(filename , file) ;
	fd1 = shm_open(filename,O_CREAT,0777);
	if (fd1 <= 0)
		{
		ERR_PRINTF2(_L("Error in opening the file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to create a shared memory"));
	fd2 = shm_open(filename,O_CREAT,0777);
	if (fd2 != (fd1+1))
		{
		ERR_PRINTF2(_L("Error in opening the file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to create a shared memory"));
	fd = fd1;
	ret =  close(fd);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in closing the file and errno is %d"),errno);
		goto close;		
		}
	fd3 = shm_open(filename,O_CREAT,0777);
	if (fd3 <= 0)
		{
		ERR_PRINTF2(_L("Error in opening the file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to create a shared memory"));
	if (fd1 != fd3)
		{
		ERR_PRINTF1(_L("Error in returning the lowest fd value"));
		goto close;
		}
	INFO_PRINTF1(_L("Successfully returned the lowest fd value"));
	ret =  close(fd2);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in closing the file and errno is %d"),errno);
		goto close;		
		}
	ret = closenunlink(fd3,filename);
	if(ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Error in deleting the file"));
		goto close;		
		}
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory6
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open()
// Description: Trying to open a shared memory with only O_WRONLY flag set
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory6 (  )
	{	
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd;
	TBuf8<50> shmname ;
	char filename[50];
    ret = GetStringFromConfig(ConfigSection(), _L("Shmname"), Shmname );
    shmname.Copy(Shmname) ; 
	char *file = (char *) shmname.Ptr() ;
    if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read input file name") );
     	goto close;
		}
	file[shmname.Length()] = '\0' ;
	strcpy(filename , file) ;
	fd = shm_open(filename,O_CREAT,0777);
	ret =  close(fd);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in closing the file and errno is %d"),errno);
		goto close;		
		}
	fd = shm_open(filename,O_WRONLY,0777);
	if ((fd != -1) || (errno != EINVAL))
		{
		ERR_PRINTF2(_L("shm_unlink() failed on negative test and errno is %d"),errno);
		goto close;	
		}
	INFO_PRINTF1(_L("shm_unlink() successfully returned EINVAL on negative test"));
	ret1 = KErrNone;
	
	close:
	shm_unlink(filename);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory7
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open()
// Description: Trying to open a shared memory with NULL as the input
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory7 (  )
	{	
	int ret1 = KErrGeneral, fd;
	fd = shm_open(NULL,O_CREAT|O_RDWR,0777);
	if ((fd != -1) || (errno != EFAULT))
		{
		ERR_PRINTF1(_L("shm_unlink() failed on negative test"));
		goto close;	
		}
	INFO_PRINTF1(_L("Successfully shm_unlink() returned EFAULT on negative test"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory8
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open()
// Description: Trying to open a shared memory name as an empty string
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory8 (  )
	{
	int fd, ret1 = KErrGeneral;
	fd = shm_open("",O_CREAT,0777);
	if ((fd != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("shm_unlink() failed on negative test"));
		goto close;	
		}
	INFO_PRINTF1(_L("Successfully shm_unlink() returned EINVAL on negative test"));
	ret1 = KErrNone;
	
	close:
	return ret1;		
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory9
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: Trying to open a shared memory with a name less than maximum bytes
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory9 (  )
	{
	int len, fd, ret1 = KErrGeneral, ret;
	char path[258];
	for (len=0; len<219; len++)
		{
		path[len] = 'a';
		}
	path[len]='\0';
	fd = shm_open(path,O_CREAT,0777);
	if (fd <= 0)
		{
		ERR_PRINTF2(_L("Error in opening the file and errno is %d"),errno);
		goto close;
		}
	ret = closenunlink(fd,path);
	if(ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Error in deleting the file"));
		goto close;		
		}
	INFO_PRINTF1(_L("Successfully able to create a shared memory"));
	ret1 = KErrNone;
	
	close:
	return ret1;		
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory10
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open()
// Description: Negative test to return ENAMETOOLONG for shm_open()
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory10 (  )
	{
	int len, fd, ret1 = KErrGeneral;
	char path[258];
	for (len=0; len<258; len++)
		{
		path[len] = 'a';
		}
	path[len]='\0';
	fd = shm_open(path,O_CREAT,0777);
	if((fd != -1) || (errno != ENAMETOOLONG))
		{
		ERR_PRINTF1(_L("shm_open() failed on negative test"));
		goto close;	
		}
	INFO_PRINTF1(_L("shm_open() successfully returned ENAMETOOLONG on negative test"));
	ret1 = KErrNone;
	
	close:
	return ret1;		
	}
	
// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory11
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: Opening an existing shared memory in the different process using posix_spawn()
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory11 (  )
	{
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd, status;
	pid_t pid1, pid;
	char **argv = (char**)malloc(2*sizeof(char*));
	argv[0] = (char*)malloc(30*sizeof(char));
	argv[1] = 0;
	fd = shm_open("shmemtest", O_CREAT, 0777);
	if (fd <= 0)
		{
		ERR_PRINTF2(_L("Error in opening the file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to create a shared memory"));	
	strcpy(argv[0], "z:\\sys\\bin\\openshmem.exe");
	ret = posix_spawn(&pid, "z:\\sys\\bin\\openshmem.exe", NULL, NULL, argv, (char**)NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
		goto close;
		}
	pid1 = waitpid(pid, &status, WUNTRACED);
	if (pid1 != pid)
		{
		ERR_PRINTF1(_L("waitpid failed..."));	
		goto close;
		}
	if(!(WEXITSTATUS(status)))
		{
		ERR_PRINTF1(_L("Failed to open the shared memory in the child process"));	
		goto close;		
		}
	INFO_PRINTF1(_L("Successfully able to open the shared memory in the child process"));
	ret = closenunlink(fd,"shmemtest");
	if(ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Error in deleting the file"));
		goto close;		
		}
	ret1 = KErrNone;

	close:
	free((void*)argv[0]);
	free((void*)argv);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory12
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: Opening an existing shared memory in the different process using popen()
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory12 (  )
	{ 
	int ret, ret1 = KErrGeneral, fd, status, fds[3], pid, pid1;
	char buf[22];
	fd = shm_open("shmemtest_popen", O_CREAT|O_RDWR , 0666);
	if (fd <= 0)
		{
		ERR_PRINTF2(_L("Error in opening the file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF2(_L("Successfully able to create a shared memory with fd  = %d"),fd);	
	ret = write(fd, "helloworldss", 6);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Error in writing into a file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to write in a shared memory for the first time"));
	ret = write(fd, "gimmick", 6);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Error in writing into a file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to write in a shared memory"));
	ret = lseek(fd, 3, SEEK_SET);
	INFO_PRINTF2(_L("lseek returns = %d"),ret);
	if(ret == -1)
		{
		INFO_PRINTF2(_L("Failed to move the offset using lseek() and  errno is %d"),errno);
		goto close;		
		}
	ret = write(fd, "symbianis", 9);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Error in writing into a file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to write in a shared memory"));
	ret = lseek(fd, -12, SEEK_END);
	INFO_PRINTF2(_L("lseek returns = %d"),ret);
	if(ret == -1)
		{
		INFO_PRINTF2(_L("Failed to move the offset using lseek() and  errno is %d"),errno);
		goto close;		
		}
	ret = read(fd, (void*)buf, 21);
	INFO_PRINTF2(_L("read returns = %d"),ret);
	if(ret == -1)
		{
		INFO_PRINTF2(_L("Error in reading from file and  errno is %d"),errno);
		goto close;		
		}
	INFO_PRINTF1(_L("Successfully able to read from shared memory"));
	pid = popen3("openshmem_popen.exe", NULL, NULL, fds);
	if (pid == 0)
		{
		ERR_PRINTF2(_L("Error in popen() and errno is set to %d"),errno);
		goto close;		
		}
	pid1 = waitpid(pid, &status, WUNTRACED);
	if (pid1 != pid)
		{
		ERR_PRINTF1(_L("waitpid failed..."));	
		goto close;
		}
	if(!(WEXITSTATUS(status)))
		{
		ERR_PRINTF1(_L("Failed to open the shared memory in the child process"));	
		goto close;		
		}
	ret = strncmp(buf, "helsymbianis",12);
	if (ret != 0)
		{
		ERR_PRINTF2(_L("Error in reading and writing and errno is %d"),errno);
		goto close;
		}
	ret1 = KErrNone;
	
	close:
	closenunlink(fd,"shmemtest_popen");
	return ret1;		
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory13
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_unlink()
// Description: To delete the shared memory which does not exists with an argument name been less than PATH_MAX/NAME_MAX
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory13 (  )
	{	
	int len, fd, ret1 = KErrGeneral;
	char path[253];
	for (len=0; len<219; len++)
		{
		path[len] = 'b';
		}
	path[len]='\0';
	fd = shm_unlink(path);
	if((fd != -1) || (errno != ENOENT) )
		{
		ERR_PRINTF1(_L("shm_unlink() failed on negative test"));
		goto close;	
		}
	INFO_PRINTF1(_L("shm_unlink() successfully returned ENOENT on negative test"));
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory14
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_unlink()
// Description: To delete the shared memory with the name of  an argument been greater than 256.
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory14 (  )
	{	
	int i, ret, ret1 = KErrGeneral;
	char path[258];
	for (i=0; i<258; i++)
		{
		path[i] = 'b';
		}
	path[i]='\0';
	ret = shm_unlink(path);
	if((ret != -1) || (errno != ENAMETOOLONG) )
		{
		ERR_PRINTF1(_L("shm_unlink() failed on negative test"));
		goto close;	
		}
	INFO_PRINTF1(_L("shm_unlink() successfully returned ENAMETOOLONG on negative test"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory15
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_unlink()
// Description: To delete the shared memory with NULL as the input
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory15 (  )
	{	
	int ret, ret1 = KErrGeneral;
	ret = shm_unlink(NULL);
	if((ret != -1) || (errno != EFAULT) )
		{
		ERR_PRINTF1(_L("shm_unlink() failed on negative test"));
		goto close;	
		}
	INFO_PRINTF1(_L("shm_unlink() successfully returned EFAULT on negative test"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory16
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_unlink()
// Description: Trying to delete an empty string shared memory
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory16 (  )
	{	
	int ret, ret1 = KErrGeneral;
	ret = shm_unlink("");
	if((ret != -1) || (errno != ENOENT) )
		{
		ERR_PRINTF1(_L("shm_unlink() failed on negative test"));
		goto close;	
		}
	INFO_PRINTF1(_L("shm_unlink() successfully returned ENOENT on negative test"));
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory17
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: Writing into a shared memory and reading it
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory17 (  )
	{
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd;
	TBuf8<50> shmname ;
	char buf1[15] = "Sharedmemory", buf2[15], filename[50];
    ret = GetStringFromConfig(ConfigSection(), _L("Shmname"), Shmname );
    shmname.Copy(Shmname) ; 
	char *file = (char *) shmname.Ptr() ;
    if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read input file name") );
     	goto close;
		}
	file[shmname.Length()] = '\0' ;
	strcpy(filename , file) ;
	fd = shm_open(filename,O_CREAT|O_RDWR,0777);
	if (fd <= 0)
		{
		ERR_PRINTF2(_L("Error in opening the file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to create a shared memory"));
	ret = write(fd,buf1,13);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Error in writing into a file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to write in a shared memory"));
	ret =  close(fd);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in closing the file and errno is %d"),errno);
		goto close;		
		}
	fd = shm_open(filename,O_RDWR,0777);
	if (fd <= 0)
		{
		ERR_PRINTF2(_L("Error in opening the file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to open a shared memory"));
	ret = read(fd,buf2,13);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Error in reading into a file and errno is %d"),errno);
		goto close;
		}
	ret = strncmp(buf1,buf2,12);
	if (ret != 0)
		{
		ERR_PRINTF1(_L("Input and output buffer are not same"));
		goto close;
		}
	ret = closenunlink(fd,filename);
	if(ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Error in deleting the file"));
		goto close;		
		}
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory18
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: Checking the size of shared memory when opened using O_CREAT and O_TRUNC
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory18 (  )
	{
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd;
	TBuf8<50> shmname ;
	char filename[50];
	struct stat statbuf;
    ret = GetStringFromConfig(ConfigSection(), _L("Shmname"), Shmname );
    shmname.Copy(Shmname) ; 
	char *file = (char *) shmname.Ptr() ;
    if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read input file name") );
     	goto close;
		}
	file[shmname.Length()] = '\0' ;
	strcpy(filename , file) ;
	fd = shm_open(filename,O_CREAT|O_TRUNC,0777);
	if (fd <= 0)
		{
		ERR_PRINTF2(_L("Error in opening the shared memory and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to create a shared memory"));
	ret = fstat(fd,&statbuf);
	if(statbuf.st_size != 0)
		{
		ERR_PRINTF2(_L("shared memory not truncated when created and errno is %d"),errno);
		close(fd);
		shm_unlink(filename);
		goto close;
		}	
	INFO_PRINTF1(_L("shm_open() successfully truncated shared memory to size zero with O_CREAT and O_TRUNC flag"));
	ret1 = KErrNone;
	close(fd);
	shm_unlink(filename);
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory19
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: To truncate the shared memory object by setting O_TRUNC flag.
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory19 (  )
	{
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd;
	TBuf8<50> shmname ;
	char buf1[15] = "Sharedmemory", buf2[15], filename[50];
    ret = GetStringFromConfig(ConfigSection(), _L("Shmname"), Shmname );
    shmname.Copy(Shmname) ; 
	char *file = (char *) shmname.Ptr() ;
    if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read input file name") );
     	goto close;
		}
	file[shmname.Length()] = '\0' ;
	strcpy(filename , file) ;
	fd = shm_open(filename,O_CREAT|O_RDWR,0777);
	if (fd <= 0)
		{
		ERR_PRINTF2(_L("Error in opening the file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to create a shared memory"));
	ret = write(fd,buf1,12);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Error in writing into a file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to write in a shared memory"));
	ret =  close(fd);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in closing the file and errno is %d"),errno);
		goto close;		
		}
	fd = shm_open(filename,O_TRUNC,0777);
	if (fd <= 0)
		{
		ERR_PRINTF2(_L("Error in opening the file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to open the shared memory with O_TRUNC flag"));
	ret = read(fd,buf2,12);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("file not trsuncated and errno is %d"),errno);
		closenunlink(fd,filename);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to truncate"));
	ret = closenunlink(fd,filename);
	if(ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Error in deleting the file"));
		goto close;		
		}
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory20
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: Trying to write into the shared memory with O_RDONLY flag
//				write() should fail with EBADF
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory20 (  )
	{
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd;
	TBuf8<50> shmname ;
	char buf[15] = "Sharedmemory", filename[50];
    ret = GetStringFromConfig(ConfigSection(), _L("Shmname"), Shmname );
    shmname.Copy(Shmname) ; 
	char *file = (char *) shmname.Ptr() ;
    if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read input file name") );
     	goto close;
		}
	file[shmname.Length()] = '\0' ;
	strcpy(filename , file) ;
	fd = shm_open(filename,O_CREAT|O_RDONLY,0777);
	if (fd <= 0)
		{
		ERR_PRINTF2(_L("Error in opening the file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to create a shared memory"));
	ret = write(fd,buf,7);
	if(ret != -1)
		{
		ERR_PRINTF2(_L("shm_open() failed on negative test and errno is %d"),errno);
		closenunlink(fd,filename);
		goto close;
		}
	if ((ret == -1) && (errno == EBADF))
		{
		INFO_PRINTF3(_L("Shm_open() successfully passed on negative test with return = %d and errno = %d"), ret, errno);
		}
	ret = closenunlink(fd,filename);
	if(ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Error in deleting the file"));
		goto close;		
		}
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory21
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: Trying to write to a file with O_TRUNC and O_RDWR
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory21 (  )
	{
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd;
	TBuf8<50> shmname ;
	char buf1[15] = "Sharedmemory", buf2[15], filename[50];
    ret = GetStringFromConfig(ConfigSection(), _L("Shmname"), Shmname );
    shmname.Copy(Shmname) ; 
    char *file = (char *) shmname.Ptr() ;
    if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read input file name") );
     	goto close;
		}
    
	file[shmname.Length()] = '\0' ;
	strcpy(filename , file) ;
	fd = shm_open(filename,O_CREAT|O_RDWR,0777);
	if (fd <= 0)
		{
		ERR_PRINTF2(_L("Error in opening the file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to create a shared memory"));
	ret =  close(fd);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in closing the file and errno is %d"),errno);
		goto close;		
		}
	fd = shm_open(filename,O_TRUNC|O_RDWR,0777);
	if (fd <= 0)
		{
		ERR_PRINTF2(_L("Error in opening the file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to open a shared memory with an O_TRUNC flag"));
	ret = write(fd,buf1,12);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Error in writing into a file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to write in a shared memory"));
	ret =  close(fd);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in closing the file and errno is %d"),errno);
		goto close;		
		}
	fd = shm_open(filename,O_RDWR,0777);
	if (fd <= 0)
		{
		ERR_PRINTF2(_L("Error in opening the file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to open a shared memory with an O_TRUNC flag"));
	ret = read(fd,buf2,12);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("error in reading from a file and errno is %d"),errno);
		goto close;
		}
	ret = strncmp(buf1,buf2,12);
	if (ret != 0)
		{
		ERR_PRINTF1(_L("Input and output buffer are not same"));
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to read the contents of shared memory"));
	ret = closenunlink(fd,filename);
	if(ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Error in deleting the file"));
		goto close;		
		}
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory22
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: Trying to write to a file with only O_TRUNC flag set
//				write() should fail with EBADF
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory22 (  )
	{
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd;
	TBuf8<50> shmname ;
	char buf1[15] = "Sharedmemory", buf2[12] = "symbianpips", filename[50];
    ret = GetStringFromConfig(ConfigSection(), _L("Shmname"), Shmname );
    shmname.Copy(Shmname) ; 
	char *file = (char *) shmname.Ptr() ;
    if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read input file name") );
     	goto close;
		}
	file[shmname.Length()] = '\0' ;
	strcpy(filename , file) ;
	fd = shm_open(filename,O_CREAT|O_RDWR,0777);
	if (fd <= 0)
		{
		ERR_PRINTF2(_L("Error in creating shared memory and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to create a shared memory"));
	ret = write(fd,buf1,12);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Error in writing into shared memory and errno is %d"),errno);
		goto close;
		}
	ret =  close(fd);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in closing shared memory and errno is %d"),errno);
		goto close;		
		}
	INFO_PRINTF1(_L("Successfully able to write to shared memory"));
	fd = shm_open(filename,O_TRUNC,0777);
	if (fd < 0)
		{
		ERR_PRINTF2(_L("Error in opening shared memory with O_TRUNC and errno is %d"),errno);
		close(fd);
		shm_unlink(filename);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to open a shared memory with O_TRUNC"));
	ret = write(fd,buf2,12);
	if ((ret != -1) && (errno != EBADF))
		{
		ERR_PRINTF2(_L("Error in writing into shared memory and errno is %d"),errno);
		ERR_PRINTF1(_L("shm_open() negative test failed"));
		goto close;
		}
	INFO_PRINTF1(_L("shm_open() negative test successful"));
	ret =  close(fd);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in closing shared memory and errno is %d"),errno);
		goto close;		
		}
	ret = shm_unlink(filename);
	if(ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Error in removing shared memory"));
		goto close;		
		}
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory23
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: Trying to open a shared memory using O_CREAT|O_EXCL|O_TRUNC|O_RDWR flag
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory23 (  )
	{
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd;
	TBuf8<50> shmname ;
	char filename[50];
    ret = GetStringFromConfig(ConfigSection(), _L("Shmname"), Shmname );
    shmname.Copy(Shmname) ; 
	char *file = (char *) shmname.Ptr() ;
    if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read input file name") );
     	goto close;
		}
	file[shmname.Length()] = '\0' ;
	strcpy(filename , file) ;
	fd = shm_open(filename,O_CREAT|O_EXCL|O_TRUNC|O_RDWR,0777);
	if (fd <= 0)
		{
		ERR_PRINTF2(_L("Error in opening the file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to create a shared memory"));
	ret =  close(fd);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in closing the file and errno is %d"),errno);
		goto close;		
		}
	fd = shm_open(filename,O_CREAT|O_EXCL|O_TRUNC,0777);
	if ((fd != -1) || (errno != EEXIST))
		{
		ERR_PRINTF2(_L("shm_open() failed on negative test and errno is %d\n"),errno);
		close(fd);
		shm_unlink(filename);
		goto close;
		}
	INFO_PRINTF1(_L("shm_open() is success on negative test"));
	ret = shm_unlink(filename);
	if(ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Error in deleting the file"));
		goto close;		
		}
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory24
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: To seek the contents of shared memory using lseek() using SEEK_SET
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory24 (  )
	{	
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd;
	TBuf8<50> shmname ;
	char buf1[15] = "Sharedmemory";
	char buf2[15], buf3[15], filename[50];
    ret = GetStringFromConfig(ConfigSection(), _L("Shmname"), Shmname );
    shmname.Copy(Shmname) ; 
	char *file = (char *) shmname.Ptr() ;
    if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read input file name") );
     	goto close;
		}
	file[shmname.Length()] = '\0' ;
	strcpy(filename , file) ;
	fd = shm_open(filename,O_CREAT|O_RDWR,0777);
	INFO_PRINTF2(_L("shm_open() returns = %d"),fd);
	if (fd <=0 )
		{
		INFO_PRINTF2(_L("error in shm_open and errno is %d"),errno);
		goto close;
		}
	ret = write(fd,buf1,7);
	INFO_PRINTF2(_L("write returns = %d"),ret);
	if(ret == -1)
		{
		INFO_PRINTF2(_L("error in writing into a file and errno is %d"),errno);
		close(fd);
		shm_unlink(filename);
		goto close;
		}
	ret = close(fd);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in closing the file and errno is %d"),errno);		
		goto close;		
		}
	fd = shm_open(filename,O_RDWR,0777);
	INFO_PRINTF2(_L("shm_open() returns = %d "),fd);
	if (fd <=0 )
		{
		INFO_PRINTF2(_L("error in shm_open and errno is %d"),errno);
		goto close;
		}
	ret = read(fd,buf2,10);
	INFO_PRINTF2(_L("read returns = %d"),ret);
	if(ret < 0)
		{
		INFO_PRINTF2(_L("not able to read from a shmem and  errno is %d"),errno);
		close(fd);
		shm_unlink(filename);
		goto close;
		}
	ret = lseek(fd,0,SEEK_SET);
	INFO_PRINTF2(_L("lseek returns = %d"),ret);
	if(ret == -1)
		{
		INFO_PRINTF2(_L("Failed to move the offset using lseek() and  errno is %d"),errno);
		close(fd);
		shm_unlink(filename);
		goto close;		
		}
	ret = read(fd,buf3,10);
	INFO_PRINTF2(_L("read returns = %d"),ret);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("error in reading from a file and errno is %d"),errno);
		close(fd);
		shm_unlink(filename);
		goto close;
		}
	ret = strncmp(buf3,buf2,7);
	if (ret != 0)
		{
		ERR_PRINTF1(_L("Input and output buffer are not same"));
	ret = closenunlink(fd,filename);
	if(ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Error in deleting the file"));
		goto close;		
		}
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to read the contents of a shared memory using SEEK_SET"));
	ret = closenunlink(fd,filename);
	if(ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Error in deleting the file"));
		goto close;		
		}
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory25
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: Description: To seek the contents of shared memory using lseek() using SEEK_CUR
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory25 (  )
	{	
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd;
	TBuf8<50> shmname ;
	char buf1[15] = "symbian", buf2[15], buf3[15], filename[50];
    ret = GetStringFromConfig(ConfigSection(), _L("Shmname"), Shmname );
    shmname.Copy(Shmname) ; 
	char *file = (char *) shmname.Ptr() ;
	if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read input file name") );
     	goto close;
		}
	file[shmname.Length()] = '\0' ;
	strcpy(filename , file) ;
	fd = shm_open(filename,O_CREAT|O_RDWR,0777);
	INFO_PRINTF2(_L("shm_open() returns = %d"),fd);
	if (fd <= 0 )
		{
		INFO_PRINTF2(_L("error in shm_open and errno is %d"),errno);
		goto close;
		}
	ret = write(fd,buf1,7);
	INFO_PRINTF2(_L("write() returns = %d"),ret);
	if(ret == -1)
		{
		INFO_PRINTF2(_L("error in writing into a file and errno is %d"),errno);
		goto close;
		}
	ret = close(fd);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in closing the file and errno is %d"),errno);
		goto close;		
		}
	fd = shm_open(filename,O_RDWR,0777);
	INFO_PRINTF2(_L("shm_open() returns = %d"),fd);
	if (fd <= 0 )
		{
		INFO_PRINTF2(_L("error in shm_open and errno is %d"),errno);
		goto close;
		}
	ret = read(fd,buf2,7);
	INFO_PRINTF2(_L("read() returns = %d"),ret);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("error in reading from a file and errno is %d"),errno);
		goto close;
		}
	ret = lseek(fd,-7,SEEK_CUR);
	INFO_PRINTF2(_L("lseek() returns = %d"),ret);
	if(ret == -1)
		{
		INFO_PRINTF2(_L("Failed to move the offset using lseek() and  errno is %d"),errno);
		goto close;		
		}
	ret = read(fd,buf3,7);
	INFO_PRINTF2(_L("read() returns = %d"),ret);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("error in reading from a file and errno is %d"),errno);
		goto close;
		}
	ret = strncmp(buf3,buf2,7);
	if (ret != 0)
		{
		ERR_PRINTF1(_L("Input and output buffer are not same"));
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to read the contents of a shared memory using SEEK_CUR"));
	ret = closenunlink(fd,filename);
	if(ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Error in deleting the file"));
		goto close;		
		}
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory26
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: Description: Combined test for file related operations(read,write,lseek) on shared memory
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory26 (  )
	{	
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd;
	TBuf8<50> shmname ;
	char buf[25];
	char filename[50];
    ret = GetStringFromConfig(ConfigSection(), _L("Shmname"), Shmname );
    shmname.Copy(Shmname) ; 
	char *file = (char *) shmname.Ptr() ;
    if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read input file name") );
     	goto close;
		}
	file[shmname.Length()] = '\0' ;
	strcpy(filename , file) ;
	fd = shm_open(filename, O_CREAT|O_RDWR , 0666);
	if (fd <= 0 )
		{
		INFO_PRINTF2(_L("error in shm_open and errno is %d\n"),errno);
		goto close;
		}
	ret = write(fd, "helloworldss", 6);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Error in writing into a file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to write in a shared memory"));
	ret = write(fd, "gimmick", 6);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Error in writing into a file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to write in a shared memory"));
	ret = lseek(fd, 3, SEEK_CUR);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Error in lseek() into a file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able use lseek on a shared memory"));
	ret = write(fd, "biswajeet", 9);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Error in writing into a file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to write in a shared memory"));
	ret = lseek(fd, -18, SEEK_END);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Error in lseek() into a file and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able use lseek on a shared memory"));
	ret = read(fd, (void*)buf, 21);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Error in reading into a file and errno is %d"),errno);
		goto close;
		}
	ret = strncmp(buf, "gimmic", 6);
	if (ret != 0)
		{
		ERR_PRINTF1(_L("Input and output buffer are not same"));
		goto close;
		}
	ret = closenunlink(fd,filename);
	if(ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Error in deleting the file"));
		goto close;		
		}
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory27
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: To perform lseek operation on an invalid shared memory 
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory27 ( )
	{ 
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd;
	TBuf8<50> shmname ;
	char filename[50];
	ret = GetStringFromConfig(ConfigSection(), _L("Shmname"), Shmname );
	shmname.Copy(Shmname) ; 
	char *file = (char *) shmname.Ptr() ;
	if(ret == 0) 
		{
		ERR_PRINTF1(_L("Failed to read input file name") );
		goto close;
		}
	file[shmname.Length()] = '\0' ;
	strcpy(filename , file) ;
	fd = shm_open(filename, O_CREAT|O_RDWR , 0666);
	if (fd <= 0 )
		{
		ERR_PRINTF2(_L("error in shm_open and errno is %d\n"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("shm_open() successful"));
	ret = close(fd);
	if(ret != 0)
		{
		ERR_PRINTF1(_L("Error in closing fd"));
		goto close; 
		}
	INFO_PRINTF1(_L("close() successful"));
	ret = lseek(fd, 0, SEEK_SET);
	if((ret != -1) || (errno != EBADF))
		{
		ERR_PRINTF2(_L("lseek() failed on negative test for a shared memory and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("lseek() successfully returned EBADF on negative test"));
	ret = shm_unlink(filename);
	if(ret != 0)
		{
		ERR_PRINTF1(_L("shm_unlink() unsuccessful"));
		goto close; 
		}
	INFO_PRINTF1(_L("shm_unlink() successful"));
	ret1 = KErrNone;

	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory28
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: To perform lseek operation on shared memory when whence option is an invalid value 
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory28 ( )
	{ 
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd;
	TBuf8<50> shmname ;
	char filename[50];
	ret = GetStringFromConfig(ConfigSection(), _L("Shmname"), Shmname );
	shmname.Copy(Shmname) ; 
	char *file = (char *) shmname.Ptr() ;
	if(ret == 0) 
		{
		ERR_PRINTF1(_L("Failed to read input file name") );
		goto close;
		}
	file[shmname.Length()] = '\0' ;
	strcpy(filename , file) ;
	fd = shm_open(filename, O_CREAT|O_RDWR , 0666);
	if (fd <= 0 )
		{
		INFO_PRINTF2(_L("error in shm_open and errno is %d\n"),errno);
		goto close;
		}
	ret = lseek(fd, 0, 6);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF2(_L("lseek() failed on negative test for a shared memory and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("lseek() successfully returned EINVAL on negative test"));
	ret = closenunlink(fd,filename);
	if(ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Error in deleting the file"));
		goto close; 
		}
	ret1 = KErrNone;

	close:
	return ret1;
	}
	
// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory29
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), shm_unlink()
// Description: To open a shared memory using O_RDONLY flag and then try to write 
//				into it. write() should fail with EBADF 
// -----------------------------------------------------------------------------

TInt CTestsharedmemory::Testsharedmemory29 ( )
	{ 
	TPtrC Shmname;
	int ret, ret1 = KErrGeneral, fd;
	TBuf8<50> shmname ;
	char buf1[15] = "Sharedmemory", buf2[12] = "symbianpips", filename[50], buf3[12];
	ret = GetStringFromConfig(ConfigSection(), _L("Shmname"), Shmname );
	shmname.Copy(Shmname) ; 
	char *file = (char *) shmname.Ptr() ;
	if(ret == 0) 
		{
		ERR_PRINTF1(_L("Failed to read input file name") );
		goto close;
		}
	file[shmname.Length()] = '\0' ;
	strcpy(filename , file) ;
	fd = shm_open(filename,O_CREAT|O_RDWR,0777);
	if (fd <= 0)
		{
		ERR_PRINTF2(_L("Error in creating shared memory and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to create a shared memory"));
	ret = write(fd,buf1,12);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Error in writing into shared memory and errno is %d"),errno);
		goto close;
		}
	ret =  close(fd);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in closing shared memory and errno is %d"),errno);
		goto close;		
		}
	INFO_PRINTF1(_L("Successfully able to write to shared memory"));
	fd = shm_open(filename,O_RDONLY,0777);
	if (fd < 0)
		{
		ERR_PRINTF2(_L("Error in opening shared memory with O_RDONLY and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to open a shared memory with O_RDONLY"));
	ret = read(fd,buf3,12);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Error in reading from shared memory and errno is %d"),errno);
		goto close;
		}
	if (strncmp(buf1, buf2, 12) != 0)
		{
		ERR_PRINTF1(_L("Error in reading from shared memory"));
		}
	INFO_PRINTF1(_L("Successfully able to read from a shared memory"));
	ret = write(fd,buf2,12);
	if ((ret != -1) && (errno != EBADF))
		{
		ERR_PRINTF2(_L("Error in writing into shared memory and errno is %d"),errno);
		ERR_PRINTF1(_L("shm_open() negative test failed"));
		goto close;
		}
	INFO_PRINTF1(_L("shm_open() negative test successful"));
	ret =  close(fd);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in closing shared memory and errno is %d"),errno);
		goto close;		
		}
	ret = shm_unlink(filename);
	if(ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Error in removing shared memory"));
		goto close;		
		}
	ret1 = KErrNone;

	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsharedmemory::Testsharedmemory30
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: shm_open(), read(), write(), lseek()
// Description: negative test case for shm_open(), read(), write() , lseek(), fcntl()
// Purpose : Coverage
// -----------------------------------------------------------------------------
TInt CTestsharedmemory::Testsharedmemory30 (  )
	{	
	int ret, fd;
	char buf1[15] = "helloworldss", buf2[21];
	char filename[50] = "page";
	INFO_PRINTF1(_L("negative test case for coverage"));
	fd = shm_open(filename,O_CREAT|O_RDWR,0666);
	INFO_PRINTF2(_L("shm_open() returns = %d"),fd);
	if (fd <=0 )
		{
		ERR_PRINTF2(_L("error in shm_open and errno is %d"),errno);
		}
	ret = write(fd,buf1,6);
	INFO_PRINTF2(_L("write returns = %d"),ret);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("error in writing into a file and errno is %d"),errno);
		}
	ret = write(fd,"gimmick",6);
	INFO_PRINTF2(_L("write returns = %d"),ret);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("error in writing into a file and errno is %d"),errno);
		}
	ret = lseek(fd,-30,SEEK_END);
	INFO_PRINTF2(_L("lseek returns = %d"),ret);
	if(ret == -1)
		{
		INFO_PRINTF2(_L("Failed to move the offset using lseek() and  errno is %d"),errno);
		}
	ret = close(fd);
	INFO_PRINTF2(_L("close returns = %d"),ret);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Failed to close shared memory object using close() and  errno is %d"),errno);
		}
	fd = shm_open(filename,O_RDWR,0666);
	INFO_PRINTF2(_L("shm_open() returns = %d"),fd);
	if (fd <=0 )
		{
		ERR_PRINTF2(_L("error in shm_open and errno is %d"),errno);
		}
	ret = read(fd,buf2,6);
	INFO_PRINTF2(_L("read returns = %d"),ret);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("error in reading from shared memory and errno is %d"),errno);
		}
	ret = fcntl(fd, F_GETFD);
	INFO_PRINTF2(_L("fcntl() with cmd F_GETFD returns = %d"),ret);
	if(ret < 0)
		{
		ERR_PRINTF1(_L("fcntl() with cmd F_GETFD failed"));
		}
	ret = fcntl(fd, F_SETFL, O_NONBLOCK);
	INFO_PRINTF2(_L("fcntl() with cmd F_GETFD and arg O_NONBLOCK returns = %d"),ret);
	if(ret == -1)
		{
		ERR_PRINTF1(_L("fcntl() with cmd F_GETFD and arg O_NONBLOCK failed"));
		}
	ret = fcntl(fd, F_SETFD, 1);
	INFO_PRINTF2(_L("fcntl() with cmd F_SETFD and arg 1 returns = %d"),ret);
	if(ret == -1)
		{
		ERR_PRINTF1(_L("fcntl() with cmd F_SETFD and arg 1 failed"));
		}
	ret = fcntl(fd, F_GETFD);
	INFO_PRINTF2(_L("fcntl() with cmd F_GETFD returns = %d"),ret);
	if(ret > 0)
		{
		ERR_PRINTF1(_L("fcntl() with cmd F_GETFD failed"));
		}
	ret = fcntl(fd, F_SETFD, 0);
	INFO_PRINTF2(_L("fcntl() with cmd F_SETFD and arg 0 returns = %d"),ret);
	if(ret == -1)
		{
		ERR_PRINTF1(_L("fcntl() with cmd F_SETFD and arg 0 failed"));
		}
	ret = fcntl(fd, F_SETFD, 2);
	INFO_PRINTF2(_L("fcntl() with cmd F_SETFD and arg 2 returns = %d"),ret);
	if(ret == -1)
		{
		ERR_PRINTF1(_L("fcntl() with cmd F_SETFD and arg 2 failed"));
		}
	ret = fcntl(fd, F_GETLK);
	INFO_PRINTF2(_L("fcntl() with cmd F_GETLK returns = %d"),ret);
	if(ret != KErrNotSupported)
		{
		ERR_PRINTF1(_L("fcntl() with cmd F_GETLK failed"));
		}
	ret = close(fd);
	INFO_PRINTF2(_L("close returns = %d"),ret);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Failed to close shared memory object using close() and  errno is %d"),errno);
		}
	ret = shm_unlink(filename);
	INFO_PRINTF2(_L("shm_unlink returns = %d"),ret);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Failed to remove shared memory using shm_unlink() and  errno is %d"),errno);
		}
	return KErrNone;
	}
//End of a file
