/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Test code for directory and file handling
* 
*
*/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <dirent.h>
#include <errno.h>

#include "CTEST.H"	/* includes C interface to EPOC32 threads, and SpawnPosixServer */

test_Data;
char rootpath[MAXPATHLEN];

/* construct a directory tree in various ways */
#define WIDENAME {L'/', L'T', L'o', L'p', L'/', L'e', L'u', L'r', L'o', 0x20ac, 0}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1047
@SYMTestCaseDesc	    Tests for operations on directory 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by creating directory with invalid name,existing directory name,wide characters
                        Tests for the error code returned while creating directories.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void make_tree()
	{
	int err;
	char namebuf[MAXPATHLEN], namebuf2[MAXPATHLEN];
	char toobig[MAXPATHLEN+MAXPATHLEN+1];
	char *p;
	wchar_t *wp;

	
	wchar_t widenamebuf[MAXPATHLEN+1];
	wchar_t widename[] = WIDENAME;



	test_Next("Create Directory Tree - relative paths");

	err=mkdir("***", 0777);
	test_errno(err==-1,EINVAL);	/* bad directory name */


	err=mkdir("top", 0777);
	test(err==0);

	err=mkdir("top", 0777);
	test_errno(err==-1,EEXIST);	/* directory already exists */

	
	//make a dir with a wide character in the name
	err = wcstombs(namebuf, widename, MAXPATHLEN);
	test(err!=-1);
	
	err=mkdir(namebuf, 0777);
	test(err==0);
	

	err=mkdir("top/middle1/bottom1", 0777);
	test_errno(err==-1,ENOENT);	/* missing middle bit of path */

	err=mkdir("top/middle1", 0777);
	test(err==0);

	err=chdir("./top//\\/.\\.\\../top/.");
	test(err==0);

	p = getcwd(rootpath,sizeof(rootpath));	/* save name of toplevel directory */
	test(p==rootpath);

	err=chdir("middle1");
	test(err==0);

	err=chdir("bottom2");
	test_errno(err==-1,ENOENT);	/* directory doesn't exist yet */

	p = getcwd(namebuf,sizeof(namebuf));	/* prepare name for tests later */
	test(p==namebuf);

	err=mkdir("bottom1",0777);
	test(err==0);

	err=mkdir("read-only",0444);
	test(err==0);

	err=mkdir("read-only/sub-read-only",0444);
	/* test_errno(err==-1,EACCES); */
	test(err==0);	/* Omission - EPOC32 has Win32 semantics for read-only directories */

	err=chdir("../../top/middle1/bottom1");
	test(err==0);

	test_Next("Create Directory Tree - absolute paths");

	p = strcat(namebuf,"/bottom2");
	test(p==namebuf);	/* .../top/middle1/bottom2 */

	err=chdir(namebuf);
	test_errno(err==-1,ENOENT);	/* directory doesn't exist yet */
	
	err=mkdir(namebuf, 0777);
	test(err==0);

	err=chdir(namebuf);
	test(err==0);

	p = getcwd(namebuf,sizeof(namebuf));
	test(p==namebuf);

	err=mkdir("../../middle2", 0777);
	test(err==0);

	p = getcwd(namebuf2,sizeof(namebuf2));
	test(p==namebuf2);
	test(strcmp(namebuf,namebuf2)==0);	/* mkdir shouldn't change cwd */

	memset(toobig,'a', sizeof(toobig));
	toobig[sizeof(toobig)-1]='\0';

	err=mkdir(toobig,0777);
	test_errno(err<0,ENAMETOOLONG);


	test_Next("Test getcwd");
	
	//size too small
	p = getcwd(namebuf, 4);
	test_errno(0==p, ERANGE);

	//make it alloc a buffer
	p = getcwd(NULL, 300);
	test (NULL != p);
	free(p);

	//alloc a buffer then fail with a too small size
	p = getcwd(NULL, 10);
	test_errno(0==p, ERANGE);

	wp = wgetcwd(widenamebuf, MAXPATHLEN-1);
	test (NULL != wp);

	p = getcwd(namebuf2, MAXPATHLEN-1);
	test (NULL != p);

	wcstombs(namebuf, widenamebuf, MAXPATHLEN-1);
	test(strcmp(namebuf, namebuf2) == 0);
	
	
	//test realpath
	strcpy(namebuf,"bobby.dog");
	test( (0==strcmp("C:\\top\\", realpath("/top/../top/../top/./",namebuf))) || 
		  (0==strcmp("D:\\top\\", realpath("/top/../top/../top/./",namebuf))));

	

}

/**
   Directory tree is now
 
     top / middle2   
           middle1 / bottom1
                   / bottom2
 		   / read-only / sub-read-only
 

@SYMTestCaseID          SYSLIB-STDLIB-CT-1048
@SYMTestCaseDesc	    Tests for operations on creating files
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by opening files which does not exists,check for closing a file twice
                        Tests for the error code returned while creating files
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void create_files()
	{
	int err;
	int fd;
	char namebuf[MAXPATHLEN],*p;

	test_Next("Creating Files - relative paths");

	err=chdir(rootpath);
	test(err==0);

	fd = open("topfile",O_RDWR+O_APPEND,0777);
	test_errno(fd<0,ENOENT);	/* doesn't exist */

	fd = open("topfile",O_RDWR+O_CREAT,0777);
	test(fd>=0);

	err=close(fd);
	test(err==0);

	err=close(fd);
	test_errno(err<0,EBADF);	/* can't close it twice */

	fd = open("topfile",O_RDWR+O_APPEND,0777);
	test(fd>=0);

	err=close(fd);
	test(err==0);

	fd = open("topfile",O_RDWR+O_CREAT+O_EXCL,0777);
	test_errno(fd<0,EEXIST);	/* already exists */

	fd = open("middle1/bottom2/file",O_RDONLY+O_CREAT,0444);
	test(fd>=0);

	err=close(fd);
	test(err==0);

	fd = open("middle1/bottom2/file",O_RDWR+O_APPEND,0777);
	/* test_errno(fd<0,EACCES); */
	test(fd>=0);	/* Omission - the original O_CREAT ignores the 0444 permissions */
	if (fd>=0)
		{
		err=close(fd);
		test(err==0);
		}

	err=chmod("middle1/bottom2/file",0444);
	test(err==0);

	fd = open("middle1/bottom2/file",O_RDWR+O_APPEND,0777);
	test_errno(fd<0,EACCES);	/* not writeable */

	fd = open("middle2",O_RDWR+O_CREAT,0777);
	/* test_errno(fd<0,EISDIR); */
	test_errno(fd<0,EACCES);	/* Omission - we don't do EISDIR */

	test_Next("Creating Files - absolute paths");

	err=chdir("middle1/bottom1");
	test(err==0);

	p = getcwd(namebuf,sizeof(namebuf));	/* prepare name for tests later */
	test(p==namebuf);

	p = strcat(namebuf,"absfile");
	test(p==namebuf);

	fd = open(namebuf,O_RDWR+O_CREAT,0777);
	test(fd>=0);

	err=close(fd);
	test(err==0);

	fd = open("../read-only/file",O_RDWR+O_CREAT,0444);
	/* test_errno(fd<0,EACCES); */
	test(fd>=0);	/* Omission - EPOC32 has Win32 semantics for read-only directories */
	if (fd>=0)
		{
		err=close(fd);
		test(err==0);
		}
	
	}

/**
   Directory tree is now
 
    top / topfile
           middle2 /   
           middle1 / bottom1 / absfile
                   / bottom2 / file                -- read-only
 		   / read-only / sub-read-only /
                                 file
 

@SYMTestCaseID          SYSLIB-STDLIB-CT-1049
@SYMTestCaseDesc	    Tests for renaming operations 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by renaming files.Tests for the error code returned while renaming files
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void renaming()
	{
	int err;

	test_Next("Renaming");

	err=chdir(rootpath);
	test(err==0);

	err=rename("middle1","middle2");
	test_errno(err<0,EEXIST);

	err=rename("middle1/bottom1/absfile","middle2/absfile");
	test(err==0);

	err=rename("middle2/absfile","middle1/bottom1/absfile");
	test(err==0);

	err=rename("middle1/bottom1/absfile","middle2/nonsuch/newname");
	test_errno(err<0,ENOENT);

	err=rename("middle1","middle1/bottom1/subdirectory_of_self");
	test_errno(err<0,EACCES);

	err=rename("middle1","newname");
	test(err==0);

	err=rename("newname/bottom2/file","middle2/file");
	test(err==0);

	err=rename("newname","middle1");
	test(err==0);

	err=rename("middle2/file","middle1/bottom2/file");
	test(err==0);

	err=rename("no such file","valid new name");
	test_errno(err<0,ENOENT);

	err=rename("no such file","topfile");
	test_errno(err<0,ENOENT);

	err=rename(".","../different top");
	/* test_errno(err<0,EACCES);	-- can't change "." */
	test(err==0);	/* STDLIB resolves "." to full path, so this works */

	err=rename("../different top",rootpath);
	test(err==0);
	}

/**
   Directory tree is now
 
     top / topfile
           middle2 /   
           middle1 / bottom1 / absfile
                   / bottom2 / file                -- read-only
 		   / read-only / sub-read-only /
                                 file
 

@SYMTestCaseID          SYSLIB-STDLIB-CT-1050
@SYMTestCaseDesc	    Tests for enumeration on directories 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by opening directories
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void directory()
	{
	int err, count, i, j, fd;
	DIR *dp;
	struct dirent *ep;
	char name[MAXPATHLEN+1];
	off_t pos;

	test_Next("Enumerating Directories");

	err=chdir(rootpath);
	test(err==0);

	dp=opendir("topfile");
	/* test_errno(dp==0,ENOTDIR); -- not convinced about this anyway */
	test_errno(dp==0,ENOENT);

	dp=opendir("no such file");
	test_errno(dp==0,ENOENT);


	//test something sensible happens if someone uses a WDIR inplace of a DIR
	{
		WDIR *wp = wopendir((wchar_t*)L".");
		test(wp!=0);

		// Test wants a WDIR passed but won't compile under CW.
		// Force the compile by casting. The function will still get a WDIR.
		// DIR inherits from WDIR.
		ep=readdir((DIR*)wp);
		test_errno(ep==0,EINVAL);

		wclosedir(wp);
	}




	dp=opendir(".");
	test(dp!=0);

	count=0;
	do
		{
		ep=readdir(dp);
		if (ep && strcmp(ep->d_name,".")!=0 && strcmp(ep->d_name,"..")!=0)
			count++;
		}
	while (ep!=0);
	test(count==4);

	for (i=0; i<4; i++)
		{
		rewinddir(dp);
		for (j=0; j<=i; j++)
			{
			ep=readdir(dp);
			test(ep!=0);
			}
		strcpy(name,ep->d_name);
		rewinddir(dp);
		for (j=0; j<=i; j++)
			{
			ep=readdir(dp);
			test(ep!=0);
			}
		test(strcmp(name,ep->d_name)==0);
		}

	for (i=0; i<4; i++)
		{
		rewinddir(dp);
		pos=telldir(dp);
		for (j=0; j<=i; j++)
			{
			pos=telldir(dp);
			ep=readdir(dp);
			test(ep!=0);
			}
		strcpy(name,ep->d_name);
		rewinddir(dp);
		seekdir(dp, pos);
		ep=readdir(dp);
		test(ep!=0);
		test(strcmp(name,ep->d_name)==0);
		}

	err=closedir(dp);
	test(err==0);

	dp=opendir("middle2");
	test(dp!=0);

	count=0;
	do
		{
		ep=readdir(dp);
		if (ep && strcmp(ep->d_name,".")!=0 && strcmp(ep->d_name,"..")!=0)
			count++;
		}
	while (ep!=0);
	test(count==0);	/* empty directory */

	rewinddir(dp);

	fd = open("middle2/extrafile",O_RDWR+O_CREAT,0777);
	test(fd>=0);

	err=close(fd);
	test(err==0);

	count=0;
	do
		{
		ep=readdir(dp);
		if (ep && strcmp(ep->d_name,".")!=0 && strcmp(ep->d_name,"..")!=0)
			count++;
		}
	while (ep!=0);
	test(count==0);	/* shouldn't have noticed the change */

	rewinddir(dp);	/* and spot the new file */
	count=0;
	do
		{
		ep=readdir(dp);
		if (ep && strcmp(ep->d_name,".")!=0 && strcmp(ep->d_name,"..")!=0)
			count++;
		}
	while (ep!=0);
	test(count==1);

	closedir(dp);

	dp=opendir("/");
	test(dp!=0);

	count=0;
	do
		{
		ep=readdir(dp);
		if (ep && strcmp(ep->d_name,".")!=0 && strcmp(ep->d_name,"..")!=0)
			count++;
		}
	while (ep!=0);
	test(count>0);

	closedir(dp);
	}

/**
   Directory tree is now
 
     top / topfile
            middle2 / extrafile  
           middle1 / bottom1 / absfile
                   / bottom2 / file                -- read-only
 		   / read-only / sub-read-only /
                                 file
 
@SYMTestCaseID          SYSLIB-STDLIB-CT-1051
@SYMTestCaseDesc	    Tests for file attributes
@SYMTestPriority 	    High
@SYMTestActions  	    Tests the attributes on files and directories
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void attributes()
	{
	int err;
	struct stat s1,s2;
	int fd;
	double diff;

	test_Next("File Attributes");

	err=chdir(rootpath);
	test(err==0);

	err=stat("middle earth/bag end/hobbit",&s1);
	test_errno(err<0,ENOENT);

	err=stat("middle1/bottom2/file",&s1);
	test(err==0);
	test(S_ISREG(s1.st_mode)!=0);
	test(S_ISDIR(s1.st_mode)==0);
	test((s1.st_mode&S_IWUSR)==0);
	test(s1.st_size==0);

	err=stat("topfile",&s1);
	test(err==0);
	test(S_ISREG(s1.st_mode)!=0);
	test(S_ISDIR(s1.st_mode)==0);
	test((s1.st_mode&S_IWUSR)!=0);
	test(s1.st_size==0);

	err=stat("topfile",&s2);
	test(err==0);
	test(s1.st_mode==s2.st_mode);
	test(s1.st_size==s2.st_size);
	diff=difftime(s1.st_mtime,s2.st_mtime);
	test(diff==(double)0.0);

	fd=open("topfile", O_RDONLY, 0);
	test(fd>=0);

	err=fstat(fd,&s2);
	test(err==0);
	test(s1.st_mode==s2.st_mode);
	test(s1.st_size==s2.st_size);
	diff=difftime(s1.st_mtime,s2.st_mtime);
	test(diff==(double)0.0);

	err=stat("topfile",&s2);
	test(err==0);
	test(s1.st_mode==s2.st_mode);
	test(s1.st_size==s2.st_size);
	diff=difftime(s1.st_mtime,s2.st_mtime);
	test(diff==(double)0.0);

	err=close(fd);
	test(err==0);

	sleep(1);	/* to ensure that the modify time changes */

	fd=open("topfile", O_RDWR+O_APPEND, 0);
	test(fd>=0);

	err=stat("topfile",&s2);
	test(err==0);
	test(s1.st_mode==s2.st_mode);
	test(s1.st_size==s2.st_size);
	/* probably not guaranteeed to have changed the modtime at this point */

	err=write(fd,rootpath,3);
	test(err==3);

	err=fsync(fd);
	test(err==0);

	err=close(fd);
	test(err==0);

	err=stat("topfile",&s2);
	test(err==0);
	test(s1.st_mode==s2.st_mode);
	test(s2.st_size==3);
	diff=difftime(s2.st_mtime,s1.st_mtime);
	test(diff>(double)0.0);

	test_Next("Directory Attributes");

	err=stat("middle1",&s1);
	test(err==0);
	test(S_ISREG(s1.st_mode)==0);
	test(S_ISDIR(s1.st_mode)==1);
	test((s1.st_mode&S_IWUSR)!=0);

	err=stat("middle1/read-only",&s1);
	test(err==0);
	test(S_ISREG(s1.st_mode)==0);
	test(S_ISDIR(s1.st_mode)==1);
	test((s1.st_mode&S_IWUSR)==0);

	err=stat("/",&s1);
	test(err==0);
	test(S_ISREG(s1.st_mode)==0);
	test(S_ISDIR(s1.st_mode)==1);

	err=access("middle1/bottom1/absfile",W_OK);
	test(err==0);

	err=access("middle1/bottom1/absfile",R_OK);
	test(err==0);

	err=access("middle1/bottom2/file",W_OK);
	test(err!=0);

	err=access("middle1/bottom2/file",R_OK);
	test(err==0);

	err=access("middle1/read-only",W_OK);
	test(err==0);

	err=access("middle1/read-only",R_OK);
	test(err==0);

	err=access("middle1/no such directory",R_OK);
	test(err!=0);
}

/**
   Directory tree is now
 
     top / topfile
           middle2 / extrafile  
           middle1 / bottom1 / absfile
                   / bottom2 / file                -- read-only
 		   / read-only / sub-read-only /
                                 file
 

@SYMTestCaseID          SYSLIB-STDLIB-CT-1052
@SYMTestCaseDesc	    Tests for searching on different drives
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by searching on z drive,test for the error codes 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
 void searching()
	{
	int err,fd;
	char name[MAXPATHLEN+1];

	test_Next("Searching across drives");

	sprintf(name,"%s/middle2/extrafile",rootpath);

	err=chdir("z:/");
	test(err==0);

	fd=open(name+2, O_RDONLY, 0);
	test_errno(fd<0,ENOENT);	// doesn't exist on z:

	name[0]='?';
	fd=open(name, O_RDWR, 0);
	test(fd>=0);			// found it on the original drive

	err=close(fd);
	test(err==0);
	}

/**
   Directory tree is now
 
     top / topfile
           middle2 / extrafile  
           middle1 / bottom1 / absfile
                   / bottom2 / file                -- read-only
 		   / read-only / sub-read-only /
                                 file
 

@SYMTestCaseID          SYSLIB-STDLIB-CT-1053
@SYMTestCaseDesc	    Tests for deleting files
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by deleting files and directories.Test for error codes
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void deletion()
	{
	int err;
	char namebuf[MAXPATHLEN];
	wchar_t widename[] = WIDENAME;


	test_Next("Deleting - files");

	err=chdir(rootpath);
	test(err==0);



	err=unlink("middle1/bottom2/file");
	test_errno(err<0,EACCES);	/* file is read-only */

	err=chmod("middle1/bottom2/file",0777);
	test(err==0);

	err=unlink("middle1/bottom2/file");
	test(err==0);

	err=unlink("middle2/extrafile");
	test(err==0);

	err=unlink("middle1/read-only/file");
	/* test_errno(err<0,EPERM);	parent directory is read-only */
	test(err==0);	/* Omission - EPOC32 uses Win32 semantics for read-only directories */

	test_Next("Deleting - directories");

	err=chdir(rootpath);
	test(err==0);

	//delete the dir with a wide character in the name
	err = wcstombs(namebuf, widename, MAXPATHLEN);
	test(err!=-1);
	
	err=rmdir(namebuf);
	test(err==0);

	err=rmdir("middle1");
	test_errno(err<0,EEXIST);	/* not empty */

	err=rmdir("middle1/bottom1");
	test_errno(err<0,EEXIST);	/* not empty */

	err=unlink("middle1/bottom1/absfile");
	test(err==0);

	err=rmdir("middle1/bottom1");
	test(err==0);

	err=rmdir("middle1/bottom1");
	test_errno(err<0,ENOENT);	/* already deleted */

	err=rmdir("middle1");
	test_errno(err<0,EEXIST);

	err=rmdir("middle1/bottom2");
	test(err==0);

	test_Next("Deleting - read-only directories");

	err=rmdir("middle1/read-only/sub-read-only");
	/* test_errno(err!=0,EACCES);	-- permission denied - read-only parent */
	test_errno(err<0,EACCES);	/* Omission - EPOC32 uses Win32 semantics */

	err=chmod("middle1/read-only",0777);
	test(err==0);

	err=rmdir("middle1/read-only/sub-read-only");
	/* test(err==0); */
	/* EPOC32 doesn't use the writeability of the parent directory, but instead looks 
	 * at the attributes of the directory itself.
	 */
	test_errno(err!=0,EACCES);

	err=chmod("middle1/read-only/sub-read-only",0777);
	test(err==0);

	err=rmdir("middle1/read-only/sub-read-only");
	test(err==0);

	err=rmdir("middle1/read-only");
	test(err==0);

	err=rmdir("middle?");
	test_errno(err<0,EINVAL);	/* no wild cards please */

	err=rmdir("middle1");
	test(err==0);

	err=rmdir("../top/middle2");
	test(err==0);

	err=rmdir(".");
	test_errno(err<0,EEXIST);	/* not empty */

	err=unlink("topfile");
	test(err==0);

	err=rmdir(".");
	test(err==0);
	}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1054
@SYMTestCaseDesc	    Tests for creation of temporary directory and files in it.
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by creating a temporary directory,files and writing to the files.
                        Check for error codes.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void temporary_files()
	{
#define tmpdir        "c:/system/temp"

	int err, count1, count2;
	DIR *dp;
	struct dirent *ep;
	FILE *fp;
	char name[L_tmpnam];
	char name2[L_tmpnam];
	char *p;

	test_Next("Temporary files");

	dp=opendir(tmpdir);
	if (dp==0)
		{
		printf("  Creating the directory %s ...\n", tmpdir);
		err=mkdir("c:/system", 0777);
		test(err==0);
		err=mkdir(tmpdir, 0777);
		test(err==0);
		dp=opendir(tmpdir);
		}
	test(dp!=0);

	count1=0;
	do
		{
		ep=readdir(dp);
		if (ep && strcmp(ep->d_name,".")!=0 && strcmp(ep->d_name,"..")!=0)
			count1++;
		}
	while (ep!=0);

	fp=tmpfile();
	test(fp!=0);

	err=fprintf(fp,"hello");
	test(err==5);

	rewinddir(dp);
	count2=0;
	do
		{
		ep=readdir(dp);
		if (ep && strcmp(ep->d_name,".")!=0 && strcmp(ep->d_name,"..")!=0)
			count2++;
		}
	while (ep!=0);
	test(count2==count1+1);	/* EPOC32 temporary files are visible in file system */
	err=fclose(fp);
	test(err==0);

	rewinddir(dp);
	count2=0;
	do
		{
		ep=readdir(dp);
		if (ep && strcmp(ep->d_name,".")!=0 && strcmp(ep->d_name,"..")!=0)
			count2++;
		}
	while (ep!=0);
	test(count2==count1);		/* should be automatically deleted */

	closedir(dp);

	p=tmpnam(NULL);
	test(p!=0);

	count1=strlen(p);
	test(count1<L_tmpnam);

	p=tmpnam(name);
	test(p==name);

	fp=fopen(name,"wb+");
	test(fp!=0);

	p=tmpnam(name2);
	test(p==name2);

	err=strcmp(name,name2);
	test(err!=0);

	err=fclose(fp);
	test(err==0);

	err=unlink(name);
	test(err==0);

	printf("  Tmpnam suggested %s and %s\n", name, name2);
	}

int close_console=0;
void allTests()
	{
	int err=chdir("C:\\");
	test(err==0);

	make_tree();
	create_files();
	renaming();
	directory();
	attributes();
	searching();
	deletion();
	temporary_files();

	if (close_console)
		{
		test_Close();
		close(0);
		close(1);
		close(2);

		CloseSTDLIB();
		}
	}

int main()
	{
	void* client;
	int err;

	test_Title("Directory Handling");

	allTests();

	test_Next("Do it again using the CPosixServer (for them, not me)");
	close_console=1;

	start_posix_server();	/* calls SpawnPosixServer from C++ code */

	client=create_thread(allTests, "TDIRS tests");
	test(client!=0);
	start_thread(client);
	err=wait_for_thread(client);
	test(err==0);

	CloseSTDLIB();
	test_Close();
	return 0;
	}
