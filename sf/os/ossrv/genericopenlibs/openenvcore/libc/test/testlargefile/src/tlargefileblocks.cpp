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
//  Name        : tlargefileblocks.cpp

#include "tlargefile.h"
#include "filetransfer.h"

//Directory under which largefile is created
#ifdef __ARMCC__
#define LARGEFILEDIR "E:\\largefile\\"
#else
#define LARGEFILEDIR "C:\\largefile\\"
#endif

#define LARGEFILESIZE 0x9502F900LL	//File size of 2.5GB. All file operations to be done beyond regular file size.
#define REGFILESIZE 5000	//Regular file size.
#define FILENAMELEN 100	//Maximum file path name length
#define BUFFERLEN 50	//Data buffer length
#define MMAP_PAGE_SIZE		4096	//offset for mmap should be page aligned
#define MAX_REG_FILE_SIZE	0x7FFFFFFFLL // 2 GB - 1

// To create a largefile using selected api based on the Enum input.
FILE* CTestLargefile::CreateLargeFile(char * aLargefilename, int aEnumtocreatelargefile)
	{
	FILE* fp = NULL;
	int fd = -1;
	switch(aEnumtocreatelargefile)
		{
		case ECreate:
			{
			INFO_PRINTF1(_L("Test on creat64() api"));
			unlink(aLargefilename);
			fd = creat64(aLargefilename,S_IRWXU);
			if (fd < 0)
				{
				ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
				return NULL;
				}
			fp = fdopen(fd,"w");
			if (!fp)
				{
				ERR_PRINTF2(_L("Failed in fdopen and errno is %d"),errno);
				close(fd);
				return NULL;		
				}
			INFO_PRINTF1(_L("Successfully able to create a large file"));
			break;
			}
		case EOpen:
			{
			INFO_PRINTF1(_L("Test on open64() api"));
			unlink(aLargefilename);
			fd = open64(aLargefilename,O_RDWR|O_CREAT, S_IRWXU);
			if (fd < 0)
				{
				ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
				return NULL;
				}
			fp = fdopen(fd,"w+");
			if (!fp)
				{
				ERR_PRINTF2(_L("Failed in fdopen and errno is %d"),errno);
				close(fd);
				return NULL;		
				}
			INFO_PRINTF1(_L("Successfully able to create a large file"));
			break;
			}
		case EMkstemp:
			{
			INFO_PRINTF1(_L("Test on mkstemp64() api"));
			unlink(aLargefilename);
			fd = mkstemp64(aLargefilename);
			if (fd < 0)
				{
				ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
				return NULL;
				}
			fp = fdopen(fd,"w+");
			if (!fp)
				{
				ERR_PRINTF2(_L("Failed in fdopen and errno is %d"),errno);
				close(fd);
				return NULL;			
				}
			INFO_PRINTF1(_L("Successfully able to create a large file"));
			break;
			}
		case EFopen:
			{
			INFO_PRINTF1(_L("Test on fopen64() api"));
			unlink(aLargefilename);
			fp = fopen64(aLargefilename,"w+");
			if (!fp)
				{
				ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
				return NULL;
				}
			INFO_PRINTF1(_L("Successfully able to create a large file"));
			break;
			}
		case ETmpfile:
			{
			INFO_PRINTF1(_L("Test on tmpfile64() api"));
			fp = tmpfile64();
			if (!fp)
				{
				ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
				return NULL;
				}
			INFO_PRINTF1(_L("Successfully able to create a large file"));
			break;
			}
		case EWcreate:
			{
			INFO_PRINTF1(_L("Test on wcreat64() api"));
			wchar_t wsfilename[FILENAMELEN];
			unlink(aLargefilename);
			mbstowcs(wsfilename,aLargefilename,FILENAMELEN);
			fd = wcreat64(wsfilename,S_IRWXU);
			if (fd < 0)
				{
				ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
				return NULL;
				}
			fp = fdopen(fd,"w");
			if (!fp)
				{
				ERR_PRINTF2(_L("Failed in fdopen and errno is %d"),errno);
				close(fd);
				return NULL;		
				}
			INFO_PRINTF1(_L("Successfully able to create a large file"));
			break;
			}
		case EWopen:
			{
			INFO_PRINTF1(_L("Test on wopen64() api"));
			wchar_t wsfilename[FILENAMELEN];
			unlink(aLargefilename);
			mbstowcs(wsfilename,aLargefilename,FILENAMELEN);
			fd = wopen64(wsfilename,O_RDWR|O_CREAT, S_IRWXU);
			if (fd < 0)
				{
				ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
				return NULL;
				}
			fp = fdopen(fd,"w+");
			if (!fp)
				{
				ERR_PRINTF2(_L("Failed in fdopen and errno is %d"),errno);
				close(fd);
				return NULL;		
				}
			INFO_PRINTF1(_L("Successfully able to create a large file"));
			break;
			}
		case EWfopen:
			{
			INFO_PRINTF1(_L("Test on wfopen64() api"));
			wchar_t wsfilename[FILENAMELEN];
			unlink(aLargefilename);
			mbstowcs(wsfilename,aLargefilename,FILENAMELEN);
			fp = wfopen64(wsfilename,L"w+");
			if (!fp)
				{
				ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
				return NULL;
				}
			INFO_PRINTF1(_L("Successfully able to create a large file"));
			break;
			}
		case EOpenwithO_largefile:
			{
			INFO_PRINTF1(_L("Test on open() api with O_LARGEFILE flag"));
			unlink(aLargefilename);
			fd = open(aLargefilename,O_RDWR|O_CREAT|O_LARGEFILE, S_IRWXU);
			if (fd < 0)
				{
				ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
				return NULL;
				}
			fp = fdopen(fd,"w+");
			if (!fp)
				{
				ERR_PRINTF2(_L("Failed in fdopen and errno is %d"),errno);
				close(fd);
				return NULL;		
				}
			INFO_PRINTF1(_L("Successfully able to create a large file"));
			break;
			}
		default:
			{
			ERR_PRINTF1(_L("Invalid switch case input"));
			return NULL;
			}
		}
	return fp;
	}

//Function to read a large filename from ini and creating a largefile directory 
TInt CTestLargefile::GetStringAndCreateDir(char *aLargefile)
	{
	TPtrC filename;
	TBuf8<FILENAMELEN> filename1;
	char *filename2 = NULL;
	int ret = -1;	
	if (!GetStringFromConfig(ConfigSection(), _L("filename"), filename))
		{
		ERR_PRINTF1(_L("Failed to read the large file name to be created")) ;
		return KErrGeneral;
	  	}
	ret = mkdir(LARGEFILEDIR,S_IRWXU);
	if (ret != 0)
		{
		if (errno != EEXIST)
			{
			ERR_PRINTF2(_L("Failed to create a directory and errno is %d"),errno);
			return KErrGeneral;
			}		
		}
	filename1.Copy(filename);
	filename1.ZeroTerminate();
	filename2 = (char*) filename1.Ptr();
	strcpy(aLargefile,LARGEFILEDIR);
	strcat(aLargefile,filename2);
	return KErrNone;
	}

//Function to read a large filename and regular filename from ini and creating a largefile directory 
TInt CTestLargefile::GetFilenamesAndCreateDir(char *aTestdirectory, char *aLargefile, char *aRegularfile)
	{
	TPtrC largefilename, regularfilename, apitestdirectory;
	TBuf8<FILENAMELEN> largefile1, regularfile1, testdirectory1;
	char *regularfile2, *largefile2, *testdirectory2;
	int ret;
	if (!GetStringFromConfig(ConfigSection(), _L("apitestdirectory"), apitestdirectory))
		{
		ERR_PRINTF1(_L("Failed to read the large file name to be created")) ;
		return KErrGeneral;
	  	}
	if (!GetStringFromConfig(ConfigSection(), _L("largefilename"), largefilename))
		{
		ERR_PRINTF1(_L("Failed to read the large file name to be created")) ;
		return KErrGeneral;
	  	}
	if (!GetStringFromConfig(ConfigSection(), _L("regularfilename"), regularfilename))
		{
		ERR_PRINTF1(_L("Failed to read the regular file name to be created")) ;
		return KErrGeneral;
	  	}
	ret = mkdir(LARGEFILEDIR,S_IRWXU);
	if (ret != 0)
		{
		if (errno != EEXIST)
			{
			ERR_PRINTF2(_L("Failed to create a directory and errno is %d"),errno);
			return KErrGeneral;
			}		
		}
	testdirectory1.Copy(apitestdirectory);
	testdirectory1.ZeroTerminate();
	testdirectory2 = (char*) testdirectory1.Ptr();
	strcpy(aTestdirectory,LARGEFILEDIR);
	strcat(aTestdirectory,testdirectory2);
	ret = mkdir(aTestdirectory,S_IRWXU);
	if (ret != 0)
		{
		if (errno != EEXIST)
			{
			ERR_PRINTF2(_L("Failed to create a directory and errno is %d"),errno);
			rmdir(LARGEFILEDIR);
			return KErrGeneral;
			}		
		}
	//for large file
	largefile1.Copy(largefilename);
	largefile1.ZeroTerminate();
	largefile2 = (char*) largefile1.Ptr();
	strcpy(aLargefile,aTestdirectory);
	strcat(aLargefile,largefile2);
	//for regular file
	regularfile1.Copy(regularfilename);
	regularfile1.ZeroTerminate();
	regularfile2 = (char*) regularfile1.Ptr();
	strcpy(aRegularfile,aTestdirectory);
	strcat(aRegularfile,regularfile2);
	return KErrNone;
	}

//ftw() call back. Checks the filesize based on the entrytype.
static int FtwEntryFunc(const char* entry, const struct stat64* statbuf, int entrytype)
	{
	int err = -1;
	switch(entrytype)
		{
		case FTW_F:
			//Check for regular file status
			if (strstr(entry,"regular.txt") != NULL)
				{
				if ((statbuf->st_size) != REGFILESIZE)
					{
					break;		
					}
				err = 0;
				}
			//Check for large file status
			else if (strstr(entry,"large.txt") != NULL)
				{
				if ((statbuf->st_size) != LARGEFILESIZE)
					{
					break;		
					}
				err = 0;
				}
			break;
		case FTW_D:
			{
			err = 0;
			break;
			}			
		default:
			break;
		}
	return err;	
	}

//Function to create large and regular files and truncating them
TInt CTestLargefile::CreateFilesAndTruncate(char *aLargefile, char *aRegularfile)
	{
	off64_t reglen = REGFILESIZE, largelen = LARGEFILESIZE;
	int fd = -1, val = -1;
	// to create a large file
	fd = open64(aLargefile,O_RDWR|O_CREAT, S_IRWXU);
	if (fd < 0)
		{
		ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
		return KErrGeneral;
		}
	close(fd);
	val = truncate64(aLargefile,largelen);
	if (val != 0)
		{
		ERR_PRINTF2(_L("Error in truncating the file and errno is set to %d"),errno);
		return KErrGeneral;
		}
	// to create a regular file
	fd = open(aRegularfile, O_WRONLY|O_CREAT, S_IRWXU);
	if (fd < 0)
		{
		ERR_PRINTF2(_L("Failed to create a regular file and errno is %d"),errno);
		return KErrGeneral;
		}
	close(fd);
	val = truncate(aRegularfile,reglen);
	if (val != 0)
		{
		ERR_PRINTF2(_L("Error in truncating the file and errno is set to %d"),errno);
		return KErrGeneral;
		}
	return KErrNone;
	}

//Helper function to perform clean up of a single file created
TInt CTestLargefile::CleanUpSingleFile(FILE *fp, char *aUnlinkFile)
	{
	int val = -1;
	if (fp)
		{
		fclose(fp);
		}
	val = unlink(aUnlinkFile);
	if (val != 0)
		{
		if (errno != ENOENT)
			{
			ERR_PRINTF2(_L("Failed to unlink the file and errno is %d"),errno);
			return KErrGeneral;
			}		
		}	
	rmdir(LARGEFILEDIR);
	return KErrNone;
	}

//Helper function to perform clean up of two files created
TInt CTestLargefile::CleanUpDoubleFile(char *aLargeFile, char *aRegularFile, char *aTestdir)
	{
	TInt val;
	val = unlink(aLargeFile);
	if (val != 0)
		{
		if (errno != ENOENT)
			{
			ERR_PRINTF2(_L("Failed to unlink the large file and errno is %d"),errno);
			return KErrGeneral;
			}		
		}	
	val = unlink(aRegularFile);
	if (val != 0)
		{
		if (errno != ENOENT)
			{
			ERR_PRINTF2(_L("Failed to unlink the regular file and errno is %d"),errno);
			return KErrGeneral;
			}		
		}	
	rmdir(aTestdir);
	rmdir(LARGEFILEDIR);	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestlargefile::TestLargeFileTruncate
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestLargeFileTruncate(  )
    {
	int val1 = -1, val2 = -1, val3 = -1, val4 = -1, val5 = -1, apiselectval;
	off64_t length = LARGEFILESIZE;
	char largefile[FILENAMELEN];
	FILE *fp = NULL;
	if(!GetIntFromConfig(ConfigSection(), _L("apiselectval"), apiselectval))
		{
		ERR_PRINTF1(_L("Failed to read the switch case input for creation of largefile")) ;
		return TestStepResult();
		}
	val1 = GetStringAndCreateDir(largefile);
	if (val1 != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetStringAndCreateDir() function"));
		return TestStepResult();	
		}
	fp = CreateLargeFile(largefile,apiselectval);
	if (!fp)
		{
		ERR_PRINTF2(_L("Error in creating a large file and errno is set to %d"),errno);
		goto end;
		}
	val1 = truncate64(largefile,length);
	if (val1 != 0)
		{
		ERR_PRINTF2(_L("Error in truncating the file and errno is set to %d"),errno);
		goto end;
		}
	INFO_PRINTF1(_L("successfully able to truncate the file"));
	struct stat64 buf, buff, bufx, bufl, buflx;
	memset(&buf, 0, sizeof(struct stat64));
	memset(&buff, 0, sizeof(struct stat64));
	memset(&bufx, 0, sizeof(struct stat64));
	memset(&bufl, 0, sizeof(struct stat64));
	memset(&buflx, 0, sizeof(struct stat64));
	val1 = stat64(largefile,&buf);
	val3 = __xstat64(5,largefile,&bufx);
	val2 = fstat64(fileno(fp),&buff);
	val4 = lstat64(largefile,&bufl);	
	val5 = __lxstat64(5,largefile,&buflx);
	if ((val1 != 0) || (val2 != 0) || (val3 != 0) || (val4 != 0) || (val5 != 0))
		{
		ERR_PRINTF2(_L("Error in stat apis and errno is set to %d"),errno);
		goto end;		
		}
	if ((buf.st_size != length) || (buff.st_size != length) || (bufx.st_size != length) || (bufl.st_size != length) || (buflx.st_size != length))
		{
		ERR_PRINTF1(_L("Error in getting the file status"));
		goto end;		
		}
	if ((apiselectval == EWcreate) || (apiselectval == EWopen) || (apiselectval == EWfopen))
		{
		INFO_PRINTF1(_L("wide character api test"));
		struct stat64 bufw;
		wchar_t wsfilename[FILENAMELEN];
		mbstowcs(wsfilename,largefile,FILENAMELEN);
		memset(&bufw, 0, sizeof(struct stat64));
		val1 = wstat64(wsfilename,&bufw);	
		if ((val1 != 0) || (bufw.st_size != length))
			{
			ERR_PRINTF2(_L("Error in stat apis and errno is set to %d"),errno);
			goto end;		
			}
		}	
	INFO_PRINTF1(_L("successfully able to get the file status"));
	SetTestStepResult(EPass);
	
	end:
	val1 = CleanUpSingleFile(fp, largefile);
	if (val1 != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CleanUpSingleFile()"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
    }

// -----------------------------------------------------------------------------
// CTestlargefile::TestLargeFileFtruncate
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestLargeFileFtruncate(  )
    {
	int val1 = -1, val2 = -1, val3 = -1, val4 = -1, apiselectval, val;
	off64_t length = LARGEFILESIZE;
	char largefile[FILENAMELEN];
	char *gettmpdirenv = NULL;
	FILE *fp = NULL;
	if(!GetIntFromConfig(ConfigSection(), _L("apiselectval"), apiselectval))
		{
		ERR_PRINTF1(_L("Failed to read the switch case input for creation of largefile")) ;
		return TestStepResult();
		}
	val = GetStringAndCreateDir(largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetStringAndCreateDir() function"));
		return TestStepResult();	
		}
	if (apiselectval == ETmpfile)
		{
		gettmpdirenv = getenv("TMPDIR");
		if (!gettmpdirenv)
			{
			ERR_PRINTF1(_L("Error in  getenv() function"));
			goto end;			
			}		
		val = setenv("TMPDIR", LARGEFILEDIR, 1);
		if (val != 0)
			{
			ERR_PRINTF1(_L("Error in  setenv() function"));
			goto end;
			}
		}
	fp = CreateLargeFile(largefile, apiselectval);
	if (!fp)
		{
		ERR_PRINTF2(_L("Error in creating a large file and errno is set to %d\n"),errno);
		goto end;
		}
	val1 = ftruncate64(fileno(fp),length);
	if (val1 != 0)
		{
		ERR_PRINTF2(_L("Error in truncating the file and errno is set to %d\n"),errno);
		goto end;
		}
	INFO_PRINTF1(_L("successfully able to truncate the file"));
	//The below operation cannot be performed for tmpfile64() since there is no specific file name for the file created.
	if (apiselectval != ETmpfile)
		{
		struct stat64 buf, bufx, bufl, buflx;
		memset(&buf, 0, sizeof(struct stat64));
		memset(&bufx, 0, sizeof(struct stat64));
		memset(&bufl, 0, sizeof(struct stat64));
		memset(&buflx, 0, sizeof(struct stat64));
		val1 = stat64(largefile,&buf);
		val2 = __xstat64(5,largefile,&bufx);
		val3 = lstat64(largefile,&bufl);	
		val4 = __lxstat64(5,largefile,&buflx);
		if ((val1 != 0) || (val2 != 0) || (val3 != 0) || (val4 != 0))
			{
			ERR_PRINTF2(_L("Error in stat apis and errno is set to %d"),errno);
			goto end;		
			}
		if ((buf.st_size != length) || (bufx.st_size != length))
			{
			ERR_PRINTF1(_L("Error in getting the file status\n"));
			goto end;
			}
		}
	struct stat64 buff;
	memset(&buff, 0, sizeof(struct stat64));
	val3 = fstat64(fileno(fp),&buff);
	if ((val3 != 0) || (buff.st_size != length))
		{
		ERR_PRINTF2(_L("Error in stat apis and errno is set to %d"),errno);
		goto end;		
		}
	if ((apiselectval == EWcreate) || (apiselectval == EWopen) || (apiselectval == EWfopen))
		{
		struct stat64 bufw;
		wchar_t wsfilename[FILENAMELEN];
		mbstowcs(wsfilename,largefile,FILENAMELEN);
		memset(&bufw, 0, sizeof(struct stat64));
		val1 = wstat64(wsfilename,&bufw);	
		if ((val1 != 0) || (bufw.st_size != length))
			{
			ERR_PRINTF2(_L("Error in stat apis and errno is set to %d"),errno);
			goto end;		
			}
		}
	INFO_PRINTF1(_L("successfully able to get the file status"));
	SetTestStepResult(EPass);
	
	end:
	if (gettmpdirenv)
		{
		setenv("TMPDIR", gettmpdirenv, 1);
		}
	val1 = CleanUpSingleFile(fp, largefile);
	if (val1 != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CleanUpSingleFile()"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
    }

// -----------------------------------------------------------------------------
// CTestlargefile::TestLargeFileReadWrite
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestLargeFileReadWrite(  )
    {
	int val = -1, apiselectval;
	off64_t length = MAX_REG_FILE_SIZE, len = -1;
	char largefile[FILENAMELEN], buf[BUFFERLEN] = "toreachabove2GB", buf1[BUFFERLEN] = "readwritetest", buf2[BUFFERLEN];
	fpos64_t pos = 0;
	ssize_t size = 0;
	char *gettmpdirenv = NULL;
	FILE *fp = NULL;
	if(!GetIntFromConfig(ConfigSection(), _L("apiselectval"), apiselectval))
		{
		ERR_PRINTF1(_L("Failed to read the switch case input for creation of largefile")) ;
		return TestStepResult();
		}
	if (apiselectval == ETmpfile)
		{
		gettmpdirenv = getenv("TMPDIR");
		if (!gettmpdirenv)
			{
			ERR_PRINTF1(_L("Error in  getenv() function"));
			goto end;			
			}		
		val = setenv("TMPDIR", LARGEFILEDIR, 1);
		if (val != 0)
			{
			ERR_PRINTF1(_L("Error in  setenv() function"));
			goto end;
			}
		}
	val = GetStringAndCreateDir(largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetStringAndCreateDir() function"));
		return TestStepResult();	
		}
	fp = CreateLargeFile(largefile, apiselectval);
	if (!fp)
		{
		ERR_PRINTF1(_L("Error in  CreateLargeFile() function"));
		goto end;
		}
	len = lseek64(fileno(fp),length,SEEK_SET);
	if (len != length)
		{
		ERR_PRINTF2(_L("Failed to set the position in a large file and errno is %d"),errno);
		goto end;	
		}
	INFO_PRINTF1(_L("successfully able to truncate the file"));
	size = write(fileno(fp),buf,5);
	if (size == -1)
		{
		ERR_PRINTF2(_L("Failed to write to a large file after getting the current position and errno is %d"),errno);
		goto end;	
		}
	val = fgetpos64(fp,&pos);
	if (val != 0)
		{
		ERR_PRINTF2(_L("Failed to get the position in a large file and errno is %d"),errno);
		goto end;	
		}
	size = write(fileno(fp),buf1,5);
	if (size == -1)
		{
		ERR_PRINTF2(_L("Failed to write to a large file after getting the current position and errno is %d"),errno);
		goto end;	
		}	
	val = fsetpos64(fp,&pos);
	if (val != 0)
		{
		ERR_PRINTF2(_L("Failed to set the position in a large file and errno is %d"),errno);
		goto end;	
		}	
	size = read(fileno(fp),buf2,5);
	if (size == -1)
		{
		ERR_PRINTF2(_L("Failed to read from the large file and errno is %d"),errno);
		goto end;	
		}
	val = strncmp(buf1,buf2,5);
	if (val != 0)
		{
		ERR_PRINTF1(_L("Both the data written and read are not same"));
		goto end;	
		}
	INFO_PRINTF1(_L("successfully able to validate get and set position"));
	SetTestStepResult(EPass);
	
	end:
	if (gettmpdirenv)
		{
		setenv("TMPDIR", gettmpdirenv, 1);
		}
	val = CleanUpSingleFile(fp, largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CleanUpSingleFile()"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
    }

// -----------------------------------------------------------------------------
// CTestlargefile::TestLargeFileFseeknTell
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestLargeFileFseeknTell(  )
    {
	int val = -1, apiselectval;
	off64_t length = LARGEFILESIZE, position = -1;
	char largefile[FILENAMELEN];
	char *gettmpdirenv = NULL;
	FILE *fp = NULL;
	if(!GetIntFromConfig(ConfigSection(), _L("apiselectval"), apiselectval))
		{
		ERR_PRINTF1(_L("Failed to read the switch case input for creation of largefile")) ;
		return TestStepResult();
		}
	if (apiselectval == ETmpfile)
		{
		gettmpdirenv = getenv("TMPDIR");
		if (!gettmpdirenv)
			{
			ERR_PRINTF1(_L("Error in  getenv() function"));
			goto end;			
			}		
		val = setenv("TMPDIR", LARGEFILEDIR, 1);
		if (val != 0)
			{
			ERR_PRINTF1(_L("Error in  setenv() function"));
			goto end;
			}
		}
	val = GetStringAndCreateDir(largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetStringAndCreateDir() function"));
		return TestStepResult();	
		}
	fp = CreateLargeFile(largefile, apiselectval);
	if (!fp)
		{
		ERR_PRINTF2(_L("Error in creating the large file and errno is set to %d\n"),errno);
		goto end;
		}
	val = fseeko64(fp,length,SEEK_SET);
	if (val != 0)
		{
		ERR_PRINTF2(_L("Failed to set the position in a large file and errno is %d"),errno);
		goto end;	
		}	
	position = ftello64(fp);
	if (position != length)
		{
		ERR_PRINTF2(_L("Failed to get the position from a large file and errno is %d"),errno);
		goto end;	
		}	
	INFO_PRINTF1(_L("successfully able to validate get and set position"));
	SetTestStepResult(EPass);
	
	end:
	if (gettmpdirenv)
		{
		setenv("TMPDIR", gettmpdirenv, 1);
		}
	val = CleanUpSingleFile(fp, largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CleanUpSingleFile()"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
    }

// -----------------------------------------------------------------------------
// CTestblargefile::TestLargeFileLseeknTell
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestLargeFileLseeknTell(  )
    {
	int val = -1, apiselectval;
	off64_t length = LARGEFILESIZE, position = -1;
	char largefile[FILENAMELEN];
	char *gettmpdirenv = NULL;
	FILE *fp = NULL;
	if(!GetIntFromConfig(ConfigSection(), _L("apiselectval"), apiselectval))
		{
		ERR_PRINTF1(_L("Failed to read the switch case input for creation of largefile")) ;
		return TestStepResult();
		}
	if (apiselectval == ETmpfile)
		{
		gettmpdirenv = getenv("TMPDIR");
		if (!gettmpdirenv)
			{
			ERR_PRINTF1(_L("Error in  getenv() function"));
			goto end;			
			}		
		val = setenv("TMPDIR", LARGEFILEDIR, 1);
		if (val != 0)
			{
			ERR_PRINTF1(_L("Error in  setenv() function"));
			goto end;
			}
		}
	val = GetStringAndCreateDir(largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetStringAndCreateDir() function"));
		return TestStepResult();	
		}
	fp = CreateLargeFile(largefile, apiselectval);
	if (!fp)
		{
		ERR_PRINTF2(_L("Error in creating the large file and errno is set to %d"),errno);
		goto end;
		}
	val = lseek64(fileno(fp),length,SEEK_SET); 
	if (val == -1)
		{
		ERR_PRINTF2(_L("Failed to set the position in a large file and errno is %d"),errno);
		goto end;	
		}	
	position = ftello64(fp);
	if (position != length)
		{
		ERR_PRINTF2(_L("Failed to get the position from a large file and errno is %d"),errno);
		goto end;	
		}	
	INFO_PRINTF1(_L("successfully able to validate get and set position"));
	SetTestStepResult(EPass);
	
	end:
	if (gettmpdirenv)
		{
		setenv("TMPDIR", gettmpdirenv, 1);
		}
	val = CleanUpSingleFile(fp, largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CleanUpSingleFile()"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
    }

// -----------------------------------------------------------------------------
// CTestlargefile::TestCreatenFseeknPos
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestCreatenFseeknPos(  )
    {
	int  val = -1, apiselectval;
	off64_t length = LARGEFILESIZE, position = -1;
	char largefile[FILENAMELEN];
	char *gettmpdirenv = NULL;
	FILE *fp = NULL;
	fpos64_t pos = 0;
	if(!GetIntFromConfig(ConfigSection(), _L("apiselectval"), apiselectval))
		{
		ERR_PRINTF1(_L("Failed to read the switch case input for creation of largefile")) ;
		return TestStepResult();
		}
	if (apiselectval == ETmpfile)
		{
		gettmpdirenv = getenv("TMPDIR");
		if (!gettmpdirenv)
			{
			ERR_PRINTF1(_L("Error in  getenv() function"));
			goto end;			
			}		
		val = setenv("TMPDIR", LARGEFILEDIR, 1);
		if (val != 0)
			{
			ERR_PRINTF1(_L("Error in  setenv() function"));
			goto end;
			}
		}
	val = GetStringAndCreateDir(largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetStringAndCreateDir() function"));
		return TestStepResult();	
		}
	fp = CreateLargeFile(largefile, apiselectval);
	if (!fp)
		{
		ERR_PRINTF2(_L("Error in creating the large file and errno is set to %d"),errno);
		goto end;
		}
	val = fseeko64(fp,length,SEEK_SET);
	if (val != 0)
		{
		ERR_PRINTF2(_L("Failed to set the position in a large file and errno is %d"),errno);
		goto end;	
		}
	val = fgetpos64(fp,&pos);
	if ((val != 0) || (pos != length))
		{
		ERR_PRINTF2(_L("Failed to get the position in a large file and errno is %d"),errno);
		goto end;	
		}
	val = fseeko64(fp,(length+5),SEEK_SET);
	if (val != 0)
		{
		ERR_PRINTF2(_L("Failed to seek the position in a large file and errno is %d"),errno);
		goto end;	
		}
	val = fsetpos64(fp,&pos);
	if (val != 0)
		{
		ERR_PRINTF2(_L("Failed to set the position back in a large file and errno is %d"),errno);
		goto end;	
		}
	position = ftello64(fp);
	if (position != length)
		{
		ERR_PRINTF2(_L("Failed to get the position from a large file and errno is %d"),errno);
		goto end;	
		}	
	INFO_PRINTF1(_L("successfully able to validate get and set position"));
	SetTestStepResult(EPass);
	
	end:
	if (gettmpdirenv)
		{
		setenv("TMPDIR", gettmpdirenv, 1);
		}
	val = CleanUpSingleFile(fp, largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CleanUpSingleFile()"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
    }

// -----------------------------------------------------------------------------
// CTestlargefile::TestCreatenLseeknPos
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestCreatenLseeknPos(  )
    {
	int val = -1, apiselectval;
	off64_t length = LARGEFILESIZE, position = -1;
	char largefile[FILENAMELEN];
	char *gettmpdirenv = NULL;
	FILE *fp = NULL;
	fpos64_t pos = 0;
	if(!GetIntFromConfig(ConfigSection(), _L("apiselectval"), apiselectval))
		{
		ERR_PRINTF1(_L("Failed to read the switch case input for creation of largefile")) ;
		return TestStepResult();
		}
	if (apiselectval == ETmpfile)
		{
		gettmpdirenv = getenv("TMPDIR");
		if (!gettmpdirenv)
			{
			ERR_PRINTF1(_L("Error in  getenv() function"));
			goto end;			
			}		
		val = setenv("TMPDIR", LARGEFILEDIR, 1);
		if (val != 0)
			{
			ERR_PRINTF1(_L("Error in  setenv() function"));
			goto end;
			}
		}
	val = GetStringAndCreateDir(largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetStringAndCreateDir() function"));
		return TestStepResult();	
		}
	fp = CreateLargeFile(largefile, apiselectval);
	if (!fp)
		{
		ERR_PRINTF2(_L("Error in creating the large file and errno is set to %d"),errno);
		goto end;
		}
	val = lseek64(fileno(fp),length,SEEK_SET); 
	if (val == -1)
		{
		ERR_PRINTF2(_L("Failed to seek the position in a large file and errno is %d"),errno);
		goto end;	
		}
	val = fgetpos64(fp,&pos);
	if ((val != 0) || (pos != length))
		{
		ERR_PRINTF2(_L("Failed to get the position in a large file and errno is %d"),errno);
		goto end;	
		}
	val = lseek64(fileno(fp),(length+5),SEEK_SET); 
	if (val == -1)
		{
		ERR_PRINTF2(_L("Failed to set the position in a large file and errno is %d"),errno);
		goto end;	
		}
	val = fsetpos64(fp,&pos);
	if (val != 0)
		{
		ERR_PRINTF2(_L("Failed to get the position in a large file and errno is %d"),errno);
		goto end;	
		}
	position = ftello64(fp);
	if (position != length)
		{
		ERR_PRINTF2(_L("Failed to get the position back in a large file and errno is %d"),errno);
		goto end;	
		}	
	INFO_PRINTF1(_L("successfully able to validate get and set position"));
	SetTestStepResult(EPass);
	
	end:
	if (gettmpdirenv)
		{
		setenv("TMPDIR", gettmpdirenv, 1);
		}
	val = CleanUpSingleFile(fp, largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CleanUpSingleFile()"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
    }

// -----------------------------------------------------------------------------
// CTestlargefile::TestLargeFileFreopen()
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestLargeFileFreopen(  )
	{
	int val = -1, fd = -1, bytes = 10;
	struct stat64 buf;
	off64_t length = LARGEFILESIZE, position;
	char largefile[FILENAMELEN] = "0", reopenfile[FILENAMELEN] = "0";
	char buf2[BUFFERLEN] = "0", buf1[BUFFERLEN] = "writetest";
	FILE *fp = NULL, *frp = NULL;
	ssize_t size;
	memset(&buf, 0, sizeof(struct stat64));
	val = GetStringAndCreateDir(largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetStringAndCreateDir() function"));
		return TestStepResult();	
		}
	strcpy(reopenfile,LARGEFILEDIR);
	strcat(reopenfile,"reopen.txt");
	frp = fopen64(reopenfile,"w+");
	if (!frp)
		{
		ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
		goto end;
		}
	unlink(largefile);
	fp = freopen64(largefile,"w+",frp);
	if (!fp)
		{
		ERR_PRINTF2(_L("error in freopen and errno is %d"),errno);
		goto end;
		}
	val = ftruncate64(fileno(frp),length);
	if (val != 0)
		{
		ERR_PRINTF2(_L("error in truncate and errno is %d"),errno);
		fclose (fp);
		goto end;
		}
	val = stat64(largefile,&buf);
	if ((val != 0) || (buf.st_size != length) )
		{
		ERR_PRINTF2(_L("error in stat and errno is %d"),errno);
		fclose (fp);
		goto end;
		}
	position = lseek64(fileno(frp),length,SEEK_SET);
	if (length != position)
		{
		ERR_PRINTF2(_L("error in lseek and errno is %d"),errno);
		fclose (fp);
		goto end;
		}
	size = write(fileno(frp),buf1,bytes);
	if (size == -1)
		{
		ERR_PRINTF2(_L("Failed to write to a large file after getting the current position and errno is %d"),errno);
		goto end;	
		} 
	fclose (fp); 
	fd = open64(largefile,O_RDWR, S_IRWXU);
	if (fd < 0)
		{
		ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
		goto end;
		}
	position = lseek64(fd,length,SEEK_SET);
	if (length != position)
		{
		ERR_PRINTF2(_L("error in lseek and errno is %d"),errno);
		goto end;
		}
	size = read(fd,buf2,bytes);
	if (size == -1)
		{
		ERR_PRINTF1(_L("Error in reading from a file"));
		goto end;
		}
	val = strncmp(buf2,buf1,bytes);
	if (val != 0)
		{
		ERR_PRINTF1(_L("error in strcmp"));
		goto end;
		}
	INFO_PRINTF1(_L("Successfully able to truncate the file using freopen"));
	SetTestStepResult(EPass);
	
	end:
	if (fd != -1)
		{
		close(fd);
		}
	if (!fp)
		{
		fclose(fp);
		}
	val = unlink(largefile);
	if (val != 0)
		{
		if (errno != ENOENT)
			{
			ERR_PRINTF2(_L("Failed to unlink the file and errno is %d"),errno);
			SetTestStepResult(EFail);
			}		
		}	
	val = unlink(reopenfile);
	if (val != 0)
		{
		if (errno != ENOENT)
			{
			ERR_PRINTF2(_L("Failed to unlink the file and errno is %d"),errno);
			SetTestStepResult(EFail);
			}		
		}	
	rmdir(LARGEFILEDIR);
	return TestStepResult();
	}

// -----------------------------------------------------------------------------
// CTestlargefile::TestLargeFileWfreopen()
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestLargeFileWfreopen(  )
	{
	int val = -1, fd = -1, bytes = 10;
	struct stat64 buf;
	off64_t length = LARGEFILESIZE, position;
	char largefile[FILENAMELEN] = "0", reopenfile[FILENAMELEN] = "0";
	wchar_t wsfilename[FILENAMELEN], wsreopenfile[FILENAMELEN];
	char buf2[BUFFERLEN] = "0", buf1[BUFFERLEN] = "writetest";
	FILE *fp = NULL, *frp = NULL;
	ssize_t size;
	memset(&buf, 0, sizeof(struct stat64));
	val = GetStringAndCreateDir(largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetStringAndCreateDir() function"));
		return TestStepResult();	
		}
	strcpy(reopenfile,LARGEFILEDIR);
	strcat(reopenfile,"reopen.txt");
	mbstowcs(wsreopenfile,reopenfile,FILENAMELEN);
	frp = wfopen64(wsreopenfile,L"w+");
	if (!frp)
		{
		ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
		goto end;
		}
	unlink(largefile);
	mbstowcs(wsfilename,largefile,FILENAMELEN);
	fp = wfreopen64(wsfilename,L"w+",frp);
	if (!fp)
		{
		ERR_PRINTF2(_L("error in freopen and errno is %d"),errno);
		goto end;
		}
	val = ftruncate64(fileno(frp),length);
	if (val != 0)
		{
		ERR_PRINTF2(_L("error in truncate and errno is %d"),errno);
		fclose (fp);
		goto end;
		}
	val = wstat64(wsfilename,&buf);
	if ((val != 0) || (buf.st_size != length) )
		{
		ERR_PRINTF2(_L("error in stat and errno is %d"),errno);
		fclose (fp);
		goto end;
		}
	position = lseek64(fileno(frp),length,SEEK_SET);
	if (length != position)
		{
		ERR_PRINTF2(_L("error in lseek and errno is %d"),errno);
		fclose (fp);
		goto end;
		}
	size = write(fileno(frp),buf1,bytes);
	if (size == -1)
		{
		ERR_PRINTF2(_L("Failed to write to a large file after getting the current position and errno is %d"),errno);
		goto end;	
		} 	
	fclose (fp); 
	fd = wopen64(wsfilename,O_RDWR, S_IRWXU);
	if (fd < 0)
		{
		ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
		goto end;
		}
	position = lseek64(fd,length,SEEK_SET);
	if (length != position)
		{
		ERR_PRINTF2(_L("error in lseek and errno is %d"),errno);
		goto end;
		}
	size = read(fd,buf2,bytes);
	if (size == -1)
		{
		ERR_PRINTF1(_L("Error in reading from a file"));
		goto end;
		}
	val = strncmp(buf2,buf1,bytes);
	if (val != 0)
		{
		ERR_PRINTF1(_L("error in strcmp"));
		goto end;
		}
	INFO_PRINTF1(_L("Successfully able to truncate the file using freopen"));
	SetTestStepResult(EPass);
	
	end:
	if (fd != -1)
		{
		close(fd);
		}
	if (!fp)
		{
		fclose(fp);
		}
	val = unlink(largefile);
	if (val != 0)
		{
		if (errno != ENOENT)
			{
			ERR_PRINTF2(_L("Failed to unlink the file and errno is %d"),errno);
			SetTestStepResult(EFail);
			}		
		}	
	val = unlink(reopenfile);
	if (val != 0)
		{
		if (errno != ENOENT)
			{
			ERR_PRINTF2(_L("Failed to unlink the file and errno is %d"),errno);
			SetTestStepResult(EFail);
			}		
		}	
	rmdir(LARGEFILEDIR);
	return TestStepResult();
	}

// -----------------------------------------------------------------------------
// CTestlargefile::TestLargeFileFtw
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestLargeFileFtw(  )
    {   
	int val = -1;
	char largefile[FILENAMELEN], regularfile[FILENAMELEN], testdirectory[FILENAMELEN];	
	val = GetFilenamesAndCreateDir(testdirectory, largefile, regularfile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetFilenamesAndCreateDir() function"));
		return TestStepResult();	
		}
	val = CreateFilesAndTruncate(largefile, regularfile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CreateFilesAndTruncate() function"));
		goto end;	
		}
	val = ftw64(testdirectory, &FtwEntryFunc, 3);
	if(val != 0)
		{
		ERR_PRINTF2(_L("FTW failed with errno %d"), errno);
		goto end;
		}
	INFO_PRINTF1(_L("Successfully traversed a file tree containing large file"));
	SetTestStepResult(EPass);
	
	end: 
	val = CleanUpDoubleFile(largefile, regularfile, testdirectory);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CleanUpDoubleFile()"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
    }

// -----------------------------------------------------------------------------
// CTestlargefile::TestNonsortScandir
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestNonsortScandir(  )
    {
	struct dirent64 **namelist;
	int val = -1, entrynum = -1;
	char largefile[FILENAMELEN], regularfile[FILENAMELEN], testdirectory[FILENAMELEN];	
	val = GetFilenamesAndCreateDir(testdirectory, largefile, regularfile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetFilenamesAndCreateDir() function"));
		return TestStepResult();	
		}
	val = CreateFilesAndTruncate(largefile, regularfile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CreateFilesAndTruncate() function"));
		return TestStepResult();	
		}
	entrynum = scandir64(testdirectory, &namelist, 0, NULL);
    if (entrynum != 2)
    	{
    	ERR_PRINTF2(_L("Error in scandir() and no. of entries are %d"),entrynum);
    	goto end;
    	}
    INFO_PRINTF1(_L("Directory list"));
    int entries = entrynum;
    while(entries--) 
    	{
    	if (strcmp("large.txt",namelist[entries]->d_name) == 0)
        	{
        	INFO_PRINTF1(_L("large.txt"));
        	}
        else if (strcmp("regular.txt",namelist[entries]->d_name) == 0)
        	{
        	INFO_PRINTF1(_L("regular.txt"));
        	}
        else
        	{
        	ERR_PRINTF1(_L("There are other files also in the directory"));
        	}  	
    	}
    SetTestStepResult(EPass);
    
    end:
    //deallocating the memory 
	if (entrynum > 0)
		{
		while(entrynum--)
			{
			free(namelist[entrynum]);
			}
		free(namelist);
		}
	val = CleanUpDoubleFile(largefile, regularfile, testdirectory);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CleanUpDoubleFile()"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();  
    }

// -----------------------------------------------------------------------------
// CTestlargefile::TestSortScandir
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestSortScandir(  )
    {
	struct dirent64 **namelist;
	int val = -1, entrynum = -1;
	char largefile[FILENAMELEN], regularfile[FILENAMELEN], testdirectory[FILENAMELEN];	
	val = GetFilenamesAndCreateDir(testdirectory, largefile, regularfile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetFilenamesAndCreateDir() function"));
		return TestStepResult();	
		}
	val = CreateFilesAndTruncate(largefile, regularfile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CreateLargenRegularfiles() function"));
		return TestStepResult();	
		}
	entrynum = scandir64(testdirectory, &namelist, 0, alphasort64);
    if (entrynum < 2)
    	{
    	ERR_PRINTF2(_L("Error in scandir() and no. of entries are %d"),entrynum);
    	goto end;
    	}
    INFO_PRINTF1(_L("Directory list"));
    if ((strcmp("large.txt",namelist[0]->d_name) != 0) || (strcmp("regular.txt",namelist[1]->d_name) != 0))
    	{
    	ERR_PRINTF1(_L("There are other files also in the directory"));
    	}      	
	INFO_PRINTF1(_L("Successfully able to retrieve the files which are sorted out"));
    SetTestStepResult(EPass);
    
    end:
    //deallocating the memory 
	if (entrynum > 0)
		{
		while(entrynum--)
			{
			free(namelist[entrynum]);
			}
		free(namelist);
		}
	val = CleanUpDoubleFile(largefile, regularfile, testdirectory);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CleanUpDoubleFile()"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();  
    }

// -----------------------------------------------------------------------------
// CTestlargefile::TestLargeFileSeek()
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestLargeFileSeek(  )
    {
	int fd = -1, val = -1;
	off64_t length = LARGEFILESIZE;
	char largefile[FILENAMELEN], buf1[BUFFERLEN] = "seekwritetest", buf2[BUFFERLEN];
	ssize_t size = 0;
    struct flock64 fl;
    struct stat64 buf;
    memset(&buf, 0, sizeof(struct stat64));
    memset(&fl, 0, sizeof(struct flock64));
	val = GetStringAndCreateDir(largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetStringAndCreateDir() function"));
		return TestStepResult();	
		}
	unlink(largefile);
	fd = open64(largefile,O_RDWR|O_CREAT, S_IRWXU);
	if (fd < 0)
		{
		ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
		goto end;
		}
	INFO_PRINTF1(_L("Successfully able to create a large file"));
	val = ftruncate64(fd,length);
	if (val != 0)
		{
		ERR_PRINTF2(_L("Error in truncating the file and errno is set to %d\n"),errno);
		goto end;
		}
	val = fstat64(fd,&buf);
	if ((val != 0) || (buf.st_size != length))
		{
		ERR_PRINTF2(_L("Error in retrieving length bytes by fstat64 api and errno is set to %d"),errno);
		goto end;		
		}
	val = lseek64(fd,0,SEEK_END); 
	if (val == -1)
		{
		ERR_PRINTF2(_L("Failed to seek the position to EOF and errno is %d"),errno);
		goto end;	
		}	
	size = write(fd,buf1,5);
	if (size == -1)
		{
		ERR_PRINTF2(_L("Failed to write to a large file and errno is %d"),errno);
		goto end;	
		}	
	val = fstat64(fd,&buf);
	if ((val != 0) || (buf.st_size != (length+5)))
		{
		ERR_PRINTF2(_L("Error in retrieving (length+n) bytes by fstat64 api and errno is set to %d"),errno);
		goto end;		
		}
	val = lseek64(fd,(-5),SEEK_CUR); 
	if (val == -1)
		{
		ERR_PRINTF2(_L("Failed to seek n bytes back from current position and errno is %d"),errno);
		goto end;	
		}	
	size = read(fd,buf2,5);
	if (size == -1)
		{
		ERR_PRINTF2(_L("Failed to read from the large file for the first time and errno is %d"),errno);
		goto end;	
		}
	val = strncmp(buf1,buf2,5);
	if (val != 0)
		{
		ERR_PRINTF1(_L("Both the data written and read for the first time are not same"));
		goto end;	
		}
	val = lseek64(fd,length,SEEK_SET); 
	if (val == -1)
		{
		ERR_PRINTF2(_L("Failed to set the position back in a large file and errno is %d"),errno);
		goto end;	
		}	
	memset(buf2,0,sizeof(buf2));
	size = read(fd,buf2,5);
	if (size == -1)
		{
		ERR_PRINTF2(_L("Failed to read from the large file for the second time and errno is %d"),errno);
		goto end;	
		}
	val = strncmp(buf1,buf2,5);
	if (val != 0)
		{
		ERR_PRINTF1(_L("Both the data written and read for the second time are not same"));
		goto end;	
		}
	val = lseek64(fd,(-5),SEEK_END); 
	if (val == -1)
		{
		ERR_PRINTF2(_L("Failed to seek the position n bytes back from EOF and errno is %d"),errno);
		goto end;	
		}	
	memset(buf2,0,sizeof(buf2));
	size = read(fd,buf2,5);
	if (size == -1)
		{
		ERR_PRINTF2(_L("Failed to read from the large file for the second time and errno is %d"),errno);
		goto end;	
		}
	val = strncmp(buf1,buf2,5);
	if (val != 0)
		{
		ERR_PRINTF1(_L("Both the data written and read for the third time are not same"));
		goto end;	
		}
 	INFO_PRINTF1(_L("Successfully performed seek functionalities on a large file"));
	SetTestStepResult(EPass);
	
	end:
	if (fd != -1)
		{
		close(fd);
		}
	val = unlink(largefile);
	if (val != 0)
		{
		if (errno != ENOENT)
			{
			ERR_PRINTF2(_L("Failed to unlink the file and errno is %d"),errno);
			SetTestStepResult(EFail);
			}		
		}	
	rmdir(LARGEFILEDIR);
	return TestStepResult();
    }

// -----------------------------------------------------------------------------
// CTestlargefile::TestLargeFileGlob()
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestLargeFileGlob(  )
    {
	int val = -1, globret = -1;
	char largefile[FILENAMELEN], regularfile[FILENAMELEN], globpattern[FILENAMELEN], testdirectory[FILENAMELEN];	
	val = GetFilenamesAndCreateDir(testdirectory, largefile, regularfile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetFilenamesAndCreateDir() function"));
		return TestStepResult();	
		}
	val = CreateFilesAndTruncate(largefile, regularfile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CreateFilesAndTruncate() function"));
		goto end;	
		}
	glob64_t globbuf;
	strcpy(globpattern,testdirectory);
	strcat(globpattern,"*.txt");
	globret = glob64(globpattern, GLOB_MARK ,NULL ,&globbuf);
    if (globret != 0)
    	{
		ERR_PRINTF2(_L("Error in  glob() api and errno is %d"),errno);
		goto end;	    	
    	}	
    if (globbuf.gl_pathc != 2)
    	{
		ERR_PRINTF1(_L("Error in  returning the total path count"));
		goto end;	
    	}
    int count = globbuf.gl_pathc;
    while(count--)
    	{
    	if (strcmp(globbuf.gl_pathv[count],largefile) == 0)
        	{
        	INFO_PRINTF1(_L("large.txt"));
        	}
        else if (strcmp(globbuf.gl_pathv[count],regularfile) == 0)
        	{
        	INFO_PRINTF1(_L("regular.txt"));
        	}
        else
        	{
        	ERR_PRINTF1(_L("String Mismatch"));
        	goto end;        		
        	}  	
    	}
 	INFO_PRINTF1(_L("Successfully performed glob() function on large file"));
	SetTestStepResult(EPass);
	
	end:
	globfree64(&globbuf);
	val = CleanUpDoubleFile(largefile, regularfile, testdirectory);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CleanUpDoubleFile()"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
    }

// -----------------------------------------------------------------------------
// CTestlargefile::TestLargeFileMmapWrite()
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestLargeFileMmapWrite(  )
    {
	int val = -1, fd = -1;
	off64_t offset = LARGEFILESIZE;  
	char largefile[FILENAMELEN], buf[BUFFERLEN];
	ssize_t size = 0;
	size_t len = 10;
	val = GetStringAndCreateDir(largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetStringAndCreateDir() function"));
		return TestStepResult();	
		}
	fd = open64(largefile,O_RDWR|O_CREAT, S_IRWXU);
	if (fd < 0)
		{
		ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
		goto end;
		}
	val = truncate64(largefile,(offset+20));
	if (val != 0)
		{
		ERR_PRINTF2(_L("Error in truncating the file and errno is set to %d\n"),errno);
		goto end;
		}
	//offset for mmap should be page aligned
	offset = LARGEFILESIZE - LARGEFILESIZE%MMAP_PAGE_SIZE;
	//map 10 bytes above 2GB
	char* mapaddr;
	mapaddr = (char*)mmap64((void*)0, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
    if(mapaddr == MAP_FAILED)
		{
		ERR_PRINTF2(_L("Failed to map the file to a process's address space and errno is %d"),errno);
		goto end;
		}        
	strcpy(mapaddr, "This is a write through mapped memory to a large file");
	val = munmap(mapaddr,len);
	if (val != 0)
		{
		ERR_PRINTF2(_L("Failed to unmap the file to a process's address space and errno is %d"),errno);
		goto end;		
		}
	val = lseek64(fd,offset,SEEK_SET); 
	if (val == -1)
		{
		ERR_PRINTF2(_L("Failed to set the position in a large file and errno is %d"),errno);
		goto end;	
		}	
	size = read(fd,buf,len);
	if (size == -1)
		{
		ERR_PRINTF2(_L("Failed to read from the large file and errno is %d"),errno);
		goto end;	
		}
	val = strncmp(buf,"This is a write through mapped memory to a large file",len);
	if (val != 0)
		{
		ERR_PRINTF1(_L("Both the data written and read are not same"));
		goto end;	
		}
	INFO_PRINTF1(_L("Successfully able to map the file to process's address space"));
	SetTestStepResult(EPass);
	
	end:
	if (fd != -1)
		{
		close(fd);
		}
	val = unlink(largefile);
	if (val != 0)
		{
		if (errno != ENOENT)
			{
			ERR_PRINTF2(_L("Failed to unlink the file and errno is %d"),errno);
			SetTestStepResult(EFail);
			}		
		}	
	rmdir(LARGEFILEDIR);
	return TestStepResult();
    }

// -----------------------------------------------------------------------------
// CTestlargefile::TestLargeFileWriteMmap()
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestLargeFileWriteMmap(  )
    {
	int val = -1, fd = -1;
	off64_t offset = LARGEFILESIZE, position = -1;  
	char largefile[FILENAMELEN], buf1[BUFFERLEN] = "write to a file";
	ssize_t size;
	size_t len = 10;
	val = GetStringAndCreateDir(largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetStringAndCreateDir() function"));
		return TestStepResult();	
		}
	fd = open64(largefile,O_RDWR|O_CREAT, S_IRWXU);
	if (fd < 0)
		{
		ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
		goto end;
		}
	val = truncate64(largefile,(offset+20));
	if (val != 0)
		{
		ERR_PRINTF2(_L("Error in truncating the file and errno is set to %d\n"),errno);
		goto end;
		}
	offset = LARGEFILESIZE - LARGEFILESIZE%MMAP_PAGE_SIZE;
	position = lseek64(fd,offset,SEEK_SET);
	if (position != offset)
		{
		ERR_PRINTF2(_L("Failed to set the position in a large file and errno is %d"),errno);
		goto end;	
		}
	INFO_PRINTF1(_L("successfully able to truncate the file"));
	size = write(fd,buf1,len);
	if (size == -1)
		{
		ERR_PRINTF2(_L("Failed to write to a large file  and errno is %d"),errno);
		goto end;	
		}
	char* mapaddr;
	//map 10 bytes above 2GB
	mapaddr = (char*)mmap64((void*)0, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
    if(mapaddr == MAP_FAILED)
		{
		ERR_PRINTF2(_L("Failed to map the file to a process's address space and errno is %d"),errno);
		goto end;
		}  
    val = strncmp((char*)mapaddr, buf1, len);
    if (val != 0)
    	{
    	ERR_PRINTF1(_L("Both the data written and read are not same"));
    	goto end;	
    	}
	val = munmap(mapaddr,len);
	if (val != 0)
		{
		ERR_PRINTF2(_L("Failed to unmap the file to a process's address space and errno is %d"),errno);
		goto end;		
		}
	INFO_PRINTF1(_L("Successfully able to map the file to process's address space"));
	SetTestStepResult(EPass);
	
	end:
	if (fd != -1)
		{
		close(fd);
		}
	val = unlink(largefile);
	if (val != 0)
		{
		if (errno != ENOENT)
			{
			ERR_PRINTF2(_L("Failed to unlink the file and errno is %d"),errno);
			SetTestStepResult(EFail);
			}		
		}	
	rmdir(LARGEFILEDIR);
	return TestStepResult();
    }

//-----------------------------------------------------------------------------
/*
 * the below test checks the write lock for max file size supported by the file system
 * 
 * */

TVerdict CTestLargefile::TestFcntlWriteLockwithMaxSize()
    {
    int fd = -1,val = -1;
    char largefile[FILENAMELEN], buf1[BUFFERLEN] = "parenttest";
    
    struct flock  flk;
    memset(&flk, 0, sizeof( struct flock ));
    val = GetStringAndCreateDir(largefile);
    if (val != KErrNone)
        {
        ERR_PRINTF1(_L("Error in  GetStringAndCreateDir() function"));
        return TestStepResult();    
        }
    unlink(largefile);
    fd = open(largefile,O_RDWR|O_CREAT);
    if (fd < 0)
    {
    ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
    goto end;
    }
    flk.l_type = F_WRLCK;
    flk.l_whence = SEEK_SET;
    flk.l_start = 0;
    flk.l_len = 0;
    flk.l_pid = getpid();
   
    val = fcntl(fd, F_SETLK, &flk);
    if(val == -1)
        {
        ERR_PRINTF2(_L("Error in locking the file and errno is set to %d"),errno);
        SetTestStepResult(EFail);
        }
    else
        {
            INFO_PRINTF1(_L("Successfully performed lock functionality"));
            SetTestStepResult(EPass);
        }
    end:
    rmdir(LARGEFILEDIR);
    return TestStepResult();
    }
//-----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// CTestlargefile::TestFcntlwithsinglefd()
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestFcntlwithSinglefd(  )
    {
	int fd = -1, val = -1;
	off64_t seek, length = 2100000000LL;//LARGEFILESIZE; 
	char largefile[FILENAMELEN], buf1[BUFFERLEN] = "parenttest";
	ssize_t size;
    struct flock64 fl;
    memset(&fl, 0, sizeof(struct flock64));
	val = GetStringAndCreateDir(largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetStringAndCreateDir() function"));
		return TestStepResult();	
		}
	unlink(largefile);
	fd = open64(largefile,O_RDWR|O_CREAT, S_IRWXU);
	if (fd < 0)
		{
		ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
		goto end;
		}
	INFO_PRINTF1(_L("Successfully able to create a large file"));
	val = ftruncate64(fd,length);
	if (val != 0)
		{
		ERR_PRINTF2(_L("Error in truncating the file and errno is set to %d"),errno);
		goto end;
		}
	INFO_PRINTF1(_L("successfully able to truncate the file"));
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = length;
	val = fcntl(fd , F_SETLK64 , &fl);
	if(val == -1)
		{
		ERR_PRINTF2(_L("Error in locking the file and errno is set to %d"),errno);
		goto end;
		}
	seek = lseek64(fd,(length-10),SEEK_SET); 
	if (seek == (off64_t)-1)
		{
		ERR_PRINTF2(_L("Failed to set the position in a large file and errno is %d"),errno);
		goto end;	
		}
	size = write(fd, buf1, 2);
	if (size == -1)			
		{
		ERR_PRINTF1(_L("Has successfully written the data to a file though it is locked"));
		goto end;	
		}
 	INFO_PRINTF1(_L("Successfully performed lock functionality"));
	SetTestStepResult(EPass);
	
	end:
	fl.l_type = F_UNLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = length;
	val = fcntl(fd , F_SETLK64, &fl);
	if(val == -1)
		{
		ERR_PRINTF2(_L("Error in unlocking the file and errno is set to %d"),errno);
		SetTestStepResult(EFail);
		}
	if (fd != -1)
		{
		close(fd);
		}
	val = unlink(largefile);
	if (val != 0)
		{
		if (errno != ENOENT)
			{
			ERR_PRINTF2(_L("Failed to unlink the file and errno is %d"),errno);
			SetTestStepResult(EFail);
			}		
		}	
	rmdir(LARGEFILEDIR);
	return TestStepResult();
    }

// -----------------------------------------------------------------------------
// CTestlargefile::TestFcntlwithDoublefds()
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestFcntlwithDoublefds(  )
    {
	int fd1 = -1, fd2 = -1, val = -1;
	off64_t length = LARGEFILESIZE, seek;
	char largefile[FILENAMELEN], buf1[BUFFERLEN] = "parenttest";
	ssize_t size;
    struct flock64 fl_in, fl_out;
    memset(&fl_in, 0, sizeof(struct flock64));
    memset(&fl_out, 0, sizeof(struct flock64));
	val = GetStringAndCreateDir(largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetStringAndCreateDir() function"));
		return TestStepResult();	
		}
	unlink(largefile);
	fd1 = open64(largefile,O_RDWR|O_CREAT, S_IRWXU);
	if (fd1 < 0)
		{
		ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
		goto end;
		}
	INFO_PRINTF1(_L("Successfully able to create a large file"));
	val = ftruncate64(fd1,length);
	if (val != 0)
		{
		ERR_PRINTF2(_L("Error in truncating the file and errno is set to %d\n"),errno);
		goto end;
		}
	INFO_PRINTF1(_L("successfully able to truncate the file"));
	fl_in.l_type = F_WRLCK;
	fl_in.l_whence = SEEK_SET;
	fl_in.l_start = length -15;
	fl_in.l_len = 10;
	val = fcntl(fd1 , F_SETLK64, &fl_in);
	if(val == -1)
		{
		ERR_PRINTF2(_L("Error in locking the file and errno is set to %d"),errno);
		goto end;
		}
	fd2 = open64(largefile, O_RDWR, S_IRWXU);
	if (fd2 < 0)
		{
		ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
		goto end;
		}
	INFO_PRINTF1(_L("Successfully able to create a large file"));
	fl_out.l_type = F_WRLCK;
	fl_out.l_whence = SEEK_SET;
	fl_out.l_start = length -15;
	fl_out.l_len = 10;
	val = fcntl(fd2 , F_GETLK64, &fl_out);
	if(val == -1)
		{
		ERR_PRINTF2(_L("Error in getting the lock of a file and errno is set to %d"),errno);
		goto end;
		}
	if ((fl_out.l_type != fl_in.l_type) || (fl_out.l_start != fl_in.l_start) || (fl_out.l_len != fl_in.l_len))
		{
		ERR_PRINTF1(_L("failed to retrieve the lock status of a file"));
		goto end;
		}
	seek = lseek64(fd2,(length-15),SEEK_SET); 
	if (seek == (off_t)-1)
		{
		ERR_PRINTF2(_L("Failed to set the position in a large file and errno is %d"),errno);
		goto end;	
		}
	size = write(fd2, buf1, 2);
	if ((size != -1) || (errno  != EACCES))
		{
		ERR_PRINTF2(_L("Has successfully written the data to a file though it is locked and errno is %d"),errno);
		goto end;	
		}
 	INFO_PRINTF1(_L("Successfully performed lock operation using two fds"));
	SetTestStepResult(EPass);
	
	end:
	fl_in.l_type = F_UNLCK;
	fl_in.l_whence = SEEK_SET;
	fl_in.l_start = length -15;
	fl_in.l_len = 10;
	val = fcntl(fd1 , F_SETLK64, &fl_in);
	if(val == -1)
		{
		ERR_PRINTF2(_L("Error in unlocking the file and errno is set to %d"),errno);
		SetTestStepResult(EFail);
		}
	if (fd1 != -1)
		{
		close(fd1);
		}
	if (fd2 != -1)
		{
		close(fd2);
		}
	val = unlink(largefile);
	if (val != 0)
		{
		if (errno != ENOENT)
			{
			ERR_PRINTF2(_L("Failed to unlink the file and errno is %d"),errno);
			SetTestStepResult(EFail);
			}		
		}	
	rmdir(LARGEFILEDIR);
	return TestStepResult();
    }

// -----------------------------------------------------------------------------
// CTestlargefile::TestLargeFileReaddir
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestLargeFileReaddir(  )
    {
	struct dirent64 *dp;
	DIR* ret = NULL;
	int val = -1;
	char largefile[FILENAMELEN], regularfile[FILENAMELEN], testdirectory[FILENAMELEN];	
	val = GetFilenamesAndCreateDir(testdirectory, largefile, regularfile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetFilenamesAndCreateDir() function"));
		return TestStepResult();	
		}
	val = CreateFilesAndTruncate(largefile, regularfile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CreateFilesAndTruncate() function"));
		goto end;	
		}
	ret = opendir(testdirectory);
	if (!ret)
		{
		ERR_PRINTF1(_L("Can't open directory"));
		goto end;
		}
	while((dp = readdir64(ret)) != NULL)
		{
    	if (strcmp("large.txt",dp->d_name) == 0)
        	{
        	INFO_PRINTF1(_L("large.txt"));
        	}
        else if (strcmp("regular.txt",dp->d_name) == 0)
        	{
        	INFO_PRINTF1(_L("regular.txt"));
        	}
        else
        	{
        	ERR_PRINTF1(_L("There are other files also in the folder"));  
        	goto end;
        	}  	
		}
	SetTestStepResult(EPass);
	
	end:
	closedir(ret);
	val = CleanUpDoubleFile(largefile, regularfile, testdirectory);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CleanUpDoubleFile()"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
    }

// -----------------------------------------------------------------------------
// CTestlargefile::TestLargeFileAlphasort
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestLargeFileAlphasort(  )
    {
	int val;
	char largefile[FILENAMELEN], regularfile[FILENAMELEN], testdirectory[FILENAMELEN];	
	struct dirent64 *dp;
	DIR* ret = NULL;
	struct dirent64 *dp1 = (struct dirent64*) malloc(20*sizeof(struct dirent64));
	struct dirent64 *dp2 = (struct dirent64*) malloc(20*sizeof(struct dirent64));
	val = GetFilenamesAndCreateDir(testdirectory, largefile, regularfile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetFilenamesAndCreateDir() function"));
		return TestStepResult();	
		}
	val = CreateFilesAndTruncate(largefile, regularfile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CreateFilesAndTruncate() function"));
		goto end;	
		}
	ret = opendir(testdirectory);
	if (!ret)
		{
		ERR_PRINTF1(_L("Can't open directory"));
		goto end;
		}
	//read first entry of LARGEFILEDIR
	dp = readdir64(ret);
	if(!dp)
		{
		ERR_PRINTF1(_L("error in retreiving the dirent structure"));
		goto end;	
		}
	strcpy(dp1->d_name,dp->d_name);
	//read second entry of LARGEFILEDIR
	dp = readdir64(ret);
	if(!dp)
		{
		ERR_PRINTF1(_L("error in retreiving the dirent structure"));
		free((void *)dp1);
		goto end;	
		}
	strcpy(dp2->d_name,dp->d_name);
	int n = alphasort64(&dp1, &dp2);
	if (n < 0)
		{
		INFO_PRINTF1(_L("first entry less than second"));
		if(strcmp(dp->d_name,"regular.txt" ) != 0)
			{
			ERR_PRINTF1(_L("wrongly compared the files"));
			goto end;
			}	
		}
	else if(n == 0) 
		{
		ERR_PRINTF1(_L("Both entries are same"));
		goto end;
		}
	else if(n > 0) 
		{
		INFO_PRINTF1(_L("first entry greater than second enttry"));
		if(strcmp(dp->d_name,"large.txt" ) != 0)
			{
			ERR_PRINTF1(_L("wrongly compared the files"));
			goto end;
			}	
		}
	SetTestStepResult(EPass);
	
	end:
	free((void *)dp1);
	free((void *)dp2);
	closedir(ret);
	val = CleanUpDoubleFile(largefile, regularfile, testdirectory);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CleanUpDoubleFile()"));
		SetTestStepResult(EFail);
		}	
	return TestStepResult();
    }

// -----------------------------------------------------------------------------
// CTestlargefile::TestLargeFileWreaddir
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestLargeFileWreaddir(  )
    {
	struct wdirent64 *dp;
	WDIR* ret = NULL;
	int val = -1;
	char largefile[FILENAMELEN], regularfile[FILENAMELEN], testdirectory[FILENAMELEN];	
	val = GetFilenamesAndCreateDir(testdirectory, largefile, regularfile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetFilenamesAndCreateDir() function"));
		return TestStepResult();	
		}
	val = CreateFilesAndTruncate(largefile, regularfile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CreateFilesAndTruncate() function"));
		goto end;	
		}
	wchar_t wcslargefiledir[FILENAMELEN];
	mbstowcs(wcslargefiledir,testdirectory,FILENAMELEN);
	ret = wopendir(wcslargefiledir);
	if (!ret)
		{
		ERR_PRINTF1(_L("Can't open directory"));
		goto end;
		}
	while((dp = wreaddir64(ret)) != NULL)
		{
    	if (wcscmp(L"large.txt",dp->d_name) == 0)
        	{
        	INFO_PRINTF1(_L("large.txt"));
        	}
        else if (wcscmp(L"regular.txt",dp->d_name) == 0)
        	{
        	INFO_PRINTF1(_L("regular.txt"));
        	}
        else
        	{
        	ERR_PRINTF1(_L("The file read is not same as an input file"));
        	goto end;        		
        	}  	
		}
	SetTestStepResult(EPass);
	
	end:
	wclosedir(ret);
	val = CleanUpDoubleFile(largefile, regularfile, testdirectory);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CleanUpDoubleFile()"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
    }

// -----------------------------------------------------------------------------
// CTestlargefile::TestNegSmallOffsetValues()
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestNegSmallOffsetValues(  )
    {
	int val = -1, negoffset = 0;
	char largefile[FILENAMELEN];
    FILE *fp = NULL;	
    if (!GetIntFromConfig(ConfigSection(), _L("negoffset"), negoffset))
		{
		ERR_PRINTF1(_L("Failed to read the large file name to be created")) ;
		return TestStepResult();	
	  	}
	val = GetStringAndCreateDir(largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetStringAndCreateDir() function"));
		return TestStepResult();	
		}	
	unlink(largefile);
	fp = fopen64(largefile,"w+");
	if (!fp)
		{
		ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
		return TestStepResult();	
		}
	val = truncate64(largefile,(off64_t)negoffset);
	if ((val != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("truncate has failed to return EINVAL"));
		goto end;
		}
	INFO_PRINTF1(_L("truncate64 has successfully returned EINVAL on negative test"));
	val = ftruncate64(fileno(fp),(off64_t)negoffset);
	if ((val != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("ftruncate has failed to return EINVAL"));
		goto end;
		}
	INFO_PRINTF1(_L("ftruncate64 has successfully returned EINVAL on negative test"));
	val = fseeko64(fp,(off64_t)negoffset,SEEK_SET);
	if ((val != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("fseeko has failed to return EINVAL"));
		goto end;	
		}
	INFO_PRINTF1(_L("fseeko64 has successfully returned EINVAL on negative test"));
	val = lseek64(fileno(fp),(off64_t)negoffset,SEEK_SET); 
	if ((val != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("lseek has failed to return EINVAL"));
		goto end;	
		}
	INFO_PRINTF1(_L("lseek64 has successfully returned EINVAL on negative test"));
	SetTestStepResult(EPass);
	
	end: 
	if (fp)
		{
		fclose(fp);
		}
	val = unlink(largefile);
	if (val != 0)
		{
		if (errno != ENOENT)
			{
			ERR_PRINTF2(_L("Failed to unlink the file and errno is %d"),errno);
			SetTestStepResult(EFail);
			}		
		}	
	rmdir(LARGEFILEDIR);
    return TestStepResult();
    }

// -----------------------------------------------------------------------------
// CTestlargefile::TestNegLargeOffsetValues()
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestNegLargeOffsetValues(  )
    {
	int val = -1;
	char largefile[FILENAMELEN];
    FILE *fp = NULL;
    off64_t length = ULLONG_MAX, position = -1;
	val = GetStringAndCreateDir(largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetStringAndCreateDir() function"));
		return TestStepResult();	
		}	
	unlink(largefile);
	fp = fopen64(largefile,"w+");
	if (!fp)
		{
		ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
		return TestStepResult();	
		}
	val = fseeko64(fp,length,SEEK_SET);
	if ((val != -1) || (errno != EINVAL))
		{
		ERR_PRINTF2(_L("fseeko has failed to return EINVAL and errno is %d"),errno);
		goto end;	
		}
	INFO_PRINTF1(_L("fseeko64 has successfully returned EINVAL on negative test for above large file size"));
	position = lseek64(fileno(fp),length, SEEK_SET); 
	if ((position != (off64_t)-1) || (errno != EINVAL))
		{
		ERR_PRINTF2(_L("lseek has failed to return EINVAL and errno is %d"),errno);
		goto end;	
		}
	INFO_PRINTF1(_L("lseek64 has successfully returned EINVAL on negative test for above large file size"));
	SetTestStepResult(EPass);
	
	end: 
	val = CleanUpSingleFile(fp, largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CleanUpSingleFile()"));
		SetTestStepResult(EFail);
		}
    return TestStepResult();
    }

// -----------------------------------------------------------------------------
// CTestlargefile::TestChildFileTransfer
// -----------------------------------------------------------------------------

TVerdict CTestLargefile::TestChildFileTransfer(  )
    {
	int fd = -1, val = -1, pid = -1, bytes = 5, pid1 = -1, status = -1;
	off64_t length = LARGEFILESIZE, position = -1;
	char largefile[FILENAMELEN], buf1[BUFFERLEN] = "parenttest", buf2[BUFFERLEN], buf3[BUFFERLEN] = "childtest";
	FILE *fp = NULL;
	fpos64_t pos = 0;
	ssize_t size = 0;
    struct flock64 fl;
    memset(&fl, 0, sizeof(struct flock64));
	val = GetStringAndCreateDir(largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  GetStringAndCreateDir() function"));
		return TestStepResult();	
		}
	char **argv = (char **)malloc(4*sizeof(char*));
    argv[0] = (char *)malloc(FILENAMELEN*(sizeof(char)));
    argv[1] = (char *)malloc(5*(sizeof(char)));
    argv[2] = (char *)malloc(5*(sizeof(char)));
	unlink(largefile);
	fd = open64(largefile,O_RDWR|O_CREAT, S_IRWXU);
	if (fd < 0)
		{
		ERR_PRINTF2(_L("Failed to create a large file and errno is %d"),errno);
		goto end;
		}
	fp = fdopen(fd,"w+");
	if (!fp)
		{
		ERR_PRINTF2(_L("Failed in fdopen and errno is %d"),errno);
		goto end;
		}
	INFO_PRINTF1(_L("Successfully able to create a large file"));

	strcpy(argv[0],"z:\\sys\\bin\\filetransfer.exe");
	sprintf(argv[1], "%d", fd);
	sprintf(argv[2], "%d", bytes);
	argv[3] = 0;
	position = lseek64(fd,length,SEEK_SET); 
	if (position == (off64_t)-1)
		{
		ERR_PRINTF2(_L("Failed to set the position in a large file and errno is %d"),errno);
		goto end;	
		}
	val = fgetpos64(fp,&pos);
	if (val != 0)
		{
		INFO_PRINTF1(_L("Failed to get the position in a large file\n"));
		goto end;	
		}
	size = write(fd, buf1, bytes);
	if (size != bytes)
		{
		INFO_PRINTF1(_L("Failed to write to a large file after getting the current position\n"));
		goto end;	
		}
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_CUR;
	fl.l_start = 0;
	fl.l_len = 10;
	val = fcntl(fd , F_SETLK64, &fl);
	if(val == -1)
		{
		ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
		goto end;
		}
	val = posix_spawn(&pid, "z:\\sys\\bin\\filetransfer.exe", NULL, NULL, argv, (char**)NULL);
	if(val != 0)
		{
		ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
		goto end;
		}
	pid1 = waitpid(pid, &status, WUNTRACED);
	if (pid1!=pid)
		{
		ERR_PRINTF2(_L("waitpid failed and errno is %d"),errno);
		goto end;
		}
	if (WEXITSTATUS(status) != EFailNone)
		{
		ERR_PRINTF2(_L("Status returned by the child process is %d"),WEXITSTATUS(status));	
		goto end;
		}	
	val = fsetpos64(fp,&pos);
	if (val != 0)
		{
		ERR_PRINTF2(_L("Failed to set the position in a large file and errno is %d"),errno);
		goto end;	
		}	
	size = read(fd,buf2,bytes);
	if (size == -1)	
		{
		ERR_PRINTF2(_L("Failed to read from the large file and errno is %d"),errno);
		goto end;	
		}
	val = strncmp(buf3,buf2,bytes);
	if (val != 0)
		{
		ERR_PRINTF1(_L("Both the data written and read are not same"));
		goto end;	
		}
	INFO_PRINTF1(_L("Successfully able to retreive the file status both in the child and parent process"));
	SetTestStepResult(EPass);
	
	end:
	fl.l_type = F_UNLCK;
	fl.l_whence = SEEK_CUR;
	fl.l_start = 0;
	fl.l_len = 10;
	val = fcntl(fd , F_SETLK64, &fl);
	if(val == -1)
		{
		ERR_PRINTF2(_L("Error in unlocking the file and errno is set to %d"),errno);
		SetTestStepResult(EFail);
		}
	free((void*)argv[0]);
	free((void*)argv[1]);
	free((void*)argv[2]);
	free((void*)argv);
	val = CleanUpSingleFile(fp, largefile);
	if (val != KErrNone)
		{
		ERR_PRINTF1(_L("Error in  CleanUpSingleFile()"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
    }

//  End of File
