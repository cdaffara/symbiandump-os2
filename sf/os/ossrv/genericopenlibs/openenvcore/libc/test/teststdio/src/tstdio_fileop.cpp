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
// Name        : tchildpopendisp.cpp
// 
//

#include "tstdio.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> //MB_LEN_MAX
#include <sys/types.h>   // for mkdir
#include <sys/stat.h>


#define MAX_SIZE 50
#include <test/testexecutestepbase.h>

// -----------------------------------------------------------------------------
//Function Name :TestRemove3
//API Tested :Remove
//TestCase Description:Tests remove() function.
// -----------------------------------------------------------------------------
TInt CTestStdio::TestRemove3()
	{
	//Try removing a nonexistent file
	char *fileptr = "c:\\abcd.txt";
	//INFO_PRINTF1
	TInt ret = remove(fileptr);
	
	if (ret== -1)
		{
		ERR_PRINTF1(_L("Error Removing non-existent file"));
		return EPass;
		}
	else
		{
		INFO_PRINTF1(_L("File Deleted"));
		return EFail;
		} 
	}

// -----------------------------------------------------------------------------
//Function Name :TestRemove4
//API Tested :Remove
//TestCase Description:malloc returns -> pointer to unitialized mem locn
// -----------------------------------------------------------------------------	
TInt CTestStdio::TestRemove4()
	{
	char *fileptr = "";
	TInt ret = remove(fileptr);
	
	if (ret== -1)
		{
		ERR_PRINTF1(_L("Error Removing empty file name "));
		return EPass;
		}
	else
		{
		INFO_PRINTF1(_L("File Deleted"));
		return EFail;
		} 
	}

// -----------------------------------------------------------------------------
//Function Name :TestRemove5
//API Tested :Remove
//TestCase Description:malloc returns -> pointer to unitialized mem locn
// -----------------------------------------------------------------------------
TInt CTestStdio::TestRemove5()	
	{
	char *fileptr="c:\\Test1";
	TInt ret = mkdir(fileptr,S_IRWXU );
	if (ret)
		{
		ERR_PRINTF1(_L("Error creating directory"));
		return ret;
		}
	//Try removing directory
	/*TBuf<MAX_SIZE> buf;
	TInt len = strlen(fileptr);
	TInt i;
	for(i=0; i< len ; i++)
		{
		buf[i]= *(fileptr+i);
		}
	buf[i]='\0';
	INFO_PRINTF2(_L("Removing Directory %S"),&buf );*/
	ret = remove(fileptr);
	if (ret==KErrNone)
		{
		INFO_PRINTF1(_L("Directory removed!"));
		}
	return ret;
	}

//-----------------------------------------------------------------------------
//Function Name :TestRemove6
//API Tested :Remove
//TestCase Description:malloc returns -> pointer to unitialized mem locn
//-----------------------------------------------------------------------------	
TInt CTestStdio::TestRemove6()	
	{
	INFO_PRINTF1(_L("Remove a non-existent directory!"));
	char *fileptr="c:\\xyz";
	//Try removing directory
/*	TBuf<MAX_SIZE> buf;
	TInt len = strlen(fileptr);
	TInt i;
	for(i=0; i< len ; i++)
		{
		buf[i]= *(fileptr+i);
		}
	buf[i]='\0';
	INFO_PRINTF2(_L("Removing Directory %S"),&buf );*/
	TInt ret = remove(fileptr);
	if (ret==-1)
		{
		ERR_PRINTF1(_L("Remove Error"));
		}
	return EPass;
	}
	
//-----------------------------------------------------------------------------
//Function Name :TestRename2
//API Tested :Remove
//TestCase Description:malloc returns -> pointer to unitialized mem locn
//-----------------------------------------------------------------------------		
TInt CTestStdio::TestRename2()	
	{
	INFO_PRINTF1(_L("Test rename()"));
	TPtrC ptrString;
   	TBool res= GetStringFromConfig(ConfigSection(),_L("parameter1"), ptrString);
//   	INFO_PRINTF2(_L("%S"),ptrBuf);	
   
	//Get old path
	res= GetStringFromConfig(ConfigSection(),_L("parameter2"), ptrString);
	TBuf8<100> buf1;
	buf1.Copy(ptrString);
	char *path1 = (char*) buf1.Ptr();
	TInt len=buf1.Length();
	path1[len]='\0';
	
	//Get New path
	res= GetStringFromConfig(ConfigSection(),_L("parameter3"), ptrString);
	TBuf8<100> buf2;
	buf2.Copy(ptrString);
	char *path2 = (char*) buf2.Ptr();
	len=buf2.Length();
	path2[len]='\0';
		
	TInt ret= rename(path1,path2); 
	if (ret==-1)
		{
		ERR_PRINTF2(_L("rename() returned %d"),ret);
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	}

	
//-----------------------------------------------------------------------------
//Function Name :TestRename4
//API Tested :Remove
//TestCase Description:malloc returns -> pointer to unitialized mem locn
//-----------------------------------------------------------------------------		
TInt CTestStdio::TestRename4()	
	{
	char *fileptr="c:\\test.dat";
	
	//Rename file
	TInt ret= rename(fileptr," "); 
	if (ret==-1)
		{
		ERR_PRINTF2(_L("rename() returned %d"),ret);
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	}

/*
*Testcase Name		:filenoTest
*Function Name 		:fileno()
*TestCase Description:fileno return the integer file descriptor associated with the stream pointed to by stream.
*/


TInt CTestStdio::filenoTest()
	{
	FILE* pFile;
	TInt err;
	char name[15]= "c:\\test.txt";
	pFile = fopen(name,"w+");	
	if (pFile==NULL) 
		{
		perror ("Error opening file");	
		}
	err = fileno (pFile);
	    if (err<0)
		    {
		    return KErrGeneral;
		   	}
	fclose(pFile);
	unlink(name);
	return KErrNone ;
	}


/*
 *Testcase Name			:feofTest
 *Function Name 		:feof()
 *TestCase Description	:feof test the end-of-file indicator for the stream pointed to by stream
 *and return non-zero if and only if the end-of-file indicator is set for stream.
 */ 
 
 
TInt CTestStdio::feofTest()
	{
	FILE* pFile;
	char name[15]= "c:\\test.txt";
	pFile = fopen(name,"w+");
	if (pFile==NULL) 
	perror ("Error opening file");
	else
		{
  		while(!feof (pFile) )
  			{
  			 fgetc (pFile);
   			}
    	}
    fclose (pFile);
    unlink(name);
    return KErrNone;
	}
	
/*
 *Test case name  		:clrerrTest	
 *Function Name 		:clearerr
 *TestCase Description	:clearerr clear the end-of-file and error indicators for the stream to which stream points and returns void.
 */

	TInt CTestStdio::clrerrTest()
    {			 
	FILE* pFile;
	char name[15]= "c:\\test.txt";
	pFile = fopen(name,"w+");
  	if (pFile==NULL) 
	  	{
	  	perror ("Error opening file");	
	  	}
	fputc ('X',pFile);
	fclose(pFile);
  	pFile = fopen(name,"r");
  	if (pFile==NULL) 
	  	{
	  	perror ("Error opening file");	
	  	}
	else
		{
		fputc ('X',pFile);
		if (ferror (pFile))
		    {
		    printf ("Error Writing to test.txt\n");
		    }	
		}
	  	
	clearerr (pFile);
    fclose (pFile);
    unlink(name);
    return KErrNone;
	}


/*	
 *Function Name		: getcTest
 *API Tested		: getc
 *Description		: get a byte from a stream and Upon successful completion,returns a non-negative value.
 */
	
   	
  TInt CTestStdio::getcTest()
	{
	FILE* pFile;
	TInt retval;
	char name[15]= "c:\\test.txt";
	pFile = fopen(name,"w+");
	if (pFile==NULL) 
		{
		perror ("Error opening file");	
		}
	fprintf(pFile, "%s", "abcdefghijklmn\n");
    fclose(pFile);
	pFile = fopen(name,"r");
    if(pFile == NULL)
    {
    printf("file reading error\n");
    }
	while((int)(retval = getc(pFile) )!= EOF)
      {
      if(retval<0)
      	{
      	return KErrGeneral;	
      	}
      }
	fclose(pFile);
	unlink(name);
    return KErrNone;
	} 
	
	
/*	
 *Function Name	: putcTest
 *API Tested	: putc 
 *Description	: put byte on a stream 
 */

TInt CTestStdio::putcTest()
    {
    FILE * pFile;
 	char c;
 	TInt err;
 	char name[15]= "c:\\test.txt";
 	pFile=fopen(name,"w+");
 	if (pFile==NULL) 
		{
		perror ("Error opening file");	
		}
  	for (c = 'A' ; c <= 'E' ; c++) 
  		{
    	err = putc (c , pFile);
    	if(err<0)
    		{
    		return KErrGeneral;
    		}
   		 }
  	fclose (pFile);
  	unlink(name);
  	return KErrNone;	
 	}	
 
 /*	
 *Function Name	: wfreopenTest
 *API Tested	: wfreopen 
 *Description	: Upon successful completion wfreopen,
 * returns a FILE pointer. 
 *Otherwise, NULL is returned and the global variable errno is set to indicate the error. 
 */
	
	
TInt CTestStdio::wfreopenTest()
	{
	wchar_t name[15]=L"c:\\test.txt";
	FILE* fp;
	fp=wfreopen (name,L"w",stdout);
	printf ("This sentence is redirected to a file.");
	if(fp==NULL)
		{
		return KErrGeneral;
		}
	fclose (stdout);
	return KErrNone;		
 	}
	
