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
// Name        : tftwblocks.cpp
// 
//

#include "tftw.h"
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/syslimits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int CTestftw::CreateDirs()
	{
	int ret=0;
	int fd;
	
	if( ((ret = mkdir("c:\\ftwtest", 666))) != -1 && (errno != 17))
		{
		mkdir("c:\\ftwtest\\test", 666);
		fd=open("C:\\ftwtest\\test1.txt", O_WRONLY|O_CREAT);
		close(fd);
		return 0;
		}
	INFO_PRINTF2(_L("The errmo value is %d\n"),errno);
	return ret;	
	}

int action(const char* entry, const struct stat* /*statbuf*/, int entrytype)
	{
	
	int err=0;
	
	switch(entrytype)
		{
		case FTW_F:
			err=unlink(entry);
			break;
		
		case FTW_D:
		case FTW_DP:
			break;
		
		case FTW_DNR:
			break;
			
		case FTW_NS:
			break;
		
		default:
			err=0;
			break;
		}
	return err;	
	}

// Util function
void CTestftw::ReadStringParam(char* aString)
	{
	_LIT( KSl, "Param%d" );
   	TBuf<8> pNameBuf;
    TPtrC string;
    pNameBuf.Format(KSl,++iParamCnt);
	TBool res = GetStringFromConfig(ConfigSection(), pNameBuf, string);
	if(!res)
	 	{
	 	_LIT(Kerr , "Unable to retrieve string parameter") ;
	 	INFO_PRINTF1(Kerr);
	  	}
	TBuf8<512> bufstring;
	bufstring.Copy(string);
	TInt paramLength=string.Length();
	char* text=(char *)(bufstring.Ptr());
 	*(text+paramLength)='\0';
	strcpy(aString,text);
	return;
	}

/**
* Function Name		: TestFtw
* Description		: 	1. ftw for a nonexistent directory
*						2. ftw for long dir name > PATH_MAX
*						3. ftw for NULL
* Return Value		: 	1. ftw returns ENOENT
*						2. ftw returns ENAMETOOLONG
*						3. ftw retunrs ENOENT
*/
TInt CTestftw::TestFtw()
    {
    char parm[512];
	int result;
	int ret;
	
	memset(&parm[0], 0, 512);    
   	ReadStringParam(parm);
	ret=ftw(parm, &action, 10);
	GetIntFromConfig(ConfigSection(), _L("Expected"), result);
	INFO_PRINTF2(_L("ftw fails with error No: %d"), errno);
   	
   	if(ret==-1 && errno==result)
   		{
   		return KErrNone;
   		}
   		
   	return KErrGeneral;	
    }

/**
* Function Name		: TestFtw2
* Description		: ftw with negative 'ndirs'
* Return Value		: ftw returns EINVAL
*/
TInt CTestftw::TestFtw2()
    {
    int ndirs, result;
	int ret;
	
	GetIntFromConfig(ConfigSection(), _L("Param1"), ndirs);
	
   	ret=ftw(".", &action, ndirs);
   	
   	GetIntFromConfig(ConfigSection(), _L("Expected"), result);
   	
   	if(ret==-1 && errno==result)
   		{
   		return KErrNone;
   		}
   	INFO_PRINTF2(_L("ftw fails with error No: %d"), errno);
   	return KErrGeneral;	
    }

/**
* Function Name		: TestFtw3
* Description		: Tests the behaviour of ftw 
*/
TInt CTestftw::TestFtw3()
	{
	int ret=KErrNone;
	
	if(CreateDirs() == -1)
		{	
		INFO_PRINTF1(_L("Failed to create directory tree"));
		return KErrGeneral;
		}
	
	if(ftw("c:\\ftwtest", &action, 3) != 0)
		{
		INFO_PRINTF2(_L("FTW failed with errno %d"), errno);
		ret = KErrGeneral;	
		}
		
	rmdir("c:\\ftwtest\\test");
	rmdir("c:\\ftwtest");
	return ret;
			
	}
/**
* Function Name		:	TestFtw4
* Description		: 	ftw for invalid path
* Return Value		: 	ftw returns ENOTDIR
*/
TInt CTestftw::TestFtw4()
    {
    char parm[PATH_MAX];
	int result; 
	int ret;
	
	int fd=open("c:\\validfile.txt", O_WRONLY|O_CREAT);
	if(fd==-1)
		{
		INFO_PRINTF2(_L("file open fails with error No: %d"), errno);
		return KErrGeneral;
		}
	
	memset(&parm[0], 0, PATH_MAX);    
   	ReadStringParam(parm);
	ret=ftw(parm, &action, 10);
   	GetIntFromConfig(ConfigSection(), _L("Expected"), result);
   	if(ret==-1 && errno==result)
   		{
   		close(fd);
   		INFO_PRINTF2(_L("ftw returns error no: %d"), errno);
		unlink("c:\\validfile.txt");
   		return KErrNone;
   		}
   	INFO_PRINTF2(_L("ftw fails with error No: %d"), errno);
   	close(fd);
   	unlink("c:\\validfile.txt");
   	return KErrGeneral;	
    }
 
/**
* Function Name		: 	TestFtw5
* Description		: 	ftw with 'ndirs' > OPEN_MAX
* Return Value		: 	ftw returns EINVAL
*/
TInt CTestftw::TestFtw5()
    {
    int ret=KErrNone;
	
	ret=ftw(".", &action, OPEN_MAX+1);
   	
   	if(ret==-1 && errno == EINVAL)
   		{
   		INFO_PRINTF2(_L("ftw returns the expected error number: %d"), errno);
   		return KErrNone;
   		}
   	INFO_PRINTF2(_L("ftw fails with error No: %d"), errno);
   	return KErrGeneral;	
    } 
    
 
//End of file

